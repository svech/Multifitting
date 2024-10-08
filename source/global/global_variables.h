#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "global_definitions.h"
#include "global/layer_data_types.h"
#include "calculation/optical_constants.h"
#include <thread>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_spline.h>

class Optical_Constants;
class Data;
class Multilayer_Approach;
class QEvent;
// -----------------------------------------------------------------------------------------
extern int loaded_Version_Major;
extern int loaded_Version_Minor;
extern int loaded_Version_Build;

extern bool lambda_Out_Of_Range;

extern Multilayer_Approach* global_Multilayer_Approach;

// calculations
extern std::vector<std::thread> global_Workers;
extern vector<double> cos_a_Coeff_For_BesselJ0;
extern vector<double> sin_a_Coeff_For_BesselJ0;
extern vector<double> cos_a_Coeff_For_BesselJ1;
extern vector<double> sin_a_Coeff_For_BesselJ1;
extern int coeff_For_Bessel_Size;

extern double max_Phi_Azimuthal_Integration;

// locale
extern QLocale Locale;

// delimiters for file parsing
extern QRegExp delimiters;

// tabulated chemical elements
extern QStringList		element_Name;

extern QVector<double>	element_Mass;
extern QMap<QString, double> sorted_Elements;
extern QVector<double>	element_Density;
extern QMap<QString, double> sorted_Density;

// lists of types
extern QStringList transition_Layer_Functions; extern int transition_Layer_Functions_Size;
enum transitional_Function_Types_Enum    { Erf, Lin, Exp, Tanh, Sin, Step };

// multilayer toolbar
enum multilayer_Toolbar_Enum    { Add_Layer,
								  Add_Multilayer,
								  Add_Aperiodic,
//								  Add_Substrate,
								  Edit,
								  Remove,
								  Cut,
								  Copy,
								  Paste,
								  Move_Up,
								  Move_Down,
//								  Group,
								  Ungroup,
//								  Thickness_Plot,
//								  Sigma_Plot,
                                  Destroy
//								  , Export_Structure
                                };

// independent variables toolbar
enum independent_Toolbar_Enum   { New_Variable_Independent,
								  Edit_Independent,
								  Remove_Independent };

// data types
extern QString no_Measurement_Type;
extern QStringList measurement_Types;
enum target_Data_Types_Enum {	/** 1D */
								Specular_Scan,		// R,T vs angle, wavelength
								Detector_Scan,		// scattering vs theta
								Rocking_Curve,		// scattering vs theta_0
								Offset_Scan,		// scattering vs theta_0

								/** 2D */
								GISAS_Map	// scattering vs theta & phi
};

// argument types
extern QString no_Argument_Type;
extern QStringList argument_Types;
enum argument_Types_Enum    { Beam_Grazing_Angle, Deviation_From_Specular_Angle, Detector_Polar_Angle, Detector_Azimuthal_Angle, Detector_Theta_Phi_Angles, Wavelength_Energy };

// value types
extern QString no_Value_Type;
extern QStringList value_Types;
extern QStringList value_Types_Short;
enum value_Types_Enum    { Reflectance, Transmittance, Absorptance, Scattering, GISAS };

// distributions
extern QStringList distributions;
extern QVector<double> distribution_Coverage;
extern double coverage(QString distr);
enum distributions_Enum    { Gate, Gaussian }; // { Gate, Cosine, Gaussian, Lorentz }

// detector types
extern QStringList detectors;
enum detectors_Enum    { Slit, Crystal, Angular, Linear };


/// -------------------------------------------------------------------------
/// GSL
/// -------------------------------------------------------------------------
// GSL optimization methods
extern QString GSL_Group;
extern QStringList GSL_Methods;
enum GSL_Methods_Enum	{	/* Nonlinear Least-Squares Fitting */
							Levenberg_Marquardt,
							Levenberg_Marquardt_with_Geodesic_Acceleration,
							Dogleg,
							Double_Dogleg,
							Two_Dimensional_Subspace,

							/* Multidimensional Minimization (derivative) */
							Fletcher_Reeves_Conjugate_Gradient,
							Polak_Ribiere_Conjugate_Gradient,
							Broyden_Fletcher_Goldfarb_Shanno_BFGS,
							Broyden_Fletcher_Goldfarb_Shanno_BFGS_2,
							Steepest_Descent,

