// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "main_calculation_module.h"

Main_Calculation_Module::Main_Calculation_Module(QString calc_Mode):
	multilayer_Tabs			(global_Multilayer_Approach->multilayer_Tabs),
	multilayers				(multilayer_Tabs->count()),
	calculation_Trees		(multilayer_Tabs->count()),
	copy_Real_Struct_Trees	(multilayer_Tabs->count()),
	calc_Mode				(calc_Mode)
{
	lambda_Out_Of_Range = false;
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		multilayers[tab_Index] = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
		calculation_Trees[tab_Index] = new Calculation_Tree(multilayers[tab_Index], calc_Mode);
	}
}

Main_Calculation_Module::~Main_Calculation_Module()
{
	for(Calculation_Tree* calculation_Tree : calculation_Trees)
	{
		delete calculation_Tree;
	}
//	for(QTreeWidget* copy_Real_Struct_Tree : copy_Real_Struct_Trees)
//	{
//		copy_Real_Struct_Tree->clear();
//	}
}

void Main_Calculation_Module::single_Calculation(bool print)
{
	if(calc_Mode!=CALCULATION)
	{
		QMessageBox::critical(nullptr, "Main_Calculation_Module::single_Calculation", "wrong calc_Mode");
		return;
	}

	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		calculation_Trees[tab_Index]->fill_Independent_Calc_Trees();
		for(Data_Element<Independent_Variables>& independent_Element : calculation_Trees[tab_Index]->independent)
		{
//			qInfo() << independent_Element.calc_Tree.begin().number_of_children();
//			calculation_Trees[tab_Index]->print_Tree(independent_Element.calc_Tree.begin(), independent_Element.calc_Tree);

			calculation_Trees[tab_Index]->calculate_1_Kind(independent_Element);
			if(lambda_Out_Of_Range) return;
		}
		calculation_Trees[tab_Index]->fill_Target_Calc_Trees();
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
//			qInfo() << target_Element.calc_Tree.begin().number_of_children();
//			calculation_Trees[tab_Index]->print_Tree(target_Element.calc_Tree.begin(), target_Element.calc_Tree);

			calculation_Trees[tab_Index]->calculate_1_Kind(target_Element);
			if(lambda_Out_Of_Range) return;
		}
	}
	if(print) print_Calculated_To_File();

	// replot graphs
	if(global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key))
	{
		for(Curve_Plot* curve_Plot : global_Multilayer_Approach->optical_Graphs->plots)
		{
			curve_Plot->plot_All_Data();
		}
	}
}

void Main_Calculation_Module::fitting_and_Confidence()
{
	if(calc_Mode!=FITTING && calc_Mode!=CONFIDENCE)
	{
		QMessageBox::critical(nullptr, "Main_Calculation_Module::fitting", "wrong calc_Mode");
		return;
	}

	// reload dependences
	if(!global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Key))
	{
		global_Multilayer_Approach->table_Of_Structures = new Table_Of_Structures;//(global_Multilayer_Approach);
		global_Multilayer_Approach->table_Of_Structures->close();
	}

	save_Init_State_Trees();

	confidentials.clear_All();
	confidentials_Rejected_Thicknesses_and_Periods.clear_All();
	confidentials_Rejected_Periods.clear_All();

	fitables.clear_All();
	fit_Rejected_Sigmas.clear_All();
	fit_Rejected_Min_Max.clear_All();
	fit_Rejected_Thicknesses_and_Periods.clear_All();
	fit_Rejected_Periods.clear_All();

	// cheking for lambda_Out_Of_Range
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		calculation_Trees[tab_Index]->fill_Target_Calc_Trees();
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
			calculation_Trees[tab_Index]->calculate_1_Kind(target_Element);
			if(lambda_Out_Of_Range)	return;
		}
	}

	/// create calc tree, fitables and confidentials;
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		// prepare real_Calc_Tree  (for all multilayers!)
		calculation_Trees[tab_Index]->fill_Tree_From_Scratch(calculation_Trees[tab_Index]->real_Calc_Tree, calculation_Trees[tab_Index]->real_Struct_Tree, TARGET);

		if( calculation_Trees[tab_Index]->target.size()>0 )
		{
			// find fitables over tree
			calc_Tree_Iteration(calculation_Trees[tab_Index]->real_Calc_Tree.begin());
		}
	}

	/// fill pointers to slaves
	// starting from fitable top-masters
	for(Parameter* parameter : fitables.param_Pointers)	{
		slaves_Pointer_Iteration(parameter);
	}	
	// starting from confidentials
	for(Parameter* parameter : confidentials.param_Pointers)	{
		slaves_Pointer_Iteration(parameter);
	}

	// not used
	/// prepare expressions for slaves, starting from fitable top-masters
