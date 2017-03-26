#ifndef STRUCTURE_TOOLBAR_H
#define STRUCTURE_TOOLBAR_H

#include "../structure_tree.h"

class Structure_Tree;

class Structure_Toolbar : public QWidget
{
	Q_OBJECT
public:
	explicit Structure_Toolbar(Structure_Tree* structure_Tree, QWidget *parent = 0);

signals:
	void refresh_Str_And_Independ_signal();

public slots:
	void add_Buffered_Layer(QTreeWidgetItem* new_Layer_Passed);
	void add_Layer		(bool);
	void add_Multilayer	(bool);
	void add_Substrate	(bool);
	void edit			(bool);
	void remove			(bool);
	void cut			(bool);
	void copy			(bool);
	void paste			(bool);
	void move_Up		(bool);
	void move_Down		(bool);
	void group			(bool);
	void ungroup		(bool);
	void thickness_Plot	(bool);
	void sigma_Plot		(bool);
	void destroy		(bool);

	void if_Selected();
	void refresh_Toolbar();

private:
	void create_Toolbar();

private:
	QTreeWidgetItem* buffered=NULL;
	Structure_Tree* structure_Tree;

public:
	QToolBar* toolbar;
};

#endif // STRUCTURE_TOOLBAR_H
