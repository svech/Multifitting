#include "profile_plots_window.h"

Profile_Plots_Window::Profile_Plots_Window(QWidget *parent) : QWidget(parent)
{
	setWindowTitle("Profile Plots");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Profile_Plots_Window::contextMenuEvent(QContextMenuEvent *event)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(main_Tabs->currentIndex()));

	QMenu wavelength_Units("Wavelength units");
	{
		QActionGroup* group_Act_Unit = new QActionGroup(this);
			group_Act_Unit->setExclusive(true);

		for(int index=0; index<wavelength_Units_List.size(); index++)
		{
			QAction* act_Unit = new QAction(wavelength_Units_List[index], this);
			act_Unit->setProperty(index_Property, index);
			act_Unit->setCheckable(true);
			act_Unit->setActionGroup(group_Act_Unit);

			if(wavelength_Units_List[index] == multilayer->profile_Plot_Options.local_wavelength_units) {act_Unit->setChecked(true);}
			wavelength_Units.addAction(act_Unit);
			connect(act_Unit,  &QAction::triggered, this, [=]
			{
				multilayer->profile_Plot_Options.local_wavelength_units = wavelength_Units_List[index];

				profile_Plot_Vector[main_Tabs->currentIndex()]->at_Wavelength_Label->setText("At fixed " + Global_Variables::wavelength_Energy_Name(multilayer->profile_Plot_Options.local_wavelength_units));
				profile_Plot_Vector[main_Tabs->currentIndex()]->at_Wavelength_LineEdit->setText(Locale.toString(Global_Variables::wavelength_Energy(multilayer->profile_Plot_Options.local_wavelength_units, multilayer->profile_Plot_Data.local_Wavelength)/wavelength_Coefficients_Map.value(multilayer->profile_Plot_Options.local_wavelength_units),line_edit_double_format,line_edit_wavelength_precision));
				profile_Plot_Vector[main_Tabs->currentIndex()]->at_Wavelength_Unints_Label->setText(" " + multilayer->profile_Plot_Options.local_wavelength_units);
			});
		}
	}

	QMenu depth_Units("Depth units");
	{
		QActionGroup* group_Act_Unit = new QActionGroup(this);
			group_Act_Unit->setExclusive(true);

		for(int index=0; index<length_Units_List.size(); index++)
		{
			QAction* act_Unit = new QAction(length_Units_List[index], this);
				act_Unit->setProperty(index_Property, index);
				act_Unit->setCheckable(true);
				act_Unit->setActionGroup(group_Act_Unit);

			if(length_Units_List[index] == multilayer->profile_Plot_Options.local_length_units) {act_Unit->setChecked(true);}
			depth_Units.addAction(act_Unit);
			connect(act_Unit,  &QAction::triggered, this, [=]
			{
				multilayer->profile_Plot_Options.local_length_units = length_Units_List[index];

				profile_Plot_Vector[main_Tabs->currentIndex()]->custom_Plot->xAxis->setLabel("Depth, "+multilayer->profile_Plot_Options.local_length_units);
				profile_Plot_Vector[main_Tabs->currentIndex()]->custom_Plot->replot();

				// TODO change argument
			});
		}
	}

	QMenu menu;
	menu.addMenu(&wavelength_Units);
	menu.addMenu(&depth_Units);
	menu.exec(event->globalPos());
}

void Profile_Plots_Window::closeEvent(QCloseEvent *event)
{
	global_Multilayer_Approach->runned_Profile_Plots_Window.remove(profile_Plots_Key);
	global_Multilayer_Approach->unlock_Mainwindow_Interface();
	write_Window_Geometry();
	event->accept();
}

void Profile_Plots_Window::create_Main_Layout()
{
	can_Change_Index = false;
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(0,0,0,0);

	global_Multilayer_Approach->lock_Mainwindow_Interface();
	create_Tabs();
		main_Layout->addWidget(main_Tabs);
	add_Tabs();

	// shortcuts
	Global_Variables::create_Shortcuts(this);
	can_Change_Index = tab_synchronization;
}

void Profile_Plots_Window::set_Window_Geometry()
{
	setGeometry(profile_plots_x_corner,profile_plots_y_corner,profile_plots_width,profile_plots_height);
}

void Profile_Plots_Window::write_Window_Geometry()
{
	// usual show
	if(!isMaximized())
	{
		profile_plots_x_corner = frameGeometry().x()-corner_x_shift;
		profile_plots_y_corner = frameGeometry().y()-corner_y_shift;

		profile_plots_width  = geometry().width() +  width_add;
		profile_plots_height = geometry().height()+ height_add;
	}
}

void Profile_Plots_Window::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(false);

	connect(main_Tabs,	&QTabWidget::currentChanged, this,
	[=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);
		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}

		if(can_Change_Index)
		{
			can_Change_Index = false;
																										  {global_Multilayer_Approach->						  multilayer_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key)) {global_Multilayer_Approach->table_Of_Structures		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key))	  {global_Multilayer_Approach->table_Of_Roughness		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key))			  {global_Multilayer_Approach->optical_Graphs			  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key)){global_Multilayer_Approach->calculation_Settings_Editor->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			can_Change_Index = tab_synchronization;
		}
	});
}

void Profile_Plots_Window::add_Tabs()
{
	profile_Plot_Vector.resize(global_Multilayer_Approach->multilayer_Tabs->count());

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

void Profile_Plots_Window::create_Tab_Content(QWidget* new_Widget, int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));

	Profile_Plot* new_Plot = new Profile_Plot(multilayer, this);
		profile_Plot_Vector[tab_Index] = new_Plot;

	QHBoxLayout* tab_Layout = new QHBoxLayout(new_Widget);
		tab_Layout->addWidget(new_Plot);
}

void Profile_Plots_Window::refresh_Plots()
{
	for(Profile_Plot* profile_Plot: profile_Plot_Vector)
	{
		profile_Plot->plot_Data(true);
	}
}
