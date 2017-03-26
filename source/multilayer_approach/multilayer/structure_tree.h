#ifndef STRUCTURE_TREE_H
#define STRUCTURE_TREE_H

#include "global/layer_data_types.h"
#include "structure_tree/item_editor.h"
#include "structure_tree/structure_toolbar.h"
#include "multilayer_approach/multilayer.h"

class Multilayer;
class Structure_Toolbar;

class Structure_Tree : public QWidget
{
	Q_OBJECT
//	Q_INVOKABLE void refreshStructureTreeDataAndText() { Structure_Tree::refresh__StructureTree__Data_and_Text(); }
public:
	explicit Structure_Tree(Multilayer* multilayer, QWidget *parent = 0);

private:
	void create_Tree();
	void create_Toolbar();

public slots:
	void if_DoubleClicked(QTreeWidgetItem*, int);	
	void editor_Close();
	void editors_Edit(QObject* sender = NULL);

public:
	void refresh__StructureTree__Data_and_Text();
	void set_Structure_Item_Text(QTreeWidgetItem* item);

private:
	void refresh_Layers(QTreeWidgetItem* item = NULL);
		void set_Layer_Text_And_WhatsThis(QTreeWidgetItem* item, int different_Layers_Counter);
			void refresh_If_Layer(QTreeWidgetItem* this_Item);
	void refresh_Multilayers(QTreeWidgetItem* item = NULL);
		void set_Multilayer_Text_And_WhatsThis(QTreeWidgetItem* item, int first, int last);
			void refresh_If_Multilayer(QTreeWidgetItem* this_Item);
				void find_First_Layer(QTreeWidgetItem* this_Item, int& first);
				void find_Last_Layer(QTreeWidgetItem* this_Item, int& last);
				void find_Period(QTreeWidgetItem* this_Item);

private:
	int different_Layers_Counter=0;

	QMap<QTreeWidgetItem*, Item_Editor*> runned_Editors;
	QList<Item_Editor*> list_Editors;

public:	
	Structure_Toolbar* structure_Toolbar;
	Multilayer* multilayer;
	QTreeWidget* tree;
};

#endif // STRUCTURE_TREE_H
