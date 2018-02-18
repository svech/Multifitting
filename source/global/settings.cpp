#include "settings.h"

// -----------------------------------------------------------------------------------------

// gui

// application style
QString application_style;

// launcher window geometry
int launcher_x_corner;
int launcher_y_corner;

// multilayer window geometry
int multilayer_x_corner;
int multilayer_y_corner;
int multilayer_min_width;
int multilayer_min_height;
int multilayer_width;
int multilayer_height;
int multilayer_height_additive;

// tab names
QString default_multilayer_tab_name;
QString default_independent_variable_tab_name;

// resource path
QString icon_path;

// measurement default file
QString default_Measured_Filename;

// -----------------------------------------------------------------------------------------

// structure default values

double dispersion; // from [0,1)
double sigma_Dispersion_Min;	// 2
double sigma_Dispersion_Max;	// 2

// Ambient_Values
QString	ambient_default_material;
double	ambient_default_absolute_density;
double	ambient_default_relative_density;
double	ambient_default_permittivity;
double	ambient_default_absorption;
bool	ambient_default_composed;
double	ambient_default_stoichiometry_composition;
QString	ambient_default_stoichiometry_element;

// Layer_Values
QString	layer_default_material;
double	layer_default_absolute_density;
double	layer_default_relative_density;
double	layer_default_permittivity;
double	layer_default_absorption;
bool	layer_default_composed;
double	layer_default_stoichiometry_composition;
QString	layer_default_stoichiometry_element;
double	layer_default_sigma;
double	layer_default_thickness;
QString	layer_default_drift_model;
double	layer_default_drift_coefficients;

// Substrate_Values
QString	substrate_default_material;
double	substrate_default_absolute_density;
double	substrate_default_relative_density;
double	substrate_default_permittivity;
double	substrate_default_absorption;
bool	substrate_default_composed;
double	substrate_default_stoichiometry_composition;
QString	substrate_default_stoichiometry_element;
double	substrate_default_sigma;

// Stack_Values
int		stack_default_number_of_repetition;
double	stack_default_period;
double	stack_default_gamma;

// -----------------------------------------------------------------------------------------

// precisions

// lineedit
char line_edit_double_format		;
char line_edit_short_double_format	;
int line_edit_angle_precision		;
int line_edit_wavelength_precision	;
int line_edit_polarization_precision;
int line_edit_density_precision		;
int line_edit_permittivity_precision;
int line_edit_absorption_precision	;
int line_edit_composition_precision	;
int line_edit_thickness_precision	;
int line_edit_sigma_precision		;
int line_edit_interlayer_precision	;
int line_edit_drift_precision		;
int line_edit_period_precision		;
int line_edit_gamma_precision		;
int line_edit_sample_size_precision	;
int line_edit_beam_size_precision	;
int line_edit_background_precision	;

// thumbnail
char thumbnail_double_format		;
int thumbnail_angle_precision		;
int thumbnail_wavelength_precision	;
int thumbnail_polarization_precision;
int thumbnail_density_precision		;
int thumbnail_permittivity_precision;
int thumbnail_absorption_precision	;
int thumbnail_composition_precision	;
int thumbnail_thickness_precision	;
int thumbnail_sigma_precision		;
int thumbnail_interlayer_precision	;
int thumbnail_drift_precision		;
int thumbnail_period_precision		;
int thumbnail_gamma_precision		;
int thumbnail_sample_size_precision	;
int thumbnail_beam_size_precision	;
int thumbnail_background_precision	;

// other
int at_weight_precision				;

// -----------------------------------------------------------------------------------------

// parameters default values

// angular and spectral independent variables
int default_num_angular_points;
int default_num_spectral_points;
double default_angle_value;
double default_min_angle;
double default_max_angle;
double default_wavelength_value;
double default_min_wavelength;
double default_max_wavelength;
QString default_angle_type;
double default_angular_resolution;
double default_polarization;
double default_spectral_resolution;
double default_polarization_sensitivity;

// -----------------------------------------------------------------------------------------

