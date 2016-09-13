#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

// use console or graphical interface
enum class Gui_Mode	{gui_On, gui_Off};
#define GUI_MODE Gui_Mode::gui_On
#define IF_GUI if(GUI_MODE == Gui_Mode::gui_On)

//----------------------------------------------



#endif // GLOBAL_DEFINITIONS_H
