#ifndef TABLE_OF_STRUCTURES_H
#define TABLE_OF_STRUCTURES_H

#include "global/settings.h"
#include "multilayer_approach/multilayer/structure_tree/structure_tree.h"
#include "multilayer_approach/table_of_structures/my_table_widget.h"
#include "multilayer_approach/calculation_settings_editor/calculation_settings_editor.h"
#include "multilayer_approach/table_of_structures/measured_psd_editor.h"

class Structure_Tree;
class MyDoubleSpinBox;
class My_Table_Widget;
class Calculation_Settings_Editor;
class Layer_Thickness_Transfer;
class Menu;
class Measured_PSD_Editor;

class Table_Of_Structures : public QWidget
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize() {  }
	bool eventFilter(QObject *obj, QEvent *event);
	void closeEvent(QCloseEvent* event);
public:
	explicit Table_Of_Structures(bool temporary = false, QWidget* parent = 0);

signals:
//	void closed();
	void data_Edited();
	void regular_Layer_Edited(QString identifier);

public:
	void create_Main_Layout();
	void set_Window_Geometry();
	void write_Window_Geometry();
		void create_Menu();
		void create_Tabs();
		void add_Tabs();
		void create_Table(My_Table_Widget* new_Table, int tab_Index);
		void read_Trees();
		void refresh_Reload_Colorize(QString refresh_Reload, QWidget* back_Widget, Parameter* parameter);

	// for material only
	void create_Combo_Elements		(My_Table_Widget* table, int,			int current_Row, int start_Column, QTreeWidgetItem* structure_Item, bool is_Particle);
	void create_Stoich_Line_Edit	(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, bool is_Particle, QString val_Type);
	void create_Stoich_Check_Box_Fit(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, bool is_Particle, int r_S, int r_F, int c_S, int c_F);

	void create_Material_Line_Edit	(My_Table_Widget* table, int tab_Index,	int current_Row, int current_Column, QTreeWidgetItem* structure_Item, bool is_Particle);
	void create_Browse_Button		(My_Table_Widget* table,				int current_Row, int start_Column, int material_LineEdit_Row, int material_LineEdit_Column);

	// for several parameters
	void create_Label				(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString text);
	void create_Check_Box_Label		(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString text, int r_S, int r_F, int c_S, int c_F);

	// for particles
	void create_Check_Box_Usage		(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString text, int r_S, int r_F, int c_S, int c_F);
	void create_Shape_Button		(My_Table_Widget* table,                int current_Row, int current_Column, QTreeWidgetItem* structure_Item);
	void create_Pattern_Button		(My_Table_Widget* table,                int current_Row, int current_Column, QTreeWidgetItem* structure_Item);
	void create_Model_Button		(My_Table_Widget* table,                int current_Row, int current_Column, QTreeWidgetItem* structure_Item);
	void open_Shape_Pattern_Model_Dialog(QTreeWidgetItem* structure_Item, QString shape_Pattern_Model);

	// for measured PSD
	void create_PSD_Load_Button					(My_Table_Widget* table,                int current_Row, int current_Column,								  Multilayer* multilayer, QString PSD_Type, MyDoubleSpinBox* PSD_Sigma_Lineedit);
	MyDoubleSpinBox* create_PSD_Sigma_Lineedit	(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, Multilayer* multilayer, QString PSD_Type);
	void create_Nu0_Label	 (My_Table_Widget* table, int tab_Index, int current_Row, int current_Column);
	void create_Nu0_Spin_Box (My_Table_Widget* table, int tab_Index, int current_Row, int current_Column);

	// for regular aperiodic
	void create_Thickness_Restriction(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item);

	// for all parameters
	void create_Line_Edit			(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString val_Type, MyDoubleSpinBox* PSD_Sigma_Lineedit = nullptr);
	void create_Check_Box_Fit		(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, int r_S, int r_F, int c_S, int c_F);

	// for interlayers
	void create_Check_Box_Label_Interlayer		(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item);
	void create_Weigts_Interlayer				(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString val_Type);
	void create_Weights_Check_Box_Fit_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item);

	void create_MySigma_Labels_Interlayer		(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item);
	void create_MySigma_Line_Edits_Interlayer	(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item);

	// min/max
	void create_Simple_Label	 (My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This, QString text);
	void create_Min_Max_Button	 (My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This);
	void create_Min_Max_Spin_Box (My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This);

	// step of spinbox
	void create_Step_Spin_Box	 (My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This);

	// spinbox recalculation
	void spin_Box_Recalculate		(My_Table_Widget* table,				 int current_Row, int current_Column);
	void spin_Box_Mouse_Wheel		(My_Table_Widget* table,				 int current_Row, int current_Column);
	void spin_Box_Change_Dependent  (My_Table_Widget* table,				 int current_Row, int current_Column);
	void change_Model				(My_Table_Widget* table, int tab_Index,	 int current_Row, int current_Column);
	void refill_All_Dependent		();
	void real_Tree_Iteration			(Multilayer* multilayer, QTreeWidget* real_Struct_Tree, QVector<Parameter>& master_Parameters);
	void change_Slaves_in_Structure_Tree(Parameter& master, const QVector<Parameter_Indicator>& slaves, QVector<id_Type>& ids);
	void refresh_Dependents		(const QVector<id_Type>& ids, bool manual_Refresh = false, bool reload_Period_Gamma = true);
	void lock_Unlock_Dependents	(const QVector<id_Type>& ids);
	void check_Multilayer_Item_For_Dependent_Thicknesses(QTreeWidgetItem* multilayer_Item, bool& has_Dependent);
	QTreeWidgetItem* get_Struct_Item_From_Multilayer_by_Id(id_Type id);

	// general
	void span_Structure_Headers	(My_Table_Widget* table);
	void span_Structure_Items	(My_Table_Widget* table);

	void disable_enable_Structure_Items	(My_Table_Widget* table);
	void disable_enable_One_Item		(My_Table_Widget* table, QTableWidgetItem* table_Item);
	void disable_enable_One_Item_Content(My_Table_Widget* table, QTableWidgetItem* table_Item, bool save, bool enable = true);
	void disable_enable_Multilayers		(My_Table_Widget* table, QTreeWidgetItem* parent);

	static void fit_Column(QTableWidget *table, int start_Width, int current_Column);
	void add_Columns(My_Table_Widget* table, int add_After);

	static Parameter& get_Parameter(Data& struct_Data, QString whats_This)
	{
		int dev_Null_Int;
		double dev_Null_Double;
		return get_Parameter(      struct_Data,         whats_This, dev_Null_Int,   dev_Null_Double);
	}
	static Parameter& get_Parameter(Data& struct_Data, QString whats_This, int& precision, double& coeff);

	// for material only
	void refresh_Element(My_Table_Widget* table, bool is_Particle);
	void refresh_Stoich(bool is_Particle);
	void refresh_Fit_Element(bool is_Particle);

	void refresh_Material(My_Table_Widget* table, bool is_Particle);
	void check_Material(QLineEdit *line_Edit = nullptr, bool close = false, bool is_Particle = false);
	void browse_Material(QLineEdit* material_Line_Edit);

	// for several parameters
	void refresh_Header(QString);
	void refresh_Check_Box_Header(bool);
	void colorize_Regular_Aperiodic_Check_Box_Header(QCheckBox* check_Box);
	void change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, const double factor);
	void reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness);
	void reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness);
	void change_Stack_Gamma(QTreeWidgetItem* structure_Item, double new_Gamma_Value);

	// for all parameters
	void refresh_Parameter(My_Table_Widget* table);
	void refresh_Fit_Parameter(bool);

	void reload_From_Regular_Aperiodic();

	// for interlayers
	void refresh_Check_Box_Label_Interlayer(bool);
	void refresh_Weigts_Interlayer();
	void refresh_Weights_Check_Box_Fit_Interlayer(bool);

	void refresh_MySigma_Interlayer();
	static double recalculate_Sigma_From_Individuals(QVector<Interlayer>& interlayer_Composition);

	// general
	void cells_On_Off		(My_Table_Widget* table, bool borders = true, bool keep_Disabled = false, bool can_Enable = false);
	void cells_On_Off_2		(My_Table_Widget* table, QTreeWidgetItem* structure_Item);

	template <typename Type>
	void resize_Line_Edit(My_Table_Widget* table, Type* line_Edit = nullptr);
	void emit_Data_Edited();

