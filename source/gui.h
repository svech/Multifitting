#ifndef GUI_H
#define GUI_H

class Gui
{
public:
	Gui();

	// launcher window geometry
	int launcher_X_Corner = 400;
	int launcher_Y_Corner = 400;

	// multilayer window geometry
	int multilayer_X_Corner = 700;
	int multilayer_Width = 500;
	int multilayer_Min_Width = 300;
	int multilayer_Y_Corner = 500;
	int multilayer_Height = 400;
	int multilayer_Min_Height = 100;
};

#endif // GUI_H
