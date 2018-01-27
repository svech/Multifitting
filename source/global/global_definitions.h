#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <QtWidgets>
#include <vector>
#include <complex>
#include "tree.hh"
#include "exprtk.hpp"

#ifdef __linux__
	#include <iostream>
#endif

using namespace std;

class Node;

// -----------------------------------------------------------------------------------------

// settings

#ifdef _WIN32
	#define Pre_Path QString("../../")
#endif
#ifdef __linux__
	#define Pre_Path QString("../")
#endif

// gui
#define Gui_Settings_Path Pre_Path + "settings/gui.ini"

	#define Application_Style			"Application_Style"
	#define Launcher_Geometry			"Launcher_Geometry"
	#define Multilayer_Window_Geometry	"Multilayer_Window_Geometry"
	#define Multilayer_Tabs				"Multilayer_Tabs"
	#define Resource_Paths				"Resource_Paths"

// structure default values
#define Structure_Default_Values_Path Pre_Path + "settings/structure_default_values.ini"
	#define Structure_Init_Values	"Structure_Init_Values"
		#define Ambient_Values		"Ambient_Values"
		#define Layer_Values		"Layer_Values"
		#define Substrate_Values	"Substrate_Values"
		#define Stack_Values		"Stack_Values"

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
	#define Threads "Threads"

// measurements
#define Measurements_Path Pre_Path + "settings/measurements.ini"
	#define Filenames "Filenames"

// -----------------------------------------------------------------------------------------

// optical constants

#define nk_Path		Pre_Path + "nk/"
#define f1f2_Path	Pre_Path + "f1f2/"

#define nk_Filter	"*.nk"
#define f1f2_Filter	"*.ff"

#define nk_Ext ".nk"
#define ff_Ext ".ff"

// -----------------------------------------------------------------------------------------

// symbols
#define space					" "
#define Angstrom_Sym			QString(QChar(0x212B))
#define Rho_Sym					QString(QChar(0x03C1))
#define Cube_Sym				QString(QChar(0x00B3))
#define Minus_One_Sym			QString(QChar(0x207B))+QString(QChar(0x00B9))
#define Sigma_Sym				QString(QChar(0x03C3))
#define Gamma_Sym				QString(QChar(0x03B3))
#define Epsilon_Sym				QString(QChar(0x03B5))
#define Cappa_Sym				QString(QChar(0x03F0))
#define Alpha_Sym				QString(QChar(0x03B1))
#define Zeta_Sym				QString(QChar(0x03B6))
#define Theta_Sym				QString(QChar(0x03B8))
#define Lambda_Sym				QString(QChar(0x03BB))
#define Degree_Sym				QString(QChar(0x00B0))
#define Delta_Big_Sym			QString(QChar(0x0394))
#define Mu_Sym					QString(QChar(0x03BC))
#define Pi_Sym					QString(QChar(0x03C0))
#define Nu_Sym					QString(QChar(0x03BD))
#define Phi_Sym					QString(QChar(0x03C6))
#define Prime_Sym				QString(QChar(0x2032))
#define Double_Prime_Sym		QString(QChar(0x2033))
#define Medium_BlackCircle_Sym	QString(QChar(0x26AB))

// -----------------------------------------------------------------------------------------

// magic numbers
#define MAX_DOUBLE 1E300
#define MAX_INTEGER INT_MAX					// MAXINT
#define MAX_PRECISION 100
#define MIN_ANGULAR_RESOLUTION_POINTS 5		// minimal number of points necessary for specifying non-zero resolution
#define MIN_SPECTRAL_RESOLUTION_POINTS 5	// -||-
#define QLINEEDIT_RESIZE_MARGIN 7			// minimal white space in right side of qlineedit, pixels
#define QLINEEDIT_TO_QCOMBOBOX_DIFFERENCE 20// difference between qcombobox size and internal qlineedit, pixels
#define DEFAULT_COLUMN 0					// as far as we use single-column trees
#define PERMANENT_INDEPENDENT 2				// angle and wavelength items in list
#define MAX_PRECISION_USED 10				// tthumbnail and lineedit precisions
#define RAND_SHIFT	100000					// rand * RAND_SHIFT + rand
#define TABLE_FIX_WIDTH_LINE_EDIT 50		// qLineEdit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT)
#define TABLE_COLUMN_ELEMENTS_SHIFT 1		// current_Column += TABLE_COLUMN_ELEMENTS_SHIFT
#define TABLE_COLUMN_INTERLAYERS_SHIFT 1	// current_Column += TABLE_COLUMN_INTERLAYERS_SHIFT

