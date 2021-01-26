#include "settings.h"

// -----------------------------------------------------------------------------------------

// gui

// application style
QString application_style;

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

///====================================================================
/// aperiodic
///====================================================================

bool recalculate_spinbox_aperiodic_table;
bool mouse_wheel_spinbox_aperiodic_table;

bool aperiodic_default_sigma_import;
bool aperiodic_default_density_import;
QString aperiodic_default_units_import;

///====================================================================
/// structure default values
///====================================================================

double default_num_confidence_points;
bool auto_density_from_elements;

// Layer_Values
QString	default_layer_material;

double	default_layer_absolute_density;
double	default_layer_absolute_density_min;
double	default_layer_absolute_density_max;

double	default_layer_relative_density;
double	default_layer_relative_density_min;
double	default_layer_relative_density_max;

bool	default_layer_composed_material;
QString	default_layer_stoichiometry_element;

double	default_layer_thickness;
double	default_layer_thickness_min;
double	default_layer_thickness_max;

bool	default_thickness_drift_line_enabled;
bool	default_thickness_drift_line_show;
double	default_thickness_drift_line;
double	default_thickness_drift_line_min;
double	default_thickness_drift_line_max;

bool	default_thickness_drift_rand_enabled;
bool	default_thickness_drift_rand_show;
double	default_thickness_drift_rand;
double	default_thickness_drift_rand_min;
double	default_thickness_drift_rand_max;

bool	default_thickness_drift_sine_enabled;
bool	default_thickness_drift_sine_show;
double	default_thickness_drift_sine_amplitude;
double	default_thickness_drift_sine_amplitude_min;
double	default_thickness_drift_sine_amplitude_max;
double	default_thickness_drift_sine_frequency;
double	default_thickness_drift_sine_frequency_min;
double	default_thickness_drift_sine_frequency_max;
double	default_thickness_drift_sine_phase;
double	default_thickness_drift_sine_phase_min;
double	default_thickness_drift_sine_phase_max;

bool	default_sigma_drift_line_enabled;
bool	default_sigma_drift_line_show;
double	default_sigma_drift_line;
double	default_sigma_drift_line_min;
double	default_sigma_drift_line_max;

bool	default_sigma_drift_rand_enabled;
bool	default_sigma_drift_rand_show;
double	default_sigma_drift_rand;
double	default_sigma_drift_rand_min;
double	default_sigma_drift_rand_max;

bool	default_sigma_drift_sine_enabled;
bool	default_sigma_drift_sine_show;
double	default_sigma_drift_sine_amplitude;
double	default_sigma_drift_sine_amplitude_min;
double	default_sigma_drift_sine_amplitude_max;
double	default_sigma_drift_sine_frequency;
double	default_sigma_drift_sine_frequency_min;
double	default_sigma_drift_sine_frequency_max;
double	default_sigma_drift_sine_phase;
double	default_sigma_drift_sine_phase_min;
double	default_sigma_drift_sine_phase_max;

// Substrate_Values
QString	default_substrate_material;
double	default_substrate_absolute_density;
double	default_substrate_absolute_density_min;
double	default_substrate_absolute_density_max;

double	default_substrate_relative_density;
double	default_substrate_relative_density_min;
double	default_substrate_relative_density_max;

bool	default_substrate_composed_material;
QString	default_substrate_stoichiometry_element;

// Common_Values
double	default_diffuse_sigma;
double	default_diffuse_sigma_min;
double	default_diffuse_sigma_max;

double	default_stoichiometry_composition;
double	default_stoichiometry_composition_min;
double	default_stoichiometry_composition_max;

// Roughness_Values
bool	default_roughness_enabled;

double	default_roughness_sigma;
double	default_roughness_sigma_min;
double	default_roughness_sigma_max;

double	default_roughness_correlation_radius;
double	default_roughness_correlation_radius_min;
double	default_roughness_correlation_radius_max;

double	default_roughness_fractal_alpha;
double	default_roughness_fractal_alpha_min;
double	default_roughness_fractal_alpha_max;

double	default_roughness_omega;
double	default_roughness_omega_min;
double	default_roughness_omega_max;

double	default_roughness_mu;
double	default_roughness_mu_min;
double	default_roughness_mu_max;

// Fluctuation_Values
bool	default_fluctuation_enabled;
bool	default_fluctuation_used;		// true

double	default_fluctuation_absolute_density;
double	default_fluctuation_absolute_density_min;
double	default_fluctuation_absolute_density_max;

double	default_fluctuation_relative_density;
double	default_fluctuation_relative_density_min;
double	default_fluctuation_relative_density_max;

double	default_fluctuation_particle_radius;
double	default_fluctuation_particle_radius_min;
double	default_fluctuation_particle_radius_max;

double	default_fluctuation_particle_height;
double	default_fluctuation_particle_height_min;
double	default_fluctuation_particle_height_max;

double	default_fluctuation_particle_distance;
double	default_fluctuation_particle_distance_min;
double	default_fluctuation_particle_distance_max;

double	default_fluctuation_particle_distance_deviation;
double	default_fluctuation_particle_distance_deviation_min;
double	default_fluctuation_particle_distance_deviation_max;

double	default_fluctuation_domain_size;
double	default_fluctuation_domain_size_min;
double	default_fluctuation_domain_size_max;

double	default_fluctuation_z_position;
double	default_fluctuation_z_position_min;
double	default_fluctuation_z_position_max;

double	default_fluctuation_z_position_deviation;
double	default_fluctuation_z_position_deviation_min;
double	default_fluctuation_z_position_deviation_max;

// Stack_Values
int		default_stack_number_of_repetition;
int		default_stack_number_of_repetition_min;
int		default_stack_number_of_repetition_max;

double	default_stack_period;
double	default_stack_period_min;
double	default_stack_period_max;

double	default_stack_gamma;
double	default_stack_gamma_min;
double	default_stack_gamma_max;

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

///====================================================================
/// precisions
///====================================================================

// lineedit
char line_edit_double_format		;
char line_edit_short_double_format	;
int line_edit_angle_precision		;
int line_edit_wavelength_precision	;
int line_edit_spectral_resolution_precision	;
int line_edit_polarization_precision;
int line_edit_density_precision		;
int line_edit_composition_precision	;
int line_edit_thickness_precision	;

int line_edit_sigma_precision			;
int line_edit_cor_radius_precision		;
int line_edit_fractal_alpha_precision	;
int line_edit_omega_precision			;
int line_edit_mu_precision				;

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
int thumbnail_composition_precision	;
int thumbnail_thickness_precision	;

int thumbnail_sigma_precision			;
int thumbnail_cor_radius_precision		;
int thumbnail_fractal_alpha_precision	;
int thumbnail_omega_precision			;
int thumbnail_mu_precision				;

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

///====================================================================
/// measurement default values
///====================================================================

int default_wavelength_num_points;
int default_beam_theta_0_angle_num_points;
int default_detector_theta_angle_num_points;
int default_phi_angle_num_points;

int default_beam_theta_0_angle_specular_position;
int default_detector_theta_angle_offset;

double default_beam_theta_0_angle;
double default_beam_theta_0_angle_min;
double default_beam_theta_0_angle_max;

double default_detector_theta_angle;
double default_detector_theta_angle_min;
double default_detector_theta_angle_max;

double default_phi_angle;
double default_phi_angle_min;
double default_phi_angle_max;

double default_wavelength;
double default_wavelength_min;
double default_wavelength_max;

double default_logarithmic_threshold_beta;
double default_logarithmic_threshold_material_density;
double default_logarithmic_threshold_element_concentration;

double default_spectral_distribution_FWHM;
double default_spectral_distribution_coverage;
bool   default_spectral_distribution_use_sampling;
int    default_spectral_distribution_number_of_Samples;

double default_beam_theta_0_distribution_FWHM;
double default_beam_theta_0_distribution_coverage;
bool   default_beam_theta_0_distribution_use_sampling;
int    default_beam_theta_0_distribution_number_of_Samples;

double default_beam_phi_0_distribution_FWHM;
double default_beam_phi_0_distribution_coverage;
bool   default_beam_phi_0_distribution_use_sampling;
int    default_beam_phi_0_distribution_number_of_Samples;

double default_detector_1D_slit_width;
double default_detector_1D_distance_to_sample;

double default_detector_1D_theta_resolution_FWHM;
double default_detector_2D_theta_resolution_FWHM;
double default_detector_2D_phi_resolution_FWHM;

double default_beam_geometry_size;
double default_beam_geometry_smoothing;
double default_beam_geometry_wings_full_width;
double default_beam_geometry_wings_intensity;
double default_beam_geometry_lateral_width;
bool   default_beam_geometry_log_profile_plot;

double default_sample_geometry_size;
double default_sample_geometry_x_position;
double default_sample_geometry_z_position;
double default_sample_geometry_curvature;

double default_polarization;
double default_background;

///====================================================================
/// general settings
///====================================================================

int general_Settings_Tab_Index;
int reflectivity_calc_threads;
int optical_constants_read_threads;

bool recalculate_spinbox_global;

bool recalculate_spinbox_structure_table;
bool mouse_wheel_spinbox_structure_table;
bool refill_dependent_structure_table;
bool replot_graphs_during_fitting_1D;
//bool replot_graphs_during_fitting_2D;

int  n_max_series;
bool use_beam_spot_in_specular_peak;

bool print_1D_Data_On_Recalculation;
bool print_2D_Data_On_Recalculation;

bool use_multifitting_directory;
bool use_working_directory;
QString  working_directory;

bool use_last_directory;
QString  last_directory;
QString preliminary_last_directory;
QString preliminary_last_data_directory;
QString				last_data_directory;
QString preliminary_last_file;
QString				last_file;

