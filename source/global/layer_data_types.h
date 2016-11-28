#ifndef LAYER_DATA_TYPES_H
#define LAYER_DATA_TYPES_H

#include "global/settings.h"

class Measurement
{
public:
	Measurement();

	// angle
	Parameter probe_Angle;
	QVector<double>  cos2;
	double			 cos2_Value;
	Parameter angular_Resolution;
	QString	  angle_Type;

	// wavelength
	Parameter wavelength;
	QVector<double>    k;
	double			   k_Value;
	Parameter spectral_Resolution; // warning: can be in eV or keV
	Parameter polarization;
	Parameter polarization_Sensitivity;

	void calc_Independent_cos2_k();
};

class Ambient
{
public:
	Ambient();

	bool composed_Material;

	QString material;
	Parameter density;

	// tabular material
	QString separate_Optical_Constants = TRIL_NOT_DEFINED;
	Parameter permittivity;
	Parameter absorption;

	// composite material
	QList<Stoichiometry> composition;
};

class Substrate : public Ambient
{
public:
	Substrate();

	bool use_PSD = false;
	Parameter sigma;
	QVector<Interlayer> interlayer_Composition;
};

class Extreme_Layer: public Substrate
{
public:
	Extreme_Layer();
	// materials near main multilayers should be the same

	int layer_Index;
	Parameter thickness;
};

class Layer : public Extreme_Layer
{
public:
	Layer();

	QString drift_Model;
	QList<Parameter> drift_Coefficients;

	bool if_First = false;
	Extreme_Layer First;

	bool if_Last = false;
	Extreme_Layer Last;
};

class Stack_Content
{
public:
	Stack_Content();

	Int_Independent num_Repetition;

	// stack parameters
	Parameter period;
	Parameter gamma;
};

QDataStream& operator <<( QDataStream& stream, const Extreme_Layer& extreme_Layer );
QDataStream& operator >>( QDataStream& stream,		 Extreme_Layer& extreme_Layer );
QDataStream& operator <<( QDataStream& stream, const Layer& layer );
QDataStream& operator >>( QDataStream& stream,		 Layer& layer );
QDataStream& operator <<( QDataStream& stream, const Stack_Content& stack_Content );
QDataStream& operator >>( QDataStream& stream,		 Stack_Content& stack_Content );
QDataStream& operator <<( QDataStream& stream, const Substrate& substrate );
QDataStream& operator >>( QDataStream& stream,		 Substrate& substrate );
QDataStream& operator <<( QDataStream& stream, const Ambient& ambient );
QDataStream& operator >>( QDataStream& stream,		 Ambient& ambient );

Q_DECLARE_METATYPE( Measurement )
Q_DECLARE_METATYPE( Ambient )
Q_DECLARE_METATYPE( Substrate )
Q_DECLARE_METATYPE( Layer )
Q_DECLARE_METATYPE( Stack_Content )

#endif // LAYER_DATA_TYPES_H
