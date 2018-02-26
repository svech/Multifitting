#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "standard/menu.h"
#include "multilayer_approach/multilayer_approach.h"

class Multilayer_Approach;

class Launcher : public QWidget
{
	Q_OBJECT
public:
	Launcher(QWidget *parent = 0);

	void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
		void create_Menu();
		void create_Buttons();
	void set_Window_Geometry();
	void open_Multilayer_Approach();

	QMap<QString, Multilayer_Approach*> runned_Multilayer_Approaches;

	QHBoxLayout* main_Layout;
		QPushButton* multilayer_Approach_Launch_Button;
};

#endif // LAUNCHER_H
