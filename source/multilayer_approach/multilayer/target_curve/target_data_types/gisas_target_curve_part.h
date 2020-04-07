#ifndef GISAS_TARGET_CURVE_PART_H
#define GISAS_TARGET_CURVE_PART_H

#include "multilayer_approach/multilayer/target_curve/distribution_editor.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve_plot.h"

class GISAS_Target_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit GISAS_Target_Curve_Part(Target_Curve* target_Curve, QWidget *parent = nullptr);

signals:

};

#endif // GISAS_TARGET_CURVE_PART_H
