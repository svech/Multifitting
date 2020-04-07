#ifndef SPECULAR_TARGET_CURVE_PART_H
#define SPECULAR_TARGET_CURVE_PART_H

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

	void connecting();

	Target_Curve* target_Curve;

	QVBoxLayout* main_Layout;

	// argument
	QComboBox* arg_Type_ComboBox;
	QComboBox* arg_Units_ComboBox;
	MyDoubleSpinBox* arg_Shift_SpinBox;
	MyDoubleSpinBox* arg_Factor_SpinBox;
	QCheckBox* main_Subinterval_Checkbox;
	MyDoubleSpinBox* horizontal_From_Subinterval_SpinBox;
	MyDoubleSpinBox* horizontal_To_Subinterval_SpinBox;

	// value
	QComboBox* value_Type_ComboBox;
	MyDoubleSpinBox* val_Shift_SpinBox;
	MyDoubleSpinBox* val_Factor_SpinBox;
	MyDoubleSpinBox* val_Factor_Min_SpinBox;
	MyDoubleSpinBox* val_Factor_Max_SpinBox;
	QCheckBox* norm_On_Beam_Intensity;
	MyDoubleSpinBox* beam_Intensity_Initial_SpinBox;
	QCheckBox* beam_Intensity_Final_CheckBox;
	MyDoubleSpinBox* beam_Intensity_Final_SpinBox;

	// beam
	QLabel* at_Fixed_Label;
	MyDoubleSpinBox* at_Fixed_SpinBox;
	QComboBox* at_Fixed_Units_ComboBox;
	MyDoubleSpinBox* polarization_SpinBox;
	MyDoubleSpinBox* background_SpinBox;
	MyDoubleSpinBox* spectral_Width_SpinBox;
	MyDoubleSpinBox* angular_Divergence_SpinBox;
	QLabel* angular_Divergence_Units_Label;
	QPushButton* setup_Button;
};

#endif // SPECULAR_TARGET_CURVE_PART_H
