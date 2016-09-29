#ifndef ITEM_EDITING_H
#define ITEM_EDITING_H

#include <QtWidgets>
#include "layer_content.h"
#include "global_definitions.h"

class Item_Editing : public QWidget
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize() { QWidget::adjustSize(); }
	Q_INVOKABLE void moreElementsClicked() { Item_Editing::more_Elements_Clicked(true); }
public:
	Item_Editing(QTreeWidgetItem* item);

	QTreeWidgetItem* item;

	QSettings gui_Settings;
	QSettings default_Values;

signals:
	void is_Closed();
private:
	void closeEvent(QCloseEvent* event);

private:
	Item_Type item_Type;
	QMap<QString,double> sorted_Elements;

private:
	void create_Main_Layout();
		void create_Menu();
		void make_Ambient_Editor();
			void make_Materials_Group_Box();
		void make_Layer_Editor();
			void make_Thickness_Group_Box();
		void make_Multilayer_Editor();
		void make_Substrate_Editor();
			void make_Sigma_Group_Box();
	void set_Window_Geometry();

// state variables
private:
	bool material_Done = false;
	bool thickness_Done = false;
	bool sigma_Done = false;
	QStringList transition_Layers_Available = transition_Layer_Functions;

private slots:
	void filename_Radio_Toggled(bool);
	void composition_Radio_Toggled(bool);
	void more_Elements_Clicked(bool);
	void read_Elements_From_Item();
	void read_Interlayers_From_Item();
	void fewer_Elements_Clicked(bool);
	void more_Interlayer_Functions_Clicked(bool);
	void fewer_Interlayer_Functions_Clicked(bool);
	void refresh_Material();
	void show_Material();
	void show_Density();
	void show_Thickness();
	void show_Sigma();
	void show_Or_Hide_Interlayer_Buttons();
	void norm_Interlayer_Composition();
	void refresh_Data();
	void refresh_Data(QString);

// ui objects to be placed in launcher window
private:
	QVBoxLayout* main_Layout;
		QMenuBar* menu_Bar;
			QMenu* file_Menu;
		QGroupBox*	material_Group_Box;
			QLineEdit* material_Line_Edit;
			QPushButton* browse_Material_Button;
			QLineEdit* density_Line_Edit;
			QRadioButton* filename_Radio;
			QRadioButton* composition_Radio;
			QFrame* composition_Frame;
				QGroupBox*	composition_Group_Box;
					QVBoxLayout* composition_Layout_With_Elements_Vector;
						QVector<QFrame*> element_Frame_Vec;
							QVector<QLineEdit*> composition_Line_Edit_Vec;
							QVector<QLabel*>    composition_Label_Vec;
							QVector<QComboBox*> composition_Combo_Box_Vec;
							QVector<QLabel*>    composition_At_Weight_Vec;
				QPushButton* more_Elements;
				QPushButton* fewer_Elements;
		QGroupBox*	thickness_Group_Box;
			QLineEdit* thickness_Line_Edit;
			QPushButton* depth_Grading_Button;
		QGroupBox*	sigma_Group_Box;
			QLineEdit* sigma_Line_Edit;
			QFrame* interlayer_Composition_Frame;
				QGroupBox* interlayer_Composition_Group_Box;
					QVBoxLayout* interlayer_Composition_Layout_With_Elements_Vector;
						QVector<QLineEdit*> interlayer_Composition_Line_Edit_Vec;
						QVector<QLabel*>    interlayer_Composition_Label_Vec;
						QVector<QComboBox*> interlayer_Composition_Combo_Box_Vec;
				QPushButton* more_Interlayer_Functions;
				QPushButton* fewer_Interlayer_Functions;

		QPushButton* done_Button;
};

#endif // ITEM_EDITING_H
