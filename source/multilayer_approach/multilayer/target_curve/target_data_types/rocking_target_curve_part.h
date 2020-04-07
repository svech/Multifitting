#ifndef ROCKING_TARGET_CURVE_PART_H
#define ROCKING_TARGET_CURVE_PART_H

#include "multilayer_approach/multilayer/target_curve/distribution_editor.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve_plot.h"

class Rocking_Target_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Rocking_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent = nullptr);

signals:

};

#endif // ROCKING_TARGET_CURVE_PART_H
