#include "fitting_gsl.h"

Fitting_GSL::Fitting_GSL(Fitting* fitting):
	multilayer_Approach(fitting->multilayer_Approach),
	main_Calculation_Module(fitting->main_Calculation_Module),
	calculation_Trees(fitting->main_Calculation_Module->calculation_Trees),
	fitables(fitting->main_Calculation_Module->fitables),
	fitting(fitting),
	params(&fitting->params)
{

}

void Fitting_GSL::callback(const size_t iter, void* bare_Params, const gsl_multifit_nlinear_workspace* w)
{
	w=w;
	Fitting_Params* params = ((struct Fitting_Params*)bare_Params);

	// print out current location
	printf("iter %zu :", iter);
	for(size_t i=0; i<params->fitables.fit_Parameters.size(); ++i)
	{
		printf("%f ", params->fitables.fit_Parameters[i]->value);
	}
	printf("\n\n");
}

bool Fitting_GSL::fit()
{
	gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();

	/// reading parameters

	// read main parameters
	const size_t max_iter = multilayer_Approach->fitting_Settings->max_Iter;
	const double xtol = multilayer_Approach->fitting_Settings->x_Tolerance;
	const double gtol = multilayer_Approach->fitting_Settings->g_Tolerance;
	const double ftol = multilayer_Approach->fitting_Settings->f_Tolerance;
	{
		// read method
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Two_Dimensional_Subspace])						{
			if(params->p<2)
			{
				QMessageBox::information(NULL,"Insufficient number of parameters", "Method\n\"Two Dimensional Subspace\"\nrequires at least 2 fitables");
				return false;
			}																														fdf_params.trs = gsl_multifit_nlinear_trs_subspace2D;	}
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Levenberg_Marquardt])							{	fdf_params.trs = gsl_multifit_nlinear_trs_lm;			};
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Levenberg_Marquardt_with_Geodesic_Acceleration]){	fdf_params.trs = gsl_multifit_nlinear_trs_lmaccel;		}
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Dogleg])										{	fdf_params.trs = gsl_multifit_nlinear_trs_dogleg;		}
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Double_Dogleg])									{	fdf_params.trs = gsl_multifit_nlinear_trs_ddogleg;		}

		/// read additional parameters
		// read scaling strategy
		if(multilayer_Approach->fitting_Settings->current_Scale == GSL_Scales[More])			{	fdf_params.scale = gsl_multifit_nlinear_scale_more;			};
		if(multilayer_Approach->fitting_Settings->current_Scale == GSL_Scales[Levenberg])		{	fdf_params.scale = gsl_multifit_nlinear_scale_levenberg;	};
		if(multilayer_Approach->fitting_Settings->current_Scale == GSL_Scales[Marquardt])		{	fdf_params.scale = gsl_multifit_nlinear_scale_marquardt;	};

		// read linear solver
		if(multilayer_Approach->fitting_Settings->current_Solver == GSL_Solvers[QR_decomposition])				{	fdf_params.solver = gsl_multifit_nlinear_solver_qr;			};
		if(multilayer_Approach->fitting_Settings->current_Solver == GSL_Solvers[Cholesky_decomposition])		{	fdf_params.solver = gsl_multifit_nlinear_solver_cholesky;	};
		if(multilayer_Approach->fitting_Settings->current_Solver == GSL_Solvers[Singular_value_decomposition])	{	fdf_params.solver = gsl_multifit_nlinear_solver_svd;		};

		// read fdtype
		if(multilayer_Approach->fitting_Settings->current_Fdtype == GSL_Fdtype[Forward])		{	fdf_params.fdtype = GSL_MULTIFIT_NLINEAR_FWDIFF;			};
		if(multilayer_Approach->fitting_Settings->current_Fdtype == GSL_Fdtype[Central])		{	fdf_params.fdtype = GSL_MULTIFIT_NLINEAR_CTRDIFF;			};

		// read numerical params
		fdf_params.factor_up = multilayer_Approach->fitting_Settings->factor_Up;
		fdf_params.factor_down = multilayer_Approach->fitting_Settings->factor_Down;
		fdf_params.avmax = multilayer_Approach->fitting_Settings->avmax;
		fdf_params.h_df = multilayer_Approach->fitting_Settings->h_df;
		fdf_params.h_fvv = multilayer_Approach->fitting_Settings->h_fvv;
	}

	const gsl_multifit_nlinear_type* T = gsl_multifit_nlinear_trust;
	gsl_multifit_nlinear_workspace* work = gsl_multifit_nlinear_alloc(T, &fdf_params, params->n, params->p);

	gsl_vector* f = gsl_multifit_nlinear_residual(work);
	gsl_vector* x = gsl_multifit_nlinear_position(work);
	init_Position(x);

	gsl_multifit_nlinear_fdf fdf;
		fdf.f = &calc_Residual;
		fdf.df = NULL;
		fdf.fvv = NULL;
		fdf.n = params->n;
		fdf.p = params->p;
		fdf.params = params;

	// storage for the output data
	int info;
	double chisq_Init, chisq_Final;

	// initialize solver
	gsl_multifit_nlinear_init(x, &fdf, work);

	// calc initial cost
	gsl_blas_ddot(f, f, &chisq_Init);

	auto start = std::chrono::system_clock::now();
		// iterate until convergence
		gsl_multifit_nlinear_driver(max_iter, xtol, gtol, ftol,	callback, params, &info, work);
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	qInfo() << "Fit  : "<< elapsed.count()/1000. << " seconds\n";
	qInfo() << "previous_ID =" << previous_ID << endl;

	// calc final cost
	gsl_blas_ddot(f, f, &chisq_Final);

	// print summary
	printf("method              : %s\n",  gsl_multifit_nlinear_trs_name(work));
	printf("NITER               = %zu\n", gsl_multifit_nlinear_niter(work));
	printf("NFEV                = %zu\n", fdf.nevalf);
	printf("initial cost        = %g\n",  chisq_Init);
	printf("final cost          = %g\n",  chisq_Final);
	printf("reason for stopping : %s\n",  (info == 1) ? "small step size" : "small gradient");
	printf("\n");

	gsl_multifit_nlinear_free(work);

	return true;
}

int Fitting_GSL::calc_Residual(const gsl_vector* x, void* bare_Params, gsl_vector* f)
{
	Fitting_Params* params = ((struct Fitting_Params*)bare_Params);

	// calc f
	Fitting::calc_Residual(x, params, f);

	return GSL_SUCCESS;
}

void Fitting_GSL::init_Position(gsl_vector* x)
{
	for(size_t i=0; i<fitables.fit_Parameters.size(); ++i)
	{
		gsl_vector_set(x, i, fitables.fit_Value_Parametrized[i]);
	}
}
