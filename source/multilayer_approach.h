#ifndef MULTILAYER_APPROACH_H
#define MULTILAYER_APPROACH_H

#include "standard/menu.h"
#include "multilayer_approach/multilayer.h"

class Multilayer_Approach : public QWidget
{
	Q_OBJECT
public:
	explicit Multilayer_Approach(QWidget *parent = 0);

signals:
	void closed();
	void refresh();
public slots:
	void refresh_All_Multilayers_View();
	void open();
	void save();

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
	QList<Multilayer*> multilayer_List;

private:
	QHBoxLayout* main_Layout;
		QTabWidget*	multilayer_Tabs;
		QToolButton* add_Tab_Corner_Button;
};

#endif // MULTILAYER_APPROACH_H