#ifdef EXPRTK
	expression_Vec.clear();
	argument_Vec.clear();
	slaves_Expression_Map.clear();
	for(Parameter* parameter : fitables.param_Pointers)
	{
		slaves_Expression_Iteration(parameter);
	}
#endif

	/// rejection
	if(reject()) return;

	/// --------------------------------------------------------------------------------------------------------------
	/// fitting from here
	/// --------------------------------------------------------------------------------------------------------------
	if( calc_Mode == FITTING )
	if( fitables.param_Pointers.size()>0 )
	{
		Fitting fitting_Instance(this);
		bool go = fitting_Instance.fit();
		if(!go) return;
		print_Calculated_To_File();

		// replace the initial parameters
		QMessageBox::StandardButton reply = QMessageBox::question(nullptr,"Replace", "Fitting is done.\nDo you want to replace the parameters?",
																  QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			// for single fitting
			if(!global_Multilayer_Approach->fitting_Settings->randomized_Start)
			{
				renew_Item_Trees();
				add_Fit(fitted_State);
			}

			// it also refreshs Independent tree copies
			global_Multilayer_Approach->refresh_All_Multilayers_View();
		} else
		{
			load_Init_State_Trees();
		}

		// refresh table (anyway)
		if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Key))
		{
			int active_Tab = global_Multilayer_Approach->table_Of_Structures->main_Tabs->currentIndex();
			global_Multilayer_Approach->table_Of_Structures->close();
			global_Multilayer_Approach->open_Table_Of_Structures();
			global_Multilayer_Approach->table_Of_Structures->main_Tabs->setCurrentIndex(active_Tab);
		}
	}
	/// --------------------------------------------------------------------------------------------------------------
	/// confidence intervals from here
	/// --------------------------------------------------------------------------------------------------------------
	if( calc_Mode == CONFIDENCE )
	if( confidentials.param_Pointers.size()>0 )
	{
		Fitables temp_Full_Fitables = fitables; // save full set, including (possibly) active confidentials
		for(uint confidence_Index=0; confidence_Index<confidentials.param_Pointers.size(); ++confidence_Index)
		{
			confidentials.param_Pointers[confidence_Index]->confidence.is_Active = true;
			fitables = temp_Full_Fitables;

			// get index of fitable with the same ID
			int corresponding_Index = fitables.param_IDs.indexOf(confidentials.param_IDs[confidence_Index]);

			// shortening fitables
			if(corresponding_Index>=0)
			{
				fitables.struct_Names.		 erase(fitables.struct_Names.begin() + corresponding_Index);
				fitables.param_Names.		 erase(fitables.param_Names.begin() + corresponding_Index);
				fitables.param_IDs.			 remove(corresponding_Index);

				fitables.param_Pointers.	 erase(fitables.param_Pointers.begin() + corresponding_Index);
				fitables.values_Parametrized.erase(fitables.values_Parametrized.begin() + corresponding_Index);
				fitables.parent_Iterators.	 erase(fitables.parent_Iterators.begin() + corresponding_Index);
			}

			// backup for restoring values
			vector<double> fitables_Pointers_Value_Backup(fitables.param_Pointers.size());
			for(size_t i=0; i<fitables.param_Pointers.size(); i++)			{
				fitables_Pointers_Value_Backup[i] = fitables.param_Pointers[i]->value;
			}
			vector<double> confidentials_Pointers_Value_Backup(confidentials.param_Pointers.size());
			for(size_t i=0; i<confidentials.param_Pointers.size(); i++)			{
				confidentials_Pointers_Value_Backup[i] = confidentials.param_Pointers[i]->value;
			}

			Fitting fitting_Instance(this);
			bool go = fitting_Instance.confidence(fitables_Pointers_Value_Backup, confidentials_Pointers_Value_Backup, confidence_Index);
			confidentials.param_Pointers[confidence_Index]->confidence.is_Active = false;
			if(!go) return;
		}
	}
	/// --------------------------------------------------------------------------------------------------------------
}

