#ifndef CALCULATION_TREE_H
#define CALCULATION_TREE_H

#include "tree.hh"
#include "Matrix.h"
#include "global/layer_data_types.h"
#include "node.h"

using namespace std;
using namespace Numeric_lib;

typedef Matrix<double,2> myMatrix;

class Calculation_Tree
{
public:
	Calculation_Tree(QVector<Independent_Variables*>& independent_Widget_Vec);

private:
	void create_Local_Item_Tree(QVector<Independent_Variables*>& independent_Widget_Vec);
	int tree_Depth(QTreeWidgetItem* item);
	void statify_Item_Tree();
	void statify_Calc_Tree_Iteration(tree<Node>::iterator parent, int depth, QVector<tree<Node>::iterator>& chosen_Iters);
	void statify_Calc_Tree();

	void fill_Calc_Trees();
	void fill_Tree(tree<Node>::iterator parent, QTreeWidgetItem* item, int independent_Index);
	
	void calculate_Intermediate_Values(QVector<Independent_Variables*>& independent_Widget_Vec);
	void calculate_Intermediate_Values_1_Tree(tree<Node>::iterator parent, tree<Node>::iterator active_Iter, QString active_Whats_This, int independent_Index);
	tree<Node>::iterator find_Node(tree<Node>::iterator parent, QString active_Whats_This, int independent_Index);

	int get_Item_Depth(QTreeWidgetItem* item);

	void print_Tree(tree<Node>::iterator parent, int independent_Index);
	void print_Item_Tree(QTreeWidgetItem* item);

public:
	int max_Depth;
	QVector<QTreeWidget*> local_Item_Tree_Vec;
	QVector<tree<Node>> calc_Tree_Vec;
};

#endif // CALCULATION_TREE_H
