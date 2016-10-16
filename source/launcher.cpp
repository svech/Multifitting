#include "launcher.h"

Launcher::Launcher():
	gui_Settings(Gui_Settings_Path, QSettings::IniFormat),
	default_Values(Default_Values_Path, QSettings::IniFormat)
{	
	reset_Gui_Settings();
	reset_Default_Values();
	Global_Definitions::set_Settings();

	qApp->setStyle(QStyleFactory::create(application_style));

	// place ui elements
	setWindowTitle("Multifitting");
	create_Main_Layout();
	set_Window_Geometry();
}

void Launcher::closeEvent(QCloseEvent* event)
{
	// TODO close launcher
	qApp->quit();
	event=NULL;
}

void Launcher::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
	create_Menu();
		main_Layout->setMenuBar(menu_Bar);
	create_Buttons();
		main_Layout->addWidget(multilayer_Launch_Button);
		main_Layout->addWidget(freeform_Launch_Button);
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
	multilayer_Launch_Button = new QPushButton("Multilayer");
		multilayer_Launch_Button->setEnabled(true);
		multilayer_Launch_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	freeform_Launch_Button	 = new QPushButton("Freeform");
		freeform_Launch_Button  ->setEnabled(false); // todo enabled
		freeform_Launch_Button  ->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	connect(multilayer_Launch_Button, SIGNAL(clicked()), this, SLOT(on_Button_Multilayer_Clicked()));
	connect(freeform_Launch_Button,   SIGNAL(clicked()), this, SLOT(on_Button_Freeform_Clicked  ()));
}

void Launcher::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
	setGeometry(launcher_x_corner, launcher_x_corner, 0, 0);
}

void Launcher::reset_Gui_Settings()
{
//	QMessageBox::warning(this,"Warning","Reset Gui Settings");
	qInfo() << "Reset Gui Settings";

	// application style
	gui_Settings.beginGroup( Application_Style );
		gui_Settings.setValue( "application_style", "WindowsXP" ); // Fusion Windows WindowsXP WindowsVista
	gui_Settings.endGroup();

	// launcher window geometry
	gui_Settings.beginGroup( Launcher_Geometry );
		gui_Settings.setValue( "launcher_x_corner", 300 );
		gui_Settings.setValue( "launcher_y_corner", 300 );
	gui_Settings.endGroup();

	// multilayer window geometry
	gui_Settings.beginGroup( Multilayer_Window_Geometry );
		gui_Settings.setValue( "multilayer_x_corner",			500 );
		gui_Settings.setValue( "multilayer_y_corner",			200 );
		gui_Settings.setValue( "multilayer_min_width",			300 );
		gui_Settings.setValue( "multilayer_min_height",			100 );
		gui_Settings.setValue( "multilayer_width",				531 );
		gui_Settings.setValue( "multilayer_height",				500 );
		gui_Settings.setValue( "multilayer_height_additive",	23 );
	gui_Settings.endGroup();

	// multilayer tab name
	gui_Settings.beginGroup( Multilayer_Tabs );
		gui_Settings.setValue( "default_multilayer_tab_name",			"Struct_" );
		gui_Settings.setValue( "default_independent_variable_tab_name", "Plot_"   );
	gui_Settings.endGroup();

	// resource path
	gui_Settings.beginGroup( Paths );
		gui_Settings.setValue( "icon_path", "../../imd icons/" );
	gui_Settings.endGroup();
}

