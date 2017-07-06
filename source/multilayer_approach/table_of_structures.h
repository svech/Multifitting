#ifndef TABLE_OF_STRUCTURES_H
#define TABLE_OF_STRUCTURES_H

#include "global/settings.h"
#include "multilayer/structure_tree.h"
#include "table_of_structures/my_table_widget.h"

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
        void create_Menu();
        void create_Tabs();
        void add_Tabs();
		void create_Table(My_Table_Widget* new_Table, int tab_Index);
		void read_Trees();

	// for material only
	void create_Combo_Elements(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
	void create_Stoich(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type);
	QString material_From_Composition(QList<Stoichiometry>& composition);
	void create_Stoich_Check_Box_Fit(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int r_S, int r_F, int c_S, int c_F);

	void create_Material(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
	void create_Browse_Button(My_Table_Widget* table, int current_Row, int start_Column);

	// for several parameters
	void create_Label			(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString text);
	void create_Check_Box_Label	(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString text, int r_S, int r_F, int c_S, int c_F);

	// for all parameters
	void create_Line_Edit    (My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString val_Type);
	void create_Check_Box_Fit(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, int r_S, int r_F, int c_S, int c_F);

	// for interlayers
	void create_Check_Box_Label_Interlayer		(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
	void create_Weigts_Interlayer				(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type);
	void create_Weights_Check_Box_Fit_Interlayer(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);

	void create_MySigma_Labels_Interlayer		(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
	void create_MySigma_Interlayer				(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);

	// general
	void span_Structure_Headers(My_Table_Widget* table);
	void span_Structure_Items(My_Table_Widget* table);
	void fit_Column(My_Table_Widget* table, int start_Width, int current_Column);
	void add_Columns(My_Table_Widget* table, int add_After);

	void get_Parameter(Parameter& parameter, QTreeWidgetItem* structure_Item, QString whats_This); 
public slots:

	// for material only
    void refresh_Element();
    void refresh_Stoich();
	void refresh_Fit_Element(bool b = false);

	void refresh_Material(QString temp = "empty");
	void check_Material();
    void browse_Material();

	// for several parameters
    void refresh_Header();
    void refresh_Check_Box_Header();
	void change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor);
	void reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness);
	void reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness);
	void change_Stack_Gamma(QTreeWidgetItem* structure_Item, double new_Gamma_Value);

	// for all parameters
	void refresh_Parameter(My_Table_Widget* table);
    void refresh_Fit_Parameter();

	// for interlayers
    void refresh_Check_Box_Label_Interlayer		 ();
    void refresh_Weigts_Interlayer				 ();
    void refresh_Weights_Check_Box_Fit_Interlayer();

    void refresh_MySigma_Interlayer();
    double recalculate_Sigma_From_Individuals(QVector<Interlayer> &interlayer_Composition);

	// general
	void cells_On_Off(My_Table_Widget* table);
	void cells_On_Off_2(My_Table_Widget* table);
	void resize_Line_Edit(My_Table_Widget* table, QLineEdit* line_Edit = NULL);
	void emit_Data_Edited();
	void reload_All_Widgets(QObject* sender = NULL);

public:
//    int temp_Counter = 0;       // TEMPORARY
//    int temp_Counter_1 = 0;     // TEMPORARY
    bool table_Is_Created = false;
	int basic_Row_Number = 0;
	int basic_Column_Number = 1;

	QTabWidget* main_Tabs;
	QList<My_Table_Widget*> list_Of_Tables;
	QList<Structure_Tree*> list_Of_Trees;

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
