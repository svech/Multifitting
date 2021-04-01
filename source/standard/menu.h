#ifndef MENU_H
#define MENU_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/structure_tree/item_editor.h"
#include "multilayer_approach/multilayer/independent_curve/independent_curve_editor.h"

class Item_Editor;
class Independent_Curve_Editor;

class Menu : public QWidget
{
	Q_OBJECT
public:
	explicit Menu(QString window_Type, QWidget* parent = 0);
	~Menu();

signals:
	void refresh();

public:
	QString window_Type;
	Item_Editor* item_Editor;
	QObject* my_Parent;

	void add_Menu_Points();
		void create_File_Menu();
		void create_Calculate_Menu();
		void create_Optical_Constants_Menu();
		void create_Help_Menu();

		void create_Length_Units_Menu();
		void create_Other_Units_Menu();
		void create_Precision_Menu();

//// menu actions

	void open_Documentation_Rus();
	void open_Documentation_Eng();
	void open_About();

	QMenuBar* menu_Bar;
		QMenu* file_Menu;
		QMenu* calculate_Menu;
		QMenu* optical_Constants_Menu;
		QMenu* help_Menu;

		QMenu* menu_Length_Units;
		QMenu* menu_Other_Units;
		QMap<QMenu*, QString*> units_Menu_Map;
		QMap<QMenu*, QStringList*> units_List_Menu_Map;

		QMenu* precision_Menu;
		QMap<QMenu*, int*> precision_Menu_Map;
};

#endif // MENU_H
