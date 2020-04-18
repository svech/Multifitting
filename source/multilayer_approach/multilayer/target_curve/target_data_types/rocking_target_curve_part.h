#ifndef ROCKING_TARGET_CURVE_PART_H
#define ROCKING_TARGET_CURVE_PART_H

#include "multilayer_approach/multilayer/common_part_1d.h"
#include "multilayer_approach/multilayer/target_curve/target_curve_plot.h"

class Multilayer_Approach;
class Common_Part_1D;

class Rocking_Target_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Rocking_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent = nullptr);

	void create_Argument_GroupBox();
	void create_Value_GroupBox();
	void create_Beam_GroupBox();

	// supplementary functions
	void reset_Subinterval();
	void fill_At_Fixed_Wavelength_Label();
	void fill_Spectral_Width_Label();

	void refresh_Argument_Units();
	void refresh_At_Fixed_Wavelength_Units();
	void refresh_At_Fixed_Specular_Position();
	void refresh_At_Fixed_Wavelength_Value();
	void reset_Range_Specular_Position();

	void refresh_Plot_Axes_Labels();

	void connecting();

	Target_Curve* target_Curve;
	Target_Curve_Plot* target_Curve_Plot;

	QVBoxLayout* main_Layout;
	Common_Part_1D* target_1D_Common_Part;

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
	QLabel* at_Fixed_Specular_Position_Label;
	MyDoubleSpinBox* at_Fixed_Specular_Position_SpinBox;
	QLabel* at_Fixed_Specular_Position_Units_Label;
	QLabel* at_Fixed_Wavelength_Label;
	MyDoubleSpinBox* at_Fixed_Wavelength_SpinBox;
	QComboBox* at_Fixed_Wavelength_Units_ComboBox;
	MyDoubleSpinBox* polarization_SpinBox;
	MyDoubleSpinBox* background_SpinBox;
	QLabel* spectral_Width_Label;
	MyDoubleSpinBox* spectral_Width_SpinBox;
	MyDoubleSpinBox* angular_Divergence_SpinBox;
	QLabel* angular_Divergence_Units_Label;
	QPushButton* setup_Beam_Distribution_Button;
};

#endif // ROCKING_TARGET_CURVE_PART_H
