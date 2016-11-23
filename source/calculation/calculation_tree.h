#ifndef CALCULATION_TREE_H
#define CALCULATION_TREE_H

#include "tree.hh"
#include <vector>
#include "Matrix.h"
#include "global/layer_data_types.h"

using namespace std;
using namespace Numeric_lib;

typedef Matrix<double,2> myMatrix;

class Calculation_Tree
{
public:
	Calculation_Tree(QTreeWidget* item_Tree);

private:
	void create_Local_Item_Tree(QTreeWidget* item_Tree);
	void fill_Tree(tree<Node>::iterator parent, QTreeWidgetItem* item);
	int get_Item_Depth(QTreeWidgetItem* item);
	void statify_Tree(QTreeWidget* item_Tree);
	void calculate_Intermediate_Values(tree<Node>::iterator parent);

	int tree_Depth(QTreeWidgetItem* item);
	void print_Tree(tree<Node>::iterator parent);	
	void print_Item_Tree(QTreeWidgetItem* item);

public:
	QTreeWidget* local_Item_Tree;
	int item_Tree_Depth;
	tree<Node> calc_Tree;
	tree<Node> stratified_Tree;
};

#endif // CALCULATION_TREE_H
