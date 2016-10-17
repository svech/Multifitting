#ifndef INDEPENDENT_VARIABLE_EDITING_H
#define INDEPENDENT_VARIABLE_EDITING_H

#include <QtWidgets>
#include "layer_content.h"
#include "global_definitions.h"

class Independent_Variable_Editing : public QDialog
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize() { QWidget::adjustSize(); }
public:
	Independent_Variable_Editing(QTreeWidgetItem* structure_Item, QListWidgetItem* item);

	QTreeWidgetItem* structure_Item;
	QListWidgetItem* item;
	QSettings gui_Settings;

private:
	void create_main_Layout();
		void create_Menu();
		void create_Standard_Interface();
		void create_Angle_Interface();
		void create_Wavelength_Interface();
	void set_Window_Geometry();

private slots:
	void resize_Line_Edit(QString text);	
	void show_Hide_Elements(int points, bool show = false);
	void refresh_Data(QString);
	void refresh_Show_Data(bool show = false);

	void show_Hide_Angular_Elements(int points, bool show = false);

	void show_Hide_Spectral_Elements(int points, bool show = false);

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

private:
	QVBoxLayout* main_Layout;
		QMenuBar* menu_Bar;
			QMenu* file_Menu;
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

#endif // INDEPENDENT_VARIABLE_EDITING_H
