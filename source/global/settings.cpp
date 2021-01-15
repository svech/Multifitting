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

// independent curve window geometry
int independent_x_corner;
int independent_y_corner;

// target curve window geometry
int target_x_corner;
int target_y_corner;

// structure table window geometry
int structure_table_x_corner;
int structure_table_y_corner;
int structure_table_width;
int structure_table_height;

// roughness table window geometry
int roughness_table_x_corner;
int roughness_table_y_corner;
int roughness_table_width;
int roughness_table_height;

// regular aperiodic table window geometry
int regular_aperiodic_table_x_corner;
int regular_aperiodic_table_y_corner;
int regular_aperiodic_table_width;
int regular_aperiodic_table_height;

// graphs 1D window geometry
int graphs_x_corner_1D;
int graphs_y_corner_1D;
int graphs_width_1D;
int graphs_height_1D;

// graphs 2D window geometry
int graphs_x_corner_2D;
int graphs_y_corner_2D;
int graphs_width_2D;
int graphs_height_2D;

// profile plots window geometry
int profile_plots_x_corner;
int profile_plots_y_corner;
int profile_plots_width;
int profile_plots_height;

// fitting settings window geometry
int fitting_settings_x_corner;
int fitting_settings_y_corner;

// calculation settings window geometry
int calculation_settings_x_corner;
int calculation_settings_y_corner;
int calculation_settings_width;
int calculation_settings_height;

// general settings window geometry
int general_settings_x_corner;
int general_settings_y_corner;
int general_settings_width;
int general_settings_height;

// fits selector window geometry
int fits_selector_settings_x_corner;
int fits_selector_settings_y_corner;
int fits_selector_settings_width;
int fits_selector_settings_height;

// tab names
QString default_multilayer_tab_name;
QString default_independent_variable_tab_name;

// paths and names
bool use_working_directory;
bool use_multifitting_directory;
bool use_last_directory;
//QString icon_path;
QString working_directory;
QString preliminary_last_directory;
QString preliminary_last_data_directory;
QString preliminary_last_file;
QString last_directory;
QString last_data_directory;
QString last_file;

// measurement default file
QString default_Measured_Filename;

// aperiodic import default parameters
bool aperiodic_default_sigma_import;
bool aperiodic_default_density_import;
QString aperiodic_default_units_import;

// -----------------------------------------------------------------------------------------

// structure default values

double dispersion; // from [0,1)
double sigma_Dispersion_Min;	// 2
double sigma_Dispersion_Max;	// 2
double default_num_confidence_points;
bool auto_density_from_elements;

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
double	layer_default_sigma_diffuse;
double	layer_default_sigma_roughness;
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
double	substrate_default_sigma_diffuse;
double	substrate_default_sigma_roughness;

// Stack_Values
int		stack_default_number_of_repetition;
double	stack_default_period;
double	stack_default_gamma;

// steps
double	step_thickness_transfer;
double	step_composition;
double	step_density;
double	step_thickness;
double	step_sigma_diffuse;

double	step_sigma_roughness;
double	step_sigma_cor_radius;
double	step_sigma_fractal_alpha;
double	step_sigma_vertical_cor_length;
double	step_sigma_omega;
double	step_sigma_mu;

double	step_particle_density;
double	step_particle_radius;
double	step_particle_height;
double	step_particle_distance;
double	step_particle_distance_deviation;
double	step_domain_size;
double	step_particle_z_position;
double	step_particle_z_position_deviation;

double	step_interlayer;
double	step_gamma;
double	step_drift;
double	step_thickness_aperiodic;
double	step_sigma_aperiodic;

// -----------------------------------------------------------------------------------------

// precisions

// lineedit
char line_edit_double_format		;
char line_edit_short_double_format	;
int line_edit_angle_precision		;
int line_edit_wavelength_precision	;
int line_edit_spectral_resolution_precision	;
int line_edit_polarization_precision;
int line_edit_density_precision		;
int line_edit_permittivity_precision;
int line_edit_absorption_precision	;
int line_edit_composition_precision	;
int line_edit_thickness_precision	;

int line_edit_sigma_precision				;
int line_edit_cor_radius_precision			;
int line_edit_fractal_alpha_precision		;
int line_edit_omega_precision				;
int line_edit_mu_precision					;

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
int thumbnail_spectral_resolution_precision	;
int thumbnail_polarization_precision;
int thumbnail_density_precision		;
int thumbnail_permittivity_precision;
int thumbnail_absorption_precision	;
int thumbnail_composition_precision	;
int thumbnail_thickness_precision	;

int thumbnail_sigma_precision				;
int thumbnail_cor_radius_precision			;
int thumbnail_fractal_alpha_precision		;
int thumbnail_omega_precision				;
int thumbnail_mu_precision					;

int thumbnail_interlayer_precision	;
int thumbnail_drift_precision		;
int thumbnail_period_precision		;
int thumbnail_gamma_precision		;
int thumbnail_sample_size_precision	;
int thumbnail_beam_size_precision	;
int thumbnail_background_precision	;

// other
int at_weight_precision				;
int thickness_transfer_precision	;

// -----------------------------------------------------------------------------------------

// measurement default values

int default_num_spectral_points;
int default_num_theta_0_angular_points;
int default_num_theta_angular_points;
int default_num_phi_angular_points;

int default_beam_theta_0_specular_position;
int default_detector_theta_offset;

double default_theta_0_angle_value;
double default_min_theta_0_angle;
double default_max_theta_0_angle;

double default_theta_angle_value;
double default_min_theta_angle;
double default_max_theta_angle;

double default_phi_angle_value;
double default_min_phi_angle;
double default_max_phi_angle;

double default_wavelength_value;
double default_min_wavelength;
double default_max_wavelength;

double default_logarithmic_threshold_beta;
double default_logarithmic_threshold_material_density;
double default_logarithmic_threshold_element_concentration;

double default_spectral_distribution_FWHM;
double default_spectral_distribution_coverage;
bool   default_spectral_distribution_use_sampling;
int    default_spectral_distribution_number_of_Samples;

double default_beam_Theta_0_distribution_FWHM;
double default_beam_Theta_0_distribution_coverage;
bool   default_beam_Theta_0_distribution_use_sampling;
int    default_beam_Theta_0_distribution_number_of_Samples;

double default_beam_Phi_0_distribution_FWHM;
double default_beam_Phi_0_distribution_coverage;
bool   default_beam_Phi_0_distribution_use_sampling;
int    default_beam_Phi_0_distribution_number_of_Samples;

double default_detector_1D_slit_width;
double default_detector_1D_distance_to_sample;

double default_detector_1D_theta_resolution_FWHM;
double default_detector_2D_theta_resolution_FWHM;
double default_detector_2D_phi_resolution_FWHM;

double default_beam_geometry_size;
double default_beam_geometry_smoothing;
double default_beam_wings_full_width;
double default_beam_wings_intensity;
double default_beam_lateral_width;
bool   default_beam_log_profile_plot;

double default_sample_geometry_size;
double default_sample_geometry_x_position;
double default_sample_geometry_z_position;
double default_sample_geometry_curvature;

double default_polarization;
double default_background;
// -----------------------------------------------------------------------------------------

// units
QString length_units;
QString density_units;
QString opt_const_units;

// -----------------------------------------------------------------------------------------