QString default_multilayer_tab_name;
QString default_independent_curve_tab_name;

bool tab_synchronization;
bool calculation_time;
bool show_residuals;

///====================================================================

Settings::Settings()
{

}

void Settings::read_Windows_Geometry_Settings(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings windows_Geometry_Settings(Windows_Geometry_Settings_Path + add_reset, QSettings::IniFormat);

	if(reset_to_default) windows_Geometry_Settings.setPath(windows_Geometry_Settings.format(),windows_Geometry_Settings.scope(),"");

	windows_Geometry_Settings.beginGroup( Application_Style );
		application_style = windows_Geometry_Settings.value( "application_style", "WindowsXP" ).toString(); // Fusion Windows WindowsXP WindowsVista
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Multilayer_Window_Geometry );
		multilayer_x_corner			= windows_Geometry_Settings.value( "multilayer_x_corner",		228 ).toInt();
		multilayer_y_corner			= windows_Geometry_Settings.value( "multilayer_y_corner",		0   ).toInt();
		multilayer_min_width		= windows_Geometry_Settings.value( "multilayer_min_width",		300 ).toInt();
		multilayer_min_height		= windows_Geometry_Settings.value( "multilayer_min_height",		100 ).toInt();
		multilayer_width			= windows_Geometry_Settings.value( "multilayer_width",			450 ).toInt();
		multilayer_height			= windows_Geometry_Settings.value( "multilayer_height",			450 ).toInt();
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Curve_Editor_Window_Geometry );
		independent_x_corner		= windows_Geometry_Settings.value( "independent_x_corner",	300 ).toInt();
		independent_y_corner		= windows_Geometry_Settings.value( "independent_y_corner",	200 ).toInt();
		target_x_corner				= windows_Geometry_Settings.value( "target_x_corner",		300 ).toInt();
		target_y_corner				= windows_Geometry_Settings.value( "target_y_corner",		10  ).toInt();
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Structure_Table_Window_Geometry );
		structure_table_x_corner	= windows_Geometry_Settings.value( "structure_table_x_corner",	500 ).toInt();
		structure_table_y_corner	= windows_Geometry_Settings.value( "structure_table_y_corner",	200 ).toInt();
		structure_table_width		= windows_Geometry_Settings.value( "structure_table_width",		1050).toInt();
		structure_table_height		= windows_Geometry_Settings.value( "structure_table_height",	700 ).toInt();
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Regular_Aperiodic_Table_Window_Geometry );
		regular_aperiodic_table_x_corner	= windows_Geometry_Settings.value( "regular_aperiodic_table_x_corner",	500 ).toInt();
		regular_aperiodic_table_y_corner	= windows_Geometry_Settings.value( "regular_aperiodic_table_y_corner",	200 ).toInt();
		regular_aperiodic_table_width		= windows_Geometry_Settings.value( "regular_aperiodic_table_width",		350 ).toInt();
		regular_aperiodic_table_height		= windows_Geometry_Settings.value( "regular_aperiodic_table_height",	700 ).toInt();
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Graphs_Window_Geometry );
		graphs_x_corner_1D			= windows_Geometry_Settings.value( "graphs_x_corner_1D",	400 ).toInt();
		graphs_y_corner_1D			= windows_Geometry_Settings.value( "graphs_y_corner_1D",	300 ).toInt();
		graphs_width_1D				= windows_Geometry_Settings.value( "graphs_width_1D",		700 ).toInt();
		graphs_height_1D			= windows_Geometry_Settings.value( "graphs_height_1D",		500 ).toInt();

		graphs_x_corner_2D			= windows_Geometry_Settings.value( "graphs_x_corner_2D",	430 ).toInt();
		graphs_y_corner_2D			= windows_Geometry_Settings.value( "graphs_y_corner_2D",	330 ).toInt();
		graphs_width_2D				= windows_Geometry_Settings.value( "graphs_width_2D",		700 ).toInt();
		graphs_height_2D			= windows_Geometry_Settings.value( "graphs_height_2D",		500 ).toInt();
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Profile_Plots_Window_Geometry );
		profile_plots_x_corner	= windows_Geometry_Settings.value( "profile_plots_x_corner",	800 ).toInt();
		profile_plots_y_corner	= windows_Geometry_Settings.value( "profile_plots_y_corner",	100 ).toInt();
		profile_plots_width		= windows_Geometry_Settings.value( "profile_plots_width",		700 ).toInt();
		profile_plots_height	= windows_Geometry_Settings.value( "profile_plots_height",		300 ).toInt();
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Fitting_Settings_Window_Geometry );
		fitting_settings_x_corner	= windows_Geometry_Settings.value( "fitting_settings_x_corner",	768 ).toInt();
		fitting_settings_y_corner	= windows_Geometry_Settings.value( "fitting_settings_y_corner",	0	).toInt();
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Calculation_Settings_Window_Geometry );
		calculation_settings_x_corner	= windows_Geometry_Settings.value( "calculation_settings_x_corner",	0  ).toInt();
		calculation_settings_y_corner	= windows_Geometry_Settings.value( "calculation_settings_y_corner",	532).toInt();
		calculation_settings_width		= windows_Geometry_Settings.value( "calculation_settings_width",	70 ).toInt();
		calculation_settings_height		= windows_Geometry_Settings.value( "calculation_settings_height",	50 ).toInt();
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( General_Settings_Window_Geometry );
		general_settings_x_corner	= windows_Geometry_Settings.value( "general_settings_x_corner",	50 ).toInt();
		general_settings_y_corner	= windows_Geometry_Settings.value( "general_settings_y_corner",	582).toInt();
		general_settings_width		= windows_Geometry_Settings.value( "general_settings_width",	70 ).toInt();
		general_settings_height		= windows_Geometry_Settings.value( "general_settings_height",	50 ).toInt();
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Fits_Selector_Window_Geometry );
		fits_selector_settings_x_corner	= windows_Geometry_Settings.value( "fits_selector_settings_x_corner",	0  ).toInt();
		fits_selector_settings_y_corner	= windows_Geometry_Settings.value( "fits_selector_settings_y_corner",	31 ).toInt();
		fits_selector_settings_width	= windows_Geometry_Settings.value( "fits_selector_settings_width",		235).toInt();
		fits_selector_settings_height	= windows_Geometry_Settings.value( "fits_selector_settings_height",		multilayer_height ).toInt();
	windows_Geometry_Settings.endGroup();
}

void Settings::save_Windows_Geometry_Settings()
{
	QSettings windows_Geometry_Settings  (Windows_Geometry_Settings_Path, QSettings::IniFormat);

	windows_Geometry_Settings.beginGroup( Application_Style );
		windows_Geometry_Settings.setValue( "application_style", application_style );
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Multilayer_Window_Geometry );
		windows_Geometry_Settings.setValue( "multilayer_x_corner",			multilayer_x_corner  );
		windows_Geometry_Settings.setValue( "multilayer_y_corner",			multilayer_y_corner  );
		windows_Geometry_Settings.setValue( "multilayer_min_width",			multilayer_min_width );
		windows_Geometry_Settings.setValue( "multilayer_min_height",		multilayer_min_height);
		windows_Geometry_Settings.setValue( "multilayer_width",				multilayer_width	 );
		windows_Geometry_Settings.setValue( "multilayer_height",			multilayer_height	 );
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Curve_Editor_Window_Geometry );
		windows_Geometry_Settings.setValue( "independent_x_corner",		independent_x_corner);
		windows_Geometry_Settings.setValue( "independent_y_corner",		independent_y_corner);
		windows_Geometry_Settings.setValue( "target_x_corner",			target_x_corner		);
		windows_Geometry_Settings.setValue( "target_y_corner",			target_y_corner		);
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Structure_Table_Window_Geometry );
		windows_Geometry_Settings.setValue( "structure_table_x_corner",		structure_table_x_corner);
		windows_Geometry_Settings.setValue( "structure_table_y_corner",		structure_table_y_corner);
		windows_Geometry_Settings.setValue( "structure_table_width",		structure_table_width	);
		windows_Geometry_Settings.setValue( "structure_table_height",		structure_table_height	);
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Regular_Aperiodic_Table_Window_Geometry );
		windows_Geometry_Settings.setValue( "regular_aperiodic_table_x_corner",		regular_aperiodic_table_x_corner );
		windows_Geometry_Settings.setValue( "regular_aperiodic_table_y_corner",		regular_aperiodic_table_y_corner );
		windows_Geometry_Settings.setValue( "regular_aperiodic_table_width",		regular_aperiodic_table_width	 );
		windows_Geometry_Settings.setValue( "regular_aperiodic_table_height",		regular_aperiodic_table_height	 );
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Graphs_Window_Geometry );
		windows_Geometry_Settings.setValue( "graphs_x_corner_1D",		graphs_x_corner_1D	);
		windows_Geometry_Settings.setValue( "graphs_y_corner_1D",		graphs_y_corner_1D	);
		windows_Geometry_Settings.setValue( "graphs_width_1D",			graphs_width_1D		);
		windows_Geometry_Settings.setValue( "graphs_height_1D",			graphs_height_1D	);

		windows_Geometry_Settings.setValue( "graphs_x_corner_2D",		graphs_x_corner_2D	);
		windows_Geometry_Settings.setValue( "graphs_y_corner_2D",		graphs_y_corner_2D	);
		windows_Geometry_Settings.setValue( "graphs_width_2D",			graphs_width_2D		);
		windows_Geometry_Settings.setValue( "graphs_height_2D",			graphs_height_2D	);
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Profile_Plots_Window_Geometry );
		windows_Geometry_Settings.setValue( "profile_plots_x_corner",		profile_plots_x_corner	);
		windows_Geometry_Settings.setValue( "profile_plots_y_corner",		profile_plots_y_corner	);
		windows_Geometry_Settings.setValue( "profile_plots_width",			profile_plots_width		);
		windows_Geometry_Settings.setValue( "profile_plots_height",			profile_plots_height	);
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Fitting_Settings_Window_Geometry );
		windows_Geometry_Settings.setValue( "fitting_settings_x_corner",		fitting_settings_x_corner );
		windows_Geometry_Settings.setValue( "fitting_settings_y_corner",		fitting_settings_y_corner );
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Calculation_Settings_Window_Geometry );
		windows_Geometry_Settings.setValue( "calculation_settings_x_corner",	calculation_settings_x_corner	);
		windows_Geometry_Settings.setValue( "calculation_settings_y_corner",	calculation_settings_y_corner	);
		windows_Geometry_Settings.setValue( "calculation_settings_width",		calculation_settings_width		);
		windows_Geometry_Settings.setValue( "calculation_settings_height",		calculation_settings_height		);
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( General_Settings_Window_Geometry );
		windows_Geometry_Settings.setValue( "general_settings_x_corner",	general_settings_x_corner	);
		windows_Geometry_Settings.setValue( "general_settings_y_corner",	general_settings_y_corner	);
		windows_Geometry_Settings.setValue( "general_settings_width",		general_settings_width		);
		windows_Geometry_Settings.setValue( "general_settings_height",		general_settings_height		);
	windows_Geometry_Settings.endGroup();

	windows_Geometry_Settings.beginGroup( Fits_Selector_Window_Geometry );
		windows_Geometry_Settings.setValue( "fits_selector_settings_x_corner",	fits_selector_settings_x_corner );
		windows_Geometry_Settings.setValue( "fits_selector_settings_y_corner",	fits_selector_settings_y_corner );
		windows_Geometry_Settings.setValue( "fits_selector_settings_width",		fits_selector_settings_width	);
		windows_Geometry_Settings.setValue( "fits_selector_settings_height",	fits_selector_settings_height	);
	windows_Geometry_Settings.endGroup();
}

