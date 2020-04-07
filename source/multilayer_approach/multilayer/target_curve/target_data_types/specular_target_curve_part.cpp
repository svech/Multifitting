#include "specular_target_curve_part.h"

Specular_Target_Curve_Part::Specular_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent) :
	target_Curve(target_Curve),
	QWidget(parent)
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setContentsMargins(4,4,4,0);
		main_Layout->setSpacing(0);

	create_Argument_GroupBox();
	create_Value_GroupBox();
	create_Beam_GroupBox();
	create_Detector_GroupBox();
}

void Specular_Target_Curve_Part::create_Argument_GroupBox()
{
	// initialization
	if(target_Curve->measurement.argument_Type == no_Argument_Type)
	{
		target_Curve->measurement.argument_Type = argument_Types[Beam_Grazing_Angle];
	}

	double arg_Coeff = 1;
	if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])  { arg_Coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units); }
	if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])   { arg_Coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units); }

	double min, max;
	if(target_Curve->loaded_And_Ready)
	{
		min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end())/arg_Coeff;
		max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end())/arg_Coeff;
	} else
	{
		min = -MAX_DOUBLE;
		max =  MAX_DOUBLE;
	}

	QGroupBox* argument_GroupBox = new QGroupBox("Argument", this);
	main_Layout->addWidget(argument_GroupBox);

	QHBoxLayout* argument_GroupBox_Layout = new QHBoxLayout(argument_GroupBox);
		argument_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// argument type
	{
		arg_Type_ComboBox = new QComboBox;
			arg_Type_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			arg_Type_ComboBox->addItem(argument_Types[Beam_Grazing_Angle]);
			arg_Type_ComboBox->addItem(argument_Types[Wavelength_Energy]);
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
			if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				arg_Units_ComboBox->addItems(angle_Units_List);
				arg_Units_ComboBox->setCurrentText(target_Curve->curve.angular_Units);
			}
			if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
				arg_Units_ComboBox->addItems(wavelength_Units_List);
				arg_Units_ComboBox->setCurrentText(target_Curve->curve.spectral_Units);
			}
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
			arg_Shift_SpinBox->setValue(target_Curve->curve.horizontal_Arg_Shift/arg_Coeff);
			arg_Shift_SpinBox->setSingleStep(0.001/arg_Coeff);
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
		main_Subinterval_Checkbox = new QCheckBox("Use only data between argument");
			main_Subinterval_Checkbox->setCheckable(true);
			main_Subinterval_Checkbox->setChecked(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(main_Subinterval_Checkbox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		horizontal_From_Subinterval_SpinBox = new MyDoubleSpinBox;
			horizontal_From_Subinterval_SpinBox->setAccelerated(true);
			horizontal_From_Subinterval_SpinBox->setRange(min, max);
			horizontal_From_Subinterval_SpinBox->setDecimals(4);
			horizontal_From_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Left/arg_Coeff);
			horizontal_From_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Right-target_Curve->curve.subinterval_Left)/arg_Coeff/100.);
			horizontal_From_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			horizontal_From_Subinterval_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(horizontal_From_Subinterval_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_From_Subinterval_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* horizontal_And_Subinterval_Label = new QLabel("  and  ");
		argument_GroupBox_Layout->addWidget(horizontal_And_Subinterval_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		horizontal_To_Subinterval_SpinBox = new MyDoubleSpinBox;
			horizontal_To_Subinterval_SpinBox->setAccelerated(true);
			horizontal_To_Subinterval_SpinBox->setRange(min, max);
			horizontal_To_Subinterval_SpinBox->setDecimals(4);
			horizontal_To_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Right/arg_Coeff);
			horizontal_To_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Right-target_Curve->curve.subinterval_Left)/arg_Coeff/100.);
			horizontal_To_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		argument_GroupBox_Layout->addWidget(horizontal_To_Subinterval_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_To_Subinterval_SpinBox);
	}
}