// general settings
int general_Settings_Tab_Index;
int optical_Constants_Read_Threads;
int reflectivity_Calc_Threads;
int epsilon_Partial_Fill_Threads;
int n_Max_Series;
bool use_Simplified_Scattering_Weak_Factor;

bool recalculate_Spinbox_Global;
bool recalculate_Spinbox_Table;
bool mouse_Wheel_Spinbox_Table;
bool refill_Dependent_Table;
bool aperiodic_Recalculate_Spinbox_Table;
bool aperiodic_Mouse_Wheel_Spinbox_Table;
bool replot_Graphs_During_Fitting_1D;
bool replot_Graphs_During_Fitting_2D;
bool print_1D_Data_On_Recalculation;
bool print_2D_Data_On_Recalculation;

// -----------------------------------------------------------------------------------------

bool tab_synchronization;
bool calculation_time;
bool show_residuals;

// -----------------------------------------------------------------------------------------

Settings::Settings()
{

}

void Settings::read_Paths(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings paths_Settings(Paths_Settings_Path + add_reset, QSettings::IniFormat);

	if(reset_to_default) paths_Settings.setPath(paths_Settings.format(),paths_Settings.scope(),"");

	// save/open path
	paths_Settings.beginGroup( Last_Paths );
		use_multifitting_directory	= paths_Settings.value( "use_multifitting_directory",	true ).toBool();
		use_working_directory		= paths_Settings.value( "use_working_directory",		false ).toBool();
		use_last_directory			= paths_Settings.value( "use_last_directory",			false ).toBool();
		working_directory	= paths_Settings.value( "working_directory",	QDir::currentPath() ).toString();
		last_directory		= paths_Settings.value( "last_directory",		QDir::currentPath() ).toString();
		last_data_directory = paths_Settings.value( "last_data_directory",	QDir::currentPath() ).toString();
		last_file			= paths_Settings.value( "last_file",			default_File		).toString();
	paths_Settings.endGroup();

	// resource path
//	paths_Settings.beginGroup( Resource_Paths );
//		icon_path = paths_Settings.value( "icon_path", Pre_Path + "icons/" ).toString();
//	paths_Settings.endGroup();
}

void Settings::save_Paths()
{
	QSettings paths_Settings(Paths_Settings_Path, QSettings::IniFormat);

	// save/open path
	paths_Settings.beginGroup( Last_Paths );
		paths_Settings.setValue( "use_multifitting_directory", use_multifitting_directory );
		paths_Settings.setValue( "use_working_directory", use_working_directory );
		paths_Settings.setValue( "use_last_directory", use_last_directory );
		paths_Settings.setValue( "working_directory", working_directory );
		paths_Settings.setValue( "last_directory", last_directory );
		paths_Settings.setValue( "last_data_directory", last_data_directory );
		paths_Settings.setValue( "last_file", last_file );
	paths_Settings.endGroup();

	// resource path
	paths_Settings.beginGroup( Resource_Paths );
//		paths_Settings.setValue( "icon_path", icon_path );
//	paths_Settings.endGroup();
}

void Settings::read_Gui_Settings(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings gui_Settings(Gui_Settings_Path + add_reset, QSettings::IniFormat);

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
		multilayer_x_corner			= gui_Settings.value( "multilayer_x_corner",		228 ).toInt();
		multilayer_y_corner			= gui_Settings.value( "multilayer_y_corner",		0   ).toInt();
		multilayer_min_width		= gui_Settings.value( "multilayer_min_width",		300 ).toInt();
		multilayer_min_height		= gui_Settings.value( "multilayer_min_height",		100 ).toInt();
		multilayer_width			= gui_Settings.value( "multilayer_width",			450 ).toInt();
		multilayer_height			= gui_Settings.value( "multilayer_height",			450 ).toInt();
	gui_Settings.endGroup();

	// curve window geometry
	gui_Settings.beginGroup( Curve_Window_Geometry );
		independent_x_corner	= gui_Settings.value( "independent_x_corner",	300 ).toInt();
		independent_y_corner	= gui_Settings.value( "independent_y_corner",	200 ).toInt();
		target_x_corner			= gui_Settings.value( "target_x_corner",		300 ).toInt();
		target_y_corner			= gui_Settings.value( "target_y_corner",		10 ).toInt();
	gui_Settings.endGroup();

	// structure table window geometry
	gui_Settings.beginGroup( Structure_Table_Window_Geometry );
		structure_table_x_corner	= gui_Settings.value( "structure_table_x_corner",	500 ).toInt();
		structure_table_y_corner	= gui_Settings.value( "structure_table_y_corner",	200 ).toInt();
		structure_table_width		= gui_Settings.value( "structure_table_width",		1050 ).toInt();
		structure_table_height		= gui_Settings.value( "structure_table_height",		700 ).toInt();
	gui_Settings.endGroup();

	// roughness table window geometry
	gui_Settings.beginGroup( Roughness_Table_Window_Geometry );
		roughness_table_x_corner	= gui_Settings.value( "roughness_table_x_corner",	900 ).toInt();
		roughness_table_y_corner	= gui_Settings.value( "roughness_table_y_corner",	200 ).toInt();
		roughness_table_width		= gui_Settings.value( "roughness_table_width",		1050 ).toInt();
		roughness_table_height		= gui_Settings.value( "roughness_table_height",		700 ).toInt();
	gui_Settings.endGroup();

	// regular aperiodic table window geometry
	gui_Settings.beginGroup( Regular_Aperiodic_Table_Window_Geometry );
		regular_aperiodic_table_x_corner	= gui_Settings.value( "regular_aperiodic_table_x_corner",	500 ).toInt();
		regular_aperiodic_table_y_corner	= gui_Settings.value( "regular_aperiodic_table_y_corner",	200 ).toInt();
		regular_aperiodic_table_width		= gui_Settings.value( "regular_aperiodic_table_width",		350 ).toInt();
		regular_aperiodic_table_height		= gui_Settings.value( "regular_aperiodic_table_height",		700 ).toInt();
	gui_Settings.endGroup();

	// graphs window geometry
	gui_Settings.beginGroup( Graphs_Window_Geometry );
		graphs_x_corner_1D		= gui_Settings.value( "graphs_x_corner_1D",	400 ).toInt();
		graphs_y_corner_1D		= gui_Settings.value( "graphs_y_corner_1D",	300 ).toInt();
		graphs_width_1D			= gui_Settings.value( "graphs_width_1D",	700 ).toInt();
		graphs_height_1D		= gui_Settings.value( "graphs_height_1D",	500 ).toInt();

		graphs_x_corner_2D		= gui_Settings.value( "graphs_x_corner_2D",	430 ).toInt();
		graphs_y_corner_2D		= gui_Settings.value( "graphs_y_corner_2D",	330 ).toInt();
		graphs_width_2D			= gui_Settings.value( "graphs_width_2D",	700 ).toInt();
		graphs_height_2D		= gui_Settings.value( "graphs_height_2D",	500 ).toInt();
	gui_Settings.endGroup();

	// profile plots window geometry
	gui_Settings.beginGroup( Profile_Plots_Window_Geometry );
		profile_plots_x_corner	= gui_Settings.value( "profile_plots_x_corner",	800 ).toInt();
		profile_plots_y_corner	= gui_Settings.value( "profile_plots_y_corner",	100 ).toInt();
		profile_plots_width		= gui_Settings.value( "profile_plots_width",	700 ).toInt();
		profile_plots_height	= gui_Settings.value( "profile_plots_height",	300 ).toInt();
	gui_Settings.endGroup();

	// fitting settings window geometry
	gui_Settings.beginGroup( Fitting_Settings_Window_Geometry );
		fitting_settings_x_corner	= gui_Settings.value( "fitting_settings_x_corner",	768 ).toInt();
		fitting_settings_y_corner	= gui_Settings.value( "fitting_settings_y_corner",	0	).toInt();
	gui_Settings.endGroup();

	// calculation settings window geometry
	gui_Settings.beginGroup( Calculation_Settings_Window_Geometry );
		calculation_settings_x_corner	= gui_Settings.value( "calculation_settings_x_corner",	0 ).toInt();
		calculation_settings_y_corner	= gui_Settings.value( "calculation_settings_y_corner",	532 ).toInt();
		calculation_settings_width		= gui_Settings.value( "calculation_settings_width",		70 ).toInt();
		calculation_settings_height		= gui_Settings.value( "calculation_settings_height",	50 ).toInt();
	gui_Settings.endGroup();

	// general settings window geometry
	gui_Settings.beginGroup( General_Settings_Window_Geometry );
		general_settings_x_corner	= gui_Settings.value( "general_settings_x_corner",	50 ).toInt();
		general_settings_y_corner	= gui_Settings.value( "general_settings_y_corner",	582 ).toInt();
		general_settings_width		= gui_Settings.value( "general_settings_width",		70 ).toInt();
		general_settings_height		= gui_Settings.value( "general_settings_height",	50 ).toInt();
	gui_Settings.endGroup();

	// fits selector window geometry
	gui_Settings.beginGroup( Fits_Selector_Window_Geometry );
		fits_selector_settings_x_corner	= gui_Settings.value( "fits_selector_settings_x_corner",	0 ).toInt();
		fits_selector_settings_y_corner	= gui_Settings.value( "fits_selector_settings_y_corner",	31 ).toInt();
		fits_selector_settings_width	= gui_Settings.value( "fits_selector_settings_width",		235 ).toInt();
		fits_selector_settings_height	= gui_Settings.value( "fits_selector_settings_height",		multilayer_height ).toInt();
	gui_Settings.endGroup();

	// tab names
	gui_Settings.beginGroup( Multilayer_Tabs );
		default_multilayer_tab_name			  = gui_Settings.value( "default_multilayer_tab_name",			 "Struct_" ).toString();
		default_independent_variable_tab_name = gui_Settings.value( "default_independent_variable_tab_name", "Plot_"   ).toString();
		tab_synchronization					  = gui_Settings.value( "tab_synchronization",					  true     ).toBool();
		calculation_time					  = gui_Settings.value( "calculation_time",						  true     ).toBool();
		show_residuals						  = gui_Settings.value( "show_residuals",					  true     ).toBool();
	gui_Settings.endGroup();
}

