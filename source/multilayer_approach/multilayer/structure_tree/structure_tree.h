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

	void create_Tree();
	void create_Toolbar();

	void if_DoubleClicked();
	void editor_Close();
	void editors_Edit(QObject* sender = nullptr);

	void refresh__StructureTree__Data_and_Text();
	void set_Structure_Item_Text(QTreeWidgetItem* item);
	void set_Item_Parent_Type(QTreeWidgetItem* item);

	void iterate_Over_Layers(QTreeWidgetItem* item = nullptr);
		void refresh_If_Layer(QTreeWidgetItem* this_Item);
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
};

#endif // STRUCTURE_TREE_H
