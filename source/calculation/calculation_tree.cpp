// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "calculation_tree.h"
#include <iostream>

Calculation_Tree::Calculation_Tree(QTabWidget* independent_Variables_Plot_Tabs, QVector<Target_Curve*>& target_Profiles_Vector, QTreeWidget* real_Struct_Tree):
	real_Struct_Tree(real_Struct_Tree)
{	
	// initialization independent vector
	{
		for(int i=0; i<independent_Variables_Plot_Tabs->count(); ++i)
		{
			Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(independent_Variables_Plot_Tabs->widget(i));
			if( independent_Variables->calc_Functions.check_Enabled	)
			if(	independent_Variables->calc_Functions.check_Reflectance		||
				independent_Variables->calc_Functions.check_Transmittance	||
				independent_Variables->calc_Functions.check_Absorptance		||
				independent_Variables->calc_Functions.check_Field			||
				independent_Variables->calc_Functions.check_Joule			||
				independent_Variables->calc_Functions.check_User )
			{
				independent.resize(independent.size()+1);

				independent.last().the_Class = independent_Variables;
				independent.last().curve_Class = INDEPENDENT;
			}
		}
	}

	// initialization target profiles vector
	{
		for(int i=0; i<target_Profiles_Vector.size(); ++i)
		{
			if(target_Profiles_Vector[i]->loaded_And_Ready)
			{
				target.resize(target.size()+1);

				target.last().the_Class = target_Profiles_Vector[i];
				target.last().curve_Class = TARGET;
				target.last().active_Item_Type = item_Type_Measurement;
			}
		}
	}

	create_Rand_Generator();
	check_If_Graded();
	max_Depth = Global_Variables::get_Tree_Depth(real_Struct_Tree->invisibleRootItem());	// unstratified depth
}

//void Calculation_Tree::run_All()
//{
//	qInfo() << "";
//	qInfo() << "Calculation_Tree::run_All : independent.size() =" << independent.size();
//	qInfo() << "Calculation_Tree::run_All : target.size()      =" << target.size();
//	if(independent.size()+target.size()>0)
//	{
//		fill_All_Calc_Trees();																	// here we have trees of "Node"
//		calculate();
//	} else
//	{
//		qInfo() << "Calculation_Tree::run_All  :  no data for calculation";
//	}
//}

void Calculation_Tree::create_Rand_Generator()
{
	const gsl_rng_type* rng_Type;
	rng_Type = gsl_rng_default;
	r = gsl_rng_alloc (rng_Type);

	auto now = std::chrono::system_clock::now();
	gsl_rng_set(r,std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count());
}

void Calculation_Tree::check_If_Graded()
{
	QTreeWidgetItem* structure_Item;
	QTreeWidgetItemIterator it(real_Struct_Tree);
	while (*it)
	{
		structure_Item = *it;

		Data layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		if(layer.item_Type == item_Type_Layer)
		{
			if(layer.thickness_Drift.is_Drift_Line) depth_Grading = true;
			if(layer.thickness_Drift.is_Drift_Sine) depth_Grading = true;
			if(layer.thickness_Drift.is_Drift_Rand) depth_Grading = true;

			if(layer.sigma_Drift.is_Drift_Line) sigma_Grading = true;
			if(layer.sigma_Drift.is_Drift_Sine) sigma_Grading = true;
			if(layer.sigma_Drift.is_Drift_Rand) sigma_Grading = true;
		}
		++it;
	}
}