void Settings::save_Gui_Settings()
{
	QSettings gui_Settings  (Gui_Settings_Path, QSettings::IniFormat);

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
	gui_Settings.endGroup();

	// curve window geometry
	gui_Settings.beginGroup( Curve_Window_Geometry );
		gui_Settings.setValue( "independent_x_corner",		independent_x_corner );
		gui_Settings.setValue( "independent_y_corner",		independent_y_corner );
		gui_Settings.setValue( "target_x_corner",			target_x_corner );
		gui_Settings.setValue( "target_y_corner",			target_y_corner );
	gui_Settings.endGroup();

	// structure table window geometry
	gui_Settings.beginGroup( Structure_Table_Window_Geometry );
		gui_Settings.setValue( "structure_table_x_corner",		structure_table_x_corner );
		gui_Settings.setValue( "structure_table_y_corner",		structure_table_y_corner );
		gui_Settings.setValue( "structure_table_width",			structure_table_width );
		gui_Settings.setValue( "structure_table_height",		structure_table_height );
	gui_Settings.endGroup();

	// roughness table window geometry
	gui_Settings.beginGroup( Roughness_Table_Window_Geometry );
		gui_Settings.setValue( "roughness_table_x_corner",		roughness_table_x_corner );
		gui_Settings.setValue( "roughness_table_y_corner",		roughness_table_y_corner );
		gui_Settings.setValue( "roughness_table_width",			roughness_table_width );
		gui_Settings.setValue( "roughness_table_height",		roughness_table_height );
	gui_Settings.endGroup();

	// regular aperiodic table window geometry
	gui_Settings.beginGroup( Regular_Aperiodic_Table_Window_Geometry );
		gui_Settings.setValue( "regular_aperiodic_table_x_corner",		regular_aperiodic_table_x_corner );
		gui_Settings.setValue( "regular_aperiodic_table_y_corner",		regular_aperiodic_table_y_corner );
		gui_Settings.setValue( "regular_aperiodic_table_width",			regular_aperiodic_table_width );
		gui_Settings.setValue( "regular_aperiodic_table_height",		regular_aperiodic_table_height );		
	gui_Settings.endGroup();

	// graphs window geometry
	gui_Settings.beginGroup( Graphs_Window_Geometry );
		gui_Settings.setValue( "graphs_x_corner_1D",		graphs_x_corner_1D );
		gui_Settings.setValue( "graphs_y_corner_1D",		graphs_y_corner_1D );
		gui_Settings.setValue( "graphs_width_1D",			graphs_width_1D );
		gui_Settings.setValue( "graphs_height_1D",			graphs_height_1D );

		gui_Settings.setValue( "graphs_x_corner_2D",		graphs_x_corner_2D );
		gui_Settings.setValue( "graphs_y_corner_2D",		graphs_y_corner_2D );
		gui_Settings.setValue( "graphs_width_2D",			graphs_width_2D );
		gui_Settings.setValue( "graphs_height_2D",			graphs_height_2D );
	gui_Settings.endGroup();

	// profile plots window geometry
	gui_Settings.beginGroup( Profile_Plots_Window_Geometry );
		gui_Settings.setValue( "profile_plots_x_corner",		profile_plots_x_corner );
		gui_Settings.setValue( "profile_plots_y_corner",		profile_plots_y_corner );
		gui_Settings.setValue( "profile_plots_width",			profile_plots_width );
		gui_Settings.setValue( "profile_plots_height",			profile_plots_height );
	gui_Settings.endGroup();

	// fitting settings window geometry
	gui_Settings.beginGroup( Fitting_Settings_Window_Geometry );
		gui_Settings.setValue( "fitting_settings_x_corner",		fitting_settings_x_corner );
		gui_Settings.setValue( "fitting_settings_y_corner",		fitting_settings_y_corner );
	gui_Settings.endGroup();

	// calculation settings window geometry
	gui_Settings.beginGroup( Calculation_Settings_Window_Geometry );
		gui_Settings.setValue( "calculation_settings_x_corner",		calculation_settings_x_corner );
		gui_Settings.setValue( "calculation_settings_y_corner",		calculation_settings_y_corner );
		gui_Settings.setValue( "calculation_settings_width",		calculation_settings_width );
		gui_Settings.setValue( "calculation_settings_height",		calculation_settings_height );
	gui_Settings.endGroup();

	// general settings window geometry
	gui_Settings.beginGroup( General_Settings_Window_Geometry );
		gui_Settings.setValue( "general_settings_x_corner",		general_settings_x_corner );
		gui_Settings.setValue( "general_settings_y_corner",		general_settings_y_corner );
		gui_Settings.setValue( "general_settings_width",		general_settings_width );
		gui_Settings.setValue( "general_settings_height",		general_settings_height );
	gui_Settings.endGroup();

	// fits selector window geometry
	gui_Settings.beginGroup( Fits_Selector_Window_Geometry );
		gui_Settings.setValue( "fits_selector_settings_x_corner",	fits_selector_settings_x_corner );
		gui_Settings.setValue( "fits_selector_settings_y_corner",	fits_selector_settings_y_corner );
		gui_Settings.setValue( "fits_selector_settings_width",		fits_selector_settings_width );
		gui_Settings.setValue( "fits_selector_settings_height",		fits_selector_settings_height );
	gui_Settings.endGroup();

	// multilayer tab name
	gui_Settings.beginGroup( Multilayer_Tabs );
		gui_Settings.setValue( "default_multilayer_tab_name",			default_multilayer_tab_name );
		gui_Settings.setValue( "default_independent_variable_tab_name", default_independent_variable_tab_name   );
		gui_Settings.setValue( "tab_synchronization",				    tab_synchronization	);
		gui_Settings.setValue( "calculation_time",						calculation_time	);
		gui_Settings.setValue( "individual_residuals",				    show_residuals		);
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
		default_num_confidence_points = structure_Default_Values.value( "default_num_confidence_points",	5 ).toInt();
		auto_density_from_elements = structure_Default_Values.value( "auto_density_from_elements",	true ).toBool();

			structure_Default_Values.beginGroup( Ambient_Values );
				ambient_default_material				  = structure_Default_Values.value( "ambient_default_material",				    Vacuum	).toString();
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
				layer_default_sigma_diffuse				= structure_Default_Values.value( "layer_default_sigma",					0			).toDouble();
				layer_default_sigma_roughness			= structure_Default_Values.value( "layer_default_sigma_roughness",			0			).toDouble();
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
				substrate_default_sigma_diffuse				= structure_Default_Values.value( "substrate_default_sigma_diffuse",			0	).toDouble();
				substrate_default_sigma_roughness			= structure_Default_Values.value( "substrate_default_sigma_roughness",		    0	).toDouble();
			structure_Default_Values.endGroup();
			structure_Default_Values.beginGroup( Stack_Values );
				stack_default_number_of_repetition	= structure_Default_Values.value( "stack_default_number_of_repetition",	1	).toInt();
				stack_default_period				= structure_Default_Values.value( "stack_default_period",				20	).toDouble();
				stack_default_gamma					= structure_Default_Values.value( "stack_default_gamma",				0.5 ).toDouble();
			structure_Default_Values.endGroup();
			structure_Default_Values.beginGroup( Steps );
				step_thickness_transfer	= structure_Default_Values.value( "step_thickness_transfer",0.1 ).toDouble();
				step_composition		= structure_Default_Values.value( "step_composition",		0.1 ).toDouble();
				step_density			= structure_Default_Values.value( "step_density",			0.1 ).toDouble();
				step_thickness			= structure_Default_Values.value( "step_thickness",			0.1 ).toDouble();
				step_sigma_diffuse		= structure_Default_Values.value( "step_sigma_diffuse",		0.1 ).toDouble();

				step_sigma_roughness			= structure_Default_Values.value( "step_sigma_roughness",			0.1  ).toDouble();
				step_sigma_cor_radius			= structure_Default_Values.value( "step_sigma_cor_radius",			100  ).toDouble();
				step_sigma_fractal_alpha		= structure_Default_Values.value( "step_sigma_fractal_alpha",		0.01 ).toDouble();
				step_sigma_vertical_cor_length	= structure_Default_Values.value( "step_sigma_vertical_cor_length",	10   ).toDouble();
				step_sigma_omega				= structure_Default_Values.value( "step_sigma_omega",				0.1  ).toDouble();
				step_sigma_mu					= structure_Default_Values.value( "step_sigma_mu",					10   ).toDouble();

				step_particle_density			   = structure_Default_Values.value( "step_particle_density",			   0.1  ).toDouble();
				step_particle_radius			   = structure_Default_Values.value( "step_particle_radius",			   0.1  ).toDouble();
				step_particle_height			   = structure_Default_Values.value( "step_particle_height",			   0.1  ).toDouble();
				step_particle_distance			   = structure_Default_Values.value( "step_particle_distance",			   1    ).toDouble();
				step_particle_distance_deviation   = structure_Default_Values.value( "step_particle_distance_deviation",   0.1  ).toDouble();
				step_domain_size				   = structure_Default_Values.value( "step_domain_size",				   10   ).toDouble();
				step_particle_z_position		   = structure_Default_Values.value( "step_particle_z_position",		   0.1  ).toDouble();
				step_particle_z_position_deviation = structure_Default_Values.value( "step_particle_z_position_deviation", 0.1  ).toDouble();

				step_interlayer			= structure_Default_Values.value( "step_interlayer",		0.1 ).toDouble();
				step_gamma				= structure_Default_Values.value( "step_gamma",				0.01 ).toDouble();
				step_drift				= structure_Default_Values.value( "step_drift",				0.001).toDouble();
				step_thickness_aperiodic= structure_Default_Values.value( "step_thickness_aperiodic",0.1 ).toDouble();
				step_sigma_aperiodic	= structure_Default_Values.value( "step_sigma_aperiodic",	 0.1 ).toDouble();
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
	structure_Default_Values.setValue( "default_num_confidence_points", default_num_confidence_points	);
	structure_Default_Values.setValue( "auto_density_from_elements", auto_density_from_elements	);

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
			structure_Default_Values.setValue( "layer_default_sigma_diffuse",				layer_default_sigma_diffuse				);
			structure_Default_Values.setValue( "layer_default_sigma_roughness",				layer_default_sigma_roughness			);
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
			structure_Default_Values.setValue( "substrate_default_sigma_diffuse",				substrate_default_sigma_diffuse				);
			structure_Default_Values.setValue( "substrate_default_sigma_roughness",				substrate_default_sigma_roughness			);
		structure_Default_Values.endGroup();
		structure_Default_Values.beginGroup( Stack_Values );
			structure_Default_Values.setValue( "stack_default_number_of_repetition",	stack_default_number_of_repetition	);
			structure_Default_Values.setValue( "stack_default_period",					stack_default_period				);
			structure_Default_Values.setValue( "stack_default_gamma",					stack_default_gamma					);
		structure_Default_Values.endGroup();
		structure_Default_Values.beginGroup( Steps );
			structure_Default_Values.setValue( "step_thickness_transfer",	step_thickness_transfer	);
			structure_Default_Values.setValue( "step_composition",			step_composition		);
			structure_Default_Values.setValue( "step_density",				step_density			);
			structure_Default_Values.setValue( "step_thickness",			step_thickness			);
			structure_Default_Values.setValue( "step_sigma_diffuse",		step_sigma_diffuse		);

			structure_Default_Values.setValue( "step_sigma_roughness",			step_sigma_roughness			);
			structure_Default_Values.setValue( "step_sigma_cor_radius",			step_sigma_cor_radius			);
			structure_Default_Values.setValue( "step_sigma_fractal_alpha",		step_sigma_fractal_alpha		);
			structure_Default_Values.setValue( "step_sigma_vertical_cor_length",step_sigma_vertical_cor_length	);
			structure_Default_Values.setValue( "step_sigma_omega",				step_sigma_omega				);
			structure_Default_Values.setValue( "step_sigma_mu",					step_sigma_mu					);

			structure_Default_Values.setValue( "step_particle_density",				step_particle_density				);
			structure_Default_Values.setValue( "step_particle_radius",				step_particle_radius				);
			structure_Default_Values.setValue( "step_particle_height",				step_particle_height				);
			structure_Default_Values.setValue( "step_particle_distance",			step_particle_distance				);
			structure_Default_Values.setValue( "step_particle_distance_deviation",	step_particle_distance_deviation	);
			structure_Default_Values.setValue( "step_domain_size",					step_domain_size					);
			structure_Default_Values.setValue( "step_particle_z_position",			step_particle_z_position			);
			structure_Default_Values.setValue( "step_particle_z_position_deviation",step_particle_z_position_deviation	);

			structure_Default_Values.setValue( "step_interlayer",			step_interlayer			);
			structure_Default_Values.setValue( "step_gamma",				step_gamma				);
			structure_Default_Values.setValue( "step_drift",				step_drift				);
			structure_Default_Values.setValue( "step_thickness_aperiodic",	step_thickness_aperiodic);
			structure_Default_Values.setValue( "step_sigma_aperiodic",		step_sigma_aperiodic	);
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
			line_edit_angle_precision				= precision_Values.value( "line_edit_angle_precision",				5 ).toInt();
			line_edit_wavelength_precision			= precision_Values.value( "line_edit_wavelength_precision",			6 ).toInt();
			line_edit_spectral_resolution_precision	= precision_Values.value( "line_edit_spectral_resolution_precision",8 ).toInt();
			line_edit_polarization_precision		= precision_Values.value( "line_edit_polarization_precision",		3 ).toInt();
			line_edit_background_precision			= precision_Values.value( "line_edit_background_precision",			3 ).toInt();
			line_edit_beam_size_precision			= precision_Values.value( "line_edit_beam_size_precision",			4 ).toInt();
			line_edit_sample_size_precision			= precision_Values.value( "line_edit_sample_size_precision",		2 ).toInt();
			line_edit_density_precision				= precision_Values.value( "line_edit_density_precision",			4 ).toInt();
			line_edit_permittivity_precision		= precision_Values.value( "line_edit_permittivity_precision",		4 ).toInt();
			line_edit_absorption_precision			= precision_Values.value( "line_edit_absorption_precision",			4 ).toInt();
			line_edit_composition_precision			= precision_Values.value( "line_edit_composition_precision",		4 ).toInt();
			line_edit_thickness_precision			= precision_Values.value( "line_edit_thickness_precision",			4 ).toInt();

			line_edit_sigma_precision				= precision_Values.value( "line_edit_sigma_precision",				4 ).toInt();	// = thickness precision
			line_edit_cor_radius_precision			= precision_Values.value( "line_edit_cor_radius_precision",			0 ).toInt();
			line_edit_fractal_alpha_precision		= precision_Values.value( "line_edit_fractal_alpha_precision",		2 ).toInt();
			line_edit_omega_precision				= precision_Values.value( "line_edit_omega_precision",				2 ).toInt();
			line_edit_mu_precision					= precision_Values.value( "line_edit_mu_precision",					3 ).toInt();

			line_edit_interlayer_precision			= precision_Values.value( "line_edit_interlayer_precision",			3 ).toInt();
			line_edit_drift_precision				= precision_Values.value( "line_edit_drift_precision",				4 ).toInt();
			line_edit_period_precision				= precision_Values.value( "line_edit_period_precision",				4 ).toInt();	// = thickness precision
			line_edit_gamma_precision				= precision_Values.value( "line_edit_gamma_precision",				7 ).toInt();
		precision_Values.endGroup();
		precision_Values.beginGroup( Thumbnail );
			thumbnail_double_format	    = qvariant_cast<char>(precision_Values.value( "thumbnail_double_format",'f'));
			thumbnail_angle_precision				= precision_Values.value( "thumbnail_angle_precision",				3 ).toInt();
			thumbnail_wavelength_precision			= precision_Values.value( "thumbnail_wavelength_precision",			3 ).toInt();
			thumbnail_spectral_resolution_precision	= precision_Values.value( "thumbnail_spectral_resolution_precision",2 ).toInt();
			thumbnail_polarization_precision		= precision_Values.value( "thumbnail_polarization_precision",		3 ).toInt();
			thumbnail_background_precision			= precision_Values.value( "thumbnail_background_precision",			3 ).toInt();
			thumbnail_beam_size_precision			= precision_Values.value( "thumbnail_beam_size_precision",			3 ).toInt();
			thumbnail_sample_size_precision			= precision_Values.value( "thumbnail_sample_size_precision",		2 ).toInt();
			thumbnail_density_precision				= precision_Values.value( "thumbnail_density_precision",			3 ).toInt();
			thumbnail_permittivity_precision		= precision_Values.value( "thumbnail_permittivity_precision",		3 ).toInt();
			thumbnail_absorption_precision			= precision_Values.value( "thumbnail_absorption_precision",			3 ).toInt();
			thumbnail_composition_precision			= precision_Values.value( "thumbnail_composition_precision",		3 ).toInt();
			thumbnail_thickness_precision			= precision_Values.value( "thumbnail_thickness_precision",			3 ).toInt();

			thumbnail_sigma_precision				= precision_Values.value( "thumbnail_sigma_precision",				3 ).toInt();	// = thickness precision
			thumbnail_cor_radius_precision			= precision_Values.value( "thumbnail_cor_radius_precision",			0 ).toInt();
			thumbnail_fractal_alpha_precision		= precision_Values.value( "thumbnail_fractal_alpha_precision",		2 ).toInt();
			thumbnail_omega_precision				= precision_Values.value( "thumbnail_omega_precision",				1 ).toInt();
			thumbnail_mu_precision					= precision_Values.value( "thumbnail_mu_precision",					1 ).toInt();

			thumbnail_interlayer_precision			= precision_Values.value( "thumbnail_interlayer_precision",			3 ).toInt();
			thumbnail_drift_precision				= precision_Values.value( "thumbnail_drift_precision",				4 ).toInt();
			thumbnail_period_precision				= precision_Values.value( "thumbnail_period_precision",				3 ).toInt();	// = thickness precision
			thumbnail_gamma_precision				= precision_Values.value( "thumbnail_gamma_precision",				3 ).toInt();
		precision_Values.endGroup();
		precision_Values.beginGroup( Other );
			at_weight_precision						= precision_Values.value( "at_weight_precision",					5 ).toInt();
			thickness_transfer_precision			= precision_Values.value( "thickness_transfer_precision",			4 ).toInt();
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

			precision_Values.setValue( "line_edit_sigma_precision",				 line_edit_sigma_precision				);
			precision_Values.setValue( "line_edit_cor_radius_precision",		 line_edit_cor_radius_precision			);
			precision_Values.setValue( "line_edit_fractal_alpha_precision",		 line_edit_fractal_alpha_precision		);
			precision_Values.setValue( "line_edit_omega_precision",				 line_edit_omega_precision				);
			precision_Values.setValue( "line_edit_mu_precision",				 line_edit_mu_precision					);

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

			precision_Values.setValue( "thumbnail_sigma_precision",					thumbnail_sigma_precision				);
			precision_Values.setValue( "thumbnail_cor_radius_precision",			thumbnail_cor_radius_precision			);
			precision_Values.setValue( "thumbnail_fractal_alpha_precision",			thumbnail_fractal_alpha_precision		);
			precision_Values.setValue( "thumbnail_omega_precision",					thumbnail_omega_precision				);
			precision_Values.setValue( "thumbnail_mu_precision",					thumbnail_mu_precision					);

			precision_Values.setValue( "thumbnail_interlayer_precision",	thumbnail_interlayer_precision		);
			precision_Values.setValue( "thumbnail_drift_precision",			thumbnail_drift_precision			);
			precision_Values.setValue( "thumbnail_period_precision",		thumbnail_period_precision			);
			precision_Values.setValue( "thumbnail_gamma_precision",			thumbnail_gamma_precision			);
		precision_Values.endGroup();
		precision_Values.beginGroup( Other );
			precision_Values.setValue( "at_weight_precision",				at_weight_precision );
			precision_Values.setValue( "thickness_transfer_precision",		thickness_transfer_precision );
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
		default_num_spectral_points							= parameters_Default_Values.value( "default_num_spectral_points",							400		).toInt();
		default_num_theta_0_angular_points					= parameters_Default_Values.value( "default_num_theta_0_angular_points",					1000	).toInt();
		default_num_theta_angular_points					= parameters_Default_Values.value( "default_num_theta_angular_points",						1000	).toInt();
		default_num_phi_angular_points						= parameters_Default_Values.value( "default_num_phi_angular_points",						501		).toInt();

		default_beam_theta_0_specular_position				= parameters_Default_Values.value( "default_beam_theta_0_specular_position",				1		).toDouble();
		default_detector_theta_offset						= parameters_Default_Values.value( "default_detector_theta_offset",							0.5		).toDouble();

		default_wavelength_value							= parameters_Default_Values.value( "default_wavelength_value",								1.540562).toDouble();
		default_min_wavelength								= parameters_Default_Values.value( "default_min_wavelength",								100		).toDouble();
		default_max_wavelength								= parameters_Default_Values.value( "default_max_wavelength",								200		).toDouble();

		default_theta_0_angle_value							= parameters_Default_Values.value( "default_theta_0_angle_value",							0.5		).toDouble();
		default_min_theta_0_angle							= parameters_Default_Values.value( "default_min_theta_0_angle",								0		).toDouble();
		default_max_theta_0_angle							= parameters_Default_Values.value( "default_max_theta_0_angle",								7		).toDouble();

		default_theta_angle_value							= parameters_Default_Values.value( "default_theta_angle_value",								1		).toDouble();
		default_min_theta_angle								= parameters_Default_Values.value( "default_min_theta_angle",								0.01	).toDouble();
		default_max_theta_angle								= parameters_Default_Values.value( "default_max_theta_angle",								6		).toDouble();

		default_phi_angle_value								= parameters_Default_Values.value( "default_phi_angle_value",								0		).toDouble();
		default_min_phi_angle								= parameters_Default_Values.value( "default_min_phi_angle",									-1.3	).toDouble();
		default_max_phi_angle								= parameters_Default_Values.value( "default_max_phi_angle",									1.3		).toDouble();

		default_logarithmic_threshold_beta					= parameters_Default_Values.value( "default_logarithmic_threshold_beta",					1E-17	).toDouble();
		default_logarithmic_threshold_material_density		= parameters_Default_Values.value( "default_logarithmic_threshold_material_density",		1E-7	).toDouble();
		default_logarithmic_threshold_element_concentration = parameters_Default_Values.value( "default_logarithmic_threshold_element_concentration",	1E15	).toDouble();


		default_spectral_distribution_FWHM					= parameters_Default_Values.value( "default_spectral_distribution_FWHM",					0	).toDouble();
		default_spectral_distribution_coverage				= parameters_Default_Values.value( "default_spectral_distribution_coverage",				2	).toInt();
		default_spectral_distribution_use_sampling			= parameters_Default_Values.value( "default_spectral_distribution_use_sampling",		false	).toBool();
		default_spectral_distribution_number_of_Samples		= parameters_Default_Values.value( "default_spectral_distribution_number_of_Samples",		5	).toInt();

		default_beam_Theta_0_distribution_FWHM				= parameters_Default_Values.value( "default_beam_Theta_0_distribution_FWHM",				0	).toDouble();
		default_beam_Theta_0_distribution_coverage			= parameters_Default_Values.value( "default_beam_Theta_0_distribution_coverage",			2	).toInt();
		default_beam_Theta_0_distribution_use_sampling		= parameters_Default_Values.value( "default_beam_Theta_0_distribution_use_sampling",	false	).toBool();
		default_beam_Theta_0_distribution_number_of_Samples = parameters_Default_Values.value( "default_beam_Theta_0_distribution_number_of_Samples",	5	).toInt();

		default_beam_Phi_0_distribution_FWHM				= parameters_Default_Values.value( "default_beam_Phi_0_distribution_FWHM",					0	).toDouble();
		default_beam_Phi_0_distribution_coverage			= parameters_Default_Values.value( "default_beam_Phi_0_distribution_coverage",				2	).toInt();
		default_beam_Phi_0_distribution_use_sampling		= parameters_Default_Values.value( "default_beam_Phi_0_distribution_use_sampling",		false	).toBool();
		default_beam_Phi_0_distribution_number_of_Samples	= parameters_Default_Values.value( "default_beam_Phi_0_distribution_number_of_Samples",		5	).toInt();

		default_detector_1D_slit_width						= parameters_Default_Values.value( "default_detector_1D_slit_width",			0.8	).toDouble();
		default_detector_1D_distance_to_sample				= parameters_Default_Values.value( "default_detector_1D_distance_to_sample",	300	).toDouble();

		default_detector_1D_theta_resolution_FWHM			= parameters_Default_Values.value( "default_detector_1D_theta_resolution_FWHM",	0	).toDouble();
		default_detector_2D_theta_resolution_FWHM			= parameters_Default_Values.value( "default_detector_2D_theta_resolution_FWHM",	0	).toDouble();
		default_detector_2D_phi_resolution_FWHM				= parameters_Default_Values.value( "default_detector_2D_phi_resolution_FWHM",	0	).toDouble();

		default_beam_geometry_size							= parameters_Default_Values.value( "default_beam_geometry_size",			0.050	).toDouble();
		default_beam_geometry_smoothing						= parameters_Default_Values.value( "default_beam_geometry_smoothing",		0.5		).toDouble();
		default_beam_wings_full_width						= parameters_Default_Values.value( "default_beam_wings_full_width",			0.25	).toDouble();
		default_beam_wings_intensity						= parameters_Default_Values.value( "default_beam_wings_intensity",			0.0		).toDouble();
		default_beam_lateral_width							= parameters_Default_Values.value( "default_beam_lateral_width",			5.0		).toDouble();
		default_beam_log_profile_plot						= parameters_Default_Values.value( "default_beam_log_profile_plot",			false	).toBool();

		default_sample_geometry_size						= parameters_Default_Values.value( "default_sample_geometry_size",			20		).toDouble();
		default_sample_geometry_x_position					= parameters_Default_Values.value( "default_sample_geometry_x_position",	0		).toDouble();
		default_sample_geometry_z_position					= parameters_Default_Values.value( "default_sample_geometry_z_position",	0		).toDouble();
		default_sample_geometry_curvature					= parameters_Default_Values.value( "default_sample_geometry_curvature",		0		).toDouble();

		default_polarization								= parameters_Default_Values.value( "default_polarization",					1		).toDouble();
		default_background									= parameters_Default_Values.value( "default_background",					0		).toDouble();

	parameters_Default_Values.endGroup();
}

