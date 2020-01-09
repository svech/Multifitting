#ifndef PROFILE_PLOTS_WINDOW_H
#define PROFILE_PLOTS_WINDOW_H

#include "multilayer_approach/multilayer_approach.h"
#include "multilayer_approach/profile_plots/profile_plot.h"

class Multilayer_Approach;
class Profile_Plot;

class Profile_Plots_Window : public QWidget
{
	Q_OBJECT

public:
	explicit Profile_Plots_Window(QWidget *parent = nullptr);

	void contextMenuEvent(QContextMenuEvent *event);
	void closeEvent(QCloseEvent* event);

	void create_Main_Layout();
	void set_Window_Geometry();
	void write_Window_Geometry();
	void create_Tabs();
	void add_Tabs();
		void create_Tab_Content(QWidget* new_Widget, int tab_Index);
	void refresh_Plots();

	QVBoxLayout* main_Layout;
	QTabWidget* main_Tabs;
	QVector<QLabel*> at_Wavelength_Label_Vector;
	QVector<QLineEdit*> at_Wavelength_LineEdit_Vector;
	QVector<QLabel*> at_Wavelength_Units_Label_Vector;
	QVector<Profile_Plot*> profile_Plot_Vector;
};

#endif // PROFILE_PLOTS_WINDOW_H
