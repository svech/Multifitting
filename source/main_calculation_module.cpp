#include "main_calculation_module.h"

Main_Calculation_Module::Main_Calculation_Module(QList<Multilayer*> multilayer_List):
	multilayer_List(multilayer_List)
{

}

void Main_Calculation_Module::run_All()
{
	for(int i=0; i<multilayer_List.size(); ++i)
	{
		Calculation_Tree* calculation_Tree = new Calculation_Tree(multilayer_List[i]->independent_Widget_Vec);
		connect(calculation_Tree, SIGNAL(error(QString)), this, SLOT(catch_Error(QString)));
		calculation_Tree->run_All();
		delete calculation_Tree;
	}
}

void Main_Calculation_Module::catch_Error(QString error_Text)
{
	emit error(error_Text);
}
