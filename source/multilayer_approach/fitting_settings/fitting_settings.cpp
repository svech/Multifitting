#include "fitting_settings.h"

Fitting_Settings::Fitting_Settings(QWidget* parent) :
	QWidget(parent)
{
	current_Method = GSL_Methods[Levenberg_Marquardt];

	// for GSL TRS
	max_Iter = 400;
	x_Tolerance = 1.0e-8;
	g_Tolerance = 1.0e-8;
	f_Tolerance = 1.0e-8;
}
