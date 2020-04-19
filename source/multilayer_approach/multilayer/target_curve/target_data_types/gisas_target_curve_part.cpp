#include "gisas_target_curve_part.h"

GISAS_Target_Curve_Part::GISAS_Target_Curve_Part(Target_Curve *target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent) :
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
	refresh_Plot_Axes_Labels();;
	target_Curve->refresh_Description_Label();
}

void GISAS_Target_Curve_Part::create_Argument_GroupBox()
{
	// initialization
	if(target_Curve->measurement.argument_Type == no_Argument_Type)
	{
		target_Curve->curve.subinterval_Left  = target_Curve->measurement.detector_Theta_Angle.independent.min;
		target_Curve->curve.subinterval_Right = target_Curve->measurement.detector_Theta_Angle.independent.max;

		target_Curve->curve.subinterval_Bottom = target_Curve->measurement.detector_Phi_Angle.independent.min;
		target_Curve->curve.subinterval_Top    = target_Curve->measurement.detector_Phi_Angle.independent.max;

		target_Curve->measurement.argument_Type = argument_Types[Detector_Theta_Phi_Angles];
	}

	QGroupBox* argument_GroupBox = new QGroupBox("Argument");
	main_Layout->addWidget(argument_GroupBox);

	QGridLayout* argument_GroupBox_Layout = new QGridLayout(argument_GroupBox);
		argument_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);

	// argument type theta
	{
		arg_Type_Theta_ComboBox = new QComboBox;
			arg_Type_Theta_ComboBox->addItem(argument_Types[Detector_Polar_Angle]);
			arg_Type_Theta_ComboBox->setCurrentText(argument_Types[Detector_Polar_Angle]);
		argument_GroupBox_Layout->addWidget(arg_Type_Theta_ComboBox,0,0);
	}
	// argument type phi
	{
		arg_Type_Phi_ComboBox = new QComboBox;
			arg_Type_Phi_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			arg_Type_Phi_ComboBox->addItem(argument_Types[Detector_Azimuthal_Angle]);
			arg_Type_Phi_ComboBox->setCurrentText(argument_Types[Detector_Azimuthal_Angle]);
		argument_GroupBox_Layout->addWidget(arg_Type_Phi_ComboBox,1,0,Qt::AlignLeft);
	}
	// argument units
	{
		QLabel* argument_Units_Label = new QLabel("  Units");
		argument_GroupBox_Layout->addWidget(argument_Units_Label,0,1,2,1,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		arg_Units_ComboBox = new QComboBox;
			arg_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			arg_Units_ComboBox->addItems(angle_Units_List);
			arg_Units_ComboBox->setCurrentText(target_Curve->angular_Units);
		argument_GroupBox_Layout->addWidget(arg_Units_ComboBox,0,2,2,1,Qt::AlignLeft);
	}
	// argument theta
	{
		QLabel* theta_From_Label = new QLabel(" from ");
		argument_GroupBox_Layout->addWidget(theta_From_Label,0,3,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		theta_Min_SpinBox = new MyDoubleSpinBox;
			theta_Min_SpinBox->setAccelerated(true);
			theta_Min_SpinBox->setRange(0, 180./coeff);
			theta_Min_SpinBox->setDecimals(4);
			theta_Min_SpinBox->setValue(target_Curve->measurement.detector_Theta_Angle.independent.min/coeff);
			theta_Min_SpinBox->setSingleStep(0.001);
			theta_Min_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			theta_Min_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(theta_Min_SpinBox,0,4,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(theta_Min_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* theta_To_Label = new QLabel(" to ");
		argument_GroupBox_Layout->addWidget(theta_To_Label,0,5,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		theta_Max_SpinBox = new MyDoubleSpinBox;
			theta_Max_SpinBox->setAccelerated(true);
			theta_Max_SpinBox->setRange(0, 180./coeff);
			theta_Max_SpinBox->setDecimals(4);
			theta_Max_SpinBox->setValue(target_Curve->measurement.detector_Theta_Angle.independent.max/coeff);
			theta_Max_SpinBox->setSingleStep(0.001);
			theta_Max_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			theta_Max_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(theta_Max_SpinBox,0,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(theta_Max_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		arg_Theta_Units_Label = new QLabel(target_Curve->angular_Units);
		argument_GroupBox_Layout->addWidget(arg_Theta_Units_Label,0,7,Qt::AlignLeft);
	}
	// argument phi
	{
		QLabel* phi_From_Label = new QLabel(" from ");
		argument_GroupBox_Layout->addWidget(phi_From_Label,1,3,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		phi_Min_SpinBox = new MyDoubleSpinBox;
			phi_Min_SpinBox->setAccelerated(true);
			phi_Min_SpinBox->setRange(-180./coeff, 180./coeff);
			phi_Min_SpinBox->setDecimals(4);
			phi_Min_SpinBox->setValue(target_Curve->measurement.detector_Phi_Angle.independent.min/coeff);
			phi_Min_SpinBox->setSingleStep(0.001);
			phi_Min_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			phi_Min_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(phi_Min_SpinBox,1,4,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(phi_Min_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* phi_To_Label = new QLabel(" to ");
		argument_GroupBox_Layout->addWidget(phi_To_Label,1,5,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		phi_Max_SpinBox = new MyDoubleSpinBox;
			phi_Max_SpinBox->setAccelerated(true);
			phi_Max_SpinBox->setRange(-180./coeff, 180./coeff);
			phi_Max_SpinBox->setDecimals(4);
			phi_Max_SpinBox->setValue(target_Curve->measurement.detector_Phi_Angle.independent.max/coeff);
			phi_Max_SpinBox->setSingleStep(0.001);
			phi_Max_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			phi_Max_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(phi_Max_SpinBox,1,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(phi_Max_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		arg_Phi_Units_Label = new QLabel(target_Curve->angular_Units);
		argument_GroupBox_Layout->addWidget(arg_Phi_Units_Label,1,7,Qt::AlignLeft);
	}
	// specifying interval for fitting
	{
		main_Subinterval_Checkbox = new QCheckBox("Use only data between argument");
			main_Subinterval_Checkbox->setCheckable(true);
			main_Subinterval_Checkbox->setChecked(target_Curve->curve.use_Subinterval);
		argument_GroupBox_Layout->addWidget(main_Subinterval_Checkbox,0,8,2,1,Qt::AlignLeft);

		// theta
		{
			from_Subinterval_Theta_SpinBox = new MyDoubleSpinBox;
				from_Subinterval_Theta_SpinBox->setAccelerated(true);
				from_Subinterval_Theta_SpinBox->setDecimals(4);
				from_Subinterval_Theta_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				from_Subinterval_Theta_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
				from_Subinterval_Theta_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
			argument_GroupBox_Layout->addWidget(from_Subinterval_Theta_SpinBox,0,9,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(from_Subinterval_Theta_SpinBox);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			and_Subinterval_Theta_Label = new QLabel("  and  ");
				and_Subinterval_Theta_Label->setEnabled(target_Curve->curve.use_Subinterval);
			argument_GroupBox_Layout->addWidget(and_Subinterval_Theta_Label,0,10,Qt::AlignLeft);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			to_Subinterval_Theta_SpinBox = new MyDoubleSpinBox;
				to_Subinterval_Theta_SpinBox->setAccelerated(true);
				to_Subinterval_Theta_SpinBox->setDecimals(4);
				to_Subinterval_Theta_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				to_Subinterval_Theta_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
				to_Subinterval_Theta_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
			argument_GroupBox_Layout->addWidget(to_Subinterval_Theta_SpinBox,0,11,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(to_Subinterval_Theta_SpinBox);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			subinterval_Theta_Units_Label = new QLabel(target_Curve->angular_Units);
				subinterval_Theta_Units_Label->setEnabled(target_Curve->curve.use_Subinterval);
			argument_GroupBox_Layout->addWidget(subinterval_Theta_Units_Label,0,12,Qt::AlignLeft);
		}
		// phi
		{
			from_Subinterval_Phi_SpinBox = new MyDoubleSpinBox;
				from_Subinterval_Phi_SpinBox->setAccelerated(true);
				from_Subinterval_Phi_SpinBox->setDecimals(4);
				from_Subinterval_Phi_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				from_Subinterval_Phi_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
				from_Subinterval_Phi_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
			argument_GroupBox_Layout->addWidget(from_Subinterval_Phi_SpinBox,1,9,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(from_Subinterval_Phi_SpinBox);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			and_Subinterval_Phi_Label = new QLabel("  and  ");
				and_Subinterval_Phi_Label->setEnabled(target_Curve->curve.use_Subinterval);
			argument_GroupBox_Layout->addWidget(and_Subinterval_Phi_Label,1,10,Qt::AlignLeft);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			to_Subinterval_Phi_SpinBox = new MyDoubleSpinBox;
				to_Subinterval_Phi_SpinBox->setAccelerated(true);
				to_Subinterval_Phi_SpinBox->setDecimals(4);
				to_Subinterval_Phi_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				to_Subinterval_Phi_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
				to_Subinterval_Phi_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
			argument_GroupBox_Layout->addWidget(to_Subinterval_Phi_SpinBox,1,11,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(to_Subinterval_Phi_SpinBox);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

			subinterval_Phi_Units_Label = new QLabel(target_Curve->angular_Units);
				subinterval_Phi_Units_Label->setEnabled(target_Curve->curve.use_Subinterval);
			argument_GroupBox_Layout->addWidget(subinterval_Phi_Units_Label,1,12,Qt::AlignLeft);
		}
		reset_Subinterval();
	}
}

void GISAS_Target_Curve_Part::create_Value_GroupBox()
{
	// initialization
	if(target_Curve->curve.value_Type == no_Value_Type)
	{
		target_Curve->curve.value_Type = value_Types[GISAS];
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
		norm_On_Beam_Intensity = new QCheckBox("Divide on beam intensity");
			norm_On_Beam_Intensity->setChecked(target_Curve->curve.divide_On_Beam_Intensity);
		value_GroupBox_Layout->addWidget(norm_On_Beam_Intensity);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Intensity_SpinBox = new MyDoubleSpinBox;
			beam_Intensity_SpinBox->setAccelerated(true);
			beam_Intensity_SpinBox->setRange(1, MAX_DOUBLE);
			beam_Intensity_SpinBox->setDecimals(0);
			beam_Intensity_SpinBox->setValue(target_Curve->curve.beam_Intensity_Initial);
			beam_Intensity_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Intensity_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Intensity_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
			beam_Intensity_SpinBox->setEnabled(target_Curve->curve.divide_On_Beam_Intensity);
		value_GroupBox_Layout->addWidget(beam_Intensity_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Intensity_SpinBox);
	}
}

void GISAS_Target_Curve_Part::create_Beam_GroupBox()
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
			spectral_Width_SpinBox->setRange(0, MAX_DOUBLE);
			spectral_Width_SpinBox->setDecimals(7);
			spectral_Width_SpinBox->setValue(target_Curve->measurement.spectral_Distribution.FWHM_distribution);
			spectral_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			spectral_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			spectral_Width_SpinBox->setProperty(min_Size_Property, TARGET_LINE_RESOLUTION_WIDTH);
		beam_GroupBox_Layout->addWidget(spectral_Width_SpinBox,0,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(spectral_Width_SpinBox);
	}
	// theta_0 angular divergence
	{
		QLabel* angular_Divergence_Label = new QLabel("      Angular divergence, FWHM, " + Delta_Big_Sym + Theta_Sym + Zero_Subscript_Sym);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Label,1,5,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double arg_Coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);

		angular_Divergence_Theta_0_SpinBox = new MyDoubleSpinBox;
			angular_Divergence_Theta_0_SpinBox->setAccelerated(true);
			angular_Divergence_Theta_0_SpinBox->setRange(0, MAX_DOUBLE);
			angular_Divergence_Theta_0_SpinBox->setDecimals(7);
			angular_Divergence_Theta_0_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/arg_Coeff);
			angular_Divergence_Theta_0_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			angular_Divergence_Theta_0_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			angular_Divergence_Theta_0_SpinBox->setProperty(min_Size_Property, TARGET_LINE_RESOLUTION_WIDTH);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Theta_0_SpinBox,1,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(angular_Divergence_Theta_0_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		angular_Divergence_Theta_0_Units_Label = new QLabel(target_Curve->angular_Units);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Theta_0_Units_Label,1,7,Qt::AlignLeft);
	}
	// phi_0 angular divergence
	{
		QLabel* angular_Divergence_Label = new QLabel("      Angular divergence, FWHM, " + Delta_Big_Sym + Theta_Sym + Zero_Subscript_Sym);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Label,2,5,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double arg_Coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);

		angular_Divergence_Phi_0_SpinBox = new MyDoubleSpinBox;
			angular_Divergence_Phi_0_SpinBox->setAccelerated(true);
			angular_Divergence_Phi_0_SpinBox->setRange(0, MAX_DOUBLE);
			angular_Divergence_Phi_0_SpinBox->setDecimals(7);
			angular_Divergence_Phi_0_SpinBox->setValue(target_Curve->measurement.beam_Phi_0_Distribution.FWHM_distribution/arg_Coeff);
			angular_Divergence_Phi_0_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			angular_Divergence_Phi_0_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			angular_Divergence_Phi_0_SpinBox->setProperty(min_Size_Property, TARGET_LINE_RESOLUTION_WIDTH);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Phi_0_SpinBox,2,6,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(angular_Divergence_Phi_0_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		angular_Divergence_Phi_0_Units_Label = new QLabel(target_Curve->angular_Units);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Phi_0_Units_Label,2,7,Qt::AlignLeft);
	}
	// set up distribution
	{
		QLabel* spacing_Label = new QLabel("         ");
		beam_GroupBox_Layout->addWidget(spacing_Label,1,8,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		setup_Beam_Distribution_Button = new QPushButton("Set up distribution");
		beam_GroupBox_Layout->addWidget(setup_Beam_Distribution_Button,1,9,Qt::AlignCenter);
	}
}

void GISAS_Target_Curve_Part::reset_Subinterval()
{
	from_Subinterval_Theta_SpinBox->blockSignals(true);
	to_Subinterval_Theta_SpinBox->blockSignals(true);
	from_Subinterval_Phi_SpinBox->blockSignals(true);
	to_Subinterval_Phi_SpinBox->blockSignals(true);

	//----------------------------------------------------------------

	double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);

	// theta
	double min_Theta = min(target_Curve->measurement.detector_Theta_Angle.independent.min, target_Curve->measurement.detector_Theta_Angle.independent.max);
	double max_Theta = max(target_Curve->measurement.detector_Theta_Angle.independent.min, target_Curve->measurement.detector_Theta_Angle.independent.max);

	target_Curve->curve.subinterval_Left  = max(target_Curve->curve.subinterval_Left,min_Theta);
	target_Curve->curve.subinterval_Left  = min(target_Curve->curve.subinterval_Left,max_Theta);
	target_Curve->curve.subinterval_Right = max(target_Curve->curve.subinterval_Right,min_Theta);
	target_Curve->curve.subinterval_Right = min(target_Curve->curve.subinterval_Right,max_Theta);
	target_Curve->curve.subinterval_Right = max(target_Curve->curve.subinterval_Left,target_Curve->curve.subinterval_Right);

	from_Subinterval_Theta_SpinBox->setRange(min_Theta/coeff, target_Curve->curve.subinterval_Right/coeff);
	to_Subinterval_Theta_SpinBox  ->setRange(target_Curve->curve.subinterval_Left/coeff, max_Theta/coeff);

	from_Subinterval_Theta_SpinBox->setValue(target_Curve->curve.subinterval_Left/coeff);
	to_Subinterval_Theta_SpinBox  ->setValue(target_Curve->curve.subinterval_Right/coeff);

	from_Subinterval_Theta_SpinBox->setSingleStep((max_Theta-min_Theta)/100./coeff);
	to_Subinterval_Theta_SpinBox  ->setSingleStep((max_Theta-min_Theta)/100./coeff);

	// phi
	double min_Phi = min(target_Curve->measurement.detector_Phi_Angle.independent.min, target_Curve->measurement.detector_Phi_Angle.independent.max);
	double max_Phi = max(target_Curve->measurement.detector_Phi_Angle.independent.min, target_Curve->measurement.detector_Phi_Angle.independent.max);

	target_Curve->curve.subinterval_Bottom = max(target_Curve->curve.subinterval_Bottom,min_Phi);
	target_Curve->curve.subinterval_Bottom = min(target_Curve->curve.subinterval_Bottom,max_Phi);
	target_Curve->curve.subinterval_Top = max(target_Curve->curve.subinterval_Top,min_Phi);
	target_Curve->curve.subinterval_Top = min(target_Curve->curve.subinterval_Top,max_Phi);
	target_Curve->curve.subinterval_Top = max(target_Curve->curve.subinterval_Bottom,target_Curve->curve.subinterval_Top);

	from_Subinterval_Phi_SpinBox->setRange(min_Phi/coeff,    target_Curve->curve.subinterval_Top/coeff);
	to_Subinterval_Phi_SpinBox  ->setRange(target_Curve->curve.subinterval_Bottom/coeff, max_Phi/coeff);

	from_Subinterval_Phi_SpinBox->setValue(target_Curve->curve.subinterval_Bottom/coeff);
	to_Subinterval_Phi_SpinBox  ->setValue(target_Curve->curve.subinterval_Top/coeff);

	from_Subinterval_Phi_SpinBox->setSingleStep((max_Phi-min_Phi)/100./coeff);
	to_Subinterval_Phi_SpinBox  ->setSingleStep((max_Phi-min_Phi)/100./coeff);

	//----------------------------------------------------------------

	from_Subinterval_Theta_SpinBox->blockSignals(false);
	to_Subinterval_Theta_SpinBox->blockSignals(false);
	from_Subinterval_Phi_SpinBox->blockSignals(false);
	to_Subinterval_Phi_SpinBox->blockSignals(false);
}

void GISAS_Target_Curve_Part::refresh_Argument_Units()
{
	theta_Min_SpinBox->blockSignals(true);
	theta_Max_SpinBox->blockSignals(true);
	phi_Min_SpinBox->blockSignals(true);
	phi_Max_SpinBox->blockSignals(true);
	at_Fixed_Beam_Theta_0_SpinBox->blockSignals(true);
	angular_Divergence_Theta_0_SpinBox->blockSignals(true);
	angular_Divergence_Phi_0_SpinBox->blockSignals(true);
	target_Common_Part->theta_Resolution_SpinBox->blockSignals(true);
	target_Common_Part->phi_Resolution_SpinBox->blockSignals(true);

	//-----------------------------------------------------------------------------------
	target_Curve->angular_Units = arg_Units_ComboBox->currentText();
	double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);

	// theta
	theta_Min_SpinBox->setRange(0,180./coeff);
	theta_Min_SpinBox->setValue(target_Curve->measurement.detector_Theta_Angle.independent.min/coeff);
	theta_Max_SpinBox->setRange(0,180./coeff);
	theta_Max_SpinBox->setValue(target_Curve->measurement.detector_Theta_Angle.independent.max/coeff);
	arg_Theta_Units_Label->setText(target_Curve->angular_Units);
	subinterval_Theta_Units_Label->setText(target_Curve->angular_Units);

	// phi
	phi_Min_SpinBox->setRange(-180./coeff,180./coeff);
	phi_Min_SpinBox->setValue(target_Curve->measurement.detector_Phi_Angle.independent.min/coeff);
	phi_Max_SpinBox->setRange(-180./coeff,180./coeff);
	phi_Max_SpinBox->setValue(target_Curve->measurement.detector_Phi_Angle.independent.max/coeff);
	arg_Phi_Units_Label->setText(target_Curve->angular_Units);
	subinterval_Phi_Units_Label->setText(target_Curve->angular_Units);

	// at fixed beam angle
	at_Fixed_Beam_Theta_0_SpinBox->setRange(0,90./coeff);
	at_Fixed_Beam_Theta_0_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Angle.value/coeff);
	at_Fixed_Beam_Theta_0_Units_Label->setText(target_Curve->angular_Units);

	// angular divergence
	angular_Divergence_Theta_0_SpinBox->setValue(target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/coeff);
	angular_Divergence_Theta_0_Units_Label->setText(target_Curve->angular_Units);
	angular_Divergence_Phi_0_SpinBox->setValue(target_Curve->measurement.beam_Phi_0_Distribution.FWHM_distribution/coeff);
	angular_Divergence_Phi_0_Units_Label->setText(target_Curve->angular_Units);

	// detector resolution
	target_Common_Part->theta_Resolution_SpinBox->setValue(target_Curve->measurement.detector_2D.detector_Theta_Resolution.FWHM_distribution/coeff);
	target_Common_Part->theta_Resolution_Units_Label->setText(target_Curve->angular_Units);
	target_Common_Part->phi_Resolution_SpinBox->setValue(target_Curve->measurement.detector_2D.detector_Phi_Resolution.FWHM_distribution/coeff);
	target_Common_Part->phi_Resolution_Units_Label->setText(target_Curve->angular_Units);

	reset_Subinterval();

	target_Curve->refresh_Description_Label();
	refresh_Plot_Axes_Labels();

	//-----------------------------------------------------------------------------------
	theta_Min_SpinBox->blockSignals(true);
	theta_Max_SpinBox->blockSignals(true);
	phi_Min_SpinBox->blockSignals(true);
	phi_Max_SpinBox->blockSignals(true);
	at_Fixed_Beam_Theta_0_SpinBox->blockSignals(true);
	angular_Divergence_Theta_0_SpinBox->blockSignals(true);
	angular_Divergence_Phi_0_SpinBox->blockSignals(true);
	target_Common_Part->theta_Resolution_SpinBox->blockSignals(true);
	target_Common_Part->phi_Resolution_SpinBox->blockSignals(true);
}

void GISAS_Target_Curve_Part::fill_At_Fixed_Wavelength_Label()
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

void GISAS_Target_Curve_Part::fill_Spectral_Width_Label()
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

void GISAS_Target_Curve_Part::refresh_At_Fixed_Wavelength_Units()
{
	at_Fixed_Wavelength_SpinBox->blockSignals(true);

	target_Curve->spectral_Units = at_Fixed_Wavelength_Units_ComboBox->currentText();

	double coeff = wavelength_Coefficients_Map.value(target_Curve->spectral_Units);
	at_Fixed_Wavelength_SpinBox->setValue(Global_Variables::wavelength_Energy(target_Curve->spectral_Units,target_Curve->measurement.wavelength.value)/coeff);

	fill_Spectral_Width_Label();
	fill_At_Fixed_Wavelength_Label();

	target_Curve->refresh_Description_Label();
	at_Fixed_Wavelength_SpinBox->blockSignals(false);
}

void GISAS_Target_Curve_Part::refresh_At_Fixed_Beam_Theta_0_Value()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
	target_Curve->measurement.beam_Theta_0_Angle.value = at_Fixed_Beam_Theta_0_SpinBox->value()*coeff;

	target_Curve->refresh_Description_Label();
}

void GISAS_Target_Curve_Part::refresh_At_Fixed_Wavelength_Value()
{
	double coeff = wavelength_Coefficients_Map.value(target_Curve->spectral_Units);
	target_Curve->measurement.wavelength.value = Global_Variables::wavelength_Energy(target_Curve->spectral_Units, at_Fixed_Wavelength_SpinBox->value()*coeff);
	target_Curve->refresh_Description_Label();
}

void GISAS_Target_Curve_Part::refresh_Plot_Axes_Labels()
{
//	target_Curve_Plot->value_Type_Text = target_Curve->curve.value_Type;
//	target_Curve_Plot->argument_Type_Text = argument_Types[Detector_Polar_Angle];
//	target_Curve_Plot->argument_Sym_Text = "";
//	target_Curve_Plot->argument_Units_Text = target_Curve->angular_Units;
//	target_Curve_Plot->refresh_Labels();
}

void GISAS_Target_Curve_Part::connecting()
{
	/// argument box
	// argument units
	connect(arg_Units_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		refresh_Argument_Units();
		global_Multilayer_Approach->calculate(true);
	});
	// theta
	connect(theta_Min_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->measurement.detector_Theta_Angle.independent.min = theta_Min_SpinBox->value()*coeff;

		reset_Subinterval();
		target_Curve->refresh_Description_Label();
		global_Multilayer_Approach->calculate(true);
	});
	connect(theta_Max_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->measurement.detector_Theta_Angle.independent.max = theta_Max_SpinBox->value()*coeff;

		reset_Subinterval();
		target_Curve->refresh_Description_Label();
		global_Multilayer_Approach->calculate(true);
	});
	// phi
	connect(phi_Min_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->measurement.detector_Phi_Angle.independent.min = phi_Min_SpinBox->value()*coeff;

		reset_Subinterval();
		target_Curve->refresh_Description_Label();
		global_Multilayer_Approach->calculate(true);
	});
	connect(phi_Max_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->measurement.detector_Phi_Angle.independent.max = phi_Max_SpinBox->value()*coeff;

		reset_Subinterval();
		target_Curve->refresh_Description_Label();
		global_Multilayer_Approach->calculate(true);
	});
	// subinterval
	connect(main_Subinterval_Checkbox, &QCheckBox::toggled, this, [=]
	{
		target_Curve->curve.use_Subinterval = main_Subinterval_Checkbox->isChecked();

		from_Subinterval_Theta_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
		and_Subinterval_Theta_Label   ->setEnabled(target_Curve->curve.use_Subinterval);
		to_Subinterval_Theta_SpinBox  ->setEnabled(target_Curve->curve.use_Subinterval);
		subinterval_Theta_Units_Label ->setEnabled(target_Curve->curve.use_Subinterval);

		from_Subinterval_Phi_SpinBox->setEnabled(target_Curve->curve.use_Subinterval);
		and_Subinterval_Phi_Label   ->setEnabled(target_Curve->curve.use_Subinterval);
		to_Subinterval_Phi_SpinBox  ->setEnabled(target_Curve->curve.use_Subinterval);
		subinterval_Phi_Units_Label ->setEnabled(target_Curve->curve.use_Subinterval);

		target_Curve_Plot->subinterval_Changed_Replot();
		Global_Variables::replot_All_Graphs();
	});
	// theta subinterval
	connect(from_Subinterval_Theta_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->curve.subinterval_Left = from_Subinterval_Theta_SpinBox->value()*coeff;

		to_Subinterval_Theta_SpinBox->setRange(target_Curve->curve.subinterval_Left/coeff,to_Subinterval_Theta_SpinBox->maximum());
		to_Subinterval_Theta_SpinBox->setValue(max(target_Curve->curve.subinterval_Left,target_Curve->curve.subinterval_Right)/coeff);

		target_Curve_Plot->subinterval_Changed_Replot();
		Global_Variables::replot_All_Graphs();
	});
	connect(to_Subinterval_Theta_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->curve.subinterval_Right = to_Subinterval_Theta_SpinBox->value()*coeff;

		from_Subinterval_Theta_SpinBox->setRange(from_Subinterval_Theta_SpinBox->minimum(),target_Curve->curve.subinterval_Right/coeff);
		from_Subinterval_Theta_SpinBox->setValue(min(target_Curve->curve.subinterval_Left,target_Curve->curve.subinterval_Right)/coeff);

		target_Curve_Plot->subinterval_Changed_Replot();
		Global_Variables::replot_All_Graphs();
	});
	// phi subinterval
	connect(from_Subinterval_Phi_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->curve.subinterval_Bottom = from_Subinterval_Phi_SpinBox->value()*coeff;

		to_Subinterval_Phi_SpinBox->setRange(target_Curve->curve.subinterval_Bottom,to_Subinterval_Phi_SpinBox->maximum());
		to_Subinterval_Phi_SpinBox->setValue(max(target_Curve->curve.subinterval_Bottom,target_Curve->curve.subinterval_Top));

		target_Curve_Plot->subinterval_Changed_Replot();
		Global_Variables::replot_All_Graphs();
	});
	connect(to_Subinterval_Phi_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->curve.subinterval_Top = to_Subinterval_Phi_SpinBox->value()*coeff;

		from_Subinterval_Phi_SpinBox->setRange(from_Subinterval_Phi_SpinBox->minimum(),target_Curve->curve.subinterval_Top);
		from_Subinterval_Phi_SpinBox->setValue(min(target_Curve->curve.subinterval_Bottom,target_Curve->curve.subinterval_Top));

		target_Curve_Plot->subinterval_Changed_Replot();
		Global_Variables::replot_All_Graphs();
	});

	/// value box
	// value shift
	connect(val_Shift_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.val_Shift = val_Shift_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);
		target_Curve->refresh_Description_Label();
		Global_Variables::plot_All_Data_in_Graphs();
	});
	// value factor
	connect(val_Factor_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.val_Factor.value = val_Factor_SpinBox->value();

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);
		target_Curve->refresh_Description_Label();
		Global_Variables::plot_All_Data_in_Graphs();
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

		beam_Intensity_SpinBox->setEnabled(target_Curve->curve.divide_On_Beam_Intensity);

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);
		Global_Variables::plot_All_Data_in_Graphs();
	});
	connect(beam_Intensity_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->curve.beam_Intensity_Initial= beam_Intensity_SpinBox->value();
		target_Curve->curve.beam_Intensity_Final = target_Curve->curve.beam_Intensity_Initial;

		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve_Plot->plot_Data(true);
		Global_Variables::plot_All_Data_in_Graphs();
	});

	/// beam box
	// at fixed theta_0
	connect(at_Fixed_Beam_Theta_0_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_At_Fixed_Beam_Theta_0_Value();

		global_Multilayer_Approach->calculate(true);
	});
	// at fixed wavelength
	connect(at_Fixed_Wavelength_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_At_Fixed_Wavelength_Value();

		global_Multilayer_Approach->calculate(true);
	});
	// at fixed units
	connect(at_Fixed_Wavelength_Units_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		refresh_At_Fixed_Wavelength_Units();

		Global_Variables::plot_All_Data_in_Graphs();
	});
	// polarization
	connect(polarization_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.polarization = polarization_SpinBox->value();

		global_Multilayer_Approach->calculate(true);
	});
	// background
	connect(background_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.background = background_SpinBox->value();

		global_Multilayer_Approach->calculate(true);
	});
	// spectral width
	connect(spectral_Width_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		target_Curve->measurement.spectral_Distribution.FWHM_distribution = spectral_Width_SpinBox->value();

		global_Multilayer_Approach->calculate(true);
	});
	// angular divergence
	connect(angular_Divergence_Theta_0_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution = angular_Divergence_Theta_0_SpinBox->value()*coeff;

		global_Multilayer_Approach->calculate(true);
	});
	connect(angular_Divergence_Phi_0_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
		target_Curve->measurement.beam_Phi_0_Distribution.FWHM_distribution = angular_Divergence_Phi_0_SpinBox->value()*coeff;

		global_Multilayer_Approach->calculate(true);
	});
	// beam distribution button
	connect(setup_Beam_Distribution_Button, &QPushButton::clicked, this, [=]
	{
		Distribution_Editor* distribution_Editor = new Distribution_Editor(target_Curve->measurement,
																		   target_Curve->spectral_Units,
																		   target_Curve->angular_Units,
																		   spectral_Width_SpinBox,
																		   angular_Divergence_Theta_0_SpinBox,
																		   angular_Divergence_Phi_0_SpinBox);
		distribution_Editor->show();
	});
}

