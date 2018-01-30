// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "main_calculation_module.h"

Main_Calculation_Module::Main_Calculation_Module(QTabWidget* multilayer_Tabs, QString calc_Mode):
	multilayers		 (multilayer_Tabs->count()),
	calculation_Trees(multilayer_Tabs->count()),
	multilayer_Tabs	 (multilayer_Tabs),
	calc_Mode		 (calc_Mode)
{
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		multilayers[tab_Index] = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
		calculation_Trees[tab_Index] = new Calculation_Tree(multilayers[tab_Index], calc_Mode);
	}
}

void Main_Calculation_Module::single_Calculation()
{
	if(calc_Mode!=CALCULATION)
	{
		qInfo() << "Main_Calculation_Module::single_Calculation  :  wrong calc_Mode";
		return;
	}

	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		calculation_Trees[tab_Index]->fill_Independent_Calc_Trees();
		for(Data_Element<Independent_Variables>& independent_Element : calculation_Trees[tab_Index]->independent)
		{
			calculation_Trees[tab_Index]->calculate_1_Kind(independent_Element);
		}
		calculation_Trees[tab_Index]->fill_Target_Calc_Trees();
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
			calculation_Trees[tab_Index]->calculate_1_Kind(target_Element);
		}
	}
	print_Calculated_To_File();
}

void Main_Calculation_Module::fitting()
{
	if(calc_Mode!=FITTING)
	{
		qInfo() << "Main_Calculation_Module::fitting  :  wrong calc_Mode";
		return;
	}

	fitables.clear_All();
	rejected_Sigmas.clear_All();
	rejected_Min_Max.clear_All();
	rejected_Thicknesses_and_Periods.clear_All();
	rejected_Periods.clear_All();

	/// create calc tree and fitables;
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

	/// fill pointers to slaves, starting from fitable top-masters
	for(Parameter* parameter : fitables.fit_Parameters)
	{
		slaves_Vector_Iteration(&parameter->coupled);
	}

	/// rejection
	if(reject()) return;

	/// fitting from here
	if( fitables.fit_Parameters.size()>0 )
	{
		Fitting_GSL fitting_GSL(this);
		fitting_GSL.fit();
		print_Calculated_To_File();
	}
}

