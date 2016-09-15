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
	~Launcher();

	Settings* settings;

private slots:
	void on_Button_Multilayer_Clicked();
	void on_Button_Freeform_Clicked();

	void change_Dir_Multilayer();
	void open_Documentation();
	void open_About();

private:
	void closeEvent(QCloseEvent *event);
	void create_Menu();
	void create_Buttons();
	void set_Window_Geometry();
	void change_Dir(QString& directory);

// ui objects to be placed in launcher window
private:
	QHBoxLayout *main_Layout;
	QMenuBar	*menu_Bar;
	QPushButton *button_Multilayer_Launch;
	QPushButton *button_Freeform_Launch;
	QMenu		*file_Menu;
	QMenu		*help_Menu;

// actions
private:
	QAction *act_Change_Dir_Multilayer;
	QAction *act_Quit;
	QAction *act_Documentation;
	QAction *act_About;
};

#endif // LAUNCHER_H
