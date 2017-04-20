#ifndef TABLE_OF_STRUCTURES_H
#define TABLE_OF_STRUCTURES_H

#include "global/settings.h"
#include "multilayer/structure_tree.h"

class Structure_Tree;

class Table_Of_Structures : public QWidget
{
	Q_OBJECT
	void closeEvent(QCloseEvent* event);
public:
	explicit Table_Of_Structures(QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures, QTabWidget* multilayer_Tabs, QWidget *parent = 0);

signals:
	void closed();
	void data_Edited();

public:
	void create_Main_Layout();
		void create_Table();
		void read_Trees();

	// for material only
	void create_Combo_Elements(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
	void create_Stoich(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type);
	QString material_From_Composition(QList<Stoichiometry>& composition);
	void create_Stoich_Check_Box_Fit(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int r_S, int r_F, int c_S, int c_F);

	void create_Material(int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
	void create_Browse_Button(int current_Row, int start_Column);

	// for several parameters
	void create_Label			(int current_Row, int current_Column, QString text);
	void create_Check_Box_Label	(int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString text, int r_S, int r_F, int c_S, int c_F);

	// for all parameters
	void create_Line_Edit    (int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString val_Type);
	void create_Check_Box_Fit(int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, int r_S, int r_F, int c_S, int c_F);

	// for interlayers
	void create_Check_Box_Label_Interlayer		(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
	void create_Weigts_Interlayer				(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type);
	void create_Weights_Check_Box_Fit_Interlayer(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);

	void create_MySigma_Labels_Interlayer		(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
	void create_MySigma_Interlayer				(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);

	// general
	void span_Structure_Headers();
	void span_Structure_Items();
	void fit_Column(int start_Width, int current_Column);
	void add_Columns(int add_After);

public slots:

	// for material only
	void refresh_Element(QString temp = "empty");
	void refresh_Stoich(QString temp = "empty");
	void refresh_Fit_Element(bool b = false);

	void refresh_Material(QString temp = "empty");
	void check_Material();
	void browse_Material(bool b = false);

	// for several parameters
	void refresh_Check_Box_Header(bool b = false);
	void change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor);
	void reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness);
	void reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness);
	void change_Stack_Gamma(QTreeWidgetItem* structure_Item, double new_Gamma_Value);

	// for all parameters
	void refresh_Parameter(QString temp = "empty");
	void refresh_Fit_Parameter(bool b = false);

	// for interlayers
	void refresh_Check_Box_Label_Interlayer		 (bool b = false);
	void refresh_Weigts_Interlayer				 (QString temp = "empty");
	void refresh_Weights_Check_Box_Fit_Interlayer(bool b = false);

	void refresh_MySigma_Interlayer(QString temp = "empty");

	// general
	void cells_On_Off(bool b = false);
	void cells_On_Off_2(bool b = false);
	void resize_Line_Edit(QString text, QLineEdit* line_Edit = NULL);
	void emit_Data_Edited();
	void reload_All_Widgets(QObject* sender = NULL);

public:
	int temp_Counter = 0;
	bool table_Is_Created = false;
	int basic_Row_Number = 0;
	int basic_Column_Number = 1;

	QTableWidget* main_Table;
	QList<Structure_Tree*> list_Of_Trees;

private:
	QList<int> rows_List_To_Span;
	QHBoxLayout* main_Layout;
	QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures;
	QTabWidget* multilayer_Tabs;

	QString item_Type_String;

	QMap<QComboBox*,QTreeWidgetItem*> elements_Map;
	QMap<QLineEdit*,QTreeWidgetItem*> line_Edits_Map;
	QMap<QCheckBox*,QTreeWidgetItem*> check_Boxes_Map;
	QList<QWidget*> all_Widgets_To_Reload;
};

#endif // TABLE_OF_STRUCTURES_H
