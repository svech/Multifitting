// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "calculation_tree.h"
#include <iostream>

Calculation_Tree::Calculation_Tree(QVector<Independent_Variables*>& independent_Variables_Vector, QVector<Target_Curve*>& measured_Data_Vector, QVector<Target_Curve*>& target_Profiles_Vector): //-V730
	independent(independent_Variables_Vector.size()),
	measured(0/*measured_Data_Vector.size()*/),
	target(0/*target_Profiles_Vector.size()*/)	// TEMPORARY
{
	// initialization of vectors
	for(int i=0; i<independent.size(); ++i)
	{
		independent[i].the_Class = independent_Variables_Vector[i];
		independent[i].whats_This = INDEPENDENT;
	}
	for(int i=0; i<measured.size(); ++i)
	{
		measured[i].the_Class = measured_Data_Vector[i];
		measured[i].whats_This = MEASURED;
	}
	for(int i=0; i<target.size(); ++i)
	{
		target[i].the_Class = target_Profiles_Vector[i];
		target[i].whats_This = OPTIMIZE;
	}

	// choose one existing item tree for analysis
	if(independent.size()>0) one_Local_Item_Tree = independent[0].local_Item_Tree;	else
	if(measured.size()>0)	 one_Local_Item_Tree = measured[0].local_Item_Tree;	else
	if(target.size()>0)		 one_Local_Item_Tree = target[0].local_Item_Tree;
}

Calculation_Tree::~Calculation_Tree()
{
//	for(int i=0; i<num_Independent; ++i)
//	{
//		delete unwrapped_Structure_Vec[i];
//		delete unwrapped_Reflection_Vec[i];
//	}
}

void Calculation_Tree::run_All()
{
	if(independent.size()+measured.size()+target.size()>0)
	{
		create_Rand_Generator();
		create_All_Local_Item_Trees();
		check_If_Graded();
		{
			max_Depth = Global_Variables::get_Tree_Depth(one_Local_Item_Tree->invisibleRootItem());	// unstratified depth
			fill_All_Calc_Trees();																	// here we have trees of "Node"
			calculate_Intermediate_Values();
		}
		//	independent.clear();
		//	measured.clear();
		//	target.clear();
	} else
	{
		qInfo() << "Calculation_Tree::run_All  :  no data for calculation";
	}
}

void Calculation_Tree::create_Rand_Generator()
{
	const gsl_rng_type * T;
	T = gsl_rng_default;
	r = gsl_rng_alloc (T);

	auto now = std::chrono::system_clock::now();
	gsl_rng_set(r,std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count());
}

template <typename T>
void Calculation_Tree::create_1_Kind_of_Local_Item_Tree(QVector<Data_Element<T>>& data_Element_Vec)
{
	// independent trees
	for(int index=0; index<data_Element_Vec.size(); ++index)
	{
		QTreeWidget* local_Item_Tree = new QTreeWidget;
		for(int i=0; i<data_Element_Vec[index].the_Class->struct_Tree_Copy->topLevelItemCount(); ++i)
		{
			local_Item_Tree->addTopLevelItem(data_Element_Vec[index].the_Class->struct_Tree_Copy->topLevelItem(i)->clone()); // this is local
		}
		data_Element_Vec[index].local_Item_Tree = local_Item_Tree;
	}
}
template void Calculation_Tree::create_1_Kind_of_Local_Item_Tree<Independent_Variables>(QVector<Data_Element<Independent_Variables>>&);
template void Calculation_Tree::create_1_Kind_of_Local_Item_Tree<Target_Curve>		   (QVector<Data_Element<Target_Curve>>&);

void Calculation_Tree::create_All_Local_Item_Trees()
{
	create_1_Kind_of_Local_Item_Tree(independent);
	create_1_Kind_of_Local_Item_Tree(measured);
	create_1_Kind_of_Local_Item_Tree(target);
}

