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

template <class T>
struct Data_Element
{
	QString whats_This;
	QTreeWidget* local_Item_Tree;
	T* the_Class;
	tree<Node> calc_Tree;
	Measurement measurement;
	QString active_Whats_This;

	Unwrapped_Reflection* unwrapped_Reflection;
	Unwrapped_Structure*  unwrapped_Structure;
};

class Calculation_Tree: public QObject
{
	Q_OBJECT
public:
	Calculation_Tree(QVector<Independent_Variables*>& independent_Variables_Vector, QVector<Target_Curve*>& measured_Data_Vector, QVector<Target_Curve*>& target_Profiles_Vector);
	~Calculation_Tree();
signals:
	void critical   (QString critical_Text);
	void warning    (QString warning_Text);
	void information(QString information_Text);

public:
	void run_All();
private:
	void create_Rand_Generator();

	template <typename T>
	void create_1_Kind_of_Local_Item_Tree(QVector<Data_Element<T>>& data_Element_Vec);
	void create_All_Local_Item_Trees();

	void check_If_Graded();

	template <typename T>
	void fill_1_Kind_of_Calc_Trees(QVector<Data_Element<T>>& data_Element_Vec);
	void fill_All_Calc_Trees();

	void fill_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree, QTreeWidgetItem* item);
//	void statify_Item_Tree();
    void statify_Calc_Tree_Iteration(const tree<Node>::iterator& parent, int depth, QVector<tree<Node>::iterator>& chosen_Iters);
	void statify_Calc_Tree(tree<Node>& calc_Tree);

	template <typename T>
	void calculate_Intermediate_Values_For_1_Kind(QVector<Data_Element<T>>& data_Element_Vec);
	void calculate_Intermediate_Values();

	void calculate_Intermediate_Values_1_Tree(const tree<Node>::iterator& parent, const tree<Node>::iterator& active_Iter, QString active_Whats_This, tree<Node>& calc_Tree, Node* above_Node = NULL);
	tree<Node>::iterator find_Node(const tree<Node>::iterator& parent, QString active_Whats_This, tree<Node>& calc_Tree);

	void calculate_Unwrapped_Structure(const tree<Node>::iterator& active_Iter, QString active_Whats_This, tree<Node>& calc_Tree, Unwrapped_Structure* unwrapped_Structure_Vec_Element);
	void calculate_Unwrapped_Reflectivity(QString active_Whats_This, Measurement& measurement, Unwrapped_Structure* unwrapped_Structure_Vec_Element, Unwrapped_Reflection*  unwrapped_Reflection_Vec_Element);

	int get_Total_Num_Layers(const tree<Node>::iterator& parent, tree<Node>& calc_Tree);

	void print_Reflect_To_File(QString active_Whats_This, Measurement& measurement, Unwrapped_Reflection* unwrapped_Reflection_Vec_Element, QString first_Name, int index);
	void print_Tree(const tree<Node>::iterator& parent, tree<Node>& calc_Tree);
	void print_Flat_list(QList<Node> flat_List);
	void print_Item_Tree(QTreeWidgetItem* item);

public:
	gsl_rng * r;

	bool depth_Grading = false;
	bool sigma_Grading = false;

	int max_Depth;
	int num_Media;

	QTreeWidget* one_Local_Item_Tree;

	QVector<Data_Element<Independent_Variables>> independent;
	QVector<Data_Element<Target_Curve>>			 measured;
	QVector<Data_Element<Target_Curve>>			 target;
};

#endif // CALCULATION_TREE_H