// units
QString wavelength_units;
QString length_units;
QString angle_units;
QString density_units;
QString opt_const_units;
QString drift_units;

// -----------------------------------------------------------------------------------------

// calculations
int optical_Constants_Read_Threads;
int reflectivity_Calc_Threads;
int epsilon_Partial_Fill_Threads;

// -----------------------------------------------------------------------------------------

Settings::Settings()
{

}

void Settings::read_Gui_Settings(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings gui_Settings(Gui_Settings_Path + add_reset,   QSettings::IniFormat);

	if(reset_to_default) gui_Settings.setPath(gui_Settings.format(),gui_Settings.scope(),"");

	// application style
	gui_Settings.beginGroup( Application_Style );
		application_style = gui_Settings.value( "application_style", "WindowsXP" ).toString(); // Fusion Windows WindowsXP WindowsVista
	gui_Settings.endGroup();

	// launcher window geometry
	gui_Settings.beginGroup( Launcher_Geometry );
		launcher_x_corner = gui_Settings.value( "launcher_x_corner", 300 ).toInt();
		launcher_y_corner = gui_Settings.value( "launcher_y_corner", 300 ).toInt();
	gui_Settings.endGroup();

	// multilayer window geometry
	gui_Settings.beginGroup( Multilayer_Window_Geometry );
		multilayer_x_corner			= gui_Settings.value( "multilayer_x_corner",		250 ).toInt();
		multilayer_y_corner			= gui_Settings.value( "multilayer_y_corner",		0   ).toInt();
		multilayer_min_width		= gui_Settings.value( "multilayer_min_width",		300 ).toInt();
		multilayer_min_height		= gui_Settings.value( "multilayer_min_height",		100 ).toInt();
		multilayer_width			= gui_Settings.value( "multilayer_width",			531 ).toInt();
		multilayer_height			= gui_Settings.value( "multilayer_height",			470 ).toInt();
		multilayer_height_additive	= gui_Settings.value( "multilayer_height_additive",	23  ).toInt();
	gui_Settings.endGroup();

	// tab names
	gui_Settings.beginGroup( Multilayer_Tabs );
		default_multilayer_tab_name			  = gui_Settings.value( "default_multilayer_tab_name",			 "Struct_" ).toString();
		default_independent_variable_tab_name = gui_Settings.value( "default_independent_variable_tab_name", "Plot_"   ).toString();
	gui_Settings.endGroup();

	// resource path
	gui_Settings.beginGroup( Resource_Paths );
		icon_path = gui_Settings.value( "icon_path", Pre_Path + "icons/" ).toString();
	gui_Settings.endGroup();
}

void Settings::save_Gui_Settings()
{
	QSettings gui_Settings  (Gui_Settings_Path,   QSettings::IniFormat);

	// application style
	gui_Settings.beginGroup( Application_Style );
		gui_Settings.setValue( "application_style", application_style );
	gui_Settings.endGroup();

	// launcher window geometry
	gui_Settings.beginGroup( Launcher_Geometry );
		gui_Settings.setValue( "launcher_x_corner", launcher_x_corner );
		gui_Settings.setValue( "launcher_y_corner", launcher_y_corner );
	gui_Settings.endGroup();

	// multilayer window geometry
	gui_Settings.beginGroup( Multilayer_Window_Geometry );
		gui_Settings.setValue( "multilayer_x_corner",			multilayer_x_corner );
		gui_Settings.setValue( "multilayer_y_corner",			multilayer_y_corner );
		gui_Settings.setValue( "multilayer_min_width",			multilayer_min_width );
		gui_Settings.setValue( "multilayer_min_height",			multilayer_min_height );
		gui_Settings.setValue( "multilayer_width",				multilayer_width );
		gui_Settings.setValue( "multilayer_height",				multilayer_height );
		gui_Settings.setValue( "multilayer_height_additive",	multilayer_height_additive );
	gui_Settings.endGroup();

	// multilayer tab name
	gui_Settings.beginGroup( Multilayer_Tabs );
		gui_Settings.setValue( "default_multilayer_tab_name",			default_multilayer_tab_name );
		gui_Settings.setValue( "default_independent_variable_tab_name", default_independent_variable_tab_name   );
	gui_Settings.endGroup();

	// resource path
	gui_Settings.beginGroup( Resource_Paths );
		gui_Settings.setValue( "icon_path", icon_path );
	gui_Settings.endGroup();
}

