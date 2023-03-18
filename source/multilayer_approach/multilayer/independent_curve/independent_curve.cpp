#include "independent_curve.h"

Independent_Curve::Independent_Curve(QWidget *parent) :
	measurement(item_Type_Measurement),
	QWidget(parent)
{
	// we can initialize values here, because withouts opening Editor there is no measurement type
	{
		plot_Options.color=QColor(0, 0, 255);
		plot_Options.scatter_Shape = QCPScatterStyle::ssDisc;
		plot_Options.scatter_Size=0;
		plot_Options.thickness=2;

		angular_Units = angle_Units_List[degree];
		spectral_Units = wavelength_Units_List[angstrom];
	}
	create_Main_Layout();
}

void Independent_Curve::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
		main_Layout->setAlignment(Qt::AlignLeft);

	setup_Button = new QPushButton("Set up");
		setup_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		setup_Button->setFixedWidth(60);
	main_Layout->addWidget(setup_Button);

	description_Label  = new QLabel("<no description>");
	main_Layout->addWidget(description_Label);
}

QString Independent_Curve::enlarge_Tab_Name()
{
	if(calc_Functions.check_Enabled)
	{
		QString suffix = "  ";
		if(calc_Functions.check_Reflectance)	suffix = suffix+symbol_R+",";
		if(calc_Functions.check_Transmittance)  suffix = suffix+symbol_T+",";
		if(calc_Functions.check_Absorptance)	suffix = suffix+symbol_A+",";
		if(calc_Functions.check_Field)			suffix = suffix+symbol_F+",";
		if(calc_Functions.check_Joule)			suffix = suffix+symbol_J+",";
		if(calc_Functions.check_Scattering)		suffix = suffix+symbol_S+",";
		if(calc_Functions.check_GISAS)			suffix = suffix+symbol_G+",";
		return suffix.remove(-1,1);
	} else
	{
		return "";
	}
}

