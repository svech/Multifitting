#ifndef MENU_H
#define MENU_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/independent_curve/independent_curve_editor.h"
#include "multilayer_approach/multilayer/structure_tree/item_editor.h"

class Independent_Curve_Editor;
class Item_Editor;

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
	Independent_Curve_Editor* independent_Variables_Editor;
	Item_Editor* item_Editor;
	QObject* my_Parent;

	// state variables
	QString precision_Menu_Thumb_Edit;
	QString precision_Menu_Title;


	void add_Menu_Points();
		void create_File_Menu();
		void create_Calculate_Menu();
		void create_Optical_Constants_Menu();
		void create_Help_Menu();

		void create_Independent_Units_Menu();
		void create_Independent_Precision_Menu();

		void create_Item_Length_Units_Menu();
		void create_Item_Precision_Menu();

		void create_Length_Units_Menu();
		void create_Table_Precision_Menu();

//// menu actions

	void menu_Focus();
	void thumb_Edit_Focus();

	void open_Documentation_Rus();
	void open_Documentation_Eng();
	void open_About();
	void set_Length_Unit();
	void activate_Item_Thumbnail_Precision();
	void activate_Item_Line_Edit_Precision();

	// precisions
	void set_Thumbnail_Density_Precision();
	void set_Line_Edit_Density_Precision();
	void set_Thumbnail_Permittivity_Precision();
	void set_Line_Edit_Permittivity_Precision();
	void set_Thumbnail_Absorption_Precision();
	void set_Line_Edit_Absorption_Precision();
	void set_Thumbnail_Composition_Precision();
	void set_Line_Edit_Composition_Precision();
	void set_Thumbnail_Thickness_Precision();
	void set_Line_Edit_Thickness_Precision();
	void set_Thumbnail_Sigma_Precision();
	void set_Line_Edit_Sigma_Precision();
	void set_Thumbnail_Interlayer_Precision();
	void set_Line_Edit_Interlayer_Precision();
	void set_Thumbnail_Period_Precision();
	void set_Line_Edit_Period_Precision();
	void set_Thumbnail_Gamma_Precision();
	void set_Line_Edit_Gamma_Precision();

	QMenuBar* menu_Bar;
		QMenu* file_Menu;
		QMenu* calculate_Menu;
		QMenu* optical_Constants_Menu;
		QMenu* help_Menu;
		QMenu* units_Menu;		
		QMenu* menu_Wavelength_Units;
		QMenu* menu_Length_Units;
		QMenu* precision_Menu;
			QMenu* menu_Density;
			QMenu* menu_Lengths;
			QMenu* menu_Gamma;
			QMenu* menu_Composition;
			QMenu* menu_Interlayer;
				QMenu* menu_Thumb;
				QMenu* menu_Edit;
};

#endif // MENU_H
