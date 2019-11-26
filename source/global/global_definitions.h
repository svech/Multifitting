#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <QtWidgets>
#include <vector>
#include <complex>
#include "tree.hh"
#include "qcustomplot.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 10
#define VERSION_BUILD 1

using namespace std;
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
	#define Structure_Table_Window_Geometry			"Structure_Table_Window_Geometry"
	#define Regular_Aperiodic_Table_Window_Geometry	"Regular_Aperiodic_Table_Window_Geometry"
	#define Graphs_Window_Geometry					"Graphs_Window_Geometry"
	#define Fitting_Settings_Window_Geometry		"Fitting_Settings_Window_Geometry"
	#define Calculation_Settings_Window_Geometry	"Calculation_Settings_Window_Geometry"
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
#define Calculations_Path Pre_Path + "settings/calculations.ini"
	#define Threads		  "Threads"
	#define Recalculation "Recalculation"

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
#define Minus_One_Sym			QString(QChar(0x207B))+QString(QChar(0x00B9))
#define Sigma_Sym				QString(QChar(0x03C3))
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
// -----------------------------------------------------------------------------------------

// magic numbers
#define MAX_DOUBLE 1E300
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
#define TABLE_FIX_WIDTH_LINE_EDIT_SIGMA		56
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
#define TARGET_LINE_AT_FIXED_WIDTH 85		// for target_Curve_Editor
#define TARGET_BEAM_INTENSITY_WIDTH 25		// for target_Curve_Editor
#define INDEPENDENT_LINE_EDIT_WIDTH 50		// for independent_Variables_Editor
#define MIN_CONFIDENCE_POINTS	3			// for confidence interval
#define COLOR_LEGEND_LABEL_WIDTH 100		// for table color legend

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
#define intensity_Function		"Field Intensity"
#define symbol_F "F"
#define joule_Function			"Joule Absorption"
#define symbol_J "J"
#define user_Function			""
#define symbol_U "U"

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
#define whats_This_Angle					"Angle"
#define whats_This_Angular_Resolution		"Angular Resolution"
#define whats_This_Wavelength				"Wavelength"
#define whats_This_Spectral_Resolution		"Spectral Resolution"
#define whats_This_Polarization				"Polarization"
#define whats_This_Polarization_Sensitivity	"Polarization Sensitivity"
#define whats_This_Absolute_Density			"Absolute Density"
#define whats_This_Relative_Density			"Relative Density"
#define whats_This_Permittivity				"Permittivity"
#define whats_This_Absorption				"Absorption"
#define whats_This_Composition				"Composition"
#define whats_This_Thickness				"Thickness"
#define whats_This_Sigma					"Sigma"
#define whats_This_Interlayer_Composition	"Interlayer Composition"
#define whats_This_Interlayer_My_Sigma		"Interlayer My Sigma"
#define whats_This_Num_Repetitions			"Num Repetitions"
#define whats_This_Period					"Period"
#define whats_This_Gamma					"Gamma"

#define whats_This_Background				"Background"
#define whats_This_Beam_Size				"Beam Size"
#define whats_This_Beam_Profile_Spreading	"Beam Profile Spreading"
#define whats_This_Sample_Size				"Sample Size"
#define whats_This_Sample_Shift				"Sample Shift"

#define whats_This_Density					"Density"
#define whats_This_Common_Thickness			"Common Thickness"
#define whats_This_Common_Sigma				"Common Sigma"
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
#define window_Type_Independent_Variables_Editor	"Independent Variable Editor"
#define window_Type_Item_Editor						"Item Editor"
#define window_Type_Regular_Aperiodic_Table			"Regular_Aperiodic_Table"
#define window_Type_Table							"Table"
#define window_Type_Calculation_Settings_Editor		"Calculation Settings Editor"
#define window_Type_Fitting_Settings_Editor			"Fitting Settings Editor"

// angle types
#define angle_Type_Grazing		"Grazing"
#define angle_Type_Incidence	"Incidence"

