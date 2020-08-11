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

// independent curve window geometry
extern int independent_x_corner;
extern int independent_y_corner;

// target curve window geometry
extern int target_x_corner;
extern int target_y_corner;

// structure table window geometry
extern int structure_table_x_corner;
extern int structure_table_y_corner;
extern int structure_table_width;
extern int structure_table_height;

// roughness table window geometry
extern int roughness_table_x_corner;
extern int roughness_table_y_corner;
extern int roughness_table_width;
extern int roughness_table_height;

// regular aperiodic table window geometry
extern int regular_aperiodic_table_x_corner;
extern int regular_aperiodic_table_y_corner;
extern int regular_aperiodic_table_width;
extern int regular_aperiodic_table_height;

// graphs 1D window geometry
extern int graphs_x_corner_1D;
extern int graphs_y_corner_1D;
extern int graphs_width_1D;
extern int graphs_height_1D;

// graphs 2D window geometry
extern int graphs_x_corner_2D;
extern int graphs_y_corner_2D;
extern int graphs_width_2D;
extern int graphs_height_2D;

// profile plots window geometry
extern int profile_plots_x_corner;
extern int profile_plots_y_corner;
extern int profile_plots_width;
extern int profile_plots_height;

// fitting settings window geometry
extern int fitting_settings_x_corner;
extern int fitting_settings_y_corner;

// calculation settings window geometry
extern int calculation_settings_x_corner;
extern int calculation_settings_y_corner;
extern int calculation_settings_width;
extern int calculation_settings_height;

// general settings window geometry
extern int general_settings_x_corner;
extern int general_settings_y_corner;
extern int general_settings_width;
extern int general_settings_height;

// fits selector window geometry
extern int fits_selector_settings_x_corner;
extern int fits_selector_settings_y_corner;
extern int fits_selector_settings_width;
extern int fits_selector_settings_height;

// tab names
extern QString default_multilayer_tab_name;
extern QString default_independent_variable_tab_name;

// paths and names
extern bool use_working_directory;
extern bool use_multifitting_directory;
extern bool use_last_directory;
//extern QString icon_path;
extern QString working_directory;
extern QString last_directory;
extern QString last_data_directory;
extern QString last_file;

// measurement default file
extern QString default_Measured_Filename;

// aperiodic import default parameters
extern bool aperiodic_default_sigma_import;
extern bool aperiodic_default_density_import;
extern QString aperiodic_default_units_import;

// -----------------------------------------------------------------------------------------

// structure default values

extern double dispersion;			// from [0,1)
extern double sigma_Dispersion_Min;	// 2
extern double sigma_Dispersion_Max;	// 2
extern double default_num_confidence_points;
extern bool auto_density_from_elements;

// Ambient_Values
extern QString	ambient_default_material;
extern double	ambient_default_absolute_density;
extern double	ambient_default_relative_density;
extern double	ambient_default_permittivity;
extern double	ambient_default_absorption;
extern bool		ambient_default_composed;
extern double	ambient_default_stoichiometry_composition;
extern QString	ambient_default_stoichiometry_element;

// Layer_Values
extern QString	layer_default_material;
extern double	layer_default_absolute_density;
extern double	layer_default_relative_density;
extern double	layer_default_permittivity;
extern double	layer_default_absorption;
extern bool		layer_default_composed;
extern double	layer_default_stoichiometry_composition;
extern QString	layer_default_stoichiometry_element;
extern double	layer_default_sigma_diffuse;
extern double	layer_default_sigma_roughness;
extern double	layer_default_thickness;
extern QString	layer_default_drift_model;
extern double	layer_default_drift_coefficients;

// Substrate_Values
extern QString	substrate_default_material;
extern double	substrate_default_absolute_density;
extern double	substrate_default_relative_density;
extern double	substrate_default_permittivity;
extern double	substrate_default_absorption;
extern bool		substrate_default_composed;
extern double	substrate_default_stoichiometry_composition;
extern QString	substrate_default_stoichiometry_element;
extern double	substrate_default_sigma_diffuse;
extern double	substrate_default_sigma_roughness;

// Stack_Values
extern int		stack_default_number_of_repetition;
extern double	stack_default_period;
extern double	stack_default_gamma;

// steps
extern double	step_thickness_transfer;
extern double	step_composition;
extern double	step_density;
extern double	step_thickness;
extern double	step_sigma_diffuse;

extern double	step_sigma_roughness;
extern double	step_sigma_cor_radius;
extern double	step_sigma_fractal_alpha;
extern double	step_sigma_vertical_cor_length;
extern double	step_sigma_omega;
extern double	step_sigma_mu;

extern double	step_interlayer;
extern double	step_gamma;
extern double	step_drift;
extern double	step_thickness_aperiodic;
extern double	step_sigma_aperiodic;

