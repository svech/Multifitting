#include "specular_independent_curve_part.h"

Specular_Independent_Curve_Part::Specular_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget* parent) :
	independent_Curve(independent_Curve),
	QWidget(parent)
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setContentsMargins(4,4,4,0);
		main_Layout->setSpacing(0);

	create_Units_GroupBox();
	create_Argument_GroupBox();
	create_Beam_GroupBox();
	create_Detector_GroupBox();
	create_Footptint_GroupBox();

	connecting();
	independent_Curve->refresh_Description_Label();
}

void Specular_Independent_Curve_Part::create_Units_GroupBox()
{
	QGroupBox* units_GroupBox = new QGroupBox("Units");
	main_Layout->addWidget(units_GroupBox);

	QHBoxLayout* units_GroupBox_Layout = new QHBoxLayout(units_GroupBox);
		units_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// angular units
	{
		QLabel* angular_Units_Label = new QLabel("Angular units");
		units_GroupBox_Layout->addWidget(angular_Units_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		angular_Units_ComboBox = new QComboBox;
			angular_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			angular_Units_ComboBox->addItems(angle_Units_List);
			angular_Units_ComboBox->setCurrentText(independent_Curve->angular_Units);
		units_GroupBox_Layout->addWidget(angular_Units_ComboBox,0,Qt::AlignLeft);
	}
	// spectral units
	{
		QLabel* spectral_Units_Label = new QLabel("          Spectral units");
		units_GroupBox_Layout->addWidget(spectral_Units_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		spectral_Units_ComboBox = new QComboBox;
			spectral_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			spectral_Units_ComboBox->addItems(wavelength_Units_List);
			spectral_Units_ComboBox->setCurrentText(independent_Curve->spectral_Units);
		units_GroupBox_Layout->addWidget(spectral_Units_ComboBox,0,Qt::AlignLeft);
	}
}

void Specular_Independent_Curve_Part::create_Argument_GroupBox()
{
	// initialization
	if(independent_Curve->measurement.argument_Type == no_Argument_Type)
	{
		independent_Curve->measurement.argument_Type = argument_Types[Beam_Grazing_Angle];
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
			arg_Type_ComboBox->setCurrentText(independent_Curve->measurement.argument_Type);
		argument_GroupBox_Layout->addWidget(arg_Type_ComboBox,0,Qt::AlignLeft);
	}
	// argument points
	{
		num_Points_Spinbox = new QSpinBox;
			num_Points_Spinbox->setAccelerated(true);
			num_Points_Spinbox->setRange(3, MAX_INTEGER);
			num_Points_Spinbox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
			num_Points_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			num_Points_Spinbox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
		argument_GroupBox_Layout->addWidget(num_Points_Spinbox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(num_Points_Spinbox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* from_Label = new QLabel(" points, from ");
		argument_GroupBox_Layout->addWidget(from_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		start_Argument_Spinbox = new MyDoubleSpinBox;
			start_Argument_Spinbox->setAccelerated(true);
			start_Argument_Spinbox->setRange(0, MAX_DOUBLE);
			start_Argument_Spinbox->setDecimals(4);
			start_Argument_Spinbox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			start_Argument_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			start_Argument_Spinbox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(start_Argument_Spinbox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(start_Argument_Spinbox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* to_Label = new QLabel(" to ");
		argument_GroupBox_Layout->addWidget(to_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		final_Argument_Spinbox = new MyDoubleSpinBox;
			final_Argument_Spinbox->setAccelerated(true);
			final_Argument_Spinbox->setRange(0, MAX_DOUBLE);
			final_Argument_Spinbox->setDecimals(4);
			final_Argument_Spinbox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			final_Argument_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			final_Argument_Spinbox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(final_Argument_Spinbox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(final_Argument_Spinbox);

		fill_Argument_Values();
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		argument_Units_Label = new QLabel;
		fill_Argument_Units();
		argument_GroupBox_Layout->addWidget(argument_Units_Label,0,Qt::AlignLeft);
	}
}

void Specular_Independent_Curve_Part::create_Beam_GroupBox()
{
	QGroupBox* beam_GroupBox = new QGroupBox("Beam");
	main_Layout->addWidget(beam_GroupBox);

	QGridLayout* beam_GroupBox_Layout = new QGridLayout(beam_GroupBox);
		beam_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// at fixed
	{
		at_Fixed_Label = new QLabel;
			fill_At_Fixed_Label();
		beam_GroupBox_Layout->addWidget(at_Fixed_Label,0,0,1,1,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_SpinBox = new MyDoubleSpinBox;
			at_Fixed_SpinBox->setAccelerated(true);
			at_Fixed_SpinBox->setDecimals(7);
			at_Fixed_SpinBox->setSingleStep(0.01);
			fill_At_Fixed_Value();
			at_Fixed_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			at_Fixed_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(at_Fixed_SpinBox,0,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(at_Fixed_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_Units_Label = new QLabel;
		fill_At_Fixed_Units();
		beam_GroupBox_Layout->addWidget(at_Fixed_Units_Label,0,2,Qt::AlignLeft);
	}
	// polarization
	{
		QLabel* polarization_Label = new QLabel("Polarization");
		beam_GroupBox_Layout->addWidget(polarization_Label,1,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		polarization_SpinBox = new MyDoubleSpinBox;
			polarization_SpinBox->setAccelerated(true);
			polarization_SpinBox->setRange(-1, 1);
			polarization_SpinBox->setDecimals(3);
			polarization_SpinBox->setValue(independent_Curve->measurement.polarization);
			polarization_SpinBox->setSingleStep(0.01);
			polarization_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			polarization_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(polarization_SpinBox,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(polarization_SpinBox);
	}
	// background
	{
		QLabel* background_Label = new QLabel("Background");
		beam_GroupBox_Layout->addWidget(background_Label,2,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		background_SpinBox = new MyDoubleSpinBox;
			background_SpinBox->setAccelerated(true);
			background_SpinBox->setRange(0, MAX_DOUBLE);
			background_SpinBox->setDecimals(10);
			background_SpinBox->setValue(independent_Curve->measurement.background);
			background_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			background_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			background_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(background_SpinBox,2,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(background_SpinBox);
	}
	// spectral distribution
	{
		spectral_Width_Label = new QLabel;
		if(	independent_Curve->spectral_Units == wavelength_Units_List[angstrom] ||
			independent_Curve->spectral_Units == wavelength_Units_List[nm]	   )
		{
			spectral_Width_Label->setText("  Spectral width, FWHM, " + Delta_Big_Sym + Lambda_Sym+"/"+Lambda_Sym);
		} else
		{
			spectral_Width_Label->setText("  Spectral width, FWHM, " + Delta_Big_Sym + "E/E");
		}
		beam_GroupBox_Layout->addWidget(spectral_Width_Label,0,4,Qt::AlignRight);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		spectral_Width_SpinBox = new MyDoubleSpinBox;
			spectral_Width_SpinBox->setAccelerated(true);
			spectral_Width_SpinBox->setRange(0, MAX_DOUBLE);
			spectral_Width_SpinBox->setDecimals(7);
			spectral_Width_SpinBox->setValue(independent_Curve->measurement.spectral_Distribution.FWHM_distribution);
			spectral_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			spectral_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			spectral_Width_SpinBox->setProperty(min_Size_Property, TARGET_LINE_RESOLUTION_WIDTH);
		beam_GroupBox_Layout->addWidget(spectral_Width_SpinBox,0,5,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(spectral_Width_SpinBox);
	}
	// angular divergence
	{
		QLabel* angular_Divergence_Label = new QLabel("  Angular divergence, FWHM, " + Delta_Big_Sym + Theta_Sym + Zero_Subscript_Sym);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Label,1,4,Qt::AlignRight);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double arg_Coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);

		angular_Divergence_SpinBox = new MyDoubleSpinBox;
			angular_Divergence_SpinBox->setAccelerated(true);
			angular_Divergence_SpinBox->setRange(0, MAX_DOUBLE);
			angular_Divergence_SpinBox->setDecimals(7);
			angular_Divergence_SpinBox->setValue(independent_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/arg_Coeff);
			angular_Divergence_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			angular_Divergence_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			angular_Divergence_SpinBox->setProperty(min_Size_Property, TARGET_LINE_RESOLUTION_WIDTH);
		beam_GroupBox_Layout->addWidget(angular_Divergence_SpinBox,1,5,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(angular_Divergence_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		angular_Divergence_Units_Label = new QLabel(independent_Curve->angular_Units);
		beam_GroupBox_Layout->addWidget(angular_Divergence_Units_Label,1,6,Qt::AlignLeft);
	}
	// set up distribution
	{
		setup_Beam_Distribution_Button = new QPushButton("Set up distribution");
		beam_GroupBox_Layout->addWidget(setup_Beam_Distribution_Button,2,4,1,2,Qt::AlignCenter);
	}
}

void Specular_Independent_Curve_Part::create_Detector_GroupBox()
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
			slit_Width_SpinBox->setValue(independent_Curve->measurement.detector_1D.slit_Width);
			slit_Width_SpinBox->setSingleStep(0.01);
			slit_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			slit_Width_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		slit_Layout->addWidget(slit_Width_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(slit_Width_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Width_Label = new QLabel("mm");
		slit_Layout->addWidget(mm_Width_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* spacing_Label = new QLabel("       ");
		slit_Layout->addWidget(spacing_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* slit_Distance_Label = new QLabel("Distance from detector to sample");
		slit_Layout->addWidget(slit_Distance_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		slit_Distance_SpinBox = new MyDoubleSpinBox;
			slit_Distance_SpinBox->setAccelerated(true);
			slit_Distance_SpinBox->setRange(0, MAX_DOUBLE);
			slit_Distance_SpinBox->setDecimals(1);
			slit_Distance_SpinBox->setValue(independent_Curve->measurement.detector_1D.distance_To_Sample);
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

		double arg_Coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);

		crystal_Resolution_SpinBox = new MyDoubleSpinBox;
			crystal_Resolution_SpinBox->setAccelerated(true);
			crystal_Resolution_SpinBox->setRange(0, MAX_DOUBLE);
			crystal_Resolution_SpinBox->setDecimals(6);
			crystal_Resolution_SpinBox->setValue(independent_Curve->measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution/arg_Coeff);
			crystal_Resolution_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			crystal_Resolution_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			crystal_Resolution_SpinBox->setProperty(min_Size_Property,TARGET_LINE_AT_FIXED_WIDTH);
		crystal_Layout->addWidget(crystal_Resolution_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(crystal_Resolution_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		crystal_Resolution_Units_Label = new QLabel(independent_Curve->angular_Units);
		crystal_Layout->addWidget(crystal_Resolution_Units_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* crystal_Resolution_Function_Label = new QLabel("       Function");
		crystal_Layout->addWidget(crystal_Resolution_Function_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		crystal_Resolution_Function_ComboBox = new QComboBox;
			crystal_Resolution_Function_ComboBox->addItems(distributions);
			crystal_Resolution_Function_ComboBox->setCurrentText(independent_Curve->measurement.detector_1D.detector_Theta_Resolution.distribution_Function);
			crystal_Resolution_Function_ComboBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
		crystal_Layout->addWidget(crystal_Resolution_Function_ComboBox,0,Qt::AlignLeft);
	}

	disable_Crystal_Detector_Type();
}

void Specular_Independent_Curve_Part::create_Footptint_GroupBox()
{
	QGroupBox* footprint_GroupBox = new QGroupBox("Footptint and distortion");
	main_Layout->addWidget(footprint_GroupBox);

	QGridLayout* footprint_GroupBox_Layout = new QGridLayout(footprint_GroupBox);
		footprint_GroupBox_Layout->setAlignment(Qt::AlignLeft);
		footprint_GroupBox_Layout->setSpacing(0);

	// beam
	{
		QGroupBox* beam_Footprint_GroupBox = new QGroupBox("Beam");
		footprint_GroupBox_Layout->addWidget(beam_Footprint_GroupBox,0,0,Qt::AlignLeft);

		QGridLayout* beam_Footprint_GroupBox_Layout = new QGridLayout(beam_Footprint_GroupBox);
			beam_Footprint_GroupBox_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* beam_Footprint_Width_Label = new QLabel("Beam width, FWHM");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Width_Label,0,0,Qt::AlignLeft);

		beam_Footprint_Width_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Width_SpinBox->setAccelerated(true);
			beam_Footprint_Width_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Footprint_Width_SpinBox->setDecimals(3);
			beam_Footprint_Width_SpinBox->setValue(independent_Curve->measurement.beam_Geometry.size);
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
			beam_Footprint_Shape_SpinBox->setValue(independent_Curve->measurement.beam_Geometry.smoothing);
			beam_Footprint_Shape_SpinBox->setSingleStep(0.01);
			beam_Footprint_Shape_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Shape_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Shape_SpinBox,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Shape_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		create_Beam_Plot();
		footprint_GroupBox_Layout->addWidget(beam_Profile_CustomPlot,1,0);
	}
	// sample
	{
		QGroupBox* sample_Footprint_GroupBox = new QGroupBox("Sample");
		footprint_GroupBox_Layout->addWidget(sample_Footprint_GroupBox,0,1/*,Qt::AlignLeft*/);

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
			sample_Size_SpinBox->setValue(independent_Curve->measurement.sample_Geometry.size);
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
			sample_X_SpinBox->setValue(independent_Curve->measurement.sample_Geometry.x_Position);
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
			sample_Z_SpinBox->setValue(independent_Curve->measurement.sample_Geometry.z_Position);
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
			sample_Curvature_SpinBox->setValue(independent_Curve->measurement.sample_Geometry.curvature);
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
		footprint_GroupBox_Layout->addWidget(sample_Profile_CustomPlot,1,1);
	}
}


void Specular_Independent_Curve_Part::create_Beam_Plot()
{
	beam_Profile_CustomPlot = new QCustomPlot;
		beam_Profile_CustomPlot->setMinimumSize(270,190);

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

void Specular_Independent_Curve_Part::plot_Beam_Profile()
{
	int data_Count = 301;
	double FWHM = independent_Curve->measurement.beam_Geometry.size;
	double sigma = FWHM*independent_Curve->measurement.beam_Geometry.smoothing;
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

void Specular_Independent_Curve_Part::create_Sample_Plot()
{
	sample_Profile_CustomPlot = new QCustomPlot;
		sample_Profile_CustomPlot->setMinimumSize(270,190);

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

	connect(sample_Profile_CustomPlot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Specular_Independent_Curve_Part::auto_Replot_Curve);
	connect(sample_Profile_CustomPlot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Specular_Independent_Curve_Part::auto_Replot_Curve);

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

void Specular_Independent_Curve_Part::plot_Sample()
{
	int curve_Point_Count = 203;

	double size = independent_Curve->measurement.sample_Geometry.size;
	double z_Pos = independent_Curve->measurement.sample_Geometry.z_Position;
	double x_Pos = independent_Curve->measurement.sample_Geometry.x_Position;
	double limit = 0.7*size;
	double sag = 0;
	double delta = size/(curve_Point_Count-3);

	curve_Data.resize(curve_Point_Count);
	curve_Data.first() = QCPCurveData(0, -size/2 + x_Pos, sample_Profile_CustomPlot->yAxis->range().lower);
	if(abs(independent_Curve->measurement.sample_Geometry.curvature)>CURVATURE_LIMIT)
	{
		double R = 1000./independent_Curve->measurement.sample_Geometry.curvature;
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

void Specular_Independent_Curve_Part::auto_Replot_Curve()
{
	double size = independent_Curve->measurement.sample_Geometry.size;
	double x_Pos = independent_Curve->measurement.sample_Geometry.x_Position;
	double z_Pos = independent_Curve->measurement.sample_Geometry.z_Position;

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


void Specular_Independent_Curve_Part::fill_Argument_Units()
{
	if(independent_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		argument_Units_Label->setText(independent_Curve->angular_Units);
	}
	if(independent_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		argument_Units_Label->setText(independent_Curve->spectral_Units);
	}
}

void Specular_Independent_Curve_Part::fill_Argument_Values()
{
	num_Points_Spinbox->blockSignals(true);
	start_Argument_Spinbox->blockSignals(true);
	final_Argument_Spinbox->blockSignals(true);

	if(independent_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		num_Points_Spinbox->setValue(independent_Curve->measurement.beam_Theta_0_Angle.independent.num_Points);

		double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
		start_Argument_Spinbox->setValue(independent_Curve->measurement.beam_Theta_0_Angle.independent.min/coeff);
		final_Argument_Spinbox->setValue(independent_Curve->measurement.beam_Theta_0_Angle.independent.max/coeff);
	}
	if(independent_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		num_Points_Spinbox->setValue(independent_Curve->measurement.wavelength.independent.num_Points);

		double coeff = wavelength_Coefficients_Map.value(independent_Curve->spectral_Units);
		start_Argument_Spinbox->setValue(Global_Variables::wavelength_Energy(independent_Curve->spectral_Units,independent_Curve->measurement.wavelength.independent.min)/coeff);
		final_Argument_Spinbox->setValue(Global_Variables::wavelength_Energy(independent_Curve->spectral_Units,independent_Curve->measurement.wavelength.independent.max)/coeff);
	}

	num_Points_Spinbox->blockSignals(false);
	start_Argument_Spinbox->blockSignals(false);
	final_Argument_Spinbox->blockSignals(false);
}

void Specular_Independent_Curve_Part::refresh_Angular_Units()
{
	start_Argument_Spinbox->blockSignals(true);
	final_Argument_Spinbox->blockSignals(true);
	at_Fixed_SpinBox->blockSignals(true);
	angular_Divergence_SpinBox->blockSignals(true);
	crystal_Resolution_SpinBox->blockSignals(true);

	double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
	if(independent_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		// argument
		start_Argument_Spinbox->setRange(0,90./coeff);
		final_Argument_Spinbox->setRange(0,90./coeff);

		start_Argument_Spinbox->setValue(independent_Curve->measurement.beam_Theta_0_Angle.independent.min/coeff);
		final_Argument_Spinbox->setValue(independent_Curve->measurement.beam_Theta_0_Angle.independent.max/coeff);
		argument_Units_Label->setText(independent_Curve->angular_Units);
	}
	if(independent_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		// at fixed
		at_Fixed_SpinBox->setRange(0,90./coeff);
		at_Fixed_SpinBox->setValue(independent_Curve->measurement.beam_Theta_0_Angle.value/coeff);
		at_Fixed_Units_Label->setText(independent_Curve->angular_Units);
	}

	// beam divergence
	angular_Divergence_SpinBox->setValue(independent_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/coeff);
	angular_Divergence_Units_Label->setText(independent_Curve->angular_Units);

	// crystal resolution
	crystal_Resolution_SpinBox->setValue(independent_Curve->measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution/coeff);
	crystal_Resolution_Units_Label->setText(independent_Curve->angular_Units);

	start_Argument_Spinbox->blockSignals(false);
	final_Argument_Spinbox->blockSignals(false);
	at_Fixed_SpinBox->blockSignals(false);
	angular_Divergence_SpinBox->blockSignals(false);
	crystal_Resolution_SpinBox->blockSignals(false);

	independent_Curve->refresh_Description_Label();
}

void Specular_Independent_Curve_Part::refresh_Spectral_Units()
{
	start_Argument_Spinbox->blockSignals(true);
	final_Argument_Spinbox->blockSignals(true);
	at_Fixed_SpinBox->blockSignals(true);

	double coeff = wavelength_Coefficients_Map.value(independent_Curve->spectral_Units);
	if(independent_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		// at fixed
		at_Fixed_SpinBox->setRange(0,MAX_DOUBLE);

		at_Fixed_SpinBox->setValue(Global_Variables::wavelength_Energy(independent_Curve->spectral_Units,independent_Curve->measurement.wavelength.value)/coeff);
		at_Fixed_Units_Label->setText(independent_Curve->spectral_Units);
	}
	if(independent_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		// argument
		start_Argument_Spinbox->setRange(0,MAX_DOUBLE);
		final_Argument_Spinbox->setRange(0,MAX_DOUBLE);

		start_Argument_Spinbox->setValue(Global_Variables::wavelength_Energy(independent_Curve->spectral_Units,independent_Curve->measurement.wavelength.independent.min)/coeff);
		final_Argument_Spinbox->setValue(Global_Variables::wavelength_Energy(independent_Curve->spectral_Units,independent_Curve->measurement.wavelength.independent.max)/coeff);
		argument_Units_Label->setText(independent_Curve->spectral_Units);
	}

	if(	independent_Curve->spectral_Units == wavelength_Units_List[angstrom] ||
		independent_Curve->spectral_Units == wavelength_Units_List[nm]	   )
	{
		spectral_Width_Label->setText("  Spectral width, FWHM, " + Delta_Big_Sym + Lambda_Sym+"/"+Lambda_Sym);
	} else
	{
		spectral_Width_Label->setText("  Spectral width, FWHM, " + Delta_Big_Sym + "E/E");
	}

	start_Argument_Spinbox->blockSignals(false);
	final_Argument_Spinbox->blockSignals(false);
	at_Fixed_SpinBox->blockSignals(false);

	independent_Curve->refresh_Description_Label();
}

void Specular_Independent_Curve_Part::refresh_Argument_Values()
{
	if(independent_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
		independent_Curve->measurement.beam_Theta_0_Angle.independent.num_Points = num_Points_Spinbox->value();
		independent_Curve->measurement.beam_Theta_0_Angle.independent.min = start_Argument_Spinbox->value()*coeff;
		independent_Curve->measurement.beam_Theta_0_Angle.independent.max = final_Argument_Spinbox->value()*coeff;
	}
	if(independent_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		double coeff = wavelength_Coefficients_Map.value(independent_Curve->spectral_Units);
		independent_Curve->measurement.wavelength.independent.num_Points = num_Points_Spinbox->value();
		independent_Curve->measurement.wavelength.independent.min = Global_Variables::wavelength_Energy(independent_Curve->spectral_Units, start_Argument_Spinbox->value()*coeff);
		independent_Curve->measurement.wavelength.independent.max = Global_Variables::wavelength_Energy(independent_Curve->spectral_Units, final_Argument_Spinbox->value()*coeff);
	}
	independent_Curve->refresh_Description_Label();
}

void Specular_Independent_Curve_Part::refresh_At_Fixed_Value()
{
	if(independent_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		double coeff = wavelength_Coefficients_Map.value(independent_Curve->spectral_Units);
		independent_Curve->measurement.wavelength.value = Global_Variables::wavelength_Energy(independent_Curve->spectral_Units, at_Fixed_SpinBox->value()*coeff);
	}
	if(independent_Curve->measurement.argument_Type  == argument_Types[Wavelength_Energy])
	{
		double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
		independent_Curve->measurement.beam_Theta_0_Angle.value = at_Fixed_SpinBox->value()*coeff;
	}
	independent_Curve->refresh_Description_Label();
}

void Specular_Independent_Curve_Part::refresh_Curvature_Radius()
{
	QString R_Text;
	if(abs(independent_Curve->measurement.sample_Geometry.curvature)>CURVATURE_LIMIT)
	{
		R_Text = QString::number(1./independent_Curve->measurement.sample_Geometry.curvature) + " m";
	} else
	{
		R_Text = Infinity_Sym;
	}
	R_Curvature_Label->setText("Radius of curvature = " + R_Text);
}

void Specular_Independent_Curve_Part::disable_Crystal_Detector_Type()
{
	detector_Type_ComboBox->blockSignals(true);

	QStandardItemModel* model =	qobject_cast<QStandardItemModel*>(detector_Type_ComboBox->model());
	QStandardItem* item = model->item( detector_Type_ComboBox->findText(detectors[Crystal] )); // crystal item
	if(	independent_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy] )
	{
		independent_Curve->measurement.detector_1D.detector_Type = detectors[Slit];
		item->setEnabled(false);
	} else
	{
		item->setEnabled(true);
	}
	detector_Type_ComboBox->setCurrentText(independent_Curve->measurement.detector_1D.detector_Type);
	detectors_Stack->setCurrentIndex(detector_Type_ComboBox->findText(independent_Curve->measurement.detector_1D.detector_Type));

	detector_Type_ComboBox->blockSignals(false);
}

void Specular_Independent_Curve_Part::fill_At_Fixed_Label()
{
	if(independent_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		if(	independent_Curve->spectral_Units == wavelength_Units_List[angstrom] ||
			independent_Curve->spectral_Units == wavelength_Units_List[nm]		  )
		{
			at_Fixed_Label->setText("At fixed wavelength:");
		} else
		{
			at_Fixed_Label->setText("At fixed energy:");
		}
	}
	if(independent_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		at_Fixed_Label->setText("At fixed grazing angle "+Theta_Sym+Zero_Subscript_Sym);
	}
}

void Specular_Independent_Curve_Part::fill_At_Fixed_Units()
{
	if(independent_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		at_Fixed_Units_Label->setText(independent_Curve->spectral_Units);
	}
	if(independent_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		at_Fixed_Units_Label->setText(independent_Curve->angular_Units);
	}
}

void Specular_Independent_Curve_Part::fill_At_Fixed_Value()
{
	at_Fixed_SpinBox->blockSignals(true);

	if(independent_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
	{
		double coeff = wavelength_Coefficients_Map.value(independent_Curve->spectral_Units);
		at_Fixed_SpinBox->setRange(0,MAX_DOUBLE);
		at_Fixed_SpinBox->setValue(Global_Variables::wavelength_Energy(independent_Curve->spectral_Units,independent_Curve->measurement.wavelength.value)/coeff);
	}
	if(independent_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
	{
		double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
		at_Fixed_SpinBox->setRange(0,90./coeff);
		at_Fixed_SpinBox->setValue(independent_Curve->measurement.beam_Theta_0_Angle.value/coeff);
	}
	at_Fixed_SpinBox->blockSignals(false);
}

void Specular_Independent_Curve_Part::connecting()
{
	refresh_Angular_Units();
	refresh_Spectral_Units();

	/// units box
	connect(angular_Units_ComboBox,	&QComboBox::currentTextChanged, this, [=]
	{
		independent_Curve->angular_Units = angular_Units_ComboBox->currentText();

		refresh_Angular_Units();
		Global_Variables::plot_All_Data_in_Graphs();
	});
	connect(spectral_Units_ComboBox,	&QComboBox::currentTextChanged, this, [=]
	{
		independent_Curve->spectral_Units = spectral_Units_ComboBox->currentText();

		refresh_Spectral_Units();
		Global_Variables::plot_All_Data_in_Graphs();
	});

	/// argument box
	// argument type
	connect(arg_Type_ComboBox,	&QComboBox::currentTextChanged, this, [=]
	{
		independent_Curve->measurement.argument_Type = arg_Type_ComboBox->currentText();

		fill_At_Fixed_Label();
		fill_At_Fixed_Value();
		fill_At_Fixed_Units();
		fill_Argument_Values();

		disable_Crystal_Detector_Type();

		independent_Curve->refresh_Description_Label();
		global_Multilayer_Approach->calculate(true);
	});
	// number of points
	connect(num_Points_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
	{
		refresh_Argument_Values();
		global_Multilayer_Approach->calculate(true);
	});
	// start value
	connect(start_Argument_Spinbox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_Argument_Values();
		global_Multilayer_Approach->calculate(true);
	});
	// final value
	connect(final_Argument_Spinbox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_Argument_Values();
		global_Multilayer_Approach->calculate(true);
	});

	/// beam box
	// at fixed value
	connect(at_Fixed_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_At_Fixed_Value();
		global_Multilayer_Approach->calculate(true);
	});
	// polarization
	connect(polarization_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.polarization = polarization_SpinBox->value();

		global_Multilayer_Approach->calculate(true);
	});
	// background
	connect(background_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.background = background_SpinBox->value();

		global_Multilayer_Approach->calculate(true);
	});
	// spectral width
	connect(spectral_Width_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.spectral_Distribution.FWHM_distribution = spectral_Width_SpinBox->value();

		global_Multilayer_Approach->calculate(true);
	});
	// angular divergence
	connect(angular_Divergence_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
		independent_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution = angular_Divergence_SpinBox->value()*coeff;

		global_Multilayer_Approach->calculate(true);
	});
	// beam distribution button
	connect(setup_Beam_Distribution_Button, &QPushButton::clicked, this, [=]
	{
		Distribution_Editor* distribution_Editor = new Distribution_Editor(independent_Curve->measurement,
																		   independent_Curve->spectral_Units,
																		   independent_Curve->angular_Units,
																		   spectral_Width_SpinBox,
																		   angular_Divergence_SpinBox);
		distribution_Editor->show();
	});

	/// detector box
	// detector type
	connect(detector_Type_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		independent_Curve->measurement.detector_1D.detector_Type = detector_Type_ComboBox->currentText();
		detectors_Stack->setCurrentIndex(detector_Type_ComboBox->findText(independent_Curve->measurement.detector_1D.detector_Type));

		global_Multilayer_Approach->calculate(true);
	});
	// slit width
	connect(slit_Width_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.detector_1D.slit_Width = slit_Width_SpinBox->value();

		global_Multilayer_Approach->calculate(true);
	});
	// slit distance
	connect(slit_Distance_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.detector_1D.distance_To_Sample = slit_Distance_SpinBox->value();

		global_Multilayer_Approach->calculate(true);
	});
	// crystal resolution
	connect(crystal_Resolution_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
		independent_Curve->measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution = crystal_Resolution_SpinBox->value()*coeff;

		global_Multilayer_Approach->calculate(true);
	});
	// crystal resolution function
	connect(crystal_Resolution_Function_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		independent_Curve->measurement.detector_1D.detector_Theta_Resolution.distribution_Function = crystal_Resolution_Function_ComboBox->currentText();

		global_Multilayer_Approach->calculate(true);
	});

	/// footprint & distortion groupbox
	// beam width
	connect(beam_Footprint_Width_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.beam_Geometry.size = beam_Footprint_Width_SpinBox->value();

		plot_Beam_Profile();
		global_Multilayer_Approach->calculate(true);
	});
	// beam shape
	connect(beam_Footprint_Shape_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.beam_Geometry.smoothing = beam_Footprint_Shape_SpinBox->value();

		plot_Beam_Profile();
		global_Multilayer_Approach->calculate(true);
	});
	// sample size
	connect(sample_Size_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.sample_Geometry.size = sample_Size_SpinBox->value();

		plot_Sample();
		global_Multilayer_Approach->calculate(true);
	});
	// sample x-position
	connect(sample_X_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.sample_Geometry.x_Position = sample_X_SpinBox->value();

		plot_Sample();
		global_Multilayer_Approach->calculate(true);
	});
	// sample z-position
	connect(sample_Z_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.sample_Geometry.z_Position = sample_Z_SpinBox->value();

		plot_Sample();
		global_Multilayer_Approach->calculate(true);
	});
	// sample curvature
	connect(sample_Curvature_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.sample_Geometry.curvature = sample_Curvature_SpinBox->value();

		refresh_Curvature_Radius();
		plot_Sample();
		global_Multilayer_Approach->calculate(true);
	});
}