// value types
#define VAL				"value"
#define MIN				"min value"
#define MAX				"max value"

// plot scales
#define lin_Scale		"lin"
#define log_Scale		"value"
#define left_Axis		"left"
#define right_Axis		"right"

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
#define table_Key				"table_Of_Structures_Key"
#define optical_Graphs_Key		"optical_Graphs_Key"
#define fits_Selector_Key		"fits_Selector_Key"
#define calc_Settings_Key		"calc_Settings_Key"
#define fit_Settings_Key		"fit_Settings_Key"

// curves types
#define INDEPENDENT	"INDEPENDENT"
#define TARGET		"TARGET"
#define TARGET_AND_INDEPENDENT "TARGET_AND_INDEPENDENT"

// from boundaries to parametrization
#define noparam		"no parametrization"
#define triangle	"triangle"

// calculation modes
#define CALCULATION	"CALCULATION"
#define FITTING		"FITTING"
#define CONFIDENCE	"CONFIDENCE"

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

struct Int_Independent			{int start = 1; int step = 1; int num_Steps = 3;
								 Parameter parameter; // double-valued, should be rounded
								 int value() const
								 {
									 return int(round(parameter.value));
								 }
								};
struct Min_Max					{double thickness_Min = 0; double thickness_Max = 0;
								 double sigma_Min = 0;     double sigma_Max = 0;
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
struct Interlayer				{Parameter interlayer; Parameter my_Sigma; bool enabled;
								 Interlayer()
								 {
									interlayer.indicator.whats_This = whats_This_Interlayer_Composition;
									my_Sigma.indicator.whats_This = whats_This_Interlayer_My_Sigma;
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
struct Plot_Options				{QString x_Scale = lin_Scale; //
								 QString y_Scale = lin_Scale;
								 bool rescale = true;

								 QColor color = QColor(255, 0, 0);
								 int scatter_Shape = QCPScatterStyle::ssCircle;
								 double scatter_Size = 5;
								 double thickness = 1;

								 QString scale_Second = lin_Scale;
								 QColor color_Second = QColor(0, 0, 255);
								 int scatter_Shape_Second = QCPScatterStyle::ssDiamond;
								 double scatter_Size_Second = 5;
								 double thickness_Second = 1;
								};

struct Graph_Options			{int num_Target_Graph_Rows = 1;		 // rows in Graphs
								 int num_Independent_Graph_Rows = 1; // rows in Graphs

								 bool show_Scatter = false;
								 bool show_Thickness = false;
								 bool show_X_Scale = false;
								 bool show_Max_Value = false;
								 bool show_Current_Coordinate = true;
								 bool show_Title = true;
								};

// calculated functions for plotting
struct Calculated_Values	    {QVector<double> R;
								 QVector<double> Phi_R;

								 QVector<double> T;
								 QVector<double> A;

								 void clear_All()
								 {
									 R.clear();
									 Phi_R.clear();

									 T.clear();
									 A.clear();
								 }
								};

// independent calculation functions
struct Calc_Functions			{bool check_Enabled = true;
								 bool check_Reflectance = true;
								 bool check_Transmittance = false;
								 bool check_Absorptance = false;
								 bool check_Field = false;
								 bool check_Joule = false;
								 bool check_User = false;
								 QString user_Functions = "R+T; cos(A)*R^3-sqrt(J) ; pow(F, log(root(3,5)))";

								 // cppcheck-suppress functionConst
								 bool if_Something_Enabled()
								 {
									 if( check_Reflectance	||
										 check_Transmittance||
										 check_Absorptance	||
										 check_Field		||
										 check_Joule		||
										 check_User			)
									 {
										 return true;
									 } else
									 {
										return false;
									 }
								 }
								};

// measured/target data types
struct Value					{double val_1; double val_2; /*double val_3; double val_4;*/};
struct Curve					{bool use_Subinterval = false;
								 double subinterval_Start = 0;
								 double subinterval_End = 5;
								 QVector<double> argument; QVector<double> shifted_Argument;
								 QVector<Value> values;	   QVector<Value> shifted_Values;
								 double arg_Offset = 0; double arg_Factor = 1; bool divide_On_Beam_Intensity = true;
								 double val_Offset = 0; double val_Factor = 1; double beam_Intensity_Start = 1; double beam_Intensity_Final = 1;
								 QString argument_Type;
								 QString angle_Type;
								 QString angular_Units;
								 QString spectral_Units;

								 QString value_Function;
								 QString value_Mode;
								};
struct Fit_Params				{bool calc = true;
								 bool fit = true;
								 bool norm = true;
								 bool maximize_Integral = false;
								 double weight = 1;
								 double weight_Sqrt = 1;
								 QString fit_Function = "log("+QString(fit_Function_Variable)+"+1E-5)";// "log(R+1E-5); sin(phi/2)";
								 double power = 2;
								 bool use_Chi2 = false;

								 double expression_Argument;							// not to store

							#ifdef EXPRTK
								 QVector<exprtk::expression<double>> expression_Vec;	// not to store
							#else
								 QVector<QString> expression_Vec;						// not to store
							#endif

								 void create_Expressions_for_Residual()
								 {
									expression_Vec.clear();

							#ifdef EXPRTK
									exprtk::parser<double> parser;
									exprtk::symbol_table<double> symbol_table;
									symbol_table.add_variable(fit_Function_Variable, expression_Argument);
									symbol_table.add_constants();
							#endif

									QVector<QString> bare_String_Expressions = fit_Function.split(fit_Function_Separator, QString::SkipEmptyParts).toVector();
									for(int i=0; i<bare_String_Expressions.size(); ++i)
									{
										if(!bare_String_Expressions[i].split(" ", QString::SkipEmptyParts).empty())
										{
							#ifdef EXPRTK
											exprtk::expression<double> new_Expression;
							#else
											QString new_Expression;
							#endif
											expression_Vec.append(new_Expression);
							#ifdef EXPRTK
											expression_Vec[i].register_symbol_table(symbol_table);
											parser.compile(bare_String_Expressions[i].toStdString(), expression_Vec[i]);
							#endif
										}
									}
								 }
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

QDataStream& operator <<( QDataStream& stream, const Stoichiometry& stoichiometry );
QDataStream& operator >>( QDataStream& stream,		 Stoichiometry& stoichiometry );

QDataStream& operator <<( QDataStream& stream, const Interlayer& interlayer );
QDataStream& operator >>( QDataStream& stream,		 Interlayer& interlayer );

QDataStream& operator <<( QDataStream& stream, const Drift& drift );
QDataStream& operator >>( QDataStream& stream,		 Drift& drift );

QDataStream& operator <<( QDataStream& stream, const Calculated_Values& calculated_Values );
QDataStream& operator >>( QDataStream& stream,		 Calculated_Values& calculated_Values );

QDataStream& operator <<( QDataStream& stream, const Calc_Functions& calc_Functions );
QDataStream& operator >>( QDataStream& stream,		 Calc_Functions& calc_Functions );

QDataStream& operator <<( QDataStream& stream, const Plot_Options& plot_Options );
QDataStream& operator >>( QDataStream& stream,		 Plot_Options& plot_Options );

QDataStream& operator <<( QDataStream& stream, const Graph_Options& graph_options );
QDataStream& operator >>( QDataStream& stream,		 Graph_Options& graph_options );

QDataStream& operator <<( QDataStream& stream, const Value& value );
QDataStream& operator >>( QDataStream& stream,		 Value& value );

QDataStream& operator <<( QDataStream& stream, const Curve& curve );
QDataStream& operator >>( QDataStream& stream,		 Curve& curve );

QDataStream& operator <<( QDataStream& stream, const Fit_Params& fit_Params );
QDataStream& operator >>( QDataStream& stream,		 Fit_Params& fit_Params );
// -----------------------------------------------------------------------------------------

bool operator ==( const Parameter_Indicator& parameter_Indicator_Left, const Parameter_Indicator& parameter_Indicator_Right );

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