void Main_Calculation_Module::save_Init_State_Trees()
{
	for(int tree_Index=0; tree_Index<calculation_Trees.size(); ++tree_Index)
	{
		copy_Real_Struct_Trees[tree_Index] = new QTreeWidget(this);
		Global_Variables::copy_Tree(calculation_Trees[tree_Index]->real_Struct_Tree, copy_Real_Struct_Trees[tree_Index]);
	}
}

void Main_Calculation_Module::load_Init_State_Trees()
{
	for(int tree_Index=0; tree_Index<calculation_Trees.size(); ++tree_Index)
	{
		Global_Variables::copy_Tree(copy_Real_Struct_Trees[tree_Index], calculation_Trees[tree_Index]->real_Struct_Tree);
		calculation_Trees[tree_Index]->real_Struct_Tree->expandAll();
	}
}

void Main_Calculation_Module::renew_Item_Trees()
{
	for(Calculation_Tree* calculation_Tree : calculation_Trees)
	{
		calculation_Tree->renew_Item_Tree_From_Calc_Tree(calculation_Tree->real_Calc_Tree.begin(),
														 calculation_Tree->real_Calc_Tree,
														 calculation_Tree->real_Struct_Tree->invisibleRootItem());
	}
}

bool Main_Calculation_Module::reject()
{
	/// fit reject

	// sigma value is close to zero
	if(fit_Rejected_Sigmas.param_IDs.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(fit_Rejected_Sigmas.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables", "Sigma start values are too small in:\n\n" +
													   rejected +
													   "\n\nChange them");
		return true;
	}

	// internal thicknesses and periods if overlying period or gamma are fitables
	if(fit_Rejected_Thicknesses_and_Periods.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(fit_Rejected_Thicknesses_and_Periods.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables", "Thicknesses and periods can't be fitted independently\nif overlying period or gamma are fitables:\n\n" +
													   rejected +
													   "\n\nRecheck fitables");
		return true;
	}

	// period value == 0
	if(fit_Rejected_Periods.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(fit_Rejected_Periods.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables", "Zero period values are not allowed for fitting:\n\n" +
													   rejected +
													   "\n\nIncrease them");
		return true;
	}

	// min>=max
	if(fit_Rejected_Min_Max.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(fit_Rejected_Min_Max.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables", "min>=max for the following fitables:\n\n" +
													   rejected +
													   "\n\nThey won't be fitted");
		return true;
	}

	/// confidence reject

	// fitiing or calculating interval for internal thicknesses and periods if overlying period or gamma are fitables or confidentials
	if(confidentials_Rejected_Thicknesses_and_Periods.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(confidentials_Rejected_Thicknesses_and_Periods.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables/confidentials", "Fit or confidence interval calculation is not possible\nfor internal thicknesses and periods if overlying\nperiod or gamma are fitables of confidentials:\n\n" +
													   rejected +
													   "\n\nRecheck fitables/confidentials");
		return true;
	}

	// one of period values in interval == 0
	if(confidentials_Rejected_Periods.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(confidentials_Rejected_Periods.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad confidentials", "Zero period values are not allowed\nfor calculation confidence interval:\n\n" +
													   rejected +
													   "\n\nChange min/max");
		return true;
	}

	return false;
}

