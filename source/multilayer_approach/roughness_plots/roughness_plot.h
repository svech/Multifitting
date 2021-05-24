#ifndef ROUGHNESS_PLOT_H
#define ROUGHNESS_PLOT_H

#include "multilayer_approach/multilayer_approach.h"

class Roughness_Plots_Window;

class Roughness_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Roughness_Plot(Multilayer* multilayer, QWidget *parent = nullptr);

signals:

};

#endif // ROUGHNESS_PLOT_H