void Calculation_Tree::fill_Tree_From_Scratch(tree<Node>& calc_Tree, QTreeWidget* item_Tree, QString curve_Class)
{
	Node empty_Top_Node;
	calc_Tree.insert(calc_Tree.begin(), empty_Top_Node);	// according to the tree.hh API

	fill_Tree(calc_Tree.begin(), calc_Tree, item_Tree->invisibleRootItem());

	// if no substrate then add it equal to ambient
	Data last_Struct_Data = item_Tree->topLevelItem(item_Tree->topLevelItemCount()-1)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(last_Struct_Data.item_Type != item_Type_Substrate)
	{
		QTreeWidgetItem substrate_Item;

		// the first (0) is the ambient in real tree, (1) in independent copy // WARNING !
		int ambient_Index_In_Item_Tree = -1;
		if(curve_Class == INDEPENDENT) ambient_Index_In_Item_Tree = 1;
		if(curve_Class == TARGET)	   ambient_Index_In_Item_Tree = 0;
		Data substrate = item_Tree->topLevelItem(ambient_Index_In_Item_Tree)->data(DEFAULT_COLUMN, Qt::UserRole).template value<Data>();

		// change id
		substrate.reset_All_IDs();
		substrate.item_Type = item_Type_Substrate;

		// set data
		QVariant var; var.setValue( substrate );
		substrate_Item.setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// create node
		Node substrate_Node(&substrate_Item);

		// add last node
		calc_Tree.append_child(calc_Tree.begin(), substrate_Node);
	}
}

void Calculation_Tree::fill_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree, QTreeWidgetItem* item)
{
	for(int i=0; i<item->childCount(); ++i)
	{
		Node temp_Node(item->child(i));
		calc_Tree.append_child(parent, temp_Node);	// Multilayer items are also here

		if(item->child(i)->childCount()>0)
		{
			fill_Tree(calc_Tree.child(parent,i), calc_Tree, item->child(i));
		}
	}
}

void Calculation_Tree::fill_All_Calc_Trees()
{
	// for independent
	for(Data_Element<Independent_Variables>& data_Element : independent)
	{
		fill_Tree_From_Scratch(data_Element.calc_Tree, data_Element.the_Class->struct_Tree_Copy, data_Element.curve_Class);
		statify_Calc_Tree(data_Element.calc_Tree);

		// remove measurement
		data_Element.calc_Tree.erase(data_Element.calc_Tree.child(data_Element.calc_Tree.begin(),0));
	}

	// for target
	if(target.size()>0)
	{
		///--------------------------------------------------------------------
		/// here is the entry point for fitting
		fill_Tree_From_Scratch(real_Calc_Tree, real_Struct_Tree, TARGET);
		///--------------------------------------------------------------------
		for(Data_Element<Target_Curve>& data_Element : target)
		{
			data_Element.calc_Tree = real_Calc_Tree;
			statify_Calc_Tree(data_Element.calc_Tree);
		}
	}
}

void Calculation_Tree::statify_Calc_Tree_Iteration(const tree<Node>::iterator& parent, int depth, QVector<tree<Node>::iterator>& chosen_Nodes)
{
	// iterate over tree, looking for fixed depth
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);

		if(tree<Node>::depth(child) == depth)
		{
			if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
			{
				chosen_Nodes.append(child);
			}
		}
		if(child.number_of_children()>0)
		{
			statify_Calc_Tree_Iteration(child, depth, chosen_Nodes);
		}
	}
}

