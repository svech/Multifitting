#include "menu.h"
#include "multilayer_approach/table_of_structures/table_of_structures.h"

const QString engManualName = "Multifitting_manual_English_v2.0.pdf";
const QString rusManualName = "Multifitting_manual_Russian_v2.0.pdf";

Menu::Menu(QString window_Type, QWidget *parent):
	window_Type(window_Type),
	QWidget(parent),
	my_Parent(parent)
{
	menu_Bar = new QMenuBar(this);
	add_Menu_Points();
}

Menu::~Menu()
{
}

void Menu::add_Menu_Points()
{
	if(window_Type == window_Type_Multilayer_Approach)
	{
		create_File_Menu();
			menu_Bar->addMenu(file_Menu);
		create_Calculate_Menu();
			menu_Bar->addMenu(calculate_Menu);
		create_Optical_Constants_Menu();
			menu_Bar->addMenu(optical_Constants_Menu);
		create_Help_Menu();
			menu_Bar->addMenu(help_Menu);
	}
	if(window_Type == window_Type_Item_Editor)
	{
		item_Editor = qobject_cast<Item_Editor*>(my_Parent);

        QString item_Type = item_Editor->item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type;

        if(item_Type == item_Type_Layer     ||
           item_Type == item_Type_Substrate ||
           item_Type == item_Type_Multilayer)
        {
            create_Length_Units_Menu();
                menu_Bar->addMenu(menu_Length_Units);
        }
        if(item_Type == item_Type_Ambient   ||
           item_Type == item_Type_Layer     ||
           item_Type == item_Type_Substrate ||
           item_Type == item_Type_Multilayer)
        {
            create_Precision_Menu();
                menu_Bar->addMenu(precision_Menu);
        }
	}
	if(window_Type == window_Type_Table_Of_Structures)
	{
		create_File_Menu();
			menu_Bar->addMenu(file_Menu);
		create_Calculate_Menu();
			menu_Bar->addMenu(calculate_Menu);
		create_Length_Units_Menu();
			menu_Bar->addMenu(menu_Length_Units);
		create_Other_Units_Menu();
			menu_Bar->addMenu(menu_Other_Units);
		create_Precision_Menu();
			menu_Bar->addMenu(precision_Menu);
	}
	if(window_Type == window_Type_Regular_Aperiodic_Table )
	{
		create_File_Menu();
//			menu_Bar->addMenu(file_Menu);
		create_Length_Units_Menu();
			menu_Bar->addMenu(menu_Length_Units);
		create_Precision_Menu();
			menu_Bar->addMenu(precision_Menu);
	}
//	if(window_Type == window_Type_Launcher)
//	{
//		create_File_Menu();
//			menu_Bar->addMenu(file_Menu);
//		create_Help_Menu();
//			menu_Bar->addMenu(help_Menu);
//	}
}

