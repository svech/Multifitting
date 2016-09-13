#include "launcher.h"

Launcher::Launcher(QWidget *parent)
	: QWidget(parent)
{
	IF_GUI
	{
		// place ui elements
		setWindowTitle("Multifitting");
		create_Menu();
//		create_Buttons();
//		main_Layout = new QHBoxLayout(this);
//		main_Layout->setMenuBar(menu_Bar);
//		main_Layout->addWidget(button_Multilayer_Launch);
//		main_Layout->addWidget(button_One_More_Button);
//		setLayout(main_Layout);
//		adjustSize();
//		setFixedSize(size());
	}
	else
	{
		QTextStream(stdout) << "Console launch" << endl;
	}
}

Launcher::~Launcher()
{

}

void Launcher::closeEvent(QCloseEvent* event)
{
	//hide();
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
	button_Multilayer_Launch = new QPushButton("Multilayer", this);
	button_One_More_Button	 = new QPushButton("+1", this);
	button_One_More_Button->hide();

	button_Multilayer_Launch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// pushButtons' actions
	connect(button_Multilayer_Launch, SIGNAL(clicked()), this, SLOT(on_Button_Multilayer_clicked()));
	connect(button_One_More_Button, SIGNAL(clicked()), this, SLOT(on_Button_One_More_Button_clicked()));
}

// change default direction
void Launcher::change_Dir(QString& directory)
{
	directory = QFileDialog::getExistingDirectory(this,	"Select a Directory", directory);
}
void Launcher::change_Dir_Multilayer()
{
	change_Dir(settings->dir_of_Saved_Files_Multilayer);
}

void Launcher::pushButton_Multilayer_Clicked()
{

}
