#ifndef FITTING_SWARMOPS_H
#define FITTING_SWARMOPS_H

#include "calculation/fitting/fitting.h"
#include "SwarmOps/Optimize.h"
#include "SwarmOps/MetaOptimize.h"
#include "SwarmOps/Methods/Methods.h"
#include "SwarmOps/Tools/Vector.h"
#include <RandomOps/Random.h>

struct Fitting_Params;
class Fitting;

class Fitting_SwarmOps
{
public:
	Fitting_SwarmOps(Fitting* fitting);

	Multilayer_Approach* multilayer_Approach;
	Main_Calculation_Module* main_Calculation_Module;
	QVector<Calculation_Tree*>& calculation_Trees;
	Fitables& fitables;
	Fitting* fitting;
	Fitting_Params* params;

	static void callback(Fitting_Params* params);
	static SO_TFitness calc_Residual(SO_TElm* x, void const*context, const SO_TFitness fitnessLimit);
	bool fit();
};

#endif // FITTING_SWARMOPS_H
