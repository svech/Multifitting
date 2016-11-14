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
	Q_INVOKABLE void refreshOverStruct() { Structure_Tree::refresh_Over_Struct(); }
public:
	explicit Structure_Tree(Multilayer* multilayer, QWidget *parent = 0);

signals:
	void refresh();
private slots:
	void emit_Refresh();

private:
	void create_Tree();
	void create_Toolbar();

	void refresh_Over_Struct();
		void iterate_Over_Struct(QTreeWidgetItem* item = NULL);
			void refresh_If_Layer(QTreeWidgetItem* this_Item);
		void iterate_Over_Multilayers(QTreeWidgetItem* item = NULL);
			void refresh_If_Multilayer(QTreeWidgetItem* this_Item);
				void find_First_Layer(QTreeWidgetItem* this_Item, int& first);
				void find_Last_Layer(QTreeWidgetItem* this_Item, int& last);
				void find_Period(QTreeWidgetItem* this_Item);

public slots:
	void if_DoubleClicked(QTreeWidgetItem*, int);	
	void set_Structure_Item_Text(QTreeWidgetItem* item);
	void editor_Close();
	void editors_Edit();
	void editors_Refresh();
	void multiple_Refresh_Over_Struct();
	void refresh_Text();

private:
	int different_Layers_Counter=0;
	Multilayer* multilayer;

	QMap<QTreeWidgetItem*, Item_Editor*> runned_Editors;
	QList<Item_Editor*> list_Editors;

public:	
	Structure_Toolbar* structure_Toolbar;
	QTreeWidget* tree;
};

#endif // STRUCTURE_TREE_H
