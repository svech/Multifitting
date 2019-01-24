#ifndef STRUCTURE_TOOLBAR_H
#define STRUCTURE_TOOLBAR_H

#include "multilayer_approach/multilayer/structure_tree/structure_tree.h"

class Structure_Tree;

class Structure_Toolbar : public QWidget
{
	Q_OBJECT
public:
	explicit Structure_Toolbar(Structure_Tree* structure_Tree, QWidget *parent = 0);

signals:
	void refresh_Str_And_Independ_signal();

public:
	QTreeWidgetItem* add_Buffered_Layer(QTreeWidgetItem* new_Layer_Passed);
	void add_Ambient	 ();
	void add_Layer		 ();
	void add_Multilayer	 ();
	void add_Aperiodic	 ();
	void add_Substrate	 ();
	void edit			 ();
	void remove			 ();
	void cut			 ();
	void copy			 ();
	void paste			 ();
	void move_Up		 ();
	void move_Down		 ();
	void group			 ();
	void ungroup		 ();
	void thickness_Plot	 ();
	void sigma_Plot		 ();
	void destroy		 ();
	void export_Structure();

	int material_Width;
	int thickness_Width;
	int sigma_Width;
	int density_Width;
	QString thickness_units = "A";
	QString empty_Thickness_units = " ";
	QString absolute_Density_Units = "g/cm3";
	QString relative_Density_Units = "r.u.";
	int thickness_Precision = 3;
	int sigma_Precision = 3;
	int density_Precision = 3;

	void iterate_Over_Tree(QTextStream& out, QTreeWidgetItem* parent_Item, QString parent_Index_String = "");
	void print_Structure_Header(QTextStream& out);
	void look_Over_Tree();
	void print_Structure_Item(QTextStream& out, QTreeWidgetItem* structure_Item, QString current_Index_String, int item_Depth, int item_Index);

	bool ask_Parent_Multilayer();
	void if_Selected();
	void refresh_Toolbar();

	void create_Toolbar();
	void change_IDs_Of_Subtree(QTreeWidgetItem* item);

	QTreeWidgetItem* buffered=nullptr;
	QString buffered_Copy_Type = copy_Type_Cut;
	Structure_Tree* structure_Tree;
	QToolBar* toolbar;
};

#endif // STRUCTURE_TOOLBAR_H
