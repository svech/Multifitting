#ifndef MY_TABLE_WIDGET_H
#define MY_TABLE_WIDGET_H

#include "global/settings.h"

class My_Table_Widget : public QTableWidget
{
	Q_OBJECT
public:
	My_Table_Widget(int rows, int columns, QWidget *parent = Q_NULLPTR);
//	explicit My_Table_Widget(QWidget *parent = nullptr);

	void contextMenuEvent(QContextMenuEvent *event);

	void cell_Response();

	void add_Slave_Func(int id, QMenu* menu);

	QLabel* label;
	QWidgetAction* a_Label;
	QAction* my_Id;
	QMenu* master;
	QVector<QMenu*> slaves;
	QAction* add_Slave;

	bool get_Id_From_Cell = false;
};

#endif // MY_TABLE_WIDGET_H
