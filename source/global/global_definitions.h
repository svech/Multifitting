#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include "boost/math/quadrature/ooura_fourier_integrals.hpp"
#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/quadrature/gauss.hpp>
#include <boost/math/quadrature/trapezoidal.hpp>
#include <boost/math/special_functions/hypergeometric_pFq.hpp>
#include <QtWidgets>
#include <vector>
#include <complex>
#include "tree.hh"
#include "qcustomplot.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 11
#define VERSION_BUILD 0

using namespace std;
using namespace boost::math::quadrature;
class Node;

//#define RELEASE

//#define EXPRTK

#ifdef RELEASE
   #define EXPRTK
#endif

#ifdef EXPRTK
    #include "exprtk.hpp"
#endif

#ifdef __linux__
	#include <iostream>
#endif

// -----------------------------------------------------------------------------------------


#define control_String						 "Multifitting"
#define default_File						 "save_v."+QString::number(VERSION_MAJOR)+"."+QString::number(VERSION_MINOR)+"."+QString::number(VERSION_BUILD)+".fit"
#define default_Fit_Statictics_File			 "fits.txt"
#define default_Confidence_Distribution_File "confidence.txt"
#define file_Extension						 "fit"

// settings

#ifdef _WIN32	
	#ifdef RELEASE
	   #define Pre_Path QString("../")
	#else
		#define Pre_Path QString("../../")
	#endif
#endif
#ifdef __linux__
	#define Pre_Path QString("../")
#endif

// paths
#define Paths_Settings_Path Pre_Path + "settings/paths.ini"
	#define Resource_Paths				"Resource_Paths"
	#define Last_Paths					"Last_Paths"

#define Paths_Icon_Path Pre_Path + "icons/"

// gui
#define Gui_Settings_Path Pre_Path + "settings/gui.ini"
	#define Application_Style						"Application_Style"
	#define Launcher_Geometry						"Launcher_Geometry"
	#define Multilayer_Window_Geometry				"Multilayer_Window_Geometry"
	#define Curve_Window_Geometry					"Curve_Window_Geometry"
	#define Structure_Table_Window_Geometry			"Structure_Table_Window_Geometry"
	#define Roughness_Table_Window_Geometry			"Roughness_Table_Window_Geometry"
	#define Regular_Aperiodic_Table_Window_Geometry	"Regular_Aperiodic_Table_Window_Geometry"
	#define Graphs_Window_Geometry					"Graphs_Window_Geometry"
	#define Profile_Plots_Window_Geometry			"Profile_Plots_Window_Geometry"
	#define Fitting_Settings_Window_Geometry		"Fitting_Settings_Window_Geometry"
	#define Calculation_Settings_Window_Geometry	"Calculation_Settings_Window_Geometry"
	#define General_Settings_Window_Geometry		"General_Settings_Window_Geometry"
	#define Fits_Selector_Window_Geometry			"Fits_Selector_Window_Geometry"

	#define Multilayer_Tabs							"Multilayer_Tabs"

// structure default values
#define Structure_Default_Values_Path Pre_Path + "settings/structure_default_values.ini"
	#define Structure_Init_Values	"Structure_Init_Values"
		#define Ambient_Values		"Ambient_Values"
		#define Layer_Values		"Layer_Values"
		#define Substrate_Values	"Substrate_Values"
		#define Stack_Values		"Stack_Values"
		#define Steps				"Steps"

// precisions
#define Precisions_Path Pre_Path + "settings/precisions.ini"
	#define Precisions "Precisions"
		#define Line_Edit "Line_Edit"
		#define Thumbnail "Thumbnail"
		#define Other	  "Other"

// parameters default values
#define Parameters_Default_Values_Path Pre_Path + "settings/parameters_default_values.ini"
	#define Independent_Values  "Independent_Values"
	#define Optimization_Values "Optimization_Values"
	#define Fitting_Values		"Fitting_Values"

// units
#define Units_Path Pre_Path + "settings/units.ini"
	#define Units "Units"

// calculations
#define General_Settings_Path Pre_Path + "settings/general.ini"
	#define Threads		  "Threads"
	#define Recalculation "Recalculation"
	#define Output "Output"

// measurements
#define Measurements_Path Pre_Path + "settings/measurements.ini"
	#define Filenames "Filenames"
	#define Aperiodic "Aperiodic"

// -----------------------------------------------------------------------------------------

// optical constants

#define nk_Path		Pre_Path + "nk/"
#define f1f2_Path	Pre_Path + "f1f2/"

#define nk_Filter	"*.nk"
#define f1f2_Filter	"*.ff"

#define nk_Ext ".nk"
#define ff_Ext ".ff"

#define Vacuum "Vacuum"

// -----------------------------------------------------------------------------------------

