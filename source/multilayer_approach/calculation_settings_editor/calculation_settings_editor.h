#ifndef CALCULATION_SETTINGS_H
#define CALCULATION_SETTINGS_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/multilayer.h"

//class Table_Of_Structures;

class Calculation_Settings_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Calculation_Settings_Editor(QWidget *parent = nullptr);

	void contextMenuEvent(QContextMenuEvent* event);
	void settings();

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
	void set_Window_Geometry();
	void write_Window_Geometry();
	void create_Tabs();
	void add_Tabs();
		void create_Tab_Content(QWidget* new_Widget, int tab_Index);
			void load_Discretization_Parameters(int tab_Index);
			void load_Target_Parameters(int tab_Index);
			void load_Independent_Parameters(int tab_Index);

	void refresh_Independent_Calc_Properties(int tab_Index, int independent_Index, QGroupBox* box);
	static void reopen_Optical_Graphs_1D(QString keep_Splitter = "");
	static void reopen_Optical_Graphs_2D(QString keep_Splitter = "");

	vector<int> total_Number_of_Targets;
	vector<int> total_Number_of_Independents;

	QTabWidget* multilayer_Tabs;

	QVBoxLayout* main_Layout;
	QTabWidget* main_Tabs;
		QVector<QGroupBox*> discretization_Group_Box_Vec;
		QVector<QGroupBox*> target_Group_Box_Vec;
		QMap<Target_Curve*,QCheckBox*> max_Integral_Map;
		QVector<QGroupBox*> independent_Group_Box_Vec;

	QPushButton* done_Button;
	QPushButton* global_Norm_Button;
};

#endif // CALCULATION_SETTINGS_H