							/* Multidimensional Minimization (derivative-free) */
							Nelder_Mead_Simplex,
							Nelder_Mead_Simplex_2,
							Nelder_Mead_Simplex_2_Randomized
							};

// GSL solvers
extern QStringList GSL_Solvers;
enum GSL_Solvers_Enum		{QR_decomposition,
							 Cholesky_decomposition,
							 Singular_value_decomposition
							};

// GSL scaling
extern QStringList GSL_Scales;
enum GSL_Scales_Enum		{More,
							 Levenberg,
							 Marquardt
							};

// GSL finite differences
extern QStringList GSL_Fdtype;
enum GSL_Fdtype_Enum		{Forward,
							 Central
							};

/// -------------------------------------------------------------------------
/// SwarmOps
/// -------------------------------------------------------------------------
// SO optimization methods
extern QString SO_Group;
extern QStringList SO_Methods;
enum SO_Methods_Enum	{	/* Mesh Iteration. */
							Mesh_Iteration,

							/* Random Sampling */
							Random_Sampling_Uniform,

							/* Gradient-Based Optimization */
//							Gradient_Descent,
//							Gradient_Emancipated_Descent,

							/* Local Sampling */
							Hill_Climber,
							Simulated_Annealing,
							Pattern_Search,
							Local_Unimodal_Sampling,

							/* Swarm-Based Optimization, DE and variants */
							Differential_Evolution,
							Differential_Evolution_Suite,
							DE_with_Temporal_Parameters,
							Jan_Differential_Evolution,
							Evolution_by_Lingering_Global_Best,
							More_Yo_yos_Doing_Global_optimization,

							/* Swarm-Based Optimization, PSO and variants */
							Particle_Swarm_Optimization,
							Forever_Accumulating_Evolution,
							Many_Optimizing_Liaisons,

							/* Compound Methods */
							Layered_and_Interleaved_CoEvolution
							};

// -----------------------------------------------------------------------------------------

// units
extern QStringList wavelength_Units_List;	extern QMap<QString, double> wavelength_Coefficients_Map;	extern QMap<QString, QString> wavelength_Units_Legend_Map;
enum wavelength_Units_List_Enum		{ angstrom, nm, eV, keV }; // mcm

extern QStringList length_Units_List;		extern QMap<QString, double> length_Coefficients_Map;       extern QMap<QString, QString> length_Units_Legend_Map;

extern QStringList correlation_Length_Units_List;	extern QMap<QString, double> correlation_Length_Coefficients_Map;

extern QStringList omega_Units_List;	extern QMap<QString, double> omega_Coefficients_Map;

extern QStringList mu_Units_List;	extern QMap<QString, double> mu_Coefficients_Map;

extern QStringList linear_a2_Units_List;	extern QMap<QString, double> linear_a2_Coefficients_Map;
extern QStringList linear_a3_Units_List;	extern QMap<QString, double> linear_a3_Coefficients_Map;
extern QStringList linear_a4_Units_List;	extern QMap<QString, double> linear_a4_Coefficients_Map;

extern QStringList angle_Units_List;		extern QMap<QString, double> angle_Coefficients_Map;		extern QMap<QString, QString> angle_Units_Legend_Map;
enum angle_Units_List_Enum {degree, minute, second, mrad}; // radian

extern QStringList spatial_Frequency_Units_List;	extern QMap<QString, double> spatial_Frequency_Coefficients_Map;	extern QMap<QString, QString> spatial_Frequency_Units_Legend_Map;
enum Spatial_Frequency_Units_List_Enum {inv_mcm, inv_nm, inv_angstrom};

extern QStringList PSD_1D_Value_Units_List;		extern QMap<QString, double> PSD_1D_Value_Coefficients_Map;		extern QMap<QString, QString> PSD_1D_Value_Units_Legend_Map;
enum PSD_1D_Value_Units_List_Enum {mcm_3, nm_3, angstrom_3};

extern QStringList PSD_2D_Value_Units_List;		extern QMap<QString, double> PSD_2D_Value_Coefficients_Map;		extern QMap<QString, QString> PSD_2D_Value_Units_Legend_Map;
enum PSD_2D_Value_Units_List_Enum {mcm_4, nm_4, angstrom_4};

