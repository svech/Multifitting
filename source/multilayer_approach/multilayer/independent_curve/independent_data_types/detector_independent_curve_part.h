#ifndef DETECTOR_INDEPENDENT_CURVE_PART_H
#define DETECTOR_INDEPENDENT_CURVE_PART_H

#include "multilayer_approach/multilayer/common_part_1d.h"

class Multilayer_Approach;
class Common_Part_1D;

class Detector_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Detector_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

	void create_Units_GroupBox();
	void create_Argument_GroupBox();
	void create_Beam_GroupBox();

	// supplementary functions
	void fill_At_Fixed_Wavelength_Label();
	void fill_Spectral_Width_Label();

	void refresh_Angular_Units();
	void refresh_Spectral_Units();
	void refresh_Argument_Values();
	void refresh_At_Fixed_Wavelength_Value();
	void refresh_At_Fixed_Beam_Theta_0_Value();

	void connecting();

	Independent_Curve* independent_Curve;

	QVBoxLayout* main_Layout;
	Common_Part_1D* independent_1D_Common_Part;

	// units
	QComboBox* angular_Units_ComboBox;
	QComboBox* spectral_Units_ComboBox;

	// argument
	QLabel* arg_Type_Label;
	QSpinBox* num_Points_Spinbox;
	MyDoubleSpinBox* start_Argument_Spinbox;
	MyDoubleSpinBox* final_Argument_Spinbox;
	QLabel* argument_Units_Label;

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
	MyDoubleSpinBox* angular_Divergence_SpinBox;
	QLabel* angular_Divergence_Units_Label;
	QPushButton* setup_Beam_Distribution_Button;
};

#endif // DETECTOR_INDEPENDENT_CURVE_PART_H
