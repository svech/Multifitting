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
        void create_Menu();
        void create_Tabs();
        void add_Tabs();
        void create_Table(QTableWidget* new_Table, int tab_Index);
		void read_Trees();

	// for material only
    void create_Combo_Elements(QTableWidget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
    void create_Stoich(QTableWidget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type);
	QString material_From_Composition(QList<Stoichiometry>& composition);
    void create_Stoich_Check_Box_Fit(QTableWidget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int r_S, int r_F, int c_S, int c_F);

    void create_Material(QTableWidget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
    void create_Browse_Button(QTableWidget* table, int current_Row, int start_Column);

	// for several parameters
    void create_Label			(QTableWidget* table, int current_Row, int current_Column, QString item_Type_String, QString whats_This, QString text);
    void create_Check_Box_Label	(QTableWidget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString text, int r_S, int r_F, int c_S, int c_F);

	// for all parameters
    void create_Line_Edit    (QTableWidget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString val_Type);
    void create_Check_Box_Fit(QTableWidget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, int r_S, int r_F, int c_S, int c_F);

	// for interlayers
    void create_Check_Box_Label_Interlayer		(QTableWidget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
    void create_Weigts_Interlayer				(QTableWidget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type);
    void create_Weights_Check_Box_Fit_Interlayer(QTableWidget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);

    void create_MySigma_Labels_Interlayer		(QTableWidget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
    void create_MySigma_Interlayer				(QTableWidget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);

	// general
    void span_Structure_Headers(QTableWidget* table);
    void span_Structure_Items(QTableWidget* table);
    void fit_Column(QTableWidget* table, int start_Width, int current_Column);
    void add_Columns(QTableWidget* table, int add_After);

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
    void refresh_Parameter(QTableWidget* table);
    void refresh_Fit_Parameter();

	// for interlayers
    void refresh_Check_Box_Label_Interlayer		 ();
    void refresh_Weigts_Interlayer				 ();
    void refresh_Weights_Check_Box_Fit_Interlayer();

    void refresh_MySigma_Interlayer();
    double recalculate_Sigma_From_Individuals(QVector<Interlayer> &interlayer_Composition);

	// general
    void cells_On_Off(QTableWidget* table);
    void cells_On_Off_2(QTableWidget* table, bool b = false);
    void resize_Line_Edit(QTableWidget* table, QLineEdit* line_Edit = NULL);
	void emit_Data_Edited();
	void reload_All_Widgets(QObject* sender = NULL);

public:
//    int temp_Counter = 0;       // TEMPORARY
//    int temp_Counter_1 = 0;     // TEMPORARY
    bool table_Is_Created = false;
	int basic_Row_Number = 0;
	int basic_Column_Number = 1;

    QTabWidget* main_Tabs;
    QList<QTableWidget*> list_Of_Tables;
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
