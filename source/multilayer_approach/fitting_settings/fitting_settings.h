#ifndef FITTING_SETTINGS_H
#define FITTING_SETTINGS_H

#include "global/layer_data_types.h"

class Fitting_Settings: public QWidget
{
	Q_OBJECT
public:
	explicit Fitting_Settings(QWidget *parent = nullptr);

	QString current_Method;

	// for GSL TRS
	size_t max_Iter;
	double x_Tolerance;
	double g_Tolerance;
	double f_Tolerance;

};

#endif // FITTING_SETTINGS_H
