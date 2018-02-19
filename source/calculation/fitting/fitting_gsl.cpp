#include "fitting_gsl.h"

Fitting_GSL::Fitting_GSL(Multilayer_Approach* multilayer_Approach, Main_Calculation_Module* main_Calculation_Module):
	multilayer_Approach(multilayer_Approach),
	main_Calculation_Module(main_Calculation_Module),
	calculation_Trees(main_Calculation_Module->calculation_Trees),
	fitables(main_Calculation_Module->fitables)
{

}

void Fitting_GSL::callback(const size_t iter, void* bare_Params, const gsl_multifit_nlinear_workspace* w)
{
	w=w;
	Params* params = ((struct Params*)bare_Params);

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
	// if expression contains necessary number of functions
	if(check_Residual_Expression()) return false;

	const size_t n = num_Residual_Points();
	const size_t p = fitables.fit_Parameters.size();

	if(n<p)
	{
		QMessageBox::information(NULL,"Insufficient number of points", "# of fitables: " + QString::number(p) + "\n# of points: " + QString::number(n) + "\n\nShould be f>p");
		return false;
	}


	Params params {	main_Calculation_Module, calculation_Trees, fitables };
	gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();

	/// reading parameters

	// read main parameters
	const size_t max_iter = multilayer_Approach->fitting_Settings->max_Iter;
	const double xtol = multilayer_Approach->fitting_Settings->x_Tolerance;
	const double gtol = multilayer_Approach->fitting_Settings->g_Tolerance;
	const double ftol = multilayer_Approach->fitting_Settings->f_Tolerance;
	{
		// read method
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Levenberg_Marquardt])
		{	fdf_params.trs = gsl_multifit_nlinear_trs_lm;		};
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Levenberg_Marquardt_with_Geodesic_Acceleration])
		{	fdf_params.trs = gsl_multifit_nlinear_trs_lmaccel;	}
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Dogleg])
		{	fdf_params.trs = gsl_multifit_nlinear_trs_dogleg;	}
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Double_Dogleg])
		{	fdf_params.trs = gsl_multifit_nlinear_trs_ddogleg;	}
		if(multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Two_Dimensional_Subspace])
		{
			if(p<2)
			{
				QMessageBox::information(NULL,"Insufficient number of parameters", "Method\n\"Two Dimensional Subspace\"\nrequires at least 2 fitables");
				return false;
			}
			fdf_params.trs = gsl_multifit_nlinear_trs_subspace2D;
		}

		/// read additional parameters
		// read scaling strategy
		if(multilayer_Approach->fitting_Settings->current_Scale == GSL_Scales[More])
		{	fdf_params.scale = gsl_multifit_nlinear_scale_more;			};
		if(multilayer_Approach->fitting_Settings->current_Scale == GSL_Scales[Levenberg])
		{	fdf_params.scale = gsl_multifit_nlinear_scale_levenberg;	};
		if(multilayer_Approach->fitting_Settings->current_Scale == GSL_Scales[Marquardt])
		{	fdf_params.scale = gsl_multifit_nlinear_scale_marquardt;	};

		// read linear solver
		if(multilayer_Approach->fitting_Settings->current_Solver == GSL_Solvers[QR_decomposition])
		{	fdf_params.solver = gsl_multifit_nlinear_solver_qr;			};
		if(multilayer_Approach->fitting_Settings->current_Solver == GSL_Solvers[Cholesky_decomposition])
		{	fdf_params.solver = gsl_multifit_nlinear_solver_cholesky;	};
		if(multilayer_Approach->fitting_Settings->current_Solver == GSL_Solvers[Singular_value_decomposition])
		{	fdf_params.solver = gsl_multifit_nlinear_solver_svd;		};

		// read fdtype
		if(multilayer_Approach->fitting_Settings->current_Fdtype == GSL_Fdtype[Forward])
		{	fdf_params.fdtype = GSL_MULTIFIT_NLINEAR_FWDIFF;			};
		if(multilayer_Approach->fitting_Settings->current_Fdtype == GSL_Fdtype[Central])
		{	fdf_params.fdtype = GSL_MULTIFIT_NLINEAR_CTRDIFF;			};

		// read numerical params
		fdf_params.factor_up = multilayer_Approach->fitting_Settings->factor_Up;
		fdf_params.factor_down = multilayer_Approach->fitting_Settings->factor_Down;
		fdf_params.avmax = multilayer_Approach->fitting_Settings->avmax;
		fdf_params.h_df = multilayer_Approach->fitting_Settings->h_df;
		fdf_params.h_fvv = multilayer_Approach->fitting_Settings->h_fvv;
	}

	const gsl_multifit_nlinear_type* T = gsl_multifit_nlinear_trust;
	gsl_multifit_nlinear_workspace* work = gsl_multifit_nlinear_alloc(T, &fdf_params, n, p);

	gsl_vector* f = gsl_multifit_nlinear_residual(work);
	gsl_vector* x = gsl_multifit_nlinear_position(work);
	init_Position(x, &params);

	gsl_multifit_nlinear_fdf fdf;
	fdf.f = &calc_Residual;
	fdf.df = NULL;
	fdf.fvv = NULL;
	fdf.n = n;
	fdf.p = p;
	fdf.params = &params;

	// storage for the output data
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

	qInfo() << "previous_ID =" << previous_ID << endl;

	// store final cost
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
	Params* params = ((struct Params*)bare_Params);

	// change value of real fitables
	for(size_t i=0; i<params->fitables.fit_Parameters.size(); ++i)
	{
		double old_Value = params->fitables.fit_Parameters[i]->value;
		params->fitables.fit_Parameters[i]->value = params->main_Calculation_Module->unparametrize(	gsl_vector_get(x, i),
																									params->fitables.fit_Parameters[i]->fit.min,
																									params->fitables.fit_Parameters[i]->fit.max);
		double new_Value = params->fitables.fit_Parameters[i]->value;

		// recalculate underlying tree if period or gamma
		if(params->fitables.fit_Parameters[i]->indicator.whats_This == whats_This_Period)
		{
			double coeff = new_Value/old_Value;
			period_Subtree_Iteration(params->fitables.fit_Parent_Iterators[i], coeff);

		} else
		if(params->fitables.fit_Parameters[i]->indicator.whats_This == whats_This_Gamma)
		{
			gamma_Subtree_Iteration(params->fitables.fit_Parent_Iterators[i], old_Value);
		}

		// recalculate underlying slaves
		slaves_Recalculation(params->fitables.fit_Parameters[i], params);
	}

	/// now all real_Calc_Tree are changed; we can replicate and stratify them

	int residual_Shift=0;

	// over multilayers
	for(int tab_Index=0; tab_Index<params->calculation_Trees.size(); ++tab_Index)
	{
		// over target curves
		int target_Index = 0;

		// if use reference Data_Element<Target_Curve>& then addresses of slaves are changing!!! WTF?
		for(Data_Element<Target_Curve>& target_Element : params->calculation_Trees[tab_Index]->target)
		{
			// replication of real_Calc_Tree for each target
			target_Element.calc_Tree = params->calculation_Trees[tab_Index]->real_Calc_Tree;
			params->calculation_Trees[tab_Index]->statify_Calc_Tree(target_Element.calc_Tree);

			// calculation
			params->calculation_Trees[tab_Index]->calculate_1_Kind(target_Element);

			// fill residual
			fill_Residual(residual_Shift, target_Element, f, target_Index);
			target_Index++;
		}
	}
	return GSL_SUCCESS;
}

