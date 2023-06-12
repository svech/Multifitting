#ifndef COMMON_PART_1D_H
#define COMMON_PART_1D_H

#include "distribution_editor.h"
#include "multilayer_approach/multilayer_approach.h"
#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"

class Multilayer_Approach;
class Target_Curve_Editor;

class Common_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Common_Part(Independent_Curve* independent_Curve, Target_Curve* target_Curve, bool is_Independent = false, Target_Curve_Editor* target_Curve_Editor = nullptr, QWidget *parent = nullptr);

	void create_1D_Detector_GroupBox();
	void create_2D_Detector_GroupBox();
	void create_Footptint_GroupBox();
	void create_Beam_Plot();
		void apply_Lin_Log();
		void plot_Beam_Profile();
	void create_Sample_Plot();
		void plot_Sample();
		void auto_Replot_Curve();

	// supplementary functions
	void refresh_Curvature_Radius();

	void connecting();

	bool is_Independent = true;

	Target_Curve_Editor* target_Curve_Editor;
	Data& measurement;
	QString& angular_Units;
	QString& spectral_Units;

	QVBoxLayout* main_Layout;

	// detector
	QLabel* detector_Type_Label;
	QComboBox* detector_Type_ComboBox;
	MyDoubleSpinBox* detector_Distance_SpinBox;
    QPushButton* setup_Beam_Distribution_Button_slit;
    QPushButton* setup_Beam_Distribution_Button_crystal;
    QStackedWidget* detectors_Stack;
	QComboBox* resolution_Function_ComboBox;
	QCheckBox* use_Binning_Checkbox;
	QCheckBox* finite_Slit_Length_Checkbox;
	MyDoubleSpinBox* slit_Length_SpinBox;
	QLabel* slit_Length_Units_Label;

	// 1D
	MyDoubleSpinBox* slit_Width_SpinBox;
	MyDoubleSpinBox* crystal_Resolution_SpinBox;
	QLabel* crystal_Resolution_Units_Label;
	QSpinBox* binning_Factor_Spinbox;
	// 2D
	QCheckBox* set_Pixel_Size_Checkbox;
	MyDoubleSpinBox* pixel_Polar_Height_SpinBox;
	MyDoubleSpinBox* pixel_Azimuthal_Width_SpinBox;
	MyDoubleSpinBox* theta_Resolution_SpinBox;
	QLabel* theta_Resolution_Units_Label;
	MyDoubleSpinBox* phi_Resolution_SpinBox;
	QLabel* phi_Resolution_Units_Label;
	QLabel* theta_Binning_Factor_Label;
	QSpinBox* theta_Binning_Factor_Spinbox;
	QLabel* phi_Binning_Factor_Label;
	QSpinBox* phi_Binning_Factor_Spinbox;

	// footprint
	MyDoubleSpinBox* beam_Footprint_Width_SpinBox;	
	MyDoubleSpinBox* beam_Footprint_Left_Smoothing_SpinBox;
	QCheckBox* beam_Footprint_Right_Smoothing_Checkbox;
	MyDoubleSpinBox* beam_Footprint_Right_Smoothing_SpinBox;
	MyDoubleSpinBox* beam_Footprint_Left_Wing_Width_SpinBox;
	QCheckBox* beam_Footprint_Right_Wing_Width_Checkbox;
	MyDoubleSpinBox* beam_Footprint_Right_Wing_Width_SpinBox;
	MyDoubleSpinBox* beam_Footprint_Wing_Intensity_SpinBox;
	MyDoubleSpinBox* beam_Footprint_Lateral_Width_SpinBox;
	QCheckBox* beam_Footprint_Log_Profile_Checkbox;
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

#endif // COMMON_PART_1D_H
