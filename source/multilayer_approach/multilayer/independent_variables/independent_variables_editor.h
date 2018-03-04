#ifndef INDEPENDENT_VARIABLES_EDITOR_H
#define INDEPENDENT_VARIABLES_EDITOR_H

#include "standard/menu.h"

class Independent_Variables_Editor : public QDialog
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize() { QWidget::adjustSize(); }
public:
	explicit Independent_Variables_Editor(QTreeWidgetItem* structure_Item, QListWidgetItem* list_Item, QListWidget* variables_List, QWidget *parent = 0);
	~Independent_Variables_Editor();

signals:
	void refresh_Multilayer();
	void refresh_Measurement();

public:
	void refresh_All();
	void create_Main_Layout();
		void create_Shortcuts();
		void create_Menu();
		void create_Standard_Interface();
		void create_Angle_Interface();
		void create_Wavelength_Interface();
	void set_Window_Geometry();

	void show_Hide_Elements();
	void refresh_Show_Data(bool show = false);
	void activate_Variable();
	void show_Active_Check_Box();
	void show_Hide_Angular_Elements();
	void show_Hide_Spectral_Elements();

public slots:
	void resize_Line_Edit(QString text, QLineEdit* line_Edit = NULL);

public:
	int globalcounter=0;
	QTreeWidgetItem* structure_Item;
	QListWidgetItem* list_Item;
	QListWidget* variables_List;
	Independent_Indicator indicator;
	Data struct_Data;

	bool show_Data = true;
	QString units = "";
	QString name;

	QString value_Text = "value:";
	QString from_Text = "values, from";
	QString to_Text = "to";
	QString step_Text = "step size";
	QString end_Bracket_Text = "]";

	QVBoxLayout* main_Layout;
		QGroupBox* group_Box;
			QVBoxLayout* group_Box_Layout;
				QLineEdit* num_Points;
				QLabel* val_Label;
				QLineEdit* val_Edit;
				QLabel* from_Label;
				QLineEdit* min_Edit;
				QLabel* to_Label;
				QLineEdit* max_Edit;
				QLabel* units_Label;
				QLabel* step_Label;
				QLineEdit* step_Edit;
				QLabel* step_Units_Label;
				QCheckBox* active_Check_Box;

				QLabel* angular_Resolution_Label;
				QLineEdit* angular_Resolution_Edit;
				QLabel* angle_Units_Label;
				QLabel* beam_Size_Label;
				QLineEdit* beam_Size_LineEdit;
				QLabel* beam_Size_Units_Label;
				QLabel* sample_Size_Label;
				QLineEdit* sample_Size_LineEdit;
				QLabel* sample_Size_Units_Label;
				QLabel* beam_Profile_Spreading_Label;
				QLineEdit* beam_Profile_Spreading_LineEdit;
				QLabel* sample_Shift_Label;
				QLineEdit* sample_Shift_LineEdit;
				QLabel* sample_Shift_Units_Label;

				QLabel* spectral_Resolution_Label;
				QLineEdit* spectral_Resolution_Edit;
				QLabel* polarization_Label;
				QLineEdit* polarization_Edit;
				QLabel* background_Label;
				QLineEdit* background_Edit;

		QPushButton* done_Button;
};

#endif // INDEPENDENT_VARIABLES_EDITOR_H
