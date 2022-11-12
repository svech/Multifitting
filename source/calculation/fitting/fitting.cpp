#include "fitting.h"

Fitting::Fitting(Main_Calculation_Module* main_Calculation_Module):
	main_Calculation_Module(main_Calculation_Module),
	calculation_Trees	(main_Calculation_Module->calculation_Trees),
	fitables			(main_Calculation_Module->fitables),
	confidentials		(main_Calculation_Module->confidentials),
	number_Of_Restricted_Regular_Components(main_Calculation_Module->number_Of_Restricted_Regular_Components),

	n_Full_With_GISAS(num_Residual_Points_Full_With_GISAS(main_Calculation_Module->calculation_Trees)+main_Calculation_Module->number_Of_Restricted_Regular_Components),
	n(num_Residual_Points(main_Calculation_Module->calculation_Trees)+main_Calculation_Module->number_Of_Restricted_Regular_Components),
	p(fitables.param_Pointers.size()),

	params({main_Calculation_Module,
		   calculation_Trees,
		   fitables,
		   confidentials,
		   main_Calculation_Module->calc_Mode,
		   p,
		   f,
		   n_Full_With_GISAS,
		   n,
		   x,

		   -2018,
		   -2018,

		   // for SwarmOps
		   0
		   // no my_Res initialization
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

double Fitting::func(double argument)
{
	return log(argument+1E-6);
}

size_t Fitting::num_Residual_Points(QVector<Calculation_Tree*>& calculation_Trees)
{
	size_t residual_Points = 0;
	// over multilayers
	for(int tab_Index=0; tab_Index<calculation_Trees.size(); ++tab_Index)
	{
		// over target curves
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
			// over points
			if(target_Element.the_Class->measurement.measurement_Type != measurement_Types[GISAS_Map] )
			{
				residual_Points += target_Element.the_Class->curve.shifted_Values.size();
			} else
			{
				residual_Points += target_Element.the_Class->curve.value_2D_Shifted.front().size();
			}
		}
	}
	return residual_Points;
}

size_t Fitting::num_Residual_Points_Full_With_GISAS(QVector<Calculation_Tree*>& calculation_Trees)
{
	size_t residual_Points = 0;
	// over multilayers
	for(int tab_Index=0; tab_Index<calculation_Trees.size(); ++tab_Index)
	{
		// over target curves
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
			// over points
			if(target_Element.the_Class->measurement.measurement_Type != measurement_Types[GISAS_Map] )
			{
				residual_Points += target_Element.the_Class->curve.shifted_Values.size();
			} else
			{
				residual_Points += target_Element.the_Class->curve.value_2D_Shifted.front().size()*target_Element.the_Class->curve.value_2D_Shifted.size();
			}
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
//		Parameter_Indicator& slave_Parameter_Indicator = master->coupled.slaves[slave_Index];
		Parameter* slave = master->coupled.slave_Pointers[slave_Index];

#ifdef EXPRTK
		// local parsing. not used, slower
//		exprtk::parser<double> parser;
//		exprtk::symbol_table<double> symbol_table;
//		exprtk::expression<double> expression_Exprtk;

//		symbol_table.add_variable(expression_Master_Slave_Variable, master->value);
//		symbol_table.add_constants();

//		expression_Exprtk.register_symbol_table(symbol_table);
//		parser.compile(slave_Parameter_Indicator.expression.toStdString(), expression_Exprtk);
//		slave->value = expression_Exprtk.value();

		int index = params->main_Calculation_Module->slaves_Expression_Map.value(slave->indicator.id);
		// argument changes automatically
//		*params->main_Calculation_Module->argument_Vec[index] = master->value;
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
	// make gui responsive
	QCoreApplication::processEvents();

	// change value of real fitables
	for(size_t i=0; i<params->fitables.param_Pointers.size(); ++i)
	{
		double old_Value = params->fitables.param_Pointers[i]->value;

		// refresh
		params->fitables.param_Pointers[i]->value = params->main_Calculation_Module->unparametrize(	gsl_vector_get(x, i),
																									params->fitables.param_Pointers[i]->fit.min,
																									params->fitables.param_Pointers[i]->fit.max);
		double new_Value = params->fitables.param_Pointers[i]->value;
		change_Real_Fitables_and_Dependent(params, old_Value, new_Value, i, FITTING);
	}

	// change value of slaves of ACTIVE confidentials
	if(params->calc_Mode == CONFIDENCE )
	{
		for(size_t i=0; i<params->confidentials.param_Pointers.size(); ++i)
		{
			if(params->confidentials.param_Pointers[i]->confidence.is_Active)
			{
				double old_Value = params->confidentials.param_Pointers[i]->value;
				double new_Value = params->confidentials.values[i];

				// refresh
				params->confidentials.param_Pointers[i]->value = params->confidentials.values[i];
				change_Real_Fitables_and_Dependent(params, old_Value, new_Value, i, CONFIDENCE);
			}
		}
	}

	/// now all real_Calc_Tree are changed; we can replicate and stratify them
	int residual_Shift = 0;

	// iterate over structures
	for(int tab_Index=0; tab_Index<params->calculation_Trees.size(); ++tab_Index)
	{
		// refresh common number of media
		params->calculation_Trees[tab_Index]->num_Media_Sharp = params->calculation_Trees[tab_Index]->get_Total_Num_Layers(params->calculation_Trees[tab_Index]->real_Calc_Tree.begin()); // non-discretized

		// if use reference Data_Element<Target_Curve>& then addresses of slaves are changing!!! WTF?
		for(Data_Element<Target_Curve>& target_Element : params->calculation_Trees[tab_Index]->target)
		{
			target_Element.calc_Tree = params->calculation_Trees[tab_Index]->real_Calc_Tree;

			// unstratified
			target_Element.media_Data_Map_Vector.resize(params->calculation_Trees[tab_Index]->num_Media_Sharp);
			target_Element.media_Period_Index_Map_Vector.resize(params->calculation_Trees[tab_Index]->num_Media_Sharp);
//	WRONG	params->calculation_Trees[tab_Index]->unwrap_Calc_Tree_Data(params->calculation_Trees[tab_Index]->real_Calc_Tree.begin(), target_Element.media_Data_Map_Vector, target_Element.media_Period_Index_Map_Vector);
			params->calculation_Trees[tab_Index]->unwrap_Calc_Tree_Data(target_Element.calc_Tree.begin(), target_Element.media_Data_Map_Vector, target_Element.media_Period_Index_Map_Vector);

			params->calculation_Trees[tab_Index]->stratify_Calc_Tree(target_Element.calc_Tree);

			// stratified
			target_Element.media_Node_Map_Vector.resize(params->calculation_Trees[tab_Index]->num_Media_Sharp);
			target_Element.flat_Calc_Tree.clear();
			params->calculation_Trees[tab_Index]->flatten_Tree(target_Element.calc_Tree.begin(), target_Element.calc_Tree, target_Element.flat_Calc_Tree);
			params->calculation_Trees[tab_Index]->short_Tree(target_Element.flat_Calc_Tree, target_Element.short_Flat_Calc_Tree);
			params->calculation_Trees[tab_Index]->unwrap_Calc_Tree_Node(target_Element.calc_Tree.begin(), target_Element.media_Node_Map_Vector);

			// calculation
			params->main_Calculation_Module->calculation_With_Sampling(params->calculation_Trees[tab_Index], target_Element);
			params->main_Calculation_Module->postprocessing(target_Element, params->calculation_Trees[tab_Index]->multilayer, true);

			// fill residual
			fill_Residual(params, residual_Shift, target_Element, f);
		}
	}

	/// addition to residual from restrictions of regular aperiodics
	size_t counter = 0;
	for(int tab_Index=0; tab_Index<params->main_Calculation_Module->multilayers.size(); ++tab_Index)
	{
		regular_Restriction_Tree_Iteration(params->calculation_Trees[tab_Index]->real_Calc_Tree.begin(), params, f, counter);
	}

	// replot
	// 1D
	if(replot_graphs_during_fitting_1D && global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		for(QVector<Curve_Plot_1D*>& tab_Plots : global_Multilayer_Approach->optical_Graphs_1D->plots_1D)
		{
			for(Curve_Plot_1D* curve_Plot : tab_Plots)
			{
				bool plot_Options_First_Rescale  = curve_Plot->plot_Options_First.rescale;
				bool plot_Options_Second_Rescale = curve_Plot->plot_Options_Second.rescale;
				curve_Plot->plot_Options_First .rescale = false;
				curve_Plot->plot_Options_Second.rescale = false;
				curve_Plot->plot_All_Data();
				curve_Plot->plot_Options_First .rescale = plot_Options_First_Rescale;
				curve_Plot->plot_Options_Second.rescale = plot_Options_Second_Rescale;
			}
		}
	}
	// 2D
//	if(replot_graphs_during_fitting_2D && global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
//	{
//		for(QVector<Curve_Plot_2D*>& tab_Plots : global_Multilayer_Approach->optical_Graphs_2D->plots_2D)
//		{
//			for(Curve_Plot_2D* curve_Plot : tab_Plots)
//			{
//				// TODO
//				qInfo() << "Fitting::calc_Residual  :   replot_Graphs_During_Fitting 2D" << endl;
//				bool plot_Options_First_Rescale  = curve_Plot->plot_Options_First.rescale;
//				bool plot_Options_Second_Rescale = curve_Plot->plot_Options_Second.rescale;
//				curve_Plot->plot_Options_First .rescale = false;
//				curve_Plot->plot_Options_Second.rescale = false;
//				curve_Plot->plot_All_Data();
//				curve_Plot->plot_Options_First .rescale = plot_Options_First_Rescale;
//				curve_Plot->plot_Options_Second.rescale = plot_Options_Second_Rescale;
//			}
//		}
//	}
}

void Fitting::regular_Restriction_Tree_Iteration(const tree<Node>::iterator& parent, Fitting_Params* params, gsl_vector* f, size_t& counter)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& struct_Data = child.node->data.struct_Data;

		if(struct_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			for(int k=0; k<struct_Data.regular_Components.size(); k++)
			{
				Regular_Component& regular_Component = struct_Data.regular_Components[k];
				if(regular_Component.use_Soft_Restrictions)
				{
					++counter;

					double addition = 0;
					double mean = regular_Component.get_Mean_Thickness();
					for(int n=0; n<struct_Data.num_Repetition.value(); n++)
					{
						Data& regular_Data = regular_Component.components[n];

						double deviation = abs(regular_Data.thickness.value - mean)/mean;
						double exceeding = deviation - double(regular_Component.threshold)/100.; // in relative units
						if(exceeding>0)
						{
//							addition += pow(exceeding*regular_Component.Q_factor, regular_Component.power); // not sure
							addition += pow(exceeding, regular_Component.power)*regular_Component.Q_factor;
						}
					}					
					gsl_vector_set(f, params->n-counter, sqrt(addition));
				}
			}
		}

		// deeper
		if( struct_Data.item_Type == item_Type_Multilayer ||
			struct_Data.item_Type == item_Type_General_Aperiodic )
		{
			regular_Restriction_Tree_Iteration(child, params, f, counter);
		}
	}
}

