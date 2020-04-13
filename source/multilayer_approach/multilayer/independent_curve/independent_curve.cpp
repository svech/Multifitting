#include "independent_curve.h"

Independent_Curve::Independent_Curve(QWidget *parent) :
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
	main_Layout->addWidget(setup_Button);

	description_Label  = new QLabel("<no description>");
	main_Layout->addWidget(description_Label);
}

QString Independent_Curve::enlarge_Tab_Name()
{
	// TODO INDEPENDENT
	if(calc_Functions.check_Enabled)
	{
		QString suffix = "  ";
		if(calc_Functions.check_Reflectance)	suffix = suffix+symbol_R+",";
		if(calc_Functions.check_Transmittance)  suffix = suffix+symbol_T+",";
		if(calc_Functions.check_Absorptance)	suffix = suffix+symbol_A+",";
		if(calc_Functions.check_Field)			suffix = suffix+symbol_F+",";
		if(calc_Functions.check_Joule)			suffix = suffix+symbol_J+",";
		if(calc_Functions.check_User)			suffix = suffix+symbol_U+",";
		return suffix.remove(-1,1);
	} else
	{
		return "";
	}
}

void Independent_Curve::refresh_Description_Label()
{
	// TODO
	if(measurement.measurement_Type != no_Measurement_Type)
	{
		double spectral_Coeff = wavelength_Coefficients_Map.value(spectral_Units);
		double angular_Coeff = angle_Coefficients_Map.value(angular_Units);

		if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " +
							  Locale.toString(measurement.beam_Theta_0_Angle.independent.min/angular_Coeff) +
						"-" + Locale.toString(measurement.beam_Theta_0_Angle.independent.max/angular_Coeff) +
							  "" + angular_Units + "; " + "at " +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units;
			}
			if(measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
				label_Text =  measurement.measurement_Type + "; " +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.independent.min)/spectral_Coeff) +
						"-" + Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.independent.max)/spectral_Coeff) +
							  " " + spectral_Units + "; " + "at " +
							  Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  "" + angular_Units;
			}

		}
		if(	measurement.measurement_Type == measurement_Types[Detector_Scan] )
		{
	//		detector_Target_Curve_Part->refresh_Description_Label();
		}
		if(	measurement.measurement_Type == measurement_Types[Rocking_Curve] )
		{
	//		rocking_Target_Curve_Part->refresh_Description_Label();
		}
		if(	measurement.measurement_Type == measurement_Types[Offset_Scan] )
		{
	//		offset_Target_Curve_Part->refresh_Description_Label();
		}
		if(	measurement.measurement_Type == measurement_Types[GISAS] )
		{
	//		gisas_Target_Curve_Part->refresh_Description_Label();
		}

		description_Label->setText(label_Text);
	} else
	{
		label_Text = "<no description>";
		description_Label->setText(label_Text);
	}
}

Independent_Curve& Independent_Curve::operator =(const Independent_Curve& referent_Independent_Curve)
{
	// TODO INDEPENDENT ?

	return *this;
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Independent_Curve* independent_Curve )
{	
	return stream	<< independent_Curve->tab_Name << independent_Curve->measurement
					<< independent_Curve->calculated_Values	<< independent_Curve->plot_Options
					<< independent_Curve->angular_Units	<< independent_Curve->spectral_Units
					<< independent_Curve->label_Text;
}
QDataStream& operator >>(QDataStream& stream,		 Independent_Curve* independent_Curve )
{
	if(Global_Variables::check_Loaded_Version(1,11,0))
	{
		stream	>> independent_Curve->tab_Name >> independent_Curve->measurement
				>> independent_Curve->calculated_Values	>> independent_Curve->plot_Options
				>> independent_Curve->angular_Units	>> independent_Curve->spectral_Units
				>> independent_Curve->label_Text;
	} else
	{
		stream >> independent_Curve->measurement >> independent_Curve->calc_Functions;
		stream >> independent_Curve->calculated_Values;
		stream >> independent_Curve->tab_Name >> independent_Curve->plot_Options;

		if(Global_Variables::check_Loaded_Version(1,7,5))
		{
			QString argument_Type;
			stream >> argument_Type;
		}
	}
	independent_Curve->refresh_Description_Label();
	return stream;
}
