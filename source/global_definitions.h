#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <QtWidgets>

// gui settings
#define Gui_Settings_Path "../../settings/gui_settings.ini"
	#define Application_Style "Application_Style"
	#define Launcher_Geometry "Launcher_Geometry"
	#define Multilayer_Window_Geometry "Multilayer_Window_Geometry"
	#define Multilayer_Tabs "Multilayer_Tabs"
	#define Paths "Paths"

// default values settings
#define Default_Values_Path "../../settings/default_values.ini"
	#define Structure_Init_Values "Structure_Init_Values"
		#define Ambient_Values "Ambient_Values"
		#define Layer_Values "Layer_Values"
		#define Substrate_Values "Substrate_Values"
		#define Stack_Values "Stack_Values"
	#define Structure_Values_Representation "Structure_Values_Representation"
	#define Parameters_Init_Values "Parameters_Init_Values"
		#define Independent_Values "Independent_Values"
			#define Angle_Values "Angle_Values"
			#define Radiation_Values "Radiation_Values"


#define Angstrom_Sym	QString(QChar(0x212B))
#define Rho_Sym			QString(QChar(0x03C1))
#define Cube_Sym		QString(QChar(0x00B3))
#define Sigma_Sym		QString(QChar(0x03C3))
#define Gamma_Sym		QString(QChar(0x03B3))
#define Epsilon_Sym		QString(QChar(0x03B5))
#define Cappa_Sym		QString(QChar(0x03F0))
#define Alpha_Sym		QString(QChar(0x03B1))
#define Zeta_Sym		QString(QChar(0x03B6))
#define Theta_Sym		QString(QChar(0x03B8))
#define Lambda_Sym		QString(QChar(0x03BB))
#define Degree_Sym		QString(QChar(0x00B0))


#define MAX_DOUBLE 1E100
#define MAX_INTEGER MAXINT
#define MAX_PRECISION 100

extern int default_Column;
extern int num_Permanent_Independent_Variables;

extern QString whats_This_Delimiter;
extern QString item_Type_Delimiter;

// main whatsThis
extern QString whats_This_Angle;
extern QString whats_This_Wavelength;
extern QString whats_This_Ambient;
extern QString whats_This_Layer;
extern QString whats_This_Multilayer;
extern QString whats_This_Substrate;

// specialized whatsThis additions
extern QString whats_This_Density;
extern QString whats_This_Permittivity;
extern QString whats_This_Absorption;
extern QString whats_This_Composition;
extern QString whats_This_Thickness;
extern QString whats_This_Sigma;
extern QString whats_This_Interlayer_Composition;
extern QString whats_This_Num_Repetitions;
extern QString whats_This_Period;
extern QString whats_This_Gamma;

extern QStringList element_Name;
extern QVector<double> element_Mass;

struct Independent				{bool is_Independent=false;	double min; double max; int num_Points;};
struct Fit						{bool is_Fitable=false;		bool min_Bounded; double min;
															bool max_Bounded; double max;};
struct Optimize					{bool is_Optimizable=false;	bool min_Bounded; double min;
															bool max_Bounded; double max;};
struct Int_Independent			{int value; bool is_Independent=false;	int start; int step; int num_steps;};
struct Parameter				{double value; Independent independent; Fit fit; Optimize optimize;};
struct Stoichiometry			{Parameter composition; QString type;};
struct Interlayer_Enabled		{Parameter interlayer; bool enabled;};

extern QStringList transition_Layer_Functions;
extern QStringList drift_Models;


enum class Tril				{True, False, NotDefined};
enum       Angle_Type		{Grazing, Incidence};
enum class Item_Type		{Ambient, Layer, Substrate, Stack_Content};
enum class Variable_Type	{Independent, Coupled, Fitted, Optimized};

//Q_DECLARE_METATYPE( Angle_Type )

class Global_Definitions
{
};

#endif // GLOBAL_DEFINITIONS_H
