#ifndef LAYER_CONTENT_H
#define LAYER_CONTENT_H

#include "global_definitions.h"

class Probe_Angle
{
public:
	Probe_Angle();

	Independent angle;

	double angular_Resolution;
	Angle_Type type;
};

class Radiation
{
public:
	Radiation();

	Independent radiation;
	double spectral_Resolution;
	double polarization;
	double polarization_Sensitivity;
};

class Ambient
{
public:
	Ambient();

	bool composed_Material;

	QString material;
	Parameter density;

	// tabular material
	Tril separate_Optical_Constants = Tril::NotDefined;
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
	QVector<Interlayer_Enabled> interlayer_Composition;
};

class Extreme_Layer: public Substrate
{
public:
	Extreme_Layer();

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

Q_DECLARE_METATYPE( Probe_Angle )
Q_DECLARE_METATYPE( Radiation )
Q_DECLARE_METATYPE( Ambient )
Q_DECLARE_METATYPE( Substrate )
Q_DECLARE_METATYPE( Layer )
Q_DECLARE_METATYPE( Stack_Content )

#endif // LAYER_CONTENT_H
