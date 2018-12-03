#ifndef TABLE_OF_STRUCTURES_H
#define TABLE_OF_STRUCTURES_H

#include "global/settings.h"
#include "multilayer_approach/multilayer/structure_tree/structure_tree.h"
#include "multilayer_approach/table_of_structures/my_table_widget.h"
#include "multilayer_approach/calculation_settings_editor/calculation_settings_editor.h"

class Structure_Tree;
class MyDoubleSpinBox;
class My_Table_Widget;
class Calculation_Settings_Editor;
class Layer_Thickness_Transfer;

class Table_Of_Structures : public QWidget
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize() {  }
	void closeEvent(QCloseEvent* event);
public:
	explicit Table_Of_Structures(bool temporary = false, QWidget* parent = 0);

signals:
//	void closed();
	void data_Edited();

public:
	void create_Main_Layout();
	void set_Window_Geometry();
	void write_Window_Geometry();
		void lock_Mainwindow_Interface();
		void unlock_Mainwindow_Interface();
		void create_Menu();
		void create_Tabs();
		void add_Tabs();
		void create_Table(My_Table_Widget* new_Table, int tab_Index);
		void read_Trees();
		void refresh_Reload_Colorize(QString refresh_Reload, QWidget* back_Widget, Parameter* parameter);

	// for material only
	QString material_From_Composition(const QList<Stoichiometry>& composition);
	void create_Combo_Elements		(My_Table_Widget* table, int,			int current_Row, int start_Column, QTreeWidgetItem* structure_Item);
	void create_Stoich_Line_Edit	(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString val_Type);
	void create_Stoich_Check_Box_Fit(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, int r_S, int r_F, int c_S, int c_F);

	void create_Material_Line_Edit	(My_Table_Widget* table, int tab_Index,	int current_Row, int current_Column, QTreeWidgetItem* structure_Item);
	void create_Browse_Button		(My_Table_Widget* table,				int current_Row, int start_Column, int material_LineEdit_Row, int material_LineEdit_Column);

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
	void create_MySigma_Line_Edits_Interlayer	(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item);

	// min/max
	void create_Simple_Label	 (My_Table_Widget* table,				 int current_Row, int current_Column, QString text);
	void create_Min_Max_Button	 (My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This);
	void create_Min_Max_Spin_Box (My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This);

	// step of spinbox
	void create_Step_Spin_Box	 (My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This);

	// spinbox recalculation
	void spin_Box_Recalculate	 (My_Table_Widget* table,				 int current_Row, int current_Column);

	// general
	void span_Structure_Headers(My_Table_Widget* table);
	void span_Structure_Items(My_Table_Widget* table);

	void disable_enable_Structure_Items(My_Table_Widget* table);
	void disable_enable_One_Item(My_Table_Widget* table, QTableWidgetItem* table_Item);
	void disable_enable_One_Item_Content(My_Table_Widget* table, QTableWidgetItem* table_Item, bool save, bool enable = true);
	void disable_enable_Multilayers(My_Table_Widget* table, QTreeWidgetItem* parent);

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
	void refresh_Element(My_Table_Widget *table, QString);
	void refresh_Stoich();
	void refresh_Fit_Element(bool);

	void refresh_Material(My_Table_Widget *table, QString);
	void check_Material(QLineEdit *line_Edit = nullptr);
	void browse_Material(QLineEdit* material_Line_Edit);

	// for several parameters
	void refresh_Header(QString);
	void refresh_Check_Box_Header(bool);
	void change_Parent_Period_Gamma_Thickness(QTreeWidgetItem* current_Item);
	void change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor);
	void reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness);
	void reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness);
	void change_Stack_Gamma(QTreeWidgetItem* structure_Item, double new_Gamma_Value);

	// for all parameters
	void refresh_Parameter(My_Table_Widget* table);
	void refresh_Fit_Parameter(bool);

	// for interlayers
	void refresh_Check_Box_Label_Interlayer(bool);
	void refresh_Weigts_Interlayer();
	void refresh_Weights_Check_Box_Fit_Interlayer(bool);

	void refresh_MySigma_Interlayer();
	double recalculate_Sigma_From_Individuals(QVector<Interlayer>& interlayer_Composition);

	// general
	void cells_On_Off    (My_Table_Widget* table);
	void cells_On_Off_2  (My_Table_Widget* table, QTreeWidgetItem* structure_Item);

	template <typename Type>
	void resize_Line_Edit(My_Table_Widget* table, Type* line_Edit = nullptr);
	void emit_Data_Edited();

public slots:
	void reload_All_Widgets(QObject* sender = nullptr);
	void reload_One_Widget (QWidget* widget_To_Reload);
	void reload_Related_Widgets(QObject* sender = nullptr);
	void reload_Min_Max_Line_Edits(QString whats_This);

public:
	int temp_Counter = 0;       // TEMPORARY
	int temp_Counter_1 = 0;     // TEMPORARY
	bool table_Is_Created = false;
	bool layer_Thickness_Transfer_Is_Created = false;
	bool layer_Thickness_Transfer_Reload_Block = false;
	int basic_Row_Number = 0;
	int basic_Column_Number = 1;
	bool temporary = false;

	Menu* menu;
	QTabWidget* main_Tabs;
	QList<Structure_Tree*> list_Of_Trees;
	QList<int> rows_List_To_Span;

	QMap<QString, Table_Of_Structures*>& runned_Tables_Of_Structures;
	QMap<QString, Calculation_Settings_Editor*>& runned_Calculation_Settings_Editor;
	Layer_Thickness_Transfer* layer_Thickness_Transfer;
	QTabWidget* multilayer_Tabs;

	QHBoxLayout* main_Layout;

	QList<QLineEdit*> material_Line_Edits;
	QMap<QComboBox*,QTreeWidgetItem*> elements_Map;
	QMap<QLineEdit*,QTreeWidgetItem*> line_Edits_Map;	// lineEdit
	QMap<MyDoubleSpinBox*,QTreeWidgetItem*> spin_Boxes_Map;	// spinBox
	QMap<QCheckBox*,QTreeWidgetItem*> check_Boxes_Map;

	QList<QList<MyDoubleSpinBox*>> min_Max_Density_Spin_Boxes_List;
	QList<QList<MyDoubleSpinBox*>> min_Max_Thickness_Spin_Boxes_List;
	QList<QList<MyDoubleSpinBox*>> min_Max_Sigma_Spin_Boxes_List;

	// coupling
	QMap<QWidget*, QTreeWidgetItem*> coupled_Back_Widget_and_Struct_Item;
	QMap<QWidget*, id_Type>			 coupled_Back_Widget_and_Id;

	// refresh/reload
	QList<QList<QWidget*>>			all_Widgets_To_Reload;
	QMultiMap<QWidget*, id_Type>	reload_Show_Dependence_Map;
	QMap<QCheckBox*,id_Type>		check_Boxes_Fit_Map;
	QMap<MyDoubleSpinBox*,id_Type>	spin_Boxes_ID_Map;

	// disable/enable
	QMap<QTreeWidgetItem*, QTableWidgetItem*> struct_Table_Map;

	// steps
	QList<MyDoubleSpinBox*> composition_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> density_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> thickness_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> gamma_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> sigma_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> interlayer_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> drift_Spin_Boxes_List;
};

#endif // TABLE_OF_STRUCTURES_H
