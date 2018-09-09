// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "my_table_widget.h"

My_Table_Widget::My_Table_Widget(int rows,
								 int columns,
								 Table_Of_Structures* table_Of_Structures,
								 QWidget *parent) :
	table_Of_Structures(table_Of_Structures),
	QTableWidget(parent)
{
	horizontalHeader()->setDisabled(true);
	verticalHeader()->setDisabled(true);
	setAttribute(Qt::WA_DeleteOnClose);
	for(int i=0; i<rows; ++i)		insertRow(i);
	for(int i=0; i<columns; ++i)	insertColumn(i);
}

My_Table_Widget::~My_Table_Widget()
{
}

void My_Table_Widget::contextMenuEvent(QContextMenuEvent *event)
{
	int row = currentRow();
	int column = currentColumn();

	QWidget* back_Widget = cellWidget(row,column);

	if(back_Widget)
	if(back_Widget->property(coupling_Editor_Property).toBool())
	{
		id_Type id = table_Of_Structures->coupled_Back_Widget_and_Id.value(back_Widget);
		QTreeWidgetItem* struct_Item = table_Of_Structures->coupled_Back_Widget_and_Struct_Item.value(back_Widget);
		Data struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		Parameter* parameter = Global_Variables::get_Parameter_From_Struct_Item_by_Id(struct_Data, id);

//		qInfo() << "parameter id = " << parameter->indicator->id << "\n" << main_Tabs->tabText(parameter->indicator->tab_Index) << " " << parameter->indicator->full_Name << endl;

		// period and gamma can't be connected
//		if(parameter->indicator.whats_This == whats_This_Period || parameter->indicator.whats_This == whats_This_Gamma) return;

		QMenu menu;
		QAction my_Name_Action(parameter->indicator.full_Name);
		menu.addAction(&my_Name_Action);

		connect(&my_Name_Action, &QAction::triggered, [=]{ open_Coupling_Editor(back_Widget);});

		menu.exec(event->globalPos());
	}
}

void My_Table_Widget::open_Coupling_Editor(QWidget* coupling_Widget)
{
	Coupling_Editor* new_Coupling_Editor = new Coupling_Editor(coupling_Widget, table_Of_Structures, this);
		new_Coupling_Editor->show();
}

QWidget* My_Table_Widget::get_Cell_Widget()
{
	return cellWidget(currentRow(),currentColumn());
}