void Fitting::change_Real_Fitables_and_Dependent(Fitting_Params* params, double old_Value, double new_Value, size_t i, QString fit_Conf)
{
	double coeff = 1;
	if(old_Value>1E-100 && new_Value<1E100) {coeff = new_Value/old_Value;}

	if(fit_Conf == FITTING)
	{
		// recalculate underlying tree if period or gamma is fitable
		if(params->fitables.param_Pointers[i]->indicator.whats_This == whats_This_Period)		{
			Fitting::period_Subtree_Iteration(params->fitables.parent_Iterators[i], coeff);
		}
		if(params->fitables.param_Pointers[i]->indicator.whats_This == whats_This_Gamma)		{
			Fitting::gamma_Subtree_Iteration(params->fitables.parent_Iterators[i], old_Value);
		}
		// recalculate underlying slaves
		Fitting::slaves_Recalculation(params->fitables.param_Pointers[i], params);
	}
	if(fit_Conf == CONFIDENCE)
	{
		// recalculate underlying tree if period or gamma is confidential
		if(params->confidentials.param_Pointers[i]->indicator.whats_This == whats_This_Period)		{
			Fitting::period_Subtree_Iteration(params->confidentials.parent_Iterators[i], coeff);
		}
		if(params->confidentials.param_Pointers[i]->indicator.whats_This == whats_This_Gamma)		{
			Fitting::gamma_Subtree_Iteration(params->confidentials.parent_Iterators[i], old_Value);
		}
		// recalculate underlying slaves
		Fitting::slaves_Recalculation(params->confidentials.param_Pointers[i], params);
	}
}

