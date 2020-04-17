#ifndef SPECULAR_INDEPENDENT_CURVE_PART_H
#define SPECULAR_INDEPENDENT_CURVE_PART_H

#include "independent_1d_common_part.h"

class Multilayer_Approach;
class Independent_1D_Common_Part;

class Specular_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Specular_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

	void create_Units_GroupBox();
	void create_Argument_GroupBox();
	void create_Beam_GroupBox();

	// supplementary functions
	void fill_Argument_Units();
	void fill_Argument_Values();
	void fill_Spectral_Width_Label();
	void fill_At_Fixed_Label();
	void fill_At_Fixed_Units();
	void fill_At_Fixed_Value();

	void refresh_Angular_Units();
	void refresh_Spectral_Units();
	void refresh_Argument_Values();
	void refresh_At_Fixed_Value();

	void disable_Crystal_Detector_Type();

	void connecting();

	Independent_Curve* independent_Curve;

	QVBoxLayout* main_Layout;
	Independent_1D_Common_Part* independent_1D_Common_Part;

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
	QLabel* spectral_Width_Label;
	MyDoubleSpinBox* spectral_Width_SpinBox;
	MyDoubleSpinBox* angular_Divergence_SpinBox;
	QLabel* angular_Divergence_Units_Label;
	QPushButton* setup_Beam_Distribution_Button;
};

#endif // SPECULAR_INDEPENDENT_CURVE_PART_H
