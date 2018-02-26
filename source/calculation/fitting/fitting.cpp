#include "fitting.h"

Fitting::Fitting(Main_Calculation_Module* main_Calculation_Module):

	main_Calculation_Module(main_Calculation_Module),
	calculation_Trees(main_Calculation_Module->calculation_Trees),
	fitables(main_Calculation_Module->fitables),

	n(num_Residual_Points()),
	p(fitables.fit_Parameters.size()),

	params({main_Calculation_Module,
		   calculation_Trees,
		   fitables,
		   p,
		   f,
		   n,
		   x,

		   // for SwarmOps
		   0
		   })
{
	f = gsl_vector_calloc(n);
	x = gsl_vector_calloc(p);
	params.f = f;
	params.x = x;
}

Fitting::~Fitting()
{
	gsl_vector_free(f);
	gsl_vector_free(x);
}

double Fitting::func(double argument, int index)
{
//	if(index == 0)
//	{
//		return log(argument+1E-4);
//	} else
	{
		return argument;
	}
}

size_t Fitting::num_Residual_Points()
{
	size_t residual_Points = 0;
	// over multilayers
	for(int tab_Index=0; tab_Index<calculation_Trees.size(); ++tab_Index)
	{
		// over target curves
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
			// over points
			residual_Points += target_Element.the_Class->curve.values.size();
		}
	}
	return residual_Points;
}

void Fitting::period_Subtree_Iteration(const tree<Node>::iterator& parent, double coeff)
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

void Fitting::gamma_Subtree_Iteration(const tree<Node>::iterator& parent, double old_Value)
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

void Fitting::slaves_Recalculation(Parameter* master, Fitting_Params* params)
{
	for(int slave_Index=0; slave_Index<master->coupled.slaves.size(); ++slave_Index)
	{
		Parameter_Indicator& slave_Parameter_Indicator = master->coupled.slaves[slave_Index];
		Parameter* slave = master->coupled.slave_Pointers[slave_Index];

#ifdef EXPRTK
		// local parsing. not used
//		exprtk::parser<double> parser;
//		exprtk::symbol_table<double> symbol_table;
//		exprtk::expression<double> expression_Exprtk;

//		symbol_table.add_variable(expression_Master_Slave_Variable, master->value);
//		symbol_table.add_constants();

//		expression_Exprtk.register_symbol_table(symbol_table);
//		parser.compile(slave_Parameter_Indicator.expression.toStdString(), expression_Exprtk);
//		slave->value = expression_Exprtk.value();

		int index = params->main_Calculation_Module->slaves_Expression_Map.value(slave->indicator.id);
		params->main_Calculation_Module->argument_Vec[index] = master->value;
		slave->value = params->main_Calculation_Module->expression_Vec[index].value();
#else
		slave->value = master->value;
#endif
		if(slave->coupled.slaves.size()>0)
		{
			slaves_Recalculation(slave, params);
		}
	}
}

void Fitting::calc_Residual(const gsl_vector* x, Fitting_Params* params, gsl_vector* f)
{
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
			Fitting::period_Subtree_Iteration(params->fitables.fit_Parent_Iterators[i], coeff);

		} else
		if(params->fitables.fit_Parameters[i]->indicator.whats_This == whats_This_Gamma)
		{
			Fitting::gamma_Subtree_Iteration(params->fitables.fit_Parent_Iterators[i], old_Value);
		}

		// recalculate underlying slaves
		Fitting::slaves_Recalculation(params->fitables.fit_Parameters[i], params);
	}

	/// now all real_Calc_Tree are changed; we can replicate and stratify them

	int residual_Shift=0;

	// iterate over structures
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
}

void Fitting::fill_Residual(int& residual_Shift, Data_Element<Target_Curve>& target_Element, gsl_vector* f, int index)
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
		qInfo() << "Fitting::fill_Residual  :  sorry, R_Phi is not ready";
	} else
	if(target_Curve->curve.value_Mode == value_R_Mode[r_Re_Im] )		// r, Re+Im
	{
		qInfo() << "Fitting::fill_Residual  :  sorry, r_Re_Im is not ready";
	} else
	if(target_Curve->curve.value_Mode == value_R_Mode[r_Abs_Phi] )		// |r|+phi
	{
		qInfo() << "Fitting::fill_Residual  :  sorry, r_Abs_Phi is not ready";
	} else

	/// -------------------------------------------------------------------------------
	/// transmittance
	/// -------------------------------------------------------------------------------

	if(target_Curve->curve.value_Mode == value_T_Mode[T] )				// T
	{
		qInfo() << "Fitting::fill_Residual  :  sorry, T is not ready";
	} else

	/// -------------------------------------------------------------------------------
	/// absorptance
	/// -------------------------------------------------------------------------------

	if(target_Curve->curve.value_Mode == value_A_Mode[A] )				// A
	{
		qInfo() << "Fitting::fill_Residual  :  sorry, A is not ready";
	}
}

void Fitting::initialize_Position()
{
	// set value of slaves
	for(size_t i=0; i<fitables.fit_Parameters.size(); ++i)
	{
		Fitting::slaves_Recalculation(fitables.fit_Parameters[i], &params);
	}

	// set value of real fitables
	for(size_t i=0; i<fitables.fit_Parameters.size(); ++i)
	{
		fitables.fit_Value_Parametrized[i] = main_Calculation_Module->parametrize(fitables.fit_Parameters[i]->value,
																				  fitables.fit_Parameters[i]->fit.min,
																				  fitables.fit_Parameters[i]->fit.max);
		gsl_vector_set(x, i, fitables.fit_Value_Parametrized[i]);
	}
}

bool Fitting::fit()
{
	if(check_Residual_Expression()) return false;

	// --------------------------------------------------------------------------------
	if(n<=p)
	{
		QMessageBox::information(NULL,"Insufficient number of points", "# of fitables: " + QString::number(p) + "\n# of points: " + QString::number(n) + "\n\nShould be f<p");
		return false;
	}
	// --------------------------------------------------------------------------------
	initialize_Position();
	// --------------------------------------------------------------------------------

	// GSL
	if(	GSL_Methods.contains(global_Multilayer_Approach->fitting_Settings->current_Method) )
	{
		qInfo() << "GSL " << global_Multilayer_Approach->fitting_Settings->current_Method << "optimization";
		Fitting_GSL fitting_GSL(this);
		return fitting_GSL.fit();
	}

	// SO
	if(	SO_Methods.contains(global_Multilayer_Approach->fitting_Settings->current_Method) )
	{
		qInfo() << "SO " << global_Multilayer_Approach->fitting_Settings->current_Method << "optimization";
		Fitting_SwarmOps fitting_SwarmOps(this);
		return fitting_SwarmOps.fit();
	}

	return false;
}

bool Fitting::check_Residual_Expression()
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
