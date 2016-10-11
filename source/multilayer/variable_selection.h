#ifndef VARIABLE_SELECTION_H
#define VARIABLE_SELECTION_H

#include <QtWidgets>
#include "layer_content.h"
#include "global_definitions.h"

class Variable_Selection : public QDialog
{
	Q_OBJECT

public:
	Variable_Selection(QTreeWidget* struct_Tree_Copy, QMap<QString, QListWidgetItem*>* independent_Variables_List_Map, QListWidget* independent_Variables_List, Variable_Type type);

	QSettings default_Values;

	QTreeWidget* struct_Tree_Copy;
	QListWidget* independent_Variables_List;
	QMap<QString, QListWidgetItem*>* independent_Variables_List_Map;

signals:
	void is_Closed();

private:
	QStackedWidget* filtered_Parameters_Pages;
		QMap<QString,QListWidget*> map_Of_Parameters_Lists;
			QListWidget* nonfiltered_Parameters;
			QListWidget* optical_Constants;
			QListWidget* interface_Parameters;
			QListWidget* thickness_Parameters;
	QLabel* filters_Label;
	QComboBox* filters_Combo_Box;
	QPushButton* cancel_Button;
	QPushButton* ok_Button;

private:
	void closeEvent(QCloseEvent* event);
	void create_Window();
	void iterate_Over_Children(QTreeWidgetItem* this_Stack, bool& thickness_Condition);
	void iterate_Over_Parents(QTreeWidgetItem* this_Item, bool& thickness_Condition);

	// independent variables
	void fill_Variables_List();
		void fill_Ambient_Variables(QTreeWidgetItem* item);
		void fill_Layer_Variables(QTreeWidgetItem* item);
		void fill_Multilayer_Variables(QTreeWidgetItem* item);
		void fill_Substrate_Variables(QTreeWidgetItem* item);

private slots:
	void add_Variable();
	void filter_Change(int index);

};

#endif // VARIABLE_SELECTION_H
