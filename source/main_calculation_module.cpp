// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "main_calculation_module.h"

Main_Calculation_Module::Main_Calculation_Module(QTabWidget* multilayer_Tabs):
	multilayers		 (multilayer_Tabs->count()),
	calculation_Trees(multilayer_Tabs->count())
{
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		multilayers[tab_Index] = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
		calculation_Trees[tab_Index] = new Calculation_Tree(multilayers[tab_Index]->independent_Variables_Plot_Tabs,
															multilayers[tab_Index]->target_Profiles_Vector,
															multilayers[tab_Index]->structure_Tree->tree);
	}
}

void Main_Calculation_Module::single_Calculation()
{
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		// preparing trees
		if( multilayers[tab_Index]->enable_Calc_Independent_Curves ||
			multilayers[tab_Index]->enable_Calc_Target_Curves )
		{
			calculation_Trees[tab_Index]->fill_All_Calc_Trees();
			calculation_Trees[tab_Index]->calculate();
		}
	}
}