void Settings::save_Parameters_Default_Values()
{
	QSettings parameters_Default_Values(Parameters_Default_Values_Path, QSettings::IniFormat);

	// parameters default values
	parameters_Default_Values.beginGroup( Independent_Values );
		parameters_Default_Values.setValue( "default_num_spectral_points",					default_num_spectral_points			   );
		parameters_Default_Values.setValue( "default_num_theta_0_angular_points",			default_num_theta_0_angular_points	   );
		parameters_Default_Values.setValue( "default_num_theta_angular_points",				default_num_theta_angular_points	   );
		parameters_Default_Values.setValue( "default_num_phi_angular_points",				default_num_phi_angular_points		   );

		parameters_Default_Values.setValue( "default_beam_theta_0_specular_position",		default_beam_theta_0_specular_position );
		parameters_Default_Values.setValue( "default_detector_theta_offset",				default_detector_theta_offset		   );

		parameters_Default_Values.setValue( "default_wavelength_value",						default_wavelength_value			   );
		parameters_Default_Values.setValue( "default_min_wavelength",						default_min_wavelength				   );
		parameters_Default_Values.setValue( "default_max_wavelength",						default_max_wavelength				   );

		parameters_Default_Values.setValue( "default_theta_0_angle_value",					default_theta_0_angle_value			   );
		parameters_Default_Values.setValue( "default_min_theta_0_angle",					default_min_theta_0_angle			   );
		parameters_Default_Values.setValue( "default_max_theta_0_angle",					default_max_theta_0_angle			   );

		parameters_Default_Values.setValue( "default_theta_angle_value",					default_theta_angle_value			   );
		parameters_Default_Values.setValue( "default_min_theta_angle",						default_min_theta_angle				   );
		parameters_Default_Values.setValue( "default_max_theta_angle",						default_max_theta_angle				   );

		parameters_Default_Values.setValue( "default_phi_angle_value",						default_phi_angle_value				   );
		parameters_Default_Values.setValue( "default_min_phi_angle",						default_min_phi_angle				   );
		parameters_Default_Values.setValue( "default_max_phi_angle",						default_max_phi_angle				   );

		parameters_Default_Values.setValue( "default_logarithmic_threshold_beta",					default_logarithmic_threshold_beta				   );
		parameters_Default_Values.setValue( "default_logarithmic_threshold_material_density",		default_logarithmic_threshold_material_density	   );
		parameters_Default_Values.setValue( "default_logarithmic_threshold_element_concentration",	default_logarithmic_threshold_element_concentration);


		parameters_Default_Values.setValue( "default_spectral_distribution_FWHM",					default_spectral_distribution_FWHM);
		parameters_Default_Values.setValue( "default_spectral_distribution_coverage",				default_spectral_distribution_coverage);
		parameters_Default_Values.setValue( "default_spectral_distribution_use_sampling",			default_spectral_distribution_use_sampling);
		parameters_Default_Values.setValue( "default_spectral_distribution_number_of_Samples",		default_spectral_distribution_number_of_Samples);

		parameters_Default_Values.setValue( "default_beam_Theta_0_distribution_FWHM",				default_beam_Theta_0_distribution_FWHM);
		parameters_Default_Values.setValue( "default_beam_Theta_0_distribution_coverage",			default_beam_Theta_0_distribution_coverage);
		parameters_Default_Values.setValue( "default_beam_Theta_0_distribution_use_sampling",		default_beam_Theta_0_distribution_use_sampling);
		parameters_Default_Values.setValue( "default_beam_Theta_0_distribution_number_of_Samples",	default_beam_Theta_0_distribution_number_of_Samples);

		parameters_Default_Values.setValue( "default_beam_Phi_0_distribution_FWHM",					default_beam_Phi_0_distribution_FWHM);
		parameters_Default_Values.setValue( "default_beam_Phi_0_distribution_coverage",				default_beam_Phi_0_distribution_coverage);
		parameters_Default_Values.setValue( "default_beam_Phi_0_distribution_use_sampling",			default_beam_Phi_0_distribution_use_sampling);
		parameters_Default_Values.setValue( "default_beam_Phi_0_distribution_number_of_Samples",	default_beam_Phi_0_distribution_number_of_Samples);

		parameters_Default_Values.setValue( "default_detector_1D_slit_width",						default_detector_1D_slit_width);
		parameters_Default_Values.setValue( "default_detector_1D_distance_to_sample",				default_detector_1D_distance_to_sample);

		parameters_Default_Values.setValue( "default_detector_1D_theta_resolution_FWHM",			default_detector_1D_theta_resolution_FWHM);
		parameters_Default_Values.setValue( "default_detector_2D_theta_resolution_FWHM",			default_detector_2D_theta_resolution_FWHM);
		parameters_Default_Values.setValue( "default_detector_2D_phi_resolution_FWHM",				default_detector_2D_phi_resolution_FWHM);

		parameters_Default_Values.setValue( "default_beam_geometry_size",							default_beam_geometry_size);
		parameters_Default_Values.setValue( "default_beam_geometry_smoothing",						default_beam_geometry_smoothing);
		parameters_Default_Values.setValue( "default_beam_wings_full_width",						default_beam_wings_full_width);
		parameters_Default_Values.setValue( "default_beam_wings_intensity",							default_beam_wings_intensity);
		parameters_Default_Values.setValue( "default_beam_lateral_width",							default_beam_lateral_width);
		parameters_Default_Values.setValue( "default_beam_log_profile_plot",						default_beam_log_profile_plot);

		parameters_Default_Values.setValue( "default_sample_geometry_size",							default_sample_geometry_size);
		parameters_Default_Values.setValue( "default_sample_geometry_x_position",					default_sample_geometry_x_position);
		parameters_Default_Values.setValue( "default_sample_geometry_z_position",					default_sample_geometry_z_position);
		parameters_Default_Values.setValue( "default_sample_geometry_curvature",					default_sample_geometry_curvature);

		parameters_Default_Values.setValue( "default_polarization",									default_polarization);
		parameters_Default_Values.setValue( "default_background",									default_background);
	parameters_Default_Values.endGroup();
}

