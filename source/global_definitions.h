#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <QtWidgets>

// gui settings
#define Gui_Settings_Path "../../settings/gui_settings.ini"
	#define Application_Style "Application_Style"
	#define Launcher_Geometry "Launcher_Geometry"
	#define Multilayer_Window_Geometry "Multilayer_Window_Geometry"
	#define Multilayer_Tabs "Multilayer_Tabs"
	#define Paths "Paths"

// default values settings
#define Default_Values_Path "../../settings/default_values.ini"
	#define Structure_Init_Values "Structure_Init_Values"
		#define Ambient_Values "Ambient_Values"
		#define Layer_Values "Layer_Values"
		#define Substrate_Values "Substrate_Values"
		#define Stack_Values "Stack_Values"
	#define Structure_Values_Representation "Structure_Values_Representation"
	#define Parameters_Init_Values "Parameters_Init_Values"
		#define Independent_Values "Independent_Values"

#define Angstrom_Sym	QString(QChar(0x212B))
#define Rho_Sym			QString(QChar(0x03C1))
#define Cube_Sym		QString(QChar(0x00B3))
#define Sigma_Sym		QString(QChar(0x03C3))
#define Gamma_Sym		QString(QChar(0x03B3))
#define Epsilon_Sym		QString(QChar(0x03B5))
#define Cappa_Sym		QString(QChar(0x03F0))
#define Alpha_Sym		QString(QChar(0x03B1))
#define Zeta_Sym		QString(QChar(0x03B6))
#define Theta_Sym		QString(QChar(0x03B8))
#define Lambda_Sym		QString(QChar(0x03BB))
#define Degree_Sym		QString(QChar(0x00B0))



#define MAX_DOUBLE 1E100
#define MAX_INTEGER MAXINT
#define MAX_PRECISION 100

extern int default_Column;
extern int num_Permanent_Independent_Variables;

extern QString whats_This_Delimiter;
extern QString item_Type_Delimiter;

// main whatsThis
extern QString whats_This_Measurement;
extern QString whats_This_Ambient;
extern QString whats_This_Layer;
extern QString whats_This_Multilayer;
extern QString whats_This_Substrate;

// specialized whatsThis additions
extern QString whats_This_Angle;
extern QString whats_This_Wavelength;
extern QString whats_This_Density;
extern QString whats_This_Permittivity;
extern QString whats_This_Absorption;
extern QString whats_This_Composition;
extern QString whats_This_Thickness;
extern QString whats_This_Sigma;
extern QString whats_This_Interlayer_Composition;
extern QString whats_This_Num_Repetitions;
extern QString whats_This_Period;
extern QString whats_This_Gamma;

// properties
#define min_Size "min_Size"
#define true_Value "true_Value"

extern QStringList element_Name;
extern QVector<double> element_Mass;

struct Independent				{bool is_Independent=false;	double min; double max; int num_Points;};
struct Coupled					{bool is_Coupled; };
struct Fit						{bool is_Fitable=false;		bool min_Bounded; double min;
															bool max_Bounded; double max;};
struct Optimize					{bool is_Optimizable=false;	bool min_Bounded; double min;
															bool max_Bounded; double max;};
struct Int_Independent			{int value; bool is_Independent=false;	int start; int step; int num_steps;};
struct Parameter				{double value; Independent independent; Coupled coupled; Fit fit; Optimize optimize;};
struct Stoichiometry			{Parameter composition; QString type;};
struct Interlayer_Enabled		{Parameter interlayer; bool enabled;};

extern QStringList transition_Layer_Functions;
extern QStringList drift_Models;


enum class Tril				{True, False, NotDefined};
enum       Angle_Type		{Grazing, Incidence};
enum class Item_Type		{Ambient, Layer, Substrate, Stack_Content};
enum class Variable_Type	{Independent, Coupled, Fitted, Optimized};

//-----------------------------------------------
// settings
//-----------------------------------------------

// gui_Settings
extern QString application_style;
/// launcher window geometry
extern int launcher_x_corner;
extern int launcher_y_corner;
/// multilayer window geometry
extern int multilayer_x_corner;
extern int multilayer_y_corner;
extern int multilayer_min_width;
extern int multilayer_min_height;
extern int multilayer_width;
extern int multilayer_height;
extern int multilayer_height_additive;
/// multilayer tab name
extern QString default_multilayer_tab_name;
extern QString default_independent_variable_tab_name;
/// resource path
extern QString icon_path;

