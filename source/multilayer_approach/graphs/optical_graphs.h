#ifndef OPTICAL_GRAPHS_H
#define OPTICAL_GRAPHS_H

#include "multilayer_approach/multilayer_approach.h"
#include "curve_plot.h"

class Multilayer_Approach;
class Curve_Plot;

class Optical_Graphs : public QWidget
{
	Q_OBJECT
public:
	explicit Optical_Graphs(QString keep_Splitter = "", QWidget *parent = nullptr);

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
	void create_Tabs();
	void add_Tabs();
	void create_Tab_Content(QWidget* new_Widget, int tab_Index);
	void set_Window_Geometry();
	void write_Window_Geometry();
	void lock_Mainwindow_Interface();
	void unlock_Mainwindow_Interface();

	QString keep_Splitter;
	QVBoxLayout* main_Layout;

	QTabWidget* main_Tabs;
		QVector<QGroupBox*> target_GroupBox_Vec;
		QVector<QGroupBox*> independent_GroupBox_Vec;

	QVector<QSplitter*> target_Independent_Splitter_Vec;
	QVector<QSplitter*> target_Splitter_Vec;
	QVector<QSplitter*> independent_Splitter_Vec;
	QVector<Curve_Plot*> plots;
};

#endif // OPTICAL_GRAPHS_H