void Calculation_Tree::check_If_Graded()
{
	QTreeWidgetItem* structure_Item;
	QTreeWidgetItemIterator it(one_Local_Item_Tree);
	while (*it)
	{
		structure_Item = *it;

		QString whats_This = structure_Item->whatsThis(DEFAULT_COLUMN);
		QStringList whats_This_List = whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

		if(whats_This_List[0] == whats_This_Layer)
		{
			Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

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

template <typename T>
void Calculation_Tree::fill_1_Kind_of_Calc_Trees(QVector<Data_Element<T>>& data_Element_Vec)
{
	// for each plot
	for(int index=0; index<data_Element_Vec.size(); ++index)
	{
		Node empty_Top_Node;
		data_Element_Vec[index].calc_Tree.insert(data_Element_Vec[index].calc_Tree.begin(), empty_Top_Node);	// according to the tree.hh API

		fill_Tree(data_Element_Vec[index].calc_Tree.begin(), data_Element_Vec[index].calc_Tree, data_Element_Vec[index].local_Item_Tree->invisibleRootItem());

		// if no set substrate then the second ambient
		if(data_Element_Vec[index].local_Item_Tree->topLevelItem(data_Element_Vec[index].local_Item_Tree->topLevelItemCount()-1)->whatsThis(DEFAULT_COLUMN) != whats_This_Substrate)
		{
			QTreeWidgetItem substrate_Item;
			substrate_Item.setWhatsThis(DEFAULT_COLUMN, whats_This_Substrate);

			Ambient ambient = data_Element_Vec[index].local_Item_Tree->topLevelItem(1)->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
			Substrate substrate;

			// ambient inheritance
			substrate.composed_Material				= ambient.composed_Material;
			substrate.material						= ambient.material;
			substrate.absolute_Density				= ambient.absolute_Density;
			substrate.relative_Density				= ambient.relative_Density;
			substrate.separate_Optical_Constants	= ambient.separate_Optical_Constants;
			substrate.permittivity					= ambient.permittivity;
			substrate.absorption					= ambient.absorption;
			substrate.composition					= ambient.composition;

			// substrate' fields
			substrate.use_PSD = false;
			substrate.sigma.value = 0;

			// set data
			QVariant var; var.setValue( substrate );
			substrate_Item.setData(DEFAULT_COLUMN, Qt::UserRole, var);

			// create node
			Node substrate_Node(&substrate_Item);

			// add node
			data_Element_Vec[index].calc_Tree.append_child(data_Element_Vec[index].calc_Tree.begin(), substrate_Node);
		}
	}
}
template void Calculation_Tree::fill_1_Kind_of_Calc_Trees<Independent_Variables>(QVector<Data_Element<Independent_Variables>>&);
template void Calculation_Tree::fill_1_Kind_of_Calc_Trees<Target_Curve>			(QVector<Data_Element<Target_Curve>>&);

void Calculation_Tree::fill_All_Calc_Trees()
{
	fill_1_Kind_of_Calc_Trees(independent);
	fill_1_Kind_of_Calc_Trees(measured);
	fill_1_Kind_of_Calc_Trees(target);
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

void Calculation_Tree::statify_Calc_Tree_Iteration(const tree<Node>::iterator& parent, int depth, QVector<tree<Node>::iterator>& chosen_Nodes)
{
	// iterate over tree, looking for fixed depth
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
        tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);

		if(tree<Node>::depth(child) == depth)
		{
			QStringList whats_This_List = child.node->data.whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

			if(whats_This_List[0] == whats_This_Multilayer)
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
			if(chosen_Child.node->data.stack_Content.num_Repetition.value == 0)
			{
				// delete
				calc_Tree.erase(chosen_Child);
			} else

			// if 1 period
			if(chosen_Child.node->data.stack_Content.num_Repetition.value == 1)
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
			if(chosen_Child.node->data.stack_Content.num_Repetition.value == 2)
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
			if(chosen_Child.node->data.stack_Content.num_Repetition.value >= 3)
			{
				tree<Node>::iterator next = chosen_Child;
				for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
				{
					calc_Tree.insert_subtree     (chosen_Child, tree<Node>::child(chosen_Child,child_Index));
					next = calc_Tree.insert_subtree_after(next, tree<Node>::child(chosen_Child,child_Index));
				}
				// change data
				chosen_Child.node->data.stack_Content.num_Repetition.value -= 2;
			}
		}
	}
}

template <typename T>
void Calculation_Tree::calculate_Intermediate_Values_For_1_Kind(QVector<Data_Element<T>>& data_Element_Vec)
{
	// for each plot
	for(int index=0; index<data_Element_Vec.size(); ++index)
	{
		// for independent only
		if(data_Element_Vec[index].whats_This == INDEPENDENT)
		{
			Independent_Variables* object = qobject_cast<Independent_Variables*>(data_Element_Vec[index].the_Class);
			// find whatsThis of active item
			for(int item_Index=0; item_Index<object->independent_Variables_List->count(); ++item_Index)
			{
				QListWidgetItem* list_Item = object->independent_Variables_List->item(item_Index);

				// if active
				if(list_Item->data(Qt::UserRole).toBool())
				{
					QStringList whats_This_List = list_Item->whatsThis().split(whats_This_Delimiter,QString::SkipEmptyParts);
					whats_This_List.removeFirst();
					QString good_Whats_This = whats_This_List.join(whats_This_Delimiter);

					data_Element_Vec[index].active_Whats_This = good_Whats_This;
				}
			}
			qInfo() << "Calculation_Tree::calculate_Intermediate_Values_For_1_Kind : independent\t" << data_Element_Vec[index].active_Whats_This;
		} else
		{
			qInfo() << "Calculation_Tree::calculate_Intermediate_Values_For_1_Kind : not independent";
			data_Element_Vec[index].active_Whats_This = whats_This_Measurement;
		}

		// preliminary "measurement" calculation for each tree
		tree<Node>::iterator measurement_Iter = find_Node(data_Element_Vec[index].calc_Tree.begin(), whats_This_Measurement, data_Element_Vec[index].calc_Tree);

		// ....................................................................
		// emergency case
		if (measurement_Iter.node->data.whats_This == stop_Calculation) return;
		// ....................................................................

		// calculation of wavenumbers and cos squares
		if(data_Element_Vec[index].whats_This == INDEPENDENT)
		{
			measurement_Iter.node->data.measurement.calc_Independent_cos2_k();
		} else
		{
			measurement_Iter.node->data.measurement.calc_Measured_cos2_k();
		}
		data_Element_Vec[index].measurement = measurement_Iter.node->data.measurement;

		// find corresponding node for active variable
		QStringList active_Whats_This_List = data_Element_Vec[index].active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
		tree<Node>::iterator active_Iter = find_Node(data_Element_Vec[index].calc_Tree.begin(), active_Whats_This_List[0], data_Element_Vec[index].calc_Tree);

		// ....................................................................
		// emergency case
		if (active_Iter.node->data.whats_This == stop_Calculation) return;
		// ....................................................................


		// if measurement is not active, create tree for each plot point
		if(active_Iter.node->data.whats_This != whats_This_Measurement)
		{
			// TODO

		} else
		// 1 tree for 1 plot
		{
			QStringList active_Whats_This_List = data_Element_Vec[index].active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
			qInfo() << active_Whats_This_List[1] << endl;

			auto start = std::chrono::system_clock::now();
			statify_Calc_Tree(data_Element_Vec[index].calc_Tree);
			calculate_Intermediate_Values_1_Tree(data_Element_Vec[index].calc_Tree.begin(), active_Iter, data_Element_Vec[index].active_Whats_This, data_Element_Vec[index].calc_Tree);
			auto end = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			qInfo() << "Intermediate: "<< elapsed.count()/1000000. << " seconds" << endl;

			start = std::chrono::system_clock::now();
			calculate_Unwrapped_Structure(active_Iter, data_Element_Vec[index].active_Whats_This, data_Element_Vec[index].calc_Tree, data_Element_Vec[index].unwrapped_Structure);
			end = std::chrono::system_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			qInfo() << "Unwrap: "<< elapsed.count()/1000000. << " seconds" << endl;

			start = std::chrono::system_clock::now();
			calculate_Unwrapped_Reflectivity(data_Element_Vec[index].active_Whats_This, data_Element_Vec[index].measurement, data_Element_Vec[index].unwrapped_Structure, data_Element_Vec[index].unwrapped_Reflection);
			end = std::chrono::system_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			qInfo() << "Unwrap Reflect: "<< elapsed.count()/1000000. << " seconds" << endl;

			QString first_Name, arg_Type;
			if(data_Element_Vec[index].whats_This == INDEPENDENT)
			{
				QStringList active_Whats_This_List = data_Element_Vec[index].active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
				if(active_Whats_This_List[1] == whats_This_Angle )			arg_Type = measurement_Types[0];
				if(active_Whats_This_List[1] == whats_This_Wavelength )		arg_Type = measurement_Types[1];
				first_Name = "calc_independent";
			} else
			if(data_Element_Vec[index].whats_This == MEASURED)
			{
				arg_Type = qobject_cast<Target_Curve*>(data_Element_Vec[index].the_Class)->curve.measurement_Type;
				first_Name = "calc_measured";
			} else
			if(data_Element_Vec[index].whats_This == OPTIMIZE)
			{
				arg_Type = qobject_cast<Target_Curve*>(data_Element_Vec[index].the_Class)->curve.measurement_Type;
				first_Name = "calc_target";
			}

			print_Reflect_To_File(data_Element_Vec[index].active_Whats_This, measurement_Iter.node->data.measurement, data_Element_Vec[index].unwrapped_Reflection, first_Name, index);
		}
	}
}
template void Calculation_Tree::calculate_Intermediate_Values_For_1_Kind<Independent_Variables>(QVector<Data_Element<Independent_Variables>>&);
template void Calculation_Tree::calculate_Intermediate_Values_For_1_Kind<Target_Curve>		   (QVector<Data_Element<Target_Curve>>&);


void Calculation_Tree::calculate_Intermediate_Values()
{
	calculate_Intermediate_Values_For_1_Kind(independent);
	calculate_Intermediate_Values_For_1_Kind(measured);
	calculate_Intermediate_Values_For_1_Kind(target);
}

void Calculation_Tree::calculate_Intermediate_Values_1_Tree(const tree<Node>::iterator& parent, const tree<Node>::iterator& active_Iter, QString active_Whats_This, tree<Node>& calc_Tree, Node* above_Node)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

		QString warning_Text;

		int status = child.node->data.calculate_Intermediate_Points(active_Iter, above_Node, active_Whats_This, warning_Text, depth_Grading, sigma_Grading);
		if(status!=0)
		{
			emit warning(warning_Text);
			return;
		}
		if(whats_This_List[0] != whats_This_Multilayer)
		{
			above_Node = &child.node->data;
		} else
		{
			calculate_Intermediate_Values_1_Tree(child, active_Iter, active_Whats_This, calc_Tree, above_Node);
		}
	}
}