#define MIN_FORMULA_WIDTH_LINE_EDIT 80		// master/slave width for dependance formula
#define id_Of_Thicknesses 2017				// used fore refresh/reload of corresponding line_Edits

// -----------------------------------------------------------------------------------------

#define active "     Active"
//#define	stop_Calculation "stop_Calculation"
#define fit_Text "fit"
#define expression_Master_Slave_Variable "x"

#define fit_Function_Separator ";"
#define fit_Function_Variable "x"

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
#define item_Type_Measurement	"Measurement"
#define item_Type_Ambient		"Ambient"
#define item_Type_Layer			"Layer"
#define item_Type_Multilayer	"Multilayer"
#define item_Type_Substrate		"Substrate"

// whatsThis : specialized additions
#define whats_This_Angle					"Angle"
#define whats_This_Wavelength				"Wavelength"
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

// whatsThis : thickness drifts
#define whats_This_Thickness_Drift_Line_Value		"Thickness Drift Lin Value"
#define whats_This_Thickness_Drift_Rand_Rms			"Thickness Drift Rand Rms"
#define whats_This_Thickness_Drift_Sine				"Thickness Drift Sine"
#define whats_This_Thickness_Drift_Sine_Amplitude	"Thickness Drift Sine Amplitude"
#define whats_This_Thickness_Drift_Sine_Frequency	"Thickness Drift Sine Frequency"
#define whats_This_Thickness_Drift_Sine_Phase		"Thickness Drift Sine Phase"

// whatsThis : sigma drifts
#define whats_This_Sigma_Drift_Line_Value			"Sigma Drift Lin Value"
#define whats_This_Sigma_Drift_Rand_Rms				"Sigma Drift Rand Rms"
#define whats_This_Sigma_Drift_Sine					"Sigma Drift Sine"
#define whats_This_Sigma_Drift_Sine_Amplitude		"Sigma Drift Sine Amplitude"
#define whats_This_Sigma_Drift_Sine_Frequency		"Sigma Drift Sine Frequency"
#define whats_This_Sigma_Drift_Sine_Phase			"Sigma Drift Sine Phase"

// window types
#define window_Type_Launcher						"Launcher"
#define window_Type_Multilayer_Approach				"Multilayer Approach"
#define window_Type_Independent_Variables_Editor	"Independent Variable Editor"
#define window_Type_Item_Editor						"Item Editor"
#define window_Type_Table							"Table"
#define window_Type_Calculation_Settings_Editor		"Calculation Settings Editor"

// angle types
#define angle_Type_Grazing		"Grazing"
#define angle_Type_Incidence	"Incidence"

// value types
#define VAL				"value"
#define MIN				"min value"
#define MAX				"max value"

// insert items with or without changing IDs
#define copy_Type_Copy	"copy"
#define copy_Type_Cut	"cut"

// -----------------------------------------------------------------------------------------

// properties
#define min_Size_Property	"min_Size"
#define index_Property		"index"
#define TRIL_TRUE			"True"
#define TRIL_FALSE			"False"
#define TRIL_NOT_DEFINED	"NotDefined"
#define NOPARENT			"No parents"
#define previous_Expression_Property "previous_Expression"

#define num_Chemic_Element_Property "num_Chemic_Element"
#define interlayer_Index_Property	"interlayer_Index"
#define tab_Index_Property			"tab_Index"
#define row_Property				"row"
#define column_Property				"column"
#define text_Property				"text"
#define whats_This_Property			"whats_This"
#define item_Type_Property			"item_Type"
#define value_Type_Property			"value_Type"
#define parameter_Property			"parameter"
#define coupling_Editor_Property	"coupling_Editor"

