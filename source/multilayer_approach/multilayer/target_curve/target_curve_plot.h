#ifndef TARGET_CURVE_PLOT_H
#define TARGET_CURVE_PLOT_H

#include "target_curve.h"

class Target_Curve_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Target_Curve_Plot(Target_Curve* target_Curve, QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Plot();
	void create_Plot_Frame_And_Scale_1D();
	void create_Subinterval_Rectangle();
	void subinterval_Changed_Replot();
	void plot_Data(bool fast = false);
	void refresh_Labels();
	void create_Plot_Options_GroupBox();

	Target_Curve* target_Curve;

	QCPColorMap* color_Map;
	QCustomPlot* custom_Plot;

	QCPItemRect* start_Rect;
	QCPItemRect* end_Rect;

	QCPItemRect* top_Rect;
	QCPItemRect* bottom_Rect;

	QVBoxLayout* main_layout;

	// axes text
	QString value_Type_Text;
	QString argument_Type_Text;
	QString argument_Sym_Text;
	QString argument_Units_Text;
};

#endif // TARGET_CURVE_PLOT_H
