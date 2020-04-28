#ifndef CURVE_PLOT_2D_H
#define CURVE_PLOT_2D_H

#include "multilayer_approach/multilayer_approach.h"

class MySplitter : public QSplitter
{
public:
	using QSplitter::moveSplitter;
};

class Curve_Plot_2D : public QWidget
{
public:
	explicit Curve_Plot_2D(Multilayer* multilayer, Target_Curve* target_Curve, Independent_Curve* independent_Curve, QString curve_Class, QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Plot_Frame_And_Scale();
	void create_Left_Part();
	void create_Bottom_Part();
	void refresh_Axes_Range();
	void apply_Log_Scale();
	void apply_Lin_Scale();
	void plot_All_Data();
	void plot_Data();
	void refresh_Labels();

	void create_Plot_Options_GroupBox();
	void create_Subinterval_Rectangle();
	void subinterval_Changed_Replot();

	vector<vector<double>>* values_2D;
	vector<vector<double>>* values_2D_Meas;
	vector<vector<double>>* values_2D_Calc;

	Multilayer* multilayer;
	Target_Curve* target_Curve;
	Independent_Curve* independent_Curve;
	QString curve_Class;

	// common fields
	Data& measurement;
	Calculated_Values& calculated_Values;
	Plot_Options& plot_Options;
	QString& spectral_Units;
	QString& angular_Units;
	QString& plot_Indicator;

	// main objects
	QVBoxLayout* main_Layout;
	MySplitter* bottom_Splitter;
	MySplitter* left_Splitter;
	MySplitter* bottom_Left_Splitter;

	QCustomPlot* main_2D_Custom_Plot;
	QCPColorMap* color_Map;
	QCPColorScale* color_Scale;
	QCPMarginGroup* margin_Group_Top_Bottom;
	QCPMarginGroup* margin_Group_Left_Right;

	// sections
	QTabWidget* bottom_Section_Tabs;
	QWidget* empty_Widget;
	QHBoxLayout* bottom_Hor_Layout;
	QHBoxLayout* bottom_Ver_Layout;
	QCustomPlot* bottom_Horizontal_Custom_Plot;
	QCustomPlot* bottom_Vertical_Custom_Plot;
	QCustomPlot* left_Vertical_Custom_Plot;
	double shift;

	// fitting rectangle
	QCPItemRect* start_Rect;
	QCPItemRect* end_Rect;
	QCPItemRect* top_Rect;
	QCPItemRect* bottom_Rect;

	// axes text
	QString value_Type_Text;
	QString argument_Type_Text;
	QString argument_Sym_Text;
	QString argument_Units_Text;
};

#endif // CURVE_PLOT_2D_H
