#include "launcher.h"

Launcher::Launcher()
{	
	settings = new QSettings("../../settings.ini", QSettings::IniFormat);
//	reset_Settings();

	// place ui elements
	setWindowTitle("Multifitting");
	create_Main_Layout();
	set_Window_Geometry();
}

void Launcher::closeEvent(QCloseEvent* event)
{
	qApp->quit();
	event=NULL;
}

void Launcher::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);

	create_Menu();
		main_Layout->setMenuBar(menu_Bar);

	create_Buttons();
		main_Layout->addWidget(button_Multilayer_Launch);
		main_Layout->addWidget(button_Freeform_Launch);

	setLayout(main_Layout);
}

void Launcher::create_Menu()
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

void Launcher::create_Buttons()
{
	button_Multilayer_Launch = new QPushButton("Multilayer", this); button_Multilayer_Launch->setEnabled(true);
	button_Freeform_Launch	 = new QPushButton("Freeform",   this);	button_Freeform_Launch  ->setEnabled(false);

	button_Multilayer_Launch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	button_Freeform_Launch  ->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// pushButtons' actions
	connect(button_Multilayer_Launch, SIGNAL(clicked()), this, SLOT(on_Button_Multilayer_Clicked()));
	connect(button_Freeform_Launch,   SIGNAL(clicked()), this, SLOT(on_Button_Freeform_Clicked  ()));
}

void Launcher::set_Window_Geometry()
{
	settings->beginGroup( Launcher_Geometry );
		int launcher_X_Corner = settings->value( "launcher_X_Corner", 0 ).toInt();
		int launcher_Y_Corner = settings->value( "launcher_Y_Corner", 0 ).toInt();
	settings->endGroup();

	adjustSize();
	setFixedSize(size());

	setGeometry(launcher_X_Corner, launcher_Y_Corner, 0, 0);
}

void Launcher::reset_Settings()
{
	// launcher window geometry
	settings->beginGroup( Launcher_Geometry );
		settings->setValue( "launcher_X_Corner", 300 );
		settings->setValue( "launcher_Y_Corner", 300 );
	settings->endGroup();

	// multilayer window geometry
	settings->beginGroup( Multilayer_Window_Geometry );
		settings->setValue( "multilayer_X_Corner", 500 );
		settings->setValue( "multilayer_Y_Corner", 200 );
		settings->setValue( "multilayer_Min_Width", 300 );
		settings->setValue( "multilayer_Width", 531 );
		settings->setValue( "multilayer_Min_Height", 100 );
		settings->setValue( "multilayer_Height", 500 );
		settings->setValue( "multilayer_Height_Additive", 23 );
	settings->endGroup();

	// multilayer tab name
	settings->beginGroup( Multilayer_Tabs );
		settings->setValue( "default_Multilayer_Tab_Name", "Struct_" );
		settings->setValue( "default_New_Struct_Name", "New Struct" );
	settings->endGroup();

	// resource path
	settings->beginGroup( Paths );
		settings->setValue( "icon_Path", "../../imd icons/" );
	settings->endGroup();

	// whatsThis Property
	settings->beginGroup( whatsThis_Properties );
		settings->setValue( "what_is_This_Ambient", "ambient" );
		settings->setValue( "what_is_This_Substrate", "substrate" );
	settings->endGroup();
}

// slots

void Launcher::on_Button_Multilayer_Clicked()
{
	Multilayer_Approach* multilayer_Approach = new Multilayer_Approach(settings);
	multilayer_Approach->show();
}

void Launcher::on_Button_Freeform_Clicked()
{

}

void Launcher::open_Documentation()
{
	// TODO open doumentation
	qInfo() << "open doumentation";
}

void Launcher::open_About()
{
	// TODO about
	QMessageBox::about(this,"About","-Say something\n-Something");
}




