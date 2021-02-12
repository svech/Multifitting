#ifndef SETTINGS_H
#define SETTINGS_H

#include "global_definitions.h"
#include "global_variables.h"

// -----------------------------------------------------------------------------------------

///====================================================================
/// windows geometry
///====================================================================

// application style
extern QString application_style;

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
extern int structure_table_width_of_empty_columns;

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

///====================================================================
/// aperiodic
///====================================================================

extern bool recalculate_spinbox_aperiodic_table;
extern bool mouse_wheel_spinbox_aperiodic_table;

extern bool aperiodic_default_sigma_import;
extern bool aperiodic_default_density_import;
extern QString aperiodic_default_units_import;

///====================================================================
/// structure default values
///====================================================================

extern double default_num_confidence_points;
extern bool auto_density_from_elements;

// Layer_Values
extern QString	default_layer_material;

extern double	default_layer_absolute_density;
extern double	default_layer_absolute_density_min;
extern double	default_layer_absolute_density_max;

extern double	default_layer_relative_density;
extern double	default_layer_relative_density_min;
extern double	default_layer_relative_density_max;

extern bool		default_layer_composed_material;
extern QString	default_layer_stoichiometry_element;

extern double	default_layer_thickness;
extern double	default_layer_thickness_min;
extern double	default_layer_thickness_max;


extern bool		default_thickness_drift_line_enabled;
extern bool		default_thickness_drift_line_show;
extern double	default_thickness_drift_line;
extern double	default_thickness_drift_line_min;
extern double	default_thickness_drift_line_max;

extern bool		default_thickness_drift_rand_enabled;
extern bool		default_thickness_drift_rand_show;
extern double	default_thickness_drift_rand;
extern double	default_thickness_drift_rand_min;
extern double	default_thickness_drift_rand_max;

extern bool		default_thickness_drift_sine_enabled;
extern bool		default_thickness_drift_sine_show;
extern double	default_thickness_drift_sine_amplitude;
extern double	default_thickness_drift_sine_amplitude_min;
extern double	default_thickness_drift_sine_amplitude_max;
extern double	default_thickness_drift_sine_frequency;
extern double	default_thickness_drift_sine_frequency_min;
extern double	default_thickness_drift_sine_frequency_max;
extern double	default_thickness_drift_sine_phase;
extern double	default_thickness_drift_sine_phase_min;
extern double	default_thickness_drift_sine_phase_max;

extern bool		default_sigma_drift_line_enabled;
extern bool		default_sigma_drift_line_show;
extern double	default_sigma_drift_line;
extern double	default_sigma_drift_line_min;
extern double	default_sigma_drift_line_max;

extern bool		default_sigma_drift_rand_enabled;
extern bool		default_sigma_drift_rand_show;
extern double	default_sigma_drift_rand;
extern double	default_sigma_drift_rand_min;
extern double	default_sigma_drift_rand_max;

extern bool		default_sigma_drift_sine_enabled;
extern bool		default_sigma_drift_sine_show;
extern double	default_sigma_drift_sine_amplitude;
extern double	default_sigma_drift_sine_amplitude_min;
extern double	default_sigma_drift_sine_amplitude_max;
extern double	default_sigma_drift_sine_frequency;
extern double	default_sigma_drift_sine_frequency_min;
extern double	default_sigma_drift_sine_frequency_max;
extern double	default_sigma_drift_sine_phase;
extern double	default_sigma_drift_sine_phase_min;
extern double	default_sigma_drift_sine_phase_max;

// Substrate_Values
extern QString	default_substrate_material;

extern double	default_substrate_absolute_density;
extern double	default_substrate_absolute_density_min;
extern double	default_substrate_absolute_density_max;

extern double	default_substrate_relative_density;
extern double	default_substrate_relative_density_min;
extern double	default_substrate_relative_density_max;

extern bool		default_substrate_composed_material;
extern QString	default_substrate_stoichiometry_element;

// Common_Values
extern double	default_diffuse_sigma;
extern double	default_diffuse_sigma_min;
extern double	default_diffuse_sigma_max;

extern double	default_stoichiometry_composition;
extern double	default_stoichiometry_composition_min;
extern double	default_stoichiometry_composition_max;

// Roughness_Values
extern bool		default_roughness_enabled;

extern double	default_roughness_sigma;
extern double	default_roughness_sigma_min;
extern double	default_roughness_sigma_max;

extern double	default_roughness_correlation_radius;
extern double	default_roughness_correlation_radius_min;
extern double	default_roughness_correlation_radius_max;

extern double	default_roughness_fractal_alpha;
extern double	default_roughness_fractal_alpha_min;
extern double	default_roughness_fractal_alpha_max;

