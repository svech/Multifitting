#ifndef CURVE_PLOT_H
#define CURVE_PLOT_H

#include "multilayer_approach/multilayer_approach.h"

class Curve_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Curve_Plot(Multilayer* multilayer, Target_Curve* target_Curve, Independent_Variables* independent_Variables, QString curve_Class, QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Subinterval_Rectangle();
	void subinterval_Changed_Replot();
	void create_Plot_Frame_And_Scale();
	void create_Options();
	void set_Title_Text();
	void plot_All_Data();
	void plot_Data(const QVector<double>& argument, const QVector<double>& values, Plot_Options* plot_Options, QString left_Right);
	void refresh_Labels();
	QCPGraph* get_Selected_Graph();
	void choose_Graph_Color();
	void set_Graph_Color(QCPGraph *graph, QColor color);
	void colorize_Color_Button();
	void show_Thickness();
	void show_Scatter_Size();
	void change_Thickness();
	void change_Scatter_Size();
	void get_Min_Max_For_Graph(Plot_Options* plot_Options, QString left_Right, const QVector<double>& values, double& minimum, double& maximum);

	QVector<double> argument;
	QVector<double> values;
	QString curve_Class;
	QString* argument_Type;
	QString* spectral_Units;
	QString* angle_Type;
	QString* angular_Units;

	Multilayer* multilayer;
	Target_Curve* target_Curve;
	Independent_Variables* independent_Variables;
	double min_Value_Left = DBL_MAX;
	double max_Value_Left = -DBL_MAX;
	double min_Value_Right = DBL_MAX;
	double max_Value_Right = -DBL_MAX;

	// common fields
	Data* measurement;
	Calculated_Values* calculated_Values;
	Plot_Options* plot_Options_First;
	Plot_Options* plot_Options_Second;
	QString* plot_Indicator;

	QColorDialog* colorDialog;
	QCPGraph* selected_Graph;
	QMap<QCPGraph*, Plot_Options*> graph_Options_Map;
	QMap<QCPGraph*, QString> graph_Left_Right_Map;

	QVBoxLayout* main_Layout;
		QCustomPlot* custom_Plot;
			QCPItemRect* start_Rect;
			QCPItemRect* end_Rect;
		QCPTextElement* plot_Title;
		QGroupBox* options_GroupBox;
			QHBoxLayout* options_Layout;

				QLabel* scale_Y_Label;
				QButtonGroup* Y_ButtonGroup;
				QRadioButton* lin_Y_RadioButton;
				QRadioButton* log_Y_RadioButton;

				QLabel* scale_X_Label;
				QButtonGroup* X_ButtonGroup;
				QRadioButton* lin_X_RadioButton;
				QRadioButton* log_X_RadioButton;

				QPushButton*	colors_Button;	QPalette default_Colors_Button_Palette;
				QComboBox*		symbol_ComboBox;

				QLabel* scatter_Label;
				QDoubleSpinBox* scatter_Spin;

				QLabel* thickness_Label;
				QDoubleSpinBox* thickness_Spin;

				QCheckBox* rescale_Check_Box;

				QLabel* max_Value_Label;
				QString max_Value_Title;
				QString argument_Units;

	bool graph_Done = false;

	QString val_Type_Label;
	QString val_Mode_Label_1;
	QString val_Mode_Label_2;

	QString argument_Type_Label;
	QString argument_Label;
};

#endif // CURVE_PLOT_H
