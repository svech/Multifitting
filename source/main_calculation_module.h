#ifndef MAIN_CALCULATION_MODULE_H
#define MAIN_CALCULATION_MODULE_H

#include "global/layer_data_types.h"
#include "calculation/calculation_tree.h"
#include "multilayer_approach/multilayer.h"

class Multilayer;

class Main_Calculation_Module: public QObject
{
	Q_OBJECT
public:
	Main_Calculation_Module(QTabWidget*	multilayer_Tabs);

	QVector<Multilayer*> multilayers;
	QVector<Calculation_Tree*> calculation_Trees;

	void single_Calculation();

	void run_All();
};

#endif // MAIN_CALCULATION_MODULE_H
