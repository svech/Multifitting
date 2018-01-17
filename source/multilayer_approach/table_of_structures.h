#ifndef TABLE_OF_STRUCTURES_H
#define TABLE_OF_STRUCTURES_H

#include "global/settings.h"
#include "multilayer/structure_tree.h"
#include "table_of_structures/my_table_widget.h"

class Structure_Tree;
class My_Table_Widget;

class Table_Of_Structures : public QWidget
{
	Q_OBJECT
	void closeEvent(QCloseEvent* event);
public:
	explicit Table_Of_Structures(QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures, QTabWidget* multilayer_Tabs, QWidget *parent = 0);

signals:
//	void closed();
	void data_Edited();

public:
	void create_Main_Layout();
		void lock_Mainwindow_Interface();
		void unlock_Mainwindow_Interface();
		void create_Menu();
		void create_Tabs();
		void add_Tabs();
		void create_Table(My_Table_Widget* new_Table, int tab_Index);
		void read_Trees();
//		void revise_All_Parameters();
		static void refresh_Reload_Core(QString refresh_Reload, QWidget* widget, Parameter& parameter, QMap<QWidget*,QTreeWidgetItem*>& coup_Widgets_Map);

	// for material only
	QString material_From_Composition(const QList<Stoichiometry>& composition);
	void create_Combo_Elements		(My_Table_Widget* table, int,			int current_Row, int start_Column, QTreeWidgetItem* structure_Item);
	void create_Stoich				(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString val_Type);
	void create_Stoich_Check_Box_Fit(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, int r_S, int r_F, int c_S, int c_F);

	void create_Material			(My_Table_Widget* table, int,			int current_Row, int current_Column, QTreeWidgetItem* structure_Item);
	void create_Browse_Button		(My_Table_Widget* table,				int current_Row, int start_Column);

	// for several parameters
	void create_Label				(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString text);
	void create_Check_Box_Label		(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString text, int r_S, int r_F, int c_S, int c_F);

	// for all parameters
	void create_Line_Edit			(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString val_Type);
	void create_Check_Box_Fit		(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, int r_S, int r_F, int c_S, int c_F);

	// for interlayers
	void create_Check_Box_Label_Interlayer		(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item);
	void create_Weigts_Interlayer				(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString val_Type);
	void create_Weights_Check_Box_Fit_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item);

	void create_MySigma_Labels_Interlayer		(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item);
	void create_MySigma_Interlayer				(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item);

	// general
	void span_Structure_Headers(My_Table_Widget* table);
	void span_Structure_Items(My_Table_Widget* table);
	void fit_Column(My_Table_Widget* table, int start_Width, int current_Column);
	void add_Columns(My_Table_Widget* table, int add_After);

	static Parameter& get_Parameter(Data& struct_Data, QString whats_This)
	{
		int dev_Null_Int;
		double dev_Null_Double;
		return get_Parameter(      struct_Data,         whats_This, dev_Null_Int,   dev_Null_Double);
	}
	static Parameter& get_Parameter(Data& struct_Data, QString whats_This, int& precision, double& coeff);

	// for material only
	void refresh_Element(QString);
	void refresh_Stoich(QString);
	void refresh_Fit_Element(bool);

	void refresh_Material(QString);
	void check_Material();
	void browse_Material(bool);

	// for several parameters
	void refresh_Header(QString);
	void refresh_Check_Box_Header(bool);
	void change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor);
	void reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness);
	void reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness);
	void change_Stack_Gamma(QTreeWidgetItem* structure_Item, double new_Gamma_Value);

	// for all parameters
	void refresh_Parameter(My_Table_Widget* table);
	void refresh_Fit_Parameter(bool);

	// for interlayers
	void refresh_Check_Box_Label_Interlayer(bool);
	void refresh_Weigts_Interlayer(QString);
	void refresh_Weights_Check_Box_Fit_Interlayer(bool);

	void refresh_MySigma_Interlayer(QString);
	double recalculate_Sigma_From_Individuals(QVector<Interlayer>& interlayer_Composition);

	// general
	void cells_On_Off    (My_Table_Widget* table);
	void cells_On_Off_2  (My_Table_Widget* table, QTreeWidgetItem* structure_Item);
	void resize_Line_Edit(My_Table_Widget* table, QLineEdit* line_Edit = NULL);
	void emit_Data_Edited();

public slots:
	void reload_All_Widgets(QObject* sender = NULL);
	void reload_Related_Widgets(QObject* sender = NULL);

public:
	int temp_Counter = 0;       // TEMPORARY
	int temp_Counter_1 = 0;     // TEMPORARY
	bool table_Is_Created = false;
	int basic_Row_Number = 0;
	int basic_Column_Number = 1;

	QTabWidget* main_Tabs;
	QList<Structure_Tree*> list_Of_Trees;

	QList<int> rows_List_To_Span;
	QHBoxLayout* main_Layout;
	QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures;
	QTabWidget* multilayer_Tabs;

	QMap<QComboBox*,QTreeWidgetItem*> elements_Map;
	QMap<QLineEdit*,QTreeWidgetItem*> line_Edits_Map;
	QMap<QCheckBox*,QTreeWidgetItem*> check_Boxes_Map;

	// coupling
	QMap<QWidget*,QTreeWidgetItem*> coupled_Widget_Item;
	QMap<int, QWidget*>				coupled_Widget_Id;

	// refresh/reload
	QList<QList<QWidget*>> all_Widgets_To_Reload;
	QMultiMap<int, QWidget*> reload_Show_Dependence_Map;
};

#endif // TABLE_OF_STRUCTURES_H
