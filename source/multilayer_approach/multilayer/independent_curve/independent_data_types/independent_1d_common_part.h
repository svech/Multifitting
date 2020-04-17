#ifndef INDEPENDENT_1D_COMMON_PART_H
#define INDEPENDENT_1D_COMMON_PART_H

#include "standard/mydoublespinbox.h"
#include "multilayer_approach/multilayer_approach.h"
#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"

class Multilayer_Approach;

class Independent_1D_Common_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Independent_1D_Common_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

	void create_Detector_GroupBox();
	void create_Footptint_GroupBox();
	void create_Beam_Plot();
		void plot_Beam_Profile();
	void create_Sample_Plot();
		void plot_Sample();
		void auto_Replot_Curve();

	// supplementary functions
	void refresh_Curvature_Radius();

	void connecting();

	Independent_Curve* independent_Curve;

	QVBoxLayout* main_Layout;


	// detector
	QComboBox* detector_Type_ComboBox;
	QStackedWidget* detectors_Stack;
	MyDoubleSpinBox* slit_Width_SpinBox;
	MyDoubleSpinBox* slit_Distance_SpinBox;
	MyDoubleSpinBox* crystal_Resolution_SpinBox;
	QLabel* crystal_Resolution_Units_Label;
	QComboBox* crystal_Resolution_Function_ComboBox;

	// footprint
	MyDoubleSpinBox* beam_Footprint_Width_SpinBox;
	MyDoubleSpinBox* beam_Footprint_Shape_SpinBox;
	QCustomPlot* beam_Profile_CustomPlot;
	MyDoubleSpinBox* sample_Size_SpinBox;
	MyDoubleSpinBox* sample_X_SpinBox;
	MyDoubleSpinBox* sample_Z_SpinBox;
	MyDoubleSpinBox* sample_Curvature_SpinBox;
	QLabel* R_Curvature_Label;
	QCustomPlot* sample_Profile_CustomPlot;
	QCPCurve* sample_Curve;
	QVector<QCPCurveData> curve_Data;
	QCPItemStraightLine* center_Line;
	QCPItemLine* arrow_Incident;
	QCPItemLine* arrow_Reflected;
};

#endif // INDEPENDENT_1D_COMMON_PART_H
