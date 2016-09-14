#ifndef MULTILAYER_APPROACH_H
#define MULTILAYER_APPROACH_H

#include <QtWidgets>
#include "settings.h"
#include "optical_constants.h"

class Multilayer_Approach : public QWidget
{
	Q_OBJECT

public:
	Multilayer_Approach(Settings* settings);
	~Multilayer_Approach();

	Settings* settings;
	Optical_Constants* optical_Constants;

private slots:
	void open_Documentation();
	void open_About();

private:
	void closeEvent(QCloseEvent *event);
	void set_Window_Geometry();
	void create_Menu();

// ui objects to be placed in launcher window
private:
	QHBoxLayout *main_Layout;

	// menu
	QMenuBar	*menu_Bar;
		QMenu		*file_Menu;
			QAction		*act_Quit;
		QMenu		*help_Menu;
			QAction		*act_Documentation;
			QAction		*act_About;
//---------------------------------------------


};

#endif // MULTILAYER_APPROACH_H
