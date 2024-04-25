#ifndef INDEPENDENT_CURVE_EDITOR_H
#define INDEPENDENT_CURVE_EDITOR_H

#include "multilayer_approach/multilayer/multilayer.h"
#include "independent_curve.h"

#include "independent_data_types/specular_independent_curve_part.h"
#include "independent_data_types/detector_independent_curve_part.h"
#include "independent_data_types/rocking_independent_curve_part.h"
#include "independent_data_types/offset_independent_curve_part.h"
#include "independent_data_types/gisas_independent_curve_part.h"

class Specular_Independent_Curve_Part;
class Detector_Independent_Curve_Part;
class Rocking_Independent_Curve_Part;
class Offset_Independent_Curve_Part;
class GISAS_Independent_Curve_Part;
class Multilayer;

class Independent_Curve_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Independent_Curve_Editor(Independent_Curve* independent_Curve, Multilayer* multilayer_Parent, QWidget *parent = 0);

    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
	void create_Main_Part();
	void write_Window_Geometry();

	Independent_Curve* independent_Curve;
	Multilayer* multilayer_Parent;

	QGridLayout* main_Layout;
		Specular_Independent_Curve_Part* specular_Independent_Curve_Part;
		Detector_Independent_Curve_Part* detector_Independent_Curve_Part;
		Rocking_Independent_Curve_Part* rocking_Independent_Curve_Part;
		Offset_Independent_Curve_Part* offset_Independent_Curve_Part;
		GISAS_Independent_Curve_Part* gisas_Independent_Curve_Part;

		QPushButton* done_Button;
};

#endif // INDEPENDENT_CURVE_EDITOR_H