void Fitting::fill_Residual(Fitting_Params* params, int& residual_Shift, Data_Element<Target_Curve>& target_Element, gsl_vector* f)
{
	Target_Curve* target_Curve = target_Element.the_Class;
	double fi_1, fi_2, fi_1_next, fi_2_next, factor;
	size_t N = -2020;
	double N_sqrt = -2020;
	double n_P_sqrt = 2020;
	size_t param_Names_Size = params->fitables.param_Names.size();
	if(target_Curve->measurement.measurement_Type != measurement_Types[GISAS_Map] )
	{
		n_P_sqrt = sqrt(double(params->n - param_Names_Size));
		N = target_Curve->curve.shifted_Values.size();
		N_sqrt = sqrt(double(N));
	} else
	// GISAS
	{
		n_P_sqrt = sqrt(double(params->n_Full_With_GISAS - param_Names_Size));
		N = target_Curve->curve.value_2D_Shifted.front().size();
		size_t phi_N = target_Curve->curve.value_2D_Shifted.size();
		N_sqrt = sqrt(double(N*phi_N));
	}
	double power = target_Curve->fit_Params.power/2.;

	vector<double> model_Curve(N);

	if(target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if(target_Curve->curve.value_Type == value_Types[Reflectance] )		model_Curve = target_Element.unwrapped_Reflection->calculated_Values.R_Instrumental;
		if(target_Curve->curve.value_Type == value_Types[Transmittance] )	model_Curve = target_Element.unwrapped_Reflection->calculated_Values.T_Instrumental;
	}
	if( target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan])
	{
		model_Curve = target_Element.unwrapped_Reflection->calculated_Values.S_Instrumental;
	}

	// use_Chi2
	if(!target_Curve->fit_Params.maximize_Integral)
	{
		if(target_Curve->fit_Params.use_Chi2)
		{
			factor = target_Curve->fit_Params.weight_Sqrt/n_P_sqrt;
			for(size_t point_Index=0; point_Index<N; ++point_Index)
			{
				double f_Val = 0;
				if(target_Curve->measurement.measurement_Type != measurement_Types[GISAS_Map] )
				{
					bool in_Interval = false;
					if(target_Curve->curve.outer_Area)
					{
						in_Interval = (target_Curve->curve.shifted_Argument[point_Index]<target_Curve->curve.subinterval_Left) ||
									  (target_Curve->curve.shifted_Argument[point_Index]>target_Curve->curve.subinterval_Right);
					} else
					{
						in_Interval = (target_Curve->curve.shifted_Argument[point_Index]>=target_Curve->curve.subinterval_Left) &&
									  (target_Curve->curve.shifted_Argument[point_Index]<=target_Curve->curve.subinterval_Right);
					}

					// if subinterval then use only data from inside
					if( !target_Curve->curve.use_Subinterval || in_Interval	)
					{
//						fi_1 = target_Curve->curve.shifted_Values[point_Index].val_1;
						fi_1 = target_Curve->curve.shifted_Values_No_Scaling_And_Offset[point_Index]*
							   target_Curve->curve.val_Factor.value +
							   target_Curve->curve.val_Shift;
						fi_2 = max(model_Curve[point_Index], DBL_MIN);
						f_Val = factor*(fi_1-fi_2)*sqrt(target_Curve->curve.beam_Time*(target_Curve->curve.beam_Intensity_Initial+target_Curve->curve.beam_Intensity_Final)/(2*fi_2));
					}
				} else
				// GISAS
				{
					f_Val = 0;
					fi_2 = DBL_EPSILON;
					for(size_t phi_Index = 0; phi_Index<target_Curve->curve.value_2D_Shifted.size(); phi_Index++)
					{
						bool in_Rectangle = false;
						if(target_Curve->curve.outer_Area)
						{
							in_Rectangle = (target_Curve->measurement.detector_Theta_Angle_Vec[point_Index]<target_Curve->curve.subinterval_Left)  ||
										   (target_Curve->measurement.detector_Theta_Angle_Vec[point_Index]>target_Curve->curve.subinterval_Right) ||
										   (target_Curve->measurement.detector_Phi_Angle_Vec  [point_Index]<target_Curve->curve.subinterval_Bottom)||
										   (target_Curve->measurement.detector_Phi_Angle_Vec  [point_Index]>target_Curve->curve.subinterval_Top);
						} else
						{
							in_Rectangle = (target_Curve->measurement.detector_Theta_Angle_Vec[point_Index]>=target_Curve->curve.subinterval_Left)  &&
										   (target_Curve->measurement.detector_Theta_Angle_Vec[point_Index]<=target_Curve->curve.subinterval_Right) &&
										   (target_Curve->measurement.detector_Phi_Angle_Vec  [point_Index]>=target_Curve->curve.subinterval_Bottom)&&
										   (target_Curve->measurement.detector_Phi_Angle_Vec  [point_Index]<=target_Curve->curve.subinterval_Top);
						}

						// if subinterval then use only data from inside
						if( !target_Curve->curve.use_Subinterval || in_Rectangle	)
						{
							fi_2 += target_Element.unwrapped_Reflection->calculated_Values.GISAS_Instrumental[phi_Index][point_Index];
							fi_1 = pow(target_Curve->curve.value_2D_No_Scaling_And_Offset[phi_Index][point_Index] *
										 target_Curve->curve.val_Factor.value +
										 target_Curve->curve.val_Shift
										 -
										 target_Element.unwrapped_Reflection->calculated_Values.GISAS_Instrumental[phi_Index][point_Index]
										,2);

							if(!isnan(fi_1)) f_Val += fi_1;
						}
					}
					f_Val = factor * sqrt(f_Val) * sqrt(target_Curve->curve.beam_Time*(target_Curve->curve.beam_Intensity_Initial+target_Curve->curve.beam_Intensity_Final)/(2*fi_2));
				}

				// fill
				gsl_vector_set(f, residual_Shift+point_Index, f_Val );
			}
		} else
		// use custom expression
		{
			// weight
			factor = target_Curve->fit_Params.weight_Sqrt;
			if(target_Curve->fit_Params.norm) { factor /= N_sqrt; }

			for(size_t point_Index=0; point_Index<N; ++point_Index)
			{
				double f_Val = 0;
				// calculate with expression
				if(target_Curve->measurement.measurement_Type != measurement_Types[GISAS_Map] )
				{
					f_Val = 0;
					bool in_Interval = false;
					if(target_Curve->curve.outer_Area)
					{
						in_Interval = (target_Curve->curve.shifted_Argument[point_Index]<target_Curve->curve.subinterval_Left) ||
									  (target_Curve->curve.shifted_Argument[point_Index]>target_Curve->curve.subinterval_Right);
					} else
					{
						in_Interval = (target_Curve->curve.shifted_Argument[point_Index]>=target_Curve->curve.subinterval_Left) &&
									  (target_Curve->curve.shifted_Argument[point_Index]<=target_Curve->curve.subinterval_Right);
					}

					// if subinterval then use only data from inside
					if( !target_Curve->curve.use_Subinterval || in_Interval	)
					{
						{
#ifdef EXPRTK
//							target_Curve->fit_Params.expression_Argument = target_Curve->curve.shifted_Values[point_Index].val_1;
							target_Curve->fit_Params.expression_Argument = target_Curve->curve.shifted_Values_No_Scaling_And_Offset[point_Index]*
																		   target_Curve->curve.val_Factor.value +
																		   target_Curve->curve.val_Shift;

							fi_1 = target_Curve->fit_Params.expression_Vec[0].value();
#else
//							fi_1 = func(target_Curve->curve.shifted_Values[point_Index].val_1, index);
							fi_1 = func(target_Curve->curve.shifted_Values_No_Scaling_And_Offset[point_Index]*
										target_Curve->curve.val_Factor.value +
										target_Curve->curve.val_Shift);
#endif
						}
						{
#ifdef EXPRTK
							target_Curve->fit_Params.expression_Argument = model_Curve[point_Index];
							fi_2 = target_Curve->fit_Params.expression_Vec[0].value();
#else
							fi_2 = func(model_Curve[point_Index]);
#endif
						}
						f_Val = factor*pow(abs(fi_1-fi_2),power);
					}
				} else
				// GISAS
				{
					f_Val = 0;
					double f_Temp_Phi = 0;
					fi_2 = DBL_EPSILON;
					for(size_t phi_Index = 0; phi_Index<target_Curve->curve.value_2D_Shifted.size(); phi_Index++)
					{
						bool in_Rectangle = false;
						if(target_Curve->curve.outer_Area)
						{
							in_Rectangle = (target_Curve->measurement.detector_Theta_Angle_Vec[point_Index]<target_Curve->curve.subinterval_Left)  ||
										   (target_Curve->measurement.detector_Theta_Angle_Vec[point_Index]>target_Curve->curve.subinterval_Right) ||
										   (target_Curve->measurement.detector_Phi_Angle_Vec  [phi_Index]<target_Curve->curve.subinterval_Bottom)||
										   (target_Curve->measurement.detector_Phi_Angle_Vec  [phi_Index]>target_Curve->curve.subinterval_Top);
						} else
						{
							in_Rectangle = (target_Curve->measurement.detector_Theta_Angle_Vec[point_Index]>=target_Curve->curve.subinterval_Left)  &&
										   (target_Curve->measurement.detector_Theta_Angle_Vec[point_Index]<=target_Curve->curve.subinterval_Right) &&
										   (target_Curve->measurement.detector_Phi_Angle_Vec  [phi_Index]>=target_Curve->curve.subinterval_Bottom)&&
										   (target_Curve->measurement.detector_Phi_Angle_Vec  [phi_Index]<=target_Curve->curve.subinterval_Top);
						}

						// if subinterval then use only data from inside
						if( (!target_Curve->curve.use_Subinterval) || in_Rectangle )
						{
							{
#ifdef EXPRTK
//								target_Curve->fit_Params.expression_Argument = target_Curve->curve.value_2D_Shifted[phi_Index][point_Index];
								target_Curve->fit_Params.expression_Argument = target_Curve->curve.value_2D_No_Scaling_And_Offset[phi_Index][point_Index] *
																			   target_Curve->curve.val_Factor.value +
																			   target_Curve->curve.val_Shift;

								fi_1 = target_Curve->fit_Params.expression_Vec[0].value();
#else
//								fi_1 = func(target_Curve->curve.value_2D_Shifted[phi_Index][point_Index], index);

								fi_1 = func(target_Curve->curve.value_2D_No_Scaling_And_Offset[phi_Index][point_Index] *
											target_Curve->curve.val_Factor.value +
											target_Curve->curve.val_Shift);
#endif
							}
							{
#ifdef EXPRTK
								target_Curve->fit_Params.expression_Argument = target_Element.unwrapped_Reflection->calculated_Values.GISAS_Instrumental[phi_Index][point_Index];
								fi_2 = target_Curve->fit_Params.expression_Vec[0].value();
#else
								fi_2 = func(target_Element.unwrapped_Reflection->calculated_Values.GISAS_Instrumental[phi_Index][point_Index]);
#endif
							}
							if(!isnan(fi_1)) f_Temp_Phi += pow(fi_1 - fi_2, 2);
						}
					}
					f_Val = factor * pow(f_Temp_Phi, power/2);
				}

				// fill
				gsl_vector_set(f, residual_Shift+point_Index, f_Val);
			}
		}
	} else
	// integral maximization
	{
		double integral = 0;
		params->max_Integral = 0;
		double delta_Lambda = 0;

		// calculate with expression
		for(size_t point_Index=0; point_Index<N-1; ++point_Index)
		{
			fi_1      = target_Curve->curve.shifted_Values[point_Index  ];
			fi_1_next = target_Curve->curve.shifted_Values[point_Index+1];
#ifdef EXPRTK
			target_Curve->fit_Params.expression_Argument = model_Curve[point_Index  ];
			fi_2      = target_Curve->fit_Params.expression_Vec[0].value();
			target_Curve->fit_Params.expression_Argument = model_Curve[point_Index+1];
			fi_2_next = target_Curve->fit_Params.expression_Vec[0].value();
#else
			fi_2      = model_Curve[point_Index  ];
			fi_2_next = model_Curve[point_Index+1];
#endif
			delta_Lambda = abs(target_Curve->curve.shifted_Argument[point_Index]-target_Curve->curve.shifted_Argument[point_Index+1]);

			// use only data from subinterval
			if( target_Curve->curve.use_Subinterval &&
				((target_Curve->curve.shifted_Argument[point_Index]<target_Curve->curve.subinterval_Left) ||
				(target_Curve->curve.shifted_Argument[point_Index]>target_Curve->curve.subinterval_Right))	)
			{
				integral += 0;
				params->max_Integral+=0;
			} else
			{
				integral += (fi_1+fi_1_next)/2*(fi_2+fi_2_next)/2*delta_Lambda;
				params->max_Integral+=1000*(fi_1+fi_1_next)*delta_Lambda; // 1000 is a big number, but if f(R) is bigger, then incorrect
			}
		}

		// fill
		double filling = pow(abs(params->max_Integral-integral)/N,0.5);
		for(size_t point_Index=0; point_Index<N; ++point_Index)
		{
			gsl_vector_set(f, residual_Shift+point_Index, filling);
		}
	}
	residual_Shift += int(N);
}

