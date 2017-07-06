#include "my_table_widget.h"

My_Table_Widget::My_Table_Widget(int rows, int columns, QWidget *parent) : QTableWidget(parent)
{
	for(int i=0; i<rows; ++i)		insertRow(i);
	for(int i=0; i<columns; ++i)	insertColumn(i);
}

void My_Table_Widget::contextMenuEvent(QContextMenuEvent *event)
{
	int row = currentRow();
	int column = currentColumn();;
	qInfo() << "row = " << row << "column = " << column;

	QWidget* widget = cellWidget(row,column);

	if(widget)
	if(!widget->whatsThis().isEmpty())
	{
		qInfo() << "wtf = " << widget->whatsThis();
		QMenu menu(this);
		menu.addAction(new QAction("id = " + widget->whatsThis()));
		menu.exec(event->globalPos());
	}
}

//My_Table_Widget::My_Table_Widget(QWidget *parent) : QWidget(parent)
//{

//}
