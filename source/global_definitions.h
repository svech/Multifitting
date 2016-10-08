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

#define Angstrom_Sym	QString(QChar(0x212B))
#define Rho_Sym			QString(QChar(0x03C1))
#define Cube_Sym		QString(QChar(0x00B3))
#define Sigma_Sym		QString(QChar(0x03C3))
#define Gamma_Sym		QString(QChar(0x03B3))
#define Epsilon_Sym		QString(QChar(0x03B5))
#define Cappa_Sym		QString(QChar(0x03F0))
#define Alpha_Sym		QString(QChar(0x03B1))
#define Zeta_Sym		QString(QChar(0x03B6))


#define MAX_DOUBLE 1E100
#define MAX_INTEGER MAXINT
#define MAX_PRECISION 100

extern int default_Column;
extern QString whats_This_Ambient;
extern QString whats_This_Layer;
extern QString whats_This_Multilayer;
extern QString whats_This_Substrate;

extern QStringList element_Name;
extern QVector<double> element_Mass;

struct Parameter				{double value; bool independent=false; bool fitable=false; bool optimizable=false;};
struct Stoichiometry			{Parameter composition; QString type;};
struct Interlayer_Enabled		{Parameter interlayer; bool enabled;};

extern QStringList transition_Layer_Functions;
extern QStringList drift_Models;

enum class Tril				{True, False, NotDefined};
enum class Item_Type		{Ambient, Layer, Substrate, Stack_Content};
enum class Variable_Type	{Independent, Coupled, Fitted, Optimized};

class Global_Definitions
{
};

#endif // GLOBAL_DEFINITIONS_H