void Main_Calculation_Module::calc_Tree_Iteration(const tree<Node>::iterator& parent, bool fitables_Period_Gamma, bool confidentials_Period_Gamma)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& struct_Data = child.node->data.struct_Data;

		find_Fittable_Confidence_Parameters(struct_Data, child, fitables_Period_Gamma, confidentials_Period_Gamma);

		// check period and gamma
		if(struct_Data.item_Type == item_Type_Multilayer ||
		   struct_Data.item_Type == item_Type_Aperiodic	 )
		{
			// check if period or gamma are fitables
			bool local_Fitables_Period_Gamma = fitables_Period_Gamma;
			if(struct_Data.period.fit.is_Fitable || (struct_Data.gamma.fit.is_Fitable && child.number_of_children() == 2))
			{
				local_Fitables_Period_Gamma = true;
			}

			// check if period or gamma are confidentials
			bool local_Confidentials_Period_Gamma = confidentials_Period_Gamma;
			if(struct_Data.period.confidence.calc_Conf_Interval || (struct_Data.gamma.confidence.calc_Conf_Interval && child.number_of_children() == 2))
			{
				local_Confidentials_Period_Gamma = true;
			}
			calc_Tree_Iteration(child, local_Fitables_Period_Gamma, local_Confidentials_Period_Gamma);
		}
	}
}

void Main_Calculation_Module::find_Fittable_Confidence_Parameters(Data& struct_Data, const tree<Node>::iterator& parent, bool fitables_Period_Gamma, bool confidentials_Period_Gamma)
{
	struct_Data.fill_Potentially_Fitable_Parameters_Vector();
	for(Parameter* parameter : struct_Data.potentially_Fitable_Parameters)
	{
		// for my_Sigmas
		if(parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma &&	!struct_Data.common_Sigma)
			parameter->fit.is_Fitable = struct_Data.sigma.fit.is_Fitable;

		QString total_Name = "  " + Medium_BlackCircle_Sym + "  <" + multilayer_Tabs->tabText(parameter->indicator.tab_Index) + "> " + parameter->indicator.full_Name;

		// fitable and has no master
		if(parameter->fit.is_Fitable && !parameter->coupled.master.exist)
//		if(calc_Mode != CONFIDENCE || !parameter->confidence.calc_Conf_Interval) // only 1 confidential should be treated as non-fitable in CONFIDENCE mode at the same time
		{
			// fixed
			fitables.struct_Names 		.push_back(multilayer_Tabs->tabText(parameter->indicator.tab_Index));
			fitables.param_Names		.push_back(parameter->indicator.full_Name);
			fitables.param_IDs			.push_back(parameter->indicator.id);

			// changeable
			fitables.param_Pointers		.push_back(parameter);
			fitables.values_Parametrized.push_back(parametrize(parameter->value, parameter->fit.min, parameter->fit.max)); // will be recalculated at solver initialization
			fitables.parent_Iterators	.push_back(parent);					// used for period and gamma only, but should be filled for all for the length purpose!

			/// for rejection

			// sigma value is close to zero
			if(parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma || parameter->indicator.whats_This == whats_This_Sigma)
			if(struct_Data.sigma.value < 0.1)
			if(!fit_Rejected_Sigmas.param_IDs.contains(struct_Data.sigma.indicator.id))
			{
				fit_Rejected_Sigmas.param_IDs.push_back(struct_Data.sigma.indicator.id);
				fit_Rejected_Sigmas.param_Names.push_back("  " + Medium_BlackCircle_Sym + "  <" + multilayer_Tabs->tabText(struct_Data.sigma.indicator.tab_Index) + "> " + struct_Data.sigma.indicator.full_Name);
			}

			// forbid fitting internal thicknesses and periods if overlying period or gamma are fitables
			if(fitables_Period_Gamma)
			if(parameter->indicator.whats_This == whats_This_Thickness || parameter->indicator.whats_This == whats_This_Period)
			{
				fit_Rejected_Thicknesses_and_Periods.param_Names.push_back(total_Name);
			}

			// forbid calculating interval for outer period or gamma if inner thickness or period are fitables
			if(confidentials_Period_Gamma)
			if(parameter->indicator.whats_This == whats_This_Thickness || parameter->indicator.whats_This == whats_This_Period)
			{ // qLineEdit.setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT)
				confidentials_Rejected_Thicknesses_and_Periods.param_Names.push_back(total_Name);
			}

			// period value == 0
			if(parameter->indicator.whats_This == whats_This_Period)
			if(abs(parameter->value) < DBL_MIN)
			{
				fit_Rejected_Periods.param_Names.push_back(total_Name);
			}

			// min>=max
			if(parameter->fit.min>=parameter->fit.max)
			{
				fit_Rejected_Min_Max.param_Names.push_back(total_Name);
			}
		}

		// confidential
		if(calc_Mode == CONFIDENCE && parameter->confidence.calc_Conf_Interval)
		{
			// fixed
			confidentials.struct_Names	  .push_back(multilayer_Tabs->tabText(parameter->indicator.tab_Index));
			confidentials.param_Names	  .push_back(parameter->indicator.full_Name);
			confidentials.param_IDs		  .push_back(parameter->indicator.id);

			// changeable
			confidentials.param_Pointers  .push_back(parameter);
			confidentials.values		  .push_back(parameter->value);
			confidentials.parent_Iterators.push_back(parent);		// used for period and gamma only, but should be filled for all for the length purpose!

			/// for rejection

			// forbid calculating interval for internal thicknesses and periods if overlying period or gamma are fitables or confidentials
			if(confidentials_Period_Gamma || fitables_Period_Gamma)
			if(parameter->indicator.whats_This == whats_This_Thickness || parameter->indicator.whats_This == whats_This_Period)
			{
				confidentials_Rejected_Thicknesses_and_Periods.param_Names.push_back(total_Name);
			}

			// one of period values in interval == 0
			if(parameter->indicator.whats_This == whats_This_Period)
			if(abs(parameter->confidence.min) < DBL_MIN || abs(parameter->confidence.max) < DBL_MIN)
			{
				confidentials_Rejected_Periods.param_Names.push_back(total_Name);
			}
		}
	}
}

