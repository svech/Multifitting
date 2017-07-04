#include "my_table_widget.h"

My_Table_Widget::My_Table_Widget(int rows, int columns, QWidget *parent) : QTableWidget(parent)
{
	for(int i=0; i<rows; ++i)		insertRow(i);
	for(int i=0; i<columns; ++i)	insertColumn(i);
}

void My_Table_Widget::contextMenuEvent(QContextMenuEvent *event)
{
	qInfo() << "row = " << currentRow() << "column = " << currentColumn();
}

//My_Table_Widget::My_Table_Widget(QWidget *parent) : QWidget(parent)
//{

//}