void Fitting::initialize_Position()
{
	// set value of real fitables
	for(size_t i=0; i<fitables.param_Pointers.size(); ++i)
	{
		fitables.values_Parametrized[i] = main_Calculation_Module->parametrize(fitables.param_Pointers[i]->value,
																			   fitables.param_Pointers[i]->fit.min,
																			   fitables.param_Pointers[i]->fit.max);
		gsl_vector_set(x, i, fitables.values_Parametrized[i]);
	}
	// initialize residual
	calc_Residual(params.x, &params, params.f);
	gsl_blas_ddot(params.f, params.f, &params.init_Residual);
}

void Fitting::randomize_Position(bool randomize)
{
	if(!randomize) return;

	const gsl_rng_type* T = gsl_rng_default;
	gsl_rng* r = gsl_rng_alloc (T);
	gsl_rng_env_setup();
	auto now = chrono::system_clock::now();
	auto now_ns = chrono::time_point_cast<chrono::nanoseconds>(now);
	auto value = now_ns.time_since_epoch();
	long duration = value.count();
	gsl_rng_set(r, duration);

	// set random value of real fitables
	for(size_t i=0; i<fitables.param_Pointers.size(); ++i)
	{
		fitables.values_Parametrized[i] = gsl_rng_uniform(r);
		gsl_vector_set(x, i, fitables.values_Parametrized[i]);
	}

	gsl_rng_free(r);
}

