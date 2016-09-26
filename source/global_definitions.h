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

#define Angstrom_Sym QString(QChar(0x212B))
#define Rho_Sym QString(QChar(0x03C1))
#define Cube_Sym QString(QChar(0x00B3))
#define Sigma_Sym QString(QChar(0x03C3))
#define Gamma_Sym QString(QChar(0x03B3))

extern int default_Column;
extern QString what_is_This_Ambient;
extern QString what_is_This_Substrate;

extern QStringList element_Name;
extern QVector<double> element_Mass;

struct Stoichiometry			{double composition; QString type;};
extern QStringList transition_Layer_Functions;
extern QStringList drift_Models;

enum class Item_Type {Ambient, Layer, Substrate, Stack_Content};

class Global_Definitions
{
};

#endif // GLOBAL_DEFINITIONS_H