void Calculation_Tree::statify_Calc_Tree(tree<Node>& calc_Tree)
{
	for(int depth=max_Depth-1; depth>0; depth--)
	{
		QVector<tree<Node>::iterator> chosen_Nodes;

		statify_Calc_Tree_Iteration(calc_Tree.begin(), depth, chosen_Nodes);

		for(int vec_Index=chosen_Nodes.size()-1; vec_Index>=0; --vec_Index)
		{
			tree<Node>::iterator chosen_Child = chosen_Nodes[vec_Index];

			// if 0 periods
			if(chosen_Child.node->data.struct_Data.num_Repetition.value == 0)
			{
				// delete
				calc_Tree.erase(chosen_Child);
			} else

			// if 1 period
			if(chosen_Child.node->data.struct_Data.num_Repetition.value == 1)
			{
				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
				{
					calc_Tree.insert_subtree(chosen_Child, tree<Node>::child(chosen_Child,child_Index));
				}
				// delete
				calc_Tree.erase(chosen_Child);
				// not delete
//				chosen_Child.node->data.stack_Content.num_Repetition.value = 0;
			} else

			// if 2 periods
			if(chosen_Child.node->data.struct_Data.num_Repetition.value == 2)
			{
				tree<Node>::iterator next = chosen_Child;
				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
				{
					calc_Tree.insert_subtree     (chosen_Child, tree<Node>::child(chosen_Child,child_Index));
					next = calc_Tree.insert_subtree_after(next, tree<Node>::child(chosen_Child,child_Index));
				}
				// delete
				calc_Tree.erase(chosen_Child);
				// not delete
//				chosen_Child.node->data.stack_Content.num_Repetition.value = 0;
			} else

			// if >=3 periods
			if(chosen_Child.node->data.struct_Data.num_Repetition.value >= 3)
			{
				tree<Node>::iterator next = chosen_Child;
				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
				{
					calc_Tree.insert_subtree     (chosen_Child, tree<Node>::child(chosen_Child,child_Index));
					next = calc_Tree.insert_subtree_after(next, tree<Node>::child(chosen_Child,child_Index));
				}
				// change data
				chosen_Child.node->data.struct_Data.num_Repetition.value -= 2;
			}
		}
	}
}

template <typename Type>
void Calculation_Tree::calculate_1_Kind(QVector<Data_Element<Type>>& data_Element_Vec)
{
	// for each plot
	int plot_Counter=0;
	for(Data_Element<Type>& data_Element : data_Element_Vec)
	{
		// calculation of wavenumbers and cos squares
		if(data_Element.curve_Class == INDEPENDENT)	data_Element.the_Class->measurement.calc_Independent_cos2_k(); else
		if(data_Element.curve_Class == TARGET)		data_Element.the_Class->measurement.calc_Measured_cos2_k();

		// find active node
		if(data_Element.curve_Class == INDEPENDENT)
		{
			Independent_Variables* object = qobject_cast<Independent_Variables*>(data_Element.the_Class);

			// find whats_This of active item
			for(int item_Index=0; item_Index<object->independent_Variables_List->count(); ++item_Index)
			{
				QListWidgetItem* list_Item = object->independent_Variables_List->item(item_Index);
				Independent_Indicator item_Indicator = list_Item->data(Qt::UserRole).value<Independent_Indicator>();

				// if active
				if(item_Indicator.is_Active)
				{
					data_Element.active_Item_Type			 = item_Indicator.item_Type;
					data_Element.active_Item_Id				 = item_Indicator.item_Id;
					data_Element.active_Parameter_Whats_This = item_Indicator.parameter_Whats_This;
				}
			}
		} else
		if(data_Element.curve_Class == TARGET)
		{
			Target_Curve* target_Curve = qobject_cast<Target_Curve*>(data_Element.the_Class);

			// only measurement can be active here
			data_Element.active_Item_Type			 = item_Type_Measurement;
			data_Element.active_Item_Id				 = target_Curve->measurement.id;
			data_Element.active_Parameter_Whats_This = target_Curve->curve.argument_Type;
		}

		// find corresponding node for active variable
//		tree<Node>::iterator active_Iter = find_Node_By_Item_Id(data_Element.calc_Tree.begin(), data_Element.active_Item_Id, data_Element.calc_Tree);
		// ....................................................................
		// emergency case
//		if (active_Iter.node->data.stop_Calculation) { qInfo() << "stop_Calculation"; return; }
		// ....................................................................
		// if measurement is not active, create tree for each plot point
		if(data_Element.active_Item_Type != item_Type_Measurement)
		{
			// TODO
			qInfo() << "Active" << data_Element.active_Item_Type;
		} else
		if(data_Element.active_Item_Type == item_Type_Measurement)
		{
			auto start = std::chrono::system_clock::now();
			calculate_Intermediate_Values_1_Tree(data_Element.calc_Tree, data_Element.the_Class->measurement, data_Element.active_Parameter_Whats_This, data_Element.calc_Tree.begin());
			auto end = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			qInfo() << "Intermediate: "<< elapsed.count()/1000000. << " seconds" << endl;

			start = std::chrono::system_clock::now();
			calculate_Unwrapped_Structure		(data_Element.calc_Tree, data_Element.the_Class->measurement, data_Element.active_Parameter_Whats_This, data_Element.unwrapped_Structure);
			end = std::chrono::system_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			qInfo() << "Unwrap: "<< elapsed.count()/1000000. << " seconds" << endl;

			start = std::chrono::system_clock::now();
			calculate_Unwrapped_Reflectivity	(						 data_Element.the_Class->measurement, data_Element.active_Parameter_Whats_This, data_Element.unwrapped_Structure, data_Element.unwrapped_Reflection);
			end = std::chrono::system_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			qInfo() << "Unwrap Reflect: "<< elapsed.count()/1000000. << " seconds" << endl;

			print_Reflect_To_File(data_Element,plot_Counter);
		}
		plot_Counter++;
	}
}
template void Calculation_Tree::calculate_1_Kind<Independent_Variables>(QVector<Data_Element<Independent_Variables>>&);
template void Calculation_Tree::calculate_1_Kind<Target_Curve>		   (QVector<Data_Element<Target_Curve>>&);

