#ifndef MAIN_CALCULATION_MODULE_H
#define MAIN_CALCULATION_MODULE_H

#include "global/layer_data_types.h"
#include "calculation/calculation_tree.h"
#include "multilayer_approach/multilayer.h"
#include "calculation/fitting_gsl.h"

class Multilayer;

struct Fitables
{
	vector<QString> fit_Names;				// names of parameters to be fitted
	vector<QString>	fit_Whats_This;			// whats_This of parameters to be fitted
	vector<int>		fit_IDs;				// ID of parameters to be fitted
	vector<double>	fit_Min;				// min boundary
	vector<double>	fit_Max;				// max boundary
	vector<double>	fit_Value_Parametrized;	// unbounded parametrized variables
	vector<double*> fit_Value_Pointers;		// poiners to real parameters

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

class Main_Calculation_Module: public QObject
{
	Q_OBJECT
public:
	Main_Calculation_Module(QTabWidget*	multilayer_Tabs);

	QTabWidget* multilayer_Tabs;
	QVector<Multilayer*> multilayers;
	QVector<Calculation_Tree*> calculation_Trees;

	Fitables fitables;

	QString parametrization_Type = triangle;

	void single_Calculation();
	void fitting();
	void calc_Tree_Iteration(const tree<Node>::iterator& parent);
	void find_Fittable_Parameters(Data& struct_Data);

	double triangle_Wave(double x);
	double parametrize  (double value,                      double min, double max);
	double unparametrize(double parametrized_Shifted_Value, double min, double max);

	template <typename Type>
	void print_Reflect_To_File(Data_Element<Type>& data_Element, QString struct_Name, int index);
	void print_Calculated_To_File();
};

#endif // MAIN_CALCULATION_MODULE_H
