#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <QtWidgets>

// settings groups
#define Launcher_Geometry "Launcher Geometry"
#define Multilayer_Window_Geometry "Multilayer Window Geometry"
#define Multilayer_Tabs "Multilayer Tabs"
#define Paths "Paths"
#define whatsThis_Properties "whatsThis Properties"

extern QStringList element_Name;
extern QVector<double> element_Mass;

struct stoichiometry			{double composition; QString element;};
extern QStringList transition_Layer_Functions;
extern QStringList drift_Models;

class Global_Definitions
{

};

#endif // GLOBAL_DEFINITIONS_H
