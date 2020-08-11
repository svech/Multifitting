#include "fitting_gsl.h"

jmp_buf buffer_GSL;

Fitting_GSL::Fitting_GSL(Fitting* fitting):
	main_Calculation_Module(fitting->main_Calculation_Module),
	calculation_Trees(fitting->main_Calculation_Module->calculation_Trees),
	fitables(fitting->main_Calculation_Module->fitables),
	fitting(fitting),
	params(&fitting->params)
{

}

void Fitting_GSL::callback(const size_t iter, void* bare_Params, const gsl_multifit_nlinear_workspace* w)
{
	Q_UNUSED(w);
	Fitting_Params* params = ((struct Fitting_Params*)bare_Params);

	double residual = -2020;
	gsl_blas_ddot(w->f, w->f, &residual);
	if(params->maximize) residual = params->max_Integral-residual;

	// print out current location
	printf("iter %zu : ", iter);
	for(size_t i=0; i<params->fitables.param_Pointers.size(); ++i)
	{
		printf("%f\t|f|=%g", params->fitables.param_Pointers[i]->value, residual);
	}
	printf("\n\n");
}

bool Fitting_GSL::fit()
{
	gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();

	/// reading parameters

	// read main parameters
	const size_t max_iter = global_Multilayer_Approach->fitting_Settings->max_Iter;
	const double xtol = global_Multilayer_Approach->fitting_Settings->x_Tolerance;
	const double gtol = global_Multilayer_Approach->fitting_Settings->g_Tolerance;
	const double ftol = global_Multilayer_Approach->fitting_Settings->f_Tolerance;
	{
		// read method
		if(global_Multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Two_Dimensional_Subspace])						{
		if(params->p<2)
		{
			QMessageBox::information(nullptr,"Insufficient number of parameters", "Method\n\"Two Dimensional Subspace\"\nrequires at least 2 fitables");
			return false;
		}																																	fdf_params.trs = gsl_multifit_nlinear_trs_subspace2D;	}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Levenberg_Marquardt])							{	fdf_params.trs = gsl_multifit_nlinear_trs_lm;			}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Levenberg_Marquardt_with_Geodesic_Acceleration])	{	fdf_params.trs = gsl_multifit_nlinear_trs_lmaccel;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Dogleg])											{	fdf_params.trs = gsl_multifit_nlinear_trs_dogleg;		}
		if(global_Multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Double_Dogleg])									{	fdf_params.trs = gsl_multifit_nlinear_trs_ddogleg;		}

		/// read additional parameters
		// read scaling strategy
		if(global_Multilayer_Approach->fitting_Settings->current_Scale == GSL_Scales[More])				{	fdf_params.scale = gsl_multifit_nlinear_scale_more;			};
		if(global_Multilayer_Approach->fitting_Settings->current_Scale == GSL_Scales[Levenberg])		{	fdf_params.scale = gsl_multifit_nlinear_scale_levenberg;	};
		if(global_Multilayer_Approach->fitting_Settings->current_Scale == GSL_Scales[Marquardt])		{	fdf_params.scale = gsl_multifit_nlinear_scale_marquardt;	};

		// read linear solver
		if(global_Multilayer_Approach->fitting_Settings->current_Solver == GSL_Solvers[QR_decomposition])				{	fdf_params.solver = gsl_multifit_nlinear_solver_qr;			};
		if(global_Multilayer_Approach->fitting_Settings->current_Solver == GSL_Solvers[Cholesky_decomposition])			{	fdf_params.solver = gsl_multifit_nlinear_solver_cholesky;	};
		if(global_Multilayer_Approach->fitting_Settings->current_Solver == GSL_Solvers[Singular_value_decomposition])	{	fdf_params.solver = gsl_multifit_nlinear_solver_svd;		};

		// read fdtype
		if(global_Multilayer_Approach->fitting_Settings->current_Fdtype == GSL_Fdtype[Forward])		{	fdf_params.fdtype = GSL_MULTIFIT_NLINEAR_FWDIFF;			};
		if(global_Multilayer_Approach->fitting_Settings->current_Fdtype == GSL_Fdtype[Central])		{	fdf_params.fdtype = GSL_MULTIFIT_NLINEAR_CTRDIFF;			};

		// read numerical params
		fdf_params.factor_up = global_Multilayer_Approach->fitting_Settings->factor_Up;
		fdf_params.factor_down = global_Multilayer_Approach->fitting_Settings->factor_Down;
		fdf_params.avmax = global_Multilayer_Approach->fitting_Settings->avmax;
		fdf_params.h_df = global_Multilayer_Approach->fitting_Settings->h_df;
		fdf_params.h_fvv = global_Multilayer_Approach->fitting_Settings->h_fvv;
	}

	const gsl_multifit_nlinear_type* T = gsl_multifit_nlinear_trust;
	gsl_multifit_nlinear_workspace* work = gsl_multifit_nlinear_alloc(T, &fdf_params, params->n, params->p);

	gsl_vector* f = gsl_multifit_nlinear_residual(work);
	gsl_vector* x = gsl_multifit_nlinear_position(work);
	init_Position(x);

	gsl_multifit_nlinear_fdf fdf;
		fdf.f = &calc_Residual;
		fdf.df = nullptr;
		fdf.fvv = nullptr;
		fdf.n = params->n;
		fdf.p = params->p;
		fdf.params = params;

	// storage for the output data
	int info;

	// initialize solver
	gsl_multifit_nlinear_init(x, &fdf, work);

	// calc initial cost
	gsl_blas_ddot(f, f, &params->init_Residual);

	auto start = std::chrono::system_clock::now();

	int repeat = setjmp(buffer_GSL);
	if(repeat == 0)
	{
		// iterate until convergence
		gsl_multifit_nlinear_driver(max_iter, xtol, gtol, ftol,	callback, params, &info, work);
	}

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	qInfo() << "Fit  : "<< elapsed.count()/1000. << " seconds\n";
	qInfo() << "previous_ID =" << previous_ID << endl;

	// calc final cost
	gsl_blas_ddot(f, f, &params->final_Residual);

	// print summary
	printf("method              : %s\n",  gsl_multifit_nlinear_trs_name(work));
	printf("NITER               = %zu\n", gsl_multifit_nlinear_niter(work));
	printf("NFEV                = %zu\n", fdf.nevalf);

	if(params->maximize)
	{
		printf("initial cost        = %g\n",  params->max_Integral-params->init_Residual);
		printf("final cost          = %g\n",  params->max_Integral-params->final_Residual);
	} else
	{
		printf("initial cost        = %g\n",  params->init_Residual);
		printf("final cost          = %g\n",  params->final_Residual);
	}

	printf("reason for stopping : %s\n",  (info == 1) ? "small step size" : "small gradient");
	printf("\n");

	// actualize params->x
	gsl_vector_memcpy(params->x, x);

	gsl_multifit_nlinear_free(work);

	return true;
}

int Fitting_GSL::calc_Residual(const gsl_vector* x, void* bare_Params, gsl_vector* f)
{
	if(global_Multilayer_Approach->fitting_Settings->abort)
	{
		longjmp(buffer_GSL, 2018); // not zero! zero means repeating in infinite loop!
	}

	Fitting_Params* params = ((struct Fitting_Params*)bare_Params);

	// calc f
	Fitting::calc_Residual(x, params, f);

	return GSL_SUCCESS;
}

void Fitting_GSL::init_Position(gsl_vector* x)
{
	for(size_t i=0; i<fitables.param_Pointers.size(); ++i)
	{
		gsl_vector_set(x, i, fitables.values_Parametrized[i]);
	}
}
