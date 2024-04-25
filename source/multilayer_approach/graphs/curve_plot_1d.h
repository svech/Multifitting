#ifndef CURVE_PLOT_H
#define CURVE_PLOT_H

#include "multilayer_approach/multilayer_approach.h"

class Curve_Plot_1D : public QWidget
{
	Q_OBJECT
public:
	explicit Curve_Plot_1D(Multilayer* multilayer, Target_Curve* target_Curve, Independent_Curve* independent_Curve, QString curve_Class, QWidget *parent = nullptr);

	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);

	void create_Main_Layout();
	void create_Subinterval_Rectangle();
	void subinterval_Changed_Replot();
	void discretized_Threshold_Line();

	void create_Context_Menu();
	void create_Plot_Frame_And_Scale();
		void apply_Log_Scale(QString xy);
		void apply_Lin_Scale(QString xy);
	void create_Options();

	void set_Title_Text();
	void plot_All_Data();
    void plot_Data(const vector<double>& argument, const vector<double>& values, Plot_Options plot_Options, int graph_Index);
    void refresh_Labels();
	QCPGraph* get_Selected_Graph();
	void choose_Graph_Color();
	void set_Graph_Color(QCPGraph *graph, QColor color);
	void show_Thickness();
	void show_Scatter_Size();
	void change_Thickness();
	void change_Scatter_Size();
	void get_Min_Max_For_Graph(QString y_Scale, const vector<double>& values, double& minimum, double& maximum);

	vector<double> argument;
	vector<double> values;
	QString curve_Class;

	Multilayer* multilayer;
	Target_Curve* target_Curve;
	Independent_Curve* independent_Curve;
	double min_Value_Left = DBL_MAX;
	double max_Value_Left = -DBL_MAX;
	double min_Value_Right = DBL_MAX;
	double max_Value_Right = -DBL_MAX;

	// common fields
	Data& measurement;
	Calculated_Values& calculated_Values;
	Plot_Options& plot_Options_First;
	Plot_Options& plot_Options_Second;
	QString& spectral_Units;
	QString& angular_Units;
	QString& plot_Indicator;
	vector<Simple_Curve>& additional_Curves;


	QColorDialog* colorDialog;
	QCPGraph* selected_Graph;
	QMap<QCPGraph*, Plot_Options*> graph_Options_Map;

    QCPGraph* main_first_graph;
    QCPErrorBars* error_Bars;
    QCPGraph* lower_graph;
    QCPGraph* upper_graph;

	QVBoxLayout* main_Layout;
	QCustomPlot* custom_Plot;
	QCPItemRect* start_Rect;
	QCPItemRect* end_Rect;
	QCPItemStraightLine* infLine;
	QCPTextElement* plot_Title;

	QDoubleSpinBox* scatter_Spin;
	QDoubleSpinBox* thickness_Spin;
	QDoubleSpinBox* range_Spin;

        QLabel* max_Value_Label;
        QLabel* FWFM_Label;
        QString max_Value_Title;

	bool graph_Done = false;
};

#endif // CURVE_PLOT_H
