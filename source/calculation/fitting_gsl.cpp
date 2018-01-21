#include "fitting_gsl.h"

QTextStream& operator <<( QTextStream& stream, const gsl_vector* gsl_vec )
{
	for(size_t i=0; i<gsl_vec->size; ++i)
	{
		stream << gsl_vector_get(gsl_vec, i);
	}
	return stream;
}


Fitting_GSL::Fitting_GSL(Main_Calculation_Module* main_Calculation_Module):
	main_Calculation_Module(main_Calculation_Module),
	calculation_Trees(main_Calculation_Module->calculation_Trees),
	fitables(main_Calculation_Module->fitables)
{

}

size_t Fitting_GSL::num_Residual_Points()
{
	size_t residual_Points = 0;
	// over multilayers
	for(Calculation_Tree* calculation_Tree : calculation_Trees)
	{
		// over target curves
		for(Data_Element<Target_Curve>& target_Element : calculation_Tree->target)
		{
			// loaded_And_ready and fit_Params.calc values have been already checked in Calculation_Tree constructor
			if(target_Element.the_Class->fit_Params.fit)
			{
				// over points
				residual_Points += target_Element.the_Class->curve.values.size();
			}
		}
	}
	return residual_Points;
}

void Fitting_GSL::callback(const size_t iter, void* bare_Params, const gsl_multifit_nlinear_workspace* w)
{
	Params* params = ((struct Params*)bare_Params);

	// print out current location
	printf("iter %zu :", iter);
	for(size_t i=0; i<params->fitables.fit_Value_Pointers.size(); ++i)
	{
		printf("%f ", *params->fitables.fit_Value_Pointers[i]);
	}
	printf("\n\n");
}

void Fitting_GSL::fit()
{
	const size_t n = num_Residual_Points();
	const size_t p = fitables.fit_Value_Pointers.size();
	const size_t max_iter = 200;
	const double xtol = 1.0e-8;
	const double gtol = 1.0e-8;
	const double ftol = 1.0e-8;

	Params params {	main_Calculation_Module, calculation_Trees, fitables };
	const gsl_multifit_nlinear_type* T = gsl_multifit_nlinear_trust;
	gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();

	fdf_params.trs = gsl_multifit_nlinear_trs_lm;
//	fdf_params.trs = gsl_multifit_nlinear_trs_lmaccel;
//	fdf_params.trs = gsl_multifit_nlinear_trs_dogleg;
//	fdf_params.trs = gsl_multifit_nlinear_trs_ddogleg;
//	fdf_params.trs = gsl_multifit_nlinear_trs_subspace2D;

	gsl_multifit_nlinear_workspace* work = gsl_multifit_nlinear_alloc(T, &fdf_params, n, p);

	gsl_vector* f = gsl_multifit_nlinear_residual(work);
	gsl_vector* x = gsl_multifit_nlinear_position(work);
	init_Position(x);

	gsl_multifit_nlinear_fdf fdf;
	fdf.f = &calc_Residual;
	fdf.df = NULL;
	fdf.fvv = NULL;
	fdf.n = n;
	fdf.p = p;
	fdf.params = &params;

	int info;
	double chisq_Init, chisq_Final;

	// initialize solver
	gsl_multifit_nlinear_init(x, &fdf, work);

	// calc initial cost
	gsl_blas_ddot(f, f, &chisq_Init);


	auto start = std::chrono::system_clock::now();

	// iterate until convergence
	gsl_multifit_nlinear_driver(max_iter, xtol, gtol, ftol,	callback, &params, &info, work);

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	qInfo() << "Fit  : "<< elapsed.count()/1000. << " seconds\n";

	// store final cost
	gsl_blas_ddot(f, f, &chisq_Final);

	// print summary
	printf("METHOD        = %s\n", gsl_multifit_nlinear_trs_name(work));
	printf("NITER         = %zu\n", gsl_multifit_nlinear_niter(work));
	printf("NFEV          = %zu\n", fdf.nevalf);
	printf("initial cost  = %.12e\n", chisq_Init);
	printf("final cost    = %.12e\n", chisq_Final);
	printf("final x       = (%.12e)\n\n", gsl_vector_get(x, 0));
	printf("\n");

	gsl_multifit_nlinear_free(work);
}

