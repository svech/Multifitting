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

private:
	Item_Type item_Type;

private:
	void create_Main_Layout();
		void create_Menu();
		void make_Ambient_Editor();
			void make_Materials_Group_Box();
		void make_Layer_Editor();
		void make_Multilayer_Editor();
		void make_Substrate_Editor();
	void set_Window_Geometry();



private slots:
	void filename_Radio_Toggled(bool);
	void composition_Radio_Toggled(bool);
	void more_Elements_Clicked(bool);
	void fewer_Elements_Clicked(bool);
	void refresh_Data();
	void refresh_Data(QString);

// ui objects to be placed in launcher window
private:
	QVBoxLayout* main_Layout;
		QMenuBar* menu_Bar;
			QMenu* file_Menu;
		QGroupBox*	material_Group_Box;
			QVBoxLayout* material_Group_Box_Layout;
				QLineEdit* material_Line_Edit;
				QPushButton* browse_Material_Button;
				QLineEdit* density_Line_Edit;
				QRadioButton* filename_Radio;
				QRadioButton* composition_Radio;
				QFrame* composition_Frame;
					QVBoxLayout* composition_Layout;
						QGroupBox*	composition_Group_Box;
							QVBoxLayout* composition_Layout_With_Elements_Vector;
								QList<QFrame*> element_Frame_Vec;
									QList<QLineEdit*> composition_Line_Edit_Vec;
									QList<QLabel*>    composition_Label_Vec;
									QList<QComboBox*> composition_Combo_Box_Vec;
									QList<QLabel*>    composition_At_Weight_Vec;
						QPushButton* more_Elements;
						QPushButton* fewer_Elements;

		QPushButton* done_Button;
};

#endif // ITEM_EDITING_H
