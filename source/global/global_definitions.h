#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <QtWidgets>
#include <vector>
#include <complex>

#ifdef __linux__
	#include <iostream>
#endif

using namespace std;

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
#define space				" "
#define Angstrom_Sym		QString(QChar(0x212B))
#define Rho_Sym				QString(QChar(0x03C1))
#define Cube_Sym			QString(QChar(0x00B3))
#define Minus_One_Sym		QString(QChar(0x207B))+QString(QChar(0x00B9))
#define Sigma_Sym			QString(QChar(0x03C3))
#define Gamma_Sym			QString(QChar(0x03B3))
#define Epsilon_Sym			QString(QChar(0x03B5))
#define Cappa_Sym			QString(QChar(0x03F0))
#define Alpha_Sym			QString(QChar(0x03B1))
#define Zeta_Sym			QString(QChar(0x03B6))
#define Theta_Sym			QString(QChar(0x03B8))
#define Lambda_Sym			QString(QChar(0x03BB))
#define Degree_Sym			QString(QChar(0x00B0))
#define Delta_Big_Sym		QString(QChar(0x0394))
#define Mu_Sym				QString(QChar(0x03BC))
#define Pi_Sym				QString(QChar(0x03C0))
#define Nu_Sym				QString(QChar(0x03BD))
#define Phi_Sym				QString(QChar(0x03C6))
#define Prime_Sym			QString(QChar(0x2032))
#define Double_Prime_Sym	QString(QChar(0x2033))

// -----------------------------------------------------------------------------------------

// magic numbers
#define MAX_DOUBLE 1E300
#define MAX_INTEGER INT_MAX					// MAXINT
#define MAX_PRECISION 100
#define MIN_ANGULAR_RESOLUTION_POINTS 5		// minimal number of points necessary for specifying non-zero resolution
#define MIN_SPECTRAL_RESOLUTION_POINTS 5	// -||-
#define QLINEEDIT_RESIZE_MARGIN 7			// minimal white space in right side of qlineedit, pixels
#define DEFAULT_COLUMN 0					// as far as we use single-column trees
#define PERMANENT_INDEPENDENT 2				// angle and wavelength items in list
#define MAX_PRECISION_USED 10				// tthumbnail and lineedit precisions
#define RAND_SHIFT	100000					// rand * RAND_SHIFT + rand
#define TABLE_FIX_WIDTH_LINE_EDIT 50		// qLineEdit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT)
#define TABLE_COLUMN_ELEMENTS_SHIFT 1		// current_Column += TABLE_COLUMN_ELEMENTS_SHIFT
#define TABLE_COLUMN_INTERLAYERS_SHIFT 1	// current_Column += TABLE_COLUMN_INTERLAYERS_SHIFT

// -----------------------------------------------------------------------------------------

#define active "     Active"
#define	stop_Calculation "stop_Calculation"

// whatsThis : delimiters
#define whats_This_Delimiter ";"
#define item_Type_Delimiter  " "

// whatsThis : treeWidgetItem types
#define whats_This_Measurement	 "Measurement"
#define whats_This_Ambient		 "Ambient"
#define whats_This_Layer		 "Layer"
#define whats_This_Multilayer	 "Multilayer"
#define whats_This_Substrate	 "Substrate"

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
#define whats_This_Num_Repetitions			"Num_Repetitions"
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

// value types
#define VAL				"value"
#define MIN				"min value"
#define MAX				"max value"

// -----------------------------------------------------------------------------------------

// properties
#define min_Size_Property	"min_Size"
#define index_Property		"index"
#define TRIL_TRUE			"True"
#define TRIL_FALSE			"False"
#define TRIL_NOT_DEFINED	"NotDefined"
#define NOPARENT			"No parents"

#define num_Chemic_Element_Property "num_Chemic_Element"
#define interlayer_Index_Property	"interlayer_Index"
#define item_Type_Property			"item_Type"
#define row_Property				"row"
#define column_Property				"column"
#define whats_This_Property			"whats_This"
#define value_Type_Property			"value_Type"

#define reload_Property	"reload"

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
#define table_Key	"table_Of_Structures"

// -----------------------------------------------------------------------------------------

// enums
enum class Item_Type			{Ambient, Layer, Substrate, Stack_Content};
enum class Variable_Type		{Independent, Coupled, Fitted, Optimized};
enum       Transitional_Layer	{ Erf , Lin , Exp , Tanh , Sin , Step};

// -----------------------------------------------------------------------------------------

// calculation
#define I complex<double>(0,1)
#define Na 6.022140857E23
#define Q 4.484891E-30

