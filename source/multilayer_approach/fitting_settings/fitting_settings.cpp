// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "fitting_settings.h"
#include "gsl/gsl_machine.h"

Fitting_Settings::Fitting_Settings(QWidget* parent) :
	QWidget(parent)
{
	// common
	current_Method = SO_Methods[Differential_Evolution];
	num_Runs = 1;
	randomized_Start = false;

	/// for GSL TRS
	// main
	max_Iter = 20;
	x_Tolerance = 1.0e-8;
	g_Tolerance = 1.0e-8;
	f_Tolerance = 1.0e-8;

	// additional
	current_Scale = GSL_Scales[More];
	current_Solver = GSL_Solvers[QR_decomposition];
	current_Fdtype = GSL_Fdtype[Forward];
	factor_Up = 3;
	factor_Down = 2;
	avmax = 0.75;
	h_df = GSL_SQRT_DBL_EPSILON;
	h_fvv = 0.02;

	/// for SwarmOps
	// main
	max_Evaluations=1000;
	max_Eval_Factor=250;
	max_Eval_Check=false;
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Fitting_Settings* fitting_Settings )
{
	return stream << fitting_Settings->current_Method << fitting_Settings->num_Runs << fitting_Settings->randomized_Start

	/// GSL TRS
	// main
	<< fitting_Settings->max_Iter << fitting_Settings->x_Tolerance << fitting_Settings->g_Tolerance << fitting_Settings->f_Tolerance
	// additional
	<< fitting_Settings->current_Scale << fitting_Settings->current_Solver << fitting_Settings->current_Fdtype
	<< fitting_Settings->factor_Up << fitting_Settings->factor_Down << fitting_Settings->avmax << fitting_Settings->h_df << fitting_Settings->h_fvv

	/// SwarmOps
	// main
	<< fitting_Settings->max_Evaluations << fitting_Settings->max_Eval_Factor << fitting_Settings->max_Eval_Check ;
}
QDataStream& operator >>( QDataStream& stream,		  Fitting_Settings* fitting_Settings )
{
	return stream >> fitting_Settings->current_Method >> fitting_Settings->num_Runs >> fitting_Settings->randomized_Start

	/// GSL TRS
	// main
	>> fitting_Settings->max_Iter >> fitting_Settings->x_Tolerance >> fitting_Settings->g_Tolerance >> fitting_Settings->f_Tolerance
	// additional
	>> fitting_Settings->current_Scale >> fitting_Settings->current_Solver >> fitting_Settings->current_Fdtype
	>> fitting_Settings->factor_Up >> fitting_Settings->factor_Down >> fitting_Settings->avmax >> fitting_Settings->h_df >> fitting_Settings->h_fvv

	/// SwarmOps
	// main
	>> fitting_Settings->max_Evaluations >> fitting_Settings->max_Eval_Factor >> fitting_Settings->max_Eval_Check ;
}