// symbols
#define space_Sym				" "
#define Angstrom_Sym			QString(QChar(0x212B))
#define Rho_Sym					QString(QChar(0x03C1))
#define Cube_Sym				QString(QChar(0x00B3))
#define Quadro_Sym				QString(QChar(0x00B2))
#define Zero_Subscript_Sym		QString(QChar(0x2080))
#define Minus_Superscript		QString(QChar(0x207B))
#define Minus_One_Sym			Minus_Superscript+QString(QChar(0x00B9))
#define Minus_Three_Sym			Minus_Superscript+Cube_Sym
#define Power_23_Sym			Quadro_Sym+Cube_Sym
#define Sigma_Sym				QString(QChar(0x03C3))
#define Xi_Sym					QString(QChar(0x03BE))
#define Lambda_Big_Sym			QString(QChar(0x039B))
#define Omega_Big_Sym			QString(QChar(0x03A9))
#define Gamma_Sym				QString(QChar(0x03B3))
#define Epsilon_Sym				QString(QChar(0x03B5))
#define Cappa_Sym				QString(QChar(0x03F0))
#define Alpha_Sym				QString(QChar(0x03B1))
#define Beta_Sym				QString(QChar(0x03B2))
#define Zeta_Sym				QString(QChar(0x03B6))
#define Theta_Sym				QString(QChar(0x03B8))
#define Lambda_Sym				QString(QChar(0x03BB))
#define Degree_Sym				QString(QChar(0x00B0))
#define Delta_Big_Sym			QString(QChar(0x0394))
#define Delta_Small_Sym			QString(QChar(0x03B4))
#define Epsilon_Sym				QString(QChar(0x03B5))
#define Mu_Sym					QString(QChar(0x03BC))
#define Pi_Sym					QString(QChar(0x03C0))
#define Nu_Sym					QString(QChar(0x03BD))
#define Phi_Sym					QString(QChar(0x03C6))
#define Tau_Sym					QString(QChar(0x03C4))
#define Omega_Sym				QString(QChar(0x03C9))
#define Prime_Sym				QString(QChar(0x2032))
#define Double_Prime_Sym		QString(QChar(0x2033))
#define Medium_BlackCircle_Sym	QString(QChar(0x26AB))
#define Plus_Minus_Sym			QString(QChar(0x00B1))
#define Minus_Sym				QString(QChar(0x2212))
#define More_Sym				QString(QChar(0x00E9))
#define Proportional_Sym		QString(QChar(0x221D))
#define Chi2_Sym				QString(QChar(0x03C7))+QString(QChar(0x00B2))
#define Element_Sym				QString(QChar(0x2208))
#define Multiply_Sym			QString(QChar(0x00D7))
#define Infinity_Sym			QString(QChar(0x221E))

// -----------------------------------------------------------------------------------------

// magic numbers
#define POLARIZATION_TOLERANCE 0.000111111111
#define MAX_DOUBLE 1E300
#define CURVATURE_LIMIT 0.00005
#define MAX_INTEGER INT_MAX					// MAXINT
#define MAX_PRECISION 100
#define MIN_ANGULAR_RESOLUTION_POINTS 5		// minimal number of points necessary for specifying non-zero resolution
#define MIN_SPECTRAL_RESOLUTION_POINTS 5	// -||-
#define QLINEEDIT_RESIZE_MARGIN 8			// minimal white space in right side of qlineedit, pixels
#define SPINBOX_BUTTONS_RESIZE_MARGIN 17	// minimal white space in right side of qlineedit, pixels
#define QLINEEDIT_TO_QCOMBOBOX_DIFFERENCE 20// difference between qcombobox size and internal qlineedit, pixels
#define DEFAULT_COLUMN 0					// as far as we use single-column trees
#define PERMANENT_INDEPENDENT 2				// angle and wavelength items in list
#define MAX_PRECISION_USED 10				// tthumbnail and lineedit precisions
#define RAND_SHIFT	100000					// rand * RAND_SHIFT + rand
#define TABLE_FIX_WIDTH_LINE_EDIT_SHORT		50  // qLineEdit.setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT)
#define TABLE_FIX_WIDTH_LINE_EDIT_SIGMA		62
#define TABLE_FIX_WIDTH_LINE_EDIT_THICKNESS 60
#define TABLE_FIX_WIDTH_LINE_EDIT_DENSITY	57
#define TABLE_FIX_WIDTH_LINE_EDIT_LONG		64
#define TABLE_COLUMN_ELEMENTS_SHIFT 1		// current_Column += TABLE_COLUMN_ELEMENTS_SHIFT
#define TABLE_COLUMN_INTERLAYERS_SHIFT 1	// current_Column += TABLE_COLUMN_INTERLAYERS_SHIFT
#define APERIODIC_TABLE_THICKNESS_VALUE_WIDTH  70
#define APERIODIC_TABLE_DENSITY_VALUE_WIDTH    110
#define MIN_FORMULA_WIDTH_LINE_EDIT 80		// master/slave width for dependance formula
#define id_Of_Thicknesses 9					// used fore refresh/reload of corresponding line_Edits
#define TARGET_LINE_EDIT_WIDTH 65			// for target_Curve_Editor
#define TARGET_LINE_AT_FIXED_WIDTH 105		// for target_Curve_Editor
#define TARGET_LINE_RESOLUTION_WIDTH 80		// for target_Curve_Editor
#define TARGET_BEAM_INTENSITY_WIDTH 25		// for target_Curve_Editor
#define INDEPENDENT_LINE_EDIT_WIDTH 50		// for independent_Variables_Editor
#define MIN_CONFIDENCE_POINTS	3			// for confidence interval
#define COLOR_LEGEND_LABEL_WIDTH 100		// for table color legend
#define DISTRIBUTION_BOX_FIELD_WIDTH 90		// for target_Curve_Editor: distribution box

// -----------------------------------------------------------------------------------------