void Settings::read_Units(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings units(Units_Path + add_reset, QSettings::IniFormat);

	// units
	units.beginGroup( Units );
		length_units			= units.value( "length_units",		length_Units_List    .first() ).toString();
		density_units			= units.value( "density_units",		density_Units_List   .first() ).toString();
		opt_const_units			= units.value( "opt_const_units",	opt_Const_Units_List .first() ).toString();
	units.endGroup();
}

void Settings::save_Units()
{
	QSettings units(Units_Path, QSettings::IniFormat);

	// units
	units.beginGroup( Units );
		units.setValue( "length_units",		length_units	);
		units.setValue( "density_units",	density_units   );
		units.setValue( "opt_const_units",	opt_const_units	);
	units.endGroup();
}

void Settings::read_General_Settings(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings general_Settings(General_Settings_Path + add_reset, QSettings::IniFormat);

	general_Settings_Tab_Index = general_Settings.value( "general_Settings_Tab_Index",	0 ).toInt();
	general_Settings.beginGroup( Threads );
		optical_Constants_Read_Threads = general_Settings.value( "optical_Constants_Read_Threads",	max(QThread::idealThreadCount(),1) ).toInt();
		reflectivity_Calc_Threads	   = general_Settings.value( "reflectivity_Calc_Threads",		max(QThread::idealThreadCount(),1) ).toInt();
		epsilon_Partial_Fill_Threads   = general_Settings.value( "epsilon_Partial_Fill_Threads",	max(QThread::idealThreadCount(),1) ).toInt();
		n_Max_Series				   = general_Settings.value( "n_Max_Series",													 5 ).toInt();
		use_Simplified_Scattering_Weak_Factor  = general_Settings.value( "use_Simplified_Scattering_Weak_Factor",			     false ).toBool();
	general_Settings.endGroup();
	general_Settings.beginGroup( Recalculation );
		recalculate_Spinbox_Global			 = general_Settings.value( "recalculate_Spinbox_Global",			false  ).toBool();
		recalculate_Spinbox_Table			 = general_Settings.value( "recalculate_Spinbox_Table",				true  ).toBool();
		mouse_Wheel_Spinbox_Table			 = general_Settings.value( "mouse_Wheel_Spinbox_Table",				true ).toBool();
		refill_Dependent_Table				 = general_Settings.value( "refill_Dependent_Table",				true ).toBool();
		aperiodic_Recalculate_Spinbox_Table	 = general_Settings.value( "aperiodic_Recalculate_Spinbox_Table",	true ).toBool();
		aperiodic_Mouse_Wheel_Spinbox_Table	 = general_Settings.value( "aperiodic_Mouse_Wheel_Spinbox_Table",	true ).toBool();
		replot_Graphs_During_Fitting_1D		 = general_Settings.value( "replot_Graphs_During_Fitting_1D",		true  ).toBool();
		replot_Graphs_During_Fitting_2D		 = general_Settings.value( "replot_Graphs_During_Fitting_2D",		false ).toBool();
	general_Settings.endGroup();
	general_Settings.beginGroup( Output );
		print_1D_Data_On_Recalculation		 = general_Settings.value( "print_1D_Data_On_Recalculation",		true  ).toBool();
		print_2D_Data_On_Recalculation		 = general_Settings.value( "print_2D_Data_On_Recalculation",		false ).toBool();
	general_Settings.endGroup();

	// limit max number of threads
	reflectivity_Calc_Threads = min(QThread::idealThreadCount(),reflectivity_Calc_Threads);
}

