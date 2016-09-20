#include "multilayer_approach.h"

Multilayer_Approach::Multilayer_Approach(Settings* settings):
	settings(settings)
{
	optical_Constants = new Optical_Constants(settings);

	// place ui elements
	setWindowTitle("Multilayer_Approach");
	create_Main_Layout();
	set_Window_Geometry();
}

void Multilayer_Approach::closeEvent(QCloseEvent* event)
{
	// TODO close multalayer window
	qApp->quit();
	event=NULL;
}

void Multilayer_Approach::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);

	create_Menu();
		main_Layout->setMenuBar(menu_Bar);

	create_Multilayer_Tabs();
		main_Layout->addWidget(multilayer_Tabs);

	main_Layout->setContentsMargins(4,3,2,3);
	setLayout(main_Layout);
}

void Multilayer_Approach::set_Window_Geometry()
{
	setMinimumWidth(settings->gui.multilayer_Min_Width);
	setMinimumHeight(settings->gui.multilayer_Min_Height);
	setGeometry(settings->gui.multilayer_X_Corner,settings->gui.multilayer_Y_Corner,settings->gui.multilayer_Width,settings->gui.multilayer_Height);
}

void Multilayer_Approach::create_Menu()
{
	// File
	file_Menu = new QMenu("File", this);

		QAction* act_Quit = new QAction("Close all", this);
			connect(act_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
			file_Menu->addAction(act_Quit);

	// Help
	help_Menu = new QMenu("Help",this);
		QAction* act_Documentation = new QAction("Multifitting.pdf", this);
			connect(act_Documentation, SIGNAL(triggered()), this, SLOT(open_Documentation()));
			help_Menu->addAction(act_Documentation);

		QAction* act_About = new QAction("About Multifitting",this);
			connect(act_About, SIGNAL(triggered()), this, SLOT(open_About()));
			help_Menu->addAction(act_About);

	// add menu to menu bar
	menu_Bar = new QMenuBar(this);
	menu_Bar->addMenu(file_Menu);
	menu_Bar->addMenu(help_Menu);
}

void Multilayer_Approach::create_Multilayer_Tabs()
{
	multilayer_Tabs = new QTabWidget;
	multilayer_Tabs->setMovable(true);

	add_Tab_button = new QToolButton;
	add_Tab_button->setText("+");
	QFont tmp_Qf; tmp_Qf.setBold(true);
	add_Tab_button->setFont(tmp_Qf);
	multilayer_Tabs->setCornerWidget(add_Tab_button);

	connect(multilayer_Tabs, SIGNAL(currentChanged(int)),this,SLOT(change_Tab_Color(int)));

	connect(add_Tab_button,  SIGNAL(clicked()),				  this, SLOT(add_Multilayer()));
	connect(multilayer_Tabs, SIGNAL(tabBarDoubleClicked(int)),this, SLOT(rename_Multilayer(int)));

	add_Multilayer();
}

// slots

void Multilayer_Approach::change_Tab_Color(int index)
{
	multilayer_Tabs->tabBar()->setTabTextColor(index,Qt::black);
	for(int i = 0;i<multilayer_Tabs->tabBar()->count();i++)
	{
		if(i != index)
			multilayer_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
	}
}

void Multilayer_Approach::add_Multilayer()
{
	Multilayer* new_Multilayer = new Multilayer(settings);
	new_Multilayer->setContentsMargins(-8,-10,-8,-10);

	multilayer.append(new_Multilayer);
	multilayer_Tabs->addTab(new_Multilayer,settings->gui.default_Multilayer_Tab_Name);
	multilayer_Tabs->setTabText(multilayer_Tabs->count()-1,settings->gui.default_Multilayer_Tab_Name+QString::number(multilayer_Tabs->count()));
	if(multilayer_Tabs->count()>1)
		multilayer_Tabs->tabBar()->setTabTextColor(multilayer_Tabs->count()-1,Qt::gray);
}

void Multilayer_Approach::rename_Multilayer(int tab_Index)
{
	bool ok;
	QString text = QInputDialog::getText(this, "Rename structure", "New name", QLineEdit::Normal, QDir::home().dirName(), &ok);
	if (ok && !text.isEmpty())
		multilayer_Tabs->setTabText(tab_Index, text);
}

void Multilayer_Approach::open_Documentation()
{
	// TODO open doumentation
	qInfo() << "open documentation";
}

void Multilayer_Approach::open_About()
{
	// TODO about
	QMessageBox::about(this,"About","-Say something\n-Something");
}
