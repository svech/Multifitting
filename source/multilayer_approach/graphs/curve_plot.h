#ifndef CURVE_PLOT_H
#define CURVE_PLOT_H

#include "multilayer_approach/multilayer_approach.h"

class Curve_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Curve_Plot(Target_Curve* target_Curve, Independent_Variables* independent_Variables, QString curve_Class, QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Plot_Frame_And_Scale();
	void create_Options();

	QString curve_Class;
	Target_Curve* target_Curve;
	Independent_Variables* independent_Variables;

	// common fields
	Data* measurement;
	Calculated_Values* calculated_Values;
	Plot_Options* plot_Options;

	QVBoxLayout* main_Layout;
		QCustomPlot* custom_Plot;
		QGroupBox* options_GroupBox;
			QHBoxLayout* options_Layout;

				QLabel* scale_Label;
				QRadioButton* lin_RadioButton;
				QRadioButton* log_RadioButton;

				QPushButton*	colors_Button;
				QComboBox*		symbol_ComboBox;

				QLabel* thickness_Label;
				QDoubleSpinBox* thickness_Spin;

};

#endif // CURVE_PLOT_H
