// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "main_calculation_module.h"

Main_Calculation_Module::Main_Calculation_Module(QTabWidget* multilayer_Tabs):
	multilayers		 (multilayer_Tabs->count()),
	calculation_Trees(multilayer_Tabs->count()),
	multilayer_Tabs	 (multilayer_Tabs)
{
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		multilayers[tab_Index] = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
		calculation_Trees[tab_Index] = new Calculation_Tree(multilayers[tab_Index]->independent_Variables_Plot_Tabs,
															multilayers[tab_Index]->target_Profiles_Vector,
															multilayers[tab_Index]->structure_Tree->tree);
	}
}

void Main_Calculation_Module::single_Calculation()
{
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		if( multilayers[tab_Index]->enable_Calc_Independent_Curves )
		{
			calculation_Trees[tab_Index]->fill_Independent_Calc_Trees();			
			for(Data_Element<Independent_Variables>& independent_Element : calculation_Trees[tab_Index]->independent)
			{
				calculation_Trees[tab_Index]->calculate_1_Kind(independent_Element);
			}
		}
		if( multilayers[tab_Index]->enable_Calc_Target_Curves )
		{
			calculation_Trees[tab_Index]->fill_Target_Calc_Trees();
			for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
			{
				calculation_Trees[tab_Index]->calculate_1_Kind(target_Element);
			}
		}
	}
}

void Main_Calculation_Module::fitting()
{
	fitables.clear_All();
	rejected_Fitables.clear_All();

	// create calc tree and fitables;
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		if( multilayers[tab_Index]->enable_Calc_Target_Curves )
		if( calculation_Trees[tab_Index]->target.size()>0 )
		{
			// prepare real_Calc_Tree
			calculation_Trees[tab_Index]->fill_Tree_From_Scratch(calculation_Trees[tab_Index]->real_Calc_Tree, calculation_Trees[tab_Index]->real_Struct_Tree, TARGET);

			// find fitables over tree
			calc_Tree_Iteration(calculation_Trees[tab_Index]->real_Calc_Tree.begin());
		}
	}

	// warning
	if(rejected_Fitables.fit_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(rejected_Fitables.fit_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(NULL, "Bad fitables",
													   "min>=max for the following fitables:\n\n" +
													   rejected +
													   "\n\n They won't be fitted");
	}

	/// fitting from here
	if( fitables.fit_Value_Pointers.size()>0 )
	{
		Fitting_GSL fitting_GSL(this);
		fitting_GSL.fit();
	}

}

void Main_Calculation_Module::calc_Tree_Iteration(const tree<Node>::iterator& parent)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& struct_Data = child.node->data.struct_Data;
		find_Fittable_Parameters(struct_Data);

		if(child.number_of_children()>0)
		{
			calc_Tree_Iteration(child);
		}
	}
}

void Main_Calculation_Module::find_Fittable_Parameters(Data& struct_Data)
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
			if(parameter->fit.min<parameter->fit.max)
			{
				// fixed
				fitables.fit_Struct_Names 		.push_back(multilayer_Tabs->tabText(parameter->indicator.tab_Index));
				fitables.fit_Names				.push_back(parameter->indicator.full_Name);
				fitables.fit_Whats_This			.push_back(parameter->indicator.whats_This);
				fitables.fit_IDs				.push_back(parameter->indicator.id);
				fitables.fit_Min				.push_back(parameter->fit.min);
				fitables.fit_Max				.push_back(parameter->fit.max);

				// changeable
				fitables.fit_Value_Parametrized	.push_back(parametrize(parameter->value, parameter->fit.min, parameter->fit.max));
				fitables.fit_Value_Pointers		.push_back(&parameter->value);

			} else
			{
				rejected_Fitables.fit_Names		.push_back("  " + Medium_BlackCircle_Sym + "  <" + multilayer_Tabs->tabText(parameter->indicator.tab_Index) + "> " + parameter->indicator.full_Name);
			}
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
		qInfo() << "Main_Calculation_Module::parametrize : wrong type";
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
		qInfo() << "Main_Calculation_Module::unparametrize : wrong type";
		return parametrized_Shifted_Value;
	}
}

void Main_Calculation_Module::print_Calculated_To_File()
{
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		int counter = 0;
		for(Data_Element<Independent_Variables>& independent : calculation_Trees[tab_Index]->independent)
		{
			print_Reflect_To_File(independent, multilayer_Tabs->tabText(tab_Index), counter++);
		}
		counter = 0;
		for(Data_Element<Target_Curve>& target : calculation_Trees[tab_Index]->target)
		{
			print_Reflect_To_File(target, multilayer_Tabs->tabText(tab_Index), counter++);
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
	} else{QTextStream(stderr) << "Calculation_Tree::print_Reflect_To_File  :  Can't write file " + name << endl;	exit(EXIT_FAILURE);}
}
template void Main_Calculation_Module::print_Reflect_To_File<Independent_Variables>(Data_Element<Independent_Variables>&, QString, int);
template void Main_Calculation_Module::print_Reflect_To_File<Target_Curve>		   (Data_Element<Target_Curve>&, QString, int);
