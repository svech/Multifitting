#ifndef MULTILAYER_APPROACH_H
#define MULTILAYER_APPROACH_H

#include "launcher.h"
#include "standard/menu.h"
#include "multilayer_approach/multilayer/multilayer.h"
#include "calculation/main_calculation_module.h"
#include "multilayer_approach/table_of_structures/table_of_structures.h"
#include "multilayer_approach/fits_selector/fits_selector.h"
#include "multilayer_approach/calculation_settings_editor/calculation_settings_editor.h"
#include "multilayer_approach/fitting_settings/fitting_settings_editor.h"
#include "multilayer_approach/graphs/optical_graphs.h"

class Launcher;
class Multilayer;
class Table_Of_Structures;
class Fits_Selector;
class Calculation_Settings_Editor;
class Fitting_Settings_Editor;
class Fitting_Settings;
class Optical_Graphs;

class Multilayer_Approach : public QWidget
{
	Q_OBJECT
public:
	explicit Multilayer_Approach(Launcher* launcher, QWidget *parent = 0);
	~Multilayer_Approach();

signals:
	void closed();

public:
	void open_Launcher();
	void refresh_All_Multilayers_View();
	void open(QString filename);
	void open_As();
	void save(QString filename);
	void save_As();
	void calc_Reflection(bool silent = false);
	void start_Fitting();
	void abort_Calculations();
	void calculation_Started();
	void calculation_Finished();
	void calc_Confidence_Intervals();
	void reload_Optical_Constants();
	void open_Table_Of_Structures();
	void open_Optical_Graphs(QString keep_Splitter = "");
	void open_Fits_Selector();
	void open_Calculation_Settings();
	void open_Fitting_Settings();

	void closeEvent(QCloseEvent *event);
	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);
	void create_Main_Layout();
		void create_Menu();
		void create_Multilayer_Tabs();
		void create_Fitting_Settings();
	void set_Window_Geometry();	
	void write_Window_Geometry();
	void fast_Hide_Windows();

	void add_Multilayer();
	void remove_Multilayer(int index);
	void change_Tab_Color(int index);
	void rename_Multilayer(int tab_Index);

	void catch_Critical   (QString critical_Text);
	void catch_Warning    (QString warning_Text);
	void catch_Information(QString information_Text);

	void add_Fitted_Structure(QVector<QTreeWidget*>& fitted_Trees_for_Copying, QString name_Modificator, int run=0);

	bool file_Was_Opened = false;
	Menu* menu;
	Launcher* launcher;					// "parent"
	QHBoxLayout* main_Layout;
		QTabWidget*	multilayer_Tabs;
		QToolButton* add_Tab_Corner_Button;

		QMap<QString, Table_Of_Structures*> runned_Tables_Of_Structures;
		Table_Of_Structures* table_Of_Structures;

		QMap<QString, Optical_Graphs*> runned_Optical_Graphs;
		Optical_Graphs* optical_Graphs;
		QVector<QByteArray>  target_Independent_Splitter_Position_Vec;
		QVector        <QByteArray>  target_Vertical_Splitter_Position_Vec;
		QVector<QVector<QByteArray>> target_Horizontal_Splitter_Position_Vec_Vec;
		QVector        <QByteArray>  independent_Vertical_Splitter_Position_Vec;
		QVector<QVector<QByteArray>> independent_Horizontal_Splitter_Position_Vec_Vec;

		QMap<QString, Fits_Selector*> runned_Fits_Selectors;
		Fits_Selector* fits_Selector;
		QVector<Fitted_Structure> fitted_Structures;
		id_Type fits_Positive_Counter = 0;

		QMap<QString, Calculation_Settings_Editor*> runned_Calculation_Settings_Editor;
		Calculation_Settings_Editor* calculation_Settings_Editor;

		QMap<QString, Fitting_Settings_Editor*> runned_Fitting_Settings_Editor;
		Fitting_Settings_Editor* fitting_Settings_Editor;
		Fitting_Settings* fitting_Settings;
};

#endif // MULTILAYER_APPROACH_H
