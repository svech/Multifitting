#include "roughness_plots_window.h"

Roughness_Plots_Window::Roughness_Plots_Window(QWidget *parent) : QWidget(parent)
{
	setWindowTitle("Roughness plot");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Roughness_Plots_Window::closeEvent(QCloseEvent* event)
{
	global_Multilayer_Approach->runned_Roughness_Plots_Window.remove(roughness_Plots_Key);
	global_Multilayer_Approach->unlock_Mainwindow_Interface();
	write_Window_Geometry();
	event->accept();
}

void Roughness_Plots_Window::create_Main_Layout()
{
	QWidget* main_Widget = this;
	if(make_all_windows_resizeable)
	{
		QVBoxLayout* top_Layout = new QVBoxLayout(this);
			top_Layout->setMargin(0);

		main_Widget = new QWidget;
		QScrollArea* scrollArea = new QScrollArea;
			scrollArea->setWidget(main_Widget);
			scrollArea->setWidgetResizable(true);
		top_Layout->addWidget(scrollArea);
	}

	main_Layout = new QVBoxLayout(main_Widget);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(0,0,0,0);

	global_Multilayer_Approach->lock_Mainwindow_Interface();
	create_Tabs();
		main_Layout->addWidget(main_Tabs);
	add_Tabs();

	// shortcuts
	Global_Variables::create_Shortcuts(this);
	connect(main_Tabs,	&QTabWidget::currentChanged, this, [=]
	{
		if(tab_synchronization)
		{
																										  {global_Multilayer_Approach->						  multilayer_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Profile_Plots_Window.contains(profile_Plots_Key))		  {global_Multilayer_Approach->profile_Plots_Window       ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key)) {global_Multilayer_Approach->table_Of_Structures		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))	  {global_Multilayer_Approach->optical_Graphs_2D		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))	  {global_Multilayer_Approach->optical_Graphs_1D		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key)){global_Multilayer_Approach->calculation_Settings_Editor->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
		}
	});
}

void Roughness_Plots_Window::set_Window_Geometry()
{
	int width_add = 0, height_add = 0;
	if(make_all_windows_resizeable && !previous_all_windows_resizeable) {width_add+=2; height_add+=2;}

	setGeometry(roughness_plots_x_corner,roughness_plots_y_corner,roughness_plots_width+width_add,roughness_plots_height+height_add);
}

void Roughness_Plots_Window::write_Window_Geometry()
{
	// usual show
	if(!isMaximized())
	{
		roughness_plots_x_corner = frameGeometry().x()-corner_x_shift;
		roughness_plots_y_corner = frameGeometry().y()-corner_y_shift;

		roughness_plots_width  = geometry().width() +  width_add;
		roughness_plots_height = geometry().height()+ height_add;
	}
}

void Roughness_Plots_Window::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(false);

	connect(main_Tabs,	&QTabWidget::currentChanged, this, [=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);
		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	});
}

void Roughness_Plots_Window::add_Tabs()
{
	roughness_Plot_Vector.resize(global_Multilayer_Approach->multilayer_Tabs->count());

	for(int tab_Index=0; tab_Index<global_Multilayer_Approach->multilayer_Tabs->count(); ++tab_Index)
	{
		QWidget* new_Widget = new QWidget;
		create_Tab_Content(new_Widget, tab_Index);
		main_Tabs->addTab(new_Widget, global_Multilayer_Approach->multilayer_Tabs->tabText(tab_Index));
	}
	for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
	{
		if(i!=main_Tabs->currentIndex()) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
	}
	main_Tabs->setCurrentIndex(global_Multilayer_Approach->multilayer_Tabs->currentIndex());
}

void Roughness_Plots_Window::create_Tab_Content(QWidget *new_Widget, int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));

	Roughness_Plot* new_Plot = new Roughness_Plot(multilayer, this, this);
		roughness_Plot_Vector[tab_Index] = new_Plot;

	QHBoxLayout* tab_Layout = new QHBoxLayout(new_Widget);
		tab_Layout->addWidget(new_Plot);
}