void Launcher::reset_Default_Values()
{
//	QMessageBox::warning(this,"Warning","Reset Default Values");
	qInfo() << "Reset Default Values";

	// structure init values
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Ambient_Values );
			default_Values.setValue( "ambient_default_material",					"Mo"	);
			default_Values.setValue( "ambient_default_density",						0		);
			default_Values.setValue( "ambient_default_permittivity",				100		);
			default_Values.setValue( "ambient_default_absorption",					100		);
			default_Values.setValue( "ambient_default_composed",					false	);
			default_Values.setValue( "ambient_default_stoichiometry_composition",	1		);
			default_Values.setValue( "ambient_default_stoichiometry_element",		"Al"	);
		default_Values.endGroup();
		default_Values.beginGroup( Layer_Values );
			default_Values.setValue( "layer_default_material",					"Al"		);
			default_Values.setValue( "layer_default_density",					1			);
			default_Values.setValue( "layer_default_permittivity",				100			);
			default_Values.setValue( "layer_default_absorption",				100			);
			default_Values.setValue( "layer_default_composed",					false		);
			default_Values.setValue( "layer_default_stoichiometry_composition", 1			);
			default_Values.setValue( "layer_default_stoichiometry_element",		"Al"		);
			default_Values.setValue( "layer_default_sigma",						123.568		);
			default_Values.setValue( "layer_default_thickness",					10			);
			default_Values.setValue( "layer_default_drift_model",				"no_drift"	);
			default_Values.setValue( "layer_default_drift_coefficients",		0			);
		default_Values.endGroup();
		default_Values.beginGroup( Substrate_Values );
			default_Values.setValue( "substrate_default_material",					"Si"	);
			default_Values.setValue( "substrate_default_density",					2.33	);
			default_Values.setValue( "substrate_default_permittivity",				100		);
			default_Values.setValue( "substrate_default_absorption",				100		);
			default_Values.setValue( "substrate_default_composed",					false	);
			default_Values.setValue( "substrate_default_stoichiometry_composition", 1		);
			default_Values.setValue( "substrate_default_stoichiometry_element",		"Be"	);
			default_Values.setValue( "substrate_default_sigma",						21.2	);
		default_Values.endGroup();
		default_Values.beginGroup( Stack_Values );
			default_Values.setValue( "stack_default_number_of_repetition",	1	);
			default_Values.setValue( "stack_default_period",				20	);
			default_Values.setValue( "stack_default_gamma",					0.5 );
		default_Values.endGroup();
	default_Values.endGroup();

	// color, precision, units etc
	default_Values.beginGroup( Structure_Values_Representation );

	// qlineedit precisions
		default_Values.setValue( "line_edit_double_format",		   'g');
		default_Values.setValue( "line_edit_angle_precision",		7 );
		default_Values.setValue( "line_edit_wavelength_precision",	7 );
		default_Values.setValue( "line_edit_density_precision",		7 );
		default_Values.setValue( "line_edit_permittivity_precision",7 );
		default_Values.setValue( "line_edit_absorption_precision",	7 );
		default_Values.setValue( "line_edit_composition_precision", 7 );
		default_Values.setValue( "line_edit_thickness_precision",	7 );
		default_Values.setValue( "line_edit_sigma_precision",		7 );
		default_Values.setValue( "line_edit_interlayer_precision",	7 );
		default_Values.setValue( "line_edit_period_precision",		7 );
		default_Values.setValue( "line_edit_gamma_precision",		7 );

	// thumbnail precisions
		default_Values.setValue( "thumbnail_double_format",		   'f');
		default_Values.setValue( "thumbnail_angle_precision",		3 );
		default_Values.setValue( "thumbnail_wavelength_precision",	3 );
		default_Values.setValue( "thumbnail_density_precision",		3 );
		default_Values.setValue( "thumbnail_permittivity_precision",3 );
		default_Values.setValue( "thumbnail_absorption_precision",	3 );
		default_Values.setValue( "thumbnail_composition_precision", 3 );
		default_Values.setValue( "thumbnail_thickness_precision",	3 );
		default_Values.setValue( "thumbnail_sigma_precision",		3 );
		default_Values.setValue( "thumbnail_interlayer_precision",	3 );
		default_Values.setValue( "thumbnail_period_precision",		3 );
		default_Values.setValue( "thumbnail_gamma_precision",		3 );

	// other precisions
		default_Values.setValue( "at_weight_precision",				5 );
	default_Values.endGroup();

	// parameters init values
	default_Values.beginGroup( Parameters_Init_Values );
		default_Values.beginGroup( Independent_Values );
			default_Values.setValue( "default_num_angular_points",	1 );
			default_Values.setValue( "default_num_spectral_points", 1 );
			default_Values.setValue( "default_min_angle",			0 );
			default_Values.setValue( "default_max_angle",			7 );
			default_Values.setValue( "default_min_wavelength",		1.54056 );
			default_Values.setValue( "default_max_wavelength",		180 );
			default_Values.setValue( "default_angle_type", int(Angle_Type::Grazing) );
			default_Values.setValue( "default_angular_resolution",	0 );
			default_Values.setValue( "default_polarization",			0 );
			default_Values.setValue( "default_spectral_resolution",		0 );
			default_Values.setValue( "default_polarization_sensitivity",0 );
		default_Values.endGroup();
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
	// TODO freeform
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
