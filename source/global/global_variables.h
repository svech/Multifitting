#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "global_definitions.h"
#include "global/layer_data_types.h"

class Measurement;
class Ambient;
class Substrate;
class Layer;
class Stack_Content;

// -----------------------------------------------------------------------------------------

// tabulated chemical elements
extern QStringList		element_Name;
extern QVector<double>	element_Mass;

// lists of types
extern QStringList transition_Layer_Functions;
extern QStringList drift_Models;
extern QStringList tril;

// -----------------------------------------------------------------------------------------

// units
extern QStringList wavelength_Units_List;	extern QMap<QString, double> wavelength_Coefficients_Map;
extern QStringList length_Units_List;		extern QMap<QString, double> length_Coefficients_Map;
extern QStringList angle_Units_List;		extern QMap<QString, double> angle_Coefficients_Map;		extern QMap<QString, QString> angle_Units_Legend_Map;
extern QStringList density_Units_List;
extern QStringList opt_Const_Units_List;

class Global_Variables
{
public:
	Global_Variables();
	static void serialize_Tree(QDataStream &out, QTreeWidget* tree);
	static void deserialize_Tree(QDataStream &in, QTreeWidget* tree);
	static double wavelength_Energy(QString wavelength_Units, double y);
	static QString wavelength_Energy_Name(QString wavelength_Units);
	static double angstrom_eV(double x);
	static void fill_Units_Maps();
};

#endif // GLOBAL_VARIABLES_H