void Fitting_GSL::init_Position(gsl_vector* x, Fitting_Params *params)
{
	// change value of slaves
	for(size_t i=0; i<fitables.fit_Parameters.size(); ++i)
	{
		slaves_Recalculation(fitables.fit_Parameters[i], params);
	}

	// change value of real fitables
	for(size_t i=0; i<fitables.fit_Parameters.size(); ++i)
	{
		fitables.fit_Value_Parametrized[i] = main_Calculation_Module->parametrize(fitables.fit_Parameters[i]->value,
																				  fitables.fit_Parameters[i]->fit.min,
																				  fitables.fit_Parameters[i]->fit.max);
		gsl_vector_set(x, i, fitables.fit_Value_Parametrized[i]);
	}
}

void Fitting_GSL::fill_Residual(int& residual_Shift, Data_Element<Target_Curve>& target_Element, gsl_vector* f, int index)
{
	Target_Curve* target_Curve = target_Element.the_Class;
	double fi_1, fi_2, factor;
	int N = target_Curve->curve.values.size();
	double N_sqrt = sqrt(double(N));

	/// -------------------------------------------------------------------------------
	/// reflectance
	/// -------------------------------------------------------------------------------

	if(target_Curve->curve.value_Mode == value_R_Mode[R] )				// R
	{
		for(int point_Index=0; point_Index<N; ++point_Index)
		{
			// calculate with expression
			{				
#ifdef EXPRTK
				target_Curve->fit_Params.expression_Argument = target_Curve->curve.values[point_Index].val_1;
				fi_1 = target_Curve->fit_Params.expression_Vec[0].value();
#else
				fi_1 = func(target_Curve->curve.values[point_Index].val_1, index);
#endif
			}
			{
#ifdef EXPRTK
				target_Curve->fit_Params.expression_Argument = target_Element.unwrapped_Reflection->R[point_Index];
				fi_2 = target_Curve->fit_Params.expression_Vec[0].value();
#else
//				fi_2 = func(target_Element.unwrapped_Reflection->R[point_Index], index);
				fi_2 = func(target_Element.unwrapped_Reflection->R_Instrumental[point_Index], index);
#endif
			}

			// weight
			factor = target_Curve->fit_Params.weight_Sqrt;
			if(target_Curve->fit_Params.norm) { factor /= N_sqrt; }

			// fill
			gsl_vector_set(f, residual_Shift+point_Index, factor*(fi_1-fi_2));
		}
		residual_Shift += N;
	} else
	if(target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )			// R+phi
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, R_Phi is not ready";
	} else
	if(target_Curve->curve.value_Mode == value_R_Mode[r_Re_Im] )		// r, Re+Im
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, r_Re_Im is not ready";
	} else
	if(target_Curve->curve.value_Mode == value_R_Mode[r_Abs_Phi] )		// |r|+phi
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, r_Abs_Phi is not ready";
	} else

	/// -------------------------------------------------------------------------------
	/// transmittance
	/// -------------------------------------------------------------------------------

	if(target_Curve->curve.value_Mode == value_T_Mode[T] )				// T
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, T is not ready";
	} else

	/// -------------------------------------------------------------------------------
	/// absorptance
	/// -------------------------------------------------------------------------------

	if(target_Curve->curve.value_Mode == value_A_Mode[A] )				// A
	{
		qInfo() << "Fitting_GSL::fill_Residual  :  sorry, A is not ready";
	}
}

