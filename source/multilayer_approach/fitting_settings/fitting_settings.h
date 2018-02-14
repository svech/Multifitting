#ifndef FITTING_SETTINGS_H
#define FITTING_SETTINGS_H

#include "global/layer_data_types.h"

class Fitting_Settings: public QWidget
{
	Q_OBJECT
public:
	explicit Fitting_Settings(QWidget *parent = nullptr);

	QString current_Method;

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
};

// serialization
QDataStream& operator <<( QDataStream& stream, const Fitting_Settings* fitting_Settings );
QDataStream& operator >>( QDataStream& stream,		 Fitting_Settings* fitting_Settings );

#endif // FITTING_SETTINGS_H
