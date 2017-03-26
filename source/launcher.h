#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "standard/menu.h"
#include "multilayer_approach.h"

class Multilayer_Approach;

class Launcher : public QWidget
{
	Q_OBJECT
public:
	Launcher(QWidget *parent = 0);

private:
	void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
		void create_Menu();
		void create_Buttons();
	void set_Window_Geometry();

private slots:
	void add_Multilayer_Approach_Instance();
	void multilayer_Approach_Closed();
	void add_Freeform_Approach_Instance();
	void freeform_Approach_Closed();

//	void refresh_All_Multilayer_Approach_View();

public:
	QList<Multilayer_Approach*> multilayer_Approach_List;

private:
	QHBoxLayout* main_Layout;
		QPushButton* multilayer_Approach_Launch_Button;
		QPushButton* freeform_Approach_Launch_Button;
};

#endif // LAUNCHER_H
