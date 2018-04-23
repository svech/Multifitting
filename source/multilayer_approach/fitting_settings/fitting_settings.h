#ifndef FITTING_SETTINGS_H
#define FITTING_SETTINGS_H

#include "global/layer_data_types.h"

class Fitting_Settings: public QWidget
{
	Q_OBJECT
public:
	explicit Fitting_Settings(QWidget *parent = nullptr);

	// common
	QString current_Method;
	int num_Runs;
	bool randomized_Start;

	/// for GSL TRS
	// main
	int max_Iter;
	double x_Tolerance;
	double g_Tolerance;
	double f_Tolerance;

	// additional
	QString current_Scale;
	QString current_Solver;
	QString current_Fdtype;
	double factor_Up;
	double factor_Down;
	double avmax;
	double h_df;
	double h_fvv;

	/// for SwarmOps
	// main
	bool initialize_By_Current_State;
	int max_Evaluations;
	int max_Eval_Factor;
	bool max_Eval_Check;

};

// serialization
QDataStream& operator <<( QDataStream& stream, const Fitting_Settings* fitting_Settings );
QDataStream& operator >>( QDataStream& stream,		 Fitting_Settings* fitting_Settings );

#endif // FITTING_SETTINGS_H