#define active "     Active"
#define fit_Text "fit"
#define expression_Master_Slave_Variable "x" // important! default internal dependence in regular aperiodic

#define fit_Function_Separator ";"
#define fit_Function_Variable "R"

// calc functions
#define reflectance_Function	"Reflectance"
#define symbol_R "R"
#define transmittance_Function	"Transmittance"
#define symbol_T "T"
#define absorptance_Function	"Absorptance"
#define symbol_A "A"
#define intensity_Function		"Field intensity"
#define symbol_F "F"
#define joule_Function			"Absorption map"
#define symbol_J "J"
#define scattering_Function		"Scattering"
#define symbol_S "S"
#define gisas_Function			"GISAS map"
#define symbol_G "G"

// roughness scattering approximations
#define PT_approximation	"PT approximation"
#define DWBA_approximation	"DWBA approximation"
#define SA_approximation	"SA approximation"
#define CSA_approximation	"CSA approximation"

#define full_Correlation		"full vertical correlation"
#define partial_Correlation		"partial vertical correlation"
#define zero_Correlation		"zero vertical correlation"

// PSD models
#define ABC_model				"ABC model"
#define linear_Growth_and_ABC_Model		"Linear growth model"
#define fractal_Gauss_Model		"Fractal Gauss model"

// whatsThis : delimiters
#define whats_This_Delimiter ";"
#define item_Type_Delimiter  " "

// whatsThis : treeWidgetItem types
#define item_Type_Measurement		"Measurement"
#define item_Type_Ambient			"Ambient"
#define item_Type_Layer				"Layer"
#define item_Type_Multilayer		"Multilayer"
#define item_Type_Regular_Aperiodic	"Regular Aperiodic"
#define item_Type_General_Aperiodic	"General Aperiodic"
#define item_Type_Substrate			"Substrate"

// whatsThis : specialized additions
#define whats_This_Wavelength					"Wavelength"
#define whats_This_Beam_Theta_0_Angle			"Theta_0 Angle"
#define whats_This_Detector_Theta_Angle			"Theta Angle"
#define whats_This_Detector_Phi_Angle			"Phi Angle"
#define whats_This_Absolute_Density				"Absolute Density"
#define whats_This_Relative_Density				"Relative Density"
#define whats_This_Permittivity					"Permittivity"
#define whats_This_Absorption					"Absorption"
#define whats_This_Composition					"Composition"
#define whats_This_Thickness					"Thickness"
#define whats_This_Sigma_Diffuse				"Sigma Diffuse"
#define whats_This_Sigma_Roughness				"Sigma Roughness"
#define whats_This_Correlation_Radius			"Correlation Radius"
#define whats_This_Fractal_Alpha				"Fractal Alpha"
#define whats_This_Linear_PSD_Omega				"Linear PSD Omega"
#define whats_This_Linear_PSD_Exponenta_Mu		"Linear PSD Exponenta Mu"
#define whats_This_Interlayer_Composition		"Interlayer Composition"
#define whats_This_Interlayer_My_Sigma_Diffuse	"Interlayer My Sigma"
#define whats_This_Num_Repetitions				"Num Repetitions"
#define whats_This_Period						"Period"
#define whats_This_Gamma						"Gamma"

#define whats_This_Density					"Density"
#define whats_This_Common_Thickness			"Common Thickness"
#define whats_This_Common_Sigma_Diffuse		"Common Sigma"
#define whats_This_Restrict_Thickness		"Restrict Thickness"

// whatsThis : thickness drifts
#define whats_This_Thickness_Drift_Line_Value		"Thickness Drift Lin Value"
#define whats_This_Thickness_Drift_Rand_Rms			"Thickness Drift Rand Rms"
#define whats_This_Thickness_Drift_Sine_Amplitude	"Thickness Drift Sine Amplitude"
#define whats_This_Thickness_Drift_Sine_Frequency	"Thickness Drift Sine Frequency"
#define whats_This_Thickness_Drift_Sine_Phase		"Thickness Drift Sine Phase"

#define whats_This_Thickness_Drift_Sine				"Thickness Drift Sine"
#define whats_This_Drift							"Drift"

// whatsThis : sigma drifts
#define whats_This_Sigma_Drift_Line_Value			"Sigma Drift Lin Value"
#define whats_This_Sigma_Drift_Rand_Rms				"Sigma Drift Rand Rms"
#define whats_This_Sigma_Drift_Sine_Amplitude		"Sigma Drift Sine Amplitude"
#define whats_This_Sigma_Drift_Sine_Frequency		"Sigma Drift Sine Frequency"
#define whats_This_Sigma_Drift_Sine_Phase			"Sigma Drift Sine Phase"

#define whats_This_Sigma_Drift_Sine					"Sigma Drift Sine"

// window types
#define window_Type_Launcher						"Launcher"
#define window_Type_Multilayer_Approach				"Multilayer Approach"
#define window_Type_Item_Editor						"Item Editor"
#define window_Type_Regular_Aperiodic_Table			"Regular Aperiodic Table"
#define window_Type_Table_Of_Structures				"Table Of Structures"
#define window_Type_Calculation_Settings_Editor		"Calculation Settings Editor"
#define window_Type_Fitting_Settings_Editor			"Fitting Settings Editor"

