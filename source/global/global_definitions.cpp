#include "global_definitions.h"
#include "global_variables.h"
#include <algorithm>    // std::reverse

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
	return stream >> parameter_Indicator.id >> parameter_Indicator.item_Id >> parameter_Indicator.tab_Index >> parameter_Indicator.full_Name >> parameter_Indicator.whats_This >> parameter_Indicator.expression >> parameter_Indicator.exist;
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
	return stream << min_Max.thickness_Min << min_Max.thickness_Max << min_Max.sigma_Min << min_Max.sigma_Max;  // since 1.8.11
}
QDataStream& operator >>( QDataStream& stream,		 Min_Max& min_Max )
{
	if(Global_Variables::check_Loaded_Version(1,8,11))
	{      stream >> min_Max.thickness_Min >> min_Max.thickness_Max >> min_Max.sigma_Min >> min_Max.sigma_Max;} // since 1.8.11

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
	return stream << interlayer.interlayer << interlayer.my_Sigma << interlayer.enabled;
}
QDataStream& operator >>( QDataStream& stream,		 Interlayer& interlayer )
{
	return stream >> interlayer.interlayer >> interlayer.my_Sigma >> interlayer.enabled;
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
	return stream << plot_Options.y_Scale
				  << plot_Options.x_Scale // since 1.9.3

				  << plot_Options.rescale // since 1.7.6

				  << plot_Options.color        << plot_Options.scatter_Shape << plot_Options.scatter_Size        << plot_Options.thickness
				  << plot_Options.scale_Second << plot_Options.color_Second << plot_Options.scatter_Shape_Second << plot_Options.scatter_Size_Second << plot_Options.thickness_Second;
}
QDataStream& operator >>( QDataStream& stream,		 Plot_Options& plot_Options )
{
	stream >> plot_Options.y_Scale;

	if(Global_Variables::check_Loaded_Version(1,9,3))
	{stream >> plot_Options.x_Scale;}	// since 1.9.3


	if(Global_Variables::check_Loaded_Version(1,7,6))
	{stream >> plot_Options.rescale; }	// since 1.7.6

	stream >> plot_Options.color        >> plot_Options.scatter_Shape >> plot_Options.scatter_Size        >> plot_Options.thickness
		   >> plot_Options.scale_Second >> plot_Options.color_Second >> plot_Options.scatter_Shape_Second >> plot_Options.scatter_Size_Second >> plot_Options.thickness_Second;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Graph_Options& graph_Options )
{
	return stream << graph_Options.num_Target_Graph_Rows << graph_Options.num_Independent_Graph_Rows
				  << graph_Options.show_Scatter << graph_Options.show_Thickness << graph_Options.show_X_Scale
				  << graph_Options.show_Max_Value << graph_Options.show_Current_Coordinate << graph_Options.show_Title;
}
QDataStream& operator >>( QDataStream& stream,		 Graph_Options& graph_Options )
{
	stream >> graph_Options.num_Target_Graph_Rows >> graph_Options.num_Independent_Graph_Rows
		   >> graph_Options.show_Scatter >> graph_Options.show_Thickness >> graph_Options.show_X_Scale
		   >> graph_Options.show_Max_Value >> graph_Options.show_Current_Coordinate >> graph_Options.show_Title;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Profile_Plot_Options& profile_Plot_Options )
{
	return stream << profile_Plot_Options.type << profile_Plot_Options.permittivity_Type
				  << profile_Plot_Options.apply_Roughness << profile_Plot_Options.apply_Diffusiness
				  << profile_Plot_Options.show_Sharp_Profile << profile_Plot_Options.show_Discretization << profile_Plot_Options.show_Cursor_Position
				  << profile_Plot_Options.rescale_X << profile_Plot_Options.rescale_Y << profile_Plot_Options.y_Scale
				  << profile_Plot_Options.old_X_Begin << profile_Plot_Options.old_X_End << profile_Plot_Options.old_Y_Begin << profile_Plot_Options.old_Y_End
				  << profile_Plot_Options.local_wavelength_units << profile_Plot_Options.local_length_units;
}
QDataStream& operator >>( QDataStream& stream,		 Profile_Plot_Options& profile_Plot_Options )
{
	stream >> profile_Plot_Options.type >> profile_Plot_Options.permittivity_Type
		   >> profile_Plot_Options.apply_Roughness >> profile_Plot_Options.apply_Diffusiness
		   >> profile_Plot_Options.show_Sharp_Profile >> profile_Plot_Options.show_Discretization >> profile_Plot_Options.show_Cursor_Position
		   >> profile_Plot_Options.rescale_X >> profile_Plot_Options.rescale_Y >> profile_Plot_Options.y_Scale
		   >> profile_Plot_Options.old_X_Begin >> profile_Plot_Options.old_X_End >> profile_Plot_Options.old_Y_Begin >> profile_Plot_Options.old_Y_End
		   >> profile_Plot_Options.local_wavelength_units >> profile_Plot_Options.local_length_units;
	return stream;
}

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