int Fitting_GSL::calc_Residual(const gsl_vector* x, void* bare_Params, gsl_vector* f)
{
	Params* params = ((struct Params*)bare_Params);

	// change value of real fitables
	for(size_t i=0; i<params->fitables.fit_Value_Pointers.size(); ++i)
	{
		*params->fitables.fit_Value_Pointers[i]=params->main_Calculation_Module->unparametrize(
																								gsl_vector_get(x, i),
																								params->fitables.fit_Min[i],
																								params->fitables.fit_Max[i]);
	}

	/// now all real_Calc_Tree are changed; we can replicate and stratify them

	int residual_Index=0;

	// over multilayers
	for(Calculation_Tree* calculation_Tree : params->calculation_Trees)
	{
		// over target curves
		for(Data_Element<Target_Curve>& target_Element : calculation_Tree->target)
		{
			// loaded_And_ready and fit_Params.calc values have been already checked in Calculation_Tree constructor
			if(target_Element.the_Class->fit_Params.fit)
			{
				// replication of real_Calc_Tree for each target
				target_Element.calc_Tree = calculation_Tree->real_Calc_Tree;
				calculation_Tree->statify_Calc_Tree(target_Element.calc_Tree);

				// calculation
				calculation_Tree->calculate_1_Kind(target_Element);

				// fill residual
				fill_Residual(residual_Index, target_Element, f);
			}
		}
	}

	return GSL_SUCCESS;
}

void Fitting_GSL::init_Position(gsl_vector* x)
{
	// change value of real fitables
	for(size_t i=0; i<fitables.fit_Value_Pointers.size(); ++i)
	{
		gsl_vector_set(x, i, fitables.fit_Value_Parametrized[i]);
	}
}

void Fitting_GSL::fill_Residual(int& residual_Index, Data_Element<Target_Curve>& target_Element, gsl_vector* f)
{
	/// -------------------------------------------------------------------------------
	/// reflectance
	/// -------------------------------------------------------------------------------

	if(target_Element.the_Class->curve.value_Mode == value_R_Mode[R] )				// R
	{
		for( ; residual_Index<target_Element.the_Class->curve.values.size(); ++residual_Index)
		{
			double fi = log(target_Element.the_Class->curve.values[residual_Index].val_1+1e-11) - log(target_Element.unwrapped_Reflection->R[residual_Index]+1e-11);
			gsl_vector_set(f, residual_Index, fi);
		}
	} else
	if(target_Element.the_Class->curve.value_Mode == value_R_Mode[R_Phi] )			// R+phi
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, R_Phi is not ready";
	} else
	if(target_Element.the_Class->curve.value_Mode == value_R_Mode[r_Re_Im] )		// r, Re+Im
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, r_Re_Im is not ready";
	} else
	if(target_Element.the_Class->curve.value_Mode == value_R_Mode[r_Abs_Phi] )		// |r|+phi
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, r_Abs_Phi is not ready";
	} else

	/// -------------------------------------------------------------------------------
	/// transmittance
	/// -------------------------------------------------------------------------------

	if(target_Element.the_Class->curve.value_Mode == value_T_Mode[T] )				// T
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, T is not ready";
	} else

	/// -------------------------------------------------------------------------------
	/// absorptance
	/// -------------------------------------------------------------------------------

	if(target_Element.the_Class->curve.value_Mode == value_A_Mode[A] )				// A
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, A is not ready";
	}
}

void Fitting_GSL::print_gsl_Vector(gsl_vector* f)
{
	for(size_t i=0; i<f->size; ++i)
	{
		printf("%f  ", gsl_vector_get(f, i));
	}
	printf("\n");
}
