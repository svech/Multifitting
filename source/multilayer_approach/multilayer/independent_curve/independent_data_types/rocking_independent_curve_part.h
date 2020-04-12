#ifndef ROCKING_INDEPENDENT_CURVE_PART_H
#define ROCKING_INDEPENDENT_CURVE_PART_H

#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"

class Rocking_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Rocking_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

signals:

};

#endif // ROCKING_INDEPENDENT_CURVE_PART_H
