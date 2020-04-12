#ifndef OFFSET_INDEPENDENT_CURVE_PART_H
#define OFFSET_INDEPENDENT_CURVE_PART_H

#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"

class Offset_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Offset_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

signals:

};

#endif // OFFSET_INDEPENDENT_CURVE_PART_H
