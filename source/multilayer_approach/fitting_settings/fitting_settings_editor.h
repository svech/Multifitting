#ifndef FITTING_SETTINGS_EDITOR_H
#define FITTING_SETTINGS_EDITOR_H

#include "multilayer_approach/multilayer_approach.h"
#include "fitting_settings.h"

class Fitting_Settings_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Fitting_Settings_Editor(Multilayer_Approach* multilayer_Approach, QWidget *parent = nullptr);

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
	void set_Window_Geometry();
	void create_Menu();

	Multilayer_Approach* multilayer_Approach;
	Fitting_Settings* fitting_Settings;

	QVBoxLayout* main_Layout;
		QListWidget* methods_List;

		QPushButton* done_Button;
};

#endif // FITTING_SETTINGS_EDITOR_H
