#ifndef SETTINGS_H
#define SETTINGS_H

#include "global_definitions.h"
#include "global_variables.h"

// -----------------------------------------------------------------------------------------

// gui

// application style
extern QString application_style;

// launcher window geometry
extern int launcher_x_corner;
extern int launcher_y_corner;

// multilayer window geometry
extern int multilayer_x_corner;
extern int multilayer_y_corner;
extern int multilayer_min_width;
extern int multilayer_min_height;
extern int multilayer_width;
extern int multilayer_height;
extern int multilayer_height_additive;

// tab names
extern QString default_multilayer_tab_name;
extern QString default_independent_variable_tab_name;

// resource path
extern QString icon_path;

// -----------------------------------------------------------------------------------------

// structure default values

// Ambient_Values
extern QString	ambient_default_material;
extern double	ambient_default_density;
extern double	ambient_default_permittivity;
extern double	ambient_default_absorption;
extern bool		ambient_default_composed;
extern double	ambient_default_stoichiometry_composition;
extern QString	ambient_default_stoichiometry_element;

// Layer_Values
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

// Substrate_Values
extern QString	substrate_default_material;
extern double	substrate_default_density;
extern double	substrate_default_permittivity;
extern double	substrate_default_absorption;
extern bool		substrate_default_composed;
extern double	substrate_default_stoichiometry_composition;
extern QString	substrate_default_stoichiometry_element;
extern double	substrate_default_sigma;

// Stack_Values
extern int		stack_default_number_of_repetition;
extern double	stack_default_period;
extern double	stack_default_gamma;

// -----------------------------------------------------------------------------------------

// precisions

// lineedit
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

// thumbnail
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

// other
extern int at_weight_precision				;

// -----------------------------------------------------------------------------------------

// parameters default values

// angular and spectral independent variables
extern int default_num_angular_points;
extern int default_num_spectral_points;
extern double default_angle_value;
extern double default_min_angle;
extern double default_max_angle;
extern double default_wavelength_value;
extern double default_min_wavelength;
extern double default_max_wavelength;
extern QString default_angle_type;
extern double default_angular_resolution;
extern double default_polarization;
extern double default_spectral_resolution;
extern double default_polarization_sensitivity;

// -----------------------------------------------------------------------------------------

// units
extern QString wavelength_units;
extern QString length_units;
extern QString angle_units;
extern QString density_units;
extern QString opt_const_units;

// -----------------------------------------------------------------------------------------

class Settings
{
public:
	Settings();

	// gui
	static void read_Gui_Settings(bool reset_to_default = false);
	static void save_Gui_Settings();

	// structure default values
	static void read_Structure_Default_Values(bool reset_to_default = false);
	static void save_Structure_Default_Values();

	// precisions
	static void read_Precisions(bool reset_to_default = false);
	static void save_Precisions();

	// parameters default values
	static void read_Parameters_Default_Values(bool reset_to_default = false);
	static void save_Parameters_Default_Values();

	// units
	static void read_Units(bool reset_to_default = false);
	static void save_Units();

	// -----------------------------------------------------------------------------------------

	// all
	static void read_All_Settings(bool reset_to_default = false);
	static void save_All_Settings();

};

#endif // SETTINGS_H