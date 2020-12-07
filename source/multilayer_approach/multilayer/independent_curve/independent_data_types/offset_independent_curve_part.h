#ifndef OFFSET_INDEPENDENT_CURVE_PART_H
#define OFFSET_INDEPENDENT_CURVE_PART_H

#include "multilayer_approach/multilayer/common_part.h"

class Multilayer_Approach;
class Common_Part;

class Offset_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Offset_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

	void create_Units_GroupBox();
	void create_Argument_GroupBox();
	void create_Beam_GroupBox();

	// supplementary functions
	void fill_At_Fixed_Wavelength_Label();
	void fill_Spectral_Width_Label();
	void fill_Offset();

	void refresh_Angular_Units();
	void refresh_Spectral_Units();
	void refresh_Argument_Values(bool num_Points_Changed = false);
	void refresh_At_Fixed_Wavelength_Value();
	void refresh_At_Fixed_Offset_Value();

	void connecting();

	Independent_Curve* independent_Curve;

	QVBoxLayout* main_Layout;
	Common_Part* independent_Common_Part;

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
	QLabel* at_Fixed_Wavelength_Label;
	MyDoubleSpinBox* at_Fixed_Wavelength_SpinBox;
	QLabel* at_Fixed_Wavelength_Units_Label;
	QLabel* at_Fixed_Detector_Offset_Label;
	MyDoubleSpinBox* at_Fixed_Detector_Offset_SpinBox;
	QLabel* at_Fixed_Detector_Offset_Units_Label;
	MyDoubleSpinBox* polarization_SpinBox;
	MyDoubleSpinBox* background_SpinBox;
	QLabel* spectral_Width_Label;
	MyDoubleSpinBox* spectral_Width_SpinBox;
	MyDoubleSpinBox* angular_Divergence_SpinBox;
	QLabel* angular_Divergence_Units_Label;
	QPushButton* setup_Beam_Distribution_Button;
};

#endif // OFFSET_INDEPENDENT_CURVE_PART_H
