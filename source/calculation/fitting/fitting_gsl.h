#ifndef FITTING_GSL_H
#define FITTING_GSL_H

#include "calculation/fitting/fitting.h"
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>

class Fitting_GSL
{
public:
	Fitting_GSL(Multilayer_Approach* multilayer_Approach, Main_Calculation_Module* main_Calculation_Module);

	Multilayer_Approach* multilayer_Approach;
	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;

	static void callback(const size_t iter, void* bare_Params, const gsl_multifit_nlinear_workspace* w);
	bool fit();
	static int calc_Residual(const gsl_vector* x, void* bare_Params, gsl_vector* f);
	void init_Position(gsl_vector* x, Fitting_Params* params);
	static void fill_Residual(int& residual_Shift, Data_Element<Target_Curve>& target_Element, gsl_vector* f, int index);
	bool check_Residual_Expression();
};

#endif // FITTING_GSL_H
