#ifndef FITTING_GSL_H
#define FITTING_GSL_H

#include "main_calculation_module.h"
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>

class Main_Calculation_Module;

struct Params
{
	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;
};

class Fitting_GSL
{
public:
	Fitting_GSL(Main_Calculation_Module* main_Calculation_Module);

	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;

	size_t num_Residual_Points();
	static void callback(const size_t iter, void* bare_Params, const gsl_multifit_nlinear_workspace* w);
	void fit();
	static void period_Subtree_Iteration(const tree<Node>::iterator& parent, double coeff);
	static void gamma_Subtree_Iteration(const tree<Node>::iterator& parent, double old_Value);
	static void slaves_Recalculation(Parameter* parameter);
	static int calc_Residual(const gsl_vector* x, void* bare_Params, gsl_vector* f);
	void init_Position(gsl_vector* x);
	static void fill_Residual(int& residual_Shift, Data_Element<Target_Curve>& target_Element, gsl_vector* f);
	bool check_Residual_Expression();
};

#endif // FITTING_GSL_H