// units
extern QString length_units;
extern QString correlation_length_units;
extern QString omega_units;
extern QString mu_units;
extern QString linear_a2_units;
extern QString linear_a3_units;
extern QString linear_a4_units;
extern QString density_units;
extern QString spatial_frequency_units;
extern QString spatial_frequency_units_export;
extern QString PSD_1D_value_units;
extern QString PSD_2D_value_units;

// -----------------------------------------------------------------------------------------

extern QString fit_Color;
extern QString white_Color;
extern QString not_Parameter_Color;
extern QString free_Parameter_Color;
extern QString slave_Parameter_Color;
extern QString master_Parameter_Color;
extern QString master_Slave_Parameter_Color;
extern QString confidence_Parameter_Color;
extern QString master_Confidence_Parameter_Color;
extern QString particles_On_Off_Color;

extern QString common_Thickness_Color;
extern QString common_Sigma_Color;
extern QString common_Thickness_and_Sigma_Color;
extern QString soft_Restriction_Color;

// -----------------------------------------------------------------------------------------

extern QPen subinterval_Plot_Pen;
extern QBrush subinterval_Plot_Brush;
extern QBrush subinterval_Plot_Brush_2D;

extern vector<QColor> color_Contrast_Sequence;
extern vector<QColor> color_Contrast_Adjoint_Sequence;

// -----------------------------------------------------------------------------------------

extern int scale;
extern int corner_x_shift;
extern int corner_y_shift;
extern int width_add;
extern int height_add;
extern bool previous_all_windows_resizeable;

// -----------------------------------------------------------------------------------------

extern QString multilayer_Approach_Default_Title;
extern bool set_PSD_to_1;
// -----------------------------------------------------------------------------------------

extern Optical_Constants* optical_Constants;

class Global_Variables : public QObject
{
	Q_OBJECT
public:
	Global_Variables();
    static QString date_Time();
    static QString multifitting_Version();
    static void find_Gui_Shifts();
	static void read_Optical_Constants();
	static void create_Sorted_Elements_Map();
	static void   serialize_Tree(QDataStream &out, QTreeWidget* tree);
	static void deserialize_Tree(QDataStream &in,  QTreeWidget* tree);
    static void add_Substrate(QTreeWidget *tree);
	static void pseudo_Deserialize_Tree(QDataStream &in);
	static void   serialize_Variables_List(QDataStream &out, QListWidget* list);
	static void deserialize_Variables_List(QDataStream &in,  QListWidget* list);
	static void pseudo_Deserialize_Variables_List(QDataStream &in);
	static double wavelength_Energy(QString wavelength_Units, double y);
	static QString wavelength_Energy_Name(QString wavelength_Units, bool if_small = false);
	static QString wavelength_Energy_Symbol(QString spectral_Units);
	static double angstrom_eV(double x);
	static void fill_Units_Maps();
	static int get_Item_Depth(QTreeWidgetItem* item);
	static int get_Number_Of_Items(QTreeWidget* tree);
	static QTreeWidgetItem* get_Item_From_Tree_By_Id(QTreeWidget* tree, id_Type id);
	static int get_Tree_Depth(QTreeWidgetItem* item);
	static int get_Tree_Depth(tree<Node>& calc_Tree, const tree<Node>::iterator& parent);
	static QString structure_Item_Name(const Data& struct_Data);
	static int get_Index_For_Parameter_Name(const Data& struct_Data, const Parameter& parameter);
	static QString parameter_Name(const Data& struct_Data, QString whats_This, int index = -1);
	static bool expression_Is_Valid(QString expression_String, QStringList expression_Variables);
	static Parameter* get_Parameter_From_Struct_Item_by_Id		  (Data& struct_Data, id_Type id);
	static Parameter* get_Parameter_From_Struct_Item_by_Whats_This(Data& struct_Data, QString whats_This, int* line_edit_precision = nullptr, int* thumbnail_precision = nullptr, QString* units = nullptr, double* coeff = nullptr);

    static void make_non_minimizable_window(QWidget* w);
    static void common_Change_Event(QEvent *event, QWidget* w);

	static QString material_From_Composition(const QList<Stoichiometry>& composition);

	static void copy_Tree(const QTreeWidget* from_Tree, QTreeWidget* to_Tree);
	static bool check_Loaded_Version(int MAJOR, int MINOR, int BUILD);
    static void create_Shortcuts(QWidget* this_Widget);
    static QString working_Directory();
    static QString slaches_To_Underline(QString text);