void Menu::create_File_Menu()
{
	file_Menu = new QMenu("File", this);
	{
		if(window_Type == window_Type_Multilayer_Approach || window_Type == window_Type_Table_Of_Structures)
		{
			QAction* act_Open = new QAction("Open last", this);
				act_Open->setShortcut(Qt::Key_O | Qt::CTRL);
            connect(act_Open, &QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::open_Action);
			file_Menu->addAction(act_Open);

			QAction* act_Open_As = new QAction("Open", this);
				act_Open_As->setShortcut(Qt::Key_O | Qt::CTRL | Qt::SHIFT);
            connect(act_Open_As, &QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::open_As);
			file_Menu->addAction(act_Open_As);

			QAction* act_Save = new QAction("Save", this);
			act_Save->setShortcut(Qt::Key_S | Qt::CTRL);
			file_Menu->addAction(act_Save);
            connect(act_Save, &QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::save_Action);

			QAction* act_Save_As = new QAction("Save as", this);
				act_Save_As->setShortcut(Qt::Key_S | Qt::CTRL | Qt::SHIFT);
			file_Menu->addAction(act_Save_As);
            connect(act_Save_As, &QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::save_As);

			QAction* act_Export_Structures = new QAction("Export structures", this);
                act_Export_Structures->setShortcut(Qt::Key_T | Qt::CTRL | Qt::SHIFT);
			file_Menu->addAction(act_Export_Structures);
            connect(act_Export_Structures, &QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::export_Structures);

			QAction* act_Export_Curves = new QAction("Export curves", this);
//				act_Export_Curves->setShortcut(Qt::Key_ | Qt::CTRL | Qt::SHIFT);
			file_Menu->addAction(act_Export_Curves);
            connect(act_Export_Curves, &QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::export_Curves);

            QAction* act_Export_Profile = new QAction("Export profile", this);
                act_Export_Profile->setShortcut(Qt::Key_P | Qt::CTRL | Qt::SHIFT);
            file_Menu->addAction(act_Export_Profile);
            connect(act_Export_Profile, &QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::export_Profiles);
		}
		if(window_Type == window_Type_Table_Of_Structures)
		{
			QAction* act_Quit = new QAction("Close", this);
			act_Quit->setShortcut(Qt::Key_D | Qt::CTRL);
			connect(act_Quit, &QAction::triggered, my_Parent, [=]{qobject_cast<QWidget*>(my_Parent)->close();});
			file_Menu->addAction(act_Quit);
		}
		if(window_Type == window_Type_Launcher ||
		   window_Type == window_Type_Multilayer_Approach)
		{
			QAction* act_Quit = new QAction("Close all",this);
			act_Quit->setShortcut(Qt::Key_Q | Qt::CTRL);
			connect(act_Quit, &QAction::triggered, qApp, &QApplication::quit);
			file_Menu->addAction(act_Quit);
		}
	}
}

void Menu::create_Calculate_Menu()
{
	calculate_Menu = new QMenu("Calculate", this);

	QAction* act_Specular = new QAction("Calculate curves", this);
		act_Specular->setShortcut(Qt::Key_C | Qt::CTRL | Qt::SHIFT);

	QAction* act_Fitting = new QAction("Start fitting", this);
		act_Fitting->setShortcut(Qt::Key_F | Qt::CTRL | Qt::SHIFT);

	QAction* act_Confidence = new QAction("Calculate confidence intervals", this);
		act_Confidence->setShortcut(Qt::Key_A | Qt::CTRL | Qt::SHIFT);

	QAction* act_Abort = new QAction("Abort calculation", this);
		act_Abort->setShortcut(Qt::Key_Period | Qt::ALT);
		act_Abort->setProperty(abort_Property,abort_Property);

	if(window_Type == window_Type_Multilayer_Approach || window_Type == window_Type_Table_Of_Structures)
	{
        connect(act_Specular,	&QAction::triggered, global_Multilayer_Approach, [=]{global_Multilayer_Approach->calculate();});
        connect(act_Fitting,	&QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::start_Fitting);
        connect(act_Confidence,	&QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::calc_Confidence_Intervals);
        connect(act_Abort,		&QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::abort_Calculations);

		calculate_Menu->addAction(act_Specular);
		calculate_Menu->addAction(act_Fitting);
		calculate_Menu->addAction(act_Confidence);
		calculate_Menu->addAction(act_Abort);
	}
}

void Menu::create_Optical_Constants_Menu()
{
	optical_Constants_Menu = new QMenu("Optical constants", this);

	QAction* act_optical_Constants = new QAction("Reload optical constants", this);
    connect(act_optical_Constants, &QAction::triggered, global_Multilayer_Approach, &Multilayer_Approach::reload_Optical_Constants);
	optical_Constants_Menu->addAction(act_optical_Constants);
}

