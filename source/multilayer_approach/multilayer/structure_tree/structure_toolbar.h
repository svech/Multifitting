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
	void add_Ambient	();
	void add_Layer		();
	void add_Multilayer	();
	void add_Aperiodic	();
	void add_Substrate	();
	void edit			();
	void remove			();
	void cut			();
	void copy			();
	void paste			();
	void move_Up		();
	void move_Down		();
	void group			();
	void ungroup		();
	void thickness_Plot	();
	void sigma_Plot		();
	void destroy		();

	void if_Selected();
	void refresh_Toolbar();

	void create_Toolbar();
	void change_IDs_Of_Subtree(QTreeWidgetItem* item);

	QTreeWidgetItem* buffered=NULL;
	QString buffered_Copy_Type = copy_Type_Cut;
	Structure_Tree* structure_Tree;
	QToolBar* toolbar;
};

#endif // STRUCTURE_TOOLBAR_H
