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

class Calculation_Tree: public QObject
{
	Q_OBJECT
public:
	Calculation_Tree(QTabWidget* independent_Variables_Plot_Tabs);
	~Calculation_Tree();
signals:
	void critical   (QString critical_Text);
	void warning    (QString warning_Text);
	void information(QString information_Text);

public:
	void run_All();
private:
	void create_Rand_Generator();
	void create_Local_Item_Tree();
	void check_If_Graded();

	int tree_Depth(QTreeWidgetItem* item);
	void fill_Calc_Trees();
    void fill_Tree(const tree<Node>::iterator& parent, QTreeWidgetItem* item, int independent_Index);
//	void statify_Item_Tree();
    void statify_Calc_Tree_Iteration(const tree<Node>::iterator& parent, int depth, QVector<tree<Node>::iterator>& chosen_Iters);
	void statify_Calc_Tree(tree<Node>& calc_Tree);

	void calculate_Intermediate_Values();
	void calculate_Intermediate_Values_1_Tree(const tree<Node>::iterator& parent, const tree<Node>::iterator& active_Iter, QString active_Whats_This, int independent_Index, Node* above_Node = NULL);
	tree<Node>::iterator find_Node(const tree<Node>::iterator& parent, QString active_Whats_This, int independent_Index);

	void calculate_Unwrapped_Structure(const tree<Node>::iterator& active_Iter, QString active_Whats_This, int independent_Index);
	void calculate_Unwrapped_Reflectivity(int independent_Index);

	int get_Total_Num_Layers(const tree<Node>::iterator& parent, int independent_Index);
	int get_Item_Depth(QTreeWidgetItem* item);

	void print_Reflect_To_File(const Measurement& measurement, QString active_Whats_This, int independent_Index);
	void print_Tree(const tree<Node>::iterator& parent, int independent_Index);
	void print_Flat_list(QList<Node> flat_List);
	void print_Item_Tree(QTreeWidgetItem* item);

public:
	gsl_rng * r;

	bool depth_Grading = false;
	bool sigma_Grading = false;

	int max_Depth;
	int num_Media;
	int num_Independent;
	QVector<QTreeWidget*> local_Item_Tree_Vec;

	QTabWidget* independent_Variables_Plot_Tabs;
	QVector<Independent_Variables*> independent_Widget_Vec;

	QVector<tree<Node>> calc_Tree_Vec;
	QVector<Measurement> measurement_Vec;
	QVector<QString> active_Whats_This_Vec;
	QVector<Unwrapped_Reflection*> unwrapped_Reflection_Vec;
	QVector<Unwrapped_Structure*>  unwrapped_Structure_Vec;
};

#endif // CALCULATION_TREE_H