void Menu::create_Length_Units_Menu()
{
	// PARAMETER

	menu_Length_Units = new QMenu("Length units", this);

	QActionGroup* group_Act_Unit = new QActionGroup(this);
		group_Act_Unit->setExclusive(true);

	for(int index=0; index<length_Units_List.size(); index++)
	{
		QAction* act_Unit = new QAction(length_Units_List[index], this);
			act_Unit->setCheckable(true);
			act_Unit->setChecked(length_Units_List[index] == length_units);
			act_Unit->setActionGroup(group_Act_Unit);

		menu_Length_Units->addAction(act_Unit);
		connect(act_Unit,  &QAction::triggered, this, [=]
		{
			length_units = length_Units_List[index];
			emit refresh();
		});
	}
}

void Menu::create_Other_Units_Menu()
{
	// PARAMETER
	menu_Other_Units = new QMenu("Other units", this);

//	QMenu* menu_Length_Units			 = new QMenu("Lengths", this);
//	units_Menu_Map.		insert(menu_Length_Units, &length_units);
//	units_List_Menu_Map.insert(menu_Length_Units, &length_Units_List);
//	menu_Other_Units-> addMenu(menu_Length_Units);
	QMenu* menu_Correlation_Length_Units = new QMenu("Lateral correlation: D, "+Xi_Sym, this);
	units_Menu_Map.		insert(menu_Correlation_Length_Units, &correlation_length_units);
	units_List_Menu_Map.insert(menu_Correlation_Length_Units, &correlation_Length_Units_List);
	menu_Other_Units-> addMenu(menu_Correlation_Length_Units);
	QMenu* menu_Mu_Units				 = new QMenu("Vertical correlation: L"+Subscript_v_Sym, this);
	units_Menu_Map.		insert(menu_Mu_Units, &mu_units);
	units_List_Menu_Map.insert(menu_Mu_Units, &mu_Units_List);
	menu_Other_Units-> addMenu(menu_Mu_Units);
	QMenu* menu_Omega_Units				 = new QMenu("Particle volume: "+Omega_Big_Sym, this);
	units_Menu_Map.		insert(menu_Omega_Units, &omega_units);
	units_List_Menu_Map.insert(menu_Omega_Units, &omega_Units_List);
	menu_Other_Units-> addMenu(menu_Omega_Units);
	QMenu* menu_a2_Units				 = new QMenu("Linear growth exponent factor: a"+Subscript_2_Sym, this);
	units_Menu_Map.		insert(menu_a2_Units, &linear_a2_units);
	units_List_Menu_Map.insert(menu_a2_Units, &linear_a2_Units_List);
	menu_Other_Units-> addMenu(menu_a2_Units);
	QMenu* menu_a3_Units				 = new QMenu("Linear growth exponent factor: a"+Subscript_3_Sym, this);
	units_Menu_Map.		insert(menu_a3_Units, &linear_a3_units);
	units_List_Menu_Map.insert(menu_a3_Units, &linear_a3_Units_List);
	menu_Other_Units-> addMenu(menu_a3_Units);
	QMenu* menu_a4_Units				 = new QMenu("Linear growth exponent factor: a"+Subscript_4_Sym, this);
	units_Menu_Map.		insert(menu_a4_Units, &linear_a4_units);
	units_List_Menu_Map.insert(menu_a4_Units, &linear_a4_Units_List);
	menu_Other_Units-> addMenu(menu_a4_Units);
	QMenu* menu_Frequency_Units			 = new QMenu("Spatial frequency: "+Nu_Sym+", "+Delta_Small_Sym+Nu_Sym, this);
	units_Menu_Map.		insert(menu_Frequency_Units, &spatial_frequency_units);
	units_List_Menu_Map.insert(menu_Frequency_Units, &spatial_Frequency_Units_List);
	menu_Other_Units-> addMenu(menu_Frequency_Units);

	// functionality
	for(QMenu* current_Menu : units_Menu_Map.keys())
	{
		QActionGroup* group_Units  = new QActionGroup(this);
			group_Units->setExclusive(true);

		QString* current_units = units_Menu_Map.value(current_Menu);
		QStringList* units_List = units_List_Menu_Map.value(current_Menu);
		for(QString unit_String : *units_List)
		{
			QAction* units_Action = new QAction(unit_String);
				units_Action->setCheckable(true);
				units_Action->setChecked(unit_String == *current_units);
				units_Action->setActionGroup(group_Units);
			current_Menu->addAction(units_Action);
			connect(units_Action, &QAction::triggered, this, [=]
			{
				*current_units = unit_String;
				emit refresh();
			});
		}
	}
}

