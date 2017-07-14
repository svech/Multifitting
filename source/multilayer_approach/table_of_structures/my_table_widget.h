#ifndef MY_TABLE_WIDGET_H
#define MY_TABLE_WIDGET_H

#include "global/settings.h"
#include "coupling_editor.h"

class My_Table_Widget : public QTableWidget
{
	Q_OBJECT
public:
	My_Table_Widget(int rows, int columns, QWidget *parent = Q_NULLPTR);

	void contextMenuEvent(QContextMenuEvent *event);
	void open_Coupling_Editor(Parameter parameter);
};

#endif // MY_TABLE_WIDGET_H
