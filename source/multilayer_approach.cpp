#include "multilayer_approach.h"

Multilayer_Approach::Multilayer_Approach():
	gui_Settings(Gui_Settings_Path, QSettings::IniFormat)
{
	// place ui elements
	setWindowTitle("Multilayer Model");
	create_Main_Layout();
	set_Window_Geometry();

	setAttribute(Qt::WA_DeleteOnClose);
}

void Multilayer_Approach::closeEvent(QCloseEvent* event)
{
	// TODO close multalayer window
	qApp->quit();
	event;
}

void Multilayer_Approach::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
		main_Layout->setContentsMargins(4,3,2,3);

	create_Menu();
		main_Layout->setMenuBar(menu_Bar);
	create_Multilayer_Tabs();
		main_Layout->addWidget(multilayer_Tabs);	
}

void Multilayer_Approach::set_Window_Geometry()
{
	gui_Settings.beginGroup( Multilayer_Window_Geometry );
		int multilayer_X_Corner   = gui_Settings.value( "multilayer_X_Corner", 0 ).toInt();
		int multilayer_Y_Corner   = gui_Settings.value( "multilayer_Y_Corner", 0 ).toInt();
		int multilayer_Min_Width  = gui_Settings.value( "multilayer_Min_Width", 0 ).toInt();
		int multilayer_Width	  = gui_Settings.value( "multilayer_Width", 0 ).toInt();
		int multilayer_Min_Height = gui_Settings.value( "multilayer_Min_Height", 0 ).toInt();
		int multilayer_Height	  = gui_Settings.value( "multilayer_Height", 0 ).toInt();
	gui_Settings.endGroup();

	setMinimumWidth(multilayer_Min_Width);
	setMinimumHeight(multilayer_Min_Height);
	setGeometry(multilayer_X_Corner,multilayer_Y_Corner,multilayer_Width,multilayer_Height);
}

void Multilayer_Approach::create_Menu()
{
	menu_Bar = new QMenuBar(this);

	// File
	file_Menu = new QMenu("File", this);
		QAction* act_Quit = new QAction("Close all", this);
			connect(act_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
			file_Menu->addAction(act_Quit);
		menu_Bar->addMenu(file_Menu);

	// Help
	help_Menu = new QMenu("Help",this);
		QAction* act_Documentation = new QAction("Multifitting.pdf", this);
			connect(act_Documentation, SIGNAL(triggered()), this, SLOT(open_Documentation()));
			help_Menu->addAction(act_Documentation);
		QAction* act_About = new QAction("About Multifitting",this);
			connect(act_About, SIGNAL(triggered()), this, SLOT(open_About()));
			help_Menu->addAction(act_About);		
		menu_Bar->addMenu(help_Menu);
}

void Multilayer_Approach::create_Multilayer_Tabs()
{
	multilayer_Tabs = new QTabWidget;
		multilayer_Tabs->setMovable(true);
		multilayer_Tabs->setTabsClosable(true);

	add_Tab_Corner_Button = new QToolButton;
		add_Tab_Corner_Button->setText("+");
	QFont tmp_Qf; tmp_Qf.setBold(true);
		add_Tab_Corner_Button->setFont(tmp_Qf);
		multilayer_Tabs->setCornerWidget(add_Tab_Corner_Button);

	connect(multilayer_Tabs, SIGNAL(currentChanged(int)),	  this,SLOT(change_Tab_Color(int)));
	connect(multilayer_Tabs, SIGNAL(tabCloseRequested(int)),  this,SLOT(remove_Multilayer(int)));

	connect(add_Tab_Corner_Button,  SIGNAL(clicked()),		  this, SLOT(add_Multilayer()));
	connect(multilayer_Tabs, SIGNAL(tabBarDoubleClicked(int)),this, SLOT(rename_Multilayer(int)));

	add_Multilayer();
}

// slots

void Multilayer_Approach::change_Tab_Color(int index)
{
	multilayer_Tabs->tabBar()->setTabTextColor(index,Qt::black);
	if(multilayer_Tabs->tabBar()->tabButton(index, QTabBar::RightSide))
	   multilayer_Tabs->tabBar()->tabButton(index, QTabBar::RightSide)->show();

	for(int i = 0; i<multilayer_Tabs->tabBar()->count(); i++)
	{
		if(i!=index)
		{
			multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->hide();
			multilayer_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	}
}

void Multilayer_Approach::add_Multilayer()
{
	gui_Settings.beginGroup( Multilayer_Tabs );
		QString default_Multilayer_Tab_Name = gui_Settings.value( "default_Multilayer_Tab_Name", 0 ).toString();
	gui_Settings.endGroup();

	Multilayer* new_Multilayer = new Multilayer;
		new_Multilayer->setContentsMargins(-8,-10,-8,-10);

	multilayer_Tabs->addTab(new_Multilayer, default_Multilayer_Tab_Name);
	multilayer_Tabs->setTabText(multilayer_Tabs->count()-1, default_Multilayer_Tab_Name+QString::number(multilayer_Tabs->count()));

	if(multilayer_Tabs->count()>1)
	{
		multilayer_Tabs->tabBar()->setTabTextColor(multilayer_Tabs->count()-1,Qt::gray);
		multilayer_Tabs->tabBar()->tabButton(multilayer_Tabs->count()-1, QTabBar::RightSide)->hide();
	}

	multilayer.append(new_Multilayer);
}

void Multilayer_Approach::remove_Multilayer(int index)
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Multilayer \"" + multilayer_Tabs->tabBar()->tabText(index) + "\" will be removed.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		Multilayer* temp_Multilayer;

		for(int i=0; i<multilayer.size(); i++)
		{
			if(multilayer[i] == multilayer_Tabs->widget(index))
			{
				temp_Multilayer = multilayer[i];
				multilayer.removeAt(i);
			}
		}
		delete temp_Multilayer;
		if(multilayer_Tabs->count()==0) add_Multilayer();
	}
}

void Multilayer_Approach::rename_Multilayer(int tab_Index)
{
	gui_Settings.beginGroup( Multilayer_Tabs );
		QString default_New_Struct_Name = gui_Settings.value( "default_New_Struct_Name", 0 ).toString();
	gui_Settings.endGroup();

	bool ok;

	QString text = QInputDialog::getText(this, "Rename structure", "New name", QLineEdit::Normal, default_New_Struct_Name, &ok);
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
