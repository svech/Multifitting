#ifndef LAYER_DATA_TYPES_H
#define LAYER_DATA_TYPES_H

#include "global/settings.h"
#include <complex>
using namespace std;

class Measurement
{
public:
	Measurement();

	// angle
	Parameter probe_Angle;
	Parameter angular_Resolution;
	QString	  angle_Type;

	// wavelength
	Parameter wavelength;
	Parameter spectral_Resolution; // warning: can be in eV or keV
	Parameter polarization;
	Parameter polarization_Sensitivity;
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

// tree node for calculation
typedef float floatX;

class Node
{
public:
	Node();
	Node(QTreeWidgetItem* item);

	QString whats_This;

	Ambient ambient;
	Layer layer;
	Stack_Content stack_Content;
	Substrate substrate;

	// for any media
	complex<floatX> epsilon;

	// for layers
	complex<floatX> exponenta;

	// for top interfaces
	complex<floatX> r_Fresnel_s;
	complex<floatX> r_Fresnel_p;
	floatX weak_Factor = 1;
};

#endif // LAYER_DATA_TYPES_H
