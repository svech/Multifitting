#include "launcher.h"

Launcher::Launcher():
	gui_Settings(Gui_Settings_Path, QSettings::IniFormat),
	default_Values(Default_Values_Path, QSettings::IniFormat)
{	
//	reset_Gui_Settings();
	reset_Default_Values();

	gui_Settings.beginGroup( Application_Style );
		QString application_Style = gui_Settings.value( "application_Style", 0 ).toString();
	gui_Settings.endGroup();
		qApp->setStyle(QStyleFactory::create(application_Style));

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
	gui_Settings.beginGroup( Launcher_Geometry );
		int launcher_X_Corner = gui_Settings.value( "launcher_X_Corner", 0 ).toInt();
		int launcher_Y_Corner = gui_Settings.value( "launcher_Y_Corner", 0 ).toInt();
	gui_Settings.endGroup();

	adjustSize();
	setFixedSize(size());

	setGeometry(launcher_X_Corner, launcher_Y_Corner, 0, 0);
}

void Launcher::reset_Gui_Settings()
{
	QMessageBox::warning(this,"Warning","Reset Gui Settings");
	qInfo() << "Reset Gui Settings";

	// application style
	gui_Settings.beginGroup( Application_Style );
		gui_Settings.setValue( "application_Style", "WindowsXP" ); // Fusion Windows WindowsXP WindowsVista
	gui_Settings.endGroup();

	// launcher window geometry
	gui_Settings.beginGroup( Launcher_Geometry );
		gui_Settings.setValue( "launcher_X_Corner", 300 );
		gui_Settings.setValue( "launcher_Y_Corner", 300 );
	gui_Settings.endGroup();

	// multilayer window geometry
	gui_Settings.beginGroup( Multilayer_Window_Geometry );
		gui_Settings.setValue( "multilayer_X_Corner", 500 );
		gui_Settings.setValue( "multilayer_Y_Corner", 200 );
		gui_Settings.setValue( "multilayer_Min_Width", 300 );
		gui_Settings.setValue( "multilayer_Width", 531 );
		gui_Settings.setValue( "multilayer_Min_Height", 100 );
		gui_Settings.setValue( "multilayer_Height", 500 );
		gui_Settings.setValue( "multilayer_Height_Additive", 23 );
	gui_Settings.endGroup();

	// multilayer tab name
	gui_Settings.beginGroup( Multilayer_Tabs );
		gui_Settings.setValue( "default_Multilayer_Tab_Name", "Struct_" );
		gui_Settings.setValue( "default_New_Struct_Name", "New Struct" );
	gui_Settings.endGroup();

	// resource path
	gui_Settings.beginGroup( Paths );
		gui_Settings.setValue( "icon_Path", "../../imd icons/" );
	gui_Settings.endGroup();
}

void Launcher::reset_Default_Values()
{
//	QMessageBox::warning(this,"Warning","Reset Default Values");
	qInfo() << "Reset Default Values";

	// Structure Init Values
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Ambient_Values );
			default_Values.setValue( "ambient_default_material", "Vacuum" );
			default_Values.setValue( "ambient_default_density", 0 );
			default_Values.setValue( "ambient_default_composed", false );
			default_Values.setValue( "ambient_default_stoichiometry_composition", 1 );
			default_Values.setValue( "ambient_default_stoichiometry_element", "Al" );
		default_Values.endGroup();
		default_Values.beginGroup( Layer_Values );
			default_Values.setValue( "layer_default_material", "Al" );
			default_Values.setValue( "layer_default_density", 1 );
			default_Values.setValue( "layer_default_composed", false );
			default_Values.setValue( "layer_default_stoichiometry_composition", 1 );
			default_Values.setValue( "layer_default_stoichiometry_element", "Al" );
			default_Values.setValue( "layer_default_sigma", 123.5678 );
			default_Values.setValue( "layer_default_interlayer_composition", 1 );
			default_Values.setValue( "layer_default_interlayer_type", "erf" );
			default_Values.setValue( "layer_default_thickness", 10 );
			default_Values.setValue( "layer_default_drift_model", "no_drift" );
			default_Values.setValue( "layer_default_drift_coefficients", 0 );
		default_Values.endGroup();
		default_Values.beginGroup( Substrate_Values );
			default_Values.setValue( "substrate_default_material", "Si" );
			default_Values.setValue( "substrate_default_density", 2.33 );
			default_Values.setValue( "substrate_default_composed", false );
			default_Values.setValue( "substrate_default_stoichiometry_composition", 1 );
			default_Values.setValue( "substrate_default_stoichiometry_element", "Be" );
			default_Values.setValue( "substrate_default_sigma", 21.2 );
			default_Values.setValue( "substrate_default_interlayer_composition", 1 );
			default_Values.setValue( "substrate_default_interlayer_type", "erf" );
		default_Values.endGroup();
		default_Values.beginGroup( Stack_Values );
			default_Values.setValue( "stack_default_number_of_repetition", 1 );
			default_Values.setValue( "stack_default_period", 20 );
			default_Values.setValue( "stack_default_gamma", 0.5 );
		default_Values.endGroup();
	default_Values.endGroup();

	// color, precision, units etc
	default_Values.beginGroup( Structure_Values_Representation );
		default_Values.setValue( "default_precision", 2 );
		default_Values.setValue( "default_density_precision", 3 );
		default_Values.setValue( "default_atomic_weight_precision", 4 );
	default_Values.endGroup();
}

// slots

void Launcher::on_Button_Multilayer_Clicked()
{
	Multilayer_Approach* multilayer_Approach = new Multilayer_Approach;
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