void Settings::read_Structure_Default_Values(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings structure_Default_Values(Structure_Default_Values_Path + add_reset, QSettings::IniFormat);

	default_num_confidence_points = structure_Default_Values.value( "default_num_confidence_points",	5 ).toInt();
	auto_density_from_elements = structure_Default_Values.value( "auto_density_from_elements",	true ).toBool();

	structure_Default_Values.beginGroup( Layer_Values );
		default_layer_material					= structure_Default_Values.value( "default_layer_material",					"Be"		).toString();

		default_layer_absolute_density			= structure_Default_Values.value( "default_layer_absolute_density",			2.699		).toDouble();
		default_layer_absolute_density_min		= structure_Default_Values.value( "default_layer_absolute_density_min",		2.500		).toDouble();
		default_layer_absolute_density_max		= structure_Default_Values.value( "default_layer_absolute_density_max",		2.700		).toDouble();

		default_layer_relative_density			= structure_Default_Values.value( "default_layer_relative_density",			1			).toDouble();
		default_layer_relative_density_min		= structure_Default_Values.value( "default_layer_relative_density_min",		0.7			).toDouble();
		default_layer_relative_density_max		= structure_Default_Values.value( "default_layer_relative_density_max",		1			).toDouble();

		default_layer_composed_material			= structure_Default_Values.value( "default_layer_composed_material",		false		).toBool();
		default_layer_stoichiometry_element		= structure_Default_Values.value( "default_layer_stoichiometry_element",	"Al"		).toString();

		default_layer_thickness					= structure_Default_Values.value( "default_layer_thickness",				20			).toDouble();
		default_layer_thickness_min				= structure_Default_Values.value( "default_layer_thickness_min",			10			).toDouble();
		default_layer_thickness_max				= structure_Default_Values.value( "default_layer_thickness_max",			30			).toDouble();

		default_thickness_drift_line_enabled	= structure_Default_Values.value( "default_thickness_drift_line_enabled",	false		).toBool();
		default_thickness_drift_line_show		= structure_Default_Values.value( "default_thickness_drift_line_show",		true		).toBool();
		default_thickness_drift_line			= structure_Default_Values.value( "default_thickness_drift_line",			 0			).toDouble();
		default_thickness_drift_line_min		= structure_Default_Values.value( "default_thickness_drift_line_min",		-0.01		).toDouble();
		default_thickness_drift_line_max		= structure_Default_Values.value( "default_thickness_drift_line_max",		 0.01		).toDouble();

		default_thickness_drift_rand_enabled	= structure_Default_Values.value( "default_thickness_drift_rand_enabled",	false		).toBool();
		default_thickness_drift_rand_show		= structure_Default_Values.value( "default_thickness_drift_rand_show",		false		).toBool();
		default_thickness_drift_rand			= structure_Default_Values.value( "default_thickness_drift_rand",			 0			).toDouble();
		default_thickness_drift_rand_min		= structure_Default_Values.value( "default_thickness_drift_rand_min",		-1			).toDouble();
		default_thickness_drift_rand_max		= structure_Default_Values.value( "default_thickness_drift_rand_max",		 1			).toDouble();

		default_thickness_drift_sine_enabled		= structure_Default_Values.value( "default_thickness_drift_sine_enabled",		false	).toBool();
		default_thickness_drift_sine_show			= structure_Default_Values.value( "default_thickness_drift_sine_show",			false	).toBool();
		default_thickness_drift_sine_amplitude		= structure_Default_Values.value( "default_thickness_drift_sine_amplitude",		0		).toDouble();
		default_thickness_drift_sine_amplitude_min	= structure_Default_Values.value( "default_thickness_drift_sine_amplitude_min",	0		).toDouble();
		default_thickness_drift_sine_amplitude_max	= structure_Default_Values.value( "default_thickness_drift_sine_amplitude_max",	1		).toDouble();
		default_thickness_drift_sine_frequency		= structure_Default_Values.value( "default_thickness_drift_sine_frequency",		0.33333 ).toDouble();
		default_thickness_drift_sine_frequency_min	= structure_Default_Values.value( "default_thickness_drift_sine_frequency_min",	0.01	).toDouble();
		default_thickness_drift_sine_frequency_max	= structure_Default_Values.value( "default_thickness_drift_sine_frequency_max",	1		).toDouble();
		default_thickness_drift_sine_phase			= structure_Default_Values.value( "default_thickness_drift_sine_phase",			0		).toDouble();
		default_thickness_drift_sine_phase_min		= structure_Default_Values.value( "default_thickness_drift_sine_phase_min",		0		).toDouble();
		default_thickness_drift_sine_phase_max		= structure_Default_Values.value( "default_thickness_drift_sine_phase_max",		1		).toDouble();

		default_sigma_drift_line_enabled	= structure_Default_Values.value( "default_sigma_drift_line_enabled",	false		).toBool();
		default_sigma_drift_line_show		= structure_Default_Values.value( "default_sigma_drift_line_show",		false		).toBool();
		default_sigma_drift_line			= structure_Default_Values.value( "default_sigma_drift_line",			 0			).toDouble();
		default_sigma_drift_line_min		= structure_Default_Values.value( "default_sigma_drift_line_min",		-0.01		).toDouble();
		default_sigma_drift_line_max		= structure_Default_Values.value( "default_sigma_drift_line_max",		 0.01		).toDouble();

		default_sigma_drift_rand_enabled	= structure_Default_Values.value( "default_sigma_drift_rand_enabled",	false		).toBool();
		default_sigma_drift_rand_show		= structure_Default_Values.value( "default_sigma_drift_rand_show",		false		).toBool();
		default_sigma_drift_rand			= structure_Default_Values.value( "default_sigma_drift_rand",			 0			).toDouble();
		default_sigma_drift_rand_min		= structure_Default_Values.value( "default_sigma_drift_rand_min",		-1			).toDouble();
		default_sigma_drift_rand_max		= structure_Default_Values.value( "default_sigma_drift_rand_max",		 1			).toDouble();

		default_sigma_drift_sine_enabled		= structure_Default_Values.value( "default_sigma_drift_sine_enabled",		false	).toBool();
		default_sigma_drift_sine_show			= structure_Default_Values.value( "default_sigma_drift_sine_show",			false	).toBool();
		default_sigma_drift_sine_amplitude		= structure_Default_Values.value( "default_sigma_drift_sine_amplitude",		0		).toDouble();
		default_sigma_drift_sine_amplitude_min	= structure_Default_Values.value( "default_sigma_drift_sine_amplitude_min",	0		).toDouble();
		default_sigma_drift_sine_amplitude_max	= structure_Default_Values.value( "default_sigma_drift_sine_amplitude_max",	1		).toDouble();
		default_sigma_drift_sine_frequency		= structure_Default_Values.value( "default_sigma_drift_sine_frequency",		0.33333 ).toDouble();
		default_sigma_drift_sine_frequency_min	= structure_Default_Values.value( "default_sigma_drift_sine_frequency_min",	0.01	).toDouble();
		default_sigma_drift_sine_frequency_max	= structure_Default_Values.value( "default_sigma_drift_sine_frequency_max",	1		).toDouble();
		default_sigma_drift_sine_phase			= structure_Default_Values.value( "default_sigma_drift_sine_phase",			0		).toDouble();
		default_sigma_drift_sine_phase_min		= structure_Default_Values.value( "default_sigma_drift_sine_phase_min",		0		).toDouble();
		default_sigma_drift_sine_phase_max		= structure_Default_Values.value( "default_sigma_drift_sine_phase_max",		1		).toDouble();
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Substrate_Values );
		default_substrate_material					= structure_Default_Values.value( "default_substrate_material",				   "Si"	).toString();

		default_substrate_absolute_density			= structure_Default_Values.value( "default_substrate_absolute_density",		   2.33	).toDouble();
		default_substrate_absolute_density_min		= structure_Default_Values.value( "default_substrate_absolute_density_min",	   1.90	).toDouble();
		default_substrate_absolute_density_max		= structure_Default_Values.value( "default_substrate_absolute_density_max",	   2.33	).toDouble();

		default_substrate_relative_density			= structure_Default_Values.value( "default_substrate_relative_density",		   1	).toDouble();
		default_substrate_relative_density_min		= structure_Default_Values.value( "default_substrate_relative_density_min",	   0.7	).toDouble();
		default_substrate_relative_density_max		= structure_Default_Values.value( "default_substrate_relative_density_max",	   1	).toDouble();

		default_substrate_composed_material			= structure_Default_Values.value( "default_substrate_composed_material",	   false).toBool();
		default_substrate_stoichiometry_element		= structure_Default_Values.value( "default_substrate_stoichiometry_element",   "Si" ).toString();
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Common_Values );
		default_diffuse_sigma				= structure_Default_Values.value( "default_diffuse_sigma",				0	).toDouble();
		default_diffuse_sigma_min			= structure_Default_Values.value( "default_diffuse_sigma_min",			1	).toDouble();
		default_diffuse_sigma_max			= structure_Default_Values.value( "default_diffuse_sigma_max",		   20	).toDouble();

		default_stoichiometry_composition	  = structure_Default_Values.value( "default_stoichiometry_composition",	1	).toDouble();
		default_stoichiometry_composition_min = structure_Default_Values.value( "default_stoichiometry_composition_min",0	).toDouble();
		default_stoichiometry_composition_max = structure_Default_Values.value( "default_stoichiometry_composition",	2	).toDouble();
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Roughness_Values );
		default_roughness_enabled					= structure_Default_Values.value( "default_roughness_enabled",					 false	).toBool();

		default_roughness_sigma						= structure_Default_Values.value( "default_roughness_sigma",						 0	).toDouble();
		default_roughness_sigma_min					= structure_Default_Values.value( "default_roughness_sigma_min",					 1	).toDouble();
		default_roughness_sigma_max					= structure_Default_Values.value( "default_roughness_sigma_max",					20	).toDouble();

		default_roughness_correlation_radius		= structure_Default_Values.value( "default_roughness_correlation_radius",		 10000	).toDouble();
		default_roughness_correlation_radius_min	= structure_Default_Values.value( "default_roughness_correlation_radius_min",	  1000	).toDouble();
		default_roughness_correlation_radius_max	= structure_Default_Values.value( "default_roughness_correlation_radius_max",	500000	).toDouble();

		default_roughness_fractal_alpha				= structure_Default_Values.value( "default_roughness_fractal_alpha",			   0.5	).toDouble();
		default_roughness_fractal_alpha_min			= structure_Default_Values.value( "default_roughness_fractal_alpha_min",		   0.1	).toDouble();
		default_roughness_fractal_alpha_max			= structure_Default_Values.value( "default_roughness_fractal_alpha_max",		   1.0	).toDouble();

		default_roughness_omega						= structure_Default_Values.value( "default_roughness_omega",					   100	).toDouble();
		default_roughness_omega_min					= structure_Default_Values.value( "default_roughness_omega_min",					20	).toDouble();
		default_roughness_omega_max					= structure_Default_Values.value( "default_roughness_omega_max",				  2000	).toDouble();

		default_roughness_mu						= structure_Default_Values.value( "default_roughness_mu",						   200	).toDouble();
		default_roughness_mu_min					= structure_Default_Values.value( "default_roughness_mu_min",						10	).toDouble();
		default_roughness_mu_max					= structure_Default_Values.value( "default_roughness_mu_max",					  1000	).toDouble();
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Fluctuation_Values );
		default_fluctuation_enabled							= structure_Default_Values.value( "default_fluctuation_enabled",						false ).toBool();
		default_fluctuation_used							= structure_Default_Values.value( "default_fluctuation_used",							true	 ).toBool();

		default_fluctuation_absolute_density				= structure_Default_Values.value( "default_fluctuation_absolute_density",				2.699 ).toDouble();
		default_fluctuation_absolute_density_min			= structure_Default_Values.value( "default_fluctuation_absolute_density_min",			2.500 ).toDouble();
		default_fluctuation_absolute_density_max			= structure_Default_Values.value( "default_fluctuation_absolute_density_max",			2.700 ).toDouble();

		default_fluctuation_relative_density				= structure_Default_Values.value( "default_fluctuation_relative_density",				1	  ).toDouble();
		default_fluctuation_relative_density_min			= structure_Default_Values.value( "default_fluctuation_relative_density_min",			0.7	  ).toDouble();
		default_fluctuation_relative_density_max			= structure_Default_Values.value( "default_fluctuation_relative_density_max",			1	  ).toDouble();

		default_fluctuation_particle_radius					= structure_Default_Values.value( "default_fluctuation_particle_radius",				10	  ).toDouble();
		default_fluctuation_particle_radius_min				= structure_Default_Values.value( "default_fluctuation_particle_radius_min",			 0	  ).toDouble();
		default_fluctuation_particle_radius_max				= structure_Default_Values.value( "default_fluctuation_particle_radius_max",			20	  ).toDouble();

		default_fluctuation_particle_height					= structure_Default_Values.value( "default_fluctuation_particle_height",				20	  ).toDouble();
		default_fluctuation_particle_height_min				= structure_Default_Values.value( "default_fluctuation_particle_height_min",			 0	  ).toDouble();
		default_fluctuation_particle_height_max				= structure_Default_Values.value( "default_fluctuation_particle_height_max",			20	  ).toDouble();

		default_fluctuation_particle_distance				= structure_Default_Values.value( "default_fluctuation_particle_distance",				100	  ).toDouble();
		default_fluctuation_particle_distance_min			= structure_Default_Values.value( "default_fluctuation_particle_distance_min",			10	  ).toDouble();
		default_fluctuation_particle_distance_max			= structure_Default_Values.value( "default_fluctuation_particle_distance_max",			200	  ).toDouble();

		default_fluctuation_particle_distance_deviation		= structure_Default_Values.value( "default_fluctuation_particle_distance_deviation",	1	  ).toDouble();
		default_fluctuation_particle_distance_deviation_min	= structure_Default_Values.value( "default_fluctuation_particle_distance_deviation_min",0.5	  ).toDouble();
		default_fluctuation_particle_distance_deviation_max	= structure_Default_Values.value( "default_fluctuation_particle_distance_deviation_max",100   ).toDouble();

		default_fluctuation_domain_size						= structure_Default_Values.value( "default_fluctuation_domain_size",				1e4		).toDouble();
		default_fluctuation_domain_size_min					= structure_Default_Values.value( "default_fluctuation_domain_size_min",			1E2		).toDouble();
		default_fluctuation_domain_size_max					= structure_Default_Values.value( "default_fluctuation_domain_size_max",			1E8		).toDouble();

		default_fluctuation_z_position						= structure_Default_Values.value( "default_fluctuation_z_position",					  0		).toDouble();
		default_fluctuation_z_position_min					= structure_Default_Values.value( "default_fluctuation_z_position_min",				-10		).toDouble();
		default_fluctuation_z_position_max					= structure_Default_Values.value( "default_fluctuation_z_position_max",				 10		).toDouble();

		default_fluctuation_z_position_deviation			= structure_Default_Values.value( "default_fluctuation_z_position_deviation",		  0		).toDouble();
		default_fluctuation_z_position_deviation_min		= structure_Default_Values.value( "default_fluctuation_z_position_deviation_min",	  0		).toDouble();
		default_fluctuation_z_position_deviation_max		= structure_Default_Values.value( "default_fluctuation_z_position_deviation_max",     5		).toDouble();
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Stack_Values );
		default_stack_number_of_repetition		= structure_Default_Values.value( "default_stack_number_of_repetition",		1	).toInt();
		default_stack_number_of_repetition_min	= structure_Default_Values.value( "default_stack_number_of_repetition_min",	1	).toInt();
		default_stack_number_of_repetition_max	= structure_Default_Values.value( "default_stack_number_of_repetition_max",	100	).toInt();

		default_stack_period					= structure_Default_Values.value( "default_stack_period",					40	).toDouble();
		default_stack_period_min				= structure_Default_Values.value( "default_stack_period_min",				20	).toDouble();
		default_stack_period_max				= structure_Default_Values.value( "default_stack_period_max",				60	).toDouble();

		default_stack_gamma						= structure_Default_Values.value( "default_stack_gamma",					0.5 ).toDouble();
		default_stack_gamma_min					= structure_Default_Values.value( "default_stack_gamma_min",				0   ).toDouble();
		default_stack_gamma_max					= structure_Default_Values.value( "default_stack_gamma_max",				1   ).toDouble();
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

		step_interlayer			= structure_Default_Values.value( "step_interlayer",		 0.1  ).toDouble();
		step_gamma				= structure_Default_Values.value( "step_gamma",				 0.01 ).toDouble();
		step_drift				= structure_Default_Values.value( "step_drift",				 0.001).toDouble();
		step_thickness_aperiodic= structure_Default_Values.value( "step_thickness_aperiodic",0.1  ).toDouble();
		step_sigma_aperiodic	= structure_Default_Values.value( "step_sigma_aperiodic",	 0.1  ).toDouble();
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Units );
		length_units			= structure_Default_Values.value( "length_units",	length_Units_List.first() ).toString();
	structure_Default_Values.endGroup();
}

