#ifndef ITEM_EDITOR_H
#define ITEM_EDITOR_H

#include "global/layer_data_types.h"
#include "standard/menu.h"

class Item_Editor : public QDialog
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize()							{ QWidget::adjustSize(); }
	Q_INVOKABLE void moreElementsClicked()					{ Item_Editor::more_Elements_Clicked(true); }
public:
	explicit Item_Editor(QTreeWidgetItem* item, QWidget *parent = 0);

signals:
	void refresh();
	void closed();
	void edited();
private slots:
	void emit_Refresh();

public:
	QTreeWidgetItem* item;
private:
	Item_Type item_Type;

	// state variables
	bool material_Done = false;
	bool thickness_Done = false;
	bool sigma_Done = false;
	bool stack_Done = false;

private:
	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
		void create_Menu();
		void make_Ambient_Editor();
			void make_Materials_Group_Box();
public:		void initial_Radio_Check(bool temp_bool);
private:void make_Layer_Editor();
			void make_Thickness_Group_Box();
		void make_Multilayer_Editor();
			void make_Multilayer_Group_Box();
		void make_Substrate_Editor();
			void make_Sigma_Group_Box();
	void set_Window_Geometry();

public slots:
	void show_All();
	void show_Material();
	void show_Density();
	void show_Thickness();
	void show_Sigma_Start();
	void show_Sigma();
	void show_Stack_Parameters();
	void show_Interlayers();

private slots:
	void done_Slot();
	void resize_Line_Edit(QString text, QLineEdit* line_Edit = NULL);
	void browse_Material(bool);
	void filename_Radio_Toggled(bool temp_bool);
	void composition_Radio_Toggled(bool temp_bool);
	void more_Elements_Clicked(bool);
	void read_Elements_From_Item(bool temp_bool);
	void read_Interlayers_From_Item();
	void fewer_Elements_Clicked(bool);
	void norm_Interlayer_Composition();
	void interlayer_Check(int);
	void refresh_Material(QString str="");
	void refresh_Data(QString str="");
	void refresh_Stack_Data();
	void fast_Refresh_Stack(QString);

	void change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor);
	void change_Stack_Gamma(const Stack_Content& stack_Content);
		void reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness);
		void reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness);

// ui objects to be placed in launcher window
private:
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

private:
	QVBoxLayout* main_Layout;
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
						QVector<QCheckBox*> interlayer_Composition_Check_Box_Vec;
						QVector<QLineEdit*> interlayer_Composition_Line_Edit_Vec;
				QCheckBox* psd_Check_Box;
			QPushButton* sigma_Grading_Button;

		QGroupBox* multilayer_Group_Box;
			QLineEdit* repetitions_Line_Edit;
			QLabel* period_Label;
			QLineEdit* period_Line_Edit;
			QLabel* gamma_Label;
			QLineEdit* gamma_Line_Edit;

		QPushButton* done_Button;
};

#endif // ITEM_EDITOR_H