// default_Values
/// Ambient_Values
extern QString	ambient_default_material;
extern double	ambient_default_density;
extern double	ambient_default_permittivity;
extern double	ambient_default_absorption;
extern bool		ambient_default_composed;
extern double	ambient_default_stoichiometry_composition;
extern QString	ambient_default_stoichiometry_element;
/// Layer_Values
extern QString	layer_default_material;
extern double	layer_default_density;
extern double	layer_default_permittivity;
extern double	layer_default_absorption;
extern bool		layer_default_composed;
extern double	layer_default_stoichiometry_composition;
extern QString	layer_default_stoichiometry_element;
extern double	layer_default_sigma;
extern double	layer_default_thickness;
extern QString	layer_default_drift_model;
extern double	layer_default_drift_coefficients;
/// Substrate_Values
extern QString	substrate_default_material;
extern double	substrate_default_density;
extern double	substrate_default_permittivity;
extern double	substrate_default_absorption;
extern bool		substrate_default_composed;
extern double	substrate_default_stoichiometry_composition;
extern QString	substrate_default_stoichiometry_element;
extern double	substrate_default_sigma;
/// Stack_Values
extern int		stack_default_number_of_repetition;
extern double	stack_default_period;
extern double	stack_default_gamma;

/// qlineedit precisions
extern char line_edit_double_format			;
extern int line_edit_angle_precision		;
extern int line_edit_wavelength_precision	;
extern int line_edit_density_precision		;
extern int line_edit_permittivity_precision	;
extern int line_edit_absorption_precision	;
extern int line_edit_composition_precision	;
extern int line_edit_thickness_precision	;
extern int line_edit_sigma_precision		;
extern int line_edit_interlayer_precision	;
extern int line_edit_period_precision		;
extern int line_edit_gamma_precision		;
/// thumbnail precisions
extern char thumbnail_double_format			;
extern int thumbnail_angle_precision		;
extern int thumbnail_wavelength_precision	;
extern int thumbnail_density_precision		;
extern int thumbnail_permittivity_precision	;
extern int thumbnail_absorption_precision	;
extern int thumbnail_composition_precision	;
extern int thumbnail_thickness_precision	;
extern int thumbnail_sigma_precision		;
extern int thumbnail_interlayer_precision	;
extern int thumbnail_period_precision		;
extern int thumbnail_gamma_precision		;
/// other precisions
extern int at_weight_precision				;

/// angular and spectral independent variables
extern int default_num_angular_points;
extern int default_num_spectral_points;
extern double default_min_angle;
extern double default_max_angle;
extern double default_min_wavelength;
extern double default_max_wavelength;
extern Angle_Type default_angle_type;
extern double default_angular_resolution;
extern double default_polarization;
extern double default_spectral_resolution;
extern double default_polarization_sensitivity;

//-----------------------------------------------

