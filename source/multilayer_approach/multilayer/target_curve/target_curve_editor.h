#ifndef TARGET_CURVE_EDITOR_H
#define TARGET_CURVE_EDITOR_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"
#include "target_curve_plot.h"

#include "target_data_types/specular_target_curve_part.h"
#include "target_data_types/detector_target_curve_part.h"
#include "target_data_types/rocking_target_curve_part.h"
#include "target_data_types/offset_target_curve_part.h"
#include "target_data_types/gisas_target_curve_part.h"

class Specular_Target_Curve_Part;
class Detector_Target_Curve_Part;
class Rocking_Target_Curve_Part;
class Offset_Target_Curve_Part;
class GISAS_Target_Curve_Part;

class MyDoubleSpinBox;

class Target_Curve_Editor : public QDialog
{
	Q_OBJECT
public:
	explicit Target_Curve_Editor(Target_Curve* target_Curve, Multilayer* multilayer, QWidget *parent = 0);

	Target_Curve* target_Curve;
	Multilayer* multilayer_Parent;

	void closeEvent(QCloseEvent *event);
	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);
	void create_Main_Layout();
		void create_Plot();
		void create_Filepath_GroupBox();
		void create_Data_GroupBox();
		void reset_Subinterval();
		void create_Buttons();
	void set_Window_Geometry();

	void read_Data_File(QString filepath);
	void browse_Data_File();
	void export_Data_File();

	void resize_ComboBox(QLineEdit* line_Edit = nullptr);

	void show_Filepath();
	void refresh_Filepath(QString filepath);


	bool is_File_Exists = false;

	QVBoxLayout* main_Layout;
		Target_Curve_Plot* target_Curve_Plot;
		QWidget* bottom_Part_Widget;
		QVBoxLayout* bottom_Part_Layout;

		QGroupBox* filepath_GroupBox;
			QComboBox* filepath_ComboBox;
			QPushButton* browse_Button;

		QGroupBox* data_GroupBox;
			Specular_Target_Curve_Part* specular_Target_Curve_Part;
			Detector_Target_Curve_Part* detector_Target_Curve_Part;
			Rocking_Target_Curve_Part* rocking_Target_Curve_Part;
			Offset_Target_Curve_Part* offset_Target_Curve_Part;
			GISAS_Target_Curve_Part* gisas_Target_Curve_Part;

		QPushButton* close_Button;
		QPushButton* read_Data_Button;
		QPushButton* export_Data_Button;
};

#endif // TARGET_CURVE_EDITOR_H
