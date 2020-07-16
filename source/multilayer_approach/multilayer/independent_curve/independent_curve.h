#ifndef INDEPENDENT_CURVE_H
#define INDEPENDENT_CURVE_H

#include "global/layer_data_types.h"

class Independent_Curve : public QWidget
{
	Q_OBJECT
public:
	explicit Independent_Curve(QWidget *parent = 0);

	void create_Main_Layout();
	QString enlarge_Tab_Name();
	void refresh_Description_Label();
	void calc_Independent_cos2_k(double angle_Shift = 0, double lambda_Shift = 0);
	void calc_cos2_k(double angle_Shift = 0, double lambda_Shift = 0);

	QHBoxLayout* main_Layout;
	QPushButton* setup_Button;
	QLabel* description_Label;

	QString tab_Name;

	Data measurement;
	Calculated_Values calculated_Values;
	vector<Simple_Curve> additional_Curves; // without saving

	Calc_Functions calc_Functions;
	Plot_Options plot_Options;

	// units
	QString angular_Units;
	QString spectral_Units;

	QString label_Text = "<no description>";

	// 2D positions
	Graph_2D_Positions graph_2D_Positions;

	Independent_Curve& operator =(const Independent_Curve& referent_Independent_Curve);
};

QDataStream& operator <<( QDataStream& stream, const Independent_Curve* independent_Curve );
QDataStream& operator >>( QDataStream& stream,	     Independent_Curve* independent_Curve );

#endif // INDEPENDENT_CURVE_H
