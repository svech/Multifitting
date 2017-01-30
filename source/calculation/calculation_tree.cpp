// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "calculation_tree.h"
#include <iostream>

Calculation_Tree::Calculation_Tree(QVector<Independent_Variables*>& independent_Widget_Vec):
	independent_Widget_Vec(independent_Widget_Vec),
	unwrapped_Reflection_Vec(independent_Widget_Vec.size()),
	unwrapped_Structure_Vec(independent_Widget_Vec.size()),
	calc_Tree_Vec(independent_Widget_Vec.size()),
	measurement_Vec(independent_Widget_Vec.size()),
	active_Whats_This_Vec(independent_Widget_Vec.size())
{
}

Calculation_Tree::~Calculation_Tree()
{
	for(int i=0; i<independent_Widget_Vec.size(); ++i)
	{
		delete unwrapped_Structure_Vec[i];
		delete unwrapped_Reflection_Vec[i];
	}
}

void Calculation_Tree::run_All()
{
	create_Local_Item_Tree();
	if(local_Item_Tree_Vec.size()>0)
	{
		int independent_Index = 0;
		max_Depth = tree_Depth(local_Item_Tree_Vec[independent_Index]->invisibleRootItem());	// unstratified depth
		fill_Calc_Trees();
		calculate_Intermediate_Values();
	}
}

void Calculation_Tree::create_Local_Item_Tree()
{
	// TODO delete local_Item_Tree at the end
	for(int independent_Index=0; independent_Index<independent_Widget_Vec.size(); ++independent_Index)
	{
		QTreeWidget*local_Item_Tree = new QTreeWidget;
		for(int i=0; i<independent_Widget_Vec[independent_Index]->struct_Tree_Copy->topLevelItemCount(); ++i)
		{
			local_Item_Tree->addTopLevelItem(independent_Widget_Vec[independent_Index]->struct_Tree_Copy->topLevelItem(i)->clone());
		}
		local_Item_Tree_Vec.append(local_Item_Tree);
	}
}

int  Calculation_Tree::get_Item_Depth(QTreeWidgetItem* item)
{
	int depth = 0;
	while(item!=NULL)
	{
	  item = item->parent();
	  ++depth;
	}
	return depth;
}

void Calculation_Tree::fill_Calc_Trees()
{
	// for each plot
	for(int independent_Index=0; independent_Index<local_Item_Tree_Vec.size(); ++independent_Index)
	{
		fill_Tree(calc_Tree_Vec[independent_Index].begin(), local_Item_Tree_Vec[independent_Index]->invisibleRootItem(), independent_Index);

		// if no set substrate then the second ambient
		if(local_Item_Tree_Vec[independent_Index]->topLevelItem(local_Item_Tree_Vec[independent_Index]->topLevelItemCount()-1)->whatsThis(DEFAULT_COLUMN) != whats_This_Substrate)
		{
			QTreeWidgetItem substrate_Item;
			substrate_Item.setWhatsThis(DEFAULT_COLUMN, whats_This_Substrate);

			Ambient ambient = local_Item_Tree_Vec[independent_Index]->topLevelItem(1)->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
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
			calc_Tree_Vec[independent_Index].append_child(calc_Tree_Vec[independent_Index].begin(), substrate_Node);
		}
	}
}

void Calculation_Tree::fill_Tree(const tree<Node>::iterator& parent, QTreeWidgetItem* item, int independent_Index)
{
	for(int i=0; i<item->childCount(); ++i)
	{
		Node temp_Node(item->child(i));
		calc_Tree_Vec[independent_Index].append_child(parent, temp_Node);

		if(item->child(i)->childCount()>0)
		{
			fill_Tree(calc_Tree_Vec[independent_Index].child(parent,i), item->child(i), independent_Index);
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

		/// stratification
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

void Calculation_Tree::calculate_Intermediate_Values()
{
	// for each plot
	for(int independent_Index=0; independent_Index<calc_Tree_Vec.size(); ++independent_Index)
	{
		// find whatsThis of active item
		for(int item_Index=0; item_Index<independent_Widget_Vec[independent_Index]->independent_Variables_List->count(); ++item_Index)
		{
			QListWidgetItem* list_Item = independent_Widget_Vec[independent_Index]->independent_Variables_List->item(item_Index);

			// if active
			if(list_Item->data(Qt::UserRole).toBool())
			{
				active_Whats_This_Vec[independent_Index] = list_Item->whatsThis();
			}
		}

		// preliminary "measurement" calculation for each tree
		tree<Node>::iterator measurement_Iter = find_Node(calc_Tree_Vec[independent_Index].begin(), whats_This_Measurement, independent_Index);

		// ....................................................................
		// emergency case
		if (measurement_Iter.node->data.whats_This == stop_Calculation) return;
		// ....................................................................

		// calculation of wavenumbers and cos squares
		measurement_Iter.node->data.measurement.calc_Independent_cos2_k();
		measurement_Vec[independent_Index] = (measurement_Iter.node->data.measurement);

		// find corresponding node for active variable
		QStringList active_Whats_This_List = active_Whats_This_Vec[independent_Index].split(whats_This_Delimiter,QString::SkipEmptyParts);
		tree<Node>::iterator active_Iter = find_Node(calc_Tree_Vec[independent_Index].begin(), active_Whats_This_List[0], independent_Index);

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
			auto start = std::chrono::system_clock::now();
			statify_Calc_Tree(calc_Tree_Vec[independent_Index]);
			calculate_Intermediate_Values_1_Tree(calc_Tree_Vec[independent_Index].begin(), active_Iter, active_Whats_This_Vec[independent_Index], independent_Index);
			auto end = std::chrono::system_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			qInfo() << "Intermediate: "<< elapsed.count()/1000000. << " seconds" << endl;

			start = std::chrono::system_clock::now();
			calculate_Unwrapped_Structure(independent_Index);
			end = std::chrono::system_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			qInfo() << "Unwrap: "<< elapsed.count()/1000000. << " seconds" << endl;

			start = std::chrono::system_clock::now();
			calculate_Unwrapped_Reflectivity(independent_Index);
			end = std::chrono::system_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			qInfo() << "Unwrap Reflect: "<< elapsed.count()/1000000. << " seconds" << endl;
		}
	}
}

void Calculation_Tree::calculate_Intermediate_Values_1_Tree(const tree<Node>::iterator& parent, tree<Node>::iterator& active_Iter, QString active_Whats_This, int independent_Index, Node* above_Node)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree_Vec[independent_Index].child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

		QString warning_Text;

		int status = child.node->data.calculate_Intermediate_Points(active_Iter, above_Node, active_Whats_This, warning_Text);
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
			calculate_Intermediate_Values_1_Tree(child, active_Iter, active_Whats_This, independent_Index, above_Node);
		}
	}
}