extern double	default_roughness_omega;
extern double	default_roughness_omega_min;
extern double	default_roughness_omega_max;

extern double	default_roughness_mu;
extern double	default_roughness_mu_min;
extern double	default_roughness_mu_max;

extern double	default_roughness_fractal_beta;
extern double	default_roughness_fractal_beta_min;
extern double	default_roughness_fractal_beta_max;

extern double	default_roughness_peak_sigma;
extern double	default_roughness_peak_sigma_min;
extern double	default_roughness_peak_sigma_max;

extern double	default_roughness_peak_frequency;
extern double	default_roughness_peak_frequency_min;
extern double	default_roughness_peak_frequency_max;

extern double	default_roughness_peak_frequency_width;
extern double	default_roughness_peak_frequency_width_min;
extern double	default_roughness_peak_frequency_width_max;

// Fluctuation_Values
extern bool		default_fluctuation_enabled;
extern bool		default_fluctuation_used;

extern double	default_fluctuation_absolute_density;
extern double	default_fluctuation_absolute_density_min;
extern double	default_fluctuation_absolute_density_max;

extern double	default_fluctuation_relative_density;
extern double	default_fluctuation_relative_density_min;
extern double	default_fluctuation_relative_density_max;

extern double	default_fluctuation_particle_radius;
extern double	default_fluctuation_particle_radius_min;
extern double	default_fluctuation_particle_radius_max;

extern double	default_fluctuation_particle_height;
extern double	default_fluctuation_particle_height_min;
extern double	default_fluctuation_particle_height_max;

extern double	default_fluctuation_particle_distance;
extern double	default_fluctuation_particle_distance_min;
extern double	default_fluctuation_particle_distance_max;

extern double	default_fluctuation_particle_distance_deviation;
extern double	default_fluctuation_particle_distance_deviation_min;
extern double	default_fluctuation_particle_distance_deviation_max;

extern double	default_fluctuation_domain_size;
extern double	default_fluctuation_domain_size_min;
extern double	default_fluctuation_domain_size_max;

extern double	default_fluctuation_z_position;
extern double	default_fluctuation_z_position_min;
extern double	default_fluctuation_z_position_max;

extern double	default_fluctuation_z_position_deviation;
extern double	default_fluctuation_z_position_deviation_min;
extern double	default_fluctuation_z_position_deviation_max;

// Stack_Values
extern int		default_stack_number_of_repetition;
extern int		default_stack_number_of_repetition_min;
extern int		default_stack_number_of_repetition_max;

extern double	default_stack_period;
extern double	default_stack_period_min;
extern double	default_stack_period_max;

extern double	default_stack_gamma;
extern double	default_stack_gamma_min;
extern double	default_stack_gamma_max;

// steps
extern double	step_thickness_transfer;
extern double	step_composition;
extern double	step_density;
extern double	step_thickness;
extern double	step_sigma_diffuse;

extern double	step_sigma_roughness;
extern double	step_sigma_cor_radius;
extern double	step_sigma_fractal_alpha_beta;
extern double	step_sigma_vertical_cor_length;
extern double	step_sigma_omega;
extern double	step_sigma_mu;
extern double	step_sigma_roughness_peak;
extern double	step_peak_frequency;
extern double	step_peak_frequency_width;

extern double	step_particle_density;
extern double	step_particle_radius;
extern double	step_particle_height;
extern double	step_particle_distance;
extern double	step_particle_distance_deviation;
extern double	step_domain_size;
extern double	step_particle_z_position;
extern double	step_particle_z_position_deviation;

extern double	step_interlayer;
extern double	step_gamma;
extern double	step_drift;
extern double	step_thickness_aperiodic;
extern double	step_sigma_aperiodic;

///====================================================================
/// precisions
///====================================================================

// lineedit
extern char line_edit_double_format			;
extern char line_edit_short_double_format	;
extern int line_edit_angle_precision		;
extern int line_edit_wavelength_precision	;
extern int line_edit_spectral_resolution_precision;
extern int line_edit_polarization_precision	;
extern int line_edit_density_precision		;
extern int line_edit_composition_precision	;
extern int line_edit_thickness_precision	;

extern int line_edit_sigma_precision		;
extern int line_edit_cor_radius_precision	;
extern int line_edit_fractal_alpha_precision;
extern int line_edit_omega_precision		;
extern int line_edit_mu_precision			;
extern int line_edit_frequency_precision	;

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
extern int thumbnail_composition_precision	;
extern int thumbnail_thickness_precision	;

extern int thumbnail_sigma_precision		;
extern int thumbnail_cor_radius_precision	;
extern int thumbnail_fractal_alpha_precision;
extern int thumbnail_omega_precision		;
extern int thumbnail_mu_precision			;
extern int thumbnail_frequency_precision	;

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