	static void parallel_For(int num_Points, int num_Threads, const std::function<void(int n_Min, int n_Max, int thread_Index)> &lambda);
	static double get_Order_Of_Magnitude(double number);
	static void fill_Vector_With_Log_Step(vector<double>& arg, double nu_Start, double nu_End, int num_Points);
	static void fill_Vector_With_Log_Step_With_Peak(vector<double>& arg, double nu_Start, double nu_End, double peak_Frequency,double  peak_Frequency_Width, int num_Points, bool if_1D = false);
	static double fill_Nu_Start_From_Xi(double xi);

	template <typename Type>
	static void resize_Line_Edit(Type* input_Line_Edit, bool adjust_Window = false);
	static void change_Parent_Period_Gamma_Thickness(QTreeWidgetItem *current_Item);
	static void plot_All_Data_in_Graphs();
	static void plot_All_Data_in_1D_Graphs();
	static void plot_All_Data_in_2D_Graphs();
	static void plot_All_Data_in_Profiles();
	static void plot_All_Data_in_Roughness();
	static void plot_All_Data_in_Particles();
	static void variable_Drift(double& value, const Drift &drift, int period_Index, int num_Repetition, gsl_rng *r);
	static void get_Prefix_Suffix(double& prefix, double& suffix, double max_Sigma);
	static void discretize_Prefix_Suffix(double prefix, double suffix, int& num_Prefix_Slices, int& num_Suffix_Slices, vector<double>& discrete_Step_Vector, double default_Step);
	static void get_Peak_Parameters(const vector<double>& argument, const vector<double>& values, double& max_Value_Position, double& max_Value, double& width);

	// profile functions
	static double theta_Function(double z);
	static double erf_Profile (double z, double sigma);
	static double lin_Profile (double z, double sigma);
	static double exp_Profile (double z, double sigma);
	static double tanh_Profile(double z, double sigma);
	static double sin_Profile (double z, double sigma);
	static double step_Profile(double z, double sigma);
        static double interface_Profile_Function(double z, const vector<Interlayer>& interlayer_Composition/*, bool for_Integration = false*/);
        static double get_Max_Sigma_From_Interlayer_Composition(vector<Interlayer>& interlayer_Composition);
//	static double layer_Normalization_GSL(double thickness, vector<Interlayer>& left_Interlayer_Composition, vector<Interlayer>& right_Interlayer_Composition, gsl_integration_workspace* w);
        static double layer_Normalization(double thickness, vector<Interlayer>& left_Interlayer_Composition, vector<Interlayer>& right_Interlayer_Composition);
	static void normalize_Interlayer(Data& struct_Data);

	// beam profile
	static double beam_Profile(double x, double FWHM, double smoothing);
	static double beam_Profile_Integral(double x, double FWHM, double smoothing);
	static double beam_Profile_Bounds(double FWHM, double smoothing);
	static double beam_Profile_Integral_Bounded(double x, double FWHM, double smoothing, double left_Bound, double right_Bound);
	static double beam_Profile_With_Wings(double x, double FWHM, double smoothing, double wings_FW, double wings_Intensity);
	static double beam_Profile_With_Wings_Integral(double x, double FWHM, double smoothing, double wings_FW, double wings_Intensity);
	static double gate_Gate_Integral  (double FWHM_a, double FWHM_b, double theta_a, double theta_b);
	static double gate_Gauss_Integral (double FWHM_a, double FWHM_b, double distance_ta_tb);
	static double gauss_Gauss_Integral(double FWHM_a, double FWHM_b, double distance_ta_tb);

	// resolution/distribution functions
	static void   distribution_Sampling(Distribution distribution, QVector<double>& positions, QVector<double>& heights);
	static double distribution_Function(QString function, double FWHM, double x);
	static double distribution_Gate		(double FWHM, double x);
	static double distribution_Cosine	(double FWHM, double x);
	static double distribution_Gaussian	(double FWHM, double x);
	static double distribution_Lorentz	(double FWHM, double x);
	static double distribution_Short_Lorentz	(double FWHM, double x);

	// 2D plots
	static void color_Scheme_Change(QCPColorMap* color_Map, QCustomPlot* main_Plot, QCPColorGradient::GradientPreset* color_Scheme);
	static void color_Scheme_Example(QCustomPlot* plot, QCPColorGradient::GradientPreset preset);

