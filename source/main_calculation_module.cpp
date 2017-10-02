// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "main_calculation_module.h"

Main_Calculation_Module::Main_Calculation_Module(QTabWidget*	multilayer_Tabs):
	multilayer_Tabs(multilayer_Tabs)
{
}

void Main_Calculation_Module::run_All()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));

		Calculation_Tree* calculation_Tree = new Calculation_Tree(multilayer->independent_Variables_Plot_Tabs, multilayer->target_Profiles_Vector, multilayer->structure_Tree->tree);
		connect(calculation_Tree, SIGNAL(critical(QString)),    this, SLOT(catch_Critical(QString)));
		connect(calculation_Tree, SIGNAL(warning(QString)),     this, SLOT(catch_Warning(QString)));
		connect(calculation_Tree, SIGNAL(information(QString)), this, SLOT(catch_Information(QString)));
		calculation_Tree->run_All();
		delete calculation_Tree;
	}
}

void Main_Calculation_Module::catch_Critical(QString critical_Text)
{
	emit critical(critical_Text);
}

void Main_Calculation_Module::catch_Warning(QString warning_Text)
{
	emit warning(warning_Text);
}

void Main_Calculation_Module::catch_Information(QString information_Text)
{
	emit information(information_Text);
}
