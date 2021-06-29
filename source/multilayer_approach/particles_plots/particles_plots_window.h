#ifndef PARTICLE_PLOTS_WINDOW_H
#define PARTICLE_PLOTS_WINDOW_H

#include "multilayer_approach/multilayer_approach.h"
#include "multilayer_approach/particles_plots/particles_plot.h"

class Multilayer_Approach;
class Particles_Plot;

class Particles_Plots_Window : public QWidget
{
	Q_OBJECT
public:
	explicit Particles_Plots_Window(QWidget *parent = nullptr);

	void closeEvent(QCloseEvent* event);

	void create_Main_Layout();
	void set_Window_Geometry();
	void write_Window_Geometry();
	void create_Tabs();
	void add_Tabs();
		void create_Tab_Content(QWidget* new_Widget, int tab_Index);

	QVBoxLayout* main_Layout;
	QTabWidget* main_Tabs;
	QVector<Particles_Plot*> particles_Plot_Vector;
};

#endif // PARTICLE_PLOTS_WINDOW_H
