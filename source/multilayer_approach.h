#ifndef MULTILAYER_APPROACH_H
#define MULTILAYER_APPROACH_H

#include "standard/menu.h"
#include "multilayer_approach/multilayer.h"
#include "main_calculation_module.h"
#include "calculation/optical_constants.h"

class Multilayer_Approach : public QWidget
{
	Q_OBJECT
public:
	explicit Multilayer_Approach(Optical_Constants* optical_Constants, QWidget *parent = 0);

signals:
	void closed();
	void refresh();
public slots:
	void refresh_All_Multilayers_View();
	void open();
	void save();
	void calc_Reflection();
	void reload_Optical_Constants();

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

private:
	Optical_Constants* optical_Constants;
	QList<Multilayer*> multilayer_List;

private:
	QHBoxLayout* main_Layout;
		QTabWidget*	multilayer_Tabs;
		QToolButton* add_Tab_Corner_Button;
};

#endif // MULTILAYER_APPROACH_H
