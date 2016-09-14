#include "multilayer_approach.h"

Multilayer_Approach::Multilayer_Approach(Settings* settings):
	settings(settings)
{
	optical_Constants = new Optical_Constants(settings);

	// place ui elements
	setWindowTitle("Multilayer_Approach");
	set_Window_Geometry();
	create_Menu();
	main_Layout = new QHBoxLayout(this);
	main_Layout->setMenuBar(menu_Bar);
	setLayout(main_Layout);
}

Multilayer_Approach::~Multilayer_Approach()
{

}

void Multilayer_Approach::open_Documentation()
{

}

void Multilayer_Approach::open_About()
{

}

void Multilayer_Approach::closeEvent(QCloseEvent* event)
{
	event=NULL;
}

void Multilayer_Approach::set_Window_Geometry()
{
	setMinimumWidth(settings->gui.multilayer_Min_Width);
	setMinimumHeight(settings->gui.multilayer_Min_Height);
	setGeometry(settings->gui.multilayer_X_Corner,settings->gui.multilayer_Y_Corner,settings->gui.multilayer_Width,settings->gui.multilayer_Height);
}

// create menu items
void Multilayer_Approach::create_Menu()
{
	// File
	file_Menu = new QMenu("File", this);

		act_Quit = new QAction("Close all", this);
			connect(act_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
			file_Menu->addAction(act_Quit);

	// Help
	help_Menu = new QMenu("Help",this);
		act_Documentation = new QAction("Multifitting.pdf", this);
			connect(act_Documentation, SIGNAL(triggered()), this, SLOT(open_Documentation()));
			help_Menu->addAction(act_Documentation);

		act_About = new QAction("About Multifitting",this);
			connect(act_About, SIGNAL(triggered()), this, SLOT(open_About()));
			help_Menu->addAction(act_About);

	// add menu to menu bar
	menu_Bar = new QMenuBar(this);
	menu_Bar->addMenu(file_Menu);
	menu_Bar->addMenu(help_Menu);
}