void Settings::read_Structure_Default_Values(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings structure_Default_Values(Structure_Default_Values_Path + add_reset, QSettings::IniFormat);

	// structure default values
		structure_Default_Values.beginGroup( Structure_Init_Values );

		dispersion = structure_Default_Values.value( "dispersion",	0.5 ).toDouble();
		sigma_Dispersion_Min = structure_Default_Values.value( "sigma_Dispersion_Min",	0 ).toDouble();
		sigma_Dispersion_Max = structure_Default_Values.value( "sigma_Dispersion_Max",	2 ).toDouble();

			structure_Default_Values.beginGroup( Ambient_Values );
				ambient_default_material				  = structure_Default_Values.value( "ambient_default_material",				    "Al"	).toString();
				ambient_default_absolute_density		  = structure_Default_Values.value( "ambient_default_absolute_density",			0		).toDouble();
				ambient_default_relative_density		  = structure_Default_Values.value( "ambient_default_relative_density",			0		).toDouble();
				ambient_default_permittivity			  = structure_Default_Values.value( "ambient_default_permittivity",				100		).toDouble();
				ambient_default_absorption				  = structure_Default_Values.value( "ambient_default_absorption",				100		).toDouble();
				ambient_default_composed				  = structure_Default_Values.value( "ambient_default_composed",					false	).toBool();
				ambient_default_stoichiometry_composition = structure_Default_Values.value( "ambient_default_stoichiometry_composition",1		).toDouble();
				ambient_default_stoichiometry_element	  = structure_Default_Values.value( "ambient_default_stoichiometry_element",	"Al"	).toString();
			structure_Default_Values.endGroup();
			structure_Default_Values.beginGroup( Layer_Values );
				layer_default_material					= structure_Default_Values.value( "layer_default_material",					"Be"		).toString();
				layer_default_absolute_density			= structure_Default_Values.value( "layer_default_absolute_density",			2.699		).toDouble();
				layer_default_relative_density			= structure_Default_Values.value( "layer_default_relative_density",			1			).toDouble();
				layer_default_permittivity				= structure_Default_Values.value( "layer_default_permittivity",				100			).toDouble();
				layer_default_absorption				= structure_Default_Values.value( "layer_default_absorption",				100			).toDouble();
				layer_default_composed					= structure_Default_Values.value( "layer_default_composed",					false		).toBool();
				layer_default_stoichiometry_composition = structure_Default_Values.value( "layer_default_stoichiometry_composition",1			).toDouble();
				layer_default_stoichiometry_element		= structure_Default_Values.value( "layer_default_stoichiometry_element",	"Al"		).toString();
				layer_default_sigma						= structure_Default_Values.value( "layer_default_sigma",					0			).toDouble();
				layer_default_thickness					= structure_Default_Values.value( "layer_default_thickness",				10			).toDouble();
				layer_default_drift_model				= structure_Default_Values.value( "layer_default_drift_model",				"no_drift"	).toString();
				layer_default_drift_coefficients		= structure_Default_Values.value( "layer_default_drift_coefficients",		0			).toDouble();
			structure_Default_Values.endGroup();
			structure_Default_Values.beginGroup( Substrate_Values );
				substrate_default_material					= structure_Default_Values.value( "substrate_default_material",				   "Si"	).toString();
				substrate_default_absolute_density			= structure_Default_Values.value( "substrate_default_absolute_density",		   2.33	).toDouble();
				substrate_default_relative_density			= structure_Default_Values.value( "substrate_default_relative_density",		   1	).toDouble();
				substrate_default_permittivity				= structure_Default_Values.value( "substrate_default_permittivity",			   100	).toDouble();
				substrate_default_absorption				= structure_Default_Values.value( "substrate_default_absorption",			   100	).toDouble();
				substrate_default_composed					= structure_Default_Values.value( "substrate_default_composed",				   false).toBool();
				substrate_default_stoichiometry_composition = structure_Default_Values.value( "substrate_default_stoichiometry_composition",1	).toDouble();
				substrate_default_stoichiometry_element		= structure_Default_Values.value( "substrate_default_stoichiometry_element",   "Si" ).toString();
				substrate_default_sigma						= structure_Default_Values.value( "substrate_default_sigma",				   0	).toDouble();
			structure_Default_Values.endGroup();
			structure_Default_Values.beginGroup( Stack_Values );
				stack_default_number_of_repetition	= structure_Default_Values.value( "stack_default_number_of_repetition",	1	).toInt();
				stack_default_period				= structure_Default_Values.value( "stack_default_period",				20	).toDouble();
				stack_default_gamma					= structure_Default_Values.value( "stack_default_gamma",				0.5 ).toDouble();
			structure_Default_Values.endGroup();
		structure_Default_Values.endGroup();
}