void Settings::save_General_Settings()
{
	QSettings general_Settings(General_Settings_Path, QSettings::IniFormat);

	general_Settings.setValue( "general_Settings_Tab_Index", general_Settings_Tab_Index );
	general_Settings.beginGroup( Threads );
		general_Settings.setValue( "optical_Constants_Read_Threads", optical_Constants_Read_Threads );
		general_Settings.setValue( "reflectivity_Calc_Threads",		 reflectivity_Calc_Threads		);
		general_Settings.setValue( "epsilon_Partial_Fill_Threads",	 epsilon_Partial_Fill_Threads	);
		general_Settings.setValue( "n_Max_Series",					 n_Max_Series					);
		general_Settings.setValue( "use_Simplified_Scattering_Weak_Factor", use_Simplified_Scattering_Weak_Factor);
	general_Settings.endGroup();
	general_Settings.beginGroup( Recalculation );
		general_Settings.setValue( "recalculate_Spinbox_Global", recalculate_Spinbox_Global	);
		general_Settings.setValue( "recalculate_Spinbox_Table",	 recalculate_Spinbox_Table	);
		general_Settings.setValue( "mouse_Wheel_Spinbox_Table",	 mouse_Wheel_Spinbox_Table	);
		general_Settings.setValue( "refill_Dependent_Table",	 refill_Dependent_Table		);
		general_Settings.setValue( "aperiodic_Recalculate_Spinbox_Table", aperiodic_Recalculate_Spinbox_Table );
		general_Settings.setValue( "aperiodic_Mouse_Wheel_Spinbox_Table", aperiodic_Mouse_Wheel_Spinbox_Table );
		general_Settings.setValue( "replot_Graphs_During_Fitting_1D",	  replot_Graphs_During_Fitting_1D );
		general_Settings.setValue( "replot_Graphs_During_Fitting_2D",	  replot_Graphs_During_Fitting_2D );
	general_Settings.endGroup();
	general_Settings.beginGroup( Output );
		general_Settings.setValue( "print_1D_Data_On_Recalculation", print_1D_Data_On_Recalculation );
		general_Settings.setValue( "print_2D_Data_On_Recalculation", print_2D_Data_On_Recalculation );
	general_Settings.endGroup();
}