tree<Node>::iterator Calculation_Tree::find_Node(const tree<Node>::iterator& parent, QString active_Whats_This, tree<Node>& calc_Tree)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(child.node->data.whats_This == active_Whats_This)
		{
			return calc_Tree.child(parent,i);
		} else
		if(whats_This_List[0] == whats_This_Multilayer)
		{
			return find_Node(child, active_Whats_This, calc_Tree);
		}
	}
	// error handling
	{
		emit critical("Calculation_Tree::find_Node\nerror: node \"" + active_Whats_This + "\"not found");
		parent.node->data.whats_This = stop_Calculation;
	}
	return parent;
}

void Calculation_Tree::calculate_Unwrapped_Structure(const tree<Node>::iterator& active_Iter, QString active_Whats_This, tree<Node>& calc_Tree, Unwrapped_Structure* unwrapped_Structure_Vec_Element)
{
	num_Media = get_Total_Num_Layers(calc_Tree.begin(), calc_Tree);
	Unwrapped_Structure*   new_Unwrapped_Structure  = new Unwrapped_Structure (&calc_Tree, active_Iter, active_Whats_This, num_Media, max_Depth, depth_Grading, sigma_Grading, r);
	unwrapped_Structure_Vec_Element = new_Unwrapped_Structure;
}

