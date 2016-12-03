#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <QtWidgets>
#include <vector>
#include <complex>

using namespace std;

// -----------------------------------------------------------------------------------------

// settings

// gui
#define Gui_Settings_Path "../../settings/gui.ini"
	#define Application_Style			"Application_Style"
	#define Launcher_Geometry			"Launcher_Geometry"
	#define Multilayer_Window_Geometry	"Multilayer_Window_Geometry"
	#define Multilayer_Tabs				"Multilayer_Tabs"
	#define Resource_Paths				"Resource_Paths"

// structure default values
#define Structure_Default_Values_Path "../../settings/structure_default_values.ini"
	#define Structure_Init_Values	"Structure_Init_Values"
		#define Ambient_Values		"Ambient_Values"
		#define Layer_Values		"Layer_Values"
		#define Substrate_Values	"Substrate_Values"
		#define Stack_Values		"Stack_Values"

// precisions
#define Precisions_Path "../../settings/precisions.ini"
	#define Precisions "Precisions"
		#define Line_Edit "Line_Edit"
		#define Thumbnail "Thumbnail"
		#define Other	  "Other"

// parameters default values
#define Parameters_Default_Values_Path "../../settings/parameters_default_values.ini"
	#define Independent_Values  "Independent_Values"
	#define Optimization_Values "Optimization_Values"
	#define Fitting_Values		"Fitting_Values"

// units
#define Units_Path "../../settings/units.ini"
	#define Units "Units"

// calculations
#define Calculations_Path "../../settings/calculations.ini"
	#define Threads "Threads"

// -----------------------------------------------------------------------------------------

// optical constants

#define nk_Path		"../../nk/"
#define f1f2_Path	"../../f1f2/"

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
#define Prime_Sym			QString(QChar(0x2032))
#define Double_Prime_Sym	QString(QChar(0x2033))

// -----------------------------------------------------------------------------------------

// magic numbers
#define MAX_DOUBLE 1E300
#define MAX_INTEGER MAXINT
#define MAX_PRECISION 100
#define MIN_ANGULAR_RESOLUTION_POINTS 5		// minimal number of points necessary for specifying non-zero resolution
#define MIN_SPECTRAL_RESOLUTION_POINTS 5	// -||-
#define QLINEEDIT_RESIZE_MARGIN 7			// minimal white space in right side of qlineedit, pixels
#define DEFAULT_COLUMN 0					// as far as we use single-column trees
#define PERMANENT_INDEPENDENT 2				// angle and wavelength items in list
#define MAX_PRECISION_USED 10				// tthumbnail and lineedit precisions

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
#define whats_This_Absolute_Density			"Density"
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

// -----------------------------------------------------------------------------------------

// properties
#define min_Size_Property	"min_Size"
#define index_Property		"index"
#define TRIL_TRUE			"True"
#define TRIL_FALSE			"False"
#define TRIL_NOT_DEFINED	"NotDefined"
#define NOPARENT			"No parents"
// -----------------------------------------------------------------------------------------

// enums
enum class Item_Type		{Ambient, Layer, Substrate, Stack_Content};
enum class Variable_Type	{Independent, Coupled, Fitted, Optimized};

// -----------------------------------------------------------------------------------------

// calculation
#define I complex<double>(0,1)
#define Na 6.022140857E23
#define Q 4.484891E-30

// -----------------------------------------------------------------------------------------

// simple types					renew corresponding serialization operators!
struct Int_Independent			{int value; bool is_Independent=false;	int start; int step; int num_steps;};
struct Independent				{bool is_Independent = false;	double min; double max; int num_Points;};
struct Coupled					{bool is_Coupled	 = false;};
struct Fit						{bool is_Fitable	 = false;	bool min_Bounded; double min; bool max_Bounded; double max;};
struct Optimize					{bool is_Optimizable = false;	bool min_Bounded; double min; bool max_Bounded; double max;};
struct Parameter				{double value; Independent independent; Coupled coupled; Fit fit; Optimize optimize;};
struct Stoichiometry			{Parameter composition; QString type;};
struct Interlayer				{Parameter interlayer; bool enabled;};

// optical constant types
struct Point					{double lambda; double re;  double im; void read_Row(QTextStream& input, bool if_Factors);};
struct Material_Data			{QString substance; QString filename; QVector<Point> material_Data; void read_Material(QString& filename);};
struct Element_Data				{QString element;					  QVector<Point> element_Data;  void read_Element (QString& filename);};

// intermediate values for optical functions
struct intermediate_Point		{
								complex<double> delta_Epsilon;	// = 1 - n^2  OR Re(1 - n^2)*permittivity/100 +(or - ??) Im(1 - n^2)*absorption/100
								complex<double> epsilon;		// = 1 - density * delta_Epsilon or = 1 - bulk_density * delta_Epsilon

								complex<double> hi;
								complex<double> exponenta;

								// for top interfaces
								complex<double> r_Fresnel_s;
								complex<double> r_Fresnel_p;
								double weak_Factor = 1;
								};

struct Window_Type				{static QString Launcher()					  { return "Launcher"  ;}
								 static QString Multilayer_Approach()		  { return "Multilayer_Approach";}
								 static QString Independent_Variables_Editor(){ return "Independent_Variable_Editor";}
								 static QString Item_Editor()				  { return "Item_Editor";}
								};

struct Angle_Type				{static QString Grazing()		 { return "Grazing"  ;}
								 static QString Incidence()		 { return "Incidence";}
								};

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
// -----------------------------------------------------------------------------------------

class Global_Definitions
{
public:
	Global_Definitions();
};

#endif // GLOBAL_DEFINITIONS_H
