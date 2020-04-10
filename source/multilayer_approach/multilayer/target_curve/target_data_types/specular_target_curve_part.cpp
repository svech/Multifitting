#include "specular_target_curve_part.h"

Specular_Target_Curve_Part::Specular_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent) :
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
	create_Detector_GroupBox();
	create_Footptint_GroupBox();

	refresh_Plot_Axes_Labels();;
	connecting();
}

void Specular_Target_Curve_Part::create_Argument_GroupBox()
{
	// initialization
	if(target_Curve->measurement.argument_Type == no_Argument_Type)
	{
		target_Curve->measurement.argument_Type = argument_Types[Beam_Grazing_Angle];
	}

	QGroupBox* argument_GroupBox = new QGroupBox("Argument");
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
			fill_Argument_Units();
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
		main_Subinterval_Checkbox = new QCheckBox("Use only data between argument");
			main_Subinterval_Checkbox->setCheckable(true);
			main_Subinterval_Checkbox->setChecked(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(main_Subinterval_Checkbox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		horizontal_From_Subinterval_SpinBox = new MyDoubleSpinBox;
			horizontal_From_Subinterval_SpinBox->setAccelerated(true);
			horizontal_From_Subinterval_SpinBox->setDecimals(4);
			horizontal_From_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Right-target_Curve->curve.subinterval_Left)/100.);
			horizontal_From_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			horizontal_From_Subinterval_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
			horizontal_From_Subinterval_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(horizontal_From_Subinterval_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_From_Subinterval_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		horizontal_And_Subinterval_Label = new QLabel("  and  ");
			horizontal_And_Subinterval_Label->setEnabled(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(horizontal_And_Subinterval_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		horizontal_To_Subinterval_SpinBox = new MyDoubleSpinBox;
			horizontal_To_Subinterval_SpinBox->setAccelerated(true);
			horizontal_To_Subinterval_SpinBox->setDecimals(4);
			horizontal_To_Subinterval_SpinBox->setSingleStep((target_Curve->curve.subinterval_Right-target_Curve->curve.subinterval_Left)/100.);
			horizontal_To_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			horizontal_To_Subinterval_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(horizontal_To_Subinterval_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(horizontal_To_Subinterval_SpinBox);

		reset_Subinterval();
	}
}

void Specular_Target_Curve_Part::create_Value_GroupBox()
{
	// initialization
	if(target_Curve->curve.value_Type == no_Value_Type)
	{
		target_Curve->curve.value_Type = value_Types[Reflectance];
	}

	QGroupBox* value_GroupBox = new QGroupBox("Value");
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

void Specular_Target_Curve_Part::create_Beam_GroupBox()
{
	QGroupBox* beam_GroupBox = new QGroupBox("Beam");
	main_Layout->addWidget(beam_GroupBox);

	QGridLayout* beam_GroupBox_Layout = new QGridLayout(beam_GroupBox);
		beam_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// at fixed
	{
		at_Fixed_Label = new QLabel;
			fill_At_Fixed_Label();
		beam_GroupBox_Layout->addWidget(at_Fixed_Label,0,0,1,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_SpinBox = new MyDoubleSpinBox;
			at_Fixed_SpinBox->setAccelerated(true);
			at_Fixed_SpinBox->setDecimals(7);
			at_Fixed_SpinBox->setSingleStep(0.01);
			fill_At_Fixed_Value();
			at_Fixed_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			at_Fixed_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(at_Fixed_SpinBox,0,2,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(at_Fixed_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* at_Fixed_Units_Label = new QLabel("  Units");
		beam_GroupBox_Layout->addWidget(at_Fixed_Units_Label,0,3,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_Units_ComboBox = new QComboBox;
			fill_At_Fixed_Units();
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
			polarization_SpinBox->setProperty(min_Size_Property, 50);
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
		QLabel* spectral_Width_Label = new QLabel("      Spectral width, FWHM, " + Delta_Big_Sym + Lambda_Sym+"/"+Lambda_Sym);
		beam_GroupBox_Layout->addWidget(spectral_Width_Label,0,5,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		spectral_Width_SpinBox = new MyDoubleSpinBox;
			spectral_Width_SpinBox->setAccelerated(true);
			spectral_Width_SpinBox->setRange(0, MAX_DOUBLE);
			spectral_Width_SpinBox->setDecimals(7);
			spectral_Width_SpinBox->setValue(target_Curve->measurement.spectral_Distribution.FWHM_distribution);
			spectral_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			spectral_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			spectral_Width_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(spectral_Width_SpinBox,0,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(spectral_Width_SpinBox);
	}
	// angular divergence
	{
		QLabel* angular_Divergence_Label = new QLabel("      Angular divergence, FWHM, " + Delta_Big_Sym + Theta_Sym + Zero_Subscript_Sym);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Label,1,5,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double arg_Coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);

		angular_Divergence_SpinBox = new MyDoubleSpinBox;
			angular_Divergence_SpinBox->setAccelerated(true);
			angular_Divergence_SpinBox->setRange(0, MAX_DOUBLE);
			angular_Divergence_SpinBox->setDecimals(7);
			angular_Divergence_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/arg_Coeff);
			angular_Divergence_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			angular_Divergence_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			angular_Divergence_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
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

		setup_Beam_Distribution_Button = new QPushButton("Set up distribution");
		beam_GroupBox_Layout->addWidget(setup_Beam_Distribution_Button,0,9,2,1,Qt::AlignLeft);
	}
}

void Specular_Target_Curve_Part::create_Detector_GroupBox()
{
	QGroupBox* detector_GroupBox = new QGroupBox("Detector");
	main_Layout->addWidget(detector_GroupBox);

	QVBoxLayout* detector_GroupBox_Layout = new QVBoxLayout(detector_GroupBox);
		detector_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// detector type
	{
		QHBoxLayout* detector_Type_Layout = new QHBoxLayout;
			detector_Type_Layout->setAlignment(Qt::AlignLeft);
		detector_GroupBox_Layout->addLayout(detector_Type_Layout);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* detector_Type_Label = new QLabel("Detector type");
		detector_Type_Layout->addWidget(detector_Type_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detector_Type_ComboBox = new QComboBox;
			detector_Type_ComboBox->addItem(detectors[Slit]);
			detector_Type_ComboBox->addItem(detectors[Crystal]);
			detector_Type_ComboBox->setFixedWidth(133);
		detector_Type_Layout->addWidget(detector_Type_ComboBox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detectors_Stack = new QStackedWidget;
		detector_GroupBox_Layout->addWidget(detectors_Stack);
	}
	// slit
	{
		QWidget* slit_Page = new QWidget;
		detectors_Stack->addWidget(slit_Page);
		QHBoxLayout* slit_Layout = new QHBoxLayout(slit_Page);
			slit_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* slit_Width_Label = new QLabel("Slit width");
		slit_Layout->addWidget(slit_Width_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		slit_Width_SpinBox = new MyDoubleSpinBox;
			slit_Width_SpinBox->setAccelerated(true);
			slit_Width_SpinBox->setRange(0, MAX_DOUBLE);
			slit_Width_SpinBox->setDecimals(2);
			slit_Width_SpinBox->setValue(target_Curve->measurement.detector_1D.slit_Width);
			slit_Width_SpinBox->setSingleStep(0.01);
			slit_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			slit_Width_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		slit_Layout->addWidget(slit_Width_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(slit_Width_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Width_Label = new QLabel("mm");
		slit_Layout->addWidget(mm_Width_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* spacing_Label = new QLabel("                   ");
		slit_Layout->addWidget(spacing_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* slit_Distance_Label = new QLabel("Distance from detector to sample");
		slit_Layout->addWidget(slit_Distance_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		slit_Distance_SpinBox = new MyDoubleSpinBox;
			slit_Distance_SpinBox->setAccelerated(true);
			slit_Distance_SpinBox->setRange(0, MAX_DOUBLE);
			slit_Distance_SpinBox->setDecimals(1);
			slit_Distance_SpinBox->setValue(target_Curve->measurement.detector_1D.distance_To_Sample);
			slit_Distance_SpinBox->setSingleStep(1);
			slit_Distance_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			slit_Distance_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		slit_Layout->addWidget(slit_Distance_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(slit_Distance_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Distance_Label = new QLabel("mm");
		slit_Layout->addWidget(mm_Distance_Label,0,Qt::AlignLeft);
	}
	// crystal
	{
		QWidget* crystal_Page = new QWidget;
		detectors_Stack->addWidget(crystal_Page);
		QHBoxLayout* crystal_Layout = new QHBoxLayout(crystal_Page);
			crystal_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* crystal_Width_Label = new QLabel("Angular resolution, FWHM, "+Delta_Big_Sym+Theta_Sym);
		crystal_Layout->addWidget(crystal_Width_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double arg_Coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);

		crystal_Resolution_SpinBox = new MyDoubleSpinBox;
			crystal_Resolution_SpinBox->setAccelerated(true);
			crystal_Resolution_SpinBox->setRange(0, MAX_DOUBLE);
			crystal_Resolution_SpinBox->setDecimals(6);
			crystal_Resolution_SpinBox->setValue(target_Curve->measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution/arg_Coeff);
			crystal_Resolution_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			crystal_Resolution_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			crystal_Resolution_SpinBox->setProperty(min_Size_Property,TARGET_LINE_AT_FIXED_WIDTH);
		crystal_Layout->addWidget(crystal_Resolution_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(crystal_Resolution_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		crystal_Resolution_Units_Label = new QLabel(target_Curve->curve.angular_Units);
		crystal_Layout->addWidget(crystal_Resolution_Units_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* spacing_Label = new QLabel("             ");
		crystal_Layout->addWidget(spacing_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		setup_Crystal_Resolution_Button = new QPushButton("Set up resolution");
		crystal_Layout->addWidget(setup_Crystal_Resolution_Button,0,Qt::AlignLeft);
	}
	disable_Crystal_Detector_Type();
}

void Specular_Target_Curve_Part::create_Footptint_GroupBox()
{
	QGroupBox* footprint_GroupBox = new QGroupBox("Footptint and distortion");
	main_Layout->addWidget(footprint_GroupBox);

	QHBoxLayout* footprint_GroupBox_Layout = new QHBoxLayout(footprint_GroupBox);
		footprint_GroupBox_Layout->setAlignment(Qt::AlignLeft);
		footprint_GroupBox_Layout->setSpacing(0);

	// beam
	{
		QGroupBox* beam_Footprint_GroupBox = new QGroupBox("Beam");
		footprint_GroupBox_Layout->addWidget(beam_Footprint_GroupBox,0,Qt::AlignLeft);

		QGridLayout* beam_Footprint_GroupBox_Layout = new QGridLayout(beam_Footprint_GroupBox);
			beam_Footprint_GroupBox_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* beam_Footprint_Width_Label = new QLabel("Beam width, FWHM");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Width_Label,0,0,Qt::AlignLeft);

		beam_Footprint_Width_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Width_SpinBox->setAccelerated(true);
			beam_Footprint_Width_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Footprint_Width_SpinBox->setDecimals(3);
			beam_Footprint_Width_SpinBox->setValue(target_Curve->measurement.beam_Geometry.size);
			beam_Footprint_Width_SpinBox->setSingleStep(0.01);
			beam_Footprint_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Width_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Width_SpinBox,0,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Width_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_beam_Footprint_Width_Label = new QLabel("mm");
		beam_Footprint_GroupBox_Layout->addWidget(mm_beam_Footprint_Width_Label,0,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* beam_Footprint_Shape_Label = new QLabel("Beam profile smoothing");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Shape_Label,1,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Footprint_Shape_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Shape_SpinBox->setAccelerated(true);
			beam_Footprint_Shape_SpinBox->setRange(0, 1);
			beam_Footprint_Shape_SpinBox->setDecimals(3);
			beam_Footprint_Shape_SpinBox->setValue(target_Curve->measurement.beam_Geometry.smoothing);
			beam_Footprint_Shape_SpinBox->setSingleStep(0.01);
			beam_Footprint_Shape_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Shape_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Shape_SpinBox,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Shape_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		create_Beam_Plot();
		footprint_GroupBox_Layout->addWidget(beam_Profile_CustomPlot,0);
	}
	// sample
	{
		QGroupBox* sample_Footprint_GroupBox = new QGroupBox("Sample");
		footprint_GroupBox_Layout->addWidget(sample_Footprint_GroupBox,0,Qt::AlignLeft);

		QGridLayout* sample_GroupBox_Layout = new QGridLayout(sample_Footprint_GroupBox);
			sample_GroupBox_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* sample_Size_Label = new QLabel("Sample size");
		sample_GroupBox_Layout->addWidget(sample_Size_Label,0,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sample_Size_SpinBox = new MyDoubleSpinBox;
			sample_Size_SpinBox->setAccelerated(true);
			sample_Size_SpinBox->setRange(0, MAX_DOUBLE);
			sample_Size_SpinBox->setDecimals(2);
			sample_Size_SpinBox->setSingleStep(0.1);
			sample_Size_SpinBox->setValue(target_Curve->measurement.sample_Geometry.size);
			sample_Size_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			sample_Size_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		sample_GroupBox_Layout->addWidget(sample_Size_SpinBox,0,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(sample_Size_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Sample_Size_Label = new QLabel("mm");
		sample_GroupBox_Layout->addWidget(mm_Sample_Size_Label,0,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* sample_X_Label = new QLabel("Sample x-position");
		sample_GroupBox_Layout->addWidget(sample_X_Label,1,0,Qt::AlignLeft);

		sample_X_SpinBox = new MyDoubleSpinBox;
			sample_X_SpinBox->setAccelerated(true);
			sample_X_SpinBox->setRange(-100, MAX_DOUBLE);
			sample_X_SpinBox->setDecimals(2);
			sample_X_SpinBox->setSingleStep(0.1);
			sample_X_SpinBox->setValue(target_Curve->measurement.sample_Geometry.x_Position);
			sample_X_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			sample_X_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		sample_GroupBox_Layout->addWidget(sample_X_SpinBox,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(sample_X_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Sample_X_Label = new QLabel("mm");
		sample_GroupBox_Layout->addWidget(mm_Sample_X_Label,1,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* sample_Z_Label = new QLabel("Sample z-position");
		sample_GroupBox_Layout->addWidget(sample_Z_Label,2,0,Qt::AlignLeft);

		sample_Z_SpinBox = new MyDoubleSpinBox;
			sample_Z_SpinBox->setAccelerated(true);
			sample_Z_SpinBox->setRange(-100, MAX_DOUBLE);
			sample_Z_SpinBox->setDecimals(3);
			sample_Z_SpinBox->setValue(target_Curve->measurement.sample_Geometry.z_Position);
			sample_Z_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			sample_Z_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			sample_Z_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		sample_GroupBox_Layout->addWidget(sample_Z_SpinBox,2,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(sample_Z_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Sample_Z_Label = new QLabel("mm");
		sample_GroupBox_Layout->addWidget(mm_Sample_Z_Label,2,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* sample_Curvature_Label = new QLabel("Sample curvature");
		sample_GroupBox_Layout->addWidget(sample_Curvature_Label,3,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sample_Curvature_SpinBox = new MyDoubleSpinBox;
			sample_Curvature_SpinBox->setAccelerated(true);
			sample_Curvature_SpinBox->setRange(-1000, MAX_DOUBLE);
			sample_Curvature_SpinBox->setDecimals(4);
			sample_Curvature_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			sample_Curvature_SpinBox->setValue(target_Curve->measurement.sample_Geometry.curvature);
			sample_Curvature_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			sample_Curvature_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		sample_GroupBox_Layout->addWidget(sample_Curvature_SpinBox,3,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(sample_Curvature_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* m_Sample_Curvature_Label = new QLabel("m"+Minus_One_Sym);
		sample_GroupBox_Layout->addWidget(m_Sample_Curvature_Label,3,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -
		R_Curvature_Label = new QLabel;
		refresh_Curvature_Radius();
		sample_GroupBox_Layout->addWidget(R_Curvature_Label,4,0,1,3,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -


		create_Sample_Plot();
		footprint_GroupBox_Layout->addWidget(sample_Profile_CustomPlot,0);
	}
}

void Specular_Target_Curve_Part::create_Beam_Plot()
{
	beam_Profile_CustomPlot = new QCustomPlot;
		beam_Profile_CustomPlot->setMinimumWidth(240);

	beam_Profile_CustomPlot->setNoAntialiasingOnDrag(false);
	beam_Profile_CustomPlot->clearGraphs();
	beam_Profile_CustomPlot->addGraph();

	// frame, axes
	QPen pen = beam_Profile_CustomPlot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
	beam_Profile_CustomPlot->yAxis->grid()->setSubGridVisible(true);
	beam_Profile_CustomPlot->xAxis->grid()->setSubGridVisible(true);
	beam_Profile_CustomPlot->yAxis->grid()->setPen(pen);
	beam_Profile_CustomPlot->xAxis->grid()->setPen(pen);

	// curve
	beam_Profile_CustomPlot->graph()->setPen(QPen(QColor(0, 135, 0),2));

	// make top right axes clones of bottom left axes:
	beam_Profile_CustomPlot->xAxis2->setVisible(true);
	beam_Profile_CustomPlot->yAxis2->setVisible(true);
	beam_Profile_CustomPlot->xAxis2->setTickLabels(false);

	// ticks
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		linTicker->setTickCount(4);
	beam_Profile_CustomPlot->xAxis->setTicker(linTicker);

	// make range draggable and zoomable:
	beam_Profile_CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	// connect signals so top and right axes move in sync with bottom and left axes:
	connect(beam_Profile_CustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), beam_Profile_CustomPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(beam_Profile_CustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), beam_Profile_CustomPlot->yAxis2, SLOT(setRange(QCPRange)));

	plot_Beam_Profile();
}

void Specular_Target_Curve_Part::plot_Beam_Profile()
{
	int data_Count = 300;
	double FWHM = target_Curve->measurement.beam_Geometry.size;
	double sigma = FWHM*target_Curve->measurement.beam_Geometry.smoothing;
	double limit = 1.7*FWHM;
	double delta = (2*limit)/(data_Count-1);

	QVector<QCPGraphData> data_To_Plot(data_Count);

	for (int i=0; i<data_Count; ++i)
	{
		double x = -limit + delta*i;
		data_To_Plot[i].key   = x;
		data_To_Plot[i].value = Global_Variables::beam_Profile(x,FWHM,sigma);
	}
	beam_Profile_CustomPlot->graph()->data()->set(data_To_Plot);

	beam_Profile_CustomPlot->xAxis->setRange(-limit,limit);
	beam_Profile_CustomPlot->yAxis->setRange(0,1.05);
	beam_Profile_CustomPlot->replot();
}

void Specular_Target_Curve_Part::create_Sample_Plot()
{
	sample_Profile_CustomPlot = new QCustomPlot;
		sample_Profile_CustomPlot->setMinimumWidth(240);

	sample_Profile_CustomPlot->setNoAntialiasingOnDrag(false);
	sample_Profile_CustomPlot->clearGraphs();
	sample_Profile_CustomPlot->addGraph();

	// frame, axes
	QPen pen = sample_Profile_CustomPlot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
	sample_Profile_CustomPlot->yAxis->grid()->setSubGridVisible(true);
	sample_Profile_CustomPlot->xAxis->grid()->setSubGridVisible(true);
	sample_Profile_CustomPlot->yAxis->grid()->setPen(pen);
	sample_Profile_CustomPlot->xAxis->grid()->setPen(pen);

	// make top right axes clones of bottom left axes:
	sample_Profile_CustomPlot->xAxis2->setVisible(true);
	sample_Profile_CustomPlot->yAxis2->setVisible(true);
	sample_Profile_CustomPlot->xAxis2->setTickLabels(false);

	// ticks
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		linTicker->setTickCount(6);
//		linTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
		linTicker->setTickStepStrategy(QCPAxisTicker::tssReadability);
	sample_Profile_CustomPlot->yAxis->setTicker(linTicker);
	sample_Profile_CustomPlot->yAxis2->setTicker(linTicker);

	// make range draggable and zoomable:
	sample_Profile_CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	// connect signals so top and right axes move in sync with bottom and left axes:
	connect(sample_Profile_CustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), sample_Profile_CustomPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(sample_Profile_CustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), sample_Profile_CustomPlot->yAxis2, SLOT(setRange(QCPRange)));

	connect(sample_Profile_CustomPlot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Specular_Target_Curve_Part::auto_Replot_Curve);
	connect(sample_Profile_CustomPlot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Specular_Target_Curve_Part::auto_Replot_Curve);

	// create empty curve objects:
	sample_Curve = new QCPCurve(sample_Profile_CustomPlot->xAxis, sample_Profile_CustomPlot->yAxis);
	sample_Curve->setPen(QPen(QColor(0, 0, 255),1));
	sample_Curve->setBrush(QBrush(QColor(15, 200, 255, 200)));

	// position of sample center
	center_Line = new QCPItemStraightLine(sample_Profile_CustomPlot);

	// add the arrow:
	arrow_Incident  = new QCPItemLine(sample_Profile_CustomPlot);
	arrow_Reflected = new QCPItemLine(sample_Profile_CustomPlot);
	arrow_Incident ->setHead(QCPLineEnding::esSpikeArrow);
	arrow_Reflected->setHead(QCPLineEnding::esSpikeArrow);
	arrow_Incident ->setPen(QPen(QColor(255, 150, 10),1.5));
	arrow_Reflected->setPen(QPen(QColor(255, 150, 10),1.5));

	plot_Sample();
}

void Specular_Target_Curve_Part::plot_Sample()
{
	int curve_Point_Count = 300;

	double size = target_Curve->measurement.sample_Geometry.size;
	double z_Pos = target_Curve->measurement.sample_Geometry.z_Position;
	double x_Pos = target_Curve->measurement.sample_Geometry.x_Position;
	double limit = 0.7*size;
	double sag = 0;
	double delta = size/(curve_Point_Count-1);

	curve_Data.resize(curve_Point_Count);
	curve_Data.first() = QCPCurveData(0, -size/2 + x_Pos, sample_Profile_CustomPlot->yAxis->range().lower);
	if(abs(target_Curve->measurement.sample_Geometry.curvature)>CURVATURE_LIMIT)
	{
		double R = 1000./target_Curve->measurement.sample_Geometry.curvature;
		for (int i=1; i<curve_Data.size()-1; ++i)
		{
			double x = -size/2+(i-1)*delta + x_Pos;
			double y = 0;
			if(R>0)	{y = R-sqrt(R*R-pow(x-x_Pos,2))+z_Pos; sag = max(sag, R-sqrt(R*R-pow(x-x_Pos,2)));}
			if(R<0) {y = R+sqrt(R*R-pow(x-x_Pos,2))+z_Pos; sag = min(sag, R+sqrt(R*R-pow(x-x_Pos,2)));}
			curve_Data[i] = QCPCurveData(i, x, y);
		}
	} else
	{
		for (int i=1; i<curve_Data.size()-1; ++i)
		{
			double x = -size/2+(i-1)*delta + x_Pos;
			double y = z_Pos;
			curve_Data[i] = QCPCurveData(i, x, y);
		}
	}

	curve_Data.last() = QCPCurveData(curve_Data.size()-1, size/2 + x_Pos, sample_Profile_CustomPlot->yAxis->range().lower);

	sample_Curve->data()->set(curve_Data, true);

	// mark center of sample
	center_Line->point1->setCoords(x_Pos, 0);
	center_Line->point2->setCoords(x_Pos, 1);

	sample_Profile_CustomPlot->xAxis->setRange(-limit+x_Pos,limit+x_Pos);
	if(sag>=0) sample_Profile_CustomPlot->yAxis->setRange(-0.019+z_Pos-1.3*sag,0.019+z_Pos+1.3*sag);
	if(sag< 0) sample_Profile_CustomPlot->yAxis->setRange(-0.019+z_Pos+1.3*sag,0.019+z_Pos-1.3*sag);

	sample_Profile_CustomPlot->replot();
}

void Specular_Target_Curve_Part::auto_Replot_Curve()
{
	double size = target_Curve->measurement.sample_Geometry.size;
	double x_Pos = target_Curve->measurement.sample_Geometry.x_Position;
	double z_Pos = target_Curve->measurement.sample_Geometry.z_Position;

	curve_Data.first() = QCPCurveData(0,                  -size/2 + x_Pos, sample_Profile_CustomPlot->yAxis->range().lower);
	curve_Data.last()  = QCPCurveData(curve_Data.size()-1, size/2 + x_Pos, sample_Profile_CustomPlot->yAxis->range().lower);
	sample_Curve->data()->set(curve_Data, true);

	// show arrows
	double factor = (sample_Profile_CustomPlot->yAxis->range().upper-sample_Profile_CustomPlot->yAxis->range().lower) /
					(sample_Profile_CustomPlot->xAxis->range().upper-sample_Profile_CustomPlot->xAxis->range().lower);

	double limit = 0.5*size;
	arrow_Incident->start->setCoords(-limit, limit*factor+z_Pos*0.8);
	arrow_Incident->end->setCoords(-size/10, size/10*factor+z_Pos*0.8);

	arrow_Reflected->start->setCoords(size/10, size/10*factor+z_Pos*0.8);
	arrow_Reflected->end->setCoords(limit, limit*factor+z_Pos*0.8);

	sample_Profile_CustomPlot->replot();
}

void Specular_Target_Curve_Part::reset_Subinterval()
{
	if(target_Curve->loaded_And_Ready)
	{
		double min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
		double max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
		horizontal_From_Subinterval_SpinBox->setRange(min,max);
		horizontal_To_Subinterval_SpinBox->setRange(min,max);

		horizontal_From_Subinterval_SpinBox->setValue(min);
		horizontal_To_Subinterval_SpinBox->setValue(max);
	} else
	{
		horizontal_From_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
		horizontal_To_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);

		horizontal_From_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Left);
		horizontal_To_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Right);
	}
}


void Specular_Target_Curve_Part::fill_Argument_Units()
{
	arg_Units_ComboBox->blockSignals(true);

	if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		arg_Units_ComboBox->clear();
		arg_Units_ComboBox->addItems(angle_Units_List);
		arg_Units_ComboBox->setCurrentText(target_Curve->curve.angular_Units);
	}
	if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		arg_Units_ComboBox->clear();
		arg_Units_ComboBox->addItems(wavelength_Units_List);
		arg_Units_ComboBox->setCurrentText(target_Curve->curve.spectral_Units);
	}
	arg_Units_ComboBox->blockSignals(false);
}

void Specular_Target_Curve_Part::refresh_Argument_Units()
{
	angular_Divergence_SpinBox->blockSignals(true);
	crystal_Resolution_SpinBox->blockSignals(true);

	if(arg_Type_ComboBox->currentText() == argument_Types[Beam_Grazing_Angle])
	{
		target_Curve->curve.angular_Units = arg_Units_ComboBox->currentText();

		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
		angular_Divergence_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/coeff);
		angular_Divergence_Units_Label->setText(target_Curve->curve.angular_Units);

		crystal_Resolution_SpinBox->setValue(target_Curve->measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution/coeff);
		crystal_Resolution_Units_Label->setText(target_Curve->curve.angular_Units);
	}
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy])
	{
		target_Curve->curve.spectral_Units = arg_Units_ComboBox->currentText();
	}
	target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
	target_Curve->refresh_Description_Label();
	refresh_Plot_Axes_Labels();

	angular_Divergence_SpinBox->blockSignals(false);
	crystal_Resolution_SpinBox->blockSignals(false);
}

void Specular_Target_Curve_Part::fill_At_Fixed_Label()
{
	if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		if(	target_Curve->curve.spectral_Units == wavelength_Units_List[angstrom] ||
			target_Curve->curve.spectral_Units == wavelength_Units_List[nm]		  )
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
}

void Specular_Target_Curve_Part::fill_At_Fixed_Units()
{
	at_Fixed_Units_ComboBox->blockSignals(true);

	if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		at_Fixed_Units_ComboBox->clear();
		at_Fixed_Units_ComboBox->addItems(wavelength_Units_List);
		at_Fixed_Units_ComboBox->setCurrentText(target_Curve->curve.spectral_Units);
	}
	if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		at_Fixed_Units_ComboBox->clear();
		at_Fixed_Units_ComboBox->addItems(angle_Units_List);
		at_Fixed_Units_ComboBox->setCurrentText(target_Curve->curve.angular_Units);
	}
	at_Fixed_Units_ComboBox->blockSignals(false);
}

void Specular_Target_Curve_Part::refresh_At_Fixed_Units()
{
	at_Fixed_SpinBox->blockSignals(true);
	angular_Divergence_SpinBox->blockSignals(true);
	crystal_Resolution_SpinBox->blockSignals(true);

	if( arg_Type_ComboBox->currentText() == argument_Types[Beam_Grazing_Angle] )
	{
		target_Curve->curve.spectral_Units = at_Fixed_Units_ComboBox->currentText();

		double coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);
		at_Fixed_SpinBox->setValue(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.wavelength.value)/coeff);
	}
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy] )
	{
		target_Curve->curve.angular_Units = at_Fixed_Units_ComboBox->currentText();

		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
		at_Fixed_SpinBox->setRange(0,90./coeff);
		at_Fixed_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Angle.value/coeff);

		angular_Divergence_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/coeff);
		angular_Divergence_Units_Label->setText(target_Curve->curve.angular_Units);

		crystal_Resolution_SpinBox->setValue(target_Curve->measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution/coeff);
		crystal_Resolution_Units_Label->setText(target_Curve->curve.angular_Units);
	}
	target_Curve->refresh_Description_Label();

	at_Fixed_SpinBox->blockSignals(false);
	angular_Divergence_SpinBox->blockSignals(false);
	crystal_Resolution_SpinBox->blockSignals(false);
}

void Specular_Target_Curve_Part::fill_At_Fixed_Value()
{
	at_Fixed_SpinBox->blockSignals(true);

	if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		double coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);
		at_Fixed_SpinBox->setRange(0,MAX_DOUBLE);
		at_Fixed_SpinBox->setValue(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.wavelength.value)/coeff);
	}
	if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
		at_Fixed_SpinBox->setRange(0,90./coeff);
		at_Fixed_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Angle.value/coeff);
	}
	at_Fixed_SpinBox->blockSignals(false);
}

void Specular_Target_Curve_Part::refresh_At_Fixed_Value()
{
	if(arg_Type_ComboBox->currentText() == argument_Types[Beam_Grazing_Angle])
	{
		double coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);
		target_Curve->measurement.wavelength.value = Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units, at_Fixed_SpinBox->value()*coeff);
	}
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy])
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
		target_Curve->measurement.beam_Theta_0_Angle.value = at_Fixed_SpinBox->value()*coeff;
	}
	target_Curve->refresh_Description_Label();
	target_Curve->refresh_Description_Label();
}

void Specular_Target_Curve_Part::refresh_Value_Type()
{
	target_Curve->curve.value_Type = value_Type_ComboBox->currentText();
	target_Curve->refresh_Description_Label();
	refresh_Plot_Axes_Labels();
}

void Specular_Target_Curve_Part::refresh_Curvature_Radius()
{
	QString R_Text;
	if(abs(target_Curve->measurement.sample_Geometry.curvature)>CURVATURE_LIMIT)
	{
		R_Text = QString::number(1./target_Curve->measurement.sample_Geometry.curvature) + " m";
	} else
	{
		R_Text = Infinity_Sym;
	}
	R_Curvature_Label->setText("Radius of curvature = " + R_Text);
}


void Specular_Target_Curve_Part::disable_Crystal_Detector_Type()
{
	detector_Type_ComboBox->blockSignals(true);

	QStandardItemModel* model =	qobject_cast<QStandardItemModel*>(detector_Type_ComboBox->model());
	QStandardItem* item = model->item( detector_Type_ComboBox->findText(detectors[Crystal] )); // crystal item
	if(	target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy] )
	{
		target_Curve->measurement.detector_1D.detector_Type = detectors[Slit];
		item->setEnabled(false);
	} else
	{
		item->setEnabled(true);
	}
	detector_Type_ComboBox->setCurrentText(target_Curve->measurement.detector_1D.detector_Type);
	detectors_Stack->setCurrentIndex(detector_Type_ComboBox->findText(target_Curve->measurement.detector_1D.detector_Type));

	detector_Type_ComboBox->blockSignals(false);
}

void Specular_Target_Curve_Part::refresh_Plot_Axes_Labels()
{
	target_Curve_Plot->value_Type_Text = target_Curve->curve.value_Type;

	if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		target_Curve_Plot->argument_Type_Text = argument_Types[Beam_Grazing_Angle];
		target_Curve_Plot->argument_Sym_Text = "";
		target_Curve_Plot->argument_Units_Text = target_Curve->curve.angular_Units;
	}
	if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		if(	target_Curve->curve.spectral_Units == wavelength_Units_List[angstrom] ||
			target_Curve->curve.spectral_Units == wavelength_Units_List[nm]		  )
		{
			target_Curve_Plot->argument_Type_Text = QString(argument_Types[Wavelength_Energy]).split("/").first();
			target_Curve_Plot->argument_Sym_Text = " "+Lambda_Sym;
			target_Curve_Plot->argument_Units_Text = target_Curve->curve.spectral_Units;
		} else
		{
			target_Curve_Plot->argument_Type_Text = QString(argument_Types[Wavelength_Energy]).split("/").last();
			target_Curve_Plot->argument_Sym_Text = " E";
			target_Curve_Plot->argument_Units_Text = target_Curve->curve.spectral_Units;
		}
	}
	target_Curve_Plot->refresh_Labels();
}

void Specular_Target_Curve_Part::connecting()
{
	/// argument box
	// argument type
	connect(arg_Type_ComboBox,	&QComboBox::currentTextChanged, this, [=]
	{
		target_Curve->measurement.argument_Type = arg_Type_ComboBox->currentText();

		fill_Argument_Units();
		fill_At_Fixed_Label();
		fill_At_Fixed_Value();
		fill_At_Fixed_Units();

		refresh_Argument_Units();
		refresh_At_Fixed_Units();

		disable_Crystal_Detector_Type();
		refresh_Plot_Axes_Labels();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve->refresh_Description_Label();

		// maximize_Integral
		if(arg_Type_ComboBox->currentText() != argument_Types[Beam_Grazing_Angle])
		{
			if(target_Curve->fit_Params.maximize_Integral)
			{
				target_Curve->fit_Params.maximize_Integral = false;
				if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
				{
					Calculation_Settings_Editor* calculation_Settings_Editor = global_Multilayer_Approach->runned_Calculation_Settings_Editor.value(calc_Settings_Key);
					QCheckBox* maximize_Integral_Checkbox = calculation_Settings_Editor->max_Integral_Map.value(target_Curve);
					maximize_Integral_Checkbox->toggled(false);
				}
			}
		}
		global_Multilayer_Approach->calc_Reflection(true);
	});
	// argument units
	connect(arg_Units_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		refresh_Argument_Units();
		Global_Variables::replot_All_Graphs();
	});
	// argument shift
	connect(arg_Shift_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.horizontal_Arg_Shift = arg_Shift_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);
		target_Curve->refresh_Description_Label();
		global_Multilayer_Approach->calc_Reflection(true);
	});
	// argument factor
	connect(arg_Factor_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.horizontal_Arg_Factor= arg_Factor_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);
		target_Curve->refresh_Description_Label();
		global_Multilayer_Approach->calc_Reflection(true);
	});
	// subinterval
	connect(main_Subinterval_Checkbox, &QCheckBox::toggled, this, [=]
	{
		target_Curve->curve.use_Subinterval = main_Subinterval_Checkbox->isChecked();

		horizontal_From_Subinterval_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
		horizontal_And_Subinterval_Label   ->setEnabled(target_Curve->curve.use_Subinterval);
		horizontal_To_Subinterval_SpinBox  ->setEnabled(target_Curve->curve.use_Subinterval);

		target_Curve_Plot->subinterval_Changed_Replot();
		Global_Variables::replot_All_Graphs();
	});
	connect(horizontal_From_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.subinterval_Left = horizontal_From_Subinterval_SpinBox->value();

		horizontal_To_Subinterval_SpinBox->setRange(target_Curve->curve.subinterval_Left,horizontal_To_Subinterval_SpinBox->maximum());
		horizontal_To_Subinterval_SpinBox->setValue(max(target_Curve->curve.subinterval_Left,target_Curve->curve.subinterval_Right));

		target_Curve_Plot->subinterval_Changed_Replot();
		Global_Variables::replot_All_Graphs();
	});
	connect(horizontal_To_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.subinterval_Right = horizontal_To_Subinterval_SpinBox->value();

		horizontal_From_Subinterval_SpinBox->setRange(horizontal_From_Subinterval_SpinBox->minimum(),target_Curve->curve.subinterval_Right);
		horizontal_From_Subinterval_SpinBox->setValue(min(target_Curve->curve.subinterval_Left,target_Curve->curve.subinterval_Right));

		target_Curve_Plot->subinterval_Changed_Replot();
		Global_Variables::replot_All_Graphs();
	});

	/// value box
	// value type
	connect(value_Type_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		refresh_Value_Type();

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// value shift
	connect(val_Shift_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.val_Shift = val_Shift_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);
		target_Curve->refresh_Description_Label();
		Global_Variables::replot_All_Graphs();
	});
	// value factor
	connect(val_Factor_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.val_Factor.value = val_Factor_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);
		target_Curve->refresh_Description_Label();
		Global_Variables::replot_All_Graphs();
	});
	connect(val_Factor_Min_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.val_Factor.fit.min = val_Factor_Min_SpinBox->value();
	});
	connect(val_Factor_Max_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
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
		target_Curve_Plot->plot_Data(true);
		Global_Variables::replot_All_Graphs();
	});
	connect(beam_Intensity_Initial_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.beam_Intensity_Initial= beam_Intensity_Initial_SpinBox->value();
		if(!target_Curve->curve.use_Final_Intensity)
		{
			target_Curve->curve.beam_Intensity_Final = target_Curve->curve.beam_Intensity_Initial;
		}

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);
		Global_Variables::replot_All_Graphs();
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
		target_Curve_Plot->plot_Data(true);
		Global_Variables::replot_All_Graphs();
	});
	connect(beam_Intensity_Final_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.beam_Intensity_Final= beam_Intensity_Final_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);

		Global_Variables::replot_All_Graphs();
	});

	/// beam box
	// at fixed
	connect(at_Fixed_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_At_Fixed_Value();

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// at fixed units
	connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		refresh_At_Fixed_Units();

		Global_Variables::replot_All_Graphs();
	});
	// polarization
	connect(polarization_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.polarization= polarization_SpinBox->value();

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// background
	connect(background_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.background= background_SpinBox->value();

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// spectral width
	connect(spectral_Width_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.spectral_Distribution.FWHM_distribution = spectral_Width_SpinBox->value();

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// angular divergence
	connect(angular_Divergence_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
		target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution = angular_Divergence_SpinBox->value()*coeff;

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// beam distribution button
	connect(setup_Beam_Distribution_Button, &QPushButton::clicked, this, [=]
	{
		// TODO
		Distribution_Editor* distribution_Editor = new Distribution_Editor;
		distribution_Editor->show();
	});

	/// detector box
	// detector type
	connect(detector_Type_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		target_Curve->measurement.detector_1D.detector_Type = detector_Type_ComboBox->currentText();
		detectors_Stack->setCurrentIndex(detector_Type_ComboBox->findText(target_Curve->measurement.detector_1D.detector_Type));

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// slit width
	connect(slit_Width_SpinBox,  static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.detector_1D.slit_Width = slit_Width_SpinBox->value();

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// slit distance
	connect(slit_Distance_SpinBox,  static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.detector_1D.distance_To_Sample = slit_Distance_SpinBox->value();

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// crystal resolution
	connect(crystal_Resolution_SpinBox,  static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
		target_Curve->measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution = crystal_Resolution_SpinBox->value()*coeff;

		global_Multilayer_Approach->calc_Reflection(true);
	});
	// crystal resolution button
	connect(setup_Crystal_Resolution_Button, &QPushButton::clicked, this, [=]
	{
		// TODO
		Distribution_Editor* distribution_Editor = new Distribution_Editor;
		distribution_Editor->show();
	});

	/// footprint & distortion groupbox
	// beam width
	connect(beam_Footprint_Width_SpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.beam_Geometry.size = beam_Footprint_Width_SpinBox->value();

		plot_Beam_Profile();
		global_Multilayer_Approach->calc_Reflection(true);
	});
	// beam shape
	connect(beam_Footprint_Shape_SpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.beam_Geometry.smoothing = beam_Footprint_Shape_SpinBox->value();

		plot_Beam_Profile();
		global_Multilayer_Approach->calc_Reflection(true);
	});
	// sample size
	connect(sample_Size_SpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.sample_Geometry.size = sample_Size_SpinBox->value();

		plot_Sample();
		global_Multilayer_Approach->calc_Reflection(true);
	});
	// sample x-position
	connect(sample_X_SpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.sample_Geometry.x_Position = sample_X_SpinBox->value();

		plot_Sample();
		global_Multilayer_Approach->calc_Reflection(true);
	});
	// sample z-position
	connect(sample_Z_SpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.sample_Geometry.z_Position = sample_Z_SpinBox->value();

		plot_Sample();
		global_Multilayer_Approach->calc_Reflection(true);
	});
	// sample curvature
	connect(sample_Curvature_SpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.sample_Geometry.curvature = sample_Curvature_SpinBox->value();

		refresh_Curvature_Radius();
		plot_Sample();
		global_Multilayer_Approach->calc_Reflection(true);
	});
}
