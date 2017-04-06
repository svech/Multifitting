#include "table_of_structures.h"

Table_Of_Structures::Table_Of_Structures(QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures, QTabWidget* multilayer_Tabs, QWidget *parent) :
	runned_Tables_Of_Structures(runned_Tables_Of_Structures),
	multilayer_Tabs(multilayer_Tabs),
	QWidget(parent)
{
	setWindowTitle("Table Of Structures");
	create_Main_Layout();
	read_Trees();

	setAttribute(Qt::WA_DeleteOnClose);
}

void Table_Of_Structures::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
		main_Layout->setContentsMargins(4,3,2,3);	
	create_Table();
	main_Layout->addWidget(main_Table);
}

void Table_Of_Structures::create_Table()
{
	read_Trees();
	main_Table = new QTableWidget(basic_Row_Number, basic_Column_Number, this);

	// set prototype
	QTableWidgetItem* proto_Item = new QTableWidgetItem("prototype");
	proto_Item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	main_Table->setItemPrototype(proto_Item);

	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		// header for each structure
		main_Table->insertRow(main_Table->rowCount());
						int current_Row = main_Table->rowCount()-1;
		main_Table->setSpan(current_Row,0,1,basic_Column_Number);
		main_Table->setItem(current_Row,0, new QTableWidgetItem(multilayer_Tabs->tabText(i)));
		main_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);

		main_Table->insertRow(main_Table->rowCount());

		// structure display
		int depth;
		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(list_Of_Trees[i]->tree);
		while (*it)
		{
			structure_Item = *it;
			depth = Global_Variables::get_Item_Depth(structure_Item);

			main_Table->insertRow(main_Table->rowCount());
								current_Row = main_Table->rowCount()-1;
			main_Table->setItem(current_Row,depth, new QTableWidgetItem(structure_Item->whatsThis(DEFAULT_COLUMN)));
			++it;
		}

		main_Table->insertRow(main_Table->rowCount());
	}
	main_Table->resizeColumnsToContents();
}

void Table_Of_Structures::read_Trees()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Structure_Tree* old_Structure_Tree = dynamic_cast<Multilayer*>(multilayer_Tabs->widget(i))->structure_Tree;
		list_Of_Trees.append(old_Structure_Tree);
	}
}

void Table_Of_Structures::closeEvent(QCloseEvent* event)
{
	event;
	runned_Tables_Of_Structures->remove(table_Key);
}
