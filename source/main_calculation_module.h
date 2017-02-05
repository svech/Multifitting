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

	QTabWidget*	multilayer_Tabs;

	void run_All();
signals:
	void critical   (QString critical_Text);
	void warning    (QString warning_Text);
	void information(QString information_Text);

public slots:
	void catch_Critical   (QString critical_Text);
	void catch_Warning    (QString warning_Text);
	void catch_Information(QString information_Text);
};

#endif // MAIN_CALCULATION_MODULE_H
