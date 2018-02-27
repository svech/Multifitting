#ifndef FITTING_H
#define FITTING_H

#include "calculation/main_calculation_module.h"
#include "calculation/fitting/fitting_gsl.h"
#include "calculation/fitting/fitting_swarmops.h"
#include <gsl/gsl_blas.h>

class Main_Calculation_Module;

struct Fitting_Params
{
	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;

	const size_t p;
	gsl_vector* f;

	const size_t n;
	gsl_vector* x;

	double init_Residual;
	double final_Residual;

	// for SwarmOps
	size_t counter;
};

class Fitting
{
public:
	Fitting(Main_Calculation_Module* main_Calculation_Module);
	~Fitting();

	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;

	const size_t n;
	const size_t p;
	gsl_vector* f;
	gsl_vector* x;
	Fitting_Params params;
	QVector<int> widths;

	static double func(double argument, int index);
	size_t num_Residual_Points();
	static void period_Subtree_Iteration(const tree<Node>::iterator& parent, double coeff);
	static void gamma_Subtree_Iteration(const tree<Node>::iterator& parent, double old_Value);
	static void slaves_Recalculation(Parameter* master, Fitting_Params* params);
	static void calc_Residual(const gsl_vector* x, Fitting_Params* params, gsl_vector* f);
	static void fill_Residual(int& residual_Shift, Data_Element<Target_Curve>& target_Element, gsl_vector* f, int index);
	void initialize_Position();
	void randomize_Position();
	bool run_Fitting();
	bool fit();

	void add_Fit_To_File(const gsl_vector* x, double residual, QString filename, int run);
	bool check_Residual_Expression();
};

#endif // FITTING_H