void Fitting::check_Maximization()
{
	params.maximize = false;
	// checking for maximization
	for(int tab_Index=0; tab_Index<global_Multilayer_Approach->multilayer_Tabs->count(); tab_Index++)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));
		for(Target_Curve* target_Curve: multilayer->target_Profiles_Vector)
		{
			if(target_Curve->fit_Params.maximize_Integral) {params.maximize = true;}
		}
	}
}

bool Fitting::run_Fitting()
{
	// GSL
	if(	GSL_Methods.contains(global_Multilayer_Approach->fitting_Settings->current_Method) )
	{
		qInfo() << "GSL " << global_Multilayer_Approach->fitting_Settings->current_Method << "optimization" << endl;
		Fitting_GSL fitting_GSL(this);
		return fitting_GSL.fit();
	}

	// SO
	if(	SO_Methods.contains(global_Multilayer_Approach->fitting_Settings->current_Method) )
	{
		qInfo() << "SO " << global_Multilayer_Approach->fitting_Settings->current_Method << "optimization" << endl;
		Fitting_SwarmOps fitting_SwarmOps(this);
		return fitting_SwarmOps.fit();
	}

	QMessageBox::warning(nullptr,"Wrong fitting method", "Fitting was canceled.\nCheck fitting method in Fitting settings window.");

	return false;
}

