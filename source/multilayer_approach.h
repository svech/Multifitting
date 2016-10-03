#ifndef MULTILAYER_APPROACH_H
#define MULTILAYER_APPROACH_H

#include <QtWidgets>
#include "launcher.h"
#include "multilayer/multilayer.h"
#include "global_definitions.h"

class Multilayer_Approach : public QWidget
{
	Q_OBJECT

public:
	Multilayer_Approach();

	QSettings gui_Settings;

	QVector<Multilayer*> multilayer;

private:
	void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
		void create_Menu();
		void create_Multilayer_Tabs();
	void set_Window_Geometry();


private slots:
	void change_Tab_Color(int index);
	void add_Multilayer();
	void remove_Multilayer(int index);
	void rename_Multilayer(int tab_Index);

	void open_Documentation();
	void open_About();

// ui objects to be placed in launcher window
private:
	QHBoxLayout* main_Layout;
		QMenuBar* menu_Bar;
			QMenu* file_Menu;
			QMenu* help_Menu;
		QTabWidget*	multilayer_Tabs;
		QToolButton* add_Tab_button;

};

#endif // MULTILAYER_APPROACH_H
