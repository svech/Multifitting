#include "global_definitions.h"
#include "global_variables.h"

Global_Definitions::Global_Definitions()
{

}

// -----------------------------------------------------------------------------------------
// serialization
QDataStream& operator <<( QDataStream& stream, const Int_Independent& int_Independent )
{
	return stream << int_Independent.value << int_Independent.is_Independent << int_Independent.start << int_Independent.step << int_Independent.num_steps;
}
QDataStream& operator >>( QDataStream& stream,		 Int_Independent& int_Independent )
{
	return stream >> int_Independent.value >> int_Independent.is_Independent >> int_Independent.start >> int_Independent.step >> int_Independent.num_steps;
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
	return stream << coupled.is_Coupled;
}
QDataStream& operator >>( QDataStream& stream,		 Coupled& coupled )
{
	return stream >> coupled.is_Coupled;
}

QDataStream& operator <<( QDataStream& stream, const Fit& fit )
{
	return stream << fit.is_Fitable << fit.min_Bounded << fit.min << fit.max_Bounded << fit.max;
}
QDataStream& operator >>( QDataStream& stream,		 Fit& fit )
{
	return stream >> fit.is_Fitable >> fit.min_Bounded >> fit.min >> fit.max_Bounded >> fit.max;
}

QDataStream& operator <<( QDataStream& stream, const Optimize& optimize )
{
	return stream << optimize.is_Optimizable << optimize.min_Bounded << optimize.min << optimize.max_Bounded << optimize.max;
}
QDataStream& operator >>( QDataStream& stream,		 Optimize& optimize )
{
	return stream >> optimize.is_Optimizable >> optimize.min_Bounded >> optimize.min >> optimize.max_Bounded >> optimize.max;
}

QDataStream& operator <<( QDataStream& stream, const Parameter& parameter )
{
	return stream << parameter.value << parameter.independent << parameter.coupled << parameter.optimize;
}
QDataStream& operator >>( QDataStream& stream,		 Parameter& parameter )
{
	return stream >> parameter.value >> parameter.independent >> parameter.coupled >> parameter.optimize;
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
	return stream << interlayer.interlayer << interlayer.enabled;
}
QDataStream& operator >>( QDataStream& stream,		 Interlayer& interlayer )
{
	return stream >> interlayer.interlayer >> interlayer.enabled;
}

// optical constants

void nk_Point::read_Row(QTextStream& input)
{
	input >> lambda >> n >> k;
}

void f1f2_Point::read_Row(QTextStream& input)
{
	double temp_Energy;
	input >> temp_Energy >> f1 >> f2;
	lambda = Global_Variables::angstrom_eV(temp_Energy);
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
			nk_Point new_nk_Point;
			QTextStream temp_Stream(&temp_Line);
			new_nk_Point.read_Row(temp_Stream);
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
			f1f2_Point new_f1f2_Point;
			QTextStream temp_Stream(&temp_Line);
			new_f1f2_Point.read_Row(temp_Stream);
			if(new_f1f2_Point.f1>-8888)
			{
				element_Data.prepend(new_f1f2_Point);
			}
		}
	}
	file.close();
}