void Calculation_Tree::calculate()
{
	calculate_1_Kind(independent);
	calculate_1_Kind(target);
}

void Calculation_Tree::calculate_Intermediate_Values_1_Tree(tree<Node>& calc_Tree, const Data& measurement, QString active_Parameter_Whats_This, const tree<Node>::iterator& parent, Node* above_Node)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);

		QString warning_Text;
		int status = child.node->data.calculate_Intermediate_Points(measurement, above_Node, active_Parameter_Whats_This, warning_Text, depth_Grading, sigma_Grading);
		if(status!=0)
		{
			QMessageBox::warning(0, "Warning", warning_Text);
			return;
		}
		if(child.node->data.struct_Data.item_Type != item_Type_Multilayer)
		{
			above_Node = &child.node->data;
		} else
		{
			calculate_Intermediate_Values_1_Tree(calc_Tree, measurement, active_Parameter_Whats_This, child, above_Node);
		}
	}
}

tree<Node>::iterator Calculation_Tree::find_Node_By_Item_Id(const tree<Node>::iterator& parent, int active_Item_Id, tree<Node>& calc_Tree)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);

		if(child.node->data.struct_Data.id == active_Item_Id)
		{
			return calc_Tree.child(parent,i);
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			return find_Node_By_Item_Id(child, active_Item_Id, calc_Tree);
		}
	}
	// error handling
	{
		QMessageBox::critical(0, "Error", "Calculation_Tree::find_Node\nerror: node \"" + QString::number(active_Item_Id) + "\"not found");
		parent.node->data.stop_Calcuation = true;
	}
	return parent;
}

void Calculation_Tree::calculate_Unwrapped_Structure(tree<Node>& calc_Tree, const Data& measurement, QString active_Parameter_Whats_This, Unwrapped_Structure*& unwrapped_Structure_Vec_Element)
{
	num_Media = get_Total_Num_Layers(calc_Tree.begin(), calc_Tree);
	Unwrapped_Structure*   new_Unwrapped_Structure  = new Unwrapped_Structure (calc_Tree, measurement, active_Parameter_Whats_This, num_Media, max_Depth, depth_Grading, sigma_Grading, r);
	unwrapped_Structure_Vec_Element = new_Unwrapped_Structure;
}