void Main_Calculation_Module::slaves_Pointer_Iteration(Parameter* master)
{
	master->coupled.slave_Pointers.clear(); // rewrite

	for(Parameter_Indicator& slave_Parameter_Indicator : master->coupled.slaves)
	{
		Parameter* slave = find_Slave_Pointer_by_Id(slave_Parameter_Indicator);
		// check
		if(slave == nullptr)
		{
			// TODO
//			QMessageBox::critical(nullptr, "Main_Calculation_Module::slaves_Vector_Iteration", slave_Parameter_Indicator.full_Name + "\n\nnot found");
//			exit(EXIT_FAILURE);

			// good way is refresh_Parameters_Connection_Over_Trees() in Multilayer_Approach. Not sure.
			master->coupled.slaves.removeAll(slave_Parameter_Indicator);
		} else
		{
			master->coupled.slave_Pointers.append(slave);
			slaves_Pointer_Iteration(slave);
		}
	}
}

void Main_Calculation_Module::slaves_Expression_Iteration(Parameter* master)
{
	if(master->coupled.slaves.size() != master->coupled.slave_Pointers.size())
	{
		QMessageBox::critical(nullptr, "Main_Calculation_Module::slaves_Expression_Iteration", "slaves.size() != slave_Pointers.size()");
		exit(EXIT_FAILURE);
	}
	for(int slave_Index=0; slave_Index<master->coupled.slaves.size(); ++slave_Index)
	{
#ifdef EXPRTK
		Parameter_Indicator& slave_Parameter_Indicator = master->coupled.slaves[slave_Index];
#endif
		Parameter* slave = master->coupled.slave_Pointers[slave_Index];

#ifdef EXPRTK
		exprtk::parser<double> parser;
		exprtk::symbol_table<double> symbol_table;

		expression_Vec.append(exprtk::expression<double>());

		argument_Vec.append(&master->value);
		slaves_Expression_Map.insert(slave->indicator.id, expression_Vec.size()-1);

		symbol_table.add_variable(expression_Master_Slave_Variable, *argument_Vec.last());
		symbol_table.add_constants();

		expression_Vec.last().register_symbol_table(symbol_table);
		parser.compile(slave_Parameter_Indicator.expression.toStdString(), expression_Vec.last());
#endif
		slaves_Expression_Iteration(slave);
	}
}

