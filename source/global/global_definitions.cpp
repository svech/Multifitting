// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "global_definitions.h"
#include "global_variables.h"

Global_Definitions::Global_Definitions()
{

}

QVector<int> used_IDs;

int Global_Definitions::random_Id()
{
	int id = 0;
	bool contains = false;
	do
	{
		id = rand()*RAND_SHIFT+rand();
		if(used_IDs.contains(id))
		{
			qInfo() << "id "<<id<<"repeated. regenerating";
			contains = true;
		} else
		{
			used_IDs.append(id);
		}
	} while (contains);
	return id;
}

// -----------------------------------------------------------------------------------------
// serialization

QDataStream& operator <<( QDataStream& stream, const Independent_Indicator& independent_Indicator )
{
	return stream << independent_Indicator.item_Id << independent_Indicator.parameter_Id << independent_Indicator.item_Type << independent_Indicator.parameter_Whats_This << independent_Indicator.index << independent_Indicator.is_Active;
}
QDataStream& operator >>( QDataStream& stream,		 Independent_Indicator& independent_Indicator )
{
	return stream >> independent_Indicator.item_Id >> independent_Indicator.parameter_Id >> independent_Indicator.item_Type >> independent_Indicator.parameter_Whats_This >> independent_Indicator.index >> independent_Indicator.is_Active;
}

QDataStream& operator <<( QDataStream& stream, const Parameter_Indicator& parameter_Indicator )
{
	return stream << parameter_Indicator.id << parameter_Indicator.tab_Index << parameter_Indicator.full_Name << parameter_Indicator.whats_This << parameter_Indicator.expression << parameter_Indicator.exist;
}
QDataStream& operator >>( QDataStream& stream,		 Parameter_Indicator& parameter_Indicator )
{
	return stream >> parameter_Indicator.id >> parameter_Indicator.tab_Index >> parameter_Indicator.full_Name >> parameter_Indicator.whats_This >> parameter_Indicator.expression >> parameter_Indicator.exist;
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

QDataStream& operator <<( QDataStream& stream, const Parameter& parameter )
{
	return stream << parameter.value << parameter.independent << parameter.coupled << parameter.fit << parameter.indicator;
}
QDataStream& operator >>( QDataStream& stream,		 Parameter& parameter )
{
	return stream >> parameter.value >> parameter.independent >> parameter.coupled >> parameter.fit >> parameter.indicator;
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
				  << curve.argument_Type << curve.angle_Type << curve.angular_Units << curve.spectral_Units << curve.value_Function << curve.value_Mode;
}
QDataStream& operator >>( QDataStream& stream,		 Curve& curve )
{
	return stream >> curve.argument >> curve.shifted_Argument >> curve.values >> curve.shifted_Values >> curve.arg_Offset >> curve.arg_Factor >> curve.val_Offset >> curve.val_Factor
				  >> curve.argument_Type >> curve.angle_Type >> curve.angular_Units >> curve.spectral_Units >> curve.value_Function >> curve.value_Mode;
}

QDataStream& operator <<( QDataStream& stream, const Fit_Params& fit_Params )
{
	return stream << fit_Params.calc << fit_Params.fit << fit_Params.norm  << fit_Params.weight << fit_Params.fit_Function;
}
QDataStream& operator >>( QDataStream& stream,		 Fit_Params& fit_Params )
{
	return stream >> fit_Params.calc >> fit_Params.fit >> fit_Params.norm  >> fit_Params.weight >> fit_Params.fit_Function;
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
			material_Data.append(new_nk_Point);
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
			{
				element_Data.prepend(new_f1f2_Point);
			}
		}
	}
	file.close();
}
