#include "launcher.h"

Launcher::Launcher()
{	
	settings = new Settings;

	// place ui elements
	setWindowTitle("Multifitting");
	create_Menu();
	create_Buttons();
	main_Layout = new QHBoxLayout(this);
	main_Layout->setMenuBar(menu_Bar);
	main_Layout->addWidget(button_Multilayer_Launch);
	main_Layout->addWidget(button_Freeform_Launch);
	setLayout(main_Layout);
	adjustSize();
	setFixedSize(size());

	setGeometry(settings->gui.launcher_X_Corner,settings->gui.launcher_Y_Corner,0,0);
}

Launcher::~Launcher()
{

}

void Launcher::closeEvent(QCloseEvent* event)
{
	qApp->quit();
	event=NULL; // do nothing
}

// create menu items
void Launcher::create_Menu()
{
	// File
	file_Menu = new QMenu("File", this);

		act_Change_Dir_Multilayer = new QAction("Change multilayer working directory", this);
			connect(act_Change_Dir_Multilayer,SIGNAL(triggered()), this, SLOT(change_Dir_Multilayer()));
			file_Menu->addAction(act_Change_Dir_Multilayer);

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

// create buttons
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

// change default direction
void Launcher::change_Dir(QString& directory)
{
	directory = QFileDialog::getExistingDirectory(this,	"Select a Directory", directory);
}

void Launcher::change_Dir_Multilayer()
{
	//change_Dir(settings.dir_of_Saved_Files_Multilayer);
}

void Launcher::open_Documentation()
{

}

void Launcher::open_About()
{
	//QMessageBox::about(this,"About","-Say something\n-Something");
}

void Launcher::on_Button_Multilayer_Clicked()
{
	Multilayer_Approach* multilayer_Approach = new Multilayer_Approach(settings);
	multilayer_Approach->show();
}

void Launcher::on_Button_Freeform_Clicked()
{

}