Parameter* Main_Calculation_Module::find_Slave_Pointer_by_Id(const Parameter_Indicator& slave_Parameter_Indicator)
{
	Parameter* pointer = nullptr;
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		find_Slave_Pointer_Calc_Tree_Iteration(calculation_Trees[tab_Index]->real_Calc_Tree.begin(), slave_Parameter_Indicator, pointer);
		if(pointer != nullptr)
		{
			return pointer;
		}
	}
	return pointer;
}

void Main_Calculation_Module::find_Slave_Pointer_Calc_Tree_Iteration(const tree<Node>::iterator& parent, const Parameter_Indicator& slave_Parameter_Indicator, Parameter*& pointer)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& struct_Data = child.node->data.struct_Data;

		if(struct_Data.id == slave_Parameter_Indicator.item_Id)
		{
			pointer = Global_Variables::get_Parameter_From_Struct_Item_by_Id(struct_Data, slave_Parameter_Indicator.id);
		}

		if(struct_Data.item_Type == item_Type_Multilayer ||
		   struct_Data.item_Type == item_Type_Aperiodic	 )
		{
			find_Slave_Pointer_Calc_Tree_Iteration(child, slave_Parameter_Indicator, pointer);
		}
	}
}

double Main_Calculation_Module::triangle_Wave(double x)
{
	return abs(x - 2*floor((x + 1)/2));
}

double Main_Calculation_Module::parametrize(double value, double min, double max)
{
	if(parametrization_Type == noparam)
	{
		return value;
	} else
	if(parametrization_Type == triangle)
	{
		return (value - min) / (max - min);
	} else
	{
		qInfo() << "Main_Calculation_Module::parametrize : wrong parametrization_Type";
		QMessageBox::warning(nullptr, "Main_Calculation_Module::parametrize", "Wrong parametrization_Type");
		return value;
	}
}

double Main_Calculation_Module::unparametrize(double parametrized_Shifted_Value, double min, double max)
{
	if(parametrization_Type == noparam)
	{
		return parametrized_Shifted_Value;
	} else
	if(parametrization_Type == triangle)
	{
		return triangle_Wave(parametrized_Shifted_Value) * (max - min) + min;
	} else
	{
		qInfo() << "Main_Calculation_Module::unparametrize : wrong parametrization_Type";
		QMessageBox::warning(nullptr, "Main_Calculation_Module::unparametrize", "Wrong parametrization_Type");
		return parametrized_Shifted_Value;
	}
}

void Main_Calculation_Module::print_Calculated_To_File()
{
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		int counter = 0;
		if( multilayers[tab_Index]->enable_Calc_Independent_Curves )
		{
			for(Data_Element<Independent_Variables>& independent : calculation_Trees[tab_Index]->independent)
			{
				print_Reflect_To_File(independent, multilayer_Tabs->tabText(tab_Index), counter++);
			}
		}
		counter = 0;
		if( multilayers[tab_Index]->enable_Calc_Target_Curves )
		{
			for(Data_Element<Target_Curve>& target : calculation_Trees[tab_Index]->target)
			{
				print_Reflect_To_File(target, multilayer_Tabs->tabText(tab_Index), counter++);
			}
		}
	}
}