bool Fitting::fit()
{
	if(check_Residual_Expression()) return false;

	// --------------------------------------------------------------------------------
	if(n<=(p+number_Of_Restricted_Regular_Components))
	{
		QMessageBox::information(nullptr,"Insufficient number of points", "# of fitables: " + Locale.toString(uint(p)) + "\n# of points: " + Locale.toString(uint(n)) + "\n\nShould be f<p");
		return false;
	}
	// --------------------------------------------------------------------------------
	initialize_Position();
    main_Calculation_Module->add_Fit(before_Fitting_State);

    bool fit_Return = false;
	// single fit
	if(!global_Multilayer_Approach->fitting_Settings->randomized_Start)
	{
//		add_Fit_To_File(params.x, params.init_Residual, default_Fit_Statictics_File, 0);
		fit_Return = run_Fitting();
//		add_Fit_To_File(params.x, params.final_Residual, default_Fit_Statictics_File, 1);
	} else
	// randomized fit
	{
		double residual_To_Write = -2019;
		if(params.maximize)		{
			residual_To_Write = params.max_Integral-params.init_Residual;
		} else	{
			residual_To_Write = params.init_Residual;
		}

		add_Fit_To_File(params.x, residual_To_Write, default_Fit_Statictics_File, 0);
		for(int run=1; run<=global_Multilayer_Approach->fitting_Settings->num_Runs; run++)
		{
			if(!global_Multilayer_Approach->fitting_Settings->abort)
			{
				randomize_Position(run-1); // first without randomization
				fit_Return = run_Fitting();
				main_Calculation_Module->renew_Item_Trees();
				main_Calculation_Module->add_Fit(fit_Run_State, run);
				if(params.maximize)		{
					residual_To_Write = params.max_Integral-params.final_Residual;
				} else	{
					residual_To_Write = params.final_Residual;
				}
				add_Fit_To_File(params.x, residual_To_Write, default_Fit_Statictics_File, run);
			}
		}
	}
	// --------------------------------------------------------------------------------

	return fit_Return;
}

