#ifndef FITTING_H
#define FITTING_H

#include "calculation/main_calculation_module.h"
#include "calculation/fitting/fitting_gsl.h"
#include "calculation/fitting/fitting_swarmops.h"
#include <gsl/gsl_blas.h>
#include <setjmp.h>
#include <SwarmOps/Statistics/Results.h>

class Main_Calculation_Module;
class Fitting_GSL;
class Fitting_SwarmOps;

struct Fitting_Params
{
	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;
	Fitables& confidentials;

	QString& calc_Mode;

	const size_t p;
	gsl_vector* f;

	const size_t n_Full_With_GISAS;
	const size_t n;
	gsl_vector* x;

	double init_Residual;
	double final_Residual;
	bool maximize = false;
	double max_Integral = 0;

	// for SwarmOps
	size_t counter;
	size_t num_Iter;
	SO_Results my_Res;
};

class Fitting
{
public:
	explicit Fitting(Main_Calculation_Module* main_Calculation_Module);
	~Fitting();

	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;
	Fitables& confidentials;
	size_t number_Of_Restricted_Regular_Components;

	const size_t n_Full_With_GISAS;
	const size_t n;
	const size_t p;
	gsl_vector* f;
	gsl_vector* x;
	Fitting_Params params;
	QVector<int> widths;

        Fitting_GSL* fitting_GSL;
        Fitting_SwarmOps* fitting_SwarmOps;


	static double func(double argument);
	static size_t num_Residual_Points(QVector<Calculation_Tree*>& calculation_Trees);
	static size_t num_Residual_Points_Full_With_GISAS(QVector<Calculation_Tree*>& calculation_Trees);
	static void period_Subtree_Iteration(const tree<Node>::iterator& parent, double coeff);
	static void gamma_Subtree_Iteration(const tree<Node>::iterator& parent, double old_Value);
	static void slaves_Recalculation(Parameter* master, Fitting_Params* params);
	static void calc_Residual(const gsl_vector* x, Fitting_Params* params, gsl_vector* f);
	static void regular_Restriction_Tree_Iteration(const tree<Node>::iterator& parent, Fitting_Params* params, gsl_vector* f, size_t& counter);
	static void change_Real_Fitables_and_Dependent(Fitting_Params* params, double old_Value, double new_Value, size_t i, QString fit_Conf);
	static void fill_Residual(Fitting_Params* params, int& residual_Shift, Data_Element<Target_Curve>& target_Element, gsl_vector* f);
	void initialize_Position();
	void randomize_Position(bool randomize = true);
	void check_Maximization();

	bool run_Fitting();
	bool fit();
	bool confidence(const vector<double>& fitables_Pointers_Value_Backup, const vector<double>& confidentials_Pointers_Value_Backup, size_t confidence_Index);

	void add_Fit_To_File(const gsl_vector* x, double residual, QString filename, int run);
	void add_Confidence_Distribution_To_File(double real_Conf_Value, QString filename, size_t confidence_Index, int point_Index, double residual, vector<double>* residuals_Set = nullptr);

	bool check_Residual_Expression();
};

#endif // FITTING_H
