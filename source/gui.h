#ifndef GUI_H
#define GUI_H

#include <QtWidgets>

class Gui
{
public:
	Gui();

	// launcher window geometry
	int launcher_X_Corner = 300;
	int launcher_Y_Corner = 300;

	// multilayer window geometry
	int multilayer_X_Corner = 500;
	int multilayer_Width = 531;
	int multilayer_Min_Width = 300;
	int multilayer_Y_Corner = 200;
	int multilayer_Height = 500;
	int multilayer_Min_Height = 100;

	int multilayer_Height_Additive = 23;
	QString what_is_This_Ambient = "ambient";
	QString what_is_This_Substrate = "substrate";

	// multilayer window names
	QString default_Multilayer_Tab_Name = "Struc_";

	// resource path
	QString icon_Path = "../../imd icons/";

	//	specular_Group_Box->setStyleSheet("QGroupBox { border: 1px solid gray; border-radius: 2px; margin-top: 1ex; }");
	//	nonspecular_Group_Box->setStyleSheet("QGroupBox { border: 1px solid gray; border-radius: 2px; margin-top: 1ex; }");

};

#endif // GUI_H
