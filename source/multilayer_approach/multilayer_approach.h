#ifndef MULTILAYER_APPROACH_H
#define MULTILAYER_APPROACH_H

#include "launcher.h"
#include "standard/menu.h"
#include "multilayer_approach/multilayer/multilayer.h"
#include "calculation/main_calculation_module.h"
#include "multilayer_approach/multilayer/structure_tree/regular_aperiodic_table.h"

#include "multilayer_approach/table_of_structures/table_of_structures.h"
#include "multilayer_approach/graphs/optical_graphs.h"
#include "multilayer_approach/profile_plots/profile_plots_window.h"
#include "multilayer_approach/calculation_settings_editor/calculation_settings_editor.h"
#include "multilayer_approach/general_settings_editor/general_settings_editor.h"
#include "multilayer_approach/fitting_settings/fitting_settings_editor.h"
#include "multilayer_approach/fits_selector/fits_selector.h"

class Launcher;
class Multilayer;
class Regular_Aperiodic_Table;

class Table_Of_Structures;
class Table_Of_Roughness;
class Optical_Graphs;
class Profile_Plots_Window;
class Calculation_Settings_Editor;
class General_Settings_Editor;
class Fitting_Settings_Editor;
class Fitting_Settings;
class Fits_Selector;
class Menu;

struct Regular_Aperiodic_Table_Launch
{
	id_Type item_Id = -2019;
	int tab_Index = -2019;
};

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
	void calculate(bool silent = false);
	void start_Fitting();
	void abort_Calculations();
	void calculation_Started();
	void calculation_Finished();
	void calc_Confidence_Intervals();
	void reload_Optical_Constants();

	void open_Table_Of_Structures();
	void open_Profile_Plots();
	void open_Optical_Graphs_1D(QString keep_Splitter = "");
	void open_Optical_Graphs_2D(QString keep_Splitter = "");
	void open_Calculation_Settings();
	void open_General_Settings();
	void open_Fits_Selector();
	void open_Fitting_Settings();

	void reopen_Table_Of_Structures(bool keep_Tab_Index = false);
	void reopen_Profile_Plots(bool keep_Tab_Index = false);
	void reopen_Optical_Graphs_1D(bool keep_Tab_Index = false, QString keep_Splitter = "");
	void reopen_Optical_Graphs_2D(bool keep_Tab_Index = false, QString keep_Splitter = "");
	void reopen_Calculation_Settings(bool keep_Tab_Index = false);

	void open_Regular_Aperiodic_Layers_Table(int tab_Index, QTreeWidgetItem *item);
	void temporarily_Close_Regular_Aperiodics();
	void close_Item_Editors();
	void reopen_Regular_Aperiodics();
	void lock_Mainwindow_Interface();
	void unlock_Mainwindow_Interface();

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

	void tab_Context_Menu(const QPoint& pos);
	void duplicate_Structure(const QPoint& pos);
	Multilayer* add_Multilayer(int index = -2019);
	void remove_Multilayer(int index);
	void change_Tab_Color(int index);
	void rename_Multilayer(int tab_Index);

	void catch_Critical   (QString critical_Text);
	void catch_Warning    (QString warning_Text);
	void catch_Information(QString information_Text);

	void add_Fitted_Structure(QVector<QTreeWidget*>& fitted_Trees_for_Copying, QString name_Modificator, int run=0);

	bool file_Was_Opened_or_Saved = false;
	Menu* menu;
	Launcher* launcher;					// "parent"
	QHBoxLayout* main_Layout;
		QTabWidget*	multilayer_Tabs;
		QToolButton* add_Tab_Corner_Button;

		Table_Of_Structures*		 table_Of_Structures;		  QMap<QString, Table_Of_Structures*>		  runned_Tables_Of_Structures;
		Optical_Graphs*				 optical_Graphs_1D;			  QMap<QString, Optical_Graphs*>			  runned_Optical_Graphs_1D;
		Optical_Graphs*				 optical_Graphs_2D;			  QMap<QString, Optical_Graphs*>			  runned_Optical_Graphs_2D;
		Profile_Plots_Window*		 profile_Plots_Window;		  QMap<QString, Profile_Plots_Window*>		  runned_Profile_Plots_Window;
		Calculation_Settings_Editor* calculation_Settings_Editor; QMap<QString, Calculation_Settings_Editor*> runned_Calculation_Settings_Editor;
		General_Settings_Editor*	 general_Settings_Editor;	  QMap<QString, General_Settings_Editor*>	  runned_General_Settings_Editor;
		Fitting_Settings_Editor*	 fitting_Settings_Editor;	  QMap<QString, Fitting_Settings_Editor*>	  runned_Fitting_Settings_Editor;
		Fits_Selector*				 fits_Selector;				  QMap<QString, Fits_Selector*>				  runned_Fits_Selectors;

		// 1D
		QVector<QByteArray>  target_Independent_Splitter_Position_1D_Vec;
		QVector        <QByteArray>  target_Vertical_Splitter_Position_1D_Vec;
		QVector<QVector<QByteArray>> target_Horizontal_Splitter_Position_1D_Vec_Vec;
		QVector        <QByteArray>  independent_Vertical_Splitter_Position_1D_Vec;
		QVector<QVector<QByteArray>> independent_Horizontal_Splitter_Position_1D_Vec_Vec;

		// 2D
		QVector<QByteArray>  target_Independent_Splitter_Position_2D_Vec;
		QVector        <QByteArray>  target_Vertical_Splitter_Position_2D_Vec;
		QVector<QVector<QByteArray>> target_Horizontal_Splitter_Position_2D_Vec_Vec;
		QVector        <QByteArray>  independent_Vertical_Splitter_Position_2D_Vec;
		QVector<QVector<QByteArray>> independent_Horizontal_Splitter_Position_2D_Vec_Vec;

		QVector<Fitted_Structure> fitted_Structures;
		id_Type fits_Positive_Counter = 0;

		Fitting_Settings* fitting_Settings;

		QMap<id_Type, Regular_Aperiodic_Table*> runned_Regular_Aperiodic_Tables;
		QList<Regular_Aperiodic_Table*> runned_Regular_Aperiodic_Tables_List;

		QList<Regular_Aperiodic_Table_Launch> rerun_Of_Regular_Aperiodic_Tables_List;

	bool independent_Target_Added_1D = false;
	bool independent_Added_1D = false;
	bool target_Added_1D = false;

	bool independent_Target_Added_2D = false;
	bool independent_Added_2D = false;
	bool target_Added_2D = false;
};

#endif // MULTILAYER_APPROACH_H