QDataStream& operator <<( QDataStream& stream, const Profile_Plot_Data& profile_Plot_Data )
{
	return stream << profile_Plot_Data.sharp_Set << profile_Plot_Data.smooth_Set << profile_Plot_Data.discrete_Set << profile_Plot_Data.local_Wavelength;
}
QDataStream& operator >>( QDataStream& stream,		 Profile_Plot_Data& profile_Plot_Data )
{
	stream >> profile_Plot_Data.sharp_Set >> profile_Plot_Data.smooth_Set >> profile_Plot_Data.discrete_Set >> profile_Plot_Data.local_Wavelength;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Calculated_Values& calculated_Values )
{
	return stream << calculated_Values.R << calculated_Values.Phi_R
				  << calculated_Values.T
				  << calculated_Values.A;
}
QDataStream& operator >>( QDataStream& stream,		 Calculated_Values& calculated_Values )
{
	return stream >> calculated_Values.R >> calculated_Values.Phi_R
				  >> calculated_Values.T
				  >> calculated_Values.A;
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

QDataStream& operator <<( QDataStream& stream, const Calc_Functions& calc_Functions )
{
	return stream << calc_Functions.check_Enabled << calc_Functions.check_Reflectance << calc_Functions.check_Transmittance << calc_Functions.check_Absorptance
				  << calc_Functions.check_Field	  << calc_Functions.check_Joule		  << calc_Functions.check_User			<< calc_Functions.user_Functions;
}
QDataStream& operator >>( QDataStream& stream,		 Calc_Functions& calc_Functions )
{
	return stream >> calc_Functions.check_Enabled >> calc_Functions.check_Reflectance >> calc_Functions.check_Transmittance >> calc_Functions.check_Absorptance
				  >> calc_Functions.check_Field	  >> calc_Functions.check_Joule		  >> calc_Functions.check_User			>> calc_Functions.user_Functions;
}


QDataStream& operator <<( QDataStream& stream, const Value& value )
{
	return stream << value.val_1 << value.val_2;
}
QDataStream& operator >>( QDataStream& stream,		 Value& value )
{
	return stream >> value.val_1 >> value.val_2;
}

QDataStream& operator <<( QDataStream& stream, const Curve& curve )
{
	return stream << curve.use_Subinterval << curve.subinterval_Start << curve.subinterval_End // since 1.10.1
				  << curve.argument << curve.shifted_Argument << curve.values << curve.shifted_Values << curve.arg_Offset << curve.arg_Factor << curve.val_Offset << curve.val_Factor
				  << curve.beam_Intensity_Start << curve.beam_Intensity_Final	// since 1.9.3
				  << curve. divide_On_Beam_Intensity							// since 1.8.1

				  << curve.argument_Type << curve.angle_Type << curve.angular_Units << curve.spectral_Units << curve.value_Function << curve.value_Mode;
}
QDataStream& operator >>( QDataStream& stream,		 Curve& curve )
{
	if(Global_Variables::check_Loaded_Version(1,10,1))		// since 1.10.1
	{
		stream >> curve.use_Subinterval >> curve.subinterval_Start >> curve.subinterval_End;
	}

	stream  >> curve.argument >> curve.shifted_Argument >> curve.values >> curve.shifted_Values >> curve.arg_Offset >> curve.arg_Factor >> curve.val_Offset >> curve.val_Factor;

	if(Global_Variables::check_Loaded_Version(1,7,1))		// since 1.7.1
	{
		if(Global_Variables::check_Loaded_Version(1,9,3))
		{stream >> curve.beam_Intensity_Start >> curve.beam_Intensity_Final; } 		// since 1.9.3
		else
		{
			stream >> curve.beam_Intensity_Start;
			curve.beam_Intensity_Final = curve.beam_Intensity_Start;
		}
	}

	if(Global_Variables::check_Loaded_Version(1,8,1))
	{stream >> curve.divide_On_Beam_Intensity; }	// since 1.8.1

	stream  >> curve.argument_Type >> curve.angle_Type >> curve.angular_Units >> curve.spectral_Units >> curve.value_Function >> curve.value_Mode;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Fit_Params& fit_Params )
{
	return stream << fit_Params.calc << fit_Params.fit << fit_Params.norm
				  << fit_Params.maximize_Integral // since 1.9.3
				  << fit_Params.weight << fit_Params.fit_Function
				  << fit_Params.use_Chi2 // since 1.8.1
				  << fit_Params.power; // since 1.8.8
}
QDataStream& operator >>( QDataStream& stream,		 Fit_Params& fit_Params )
{
	stream >> fit_Params.calc >> fit_Params.fit >> fit_Params.norm;

	if(Global_Variables::check_Loaded_Version(1,9,3))
	{stream >> fit_Params.maximize_Integral; }			// since 1.9.3

	stream >> fit_Params.weight >> fit_Params.fit_Function;

	if(Global_Variables::check_Loaded_Version(1,8,1))
	{stream >> fit_Params.use_Chi2; }		// since 1.8.1

	if(Global_Variables::check_Loaded_Version(1,8,8) && !Global_Variables::check_Loaded_Version(1,9,3))
	{
		int int_Power;
		stream >> int_Power;
		fit_Params.power = double(int_Power);	// since 1.8.8
	}

	if(Global_Variables::check_Loaded_Version(1,9,3))
	{stream >> fit_Params.power; }			// since 1.9.3

	return stream;
}

// -----------------------------------------------------------------------------------------

bool operator ==( const Parameter_Indicator& parameter_Indicator_Left, const Parameter_Indicator& parameter_Indicator_Right )
{
	return	(parameter_Indicator_Left.id		 == parameter_Indicator_Right.id		) &&
			(parameter_Indicator_Left.item_Id	 == parameter_Indicator_Right.item_Id	) &&
			(parameter_Indicator_Left.whats_This == parameter_Indicator_Right.whats_This);
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
