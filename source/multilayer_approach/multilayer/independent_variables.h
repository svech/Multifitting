#ifndef INDEPENDENT_VARIABLES_H
#define INDEPENDENT_VARIABLES_H

#include "independent_variables/independent_variables_editor.h"
#include "variable_selection.h"

class Independent_Variables : public QWidget
{
	Q_OBJECT
public:
	explicit Independent_Variables(QTreeWidget* struct_Tree_Copy, QTreeWidget* real_Struct_Tree, QWidget *parent = 0);

signals:
	void refresh();
private slots:
	void emit_Refresh();

public:
	void clear_Structure();
	void clear_Independent_List();
	void refresh_Text();
private:
	void create_Main_Layout();
	void create_Independent_Variables_List();
	void create_Independent_Variables_Toolbar();

public slots:
	void remove_Independent_Variable(QListWidgetItem* item);

private slots:
	void add_Independent_Variable(bool);
	void cause_Editing_Independent_Variable(bool);
	void edit_Independent_Variable(QListWidgetItem* item);
	void remove_Current_Independent_Variable(bool);
	void refresh_State_All();

	void if_Selected();

public:
	QMap<QString, QListWidgetItem*>* independent_Variables_List_Map;	// independent whatsThis and corresponding items in ListWidget
	QTreeWidget* struct_Tree_Copy;										// modified copy of real tree to be plotted
	QTreeWidget* real_Struct_Tree;										// real tree itself

private:
		QFrame* independent_Frame;
		QVBoxLayout* independent_Frame_Layout;
public:		QListWidget* independent_Variables_List;
private:		QToolBar* independent_Variables_Toolbar;
};

#endif // INDEPENDENT_VARIABLES_H
