#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "global_definitions.h"
#include "global/layer_data_types.h"
#include "calculation/optical_constants.h"
#include <thread>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

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
								  Add_Substrate,
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
								  Remove_Independent,
								  Function_Independent };

// measured curves
extern QStringList argument_Types;
enum argument_Types_Enum    { Grazing_angle, Incident_angle, Wavelength_Energy };

extern QStringList value_Function;
enum value_Function_Enum    { Reflectance,   Transmittance , Absorptance};

extern QStringList value_R_Mode;
extern QStringList value_R_Mode_Label_1;
extern QStringList value_R_Mode_Label_2;
enum value_R_Mode_Enum		{ R,  R_Phi };

extern QStringList value_T_Mode;
enum value_T_Mode_Enum		{ T };

extern QStringList value_A_Mode;
enum value_A_Mode_Enum		{ A };

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
extern QStringList wavelength_Units_List;	extern QMap<QString, double> wavelength_Coefficients_Map;
enum wavelength_Units_List_Enum		{ angstrom, nm, mcm, eV, keV };

extern QStringList length_Units_List;		extern QMap<QString, double> length_Coefficients_Map;
extern QStringList angle_Units_List;		extern QMap<QString, double> angle_Coefficients_Map;		extern QMap<QString, QString> angle_Units_Legend_Map;
enum angle_Units_List_Enum {degree, minute, second, radian, mrad};

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

extern QVector<QColor> color_Contrast_Sequence;
extern QVector<QColor> color_Contrast_Adjoint_Sequence;

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

extern bool can_Change_Index;

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
	static void   serialize_Variables_List(QDataStream &out, QListWidget* list);
	static void deserialize_Variables_List(QDataStream &in,  QListWidget* list);
	static double wavelength_Energy(QString wavelength_Units, double y);
	static QString wavelength_Energy_Name(QString wavelength_Units);
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
	static void resize_Line_Edit(Type* input_Line_Edit, bool adjust_Window = true);
	static void change_Parent_Period_Gamma_Thickness(QTreeWidgetItem *current_Item);
	static void plot_All_Data_in_Graphs();
	static void plot_All_Data_in_Profiles();
	static void replot_All_Graphs();
	static void variable_Drift(double& value, Drift& drift, int period_Index, int num_Repetition, gsl_rng *r);
	static void get_Prefix_Suffix(double& prefix, double& suffix, double max_Sigma);
	static void discretize_Prefix_Suffix(double prefix, double suffix, int& num_Prefix_Slices, int& num_Suffix_Slices, vector<double>& discrete_Step_Vector, double default_Step);
	static void get_Peak_Parameters(const QVector<double>& argument, const QVector<double>& values, double& max_Value_Position, double& max_Value, double& width);

	// profile functions
	static double theta_Function(double z);
	static double erf_Profile (double z, double sigma);
	static double lin_Profile (double z, double sigma);
	static double exp_Profile (double z, double sigma);
	static double tanh_Profile(double z, double sigma);
	static double sin_Profile (double z, double sigma);
	static double step_Profile(double z, double sigma);
	static double interface_Profile_Function(double z, const QVector<Interlayer>& interlayer_Composition, bool for_Integration = false);
	static double get_Max_Sigma_From_Interlayer_Composition(QVector<Interlayer>& interlayer_Composition);
	static double layer_Normalization(double thickness, QVector<Interlayer>& left_Interlayer_Composition, QVector<Interlayer>& right_Interlayer_Composition, gsl_integration_workspace* w);
};

#endif // GLOBAL_VARIABLES_H