class Global_Definitions
{
public:
	static void set_Settings()
	{
		QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
		QSettings gui_Settings  (Gui_Settings_Path,   QSettings::IniFormat);

	// gui_Settings

	/// application style
		gui_Settings.beginGroup( Application_Style );
			application_style = gui_Settings.value( "application_style", "Windows" ).toString(); // Fusion Windows WindowsXP WindowsVista
		gui_Settings.endGroup();

	/// launcher window geometry
		gui_Settings.beginGroup( Launcher_Geometry );
			launcher_x_corner = gui_Settings.value( "launcher_x_corner", 300 ).toInt();
			launcher_y_corner = gui_Settings.value( "launcher_y_corner", 300 ).toInt();
		gui_Settings.endGroup();

	/// multilayer window geometry
		gui_Settings.beginGroup( Multilayer_Window_Geometry );
			multilayer_x_corner			= gui_Settings.value( "multilayer_x_corner",		500 ).toInt();
			multilayer_y_corner			= gui_Settings.value( "multilayer_y_corner",		200 ).toInt();
			multilayer_min_width		= gui_Settings.value( "multilayer_min_width",		300 ).toInt();
			multilayer_min_height		= gui_Settings.value( "multilayer_min_height",		100 ).toInt();
			multilayer_width			= gui_Settings.value( "multilayer_width",			531 ).toInt();
			multilayer_height			= gui_Settings.value( "multilayer_height",			500 ).toInt();
			multilayer_height_additive	= gui_Settings.value( "multilayer_height_additive",	23  ).toInt();
		gui_Settings.endGroup();

	/// multilayer tab name
		gui_Settings.beginGroup( Multilayer_Tabs );
			default_multilayer_tab_name			  = gui_Settings.value( "default_multilayer_tab_name",			 "Struct_" ).toString();
			default_independent_variable_tab_name = gui_Settings.value( "default_independent_variable_tab_name", "Plot_"   ).toString();
		gui_Settings.endGroup();

	/// resource path
		gui_Settings.beginGroup( Paths );
			icon_path = gui_Settings.value( "icon_path", "../../imd icons/" ).toString();
		gui_Settings.endGroup();

	// default_Settings

	/// structure init values
		default_Values.beginGroup( Structure_Init_Values );
			default_Values.beginGroup( Ambient_Values );
				ambient_default_material				  = default_Values.value( "ambient_default_material",					"H"		).toString();
				ambient_default_density					  = default_Values.value( "ambient_default_density",					0		).toDouble();
				ambient_default_permittivity			  = default_Values.value( "ambient_default_permittivity",				50		).toDouble();
				ambient_default_absorption				  = default_Values.value( "ambient_default_absorption",					50		).toDouble();
				ambient_default_composed				  = default_Values.value( "ambient_default_composed",					false	).toBool();
				ambient_default_stoichiometry_composition = default_Values.value( "ambient_default_stoichiometry_composition",	1		).toDouble();
				ambient_default_stoichiometry_element	  = default_Values.value( "ambient_default_stoichiometry_element",		"Al"	).toString();
			default_Values.endGroup();
			default_Values.beginGroup( Layer_Values );
				layer_default_material					= default_Values.value( "layer_default_material",					"Be"		).toString();
				layer_default_density					= default_Values.value( "layer_default_density",					1			).toDouble();
				layer_default_permittivity				= default_Values.value( "layer_default_permittivity",				50			).toDouble();
				layer_default_absorption				= default_Values.value( "layer_default_absorption",					50			).toDouble();
				layer_default_composed					= default_Values.value( "layer_default_composed",					false		).toBool();
				layer_default_stoichiometry_composition = default_Values.value( "layer_default_stoichiometry_composition",	1			).toDouble();
				layer_default_stoichiometry_element		= default_Values.value( "layer_default_stoichiometry_element",		"Al"		).toString();
				layer_default_sigma						= default_Values.value( "layer_default_sigma",						123.568		).toDouble();
				layer_default_thickness					= default_Values.value( "layer_default_thickness",					10			).toDouble();
				layer_default_drift_model				= default_Values.value( "layer_default_drift_model",				"no_drift"	).toString();
				layer_default_drift_coefficients		= default_Values.value( "layer_default_drift_coefficients",			0			).toDouble();
			default_Values.endGroup();
			default_Values.beginGroup( Substrate_Values );
				substrate_default_material					= default_Values.value( "substrate_default_material",				   "Si"	).toString();
				substrate_default_density					= default_Values.value( "substrate_default_density",				   2.33	).toDouble();
				substrate_default_permittivity				= default_Values.value( "substrate_default_permittivity",			   50	).toDouble();
				substrate_default_absorption				= default_Values.value( "substrate_default_absorption",				   50	).toDouble();
				substrate_default_composed					= default_Values.value( "substrate_default_composed",				   false).toBool();
				substrate_default_stoichiometry_composition = default_Values.value( "substrate_default_stoichiometry_composition", 1	).toDouble();
				substrate_default_stoichiometry_element		= default_Values.value( "substrate_default_stoichiometry_element",	   "Be"	).toString();
				substrate_default_sigma						= default_Values.value( "substrate_default_sigma",					   21.2	).toDouble();
			default_Values.endGroup();
			default_Values.beginGroup( Stack_Values );
				stack_default_number_of_repetition = default_Values.value( "stack_default_number_of_repetition",	1	).toInt();
				stack_default_period = default_Values.value( "stack_default_period",				20	).toDouble();
				stack_default_gamma = default_Values.value( "stack_default_gamma",				0.5 ).toDouble();
			default_Values.endGroup();
		default_Values.endGroup();

	/// precisions
		default_Values.beginGroup( Structure_Values_Representation );
	/// qlineedit precisions
			line_edit_double_format	    = qvariant_cast<char>(default_Values.value( "line_edit_double_format",'g'));
			line_edit_angle_precision		= default_Values.value( "line_edit_angle_precision",		1 ).toInt();
			line_edit_wavelength_precision	= default_Values.value( "line_edit_wavelength_precision",	1 ).toInt();
			line_edit_density_precision		= default_Values.value( "line_edit_density_precision",		1 ).toInt();
			line_edit_permittivity_precision= default_Values.value( "line_edit_permittivity_precision",	1 ).toInt();
			line_edit_absorption_precision	= default_Values.value( "line_edit_absorption_precision",	1 ).toInt();
			line_edit_composition_precision	= default_Values.value( "line_edit_composition_precision",	1 ).toInt();
			line_edit_thickness_precision	= default_Values.value( "line_edit_thickness_precision",	1 ).toInt();
			line_edit_sigma_precision		= default_Values.value( "line_edit_sigma_precision",		1 ).toInt();
			line_edit_interlayer_precision	= default_Values.value( "line_edit_interlayer_precision",	1 ).toInt();
			line_edit_period_precision		= default_Values.value( "line_edit_period_precision",		1 ).toInt();
			line_edit_gamma_precision		= default_Values.value( "line_edit_gamma_precision",		1 ).toInt();
	/// thumbnail precisions
			thumbnail_double_format	    = qvariant_cast<char>(default_Values.value( "thumbnail_double_format",'f'));
			thumbnail_angle_precision		= default_Values.value( "thumbnail_angle_precision",		1 ).toInt();
			thumbnail_wavelength_precision	= default_Values.value( "thumbnail_wavelength_precision",	1 ).toInt();
			thumbnail_density_precision		= default_Values.value( "thumbnail_density_precision",		1 ).toInt();
			thumbnail_permittivity_precision= default_Values.value( "thumbnail_permittivity_precision",	1 ).toInt();
			thumbnail_absorption_precision	= default_Values.value( "thumbnail_absorption_precision",	1 ).toInt();
			thumbnail_composition_precision = default_Values.value( "thumbnail_composition_precision",	1 ).toInt();
			thumbnail_thickness_precision	= default_Values.value( "thumbnail_thickness_precision",	1 ).toInt();
			thumbnail_sigma_precision		= default_Values.value( "thumbnail_sigma_precision",		1 ).toInt();
			thumbnail_interlayer_precision	= default_Values.value( "thumbnail_interlayer_precision",	1 ).toInt();
			thumbnail_period_precision		= default_Values.value( "thumbnail_period_precision",		1 ).toInt();
			thumbnail_gamma_precision		= default_Values.value( "thumbnail_gamma_precision",		1 ).toInt();
	/// other precisions
			at_weight_precision				= default_Values.value( "at_weight_precision",				1 ).toInt();
		default_Values.endGroup();

	/// parameters init values
		default_Values.beginGroup( Parameters_Init_Values );
			default_Values.beginGroup( Independent_Values );
	/// angular and spectral independent variables
				default_num_angular_points	= default_Values.value( "default_num_angular_points",		1 ).toInt();
				default_num_spectral_points = default_Values.value( "default_num_spectral_points",		1 ).toInt();
				default_min_angle			= default_Values.value( "default_min_angle",				0 ).toDouble();
				default_max_angle			= default_Values.value( "default_max_angle",				7 ).toDouble();
				default_min_wavelength		= default_Values.value( "default_min_wavelength",			1.54056 ).toDouble();
				default_max_wavelength		= default_Values.value( "default_max_wavelength",			180 ).toDouble();
				default_angle_type	= static_cast<Angle_Type>(default_Values.value( "default_angle_type", int(Angle_Type::Grazing) ).toInt());
				default_angular_resolution	= default_Values.value( "default_angular_resolution",		0 ).toDouble();
				default_polarization		= default_Values.value( "default_polarization",				0 ).toDouble();
				default_spectral_resolution	= default_Values.value( "default_spectral_resolution",		0 ).toDouble();
				default_polarization_sensitivity = default_Values.value( "default_polarization_sensitivity",0).toDouble();
			default_Values.endGroup();
		default_Values.endGroup();
	}
};

#endif // GLOBAL_DEFINITIONS_H
