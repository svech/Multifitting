#ifndef SPECULAR_INDEPENDENT_CURVE_PART_H
#define SPECULAR_INDEPENDENT_CURVE_PART_H

#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"

class Specular_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Specular_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

signals:

};

#endif // SPECULAR_INDEPENDENT_CURVE_PART_H
