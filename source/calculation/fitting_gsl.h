#ifndef FITTING_GSL_H
#define FITTING_GSL_H

#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>

class Fitting_GSL
{
public:
	Fitting_GSL();

	static void fitting_GSL();
	int calc_Residual_GSL(const gsl_vector* x, void* params, gsl_vector* f);

};

#endif // FITTING_GSL_H