void Settings::save_Structure_Default_Values()
{
	QSettings structure_Default_Values(Structure_Default_Values_Path, QSettings::IniFormat);

	structure_Default_Values.setValue( "default_num_confidence_points", default_num_confidence_points	);
	structure_Default_Values.setValue( "auto_density_from_elements", auto_density_from_elements	);

	structure_Default_Values.beginGroup( Layer_Values );
		structure_Default_Values.setValue( "default_layer_material",					default_layer_material						);

		structure_Default_Values.setValue( "default_layer_absolute_density",			default_layer_absolute_density				);
		structure_Default_Values.setValue( "default_layer_absolute_density_min",		default_layer_absolute_density_min			);
		structure_Default_Values.setValue( "default_layer_absolute_density_max",		default_layer_absolute_density_max			);

		structure_Default_Values.setValue( "default_layer_relative_density",			default_layer_relative_density				);
		structure_Default_Values.setValue( "default_layer_relative_density_min",		default_layer_relative_density_min			);
		structure_Default_Values.setValue( "default_layer_relative_density_max",		default_layer_relative_density_max			);

		structure_Default_Values.setValue( "default_layer_composed_material",			default_layer_composed_material				);
		structure_Default_Values.setValue( "default_layer_stoichiometry_element",		default_layer_stoichiometry_element			);

		structure_Default_Values.setValue( "default_layer_thickness",					default_layer_thickness						);
		structure_Default_Values.setValue( "default_layer_thickness_min",				default_layer_thickness_min					);
		structure_Default_Values.setValue( "default_layer_thickness_max",				default_layer_thickness_max					);

		structure_Default_Values.setValue( "default_thickness_drift_line_enabled",		default_thickness_drift_line_enabled		);
		structure_Default_Values.setValue( "default_thickness_drift_line_show",			default_thickness_drift_line_show			);
		structure_Default_Values.setValue( "default_thickness_drift_line",				default_thickness_drift_line				);
		structure_Default_Values.setValue( "default_thickness_drift_line_min",			default_thickness_drift_line_min			);
		structure_Default_Values.setValue( "default_thickness_drift_line_max",			default_thickness_drift_line_max			);

		structure_Default_Values.setValue( "default_thickness_drift_rand_enabled",		default_thickness_drift_rand_enabled		);
		structure_Default_Values.setValue( "default_thickness_drift_rand_show",			default_thickness_drift_rand_show			);
		structure_Default_Values.setValue( "default_thickness_drift_rand",				default_thickness_drift_rand				);
		structure_Default_Values.setValue( "default_thickness_drift_rand_min",			default_thickness_drift_rand_min			);
		structure_Default_Values.setValue( "default_thickness_drift_rand_max",			default_thickness_drift_rand_max			);

		structure_Default_Values.setValue( "default_thickness_drift_sine_enabled",		default_thickness_drift_sine_enabled		);
		structure_Default_Values.setValue( "default_thickness_drift_sine_show",			default_thickness_drift_sine_show			);
		structure_Default_Values.setValue( "default_thickness_drift_sine_amplitude",	default_thickness_drift_sine_amplitude		);
		structure_Default_Values.setValue( "default_thickness_drift_sine_amplitude_min",default_thickness_drift_sine_amplitude_min	);
		structure_Default_Values.setValue( "default_thickness_drift_sine_amplitude_max",default_thickness_drift_sine_amplitude_max	);
		structure_Default_Values.setValue( "default_thickness_drift_sine_frequency",	default_thickness_drift_sine_frequency		);
		structure_Default_Values.setValue( "default_thickness_drift_sine_frequency_min",default_thickness_drift_sine_frequency_min	);
		structure_Default_Values.setValue( "default_thickness_drift_sine_frequency_max",default_thickness_drift_sine_frequency_max	);
		structure_Default_Values.setValue( "default_thickness_drift_sine_phase",		default_thickness_drift_sine_phase			);
		structure_Default_Values.setValue( "default_thickness_drift_sine_phase_min",	default_thickness_drift_sine_phase_min		);
		structure_Default_Values.setValue( "default_thickness_drift_sine_phase_max",	default_thickness_drift_sine_phase_max		);

		structure_Default_Values.setValue( "default_sigma_drift_line_enabled",			default_sigma_drift_line_enabled			);
		structure_Default_Values.setValue( "default_sigma_drift_line_show",				default_sigma_drift_line_show				);
		structure_Default_Values.setValue( "default_sigma_drift_line",					default_sigma_drift_line					);
		structure_Default_Values.setValue( "default_sigma_drift_line_min",				default_sigma_drift_line_min				);
		structure_Default_Values.setValue( "default_sigma_drift_line_max",				default_sigma_drift_line_max				);

		structure_Default_Values.setValue( "default_sigma_drift_rand_enabled",			default_sigma_drift_rand_enabled			);
		structure_Default_Values.setValue( "default_sigma_drift_rand_show",				default_sigma_drift_rand_show				);
		structure_Default_Values.setValue( "default_sigma_drift_rand",					default_sigma_drift_rand					);
		structure_Default_Values.setValue( "default_sigma_drift_rand_min",				default_sigma_drift_rand_min				);
		structure_Default_Values.setValue( "default_sigma_drift_rand_max",				default_sigma_drift_rand_max				);

		structure_Default_Values.setValue( "default_sigma_drift_sine_enabled",			default_sigma_drift_sine_enabled			);
		structure_Default_Values.setValue( "default_sigma_drift_sine_show",				default_sigma_drift_sine_show				);
		structure_Default_Values.setValue( "default_sigma_drift_sine_amplitude",		default_sigma_drift_sine_amplitude			);
		structure_Default_Values.setValue( "default_sigma_drift_sine_amplitude_min",	default_sigma_drift_sine_amplitude_min		);
		structure_Default_Values.setValue( "default_sigma_drift_sine_amplitude_max",	default_sigma_drift_sine_amplitude_max		);
		structure_Default_Values.setValue( "default_sigma_drift_sine_frequency",		default_sigma_drift_sine_frequency			);
		structure_Default_Values.setValue( "default_sigma_drift_sine_frequency_min",	default_sigma_drift_sine_frequency_min		);
		structure_Default_Values.setValue( "default_sigma_drift_sine_frequency_max",	default_sigma_drift_sine_frequency_max		);
		structure_Default_Values.setValue( "default_sigma_drift_sine_phase",			default_sigma_drift_sine_phase				);
		structure_Default_Values.setValue( "default_sigma_drift_sine_phase_min",		default_sigma_drift_sine_phase_min			);
		structure_Default_Values.setValue( "default_sigma_drift_sine_phase_max",		default_sigma_drift_sine_phase_max			);
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Substrate_Values );
		structure_Default_Values.setValue( "default_substrate_material",					default_substrate_material					);

		structure_Default_Values.setValue( "default_substrate_absolute_density",			default_substrate_absolute_density			);
		structure_Default_Values.setValue( "default_substrate_absolute_density_min",		default_substrate_absolute_density_min		);
		structure_Default_Values.setValue( "default_substrate_absolute_density_max",		default_substrate_absolute_density_max		);

		structure_Default_Values.setValue( "default_substrate_relative_density",			default_substrate_relative_density			);
		structure_Default_Values.setValue( "default_substrate_relative_density_min",		default_substrate_relative_density_min		);
		structure_Default_Values.setValue( "default_substrate_relative_density_max",		default_substrate_relative_density_max		);

		structure_Default_Values.setValue( "default_substrate_composed_material",			default_substrate_composed_material			);
		structure_Default_Values.setValue( "default_substrate_stoichiometry_element",		default_substrate_stoichiometry_element		);
	structure_Default_Values.endGroup();
	structure_Default_Values.beginGroup( Common_Values );
		structure_Default_Values.setValue( "default_diffuse_sigma",							default_diffuse_sigma				);
		structure_Default_Values.setValue( "default_diffuse_sigma_min",						default_diffuse_sigma_min			);
		structure_Default_Values.setValue( "default_diffuse_sigma_max",						default_diffuse_sigma_max			);

		structure_Default_Values.setValue( "default_stoichiometry_composition",				default_stoichiometry_composition		);
		structure_Default_Values.setValue( "default_stoichiometry_composition_min",			default_stoichiometry_composition_min	);
		structure_Default_Values.setValue( "default_stoichiometry_composition_max",			default_stoichiometry_composition_max	);
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Roughness_Values );
		structure_Default_Values.setValue( "default_roughness_enabled",					default_roughness_enabled				);

		structure_Default_Values.setValue( "default_roughness_sigma",					default_roughness_sigma					);
		structure_Default_Values.setValue( "default_roughness_sigma_min",				default_roughness_sigma_min				);
		structure_Default_Values.setValue( "default_roughness_sigma_max",				default_roughness_sigma_max				);

		structure_Default_Values.setValue( "default_roughness_correlation_radius",		default_roughness_correlation_radius	);
		structure_Default_Values.setValue( "default_roughness_correlation_radius_min",	default_roughness_correlation_radius_min);
		structure_Default_Values.setValue( "default_roughness_correlation_radius_max",	default_roughness_correlation_radius_max);

		structure_Default_Values.setValue( "default_roughness_fractal_alpha",			default_roughness_fractal_alpha			);
		structure_Default_Values.setValue( "default_roughness_fractal_alpha_min",		default_roughness_fractal_alpha_min		);
		structure_Default_Values.setValue( "default_roughness_fractal_alpha_max",		default_roughness_fractal_alpha_max		);

		structure_Default_Values.setValue( "default_roughness_omega",					default_roughness_omega					);
		structure_Default_Values.setValue( "default_roughness_omega_min",				default_roughness_omega_min				);
		structure_Default_Values.setValue( "default_roughness_omega_max",				default_roughness_omega_max				);

		structure_Default_Values.setValue( "default_roughness_mu",						default_roughness_mu					);
		structure_Default_Values.setValue( "default_roughness_mu_min",					default_roughness_mu_min				);
		structure_Default_Values.setValue( "default_roughness_mu_max",					default_roughness_mu_max				);
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Fluctuation_Values );
		structure_Default_Values.setValue( "default_fluctuation_enabled",							default_fluctuation_enabled				);
		structure_Default_Values.setValue( "default_fluctuation_used",								default_fluctuation_used				);

		structure_Default_Values.setValue( "default_fluctuation_absolute_density",					default_fluctuation_absolute_density	);
		structure_Default_Values.setValue( "default_fluctuation_absolute_density_min",				default_fluctuation_absolute_density_min);
		structure_Default_Values.setValue( "default_fluctuation_absolute_density_max",				default_fluctuation_absolute_density_max);

		structure_Default_Values.setValue( "default_fluctuation_relative_density",					default_fluctuation_relative_density	);
		structure_Default_Values.setValue( "default_fluctuation_relative_density_min",				default_fluctuation_relative_density_min);
		structure_Default_Values.setValue( "default_fluctuation_relative_density_max",				default_fluctuation_relative_density_max);

		structure_Default_Values.setValue( "default_fluctuation_particle_radius",					default_fluctuation_particle_radius		);
		structure_Default_Values.setValue( "default_fluctuation_particle_radius_min",				default_fluctuation_particle_radius_min	);
		structure_Default_Values.setValue( "default_fluctuation_particle_radius_max",				default_fluctuation_particle_radius_max	);

		structure_Default_Values.setValue( "default_fluctuation_particle_height",					default_fluctuation_particle_height		);
		structure_Default_Values.setValue( "default_fluctuation_particle_height_min",				default_fluctuation_particle_height_min	);
		structure_Default_Values.setValue( "default_fluctuation_particle_height_max",				default_fluctuation_particle_height_max	);

		structure_Default_Values.setValue( "default_fluctuation_particle_distance",					default_fluctuation_particle_distance		);
		structure_Default_Values.setValue( "default_fluctuation_particle_distance_min",				default_fluctuation_particle_distance_min	);
		structure_Default_Values.setValue( "default_fluctuation_particle_distance_max",				default_fluctuation_particle_distance_max	);

		structure_Default_Values.setValue( "default_fluctuation_particle_distance_deviation",		default_fluctuation_particle_distance_deviation		);
		structure_Default_Values.setValue( "default_fluctuation_particle_distance_deviation_min",	default_fluctuation_particle_distance_deviation_min	);
		structure_Default_Values.setValue( "default_fluctuation_particle_distance_deviation_max",	default_fluctuation_particle_distance_deviation_max	);

		structure_Default_Values.setValue( "default_fluctuation_domain_size",						default_fluctuation_domain_size					);
		structure_Default_Values.setValue( "default_fluctuation_domain_size_min",					default_fluctuation_domain_size_min				);
		structure_Default_Values.setValue( "default_fluctuation_domain_size_max",					default_fluctuation_domain_size_max				);

		structure_Default_Values.setValue( "default_fluctuation_z_position",						default_fluctuation_z_position					);
		structure_Default_Values.setValue( "default_fluctuation_z_position_min",					default_fluctuation_z_position_min				);
		structure_Default_Values.setValue( "default_fluctuation_z_position_max",					default_fluctuation_z_position_max				);

		structure_Default_Values.setValue( "default_fluctuation_z_position_deviation",				default_fluctuation_z_position_deviation		);
		structure_Default_Values.setValue( "default_fluctuation_z_position_deviation_min",			default_fluctuation_z_position_deviation_min	);
		structure_Default_Values.setValue( "default_fluctuation_z_position_deviation_max",			default_fluctuation_z_position_deviation_max	);
	structure_Default_Values.endGroup();

	structure_Default_Values.beginGroup( Stack_Values );
		structure_Default_Values.setValue( "default_stack_number_of_repetition",		default_stack_number_of_repetition		);
		structure_Default_Values.setValue( "default_stack_number_of_repetition_min",	default_stack_number_of_repetition_min	);
		structure_Default_Values.setValue( "default_stack_number_of_repetition_max",	default_stack_number_of_repetition_max	);

		structure_Default_Values.setValue( "default_stack_period",						default_stack_period					);
		structure_Default_Values.setValue( "default_stack_period_min",					default_stack_period_min				);
		structure_Default_Values.setValue( "default_stack_period_max",					default_stack_period_max				);

		structure_Default_Values.setValue( "default_stack_gamma",						default_stack_gamma						);
		structure_Default_Values.setValue( "default_stack_gamma_min",					default_stack_gamma_min					);
		structure_Default_Values.setValue( "default_stack_gamma_max",					default_stack_gamma_max					);
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

	structure_Default_Values.beginGroup( Units );
		structure_Default_Values.setValue( "length_units",		length_units	);
	structure_Default_Values.endGroup();
}

