#ifndef LAYER_CONTENT_H
#define LAYER_CONTENT_H

#include <QtWidgets>
#include "global_definitions.h"

class Ambient
{
public:
	Ambient();

	QString material;
	double density;
	QList<stoichiometry> composition;
};

class Substrate : public Ambient
{
public:
	Substrate();

	double sigma;
	QList<stoichiometry> interlayer_Composition;
};

class Extreme_Layer: public Substrate
{
public:
	Extreme_Layer();

	double thickness;
};

class Layer : public Extreme_Layer
{
public:
	Layer();

	double drift;
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

	int num_Repetition;
	double period;
	double gamma;
};



#endif // LAYER_CONTENT_H
