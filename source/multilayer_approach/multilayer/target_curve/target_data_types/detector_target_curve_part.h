#ifndef DETECTOR_TARGET_CURVE_PART_H
#define DETECTOR_TARGET_CURVE_PART_H

#include "multilayer_approach/multilayer/distribution_editor.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve_plot.h"

class Detector_Target_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Detector_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent = nullptr);

};

#endif // DETECTOR_TARGET_CURVE_PART_H