void Settings::save_Structure_Default_Values()
{
	QSettings structure_Default_Values(Structure_Default_Values_Path, QSettings::IniFormat);

	// structure default values
	structure_Default_Values.beginGroup( Structure_Init_Values );

	structure_Default_Values.setValue( "dispersion", dispersion	);
	structure_Default_Values.setValue( "sigma_Dispersion_Min", sigma_Dispersion_Min	);
	structure_Default_Values.setValue( "sigma_Dispersion_Max", sigma_Dispersion_Max	);

		structure_Default_Values.beginGroup( Ambient_Values );
			structure_Default_Values.setValue( "ambient_default_material",					ambient_default_material					);
			structure_Default_Values.setValue( "ambient_default_absolute_density",			ambient_default_absolute_density			);
			structure_Default_Values.setValue( "ambient_default_relative_density",			ambient_default_relative_density			);
			structure_Default_Values.setValue( "ambient_default_permittivity",				ambient_default_permittivity				);
			structure_Default_Values.setValue( "ambient_default_absorption",				ambient_default_absorption					);
			structure_Default_Values.setValue( "ambient_default_composed",					ambient_default_composed					);
			structure_Default_Values.setValue( "ambient_default_stoichiometry_composition",	ambient_default_stoichiometry_composition	);
			structure_Default_Values.setValue( "ambient_default_stoichiometry_element",		ambient_default_stoichiometry_element		);
		structure_Default_Values.endGroup();
		structure_Default_Values.beginGroup( Layer_Values );
			structure_Default_Values.setValue( "layer_default_material",					layer_default_material					);
			structure_Default_Values.setValue( "layer_default_absolute_density",			layer_default_absolute_density			);
			structure_Default_Values.setValue( "layer_default_relative_density",			layer_default_relative_density			);
			structure_Default_Values.setValue( "layer_default_permittivity",				layer_default_permittivity				);
			structure_Default_Values.setValue( "layer_default_absorption",					layer_default_absorption				);
			structure_Default_Values.setValue( "layer_default_composed",					layer_default_composed					);
			structure_Default_Values.setValue( "layer_default_stoichiometry_composition",	layer_default_stoichiometry_composition	);
			structure_Default_Values.setValue( "layer_default_stoichiometry_element",		layer_default_stoichiometry_element		);
			structure_Default_Values.setValue( "layer_default_sigma",						layer_default_sigma						);
			structure_Default_Values.setValue( "layer_default_thickness",					layer_default_thickness					);
			structure_Default_Values.setValue( "layer_default_drift_model",					layer_default_drift_model				);
			structure_Default_Values.setValue( "layer_default_drift_coefficients",			layer_default_drift_coefficients		);
		structure_Default_Values.endGroup();
		structure_Default_Values.beginGroup( Substrate_Values );
			structure_Default_Values.setValue( "substrate_default_material",					substrate_default_material					);
			structure_Default_Values.setValue( "substrate_default_absolute_density",			substrate_default_absolute_density			);
			structure_Default_Values.setValue( "substrate_default_relative_density",			substrate_default_relative_density			);
			structure_Default_Values.setValue( "substrate_default_permittivity",				substrate_default_permittivity				);
			structure_Default_Values.setValue( "substrate_default_absorption",					substrate_default_absorption				);
			structure_Default_Values.setValue( "substrate_default_composed",					substrate_default_composed					);
			structure_Default_Values.setValue( "substrate_default_stoichiometry_composition",	substrate_default_stoichiometry_composition	);
			structure_Default_Values.setValue( "substrate_default_stoichiometry_element",		substrate_default_stoichiometry_element		);
			structure_Default_Values.setValue( "substrate_default_sigma",						substrate_default_sigma						);
		structure_Default_Values.endGroup();
		structure_Default_Values.beginGroup( Stack_Values );
			structure_Default_Values.setValue( "stack_default_number_of_repetition",	stack_default_number_of_repetition	);
			structure_Default_Values.setValue( "stack_default_period",					stack_default_period				);
			structure_Default_Values.setValue( "stack_default_gamma",					stack_default_gamma					);
		structure_Default_Values.endGroup();
	structure_Default_Values.endGroup();
}

