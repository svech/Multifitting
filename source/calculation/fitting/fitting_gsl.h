#ifndef FITTING_GSL_H
#define FITTING_GSL_H

#include "calculation/fitting/fitting.h"
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>

struct Fitting_Params;
class Fitting;
class Main_Calculation_Module;

class Fitting_GSL
{
public:
	Fitting_GSL(Fitting* fitting);

	Multilayer_Approach* multilayer_Approach;
	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;
	Fitting* fitting;
	Fitting_Params* params;

	static void callback(const size_t iter, void* bare_Params, const gsl_multifit_nlinear_workspace* w);
	static int calc_Residual(const gsl_vector* x, void* bare_Params, gsl_vector* f);
	void init_Position(gsl_vector* x);
	bool fit();
};

#endif // FITTING_GSL_H
