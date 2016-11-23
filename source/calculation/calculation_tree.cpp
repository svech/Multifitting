#include "calculation_tree.h"
#include <iostream>

Calculation_Tree::Calculation_Tree(QTreeWidget* item_Tree)
{
	create_Local_Item_Tree(item_Tree);

	item_Tree_Depth = tree_Depth(local_Item_Tree->invisibleRootItem());	// unstratified depth
	statify_Tree(local_Item_Tree);
	fill_Tree(calc_Tree.begin(), local_Item_Tree->invisibleRootItem());

	qInfo() << "\ndepth = " << item_Tree_Depth;
	qInfo() << endl;
	print_Tree(calc_Tree.begin());
//	qInfo() << endl;
//	print_Item_Tree(local_Item_Tree->invisibleRootItem());
}

void Calculation_Tree::create_Local_Item_Tree(QTreeWidget* item_Tree)
{
	// TODO delete local_Item_Tree at the end
	local_Item_Tree = new QTreeWidget;
	for(int i=0; i<item_Tree->topLevelItemCount(); ++i)
	{
		local_Item_Tree->addTopLevelItem(item_Tree->topLevelItem(i)->clone());
	}
}

void Calculation_Tree::fill_Tree(tree<Node>::iterator parent, QTreeWidgetItem* item)
{
	for(int i=0; i<item->childCount(); ++i)
	{
		Node temp_Node(item->child(i));
		calc_Tree.append_child(parent, temp_Node);
		if(item->child(i)->childCount()>0)
		{
			fill_Tree(calc_Tree.child(parent,i), item->child(i));
		}
	}
}

int Calculation_Tree::get_Item_Depth(QTreeWidgetItem* item)
{
	int depth = 0;
	while(item!=NULL)
	{
	  item = item->parent();
	  ++depth;
	}
	return depth;
}

void Calculation_Tree::statify_Tree(QTreeWidget* item_Tree)
{
	for(int depth=item_Tree_Depth-1; depth>0; depth--)
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
				delete item;
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
				delete item;
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
				delete item;
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

/* statify_Tree(tree<Node>::iterator parent)
void Calculation_Tree::statify_Tree(tree<Node>::iterator parent)
{
	for(int depth=item_Tree_Depth; depth>0; depth--)
//	int depth=item_Tree_Depth-1;
	{
		// get the first node of depth "depth"
		tree<Node>::iterator it(parent.begin());
		while(it!=parent.end())
		{
			if(calc_Tree.depth(it) == depth) break;
			++it;
		}

		// iterate over fixed depth
		tree<Node>::fixed_depth_iterator fd_Iter(it);

		while((fd_Iter!=it.end()) && calc_Tree.is_valid(fd_Iter))
		{
			//if (calc_Tree.is_valid(fd_Iter)&&(fd_Iter.node))
//			if (fd_Iter.node!=0)
			{
				QStringList whats_This_List = fd_Iter.node->data.whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

				if(whats_This_List[0] == whats_This_Multilayer)
				{
					// if 0 periods
					if(fd_Iter.node->data.stack_Content.num_Repetition.value == 0)
					{
						qInfo() << "0 periods";
					}

					// if 1 period
					if(fd_Iter.node->data.stack_Content.num_Repetition.value == 1)
					{
						qInfo() << "1 period";
						for(unsigned child_Index=0; child_Index<fd_Iter.number_of_children(); ++child_Index)
						{
							calc_Tree.insert(fd_Iter, calc_Tree.child(fd_Iter,child_Index).node->data);
						}
					}

					// if 2 periods
					if(fd_Iter.node->data.stack_Content.num_Repetition.value == 2)
					{
						qInfo() << "2 periods";
					}

					// if >=3 periods
					if(fd_Iter.node->data.stack_Content.num_Repetition.value >= 3)
					{
						qInfo() << ">=3 periods";
					}
				}
			}
			++fd_Iter;
		}
	}
}
*/
void Calculation_Tree::calculate_Intermediate_Values(tree<Node>::iterator parent)
{
//	for(int i=0; i<parent.number_of_children(); ++i)
//	{
//		qInfo() << "node : "<< calc_Tree.child(parent,i).node->data.whats_This;
//		if(calc_Tree.child(parent,i).number_of_children()>0)
//		{
//			calculate_Intermediate_Values(calc_Tree.child(parent,i));
//		}
//	}
}

int Calculation_Tree::tree_Depth(QTreeWidgetItem* item)
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

void Calculation_Tree::print_Tree(tree<Node>::iterator parent)
{
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::post_order_iterator child = calc_Tree.child(parent,i);
		QStringList whats_This_List = child.node->data.whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

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
			print_Tree(child);
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
