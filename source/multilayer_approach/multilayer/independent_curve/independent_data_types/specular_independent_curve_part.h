#ifndef SPECULAR_INDEPENDENT_CURVE_PART_H
#define SPECULAR_INDEPENDENT_CURVE_PART_H

#include "standard/mydoublespinbox.h"
#include "multilayer_approach/multilayer_approach.h"
#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"

class Multilayer_Approach;

class Specular_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Specular_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

	void create_Units_GroupBox();
	void create_Argument_GroupBox();
	void create_Beam_GroupBox();
	void create_Detector_GroupBox();
	void create_Footptint_GroupBox();
	void create_Beam_Plot();
		void plot_Beam_Profile();
	void create_Sample_Plot();
		void plot_Sample();
		void auto_Replot_Curve();

	// supplementary functions
	void fill_Argument_Units();
	void fill_Argument_Values();
	void fill_At_Fixed_Label();
	void fill_At_Fixed_Units();
	void fill_At_Fixed_Value();

	void refresh_Angular_Units();
	void refresh_Spectral_Units();
	void refresh_Argument_Values();
	void refresh_At_Fixed_Value();
	void refresh_Curvature_Radius();

	void disable_Crystal_Detector_Type();

	void connecting();

	Independent_Curve* independent_Curve;

	QVBoxLayout* main_Layout;

	// units
	QComboBox* angular_Units_ComboBox;
	QComboBox* spectral_Units_ComboBox;

	// argument
	QComboBox* arg_Type_ComboBox;
	QSpinBox* num_Points_Spinbox;
	MyDoubleSpinBox* start_Argument_Spinbox;
	MyDoubleSpinBox* final_Argument_Spinbox;
	QLabel* argument_Units_Label;

	// beam
	QLabel* at_Fixed_Label;
	MyDoubleSpinBox* at_Fixed_SpinBox;
	QLabel* at_Fixed_Units_Label;
	MyDoubleSpinBox* polarization_SpinBox;
	MyDoubleSpinBox* background_SpinBox;
	MyDoubleSpinBox* spectral_Width_SpinBox;
	MyDoubleSpinBox* angular_Divergence_SpinBox;
	QLabel* angular_Divergence_Units_Label;
	QPushButton* setup_Beam_Distribution_Button;

	// detector
	QComboBox* detector_Type_ComboBox;
	QStackedWidget* detectors_Stack;
	MyDoubleSpinBox* slit_Width_SpinBox;
	MyDoubleSpinBox* slit_Distance_SpinBox;
	MyDoubleSpinBox* crystal_Resolution_SpinBox;
	QLabel* crystal_Resolution_Units_Label;
	QPushButton* setup_Crystal_Resolution_Button;

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

#endif // SPECULAR_INDEPENDENT_CURVE_PART_H
