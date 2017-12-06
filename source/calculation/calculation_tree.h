#ifndef CALCULATION_TREE_H
#define CALCULATION_TREE_H

#include "tree.hh"
#include "global/layer_data_types.h"
#include "node.h"
#include "unwrapped/unwrapped_reflection.h"
#include "unwrapped/unwrapped_structure.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;

template <class Type>
struct Data_Element
{
	~Data_Element()
	{
		delete unwrapped_Reflection;
		delete unwrapped_Structure;
	}

	QString curve_Class;			// INDEPENDENT or TARGET
	Type* the_Class;

	tree<Node> calc_Tree; // each tree creates somehow 2 nodes at creation

	Unwrapped_Reflection* unwrapped_Reflection;
	Unwrapped_Structure*  unwrapped_Structure;

	// active
	QString active_Item_Type;
	QString active_Item_Id;
	QString active_Parameter_Whats_This;
};

class Calculation_Tree
{
public:
	Calculation_Tree(QTabWidget* independent_Variables_Plot_Tabs, QVector<Target_Curve*>& target_Profiles_Vector, QTreeWidget* real_Struct_Tree);

public:
	void create_Rand_Generator();
	void check_If_Graded();

	void fill_Tree_From_Scratch(tree<Node>& calc_Tree, QTreeWidget* item_Tree, QString class_Type);
	void fill_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree, QTreeWidgetItem* item);
	void fill_Independent_Calc_Trees();
	void fill_Target_Calc_Trees();

//	void statify_Item_Tree();
	void statify_Calc_Tree_Iteration(const tree<Node>::iterator& parent, int depth, QVector<tree<Node>::iterator>& chosen_Iters);
	void statify_Calc_Tree(tree<Node>& calc_Tree);

	template <typename Type>
	void calculate_1_Kind(QVector<Data_Element<Type>>& data_Element_Vec);
	void calculate();

	void calculate_Intermediate_Values_1_Tree(tree<Node>& calc_Tree, const Data& measurement, QString active_Parameter_Whats_This, const tree<Node>::iterator& parent, Node* above_Node = NULL);
	tree<Node>::iterator find_Node_By_Item_Id(const tree<Node>::iterator& parent, int active_Item_Id, tree<Node>& calc_Tree);

	void calculate_Unwrapped_Structure(tree<Node>& calc_Tree, const Data& measurement, QString active_Parameter_Whats_This, Unwrapped_Structure*& unwrapped_Structure_Vec_Element);
	void calculate_Unwrapped_Reflectivity(const Data& measurement, QString active_Parameter_Whats_This, Unwrapped_Structure* unwrapped_Structure_Vec_Element, Unwrapped_Reflection*& unwrapped_Reflection_Vec_Element);

	int get_Total_Num_Layers(const tree<Node>::iterator& parent, const tree<Node>& calc_Tree);

	void print_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree);
	void print_Flat_list(QList<Node> flat_List);
	void print_Item_Tree(QTreeWidgetItem* item);

	gsl_rng* r;

	QTreeWidget* real_Struct_Tree;
	tree<Node>   real_Calc_Tree; // common preliminary tree for TARGET calculations for // each tree creates somehow 2 nodes at creation

	bool depth_Grading = false;
	bool sigma_Grading = false;

	int max_Depth;
	int num_Media;

	QVector<Data_Element<Independent_Variables>> independent;
	QVector<Data_Element<Target_Curve>>			 target;
};

#endif // CALCULATION_TREE_H
