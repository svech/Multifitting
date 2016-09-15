#include "multilayer_approach.h"

Multilayer_Approach::Multilayer_Approach(Settings* settings):
	settings(settings)
{
	optical_Constants = new Optical_Constants(settings);

	// place ui elements
	setWindowTitle("Multilayer_Approach");
	set_Window_Geometry();	
	main_Layout = new QHBoxLayout(this);

	create_Menu_Bar();
	main_Layout->setMenuBar(menu_Bar);

	create_Structure_Tabs();
	main_Layout->addWidget(structure_Tabs);

	setLayout(main_Layout);
}

Multilayer_Approach::~Multilayer_Approach()
{

}

void Multilayer_Approach::add_Multilayer(int tab_Index)
{
	if(tab_Index==multilayer.size())
	{
		Multilayer* new_Multilayer = new Multilayer;
		multilayer.append(new_Multilayer);
		structure_Tabs->removeTab(tab_Index);
		structure_Tabs->addTab(new_Multilayer,settings->gui.new_Multilayer_Tab_Name);
		structure_Tabs->setTabText(tab_Index,settings->gui.default_Multilayer_Tab_Name+QString::number(tab_Index+1));
		structure_Tabs->addTab(new QWidget(),settings->gui.new_Multilayer_Tab_Name);
	}
}

void Multilayer_Approach::rename_Multilayer(int tab_Index)
{
	bool ok;
	QString text = QInputDialog::getText(this, "Rename structure", "New name", QLineEdit::Normal, QDir::home().dirName(), &ok);
	if (ok && !text.isEmpty())
		structure_Tabs->setTabText(tab_Index, text);
}

void Multilayer_Approach::open_Documentation()
{

}

void Multilayer_Approach::open_About()
{

}

void Multilayer_Approach::closeEvent(QCloseEvent* event)
{
	qApp->quit();
	event=NULL;
}

void Multilayer_Approach::set_Window_Geometry()
{
	setMinimumWidth(settings->gui.multilayer_Min_Width);
	setMinimumHeight(settings->gui.multilayer_Min_Height);
	setGeometry(settings->gui.multilayer_X_Corner,settings->gui.multilayer_Y_Corner,settings->gui.multilayer_Width,settings->gui.multilayer_Height);
}

void Multilayer_Approach::create_Menu_Bar()
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

void Multilayer_Approach::create_Structure_Tabs()
{
	structure_Tabs = new QTabWidget;
	Multilayer* new_Multilayer = new Multilayer;
	multilayer.append(new_Multilayer);
	structure_Tabs->addTab(new_Multilayer,settings->gui.new_Multilayer_Tab_Name);
	structure_Tabs->setTabText(0,settings->gui.default_Multilayer_Tab_Name+QString::number(1));
	structure_Tabs->addTab(new QWidget(),settings->gui.new_Multilayer_Tab_Name);

	connect(structure_Tabs, SIGNAL(tabBarClicked(int)),		  this, SLOT(add_Multilayer(int)));
	connect(structure_Tabs, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(rename_Multilayer(int)));
}
