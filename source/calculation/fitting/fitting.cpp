#include "fitting.h"

Fitting::Fitting(Multilayer_Approach* multilayer_Approach, Main_Calculation_Module* main_Calculation_Module):
	multilayer_Approach(multilayer_Approach),
	main_Calculation_Module(main_Calculation_Module),
	calculation_Trees(main_Calculation_Module->calculation_Trees),
	fitables(main_Calculation_Module->fitables)
{

}

double Fitting::func(double argument, int index)
{
	if(index == 0)
	{
		return log(argument+1E-4);
	} else
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
		slaves_Recalculation(slave, params);
	}
}

bool Fitting::fit()
{
	if(	multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Levenberg_Marquardt]							||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Levenberg_Marquardt_with_Geodesic_Acceleration]||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Dogleg]										||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Double_Dogleg]									||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Two_Dimensional_Subspace]						||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Fletcher_Reeves_Conjugate_Gradient]			||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Polak_Ribiere_Conjugate_Gradient]				||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Broyden_Fletcher_Goldfarb_Shanno_BFGS]			||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Broyden_Fletcher_Goldfarb_Shanno_BFGS_2]		||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Steepest_Descent]								||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Two_Dimensional_Subspace]						||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Nelder_Mead_Simplex]							||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Nelder_Mead_Simplex_2]							||
		multilayer_Approach->fitting_Settings->current_Method == GSL_Methods[Nelder_Mead_Simplex_2_Randomized] )
	{
		// GSL
	}
	if(	multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Mesh_Iteration]							||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Random_Sampling_Uniform]				||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Gradient_Descent]						||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Gradient_Emancipated_Descent]			||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Hill_Climber]							||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Simulated_Annealing]					||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Pattern_Search]							||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Local_Unimodal_Sampling]				||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Differential_Evolution]					||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Differential_Evolution_Suite]			||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[DE_with_Temporal_Parameters]			||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Jan_Differential_Evolution]				||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Evolution_by_Lingering_Global_Best]		||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[More_Yo_yos_Doing_Global_optimization]	||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Particle_Swarm_Optimization]			||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Forever_Accumulating_Evolution]			||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Many_Optimizing_Liaisons]				||
		multilayer_Approach->fitting_Settings->current_Method == SO_Methods[Layered_and_Interleaved_CoEvolution] )
	{
		// SO
	}

	return true;
}
