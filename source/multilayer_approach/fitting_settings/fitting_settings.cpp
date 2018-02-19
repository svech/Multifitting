#include "fitting_settings.h"
#include "gsl/gsl_machine.h"

Fitting_Settings::Fitting_Settings(QWidget* parent) :
	QWidget(parent)
{
	current_Method = GSL_Methods[Levenberg_Marquardt];

	/// for GSL TRS
	// main
	max_Iter = 20;
	x_Tolerance = 1.0e-8;
	g_Tolerance = 1.0e-8;
	f_Tolerance = 1.0e-8;

	// additional
	current_Scale = GSL_Scales[More];;
	current_Solver = GSL_Solvers[QR_decomposition];
	current_Fdtype = GSL_Fdtype[Forward];
	factor_Up = 3;
	factor_Down = 2;
	avmax = 0.75;
	h_df = GSL_SQRT_DBL_EPSILON;
	h_fvv = 0.02;
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Fitting_Settings* fitting_Settings )
{
	return stream << fitting_Settings->current_Method

	/// GSL TRS
	// main
	<< fitting_Settings->max_Iter << fitting_Settings->x_Tolerance << fitting_Settings->g_Tolerance << fitting_Settings->f_Tolerance
	// additional
	<< fitting_Settings->current_Scale << fitting_Settings->current_Solver << fitting_Settings->current_Fdtype
	<< fitting_Settings->factor_Up << fitting_Settings->factor_Down << fitting_Settings->avmax << fitting_Settings->h_df << fitting_Settings->h_fvv;

}
QDataStream& operator >>( QDataStream& stream,		  Fitting_Settings* fitting_Settings )
{
	return stream >> fitting_Settings->current_Method

	/// GSL TRS
	// main
	>> fitting_Settings->max_Iter >> fitting_Settings->x_Tolerance >> fitting_Settings->g_Tolerance >> fitting_Settings->f_Tolerance
	// additional
	>> fitting_Settings->current_Scale >> fitting_Settings->current_Solver >> fitting_Settings->current_Fdtype
	>> fitting_Settings->factor_Up >> fitting_Settings->factor_Down >> fitting_Settings->avmax >> fitting_Settings->h_df >> fitting_Settings->h_fvv;
}
