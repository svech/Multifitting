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
// -----------------------------------------------------------------------------------------
extern int loaded_Version_Major;
extern int loaded_Version_Minor;
extern int loaded_Version_Build;

extern bool lambda_Out_Of_Range;

extern Multilayer_Approach* global_Multilayer_Approach;

// calculations
extern std::vector<std::thread> global_Workers;

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
extern QStringList tril;

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
								  Destroy,
								  Export_Structure};

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
extern QStringList distributions_No_Lorentz;
enum distributions_Enum    { Gate, Cosine, Gaussian, Lorentz };

// detector types
extern QStringList detectors;
enum detectors_Enum    { Slit, Crystal, Spherical, Rectangular };


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

extern QStringList length_Units_List;		extern QMap<QString, double> length_Coefficients_Map;

extern QStringList angle_Units_List;		extern QMap<QString, double> angle_Coefficients_Map;		extern QMap<QString, QString> angle_Units_Legend_Map;
enum angle_Units_List_Enum {degree, minute, second, mrad}; // radian

extern QStringList density_Units_List;
extern QStringList opt_Const_Units_List;
extern QStringList drift_Units_List;

// size units
extern QStringList size_Units_List;
enum size_Units_List_Enum {mm};
// -----------------------------------------------------------------------------------------

extern QString fit_Color;
extern QString white_Color;
extern QString free_Parameter_Color;
extern QString slave_Parameter_Color;
extern QString master_Parameter_Color;
extern QString master_Slave_Parameter_Color;
extern QString confidence_Parameter_Color;
extern QString master_Confidence_Parameter_Color;

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

extern double default_Profile_Line_Thickness;
extern double selected_Profile_Line_Thickness;

// -----------------------------------------------------------------------------------------

extern int scale;
extern int corner_x_shift;
extern int corner_y_shift;
extern int width_add;
extern int height_add;

// -----------------------------------------------------------------------------------------

extern QString multilayer_Approach_Default_Title;

// -----------------------------------------------------------------------------------------

extern Optical_Constants* optical_Constants;

class Global_Variables : public QObject
{
	Q_OBJECT
public:
	Global_Variables();
	static void find_Gui_Shifts();
	static void read_Optical_Constants();
	static void create_Sorted_Elements_Map();
	static void   serialize_Tree(QDataStream &out, QTreeWidget* tree);
	static void deserialize_Tree(QDataStream &in,  QTreeWidget* tree);
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
	static QString structure_Item_Name(const Data& struct_Data);
	static QString parameter_Name(const Data& struct_Data, QString whats_This, int index = -1);
	static bool expression_Is_Valid(QString expression_String, QStringList expression_Variables);
	static Parameter* get_Parameter_From_Struct_Item_by_Id		  (Data& struct_Data, id_Type id);
	static Parameter* get_Parameter_From_Struct_Item_by_Whats_This(Data& struct_Data, QString whats_This, int* line_edit_precision = nullptr, int* thumbnail_precision = nullptr, QString* units = nullptr, double* coeff = nullptr);

	static void copy_Tree(const QTreeWidget* from_Tree, QTreeWidget* to_Tree);
	static bool check_Loaded_Version(int MAJOR, int MINOR, int BUILD);
	static void create_Shortcuts(QWidget* this_Widget);

	static void parallel_For(int num_Points, int num_Threads, const std::function<void(int n_Min, int n_Max, int thread_Index)> &lambda);
	static double get_Order_Of_Magnitude(double number);

