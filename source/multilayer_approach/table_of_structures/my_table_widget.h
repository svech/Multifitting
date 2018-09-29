#ifndef MY_TABLE_WIDGET_H
#define MY_TABLE_WIDGET_H

#include "global/settings.h"
#include "coupling_editor.h"
#include "layer_thickness_transfer.h"

class Table_Of_Structures;

class My_Table_Widget : public QTableWidget
{
	Q_OBJECT
public:
	My_Table_Widget(int rows,
					int columns,
					Table_Of_Structures* table_Of_Structures,
					QWidget* parent = Q_NULLPTR);
	~My_Table_Widget();

	Table_Of_Structures* table_Of_Structures;

	void contextMenuEvent(QContextMenuEvent *event);
	void open_Coupling_Editor(QWidget *coupling_Widget);
	void open_Layer_Thickness_Transfer(QWidget *coupling_Widget);

	QWidget* get_Cell_Widget();
};

#endif // MY_TABLE_WIDGET_H
