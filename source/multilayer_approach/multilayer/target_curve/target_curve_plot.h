#ifndef TARGET_CURVE_PLOT_H
#define TARGET_CURVE_PLOT_H

#include "target_curve.h"

class Target_Curve_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Target_Curve_Plot(Target_Curve* target_Curve, QWidget *parent = nullptr);

	void create_Plot_Frame_And_Scale();
	void create_Subinterval_Rectangle();
	void subinterval_Changed_Replot();
	void plot_Data(bool fast = false);
	void refresh_Labels();

	Target_Curve* target_Curve;
	QCustomPlot* custom_Plot;

	QCPItemRect* start_Rect;
	QCPItemRect* end_Rect;

	QString val_Type_Label;
	QString val_Mode_Label_1;
	QString val_Mode_Label_2;

	QString argument_Type_Label;
	QString argument_Label;

};

#endif // TARGET_CURVE_PLOT_H
