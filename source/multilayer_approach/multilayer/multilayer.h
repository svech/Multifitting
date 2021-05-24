#ifndef MULTILAYER_H
#define MULTILAYER_H

#include "multilayer_approach/multilayer_approach.h"
#include "multilayer_approach/multilayer/structure_tree/structure_tree.h"
#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve_editor.h"

class Structure_Tree;
class Multilayer_Approach;
class Target_Curve_Editor;
class Independent_Curve_Editor;

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
		void create_Structure_Tree();
		void create_Main_Tools();
		void create_Independent_Variables_Tabs();
		void create_Data_Frame();

	Independent_Curve* add_Independent_Variables_Tab(int index = -2021);
	void change_Tab_Independent_Variables_Tab_Color(int index);
	void remove_Independent_Variables_Tab(int index);
	void rename_Independent_Variables_Tab(int tab_Index);
	void independent_Tab_Context_Menu(const QPoint& pos);
	void duplicate_Independent_Curve(const QPoint& pos);
	void refresh_Structure_And_Independent(QObject* my_Sender = nullptr);

	Target_Curve* add_Target_Curve (int index_Pressed, bool opening = false);
	void target_Curve_Context_Menu(const QPoint& pos);
	void duplicate_Target_Curve(int old_Index);
	void remove_Target_Curve(int index_Pressed, bool clean = false);
	template<typename Type_Curve, typename Type_Curve_Editor>
	void open_Editor_Window(Type_Curve* type_Curve, Type_Curve_Editor* type_Editor);
	void set_Index_To_Target_Curves();

	QWidget* parent;
	QVBoxLayout* main_Layout;
		Structure_Tree* structure_Tree;
			QVBoxLayout* bottom_Part_Layout;

				QTabWidget* independent_Curve_Tabs;
					QToolButton* independent_Variables_Corner_Button;
					QMap<Independent_Curve*, Independent_Curve_Editor*> runned_Independent_Curve_Editors;

					QPushButton* structure_Table_Button;
					QPushButton* profile_Plots_Button;		  QPushButton* roughness_Plots_Button;
					QPushButton* optical_Graphs_1D_Button;	  QPushButton* optical_Graphs_2D_Button;
					QPushButton* calculation_Settings_Button; QPushButton* general_Settings_Button;
					QPushButton* fitting_Settings_Button;	  QPushButton* fits_Selector_Button;

				QGroupBox* data_Target_Profile_Group_Box;
					QVBoxLayout* layout_Target_Profile_With_Frame_Vector;
						QVector<QFrame*> data_Target_Profile_Frame_Vector;
						QVector<QLabel*> description_Label_Vector;
						QVector<Target_Curve*> target_Profiles_Vector;
						QMap<Target_Curve*, Target_Curve_Editor*> runned_Target_Curve_Editors;

	int num_Target_Rows;		// rows in Calculation Settings
	int num_Independent_Rows;	// rows in Calculation Settings

	Graph_Options graph_Options_1D;
	Graph_Options graph_Options_2D;
	Profile_Plot_Options profile_Plot_Options;
	Discretization_Parameters discretization_Parameters;
	Imperfections_Model imperfections_Model;

	bool enable_Calc_Target_Curves = true;
	bool enable_Calc_Independent_Curves = true;

	double min_Max_Density = 10;
	double min_Max_Thickness = 10;
	double min_Max_Sigma = 10;

	Multilayer& operator =(const Multilayer& referent_Multilayer);
};


#endif // MULTILAYER_H
