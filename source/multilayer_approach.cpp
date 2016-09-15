#include "multilayer_approach.h"

Multilayer_Approach::Multilayer_Approach(Settings* settings):
	settings(settings)
{
	optical_Constants = new Optical_Constants(settings);

	// place ui elements
	setWindowTitle("Multilayer_Approach");
	set_Window_Geometry();	
	//setContextMenuPolicy(Qt::CustomContextMenu);

	//connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(show_Context_Menu(const QPoint&)));

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

// create menu items
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
	structure_Tabs->addTab(new QWidget(),"TAB 1");
	structure_Tabs->addTab(new QWidget(),"TAB 2");

	structure_Tabs->setTabText(0,"qq");

	cutAct = new QAction(tr("Cu&t"), this);
	cutAct->setShortcuts(QKeySequence::Cut);
	cutAct->setStatusTip(tr("Cut the current selection's contents to the "
							"clipboard"));

	copyAct = new QAction(tr("&Copy"), this);
	copyAct->setShortcuts(QKeySequence::Copy);
	copyAct->setStatusTip(tr("Copy the current selection's contents to the "
							 "clipboard"));

	pasteAct = new QAction(tr("&Paste"), this);
	pasteAct->setShortcuts(QKeySequence::Paste);
	pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
							  "selection"));

}

#ifndef QT_NO_CONTEXTMENU
void Multilayer_Approach::contextMenuEvent(QContextMenuEvent *event)
{

	QMenu menu(this);
	menu.addAction(cutAct);
	menu.addAction(copyAct);
	menu.addAction(pasteAct);
	menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU
