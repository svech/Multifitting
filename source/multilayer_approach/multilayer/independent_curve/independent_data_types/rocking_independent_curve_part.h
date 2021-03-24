#ifndef ROCKING_INDEPENDENT_CURVE_PART_H
#define ROCKING_INDEPENDENT_CURVE_PART_H

#include "multilayer_approach/multilayer/common_part.h"

class Multilayer_Approach;
class Common_Part;

class Rocking_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Rocking_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

	void closeEvent(QCloseEvent* event);
	void create_Units_GroupBox();
	void create_Argument_GroupBox();
	void create_Beam_GroupBox();

	// supplementary functions
	void fill_Argument();
	void fill_At_Fixed_Wavelength_Label();
	void fill_Spectral_Width_Label();

	void refresh_Angular_Units();
	void refresh_Spectral_Units();
	void refresh_Num_Points();
	void refresh_Argument_Min();
	void refresh_Argument_Max();
	void refresh_At_Fixed_Wavelength_Value();
	void refresh_At_Fixed_Specular_Position();

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
	double temporary_Min;
	double temporary_Max;

	// beam
	QLabel* at_Fixed_Wavelength_Label;
	MyDoubleSpinBox* at_Fixed_Wavelength_SpinBox;
	QLabel* at_Fixed_Wavelength_Units_Label;
	QLabel* at_Fixed_Specular_Position_Label;
	MyDoubleSpinBox* at_Fixed_Specular_Position_SpinBox;
	QLabel* at_Fixed_Specular_Position_Units_Label;
	MyDoubleSpinBox* polarization_SpinBox;
	MyDoubleSpinBox* background_SpinBox;
	QLabel* spectral_Width_Label;
	MyDoubleSpinBox* spectral_Width_SpinBox;
	MyDoubleSpinBox* angular_Divergence_SpinBox;
	QLabel* angular_Divergence_Units_Label;
	QPushButton* setup_Beam_Distribution_Button;

};

#endif // ROCKING_INDEPENDENT_CURVE_PART_H
