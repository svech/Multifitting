#ifndef SPECULAR_TARGET_CURVE_PART_H
#define SPECULAR_TARGET_CURVE_PART_H

#include "multilayer_approach/multilayer/multilayer.h"

#include "multilayer_approach/multilayer/target_curve/distribution_editor.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve_plot.h"

class Specular_Target_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Specular_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent = nullptr);

	void create_Argument_GroupBox();
	void create_Value_GroupBox();
	void create_Beam_GroupBox();
	void create_Detector_GroupBox();
	void create_Footptint_GroupBox();
	void create_Beam_Plot();
		void plot_Beam_Profile();
	void create_Sample_Plot();
		void plot_Sample();
		void auto_Replot_Curve();

	// supplementary functions
	void reset_Subinterval();
	void fill_Argument_Units();
	void fill_At_Fixed_Label();
	void fill_At_Fixed_Units();
	void fill_At_Fixed_Value();

	void refresh_Argument_Units();
	void refresh_At_Fixed_Units();
	void refresh_At_Fixed_Value();
	void refresh_Value_Type();
	void refresh_Curvature_Radius();

	void disable_Crystal_Detector_Type();
	void refresh_Plot_Axes_Labels();

	void connecting();

	Target_Curve* target_Curve;
	Target_Curve_Plot* target_Curve_Plot;

	QVBoxLayout* main_Layout;

	// argument
	QComboBox* arg_Type_ComboBox;
	QComboBox* arg_Units_ComboBox;
	MyDoubleSpinBox* arg_Shift_SpinBox;
	MyDoubleSpinBox* arg_Factor_SpinBox;
	QCheckBox* main_Subinterval_Checkbox;
	MyDoubleSpinBox* horizontal_From_Subinterval_SpinBox;
	QLabel* horizontal_And_Subinterval_Label;
	MyDoubleSpinBox* horizontal_To_Subinterval_SpinBox;

	// value
	QComboBox* value_Type_ComboBox;
	MyDoubleSpinBox* val_Shift_SpinBox;
	MyDoubleSpinBox* val_Factor_SpinBox;
	MyDoubleSpinBox* val_Factor_Min_SpinBox;
	MyDoubleSpinBox* val_Factor_Max_SpinBox;
	QCheckBox* norm_On_Beam_Intensity;
	QLabel* beam_Intensity_Start_Label;
	MyDoubleSpinBox* beam_Intensity_Initial_SpinBox;
	QCheckBox* beam_Intensity_Final_CheckBox;
	MyDoubleSpinBox* beam_Intensity_Final_SpinBox;

	// beam
	QLabel* at_Fixed_Label;
	MyDoubleSpinBox* at_Fixed_SpinBox;
	QComboBox* at_Fixed_Units_ComboBox;
	MyDoubleSpinBox* polarization_SpinBox;
	MyDoubleSpinBox* background_SpinBox;
	QLabel* spectral_Width_Label;
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

#endif // SPECULAR_TARGET_CURVE_PART_H