template <typename Type>
void Main_Calculation_Module::print_Reflect_To_File(Data_Element<Type>& data_Element, QString struct_Name, int index)
{
	QString first_Name;
	if(data_Element.curve_Class == INDEPENDENT)	first_Name = "calc_" + struct_Name + "_independent";
	if(data_Element.curve_Class == TARGET)		first_Name = "calc_" + struct_Name + "_target";

	Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(data_Element.the_Class);
	Target_Curve* target_Curve = qobject_cast<Target_Curve*>(data_Element.the_Class);

	//-----------------------------------------------------------------------------------------------------
	/// which values to print
	// reflectance
	bool print_Reflectance = false;
	if(data_Element.curve_Class == INDEPENDENT)
	if(independent_Variables->calc_Functions.check_Reflectance)
		print_Reflectance = true;
	if(data_Element.curve_Class == TARGET)
	if(target_Curve->curve.value_Function == value_Function[Reflectance])
		print_Reflectance = true;

	// transmittance
	bool print_Transmittance = false;
	if(data_Element.curve_Class == INDEPENDENT)
	if(independent_Variables->calc_Functions.check_Transmittance)
		print_Transmittance = true;
	if(data_Element.curve_Class == TARGET)
	if(target_Curve->curve.value_Function == value_Function[Transmittance])
		print_Transmittance = true;

	// absorptance
	bool print_Absorptance = false;
	if(data_Element.curve_Class == INDEPENDENT)
	if(independent_Variables->calc_Functions.check_Absorptance)
		print_Absorptance = true;

	// user function
	bool print_User = false;
	if(data_Element.curve_Class == INDEPENDENT)
	if(independent_Variables->calc_Functions.check_User)
		print_User = true;

	//-----------------------------------------------------------------------------------------------------

	QVector<double> arg;
	if(data_Element.active_Parameter_Whats_This == whats_This_Angle )
	{
		arg = data_Element.the_Class->measurement.angle;
	}
	if(data_Element.active_Parameter_Whats_This == whats_This_Wavelength )
	{
		arg = data_Element.the_Class->measurement.lambda;
	}

	QString name = first_Name+"_"+QString::number(index)+".txt";
	QFile file(name);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		out.setFieldAlignment(QTextStream::AlignLeft);

		print_Data(out,
				   arg,
				   data_Element.unwrapped_Reflection,
				   print_Reflectance,
				   print_Transmittance,
				   print_Absorptance,
				   print_User,
				   data_Element.the_Class->measurement.polarization.value
				   );
		file.close();
	} else
	{
		qInfo() << "Main_Calculation_Module::print_Reflect_To_File  :  Can't write file " + name;
		QMessageBox::critical(nullptr, "Main_Calculation_Module::print_Reflect_To_File", "Can't write file " + name);
		exit(EXIT_FAILURE);
	}
}
template void Main_Calculation_Module::print_Reflect_To_File<Independent_Variables>(Data_Element<Independent_Variables>&, QString, int);
template void Main_Calculation_Module::print_Reflect_To_File<Target_Curve>		   (Data_Element<Target_Curve>&,          QString, int);

