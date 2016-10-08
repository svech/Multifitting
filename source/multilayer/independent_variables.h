#ifndef INDEPENDENT_VARIABLES_H
#define INDEPENDENT_VARIABLES_H

#include <QtWidgets>
#include "global_definitions.h"
#include "variable_selection.h"

class Independent_Variables: public QWidget
{
	Q_OBJECT

public:
	Independent_Variables(QTreeWidget* struct_Tree);

	QSettings gui_Settings;
	QSettings default_Values;

	QTreeWidget* struct_Tree;
	QMap<QString, QListWidgetItem*>* independent_Variables_List_Map;

private:
	void create_Main_Layout();
	void create_Independent_Variables_List();
	void create_Independent_Variables_Toolbar();

private slots:
	void add_Independent_Variable(bool);
	void edit_Independent_Variable(bool);
	void remove_Independent_Variable(bool);

	void if_Selected();

private:
	QFrame* independent_Frame;
	QVBoxLayout* independent_Frame_Layout;
		QListWidget* independent_Variables_List;
		QToolBar* independent_Variables_Toolbar;
};

#endif // INDEPENDENT_VARIABLES_H
