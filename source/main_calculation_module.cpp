#include "main_calculation_module.h"

Main_Calculation_Module::Main_Calculation_Module(QList<Multilayer*> multilayer_List)
{
	for(int i=0; i<multilayer_List.size(); ++i)
	{
		Calculation_Tree calculation_Tree(multilayer_List[i]->structure_Tree->tree);
	}
}
