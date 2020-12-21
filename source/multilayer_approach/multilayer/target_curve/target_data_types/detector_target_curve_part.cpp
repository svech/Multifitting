#include "detector_target_curve_part.h"

Detector_Target_Curve_Part::Detector_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent) :
	target_Curve(target_Curve),
	target_Curve_Plot(target_Curve_Plot),
	QWidget(parent)
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setContentsMargins(4,4,4,0);
	main_Layout->setSpacing(0);

	create_Argument_GroupBox();
	create_Value_GroupBox();
	create_Beam_GroupBox();

	Independent_Curve* independent_Curve;
	target_Common_Part = new Common_Part(independent_Curve, target_Curve);
	main_Layout->addWidget(target_Common_Part);

	connecting();
	refresh_Plot_Axes_Labels();
	target_Curve->refresh_Description_Label();
}

void Detector_Target_Curve_Part::create_Argument_GroupBox()
{
	// initialization
	if(target_Curve->measurement.argument_Type == no_Argument_Type)
	{
		target_Curve->measurement.argument_Type = argument_Types[Detector_Polar_Angle];
	}

	QGroupBox* argument_GroupBox = new QGroupBox("Argument");
	main_Layout->addWidget(argument_GroupBox);

	QHBoxLayout* argument_GroupBox_Layout = new QHBoxLayout(argument_GroupBox);
	argument_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// argument type
	{
		arg_Type_ComboBox = new QComboBox;
			arg_Type_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			arg_Type_ComboBox->addItem(target_Curve->measurement.argument_Type);
			arg_Type_ComboBox->setCurrentText(target_Curve->measurement.argument_Type);
		argument_GroupBox_Layout->addWidget(arg_Type_ComboBox,0,Qt::AlignLeft);
	}
	// argument units
	{
		QLabel* argument_Units_Label = new QLabel("  Units");
		argument_GroupBox_Layout->addWidget(argument_Units_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		arg_Units_ComboBox = new QComboBox;
			arg_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			arg_Units_ComboBox->addItems(angle_Units_List);
			arg_Units_ComboBox->setCurrentText(target_Curve->angular_Units);
		argument_GroupBox_Layout->addWidget(arg_Units_ComboBox,0,Qt::AlignLeft);
	}
	// argument shift
	{
		QLabel* argument_Shift_Label = new QLabel("    Shift");
		argument_GroupBox_Layout->addWidget(argument_Shift_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		arg_Shift_SpinBox = new MyDoubleSpinBox;
			arg_Shift_SpinBox->setAccelerated(true);
			arg_Shift_SpinBox->setRange(-100000, MAX_DOUBLE);
			arg_Shift_SpinBox->setDecimals(4);
			arg_Shift_SpinBox->setValue(target_Curve->curve.horizontal_Arg_Shift);
			arg_Shift_SpinBox->setSingleStep(0.001);
			arg_Shift_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			arg_Shift_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(arg_Shift_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(arg_Shift_SpinBox);
	}
	// argument factor
	{
		QLabel* arg_Factor_Label = new QLabel("   Factor");
		argument_GroupBox_Layout->addWidget(arg_Factor_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		arg_Factor_SpinBox = new MyDoubleSpinBox;
			arg_Factor_SpinBox->setAccelerated(true);
			arg_Factor_SpinBox->setRange(0, MAX_DOUBLE);
			arg_Factor_SpinBox->setDecimals(4);
			arg_Factor_SpinBox->setValue(target_Curve->curve.horizontal_Arg_Factor);
			arg_Factor_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			arg_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			arg_Factor_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(arg_Factor_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(arg_Factor_SpinBox);
	}
	// specifying interval for fitting
	{
		int num_Steps = 500;

		main_Subinterval_Checkbox = new QCheckBox("Fit only data between argument");
			main_Subinterval_Checkbox->setCheckable(true);
			main_Subinterval_Checkbox->setChecked(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(main_Subinterval_Checkbox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		horizontal_From_Subinterval_SpinBox = new MyDoubleSpinBox;
			horizontal_From_Subinterval_SpinBox->setAccelerated(true);
			horizontal_From_Subinterval_SpinBox->setDecimals(4);
			horizontal_From_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Right-target_Curve->curve.subinterval_Left)/num_Steps);
			horizontal_From_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			horizontal_From_Subinterval_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH_SUB);
			horizontal_From_Subinterval_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(horizontal_From_Subinterval_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_From_Subinterval_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		horizontal_And_Subinterval_Label = new QLabel(" and ");
			horizontal_And_Subinterval_Label->setEnabled(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(horizontal_And_Subinterval_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		horizontal_To_Subinterval_SpinBox = new MyDoubleSpinBox;
			horizontal_To_Subinterval_SpinBox->setAccelerated(true);
			horizontal_To_Subinterval_SpinBox->setDecimals(4);
			horizontal_To_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Right-target_Curve->curve.subinterval_Left)/num_Steps);
			horizontal_To_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			horizontal_To_Subinterval_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH_SUB);
			horizontal_To_Subinterval_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(horizontal_To_Subinterval_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_To_Subinterval_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		invert_Subinterval_Checkbox = new QCheckBox("Fit outer area");
			invert_Subinterval_Checkbox->setCheckable(true);
			invert_Subinterval_Checkbox->setEnabled(target_Curve->curve.use_Subinterval);
			invert_Subinterval_Checkbox->setChecked(target_Curve->curve.outer_Area);
		argument_GroupBox_Layout->addWidget(invert_Subinterval_Checkbox,0,Qt::AlignLeft);

		if(target_Curve->loaded_And_Ready)
		{
			double min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
			double max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
			horizontal_From_Subinterval_SpinBox->setSingleStep((max-min)/num_Steps);
			horizontal_To_Subinterval_SpinBox->setSingleStep((max-min)/num_Steps);
		}

		reset_Subinterval();
	}
}

void Detector_Target_Curve_Part::create_Value_GroupBox()
{
	// initialization
	if(target_Curve->curve.value_Type == no_Value_Type)
	{
		target_Curve->curve.value_Type = value_Types[Scattering];
		target_Curve->calc_Functions.check_Scattering = true;
	}

	QGroupBox* value_GroupBox = new QGroupBox("Value");
	main_Layout->addWidget(value_GroupBox);

	QHBoxLayout* value_GroupBox_Layout = new QHBoxLayout(value_GroupBox);
	value_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// value type
	{
		value_Type_ComboBox = new QComboBox;
			value_Type_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			value_Type_ComboBox->addItem(target_Curve->curve.value_Type);
			value_Type_ComboBox->setCurrentText(target_Curve->curve.value_Type);
		value_GroupBox_Layout->addWidget(value_Type_ComboBox);
	}
	// value shift
	{
		QLabel* value_Shift_Label = new QLabel("    Shift");
		value_GroupBox_Layout->addWidget(value_Shift_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		val_Shift_SpinBox = new MyDoubleSpinBox;
			val_Shift_SpinBox->setAccelerated(true);
			val_Shift_SpinBox->setDecimals(4);
			val_Shift_SpinBox->setRange(-100000, MAX_DOUBLE);
			val_Shift_SpinBox->setValue(target_Curve->curve.val_Shift);
			val_Shift_SpinBox->setSingleStep(0.001);
			val_Shift_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			val_Shift_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		value_GroupBox_Layout->addWidget(val_Shift_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(val_Shift_SpinBox);
	}
	// value factor
	{
		QLabel* value_Factor_Label = new QLabel("   Factor");
		value_GroupBox_Layout->addWidget(value_Factor_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		val_Factor_SpinBox = new MyDoubleSpinBox;
			val_Factor_SpinBox->setAccelerated(true);
			val_Factor_SpinBox->setDecimals(4);
			val_Factor_SpinBox->setRange(0, MAX_DOUBLE);
			val_Factor_SpinBox->setValue(target_Curve->curve.val_Factor.value);
			val_Factor_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			val_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			val_Factor_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		value_GroupBox_Layout->addWidget(val_Factor_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(val_Factor_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* val_Factor_Min_Label = new QLabel(", min");
		value_GroupBox_Layout->addWidget(val_Factor_Min_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		val_Factor_Min_SpinBox = new MyDoubleSpinBox;
			val_Factor_Min_SpinBox->setDecimals(4);
			val_Factor_Min_SpinBox->setRange(1E-4,MAX_DOUBLE);
			val_Factor_Min_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			val_Factor_Min_SpinBox->setValue(target_Curve->curve.val_Factor.fit.min);
			val_Factor_Min_SpinBox->setAccelerated(true);
			val_Factor_Min_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			val_Factor_Min_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		value_GroupBox_Layout->addWidget(val_Factor_Min_SpinBox);
		Global_Variables::resize_Line_Edit(val_Factor_Min_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* val_Factor_Max_Label = new QLabel("max");
		value_GroupBox_Layout->addWidget(val_Factor_Max_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		val_Factor_Max_SpinBox = new MyDoubleSpinBox;
			val_Factor_Max_SpinBox->setDecimals(4);
			val_Factor_Max_SpinBox->setRange(1E-4,MAX_DOUBLE);
			val_Factor_Max_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			val_Factor_Max_SpinBox->setValue(target_Curve->curve.val_Factor.fit.max);
			val_Factor_Max_SpinBox->setAccelerated(true);
			val_Factor_Max_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			val_Factor_Max_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		value_GroupBox_Layout->addWidget(val_Factor_Max_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(val_Factor_Max_SpinBox);
	}
	// norm on beam intensity
	{
		norm_On_Beam_Intensity = new QCheckBox("Divide on beam intensity:");
			norm_On_Beam_Intensity->setChecked(target_Curve->curve.divide_On_Beam_Intensity);
		value_GroupBox_Layout->addWidget(norm_On_Beam_Intensity);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Intensity_Start_Label = new QLabel("initial");
			beam_Intensity_Start_Label->setEnabled(target_Curve->curve.divide_On_Beam_Intensity);
		value_GroupBox_Layout->addWidget(beam_Intensity_Start_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Intensity_Initial_SpinBox = new MyDoubleSpinBox;
			beam_Intensity_Initial_SpinBox->setAccelerated(true);
			beam_Intensity_Initial_SpinBox->setRange(1, MAX_DOUBLE);
			beam_Intensity_Initial_SpinBox->setDecimals(0);
			beam_Intensity_Initial_SpinBox->setValue(target_Curve->curve.beam_Intensity_Initial);
			beam_Intensity_Initial_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Intensity_Initial_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Intensity_Initial_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
			beam_Intensity_Initial_SpinBox->setEnabled(target_Curve->curve.divide_On_Beam_Intensity);
		value_GroupBox_Layout->addWidget(beam_Intensity_Initial_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Intensity_Initial_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Intensity_Final_CheckBox = new QCheckBox("final");
			beam_Intensity_Final_CheckBox->setChecked(target_Curve->curve.use_Final_Intensity);
			beam_Intensity_Final_CheckBox->setEnabled(target_Curve->curve.divide_On_Beam_Intensity);
		value_GroupBox_Layout->addWidget(beam_Intensity_Final_CheckBox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Intensity_Final_SpinBox = new MyDoubleSpinBox;
			beam_Intensity_Final_SpinBox->setAccelerated(true);
			beam_Intensity_Final_SpinBox->setRange(1, MAX_DOUBLE);
			beam_Intensity_Final_SpinBox->setDecimals(0);
			beam_Intensity_Final_SpinBox->setValue(target_Curve->curve.beam_Intensity_Final);
			beam_Intensity_Final_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Intensity_Final_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Intensity_Final_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
			beam_Intensity_Final_SpinBox->setEnabled(target_Curve->curve.divide_On_Beam_Intensity && target_Curve->curve.use_Final_Intensity);
		value_GroupBox_Layout->addWidget(beam_Intensity_Final_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Intensity_Final_SpinBox);
	}

}

void Detector_Target_Curve_Part::create_Beam_GroupBox()
{
	QGroupBox* beam_GroupBox = new QGroupBox("Beam");
	main_Layout->addWidget(beam_GroupBox);

	QGridLayout* beam_GroupBox_Layout = new QGridLayout(beam_GroupBox);
	beam_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// at fixed wavelength
	{
		at_Fixed_Wavelength_Label = new QLabel;
			fill_At_Fixed_Wavelength_Label();
		beam_GroupBox_Layout->addWidget(at_Fixed_Wavelength_Label,0,0,1,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double coeff = wavelength_Coefficients_Map.value(target_Curve->spectral_Units);

		at_Fixed_Wavelength_SpinBox = new MyDoubleSpinBox;
			at_Fixed_Wavelength_SpinBox->setAccelerated(true);
			at_Fixed_Wavelength_SpinBox->setRange(0,MAX_DOUBLE);
			at_Fixed_Wavelength_SpinBox->setDecimals(7);
			at_Fixed_Wavelength_SpinBox->setSingleStep(0.01);
			at_Fixed_Wavelength_SpinBox->setValue(Global_Variables::wavelength_Energy(target_Curve->spectral_Units,target_Curve->measurement.wavelength.value)/coeff);
			at_Fixed_Wavelength_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			at_Fixed_Wavelength_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(at_Fixed_Wavelength_SpinBox,0,2,1,2,Qt::AlignRight);
		Global_Variables::resize_Line_Edit(at_Fixed_Wavelength_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_Wavelength_Units_ComboBox = new QComboBox;
			at_Fixed_Wavelength_Units_ComboBox->addItems(wavelength_Units_List);
			at_Fixed_Wavelength_Units_ComboBox->setCurrentText(target_Curve->spectral_Units);
			at_Fixed_Wavelength_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		beam_GroupBox_Layout->addWidget(at_Fixed_Wavelength_Units_ComboBox,0,4,Qt::AlignLeft);
	}
	// at fixed beam theta_0
	{
		at_Fixed_Beam_Theta_0_Label = new QLabel("At fixed grazing angle "+Theta_Sym+Zero_Subscript_Sym);
		beam_GroupBox_Layout->addWidget(at_Fixed_Beam_Theta_0_Label,1,0,1,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);

		at_Fixed_Beam_Theta_0_SpinBox = new MyDoubleSpinBox;
			at_Fixed_Beam_Theta_0_SpinBox->setAccelerated(true);
			at_Fixed_Beam_Theta_0_SpinBox->setRange(0,90./coeff);
			at_Fixed_Beam_Theta_0_SpinBox->setDecimals(7);
			at_Fixed_Beam_Theta_0_SpinBox->setSingleStep(0.01);
			at_Fixed_Beam_Theta_0_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Angle.value/coeff);
			at_Fixed_Beam_Theta_0_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			at_Fixed_Beam_Theta_0_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(at_Fixed_Beam_Theta_0_SpinBox,1,2,1,2,Qt::AlignRight);
		Global_Variables::resize_Line_Edit(at_Fixed_Beam_Theta_0_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_Beam_Theta_0_Units_Label = new QLabel(target_Curve->angular_Units);
		beam_GroupBox_Layout->addWidget(at_Fixed_Beam_Theta_0_Units_Label,1,4,Qt::AlignLeft);
	}
	// background
	{
		QLabel* background_Label = new QLabel("Background");
		beam_GroupBox_Layout->addWidget(background_Label,2,0,Qt::AlignRight);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		background_SpinBox = new MyDoubleSpinBox;
			background_SpinBox->setAccelerated(true);
			background_SpinBox->setRange(0, MAX_DOUBLE);
			background_SpinBox->setDecimals(10);
			background_SpinBox->setValue(target_Curve->measurement.background);
			background_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			background_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			background_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		beam_GroupBox_Layout->addWidget(background_SpinBox,2,1,1,2,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(background_SpinBox);
	}
	// polarization
	{
		QLabel* polarization_Label = new QLabel("Polarization");
		beam_GroupBox_Layout->addWidget(polarization_Label,2,3,Qt::AlignRight);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		polarization_SpinBox = new MyDoubleSpinBox;
			polarization_SpinBox->setAccelerated(true);
			polarization_SpinBox->setRange(-1, 1);
			polarization_SpinBox->setDecimals(3);
			polarization_SpinBox->setValue(target_Curve->measurement.polarization);
			polarization_SpinBox->setSingleStep(0.01);
			polarization_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			polarization_SpinBox->setProperty(min_Size_Property, 50);
		beam_GroupBox_Layout->addWidget(polarization_SpinBox,2,4,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(polarization_SpinBox);
	}
	// spectral distribution
	{
		spectral_Width_Label = new QLabel;
			fill_Spectral_Width_Label();
		beam_GroupBox_Layout->addWidget(spectral_Width_Label,0,5,Qt::AlignRight);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		spectral_Width_SpinBox = new MyDoubleSpinBox;
			spectral_Width_SpinBox->setAccelerated(true);
			spectral_Width_SpinBox->setRange(0, 1);
			spectral_Width_SpinBox->setDecimals(7);
			spectral_Width_SpinBox->setValue(target_Curve->measurement.spectral_Distribution.FWHM_distribution);
			spectral_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			spectral_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			spectral_Width_SpinBox->setProperty(min_Size_Property, TARGET_LINE_RESOLUTION_WIDTH);
		beam_GroupBox_Layout->addWidget(spectral_Width_SpinBox,0,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(spectral_Width_SpinBox);
	}
	// angular divergence
	{
		QLabel* angular_Divergence_Label = new QLabel("      Angular divergence, FWHM, " + Delta_Big_Sym + Theta_Sym + Zero_Subscript_Sym);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Label,1,5,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double arg_Coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);

		angular_Divergence_SpinBox = new MyDoubleSpinBox;
			angular_Divergence_SpinBox->setAccelerated(true);
			angular_Divergence_SpinBox->setRange(0, 90/arg_Coeff);
			angular_Divergence_SpinBox->setDecimals(7);
			angular_Divergence_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/arg_Coeff);
			angular_Divergence_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			angular_Divergence_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			angular_Divergence_SpinBox->setProperty(min_Size_Property, TARGET_LINE_RESOLUTION_WIDTH);
		beam_GroupBox_Layout->addWidget(angular_Divergence_SpinBox,1,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(angular_Divergence_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		angular_Divergence_Units_Label = new QLabel(target_Curve->angular_Units);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Units_Label,1,7,Qt::AlignLeft);
	}
	// set up distribution
	{
		QLabel* spacing_Label = new QLabel("         ");
		beam_GroupBox_Layout->addWidget(spacing_Label,0,8,2,1,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		setup_Beam_Distribution_Button = new QPushButton("Set up distribution");
		beam_GroupBox_Layout->addWidget(setup_Beam_Distribution_Button,0,9,2,1,Qt::AlignCenter);
	}
}

void Detector_Target_Curve_Part::reset_Subinterval()
{
	if(target_Curve->loaded_And_Ready)
	{
		double min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
		double max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
		horizontal_From_Subinterval_SpinBox->setRange(min,max);
		horizontal_To_Subinterval_SpinBox->setRange(min,max);
	} else
	{
		horizontal_From_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
		horizontal_To_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);

	}
	horizontal_From_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Left);
	horizontal_To_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Right);
}

void Detector_Target_Curve_Part::refresh_Argument_Units()
{
	angular_Divergence_SpinBox->blockSignals(true);
	target_Common_Part->crystal_Resolution_SpinBox->blockSignals(true);
	at_Fixed_Beam_Theta_0_SpinBox->blockSignals(true);

	target_Curve->angular_Units = arg_Units_ComboBox->currentText();

	double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);

	at_Fixed_Beam_Theta_0_SpinBox->setRange(0,90./coeff);
	at_Fixed_Beam_Theta_0_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Angle.value/coeff);
	at_Fixed_Beam_Theta_0_Units_Label->setText(target_Curve->angular_Units);

	// angular divergence
	angular_Divergence_SpinBox->setRange(0,90./coeff);
	angular_Divergence_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/coeff);
	angular_Divergence_Units_Label->setText(target_Curve->angular_Units);

	// crystal resolution
	target_Common_Part->crystal_Resolution_SpinBox->setRange(0,90./coeff);
	target_Common_Part->crystal_Resolution_SpinBox->setValue(target_Curve->measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution/coeff);
	target_Common_Part->crystal_Resolution_Units_Label->setText(target_Curve->angular_Units);

	target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
	target_Curve->refresh_Description_Label();

	refresh_Plot_Axes_Labels();

	angular_Divergence_SpinBox->blockSignals(false);
	target_Common_Part->crystal_Resolution_SpinBox->blockSignals(false);
	at_Fixed_Beam_Theta_0_SpinBox->blockSignals(false);

	// independently of main calculation
	target_Curve->calc_Measured_cos2_k();
}

void Detector_Target_Curve_Part::fill_At_Fixed_Wavelength_Label()
{
	if(	target_Curve->spectral_Units == wavelength_Units_List[angstrom] ||
		target_Curve->spectral_Units == wavelength_Units_List[nm]		  )
	{
		at_Fixed_Wavelength_Label->setText("At fixed wavelength");
	} else
	{
		at_Fixed_Wavelength_Label->setText("At fixed energy");
	}
}

void Detector_Target_Curve_Part::fill_Spectral_Width_Label()
{
	if(	target_Curve->spectral_Units == wavelength_Units_List[angstrom] ||
		target_Curve->spectral_Units == wavelength_Units_List[nm]	   )
	{
		spectral_Width_Label->setText("      Spectral width, FWHM, " + Delta_Big_Sym + Lambda_Sym+"/"+Lambda_Sym);
	} else
	{
		spectral_Width_Label->setText("      Spectral width, FWHM, " + Delta_Big_Sym + "E/E");
	}
}

void Detector_Target_Curve_Part::refresh_At_Fixed_Wavelength_Units()
{
	at_Fixed_Wavelength_SpinBox->blockSignals(true);

	target_Curve->spectral_Units = at_Fixed_Wavelength_Units_ComboBox->currentText();

	double coeff = wavelength_Coefficients_Map.value(target_Curve->spectral_Units);
	at_Fixed_Wavelength_SpinBox->setValue(Global_Variables::wavelength_Energy(target_Curve->spectral_Units,target_Curve->measurement.wavelength.value)/coeff);

	fill_Spectral_Width_Label();
	fill_At_Fixed_Wavelength_Label();

	target_Curve->refresh_Description_Label();
	at_Fixed_Wavelength_SpinBox->blockSignals(false);

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
			curve_Plot_1D->set_Title_Text();
		}
	}
}

void Detector_Target_Curve_Part::refresh_At_Fixed_Beam_Theta_0_Value()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
	target_Curve->measurement.beam_Theta_0_Angle.value = at_Fixed_Beam_Theta_0_SpinBox->value()*coeff;

	target_Curve->refresh_Description_Label();

	// independently of main calculation
	target_Curve->calc_Measured_cos2_k();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
			curve_Plot_1D->set_Title_Text();
		}
	}
}

void Detector_Target_Curve_Part::refresh_At_Fixed_Wavelength_Value()
{
	double coeff = wavelength_Coefficients_Map.value(target_Curve->spectral_Units);
	target_Curve->measurement.wavelength.value = Global_Variables::wavelength_Energy(target_Curve->spectral_Units, at_Fixed_Wavelength_SpinBox->value()*coeff);
	target_Curve->refresh_Description_Label();

	// independently of main calculation
	target_Curve->calc_Measured_cos2_k();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
			curve_Plot_1D->set_Title_Text();
		}
	}
}

void Detector_Target_Curve_Part::refresh_Plot_Axes_Labels()
{
	target_Curve_Plot->value_Type_Text = target_Curve->curve.value_Type;
	target_Curve_Plot->argument_Type_Text = argument_Types[Detector_Polar_Angle];
	target_Curve_Plot->argument_Sym_Text = "";
	target_Curve_Plot->argument_Units_Text = target_Curve->angular_Units;
	target_Curve_Plot->refresh_Labels_1D();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
			curve_Plot_1D->set_Title_Text();
			curve_Plot_1D->refresh_Labels();
		}
	}
}

void Detector_Target_Curve_Part::connecting()
{
	/// argument box
	// argument units
	connect(arg_Units_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		refresh_Argument_Units();
		global_Multilayer_Approach->global_Recalculate();
	});
	// argument shift
	connect(arg_Shift_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.horizontal_Arg_Shift = arg_Shift_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data_1D();
		target_Curve->refresh_Description_Label();

		// independently of main calculation
		target_Curve->calc_Measured_cos2_k();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->plot_All_Data();
			}
		}
		global_Multilayer_Approach->global_Recalculate();
	});
	// argument factor
	connect(arg_Factor_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.horizontal_Arg_Factor= arg_Factor_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data_1D();
		target_Curve->refresh_Description_Label();

		// independently of main calculation
		target_Curve->calc_Measured_cos2_k();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->plot_All_Data();
			}
		}
		global_Multilayer_Approach->global_Recalculate();
	});
	// subinterval
	connect(main_Subinterval_Checkbox, &QCheckBox::toggled, this, [=]
	{
		target_Curve->curve.use_Subinterval = main_Subinterval_Checkbox->isChecked();

		invert_Subinterval_Checkbox		   ->setEnabled(target_Curve->curve.use_Subinterval);
		horizontal_From_Subinterval_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
		horizontal_And_Subinterval_Label   ->setEnabled(target_Curve->curve.use_Subinterval);
		horizontal_To_Subinterval_SpinBox  ->setEnabled(target_Curve->curve.use_Subinterval);

		target_Curve_Plot->subinterval_Changed_Replot_1D();
		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->subinterval_Changed_Replot();
			}
		}
	});
	// outer area
	connect(invert_Subinterval_Checkbox, &QCheckBox::toggled, this, [=]
	{
		target_Curve->curve.outer_Area = invert_Subinterval_Checkbox->isChecked();

		target_Curve_Plot->subinterval_Changed_Replot_1D();
		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->subinterval_Changed_Replot();
			}
		}
	});
	connect(horizontal_From_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.subinterval_Left = horizontal_From_Subinterval_SpinBox->value();

		horizontal_To_Subinterval_SpinBox->setRange(target_Curve->curve.subinterval_Left,horizontal_To_Subinterval_SpinBox->maximum());
		horizontal_To_Subinterval_SpinBox->setValue(max(target_Curve->curve.subinterval_Left,target_Curve->curve.subinterval_Right));

		target_Curve_Plot->subinterval_Changed_Replot_1D();
		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->subinterval_Changed_Replot();
			}
		}
	});
	connect(horizontal_To_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.subinterval_Right = horizontal_To_Subinterval_SpinBox->value();

		horizontal_From_Subinterval_SpinBox->setRange(horizontal_From_Subinterval_SpinBox->minimum(),target_Curve->curve.subinterval_Right);
		horizontal_From_Subinterval_SpinBox->setValue(min(target_Curve->curve.subinterval_Left,target_Curve->curve.subinterval_Right));

		target_Curve_Plot->subinterval_Changed_Replot_1D();
		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->subinterval_Changed_Replot();
			}
		}
	});

	/// value box
	// value shift
	connect(val_Shift_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.val_Shift = val_Shift_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data_1D();
		target_Curve->refresh_Description_Label();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->plot_All_Data();
			}
		}
	});
	// value factor
	connect(val_Factor_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.val_Factor.value = val_Factor_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data_1D();
		target_Curve->refresh_Description_Label();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->plot_All_Data();
			}
		}
	});
	connect(val_Factor_Min_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.val_Factor.fit.min = val_Factor_Min_SpinBox->value();
	});
	connect(val_Factor_Max_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.val_Factor.fit.max = val_Factor_Max_SpinBox->value();
	});
	// beam intensity
	connect(norm_On_Beam_Intensity, &QCheckBox::toggled, this, [=]
	{
		target_Curve->curve.divide_On_Beam_Intensity = norm_On_Beam_Intensity->isChecked();

		beam_Intensity_Start_Label	  ->setEnabled(target_Curve->curve.divide_On_Beam_Intensity);
		beam_Intensity_Initial_SpinBox->setEnabled(target_Curve->curve.divide_On_Beam_Intensity);
		beam_Intensity_Final_CheckBox ->setEnabled(target_Curve->curve.divide_On_Beam_Intensity);
		beam_Intensity_Final_SpinBox  ->setEnabled(target_Curve->curve.divide_On_Beam_Intensity && target_Curve->curve.use_Final_Intensity);

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data_1D();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->plot_All_Data();
			}
		}
	});
	connect(beam_Intensity_Initial_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.beam_Intensity_Initial= beam_Intensity_Initial_SpinBox->value();
		if(!target_Curve->curve.use_Final_Intensity)
		{
			target_Curve->curve.beam_Intensity_Final = target_Curve->curve.beam_Intensity_Initial;
		}

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data_1D();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->plot_All_Data();
			}
		}
	});
	connect(beam_Intensity_Final_CheckBox, &QCheckBox::toggled, this, [=]
	{
		target_Curve->curve.use_Final_Intensity = beam_Intensity_Final_CheckBox->isChecked();

		beam_Intensity_Final_SpinBox->setEnabled(target_Curve->curve.divide_On_Beam_Intensity && target_Curve->curve.use_Final_Intensity);
		if(target_Curve->curve.use_Final_Intensity)
		{
			target_Curve->curve.beam_Intensity_Final = beam_Intensity_Final_SpinBox->value();
		} else
		{
			target_Curve->curve.beam_Intensity_Final = target_Curve->curve.beam_Intensity_Initial;
		}

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data_1D();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->plot_All_Data();
			}
		}
	});
	connect(beam_Intensity_Final_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.beam_Intensity_Final= beam_Intensity_Final_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data_1D();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->plot_All_Data();
			}
		}
	});

	/// beam box
	// at fixed
	connect(at_Fixed_Beam_Theta_0_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_At_Fixed_Beam_Theta_0_Value();
		global_Multilayer_Approach->global_Recalculate();
	});
	// at fixed wavelength
	connect(at_Fixed_Wavelength_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_At_Fixed_Wavelength_Value();
		global_Multilayer_Approach->global_Recalculate();
	});
	// at fixed units
	connect(at_Fixed_Wavelength_Units_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		refresh_At_Fixed_Wavelength_Units();
	});
	// polarization
	connect(polarization_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.polarization = polarization_SpinBox->value();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
				curve_Plot_1D->set_Title_Text();
			}
		}
		global_Multilayer_Approach->global_Recalculate();
	});
	// background
	connect(background_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.background = background_SpinBox->value();

		global_Multilayer_Approach->global_Recalculate();
	});
	// spectral width
	connect(spectral_Width_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.spectral_Distribution.FWHM_distribution = spectral_Width_SpinBox->value();

		global_Multilayer_Approach->global_Recalculate();
	});
	// angular divergence
	connect(angular_Divergence_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution = angular_Divergence_SpinBox->value()*coeff;

		global_Multilayer_Approach->global_Recalculate();
	});
	// beam distribution button
	connect(setup_Beam_Distribution_Button, &QPushButton::clicked, this, [=]
	{
		Distribution_Editor* distribution_Editor = new Distribution_Editor(target_Curve->measurement,
																		   target_Curve->spectral_Units,
																		   target_Curve->angular_Units,
																		   spectral_Width_SpinBox,
																		   angular_Divergence_SpinBox);
		distribution_Editor->show();
	});
}