// value types
#define VAL				"value"
#define MIN				"min value"
#define MAX				"max value"

// plot scales
#define lin_Scale		"lin"
#define log_Scale		"value"
#define left			"left"
#define right			"right"

// 2D data to show
#define meas		"meas"
#define calc		"calc"

// insert items with or without changing IDs
#define copy_Type_Copy	"copy"
#define copy_Type_Cut	"cut"

// -----------------------------------------------------------------------------------------

// properties
#define id_Property				"id"
#define period_Gamma_Property	"period_Gamma"
#define min_Size_Property		"min_Size"
#define index_Property			"index"
#define TRIL_TRUE				"True"
#define TRIL_FALSE				"False"
#define TRIL_NOT_DEFINED		"NotDefined"
#define NOPARENT				"No parents"
#define previous_Expression_Property "previous_Expression"
#define abort_Property			"abort"
#define height_Property			"height_Property"

#define num_Chemic_Element_Property		"num_Chemic_Element"
#define interlayer_Index_Property		"interlayer_Index"
#define tab_Index_Property				"tab_Index"
#define row_Property					"row"
#define column_Property					"column"
#define text_Property					"text"
#define whats_This_Property				"whats_This"
#define item_Type_Property				"item_Type"
#define value_Type_Property				"value_Type"
#define coupling_Editor_Property		"coupling_Editor"
#define multilayer_Item_Table_CheckBox_Property	"multilayer_Item_Table_CheckBox"
#define regular_Aperiodic_Item_Table_CheckBox_Property	"regular_Aperiodic_Item_Table_CheckBox"
#define item_Table_CheckBox_Property	"item_Table_CheckBox"
#define previous_Value_Property			"previous_Value"
#define fit_Column_Property				"fit_Column_Property"

#define refresh_Property		"refresh"
#define reload_Property			"reload"
#define forced_Reload_Property	"forced reload"
#define colorize_Property		"colorize"
#define enabled_Property		"enabled"

// first group
#define relative_Rows_To_Disable_Start_Property		"relative_Rows_To_Disable_Start"
#define relative_Rows_To_Disable_Finish_Property	"relative_Rows_To_Disable_Finish"
#define relative_Columns_To_Disable_Start_Property	"relative_Columns_To_Disable_Start"
#define relative_Columns_To_Disable_Finish_Property	"relative_Columns_To_Disable_Finish"

// second group
#define relative_Rows_To_Disable_Start_Property_2		"relative_Rows_To_Disable_Start_2"
#define relative_Rows_To_Disable_Finish_Property_2		"relative_Rows_To_Disable_Finish_2"
#define relative_Columns_To_Disable_Start_Property_2	"relative_Columns_To_Disable_Start_2"
#define relative_Columns_To_Disable_Finish_Property_2	"relative_Columns_To_Disable_Finish_2"

// keys
#define multilayer_Approach_Key	"multilayer_Approach_Key"
#define table_Of_Structures_Key	"table_Of_Structures_Key"
#define table_Of_Roughness_Key	"table_Of_Roughness_Key"
#define optical_Graphs_1D_Key	"optical_Graphs_1D_Key"
#define optical_Graphs_2D_Key	"optical_Graphs_2D_Key"
#define profile_Plots_Key		"profile_Plots_Key"
#define fits_Selector_Key		"fits_Selector_Key"
#define calc_Settings_Key		"calc_Settings_Key"
#define general_Settings_Key	"general_Settings_Key"
#define fit_Settings_Key		"fit_Settings_Key"

// dimension
#define dim_1D	"dim_1D"
#define dim_2D	"dim_2D"

// curve types
#define INDEPENDENT	"INDEPENDENT"
#define TARGET		"TARGET"
#define TARGET_AND_INDEPENDENT "TARGET_AND_INDEPENDENT"

// profile plot types
#define PERMITTIVITY	"PERMITTIVITY"
	#define DELTA_EPS	"DELTA_EPS"
	#define BETA_EPS	"BETA_EPS"
#define MATERIAL		"MATERIAL"
#define ELEMENTS		"ELEMENTS"

// from boundaries to parametrization
#define noparam		"no parametrization"
#define triangle	"triangle"

// calculation modes
#define CALCULATION	"CALCULATION"
#define FITTING		"FITTING"
#define CONFIDENCE	"CONFIDENCE"

#define SPECULAR_MODE	"SPECULAR_MODE"
#define SCATTERED_MODE  "SCATTERED_MODE"

// name modificators
#define current_State	"current_State"
#define fitted_State	"fitted_State"
#define fit_Run_State	"fit_Run_State"

// aperiodic
#define relate_Thickness "relate_Thickness"
#define fit_Thickness	 "fit_Thickness"
#define relate_Sigma	 "relate_Sigma"
#define fit_Sigma		 "fit_Sigma"

#define material_Identifier		 "material"

// -----------------------------------------------------------------------------------------

// calculation
#define I complex<double>(0,1)
#define Na 6.022140857E23
#define Q  4.484891E-30
#define SQRT_PI 1.772453850905516

#ifndef DBL_EPSILON
#define DBL_EPSILON numeric_limits<double>::epsilon()
#endif

#ifndef DBL_MAX
#define DBL_MAX numeric_limits<double>::max()
#endif

#ifndef DBL_MIN
#define DBL_MIN numeric_limits<double>::min()
#endif
// -----------------------------------------------------------------------------------------

