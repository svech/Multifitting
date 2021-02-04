#ifndef CALCULATION_TREE_H
#define CALCULATION_TREE_H

#include "global/layer_data_types.h"
#include "node.h"
#include "unwrapped/unwrapped_reflection.h"
#include "unwrapped/unwrapped_structure.h"

class Multilayer;

using namespace std;

template <class Type>
struct Data_Element
{
	~Data_Element()
	{
		delete unwrapped_Reflection;
		delete unwrapped_Structure;
	}
	Unwrapped_Reflection* unwrapped_Reflection;
	Unwrapped_Structure*  unwrapped_Structure;

	QString curve_Class;			// INDEPENDENT or TARGET
	Type* the_Class;
	Calc_Functions calc_Functions;
	tree<Node> calc_Tree; // each tree creates somehow 2 nodes at creation
	vector<Node*> flat_Calc_Tree;	            // contains only Ambient, Layers, Substrate
	vector<Node*> short_Flat_Calc_Tree;	        // contains only Layers, Substrate, and no extreme layers
	vector<Node*> media_Node_Map_Vector;
	vector<Data*> media_Data_Map_Vector;		// pointers to real_Calc_Tree, without calculated epsilon, hi etc
	vector<int>   media_Period_Index_Map_Vector;
};

class Calculation_Tree
{
public:
	Calculation_Tree(Multilayer* multilayer, QString calc_Mode);
	~Calculation_Tree();

	void prepare_Residual_Expressions();
	void create_Rand_Generator();
	void check_If_Graded();

	static void fill_Tree_From_Scratch(tree<Node>& calc_Tree, QTreeWidget* item_Tree, Multilayer* multilayer);
	static void fill_Calc_Tree_From_Item_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree, QTreeWidgetItem* item);
	void fill_Calc_Trees();

	static int get_Total_Num_Layers(const tree<Node>::iterator& parent);

	void renew_Item_Tree_From_Calc_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree, QTreeWidgetItem* item);

	void stratify_Calc_Tree_Iteration(const tree<Node>::iterator& parent, int depth, QVector<tree<Node>::iterator>& chosen_Nodes);
	void stratify_Calc_Tree(tree<Node>& calc_Tree);
	void flatten_Tree(const tree<Node>::iterator& parent, const tree<Node>& calc_Tree, vector<Node*>& flat_Calc_Tree);
	void short_Tree(const vector<Node *> &flat_Calc_Tree, vector<Node*>& short_Flat_Calc_Tree);
	int unwrap_Calc_Tree_Node(const tree<Node>::iterator& parent, vector<Node*>& media_Node_Map_Vector, int media_Index = 0);
	static int unwrap_Calc_Tree_Data(const tree<Node>::iterator& parent, vector<Data*>& media_Data_Map_Vector, vector<int>& media_Period_Index_Map_Vector, int media_Index = 0, int period_Index = 0);

	template <typename Type>
	void calculate_1_Kind_Preliminary(Data_Element<Type>& data_Element);

	template <typename Type>
	void calculate_1_Kind(Data_Element<Type>& data_Element, QString mode = SPECULAR_MODE);
	template <typename Type>
	void calculate_1_Curve(Data_Element<Type>& data_Element);

	void calculate_Intermediate_Values_1_Tree(vector<Node*>& flat_Calc_Tree, vector<Node*>& short_Flat_Calc_Tree, const Data& measurement, QString mode);
	void clear_Spline_1_Tree(vector<Node*>& short_Flat_Calc_Tree, const Data& measurement, QString mode);

	void calculate_Unwrapped_Structure   (const Calc_Functions& calc_Functions, const vector<Node*>& media_Node_Map_Vector, const vector<Data*>& media_Data_Map_Vector, const vector<int>& media_Period_Index_Map_Vector, const Data& measurement, Unwrapped_Structure*& unwrapped_Structure_Vec_Element);
	void calculate_Unwrapped_Reflectivity(const vector<Node*>& short_Flat_Calc_Tree, Calculated_Values& calculated_Values, Unwrapped_Structure*  unwrapped_Structure, Unwrapped_Reflection*& unwrapped_Reflection_Vec_Element, QString mode);

	static void print_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree);
	static void print_Flat_Tree(const vector<Node*>& flat_Tree);

	Multilayer* multilayer;
	gsl_rng* r;

	QTreeWidget* real_Struct_Tree;
	tree<Node>   real_Calc_Tree; // common preliminary tree for TARGET calculations for // each tree creates somehow 2 nodes at creation

	bool depth_Grading = false;
	bool sigma_Grading = false;

	int num_Media_Sharp;

	QVector<Data_Element<Independent_Curve>> independent;
	QVector<Data_Element<Target_Curve>>			 target;
};

#endif // CALCULATION_TREE_H
