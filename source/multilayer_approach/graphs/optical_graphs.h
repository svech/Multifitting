#ifndef OPTICAL_GRAPHS_H
#define OPTICAL_GRAPHS_H

#include "multilayer_approach/multilayer_approach.h"
#include "curve_plot.h"

class Multilayer_Approach;

class Optical_Graphs : public QWidget
{
	Q_OBJECT
public:
	explicit Optical_Graphs(QWidget *parent = nullptr);

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
	void create_Tabs();
	void add_Tabs();
	void create_Tab_Content(QWidget* new_Widget, int tab_Index);
	void set_Window_Geometry();

	QVBoxLayout* main_Layout;

	QTabWidget* main_Tabs;
		QVector<QGroupBox*> target_GroupBox_Vec;
		QVector<QSplitter*> splitter_Vec;
		QVector<QGroupBox*> independent_GroupBox_Vec;
};

#endif // OPTICAL_GRAPHS_H
