#ifndef FITTING_SWARMOPS_H
#define FITTING_SWARMOPS_H

#include "calculation/main_calculation_module.h"
#include "SwarmOps/Optimize.h"
#include "SwarmOps/MetaOptimize.h"
#include "SwarmOps/Methods/Methods.h"
#include "SwarmOps/Tools/Vector.h"
#include <RandomOps/Random.h>

class Main_Calculation_Module;

class Fitting_SwarmOps
{
public:
	Fitting_SwarmOps(Multilayer_Approach* multilayer_Approach, Main_Calculation_Module* main_Calculation_Module);

	bool fit();

};

#endif // FITTING_SWARMOPS_H
