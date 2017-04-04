#include "table_of_structures.h"

Table_Of_Structures::Table_Of_Structures(QList<Table_Of_Structures*>* table_Of_Structures_List, QWidget *parent) :
	table_Of_Structures_List(table_Of_Structures_List),
	QWidget(parent)
{
	setWindowTitle("Table Of Structures");
	create_Main_Layout();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Table_Of_Structures::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
		main_Layout->setContentsMargins(4,3,2,3);
	main_Table = new QTableWidget;
		main_Layout->addWidget(main_Table);
}

void Table_Of_Structures::closeEvent(QCloseEvent* event)
{
	qInfo() << "closeEvent";
	event;
	qInfo() << "before " << table_Of_Structures_List->size();
	table_Of_Structures_List->clear();
	qInfo() << "after " << table_Of_Structures_List->size();
}