void Calculation_Tree::calculate_Unwrapped_Reflectivity(QString active_Whats_This, Measurement& measurement, Unwrapped_Structure* unwrapped_Structure_Vec_Element, Unwrapped_Reflection* unwrapped_Reflection_Vec_Element)
{
	Unwrapped_Reflection*  new_Unwrapped_Reflection = new Unwrapped_Reflection(unwrapped_Structure_Vec_Element, num_Media, active_Whats_This, measurement, depth_Grading, sigma_Grading);
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

void Calculation_Tree::print_Reflect_To_File(QString arg_Type, Measurement& measurement, Unwrapped_Reflection* unwrapped_Reflection_Vec_Element, QString first_Name, int index)
{
	int num_Points = 0;
	QVector<double> arg;

	if(arg_Type == measurement_Types[0])
	{
		num_Points = measurement.angle.size();
		arg = measurement.angle;
	}
	if(arg_Type == measurement_Types[1])
	{
		num_Points = measurement.lambda.size();
		arg = measurement.lambda;
	}

	QString name = first_Name+"_"+QString::number(index)+".txt";
	QFile file(name);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		for(auto i=0; i<num_Points; ++i)
		{
			out << "\t" << QString::number(arg[i],'f',4)
				<< "\t" << QString::number(unwrapped_Reflection_Vec_Element->R[i],'e',6)
				<< endl;
		}
	file.close();
	} else{QTextStream(stderr) << "Calculation_Tree::print_Reflect_To_File  :  Can't write file " + name << endl;	exit(EXIT_FAILURE);}

}

