#ifndef STRUCTURE_TREE_H
#define STRUCTURE_TREE_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/structure_tree/item_editor.h"
#include "multilayer_approach/multilayer/structure_tree/structure_toolbar.h"
#include "multilayer_approach/multilayer/multilayer.h"

class Multilayer;
class Structure_Toolbar;
class Table_Of_Structures;
class Item_Editor;

class Structure_Tree : public QWidget
{
	Q_OBJECT
	Q_INVOKABLE void refreshStructureTreeDataAndText() { Structure_Tree::refresh__StructureTree__Data_and_Text(); }
public:
	explicit Structure_Tree(Multilayer* multilayer, QWidget *parent = 0);
	~Structure_Tree();

	void create_Tree();
	void create_Toolbar();
	void get_Total_Thickness(double& thickness, QTreeWidgetItem* item);

	void if_DoubleClicked(QTreeWidgetItem* item = nullptr);
	void editor_Close();
	void editors_Edit(QObject* sender = nullptr);
	void lock_Tree();
	void unlock_Tree();

	void refresh__StructureTree__Data_and_Text();
	void set_Structure_Item_Text(QTreeWidgetItem* item, int i=0);
	void set_Item_Parent_Type(QTreeWidgetItem* item);

	void refresh_Tree_Roughness();
	void refresh_Tree_Particles(bool refresh_Shape = false, bool refresh_Interference_Function = false, bool refresh_Geometry = false);

	void iterate_Over_Layers(QTreeWidgetItem* item = nullptr);
		void refresh_If_Layer(QTreeWidgetItem* this_Item, int i=0);
	void iterate_Over_Multilayers(QTreeWidgetItem* item = nullptr);
		void refresh_If_Multilayer(QTreeWidgetItem* this_Item);
			void find_First_Layer(QTreeWidgetItem* this_Item, int& first);
			void find_Last_Layer(QTreeWidgetItem* this_Item, int& last);
			static void find_Period_And_Gamma(QTreeWidgetItem* this_Item);

	int different_Layers_Counter=0;

	QMap<QTreeWidgetItem*, Item_Editor*> runned_Editors;
	QList<Item_Editor*> list_Editors;

	Structure_Toolbar* structure_Toolbar;
	Multilayer* multilayer;
	QTreeWidget* tree;

	Structure_Tree& operator =(const Structure_Tree& referent_Structure_Tree);
};

#endif // STRUCTURE_TREE_H
