#include "launcher.h"

Launcher::Launcher(QWidget *parent) :
	QWidget(parent)
{
	Settings::read_All_Settings(true);
	Settings::save_All_Settings();
	Global_Variables::fill_Units_Maps();

	// read optical constants database
	Global_Variables::read_Optical_Constants();
	Global_Variables::create_Sorted_Elements_Map();

	qApp->setStyle(QStyleFactory::create(application_style));

	create_Main_Layout();
	set_Window_Geometry();

	// TODO
	multilayer_Approach_Launch_Button->clicked();
}

void Launcher::closeEvent(QCloseEvent* event)
{
	event;
	qApp->quit();
}

void Launcher::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
	create_Menu();
	create_Buttons();
}

void Launcher::create_Menu()
{
	Menu* menu = new Menu(Window_Type::Launcher(), this);
		main_Layout->setMenuBar(menu->menu_Bar);
}

void Launcher::create_Buttons()
{
	// Multilayer button
	{
		multilayer_Approach_Launch_Button = new QPushButton("Multilayer",this);
			multilayer_Approach_Launch_Button->setEnabled(true);
			multilayer_Approach_Launch_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		connect(multilayer_Approach_Launch_Button, SIGNAL(clicked()), this, SLOT(add_Multilayer_Approach_Instance()));
		main_Layout->addWidget(multilayer_Approach_Launch_Button);

	}
	// Freeform button
	{
		freeform_Approach_Launch_Button	 = new QPushButton("Freeform",this);
			freeform_Approach_Launch_Button->setEnabled(false);			 // TODO enabled
			freeform_Approach_Launch_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		connect(freeform_Approach_Launch_Button,   SIGNAL(clicked()), this, SLOT(add_Freeform_Approach_Instance()));
		main_Layout->addWidget(freeform_Approach_Launch_Button);
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
	Multilayer_Approach* multilayer_Approach = new Multilayer_Approach(this);
		multilayer_Approach->setWindowFlags(Qt::Window);
		multilayer_Approach->show();
		multilayer_Approach_List.append(multilayer_Approach);

		connect(multilayer_Approach, SIGNAL(closed()),  this, SLOT(multilayer_Approach_Closed()));
		connect(multilayer_Approach, SIGNAL(refresh()), this, SLOT(refresh_All_Multilayer_Approach_View()));
}

void Launcher::multilayer_Approach_Closed()
{
	for(int i=0; i<multilayer_Approach_List.size(); ++i)
	{
		if(multilayer_Approach_List[i] == sender())
			multilayer_Approach_List.removeAt(i);
	}
}

void Launcher::add_Freeform_Approach_Instance()
{
	// TODO freeform approach
}

void Launcher::freeform_Approach_Closed()
{
	// TODO
}

void Launcher::refresh_All_Multilayer_Approach_View()
{
	qInfo() << "launcher fesreshing and saving";
	for(int i=0; i<multilayer_Approach_List.size(); ++i)
	{
		if(multilayer_Approach_List[i]!=sender())
			multilayer_Approach_List[i]->refresh_All_Multilayers_View();
	}
	Settings::save_All_Settings();
}
