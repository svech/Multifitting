#ifndef VARIABLE_SELECTION_H
#define VARIABLE_SELECTION_H

#include <QtWidgets>
#include "layer_content.h"
#include "global_definitions.h"

class Variable_Selection : public QDialog
{
	Q_OBJECT

public:
	Variable_Selection(QTreeWidget* struct_Tree, QMap<QString, QListWidgetItem*>* independent_Variables_List_Map, QListWidget* independent_Variables_List, Variable_Type type);

	QTreeWidget* struct_Tree;
	QListWidget* independent_Variables_List;
	QMap<QString, QListWidgetItem*>* independent_Variables_List_Map;

private:
	QStackedWidget* filtered_Parameters_Pages;
		QMap<QString,QListWidget*> map_Of_Parameters_Lists;
			QListWidget* nonfiltered_Parameters;
	QLabel* filters_Label;
	QComboBox* filters_Combo_Box;
	QPushButton* cancel_Button;
	QPushButton* ok_Button;

private:
	void create_Window();

	// independent variables
	void fill_None_Filtered_Variables_List();
		void fill_None_Filtered_Ambient_Variables(QTreeWidgetItem* item);
		void fill_None_Filtered_Layer_Variables(QTreeWidgetItem* item);
		void fill_None_Filtered_Multilayer_Variables(QTreeWidgetItem* item);
		void fill_None_Filtered_Substrate_Variables(QTreeWidgetItem* item);

private slots:
	void add_Variable();
};

#endif // VARIABLE_SELECTION_H