void Calculation_Tree::calculate_Unwrapped_Reflectivity(				    const Data& measurement, QString active_Parameter_Whats_This, Unwrapped_Structure* unwrapped_Structure_Vec_Element, Unwrapped_Reflection*& unwrapped_Reflection_Vec_Element)
{
	Unwrapped_Reflection*  new_Unwrapped_Reflection = new Unwrapped_Reflection(unwrapped_Structure_Vec_Element, num_Media, active_Parameter_Whats_This, measurement, depth_Grading, sigma_Grading);
	unwrapped_Reflection_Vec_Element = new_Unwrapped_Reflection;

	auto start = std::chrono::system_clock::now();
	unwrapped_Reflection_Vec_Element->calc_Specular();
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	qInfo() << "Bare Reflectivity:      "<< elapsed.count()/1000. << " seconds" << endl;

	cout << "r_s     [" << 0 << "] = " << unwrapped_Reflection_Vec_Element->r_s[0] << endl;
	cout << "r_p     [" << 0 << "] = " << unwrapped_Reflection_Vec_Element->r_p[0] << endl;

	cout << "R_s     [" << 0 << "] = " << unwrapped_Reflection_Vec_Element->R_s[0] << endl;
	cout << "R_p     [" << 0 << "] = " << unwrapped_Reflection_Vec_Element->R_p[0] << endl;
	cout << "--------------------------------\n";
}

template <typename Type>
void Calculation_Tree::print_Reflect_To_File(Data_Element<Type>& data_Element, int index)
{
	QString first_Name;
	if(data_Element.curve_Class == INDEPENDENT)	first_Name = "calc_independent";
	if(data_Element.curve_Class == TARGET)		first_Name = "calc_target";

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
template void Calculation_Tree::print_Reflect_To_File<Independent_Variables>(Data_Element<Independent_Variables>&, int);
template void Calculation_Tree::print_Reflect_To_File<Target_Curve>		    (Data_Element<Target_Curve>&, int);

int Calculation_Tree::get_Total_Num_Layers(const tree<Node>::iterator& parent, const tree<Node>& calc_Tree)
{
	int num_Media_Local = 0;
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);

		if(child.node->data.struct_Data.item_Type == item_Type_Ambient ||
		   child.node->data.struct_Data.item_Type == item_Type_Layer   ||
		   child.node->data.struct_Data.item_Type == item_Type_Substrate )
		{
			num_Media_Local += 1;
		}

		if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			num_Media_Local += child.node->data.struct_Data.num_Repetition.value * get_Total_Num_Layers(child, calc_Tree);
		}
	}
	return num_Media_Local;
}

void Calculation_Tree::print_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);

		{
			std::cout << "node :  depth : " << calc_Tree.depth(child) << "   ";
			for(int y=0; y<calc_Tree.depth(child)-1; y++)
			{	std::cout << "\t";}
			std::cout << child.node->data.struct_Data.item_Type.toStdString();
			if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
			{	std::cout << " : " << child.node->data.struct_Data.num_Repetition.value;	}
			std::cout << std::endl;
		}

		if(child.node->data.struct_Data.item_Type  == item_Type_Multilayer)
		{
			print_Tree(child, calc_Tree);
		}
	}
}

void Calculation_Tree::print_Flat_list(QList<Node> flat_List)
{
	for(int i=0; i<flat_List.size(); ++i)
	{
		std::cout << "list : \t\t" << flat_List[i].struct_Data.item_Type.toStdString() << std::endl;
	}
}

void Calculation_Tree::print_Item_Tree(QTreeWidgetItem* item)
{
	for(int i=0; i<item->childCount(); ++i)
	{
		QTreeWidgetItem* child = item->child(i);

		Data struct_Data = child->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		{
			std::cout << "item :  depth : " << Global_Variables::get_Item_Depth(child) << "   ";
			for(int y=0; y<Global_Variables::get_Item_Depth(child)-1; y++)
			{	std::cout << "\t";}
			std::cout << child->whatsThis(DEFAULT_COLUMN).toStdString();
			if(struct_Data.item_Type == item_Type_Multilayer)
			{	std::cout << " : " << child->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().num_Repetition.value;	}
			std::cout << std::endl;
		}

		if(struct_Data.item_Type == item_Type_Multilayer)
		{
			print_Item_Tree(child);
		}
	}
}
