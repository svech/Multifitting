// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www->viva64->com

#include "launcher.h"

Launcher::Launcher(QWidget *parent) :
	QWidget(parent)
{
	// seed randomizator
	clock_t t = clock();
	srand(t);

	Settings::read_All_Settings(true);
	Settings::save_All_Settings();
	Global_Variables::fill_Units_Maps();

	// read optical constants database
	Global_Variables::read_Optical_Constants();
	Global_Variables::create_Sorted_Elements_Map();

	qApp->setStyle(QStyleFactory::create(application_style));

	create_Main_Layout();
	set_Window_Geometry();

	// TODO temporary
	multilayer_Approach_Launch_Button->clicked();
}

void Launcher::closeEvent(QCloseEvent* event)
{
//	qApp->quit();	// closing launcher does nothing
	event->accept();
}

void Launcher::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
	create_Menu();
	create_Buttons();
}

void Launcher::create_Menu()
{
	Menu* menu = new Menu(window_Type_Launcher, this);
		main_Layout->setMenuBar(menu->menu_Bar);
}

void Launcher::create_Buttons()
{
	// Multilayer_Approach button
	{
		multilayer_Approach_Launch_Button = new QPushButton("Multilayer",this);
			multilayer_Approach_Launch_Button->setEnabled(true);
			multilayer_Approach_Launch_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		connect(multilayer_Approach_Launch_Button, &QPushButton::clicked, this, &Launcher::add_Multilayer_Approach_Instance);
		main_Layout->addWidget(multilayer_Approach_Launch_Button);
	}
}

void Launcher::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
	setGeometry(launcher_x_corner, launcher_x_corner, 0, 0);
}

void Launcher::add_Multilayer_Approach_Instance()
{
	Multilayer_Approach* multilayer_Approach = new Multilayer_Approach(this);	// Launcher is not a parent! Multilayer instances are completely independent
		multilayer_Approach->setWindowFlags(Qt::Window);
		multilayer_Approach->show();
		multilayer_Approach_List.append(multilayer_Approach);

		connect(multilayer_Approach, &Multilayer_Approach::closed,  this, &Launcher::multilayer_Approach_Closed);				// if closed, delete from list
//		connect(multilayer_Approach, &Multilayer_Approach::refresh, this, &Launcher::refresh_All_Multilayer_Approach_View);
}

void Launcher::multilayer_Approach_Closed()
{
	for(int i=multilayer_Approach_List.size()-1; i>=0; --i)
	{
		if(multilayer_Approach_List[i] == sender())
			multilayer_Approach_List.removeAt(i);
	}
}
