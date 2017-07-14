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
		qInfo() << "parameter id = " << parameter.id << "\n" << parameter.full_Name << endl;

		QMenu menu;
		QAction my_Name_Action(parameter.full_Name);
		menu.addAction(&my_Name_Action);

		connect(&my_Name_Action, &QAction::triggered, [=]{ open_Coupling_Editor(parameter);});

		menu.exec(event->globalPos());
	}
}

void My_Table_Widget::open_Coupling_Editor(Parameter parameter)
{
	Coupling_Editor* new_Coupling_Editor = new Coupling_Editor(parameter,this);
		new_Coupling_Editor->setModal(true);
		new_Coupling_Editor->show();
}