bool Fitting_GSL::check_Residual_Expression()
{
	// over multilayers
	for(int tab_Index=0; tab_Index<calculation_Trees.size(); tab_Index++)
	{
		// over target curves
		for(int target_Element_Index=0; target_Element_Index<calculation_Trees[tab_Index]->target.size(); target_Element_Index++)
		{
			Target_Curve* target_Curve = calculation_Trees[tab_Index]->target[target_Element_Index].the_Class;
			QString struct_Name = "  " + Medium_BlackCircle_Sym + "  <" + main_Calculation_Module->multilayer_Tabs->tabText(tab_Index) + "> ";

			/// -------------------------------------------------------------------------------
			/// weights
			/// -------------------------------------------------------------------------------

			if(abs(target_Curve->fit_Params.weight) < DBL_MIN)
			{
				QMessageBox::information(NULL,"Bad weight", "Weight coefficient in\n\n" +
															struct_Name + ", measured curve #" + QString::number(target_Element_Index+1) +
															"\n\nshould be positive");
				return true;
			}

			/// -------------------------------------------------------------------------------
			/// 1 value
			/// -------------------------------------------------------------------------------

			if(target_Curve->curve.value_Mode == value_R_Mode[R] || 			// R
			   target_Curve->curve.value_Mode == value_T_Mode[T] || 			// T
			   target_Curve->curve.value_Mode == value_A_Mode[A] )				// A
			{
				if(target_Curve->fit_Params.expression_Vec.size() < 1)
				{
					QMessageBox::information(NULL,"Bad expression", "Residual function\n\n\"" +
																	target_Curve->fit_Params.fit_Function +
																	"\"\n\nin\n\n" +
																	struct_Name + ", measured curve #" + QString::number(target_Element_Index+1) +
																	"\n\nshould contains >=1 expression, separated by \"" + fit_Function_Separator + "\"");
					return true;
				}
			}

			/// -------------------------------------------------------------------------------
			/// 2 values
			/// -------------------------------------------------------------------------------

			if(target_Curve->curve.value_Mode == value_R_Mode[R_Phi]     || 	// R+phi
			   target_Curve->curve.value_Mode == value_R_Mode[r_Re_Im]   || 	// r, Re+Im
			   target_Curve->curve.value_Mode == value_R_Mode[r_Abs_Phi] )		// |r|+phi
			{
				if(target_Curve->fit_Params.expression_Vec.size() < 2)
				{
					QMessageBox::information(NULL,"Bad expression", "Residual function\n\n\"" +
																	target_Curve->fit_Params.fit_Function +
																	"\"\n\nin\n\n" +
																	struct_Name + ", measured curve #" + QString::number(target_Element_Index+1) +
																	"\n\nshould contains >=1 expression, separated by \"" + fit_Function_Separator + "\"");
					return true;
				}
			}
		}
	}
	return false;
}
