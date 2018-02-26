#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "global_definitions.h"
#include "global/layer_data_types.h"
#include "calculation/optical_constants.h"

class Optical_Constants;
class Data;
class Multilayer_Approach;
// -----------------------------------------------------------------------------------------
extern Multilayer_Approach* global_Multilayer_Approach;

// delimiters for file parsing
extern QRegExp delimiters;

// tabulated chemical elements
extern QStringList		element_Name;
extern QVector<double>	element_Mass;
extern QMap<QString, double> sorted_Elements;

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
								  Group,
								  Ungroup,
								  Thickness_Plot,
								  Sigma_Plot,
								  Destroy };

// independent variables toolbar
enum independent_Toolbar_Enum   { New_Variable_Independent,
								  Edit_Independent,
								  Remove_Independent,
								  Function_Independent };

// measured curves
extern QStringList argument_Types;
enum argument_Types_Enum    { Grazing_angle, Incident_angle, Wavelength_Energy };

extern QStringList value_Function;
enum value_Function_Enum    { Reflectance,   Transmittance,  Absorptance	   };

extern QStringList value_R_Mode;
enum value_R_Mode_Enum		{ R,  R_Phi,     r_Re_Im,        r_Abs_Phi         };

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

extern Optical_Constants* optical_Constants;

class Global_Variables
{
public:
	Global_Variables();
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
	static int get_Tree_Depth(QTreeWidgetItem* item);
	static QString structure_Item_Name(const Data& struct_Data);
	static QString parameter_Name(const Data& struct_Data, QString whats_This, int index = -1);
	static bool expression_Is_Valid(QString expression_String, QStringList expression_Variables);
	static Parameter* get_Parameter_From_Struct_Item_by_Id		  (Data& struct_Data, id_Type id);
	static Parameter* get_Parameter_From_Struct_Item_by_Whats_This(Data& struct_Data, QString whats_This);
};

#endif // GLOBAL_VARIABLES_H