bool Fitting::confidence(const vector<double>& fitables_Pointers_Value_Backup, const vector<double>& confidentials_Pointers_Value_Backup, size_t confidence_Index)
{
	if(check_Residual_Expression()) return false;

	// --------------------------------------------------------------------------------
	if(n<=(p+number_Of_Restricted_Regular_Components))
	{
		QMessageBox::information(nullptr,"Insufficient number of points", "# of fitables: " + Locale.toString(uint(p)) + "\n# of points: " + Locale.toString(uint(n)) + "\n\nShould be f<p");
		return false;
	}
	// --------------------------------------------------------------------------------

	bool fit_Return = false;
	double min     = confidentials.param_Pointers[confidence_Index]->confidence.min;
	double max     = confidentials.param_Pointers[confidence_Index]->confidence.max;
	int num_Points = confidentials.param_Pointers[confidence_Index]->confidence.num_Points;
	double step = (max-min)/(num_Points-1);
	double& real_Conf_Value = confidentials.values[confidence_Index]; // not pointers! pointers will be renewed in calc_Residual
	int point_Index = 0;

	// changing value step by step
	for(real_Conf_Value = min; real_Conf_Value <= (max+step/2); real_Conf_Value += step)
	{
		initialize_Position();

		for(size_t i=0; i<confidentials.param_Pointers.size(); i++)		{
			qInfo() << confidentials.param_Pointers[i]->value << endl;
		}qInfo() << "" << endl;

//		qInfo() << "fitables.param_Pointers.size()"<<fitables.param_Pointers.size() << endl;
//		qInfo() << "fitables.param_Names"<<fitables.param_Names << endl;
		// single fit
		if(fitables.param_Pointers.size()>0)
		{
			if(!global_Multilayer_Approach->fitting_Settings->randomized_Start)
			{
				fit_Return = run_Fitting();

				// reset to initial state
				for(size_t i=0; i<fitables.param_Pointers.size(); i++)			{
					fitables.param_Pointers[i]->value = fitables_Pointers_Value_Backup[i];
				}

				// write point to file
				add_Confidence_Distribution_To_File(real_Conf_Value, default_Confidence_Distribution_File, confidence_Index, point_Index, params.final_Residual, nullptr);
			} else
			// randomized fit
			{
				vector<double> rand_Fit_Residuals;
					rand_Fit_Residuals.reserve(global_Multilayer_Approach->fitting_Settings->num_Runs);
				for(size_t rand_Index=0; rand_Index<rand_Fit_Residuals.size(); rand_Index++)
				{
					if(!global_Multilayer_Approach->fitting_Settings->abort)
					{
						randomize_Position(rand_Index); // first without randomization
						fit_Return = run_Fitting();
						rand_Fit_Residuals.push_back(params.final_Residual);

						// reset to initial state
						for(size_t i=0; i<fitables.param_Pointers.size(); i++)			{
							fitables.param_Pointers[i]->value = fitables_Pointers_Value_Backup[i];
						}
					}
				}
				// sort residuals
				sort(rand_Fit_Residuals.begin(), rand_Fit_Residuals.end());

				// take best
				params.final_Residual = rand_Fit_Residuals.front();

				// write point to file
				add_Confidence_Distribution_To_File(real_Conf_Value, default_Confidence_Distribution_File, confidence_Index, point_Index, params.final_Residual, &rand_Fit_Residuals);
			}
		} else
		{
			fit_Return = true;
			params.final_Residual = params.init_Residual;
			add_Confidence_Distribution_To_File(real_Conf_Value, default_Confidence_Distribution_File, confidence_Index, point_Index, params.final_Residual, nullptr);
		}
		qInfo() << "parameter value = " << real_Conf_Value << "~ final residual =" << params.final_Residual << endl;
		++point_Index;
	}
	// restore current confidential to init value
	real_Conf_Value = confidentials_Pointers_Value_Backup[confidence_Index];
	confidentials.param_Pointers[confidence_Index]->value = confidentials_Pointers_Value_Backup[confidence_Index];

	// --------------------------------------------------------------------------------
	return fit_Return;
}

