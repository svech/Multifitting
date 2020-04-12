#ifndef DETECTOR_INDEPENDENT_CURVE_PART_H
#define DETECTOR_INDEPENDENT_CURVE_PART_H

#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"

class Detector_Independent_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Detector_Independent_Curve_Part(Independent_Curve* independent_Curve, QWidget *parent = nullptr);

signals:

};

#endif // DETECTOR_INDEPENDENT_CURVE_PART_H
