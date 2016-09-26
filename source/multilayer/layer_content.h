#ifndef LAYER_CONTENT_H
#define LAYER_CONTENT_H

#include "global_definitions.h"

class Ambient
{
public:
	Ambient();

	QString material;
	double density;

	bool composed;

	QList<Stoichiometry> composition;
};

class Substrate : public Ambient
{
public:
	Substrate();

	double sigma;
	QList<Stoichiometry> interlayer_Composition;
};

class Extreme_Layer: public Substrate
{
public:
	Extreme_Layer();

	int layer_Index;
	double thickness;
};

class Layer : public Extreme_Layer
{
public:
	Layer();

	QString drift_Model;
	QList<double> drift_Coefficients;

	bool if_First = 0;
		Extreme_Layer First;

	bool if_Last = 0;
		Extreme_Layer Last;
};

class Stack_Content
{
public:
	Stack_Content();

	int first_Layer_Index;
	int last_Layer_Index;

	int num_Repetition;
	double period;
	double gamma;
};

Q_DECLARE_METATYPE( Ambient )
Q_DECLARE_METATYPE( Substrate )
Q_DECLARE_METATYPE( Layer )
Q_DECLARE_METATYPE( Stack_Content )

#endif // LAYER_CONTENT_H
