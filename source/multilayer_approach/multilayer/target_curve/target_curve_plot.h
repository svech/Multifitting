#ifndef TARGET_CURVE_PLOT_H
#define TARGET_CURVE_PLOT_H

#include "target_curve.h"

class Target_Curve_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Target_Curve_Plot(Target_Curve* target_Curve, QWidget *parent = nullptr);

	void create_Main_Layout();
	void plot_Data();

	// 1D
    static void define_Bars(Target_Curve* target_Curve, QCustomPlot *custom_Plot, QCPGraph* main_graph, QCPErrorBars *&error_Bars, QCPGraph *&lower_graph, QCPGraph *&upper_graph);
    static void plot_Bars(Target_Curve* target_Curve, QCPErrorBars* error_Bars, QCPGraph* lower_graph, QCPGraph* upper_graph, double& min, double& max, QString y_Scale);
    void create_Plot_Frame_And_Scale_1D();
    void apply_Log_Scale_1D();
	void apply_Lin_Scale_1D();
	void plot_Data_1D();
	void refresh_Labels_1D();
	void create_Plot_Options_GroupBox_1D();
	void create_Subinterval_Rectangle_1D();
	void subinterval_Changed_Replot_1D();

	// 2D
	void create_Plot_Frame_And_Scale_2D();
	void refresh_Axes_Range_2D();
	void apply_Log_Scale_2D();
	void apply_Lin_Scale_2D();
	void plot_Data_2D();
	void refresh_Labels_2D();
	void create_Plot_Options_GroupBox_2D();
	void create_Subinterval_Rectangle_2D();
	void subinterval_Changed_Replot_2D();

	Target_Curve* target_Curve;

    QCPGraph* main_graph;
    QCPErrorBars* error_Bars;
    QCPGraph* lower_graph;
    QCPGraph* upper_graph;

	QCustomPlot* custom_Plot;
	QCPColorMap* color_Map;
	QCPColorScale* color_Scale;
	QCPMarginGroup* margin_Group;

	QCPItemRect* start_Rect;
	QCPItemRect* end_Rect;

	QCPItemRect* top_Rect;
	QCPItemRect* bottom_Rect;

	QVBoxLayout* main_Layout;

	// axes text
	QString value_Type_Text;
	QString argument_Type_Text;
	QString argument_Sym_Text;
	QString argument_Units_Text;
};

#endif // TARGET_CURVE_PLOT_H
