#ifndef MULTILAYER_APPROACH_H
#define MULTILAYER_APPROACH_H

#include <QtWidgets>
#include "settings.h"
#include "launcher.h"
#include "optical_constants.h"
#include "multilayer/multilayer.h"

class Multilayer_Approach : public QWidget
{
	Q_OBJECT

public:
	Multilayer_Approach(Settings* settings);
	~Multilayer_Approach();

	Settings* settings;
	Optical_Constants* optical_Constants;

	QVector<Multilayer*> multilayer;

private slots:
	void add_Multilayer(int tab_Index);
	void rename_Multilayer(int tab_Index);
	void open_Documentation();
	void open_About();

private:
	void closeEvent(QCloseEvent *event);
	void set_Window_Geometry();
	void create_Menu_Bar();
	void create_Structure_Tabs();


// ui objects to be placed in launcher window
private:
	QHBoxLayout *main_Layout;

	// menu
	QMenuBar*	menu_Bar;
		QMenu*		file_Menu;
			QAction*	act_Quit;
		QMenu*		help_Menu;
			QAction*	act_Documentation;
			QAction*		act_About;

	QTabWidget*	structure_Tabs;

};

#endif // MULTILAYER_APPROACH_H
