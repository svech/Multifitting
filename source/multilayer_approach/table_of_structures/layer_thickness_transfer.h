#ifndef LAYER_THICKNESS_TRANSFER_H
#define LAYER_THICKNESS_TRANSFER_H

#include "table_of_structures.h"

class Layer_Thickness_Transfer : public QDialog
{
	Q_OBJECT
public:
	explicit Layer_Thickness_Transfer(QWidget* item_Widget,
									  Table_Of_Structures* table_Of_Structures,
									  QWidget* parent = nullptr);

	void closeEvent(QCloseEvent*);
	void set_Window_Geometry();
	void create_Main_Layout();
	void create_Content_Box();
	void create_Layer_Lines();


	QWidget* item_Widget;
	QTreeWidgetItem* struct_Item;
	Table_Of_Structures* table_Of_Structures;
	Data struct_Data;

	QVBoxLayout* main_Layout;
		QGroupBox* content_Group_Box;
			QVBoxLayout* content_Group_Box_Layout;
};

#endif // LAYER_THICKNESS_TRANSFER_H
