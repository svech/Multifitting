#ifndef GISAS_INDEPENDENT_CURVE_PART_H
#define GISAS_INDEPENDENT_CURVE_PART_H

#include "multilayer_approach/multilayer/common_part.h"

class Multilayer_Approach;
class Common_Part;

class GISAS_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit GISAS_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

	void create_Units_GroupBox();
	void create_Argument_GroupBox();
	void create_Beam_GroupBox();

	// supplementary functions
	void fill_At_Fixed_Wavelength_Label();
	void fill_Spectral_Width_Label();

	void refresh_Angular_Units();
	void refresh_Spectral_Units();
	void refresh_Theta_Argument_Values(bool rescale_Axes = false);
	void refresh_Phi_Argument_Values(bool rescale_Axes = false);
	void refresh_At_Fixed_Wavelength_Value();
	void refresh_At_Fixed_Beam_Theta_0_Value();

	void connecting();

	Independent_Curve* independent_Curve;

	QVBoxLayout* main_Layout;
	Common_Part* independent_Common_Part;

	// units
	QComboBox* angular_Units_ComboBox;
	QComboBox* spectral_Units_ComboBox;

	// argument theta
	QComboBox* arg_Type_Theta_ComboBox;
	QSpinBox* num_Points_Theta_Spinbox;
	MyDoubleSpinBox* start_Argument_Theta_Spinbox;
	MyDoubleSpinBox* final_Argument_Theta_Spinbox;
	QLabel* argument_Units_Theta_Label;

	// argument phi
	QComboBox* arg_Type_Phi_ComboBox;
	QSpinBox* num_Points_Phi_Spinbox;
	MyDoubleSpinBox* start_Argument_Phi_Spinbox;
	MyDoubleSpinBox* final_Argument_Phi_Spinbox;
	QLabel* argument_Units_Phi_Label;

	// beam
	QLabel* at_Fixed_Wavelength_Label;
	MyDoubleSpinBox* at_Fixed_Wavelength_SpinBox;
	QLabel* at_Fixed_Wavelength_Units_Label;
	QLabel* at_Fixed_Beam_Theta_0_Label;
	MyDoubleSpinBox* at_Fixed_Beam_Theta_0_SpinBox;
	QLabel* at_Fixed_Beam_Theta_0_Units_Label;
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

#endif // GISAS_INDEPENDENT_CURVE_PART_H
