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
#define Default_Value_Path "../../settings/default_values.ini"
	#define Structure_Init_Values "Structure_Init_Values"
		#define Ambient_Values "Ambient_Values"
		#define Layer_Values "Layer_Values"

extern QStringList element_Name;
extern QVector<double> element_Mass;

struct stoichiometry			{double composition; QString type;};
extern QStringList transition_Layer_Functions;
extern QStringList drift_Models;

class Global_Definitions
{

};

#endif // GLOBAL_DEFINITIONS_H
