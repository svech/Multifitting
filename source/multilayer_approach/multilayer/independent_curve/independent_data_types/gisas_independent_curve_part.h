#ifndef GISAS_INDEPENDENT_CURVE_PART_H
#define GISAS_INDEPENDENT_CURVE_PART_H

#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"

class GISAS_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit GISAS_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

signals:

};

#endif // GISAS_INDEPENDENT_CURVE_PART_H