void Settings::read_Measurements(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings measurements(Measurements_Path + add_reset, QSettings::IniFormat);

	// measurements
	measurements.beginGroup( Filenames );
		default_Measured_Filename		 = measurements.value( "default_Measured_Filename",			"measured.txt"			  ).toString();
	measurements.endGroup();
	measurements.beginGroup( Aperiodic );
		aperiodic_default_sigma_import	 = measurements.value( "aperiodic_default_sigma_import",	false					  ).toBool();
		aperiodic_default_density_import = measurements.value( "aperiodic_default_density_import",	false					  ).toBool();
		aperiodic_default_units_import	 = measurements.value( "aperiodic_default_units_import",	length_Units_List.first() ).toString();
	measurements.endGroup();
}

void Settings::save_Measurements()
{
	QSettings measurements(Measurements_Path, QSettings::IniFormat);

	// measurements
	measurements.beginGroup( Filenames );
		measurements.setValue( "default_Measured_Filename",			default_Measured_Filename		);
	measurements.endGroup();
	measurements.beginGroup( Aperiodic );
		measurements.setValue( "aperiodic_default_sigma_import",	aperiodic_default_sigma_import	);
		measurements.setValue( "aperiodic_default_density_import",	aperiodic_default_density_import);
		measurements.setValue( "aperiodic_default_units_import",	aperiodic_default_units_import	);
	measurements.endGroup();
}

// -----------------------------------------------------------------------------------------

void Settings::read_All_Settings(bool reset_to_default)
{
	read_Paths(reset_to_default);
	read_Gui_Settings(reset_to_default);
	read_Structure_Default_Values(reset_to_default);
	read_Precisions(reset_to_default);
	read_Parameters_Default_Values(reset_to_default);
	read_Units(reset_to_default);
	read_General_Settings(reset_to_default);
	read_Measurements(reset_to_default);
}

void Settings::save_All_Settings()
{
	save_Paths();
	save_Gui_Settings();
	save_Structure_Default_Values();
	save_Precisions();
	save_Parameters_Default_Values();
	save_Units();
	save_General_Settings();
	save_Measurements();
}
