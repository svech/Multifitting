#include "calculation_tree.h"
#include <iostream>

Calculation_Tree::Calculation_Tree(QVector<Independent_Variables*>& independent_Widget_Vec):
	calc_Tree_Vec(independent_Widget_Vec.size())
{
	create_Local_Item_Tree(independent_Widget_Vec);
	if(local_Item_Tree_Vec.size()>0)
	{
		int independent_Index = 0;
		max_Depth = tree_Depth(local_Item_Tree_Vec[independent_Index]->invisibleRootItem());	// unstratified depth

		fill_Calc_Trees();

		//------------------------
		statify_Item_Tree();
		statify_Calc_Tree();
		//------------------------


//		calculate_Intermediate_Values(independent_Widget_Vec);


//		qInfo() << "\ndepth = " << max_Depth;
		qInfo() << endl;
		print_Tree(calc_Tree_Vec[independent_Index].begin(), independent_Index);
		qInfo() << endl;
		print_Item_Tree(local_Item_Tree_Vec[independent_Index]->invisibleRootItem());
	}
}

void Calculation_Tree::create_Local_Item_Tree(QVector<Independent_Variables*>& independent_Widget_Vec)
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

void Calculation_Tree::statify_Item_Tree()
{
	for(int independent_Index=0; independent_Index<local_Item_Tree_Vec.size(); ++independent_Index)
	{
		QTreeWidget* item_Tree = local_Item_Tree_Vec[independent_Index];
		for(int depth=max_Depth-1; depth>0; depth--)
		{
			QVector<QTreeWidgetItem*> chosen_Items;

			// iterate over fixed depth
			QTreeWidgetItemIterator fix_Depth_Iter(item_Tree);
			while (*fix_Depth_Iter)
			{
				QTreeWidgetItem* item = *fix_Depth_Iter;
				if(get_Item_Depth(item) == depth)
				{
					QStringList whats_This_List = item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);

					if(whats_This_List[0] == whats_This_Multilayer)
					{
						chosen_Items.append(item);
					}
				}
				++fix_Depth_Iter;
			}

			/// stratification
			for(int vec_Index=chosen_Items.size()-1; vec_Index>=0; --vec_Index)
			{
				QTreeWidgetItem* item = chosen_Items[vec_Index];

				// if 0 periods
				if(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().num_Repetition.value == 0)
				{
					// TODO delete or not delete?
//					delete item;
				} else

				// if 1 period
				if(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().num_Repetition.value == 1)
				{
					for(int child_Index=0; child_Index<item->childCount(); ++child_Index)
					{
						QTreeWidgetItem* parent_Item;
						if(item->parent()) parent_Item = item->parent(); else parent_Item = item_Tree->invisibleRootItem();
						parent_Item->insertChild(parent_Item->indexOfChild(item),item->child(child_Index)->clone());
					}
					// TODO delete or not delete?
//					delete item;
					// change data
					Stack_Content stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
					stack_Content.num_Repetition.value = 0;
					QVariant var; var.setValue(stack_Content);
					item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
				} else

				// if 2 periods
				if(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().num_Repetition.value == 2)
				{
					for(int child_Index=0; child_Index<item->childCount(); ++child_Index)
					{
						QTreeWidgetItem* parent_Item;
						if(item->parent()) parent_Item = item->parent(); else parent_Item = item_Tree->invisibleRootItem();
						parent_Item->insertChild(parent_Item->indexOfChild(item)  ,item->child(child_Index)->clone());
						parent_Item->insertChild(parent_Item->indexOfChild(item)+1+child_Index,item->child(child_Index)->clone());
					}
					// TODO delete or not delete?
//					delete item;
				} else

				// if >=3 periods
				if(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().num_Repetition.value >= 3)
				{
					for(int child_Index=0; child_Index<item->childCount(); ++child_Index)
					{
						QTreeWidgetItem* parent_Item;
						if(item->parent()) parent_Item = item->parent(); else parent_Item = item_Tree->invisibleRootItem();
						parent_Item->insertChild(parent_Item->indexOfChild(item)  ,item->child(child_Index)->clone());
						parent_Item->insertChild(parent_Item->indexOfChild(item)+1+child_Index,item->child(child_Index)->clone());
					}
					// change data
					Stack_Content stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
					stack_Content.num_Repetition.value -= 2;
					QVariant var; var.setValue(stack_Content);
					item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
				}
			}
		}
	}
}

void Calculation_Tree::statify_Calc_Tree_Iteration(tree<Node>::iterator parent, int depth, QVector<tree<Node>::iterator>& chosen_Nodes)
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