void Menu::create_Precision_Menu()
{
	// PARAMETER
	precision_Menu = new QMenu("Precision", this);

	QMenu* menu_Composition_Precision				= new QMenu("Atomic composition: "+Zeta_Sym, this);
	precision_Menu_Map.insert(menu_Composition_Precision, &line_edit_composition_precision);
	QMenu* menu_Density_Precision					= new QMenu("Density: "+Rho_Sym+", "+Rho_Sym + Subscript_p_Sym, this);
	precision_Menu_Map.insert(menu_Density_Precision, &line_edit_density_precision);
	QMenu* menu_Thickness_Precision					= new QMenu("Thickness: z, d", this);;
	precision_Menu_Map.insert(menu_Thickness_Precision, &line_edit_thickness_precision);
	QMenu* menu_Gamma_Precision						= new QMenu("Gamma: "+Gamma_Sym,  this);
	precision_Menu_Map.insert(menu_Gamma_Precision, &line_edit_gamma_precision);
	QMenu* menu_Sigma_Precision						= new QMenu("Sigma: s, "+Sigma_Sym+", "+Sigma_Sym+Subscript_v_Sym, this);
	precision_Menu_Map.insert(menu_Sigma_Precision, &line_edit_sigma_precision);
	QMenu* menu_Interlayer_Precision				= new QMenu("Interlayer composition", this);
	precision_Menu_Map.insert(menu_Interlayer_Precision, &line_edit_interlayer_precision);
	QMenu* menu_Drift_Precision						= new QMenu("Drift: dz, ds", this);
	precision_Menu_Map.insert(menu_Drift_Precision, &line_edit_drift_precision);
	QMenu* menu_Correlation_Length_Precision		= new QMenu("Lateral correlation: D, "+Xi_Sym, this);
	precision_Menu_Map.insert(menu_Correlation_Length_Precision, &line_edit_cor_radius_precision);
	QMenu* menu_Alpha_Beta_Precision				= new QMenu("Fractality: "+Alpha_Sym+", "+Beta_Sym, this);
	precision_Menu_Map.insert(menu_Alpha_Beta_Precision, &line_edit_fractal_alpha_beta_precision);
	QMenu* menu_Mu_Precision						= new QMenu("Vertical correlation: L"+Subscript_v_Sym, this);
	precision_Menu_Map.insert(menu_Mu_Precision, &line_edit_mu_precision);
	QMenu* menu_Omega_Precision						= new QMenu("Particle volume: "+Omega_Big_Sym, this);
	precision_Menu_Map.insert(menu_Omega_Precision, &line_edit_omega_precision);
	QMenu* menu_Linear_A_Precision					= new QMenu("Linear growth exponent factor: a"+Subscript_1_Sym+", a"+Subscript_2_Sym+", a"+Subscript_3_Sym+", a"+Subscript_4_Sym, this);
	precision_Menu_Map.insert(menu_Linear_A_Precision, &line_edit_linear_a_precision);
	QMenu* menu_Frequency_Precision					= new QMenu("Spatial frequency: "+Nu_Sym+", "+Delta_Small_Sym+Nu_Sym, this);
	precision_Menu_Map.insert(menu_Frequency_Precision, &line_edit_frequency_precision);
	QMenu* menu_PSD_Factor_Precision				= new QMenu("Measured roughness factor: rf", this);
	precision_Menu_Map.insert(menu_PSD_Factor_Precision, &line_edit_psd_factor_precision);
	QMenu* menu_Particle_Size_Precision				= new QMenu("Particle dimensions: R, H", this);
	precision_Menu_Map.insert(menu_Particle_Size_Precision, &line_edit_particle_size_precision);
	QMenu* menu_Particle_Lateral_Distance_Precision	= new QMenu("Particle lateral distance: r, r"+Subscript_a_Sym+", "+Delta_Small_Sym+"r, "+Delta_Big_Sym, this);
	precision_Menu_Map.insert(menu_Particle_Lateral_Distance_Precision, &line_edit_particle_lateral_distance_precision);
	QMenu* menu_Particle_Z_Position_Precision		= new QMenu("Particle z position: z"+Subscript_p_Sym+", "+Delta_Small_Sym+"z"+Subscript_p_Sym, this);
	precision_Menu_Map.insert(menu_Particle_Z_Position_Precision, &line_edit_particle_z_position_precision);

	if(window_Type == window_Type_Table_Of_Structures)
	{
		precision_Menu->addMenu(menu_Composition_Precision);
		precision_Menu->addMenu(menu_Density_Precision);
		precision_Menu->addMenu(menu_Thickness_Precision);
		precision_Menu->addMenu(menu_Gamma_Precision);
		precision_Menu->addMenu(menu_Sigma_Precision);
		precision_Menu->addMenu(menu_Interlayer_Precision);
		precision_Menu->addMenu(menu_Drift_Precision);
		precision_Menu->addMenu(menu_Correlation_Length_Precision);
		precision_Menu->addMenu(menu_Alpha_Beta_Precision);
		precision_Menu->addMenu(menu_Mu_Precision);
		precision_Menu->addMenu(menu_Omega_Precision);
		precision_Menu->addMenu(menu_Linear_A_Precision);
		precision_Menu->addMenu(menu_Frequency_Precision);
		precision_Menu->addMenu(menu_PSD_Factor_Precision);
		precision_Menu->addMenu(menu_Particle_Size_Precision);
		precision_Menu->addMenu(menu_Particle_Lateral_Distance_Precision);
		precision_Menu->addMenu(menu_Particle_Z_Position_Precision);
	}
	if(window_Type == window_Type_Regular_Aperiodic_Table)
	{
		precision_Menu->addMenu(menu_Thickness_Precision);
		precision_Menu->addMenu(menu_Sigma_Precision);
	}

	if(window_Type == window_Type_Item_Editor)
	{
		QString item_Type = item_Editor->item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type;

		if(item_Type == item_Type_Ambient ||
		   item_Type == item_Type_Layer   ||
		   item_Type == item_Type_Substrate)
		{
			precision_Menu->addMenu(menu_Composition_Precision);
			precision_Menu->addMenu(menu_Density_Precision);
		}
        if(item_Type == item_Type_Multilayer ||
		   item_Type == item_Type_Layer	   ||
		   item_Type == item_Type_Substrate)
		{
			precision_Menu->addMenu(menu_Thickness_Precision);
		}
		if(item_Type == item_Type_Multilayer)
		{
			precision_Menu->addMenu(menu_Gamma_Precision);
		}
		if(item_Type == item_Type_Layer   ||
		   item_Type == item_Type_Substrate)
		{
			precision_Menu->addMenu(menu_Interlayer_Precision);
			precision_Menu->addMenu(menu_Sigma_Precision);
		}
	}

	// functionality
	for(QMenu* current_Menu : precision_Menu_Map.keys())
	{
		QActionGroup* group_Precision  = new QActionGroup(this);
			group_Precision->setExclusive(true);

		int* current_line_edit_precision = precision_Menu_Map.value(current_Menu);
		for(int index=0; index<MAX_PRECISION_USED; index++)
		{
			QAction* precision_Action   = new QAction(Locale.toString(index));
				precision_Action->setCheckable(true);
				precision_Action->setChecked(index == *current_line_edit_precision);
				precision_Action->setActionGroup(group_Precision);
			current_Menu->addAction(precision_Action);
			connect(precision_Action, &QAction::triggered, this, [=]
			{
				*current_line_edit_precision = index;
				emit refresh();
			});
		}
	}
}

