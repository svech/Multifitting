#ifndef SPECULAR_TARGET_CURVE_PART_H
#define SPECULAR_TARGET_CURVE_PART_H

#include "multilayer_approach/multilayer/target_curve/distribution_editor.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve_plot.h"

class Specular_Target_Curve_Part : public QWidget
{
	Q_OBJECT
public:
	explicit Specular_Target_Curve_Part(Target_Curve* target_Curve, Target_Curve_Plot* target_Curve_Plot, QWidget *parent = nullptr);

	void create_Argument_GroupBox();
	void create_Value_GroupBox();
	void create_Beam_GroupBox();
	void create_Detector_GroupBox();

	QVBoxLayout* main_Layout;
};

#endif // SPECULAR_TARGET_CURVE_PART_H
