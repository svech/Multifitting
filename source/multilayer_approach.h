#ifndef MULTILAYER_APPROACH_H
#define MULTILAYER_APPROACH_H

#include "launcher.h"
#include "standard/menu.h"
#include "multilayer_approach/multilayer.h"
#include "main_calculation_module.h"
#include "multilayer_approach/table_of_structures.h"

class Launcher;
class Multilayer;
class Table_Of_Structures;

class Multilayer_Approach : public QWidget
{
	Q_OBJECT
public:
	explicit Multilayer_Approach(Launcher* launcher, QWidget *parent = 0);

signals:
	void closed();

public slots:
	void open_Launcher();
	void refresh_All_Multilayers_View();
	void open();
	void save();
	void calc_Reflection();
	void reload_Optical_Constants();
	void open_Table_Of_Structures(bool);

private slots:
	// errors
	void catch_Critical   (QString critical_Text);
	void catch_Warning    (QString warning_Text);
	void catch_Information(QString information_Text);

private:
	void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
		void create_Menu();
		void create_Multilayer_Tabs();
	void set_Window_Geometry();

private slots:
	void add_Multilayer();
	void remove_Multilayer(int index);
	void change_Tab_Color(int index);
	void rename_Multilayer(int tab_Index);

public:
	Launcher* launcher;					// "parent"
private:
	QHBoxLayout* main_Layout;
public:
		QTabWidget*	multilayer_Tabs;
		QList<Table_Of_Structures*>* table_Of_Structures_List;
		Table_Of_Structures* table_Of_Structures;
private:
		QToolButton* add_Tab_Corner_Button;
};

#endif // MULTILAYER_APPROACH_H
