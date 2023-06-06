#include "common_part.h"

Common_Part::Common_Part(Independent_Curve* independent_Curve, Target_Curve* target_Curve, bool is_Independent, Target_Curve_Editor* target_Curve_Editor, QWidget *parent) :
	measurement   (is_Independent ? independent_Curve->measurement    : target_Curve->measurement),
	angular_Units (is_Independent ? independent_Curve->angular_Units  : target_Curve->angular_Units),
	spectral_Units(is_Independent ? independent_Curve->spectral_Units : target_Curve->spectral_Units),
	is_Independent(is_Independent),
	target_Curve_Editor(target_Curve_Editor),
	QWidget(parent)
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setContentsMargins(0,0,0,0);
		main_Layout->setSpacing(0);

	if( measurement.measurement_Type == measurement_Types[Specular_Scan] ||
		measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		create_1D_Detector_GroupBox();
	}
	if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		create_2D_Detector_GroupBox();
	}
	create_Footptint_GroupBox();

	connecting();
}

void Common_Part::create_1D_Detector_GroupBox()
{
	QGroupBox* detector_GroupBox = new QGroupBox("Detector");
	main_Layout->addWidget(detector_GroupBox);

	QVBoxLayout* detector_GroupBox_Layout = new QVBoxLayout(detector_GroupBox);
	detector_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	QHBoxLayout* first_Row_Layout = new QHBoxLayout;
		first_Row_Layout->setAlignment(Qt::AlignLeft);
	detector_GroupBox_Layout->addLayout(first_Row_Layout,Qt::AlignLeft);

	// detector type
	{
		detector_Type_Label = new QLabel("Detector type");
		first_Row_Layout->addWidget(detector_Type_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detector_Type_ComboBox = new QComboBox;
			detector_Type_ComboBox->addItem(detectors[Slit]);
			detector_Type_ComboBox->addItem(detectors[Crystal]);
			detector_Type_ComboBox->setCurrentText(measurement.detector_1D.detector_Type);
			detector_Type_ComboBox->setFixedWidth(70);
		first_Row_Layout->addWidget(detector_Type_ComboBox,0,Qt::AlignLeft);
	}
	// detector distance
	{
		QLabel* slit_Distance_Label = new QLabel("        Distance from detector to sample");
		first_Row_Layout->addWidget(slit_Distance_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detector_Distance_SpinBox = new MyDoubleSpinBox;
			detector_Distance_SpinBox->setAccelerated(true);
			detector_Distance_SpinBox->setRange(0, MAX_DOUBLE);
			detector_Distance_SpinBox->setDecimals(1);
			detector_Distance_SpinBox->setValue(measurement.detector_1D.distance_To_Sample);
			detector_Distance_SpinBox->setSingleStep(1);
			detector_Distance_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			detector_Distance_SpinBox->setProperty(min_Size_Property,45);
		first_Row_Layout->addWidget(detector_Distance_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(detector_Distance_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Distance_Label = new QLabel("mm    ");
		first_Row_Layout->addWidget(mm_Distance_Label,0,Qt::AlignLeft);
	}
	// binning
	if(!is_Independent)
	{
		use_Binning_Checkbox = new QCheckBox("Merge points");
			use_Binning_Checkbox->setChecked(measurement.detector_1D.use_Binning);
		first_Row_Layout->addWidget(use_Binning_Checkbox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		binning_Factor_Spinbox = new QSpinBox;
			binning_Factor_Spinbox->setRange(1, MAX_BINNING_FACTOR);
			binning_Factor_Spinbox->setSingleStep(1);
			binning_Factor_Spinbox->setValue(measurement.detector_1D.binning_Factor);
			binning_Factor_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			binning_Factor_Spinbox->setFixedWidth(30);
			binning_Factor_Spinbox->setEnabled(measurement.detector_1D.use_Binning);
		first_Row_Layout->addWidget(binning_Factor_Spinbox,0,Qt::AlignLeft);
	}

	QHBoxLayout* second_Row_Layout = new QHBoxLayout;
		second_Row_Layout->setAlignment(Qt::AlignLeft);
	detector_GroupBox_Layout->addLayout(second_Row_Layout,Qt::AlignLeft);

	// slit length
	{
		finite_Slit_Length_Checkbox = new QCheckBox("Slit length");
			finite_Slit_Length_Checkbox->setChecked(measurement.detector_1D.finite_Slit);
		second_Row_Layout->addWidget(finite_Slit_Length_Checkbox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		slit_Length_SpinBox = new MyDoubleSpinBox;
			slit_Length_SpinBox->setEnabled(measurement.detector_1D.finite_Slit);
			slit_Length_SpinBox->setAccelerated(true);
			slit_Length_SpinBox->setRange(0, MAX_DOUBLE);
			slit_Length_SpinBox->setDecimals(3);
			slit_Length_SpinBox->setValue(measurement.detector_1D.slit_Length);
			slit_Length_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			slit_Length_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			slit_Length_SpinBox->setProperty(min_Size_Property,50);
		second_Row_Layout->addWidget(slit_Length_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(slit_Length_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		slit_Length_Units_Label = new QLabel("mm");
			slit_Length_Units_Label->setEnabled(measurement.detector_1D.finite_Slit);
		second_Row_Layout->addWidget(slit_Length_Units_Label,0,Qt::AlignLeft);
	}
	// pages
	{
		detectors_Stack = new QStackedWidget;
		second_Row_Layout->addWidget(detectors_Stack);
    }
	// slit
	{
		QWidget* slit_Page = new QWidget;
		detectors_Stack->addWidget(slit_Page);
		QHBoxLayout* slit_Layout = new QHBoxLayout(slit_Page);
			slit_Layout->setContentsMargins(0,0,0,0);
		slit_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* slit_Width_Label = new QLabel("          Slit width");
		slit_Layout->addWidget(slit_Width_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		slit_Width_SpinBox = new MyDoubleSpinBox;
			slit_Width_SpinBox->setAccelerated(true);
			slit_Width_SpinBox->setRange(0, MAX_DOUBLE);
			slit_Width_SpinBox->setDecimals(3);
			slit_Width_SpinBox->setValue(measurement.detector_1D.slit_Width);
//			slit_Width_SpinBox->setSingleStep(0.01);
			slit_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			slit_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			slit_Width_SpinBox->setProperty(min_Size_Property,50);
		slit_Layout->addWidget(slit_Width_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(slit_Width_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Width_Label = new QLabel("mm");
		slit_Layout->addWidget(mm_Width_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
            measurement.measurement_Type == measurement_Types[Offset_Scan])
        {
            setup_Beam_Distribution_Button_slit = new QPushButton("Set up distribution");
            slit_Layout->addWidget(setup_Beam_Distribution_Button_slit, 0, Qt::AlignLeft);
        }
	}
	// crystal
	{
		QWidget* crystal_Page = new QWidget;
		detectors_Stack->addWidget(crystal_Page);
		QHBoxLayout* crystal_Layout = new QHBoxLayout(crystal_Page);
			crystal_Layout->setContentsMargins(0,0,0,0);
		crystal_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* crystal_Resolution_Label = new QLabel("   Angular resolution, FWHM, "+Delta_Big_Sym+Theta_Sym);
		if(!is_Independent) crystal_Resolution_Label->setText("       "+crystal_Resolution_Label->text());
		crystal_Layout->addWidget(crystal_Resolution_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		double arg_Coeff = angle_Coefficients_Map.value(angular_Units);

		crystal_Resolution_SpinBox = new MyDoubleSpinBox;
			crystal_Resolution_SpinBox->setAccelerated(true);
			crystal_Resolution_SpinBox->setRange(0, 90/arg_Coeff);
			crystal_Resolution_SpinBox->setDecimals(6);
			crystal_Resolution_SpinBox->setValue(measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution/arg_Coeff);
			crystal_Resolution_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			crystal_Resolution_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			crystal_Resolution_SpinBox->setProperty(min_Size_Property, 55);
		crystal_Layout->addWidget(crystal_Resolution_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(crystal_Resolution_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		crystal_Resolution_Units_Label = new QLabel(angular_Units);
		crystal_Layout->addWidget(crystal_Resolution_Units_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

        QLabel* crystal_Resolution_Function_Label = new QLabel(" ");
        if(!is_Independent) crystal_Resolution_Function_Label->setText("     Function");
        crystal_Layout->addWidget(crystal_Resolution_Function_Label,0,Qt::AlignLeft);
        if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
            measurement.measurement_Type == measurement_Types[Offset_Scan])
        {
            crystal_Resolution_Function_Label->hide();
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - -

        resolution_Function_ComboBox = new QComboBox;
        resolution_Function_ComboBox->addItems(distributions);
        resolution_Function_ComboBox->setCurrentText(measurement.detector_1D.detector_Theta_Resolution.distribution_Function);
        resolution_Function_ComboBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
        crystal_Layout->addWidget(resolution_Function_ComboBox,0,Qt::AlignLeft);

        if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
            measurement.measurement_Type == measurement_Types[Offset_Scan])
        {
            resolution_Function_ComboBox->hide();

            // set up distribution
            setup_Beam_Distribution_Button_crystal = new QPushButton("Set up distribution");
            crystal_Layout->addWidget(setup_Beam_Distribution_Button_crystal, 0, Qt::AlignLeft);
        }
	}

	if(measurement.detector_1D.detector_Type == detectors[Slit])	detectors_Stack->setCurrentIndex(0);
	if(measurement.detector_1D.detector_Type == detectors[Crystal]) detectors_Stack->setCurrentIndex(1);
}

void Common_Part::create_2D_Detector_GroupBox()
{
	QGroupBox* detector_GroupBox = new QGroupBox("Detector");
	main_Layout->addWidget(detector_GroupBox);

	QGridLayout* detector_GroupBox_Layout = new QGridLayout(detector_GroupBox);
	detector_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// detector type
	{
		QHBoxLayout* detector_Type_Layout = new QHBoxLayout;
		detector_Type_Layout->setAlignment(Qt::AlignLeft);
		detector_GroupBox_Layout->addLayout(detector_Type_Layout,0,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		set_Pixel_Size_Checkbox = new QCheckBox("Set pixel size");
			set_Pixel_Size_Checkbox->setChecked(measurement.detector_2D.set_Pixel_Size);
		detector_Type_Layout->addWidget(set_Pixel_Size_Checkbox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -
                detector_Type_Label = new QLabel;//("Detector type");
                        detector_Type_Label->setEnabled(measurement.detector_2D.set_Pixel_Size);
                 detector_Type_Layout->addWidget(detector_Type_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detector_Type_ComboBox = new QComboBox;
                        detector_Type_ComboBox->addItem(detectors[Linear]);
                        detector_Type_ComboBox->addItem(detectors[Angular]);
			detector_Type_ComboBox->setCurrentText(measurement.detector_2D.detector_Type);
			detector_Type_ComboBox->setEnabled(measurement.detector_2D.set_Pixel_Size);
			detector_Type_ComboBox->setFixedWidth(70);
		detector_Type_Layout->addWidget(detector_Type_ComboBox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QHBoxLayout* detector_Distance_Layout = new QHBoxLayout;
		detector_Distance_Layout->setAlignment(Qt::AlignLeft);
		if(is_Independent)	{detector_GroupBox_Layout->addLayout(detector_Distance_Layout,0,1,Qt::AlignLeft);}
		else				{detector_GroupBox_Layout->addLayout(detector_Distance_Layout,1,0,Qt::AlignLeft);}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QString space = "";
		if(is_Independent)	space = "        ";
		QLabel* slit_Distance_Label = new QLabel(space+"Distance from detector to sample");
		detector_Distance_Layout->addWidget(slit_Distance_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detector_Distance_SpinBox = new MyDoubleSpinBox;
			detector_Distance_SpinBox->setAccelerated(true);
			detector_Distance_SpinBox->setRange(0, MAX_DOUBLE);
			detector_Distance_SpinBox->setDecimals(1);
			detector_Distance_SpinBox->setValue(measurement.detector_2D.distance_To_Sample);
			detector_Distance_SpinBox->setSingleStep(1);
			detector_Distance_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			detector_Distance_SpinBox->setProperty(min_Size_Property,45);
		detector_Distance_Layout->addWidget(detector_Distance_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(detector_Distance_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Distance_Label = new QLabel("mm");
		detector_Distance_Layout->addWidget(mm_Distance_Label,0,Qt::AlignLeft);
	}
	// binning
	if(!is_Independent)
	{
		use_Binning_Checkbox = new QCheckBox("Merge pixels");
			use_Binning_Checkbox->setChecked(measurement.detector_2D.use_Binning);
		detector_GroupBox_Layout->addWidget(use_Binning_Checkbox,0,1,2,1,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		theta_Binning_Factor_Label = new QLabel(Theta_Sym + " pixels");
			theta_Binning_Factor_Label->setEnabled(measurement.detector_2D.use_Binning);
		detector_GroupBox_Layout->addWidget(theta_Binning_Factor_Label,0,2,1,1,Qt::AlignLeft);

		theta_Binning_Factor_Spinbox = new QSpinBox;
			theta_Binning_Factor_Spinbox->setRange(1, MAX_BINNING_FACTOR);
			theta_Binning_Factor_Spinbox->setSingleStep(1);
			theta_Binning_Factor_Spinbox->setValue(measurement.detector_2D.theta_Binning_Factor);
			theta_Binning_Factor_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			theta_Binning_Factor_Spinbox->setFixedWidth(30);
			theta_Binning_Factor_Spinbox->setEnabled(measurement.detector_2D.use_Binning);
		detector_GroupBox_Layout->addWidget(theta_Binning_Factor_Spinbox,0,3,1,1,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		phi_Binning_Factor_Label = new QLabel(Phi_Sym + " pixels");
			phi_Binning_Factor_Label->setEnabled(measurement.detector_2D.use_Binning);
		detector_GroupBox_Layout->addWidget(phi_Binning_Factor_Label,1,2,1,1,Qt::AlignLeft);

		phi_Binning_Factor_Spinbox = new QSpinBox;
			phi_Binning_Factor_Spinbox->setRange(1, MAX_BINNING_FACTOR);
			phi_Binning_Factor_Spinbox->setSingleStep(1);
			phi_Binning_Factor_Spinbox->setValue(measurement.detector_2D.phi_Binning_Factor);
			phi_Binning_Factor_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			phi_Binning_Factor_Spinbox->setFixedWidth(30);
			phi_Binning_Factor_Spinbox->setEnabled(measurement.detector_2D.use_Binning);
		detector_GroupBox_Layout->addWidget(phi_Binning_Factor_Spinbox,1,3,1,1,Qt::AlignLeft);
	}
	// pages
	{
		detectors_Stack = new QStackedWidget;
			detectors_Stack->setEnabled(measurement.detector_2D.set_Pixel_Size);
		if(is_Independent)
		{
			detector_GroupBox_Layout->addWidget(detectors_Stack,1,0,1,2);
		} else
		{
			detector_GroupBox_Layout->addWidget(detectors_Stack,0,5,2,1);
		}
	}
	// matrix
	{
		QWidget* matrix_Page = new QWidget;
		detectors_Stack->addWidget(matrix_Page);
		QGridLayout* matrix_Layout = new QGridLayout(matrix_Page);
			matrix_Layout->setContentsMargins(0,0,0,0);
		matrix_Layout->setAlignment(Qt::AlignLeft);

		// height
		{
			QLabel* pixel_Height_Size_Label = new QLabel("    Pixel height (polar)");
			matrix_Layout->addWidget(pixel_Height_Size_Label,0,0,Qt::AlignLeft);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - -

			pixel_Polar_Height_SpinBox = new MyDoubleSpinBox;
				pixel_Polar_Height_SpinBox->setAccelerated(true);
				pixel_Polar_Height_SpinBox->setRange(0, MAX_DOUBLE);
				pixel_Polar_Height_SpinBox->setDecimals(1);
				pixel_Polar_Height_SpinBox->setValue(measurement.detector_2D.pixel_Polar_Height);
//				pixel_Polar_Height_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				pixel_Polar_Height_SpinBox->setSingleStep(1);
				pixel_Polar_Height_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				pixel_Polar_Height_SpinBox->setProperty(min_Size_Property,45);
			matrix_Layout->addWidget(pixel_Polar_Height_SpinBox,0,1,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(pixel_Polar_Height_SpinBox);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - -

			QLabel* pixel_Polar_Height_Units_Label = new QLabel(Mu_Sym+"m");
			matrix_Layout->addWidget(pixel_Polar_Height_Units_Label,0,2,Qt::AlignLeft);
		}
		// phi
		{
			QLabel* pixel_Width_Size_Label = new QLabel("    Pixel width (azimuthal)");
			matrix_Layout->addWidget(pixel_Width_Size_Label,1,0,Qt::AlignLeft);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - -

			pixel_Azimuthal_Width_SpinBox = new MyDoubleSpinBox;
				pixel_Azimuthal_Width_SpinBox->setAccelerated(true);
				pixel_Azimuthal_Width_SpinBox->setRange(0, MAX_DOUBLE);
				pixel_Azimuthal_Width_SpinBox->setDecimals(1);
				pixel_Azimuthal_Width_SpinBox->setValue(measurement.detector_2D.pixel_Azimuthal_Width);
//				pixel_Azimuthal_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				pixel_Azimuthal_Width_SpinBox->setSingleStep(1);
				pixel_Azimuthal_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				pixel_Azimuthal_Width_SpinBox->setProperty(min_Size_Property,45);
			matrix_Layout->addWidget(pixel_Azimuthal_Width_SpinBox,1,1,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(pixel_Azimuthal_Width_SpinBox);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - -

			QLabel* pixel_Azimuthal_Width_Units_Label = new QLabel(Mu_Sym+"m");
			matrix_Layout->addWidget(pixel_Azimuthal_Width_Units_Label,1,2,Qt::AlignLeft);
		}
	}
	// spherical
	{
		QWidget* spherical_Page = new QWidget;
		detectors_Stack->addWidget(spherical_Page);
		QGridLayout* spherical_Layout = new QGridLayout(spherical_Page);
			spherical_Layout->setContentsMargins(0,0,0,0);
		spherical_Layout->setAlignment(Qt::AlignLeft);

		// theta
		{
			QLabel* spherical_Theta_Resolution_Label = new QLabel("    Pixel height, FWHM, "+Delta_Big_Sym+Theta_Sym); // "    Angular resolution, FWHM, "+Delta_Big_Sym+Theta_Sym
			spherical_Layout->addWidget(spherical_Theta_Resolution_Label,0,0,Qt::AlignLeft);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - -

			double arg_Coeff = angle_Coefficients_Map.value(angular_Units);

			theta_Resolution_SpinBox = new MyDoubleSpinBox;
				theta_Resolution_SpinBox->setAccelerated(true);
				theta_Resolution_SpinBox->setRange(0, MAX_DOUBLE);
				theta_Resolution_SpinBox->setDecimals(6);
				theta_Resolution_SpinBox->setValue(measurement.detector_2D.detector_Theta_Resolution.FWHM_distribution/arg_Coeff);
				theta_Resolution_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				theta_Resolution_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				theta_Resolution_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
			spherical_Layout->addWidget(theta_Resolution_SpinBox,0,1,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(theta_Resolution_SpinBox);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - -

			theta_Resolution_Units_Label = new QLabel(angular_Units);
			spherical_Layout->addWidget(theta_Resolution_Units_Label,0,2,Qt::AlignLeft);
		}
		// phi
		{
			QLabel* spherical_Phi_Resolution_Label = new QLabel("    Pixel width,  FWHM, "+Delta_Big_Sym+Phi_Sym);// "    Angular resolution, FWHM, "+Delta_Big_Sym+Phi_Sym
			spherical_Layout->addWidget(spherical_Phi_Resolution_Label,1,0,Qt::AlignLeft);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - -

			double arg_Coeff = angle_Coefficients_Map.value(angular_Units);

			phi_Resolution_SpinBox = new MyDoubleSpinBox;
				phi_Resolution_SpinBox->setAccelerated(true);
				phi_Resolution_SpinBox->setRange(0, MAX_DOUBLE);
				phi_Resolution_SpinBox->setDecimals(6);
				phi_Resolution_SpinBox->setValue(measurement.detector_2D.detector_Phi_Resolution.FWHM_distribution/arg_Coeff);
				phi_Resolution_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				phi_Resolution_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				phi_Resolution_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
			spherical_Layout->addWidget(phi_Resolution_SpinBox,1,1,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(phi_Resolution_SpinBox);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - -

			phi_Resolution_Units_Label = new QLabel(angular_Units);
			spherical_Layout->addWidget(phi_Resolution_Units_Label,1,2,Qt::AlignLeft);
		}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* spherical_Resolution_Function_Label = new QLabel("       Function");
		spherical_Layout->addWidget(spherical_Resolution_Function_Label,0,3,2,1,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		resolution_Function_ComboBox = new QComboBox;
			resolution_Function_ComboBox->addItems(distributions);
			resolution_Function_ComboBox->setCurrentText(measurement.detector_2D.detector_Theta_Resolution.distribution_Function); // detector_Theta_Resolution.distribution_Function = detector_Phi_Resolution.distribution_Function
			resolution_Function_ComboBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
		spherical_Layout->addWidget(resolution_Function_ComboBox,0,4,2,1,Qt::AlignLeft);
	}

        if(measurement.detector_2D.detector_Type == detectors[Linear])		detectors_Stack->setCurrentIndex(0);
        if(measurement.detector_2D.detector_Type == detectors[Angular])	detectors_Stack->setCurrentIndex(1);
}

void Common_Part::create_Footptint_GroupBox()
{
        QGroupBox* footprint_GroupBox = new QGroupBox("Footprint and distortion");
	main_Layout->addWidget(footprint_GroupBox);

	QGridLayout* footprint_GroupBox_Layout = new QGridLayout(footprint_GroupBox);
		footprint_GroupBox_Layout->setAlignment(Qt::AlignLeft);
		if(is_Independent)	footprint_GroupBox_Layout->setHorizontalSpacing(10);
		else				footprint_GroupBox_Layout->setHorizontalSpacing(0);
		footprint_GroupBox_Layout->setVerticalSpacing(0);

	// beam
	{
		QGroupBox* beam_Footprint_GroupBox = new QGroupBox("Beam");

		QGridLayout* beam_Footprint_GroupBox_Layout = new QGridLayout(beam_Footprint_GroupBox);
			beam_Footprint_GroupBox_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -
		/// width
		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* beam_Footprint_Width_Label = new QLabel("Width, FWHM");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Width_Label,0,0,Qt::AlignLeft);

		beam_Footprint_Width_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Width_SpinBox->setAccelerated(true);
			beam_Footprint_Width_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Footprint_Width_SpinBox->setDecimals(3);
			beam_Footprint_Width_SpinBox->setValue(measurement.beam_Geometry.size);
			beam_Footprint_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Footprint_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Width_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH_BEAM);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Width_SpinBox,0,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Width_SpinBox);

		QLabel* mm_beam_Footprint_Width_Label = new QLabel("mm");
		beam_Footprint_GroupBox_Layout->addWidget(mm_beam_Footprint_Width_Label,0,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -
		/// smoothing
		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		/// "asymmetric" interface

//		QHBoxLayout* beam_Footprint_Smoothing_Layout = new QHBoxLayout;
//		beam_Footprint_GroupBox_Layout->addLayout(beam_Footprint_Smoothing_Layout,2,0,1,4,Qt::AlignLeft);

//		QLabel* beam_Footprint_Shape_Label = new QLabel("Profile: left");
//		beam_Footprint_Smoothing_Layout->addWidget(beam_Footprint_Shape_Label,Qt::AlignLeft);

//		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

//		beam_Footprint_Left_Smoothing_SpinBox = new MyDoubleSpinBox;
//			beam_Footprint_Left_Smoothing_SpinBox->setAccelerated(true);
//			beam_Footprint_Left_Smoothing_SpinBox->setRange(0.01, 1);
//			beam_Footprint_Left_Smoothing_SpinBox->setDecimals(2);
//			beam_Footprint_Left_Smoothing_SpinBox->setValue(measurement.beam_Geometry.smoothing);
//			beam_Footprint_Left_Smoothing_SpinBox->setSingleStep(0.01);
//			beam_Footprint_Left_Smoothing_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//			beam_Footprint_Left_Smoothing_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH_WING-1);
//		beam_Footprint_Smoothing_Layout->addWidget(beam_Footprint_Left_Smoothing_SpinBox,Qt::AlignLeft);
//		Global_Variables::resize_Line_Edit(beam_Footprint_Left_Smoothing_SpinBox);

//		beam_Footprint_Right_Smoothing_Checkbox = new QCheckBox("right");
//			beam_Footprint_Right_Smoothing_Checkbox->setChecked(measurement.beam_Geometry.asymmetric_Smoothing);
//		beam_Footprint_Smoothing_Layout->addWidget(beam_Footprint_Right_Smoothing_Checkbox,Qt::AlignLeft);

//		beam_Footprint_Right_Smoothing_SpinBox = new MyDoubleSpinBox;
//			beam_Footprint_Right_Smoothing_SpinBox->setAccelerated(true);
//			beam_Footprint_Right_Smoothing_SpinBox->setRange(0.01, 1);
//			beam_Footprint_Right_Smoothing_SpinBox->setDecimals(2);
//			beam_Footprint_Right_Smoothing_SpinBox->setValue(measurement.beam_Geometry.right_Smoothing);
//			beam_Footprint_Right_Smoothing_SpinBox->setSingleStep(0.01);
//			beam_Footprint_Right_Smoothing_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//			beam_Footprint_Right_Smoothing_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH_WING-1);
//			beam_Footprint_Right_Smoothing_SpinBox->setEnabled(measurement.beam_Geometry.asymmetric_Smoothing);
//		beam_Footprint_Smoothing_Layout->addWidget(beam_Footprint_Right_Smoothing_SpinBox,Qt::AlignLeft);
//		Global_Variables::resize_Line_Edit(beam_Footprint_Right_Smoothing_SpinBox);

//		QLabel* empty_beam_Footprint_Smoothing_Label = new QLabel("  ");
//		beam_Footprint_Smoothing_Layout->addWidget(empty_beam_Footprint_Smoothing_Label,Qt::AlignLeft);

		/// "symmetric" interface

		QLabel* beam_Footprint_Shape_Label = new QLabel("Profile smoothing");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Shape_Label,2,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Footprint_Left_Smoothing_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Left_Smoothing_SpinBox->setAccelerated(true);
			beam_Footprint_Left_Smoothing_SpinBox->setRange(0.01, 1);
			beam_Footprint_Left_Smoothing_SpinBox->setDecimals(2);
			beam_Footprint_Left_Smoothing_SpinBox->setValue(measurement.beam_Geometry.smoothing);
			beam_Footprint_Left_Smoothing_SpinBox->setSingleStep(0.01);
			beam_Footprint_Left_Smoothing_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Left_Smoothing_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH_BEAM);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Left_Smoothing_SpinBox,2,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Left_Smoothing_SpinBox);

		QLabel* empty_beam_Footprint_Smoothing_Label = new QLabel("  ");
		beam_Footprint_GroupBox_Layout->addWidget(empty_beam_Footprint_Smoothing_Label,2,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -
		/// lateral width
		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* beam_Footprint_Lateral_Width_Label = new QLabel("Lateral width");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Lateral_Width_Label,3,0,Qt::AlignLeft);

		beam_Footprint_Lateral_Width_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Lateral_Width_SpinBox->setAccelerated(true);
			beam_Footprint_Lateral_Width_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Footprint_Lateral_Width_SpinBox->setDecimals(3);
			beam_Footprint_Lateral_Width_SpinBox->setValue(measurement.beam_Geometry.lateral_Width);
			beam_Footprint_Lateral_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Footprint_Lateral_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Lateral_Width_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH_BEAM);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Lateral_Width_SpinBox,3,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Lateral_Width_SpinBox);

		QLabel* mm_beam_Footprint_Lateral_Width_Label = new QLabel("mm");
		beam_Footprint_GroupBox_Layout->addWidget(mm_beam_Footprint_Lateral_Width_Label,3,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -
		/// wings
		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		/// "asymmetric" interface

//		QHBoxLayout* beam_Footprint_Wings_Layout = new QHBoxLayout;
//		beam_Footprint_GroupBox_Layout->addLayout(beam_Footprint_Wings_Layout,3,0,1,4,Qt::AlignLeft);

//		QLabel* beam_Footprint_Left_Wing_Width_Label = new QLabel("Wings: left");
//		beam_Footprint_Wings_Layout->addWidget(beam_Footprint_Left_Wing_Width_Label,Qt::AlignLeft);

//		beam_Footprint_Left_Wing_Width_SpinBox = new MyDoubleSpinBox;
//			beam_Footprint_Left_Wing_Width_SpinBox->setAccelerated(true);
//			beam_Footprint_Left_Wing_Width_SpinBox->setRange(0, MAX_DOUBLE);
//			beam_Footprint_Left_Wing_Width_SpinBox->setDecimals(3);
//			beam_Footprint_Left_Wing_Width_SpinBox->setValue(measurement.beam_Geometry.full_Wings_Width);
//			beam_Footprint_Left_Wing_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
//			beam_Footprint_Left_Wing_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//			beam_Footprint_Left_Wing_Width_SpinBox->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH_WING);
//		beam_Footprint_Wings_Layout->addWidget(beam_Footprint_Left_Wing_Width_SpinBox,Qt::AlignLeft);
//		Global_Variables::resize_Line_Edit(beam_Footprint_Left_Wing_Width_SpinBox);

//		beam_Footprint_Right_Wing_Width_Checkbox = new QCheckBox("right");
//			beam_Footprint_Right_Wing_Width_Checkbox->setChecked(measurement.beam_Geometry.asymmetric_Wing_Width);
//		beam_Footprint_Wings_Layout->addWidget(beam_Footprint_Right_Wing_Width_Checkbox,Qt::AlignLeft);

//		beam_Footprint_Right_Wing_Width_SpinBox = new MyDoubleSpinBox;
//			beam_Footprint_Right_Wing_Width_SpinBox->setAccelerated(true);
//			beam_Footprint_Right_Wing_Width_SpinBox->setRange(0, MAX_DOUBLE);
//			beam_Footprint_Right_Wing_Width_SpinBox->setDecimals(3);
//			beam_Footprint_Right_Wing_Width_SpinBox->setValue(measurement.beam_Geometry.right_Wing_Width);
//			beam_Footprint_Right_Wing_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
//			beam_Footprint_Right_Wing_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//			beam_Footprint_Right_Wing_Width_SpinBox->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH_WING-1);
//			beam_Footprint_Right_Wing_Width_SpinBox->setEnabled(measurement.beam_Geometry.asymmetric_Wing_Width);
//		beam_Footprint_Wings_Layout->addWidget(beam_Footprint_Right_Wing_Width_SpinBox,Qt::AlignLeft);
//		Global_Variables::resize_Line_Edit(beam_Footprint_Right_Wing_Width_SpinBox);

//		QLabel* mm_beam_Footprint_Wings_Width_Label = new QLabel("mm");
//		beam_Footprint_Wings_Layout->addWidget(mm_beam_Footprint_Wings_Width_Label,Qt::AlignLeft);

		/// "symmetric" interface

		QLabel* beam_Footprint_Left_Wing_Width_Label = new QLabel("Wings full width");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Left_Wing_Width_Label,4,0,Qt::AlignLeft);

		beam_Footprint_Left_Wing_Width_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Left_Wing_Width_SpinBox->setAccelerated(true);
			beam_Footprint_Left_Wing_Width_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Footprint_Left_Wing_Width_SpinBox->setDecimals(3);
			beam_Footprint_Left_Wing_Width_SpinBox->setValue(measurement.beam_Geometry.wings_Full_Width);
			beam_Footprint_Left_Wing_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Footprint_Left_Wing_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Left_Wing_Width_SpinBox->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH_BEAM);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Left_Wing_Width_SpinBox,4,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Left_Wing_Width_SpinBox);

		QLabel* mm_beam_Footprint_Wings_Width_Label = new QLabel("mm");
		beam_Footprint_GroupBox_Layout->addWidget(mm_beam_Footprint_Wings_Width_Label,4,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* beam_Footprint_Wing_Intensity_Label = new QLabel("Wings intensity");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Wing_Intensity_Label,5,0,Qt::AlignLeft);

		beam_Footprint_Wing_Intensity_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Wing_Intensity_SpinBox->setAccelerated(true);
			beam_Footprint_Wing_Intensity_SpinBox->setRange(0, 1);
			beam_Footprint_Wing_Intensity_SpinBox->setDecimals(5);
			beam_Footprint_Wing_Intensity_SpinBox->setValue(measurement.beam_Geometry.wings_Intensity);
			beam_Footprint_Wing_Intensity_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Footprint_Wing_Intensity_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Wing_Intensity_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH_BEAM);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Wing_Intensity_SpinBox,5,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Wing_Intensity_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Footprint_Log_Profile_Checkbox = new QCheckBox("Log");
			beam_Footprint_Log_Profile_Checkbox->setChecked(measurement.beam_Geometry.log_Profile_Plot);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Log_Profile_Checkbox,5,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		create_Beam_Plot();
		if(is_Independent)
		{
			footprint_GroupBox_Layout->addWidget(beam_Footprint_GroupBox,0,0/*,Qt::AlignLeft*/);
			footprint_GroupBox_Layout->addWidget(beam_Profile_CustomPlot,1,0);
		} else
		{
			footprint_GroupBox_Layout->addWidget(beam_Footprint_GroupBox,0,0/*,Qt::AlignLeft*/);
			footprint_GroupBox_Layout->addWidget(beam_Profile_CustomPlot,0,1);
		}
	}
	// sample
	{
		QGroupBox* sample_Footprint_GroupBox = new QGroupBox("Sample");
//			sample_Footprint_GroupBox->setFixedWidth(215);

		QGridLayout* sample_GroupBox_Layout = new QGridLayout(sample_Footprint_GroupBox);
			sample_GroupBox_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* sample_Size_Label = new QLabel("Size");
		sample_GroupBox_Layout->addWidget(sample_Size_Label,0,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sample_Size_SpinBox = new MyDoubleSpinBox;
			sample_Size_SpinBox->setAccelerated(true);
			sample_Size_SpinBox->setRange(0, MAX_DOUBLE);
			sample_Size_SpinBox->setDecimals(2);
			sample_Size_SpinBox->setSingleStep(0.1);
			sample_Size_SpinBox->setValue(measurement.sample_Geometry.size);
			sample_Size_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			sample_Size_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		sample_GroupBox_Layout->addWidget(sample_Size_SpinBox,0,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(sample_Size_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Sample_Size_Label = new QLabel("mm");
		sample_GroupBox_Layout->addWidget(mm_Sample_Size_Label,0,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* sample_X_Label = new QLabel("X-position");
		sample_GroupBox_Layout->addWidget(sample_X_Label,1,0,Qt::AlignLeft);

		sample_X_SpinBox = new MyDoubleSpinBox;
			sample_X_SpinBox->setAccelerated(true);
			sample_X_SpinBox->setRange(-100, MAX_DOUBLE);
			sample_X_SpinBox->setDecimals(2);
			sample_X_SpinBox->setSingleStep(0.1);
			sample_X_SpinBox->setValue(measurement.sample_Geometry.x_Position);
			sample_X_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			sample_X_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		sample_GroupBox_Layout->addWidget(sample_X_SpinBox,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(sample_X_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Sample_X_Label = new QLabel("mm");
		sample_GroupBox_Layout->addWidget(mm_Sample_X_Label,1,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* sample_Z_Label = new QLabel("Z-position");
		sample_GroupBox_Layout->addWidget(sample_Z_Label,2,0,Qt::AlignLeft);

		sample_Z_SpinBox = new MyDoubleSpinBox;
			sample_Z_SpinBox->setAccelerated(true);
			sample_Z_SpinBox->setRange(-100, MAX_DOUBLE);
			sample_Z_SpinBox->setDecimals(4);
			sample_Z_SpinBox->setValue(measurement.sample_Geometry.z_Position);
			sample_Z_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			sample_Z_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			sample_Z_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		sample_GroupBox_Layout->addWidget(sample_Z_SpinBox,2,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(sample_Z_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_Sample_Z_Label = new QLabel("mm");
		sample_GroupBox_Layout->addWidget(mm_Sample_Z_Label,2,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* sample_Curvature_Label = new QLabel("Curvature");
		sample_GroupBox_Layout->addWidget(sample_Curvature_Label,3,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		sample_Curvature_SpinBox = new MyDoubleSpinBox;
			sample_Curvature_SpinBox->setAccelerated(true);
			sample_Curvature_SpinBox->setRange(-1, 1);
			sample_Curvature_SpinBox->setDecimals(4);
			sample_Curvature_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			sample_Curvature_SpinBox->setValue(measurement.sample_Geometry.curvature);
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
		if(is_Independent)
		{
			footprint_GroupBox_Layout->addWidget(sample_Footprint_GroupBox,0,1/*,Qt::AlignLeft*/);
			footprint_GroupBox_Layout->addWidget(sample_Profile_CustomPlot,1,1);
		} else
		{
			footprint_GroupBox_Layout->addWidget(sample_Footprint_GroupBox,0,2/*,Qt::AlignLeft*/);
			footprint_GroupBox_Layout->addWidget(sample_Profile_CustomPlot,0,3);
		}
	}
}


void Common_Part::create_Beam_Plot()
{	
	beam_Profile_CustomPlot = new QCustomPlot;
	if(is_Independent)
	{
#ifdef __linux__
		beam_Profile_CustomPlot->setMinimumSize(240,170); // linux, 2560x1440
#else
		beam_Profile_CustomPlot->setMinimumSize(215,160); // windows, 1920x1080
#endif
	} else
	{
#ifdef __linux__
		beam_Profile_CustomPlot->setMinimumWidth(250);  // linux, 2560x1440
#else
		beam_Profile_CustomPlot->setMinimumWidth(210);  // windows, 1920x1080
#endif
	}
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

void Common_Part::apply_Lin_Log()
{
	if(measurement.beam_Geometry.log_Profile_Plot)
	{
		QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
		beam_Profile_CustomPlot->yAxis ->setScaleType(QCPAxis::stLogarithmic);
		beam_Profile_CustomPlot->yAxis->setTicker(logTicker);
		beam_Profile_CustomPlot->yAxis ->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
		beam_Profile_CustomPlot->yAxis ->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

		beam_Profile_CustomPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
		beam_Profile_CustomPlot->yAxis2->setTicker(logTicker);
		beam_Profile_CustomPlot->yAxis2->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
		beam_Profile_CustomPlot->yAxis2->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

	} else
	{
		QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);

		beam_Profile_CustomPlot->yAxis->setScaleType(QCPAxis::stLinear);
		beam_Profile_CustomPlot->yAxis->setTicker(linTicker);
		beam_Profile_CustomPlot->yAxis->setNumberFormat("g");
		beam_Profile_CustomPlot->yAxis->setNumberPrecision(4);

		beam_Profile_CustomPlot->yAxis2->setScaleType(QCPAxis::stLinear);
		beam_Profile_CustomPlot->yAxis2->setTicker(linTicker);
		beam_Profile_CustomPlot->yAxis2->setNumberFormat("g");
		beam_Profile_CustomPlot->yAxis2->setNumberPrecision(4);
	}
}

void Common_Part::plot_Beam_Profile()
{
	apply_Lin_Log();

	int data_Count = 301;
	double FWHM = measurement.beam_Geometry.size;
	double smoothing = measurement.beam_Geometry.smoothing;
	double wings_FW = measurement.beam_Geometry.wings_Full_Width;
	double wings_Intensity = measurement.beam_Geometry.wings_Intensity;
	double limit = 1.7*FWHM;
	if(wings_Intensity > DBL_EPSILON) {limit = max(limit, 1.2*wings_FW/2);}
	double delta = (2*limit)/(data_Count-1);

	QVector<QCPGraphData> data_To_Plot(data_Count);

	for (int i=0; i<data_Count; ++i)
	{
		double x = -limit + delta*i;
		data_To_Plot[i].key   = x;
		data_To_Plot[i].value = Global_Variables::beam_Profile_With_Wings(x, FWHM, smoothing, wings_FW, wings_Intensity);
	}
	beam_Profile_CustomPlot->graph()->data()->set(data_To_Plot);

	beam_Profile_CustomPlot->xAxis->setRange(-limit,limit);	
	if(measurement.beam_Geometry.log_Profile_Plot)
	{
		if(wings_Intensity < DBL_EPSILON)
		{
			beam_Profile_CustomPlot->yAxis->setRange(1E-3,1.05);
		} else
		{
			beam_Profile_CustomPlot->yAxis->setRange(0.3*wings_Intensity,1.05);
		}
	} else
	{
		beam_Profile_CustomPlot->yAxis->setRange(0,1.05);
	}
	beam_Profile_CustomPlot->replot();
}

void Common_Part::create_Sample_Plot()
{
	sample_Profile_CustomPlot = new QCustomPlot;	
	if(is_Independent)
	{
#ifdef __linux__
		sample_Profile_CustomPlot->setMinimumSize(240,170);	  // linux, 2560x1440
#else
		sample_Profile_CustomPlot->setMinimumSize(215,160);	  // windows, 1920x1080
#endif
	} else
	{
#ifdef __linux__
		sample_Profile_CustomPlot->setMinimumWidth(250);	// linux, 2560x1440
#else
		sample_Profile_CustomPlot->setMinimumWidth(210);	// windows, 1920x1080
#endif
	}
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

	connect(sample_Profile_CustomPlot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Common_Part::auto_Replot_Curve);
	connect(sample_Profile_CustomPlot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Common_Part::auto_Replot_Curve);

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

void Common_Part::plot_Sample()
{
	int curve_Point_Count = 203;

	double size = measurement.sample_Geometry.size;
	double z_Pos = measurement.sample_Geometry.z_Position;
	double x_Pos = measurement.sample_Geometry.x_Position;
	double limit = 0.7*size;
	double sag = 0;
	double delta = size/(curve_Point_Count-3);

	curve_Data.resize(curve_Point_Count);
	curve_Data.first() = QCPCurveData(0, -size/2 + x_Pos, sample_Profile_CustomPlot->yAxis->range().lower);
	if(abs(measurement.sample_Geometry.curvature)>CURVATURE_LIMIT)
	{
		double R = 1000./measurement.sample_Geometry.curvature;
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

void Common_Part::auto_Replot_Curve()
{
	double size  = measurement.sample_Geometry.size;
	double x_Pos = measurement.sample_Geometry.x_Position;
	double z_Pos = measurement.sample_Geometry.z_Position;

	curve_Data.first() = QCPCurveData(0,                  -size/2 + x_Pos, sample_Profile_CustomPlot->yAxis->range().lower);
	curve_Data.last()  = QCPCurveData(curve_Data.size()-1, size/2 + x_Pos, sample_Profile_CustomPlot->yAxis->range().lower);
	sample_Curve->data()->set(curve_Data, true);

	// show arrows
	double factor = (sample_Profile_CustomPlot->yAxis->range().upper-sample_Profile_CustomPlot->yAxis->range().lower) /
					(sample_Profile_CustomPlot->xAxis->range().upper-sample_Profile_CustomPlot->xAxis->range().lower);

	double limit = 0.5*size;
	arrow_Incident->start->setCoords(-limit, limit*factor+z_Pos*0.2);
	arrow_Incident->end->setCoords(-size/10, size/10*factor+z_Pos*0.2);

	arrow_Reflected->start->setCoords(size/10, size/10*factor+z_Pos*0.2);
	arrow_Reflected->end->setCoords(limit, limit*factor+z_Pos*0.2);

	sample_Profile_CustomPlot->replot();
}

void Common_Part::refresh_Curvature_Radius()
{
	QString R_Text;
	if(abs(measurement.sample_Geometry.curvature)>CURVATURE_LIMIT)
	{
		R_Text = QString::number(1./measurement.sample_Geometry.curvature) + " m";
	} else
	{
		R_Text = Infinity_Sym;
	}
	R_Curvature_Label->setText("Radius of curvature = " + R_Text);
}

void Common_Part::connecting()
{
	/// detector box
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] ||
		measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		// 1D detector type
		connect(detector_Type_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			measurement.detector_1D.detector_Type = detector_Type_ComboBox->currentText();
			detectors_Stack->setCurrentIndex(detector_Type_ComboBox->findText(measurement.detector_1D.detector_Type));
			global_Multilayer_Approach->global_Recalculate();
		});
		// 1D detector distance
		connect(detector_Distance_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			measurement.detector_1D.distance_To_Sample = detector_Distance_SpinBox->value();
			global_Multilayer_Approach->global_Recalculate();
		});
		// binning
		if(!is_Independent)
		{
			connect(use_Binning_Checkbox,&QCheckBox::toggled, this, [=]
			{
				measurement.detector_1D.use_Binning = use_Binning_Checkbox->isChecked();

				binning_Factor_Spinbox->setEnabled(measurement.detector_1D.use_Binning);

				target_Curve_Editor->binning_Changed();
				global_Multilayer_Approach->global_Recalculate();
			});
			connect(binning_Factor_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
			{
				measurement.detector_1D.binning_Factor = binning_Factor_Spinbox->value();

				target_Curve_Editor->binning_Changed();
				global_Multilayer_Approach->global_Recalculate();
			});
		}
		// slit length
		connect(finite_Slit_Length_Checkbox,&QCheckBox::toggled, this, [=]
		{
			measurement.detector_1D.finite_Slit = finite_Slit_Length_Checkbox->isChecked();

			slit_Length_SpinBox->setEnabled(measurement.detector_1D.finite_Slit);
			slit_Length_Units_Label->setEnabled(measurement.detector_1D.finite_Slit);

			global_Multilayer_Approach->global_Recalculate();
		});
		connect(slit_Length_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			measurement.detector_1D.slit_Length = slit_Length_SpinBox->value();
			global_Multilayer_Approach->global_Recalculate();
		});
		// 1D slit width
		connect(slit_Width_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			measurement.detector_1D.slit_Width = slit_Width_SpinBox->value();
			global_Multilayer_Approach->global_Recalculate();
		});
		// 1D crystal resolution
		connect(crystal_Resolution_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			double coeff = angle_Coefficients_Map.value(angular_Units);
			measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution = crystal_Resolution_SpinBox->value()*coeff;
			global_Multilayer_Approach->global_Recalculate();
		});
		// resolution function
        connect(resolution_Function_ComboBox, &QComboBox::currentTextChanged, this, [=]
        {
            measurement.detector_1D.detector_Theta_Resolution.distribution_Function = resolution_Function_ComboBox->currentText();
            global_Multilayer_Approach->global_Recalculate();
        });
        // beam distribution button
        auto lambda = [=]() {
//            qInfo() << "crystal\n";
//            qInfo() << "slit\n";

//            Distribution_Editor* distribution_Editor = new Distribution_Editor(measurement,
//                                                                               independent_Curve->spectral_Units,
//                                                                               independent_Curve->angular_Units,
//                                                                               spectral_Width_SpinBox,
//                                                                               angular_Divergence_SpinBox);
//            distribution_Editor->show();
        };
        connect(setup_Beam_Distribution_Button_crystal, &QPushButton::clicked, lambda);
        connect(setup_Beam_Distribution_Button_slit, &QPushButton::clicked, lambda);
	}
	if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		connect(set_Pixel_Size_Checkbox,&QCheckBox::toggled, this, [=]
		{
			measurement.detector_2D.set_Pixel_Size = set_Pixel_Size_Checkbox->isChecked();

			detector_Type_Label->setEnabled(measurement.detector_2D.set_Pixel_Size);
			detector_Type_ComboBox->setEnabled(measurement.detector_2D.set_Pixel_Size);
			detectors_Stack->setEnabled(measurement.detector_2D.set_Pixel_Size);

			global_Multilayer_Approach->global_Recalculate();
		});
		// 2D detector type
		connect(detector_Type_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			measurement.detector_2D.detector_Type = detector_Type_ComboBox->currentText();
			detectors_Stack->setCurrentIndex(detector_Type_ComboBox->findText(measurement.detector_2D.detector_Type));
			global_Multilayer_Approach->global_Recalculate();
		});
		// 2D detector distance
		connect(detector_Distance_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			measurement.detector_2D.distance_To_Sample = detector_Distance_SpinBox->value();
			global_Multilayer_Approach->global_Recalculate();
		});		
		// binning
		if(!is_Independent)
		{
			connect(use_Binning_Checkbox,&QCheckBox::toggled, this, [=]
			{
				measurement.detector_2D.use_Binning = use_Binning_Checkbox->isChecked();

				theta_Binning_Factor_Label->setEnabled(measurement.detector_2D.use_Binning);
				theta_Binning_Factor_Spinbox->setEnabled(measurement.detector_2D.use_Binning);
				phi_Binning_Factor_Label->setEnabled(measurement.detector_2D.use_Binning);
				phi_Binning_Factor_Spinbox->setEnabled(measurement.detector_2D.use_Binning);

				target_Curve_Editor->binning_Changed();
				global_Multilayer_Approach->global_Recalculate();
			});
			connect(theta_Binning_Factor_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
			{
				measurement.detector_2D.theta_Binning_Factor = theta_Binning_Factor_Spinbox->value();

				target_Curve_Editor->binning_Changed();
				global_Multilayer_Approach->global_Recalculate();
			});
			connect(phi_Binning_Factor_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
			{
				measurement.detector_2D.phi_Binning_Factor = phi_Binning_Factor_Spinbox->value();

				target_Curve_Editor->binning_Changed();
				global_Multilayer_Approach->global_Recalculate();
			});
		}
		// 2D pixel polar height
		connect(pixel_Polar_Height_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			measurement.detector_2D.pixel_Polar_Height = pixel_Polar_Height_SpinBox->value();
			global_Multilayer_Approach->global_Recalculate();
		});
		// 2D pixel azimuthal width
		connect(pixel_Azimuthal_Width_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			measurement.detector_2D.pixel_Azimuthal_Width = pixel_Azimuthal_Width_SpinBox->value();
			global_Multilayer_Approach->global_Recalculate();
		});
		// 2D theta resolution
		connect(theta_Resolution_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			double coeff = angle_Coefficients_Map.value(angular_Units);
			measurement.detector_2D.detector_Theta_Resolution.FWHM_distribution = theta_Resolution_SpinBox->value()*coeff;
			global_Multilayer_Approach->global_Recalculate();
		});
		// 2D phi resolution
		connect(phi_Resolution_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			double coeff = angle_Coefficients_Map.value(angular_Units);
			measurement.detector_2D.detector_Phi_Resolution.FWHM_distribution = phi_Resolution_SpinBox->value()*coeff;
			global_Multilayer_Approach->global_Recalculate();
		});
		// resolution function
		connect(resolution_Function_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			measurement.detector_2D.detector_Theta_Resolution.distribution_Function = resolution_Function_ComboBox->currentText();
			measurement.detector_2D.detector_Phi_Resolution.distribution_Function = resolution_Function_ComboBox->currentText();
			global_Multilayer_Approach->global_Recalculate();
		});
	}


	/// footprint & distortion groupbox
	// beam width
	connect(beam_Footprint_Width_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.beam_Geometry.size = beam_Footprint_Width_SpinBox->value();

		plot_Beam_Profile();
		global_Multilayer_Approach->global_Recalculate();
	});
	// beam left smoothing
	connect(beam_Footprint_Left_Smoothing_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.beam_Geometry.smoothing = beam_Footprint_Left_Smoothing_SpinBox->value();
	/// "asymmetric" interface
//		if(!measurement.beam_Geometry.asymmetric_Smoothing)	{
//			measurement.beam_Geometry.right_Smoothing = measurement.beam_Geometry.smoothing;
//		}
		plot_Beam_Profile();
		global_Multilayer_Approach->global_Recalculate();
	});
	/// "asymmetric" interface
	// beam asimmetric smoothing
//	connect(beam_Footprint_Right_Smoothing_Checkbox, &QCheckBox::clicked, this, [=]
//	{
//		measurement.beam_Geometry.asymmetric_Smoothing = beam_Footprint_Right_Smoothing_Checkbox->isChecked();
//		beam_Footprint_Right_Smoothing_SpinBox->setEnabled(measurement.beam_Geometry.asymmetric_Smoothing);
//		if(!measurement.beam_Geometry.asymmetric_Smoothing)	{
//			measurement.beam_Geometry.right_Smoothing = measurement.beam_Geometry.smoothing;
//		}
//		plot_Beam_Profile();
//		global_Multilayer_Approach->global_Recalculate();
//	});
	// beam right smoothing
//	connect(beam_Footprint_Right_Smoothing_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
//	{
//		measurement.beam_Geometry.right_Smoothing = beam_Footprint_Right_Smoothing_SpinBox->value();

//		plot_Beam_Profile();
//		global_Multilayer_Approach->global_Recalculate();
//	});

	// beam left wing
	connect(beam_Footprint_Left_Wing_Width_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.beam_Geometry.wings_Full_Width = beam_Footprint_Left_Wing_Width_SpinBox->value();
		/// "asymmetric" interface
//		if(!measurement.beam_Geometry.asymmetric_Wing_Width)	{
//			measurement.beam_Geometry.right_Wing_Width = measurement.beam_Geometry.full_Wings_Width;
//		}
		plot_Beam_Profile();
		global_Multilayer_Approach->global_Recalculate();
	});
	/// "asymmetric" interface
	// beam asimmetric wings
//	connect(beam_Footprint_Right_Wing_Width_Checkbox, &QCheckBox::clicked, this, [=]
//	{
//		measurement.beam_Geometry.asymmetric_Wing_Width = beam_Footprint_Right_Wing_Width_Checkbox->isChecked();
//		beam_Footprint_Right_Wing_Width_SpinBox->setEnabled(measurement.beam_Geometry.asymmetric_Wing_Width);
//		if(!measurement.beam_Geometry.asymmetric_Wing_Width)	{
//			measurement.beam_Geometry.right_Wing_Width = measurement.beam_Geometry.full_Wings_Width;
//		}
//		plot_Beam_Profile();
//		global_Multilayer_Approach->global_Recalculate();
//	});
	// beam right wing
//	connect(beam_Footprint_Right_Wing_Width_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
//	{
//		measurement.beam_Geometry.right_Wing_Width = beam_Footprint_Right_Wing_Width_SpinBox->value();

//		plot_Beam_Profile();
//		global_Multilayer_Approach->global_Recalculate();
//	});

	// beam wings intensity
	connect(beam_Footprint_Wing_Intensity_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.beam_Geometry.wings_Intensity = beam_Footprint_Wing_Intensity_SpinBox->value();

		plot_Beam_Profile();
		global_Multilayer_Approach->global_Recalculate();
	});
	// beam lateral width
	connect(beam_Footprint_Lateral_Width_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.beam_Geometry.lateral_Width = beam_Footprint_Lateral_Width_SpinBox->value();

		global_Multilayer_Approach->global_Recalculate();
	});
	// beam log profile plot
	connect(beam_Footprint_Log_Profile_Checkbox, &QCheckBox::clicked, this, [=]
	{
		measurement.beam_Geometry.log_Profile_Plot = beam_Footprint_Log_Profile_Checkbox->isChecked();

		plot_Beam_Profile();
	});


	// sample size
	connect(sample_Size_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.sample_Geometry.size = sample_Size_SpinBox->value();

		plot_Sample();
		global_Multilayer_Approach->global_Recalculate();
	});
	// sample x-position
	connect(sample_X_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.sample_Geometry.x_Position = sample_X_SpinBox->value();

		plot_Sample();
		global_Multilayer_Approach->global_Recalculate();
	});
	// sample z-position
	connect(sample_Z_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.sample_Geometry.z_Position = sample_Z_SpinBox->value();

		plot_Sample();
		global_Multilayer_Approach->global_Recalculate();
	});
	// sample curvature
	connect(sample_Curvature_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.sample_Geometry.curvature = sample_Curvature_SpinBox->value();

		refresh_Curvature_Radius();
		plot_Sample();
		global_Multilayer_Approach->global_Recalculate();
	});
}
