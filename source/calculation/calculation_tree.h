#ifndef CALCULATION_TREE_H
#define CALCULATION_TREE_H

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

class Multilayer;

class Calculation_Tree
{
public:
	Calculation_Tree(Multilayer* multilayer, QString calc_Mode);
	~Calculation_Tree();

public:
	void prepare_Residual_Expressions();
	void create_Rand_Generator();
	void check_If_Graded();

	void fill_Tree_From_Scratch(tree<Node>& calc_Tree, QTreeWidget* item_Tree, QString curve_Class);
	void fill_Calc_Tree_From_Item_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree, QTreeWidgetItem* item);
	void fill_Independent_Calc_Trees();
	void fill_Target_Calc_Trees();

	void renew_Item_Tree_From_Calc_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree, QTreeWidgetItem* item);

//	void statify_Item_Tree();
	void statify_Calc_Tree_Iteration(const tree<Node>::iterator& parent, int depth, QVector<tree<Node>::iterator>& chosen_Nodes);
	void statify_Calc_Tree(tree<Node>& calc_Tree);

	template <typename Type>
	void calculate_1_Kind(Data_Element<Type>& data_Element);

	void calculate_Intermediate_Values_1_Tree(tree<Node>& calc_Tree, const Data& measurement, QString active_Parameter_Whats_This, const tree<Node>::iterator& parent, Node* above_Node = NULL);
	tree<Node>::iterator find_Node_By_Item_Id(const tree<Node>::iterator& parent, id_Type active_Item_Id, tree<Node>& calc_Tree);

	void calculate_Unwrapped_Structure(tree<Node>& calc_Tree,                   const Data& measurement, QString active_Parameter_Whats_This, Unwrapped_Structure*& unwrapped_Structure_Vec_Element);
	void calculate_Unwrapped_Reflectivity(Calculated_Values& calculated_Values, const Data& measurement, QString active_Parameter_Whats_This, Unwrapped_Structure*  unwrapped_Structure_Vec_Element, Unwrapped_Reflection*& unwrapped_Reflection_Vec_Element);

	int get_Total_Num_Layers(const tree<Node>::iterator& parent, const tree<Node>& calc_Tree);

	void print_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree);
//	void print_Flat_list(QList<Node> flat_List);
//	void print_Item_Tree(QTreeWidgetItem* item);

	gsl_rng* r;

	QString calc_Mode;
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