typedef long long id_Type ;
extern id_Type previous_ID;

class Global_Definitions
{
public:
	Global_Definitions();
	static id_Type generate_Id();

};

// -----------------------------------------------------------------------------------------

// simple types					renew corresponding serialization operators!
struct Independent_Indicator	{id_Type id = 0; id_Type item_Id = 0; QString item_Type; QString parameter_Whats_This; int index = -1; bool is_Active = false;};
struct Parameter_Indicator		{id_Type id = 0; id_Type item_Id = 0; QString whats_This;		// once and for all
								 int tab_Index = -1; QString full_Name;												// can be changed
								 QString expression = expression_Master_Slave_Variable; bool exist = false;
								};

struct Independent				{bool is_Independent = false; double min; double max; int num_Points;};

struct Parameter;
struct Coupled					{bool is_Coupled = false;
								 Parameter_Indicator master;
								 QVector<Parameter_Indicator> slaves;

								 QVector<Parameter*> slave_Pointers;		// not to store
								 void clear_Coupled()
								 {
									 slaves.clear();
									 Parameter_Indicator new_Parameter_Indicator;
									 master = new_Parameter_Indicator;
								 }
								};

struct Confidence				{bool calc_Conf_Interval = false; bool is_Active = false; double min; double max; int num_Points;};

struct Fit						{bool is_Fitable = false; bool min_Bounded/* still unused */; double min; bool max_Bounded/* still unused */; double max;};

struct Parameter				{double value; Independent independent; Coupled coupled; Confidence confidence; Fit fit;
								 Parameter_Indicator indicator;
								 Parameter()
								 {
									indicator.id = Global_Definitions::generate_Id();	// create unique id
								 }};
struct Distribution             { double FWHM_distribution = 0;
								  QString distribution_Function = "Gaussian"; // see distributions in global_variables
								  double coverage = 2; // in units of FWHM
								  bool use_Sampling = false;
								  int number_of_Samples = 1;
								};

struct Detector_1D				{ QString detector_Type;

								  // slit
								  double slit_Width;
								  double distance_To_Sample;

								  // crystal
								  Distribution detector_Theta_Resolution;
								};

struct Detector_2D				{ QString detector_Type;

								  // spherical
								  Distribution detector_Theta_Resolution;
								  Distribution detector_Phi_Resolution;
								};

struct Sample_Geometry			{ double size = 20;
								  double x_Position = 0;
								  double z_Position = 0;
								  double curvature = 0;
								};

struct Beam_Geometry			{ double size = 0.055;
								  double smoothing = 0.5;
								};


struct Int_Independent			{int start = 1; int step = 1; int num_Steps = 3;
								 Parameter parameter; // double-valued, should be rounded
								 int value() const
								 {
									 return int(round(parameter.value));
								 }
								};
struct Min_Max					{double thickness_Min = 0;		double thickness_Max = 0;
								 double sigma_Diffuse_Min = 0;  double sigma_Diffuse_Max = 0;
								};

Q_DECLARE_METATYPE( Parameter )
Q_DECLARE_METATYPE( Independent_Indicator )

struct Stoichiometry			{Parameter composition; QString type;
								 Stoichiometry()
								 {
									composition.indicator.whats_This = whats_This_Composition;
									composition.independent.min = 0;
									composition.independent.max = 1;
									composition.independent.num_Points = 1;
								 }
								};
struct Interlayer				{Parameter interlayer; Parameter my_Sigma_Diffuse; bool enabled;
								 Interlayer()
								 {
									interlayer.indicator.whats_This = whats_This_Interlayer_Composition;
									my_Sigma_Diffuse.indicator.whats_This = whats_This_Interlayer_My_Sigma_Diffuse;
								 }
								};
struct Drift					{bool is_Drift_Line;  bool show_Drift_Line; Parameter drift_Line_Value;
								 bool is_Drift_Sine;  bool show_Drift_Sine; Parameter drift_Sine_Amplitude; Parameter drift_Sine_Frequency; Parameter drift_Sine_Phase;
								 bool is_Drift_Rand;  bool show_Drift_Rand; Parameter drift_Rand_Rms;	};

// optical constant types
struct Point					{double lambda; double re;  double im; void read_Row(QTextStream& input, bool if_Factors);};
struct Material_Data			{QString substance; QString filename; QVector<Point> material_Data; void read_Material(QString& filename);};
struct Element_Data				{QString element;					  QVector<Point> element_Data;  void read_Element (QString& filename);};

// curve plot options
struct Plot_Options				{ bool rescale = true;

								  // 1D
								  QString x_Scale = lin_Scale;
								  QString y_Scale = log_Scale;
								  QColor color = QColor(255, 0, 0);
								  int scatter_Shape = QCPScatterStyle::ssCircle;
								  double scatter_Size = 5;
								  double thickness = 1;

								  // 2D
								  bool use_Interpolation = true;
								  QString z_Scale = lin_Scale;
								  QCPColorGradient::GradientPreset color_Scheme = QCPColorGradient::gpJet;
								  int rotation_Angle = 0;
								  bool left_Section_Plot = false;
								  bool bottom_Section_Plot = false;
								  int bottom_Section_Tab_Index = 0;
								  QString data_To_Show = meas;
								  double orders_To_Show = 9;
								};
