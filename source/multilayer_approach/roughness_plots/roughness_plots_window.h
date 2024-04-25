#ifndef ROUGHNESS_PLOTS_WINDOW_H
#define ROUGHNESS_PLOTS_WINDOW_H

#include "multilayer_approach/multilayer_approach.h"
#include "multilayer_approach/roughness_plots/roughness_plot.h"

class Multilayer_Approach;
class Roughness_Plot;

class Roughness_Plots_Window : public QWidget
{
	Q_OBJECT
public:
	explicit Roughness_Plots_Window(QWidget *parent = nullptr);

    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent* event);

	void create_Main_Layout();
	void set_Window_Geometry();
	void write_Window_Geometry();
	void create_Tabs();
	void add_Tabs();
		void create_Tab_Content(QWidget* new_Widget, int tab_Index);

	QVBoxLayout* main_Layout;
	QTabWidget* main_Tabs;
	QVector<Roughness_Plot*> roughness_Plot_Vector;
};

#endif // ROUGHNESS_PLOTS_WINDOW_H