int Calculation_Tree::get_Total_Num_Layers(const tree<Node>::iterator& parent, tree<Node>& calc_Tree)
{
	int num_Media_Local = 0;
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient ||
		   whats_This_List[0] == whats_This_Layer   ||
		   whats_This_List[0] == whats_This_Substrate )
		{
			num_Media_Local += 1;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			num_Media_Local += child.node->data.stack_Content.num_Repetition.value * get_Total_Num_Layers(child, calc_Tree);
		}
	}
	return num_Media_Local;
}

void Calculation_Tree::print_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This_List;

		{
			std::cout << "node :  depth : " << calc_Tree.depth(child) << "   ";
			for(int y=0; y<calc_Tree.depth(child)-1; y++)
			{	std::cout << "\t";}
			std::cout << child.node->data.whats_This.toStdString();
			if(whats_This_List[0] == whats_This_Multilayer)
			{	std::cout << " : " << child.node->data.stack_Content.num_Repetition.value;	}
			std::cout << std::endl;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			print_Tree(child, calc_Tree);
		}
	}
}

void Calculation_Tree::print_Flat_list(QList<Node> flat_List)
{
	for(int i=0; i<flat_List.size(); ++i)
	{
		std::cout << "list : \t\t" << flat_List[i].whats_This.toStdString() << std::endl;
	}
}

void Calculation_Tree::print_Item_Tree(QTreeWidgetItem* item)
{
	for(int i=0; i<item->childCount(); ++i)
	{
		QTreeWidgetItem* child = item->child(i);
		QStringList whats_This_List = child->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);

		{
			std::cout << "item :  depth : " << Global_Variables::get_Item_Depth(child) << "   ";
			for(int y=0; y<Global_Variables::get_Item_Depth(child)-1; y++)
			{	std::cout << "\t";}
			std::cout << child->whatsThis(DEFAULT_COLUMN).toStdString();
			if(whats_This_List[0] == whats_This_Multilayer)
			{	std::cout << " : " << child->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().num_Repetition.value;	}
			std::cout << std::endl;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			print_Item_Tree(child);
		}
	}
}
