#ifndef TARGET_CURVE_EDITOR_H
#define TARGET_CURVE_EDITOR_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/target_curve.h"

class Target_Curve_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Target_Curve_Editor(Target_Curve* target_Curve, QWidget *parent = 0);

private:
	void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
	void set_Window_Geometry();

	QVBoxLayout* main_Layout;

signals:

public slots:
};

#endif // TARGET_CURVE_EDITOR_H