#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131e-16
#endif
// -----------------------------------------------------------------------------------------

// simple types					renew corresponding serialization operators!
struct Int_Independent			{int value; bool is_Independent=false;	int start; int step; int num_steps;};
struct Independent				{bool is_Independent = false;	double min; double max; int num_Points;};
struct Coupled					{bool is_Coupled	 = false;};
struct Fit						{bool is_Fitable	 = false;	bool min_Bounded; double min; bool max_Bounded; double max;};
struct Optimize					{bool is_Optimizable = false;	bool min_Bounded; double min; bool max_Bounded; double max;};
struct Parameter				{double value; Independent independent; Coupled coupled; Fit fit; Optimize optimize;                     };
struct Stoichiometry			{Parameter composition; QString type;};
struct Interlayer				{Parameter interlayer; Parameter my_Sigma; bool enabled;};
struct Drift					{bool is_Drift_Line;  Parameter drift_Line_Value;
								 bool is_Drift_Sine;  Parameter drift_Sine_Amplitude; Parameter drift_Sine_Frequency; Parameter drift_Sine_Phase;
								 bool is_Drift_Rand;  Parameter drift_Rand_Rms;};


// optical constant types
struct Point					{double lambda; double re;  double im; void read_Row(QTextStream& input, bool if_Factors);};
struct Material_Data			{QString substance; QString filename; QVector<Point> material_Data; void read_Material(QString& filename);};
struct Element_Data				{QString element;					  QVector<Point> element_Data;  void read_Element (QString& filename);};

struct Window_Type				{static QString Launcher()					  { return "Launcher"  ;}
								 static QString Multilayer_Approach()		  { return "Multilayer_Approach";}
								 static QString Independent_Variables_Editor(){ return "Independent_Variable_Editor";}
								 static QString Item_Editor()				  { return "Item_Editor";}
								};

struct Angle_Type				{static QString Grazing()		 { return "Grazing"  ;}
								 static QString Incidence()		 { return "Incidence";}
								};

// measured data types
struct Argument					{vector<double> val; double shift;									   };
struct Value					{vector<double> val; double shift; vector<double> sigma; double factor;};
struct Measured_Curve			{vector<Argument> arguments; vector<Value> values; QString measurement_Type; QString angle_Type; QString anglular_Units; QString spectral_Units;};

// -----------------------------------------------------------------------------------------

// serialization
QDataStream& operator <<( QDataStream& stream, const Int_Independent& int_Independent );
QDataStream& operator >>( QDataStream& stream,		 Int_Independent& int_Independent );

QDataStream& operator <<( QDataStream& stream, const Independent& independent );
QDataStream& operator >>( QDataStream& stream,		 Independent& independent );

QDataStream& operator <<( QDataStream& stream, const Coupled& coupled );
QDataStream& operator >>( QDataStream& stream,		 Coupled& coupled );

QDataStream& operator <<( QDataStream& stream, const Fit& fit );
QDataStream& operator >>( QDataStream& stream,		 Fit& fit );

QDataStream& operator <<( QDataStream& stream, const Optimize& optimize );
QDataStream& operator >>( QDataStream& stream,		 Optimize& optimize );

QDataStream& operator <<( QDataStream& stream, const Parameter& parameter );
QDataStream& operator >>( QDataStream& stream,		 Parameter& parameter );

QDataStream& operator <<( QDataStream& stream, const Stoichiometry& stoichiometry );
QDataStream& operator >>( QDataStream& stream,		 Stoichiometry& stoichiometry );

QDataStream& operator <<( QDataStream& stream, const Interlayer& interlayer );
QDataStream& operator >>( QDataStream& stream,		 Interlayer& interlayer );

QDataStream& operator <<( QDataStream& stream, const Drift& drift );
QDataStream& operator >>( QDataStream& stream,		 Drift& drift );
// -----------------------------------------------------------------------------------------

class Global_Definitions
{
public:
	Global_Definitions();
};

template <typename T>
void print_Vector	(QString name, vector<T>& vec, int transpose)	// output
{
	if(vec.size()==0) return;
	if(transpose==0)
	{
		cout<<name.toStdString()<<"[0.."<<vec.size()-1<<"] = "; //-V128
		for(auto i=0; i<vec.size(); ++i) //-V104
			cout<<vec[i]<<"\t";
	} else
	for(auto i=0; i<vec.size(); ++i) //-V104
	{
		cout<<name.toStdString()<<"["<<i<<"] = ";
		cout<<vec[i]<<endl;
	}
	cout<<endl;
}

#endif // GLOBAL_DEFINITIONS_H