#define reload_Property		"reload"
#define refresh_Property	"refresh"
//#define table_Index_Property "table_Index"
//#define master_Reload_Property "master_Reload"
//#define slave_Reload_Property  "slave_Reload"

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
#define table_Key			"table_Of_Structures"
#define calc_Settings_Key	"calc_Settings_Key"

// curves types
#define INDEPENDENT	"INDEPENDENT"
#define TARGET		"TARGET"

// from boundaries to parametrization
#define noparam		"no parametrization"
#define triangle	"triangle"

// calculation modes
#define CALCULATION	"CALCULATION"
#define FITTING		"FITTING"

// -----------------------------------------------------------------------------------------

// calculation
#define I complex<double>(0,1)
#define Na 6.022140857E23
#define Q 4.484891E-30

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

extern unsigned long long int previous_ID;

class Global_Definitions
{
public:
	Global_Definitions();
	static unsigned long long int generate_Id();
};

// -----------------------------------------------------------------------------------------

// simple types					renew corresponding serialization operators!
struct Independent_Indicator	{int item_Id = 0; int parameter_Id = 0; QString item_Type; QString parameter_Whats_This; int index = -1; bool is_Active = false;};
struct Parameter_Indicator		{unsigned long long int id = 0; QString whats_This;								// once and for all
								 int tab_Index = -1; QString full_Name;			// can be changed
								 QString expression = expression_Master_Slave_Variable; bool exist = false;};	// for master/slave dependencies

struct Int_Independent			{int value=1; bool is_Independent=false;	int start = 1; int step = 1; int num_Steps = 3; int id = 0; QString whats_This;
								 Int_Independent()
								 {
									id = Global_Definitions::generate_Id();	// create unique id
									whats_This = whats_This_Num_Repetitions;
								 }
								};
struct Independent				{bool is_Independent = false; double min; double max; int num_Points;};

struct Coupled					{bool is_Coupled = false;
								 Parameter_Indicator master;
								 QVector<Parameter_Indicator> slaves;};

struct Fit						{bool is_Fitable = false; bool min_Bounded; double min; bool max_Bounded; double max;};

struct Parameter				{double value; Independent independent; Coupled coupled; Fit fit;
								 Parameter_Indicator indicator;
								 Parameter()
								 {
									indicator.id = Global_Definitions::generate_Id();	// create unique id
								 }};
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
struct Drift					{bool is_Drift_Line;  Parameter drift_Line_Value;
								 bool is_Drift_Sine;  Parameter drift_Sine_Amplitude; Parameter drift_Sine_Frequency; Parameter drift_Sine_Phase;
								 bool is_Drift_Rand;  Parameter drift_Rand_Rms;	};


// optical constant types
struct Point					{double lambda; double re;  double im; void read_Row(QTextStream& input, bool if_Factors);};
struct Material_Data			{QString substance; QString filename; QVector<Point> material_Data; void read_Material(QString& filename);};
struct Element_Data				{QString element;					  QVector<Point> element_Data;  void read_Element (QString& filename);};

// independent calculation functions
struct Calc_Functions			{bool check_Enabled = true;
								 bool check_Reflectance = true;
								 bool check_Transmittance = false;
								 bool check_Absorptance = false;
								 bool check_Field = false;
								 bool check_Joule = false;
								 bool check_User = false;
								 QString user_Functions = "R+T; cos(A)*R^3-sqrt(J) ; pow(F, log(root(3,5)))";
								};

// measured/target data types
struct Value					{double val_1; double val_2; /*double sigma_1; double sigma_2;*/};
struct Curve					{QVector<double> argument; QVector<double> shifted_Argument;
								 QVector<Value> values;	   QVector<Value> shifted_Values;
								 double arg_Offset = 0; double arg_Factor = 1;
								 double val_Offset = 0; double val_Factor = 1;
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
								 double weight = 1;
								 double weight_Sqrt = 1;
								 QString fit_Function = "log(x+1E-5); sin(x/2)";

								 double expression_Argument;							// not to store
								 QVector<exprtk::expression<double>> expression_Vec;	// not to store