void Independent_Curve::refresh_Description_Label()
{
	if(measurement.measurement_Type != no_Measurement_Type)
	{
		QString lambda_Energy;
		if(	spectral_Units == wavelength_Units_List[angstrom] ||
			spectral_Units == wavelength_Units_List[nm]		  )
		{
			lambda_Energy = Lambda_Sym;
		} else
		{
			lambda_Energy = "E";
		}

		double spectral_Coeff = wavelength_Coefficients_Map.value(spectral_Units);
		double angular_Coeff = angle_Coefficients_Map.value(angular_Units);

		if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(measurement.beam_Theta_0_Angle.independent.min/angular_Coeff) +
						"-" + Locale.toString(measurement.beam_Theta_0_Angle.independent.max/angular_Coeff) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units;
			}
			if(measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
				label_Text =  measurement.measurement_Type + "; " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.independent.min)/spectral_Coeff) +
						"-" + Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.independent.max)/spectral_Coeff) +
							  " " + spectral_Units + "; " + "at " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Detector_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Detector_Polar_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " + Theta_Sym + "=" +
							  Locale.toString(measurement.detector_Theta_Angle.independent.min/angular_Coeff) +
						"-" + Locale.toString(measurement.detector_Theta_Angle.independent.max/angular_Coeff) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + " and " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Rocking_Curve] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(measurement.beam_Theta_0_Angle.independent.min/angular_Coeff) +
						"-" + Locale.toString(measurement.beam_Theta_0_Angle.independent.max/angular_Coeff) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + ", specular=" +
							  Locale.toString(measurement.beam_Theta_0_Specular_Position/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
			if(measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " + Delta_Big_Sym + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString((measurement.beam_Theta_0_Angle.independent.min-measurement.beam_Theta_0_Specular_Position)/angular_Coeff) +
						"-" + Locale.toString((measurement.beam_Theta_0_Angle.independent.max-measurement.beam_Theta_0_Specular_Position)/angular_Coeff) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + ", specular=" +
							  Locale.toString(measurement.beam_Theta_0_Specular_Position/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Offset_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(measurement.beam_Theta_0_Angle.independent.min/angular_Coeff) +
						"-" + Locale.toString(measurement.beam_Theta_0_Angle.independent.max/angular_Coeff) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + ", offset =" +
							  Locale.toString(measurement.detector_Theta_Offset/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}
		if(	measurement.measurement_Type == measurement_Types[GISAS_Map] )
		{
			if(measurement.argument_Type == argument_Types[Detector_Theta_Phi_Angles])
			{
				label_Text =  measurement.measurement_Type + "; " + Theta_Sym + "=" +
							  Locale.toString(measurement.detector_Theta_Angle.independent.min/angular_Coeff) +
						"-" + Locale.toString(measurement.detector_Theta_Angle.independent.max/angular_Coeff) +
							  " " + angular_Units + "; " +			Phi_Sym + "=" +
							  Locale.toString(measurement.detector_Phi_Angle.independent.min/angular_Coeff) +
						"-" + Locale.toString(measurement.detector_Phi_Angle.independent.max/angular_Coeff) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + " and " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}
		description_Label->setText(label_Text);
	} else
	{
		label_Text = "<no description>";
		description_Label->setText(label_Text);
	}
}

void Independent_Curve::calc_Independent_cos2_k(double angle_Shift, double lambda_Shift)
{
	// if parameter is active, num points >=3

	if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// beam angle_cos_sin_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.beam_Theta_0_Cos_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.beam_Theta_0_Sin_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.beam_Theta_0_Cos2_Vec. resize(measurement.beam_Theta_0_Angle.independent.num_Points);

				double angle_Step = (measurement.beam_Theta_0_Angle.independent.max - measurement.beam_Theta_0_Angle.independent.min) / (measurement.beam_Theta_0_Angle.independent.num_Points - 1);
				double angle_Temp =  measurement.beam_Theta_0_Angle.independent.min + angle_Shift;
				for(int i=0; i<measurement.beam_Theta_0_Angle.independent.num_Points; ++i)
				{
					measurement.beam_Theta_0_Cos_Vec [i] = abs(cos(qDegreesToRadians(angle_Temp)));
					measurement.beam_Theta_0_Sin_Vec [i] = abs(sin(qDegreesToRadians(angle_Temp)));
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
					measurement.beam_Theta_0_Angle_Vec[i] = angle_Temp;
					angle_Temp += angle_Step;
				}

			}
			// k : single_value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
		if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
		{
			// beam angle_cos_sin_cos2 : single value
			{
				measurement.beam_Theta_0_Angle_Value = measurement.beam_Theta_0_Angle.value + angle_Shift;
				measurement.beam_Theta_0_Cos_Value = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Sin_Value = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Cos2_Value = pow(measurement.beam_Theta_0_Cos_Value,2);
			}
			// k : vector
			{
				measurement.lambda_Vec.resize(measurement.wavelength.independent.num_Points);
				measurement.k_Vec.resize(measurement.wavelength.independent.num_Points);

				double wave_Energy_Step = (measurement.wavelength.independent.max - measurement.wavelength.independent.min) / (measurement.wavelength.independent.num_Points - 1);
				double wave_Energy = measurement.wavelength.independent.min + lambda_Shift;
				for(int i=0; i<measurement.wavelength.independent.num_Points; ++i)
				{
					measurement.lambda_Vec[i] = max(wave_Energy, DBL_EPSILON);
					measurement.k_Vec[i] = 2*M_PI/measurement.lambda_Vec[i];
					wave_Energy += wave_Energy_Step;
				}
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Detector_Polar_Angle] )
		{
			// detector angle_cos_sin_cos2 : vector
			{
				measurement.detector_Theta_Angle_Vec.resize(measurement.detector_Theta_Angle.independent.num_Points);
				measurement.detector_Theta_Cos_Vec.  resize(measurement.detector_Theta_Angle.independent.num_Points);
				measurement.detector_Theta_Sin_Vec.  resize(measurement.detector_Theta_Angle.independent.num_Points);
				measurement.detector_Theta_Cos2_Vec. resize(measurement.detector_Theta_Angle.independent.num_Points);

				double angle_Step = (measurement.detector_Theta_Angle.independent.max - measurement.detector_Theta_Angle.independent.min) / (measurement.detector_Theta_Angle.independent.num_Points - 1);
				double angle_Temp =  measurement.detector_Theta_Angle.independent.min;
				for(int i=0; i<measurement.detector_Theta_Angle.independent.num_Points; ++i)
				{
					measurement.detector_Theta_Angle_Vec[i] = angle_Temp;
					measurement.detector_Theta_Cos_Vec [i] = cos(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Sin_Vec [i] = sin(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Cos2_Vec[i] = pow(measurement.detector_Theta_Cos_Vec[i],2);
					angle_Temp += angle_Step;
				}
			}
			// beam angle_cos_sin_cos2 : single value
			{
				measurement.beam_Theta_0_Angle_Value = measurement.beam_Theta_0_Angle.value + angle_Shift;
				measurement.beam_Theta_0_Cos_Value  = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Sin_Value  = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Cos2_Value = pow(measurement.beam_Theta_0_Cos_Value,2);

				// for convenience of calculation
				measurement.beam_Theta_0_Cos_Vec.resize(measurement.detector_Theta_Angle.independent.num_Points);
				measurement.beam_Theta_0_Sin_Vec.resize(measurement.detector_Theta_Angle.independent.num_Points);
				for(int i=0; i<measurement.detector_Theta_Angle.independent.num_Points; ++i)
				{
					measurement.beam_Theta_0_Cos_Vec [i] = measurement.beam_Theta_0_Cos_Value;
					measurement.beam_Theta_0_Sin_Vec [i] = measurement.beam_Theta_0_Sin_Value;
				}
			}
			// k : single value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		/// common part
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] ||
			measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle] )
		{
			// beam angle_cos_sin_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.beam_Theta_0_Cos_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.beam_Theta_0_Sin_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.beam_Theta_0_Cos2_Vec. resize(measurement.beam_Theta_0_Angle.independent.num_Points);

				measurement.beam_Theta_0_Angle.independent.max = min(2*measurement.beam_Theta_0_Specular_Position, measurement.beam_Theta_0_Angle.independent.max);
				measurement.beam_Theta_0_Angle.independent.min = min(2*measurement.beam_Theta_0_Specular_Position, measurement.beam_Theta_0_Angle.independent.min);

				double angle_Step = (measurement.beam_Theta_0_Angle.independent.max - measurement.beam_Theta_0_Angle.independent.min) / (measurement.beam_Theta_0_Angle.independent.num_Points - 1);
				double angle_Temp =  measurement.beam_Theta_0_Angle.independent.min + angle_Shift;
				for(int i=0; i<measurement.beam_Theta_0_Angle.independent.num_Points; ++i)
				{
					measurement.beam_Theta_0_Cos_Vec [i] = abs(cos(qDegreesToRadians(angle_Temp)));
					measurement.beam_Theta_0_Sin_Vec [i] = abs(sin(qDegreesToRadians(angle_Temp)));
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
					measurement.beam_Theta_0_Angle_Vec[i] = angle_Temp;
					angle_Temp += angle_Step;
				}
			}
			// detector angle_cos_sin_cos2 : vector, depends on beam
			{
				measurement.detector_Theta_Angle_Vec.resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.detector_Theta_Cos_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.detector_Theta_Sin_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.detector_Theta_Cos2_Vec. resize(measurement.beam_Theta_0_Angle.independent.num_Points);

				for(int i=0; i<measurement.beam_Theta_0_Angle.independent.num_Points; ++i)
				{
					double angle_Temp = 2*measurement.beam_Theta_0_Specular_Position - measurement.beam_Theta_0_Angle_Vec[i] + angle_Shift; // remains unchanged, independent of angle_Shift
					measurement.detector_Theta_Angle_Vec[i] = angle_Temp;
					measurement.detector_Theta_Cos_Vec [i] = cos(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Sin_Vec [i] = sin(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Cos2_Vec[i] = pow(measurement.detector_Theta_Cos_Vec[i],2);
				}
			}
			// k : single value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// beam angle_cos_sin_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.beam_Theta_0_Cos_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.beam_Theta_0_Sin_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.beam_Theta_0_Cos2_Vec. resize(measurement.beam_Theta_0_Angle.independent.num_Points);

				double angle_Step = (measurement.beam_Theta_0_Angle.independent.max - measurement.beam_Theta_0_Angle.independent.min) / (measurement.beam_Theta_0_Angle.independent.num_Points - 1);
				double angle_Temp =  measurement.beam_Theta_0_Angle.independent.min + angle_Shift;
				for(int i=0; i<measurement.beam_Theta_0_Angle.independent.num_Points; ++i)
				{
					measurement.beam_Theta_0_Cos_Vec [i] = abs(cos(qDegreesToRadians(angle_Temp)));
					measurement.beam_Theta_0_Sin_Vec [i] = abs(sin(qDegreesToRadians(angle_Temp)));
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
					measurement.beam_Theta_0_Angle_Vec[i] = angle_Temp;
					angle_Temp += angle_Step;
				}
			}
			// detector angle_cos_sin_cos2 : vector, depends on beam
			{
				measurement.detector_Theta_Angle_Vec.resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.detector_Theta_Cos_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.detector_Theta_Sin_Vec.  resize(measurement.beam_Theta_0_Angle.independent.num_Points);
				measurement.detector_Theta_Cos2_Vec. resize(measurement.beam_Theta_0_Angle.independent.num_Points);

				for(int i=0; i<measurement.beam_Theta_0_Angle.independent.num_Points; ++i)
				{
					double angle_Temp = measurement.beam_Theta_0_Angle_Vec[i] - angle_Shift + measurement.detector_Theta_Offset;
					measurement.detector_Theta_Angle_Vec[i] = angle_Temp;
					measurement.detector_Theta_Cos_Vec [i] = cos(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Sin_Vec [i] = sin(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Cos2_Vec[i] = pow(measurement.detector_Theta_Cos_Vec[i],2);
				}
			}
			// k : single value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		if( measurement.argument_Type == argument_Types[Detector_Theta_Phi_Angles] )
		{
			// detector THETA angle_cos_sin_cos2 : vector
			{
				measurement.detector_Theta_Angle_Vec.resize(measurement.detector_Theta_Angle.independent.num_Points);
				measurement.detector_Theta_Cos_Vec.  resize(measurement.detector_Theta_Angle.independent.num_Points);
				measurement.detector_Theta_Sin_Vec.  resize(measurement.detector_Theta_Angle.independent.num_Points);
				measurement.detector_Theta_Cos2_Vec. resize(measurement.detector_Theta_Angle.independent.num_Points);

				double angle_Step = (measurement.detector_Theta_Angle.independent.max - measurement.detector_Theta_Angle.independent.min) / (measurement.detector_Theta_Angle.independent.num_Points - 1);
				double angle_Temp =  measurement.detector_Theta_Angle.independent.min;
				for(int i=0; i<measurement.detector_Theta_Angle.independent.num_Points; ++i)
				{
					measurement.detector_Theta_Angle_Vec[i] = angle_Temp;
					measurement.detector_Theta_Cos_Vec [i] = cos(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Sin_Vec [i] = sin(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Cos2_Vec[i] = pow(measurement.detector_Theta_Cos_Vec[i],2);
					angle_Temp += angle_Step;
				}
			}
			// detector PHI angle_cos_sin : vector
			{
				measurement.detector_Phi_Angle_Vec.resize(measurement.detector_Phi_Angle.independent.num_Points);
				measurement.detector_Phi_Cos_Vec.  resize(measurement.detector_Phi_Angle.independent.num_Points);
				measurement.detector_Phi_Sin_Vec.  resize(measurement.detector_Phi_Angle.independent.num_Points);

				double minimum = min(measurement.detector_Phi_Angle.independent.max,measurement.detector_Phi_Angle.independent.min);
				double maximum = max(measurement.detector_Phi_Angle.independent.max,measurement.detector_Phi_Angle.independent.min);
				size_t num_Points_Phi = measurement.detector_Phi_Angle.independent.num_Points;

				double angle_Step = (maximum - minimum) / (num_Points_Phi - 1);
				double angle_Temp =  minimum;
				for(size_t i=0; i<num_Points_Phi; ++i)
				{
					measurement.detector_Phi_Cos_Vec[i] = cos(qDegreesToRadians(angle_Temp));
					measurement.detector_Phi_Sin_Vec[i] = sin(qDegreesToRadians(angle_Temp));
					measurement.detector_Phi_Angle_Vec[i] = angle_Temp;
					angle_Temp += angle_Step;
				}

				// splitting PHI points on two parts for honest calculation and interpolation

				measurement.start_Phi_Index = 0;
				measurement.end_Phi_Number = num_Points_Phi;
				if( maximum * minimum < 0 && num_Points_Phi >=11 )
				{
					int index = std::lower_bound(measurement.detector_Phi_Angle_Vec.begin(), measurement.detector_Phi_Angle_Vec.end(), 0) - measurement.detector_Phi_Angle_Vec.begin() - 1;
					if(abs(maximum) >= abs(minimum))	measurement.start_Phi_Index = index;
					else								measurement.end_Phi_Number = index+2;
				}
			}
			// beam angle_cos_sin_cos2 : single value
			{
				measurement.beam_Theta_0_Angle_Value = measurement.beam_Theta_0_Angle.value + angle_Shift;
				measurement.beam_Theta_0_Cos_Value  = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Sin_Value  = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Cos2_Value = pow(measurement.beam_Theta_0_Cos_Value,2);
			}
			// k : single value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
	}
}

void Independent_Curve::calc_cos2_k(double angle_Shift, double lambda_Shift)
{
	calc_Independent_cos2_k(angle_Shift, lambda_Shift);
}

Independent_Curve& Independent_Curve::operator =(const Independent_Curve& referent_Independent_Curve)
{
	name = referent_Independent_Curve.name;
	measurement = referent_Independent_Curve.measurement;	measurement.reset_All_IDs();
	calc_Functions = referent_Independent_Curve.calc_Functions;
	plot_Options = referent_Independent_Curve.plot_Options;
	angular_Units = referent_Independent_Curve.angular_Units;
	spectral_Units = referent_Independent_Curve.spectral_Units;
	label_Text = referent_Independent_Curve.spectral_Units;

	return *this;
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Independent_Curve* independent_Curve )
{	
	return stream	<< independent_Curve->name << independent_Curve->measurement
					<< independent_Curve->calc_Functions << independent_Curve->plot_Options
					<< independent_Curve->angular_Units	<< independent_Curve->spectral_Units
					<< independent_Curve->label_Text << independent_Curve->graph_2D_Positions;
}
QDataStream& operator >>(QDataStream& stream,		 Independent_Curve* independent_Curve )
{
	if(Global_Variables::check_Loaded_Version(1,11,0))
	{
		stream	>> independent_Curve->name >> independent_Curve->measurement
				>> independent_Curve->calc_Functions >> independent_Curve->plot_Options
				>> independent_Curve->angular_Units	>> independent_Curve->spectral_Units
				>> independent_Curve->label_Text >> independent_Curve->graph_2D_Positions;
    } else // before 1.11.0
	{
        stream >> independent_Curve->measurement >> independent_Curve->calc_Functions;
		{
			Old_Calculated_Values old_Calculated_Values;
            stream >> old_Calculated_Values;
		}
        stream >> independent_Curve->name >> independent_Curve->plot_Options;

        if(Global_Variables::check_Loaded_Version(1,7,5))
        {
            QString argument_Type;
            stream >> argument_Type;
        }
        Global_Variables::pseudo_Deserialize_Tree(stream);
        Global_Variables::pseudo_Deserialize_Variables_List(stream);
	}
	independent_Curve->refresh_Description_Label();
	return stream;
}