void Settings::read_Precisions(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings precision_Values(Precisions_Path + add_reset, QSettings::IniFormat);

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
}

void Settings::save_Precisions()
{
	QSettings precision_Values(Precisions_Path, QSettings::IniFormat);

	precision_Values.beginGroup( Line_Edit );
		precision_Values.setValue( "line_edit_double_format",			line_edit_double_format				);
		precision_Values.setValue( "line_edit_short_double_format",		line_edit_short_double_format		);
		precision_Values.setValue( "line_edit_angle_precision",			line_edit_angle_precision			);
		precision_Values.setValue( "line_edit_wavelength_precision",	line_edit_wavelength_precision		);
		precision_Values.setValue( "line_edit_density_precision",		line_edit_density_precision			);
		precision_Values.setValue( "line_edit_composition_precision",	line_edit_composition_precision		);
		precision_Values.setValue( "line_edit_thickness_precision",		line_edit_thickness_precision		);

		precision_Values.setValue( "line_edit_sigma_precision",			line_edit_sigma_precision			);
		precision_Values.setValue( "line_edit_cor_radius_precision",	line_edit_cor_radius_precision		);
		precision_Values.setValue( "line_edit_fractal_alpha_precision",	line_edit_fractal_alpha_precision	);
		precision_Values.setValue( "line_edit_omega_precision",			line_edit_omega_precision			);
		precision_Values.setValue( "line_edit_mu_precision",			line_edit_mu_precision				);

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
		precision_Values.setValue( "thumbnail_composition_precision",	thumbnail_composition_precision		);
		precision_Values.setValue( "thumbnail_thickness_precision",		thumbnail_thickness_precision		);

		precision_Values.setValue( "thumbnail_sigma_precision",			thumbnail_sigma_precision			);
		precision_Values.setValue( "thumbnail_cor_radius_precision",	thumbnail_cor_radius_precision		);
		precision_Values.setValue( "thumbnail_fractal_alpha_precision",	thumbnail_fractal_alpha_precision	);
		precision_Values.setValue( "thumbnail_omega_precision",			thumbnail_omega_precision			);
		precision_Values.setValue( "thumbnail_mu_precision",			thumbnail_mu_precision				);

		precision_Values.setValue( "thumbnail_interlayer_precision",	thumbnail_interlayer_precision		);
		precision_Values.setValue( "thumbnail_drift_precision",			thumbnail_drift_precision			);
		precision_Values.setValue( "thumbnail_period_precision",		thumbnail_period_precision			);
		precision_Values.setValue( "thumbnail_gamma_precision",			thumbnail_gamma_precision			);
	precision_Values.endGroup();

	precision_Values.beginGroup( Other );
		precision_Values.setValue( "at_weight_precision",				at_weight_precision					);
		precision_Values.setValue( "thickness_transfer_precision",		thickness_transfer_precision		);
	precision_Values.endGroup();
}