tree<Node>::iterator Calculation_Tree::find_Node(const tree<Node>::iterator& parent, QString active_Whats_This, int independent_Index)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree_Vec[independent_Index].child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(child.node->data.whats_This == active_Whats_This)
		{
			return calc_Tree_Vec[independent_Index].child(parent,i);
		} else
		if(whats_This_List[0] == whats_This_Multilayer)
		{
			return find_Node(child, active_Whats_This, independent_Index);
		}
	}
	// error handling
	{
		emit critical("Calculation_Tree::find_Node\nerror: node \"" + active_Whats_This + "\"not found");
		parent.node->data.whats_This = stop_Calculation;
	}
	return parent;
}

void Calculation_Tree::calculate_Unwrapped_Structure(int independent_Index)
{
	num_Media = get_Total_Num_Layers(calc_Tree_Vec[independent_Index].begin(), independent_Index);
	Unwrapped_Structure*   new_Unwrapped_Structure  = new Unwrapped_Structure (&calc_Tree_Vec[independent_Index], num_Media, max_Depth);
	unwrapped_Structure_Vec [independent_Index] = new_Unwrapped_Structure;
}

void Calculation_Tree::calculate_Unwrapped_Reflectivity(int independent_Index)
{
	Unwrapped_Reflection*  new_Unwrapped_Reflection = new Unwrapped_Reflection(unwrapped_Structure_Vec[independent_Index], num_Media, active_Whats_This_Vec[independent_Index], measurement_Vec[independent_Index]);
	unwrapped_Reflection_Vec[independent_Index] = new_Unwrapped_Reflection;

	auto start = std::chrono::system_clock::now();
	unwrapped_Reflection_Vec[independent_Index]->calc_Reflectivity();
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	qInfo() << "Bare Reflectivity:      "<< elapsed.count()/1000. << " seconds" << endl;

	cout << "r_s     [" << 0 << "] = " << unwrapped_Reflection_Vec[independent_Index]->r_s[0] << endl;
	cout << "r_p     [" << 0 << "] = " << unwrapped_Reflection_Vec[independent_Index]->r_p[0] << endl;

	cout << "R_s     [" << 0 << "] = " << unwrapped_Reflection_Vec[independent_Index]->R_s[0] << endl;
	cout << "R_p     [" << 0 << "] = " << unwrapped_Reflection_Vec[independent_Index]->R_p[0] << endl;
	cout << "--------------------------------\n";
}

int Calculation_Tree::get_Total_Num_Layers(const tree<Node>::iterator& parent, int independent_Index)
{
	int num_Media_Local = 0;
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree_Vec[independent_Index].child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient ||
		   whats_This_List[0] == whats_This_Layer   ||
		   whats_This_List[0] == whats_This_Substrate )
		{
			num_Media_Local += 1;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			num_Media_Local += child.node->data.stack_Content.num_Repetition.value * get_Total_Num_Layers(child, independent_Index);
		}
	}
	return num_Media_Local;
}

int  Calculation_Tree::tree_Depth(QTreeWidgetItem* item)
{
	int depth = 0;
	QVector<int> child_Depth;
	if(item->childCount()>0)
	{
		for(int item_Child_Index=0; item_Child_Index<item->childCount(); item_Child_Index++)
		{
			child_Depth.append(tree_Depth(item->child(item_Child_Index)));
		}
		depth=1+*(std::minmax_element(child_Depth.begin(),child_Depth.end()).second);
	}
	return depth;
}

void Calculation_Tree::print_Tree(const tree<Node>::iterator& parent, int independent_Index)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree_Vec[independent_Index].child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This_List;

		{
			std::cout << "node :  depth : " << calc_Tree_Vec[independent_Index].depth(child) << "   ";
			for(int y=0; y<calc_Tree_Vec[independent_Index].depth(child)-1; y++)
			{	std::cout << "\t";}
			std::cout << child.node->data.whats_This.toStdString();
			if(whats_This_List[0] == whats_This_Multilayer)
			{	std::cout << " : " << child.node->data.stack_Content.num_Repetition.value;	}
			std::cout << std::endl;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			print_Tree(child, independent_Index);
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
			std::cout << "item :  depth : " << get_Item_Depth(child) << "   ";
			for(int y=0; y<get_Item_Depth(child)-1; y++)
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
