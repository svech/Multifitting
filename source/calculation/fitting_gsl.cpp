#include "fitting_gsl.h"

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
			// over points
			residual_Points += target_Element.the_Class->curve.values.size();
		}
	}
	return residual_Points;
}

void Fitting_GSL::callback(const size_t iter, void* bare_Params, const gsl_multifit_nlinear_workspace* w)
{
	w=w;
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

	// storage for the output data
	int info;
	double chisq_Init, chisq_Final;

	// initialize solver
	gsl_multifit_nlinear_init(x, &fdf, work);

	// calc initial cost
	gsl_blas_ddot(f, f, &chisq_Init);

	auto start = std::chrono::system_clock::now();
		// iterate until convergence
//		gsl_multifit_nlinear_driver(max_iter, xtol, gtol, ftol,	callback, &params, &info, work);
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	qInfo() << "Fit  : "<< elapsed.count()/1000. << " seconds\n";

	// store final cost
	gsl_blas_ddot(f, f, &chisq_Final);

	// print summary
	printf("method              : %s\n", gsl_multifit_nlinear_trs_name(work));
	printf("NITER               = %zu\n", gsl_multifit_nlinear_niter(work));
	printf("NFEV                = %zu\n", fdf.nevalf);
	printf("initial cost        = %g\n", chisq_Init);
	printf("final cost          = %g\n", chisq_Final);
	printf("reason for stopping : %s\n", (info == 1) ? "small step size" : "small gradient");
	printf("\n");

	gsl_multifit_nlinear_free(work);
}

void Fitting_GSL::period_Subtree_Iteration(const tree<Node>::iterator& parent, double coeff)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& struct_Data = child.node->data.struct_Data;

		if(abs(coeff)>DBL_MIN) // Do we need it?
		{
			if(struct_Data.item_Type == item_Type_Layer)
			{
				struct_Data.thickness.value *= coeff;
			} else
			if(struct_Data.item_Type == item_Type_Multilayer)
			{
				struct_Data.period.value *= coeff;
				period_Subtree_Iteration(child, coeff);
			}
		}
	}
}

void Fitting_GSL::gamma_Subtree_Iteration(const tree<Node>::iterator& parent, double old_Value)
{
	Data& gamma_Multilayer_Data = parent.node->data.struct_Data;
	double old_First_Thickness = gamma_Multilayer_Data.period.value * old_Value;
	double new_First_Thickness = gamma_Multilayer_Data.period.value * gamma_Multilayer_Data.gamma.value;

	unsigned i=0;
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			child_Data.thickness.value = new_First_Thickness;
		} else
		if(child_Data.item_Type == item_Type_Multilayer)
		{
			if(abs(old_First_Thickness) > DBL_MIN)
			{
				double coeff = new_First_Thickness/old_First_Thickness;
				child_Data.period.value *= coeff;
				period_Subtree_Iteration(child, coeff);
			}
		}
	}

	double old_Second_Thickness = gamma_Multilayer_Data.period.value - old_First_Thickness;
	double new_Second_Thickness = gamma_Multilayer_Data.period.value - new_First_Thickness;

	i=1;
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			child_Data.thickness.value = new_Second_Thickness;
		} else
		if(child_Data.item_Type == item_Type_Multilayer)
		{
			if(abs(old_Second_Thickness) > DBL_MIN)
			{
				double coeff = new_Second_Thickness/old_Second_Thickness;
				child_Data.period.value *= coeff;
				period_Subtree_Iteration(child, coeff);
			}
		}
	}
}

int Fitting_GSL::calc_Residual(const gsl_vector* x, void* bare_Params, gsl_vector* f)
{
	Params* params = ((struct Params*)bare_Params);

	// change value of real fitables
	for(size_t i=0; i<params->fitables.fit_Value_Pointers.size(); ++i)
	{
		double old_Value = *params->fitables.fit_Value_Pointers[i];
		*params->fitables.fit_Value_Pointers[i]=params->main_Calculation_Module->unparametrize(	gsl_vector_get(x, i),
																								params->fitables.fit_Min[i],
																								params->fitables.fit_Max[i]);
		double new_Value = *params->fitables.fit_Value_Pointers[i];

		// special cases
		if(params->fitables.fit_Whats_This[i] == whats_This_Period)
		{
			double coeff = new_Value/old_Value;
			period_Subtree_Iteration(params->fitables.fit_Parent_Iterators[i], coeff);
		} else
		if(params->fitables.fit_Whats_This[i] == whats_This_Gamma)
		{
			gamma_Subtree_Iteration(params->fitables.fit_Parent_Iterators[i], old_Value);
		}
	}

	/// now all real_Calc_Tree are changed; we can replicate and stratify them

	int residual_Index=0;

	// over multilayers
	for(Calculation_Tree* calculation_Tree : params->calculation_Trees)
	{
		// over target curves
		for(Data_Element<Target_Curve>& target_Element : calculation_Tree->target)
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
	qInfo()<< "\nFitting_GSL::fill_Residual\n";
	target_Element.the_Class->fit_Params.expression_Argument = 1;

	for(int i=0; i<target_Element.the_Class->fit_Params.expression_Vec.size(); ++i)
	{
		qInfo() << "val" << i << "=" << target_Element.the_Class->fit_Params.expression_Vec[i].value();
	}

	/// -------------------------------------------------------------------------------
	/// reflectance
	/// -------------------------------------------------------------------------------

	if(target_Element.the_Class->curve.value_Mode == value_R_Mode[R] )				// R
	{
		for( ; residual_Index<target_Element.the_Class->curve.values.size(); ++residual_Index)
		{
			double fi = log(target_Element.the_Class->curve.values[residual_Index].val_1+1e-11) - log(target_Element.unwrapped_Reflection->R[residual_Index]+1e-11);
//			double fi = log(target_Element.the_Class->curve.values[residual_Index].val_1+1e-8) - log(target_Element.unwrapped_Reflection->R[residual_Index]+1e-8);
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