void Menu::create_Help_Menu()
{
	help_Menu = new QMenu("Help",this);
	{
		QAction* act_Documentation_Rus = new QAction("Multifitting (Russian).pdf", this);
		help_Menu->addAction(act_Documentation_Rus);
		connect(act_Documentation_Rus, &QAction::triggered, this, &Menu::open_Documentation_Rus);
	}
	{
		QAction* act_Documentation_Eng = new QAction("Multifitting (English).pdf", this);
		help_Menu->addAction(act_Documentation_Eng);
		connect(act_Documentation_Eng, &QAction::triggered, this, &Menu::open_Documentation_Eng);
	}
	{
		QAction* act_About = new QAction("About Multifitting",this);
		help_Menu->addAction(act_About);
		connect(act_About, &QAction::triggered, this, &Menu::open_About);
	}
}

//// menu actions

void Menu::open_Documentation_Rus()
{
#ifdef _WIN32
    QDesktopServices::openUrl(QUrl::fromLocalFile(Pre_Path+rusManualName));
#endif
#ifdef __linux__
    QDesktopServices::openUrl(QUrl(Pre_Path+rusManualName));
#endif
}

void Menu::open_Documentation_Eng()
{
#ifdef _WIN32
    QDesktopServices::openUrl(QUrl::fromLocalFile(Pre_Path+engManualName));
#endif
#ifdef __linux__
    QDesktopServices::openUrl(QUrl(Pre_Path+engManualName));
#endif
}

