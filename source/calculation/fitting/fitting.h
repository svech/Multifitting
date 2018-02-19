#ifndef FITTING_H
#define FITTING_H

#include "calculation/main_calculation_module.h"

class Main_Calculation_Module;

struct Fitting_Params
{
	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;
};

class Fitting
{
public:
	Fitting(Multilayer_Approach* multilayer_Approach, Main_Calculation_Module* main_Calculation_Module);

	Multilayer_Approach* multilayer_Approach;
	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;

	static double func(double argument, int index);
	size_t num_Residual_Points();
	static void period_Subtree_Iteration(const tree<Node>::iterator& parent, double coeff);
	static void gamma_Subtree_Iteration(const tree<Node>::iterator& parent, double old_Value);
	static void slaves_Recalculation(Parameter* master, Fitting_Params* params);

	bool fit();

};

#endif // FITTING_H
