#include "global_definitions.h"
#include "global_variables.h"
#include <algorithm>    // std::reverse

QString mainDirPath()
{
#ifdef __linux__
    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    QString appPath = QString::fromStdString(std::string( result, (count > 0) ? count : 0 ));
    QDir dir = QFileInfo(appPath).dir();
    return dir.filePath("../");
#endif
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    QString appPath = QString::fromStdString(std::string(buffer));
    QDir dir = QFileInfo(appPath).dir();
#ifdef RELEASE
    return dir.filePath("../");
#else
    return dir.filePath("../../");
#endif
#endif
}

Global_Definitions::Global_Definitions()
{

}

id_Type previous_ID = 100; // zero is reserved ID

id_Type Global_Definitions::generate_Id()
{
	id_Type current_ID = previous_ID+1;

	if(current_ID < previous_ID)
	{
		// should never appear
		QMessageBox::critical(nullptr, "Global_Definitions::generate_Id()", "Out of ID range!");
		exit(EXIT_FAILURE);
	}
	++previous_ID;
	return current_ID;
}


Simple_Curve::Simple_Curve()
{
	plot_Options.thickness = 1.5;
	plot_Options.color = QColor(0, 0, 0, 254);
}

void Simple_Curve::read_Simple_Curve(QString bare_Filename)
{
	QFileInfo filename = bare_Filename;
	QStringList lines_List;

	// reading whole file to lines_list
	QFile input_File(filename.absoluteFilePath());
	QString temp_Line = "not empty now";
	if(input_File.open(QIODevice::ReadOnly))
	{
		QTextStream input_Stream(&input_File);

		while ( !input_Stream.atEnd() )
		{
			temp_Line=input_Stream.readLine();
			lines_List.append(temp_Line);
		}
		input_File.close();
	}

	// parsing content
	argument.clear();
	values.clear();

	for(int line_Index=0; line_Index<lines_List.size(); ++line_Index)
	{
		QString temp_Line = lines_List[line_Index];
		temp_Line = temp_Line.simplified();

		// check if not header line (starts from digit)
		bool is_Decimal = false;
		QString(temp_Line[0]).toInt(&is_Decimal);

		// split line into "numbers"
		QStringList potentional_Numbers = temp_Line.split(delimiters,QString::SkipEmptyParts);

		if(is_Decimal && potentional_Numbers.size()>=2)
		{
			bool ok_To_Double = false;

			double temp_Argument = QString(potentional_Numbers[0]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas are considered as dots
			double temp_Value    = QString(potentional_Numbers[1]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas are considered as dots

			if(!ok_To_Double) goto skip_line_label;

			// argument should be monotonic
			if(argument.size()>=2)
			{
				if(argument[argument.size()-1]>argument[argument.size()-2]) // increasing argument is allowed
				{
					if(temp_Argument <= argument.back()) goto skip_line_label; // read only monotonical arguments
				}
				if(argument[argument.size()-1]<argument[argument.size()-2]) // decreasing argument is allowed
				{
					if(temp_Argument >= argument.back()) goto skip_line_label; // read only monotonical arguments
				}
			}

			argument.push_back(temp_Argument);
			values.push_back(temp_Value);

			// this line may be skipped
			skip_line_label: ok_To_Double = false;
		}
	}
}


bool Calc_Functions::if_Something_Enabled_1D() const
{
	if( check_Reflectance	||
		check_Transmittance||
		check_Absorptance	||
		check_Scattering	)
	{
		return true;
	} else
	{
		return false;
	}
}

bool Calc_Functions::if_Something_Enabled_2D() const
{
	if( check_Field ||
		check_Joule ||
		check_GISAS )
	{
		return true;
	} else
	{
		return false;
	}
}

bool Calc_Functions::if_Reflectance_Only() const
{
	if(  check_Reflectance &&
		!check_Transmittance &&
		!check_Absorptance &&
		!check_Scattering &&
		!check_Field &&
		!check_Joule &&
		!check_GISAS )
	{
		return true;
	} else
	{
		return false;
	}
}



// -----------------------------------------------------------------------------------------
// serialization

QDataStream& operator <<( QDataStream& stream, const Independent_Indicator& independent_Indicator )
{
	return stream << independent_Indicator.item_Id << independent_Indicator.id << independent_Indicator.item_Type << independent_Indicator.parameter_Whats_This << independent_Indicator.index << independent_Indicator.is_Active;
}
QDataStream& operator >>( QDataStream& stream,		 Independent_Indicator& independent_Indicator )
{
	return stream >> independent_Indicator.item_Id >> independent_Indicator.id >> independent_Indicator.item_Type >> independent_Indicator.parameter_Whats_This >> independent_Indicator.index >> independent_Indicator.is_Active;
}

QDataStream& operator <<( QDataStream& stream, const Parameter_Indicator& parameter_Indicator )
{
	return stream << parameter_Indicator.id << parameter_Indicator.item_Id << parameter_Indicator.tab_Index << parameter_Indicator.full_Name << parameter_Indicator.whats_This << parameter_Indicator.expression << parameter_Indicator.exist;
}
QDataStream& operator >>( QDataStream& stream,		 Parameter_Indicator& parameter_Indicator )
{
	stream >> parameter_Indicator.id >> parameter_Indicator.item_Id >> parameter_Indicator.tab_Index >> parameter_Indicator.full_Name >> parameter_Indicator.whats_This >> parameter_Indicator.expression >> parameter_Indicator.exist;

	QString old_Whats_This = "Particle Correlation Depth";
	if(parameter_Indicator.whats_This == old_Whats_This)
	{
		parameter_Indicator.whats_This = whats_This_Particle_Cross_Layer_Deviation;
	}
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Int_Independent& int_Independent )
{
	return stream << int_Independent.start << int_Independent.step << int_Independent.num_Steps
				  << int_Independent.parameter; // since 1.8.4
}
QDataStream& operator >>( QDataStream& stream,		 Int_Independent& int_Independent )
{
	if(!Global_Variables::check_Loaded_Version(1,8,5))
	{int value; stream >> value >> int_Independent.parameter.independent.is_Independent; int_Independent.parameter.value = double(value);}	// since 1.8.5
	stream >> int_Independent.start >> int_Independent.step >> int_Independent.num_Steps;
	if(Global_Variables::check_Loaded_Version(1,8,4))
	{stream >> int_Independent.parameter;}		    // since 1.8.4
	else
	{stream >> int_Independent.parameter.indicator.id >> int_Independent.parameter.indicator.whats_This;} // before 1.8.4

	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Min_Max& min_Max )
{
	return stream << min_Max.thickness_Min << min_Max.thickness_Max << min_Max.sigma_Diffuse_Min << min_Max.sigma_Diffuse_Max;  // since 1.8.11
}
QDataStream& operator >>( QDataStream& stream,		 Min_Max& min_Max )
{
	if(Global_Variables::check_Loaded_Version(1,8,11))
	{      stream >> min_Max.thickness_Min >> min_Max.thickness_Max >> min_Max.sigma_Diffuse_Min >> min_Max.sigma_Diffuse_Max;} // since 1.8.11

	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Independent& independent )
{
	return stream << independent.is_Independent << independent.min << independent.max << independent.num_Points;
}
QDataStream& operator >>( QDataStream& stream,		 Independent& independent )
{
    return stream >> independent.is_Independent >> independent.min >> independent.max >> independent.num_Points;
}

QDataStream& operator <<( QDataStream& stream, const Coupled& coupled )
{
	return stream << coupled.is_Coupled << coupled.master << coupled.slaves;
}
QDataStream& operator >>( QDataStream& stream,		 Coupled& coupled )
{
	return stream >> coupled.is_Coupled >> coupled.master >> coupled.slaves;
}

QDataStream& operator <<( QDataStream& stream, const Fit& fit )
{
	return stream << fit.is_Fitable << fit.min_Bounded << fit.min << fit.max_Bounded << fit.max;
}
QDataStream& operator >>( QDataStream& stream,		 Fit& fit )
{
	return stream >> fit.is_Fitable >> fit.min_Bounded >> fit.min >> fit.max_Bounded >> fit.max;
}

QDataStream& operator <<( QDataStream& stream, const Confidence& confidence )
{
	return stream << confidence.calc_Conf_Interval << confidence.min << confidence.max << confidence.num_Points
				  << confidence.is_Active; // since 1.8.0
}
QDataStream& operator >>( QDataStream& stream,		 Confidence& confidence )
{
	stream >> confidence.calc_Conf_Interval >> confidence.min >> confidence.max >> confidence.num_Points;

	if(Global_Variables::check_Loaded_Version(1,8,0))
	{stream >> confidence.is_Active;}		// since 1.8.0

	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Parameter& parameter )
{
	return stream << parameter.value << parameter.independent << parameter.coupled << parameter.fit << parameter.indicator
				  << parameter.confidence;	// since 1.7.9
}
QDataStream& operator >>( QDataStream& stream,		 Parameter& parameter )
{
	stream >> parameter.value >> parameter.independent >> parameter.coupled >> parameter.fit >> parameter.indicator;

	if(Global_Variables::check_Loaded_Version(1,7,9))
	{stream >> parameter.confidence; }		// since 1.7.9

	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Distribution& resolution )
{
	return stream << resolution.FWHM_distribution << resolution.distribution_Function << resolution.coverage << resolution.use_Sampling << resolution.number_of_Samples;
}
QDataStream& operator >>( QDataStream& stream,		 Distribution& resolution )
{
	return stream >> resolution.FWHM_distribution >> resolution.distribution_Function >> resolution.coverage >> resolution.use_Sampling >> resolution.number_of_Samples;
}

QDataStream& operator <<( QDataStream& stream, const Detector_1D& detector_1D )
{
	return stream << detector_1D.detector_Type
				  << detector_1D.slit_Width << detector_1D.distance_To_Sample
                  << detector_1D.finite_Slit << detector_1D.slit_Length
                  << detector_1D.detector_Slit_Distribution
				  << detector_1D.detector_Theta_Resolution
				  << detector_1D.use_Binning << detector_1D.binning_Factor;
}
QDataStream& operator >>( QDataStream& stream,		 Detector_1D& detector_1D )
{
	stream >> detector_1D.detector_Type
		   >> detector_1D.slit_Width >> detector_1D.distance_To_Sample;
	if(Global_Variables::check_Loaded_Version(1,11,20))
	{
		stream >> detector_1D.finite_Slit >> detector_1D.slit_Length;
    }
    if(Global_Variables::check_Loaded_Version(2,1,0))
    {
        stream >> detector_1D.detector_Slit_Distribution;
    }
	stream >> detector_1D.detector_Theta_Resolution;
	if(Global_Variables::check_Loaded_Version(1,11,19))
	{
		stream >> detector_1D.use_Binning >> detector_1D.binning_Factor;
	}
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Detector_2D& detector_2D )
{
	return stream << detector_2D.detector_Type << detector_2D.set_Pixel_Size
				  << detector_2D.pixel_Azimuthal_Width << detector_2D.pixel_Polar_Height << detector_2D.distance_To_Sample
				  << detector_2D.detector_Theta_Resolution << detector_2D.detector_Phi_Resolution
				  << detector_2D.use_Binning << detector_2D.theta_Binning_Factor << detector_2D.phi_Binning_Factor;
}
QDataStream& operator >>( QDataStream& stream,		 Detector_2D& detector_2D )
{
	stream >> detector_2D.detector_Type;

    // legacy from 1.11.30
    if(detector_2D.detector_Type == "Matrix")
        detector_2D.detector_Type = "Linear";
    if(detector_2D.detector_Type == "Spherical")
        detector_2D.detector_Type = "Angular";

	if(Global_Variables::check_Loaded_Version(1,11,30))
	{stream >> detector_2D.set_Pixel_Size;}

	if(Global_Variables::check_Loaded_Version(1,11,1))
	{stream >> detector_2D.pixel_Azimuthal_Width >> detector_2D.pixel_Polar_Height >> detector_2D.distance_To_Sample;}

	stream >> detector_2D.detector_Theta_Resolution >> detector_2D.detector_Phi_Resolution;

	if(Global_Variables::check_Loaded_Version(1,11,19))
	{
		stream >> detector_2D.use_Binning >> detector_2D.theta_Binning_Factor >> detector_2D.phi_Binning_Factor;
	}
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Sample_Geometry& sample_Geometry )
{
	return stream << sample_Geometry.size << sample_Geometry.x_Position << sample_Geometry.z_Position << sample_Geometry.curvature;
}
QDataStream& operator >>( QDataStream& stream,		 Sample_Geometry& sample_Geometry )
{
	return stream >> sample_Geometry.size >> sample_Geometry.x_Position >> sample_Geometry.z_Position >> sample_Geometry.curvature;
}

QDataStream& operator <<( QDataStream& stream, const Beam_Geometry& beam_Geometry )
{
	return stream << beam_Geometry.size << beam_Geometry.lateral_Width
				  << beam_Geometry.smoothing  << beam_Geometry.right_Smoothing  << beam_Geometry.asymmetric_Smoothing
				  << beam_Geometry.wings_Full_Width << beam_Geometry.right_Wing_Width << beam_Geometry.asymmetric_Wing_Width << beam_Geometry.wings_Intensity
				  << beam_Geometry.log_Profile_Plot;
}
QDataStream& operator >>( QDataStream& stream,		 Beam_Geometry& beam_Geometry )
{
	stream >> beam_Geometry.size;

	if(Global_Variables::check_Loaded_Version(1,11,0))
	{stream >> beam_Geometry.lateral_Width;}

	stream >> beam_Geometry.smoothing;

	if(Global_Variables::check_Loaded_Version(1,11,3))
	{stream >> beam_Geometry.right_Smoothing >> beam_Geometry.asymmetric_Smoothing;}


	if(Global_Variables::check_Loaded_Version(1,11,2))
	{stream >> beam_Geometry.wings_Full_Width >> beam_Geometry.right_Wing_Width;}

	if(Global_Variables::check_Loaded_Version(1,11,3))
	{stream >> beam_Geometry.asymmetric_Wing_Width;}

	if(Global_Variables::check_Loaded_Version(1,11,2))
	{stream >> beam_Geometry.wings_Intensity;}

	if(Global_Variables::check_Loaded_Version(1,11,3))
	{stream >> beam_Geometry.log_Profile_Plot;}

	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Stoichiometry& stoichiometry )
{
	return stream << stoichiometry.composition << stoichiometry.type;
}
QDataStream& operator >>( QDataStream& stream,		 Stoichiometry& stoichiometry )
{
	return stream >> stoichiometry.composition >> stoichiometry.type;
}

QDataStream& operator <<( QDataStream& stream, const Interlayer& interlayer )
{
	return stream << interlayer.interlayer << interlayer.my_Sigma_Diffuse << interlayer.enabled;
}
QDataStream& operator >>( QDataStream& stream,		 Interlayer& interlayer )
{
	return stream >> interlayer.interlayer >> interlayer.my_Sigma_Diffuse >> interlayer.enabled;
}

QDataStream& operator <<( QDataStream& stream, const Drift& drift )
{
	return stream << drift.is_Drift_Line << drift.is_Drift_Sine << drift.is_Drift_Rand
				  << drift.drift_Line_Value << drift.drift_Rand_Rms << drift.drift_Sine_Amplitude << drift.drift_Sine_Frequency << drift.drift_Sine_Phase
				  << drift.show_Drift_Line << drift.show_Drift_Rand << drift.show_Drift_Sine; // since 1.8.2
}
QDataStream& operator >>( QDataStream& stream,		 Drift& drift )
{
	stream >> drift.is_Drift_Line >> drift.is_Drift_Sine >> drift.is_Drift_Rand
		   >> drift.drift_Line_Value >> drift.drift_Rand_Rms >> drift.drift_Sine_Amplitude >> drift.drift_Sine_Frequency >> drift.drift_Sine_Phase;

	if(Global_Variables::check_Loaded_Version(1,8,2))
	{stream >> drift.show_Drift_Line >> drift.show_Drift_Rand >> drift.show_Drift_Sine; }	// since 1.8.2

	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Plot_Options& plot_Options )
{
	return stream << plot_Options.rescale
				  // 1D
				  << plot_Options.x_Scale << plot_Options.y_Scale << plot_Options.color
				  << plot_Options.scatter_Shape << plot_Options.scatter_Size << plot_Options.thickness
				  // 2D
				  << plot_Options.use_Interpolation << plot_Options.z_Scale << plot_Options.color_Scheme
				  << plot_Options.rotation_Angle << plot_Options.left_Section_Plot
				  << plot_Options.bottom_Section_Plot << plot_Options.bottom_Section_Tab_Index
				  << plot_Options.data_To_Show << plot_Options.orders_To_Show << plot_Options.orientation;
}
QDataStream& operator >>( QDataStream& stream,		 Plot_Options& plot_Options )
{
	if(Global_Variables::check_Loaded_Version(1,11,0))
	{
		stream>> plot_Options.rescale
			  // 1D
			  >> plot_Options.x_Scale >> plot_Options.y_Scale >> plot_Options.color
			  >> plot_Options.scatter_Shape >> plot_Options.scatter_Size >> plot_Options.thickness
			  // 2D
			  >> plot_Options.use_Interpolation >> plot_Options.z_Scale >> plot_Options.color_Scheme
			  >> plot_Options.rotation_Angle >> plot_Options.left_Section_Plot
			  >> plot_Options.bottom_Section_Plot >> plot_Options.bottom_Section_Tab_Index
			  >> plot_Options.data_To_Show >> plot_Options.orders_To_Show >> plot_Options.orientation;
	} else
	{
		stream >> plot_Options.y_Scale;

		if(Global_Variables::check_Loaded_Version(1,9,3))
		{stream >> plot_Options.x_Scale;}	// since 1.9.3

		if(Global_Variables::check_Loaded_Version(1,7,6))
		{stream >> plot_Options.rescale; }	// since 1.7.6

		stream >> plot_Options.color >> plot_Options.scatter_Shape >> plot_Options.scatter_Size        >> plot_Options.thickness;

		// ------------------------------------------------------
		QString scale_Second;
		QColor color_Second;
		int scatter_Shape_Second;
		double scatter_Size_Second;
		double thickness_Second;
		stream >> scale_Second >> color_Second >> scatter_Shape_Second >> scatter_Size_Second >> thickness_Second;
		// ------------------------------------------------------
	}
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Graph_Options& graph_Options )
{
	return stream << graph_Options.num_Target_Graph_Rows << graph_Options.num_Independent_Graph_Rows
				  << graph_Options.show_Scatter << graph_Options.show_Thickness << graph_Options.show_X_Scale
                                  << graph_Options.show_Max_Value << graph_Options.show_FWHM
                                  << graph_Options.show_Range << graph_Options.show_Current_Coordinate
                                  << graph_Options.show_Title << graph_Options.show_Interpolation;
}
QDataStream& operator >>( QDataStream& stream,		 Graph_Options& graph_Options )
{
	stream >> graph_Options.num_Target_Graph_Rows >> graph_Options.num_Independent_Graph_Rows
		   >> graph_Options.show_Scatter >> graph_Options.show_Thickness >> graph_Options.show_X_Scale
		   >> graph_Options.show_Max_Value;

        if(Global_Variables::check_Loaded_Version(1,11,31))
        {
                stream >> graph_Options.show_FWHM;
        }
	if(Global_Variables::check_Loaded_Version(1,11,11))
	{
		stream >> graph_Options.show_Range;
	}

	stream >> graph_Options.show_Current_Coordinate >> graph_Options.show_Title;
	if(Global_Variables::check_Loaded_Version(1,11,0))
	{
		stream >> graph_Options.show_Interpolation;
	}

	return stream;
}
QDataStream& operator <<( QDataStream& stream, const Graph_2D_Positions& graph_2D_Positions )
{
	return stream << graph_2D_Positions.left_Min_Width << graph_2D_Positions.bottom_Min_Width
				  << graph_2D_Positions.x_Mark << graph_2D_Positions.y_Mark << graph_2D_Positions.current_Pos;
}
QDataStream& operator >>( QDataStream& stream,		 Graph_2D_Positions& graph_2D_Positions )
{
	return stream >> graph_2D_Positions.left_Min_Width >> graph_2D_Positions.bottom_Min_Width
				  >> graph_2D_Positions.x_Mark >> graph_2D_Positions.y_Mark >> graph_2D_Positions.current_Pos;
}

QDataStream& operator <<( QDataStream& stream, const Profile_Plot_Options& profile_Plot_Options )
{
	return stream << profile_Plot_Options.type << profile_Plot_Options.permittivity_Type
				  << profile_Plot_Options.apply_Roughness << profile_Plot_Options.apply_Diffuseness
				  << profile_Plot_Options.show_Sharp_Profile << profile_Plot_Options.show_Discretization << profile_Plot_Options.show_Cursor_Position
                                  << profile_Plot_Options.rescale_X << profile_Plot_Options.rescale_Y << profile_Plot_Options.y_Scale
                                  << profile_Plot_Options.first_opening
                                  << profile_Plot_Options.old_X_Begin << profile_Plot_Options.old_X_End << profile_Plot_Options.old_Y_Begin << profile_Plot_Options.old_Y_End
                                  << profile_Plot_Options.local_wavelength_units << profile_Plot_Options.local_length_units << profile_Plot_Options.local_Wavelength;
}
QDataStream& operator >>( QDataStream& stream,		 Profile_Plot_Options& profile_Plot_Options )
{
	stream >> profile_Plot_Options.type >> profile_Plot_Options.permittivity_Type
               >> profile_Plot_Options.apply_Roughness >> profile_Plot_Options.apply_Diffuseness
               >> profile_Plot_Options.show_Sharp_Profile >> profile_Plot_Options.show_Discretization >> profile_Plot_Options.show_Cursor_Position
               >> profile_Plot_Options.rescale_X >> profile_Plot_Options.rescale_Y >> profile_Plot_Options.y_Scale;
        if(Global_Variables::check_Loaded_Version(1,11,32))
        {
            stream >> profile_Plot_Options.first_opening;
        }
        stream >> profile_Plot_Options.old_X_Begin >> profile_Plot_Options.old_X_End >> profile_Plot_Options.old_Y_Begin >> profile_Plot_Options.old_Y_End
               >> profile_Plot_Options.local_wavelength_units >> profile_Plot_Options.local_length_units >> profile_Plot_Options.local_Wavelength;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Roughness_Plot_Options& roughness_Plot_Options )
{
	return stream << roughness_Plot_Options.PSD_Type
                      << roughness_Plot_Options.show_Top_Surface << roughness_Plot_Options.show_Interface_With_Number << roughness_Plot_Options.show_Substrate_Surface << roughness_Plot_Options.interface_Number_To_Show << roughness_Plot_Options.show_Cursor_Position
                      << roughness_Plot_Options.rescale_X << roughness_Plot_Options.rescale_Y
                      << roughness_Plot_Options.x_Min << roughness_Plot_Options.x_Max
                      << roughness_Plot_Options.old_X_Begin << roughness_Plot_Options.old_X_End
                      << roughness_Plot_Options.orders_To_Show << roughness_Plot_Options.old_Y_Begin << roughness_Plot_Options.old_Y_End
                      << roughness_Plot_Options.x_Scale << roughness_Plot_Options.y_Scale
                      << roughness_Plot_Options.local_frequency_units << roughness_Plot_Options.local_PSD_1D_units << roughness_Plot_Options.local_PSD_2D_units;
}
QDataStream& operator >>( QDataStream& stream,		 Roughness_Plot_Options& roughness_Plot_Options )
{
	return stream >> roughness_Plot_Options.PSD_Type
				  >> roughness_Plot_Options.show_Top_Surface >> roughness_Plot_Options.show_Interface_With_Number >> roughness_Plot_Options.show_Substrate_Surface >> roughness_Plot_Options.interface_Number_To_Show >> roughness_Plot_Options.show_Cursor_Position
				  >> roughness_Plot_Options.rescale_X >> roughness_Plot_Options.rescale_Y
				  >> roughness_Plot_Options.x_Min >> roughness_Plot_Options.x_Max
				  >> roughness_Plot_Options.old_X_Begin >> roughness_Plot_Options.old_X_End
				  >> roughness_Plot_Options.orders_To_Show >> roughness_Plot_Options.old_Y_Begin >> roughness_Plot_Options.old_Y_End
				  >> roughness_Plot_Options.x_Scale >> roughness_Plot_Options.y_Scale
				  >> roughness_Plot_Options.local_frequency_units >> roughness_Plot_Options.local_PSD_1D_units >> roughness_Plot_Options.local_PSD_2D_units;
}

QDataStream& operator <<( QDataStream& stream, const Particles_Plot_Options& particles_Plot_Options )
{
	return stream << particles_Plot_Options.A_show_Layer_With_Number << particles_Plot_Options.B_show_Layer_With_Number
				  << particles_Plot_Options.A_layer_Number_To_Show << particles_Plot_Options.B_layer_Number_To_Show << particles_Plot_Options.show_Cursor_Position
				  << particles_Plot_Options.rescale_X << particles_Plot_Options.rescale_Y
				  << particles_Plot_Options.x_Min << particles_Plot_Options.x_Max
				  << particles_Plot_Options.old_X_Begin << particles_Plot_Options.old_X_End
				  << particles_Plot_Options.orders_To_Show << particles_Plot_Options.old_Y_Begin << particles_Plot_Options.old_Y_End
				  << particles_Plot_Options.x_Scale << particles_Plot_Options.y_Scale
				  << particles_Plot_Options.local_frequency_units << particles_Plot_Options.local_value_units;
}
QDataStream& operator >>( QDataStream& stream,		 Particles_Plot_Options& particles_Plot_Options )
{
	return stream >> particles_Plot_Options.A_show_Layer_With_Number >> particles_Plot_Options.B_show_Layer_With_Number
				  >> particles_Plot_Options.A_layer_Number_To_Show >> particles_Plot_Options.B_layer_Number_To_Show >> particles_Plot_Options.show_Cursor_Position
				  >> particles_Plot_Options.rescale_X >> particles_Plot_Options.rescale_Y
				  >> particles_Plot_Options.x_Min >> particles_Plot_Options.x_Max
				  >> particles_Plot_Options.old_X_Begin >> particles_Plot_Options.old_X_End
				  >> particles_Plot_Options.orders_To_Show >> particles_Plot_Options.old_Y_Begin >> particles_Plot_Options.old_Y_End
				  >> particles_Plot_Options.x_Scale >> particles_Plot_Options.y_Scale
				  >> particles_Plot_Options.local_frequency_units >> particles_Plot_Options.local_value_units;}

QDataStream& operator <<( QDataStream& stream, const Material_Profile& material_Profile )
{
	return stream << material_Profile.material << material_Profile.color << material_Profile.relative_Concentration;
}
QDataStream& operator >>( QDataStream& stream,		 Material_Profile& material_Profile )
{
	stream >> material_Profile.material >> material_Profile.color >> material_Profile.relative_Concentration;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Element_Profile& element_Profile )
{
	return stream << element_Profile.element << element_Profile.color << element_Profile.absolute_Concentration;
}
QDataStream& operator >>( QDataStream& stream,		 Element_Profile& element_Profile )
{
	stream >> element_Profile.element >> element_Profile.color >> element_Profile.absolute_Concentration;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Basic_Profile_Plot_Set& basic_Profile_Plot_Set )
{
	return stream << basic_Profile_Plot_Set.depth
				  << basic_Profile_Plot_Set.delta_Epsilon << basic_Profile_Plot_Set.beta_Epsilon
				  << basic_Profile_Plot_Set.materials << basic_Profile_Plot_Set.elements;
}
QDataStream& operator >>( QDataStream& stream,		 Basic_Profile_Plot_Set& basic_Profile_Plot_Set )
{
	stream >> basic_Profile_Plot_Set.depth
		   >> basic_Profile_Plot_Set.delta_Epsilon >> basic_Profile_Plot_Set.beta_Epsilon
		   >> basic_Profile_Plot_Set.materials >> basic_Profile_Plot_Set.elements;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Old_Calculated_Values& old_Calculated_Values )
{
	return stream << old_Calculated_Values.R << old_Calculated_Values.Phi_R
				  << old_Calculated_Values.T
				  << old_Calculated_Values.A;
}
QDataStream& operator >>( QDataStream& stream,		 Old_Calculated_Values& old_Calculated_Values )
{
	return stream >> old_Calculated_Values.R >> old_Calculated_Values.Phi_R
				  >> old_Calculated_Values.T
				  >> old_Calculated_Values.A;
}

QDataStream& operator <<( QDataStream& stream, const Discretization_Parameters& discretization_Parameters )
{
	return stream << discretization_Parameters.enable_Discretization
				  << discretization_Parameters.discretization_Step;
}
QDataStream& operator >>( QDataStream& stream,		 Discretization_Parameters& discretization_Parameters )
{
	return stream >> discretization_Parameters.enable_Discretization
				  >> discretization_Parameters.discretization_Step;
}

QDataStream& operator <<( QDataStream& stream, const Roughness_Model& roughness_Model )
{
	return stream << roughness_Model.is_Enabled
				  << roughness_Model.sigma << roughness_Model.cor_radius << roughness_Model.fractal_alpha
				  << roughness_Model.omega << roughness_Model.mu << roughness_Model.fractal_beta

				  << roughness_Model.a1 << roughness_Model.a2 << roughness_Model.a3 << roughness_Model.a4

				  << roughness_Model.peak_Sigma << roughness_Model.peak_Frequency << roughness_Model.peak_Frequency_Width
				  << roughness_Model.sigma_Factor_PSD_1D << roughness_Model.sigma_Factor_PSD_2D;
}
QDataStream& operator >>( QDataStream& stream,		 Roughness_Model& roughness_Model )
{
	stream >> roughness_Model.is_Enabled
		   >> roughness_Model.sigma >> roughness_Model.cor_radius >> roughness_Model.fractal_alpha
		   >> roughness_Model.omega >> roughness_Model.mu;

	if(Global_Variables::check_Loaded_Version(1,11,7))
	{
	stream >> roughness_Model.fractal_beta;

		if(Global_Variables::check_Loaded_Version(1,11,13))	{
			stream >> roughness_Model.a1 >> roughness_Model.a2 >> roughness_Model.a3 >> roughness_Model.a4;
		}

	stream >> roughness_Model.peak_Sigma >> roughness_Model.peak_Frequency >> roughness_Model.peak_Frequency_Width;
	}
	if(Global_Variables::check_Loaded_Version(1,11,10))
	{stream >> roughness_Model.sigma_Factor_PSD_1D >> roughness_Model.sigma_Factor_PSD_2D;}

	return stream;
}

QDataStream& operator <<(QDataStream& stream, const Particles_Model& particles_Model )
{
	return stream << particles_Model.is_Enabled << particles_Model.is_Independent << particles_Model.is_Last_Layer
				  << particles_Model.is_Used

				  << particles_Model.particle_Shape
				  << particles_Model.particle_Material << particles_Model.particle_Approved_Material << particles_Model.particle_Composition
				  << particles_Model.particle_Absolute_Density << particles_Model.particle_Relative_Density
				  << particles_Model.particle_Radius << particles_Model.particle_Height

				  << particles_Model.particle_Interference_Function << particles_Model.geometric_Model
				  << particles_Model.particle_Average_Distance
				  << particles_Model.particle_Radial_Distance << particles_Model.particle_Radial_Distance_Deviation << particles_Model.domain_Size
				  << particles_Model.particle_Cross_Layer_Deviation
				  << particles_Model.particle_Z_Position << particles_Model.particle_Z_Position_Deviation;
}
QDataStream& operator >>( QDataStream& stream,		 Particles_Model& particles_Model )
{
	stream >> particles_Model.is_Enabled;

	if(Global_Variables::check_Loaded_Version(1,11,24))
	{stream>> particles_Model.is_Independent >> particles_Model.is_Last_Layer;}

	if(Global_Variables::check_Loaded_Version(1,11,24) && !Global_Variables::check_Loaded_Version(1,11,28))
	{bool is_Second_Last_Layer;	stream>> is_Second_Last_Layer;}

	stream >> particles_Model.is_Used;

	if(Global_Variables::check_Loaded_Version(1,11,21) && !Global_Variables::check_Loaded_Version(1,11,24))
	{
		bool set_Another_Material;
		stream>> set_Another_Material;
	}

	stream >> particles_Model.particle_Shape;

	if(Global_Variables::check_Loaded_Version(1,11,21))
	{stream >> particles_Model.particle_Material >> particles_Model.particle_Approved_Material >> particles_Model.particle_Composition;}

	stream >> particles_Model.particle_Absolute_Density >> particles_Model.particle_Relative_Density
		   >> particles_Model.particle_Radius >> particles_Model.particle_Height

		   >> particles_Model.particle_Interference_Function >> particles_Model.geometric_Model
		   >> particles_Model.particle_Average_Distance
		   >> particles_Model.particle_Radial_Distance >> particles_Model.particle_Radial_Distance_Deviation >> particles_Model.domain_Size;
	if(Global_Variables::check_Loaded_Version(1,11,21))
	{stream>> particles_Model.particle_Cross_Layer_Deviation;}
	stream >> particles_Model.particle_Z_Position >> particles_Model.particle_Z_Position_Deviation;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const PSD_Data& psd_Data )
{
	return stream << psd_Data.raw_Argument << psd_Data.raw_Value << psd_Data.argument << psd_Data.value
				  << psd_Data.PSD_Type << psd_Data.argument_Units << psd_Data.value_Units << psd_Data.filepath;
}
QDataStream& operator >>( QDataStream& stream,		 PSD_Data& psd_Data )
{
	return stream >> psd_Data.raw_Argument >> psd_Data.raw_Value >> psd_Data.argument >> psd_Data.value
				  >> psd_Data.PSD_Type >> psd_Data.argument_Units >> psd_Data.value_Units >> psd_Data.filepath;
}

QDataStream& operator <<( QDataStream& stream, const Imperfections_Model& imperfections_Model )
{
	return stream << imperfections_Model.use_Interlayer << imperfections_Model.use_Func

				  << imperfections_Model.show_Drift
				  << imperfections_Model.show_Thickness_Drift_Line << imperfections_Model.show_Thickness_Drift_Rand << imperfections_Model.show_Thickness_Drift_Sine
				  << imperfections_Model.show_Sigma_Drift_Line << imperfections_Model.show_Sigma_Drift_Rand << imperfections_Model.show_Sigma_Drift_Sine

				  << imperfections_Model.use_Roughness << imperfections_Model.approximation << imperfections_Model.PSD_Model
				  << imperfections_Model.vertical_Correlation << imperfections_Model.vertical_Inheritance_Frequency

				  << imperfections_Model.add_Measured_PSD_1D << imperfections_Model.add_Measured_PSD_2D
				  << imperfections_Model.add_Gauss_Peak << imperfections_Model.use_Common_Roughness_Function
				  << imperfections_Model.inheritance_Model << imperfections_Model.reflectivity_With_Roughness << imperfections_Model.DWBA_n_Max_Series << imperfections_Model.nu_Limit


				  << imperfections_Model.PSD_1D << imperfections_Model.PSD_2D

				  << imperfections_Model.use_Particles << imperfections_Model.use_Particles_Material << imperfections_Model.particle_Vertical_Correlation << imperfections_Model.initial_Particle_Shape
				  << imperfections_Model.initial_Interference_Function << imperfections_Model.initial_Geometric_Model << imperfections_Model.use_Common_Particle_Function;
}
QDataStream& operator >>( QDataStream& stream,		 Imperfections_Model& imperfections_Model )
{
	stream >> imperfections_Model.use_Interlayer >> imperfections_Model.use_Func

		   >> imperfections_Model.show_Drift
		   >> imperfections_Model.show_Thickness_Drift_Line >> imperfections_Model.show_Thickness_Drift_Rand >> imperfections_Model.show_Thickness_Drift_Sine
		   >> imperfections_Model.show_Sigma_Drift_Line >> imperfections_Model.show_Sigma_Drift_Rand >> imperfections_Model.show_Sigma_Drift_Sine

		   >> imperfections_Model.use_Roughness >> imperfections_Model.approximation >> imperfections_Model.PSD_Model
		   >> imperfections_Model.vertical_Correlation;

    // change model name. fractal_Gauss_Model is deprecated
    if(imperfections_Model.PSD_Model == fractal_Gauss_Model) {
        imperfections_Model.PSD_Model = stretched_exp_Model;
    }

	if(Global_Variables::check_Loaded_Version(1,11,9))
	{stream >> imperfections_Model.vertical_Inheritance_Frequency;}

	if(Global_Variables::check_Loaded_Version(1,11,16))
	{stream >> imperfections_Model.add_Measured_PSD_1D >> imperfections_Model.add_Measured_PSD_2D;}

	if(Global_Variables::check_Loaded_Version(1,11,7))
	{stream >> imperfections_Model.add_Gauss_Peak;}

	stream >> imperfections_Model.use_Common_Roughness_Function;

	if(Global_Variables::check_Loaded_Version(1,11,7))
	{
		stream >> imperfections_Model.inheritance_Model;
		if(!Global_Variables::check_Loaded_Version(1,11,13))
		{
			if(imperfections_Model.inheritance_Model == "Linear growth") {
				imperfections_Model.inheritance_Model = linear_Growth_Alpha_Inheritance_Model;
			}
		}
	}

	if(Global_Variables::check_Loaded_Version(1,11,12))
    {stream >> imperfections_Model.reflectivity_With_Roughness >> imperfections_Model.DWBA_n_Max_Series;

        // typo fix
        if(imperfections_Model.reflectivity_With_Roughness == Debye_Waller_R_old)
            imperfections_Model.reflectivity_With_Roughness = Debye_Waller_R;
    }

	if(Global_Variables::check_Loaded_Version(1,11,17))
	{stream >> imperfections_Model.nu_Limit;}

	if(Global_Variables::check_Loaded_Version(1,11,10))
	{stream >> imperfections_Model.PSD_1D >> imperfections_Model.PSD_2D;}

	stream >> imperfections_Model.use_Particles;
	if(Global_Variables::check_Loaded_Version(1,11,22))
	{stream>> imperfections_Model.use_Particles_Material;}

	stream >> imperfections_Model.particle_Vertical_Correlation >> imperfections_Model.initial_Particle_Shape
		   >> imperfections_Model.initial_Interference_Function >> imperfections_Model.initial_Geometric_Model;

	if(Global_Variables::check_Loaded_Version(1,11,22))
	{stream >> imperfections_Model.use_Common_Particle_Function;}

	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Calc_Functions& calc_Functions )
{
	return stream << calc_Functions.check_Enabled << calc_Functions.check_Reflectance << calc_Functions.check_Transmittance << calc_Functions.check_Absorptance
				  << calc_Functions.check_Field	  << calc_Functions.check_Joule << calc_Functions.check_Scattering << calc_Functions.check_GISAS
				  << calc_Functions.field_Step << calc_Functions.depth_Units
				  << calc_Functions.field_Ambient_Distance << calc_Functions.field_Surface_Distance << calc_Functions.field_Substrate_Distance
				  << calc_Functions.show_Surface << calc_Functions.show_Substrate
				  << calc_Functions.use_Surface_Distance
				  << calc_Functions.add_Specular_Peak << calc_Functions.instrumental_Smoothing << calc_Functions.integrate_PSD_in_Detector;
}
QDataStream& operator >>( QDataStream& stream,		 Calc_Functions& calc_Functions )
{
	stream >> calc_Functions.check_Enabled >> calc_Functions.check_Reflectance >> calc_Functions.check_Transmittance >> calc_Functions.check_Absorptance
		   >> calc_Functions.check_Field   >> calc_Functions.check_Joule;

	if(!Global_Variables::check_Loaded_Version(1,11,0))
	{
		bool check_User;
		stream	>> check_User;
		QString user_Functions;
		stream >> user_Functions;
	} else
	{
		stream >> calc_Functions.check_Scattering >> calc_Functions.check_GISAS;
	}

	if(Global_Variables::check_Loaded_Version(1,10,3))
	{
		stream >> calc_Functions.field_Step;
		if(Global_Variables::check_Loaded_Version(1,11,26))
		{stream >> calc_Functions.depth_Units;}
		stream >> calc_Functions.field_Ambient_Distance;

		if(Global_Variables::check_Loaded_Version(1,11,25))
		{stream >> calc_Functions.field_Surface_Distance;}

		stream >> calc_Functions.field_Substrate_Distance;

		if(Global_Variables::check_Loaded_Version(1,11,8))
		{stream >> calc_Functions.show_Surface;}
		if(Global_Variables::check_Loaded_Version(1,11,25))
		{stream >> calc_Functions.show_Substrate;}

		if(Global_Variables::check_Loaded_Version(1,11,25))
		{stream >> calc_Functions.use_Surface_Distance;}
	}

	if(Global_Variables::check_Loaded_Version(1,11,0))
	{stream >> calc_Functions.add_Specular_Peak >> calc_Functions.instrumental_Smoothing;}

	if(Global_Variables::check_Loaded_Version(1,11,15))
	{stream >> calc_Functions.integrate_PSD_in_Detector;}

	return stream;
}

// -----------------------------------------------------------------------------------------

bool operator ==( const Parameter_Indicator& parameter_Indicator_Left, const Parameter_Indicator& parameter_Indicator_Right )
{
	return	(parameter_Indicator_Left.id		 == parameter_Indicator_Right.id		) &&
			(parameter_Indicator_Left.item_Id	 == parameter_Indicator_Right.item_Id	) &&
			(parameter_Indicator_Left.whats_This == parameter_Indicator_Right.whats_This);
}

bool operator ==( const Different_Norm_Layer& different_Norm_Layer_Left, const Different_Norm_Layer& different_Norm_Layer_Right )
{
	return	/*(different_Norm_Layer_Left.interlayer_Composition_Weight_Left		 == different_Norm_Layer_Right.interlayer_Composition_Weight_Left		) &&
			(different_Norm_Layer_Left.interlayer_My_Sigma_Left	 == different_Norm_Layer_Right.interlayer_My_Sigma_Left	) &&
			(different_Norm_Layer_Left.interlayer_Composition_Weight_Right == different_Norm_Layer_Right.interlayer_Composition_Weight_Right) &&
			(different_Norm_Layer_Left.interlayer_My_Sigma_Right == different_Norm_Layer_Right.interlayer_My_Sigma_Right) &&*/
			(different_Norm_Layer_Left.sigma_Diffuse_Left == different_Norm_Layer_Right.sigma_Diffuse_Left) &&
			(different_Norm_Layer_Left.sigma_Diffuse_Right == different_Norm_Layer_Right.sigma_Diffuse_Right) &&
			(different_Norm_Layer_Left.thickness == different_Norm_Layer_Right.thickness);
}

// optical constants

void Point::read_Row(QTextStream& input, bool if_Factors)
{
	if(if_Factors)
	{
		double temp_Energy;
		input >> temp_Energy >> re >> im;
		lambda = Global_Variables::angstrom_eV(temp_Energy);
	} else
	{        
		input >> lambda >> re >> im;
	}
}

void Material_Data::read_Material(QString& filename)
{
	// TODO
	if(filename == QString(Vacuum)+nk_Ext)
	{
		// several point for good interpolation
		for(int i=0; i<=20; i++)
		{
			Point vacuum_nk_Point;
			vacuum_nk_Point.lambda = i*100;
			vacuum_nk_Point.re = 1;
			vacuum_nk_Point.im = 0;
			material_Data.append(vacuum_nk_Point);
		}
		// last very big point
		Point vacuum_nk_Point;
			vacuum_nk_Point.lambda = DBL_MAX;
			vacuum_nk_Point.re = 1;
			vacuum_nk_Point.im = 0;
		material_Data.append(vacuum_nk_Point);
	} else
	{
		QFile file(nk_Path + filename);
		file.open(QIODevice::ReadOnly);
		QTextStream input(&file);
		Point new_nk_Point;
		bool growth = true;
		while(!input.atEnd())
		{
			QString temp_Line = input.readLine();

			// check if header
			temp_Line = temp_Line.simplified();
			bool is_Decimal = false;
			QString(temp_Line[0]).toInt(&is_Decimal);

			if(is_Decimal) // temp_Line[0]!=';' less common
			{
				QTextStream temp_Stream(&temp_Line);
				new_nk_Point.read_Row(temp_Stream, false);

				if(material_Data.size()==0)
				{
					material_Data.append(new_nk_Point);
				}
				if(material_Data.size()==1 && (abs(new_nk_Point.lambda - material_Data.last().lambda)>DBL_EPSILON)) // not equal
				{
					material_Data.append(new_nk_Point);
				}
				if(material_Data.size()==2 )
				{
					if(material_Data[1].lambda < material_Data[0].lambda) {growth = false;} else {growth = true;}

				}
				if(material_Data.size()>=2)
				{
					if(growth)
					{
						if(new_nk_Point.lambda > material_Data.last().lambda)
						{
							material_Data.append(new_nk_Point);
						}
					} else
					{
						if(new_nk_Point.lambda < material_Data.last().lambda)
						{
							material_Data.append(new_nk_Point);
						}
					}
				}
			}			
		}
                bool had_one_point = (material_Data.size()==1);
		// add 2 points before
		{
			Point first_nk_Point = material_Data.first();
			if(growth)
			{
				first_nk_Point.lambda*=0.99995;
				material_Data.prepend(first_nk_Point);
				first_nk_Point.lambda*=0.99995;
				material_Data.prepend(first_nk_Point);
			} else
			{
				first_nk_Point.lambda*=1.00005;
				material_Data.prepend(first_nk_Point);
				first_nk_Point.lambda*=1.00005;
				material_Data.prepend(first_nk_Point);
			}
		}
		// add 2 points after
		{
			Point last_nk_Point = material_Data.last();
			if(growth)
			{
				last_nk_Point.lambda*=1.00005;
				material_Data.append(last_nk_Point);
				last_nk_Point.lambda*=1.00005;
				material_Data.append(last_nk_Point);
			} else
			{
				last_nk_Point.lambda*=0.99995;
				material_Data.append(last_nk_Point);
				last_nk_Point.lambda*=0.99995;
				material_Data.append(last_nk_Point);
			}
		}
                // use whole range for one point
                if(had_one_point) {
                    Point first_nk_Point = material_Data.first();
                    first_nk_Point.lambda = 0.0;
                    material_Data.prepend(first_nk_Point);

                    Point last_nk_Point = material_Data.last();
                    last_nk_Point.lambda = MAX_DOUBLE;
                    material_Data.append(last_nk_Point);
                }
		// final vector should be increasing
		if(!growth) {std::reverse(material_Data.begin(), material_Data.end());}

		file.close();
	}
}

void Element_Data::read_Element(QString& filename)
{
	// TODO
	QFile file(f1f2_Path + filename);
	file.open(QIODevice::ReadOnly);
	QTextStream input(&file);
	while(!input.atEnd())
	{
		QString temp_Line = input.readLine();
		if(temp_Line[0]!=';')
		{
			Point new_f1f2_Point;
			QTextStream temp_Stream(&temp_Line);
			new_f1f2_Point.read_Row(temp_Stream, true);
			if(new_f1f2_Point.re>-8888)
			if(element_Data.size()==0 || (new_f1f2_Point.lambda < element_Data.first().lambda))
			{
				element_Data.prepend(new_f1f2_Point);
			}
		}
	}
	file.close();
}

bool PSD_Data::is_Loaded() const
{
	if(argument.size()>2)
		return true;
	else
		return false;
}

double PSD_Data::calc_Sigma_Full() const
{
	if(is_Loaded())
	{
		double sigma2 = 0;
		double dnu;
		if(PSD_Type == PSD_Type_1D)
		{
			/// integrating by rectangles
//			// first point
//			dnu = argument[1] - argument[0];
//			sigma2 += value[0]*dnu;

//			// intermediate points
//			for(int i=1; i<argument.size()-1; i++)
//			{
//				dnu = (argument[i+1] - argument[i-1])/2;
//				sigma2 += value[i]*dnu;
//			}

//			// last point
//			dnu = argument[argument.size()-1] - argument[argument.size()-2];
//			sigma2 += value[argument.size()-1]*dnu;

			/// integrating by trapezoids
			for(int i=0; i<argument.size()-1; i++)
			{
				dnu = argument[i+1] - argument[i];
				sigma2 += (value[i] + value[i+1])/2 * dnu;
			}
		}
		if(PSD_Type == PSD_Type_2D)
		{
			/// integrating by rectangles
//			// first point
//			dnu = argument[1] - argument[0];
//			sigma2 += value[0]*dnu * 2*M_PI*argument[0];

//			// intermediate points
//			for(int i=1; i<argument.size()-1; i++)
//			{
//				dnu = (argument[i+1] - argument[i-1])/2;
//				sigma2 += value[i]*dnu * 2*M_PI*argument[i];
//			}

//			// last point
//			dnu = argument[argument.size()-1] - argument[argument.size()-2];
//			sigma2 += value[argument.size()-1]*dnu * 2*M_PI*argument[argument.size()-1];

			/// integrating by trapezoids
			for(int i=0; i<argument.size()-1; i++)
			{
				dnu = argument[i+1] - argument[i];
				sigma2 += M_PI*(value[i]*argument[i] + value[i+1]*argument[i+1]) * dnu;
			}
		}
		return sqrt(sigma2);
	} else
	{
		return 0;
	}
}

double PSD_Data::calc_Sigma_Effective(double nu_Min, double nu_Max) const
{
	if(is_Loaded())
	{
		double arg_Min = argument.front();
		double arg_Max = argument.back();

		int i_Min = 0;
		int i_Max = argument.size()-1;

		// nu_Min < arg_Min < arg_Max < nu_Max
		if(nu_Min < arg_Min && arg_Max < nu_Max)
		{
			return calc_Sigma_Full();
		} else
		// arg_Min < arg_Max < nu_Min < nu_Max
		if(arg_Min < nu_Min && arg_Max < nu_Min)
		{
			return 0.;
		} else
		// nu_Min < nu_Max < arg_Min < arg_Max
		if(arg_Max < nu_Min && arg_Max < nu_Max)
		{
			return 0.;
		} else
		// other cases
		{
			// first point
			for(i_Min = 0; i_Min<argument.size(); i_Min++) {
				if(argument[i_Min] > nu_Min) break;
			}
			// last point
			for(i_Max = argument.size()-1; i_Max>=0; i_Max--) {
				if(argument[i_Max] < nu_Max) break;
			}

			double sigma2 = 0;
			double dnu;
			if(PSD_Type == PSD_Type_1D)
			{
				/// integrating by trapezoids
				for(int i=i_Min; i<i_Max; i++)
				{
					dnu = argument[i+1] - argument[i];
					sigma2 += (value[i] + value[i+1])/2 * dnu;
				}
				// first point, if necessary
				if(i_Min>0) {
					double value_At_Nu_Min = value[i_Min] + (value[i_Min-1] - value[i_Min]) * (argument[i_Min] - nu_Min)/(argument[i_Min] - argument[i_Min-1]);
					sigma2 += (value_At_Nu_Min + value[i_Min])/2 * (argument[i_Min] - nu_Min);
				}
				// last point, if necessary
				if(i_Max<argument.size()-1) {
					double value_At_Nu_Max = value[i_Max] + (value[i_Max+1] - value[i_Max]) * (nu_Max - argument[i_Max])/(argument[i_Max+1] - argument[i_Max]);
					sigma2 += (value[i_Max] + value_At_Nu_Max)/2 * (nu_Max - argument[i_Max]);
				}
			}
			if(PSD_Type == PSD_Type_2D)
			{
				/// integrating by trapezoids
				for(int i=i_Min; i<i_Max; i++)
				{
					dnu = argument[i+1] - argument[i];
					sigma2 += M_PI*(value[i]*argument[i] + value[i+1]*argument[i+1]) * dnu;
				}
				// first point, if necessary
				if(i_Min>0) {
					double value_At_Nu_Min = value[i_Min] + (value[i_Min-1] - value[i_Min]) * (argument[i_Min] - nu_Min)/(argument[i_Min] - argument[i_Min-1]);
					sigma2 +=  M_PI*(value_At_Nu_Min*nu_Min + value[i_Min]*argument[i_Min]) * (argument[i_Min] - nu_Min);
				}
				// last point, if necessary
				if(i_Max<argument.size()-1) {
					double value_At_Nu_Max = value[i_Max] + (value[i_Max+1] - value[i_Max]) * (nu_Max - argument[i_Max])/(argument[i_Max+1] - argument[i_Max]);
					sigma2 +=  M_PI*(value[i_Max]*argument[i_Max] + value_At_Nu_Max*nu_Max) * (nu_Max - argument[i_Max]);
				}
			}
			return sqrt(sigma2);
		}
	} else
	{
		return 0;
	}
}