bool Main_Calculation_Module::reject()
{
	// sigma value is close to zero
	if(rejected_Sigmas.fit_IDs.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(rejected_Sigmas.fit_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(NULL, "Bad fitables", "Sigma start values are too small in:\n\n" +
													   rejected +
													   "\n\nChange them");
		return true;
	}

	// internal thicknesses and periods if overlying period or gamma are fitables
	if(rejected_Thicknesses_and_Periods.fit_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(rejected_Thicknesses_and_Periods.fit_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(NULL, "Bad fitables", "Thicknesses and periods can't be fitted independently\nif overlying period or gamma are fitables:\n\n" +
													   rejected +
													   "\n\nRecheck fitables");
		return true;
	}

	// internal thicknesses and periods if overlying period or gamma are fitables
	if(rejected_Periods.fit_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(rejected_Periods.fit_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(NULL, "Bad fitables", "Zero period values are not allowed for fitting:\n\n" +
													   rejected +
													   "\n\nIncrease them");
		return true;
	}

	// min>=max
	if(rejected_Min_Max.fit_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(rejected_Min_Max.fit_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(NULL, "Bad fitables", "min>=max for the following fitables:\n\n" +
													   rejected +
													   "\n\nThey won't be fitted");
		return true;
	}

	return false;
}

void Main_Calculation_Module::calc_Tree_Iteration(const tree<Node>::iterator& parent, bool fitables_Period_Gamma)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& struct_Data = child.node->data.struct_Data;

		find_Fittable_Parameters(struct_Data, child, fitables_Period_Gamma);

		// check if period or gamma are fitables
		if(struct_Data.item_Type == item_Type_Multilayer)
		{
			if(struct_Data.period.fit.is_Fitable || (struct_Data.gamma.fit.is_Fitable && child.number_of_children() == 2))
			{
				fitables_Period_Gamma = true;
			}
			calc_Tree_Iteration(child, fitables_Period_Gamma);
		}
	}
}

void Main_Calculation_Module::find_Fittable_Parameters(Data& struct_Data, const tree<Node>::iterator& parent, bool fitables_Period_Gamma)
{
	struct_Data.fill_Potentially_Fitable_Parameters_Vector();
	for(Parameter* parameter : struct_Data.potentially_Fitable_Parameters)
	{
		// for my_Sigmas
		if(parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma &&	!struct_Data.common_Sigma)
			parameter->fit.is_Fitable = struct_Data.sigma.fit.is_Fitable;

		// fitable and has no master
		if(parameter->fit.is_Fitable && !parameter->coupled.master.exist)
		{
			// fixed
			fitables.fit_Struct_Names 		.push_back(multilayer_Tabs->tabText(parameter->indicator.tab_Index));
			fitables.fit_Names				.push_back(parameter->indicator.full_Name);
//			fitables.fit_Whats_This			.push_back(parameter->indicator.whats_This);
			fitables.fit_IDs				.push_back(parameter->indicator.id);
//			fitables.fit_Min				.push_back(parameter->fit.min);
//			fitables.fit_Max				.push_back(parameter->fit.max);

			// changeable
			fitables.fit_Parameters			.push_back(parameter);
			fitables.fit_Value_Parametrized	.push_back(parametrize(parameter->value, parameter->fit.min, parameter->fit.max));
//			fitables.fit_Value_Pointers		.push_back(&parameter->value);
			fitables.fit_Parent_Iterators	.push_back(parent);					// used for period and gamma only, but should be filled for all for the length purpose!
//			fitables.fit_Coupled_Pointers	.push_back(&parameter->coupled);	// used for top-masters only, but should be filled for all for the length purpose!

			/// for rejection

			QString total_Name = "  " + Medium_BlackCircle_Sym + "  <" + multilayer_Tabs->tabText(parameter->indicator.tab_Index) + "> " + parameter->indicator.full_Name;

			// sigma value is close to zero
			if(parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma || parameter->indicator.whats_This == whats_This_Sigma)
			if(struct_Data.sigma.value < 0.1)
			if(!rejected_Sigmas.fit_IDs.contains(struct_Data.sigma.indicator.id))
			{
				rejected_Sigmas.fit_IDs.push_back(struct_Data.sigma.indicator.id);
				rejected_Sigmas.fit_Names.push_back("  " + Medium_BlackCircle_Sym + "  <" + multilayer_Tabs->tabText(struct_Data.sigma.indicator.tab_Index) + "> " + struct_Data.sigma.indicator.full_Name);
			}

			// forbid fitting internal thicknesses and periods if overlying period or gamma are fitables
			if(fitables_Period_Gamma)
			if(parameter->indicator.whats_This == whats_This_Thickness || parameter->indicator.whats_This == whats_This_Period)
			{
				rejected_Thicknesses_and_Periods.fit_Names.push_back(total_Name);
			}

			// period value == 0
			if(parameter->indicator.whats_This == whats_This_Period)
			if(abs(parameter->value) < DBL_MIN)
			{
				rejected_Periods.fit_Names.push_back(total_Name);
			}

			// min>=max
			if(parameter->fit.min>=parameter->fit.max)
			{
				rejected_Min_Max.fit_Names.push_back(total_Name);
			}
		}
	}
}

void Main_Calculation_Module::slaves_Vector_Iteration(Coupled* coupled)
{
	coupled->slave_Pointers.clear();
	for(Parameter_Indicator& slave_Parameter_Indicator : coupled->slaves)
	{
		Parameter* slave = find_Slave_Pointer_by_Id(slave_Parameter_Indicator);

		// check
		if(slave == NULL)
		{
			qInfo() << "Main_Calculation_Module::slaves_Vector_Iteration  : " << slave_Parameter_Indicator.full_Name << "not found\n";
			QMessageBox::critical(NULL, "Main_Calculation_Module::slaves_Vector_Iteration", slave_Parameter_Indicator.full_Name + "\n\nnot found");
			exit(EXIT_FAILURE);
		} else
		{
			coupled->slave_Pointers.append(slave);
			slaves_Vector_Iteration(&slave->coupled);
		}
	}
}

Parameter* Main_Calculation_Module::find_Slave_Pointer_by_Id(const Parameter_Indicator& slave_Parameter_Indicator)
{
	Parameter* pointer = NULL;
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		find_Slave_Pointer_Calc_Tree_Iteration(calculation_Trees[tab_Index]->real_Calc_Tree.begin(), slave_Parameter_Indicator, pointer);
		if(pointer != NULL) return pointer;
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

		if(struct_Data.item_Type == item_Type_Multilayer)
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
		QMessageBox::warning(NULL, "Main_Calculation_Module::parametrize", "Wrong parametrization_Type");
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
		QMessageBox::warning(NULL, "Main_Calculation_Module::unparametrize", "Wrong parametrization_Type");
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

	int num_Points = 0;
	QVector<double> arg;
	if(data_Element.active_Parameter_Whats_This == whats_This_Angle )
	{
		num_Points = data_Element.the_Class->measurement.angle.size();
		arg = data_Element.the_Class->measurement.angle;
	}
	if(data_Element.active_Parameter_Whats_This == whats_This_Wavelength )
	{
		num_Points = data_Element.the_Class->measurement.lambda.size();
		arg = data_Element.the_Class->measurement.lambda;
	}

	QString name = first_Name+"_"+QString::number(index)+".txt";
	QFile file(name);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		for(auto i=0; i<num_Points; ++i)
		{
			out << "\t" << QString::number(arg[i],'f',4)
				<< "\t" << QString::number(data_Element.unwrapped_Reflection->R[i],'e',15)
				<< endl;
		}
	file.close();
	} else
	{
		qInfo() << "Calculation_Tree::print_Reflect_To_File  :  Can't write file " + name;
		QMessageBox::critical(NULL, "Calculation_Tree::print_Reflect_To_File", "Can't write file " + name);
		exit(EXIT_FAILURE);
	}
}
template void Main_Calculation_Module::print_Reflect_To_File<Independent_Variables>(Data_Element<Independent_Variables>&, QString, int);
template void Main_Calculation_Module::print_Reflect_To_File<Target_Curve>		   (Data_Element<Target_Curve>&, QString, int);
