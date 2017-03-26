#ifndef VARIABLE_SELECTION_H
#define VARIABLE_SELECTION_H

#include "global/layer_data_types.h"

class Variable_Selection : public QDialog
{
	Q_OBJECT
public:
	explicit Variable_Selection(QTreeWidget* struct_Tree_Copy, QMap<QString, QListWidgetItem*>* variables_List_Map, QListWidget* variables_List, Variable_Type type, QWidget *parent = 0);

signals:
	void closed();

private:
	QTreeWidget* struct_Tree_Copy;
	QListWidget* variables_List;
	QMap<QString, QListWidgetItem*>* variables_List_Map;
	Variable_Type type;

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

		// parameters one by one
			void add_Absolute_Density		(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Relative_Density		(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Permittivity			(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Absorption				(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Composition			(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Thickness				(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Sigma					(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Interlayer_Composition	(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Num_repetitions		(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Period					(QTreeWidgetItem* item, QString whats_This_Type);
			void add_Gamma					(QTreeWidgetItem* item, QString whats_This_Type);

public slots:
	static void refresh_Independent_State_Of_Struct_Tree_Copy_Item(QTreeWidgetItem* structure_Item);
	void add_Variable_Item(QListWidgetItem* new_Item);
	void add_Variable();
	void filter_Change(int index);
};

#endif // VARIABLE_SELECTION_H
