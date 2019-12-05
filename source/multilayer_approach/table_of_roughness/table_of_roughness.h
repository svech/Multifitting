#ifndef TABLE_OF_ROUGHNESS_H
#define TABLE_OF_ROUGHNESS_H

#include "global/settings.h"
#include "multilayer_approach/table_of_structures/table_of_structures.h"
//#include "multilayer_approach/calculation_settings_editor/calculation_settings_editor.h"

//class Structure_Tree;
//class MyDoubleSpinBox;
//class My_Table_Widget;
//class Calculation_Settings_Editor;
//class Layer_Thickness_Transfer;

class Table_Of_Roughness : public QWidget
{
	Q_OBJECT
public:
	explicit Table_Of_Roughness(QWidget *parent = nullptr);

	void closeEvent(QCloseEvent* event);

	void create_Main_Layout();
	void set_Window_Geometry();
	void write_Window_Geometry();
	void create_Menu();
	void create_Tabs();
		void add_Tabs();
		void create_Tab_Content(QWidget* new_Widget, int tab_Index);


	QVBoxLayout* main_Layout;
	Menu* menu;
	QTabWidget* main_Tabs;

};

#endif // TABLE_OF_ROUGHNESS_H
