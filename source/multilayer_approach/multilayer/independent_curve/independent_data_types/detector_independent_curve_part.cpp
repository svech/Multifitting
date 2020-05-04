#include "detector_independent_curve_part.h"

Detector_Independent_Curve_Part::Detector_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent) :
	independent_Curve(independent_Curve),
	QWidget(parent)
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setContentsMargins(4,4,4,0);
	main_Layout->setSpacing(0);

	create_Units_GroupBox();
	create_Argument_GroupBox();
	create_Beam_GroupBox();

	Target_Curve* target_Curve;
	independent_Common_Part = new Common_Part(independent_Curve, target_Curve);
	main_Layout->addWidget(independent_Common_Part);

	connecting();
	independent_Curve->refresh_Description_Label();
}

void Detector_Independent_Curve_Part::create_Units_GroupBox()
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

void Detector_Independent_Curve_Part::create_Argument_GroupBox()
{
	// initialization
	if(independent_Curve->measurement.argument_Type == no_Argument_Type)
	{
		independent_Curve->measurement.argument_Type = argument_Types[Detector_Polar_Angle];
	}

	QGroupBox* argument_GroupBox = new QGroupBox("Argument");
	main_Layout->addWidget(argument_GroupBox);

	QHBoxLayout* argument_GroupBox_Layout = new QHBoxLayout(argument_GroupBox);
	argument_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// argument type
	{
		arg_Type_ComboBox = new QComboBox;
			arg_Type_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			arg_Type_ComboBox->addItem(independent_Curve->measurement.argument_Type);
			arg_Type_ComboBox->setCurrentText(independent_Curve->measurement.argument_Type);
		argument_GroupBox_Layout->addWidget(arg_Type_ComboBox,0,Qt::AlignLeft);
	}
	// argument points
	{
		num_Points_Spinbox = new QSpinBox;
			num_Points_Spinbox->setAccelerated(true);
			num_Points_Spinbox->setRange(3, MAX_INTEGER);
			num_Points_Spinbox->setValue(independent_Curve->measurement.detector_Theta_Angle.independent.num_Points);
			num_Points_Spinbox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
			num_Points_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			num_Points_Spinbox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
		argument_GroupBox_Layout->addWidget(num_Points_Spinbox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(num_Points_Spinbox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* from_Label = new QLabel(" points, from ");
		argument_GroupBox_Layout->addWidget(from_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);

		start_Argument_Spinbox = new MyDoubleSpinBox;
			start_Argument_Spinbox->setAccelerated(true);
			start_Argument_Spinbox->setRange(0, 180./coeff);
			start_Argument_Spinbox->setDecimals(4);
			start_Argument_Spinbox->setValue(independent_Curve->measurement.detector_Theta_Angle.independent.min/coeff);
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
			final_Argument_Spinbox->setRange(0, 180./coeff);
			final_Argument_Spinbox->setDecimals(4);
			final_Argument_Spinbox->setValue(independent_Curve->measurement.detector_Theta_Angle.independent.max/coeff);
			final_Argument_Spinbox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			final_Argument_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			final_Argument_Spinbox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		argument_GroupBox_Layout->addWidget(final_Argument_Spinbox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(final_Argument_Spinbox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		argument_Units_Label = new QLabel(independent_Curve->angular_Units);
		argument_GroupBox_Layout->addWidget(argument_Units_Label,0,Qt::AlignLeft);
	}
}

void Detector_Independent_Curve_Part::create_Beam_GroupBox()
{
	QGroupBox* beam_GroupBox = new QGroupBox("Beam");
	main_Layout->addWidget(beam_GroupBox);

	QGridLayout* beam_GroupBox_Layout = new QGridLayout(beam_GroupBox);
	beam_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// at fixed wavelength
	{
		at_Fixed_Wavelength_Label = new QLabel;
			fill_At_Fixed_Wavelength_Label();
		beam_GroupBox_Layout->addWidget(at_Fixed_Wavelength_Label,0,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double coeff = wavelength_Coefficients_Map.value(independent_Curve->spectral_Units);

		at_Fixed_Wavelength_SpinBox = new MyDoubleSpinBox;
			at_Fixed_Wavelength_SpinBox->setAccelerated(true);
			at_Fixed_Wavelength_SpinBox->setRange(0,MAX_DOUBLE);
			at_Fixed_Wavelength_SpinBox->setDecimals(7);
			at_Fixed_Wavelength_SpinBox->setSingleStep(0.01);
			at_Fixed_Wavelength_SpinBox->setValue(Global_Variables::wavelength_Energy(independent_Curve->spectral_Units,independent_Curve->measurement.wavelength.value)/coeff);
			at_Fixed_Wavelength_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			at_Fixed_Wavelength_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(at_Fixed_Wavelength_SpinBox,0,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(at_Fixed_Wavelength_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_Wavelength_Units_Label = new QLabel(independent_Curve->spectral_Units);
		beam_GroupBox_Layout->addWidget(at_Fixed_Wavelength_Units_Label,0,2,Qt::AlignLeft);
	}
	// at fixed beam theta_0
	{
		at_Fixed_Beam_Theta_0_Label = new QLabel("At fixed grazing angle "+Theta_Sym+Zero_Subscript_Sym);
		beam_GroupBox_Layout->addWidget(at_Fixed_Beam_Theta_0_Label,1,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);

		at_Fixed_Beam_Theta_0_SpinBox = new MyDoubleSpinBox;
			at_Fixed_Beam_Theta_0_SpinBox->setAccelerated(true);
			at_Fixed_Beam_Theta_0_SpinBox->setRange(0,90./coeff);
			at_Fixed_Beam_Theta_0_SpinBox->setDecimals(7);
			at_Fixed_Beam_Theta_0_SpinBox->setSingleStep(0.01);
			at_Fixed_Beam_Theta_0_SpinBox->setValue(independent_Curve->measurement.beam_Theta_0_Angle.value/coeff);
			at_Fixed_Beam_Theta_0_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			at_Fixed_Beam_Theta_0_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(at_Fixed_Beam_Theta_0_SpinBox,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(at_Fixed_Beam_Theta_0_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		at_Fixed_Beam_Theta_0_Units_Label = new QLabel(independent_Curve->angular_Units);
		beam_GroupBox_Layout->addWidget(at_Fixed_Beam_Theta_0_Units_Label,1,2,Qt::AlignLeft);
	}
	// polarization
	{
		QLabel* polarization_Label = new QLabel("Polarization");
		beam_GroupBox_Layout->addWidget(polarization_Label,2,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		polarization_SpinBox = new MyDoubleSpinBox;
			polarization_SpinBox->setAccelerated(true);
			polarization_SpinBox->setRange(-1, 1);
			polarization_SpinBox->setDecimals(3);
			polarization_SpinBox->setValue(independent_Curve->measurement.polarization);
			polarization_SpinBox->setSingleStep(0.01);
			polarization_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			polarization_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(polarization_SpinBox,2,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(polarization_SpinBox);
	}
	// background
	{
		QLabel* background_Label = new QLabel("Background");
		beam_GroupBox_Layout->addWidget(background_Label,3,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		background_SpinBox = new MyDoubleSpinBox;
			background_SpinBox->setAccelerated(true);
			background_SpinBox->setRange(0, MAX_DOUBLE);
			background_SpinBox->setDecimals(10);
			background_SpinBox->setValue(independent_Curve->measurement.background);
			background_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			background_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			background_SpinBox->setProperty(min_Size_Property, TARGET_LINE_AT_FIXED_WIDTH);
		beam_GroupBox_Layout->addWidget(background_SpinBox,3,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(background_SpinBox);
	}
	// spectral distribution
	{
		spectral_Width_Label = new QLabel;
			fill_Spectral_Width_Label();
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


void Detector_Independent_Curve_Part::refresh_Angular_Units()
{
	start_Argument_Spinbox->blockSignals(true);
	final_Argument_Spinbox->blockSignals(true);
	at_Fixed_Beam_Theta_0_SpinBox->blockSignals(true);
	angular_Divergence_SpinBox->blockSignals(true);
	independent_Common_Part->crystal_Resolution_SpinBox->blockSignals(true);

	double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);

	// argument
	start_Argument_Spinbox->setRange(0,180./coeff);
	final_Argument_Spinbox->setRange(0,180./coeff);
	start_Argument_Spinbox->setValue(independent_Curve->measurement.detector_Theta_Angle.independent.min/coeff);
	final_Argument_Spinbox->setValue(independent_Curve->measurement.detector_Theta_Angle.independent.max/coeff);
	argument_Units_Label->setText(independent_Curve->angular_Units);

	// at fixed beam angle
	at_Fixed_Beam_Theta_0_SpinBox->setRange(0,90./coeff);
	at_Fixed_Beam_Theta_0_SpinBox->setValue(independent_Curve->measurement.beam_Theta_0_Angle.value/coeff);
	at_Fixed_Beam_Theta_0_Units_Label->setText(independent_Curve->angular_Units);

	// beam divergence
	angular_Divergence_SpinBox->setValue(independent_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution/coeff);
	angular_Divergence_Units_Label->setText(independent_Curve->angular_Units);

	// crystal resolution
	independent_Common_Part->crystal_Resolution_SpinBox->setValue(independent_Curve->measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution/coeff);
	independent_Common_Part->crystal_Resolution_Units_Label->setText(independent_Curve->angular_Units);

	start_Argument_Spinbox->blockSignals(false);
	final_Argument_Spinbox->blockSignals(false);
	at_Fixed_Beam_Theta_0_SpinBox->blockSignals(false);
	angular_Divergence_SpinBox->blockSignals(false);
	independent_Common_Part->crystal_Resolution_SpinBox->blockSignals(false);

	independent_Curve->refresh_Description_Label();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(independent_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(independent_Curve->measurement.id);
			curve_Plot_1D->plot_All_Data();
			curve_Plot_1D->set_Title_Text();
		}
	}
}

void Detector_Independent_Curve_Part::refresh_Spectral_Units()
{
	at_Fixed_Wavelength_SpinBox->blockSignals(true);

	double coeff = wavelength_Coefficients_Map.value(independent_Curve->spectral_Units);

	fill_At_Fixed_Wavelength_Label();
	at_Fixed_Wavelength_SpinBox->setValue(Global_Variables::wavelength_Energy(independent_Curve->spectral_Units,independent_Curve->measurement.wavelength.value)/coeff);
	at_Fixed_Wavelength_Units_Label->setText(independent_Curve->spectral_Units);
	fill_Spectral_Width_Label();

	at_Fixed_Wavelength_SpinBox->blockSignals(false);

	independent_Curve->refresh_Description_Label();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(independent_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(independent_Curve->measurement.id);
			curve_Plot_1D->plot_All_Data();
			curve_Plot_1D->set_Title_Text();
		}
	}
}

void Detector_Independent_Curve_Part::refresh_Argument_Values()
{
	double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
	independent_Curve->measurement.detector_Theta_Angle.independent.num_Points = num_Points_Spinbox->value();
	independent_Curve->measurement.detector_Theta_Angle.independent.min = start_Argument_Spinbox->value()*coeff;
	independent_Curve->measurement.detector_Theta_Angle.independent.max = final_Argument_Spinbox->value()*coeff;

	independent_Curve->refresh_Description_Label();

	// independently of main calculation
	independent_Curve->calc_Independent_cos2_k();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(independent_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(independent_Curve->measurement.id);
			curve_Plot_1D->plot_All_Data();
		}
	}
}

void Detector_Independent_Curve_Part::refresh_At_Fixed_Wavelength_Value()
{
	double coeff = wavelength_Coefficients_Map.value(independent_Curve->spectral_Units);
	independent_Curve->measurement.wavelength.value = Global_Variables::wavelength_Energy(independent_Curve->spectral_Units, at_Fixed_Wavelength_SpinBox->value()*coeff);

	independent_Curve->refresh_Description_Label();

	// independently of main calculation
	independent_Curve->calc_Independent_cos2_k();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(independent_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(independent_Curve->measurement.id);
			curve_Plot_1D->set_Title_Text();
		}
	}
}

void Detector_Independent_Curve_Part::refresh_At_Fixed_Beam_Theta_0_Value()
{
	double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
	independent_Curve->measurement.beam_Theta_0_Angle.value = at_Fixed_Beam_Theta_0_SpinBox->value()*coeff;

	independent_Curve->refresh_Description_Label();

	// independently of main calculation
	independent_Curve->calc_Independent_cos2_k();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(independent_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(independent_Curve->measurement.id);
			curve_Plot_1D->set_Title_Text();
		}
	}
}

void Detector_Independent_Curve_Part::fill_At_Fixed_Wavelength_Label()
{
	if(	independent_Curve->spectral_Units == wavelength_Units_List[angstrom] ||
		independent_Curve->spectral_Units == wavelength_Units_List[nm]		  )
	{
		at_Fixed_Wavelength_Label->setText("At fixed wavelength");
	} else
	{
		at_Fixed_Wavelength_Label->setText("At fixed energy");
	}
}

void Detector_Independent_Curve_Part::fill_Spectral_Width_Label()
{
	if(	independent_Curve->spectral_Units == wavelength_Units_List[angstrom] ||
		independent_Curve->spectral_Units == wavelength_Units_List[nm]	   )
	{
		spectral_Width_Label->setText("  Spectral width, FWHM, " + Delta_Big_Sym + Lambda_Sym+"/"+Lambda_Sym);
	} else
	{
		spectral_Width_Label->setText("  Spectral width, FWHM, " + Delta_Big_Sym + "E/E");
	}
}


void Detector_Independent_Curve_Part::connecting()
{
	refresh_Angular_Units();
	refresh_Spectral_Units();

	/// units box
	connect(angular_Units_ComboBox,	&QComboBox::currentTextChanged, this, [=]
	{
		independent_Curve->angular_Units = angular_Units_ComboBox->currentText();
		refresh_Angular_Units();
	});
	connect(spectral_Units_ComboBox,	&QComboBox::currentTextChanged, this, [=]
	{
		independent_Curve->spectral_Units = spectral_Units_ComboBox->currentText();
		refresh_Spectral_Units();
	});

	/// argument box
	// number of points
	connect(num_Points_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
	{
		refresh_Argument_Values();
		global_Multilayer_Approach->global_Recalculate();
	});
	// start value
	connect(start_Argument_Spinbox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_Argument_Values();
		global_Multilayer_Approach->global_Recalculate();
	});
	// final value
	connect(final_Argument_Spinbox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		refresh_Argument_Values();
		global_Multilayer_Approach->global_Recalculate();
	});

	/// beam box
	// at fixed beam
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
	// polarization
	connect(polarization_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.polarization = polarization_SpinBox->value();

		// curve plots
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(independent_Curve->measurement.id))
			{
				Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(independent_Curve->measurement.id);
				curve_Plot_1D->set_Title_Text();
			}
		}
		global_Multilayer_Approach->global_Recalculate();
	});
	// background
	connect(background_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.background = background_SpinBox->value();

		global_Multilayer_Approach->global_Recalculate();
	});
	// spectral width
	connect(spectral_Width_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		independent_Curve->measurement.spectral_Distribution.FWHM_distribution = spectral_Width_SpinBox->value();

		global_Multilayer_Approach->global_Recalculate();
	});
	// angular divergence
	connect(angular_Divergence_SpinBox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double coeff = angle_Coefficients_Map.value(independent_Curve->angular_Units);
		independent_Curve->measurement.beam_Theta_0_Distribution.FWHM_distribution = angular_Divergence_SpinBox->value()*coeff;

		global_Multilayer_Approach->global_Recalculate();
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
}