// -----------------------------------------------------------------------------------------

// precisions

// lineedit
extern char line_edit_double_format			;
extern char line_edit_short_double_format	;
extern int line_edit_angle_precision		;
extern int line_edit_wavelength_precision	;
extern int line_edit_spectral_resolution_precision	;
extern int line_edit_polarization_precision	;
extern int line_edit_density_precision		;
extern int line_edit_permittivity_precision	;
extern int line_edit_absorption_precision	;
extern int line_edit_composition_precision	;
extern int line_edit_thickness_precision	;

extern int line_edit_sigma_precision				;
extern int line_edit_cor_radius_precision			;
extern int line_edit_fractal_alpha_precision		;
extern int line_edit_omega_precision				;
extern int line_edit_mu_precision					;

extern int line_edit_interlayer_precision	;
extern int line_edit_drift_precision		;
extern int line_edit_period_precision		;
extern int line_edit_gamma_precision		;
extern int line_edit_sample_size_precision	;
extern int line_edit_beam_size_precision	;
extern int line_edit_background_precision	;

// thumbnail
extern char thumbnail_double_format			;
extern int thumbnail_angle_precision		;
extern int thumbnail_wavelength_precision	;
extern int thumbnail_spectral_resolution_precision	;
extern int thumbnail_polarization_precision	;
extern int thumbnail_density_precision		;
extern int thumbnail_permittivity_precision	;
extern int thumbnail_absorption_precision	;
extern int thumbnail_composition_precision	;
extern int thumbnail_thickness_precision	;

extern int thumbnail_sigma_precision				;
extern int thumbnail_cor_radius_precision			;
extern int thumbnail_fractal_alpha_precision		;
extern int thumbnail_omega_precision				;
extern int thumbnail_mu_precision					;

extern int thumbnail_interlayer_precision	;
extern int thumbnail_drift_precision		;
extern int thumbnail_period_precision		;
extern int thumbnail_gamma_precision		;
extern int thumbnail_sample_size_precision	;
extern int thumbnail_beam_size_precision	;
extern int thumbnail_background_precision	;

// other
extern int at_weight_precision				;
extern int thickness_transfer_precision		;

// -----------------------------------------------------------------------------------------

// parameters default values

// angular and spectral independent variables
extern int default_num_spectral_points;
extern int default_num_theta_0_angular_points;
extern int default_num_theta_angular_points;
extern int default_num_phi_angular_points;

extern double default_theta_0_angle_value;
extern double default_min_theta_0_angle;
extern double default_max_theta_0_angle;

extern double default_theta_angle_value;
extern double default_min_theta_angle;
extern double default_max_theta_angle;

extern double default_phi_angle_value;
extern double default_min_phi_angle;
extern double default_max_phi_angle;

extern double default_wavelength_value;
extern double default_min_wavelength;
extern double default_max_wavelength;

extern double default_logarithmic_threshold_beta;
extern double default_logarithmic_threshold_material_density;
extern double default_logarithmic_threshold_element_concentration;

// -----------------------------------------------------------------------------------------


// units
extern QString length_units;
extern QString density_units;
extern QString opt_const_units;


// -----------------------------------------------------------------------------------------

// general settings
extern int general_Settings_Tab_Index;
extern int optical_Constants_Read_Threads;
extern int reflectivity_Calc_Threads;
extern int epsilon_Partial_Fill_Threads;
extern int n_Max_Series;
extern bool use_Simplified_Scattering_Weak_Factor;

extern bool recalculate_Spinbox_Global;
extern bool recalculate_Spinbox_Table;
extern bool mouse_Wheel_Spinbox_Table;
extern bool refill_Dependent_Table;
extern bool aperiodic_Recalculate_Spinbox_Table;
extern bool aperiodic_Mouse_Wheel_Spinbox_Table;
extern bool replot_Graphs_During_Fitting_1D;
extern bool replot_Graphs_During_Fitting_2D;
extern bool print_1D_Data_On_Recalculation;
extern bool print_2D_Data_On_Recalculation;

// -----------------------------------------------------------------------------------------

extern bool tab_synchronization;
extern bool calculation_time;
extern bool show_residuals;

// -----------------------------------------------------------------------------------------

class Settings
{
public:
	Settings();

	// paths
	static void read_Paths(bool reset_to_default = false);
	static void save_Paths();

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

	// calculation params
	static void read_General_Settings(bool reset_to_default = false);
	static void save_General_Settings();

	// measurements
	static void read_Measurements(bool reset_to_default = false);
	static void save_Measurements();

	// -----------------------------------------------------------------------------------------

	// all
	static void read_All_Settings(bool reset_to_default = false);
	static void save_All_Settings();

};

#endif // SETTINGS_H