	// roughness & particles
	static bool if_Last_Layer       (QTreeWidget* tree, QTreeWidgetItem* potential_Layer);
	static bool if_Second_Last_Layer(QTreeWidget* tree, QTreeWidgetItem* potential_Layer);
	static void enable_Disable_Roughness_Model(Data& struct_Data, const Imperfections_Model& imperfections_Model, bool last_Layer = false);
	static void enable_Disable_Particles_Model(Data& struct_Data, const Imperfections_Model& imperfections_Model, bool last_Layer = false/*, bool second_Last_Layer = false*/);
	static void new_Layer_Particles_Model(Data& struct_Data, const Imperfections_Model& imperfections_Model);
	static double PSD_ABC_1D						 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline = nullptr, gsl_interp_accel* acc = nullptr);
	static double PSD_ABC_1D_from_nu				 (double factor, double xi, double alpha, double p, double a, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_ABC_1D_Finite_from_nu			 (double sigma,  double xi, double alpha, double p, double a, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_ABC_2D						 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_ABC_2D_from_nu				 (double factor, double xi, double alpha, double nu, double a, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Real_Gauss_1D					 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline = nullptr, gsl_interp_accel* acc = nullptr);
	static double PSD_Real_Gauss_1D_from_nu			 (double factor, double xi, double alpha, double p, double a, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Real_Gauss_1D_Finite_from_nu	 (double sigma,  double xi, double alpha, double p, double a, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Real_Gauss_2D					 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Real_Gauss_2D_from_nu			 (double factor, double xi, double alpha, double nu, double a, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Fractal_Gauss_1D				 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Fractal_Gauss_1D_from_nu		 (double factor, double xi, double alpha, double p, double a, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Fractal_Gauss_1D_Finite_from_nu(double sigma, double xi, double alpha, double p, double a, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Fractal_Gauss_1D_Asymp_from_nu (double factor, double alpha, double p);
	static double PSD_Fractal_Gauss_2D				 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Fractal_Gauss_2D_from_nu		 (double factor, double xi, double alpha, double nu, double a, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Fractal_Gauss_2D_Asymp_from_nu (double factor, double alpha, double nu);
	static double PSD_Gauss_Peak_1D					 (double factor, double peak_Frequency, double peak_Width, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Gauss_Peak_1D_from_nu			 (double factor, double peak_Frequency, double peak_Width, double p, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Gauss_Peak_2D					 (double factor, double peak_Frequency, double peak_Width, double k, double cos_Theta, double cos_Theta_0, double cos_Phi, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Gauss_Peak_2D_from_nu			 (double factor, double peak_Frequency, double peak_Width, double nu, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Measured_1D					 (double sigma,  double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline, gsl_interp_accel* acc);
	static double PSD_Measured_2D					 (double sigma,  double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi, gsl_spline* spline, gsl_interp_accel* acc);
	static double no_PSD_at_all_1D					 (double sigma,  double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline = nullptr, gsl_interp_accel* acc = nullptr);
	static double zero_PSD_1D						 (double sigma,  double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline = nullptr, gsl_interp_accel* acc = nullptr);
	static double zero_PSD_1D_from_nu				 (double factor, double xi, double alpha, double p, gsl_spline* spline, gsl_interp_accel* acc);
	static double no_PSD_at_all_2D					 (double sigma,  double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi, gsl_spline* spline, gsl_interp_accel* acc);
	static double zero_PSD_2D						 (double sigma,  double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi, gsl_spline* spline, gsl_interp_accel* acc);
	static double zero_PSD_2D_from_nu				 (double factor, double xi, double alpha, double nu, gsl_spline* spline, gsl_interp_accel* acc);

	static double ABC_1D_Integral_0_Nu				 (double sigma, double xi, double alpha, double nu_Start, double nu, double dp2, ooura_fourier_sin<double>& integrator, gsl_spline* spline, gsl_interp_accel* acc, QString PSD_Model);
	static double ABC_2D_Integral_0_Nu				 (double sigma, double xi, double alpha, double nu_Start, double nu, double dp2, ooura_fourier_sin<double>& integrator, gsl_spline* spline, gsl_interp_accel* acc, QString PSD_Model);
	static double FG_1D_Integral_0_Nu				 (double sigma, double xi, double alpha, double nu_Start, double nu, double dp2, ooura_fourier_sin<double>& integrator, gsl_spline* spline, gsl_interp_accel* acc, QString PSD_Model);
	static double FG_2D_Integral_0_Nu				 (double sigma, double xi, double alpha, double nu_Start, double nu, double dp2, ooura_fourier_sin<double>& integrator, gsl_spline* spline, gsl_interp_accel* acc, QString PSD_Model);
	static double real_Gauss_1D_Integral_0_Nu		 (double sigma, double xi, double alpha, double nu_Start, double nu, double dp2, ooura_fourier_sin<double>& integrator, gsl_spline* spline, gsl_interp_accel* acc, QString PSD_Model);
	static double real_Gauss_2D_Integral_0_Nu		 (double sigma, double xi, double alpha, double nu_Start, double nu, double dp2, ooura_fourier_sin<double>& integrator, gsl_spline* spline, gsl_interp_accel* acc, QString PSD_Model);
	static double integral_1D_0_p_Finite_Slit		 (double factor, double xi, double alpha, double nu_Start, double nu, double dp2, ooura_fourier_sin<double>& integrator, gsl_spline* spline, gsl_interp_accel* acc, QString PSD_Model);

	static double get_Phi_Max_From_Finite_Slit		 (const Data& measurement, double cos_Theta = 1);
	static double get_Nu_Max_From_Finite_Slit		 (double p, const Data& measurement);

	static double Cor_Fractal_Gauss					 (				 double xi, double alpha, double r);
	static double Cor_ABC							 (				 double xi, double alpha, double r);

	static double nu_Alpha_2D						 (              double nu0, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi);
	static double nu_Beta_2D						 (				double nu0, double beta,  double k, double cos_Theta, double cos_Theta_0, double cos_Phi);
	static double nu2								 (                                        double k, double cos_Theta, double cos_Theta_0, double cos_Phi);
	static double PSD_Linear_Growth_2D (double exponent, double nu2_mu_Alpha_2, double omega, double thickness);

	static double val_Cos_Expansion (double arg, const vector<double>& cos_a_Coeff);
	static double val_Sin_Expansion (double arg, const vector<double>& sin_a_Coeff);

	// particles
	static double G1_Square			(double a);
	static double G2_Square			(double q, double phi, double a, double sigma, double N, double M);
	static double G2_Square_q_Zero	(double q,             double a, double sigma, double N, double M);
	static double G2_Square_long	(long double q, long double phi, long double a, long double sigma, long double N, long double M);

	static double G1_Hexagone		(double a);
	static double G2_Hexagone		(double q, double phi, double a, double sigma, double N, double M);
	static double G2_Hexagone_q_Zero(double q,             double a, double sigma, double N, double M);
	static double G2_Hexagone_long	(long double q, long double phi, long double a, long double sigma, long double N, long double M);

	static double G1_Pure_Radial		(double a);
	static double G2_Pure_Radial		(double q, double phi, double a, double sigma, double N, double M);
	static double G2_Pure_Radial_q_Zero	(double q,			   double a, double sigma, double N, double M);

	static complex<double> full_Sphere_FF_Prefactor  (          const complex<double>& qz, double R, double H, double z);
	static complex<double> full_Sphere_FF_q_Factor	 (double q, const complex<double>& qz, double R, double H, double z);
	static complex<double> full_Sphere_FF			 (double q, const complex<double>& qz, double R, double H, double z);
	static complex<double> full_Spheroid_FF_Prefactor(          const complex<double>& qz, double R, double H, double z);
	static complex<double> full_Spheroid_FF_q_Factor (double q, const complex<double>& qz, double R, double H, double z);
	static complex<double> full_Spheroid_FF			 (double q, const complex<double>& qz, double R, double H, double z);
	static complex<double> cylinder_FF_Prefactor	 (          const complex<double>& qz, double R, double H, double z);
	static complex<double> cylinder_FF_q_Factor		 (double q, const complex<double>& qz, double R, double H, double z);
	static complex<double> cylinder_FF				 (double q, const complex<double>& qz, double R, double H, double z);

	static complex<double> omega_Factor (complex<double> k,                      double sigma);
	static complex<double> gamma_Factor (complex<double> kl, complex<double> kr, double sigma);
	};

#endif // GLOBAL_VARIABLES_H
