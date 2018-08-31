// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "global_definitions.h"
#include "global_variables.h"

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
	return stream << int_Independent.value << int_Independent.is_Independent << int_Independent.start << int_Independent.step << int_Independent.num_Steps << int_Independent.id << int_Independent.whats_This;
}
QDataStream& operator >>( QDataStream& stream,		 Int_Independent& int_Independent )
{
	return stream >> int_Independent.value >> int_Independent.is_Independent >> int_Independent.start >> int_Independent.step >> int_Independent.num_Steps >> int_Independent.id >> int_Independent.whats_This;
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
	return stream << confidence.calc_Conf_Interval << confidence.min << confidence.max << confidence.num_Points;
}
QDataStream& operator >>( QDataStream& stream,		 Confidence& confidence )
{
	return stream >> confidence.calc_Conf_Interval >> confidence.min >> confidence.max >> confidence.num_Points;
}

QDataStream& operator <<( QDataStream& stream, const Parameter& parameter )
{
	return stream << parameter.value << parameter.independent << parameter.coupled << parameter.fit << parameter.indicator
				  << parameter.confidence;	// since 1.7.9
}
QDataStream& operator >>( QDataStream& stream,		 Parameter& parameter )
{
	stream >> parameter.value >> parameter.independent >> parameter.coupled >> parameter.fit >> parameter.indicator;
	if( loaded_Version_Major>=1 &&
		loaded_Version_Minor>=7 &&
		loaded_Version_Build>=9)
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
				  << drift.drift_Line_Value << drift.drift_Rand_Rms << drift.drift_Sine_Amplitude << drift.drift_Sine_Frequency << drift.drift_Sine_Phase;
}
QDataStream& operator >>( QDataStream& stream,		 Drift& drift )
{
	return stream >> drift.is_Drift_Line >> drift.is_Drift_Sine >> drift.is_Drift_Rand
				  >> drift.drift_Line_Value >> drift.drift_Rand_Rms >> drift.drift_Sine_Amplitude >> drift.drift_Sine_Frequency >> drift.drift_Sine_Phase;
}

QDataStream& operator <<( QDataStream& stream, const Plot_Options& plot_Options )
{
	return stream << plot_Options.scale
				  << plot_Options.rescale // since 1.7.6

				  << plot_Options.color        << plot_Options.scatter_Shape << plot_Options.scatter_Size        << plot_Options.thickness
				  << plot_Options.scale_Second << plot_Options.color_Second << plot_Options.scatter_Shape_Second << plot_Options.scatter_Size_Second << plot_Options.thickness_Second;
}
QDataStream& operator >>( QDataStream& stream,		 Plot_Options& plot_Options )
{
	stream >> plot_Options.scale;
	if( loaded_Version_Major>=1 &&
		loaded_Version_Minor>=7 &&
		loaded_Version_Build>=6) {stream >> plot_Options.rescale; }	// since 1.7.6

	stream >> plot_Options.color        >> plot_Options.scatter_Shape >> plot_Options.scatter_Size        >> plot_Options.thickness
		   >> plot_Options.scale_Second >> plot_Options.color_Second >> plot_Options.scatter_Shape_Second >> plot_Options.scatter_Size_Second >> plot_Options.thickness_Second;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Calculated_Values& calculated_Values )
{
	return stream << calculated_Values.R << calculated_Values.Phi
				  << calculated_Values.T
				  << calculated_Values.A;
}
QDataStream& operator >>( QDataStream& stream,		 Calculated_Values& calculated_Values )
{
	return stream >> calculated_Values.R >> calculated_Values.Phi
				  >> calculated_Values.T
				  >> calculated_Values.A;
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
	return stream << curve.argument << curve.shifted_Argument << curve.values << curve.shifted_Values << curve.arg_Offset << curve.arg_Factor << curve.val_Offset << curve.val_Factor
				  << curve.beam_Intensity	// since 1.7.1

				  << curve.argument_Type << curve.angle_Type << curve.angular_Units << curve.spectral_Units << curve.value_Function << curve.value_Mode;
}
QDataStream& operator >>( QDataStream& stream,		 Curve& curve )
{
	stream  >> curve.argument >> curve.shifted_Argument >> curve.values >> curve.shifted_Values >> curve.arg_Offset >> curve.arg_Factor >> curve.val_Offset >> curve.val_Factor;
	if( loaded_Version_Major>=1 &&
		loaded_Version_Minor>=7 &&
		loaded_Version_Build>=1)
	{stream >> curve.beam_Intensity; }	// since 1.7.1

	stream  >> curve.argument_Type >> curve.angle_Type >> curve.angular_Units >> curve.spectral_Units >> curve.value_Function >> curve.value_Mode;
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Fit_Params& fit_Params )
{
	return stream << fit_Params.calc << fit_Params.fit << fit_Params.norm  << fit_Params.weight << fit_Params.fit_Function;
}
QDataStream& operator >>( QDataStream& stream,		 Fit_Params& fit_Params )
{
	return stream >> fit_Params.calc >> fit_Params.fit >> fit_Params.norm  >> fit_Params.weight >> fit_Params.fit_Function;
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
	QFile file(nk_Path + filename);
	file.open(QIODevice::ReadOnly);
	QTextStream input(&file);
	while(!input.atEnd())
	{
		QString temp_Line = input.readLine();
		if(temp_Line[0]!=';')
		{
			Point new_nk_Point;
			QTextStream temp_Stream(&temp_Line);
			new_nk_Point.read_Row(temp_Stream, false);
			if(material_Data.size()==0 || (new_nk_Point.lambda > material_Data.last().lambda))
			{
				material_Data.append(new_nk_Point);
			}
		}
	}
	file.close();
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
			if(element_Data.size()==0 || (new_f1f2_Point.lambda < element_Data.last().lambda))
			{
				element_Data.prepend(new_f1f2_Point);
			}
		}
	}
	file.close();
}
