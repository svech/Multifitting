#ifndef MULTILAYER_H
#define MULTILAYER_H

#include "multilayer_approach/multilayer_approach.h"
#include "multilayer_approach/multilayer/structure_tree/structure_tree.h"
#include "multilayer_approach/multilayer/independent_variables/independent_variables.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve_editor.h"

class Structure_Tree;
class Multilayer_Approach;
class Target_Curve_Editor;

class Multilayer : public QWidget
{
	Q_OBJECT
public:	
	explicit Multilayer(QWidget *parent = 0);
	~Multilayer();

signals:
	void refresh_All_Multilayers();

public:
	void create_Main_Layout();
		void create_Splitter();
		void create_Structure_Frame();
		void create_Variables_Frame();
			void create_Variables_Tabs();
				void create_Independent_Variables_Tabs();
		void create_Data_Frame();

	void add_Independent_Variables_Tab();
	void change_Tab_Independent_Variables_Tab_Color(int index);
	void remove_Independent_Variables_Tab(int index);
	void rename_Independent_Variables_Tab(int tab_Index);

	void refresh_Structure_And_Independent(QObject* my_Sender = nullptr);
	void add_Target_Curve   (int index_Pressed, bool opening = false);
	void remove_Target_Curve(int index_Pressed, bool clean = false);
	void open_Import_Window(Target_Curve* target_Curve);

	void set_Index_To_Target_Curves();

	QWidget* parent;
	QVBoxLayout* main_Layout;
		QSplitter* multilayer_Splitter;
		QWidget* top_Widget_Split;
			QVBoxLayout* top_Widget_Layout;
				Structure_Tree* structure_Tree;

		QWidget* bottom_Widget_Split;
			QVBoxLayout* bottom_Widget_Layout;
				QTabWidget*	variables_Tabs;
					QTabWidget* independent_Variables_Plot_Tabs;
						QToolButton* independent_Variables_Corner_Button;

					QPushButton* structure_Table_Button;	  QPushButton* roughness_Table_Button;
					QPushButton* optical_Graphs_Button;		  QPushButton* profile_Plots_Button;
					QPushButton* calculation_Settings_Button; QPushButton* general_Settings_Button;
					QPushButton* fitting_Settings_Button;	  QPushButton* fits_Selector_Button;

				QGroupBox* data_Target_Profile_Group_Box;
					QVBoxLayout* layout_Target_Profile_With_Frame_Vector;
						QVector<QFrame*> data_Target_Profile_Frame_Vector;
						QVector<QPushButton*> add_Buttons_To_Lock;
						QVector<QPushButton*> remove_Buttons_To_Lock;
						QVector<Target_Curve*> target_Profiles_Vector;
						QMap<Target_Curve*, Target_Curve_Editor*> runned_Target_Curve_Editors;

	int num_Target_Rows;		// rows in Calculation Settings
	int num_Independent_Rows;	// rows in Calculation Settings

	Graph_Options graph_Options;

	bool enable_Calc_Target_Curves = true;
	bool enable_Calc_Independent_Curves = true;

	double min_Max_Density = 10;
	double min_Max_Thickness = 10;
	double min_Max_Sigma = 10;

	Multilayer& operator =(const Multilayer& referent_Multilayer);
};


#endif // MULTILAYER_H
