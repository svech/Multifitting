#ifndef VARIABLE_SELECTION_H
#define VARIABLE_SELECTION_H

#include "global/layer_data_types.h"

class Variable_Selection : public QDialog
{
	Q_OBJECT
public:
	explicit Variable_Selection(QTreeWidget* struct_Tree_Copy, QMap<id_Type, QListWidgetItem*>* variables_List_Map, QListWidget* variables_List, QWidget *parent = 0);

signals:
	void closed();

public:
	QTreeWidget* struct_Tree_Copy;
	QListWidget* variables_List;
	QMap<id_Type, QListWidgetItem*>* variables_List_Map;

	QStackedWidget* filtered_Parameters_Pages;
		QMap<QString,QListWidget*> map_Of_Parameters_Lists;
			QListWidget* nonfiltered_Parameters;
			QListWidget* optical_Constants_List;
			QListWidget* interface_Parameters;
			QListWidget* thickness_Parameters;
	QLabel* filters_Label;
	QComboBox* filters_Combo_Box;
	QPushButton* cancel_Button;
	QPushButton* ok_Button;

	void closeEvent(QCloseEvent* event);
	void create_Window();
	void set_Geometry();
	void iterate_Over_Children(QTreeWidgetItem* this_Stack, bool& thickness_Condition);
	void iterate_Over_Parents(QTreeWidgetItem* this_Item, bool& parent_Condition);

	// independent variables
	void fill_Variables_List();
		void add_Parameter				(QTreeWidgetItem* struct_Item, QString whats_This);

		// special
		void add_Composition			(QTreeWidgetItem* struct_Item);
		void add_Interlayer_Composition	(QTreeWidgetItem* struct_Item);
		void add_Interlayer_My_Sigma	(QTreeWidgetItem* struct_Item);
		void add_Num_repetitions		(QTreeWidgetItem* struct_Item);

	static void refresh_Independent_State_Of_Struct_Tree_Copy_Item(QTreeWidgetItem* struct_Item);
	void add_Variable_Item(QListWidgetItem* new_Item);
	void add_Variable();

public slots:
	void filter_Change(int index);
};

#endif // VARIABLE_SELECTION_H