void Fitting::add_Fit_To_File(const gsl_vector* x, double residual, QString filename, int run)
{
	// point as decimal separator
	Locale = QLocale::c();

    QFile file(Global_Variables::working_Directory() + "/" + filename);
	if(file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream out(&file);
		out.setFieldAlignment(QTextStream::AlignLeft);
		QString current_String;
		int index=0;
		int spacer = 5;

		if(run==0)
		{
			if(out.pos()!=0)
			{
				out << qSetFieldWidth(0) << endl;
			}
            out << Global_Variables::date_Time() << qSetFieldWidth(0) <<endl;

			widths.clear();
			//------------------------------------------------------
			current_String = "run";
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;

			current_String = "residual";
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;
			//------------------------------------------------------
			out.setFieldWidth(35);
			for(size_t param_Index=0; param_Index<params.p; ++param_Index)
			{
				//------------------------------------------------------
				current_String = params.fitables.param_Names[param_Index];
				widths.append(current_String.length()+spacer);
				out << qSetFieldWidth(widths[index]) << current_String;
				index++;
				//------------------------------------------------------
			}
			//------------------------------------------------------
			out << qSetFieldWidth(0) <<endl;
			//------------------------------------------------------
		}
		//------------------------------------------------------
		index=0;

		current_String = Locale.toString(run);
		out << qSetFieldWidth(widths[index]) << current_String;
		index++;

		current_String = Locale.toString(residual,'e',3);
		out << qSetFieldWidth(widths[index]) << current_String;
		index++;

		//------------------------------------------------------
		for(size_t param_Index=0; param_Index<params.p; ++param_Index)
		{
			double unparametrized = params.main_Calculation_Module->unparametrize(	gsl_vector_get(x, param_Index),
																					params.fitables.param_Pointers[param_Index]->fit.min,
																					params.fitables.param_Pointers[param_Index]->fit.max);
			if(params.fitables.param_Pointers[param_Index]->indicator.whats_This == whats_This_Num_Repetitions)
			{
				current_String = Locale.toString(round(unparametrized));
			} else
			{
				current_String = Locale.toString(unparametrized,'f',4);
			}
			//------------------------------------------------------
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;
			//------------------------------------------------------
		}
		//------------------------------------------------------
		out << qSetFieldWidth(0) <<endl;
		//------------------------------------------------------

	file.close();
	} else
	{
		QMessageBox::critical(nullptr, "Fitting::add_Fit_To_File", "Can't write file " + filename);
		exit(EXIT_FAILURE);
	}

	// back to system locale
	Locale = QLocale::system();
}

void Fitting::add_Confidence_Distribution_To_File(double real_Conf_Value, QString filename, size_t confidence_Index, int point_Index, double residual, vector<double>* residuals_Set)
{
	// point as decimal separator
	Locale = QLocale::c();

    QFile file(Global_Variables::working_Directory() + "/" + filename);
	if(file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream out(&file);
		out.setFieldAlignment(QTextStream::AlignLeft);
		QString current_String;
		int index=0;
		int spacer = 5;

		if(point_Index==0)
		{
			if(out.pos()!=0)
			{
				out << qSetFieldWidth(0) << endl;
			}
            out << Global_Variables::date_Time() << qSetFieldWidth(0) <<endl;

			widths.clear();
			//------------------------------------------------------

			out << "----------------------------------------------------" << qSetFieldWidth(0) <<endl;
			current_String = confidentials.param_Names[confidence_Index];
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String << qSetFieldWidth(0) <<endl;
			index++;
			out << "----------------------------------------------------" << qSetFieldWidth(0) <<endl;
			//------------------------------------------------------

			current_String = "value";
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;

			current_String = "residual";
			widths.append(current_String.length()+spacer);
			out << qSetFieldWidth(widths[index]) << current_String;
			index++;

			out << qSetFieldWidth(0) <<endl <<endl;
			//------------------------------------------------------

		}
		//------------------------------------------------------
		index=1;

		current_String = Locale.toString(real_Conf_Value);
		out << qSetFieldWidth(widths[index]) << current_String;
		index++;

		current_String = Locale.toString(residual,'e',5);
		out << qSetFieldWidth(widths[index]) << current_String;
		index++;
		//------------------------------------------------------

		// whole set of values
		if(residuals_Set != nullptr)
		{
			out << qSetFieldWidth(2) <<  "(";
			for(size_t i=0; i<residuals_Set->size(); ++i)
			{
				current_String = Locale.toString((*residuals_Set)[i],'e',3);
				out << qSetFieldWidth(0) << current_String;
				if(i!=residuals_Set->size()-1)
					out << qSetFieldWidth(3) << "," <<qSetFieldWidth(2) ;
			}
			out << qSetFieldWidth(0) <<  ")";
		}

		//------------------------------------------------------
		out << qSetFieldWidth(0) <<endl;
		//------------------------------------------------------

	file.close();
	} else
	{
		QMessageBox::critical(nullptr, "Fitting::add_Confidence_Distribution_To_File", "Can't write file " + filename);
		exit(EXIT_FAILURE);
	}

	// back to system locale
	Locale = QLocale::system();
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
				QMessageBox::information(nullptr,"Bad weight", "Weight coefficient in\n\n" +
															struct_Name + ", measured curve #" + Locale.toString(target_Element_Index+1) +
															"\n\nshould be positive");
				return true;
			}

			/// -------------------------------------------------------------------------------
			/// 1 value
			/// -------------------------------------------------------------------------------

			if(target_Curve->fit_Params.expression_Vec.size() < 1)
			{
				QMessageBox::information(nullptr,"Bad expression", "Residual function\n\n\"" +
																target_Curve->fit_Params.fit_Function +
																"\"\n\nin\n\n" +
																struct_Name + ", measured curve #" + Locale.toString(target_Element_Index+1) +
																"\n\nshould contains >=1 expression, separated by \"" + fit_Function_Separator + "\"");
				return true;
			}
		}
	}
	return false;
}