void Settings::read_Measurement_Default_Values(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings measurement_Default_Values(Measurement_Default_Values_Path + add_reset, QSettings::IniFormat);

	// parameters default values
	measurement_Default_Values.beginGroup( Independent_Values );
		default_wavelength_num_points						= measurement_Default_Values.value( "default_wavelength_num_points",						400		).toInt();
		default_beam_theta_0_angle_num_points				= measurement_Default_Values.value( "default_beam_theta_0_angle_num_points",				1000	).toInt();
		default_detector_theta_angle_num_points				= measurement_Default_Values.value( "default_detector_theta_angle_num_points",				1000	).toInt();
		default_phi_angle_num_points						= measurement_Default_Values.value( "default_phi_angle_num_points",							501		).toInt();

		default_beam_theta_0_angle_specular_position		= measurement_Default_Values.value( "default_beam_theta_0_angle_specular_position",			1		).toDouble();
		default_detector_theta_angle_offset					= measurement_Default_Values.value( "default_detector_theta_angle_offset",					0.5		).toDouble();

		default_wavelength									= measurement_Default_Values.value( "default_wavelength",									1.540562).toDouble();
		default_wavelength_min								= measurement_Default_Values.value( "default_wavelength_min",								100		).toDouble();
		default_wavelength_max								= measurement_Default_Values.value( "default_wavelength_max",								200		).toDouble();

		default_beam_theta_0_angle							= measurement_Default_Values.value( "default_beam_theta_0_angle",							0.5		).toDouble();
		default_beam_theta_0_angle_min						= measurement_Default_Values.value( "default_beam_theta_0_angle_min",						0		).toDouble();
		default_beam_theta_0_angle_max						= measurement_Default_Values.value( "default_beam_theta_0_angle_max",						7		).toDouble();

		default_detector_theta_angle						= measurement_Default_Values.value( "default_detector_theta_angle",							1		).toDouble();
		default_detector_theta_angle_min					= measurement_Default_Values.value( "default_detector_theta_angle_min",						0.01	).toDouble();
		default_detector_theta_angle_max					= measurement_Default_Values.value( "default_detector_theta_angle_max",						6		).toDouble();

		default_phi_angle									= measurement_Default_Values.value( "default_phi_angle",									0		).toDouble();
		default_phi_angle_min								= measurement_Default_Values.value( "default_phi_angle_min",							   -1.3		).toDouble();
		default_phi_angle_max								= measurement_Default_Values.value( "default_phi_angle_max",								1.3		).toDouble();

		default_logarithmic_threshold_beta					= measurement_Default_Values.value( "default_logarithmic_threshold_beta",					1E-17	).toDouble();
		default_logarithmic_threshold_material_density		= measurement_Default_Values.value( "default_logarithmic_threshold_material_density",		1E-7	).toDouble();
		default_logarithmic_threshold_element_concentration = measurement_Default_Values.value( "default_logarithmic_threshold_element_concentration",	1E15	).toDouble();


		default_spectral_distribution_FWHM					= measurement_Default_Values.value( "default_spectral_distribution_FWHM",					0	).toDouble();
		default_spectral_distribution_coverage				= measurement_Default_Values.value( "default_spectral_distribution_coverage",				2	).toInt();
		default_spectral_distribution_use_sampling			= measurement_Default_Values.value( "default_spectral_distribution_use_sampling",		false	).toBool();
		default_spectral_distribution_number_of_Samples		= measurement_Default_Values.value( "default_spectral_distribution_number_of_Samples",		5	).toInt();

		default_beam_theta_0_distribution_FWHM				= measurement_Default_Values.value( "default_beam_theta_0_distribution_FWHM",				0	).toDouble();
		default_beam_theta_0_distribution_coverage			= measurement_Default_Values.value( "default_beam_theta_0_distribution_coverage",			2	).toInt();
		default_beam_theta_0_distribution_use_sampling		= measurement_Default_Values.value( "default_beam_theta_0_distribution_use_sampling",	false	).toBool();
		default_beam_theta_0_distribution_number_of_Samples = measurement_Default_Values.value( "default_beam_theta_0_distribution_number_of_Samples",	5	).toInt();

		default_beam_phi_0_distribution_FWHM				= measurement_Default_Values.value( "default_beam_phi_0_distribution_FWHM",					0	).toDouble();
		default_beam_phi_0_distribution_coverage			= measurement_Default_Values.value( "default_beam_phi_0_distribution_coverage",				2	).toInt();
		default_beam_phi_0_distribution_use_sampling		= measurement_Default_Values.value( "default_beam_phi_0_distribution_use_sampling",		false	).toBool();
		default_beam_phi_0_distribution_number_of_Samples	= measurement_Default_Values.value( "default_beam_phi_0_distribution_number_of_Samples",	5	).toInt();

		default_detector_1D_slit_width						= measurement_Default_Values.value( "default_detector_1D_slit_width",					0.8		).toDouble();
		default_detector_1D_distance_to_sample				= measurement_Default_Values.value( "default_detector_1D_distance_to_sample",			300		).toDouble();

		default_detector_1D_theta_resolution_FWHM			= measurement_Default_Values.value( "default_detector_1D_theta_resolution_FWHM",		0		).toDouble();
		default_detector_2D_theta_resolution_FWHM			= measurement_Default_Values.value( "default_detector_2D_theta_resolution_FWHM",		0		).toDouble();
		default_detector_2D_phi_resolution_FWHM				= measurement_Default_Values.value( "default_detector_2D_phi_resolution_FWHM",			0		).toDouble();

		default_beam_geometry_size							= measurement_Default_Values.value( "default_beam_geometry_size",						0.050	).toDouble();
		default_beam_geometry_smoothing						= measurement_Default_Values.value( "default_beam_geometry_smoothing",					0.5		).toDouble();
		default_beam_geometry_wings_full_width				= measurement_Default_Values.value( "default_beam_geometry_wings_full_width",			0.25	).toDouble();
		default_beam_geometry_wings_intensity				= measurement_Default_Values.value( "default_beam_geometry_wings_intensity",			0.0		).toDouble();
		default_beam_geometry_lateral_width					= measurement_Default_Values.value( "default_beam_geometry_lateral_width",				5.0		).toDouble();
		default_beam_geometry_log_profile_plot				= measurement_Default_Values.value( "default_beam_geometry_log_profile_plot",			false	).toBool();

		default_sample_geometry_size						= measurement_Default_Values.value( "default_sample_geometry_size",						20		).toDouble();
		default_sample_geometry_x_position					= measurement_Default_Values.value( "default_sample_geometry_x_position",				0		).toDouble();
		default_sample_geometry_z_position					= measurement_Default_Values.value( "default_sample_geometry_z_position",				0		).toDouble();
		default_sample_geometry_curvature					= measurement_Default_Values.value( "default_sample_geometry_curvature",				0		).toDouble();

		default_polarization								= measurement_Default_Values.value( "default_polarization",								1		).toDouble();
		default_background									= measurement_Default_Values.value( "default_background",								0		).toDouble();

	measurement_Default_Values.endGroup();
}

