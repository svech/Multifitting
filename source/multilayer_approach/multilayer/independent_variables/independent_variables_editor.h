#ifndef INDEPENDENT_VARIABLES_EDITOR_H
#define INDEPENDENT_VARIABLES_EDITOR_H

#include "standard/menu.h"

class Independent_Variables_Editor : public QDialog
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize() { QWidget::adjustSize(); }
public:
	explicit Independent_Variables_Editor(QTreeWidgetItem* structure_Item, QListWidgetItem* item, QListWidget* variables_List, QWidget *parent = 0);

signals:
	void refresh_Multilayer();
private slots:
	void refresh_All();

private:
	void create_Main_Layout();
		void create_Menu();
		void create_Standard_Interface();
		void create_Angle_Interface();
		void create_Wavelength_Interface();
	void set_Window_Geometry();

public slots:
	void resize_Line_Edit(QString text, QLineEdit* line_Edit = NULL);
	void refresh_Special_Line_Edit();
	void show_Hide_Elements(int points, bool show = false);
	void refresh_Data();
	void refresh_Show_Data(bool show = false);
	void activate_Variable(bool);
	void show_Active_Check_Box();
	void show_Hide_Angular_Elements(int points, bool show = false);
	void show_Hide_Spectral_Elements(int points, bool show = false);

public:
	QTreeWidgetItem* structure_Item;
	QListWidgetItem* item;
	QListWidget* variables_List;

private:
	bool angle_Done = false;
	bool wavelength_Done = false;
	bool show_Data = true;
	QString units = "nm";
	QString name;

	QString from_One = "value:";
	QString from_Many = "values, from";
	QString to = "to";
	QString step = "step size";

	QString end_Bracket = "]";
private:
	QVBoxLayout* main_Layout;
		QGroupBox* group_Box;
			QVBoxLayout* group_Box_Layout;
				QLineEdit* num_Points;
				QLabel* from_Label;
				QLineEdit* min;
				QLabel* to_Label;
				QLineEdit* max;
				QLabel* units_Label;
				QLabel* step_Label;
				QLineEdit* step_Edit;
				QLabel* step_Units_Label;
				QCheckBox* active_Check_Box;

				QLabel* angular_Resolution_Label;
				QLineEdit* angular_Resolution_Edit;
				QLabel* angle_Units_Label;

				QLabel* spectral_Resolution_Label;
				QLineEdit* spectral_Resolution_Edit;
				QLabel* wavelength_Units_Label;
				QLabel* polarization_Label;
				QLineEdit* polarization_Edit;
				QLabel* analyzer_Label;
				QLineEdit* analyzer_Edit;

		QPushButton* done_Button;
};

#endif // INDEPENDENT_VARIABLES_EDITOR_H