struct Graph_Options			{int num_Target_Graph_Rows = 1;		 // rows in Graphs
								 int num_Independent_Graph_Rows = 1; // rows in Graphs

								 bool show_Current_Coordinate = true;
								 bool show_Title = true;

								 // 1D
								 bool show_Scatter = false;
								 bool show_Thickness = false;
								 bool show_X_Scale = false;
								 bool show_Max_Value = false;

								 // 2D
								 bool show_Interpolation = false;
								};
struct Graph_2D_Positions		{
								 double left_Min_Width = 130;
								 double bottom_Min_Width = 130;
								 double x_Mark = -MAX_DOUBLE;
								 double y_Mark = -MAX_DOUBLE;
								 QPoint current_Pos;
								};

// profile plot options
struct Profile_Plot_Options		{QString type = PERMITTIVITY;
								 QString permittivity_Type = DELTA_EPS;
								 bool apply_Roughness = true;
								 bool apply_Diffuseness = true;
								 bool show_Sharp_Profile = true;
								 bool show_Discretization = false;
								 bool show_Cursor_Position = false;

								 bool rescale_X = true;
								 bool rescale_Y = true;
								 QString y_Scale = lin_Scale;

								 double old_X_Begin = -10;
								 double old_X_End   = +200;
								 double old_Y_Begin = 0;
								 double old_Y_End   = 1;

								 QString local_wavelength_units = Angstrom_Sym;
								 QString local_length_units = Angstrom_Sym;

								 double local_Wavelength = 1.540562;
								};

// calculated values for profile plotting
struct Material_Profile			{QString material;
								 QColor color;
								 QVector<double> relative_Concentration;
								};
struct Element_Profile			{QString element;
								 QColor color;
								 QVector<double> absolute_Concentration;
								};
struct Basic_Profile_Plot_Set	{QVector<double> depth; // in angstroms
								 QVector<double> delta_Epsilon;
								 QVector<double> beta_Epsilon;
								 QVector<Material_Profile> materials;
								 QVector<Element_Profile> elements;

								 void clear_And_Resize_All(int new_Size)
								 {
									depth.clear();			depth.resize(new_Size);
									delta_Epsilon.clear();	delta_Epsilon.resize(new_Size);
									beta_Epsilon.clear();	beta_Epsilon.resize(new_Size);
									materials.clear();		materials.resize(new_Size);
									elements.clear();		elements.resize(new_Size);
								 }
								};


struct Different_Norm_Layer
{
	Different_Norm_Layer()
	{
//		interlayer_Composition_Weight_Left.resize(transition_Layer_Functions_Size);
//		interlayer_My_Sigma_Left.resize(transition_Layer_Functions_Size);
//		interlayer_Composition_Weight_Right.resize(transition_Layer_Functions_Size);
//		interlayer_My_Sigma_Right.resize(transition_Layer_Functions_Size);
	}

//	void set_Interlayer(QVector<Interlayer>& interlayer_Composition, QString side)
//	{
//		for(int i=0; i<transition_Layer_Functions_Size; i++)
//		{
//			if(interlayer_Composition[i].enabled)
//			{
//				if(side == "left")
//				{
//					interlayer_Composition_Weight_Left[i] = interlayer_Composition[i].interlayer.value;
//					interlayer_My_Sigma_Left[i] = interlayer_Composition[i].my_Sigma.value;
//				}
//				if(side == "right")
//				{
//					interlayer_Composition_Weight_Right[i] = interlayer_Composition[i].interlayer.value;
//					interlayer_My_Sigma_Right[i] = interlayer_Composition[i].my_Sigma.value;
//				}
//			}
//		}
//	}

//	QVector<double> interlayer_Composition_Weight_Left;
//	QVector<double> interlayer_My_Sigma_Left;
//	QVector<double> interlayer_Composition_Weight_Right;
//	QVector<double> interlayer_My_Sigma_Right;

	// only checking
	double thickness = -2019;
	double sigma_Diffuse_Left = -2019;
	double sigma_Diffuse_Right = -2019;

	double norm = -2019;
};

// calculated functions for plotting WITHOUT SAVING
struct Calculated_Values	  { // reflectance
								vector<double> Phi_R_s;
								vector<double> Phi_R_p;
								vector<double> R_s;
								vector<double> R_p;
								vector<double> R;
								vector<double> R_Instrumental;

								// transmittance
								vector<double> Phi_T_s;
								vector<double> Phi_T_p;
								vector<double> T_s;
								vector<double> T_p;
								vector<double> T;
								vector<double> T_Instrumental;

								// absorptance
								vector<double> A_s;
								vector<double> A_p;
								vector<double> A;

								// field intensity in-depth
								vector<vector<double>> field_Intensity;
								vector<vector<double>> absorption_Map;

								// scattering
								vector<double> S_s;
								vector<double> S_p;
								vector<double> S;
								vector<double> S_Instrumental;

								// GISAS
//								vector<vector<double>> GISAS_Map_s;				// to expensive to handle
//								vector<vector<double>> GISAS_Map_p;				// to expensive to handle
								vector<vector<double>> GISAS_Map;				// for preliminary calculations
								vector<vector<double>> GISAS_Instrumental;