void Calculation_Tree::statify_Calc_Tree()
{
	for(int independent_Index=0; independent_Index<calc_Tree_Vec.size(); ++independent_Index)
	{
//		tree<Node> calc_Tree = calc_Tree_Vec[independent_Index];
		for(int depth=max_Depth-1; depth>0; depth--)
		{
			QVector<tree<Node>::iterator> chosen_Nodes;

			statify_Calc_Tree_Iteration(calc_Tree_Vec[independent_Index].begin(), depth, chosen_Nodes);

			/// stratification
			for(int vec_Index=chosen_Nodes.size()-1; vec_Index>=0; --vec_Index)
			{
				tree<Node>::iterator chosen_Child = chosen_Nodes[vec_Index];

				// if 0 periods
				if(chosen_Child.node->data.stack_Content.num_Repetition.value == 0)
				{
					// TODO delete or not delete?
//					delete item;
				} else

				// if 1 period
				if(chosen_Child.node->data.stack_Content.num_Repetition.value == 1)
				{
					for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
					{
						calc_Tree_Vec[independent_Index].insert_subtree(chosen_Child, tree<Node>::child(chosen_Child,child_Index));
					}
					// TODO delete or not delete?
//					delete item;
					// change data
					chosen_Child.node->data.stack_Content.num_Repetition.value = 0;
				} else

				// if 2 periods
				if(chosen_Child.node->data.stack_Content.num_Repetition.value == 2)
				{
					tree<Node>::iterator next = chosen_Child;
					for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
					{
						calc_Tree_Vec[independent_Index].insert_subtree      (chosen_Child, tree<Node>::child(chosen_Child,child_Index));
						next = calc_Tree_Vec[independent_Index].insert_subtree_after(next, tree<Node>::child(chosen_Child,child_Index));
					}
					// TODO delete or not delete?
//					delete item;
					// change data
					chosen_Child.node->data.stack_Content.num_Repetition.value = 0;
				} else

				// if >=3 periods
				if(chosen_Child.node->data.stack_Content.num_Repetition.value >= 3)
				{
					tree<Node>::iterator next = chosen_Child;
					for(unsigned child_Index=0; child_Index<chosen_Child.number_of_children(); ++child_Index)
					{
						calc_Tree_Vec[independent_Index].insert_subtree     (chosen_Child, tree<Node>::child(chosen_Child,child_Index));
						next = calc_Tree_Vec[independent_Index].insert_subtree_after(next, tree<Node>::child(chosen_Child,child_Index));
					}
					// change data
					chosen_Child.node->data.stack_Content.num_Repetition.value -= 2;
				}
			}
		}
	}
}

void Calculation_Tree::fill_Calc_Trees()
{
	// for each plot
	for(int independent_Index=0; independent_Index<local_Item_Tree_Vec.size(); ++independent_Index)
	{
		fill_Tree(calc_Tree_Vec[independent_Index].begin(), local_Item_Tree_Vec[independent_Index]->invisibleRootItem(), independent_Index);
	}
}

void Calculation_Tree::fill_Tree(tree<Node>::iterator parent, QTreeWidgetItem* item, int independent_Index)
{
	for(int i=0; i<item->childCount(); ++i)
	{
		Node temp_Node(item->child(i));
		calc_Tree_Vec[independent_Index].append_child(parent, temp_Node);
		calc_Tree_Vec[independent_Index].child(parent,i).node->data.this_Iter = calc_Tree_Vec[independent_Index].child(parent,i);

		if(item->child(i)->childCount()>0)
		{
			fill_Tree(calc_Tree_Vec[independent_Index].child(parent,i), item->child(i), independent_Index);
		}
	}
}

void Calculation_Tree::calculate_Intermediate_Values(QVector<Independent_Variables*>& independent_Widget_Vec)
{
	// for each plot
	for(int independent_Index=0; independent_Index<calc_Tree_Vec.size(); ++independent_Index)
	{
		QString	active_Whats_This;
		// find whatsThis of active item
		for(int item_Index=0; item_Index<independent_Widget_Vec[independent_Index]->independent_Variables_List->count(); ++item_Index)
		{
			QListWidgetItem* list_Item = independent_Widget_Vec[independent_Index]->independent_Variables_List->item(item_Index);

			// if active
			if(list_Item->data(Qt::UserRole).toBool())
			{
				active_Whats_This = list_Item->whatsThis();
			}
		}

		// find corresponding node for active variable
		QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

		tree<Node>::iterator active_Iter = find_Node(calc_Tree_Vec[independent_Index].begin(), active_Whats_This_List[0], independent_Index);

		// TODO if angle, if wavelength etc
		// if angle
		if(active_Whats_This_List[1] == whats_This_Angle)
		{
			// we know that data type == "Measurement"
			Measurement* measur = &(active_Iter.node->data.measurement);
			measur->calc_Independent_cos2_k();
		}

		calculate_Intermediate_Values_1_Tree(calc_Tree_Vec[independent_Index].begin(), active_Iter, active_Whats_This, independent_Index);
	}
}

void Calculation_Tree::calculate_Intermediate_Values_1_Tree(tree<Node>::iterator parent, tree<Node>::iterator active_Iter, QString active_Whats_This, int independent_Index)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree_Vec[independent_Index].child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

		child.node->data.calculate_Intermediate_Points(active_Iter, active_Whats_This);

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			calculate_Intermediate_Values_1_Tree(child, active_Iter, active_Whats_This, independent_Index);
		}
	}
}

tree<Node>::iterator Calculation_Tree::find_Node(tree<Node>::iterator parent, QString active_Whats_This, int independent_Index)
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
	// TODO potential danger?
	qInfo() << "\ntree<Node>::iterator Calculation_Tree::find_Node error\n";
	exit(1);
	return NULL;
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

void Calculation_Tree::print_Tree(tree<Node>::iterator parent, int independent_Index)
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
