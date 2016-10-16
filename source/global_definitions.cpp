#include "global_definitions.h"

QStringList element_Name {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe","Cs","Ba","La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu","Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra","Ac","Th","Pa","U"};
QVector<double> element_Mass
							{1.00794,
							 4.002602,
							 6.941,
							 9.012182,
							 10.811,
							 12.0107,
							 14.0067,
							 15.9994,
							 18.9984032,
							 20.1791,
							 22.98976928,
							 24.3050,
							 26.9815386,
							 28.0855,
							 30.973762,
							 32.065,
							 35.453,
							 39.948,
							 39.0983,
							 40.078,
							 44.955912,
							 47.867,
							 50.9415,
							 51.9961,
							 54.938045,
							 55.845,
							 58.933195,
							 58.6934,
							 63.546,
							 65.38,
							 69.723,
							 72.63,
							 74.92160,
							 78.96,
							 79.904,
							 83.798,
							 85.4678,
							 87.62,
							 88.90585,
							 91.224,
							 92.90638,
							 95.96,
							 98.,
							 101.07,
							 102.90550,
							 106.42,
							 107.8682,
							 112.411,
							 114.818,
							 118.710,
							 121.760,
							 127.60,
							 126.90447,
							 131.293,
							 132.9054519,
							 137.327,
							 138.90547,
							 140.116,
							 140.90765,
							 144.242,
							 145.,
							 150.36,
							 151.964,
							 157.25,
							 158.92535,
							 162.500,
							 164.93032,
							 167.259,
							 168.93421,
							 173.054,
							 174.9668,
							 178.49,
							 180.94788,
							 183.84,
							 186.207,
							 190.23,
							 192.217,
							 195.084,
							 196.966569,
							 200.59,
							 204.3833,
							 207.2,
							 208.98040,
							 209.,
							 210.,
							 222.,
							 223.,
							 226.,
							 227.,
							 232.03806,
							 231.03586,
							 238.02891};

QStringList transition_Layer_Functions	{"erf","lin","exp","tanh","sin"};
QStringList drift_Models				{"no_drift","lin_sin","par_sin","hyp_sin","exp_sin","log_sin"};

int default_Column = 0;
int num_Permanent_Independent_Variables = 2;

QString whats_This_Delimiter = ";";
QString item_Type_Delimiter = " ";
QString space = " ";

// main whatsThis
QString whats_This_Measurement = "Measurement";
QString whats_This_Ambient = "Ambient";
QString whats_This_Layer = "Layer";
QString whats_This_Multilayer = "Multilayer";
QString whats_This_Substrate = "Substrate";

// specialized whatsThis additions
QString whats_This_Angle = "Angle";
QString whats_This_Wavelength = "Wavelength";
QString whats_This_Density = "Density";
QString whats_This_Permittivity = "Permittivity";
QString whats_This_Absorption = "Absorption";
QString whats_This_Composition = "Composition";
QString whats_This_Thickness = "Thickness";
QString whats_This_Sigma = "Sigma";
QString whats_This_Interlayer_Composition = "Interlayer_Composition";
QString whats_This_Num_Repetitions = "Num_Repetitions";
QString whats_This_Period = "Period";
QString whats_This_Gamma = "Gamma";

//-----------------------------------------------
// settings
//-----------------------------------------------

// gui_Settings
QString application_style;
/// launcher window geometry
int launcher_x_corner;
int launcher_y_corner;
/// multilayer window geometry
int multilayer_x_corner;
int multilayer_y_corner;
int multilayer_min_width;
int multilayer_min_height;
int multilayer_width;
int multilayer_height;
int multilayer_height_additive;
/// multilayer tab name
QString default_multilayer_tab_name;
QString default_independent_variable_tab_name;
/// resource path
QString icon_path;

// default_Values
/// Ambient_Values
QString	ambient_default_material;
double	ambient_default_density;
double	ambient_default_permittivity;
double	ambient_default_absorption;
bool	ambient_default_composed;
double	ambient_default_stoichiometry_composition;
QString	ambient_default_stoichiometry_element;
/// Layer_Values
QString	layer_default_material;
double	layer_default_density;
double	layer_default_permittivity;
double	layer_default_absorption;
bool	layer_default_composed;
double	layer_default_stoichiometry_composition;
QString	layer_default_stoichiometry_element;
double	layer_default_sigma;
double	layer_default_thickness;
QString	layer_default_drift_model;
double	layer_default_drift_coefficients;
/// Substrate_Values
QString	substrate_default_material;
double	substrate_default_density;
double	substrate_default_permittivity;
double	substrate_default_absorption;
bool	substrate_default_composed;
double	substrate_default_stoichiometry_composition;
QString	substrate_default_stoichiometry_element;
double	substrate_default_sigma;
/// Stack_Values
int		stack_default_number_of_repetition;
double	stack_default_period;
double	stack_default_gamma;

/// qlineedit precisions
char line_edit_double_format		;
int line_edit_angle_precision		;
int line_edit_wavelength_precision	;
int line_edit_density_precision		;
int line_edit_permittivity_precision;
int line_edit_absorption_precision	;
int line_edit_composition_precision	;
int line_edit_thickness_precision	;
int line_edit_sigma_precision		;
int line_edit_interlayer_precision	;
int line_edit_period_precision		;
int line_edit_gamma_precision		;
/// thumbnail precisions
char thumbnail_double_format		;
int thumbnail_angle_precision		;
int thumbnail_wavelength_precision	;
int thumbnail_density_precision		;
int thumbnail_permittivity_precision;
int thumbnail_absorption_precision	;
int thumbnail_composition_precision	;
int thumbnail_thickness_precision	;
int thumbnail_sigma_precision		;
int thumbnail_interlayer_precision	;
int thumbnail_period_precision		;
int thumbnail_gamma_precision		;
/// other precisions
int at_weight_precision				;

/// angular and spectral independent variables
int default_num_angular_points;
int default_num_spectral_points;
double default_min_angle;
double default_max_angle;
double default_min_wavelength;
double default_max_wavelength;
Angle_Type default_angle_type;
double default_angular_resolution;
double default_polarization;
double default_spectral_resolution;
double default_polarization_sensitivity;

//-----------------------------------------------
