#ifndef ITEM_EDITOR_H
#define ITEM_EDITOR_H

#include "global/layer_data_types.h"
#include "standard/menu.h"
#include "grading_editor.h"

class Structure_Tree;

class Item_Editor : public QDialog
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize()							{ QWidget::adjustSize(); }
//	Q_INVOKABLE void moreElementsClicked()					{ Item_Editor::more_Elements_Clicked(); }
public:
	explicit Item_Editor(QList<Item_Editor*>& list_Editors, QTreeWidgetItem* item, Structure_Tree* structure_Tree, QWidget *parent = 0);

signals:
////	void refresh();
	void closed();
	void item_Data_Edited();

public:
	QTreeWidgetItem* item;
	QList<Item_Editor*> list_Editors;
	Data struct_Data;
	Structure_Tree* structure_Tree;

	// state variables
	bool material_Done = false;
	bool thickness_Done = false;
	bool sigma_Done = false;
	bool stack_Done = false;

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
		void create_Menu();
		void make_Ambient_Editor();
			void make_Materials_Group_Box();
			void initial_Radio_Check();
		void make_Layer_Editor();
			void make_Thickness_Group_Box();
		void make_Multilayer_Editor();
			void make_Multilayer_Group_Box();
		void make_Aperiodic_Editor();
			void make_Aperiodic_Group_Box();
		void make_Substrate_Editor();
			void make_Sigma_Group_Box();
	void set_Window_Geometry();

	void reload_And_Show_All();
	void show_All();	
	void show_Material();
	void show_Density();
	void show_Thickness();
	void show_Sigma_Start(bool at_Start=false);
	void show_Sigma(bool b);
	void show_Stack_Parameters();
	void show_Interlayers(QObject* my_Sender = nullptr);

	void emit_Item_Data_Edited();

	void browse_Material();
	void depth_Grading();
	void sigma_Grading();
	void filename_Radio_Toggled();
	void composition_Radio_Toggled();
	void more_Elements_Clicked();
	void read_Elements_From_Item();
	void read_Interlayers_From_Item();
	void fewer_Elements_Clicked();
	void interlayer_Check();
	void norm_Interlayer_Composition();

	void refresh_Material();
	void refresh_Data();
	void fast_Refresh_Stack();

	void change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor);
	void change_Stack_Gamma();
		void reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness);
		void reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness);

	void make_Aperiodic();
	void make_Multilayer();
	void invert_Aperiodic();
	void calc_Uniqueness();
	void unique_Item_Do(QString action, int uniqueness);
	void soft_Restriction_Save(int current_Uniqueness, bool use_Soft_Restrictions, int threshold, double Q_factor);

	QString absolute_Density_Label = "Density [g/cm" + Cube_Sym + "]:";
	QString relative_Density_Label = "Relative Density:";
	QString atoms = "atoms of";
	QString AtWt = "At. Wt.=";
	QString thickness_Label_1 = "Thickness, z [";
	QString thickness_Label_2 = "]:";
	QString period_Label_1 = "Period, d[";
	QString period_Label_2 = "]:";
	QString sigma_Label_1 = "Roughness/Diffuseness, " + Sigma_Sym + "[";
	QString sigma_Label_2 = "]:";
	QString sigma_Label_3 = Sigma_Sym + "[";
	QString sigma_Weight = "weight:";

	bool auto_Density_On = false;

	QVBoxLayout* main_Layout;
	QHBoxLayout* hor_Layout;
		QGroupBox* material_Group_Box;
			QLineEdit* material_Line_Edit;
			QPushButton* browse_Material_Button;
			QLineEdit* density_Line_Edit;
			QLabel* density_Label;
			QRadioButton* filename_Radio;
			QRadioButton* composition_Radio;
			QFrame* composition_Frame;
				QGroupBox* composition_Group_Box;
					QVBoxLayout* composition_Layout_With_Elements_Vector;
						QVector<QFrame*> element_Frame_Vec;
							QVector<QLineEdit*> composition_Line_Edit_Vec;
							QVector<QLabel*>    composition_Label_Vec;
							QVector<QComboBox*> composition_Combo_Box_Vec;
							QVector<QLabel*>    composition_At_Weight_Vec;
				QPushButton* more_Elements;
				QPushButton* fewer_Elements;

		QGroupBox* thickness_Group_Box;
			QLabel* thickness_Label;
			QLineEdit* thickness_Line_Edit;
			QPushButton* depth_Grading_Button;

		QGroupBox* sigma_Group_Box;
			QLabel* roughness_Label;
			QLineEdit* sigma_Line_Edit;
			QFrame* interlayer_Composition_Frame;
				QGroupBox* interlayer_Composition_Group_Box;
					QHBoxLayout* interlayer_Composition_Layout_With_Elements_Vector;
						QLabel* my_Sigma_Label_Layer;
						QLabel* my_Sigma_Label_Substrate;
						QVector<QCheckBox*> interlayer_Composition_Check_Box_Vec;
						QVector<QLineEdit*> interlayer_Composition_Comp_Line_Edit_Vec;
						QVector<QLineEdit*> interlayer_Composition_My_Sigma_Line_Edit_Vec;
				QCheckBox* PSD_Check_Box;
				QCheckBox* individual_Sigma_Check_Box;
			QPushButton* sigma_Grading_Button;

		QGroupBox* multilayer_Group_Box;
			QLineEdit* repetitions_Line_Edit;
			QLabel* period_Label;
			QLineEdit* period_Line_Edit;
			QLabel* gamma_Label;
			QLineEdit* gamma_Line_Edit;
			QCheckBox* make_Aperiodic_CheckBox;

		QGroupBox* aperiodic_Group_Box;
			QCheckBox* make_Multilayer_CheckBox;
			QCheckBox* invert_CheckBox;
			QMap<QTreeWidgetItem*, int> item_Uniqueness_Map;
			QVector<int> uniqueness_Vector;

		QPushButton* done_Button;
};

#endif // ITEM_EDITOR_H