void Settings::read_Precisions(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings precision_Values(Precisions_Path + add_reset, QSettings::IniFormat);

	// precisions
	precision_Values.beginGroup(Precisions);
		precision_Values.beginGroup( Line_Edit );
			line_edit_double_format		  = qvariant_cast<char>(precision_Values.value( "line_edit_double_format",      'f'));
			line_edit_short_double_format = qvariant_cast<char>(precision_Values.value( "line_edit_short_double_format",'g'));
			line_edit_angle_precision		= precision_Values.value( "line_edit_angle_precision",			5 ).toInt();
			line_edit_wavelength_precision	= precision_Values.value( "line_edit_wavelength_precision",		6 ).toInt();
			line_edit_polarization_precision= precision_Values.value( "line_edit_polarization_precision",	3 ).toInt();
			line_edit_background_precision	= precision_Values.value( "line_edit_background_precision",		3 ).toInt();
			line_edit_beam_size_precision	= precision_Values.value( "line_edit_beam_size_precision",		4 ).toInt();
			line_edit_sample_size_precision	= precision_Values.value( "line_edit_sample_size_precision",	2 ).toInt();
			line_edit_density_precision		= precision_Values.value( "line_edit_density_precision",		4 ).toInt();
			line_edit_permittivity_precision= precision_Values.value( "line_edit_permittivity_precision",	4 ).toInt();
			line_edit_absorption_precision	= precision_Values.value( "line_edit_absorption_precision",		4 ).toInt();
			line_edit_composition_precision	= precision_Values.value( "line_edit_composition_precision",	4 ).toInt();
			line_edit_thickness_precision	= precision_Values.value( "line_edit_thickness_precision",		4 ).toInt();
			line_edit_sigma_precision		= precision_Values.value( "line_edit_sigma_precision",			3 ).toInt();	// = thickness precision
			line_edit_interlayer_precision	= precision_Values.value( "line_edit_interlayer_precision",		3 ).toInt();
			line_edit_drift_precision		= precision_Values.value( "line_edit_drift_precision",			3 ).toInt();
			line_edit_period_precision		= precision_Values.value( "line_edit_period_precision",			4 ).toInt();	// = thickness precision
			line_edit_gamma_precision		= precision_Values.value( "line_edit_gamma_precision",			8 ).toInt();	// = thickness precision
		precision_Values.endGroup();
		precision_Values.beginGroup( Thumbnail );
			thumbnail_double_format	    = qvariant_cast<char>(precision_Values.value( "thumbnail_double_format",'f'));
			thumbnail_angle_precision		= precision_Values.value( "thumbnail_angle_precision",			3 ).toInt();
			thumbnail_wavelength_precision	= precision_Values.value( "thumbnail_wavelength_precision",		3 ).toInt();
			thumbnail_polarization_precision= precision_Values.value( "thumbnail_polarization_precision",	3 ).toInt();
			thumbnail_background_precision	= precision_Values.value( "thumbnail_background_precision",		3 ).toInt();
			thumbnail_beam_size_precision	= precision_Values.value( "thumbnail_beam_size_precision",		3 ).toInt();
			thumbnail_sample_size_precision	= precision_Values.value( "thumbnail_sample_size_precision",	2 ).toInt();
			thumbnail_density_precision		= precision_Values.value( "thumbnail_density_precision",		3 ).toInt();
			thumbnail_permittivity_precision= precision_Values.value( "thumbnail_permittivity_precision",	3 ).toInt();
			thumbnail_absorption_precision	= precision_Values.value( "thumbnail_absorption_precision",		3 ).toInt();
			thumbnail_composition_precision = precision_Values.value( "thumbnail_composition_precision",	3 ).toInt();
			thumbnail_thickness_precision	= precision_Values.value( "thumbnail_thickness_precision",		3 ).toInt();
			thumbnail_sigma_precision		= precision_Values.value( "thumbnail_sigma_precision",			3 ).toInt();	// = thickness precision
			thumbnail_interlayer_precision	= precision_Values.value( "thumbnail_interlayer_precision",		3 ).toInt();
			thumbnail_drift_precision		= precision_Values.value( "thumbnail_drift_precision",			4 ).toInt();
			thumbnail_period_precision		= precision_Values.value( "thumbnail_period_precision",			3 ).toInt();	// = thickness precision
			thumbnail_gamma_precision		= precision_Values.value( "thumbnail_gamma_precision",			3 ).toInt();	// = thickness precision
		precision_Values.endGroup();
		precision_Values.beginGroup( Other );
			at_weight_precision				= precision_Values.value( "at_weight_precision",				5 ).toInt();
		precision_Values.endGroup();
	precision_Values.endGroup();
}

