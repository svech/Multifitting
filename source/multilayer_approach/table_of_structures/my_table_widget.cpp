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
		Parameter parameter = widget->property(parameter_Property).value<Parameter>();
		qInfo() << "from parameter = " << parameter.id;

		QMenu menu;
		label = new QLabel("id = " + QString::number(parameter.id), &menu);
			a_Label = new QWidgetAction(&menu);
			a_Label->setDefaultWidget(label);
			menu.addAction(a_Label);
		master = new QMenu ("master <id>");
			menu.addMenu(master);

		slaves.resize(parameter.coupled.slaves_Id.size());
		for(int i=0; i<slaves.size(); ++i)
		{
			add_Slave_Func(parameter.coupled.slaves_Id[i], &menu);
		}

		add_Slave = new QAction("add slave", &menu);
		connect(add_Slave, &QAction::triggered, [=]{ add_Slave_Func(-2017,&menu);});
			menu.addAction(add_Slave);

		menu.exec(event->globalPos());

	//		connect(this, &My_Table_Widget::con, [=]{cell_Response();});
	}
}

void My_Table_Widget::cell_Response()
{
	int row = currentRow();
	int column = currentColumn();;

	QWidget* widget = cellWidget(row,column);

	if(widget)
	if(!widget->whatsThis().isEmpty())
	{
		qInfo() << "target id = " << widget->whatsThis();
	}
}

void My_Table_Widget::add_Slave_Func(int id, QMenu* menu)
{
	QMenu* new_Slave = new QMenu(QString::number(id), menu);
	slaves.append(new_Slave);
	qInfo() << "new_Slave";
}



//My_Table_Widget::My_Table_Widget(QWidget *parent) : QWidget(parent)
//{

//}
