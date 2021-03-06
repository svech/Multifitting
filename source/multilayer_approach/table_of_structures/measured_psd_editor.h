#ifndef MEASURED_PSD_EDITOR_H
#define MEASURED_PSD_EDITOR_H

#include "global/settings.h"
#include "multilayer_approach/table_of_structures/table_of_structures.h"

class Table_Of_Structures;

class Measured_PSD_Editor : public QDialog
{
	Q_OBJECT
public:
	explicit Measured_PSD_Editor(Table_Of_Structures* table_Of_Structures, Multilayer* multilayer, QString PSD_Type, QPushButton* PSD_Button, QWidget* parent = 0);
	~Measured_PSD_Editor();

	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);
	void create_Main_Layout();
		void create_Plot();
		void plot_Data();
		void refresh_Labels();
		void create_Plot_Options_Box();
		void create_PSD_Options_Box();
		void create_Filepath_Box();
		void create_Buttons();
	void read_PSD_File();

	Multilayer* multilayer;
	Table_Of_Structures* table_Of_Structures;
	QString PSD_Type;
	QPushButton* PSD_Button;

	PSD_Data& psd_Data;

	QVBoxLayout* main_Layout;
	QWidget* bottom_Widget;
	QVBoxLayout* bottom_Widget_Layout;

	QCustomPlot* custom_Plot;

	QLineEdit* path_Line_Edit;
};

#endif // MEASURED_PSD_EDITOR_H