								// storing field parameters on boundaries at each point
								/// for DWBA SA CSA
								vector<vector<complex<double>>> q0_U_i_s;
								vector<vector<complex<double>>> q0_U_r_s;
								vector<vector<complex<double>>> q0_U_i_p;
								vector<vector<complex<double>>> q0_U_r_p;

								vector<vector<complex<double>>> q_U_i_s;
								vector<vector<complex<double>>> q_U_r_s;
								vector<vector<complex<double>>> q_U_i_p;
								vector<vector<complex<double>>> q_U_r_p;

								vector<vector<complex<double>>> q0_Hi;
								vector<vector<complex<double>>> q_Hi;

								vector<vector<complex<double>>> q0_Exponenta;
								vector<vector<complex<double>>> q_Exponenta;
							   };
struct Simple_Curve		       {
								Simple_Curve();
								Plot_Options plot_Options;// without saving
								vector<double> argument;// without saving
								vector<double> values;	// without saving

								void read_Simple_Curve(QString bare_Filename);
							   };
struct Discretization_Parameters{
								 bool enable_Discretization = false;
								 double discretization_Step = 1.0;
								};
struct Roughness_Model			{
								bool is_Enabled = false;
								Parameter sigma;
								Parameter cor_radius;
								Parameter fractal_alpha;
								Parameter omega;
								Parameter mu;
								};
struct Imperfections_Model		{
								// interlayer
								bool use_Interlayer = true;
								QVector<bool> use_Func;

								Imperfections_Model() :
									use_Func(6)
								{
									use_Func[0] = true;
									use_Func[1] = false;
									use_Func[2] = false;
									use_Func[3] = false;
									use_Func[4] = false;
									use_Func[5] = false;
								}

								// drift
								bool show_Drift = true;

								bool show_Thickness_Drift_Line = true;
								bool show_Thickness_Drift_Rand = false;
								bool show_Thickness_Drift_Sine = false;

								bool show_Sigma_Drift_Line = false;
								bool show_Sigma_Drift_Rand = false;
								bool show_Sigma_Drift_Sine = false;

								// rougness
								bool use_Roughness = false;

								QString approximation = PT_approximation;
								QString common_Model = ABC_model;
								QString vertical_Correlation = full_Correlation;
								bool use_Common_Roughness_Function = true;

								// density fluctuations
								bool use_Fluctuations = false;
								};

// independent calculation functions
struct Calc_Functions			{bool check_Enabled = true;
								 // all types of data, target or independent should be "interpreted" according to this list
								 // 1D
								 bool check_Reflectance = false;
								 bool check_Transmittance = false;
								 bool check_Absorptance = false;
								 bool check_Scattering = false;

								 // 2D
								 bool check_Field = false;
								 bool check_Joule = false;
								 bool check_GISAS = false;

								 double field_Step = 2;
								 double field_Ambient_Distance = 0;
								 double field_Substrate_Distance = 0;
								 bool add_Specular_Peak = true;
								 bool instrumental_Smoothing = true;

								 bool if_Something_Enabled_1D() const;
								 bool if_Something_Enabled_2D() const;
								 bool if_Reflectance_Only() const;
								};

struct Fitables					{	vector<QString> struct_Names;		// names of structures
									vector<QString> param_Names;		// names of parameters to be fitted
									QVector<id_Type>param_IDs;			// ID of parameters to be fitted

									// changeable
									vector<Parameter*> param_Pointers;
                                    vector<double>	values;
									vector<double>	values_Parametrized;	// unbounded parametrized variables
									vector<tree<Node>::iterator> parent_Iterators;

									void clear_All()
									{
										struct_Names.clear();
										param_Names.clear();
										param_IDs.clear();

										param_Pointers.clear();
										values.clear();
										values_Parametrized.clear();
										parent_Iterators.clear();
									}
								};

struct Fitted_Structure			{	QString name;
									QVector<QTreeWidget*> fitted_Trees;
								};

struct Aperiodic_Settings
								{
									bool contin = false;
									QString column_4 = "";
									QString column_5 = "";
									QString length_Units = "";
								};

// -----------------------------------------------------------------------------------------
// DEPRECATED
struct Old_Calculated_Values	{QVector<double> R;
								 QVector<double> Phi_R;

								 QVector<double> T;
								 QVector<double> A;
								};
// -----------------------------------------------------------------------------------------
// serialization

QDataStream& operator <<( QDataStream& stream, const Independent_Indicator& independent_Indicator );
QDataStream& operator >>( QDataStream& stream,		 Independent_Indicator& independent_Indicator );

QDataStream& operator <<( QDataStream& stream, const Parameter_Indicator& parameter_Indicator );
QDataStream& operator >>( QDataStream& stream,		 Parameter_Indicator& parameter_Indicator );

QDataStream& operator <<( QDataStream& stream, const Int_Independent& int_Independent );
QDataStream& operator >>( QDataStream& stream,		 Int_Independent& int_Independent );

QDataStream& operator <<( QDataStream& stream, const Min_Max& min_Max );
QDataStream& operator >>( QDataStream& stream,		 Min_Max& min_Max );

QDataStream& operator <<( QDataStream& stream, const Independent& independent );
QDataStream& operator >>( QDataStream& stream,		 Independent& independent );

QDataStream& operator <<( QDataStream& stream, const Coupled& coupled );
QDataStream& operator >>( QDataStream& stream,		 Coupled& coupled );

