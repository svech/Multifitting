#include "fitting_settings_editor.h"

Fitting_Settings_Editor::Fitting_Settings_Editor(Multilayer_Approach* multilayer_Approach, QWidget* parent) :
	multilayer_Approach(multilayer_Approach),
	QWidget(parent)
{
	setWindowTitle("Fitting Settings");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Fitting_Settings_Editor::closeEvent(QCloseEvent *event)
{
	multilayer_Approach->runned_Fitting_Settings_Editor.remove(fit_Settings_Key);
	event->accept();
}

void Fitting_Settings_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	create_Menu();
}

void Fitting_Settings_Editor::set_Window_Geometry()
{
	setGeometry(430,498,width(),height());
}

void Fitting_Settings_Editor::create_Menu()
{
	Menu* menu = new Menu(window_Type_Fitting_Settings_Editor,this);
		main_Layout->setMenuBar(menu->menu_Bar);
}
