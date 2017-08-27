#include "my_table_widget.h"

My_Table_Widget::My_Table_Widget(int rows, int columns, QTabWidget* main_Tabs, QWidget *parent) :
	main_Tabs(main_Tabs),
	QTableWidget(parent)
{
	for(int i=0; i<rows; ++i)		insertRow(i);
	for(int i=0; i<columns; ++i)	insertColumn(i);
}

void My_Table_Widget::contextMenuEvent(QContextMenuEvent *event)
{
	int row = currentRow();
	int column = currentColumn();;

	QWidget* widget = cellWidget(row,column);

	if(widget)
	if(!widget->whatsThis().isEmpty())
	{
		Parameter parameter = widget->property(parameter_Property).value<Parameter>();
		qInfo() << "parameter id = " << parameter.indicator.id << "\n" << main_Tabs->tabText(parameter.indicator.tab_Index) << " " << parameter.indicator.full_Name << endl;

		QMenu menu;
		QAction my_Name_Action(parameter.indicator.full_Name);
		menu.addAction(&my_Name_Action);

		connect(&my_Name_Action, &QAction::triggered, [=]{ open_Coupling_Editor(widget);});

		menu.exec(event->globalPos());
	}
}

void My_Table_Widget::open_Coupling_Editor(QWidget* coupling_Widget)
{
	Coupling_Editor* new_Coupling_Editor = new Coupling_Editor(coupling_Widget, main_Tabs, this);
//		new_Coupling_Editor->setModal(true);
		new_Coupling_Editor->show();
}

QWidget* My_Table_Widget::get_Cell_Widget()
{
	QWidget* widget = cellWidget(currentRow(),currentColumn());

//	if(widget)
//	{
//		widget->setProperty(row_Property,currentRow());
//		widget->setProperty(column_Property,currentColumn());
//	}

	return widget;
}

