#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "global_definitions.h"
#include "global/layer_data_types.h"
#include "calculation/optical_constants.h"

class Measurement;
class Ambient;
class Substrate;
class Layer;
class Stack_Content;
class Optical_Constants;

// -----------------------------------------------------------------------------------------

// tabulated chemical elements
extern QStringList		element_Name;
extern QVector<double>	element_Mass;
extern QMap<QString, double> sorted_Elements;

// lists of types
extern QStringList transition_Layer_Functions; extern int transition_Layer_Functions_Size;
//extern QStringList thickness_Drift_Functions;  extern size_t thickness_Drift_Functions_Size;
//extern QStringList drift_Models;
extern QStringList tril;

// measured curves
extern QStringList argument_Types;
enum argument_Types_Enum    { Grazing_angle, Incident_angle, Wavelength_Energy };

extern QStringList value_Function;
enum value_Function_Enum    { Reflectance,   Transmittance,  Absorptance	   };

extern QStringList value_R_Mode;
enum value_R_Mode_Enum		{ R,  R_Phi,     r_Re_Im,        r_Abs_Phi         };

extern QStringList value_T_Mode;
enum value_T_Mode_Enum		{ T };

extern QStringList value_A_Mode;
enum value_A_Mode_Enum		{ A };
// -----------------------------------------------------------------------------------------

// units
extern QStringList wavelength_Units_List;	extern QMap<QString, double> wavelength_Coefficients_Map;
extern QStringList length_Units_List;		extern QMap<QString, double> length_Coefficients_Map;
extern QStringList angle_Units_List;		extern QMap<QString, double> angle_Coefficients_Map;		extern QMap<QString, QString> angle_Units_Legend_Map;
extern QStringList density_Units_List;
extern QStringList opt_Const_Units_List;
extern QStringList drift_Units_List;

// -----------------------------------------------------------------------------------------

extern Optical_Constants* optical_Constants;

class Global_Variables
{
public:
	Global_Variables();
	static void read_Optical_Constants();
	static void create_Sorted_Elements_Map();
	static void   serialize_Tree(QDataStream &out, QTreeWidget* tree);
	static void deserialize_Tree(QDataStream &in,  QTreeWidget* tree);
	static void   serialize_Variables_List(QDataStream &out, QListWidget* list);
	static void deserialize_Variables_List(QDataStream &in,  QListWidget* list);
	static double wavelength_Energy(QString wavelength_Units, double y);
	static QString wavelength_Energy_Name(QString wavelength_Units);
	static double angstrom_eV(double x);
	static void fill_Units_Maps();
	static int get_Item_Depth(QTreeWidgetItem* item);
	static int get_Number_Of_Items(QTreeWidget* tree);
	static int get_Tree_Depth(QTreeWidgetItem* item);
};

#endif // GLOBAL_VARIABLES_H
