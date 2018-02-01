#ifndef MULTILAYER_APPROACH_H
#define MULTILAYER_APPROACH_H

#include "launcher.h"
#include "standard/menu.h"
#include "multilayer_approach/multilayer.h"
#include "main_calculation_module.h"
#include "multilayer_approach/table_of_structures.h"
#include "calculation/fitting/fits_selector.h"
#include "calculation/calculation_settings_editor.h"

class Launcher;
class Multilayer;
class Table_Of_Structures;
class Calculation_Settings_Editor;
class Fits_Selector;

class Multilayer_Approach : public QWidget
{
	Q_OBJECT
public:
	explicit Multilayer_Approach(Launcher* launcher, QWidget *parent = 0);

signals:
	void closed();

public:
	void open_Launcher();
	void refresh_All_Multilayers_View();
	void open();
	void save();
	void calc_Reflection();
	void start_Fitting();
	void reload_Optical_Constants();
	void open_Table_Of_Structures();
	void open_Calculation_Settings();
	void open_Fits_Selector();

	void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
		void create_Menu();
		void create_Multilayer_Tabs();
	void set_Window_Geometry();


	void add_Multilayer();
	void remove_Multilayer(int index);
	void change_Tab_Color(int index);
	void rename_Multilayer(int tab_Index);

	void catch_Critical   (QString critical_Text);
	void catch_Warning    (QString warning_Text);
	void catch_Information(QString information_Text);

	void add_Fitted_Structure(QVector<QTreeWidget*>& fitted_Trees_for_Copying);

	Menu* menu;
	Launcher* launcher;					// "parent"
	QHBoxLayout* main_Layout;
		QTabWidget*	multilayer_Tabs;
		QToolButton* add_Tab_Corner_Button;

		QMap<QString, Table_Of_Structures*>  runned_Tables_Of_Structures;
		Table_Of_Structures* table_Of_Structures;

		QMap<QString, Calculation_Settings_Editor*> runned_Calculation_Settings_Editor;
		Calculation_Settings_Editor* calculation_Settings;

		QMap<QString, Fits_Selector*>  runned_Fits_Selectors;
		Fits_Selector* fits_Selector;
		QVector<Fitted_Structure> fitted_Structures;
};

#endif // MULTILAYER_APPROACH_H
