#ifndef MAIN_CALCULATION_MODULE_H
#define MAIN_CALCULATION_MODULE_H

#include "global/layer_data_types.h"
#include "calculation/calculation_tree.h"
#include "multilayer_approach/multilayer.h"

class Main_Calculation_Module: public QObject
{
	Q_OBJECT
public:
	Main_Calculation_Module(QList<Multilayer*> multilayer_List);

	QList<Multilayer*> multilayer_List;

	void run_All();
signals:
	void error(QString error_Text);
public slots:
	void catch_Error(QString error_Text);
};

#endif // MAIN_CALCULATION_MODULE_H
