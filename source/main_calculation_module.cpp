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
		if( multilayers[tab_Index]->enable_Calc_Independent_Curves ||
			multilayers[tab_Index]->enable_Calc_Target_Curves )
		{
			calculation_Trees[tab_Index]->fill_Independent_Calc_Trees();
			calculation_Trees[tab_Index]->fill_Target_Calc_Trees();
			calculation_Trees[tab_Index]->calculate();
		}
	}
}

void Main_Calculation_Module::fitting()
{
	fitables.clear_All();

	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		if( multilayers[tab_Index]->enable_Calc_Target_Curves )
		if( calculation_Trees[tab_Index]->target.size()>0)
		{
			// prepare real_Calc_Tree
			calculation_Trees[tab_Index]->fill_Tree_From_Scratch(calculation_Trees[tab_Index]->real_Calc_Tree, calculation_Trees[tab_Index]->real_Struct_Tree, TARGET);

			calc_Tree_Iteration(calculation_Trees[tab_Index]->real_Calc_Tree.begin());
//			for(Target_Curve* target_Profile : multilayers[tab_Index]->target_Profiles_Vector)
//			{
//				if(target_Profile->fit_Params.fit)
//				{

//				}
//			}
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
			qInfo() << parameter->indicator.whats_This << "is fitable";
		}
	}
}

void Main_Calculation_Module::calc_Tree_Iteration(const tree<Node>::iterator& parent)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);

		Data& struct_Data = child.node->data.struct_Data;
//		qInfo() << struct_Data.item_Type << ", depth = " << tree<Node>::depth(child);
		find_Fittable_Parameters(struct_Data);

		if(child.number_of_children()>0)
		{
			calc_Tree_Iteration(child);
		}
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
				<< "\t" << QString::number(data_Element.unwrapped_Reflection->R[i],'e',6)
				<< endl;
		}
	file.close();
	} else{QTextStream(stderr) << "Calculation_Tree::print_Reflect_To_File  :  Can't write file " + name << endl;	exit(EXIT_FAILURE);}
}
template void Main_Calculation_Module::print_Reflect_To_File<Independent_Variables>(Data_Element<Independent_Variables>&, QString, int);
template void Main_Calculation_Module::print_Reflect_To_File<Target_Curve>		   (Data_Element<Target_Curve>&, QString, int);
