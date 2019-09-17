#include "launcher.h"

Launcher::Launcher(QWidget *parent) :
	QWidget(parent)
{
	QApplication::setWindowIcon(QIcon(QPixmap(icon_path + "icon.ico")));

	Locale.setNumberOptions(QLocale::OmitGroupSeparator);

	// seed randomizator
	clock_t t = clock();
	srand(t);

	Global_Variables::find_Gui_Shifts();

	Settings::read_All_Settings(/*true*/);
//	Settings::save_All_Settings();
	Global_Variables::fill_Units_Maps();
	global_Workers.resize(reflectivity_Calc_Threads);

	printf("\n%d logical cores, %d used\n", QThread::idealThreadCount(),reflectivity_Calc_Threads);
	printf("\nMultifitting version %d.%d.%d\n\n", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);


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

		connect(multilayer_Approach_Launch_Button, &QPushButton::clicked, this, &Launcher::open_Multilayer_Approach);
		main_Layout->addWidget(multilayer_Approach_Launch_Button);
	}
}

void Launcher::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
	setGeometry(launcher_x_corner, launcher_x_corner, 0, 0);
}

void Launcher::open_Multilayer_Approach()
{
	if(!runned_Multilayer_Approaches.contains(multilayer_Approach_Key))
	{
		global_Multilayer_Approach = new Multilayer_Approach(this);	// Launcher is not a parent!
		global_Multilayer_Approach->create_Menu();
			global_Multilayer_Approach->setWindowFlags(Qt::Window);
			global_Multilayer_Approach->show();
		runned_Multilayer_Approaches.insert(multilayer_Approach_Key, global_Multilayer_Approach);

	} else
	{
		global_Multilayer_Approach->activateWindow();
	}
}