void Settings::save_Precisions()
{
	QSettings precision_Values(Precisions_Path, QSettings::IniFormat);

	// precisions
	precision_Values.beginGroup(Precisions);
		precision_Values.beginGroup( Line_Edit );
			precision_Values.setValue( "line_edit_double_format",			line_edit_double_format				);
			precision_Values.setValue( "line_edit_short_double_format",		line_edit_short_double_format		);
			precision_Values.setValue( "line_edit_angle_precision",			line_edit_angle_precision			);
			precision_Values.setValue( "line_edit_wavelength_precision",	line_edit_wavelength_precision		);
			precision_Values.setValue( "line_edit_density_precision",		line_edit_density_precision			);
			precision_Values.setValue( "line_edit_permittivity_precision",	line_edit_permittivity_precision	);
			precision_Values.setValue( "line_edit_absorption_precision",	line_edit_absorption_precision		);
			precision_Values.setValue( "line_edit_composition_precision",	line_edit_composition_precision		);
			precision_Values.setValue( "line_edit_thickness_precision",		line_edit_thickness_precision		);
			precision_Values.setValue( "line_edit_sigma_precision",			line_edit_sigma_precision			);
			precision_Values.setValue( "line_edit_interlayer_precision",	line_edit_interlayer_precision		);
			precision_Values.setValue( "line_edit_drift_precision",			line_edit_drift_precision			);
			precision_Values.setValue( "line_edit_period_precision",		line_edit_period_precision			);
			precision_Values.setValue( "line_edit_gamma_precision",			line_edit_gamma_precision			);
		precision_Values.endGroup();
		precision_Values.beginGroup( Thumbnail );
			precision_Values.setValue( "thumbnail_double_format",			thumbnail_double_format				);
			precision_Values.setValue( "thumbnail_angle_precision",			thumbnail_angle_precision			);
			precision_Values.setValue( "thumbnail_wavelength_precision",	thumbnail_wavelength_precision		);
			precision_Values.setValue( "thumbnail_density_precision",		thumbnail_density_precision			);
			precision_Values.setValue( "thumbnail_permittivity_precision",	thumbnail_permittivity_precision	);
			precision_Values.setValue( "thumbnail_absorption_precision",	thumbnail_absorption_precision		);
			precision_Values.setValue( "thumbnail_composition_precision",	thumbnail_composition_precision		);
			precision_Values.setValue( "thumbnail_thickness_precision",		thumbnail_thickness_precision		);
			precision_Values.setValue( "thumbnail_sigma_precision",			thumbnail_sigma_precision			);
			precision_Values.setValue( "thumbnail_interlayer_precision",	thumbnail_interlayer_precision		);
			precision_Values.setValue( "thumbnail_drift_precision",			thumbnail_drift_precision			);
			precision_Values.setValue( "thumbnail_period_precision",		thumbnail_period_precision			);
			precision_Values.setValue( "thumbnail_gamma_precision",			thumbnail_gamma_precision			);
		precision_Values.endGroup();
		precision_Values.beginGroup( Other );
			precision_Values.setValue( "at_weight_precision",				at_weight_precision );
		precision_Values.endGroup();
	precision_Values.endGroup();
}

