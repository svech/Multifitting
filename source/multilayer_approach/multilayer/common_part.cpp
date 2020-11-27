#include "common_part.h"

Common_Part::Common_Part(Independent_Curve* independent_Curve, Target_Curve* target_Curve, bool is_Independent, QWidget *parent) :
	measurement   (is_Independent ? independent_Curve->measurement    : target_Curve->measurement),
	angular_Units (is_Independent ? independent_Curve->angular_Units  : target_Curve->angular_Units),
	spectral_Units(is_Independent ? independent_Curve->spectral_Units : target_Curve->spectral_Units),
	is_Independent(is_Independent),
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
		create_Detector_GroupBox();
	}
	if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		create_2D_Detector_GroupBox();
	}
	create_Footptint_GroupBox();

	connecting();
}

void Common_Part::create_Detector_GroupBox()
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

		QLabel* detector_Type_Label = new QLabel("Detector type");
		detector_Type_Layout->addWidget(detector_Type_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detector_Type_ComboBox = new QComboBox;
			detector_Type_ComboBox->addItem(detectors[Slit]);
			detector_Type_ComboBox->addItem(detectors[Crystal]);
			detector_Type_ComboBox->setCurrentText(measurement.detector_1D.detector_Type);
			detector_Type_ComboBox->setFixedWidth(100);
		detector_Type_Layout->addWidget(detector_Type_ComboBox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detectors_Stack = new QStackedWidget;
		if(is_Independent)
		{
			detector_GroupBox_Layout->addWidget(detectors_Stack,1,0);
		} else
		{
			detector_GroupBox_Layout->addWidget(detectors_Stack,0,1);
		}
	}
	// slit
	{
		QWidget* slit_Page = new QWidget;
		detectors_Stack->addWidget(slit_Page);
		QHBoxLayout* slit_Layout = new QHBoxLayout(slit_Page);
			slit_Layout->setContentsMargins(0,0,0,0);
		slit_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* slit_Width_Label = new QLabel("    Slit width");
		slit_Layout->addWidget(slit_Width_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		slit_Width_SpinBox = new MyDoubleSpinBox;
			slit_Width_SpinBox->setAccelerated(true);
			slit_Width_SpinBox->setRange(0, MAX_DOUBLE);
			slit_Width_SpinBox->setDecimals(2);
			slit_Width_SpinBox->setValue(measurement.detector_1D.slit_Width);
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
			slit_Distance_SpinBox->setValue(measurement.detector_1D.distance_To_Sample);
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
			crystal_Layout->setContentsMargins(0,0,0,0);
		crystal_Layout->setAlignment(Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* crystal_Resolution_Label = new QLabel("    Angular resolution, FWHM, "+Delta_Big_Sym+Theta_Sym);
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
			crystal_Resolution_SpinBox->setProperty(min_Size_Property,TARGET_LINE_AT_FIXED_WIDTH);
		crystal_Layout->addWidget(crystal_Resolution_SpinBox,0,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(crystal_Resolution_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		crystal_Resolution_Units_Label = new QLabel(angular_Units);
		crystal_Layout->addWidget(crystal_Resolution_Units_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* crystal_Resolution_Function_Label = new QLabel("       Function");
		crystal_Layout->addWidget(crystal_Resolution_Function_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		resolution_Function_ComboBox = new QComboBox;
//		if( measurement.measurement_Type != measurement_Types[Rocking_Curve] &&
//			measurement.measurement_Type != measurement_Types[Offset_Scan])
//		{
//			resolution_Function_ComboBox->addItems(distributions);
//		} else
//		{
			resolution_Function_ComboBox->addItems(distributions_No_Lorentz);
//		}
			resolution_Function_ComboBox->setCurrentText(measurement.detector_1D.detector_Theta_Resolution.distribution_Function);
			resolution_Function_ComboBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
		crystal_Layout->addWidget(resolution_Function_ComboBox,0,Qt::AlignLeft);
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

		QLabel* detector_Type_Label = new QLabel("Detector type");
		detector_Type_Layout->addWidget(detector_Type_Label,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detector_Type_ComboBox = new QComboBox;
			detector_Type_ComboBox->addItem(detectors[Spherical]);
			//detector_Type_ComboBox->addItem(detectors[Rectangular]);
			detector_Type_ComboBox->setCurrentText(measurement.detector_2D.detector_Type);
			detector_Type_ComboBox->setFixedWidth(130);
		detector_Type_Layout->addWidget(detector_Type_ComboBox,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		detectors_Stack = new QStackedWidget;
		if(is_Independent)
		{
			detector_GroupBox_Layout->addWidget(detectors_Stack,1,0);
		} else
		{
			detector_GroupBox_Layout->addWidget(detectors_Stack,0,1);
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
			QLabel* spherical_Theta_Resolution_Label = new QLabel("    Angular resolution, FWHM, "+Delta_Big_Sym+Theta_Sym);
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
				theta_Resolution_SpinBox->setProperty(min_Size_Property,TARGET_LINE_AT_FIXED_WIDTH);
			spherical_Layout->addWidget(theta_Resolution_SpinBox,0,1,Qt::AlignLeft);
			Global_Variables::resize_Line_Edit(theta_Resolution_SpinBox);

			// - - - - - - - - - - - - - - - - - - - - - - - - - - -

			theta_Resolution_Units_Label = new QLabel(angular_Units);
			spherical_Layout->addWidget(theta_Resolution_Units_Label,0,2,Qt::AlignLeft);
		}
		// phi
		{
			QLabel* spherical_Phi_Resolution_Label = new QLabel("    Angular resolution, FWHM, "+Delta_Big_Sym+Phi_Sym);
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
				phi_Resolution_SpinBox->setProperty(min_Size_Property,TARGET_LINE_AT_FIXED_WIDTH);
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
//			resolution_Function_ComboBox->addItems(distributions);
			resolution_Function_ComboBox->addItems(distributions_No_Lorentz);
			resolution_Function_ComboBox->setCurrentText(measurement.detector_2D.detector_Theta_Resolution.distribution_Function); // detector_Theta_Resolution.distribution_Function = detector_Phi_Resolution.distribution_Function
			resolution_Function_ComboBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
		spherical_Layout->addWidget(resolution_Function_ComboBox,0,4,2,1,Qt::AlignLeft);
	}

	if(measurement.detector_2D.detector_Type == detectors[Spherical])	detectors_Stack->setCurrentIndex(0);
	if(measurement.detector_2D.detector_Type == detectors[Rectangular]) detectors_Stack->setCurrentIndex(1);
}

void Common_Part::create_Footptint_GroupBox()
{
	QGroupBox* footprint_GroupBox = new QGroupBox("Footptint and distortion");
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

		QLabel* beam_Footprint_Width_Label = new QLabel("Beam width, FWHM");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Width_Label,0,0,Qt::AlignLeft);

		beam_Footprint_Width_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Width_SpinBox->setAccelerated(true);
			beam_Footprint_Width_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Footprint_Width_SpinBox->setDecimals(3);
			beam_Footprint_Width_SpinBox->setValue(measurement.beam_Geometry.size);
			beam_Footprint_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Footprint_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Width_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Width_SpinBox,0,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Width_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_beam_Footprint_Width_Label = new QLabel("mm");
		beam_Footprint_GroupBox_Layout->addWidget(mm_beam_Footprint_Width_Label,0,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* beam_Footprint_Lateral_Width_Label = new QLabel("Beam lateral width");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Lateral_Width_Label,1,0,Qt::AlignLeft);

		beam_Footprint_Lateral_Width_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Lateral_Width_SpinBox->setAccelerated(true);
			beam_Footprint_Lateral_Width_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Footprint_Lateral_Width_SpinBox->setDecimals(3);
			beam_Footprint_Lateral_Width_SpinBox->setValue(measurement.beam_Geometry.lateral_Width);
			beam_Footprint_Lateral_Width_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			beam_Footprint_Lateral_Width_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Lateral_Width_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Lateral_Width_SpinBox,1,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Lateral_Width_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* mm_beam_Footprint_Lateral_Width_Label = new QLabel("mm");
		beam_Footprint_GroupBox_Layout->addWidget(mm_beam_Footprint_Lateral_Width_Label,1,2,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		QLabel* beam_Footprint_Shape_Label = new QLabel("Beam profile smoothing");
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Shape_Label,2,0,Qt::AlignLeft);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		beam_Footprint_Shape_SpinBox = new MyDoubleSpinBox;
			beam_Footprint_Shape_SpinBox->setAccelerated(true);
			beam_Footprint_Shape_SpinBox->setRange(0, 1);
			beam_Footprint_Shape_SpinBox->setDecimals(3);
			beam_Footprint_Shape_SpinBox->setValue(measurement.beam_Geometry.smoothing);
			beam_Footprint_Shape_SpinBox->setSingleStep(0.01);
			beam_Footprint_Shape_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Footprint_Shape_SpinBox->setProperty(min_Size_Property,TARGET_LINE_EDIT_WIDTH);
		beam_Footprint_GroupBox_Layout->addWidget(beam_Footprint_Shape_SpinBox,2,1,Qt::AlignLeft);
		Global_Variables::resize_Line_Edit(beam_Footprint_Shape_SpinBox);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - -

		create_Beam_Plot();
		if(is_Independent)
		{
			footprint_GroupBox_Layout->addWidget(beam_Footprint_GroupBox,0,0,Qt::AlignLeft);
			footprint_GroupBox_Layout->addWidget(beam_Profile_CustomPlot,1,0);
		} else
		{
			footprint_GroupBox_Layout->addWidget(beam_Footprint_GroupBox,0,0,Qt::AlignLeft);
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

		QLabel* sample_Size_Label = new QLabel("Sample size");
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

		QLabel* sample_X_Label = new QLabel("Sample x-position");
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

		QLabel* sample_Z_Label = new QLabel("Sample z-position");
		sample_GroupBox_Layout->addWidget(sample_Z_Label,2,0,Qt::AlignLeft);

		sample_Z_SpinBox = new MyDoubleSpinBox;
			sample_Z_SpinBox->setAccelerated(true);
			sample_Z_SpinBox->setRange(-100, MAX_DOUBLE);
			sample_Z_SpinBox->setDecimals(3);
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

		QLabel* sample_Curvature_Label = new QLabel("Sample curvature");
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
			footprint_GroupBox_Layout->addWidget(sample_Footprint_GroupBox,0,1,Qt::AlignLeft);
			footprint_GroupBox_Layout->addWidget(sample_Profile_CustomPlot,1,1);
		} else
		{
			footprint_GroupBox_Layout->addWidget(sample_Footprint_GroupBox,0,2,Qt::AlignLeft);
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

void Common_Part::plot_Beam_Profile()
{
	int data_Count = 301;
	double FWHM = measurement.beam_Geometry.size;
	double sigma = FWHM*measurement.beam_Geometry.smoothing;
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
	arrow_Incident->start->setCoords(-limit, limit*factor+z_Pos*0.8);
	arrow_Incident->end->setCoords(-size/10, size/10*factor+z_Pos*0.8);

	arrow_Reflected->start->setCoords(size/10, size/10*factor+z_Pos*0.8);
	arrow_Reflected->end->setCoords(limit, limit*factor+z_Pos*0.8);

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

	// 1D slit width
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] ||
		measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		// detector type
		connect(detector_Type_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			measurement.detector_1D.detector_Type = detector_Type_ComboBox->currentText();
			detectors_Stack->setCurrentIndex(detector_Type_ComboBox->findText(measurement.detector_1D.detector_Type));

			global_Multilayer_Approach->global_Recalculate();
		});
		// 1D slit width
		connect(slit_Width_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			measurement.detector_1D.slit_Width = slit_Width_SpinBox->value();

			global_Multilayer_Approach->global_Recalculate();
		});
		// 1D slit distance
		connect(slit_Distance_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			measurement.detector_1D.distance_To_Sample = slit_Distance_SpinBox->value();

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
	}
	if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		// detector type
		connect(detector_Type_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			measurement.detector_2D.detector_Type = detector_Type_ComboBox->currentText();
			detectors_Stack->setCurrentIndex(detector_Type_ComboBox->findText(measurement.detector_2D.detector_Type));

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
	// beam lateral width
	connect(beam_Footprint_Lateral_Width_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.beam_Geometry.lateral_Width = beam_Footprint_Lateral_Width_SpinBox->value();

		global_Multilayer_Approach->global_Recalculate();
	});
	// beam shape
	connect(beam_Footprint_Shape_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		measurement.beam_Geometry.smoothing = beam_Footprint_Shape_SpinBox->value();

		plot_Beam_Profile();
		global_Multilayer_Approach->global_Recalculate();
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