public slots:
	void reload_All_Widgets(bool reload_Disabled = false, QObject* sender = nullptr);
	void reload_One_Widget (QWidget* widget_To_Reload, bool reload_Disabled = false);
	void reload_Related_Widgets(QObject* sender = nullptr);
	void reload_Min_Max_Line_Edits(QString whats_This);

public:
	int temp_Counter = 0;       // TEMPORARY
	int temp_Counter_1 = 0;     // TEMPORARY
	bool table_Is_Created = false;
	bool table_Is_Fully_Created = false;
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
	QMap<QLineEdit*,QTreeWidgetItem*> line_Edits_Map;		// lineEdit
	QMap<MyDoubleSpinBox*,QTreeWidgetItem*> spin_Boxes_Map;	// spinBox
	QMap<QCheckBox*,QTreeWidgetItem*> check_Boxes_Map;

	QList<QList<MyDoubleSpinBox*>> min_Max_Density_Spin_Boxes_List;
	QList<QList<MyDoubleSpinBox*>> min_Max_Thickness_Spin_Boxes_List;
	QList<QList<MyDoubleSpinBox*>> min_Max_Sigma_Diffuse_Spin_Boxes_List;

	// coupling
	QMap<QWidget*, QTreeWidgetItem*> coupled_Back_Widget_and_Struct_Item;
	QMap<QWidget*, id_Type>			 coupled_Back_Widget_and_Id;

	// refresh/reload
	QList<QList<QWidget*>>			all_Widgets_To_Reload;
	QMultiMap<QWidget*, id_Type>	reload_Show_Dependence_Map;
	QMap<QCheckBox*,id_Type>		check_Boxes_Fit_Map;
	QMap<QWidget*,id_Type>			back_Widgets_Fit_Map;
	QMap<MyDoubleSpinBox*,id_Type>	spin_Boxes_ID_Map;
	QList<QList<QCheckBox*>>		check_Boxes_Particles_Usage_List;

	// refresh/reload regular aperiodic
	QList<QList<QWidget*>>			regular_Aperiodic_Widgets_To_Reload;

	// disable/enable
	QMap<QTreeWidgetItem*, QTableWidgetItem*> struct_Table_Map;

	// access to item checkboxes
	QMap<QTreeWidgetItem*, QCheckBox*> item_Check_Boxes_Map;

	// steps
	QList<MyDoubleSpinBox*> composition_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> density_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> thickness_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> gamma_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> sigma_Diffuse_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> interlayer_Spin_Boxes_List;

	QList<MyDoubleSpinBox*> drift_Linear_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> drift_Random_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> drift_Amplitude_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> drift_Frequency_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> drift_Phase_Spin_Boxes_List;

	QList<MyDoubleSpinBox*> sigma_Roughness_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> correlation_Radius_Roughness_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> fractal_Alpha_Beta_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> vertical_Correlation_Length_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> linear_PSD_Omega_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> linear_PSD_Exponenta_Mu_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> linear_A1_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> linear_A2_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> linear_A3_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> linear_A4_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> sigma_Roughness_Peak_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> peak_Frequency_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> peak_Frequency_Width_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> PSD_Sigma_Factor_Spin_Boxes_List;

	QList<MyDoubleSpinBox*> particle_Density_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> particle_Radius_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> particle_Height_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> particle_Distance_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> particle_Distance_Deviation_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> domain_Size_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> particle_Z_Position_Spin_Boxes_List;
	QList<MyDoubleSpinBox*> particle_Z_Position_Deviation_Spin_Boxes_List;
};

#endif // TABLE_OF_STRUCTURES_H