	template <typename Type>
	static void resize_Line_Edit(Type* input_Line_Edit, bool adjust_Window = false);
	static void change_Parent_Period_Gamma_Thickness(QTreeWidgetItem *current_Item);
	static void plot_All_Data_in_Graphs();
	static void plot_All_Data_in_1D_Graphs();
	static void plot_All_Data_in_2D_Graphs();
	static void plot_All_Data_in_Profiles();
	static void variable_Drift(double& value, Drift& drift, int period_Index, int num_Repetition, gsl_rng *r);
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
	static double interface_Profile_Function(double z, const QVector<Interlayer>& interlayer_Composition/*, bool for_Integration = false*/);
	static double get_Max_Sigma_From_Interlayer_Composition(QVector<Interlayer>& interlayer_Composition);
//	static double layer_Normalization_GSL(double thickness, QVector<Interlayer>& left_Interlayer_Composition, QVector<Interlayer>& right_Interlayer_Composition, gsl_integration_workspace* w);
	static double layer_Normalization(double thickness, QVector<Interlayer>& left_Interlayer_Composition, QVector<Interlayer>& right_Interlayer_Composition);

	// beam profile
	static double beam_Profile(double x, double FWHM, double smoothing);

	// resolution/distribution functions
	static void   distribution_Sampling(Distribution distribution, QVector<double>& positions, QVector<double>& heights);
	static double distribution_Function(QString function, double FWHM, double x);
	static double distribution_Gate		(double FWHM, double x);
	static double distribution_Cosine	(double FWHM, double x);
	static double distribution_Gaussian	(double FWHM, double x);
	static double distribution_Lorentz	(double FWHM, double x);

	// 2D plots
	static void color_Scheme_Change(QCPColorMap* color_Map, QCustomPlot* main_Plot, QCPColorGradient::GradientPreset* color_Scheme);
	static void color_Scheme_Example(QCustomPlot* plot, QCPColorGradient::GradientPreset preset);

	// roughness
	static void enable_Disable_Roughness_Model(Data& struct_Data, const Imperfections_Model& imperfections_Model);
	static void enable_Disable_Fluctuations_Model(Data& struct_Data, const Imperfections_Model& imperfections_Model);
	static void new_Layer_Fluctuations_Model(Data& struct_Data, const Imperfections_Model& imperfections_Model);
	static double PSD_ABC_1D			 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline = nullptr, gsl_interp_accel* acc = nullptr);
	static double PSD_ABC_2D			 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi);
	static double PSD_Real_Gauss_1D		 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline = nullptr, gsl_interp_accel* acc = nullptr);
	static double PSD_Real_Gauss_2D		 (double factor, double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi);
	static double PSD_Fractal_Gauss_1D	 (double sigma,  double xi, double alpha, double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline, gsl_interp_accel* acc);
	static double Cor_Fractal_Gauss		 (				 double xi, double alpha, double r);
	static double Cor_ABC				 (				 double xi, double alpha, double r);
	static double splined_Value_1D		 (                                        double k, double cos_Theta, double cos_Theta_0, gsl_spline* spline, gsl_interp_accel* acc);

	static double nu_Alpha_2D			 (                          double alpha, double k, double cos_Theta, double cos_Theta_0, double cos_Phi);
	static double PSD_Linear_Growth_2D	 (double exponent, double nu2_mu_Alpha_2, double omega, double nu2_omega_23, double alpha, double thickness);

	// density fluctuations
	static double G1_Square	 (double a);
	static double G2_Square	 (double q, double phi, double a,  double sigma, double N, double M);
	static double G2_Square_long	 (long double q, long double phi, long double a, long double sigma, long double N, long double M);
	static double G1_Hexagone(double a);
	static double G2_Hexagone(double q, double phi, double a,  double sigma, double N, double M);
	static double G2_Hexagone_long	 (long double q, long double phi, long double a, long double sigma, long double N, long double M);

	static complex<double> full_Sphere_FF  (double q, complex<double> qz, double R, double H);
	static complex<double> full_Spheroid_FF(double q, complex<double> qz, double R, double H);
	static complex<double> cylinder_FF	   (double q, complex<double> qz, double R, double H);

	static complex<double> omega_Factor (complex<double> k,                      double sigma);
	static complex<double> gamma_Factor (complex<double> kl, complex<double> kr, double sigma);

};

#endif // GLOBAL_VARIABLES_H