void Settings::read_Parameters_Default_Values(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings parameters_Default_Values(Parameters_Default_Values_Path + add_reset, QSettings::IniFormat);

	// parameters default values
	parameters_Default_Values.beginGroup( Independent_Values );
		default_num_angular_points		 = parameters_Default_Values.value( "default_num_angular_points",		1		).toInt();
		default_num_spectral_points		 = parameters_Default_Values.value( "default_num_spectral_points",		1		).toInt();
		default_angle_value				 = parameters_Default_Values.value( "default_angle_value",				88		).toDouble();
		default_min_angle				 = parameters_Default_Values.value( "default_min_angle",				1		).toDouble();
		default_max_angle				 = parameters_Default_Values.value( "default_max_angle",				7		).toDouble();
		default_wavelength_value		 = parameters_Default_Values.value( "default_wavelength_value",			1.54056 ).toDouble();
		default_min_wavelength			 = parameters_Default_Values.value( "default_min_wavelength",			100		).toDouble();
		default_max_wavelength			 = parameters_Default_Values.value( "default_max_wavelength",			180		).toDouble();
		default_angle_type				 = parameters_Default_Values.value( "default_angle_type",				angle_Type_Grazing).toString();
		default_angular_resolution		 = parameters_Default_Values.value( "default_angular_resolution",		0.009	).toDouble();
		default_polarization			 = parameters_Default_Values.value( "default_polarization",				0		).toDouble();
		default_spectral_resolution		 = parameters_Default_Values.value( "default_spectral_resolution",		0.001	).toDouble();
		default_polarization_sensitivity = parameters_Default_Values.value( "default_polarization_sensitivity",	1		).toDouble();
	parameters_Default_Values.endGroup();
}

void Settings::save_Parameters_Default_Values()
{
	QSettings parameters_Default_Values(Parameters_Default_Values_Path, QSettings::IniFormat);

	// parameters default values
	parameters_Default_Values.beginGroup( Independent_Values );
		parameters_Default_Values.setValue( "default_num_angular_points",		default_num_angular_points		);
		parameters_Default_Values.setValue( "default_num_spectral_points",		default_num_spectral_points		);
		parameters_Default_Values.setValue( "default_angle_value",				default_angle_value				);
		parameters_Default_Values.setValue( "default_min_angle",				default_min_angle				);
		parameters_Default_Values.setValue( "default_max_angle",				default_max_angle				);
		parameters_Default_Values.setValue( "default_wavelength_value",			default_wavelength_value		);
		parameters_Default_Values.setValue( "default_min_wavelength",			default_min_wavelength			);
		parameters_Default_Values.setValue( "default_max_wavelength",			default_max_wavelength			);
		parameters_Default_Values.setValue( "default_angle_type",				default_angle_type				);
		parameters_Default_Values.setValue( "default_angular_resolution",		default_angular_resolution		);
		parameters_Default_Values.setValue( "default_polarization",				default_polarization			);
		parameters_Default_Values.setValue( "default_spectral_resolution",		default_spectral_resolution		);
		parameters_Default_Values.setValue( "default_polarization_sensitivity",	default_polarization_sensitivity);
	parameters_Default_Values.endGroup();
}

