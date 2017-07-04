#ifndef MY_TABLE_WIDGET_H
#define MY_TABLE_WIDGET_H

#include <QtWidgets>

class My_Table_Widget : public QTableWidget
{
	Q_OBJECT
public:
	My_Table_Widget(int rows, int columns, QWidget *parent = Q_NULLPTR);
//	explicit My_Table_Widget(QWidget *parent = nullptr);

	void contextMenuEvent(QContextMenuEvent *event);

signals:

public slots:
};

#endif // MY_TABLE_WIDGET_H
