#ifndef CALCULATION_SETTINGS_H
#define CALCULATION_SETTINGS_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer.h"

class Table_Of_Structures;

class Calculation_Settings_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Calculation_Settings_Editor(Multilayer_Approach* multilayer_Approach, QWidget *parent = nullptr);

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
	void set_Window_Geometry();
	void create_Menu();
	void create_Tabs();
	void lock_Mainwindow_Interface();
	void unlock_Mainwindow_Interface();
	void add_Tabs();
		void create_Tab_Content(QWidget* new_Widget, int tab_Index);
			void load_Target_Parameters(int tab_Index);
			void load_Independent_Parameters(int tab_Index);

	void refresh_Independent_Calc_Properties(int tab_Index, int independent_Index, QGroupBox* box);

	Multilayer_Approach* multilayer_Approach;
	QMap<QString, Table_Of_Structures*>& runned_Tables_Of_Structures;
	QMap<QString, Calculation_Settings_Editor*>& runned_Calculation_Settings_Editor;
	QTabWidget* multilayer_Tabs;

	QVBoxLayout* main_Layout;
	QTabWidget* main_Tabs;
		QVector<QGroupBox*> target_Group_Box_Vec;
			QVector<QFrame*> target_Frame_Vec;
		QVector<QGroupBox*> independent_Group_Box_Vec;
			QVector<QFrame*> independent_Frame_Vec;

	QVector<QLineEdit*> different_Lines; // for editingFinished calling from menu

	QPushButton* global_Norm_Button;
	QPushButton* optimization_Method_Button;
	QPushButton* done_Button;
};

#endif // CALCULATION_SETTINGS_H