void Settings::read_Units(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings units(Units_Path + add_reset, QSettings::IniFormat);

	// units
	units.beginGroup( Units );
		wavelength_units		= units.value( "wavelength_units",	wavelength_Units_List.first() ).toString();
		length_units			= units.value( "length_units",		length_Units_List    .first() ).toString();
		angle_units				= units.value( "angle_units",		angle_Units_List	 .first() ).toString();
		density_units			= units.value( "density_units",		density_Units_List   .first() ).toString();
		opt_const_units			= units.value( "opt_const_units",	opt_Const_Units_List .first() ).toString();
		drift_units				= units.value( "drift_units",		drift_Units_List     .first() ).toString();
	units.endGroup();
}

void Settings::save_Units()
{
	QSettings units(Units_Path, QSettings::IniFormat);

	// units
	units.beginGroup( Units );
		units.setValue( "wavelength_units",	wavelength_units);
		units.setValue( "length_units",		length_units	);
		units.setValue( "angle_units",		angle_units		);
		units.setValue( "density_units",	density_units   );
		units.setValue( "opt_const_units",	opt_const_units	);
		units.setValue( "drift_units",		drift_units		);
	units.endGroup();
}

void Settings::read_Calculations(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings calculations(Calculations_Path + add_reset, QSettings::IniFormat);

	// calculations
	calculations.beginGroup( Threads );
		optical_Constants_Read_Threads = calculations.value( "optical_Constants_Read_Threads",	4 ).toInt();
		reflectivity_Calc_Threads	   = calculations.value( "reflectivity_Calc_Threads",		4 ).toInt();
		epsilon_Partial_Fill_Threads   = calculations.value( "epsilon_Partial_Fill_Threads",	4 ).toInt();
	calculations.endGroup();
}

void Settings::save_Calculations()
{
	QSettings calculations(Calculations_Path, QSettings::IniFormat);

	// calculations
	calculations.beginGroup( Threads );
		calculations.setValue( "optical_Constants_Read_Threads", optical_Constants_Read_Threads );
		calculations.setValue( "reflectivity_Calc_Threads",		 reflectivity_Calc_Threads		);
		calculations.setValue( "epsilon_Partial_Fill_Threads",	 epsilon_Partial_Fill_Threads	);
	calculations.endGroup();
}

void Settings::read_Measurements(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings measurements(Measurements_Path + add_reset, QSettings::IniFormat);

	// measurements
	measurements.beginGroup( Threads );
		default_Measured_Filename = measurements.value( "default_Measured_Filename",	"measured.txt" ).toString();
	measurements.endGroup();
}

void Settings::save_Measurements()
{
	QSettings measurements(Measurements_Path, QSettings::IniFormat);

	// measurements
	measurements.beginGroup( Filenames );
		measurements.setValue( "default_Measured_Filename", default_Measured_Filename );
	measurements.endGroup();
}

// -----------------------------------------------------------------------------------------

void Settings::read_All_Settings(bool reset_to_default)
{
	read_Gui_Settings(reset_to_default);
	read_Structure_Default_Values(reset_to_default);
	read_Precisions(reset_to_default);
	read_Parameters_Default_Values(reset_to_default);
	read_Units(reset_to_default);
	read_Calculations(reset_to_default);
	read_Measurements(reset_to_default);
}

void Settings::save_All_Settings()
{
	save_Gui_Settings();
	save_Structure_Default_Values();
	save_Precisions();
	save_Parameters_Default_Values();
	save_Units();
	save_Calculations();
	save_Measurements();
}