void Specular_Target_Curve_Part::create_Value_GroupBox()
{
	// initialization
	if(target_Curve->curve.value_Type == no_Value_Type)
	{
		target_Curve->curve.value_Type = value_Types[Reflectance];
	}

	QGroupBox* value_GroupBox = new QGroupBox("Value", this);
	main_Layout->addWidget(value_GroupBox);

	QHBoxLayout* value_GroupBox_Layout = new QHBoxLayout(value_GroupBox);
		value_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// value type
	{
		value_Type_ComboBox = new QComboBox;
			value_Type_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

			value_Type_ComboBox->addItem(value_Types[Reflectance]);
			value_Type_ComboBox->addItem(value_Types[Transmittance]);
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
		Global_Variables::resize_Line_Edit(val_Factor_Min_SpinBox, false);

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
		Global_Variables::resize_Line_Edit(val_Factor_Max_SpinBox, false);
	}
	// norm on beam intensity
	{
		norm_On_Beam_Intensity = new QCheckBox("Divide on beam intensity:");
			norm_On_Beam_Intensity->setChecked(target_Curve->curve.divide_On_Beam_Intensity);
		value_GroupBox_Layout->addWidget(norm_On_Beam_Intensity);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* beam_Intensity_Start_Label = new QLabel("initial");
		value_GroupBox_Layout->addWidget(beam_Intensity_Start_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Intensity_Initial_SpinBox = new MyDoubleSpinBox;
			beam_Intensity_Initial_SpinBox->setAccelerated(true);
			beam_Intensity_Initial_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Intensity_Initial_SpinBox->setDecimals(0);
			beam_Intensity_Initial_SpinBox->setValue(target_Curve->curve.beam_Intensity_Initial);
			beam_Intensity_Initial_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Intensity_Initial_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Intensity_Initial_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		Global_Variables::resize_Line_Edit(beam_Intensity_Initial_SpinBox, false);
		value_GroupBox_Layout->addWidget(beam_Intensity_Initial_SpinBox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Intensity_Final_CheckBox = new QCheckBox("final");
			beam_Intensity_Final_CheckBox->setChecked(target_Curve->curve.use_Final_Intensity);
		value_GroupBox_Layout->addWidget(beam_Intensity_Final_CheckBox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Intensity_Final_SpinBox = new MyDoubleSpinBox;
			beam_Intensity_Final_SpinBox->setAccelerated(true);
			beam_Intensity_Final_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Intensity_Final_SpinBox->setDecimals(0);
			beam_Intensity_Final_SpinBox->setValue(target_Curve->curve.beam_Intensity_Final);
			beam_Intensity_Final_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Intensity_Final_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Intensity_Final_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		Global_Variables::resize_Line_Edit(beam_Intensity_Final_SpinBox, false);
		value_GroupBox_Layout->addWidget(beam_Intensity_Final_SpinBox,0,Qt::AlignLeft);
	}
}

void Specular_Target_Curve_Part::create_Beam_GroupBox()
{
	QGroupBox* beam_GroupBox = new QGroupBox("Beam", this);
	main_Layout->addWidget(beam_GroupBox);

	QGridLayout* beam_GroupBox_Layout = new QGridLayout(beam_GroupBox);
		beam_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// at fixed
	{
		at_Fixed_Label = new QLabel;
			if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				if(	target_Curve->curve.spectral_Units == wavelength_Units_List[angstrom] ||
					target_Curve->curve.spectral_Units == wavelength_Units_List[nm]		  ||
					target_Curve->curve.spectral_Units == wavelength_Units_List[mcm]	   )
				{
					at_Fixed_Label->setText("At fixed wavelength:");
				} else
				{
					at_Fixed_Label->setText("At fixed energy:");
				}
			}
			if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
				at_Fixed_Label->setText("At fixed grazing angle "+Theta_Sym+Zero_Subscript_Sym);
			}
		beam_GroupBox_Layout->addWidget(at_Fixed_Label,0,0,1,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_SpinBox = new MyDoubleSpinBox;
			at_Fixed_SpinBox->setAccelerated(true);
			at_Fixed_SpinBox->setRange(0, MAX_DOUBLE);
			at_Fixed_SpinBox->setDecimals(7);
			at_Fixed_SpinBox->setSingleStep(0.01);
			at_Fixed_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			at_Fixed_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		beam_GroupBox_Layout->addWidget(at_Fixed_SpinBox,0,2,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(at_Fixed_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* at_Fixed_Units_Label = new QLabel("  Units");
		beam_GroupBox_Layout->addWidget(at_Fixed_Units_Label,0,3,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_Units_ComboBox = new QComboBox;
			if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				double arg_Coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);
				at_Fixed_SpinBox->setValue(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.wavelength.value/arg_Coeff));

				at_Fixed_Units_ComboBox->addItems(wavelength_Units_List);
				at_Fixed_Units_ComboBox->setCurrentText(target_Curve->curve.spectral_Units);
			}
			if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
				double arg_Coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
				at_Fixed_Label->setText("At fixed grazing angle "+Theta_Sym+Zero_Subscript_Sym);
				at_Fixed_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Angle.value/arg_Coeff);

				at_Fixed_Units_ComboBox->addItems(angle_Units_List);
				at_Fixed_Units_ComboBox->setCurrentText(target_Curve->curve.angular_Units);
			}
			at_Fixed_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		beam_GroupBox_Layout->addWidget(at_Fixed_Units_ComboBox,0,4,Qt::AlignLeft);
	}
	// polarization
	{
		QLabel* polarization_Label = new QLabel("Polarization");
		beam_GroupBox_Layout->addWidget(polarization_Label,1,0,Qt::AlignRight);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		polarization_SpinBox = new MyDoubleSpinBox;
			polarization_SpinBox->setAccelerated(true);
			polarization_SpinBox->setRange(-1, 1);
			polarization_SpinBox->setDecimals(3);
			polarization_SpinBox->setValue(target_Curve->measurement.polarization);
			polarization_SpinBox->setSingleStep(0.01);
			polarization_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			polarization_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		beam_GroupBox_Layout->addWidget(polarization_SpinBox,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(polarization_SpinBox);
	}
	// background
	{
		QLabel* background_Label = new QLabel("Background");
		beam_GroupBox_Layout->addWidget(background_Label,1,2,Qt::AlignRight);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		background_SpinBox = new MyDoubleSpinBox;
			background_SpinBox->setAccelerated(true);
			background_SpinBox->setRange(0, MAX_DOUBLE);
			background_SpinBox->setDecimals(10);
			background_SpinBox->setValue(target_Curve->measurement.background);
			background_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			background_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			background_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		beam_GroupBox_Layout->addWidget(background_SpinBox,1,3,1,2,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(background_SpinBox);
	}
	// spectral distribution
	{
		QLabel* spectral_Width_Label = new QLabel("    Spectral width, FWHM, " + Delta_Big_Sym + Lambda_Sym+"/"+Lambda_Sym);
		beam_GroupBox_Layout->addWidget(spectral_Width_Label,0,5,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		spectral_Width_SpinBox = new MyDoubleSpinBox;
			spectral_Width_SpinBox->setAccelerated(true);
			spectral_Width_SpinBox->setRange(0, MAX_DOUBLE);
			spectral_Width_SpinBox->setDecimals(6);
			spectral_Width_SpinBox->setValue(target_Curve->measurement.spectral_Distribution.FWHM_distribution);
			spectral_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			spectral_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			spectral_Width_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		beam_GroupBox_Layout->addWidget(spectral_Width_SpinBox,0,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(spectral_Width_SpinBox);
	}
	// angular divergence
	{
		QLabel* angular_Divergence_Label = new QLabel("    Angular divergence, FWHM, " + Delta_Big_Sym + Theta_Sym + Zero_Subscript_Sym);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Label,1,5,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double arg_Coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);

		angular_Divergence_SpinBox = new MyDoubleSpinBox;
			angular_Divergence_SpinBox->setAccelerated(true);
			angular_Divergence_SpinBox->setRange(0, MAX_DOUBLE);
			angular_Divergence_SpinBox->setDecimals(6);
			angular_Divergence_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/arg_Coeff);
			angular_Divergence_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			angular_Divergence_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			angular_Divergence_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		beam_GroupBox_Layout->addWidget(angular_Divergence_SpinBox,1,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(angular_Divergence_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		angular_Divergence_Units_Label = new QLabel(target_Curve->curve.angular_Units);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Units_Label,1,7,Qt::AlignLeft);
	}
	// set up distribution
	{
		QLabel* spacing_Label = new QLabel("         ");
		beam_GroupBox_Layout->addWidget(spacing_Label,0,8,2,1,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		setup_Button = new QPushButton("Set up distribution");
		beam_GroupBox_Layout->addWidget(setup_Button,0,9,2,1,Qt::AlignLeft);
	}

//	// interpolation/sampling
//	{
//		QGroupBox* setup_GroupBox = new QGroupBox;
//		setup_GroupBox->setContentsMargins(0,0,0,0);
//		QGridLayout* setup_GroupBox_Layout = new QGridLayout(setup_GroupBox);
//		beam_GroupBox_Layout->addWidget(setup_GroupBox, 1,3,2+int(target_Curve->measurement.measurement_Type == measurement_Types[GISAS]),2);

//		QRadioButton* interpolation_RadioButton = new QRadioButton("Interpolation (fast)");
//		setup_GroupBox_Layout->addWidget(interpolation_RadioButton,0,0,1,1,Qt::AlignLeft);

//		QRadioButton* sampling_RadioButton = new QRadioButton("Sampling (precise)");
//		setup_GroupBox_Layout->addWidget(sampling_RadioButton,1,0,1,1,Qt::AlignLeft);
//		QButtonGroup* buttonGroup = new QButtonGroup;
//			buttonGroup->addButton(interpolation_RadioButton);
//			buttonGroup->addButton(sampling_RadioButton);
//	}
}

void Specular_Target_Curve_Part::create_Detector_GroupBox()
{

}

void Specular_Target_Curve_Part::connecting()
{
//	connect(main_Subinterval_Checkbox, &QCheckBox::toggled, this, [=]
//	{
//		target_Curve->curve.use_Subinterval = main_Subinterval_Checkbox->isChecked();

//		horizontal_Label					->setDisabled(!target_Curve->curve.use_Subinterval);
//		vertical_Label						->setDisabled(!target_Curve->curve.use_Subinterval);
//		horizontal_From_Subinterval_SpinBox	->setDisabled(!target_Curve->curve.use_Subinterval);
//		vertical_From_Subinterval_SpinBox	->setDisabled(!target_Curve->curve.use_Subinterval);
//		horizontal_And_Subinterval_Label	->setDisabled(!target_Curve->curve.use_Subinterval);
//		vertical_And_Subinterval_Label		->setDisabled(!target_Curve->curve.use_Subinterval);
//		horizontal_To_Subinterval_SpinBox	->setDisabled(!target_Curve->curve.use_Subinterval);
//		vertical_To_Subinterval_SpinBox		->setDisabled(!target_Curve->curve.use_Subinterval);

//		horizontal_From_Subinterval_SpinBox	->setMaximum(target_Curve->curve.subinterval_Right);
//		horizontal_To_Subinterval_SpinBox	->setMinimum(target_Curve->curve.subinterval_Left);
//		vertical_From_Subinterval_SpinBox	->setMaximum(target_Curve->curve.subinterval_Top);
//		vertical_To_Subinterval_SpinBox		->setMinimum(target_Curve->curve.subinterval_Bottom);

//		// show/hide subinterval on plot
//		target_Curve_Plot->start_Rect->setVisible(target_Curve->curve.use_Subinterval);
//		target_Curve_Plot->end_Rect->setVisible(target_Curve->curve.use_Subinterval);
//		if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS] )
//		{
//			target_Curve_Plot->top_Rect->setVisible(target_Curve->curve.use_Subinterval);
//			target_Curve_Plot->bottom_Rect->setVisible(target_Curve->curve.use_Subinterval);
//		}
//		target_Curve_Plot->subinterval_Changed_Replot();
//		Global_Variables::replot_All_Graphs();
//	});

//	connect(horizontal_From_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
//	{
//		target_Curve->curve.subinterval_Left = horizontal_From_Subinterval_SpinBox->value();
//		horizontal_To_Subinterval_SpinBox->setRange(target_Curve->curve.subinterval_Left,horizontal_To_Subinterval_SpinBox->maximum());
//		target_Curve_Plot->subinterval_Changed_Replot();
//		Global_Variables::replot_All_Graphs();
//	});
//	connect(vertical_From_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
//	{
//		target_Curve->curve.subinterval_Bottom = vertical_From_Subinterval_SpinBox->value();
//		vertical_To_Subinterval_SpinBox->setRange(target_Curve->curve.subinterval_Bottom,vertical_To_Subinterval_SpinBox->maximum());
//		target_Curve_Plot->subinterval_Changed_Replot();
//		Global_Variables::replot_All_Graphs();
//	});
//	connect(horizontal_To_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
//	{
//		target_Curve->curve.subinterval_Right = horizontal_To_Subinterval_SpinBox->value();
//		horizontal_From_Subinterval_SpinBox->setRange(horizontal_From_Subinterval_SpinBox->minimum(),target_Curve->curve.subinterval_Right);
//		target_Curve_Plot->subinterval_Changed_Replot();
//		Global_Variables::replot_All_Graphs();
//	});
//	connect(vertical_To_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
//	{
//		target_Curve->curve.subinterval_Top = vertical_To_Subinterval_SpinBox->value();
//		vertical_From_Subinterval_SpinBox->setRange(vertical_From_Subinterval_SpinBox->minimum(),target_Curve->curve.subinterval_Top);
//		target_Curve_Plot->subinterval_Changed_Replot();
//		Global_Variables::replot_All_Graphs();
//	});
//	main_Subinterval_Checkbox->toggled(target_Curve->curve.use_Subinterval);






//		connect(val_Factor_Min_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
//		{
//			target_Curve->curve.val_Factor.fit.min = val_Factor_Min_SpinBox->value();
//		});
//		connect(val_Factor_Max_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
//		{
//			target_Curve->curve.val_Factor.fit.max = val_Factor_Max_SpinBox->value();
//		});




	// -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
//		connect(norm_On_Beam_Intensity, &QCheckBox::toggled, this, [=]
//		{
//			target_Curve->curve.divide_On_Beam_Intensity = norm_On_Beam_Intensity->isChecked();
//			beam_Intensity_Start_Label  ->setDisabled(!target_Curve->curve.divide_On_Beam_Intensity);
//			beam_Intensity_Start_SpinBox->setDisabled(!target_Curve->curve.divide_On_Beam_Intensity);
//			beam_Intensity_Final_Label  ->setDisabled(!target_Curve->curve.divide_On_Beam_Intensity);
//			beam_Intensity_Final_SpinBox->setDisabled(!target_Curve->curve.divide_On_Beam_Intensity);
//			refresh_Beam_Intensity();
//		});
//		connect(beam_Intensity_Start_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Beam_Intensity);
//		connect(beam_Intensity_Final_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Beam_Intensity);

//		norm_On_Beam_Intensity->toggled(target_Curve->curve.divide_On_Beam_Intensity);

	//	connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::change_At_Fixed_Units_ComboBox);

//	connect(setup_Button, &QPushButton::clicked, this, [=]
//	{
//		Distribution_Editor* distribution_Editor = new Distribution_Editor;
//		distribution_Editor->show();
//	});

}