void Menu::open_About()
{
	QMessageBox about_Box;
	about_Box.setTextFormat(Qt::RichText);
	about_Box.setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
	about_Box.setWindowTitle("About");
	about_Box.setText(  "Multifitting v."+QString::number(VERSION_MAJOR)+"."+QString::number(VERSION_MINOR)+"."+QString::number(VERSION_BUILD)+"<br><br>"+
						"Mikhail Svechnikov<br>"
						"svechnikovmv@gmail.com<br>"
                        "2018-2023<br><br>"
						"Multifitting is based on Qt5 framework<br>"//(GNU LGPL/GPL)
						"and also uses the following libraries:<br><br>"
						"<a href='https://www.gnu.org/software/gsl/'>GSL</a> - GNU Scientific Library<br>"//(GNU GPL)
						"<a href='https://www.boost.org/'>Boost</a> - Boost C++ libraries<br>"//(Boost Software Licence (BSL))
						"<a href='http://ab-initio.mit.edu/wiki/index.php/Faddeeva_Package'>Faddeeva Package</a> by Steven G. Johnson<br>"//(MIT licence)
                        "<a href='https://github.com/Hvass-Labs/swarmops-other'>SwarmOps</a> by Magnus Erik Hvass Pedersen<br>"//(LGPL/MIT licence)
                        "<a href='https://github.com/kpeeters/tree.hh'>tree.hh</a> by Kasper Peeters<br>" //(GNU GPL)
						"<a href='http://www.partow.net/programming/exprtk/'>ExprTk</a> by Arash Partow<br>"//(MIT licence)
						"<a href='https://www.qcustomplot.com/'>QCustomPlot</a> by Emanuel Eichhammer<br>"//(GNU GPL)
						"<br>"
						"Main part of optical constants<br>"
						"database and some pieces of design<br>"
						"are borrowed from <a href='http://www.rxollc.com/idl/'>IMD</a> by Dawid Windt<br>"
						"<br>"
						"Form factors for scattering on<br>"
						"particles are taken from <a href='https://www.bornagainproject.org/'>BornAgain</a><br>"
					  );
	about_Box.setIcon(QMessageBox::Information);
	about_Box.exec();
}