void Settings::save_Measurement_Default_Values()
{
	QSettings measurement_Default_Values(Measurement_Default_Values_Path, QSettings::IniFormat);

	// parameters default values
	measurement_Default_Values.beginGroup( Independent_Values );
		measurement_Default_Values.setValue( "default_wavelength_num_points",						default_wavelength_num_points						);
		measurement_Default_Values.setValue( "default_beam_theta_0_angle_num_points",				default_beam_theta_0_angle_num_points				);
		measurement_Default_Values.setValue( "default_detector_theta_angle_num_points",				default_detector_theta_angle_num_points				);
		measurement_Default_Values.setValue( "default_phi_angle_num_points",							default_phi_angle_num_points						);

		measurement_Default_Values.setValue( "default_beam_theta_0_angle_specular_position",			default_beam_theta_0_angle_specular_position		);
		measurement_Default_Values.setValue( "default_detector_theta_angle_offset",					default_detector_theta_angle_offset					);

		measurement_Default_Values.setValue( "default_wavelength",									default_wavelength									);
		measurement_Default_Values.setValue( "default_wavelength_min",								default_wavelength_min								);
		measurement_Default_Values.setValue( "default_wavelength_max",								default_wavelength_max								);

		measurement_Default_Values.setValue( "default_beam_theta_0_angle",							default_beam_theta_0_angle							);
		measurement_Default_Values.setValue( "default_beam_theta_0_angle_min",						default_beam_theta_0_angle_min						);
		measurement_Default_Values.setValue( "default_beam_theta_0_angle_max",						default_beam_theta_0_angle_max						);

		measurement_Default_Values.setValue( "default_detector_theta_angle",							default_detector_theta_angle						);
		measurement_Default_Values.setValue( "default_detector_theta_angle_min",						default_detector_theta_angle_min					);
		measurement_Default_Values.setValue( "default_detector_theta_angle_max",						default_detector_theta_angle_max					);

		measurement_Default_Values.setValue( "default_phi_angle",									default_phi_angle									);
		measurement_Default_Values.setValue( "default_phi_angle_min",								default_phi_angle_min								);
		measurement_Default_Values.setValue( "default_phi_angle_max",								default_phi_angle_max								);

		measurement_Default_Values.setValue( "default_logarithmic_threshold_beta",					default_logarithmic_threshold_beta					);
		measurement_Default_Values.setValue( "default_logarithmic_threshold_material_density",		default_logarithmic_threshold_material_density		);
		measurement_Default_Values.setValue( "default_logarithmic_threshold_element_concentration",	default_logarithmic_threshold_element_concentration	);


		measurement_Default_Values.setValue( "default_spectral_distribution_FWHM",					default_spectral_distribution_FWHM					);
		measurement_Default_Values.setValue( "default_spectral_distribution_coverage",				default_spectral_distribution_coverage				);
		measurement_Default_Values.setValue( "default_spectral_distribution_use_sampling",			default_spectral_distribution_use_sampling			);
		measurement_Default_Values.setValue( "default_spectral_distribution_number_of_Samples",		default_spectral_distribution_number_of_Samples		);

		measurement_Default_Values.setValue( "default_beam_theta_0_distribution_FWHM",				default_beam_theta_0_distribution_FWHM				);
		measurement_Default_Values.setValue( "default_beam_theta_0_distribution_coverage",			default_beam_theta_0_distribution_coverage			);
		measurement_Default_Values.setValue( "default_beam_theta_0_distribution_use_sampling",		default_beam_theta_0_distribution_use_sampling		);
		measurement_Default_Values.setValue( "default_beam_theta_0_distribution_number_of_Samples",	default_beam_theta_0_distribution_number_of_Samples	);

		measurement_Default_Values.setValue( "default_beam_phi_0_distribution_FWHM",					default_beam_phi_0_distribution_FWHM				);
		measurement_Default_Values.setValue( "default_beam_phi_0_distribution_coverage",				default_beam_phi_0_distribution_coverage			);
		measurement_Default_Values.setValue( "default_beam_phi_0_distribution_use_sampling",			default_beam_phi_0_distribution_use_sampling		);
		measurement_Default_Values.setValue( "default_beam_phi_0_distribution_number_of_Samples",	default_beam_phi_0_distribution_number_of_Samples	);

		measurement_Default_Values.setValue( "default_detector_1D_slit_width",						default_detector_1D_slit_width						);
		measurement_Default_Values.setValue( "default_detector_1D_distance_to_sample",				default_detector_1D_distance_to_sample				);

		measurement_Default_Values.setValue( "default_detector_1D_theta_resolution_FWHM",			default_detector_1D_theta_resolution_FWHM			);
		measurement_Default_Values.setValue( "default_detector_2D_theta_resolution_FWHM",			default_detector_2D_theta_resolution_FWHM			);
		measurement_Default_Values.setValue( "default_detector_2D_phi_resolution_FWHM",				default_detector_2D_phi_resolution_FWHM				);

		measurement_Default_Values.setValue( "default_beam_geometry_size",							default_beam_geometry_size							);
		measurement_Default_Values.setValue( "default_beam_geometry_smoothing",						default_beam_geometry_smoothing						);
		measurement_Default_Values.setValue( "default_beam_geometry_wings_full_width",				default_beam_geometry_wings_full_width				);
		measurement_Default_Values.setValue( "default_beam_geometry_wings_intensity",				default_beam_geometry_wings_intensity				);
		measurement_Default_Values.setValue( "default_beam_geometry_lateral_width",					default_beam_geometry_lateral_width					);
		measurement_Default_Values.setValue( "default_beam_geometry_log_profile_plot",				default_beam_geometry_log_profile_plot				);

		measurement_Default_Values.setValue( "default_sample_geometry_size",							default_sample_geometry_size						);
		measurement_Default_Values.setValue( "default_sample_geometry_x_position",					default_sample_geometry_x_position					);
		measurement_Default_Values.setValue( "default_sample_geometry_z_position",					default_sample_geometry_z_position					);
		measurement_Default_Values.setValue( "default_sample_geometry_curvature",					default_sample_geometry_curvature					);

		measurement_Default_Values.setValue( "default_polarization",									default_polarization								);
		measurement_Default_Values.setValue( "default_background",									default_background									);
	measurement_Default_Values.endGroup();
}

