#ifndef MAIN_CALCULATION_MODULE_H
#define MAIN_CALCULATION_MODULE_H

#include "global/layer_data_types.h"
#include "calculation/calculation_tree.h"
#include "calculation/optical_constants.h"
#include "multilayer_approach/multilayer.h"

class Main_Calculation_Module
{
public:
	Main_Calculation_Module(QList<Multilayer*> multilayer_List, Optical_Constants* optical_Constants);
};

#endif // MAIN_CALCULATION_MODULE_H