void Main_Calculation_Module::print_Data(QTextStream &out, QVector<double> &arg, Unwrapped_Reflection* unwrapped_Reflection,
										bool print_Reflectance,
										bool print_Transmittance,
										bool print_Absorptance,
										bool print_User,
										double incident_Polarization
										)
{
	// headline
	QString argument = "argument";

	QString R_mixed  = "R_mixed";
	QString R_s      = "R_s";
	QString R_p      = "R_p";
	QString Phi_R_s  = "Phase_R_s";
	QString Phi_R_p  = "Phase_R_p";

	QString T_mixed  = "T_mixed";
	QString T_s      = "T_s";
	QString T_p      = "T_p";
	QString Phi_T_s  = "Phase_T_s";
	QString Phi_T_p  = "Phase_T_p";

	QString A_mixed  = "A_mixed";
	QString A_s      = "A_s";
	QString A_p      = "A_p";

	QString user     = "user";


	int precision_Arg = 4;
	int precision_R_T_A = 6;
	int precision_Phi = 4;

	int arg_Shift = 3;
	int width_Short= 8+precision_Arg;
	int width_Long = 11+precision_R_T_A;

	///------------------------------------------------------------------------
	/// headline
	{	// argument
		out << qSetFieldWidth(arg_Shift-1) << ";";
		out << qSetFieldWidth(width_Short) << argument  << qSetFieldWidth(width_Long);

		// reflectance
		if(print_Reflectance)
		{
											out << R_mixed;
			if(incident_Polarization>-1)	out << R_s;
			if(incident_Polarization< 1)	out << R_p;
			if(incident_Polarization>-1)	out << Phi_R_s;
			if(incident_Polarization< 1)	out << Phi_R_p;
		}

		// transmittance
		if(print_Transmittance)
		{
											out <<T_mixed;
			if(incident_Polarization>-1)	out << T_s;
			if(incident_Polarization< 1)	out << T_p;
			if(incident_Polarization>-1)	out << Phi_T_s;
			if(incident_Polarization< 1)	out << Phi_T_p;
		}

		// absorptance
		if(print_Absorptance)
		{
											out << A_mixed;
			if(incident_Polarization>-1)	out << A_s;
			if(incident_Polarization< 1)	out << A_p;
		}

		// user
		if(print_User)
		{
			out << user;
		}
		out << qSetFieldWidth(arg_Shift) << endl  << qSetFieldWidth(width_Short);
	}
	///------------------------------------------------------------------------
	/// data
	{
		for(auto i=0; i<arg.size(); ++i)
		{
			// argument
			out << qSetFieldWidth(width_Short) << QString::number(arg[i],'f',precision_Arg)  << qSetFieldWidth(width_Long);

			// reflectance
			if(print_Reflectance)
			{
												out << QString::number(unwrapped_Reflection->R_Instrumental[i],'e',precision_R_T_A);
				if(incident_Polarization>-1)	out << QString::number(unwrapped_Reflection->R_s           [i],'e',precision_R_T_A);
				if(incident_Polarization< 1)	out << QString::number(unwrapped_Reflection->R_p           [i],'e',precision_R_T_A);
				if(incident_Polarization>-1)	out << QString::number(unwrapped_Reflection->Phi_R_s       [i],'f',precision_Phi);
				if(incident_Polarization< 1)	out << QString::number(unwrapped_Reflection->Phi_R_p       [i],'f',precision_Phi);
			}

			// transmittance
			if(print_Transmittance)
			{
												out << QString::number(unwrapped_Reflection->T_Instrumental[i],'e',precision_R_T_A);
				if(incident_Polarization>-1)	out << QString::number(unwrapped_Reflection->T_s           [i],'e',precision_R_T_A);
				if(incident_Polarization< 1)	out << QString::number(unwrapped_Reflection->T_p           [i],'e',precision_R_T_A);
				if(incident_Polarization>-1)	out << QString::number(unwrapped_Reflection->Phi_T_s       [i],'f',precision_Phi);
				if(incident_Polarization< 1)	out << QString::number(unwrapped_Reflection->Phi_T_p       [i],'f',precision_Phi);
			}

			// absorptance
			if(print_Absorptance)
			{
												out << QString::number(unwrapped_Reflection->A_Instrumental[i],'e',precision_R_T_A);
				if(incident_Polarization>-1)	out << QString::number(unwrapped_Reflection->A_s           [i],'e',precision_R_T_A);
				if(incident_Polarization< 1)	out << QString::number(unwrapped_Reflection->A_p           [i],'e',precision_R_T_A);			}

			/////////////////////////////////////// TODO /////////////////////////////////
			// user
			if(print_User)
			{
				out << user;
			}

			if(i!=arg.size()-1)
				out << qSetFieldWidth(arg_Shift) << endl  << qSetFieldWidth(width_Short);
		}
	}
}

void Main_Calculation_Module::add_Fit(QString name_Modificator, int run)
{
	// save new trees
	QVector<QTreeWidget*> fitted_Trees(multilayers.size());
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		fitted_Trees[tab_Index] = calculation_Trees[tab_Index]->real_Struct_Tree;
	}
	global_Multilayer_Approach->add_Fitted_Structure(fitted_Trees, name_Modificator, run);
}
