#ifndef GENERAL_SETTINGS_EDITOR_H
#define GENERAL_SETTINGS_EDITOR_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/multilayer.h"

class General_Settings_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit General_Settings_Editor(QWidget *parent = nullptr);

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
	void create_Output_Tab();
	void create_Calculation_Tab();
	void create_Interface_Tab();
	void set_Window_Geometry();
	void write_Window_Geometry();

	QVBoxLayout* main_Layout;
	QTabWidget* main_Tabs;
	QPushButton* close_Button;
	QCheckBox* structure_Table_Recalculation_Checkbox;
        QLineEdit* last_LineEdit;
};

#endif // GENERAL_SETTINGS_EDITOR_H
