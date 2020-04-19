#ifndef GISAS_TARGET_CURVE_PART_H
#define GISAS_TARGET_CURVE_PART_H

#include "multilayer_approach/multilayer/common_part.h"
#include "multilayer_approach/multilayer/target_curve/target_curve_plot.h"

class Multilayer_Approach;
class Common_Part;

class GISAS_Target_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit GISAS_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent = nullptr);

	void create_Argument_GroupBox();
	void create_Value_GroupBox();
	void create_Beam_GroupBox();

	// supplementary functions
	void reset_Subinterval();
	void fill_At_Fixed_Wavelength_Label();
	void fill_Spectral_Width_Label();

	void refresh_Argument_Units();
	void refresh_At_Fixed_Wavelength_Units();
	void refresh_At_Fixed_Beam_Theta_0_Value();
	void refresh_At_Fixed_Wavelength_Value();

	void refresh_Plot_Axes_Labels();

	void connecting();

	Target_Curve* target_Curve;
	Target_Curve_Plot* target_Curve_Plot;

	QVBoxLayout* main_Layout;
	Common_Part* target_Common_Part;

	// argument
	QComboBox* arg_Units_ComboBox;
	QCheckBox* main_Subinterval_Checkbox;

	// theta
	QComboBox* arg_Type_Theta_ComboBox;
	MyDoubleSpinBox* theta_Min_SpinBox;
	MyDoubleSpinBox* theta_Max_SpinBox;
	QLabel* arg_Theta_Units_Label;
	MyDoubleSpinBox* from_Subinterval_Theta_SpinBox;
	QLabel* and_Subinterval_Theta_Label;
	MyDoubleSpinBox* to_Subinterval_Theta_SpinBox;
	QLabel* subinterval_Theta_Units_Label;

	// phi
	QComboBox* arg_Type_Phi_ComboBox;
	MyDoubleSpinBox* phi_Min_SpinBox;
	MyDoubleSpinBox* phi_Max_SpinBox;
	MyDoubleSpinBox* from_Subinterval_Phi_SpinBox;
	QLabel* arg_Phi_Units_Label;
	QLabel* and_Subinterval_Phi_Label;
	MyDoubleSpinBox* to_Subinterval_Phi_SpinBox;
	QLabel* subinterval_Phi_Units_Label;

	// value
	QComboBox* value_Type_ComboBox;
	MyDoubleSpinBox* val_Shift_SpinBox;
	MyDoubleSpinBox* val_Factor_SpinBox;
	MyDoubleSpinBox* val_Factor_Min_SpinBox;
	MyDoubleSpinBox* val_Factor_Max_SpinBox;
	QCheckBox* norm_On_Beam_Intensity;
	MyDoubleSpinBox* beam_Intensity_SpinBox;

	// beam
	QLabel* at_Fixed_Beam_Theta_0_Label;
	MyDoubleSpinBox* at_Fixed_Beam_Theta_0_SpinBox;
	QLabel* at_Fixed_Beam_Theta_0_Units_Label;
	QLabel* at_Fixed_Wavelength_Label;
	MyDoubleSpinBox* at_Fixed_Wavelength_SpinBox;
	QComboBox* at_Fixed_Wavelength_Units_ComboBox;
	MyDoubleSpinBox* polarization_SpinBox;
	MyDoubleSpinBox* background_SpinBox;
	QLabel* spectral_Width_Label;
	MyDoubleSpinBox* spectral_Width_SpinBox;
	MyDoubleSpinBox* angular_Divergence_Theta_0_SpinBox;
	QLabel* angular_Divergence_Theta_0_Units_Label;
	MyDoubleSpinBox* angular_Divergence_Phi_0_SpinBox;
	QLabel* angular_Divergence_Phi_0_Units_Label;
	QPushButton* setup_Beam_Distribution_Button;
};

#endif // GISAS_TARGET_CURVE_PART_H
