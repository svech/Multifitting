#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QtWidgets>
#include "settings.h"
#include "multilayer_approach.h"

class Launcher : public QWidget
{
	Q_OBJECT

public:
	Launcher();

	Settings* settings;

private slots:
	void on_Button_Multilayer_Clicked();
	void on_Button_Freeform_Clicked();

	void open_Documentation();
	void open_About();

private:
	void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
		void create_Menu();
			void create_Buttons();
	void set_Window_Geometry();

// ui objects to be placed in launcher window
private:
	QHBoxLayout* main_Layout;
		QMenuBar* menu_Bar;
			QMenu* file_Menu;
			QMenu* help_Menu;
		QPushButton* button_Multilayer_Launch;
		QPushButton* button_Freeform_Launch;

};

#endif // LAUNCHER_H
