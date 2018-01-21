#ifndef MAIN_CALCULATION_MODULE_H
#define MAIN_CALCULATION_MODULE_H

#include "global/layer_data_types.h"
#include "calculation/calculation_tree.h"
#include "multilayer_approach/multilayer.h"
#include "calculation/fitting_gsl.h"

class Multilayer;

class Main_Calculation_Module: public QObject
{
	Q_OBJECT
public:
	Main_Calculation_Module(QTabWidget*	multilayer_Tabs, QString calc_Mode);

	QTabWidget* multilayer_Tabs;
	QString calc_Mode;
	QVector<Multilayer*> multilayers;
	QVector<Calculation_Tree*> calculation_Trees;

	Fitables fitables;
	Fitables rejected_Fitables;

	QString parametrization_Type = triangle;
	QVector<Parameter*> bad_Sigmas;

	void single_Calculation();
	void fitting();
	bool check_Zero_Sigma();
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