								 void create_Expressions_for_Residual()
								 {
									expression_Vec.clear();

									exprtk::parser<double> parser;
									exprtk::symbol_table<double> symbol_table;
									symbol_table.add_variable(fit_Function_Variable, expression_Argument);
									symbol_table.add_constants();

									QVector<QString> bare_String_Expressions = fit_Function.split(fit_Function_Separator, QString::SkipEmptyParts).toVector();
									for(int i=0; i<bare_String_Expressions.size(); ++i)
									{
										if(!bare_String_Expressions[i].split(" ", QString::SkipEmptyParts).empty())
										{
											exprtk::expression<double> new_Expression;
											expression_Vec.append(new_Expression);
											expression_Vec[i].register_symbol_table(symbol_table);
											parser.compile(bare_String_Expressions[i].toStdString(), expression_Vec[i]);
										}
									}
								 }
								};
struct Fitables
								{	vector<QString> fit_Struct_Names;		// names of structures
									vector<QString> fit_Names;				// names of parameters to be fitted
									vector<QString>	fit_Whats_This;			// whats_This of parameters to be fitted
									QVector<unsigned long long int>	fit_IDs;// ID of parameters to be fitted
									vector<double>	fit_Min;				// min boundary
									vector<double>	fit_Max;				// max boundary

									// changeable
									vector<double>	fit_Value_Parametrized;	// unbounded parametrized variables
									vector<double*> fit_Value_Pointers;		// poiners to real parameters
									vector<tree<Node>::iterator> fit_Parent_Iterators;

									void clear_All()
									{
										fit_Names.clear();
										fit_Whats_This.clear();
										fit_IDs.clear();
										fit_Min.clear();
										fit_Max.clear();

										fit_Value_Parametrized.clear();
										fit_Value_Pointers.clear();
									}
								};
// -----------------------------------------------------------------------------------------

// serialization

QDataStream& operator <<( QDataStream& stream, const Independent_Indicator& independent_Indicator );
QDataStream& operator >>( QDataStream& stream,		 Independent_Indicator& independent_Indicator );

QDataStream& operator <<( QDataStream& stream, const Parameter_Indicator& parameter_Indicator );
QDataStream& operator >>( QDataStream& stream,		 Parameter_Indicator& parameter_Indicator );

QDataStream& operator <<( QDataStream& stream, const Int_Independent& int_Independent );
QDataStream& operator >>( QDataStream& stream,		 Int_Independent& int_Independent );

QDataStream& operator <<( QDataStream& stream, const Independent& independent );
QDataStream& operator >>( QDataStream& stream,		 Independent& independent );

QDataStream& operator <<( QDataStream& stream, const Coupled& coupled );
QDataStream& operator >>( QDataStream& stream,		 Coupled& coupled );

QDataStream& operator <<( QDataStream& stream, const Fit& fit );
QDataStream& operator >>( QDataStream& stream,		 Fit& fit );

QDataStream& operator <<( QDataStream& stream, const Parameter& parameter );
QDataStream& operator >>( QDataStream& stream,		 Parameter& parameter );

QDataStream& operator <<( QDataStream& stream, const Stoichiometry& stoichiometry );
QDataStream& operator >>( QDataStream& stream,		 Stoichiometry& stoichiometry );

QDataStream& operator <<( QDataStream& stream, const Interlayer& interlayer );
QDataStream& operator >>( QDataStream& stream,		 Interlayer& interlayer );

QDataStream& operator <<( QDataStream& stream, const Drift& drift );
QDataStream& operator >>( QDataStream& stream,		 Drift& drift );

QDataStream& operator <<( QDataStream& stream, const Calc_Functions& calc_Functions );
QDataStream& operator >>( QDataStream& stream,		 Calc_Functions& calc_Functions );

QDataStream& operator <<( QDataStream& stream, const Value& value );
QDataStream& operator >>( QDataStream& stream,		 Value& value );

QDataStream& operator <<( QDataStream& stream, const Curve& curve );
QDataStream& operator >>( QDataStream& stream,		 Curve& curve );

QDataStream& operator <<( QDataStream& stream, const Fit_Params& fit_Params );
QDataStream& operator >>( QDataStream& stream,		 Fit_Params& fit_Params );
// -----------------------------------------------------------------------------------------

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