///====================================================================
/// measurement default values
///====================================================================

extern int default_wavelength_num_points;
extern int default_beam_theta_0_angle_num_points;
extern int default_detector_theta_angle_num_points;
extern int default_phi_angle_num_points;

extern int default_beam_theta_0_angle_specular_position;
extern int default_detector_theta_angle_offset;

extern double default_beam_theta_0_angle;
extern double default_beam_theta_0_angle_min;
extern double default_beam_theta_0_angle_max;

extern double default_detector_theta_angle;
extern double default_detector_theta_angle_min;
extern double default_detector_theta_angle_max;

extern double default_phi_angle;
extern double default_phi_angle_min;
extern double default_phi_angle_max;

extern double default_wavelength;
extern double default_wavelength_min;
extern double default_wavelength_max;

extern double default_logarithmic_threshold_beta;
extern double default_logarithmic_threshold_material_density;
extern double default_logarithmic_threshold_element_concentration;

extern double default_spectral_distribution_FWHM;
extern double default_spectral_distribution_coverage;
extern bool	  default_spectral_distribution_use_sampling;
extern int    default_spectral_distribution_number_of_Samples;

extern double default_beam_theta_0_distribution_FWHM;
extern double default_beam_theta_0_distribution_coverage;
extern bool   default_beam_theta_0_distribution_use_sampling;
extern int    default_beam_theta_0_distribution_number_of_Samples;

extern double default_beam_phi_0_distribution_FWHM;
extern double default_beam_phi_0_distribution_coverage;
extern bool   default_beam_phi_0_distribution_use_sampling;
extern int    default_beam_phi_0_distribution_number_of_Samples;

extern double default_detector_1D_slit_width;
extern double default_detector_1D_distance_to_sample;
extern double default_detector_1D_theta_resolution_FWHM;

extern double default_detector_2D_pixel_polar_height;
extern double default_detector_2D_pixel_azimuthal_width;
extern double default_detector_2D_distance_to_sample;
extern double default_detector_2D_theta_resolution_FWHM;
extern double default_detector_2D_phi_resolution_FWHM;

extern double default_beam_geometry_size;
extern double default_beam_geometry_smoothing;
extern double default_beam_geometry_wings_full_width;
extern double default_beam_geometry_wings_intensity;
extern double default_beam_geometry_lateral_width;
extern bool   default_beam_geometry_log_profile_plot;

extern double default_sample_geometry_size;
extern double default_sample_geometry_x_position;
extern double default_sample_geometry_z_position;
extern double default_sample_geometry_curvature;

extern double default_polarization;
extern double default_background;

///====================================================================
/// general settings
///====================================================================

extern int general_Settings_Tab_Index;
extern int reflectivity_calc_threads;
extern int optical_constants_read_threads;

extern bool recalculate_spinbox_global;

extern bool recalculate_spinbox_structure_table;
extern bool mouse_wheel_spinbox_structure_table;
extern bool refill_dependent_structure_table;
extern bool replot_graphs_during_fitting_1D;
//extern bool replot_graphs_during_fitting_2D;

extern int  n_max_series;
//extern bool use_beam_spot_in_specular_peak;

extern bool print_1D_Data_On_Recalculation;
extern bool print_2D_Data_On_Recalculation;

extern bool use_multifitting_directory;
extern bool use_working_directory;
extern QString  working_directory;

extern bool use_last_directory;
extern QString  last_directory;
extern QString preliminary_last_directory;
extern QString preliminary_last_data_directory;
extern QString			   last_data_directory;
extern QString preliminary_last_file;
extern QString			   last_file;

extern QString default_multilayer_tab_name;
extern QString default_independent_curve_tab_name;

extern bool tab_synchronization;
extern bool calculation_time;
extern bool show_residuals;

///====================================================================

class Settings
{
public:
	Settings();

	// gui
	static void read_Windows_Geometry_Settings(bool reset_to_default = false);
	static void save_Windows_Geometry_Settings();

	// structure default values
	static void read_Structure_Default_Values(bool reset_to_default = false);
	static void save_Structure_Default_Values();

	// precisions
	static void read_Precisions(bool reset_to_default = false);
	static void save_Precisions();

	// parameters default values
	static void read_Measurement_Default_Values(bool reset_to_default = false);
	static void save_Measurement_Default_Values();

	// general settings
	static void read_General_Settings(bool reset_to_default = false);
	static void save_General_Settings();

	// regular aperiodic parameters
	static void read_Regular_Aperiodic(bool reset_to_default = false);
	static void save_Regular_Aperiodic();

	// -----------------------------------------------------------------------------------------

	// all
	static void read_All_Settings(bool reset_to_default = false);
	static void save_All_Settings();

};

#endif // SETTINGS_H