void Settings::read_General_Settings(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings general_Settings(General_Settings_Path + add_reset, QSettings::IniFormat);

	general_Settings_Tab_Index = general_Settings.value( "general_Settings_Tab_Index",	0 ).toInt();

	general_Settings.beginGroup( Output_Tab );
		print_1D_Data_On_Recalculation	= general_Settings.value( "print_1D_Data_On_Recalculation",		true	).toBool();
		print_2D_Data_On_Recalculation	= general_Settings.value( "print_2D_Data_On_Recalculation",		false	).toBool();

		use_multifitting_directory		= general_Settings.value( "use_multifitting_directory",			true	).toBool();
		use_working_directory			= general_Settings.value( "use_working_directory",				false	).toBool();
		working_directory				= general_Settings.value( "working_directory",		QDir::currentPath() ).toString();
		use_last_directory				= general_Settings.value( "use_last_directory",					false	).toBool();
		last_directory					= general_Settings.value( "last_directory",			QDir::currentPath() ).toString();
		last_data_directory				= general_Settings.value( "last_data_directory",	QDir::currentPath() ).toString();
		last_file						= general_Settings.value( "last_file",				default_File		).toString();
	general_Settings.endGroup();

	general_Settings.beginGroup( Calculation_Tab );
		reflectivity_calc_threads			= general_Settings.value( "reflectivity_calc_threads",		max(QThread::idealThreadCount(),1) ).toInt();
		optical_constants_read_threads		= general_Settings.value( "optical_constants_read_threads",	max(QThread::idealThreadCount(),1) ).toInt();

		recalculate_spinbox_global			= general_Settings.value( "recalculate_spinbox_global",				true	).toBool();

		recalculate_spinbox_structure_table	= general_Settings.value( "recalculate_spinbox_structure_table",	true	).toBool();
		mouse_wheel_spinbox_structure_table	= general_Settings.value( "mouse_wheel_spinbox_structure_table",	true	).toBool();
		refill_dependent_structure_table	= general_Settings.value( "refill_dependent_structure_table",		true	).toBool();

		n_max_series						= general_Settings.value( "n_max_series",							5		).toInt();
		use_beam_spot_in_specular_peak		= general_Settings.value( "use_beam_spot_in_specular_peak",			true	).toInt();
	general_Settings.endGroup();

	general_Settings.beginGroup( Interface_Tab );
		default_multilayer_tab_name			= general_Settings.value( "default_multilayer_tab_name",			"Struct_" ).toString();
		default_independent_curve_tab_name	= general_Settings.value( "default_independent_curve_tab_name",		"Plot_"   ).toString();

		tab_synchronization					= general_Settings.value( "tab_synchronization",					true    ).toBool();
		calculation_time					= general_Settings.value( "calculation_time",						true    ).toBool();
		show_residuals						= general_Settings.value( "show_residuals",							true    ).toBool();
		replot_graphs_during_fitting_1D		= general_Settings.value( "replot_graphs_during_fitting_1D",		true	).toBool();
//		replot_graphs_during_fitting_2D		= general_Settings.value( "replot_graphs_during_fitting_2D",		false	).toBool();
	general_Settings.endGroup();

	// limit max number of threads
	reflectivity_calc_threads = min(QThread::idealThreadCount(),reflectivity_calc_threads);
}

void Settings::save_General_Settings()
{
	QSettings general_Settings(General_Settings_Path, QSettings::IniFormat);

	general_Settings.setValue( "general_Settings_Tab_Index", general_Settings_Tab_Index );

	general_Settings.beginGroup( Output_Tab );
		general_Settings.setValue("print_1D_Data_On_Recalculation",		print_1D_Data_On_Recalculation		);
		general_Settings.setValue("print_2D_Data_On_Recalculation",		print_2D_Data_On_Recalculation		);

		general_Settings.setValue("use_multifitting_directory",			use_multifitting_directory			);
		general_Settings.setValue("use_working_directory",				use_working_directory				);
		general_Settings.setValue("working_directory",					working_directory					);

		general_Settings.setValue("use_last_directory",					use_last_directory					);

		general_Settings.setValue("last_directory",						last_directory						);
		general_Settings.setValue("last_data_directory",				last_data_directory					);
		general_Settings.setValue("last_file",							last_file							);
	general_Settings.endGroup();

	general_Settings.beginGroup( Calculation_Tab );
		general_Settings.setValue("reflectivity_calc_threads",			reflectivity_calc_threads			);
		general_Settings.setValue("optical_constants_read_threads",		optical_constants_read_threads		);

		general_Settings.setValue("recalculate_spinbox_global",			recalculate_spinbox_global			);

		general_Settings.setValue("recalculate_spinbox_structure_table",recalculate_spinbox_structure_table	);
		general_Settings.setValue("mouse_wheel_spinbox_structure_table",mouse_wheel_spinbox_structure_table	);
		general_Settings.setValue("refill_dependent_structure_table",	refill_dependent_structure_table	);

		general_Settings.setValue("n_max_series",						n_max_series						);
		general_Settings.setValue("use_beam_spot_in_specular_peak",		use_beam_spot_in_specular_peak		);
	general_Settings.endGroup();

	general_Settings.beginGroup( Interface_Tab );
		general_Settings.setValue( "default_multilayer_tab_name",			default_multilayer_tab_name			);
		general_Settings.setValue( "default_independent_curve_tab_name",	default_independent_curve_tab_name  );

		general_Settings.setValue( "tab_synchronization",				    tab_synchronization					);
		general_Settings.setValue( "calculation_time",						calculation_time					);
		general_Settings.setValue( "individual_residuals",				    show_residuals						);
		general_Settings.setValue("replot_graphs_during_fitting_1D",		replot_graphs_during_fitting_1D		);
//		general_Settings.setValue("replot_graphs_during_fitting_2D",		replot_graphs_during_fitting_2D		);
	general_Settings.endGroup();
}

void Settings::read_Regular_Aperiodic(bool reset_to_default)
{
	QString add_reset;
	if(reset_to_default) add_reset = "wrong_path";

	QSettings regular_Aperiodic(Regular_Aperiodic_Path + add_reset, QSettings::IniFormat);

	recalculate_spinbox_aperiodic_table = regular_Aperiodic.value( "recalculate_spinbox_aperiodic_table",	true	).toBool();
	mouse_wheel_spinbox_aperiodic_table = regular_Aperiodic.value( "mouse_wheel_spinbox_aperiodic_table",	true	).toBool();

	aperiodic_default_sigma_import		= regular_Aperiodic.value( "aperiodic_default_sigma_import",		false	  ).toBool();
	aperiodic_default_density_import	= regular_Aperiodic.value( "aperiodic_default_density_import",		false	  ).toBool();
	aperiodic_default_units_import		= regular_Aperiodic.value( "aperiodic_default_units_import",	length_Units_List.first() ).toString();
}

void Settings::save_Regular_Aperiodic()
{
	QSettings regular_Aperiodic(Regular_Aperiodic_Path, QSettings::IniFormat);

	regular_Aperiodic.setValue("recalculate_spinbox_aperiodic_table",	recalculate_spinbox_aperiodic_table );
	regular_Aperiodic.setValue("mouse_wheel_spinbox_aperiodic_table",	mouse_wheel_spinbox_aperiodic_table );

	regular_Aperiodic.setValue("aperiodic_default_sigma_import",		aperiodic_default_sigma_import		);
	regular_Aperiodic.setValue("aperiodic_default_density_import",		aperiodic_default_density_import	);
	regular_Aperiodic.setValue("aperiodic_default_units_import",		aperiodic_default_units_import		);
}

// -----------------------------------------------------------------------------------------

void Settings::read_All_Settings(bool reset_to_default)
{
	read_Windows_Geometry_Settings(reset_to_default);
	read_Structure_Default_Values(reset_to_default);
	read_Precisions(reset_to_default);
	read_Measurement_Default_Values(reset_to_default);
	read_General_Settings(reset_to_default);
	read_Regular_Aperiodic(reset_to_default);
}

void Settings::save_All_Settings()
{
	save_Windows_Geometry_Settings();
	save_Structure_Default_Values();
	save_Precisions();
	save_Measurement_Default_Values();
	save_General_Settings();
	save_Regular_Aperiodic();
}