QDataStream& operator <<( QDataStream& stream, const Fit& fit );
QDataStream& operator >>( QDataStream& stream,		 Fit& fit );

QDataStream& operator <<( QDataStream& stream, const Confidence& confidence );
QDataStream& operator >>( QDataStream& stream,		 Confidence& confidence );

QDataStream& operator <<( QDataStream& stream, const Parameter& parameter );
QDataStream& operator >>( QDataStream& stream,		 Parameter& parameter );

QDataStream& operator <<( QDataStream& stream, const Distribution& resolution );
QDataStream& operator >>( QDataStream& stream,		 Distribution& resolution );

QDataStream& operator <<( QDataStream& stream, const Detector_1D& detector_1D );
QDataStream& operator >>( QDataStream& stream,		 Detector_1D& detector_1D );

QDataStream& operator <<( QDataStream& stream, const Detector_2D& detector_2D );
QDataStream& operator >>( QDataStream& stream,		 Detector_2D& detector_2D );

QDataStream& operator <<( QDataStream& stream, const Sample_Geometry& sample_Geometry );
QDataStream& operator >>( QDataStream& stream,		 Sample_Geometry& sample_Geometry );

QDataStream& operator <<( QDataStream& stream, const Beam_Geometry& beam_Geometry );
QDataStream& operator >>( QDataStream& stream,		 Beam_Geometry& beam_Geometry );

QDataStream& operator <<( QDataStream& stream, const Stoichiometry& stoichiometry );
QDataStream& operator >>( QDataStream& stream,		 Stoichiometry& stoichiometry );

QDataStream& operator <<( QDataStream& stream, const Interlayer& interlayer );
QDataStream& operator >>( QDataStream& stream,		 Interlayer& interlayer );

QDataStream& operator <<( QDataStream& stream, const Drift& drift );
QDataStream& operator >>( QDataStream& stream,		 Drift& drift );

QDataStream& operator <<( QDataStream& stream, const Old_Calculated_Values& old_Calculated_Values );
QDataStream& operator >>( QDataStream& stream,		 Old_Calculated_Values& old_Calculated_Values );

QDataStream& operator <<( QDataStream& stream, const Discretization_Parameters& discretization_Parameters );
QDataStream& operator >>( QDataStream& stream,		 Discretization_Parameters& discretization_Parameters );

QDataStream& operator <<( QDataStream& stream, const Roughness_Model& roughness_Model );
QDataStream& operator >>( QDataStream& stream,		 Roughness_Model& roughness_Model );

QDataStream& operator <<( QDataStream& stream, const Imperfections_Model& imperfections_Model );
QDataStream& operator >>( QDataStream& stream,		 Imperfections_Model& imperfections_Model );

QDataStream& operator <<( QDataStream& stream, const Calc_Functions& calc_Functions );
QDataStream& operator >>( QDataStream& stream,		 Calc_Functions& calc_Functions );

QDataStream& operator <<( QDataStream& stream, const Plot_Options& plot_Options );
QDataStream& operator >>( QDataStream& stream,		 Plot_Options& plot_Options );

QDataStream& operator <<( QDataStream& stream, const Graph_Options& graph_options );
QDataStream& operator >>( QDataStream& stream,		 Graph_Options& graph_options );

QDataStream& operator <<( QDataStream& stream, const Graph_2D_Positions& graph_2D_Positions );
QDataStream& operator >>( QDataStream& stream,		 Graph_2D_Positions& graph_2D_Positions );

QDataStream& operator <<( QDataStream& stream, const Profile_Plot_Options& profile_Plot_Options );
QDataStream& operator >>( QDataStream& stream,		 Profile_Plot_Options& profile_Plot_Options );

QDataStream& operator <<( QDataStream& stream, const Material_Profile& material_Profile );
QDataStream& operator >>( QDataStream& stream,		 Material_Profile& material_Profile );

QDataStream& operator <<( QDataStream& stream, const Element_Profile& element_Profile );
QDataStream& operator >>( QDataStream& stream,		 Element_Profile& element_Profile );

QDataStream& operator <<( QDataStream& stream, const Basic_Profile_Plot_Set& basic_Profile_Plot_Set );
QDataStream& operator >>( QDataStream& stream,		 Basic_Profile_Plot_Set& basic_Profile_Plot_Set );

// -----------------------------------------------------------------------------------------

bool operator ==( const Parameter_Indicator& parameter_Indicator_Left, const Parameter_Indicator& parameter_Indicator_Right );
bool operator ==( const Different_Norm_Layer& different_Norm_Layer_Left, const Different_Norm_Layer& different_Norm_Layer_Right );

//template <typename T>
//void print_Vector	(QString name, vector<T>& vec, int transpose)	// output
//{
//	if(vec.size()==0) return;
//	if(transpose==0)
//	{
//		cout<<name.toStdString()<<"[0.."<<vec.size()-1<<"] = "; //-V128
//		for(auto i=0; i<vec.size(); ++i) //-V104
//			cout<<vec[i]<<"\t";
//	} else
//	for(auto i=0; i<vec.size(); ++i) //-V104
//	{
//		cout<<name.toStdString()<<"["<<i<<"] = ";
//		cout<<vec[i]<<endl;
//	}
//	cout<<endl;
//}

#endif // GLOBAL_DEFINITIONS_H
