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
		main_Layout->setContentsMargins(0,0,0,0);
	create_Table();	
	main_Layout->addWidget(main_Table);
	resize(800,300);
}

void Table_Of_Structures::create_Table()
{
	// PARAMETER

	read_Trees();
	main_Table = new QTableWidget(basic_Row_Number, basic_Column_Number, this);
	rows_List_To_Span.clear();

	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		// header for each structure
		main_Table->insertRow(main_Table->rowCount());
		int current_Row = main_Table->rowCount()-1;
		rows_List_To_Span.append(current_Row);
		main_Table->setItem(current_Row,0, new QTableWidgetItem(multilayer_Tabs->tabText(tab_Index)));
		main_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);
		main_Table->item   (current_Row,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

		main_Table->insertRow(main_Table->rowCount());

		// structure display
		int max_Depth = Global_Variables::get_Tree_Depth(list_Of_Trees[tab_Index]->tree->invisibleRootItem());
		int depth;
		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);
		while (*it)
		{
			structure_Item = *it;
			depth = Global_Variables::get_Item_Depth(structure_Item);
			main_Table->insertRow(main_Table->rowCount());
			current_Row = main_Table->rowCount()-1;

			// print whatsThis
			add_Columns(depth);
			main_Table->setItem(current_Row,depth, new QTableWidgetItem(structure_Item->whatsThis(DEFAULT_COLUMN)));
			{
				main_Table->item(current_Row,depth)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			}

			// load data
			QStringList whats_This_List = structure_Item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);
			if(whats_This_List[0] == whats_This_Ambient)	{item_Type = Item_Type::Ambient;		ambient		  = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();		}
			if(whats_This_List[0] == whats_This_Layer)		{item_Type = Item_Type::Layer;			layer		  = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();			}
			if(whats_This_List[0] == whats_This_Multilayer) {item_Type = Item_Type::Stack_Content;	stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();	}
			if(whats_This_List[0] == whats_This_Substrate)	{item_Type = Item_Type::Substrate;		substrate	  = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();		}

			// common variables
			int current_Column = -5;
			bool composed_Material;
			QString material = "empty";
			QList<Stoichiometry> composition;

			if(item_Type == Item_Type::Ambient)
			{
				composed_Material = ambient.composed_Material;
				composition = ambient.composition;
				material = ambient.material;
			}
			if(item_Type == Item_Type::Layer)
			{
				composed_Material = layer.composed_Material;
				composition = layer.composition;
				material = layer.material;
			}
			if(item_Type == Item_Type::Stack_Content)
			{

			}
			if(item_Type == Item_Type::Substrate)
			{
				composed_Material = substrate.composed_Material;
				composition = substrate.composition;
				material = substrate.material;
			}

			current_Column = max_Depth+2;

			if(item_Type == Item_Type::Ambient || item_Type == Item_Type::Layer || item_Type == Item_Type::Substrate)
			if(composed_Material)
			{
				for(int comp_Index=0; comp_Index<composition.size(); ++comp_Index)
				{
					add_Columns(current_Column);
					main_Table->setItem(current_Row,current_Column, new QTableWidgetItem(composition[comp_Index].type));
					main_Table->item(current_Row,current_Column)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
					++current_Column;
				}
			} else
			{
				add_Columns(current_Column);
				main_Table->setItem(current_Row,current_Column, new QTableWidgetItem(material));
				main_Table->item(current_Row,current_Column)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				++current_Column;
			}




			++it;
		}

		main_Table->insertRow(main_Table->rowCount());
	}
	span_Title_Rows();
	main_Table->resizeColumnsToContents();
	main_Table->resizeRowsToContents();
}

void Table_Of_Structures::read_Trees()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Structure_Tree* old_Structure_Tree = dynamic_cast<Multilayer*>(multilayer_Tabs->widget(i))->structure_Tree;
		list_Of_Trees.append(old_Structure_Tree);
		old_Structure_Tree->structure_Toolbar->toolbar->setDisabled(true);
	}
}

void Table_Of_Structures::add_Columns(int add_After)
{
	while(add_After>=main_Table->columnCount())
	{
		main_Table->insertColumn(main_Table->columnCount());
	}
}

void Table_Of_Structures::span_Title_Rows()
{
	for(int i=0; i<rows_List_To_Span.size(); ++i)
	{
		main_Table->setSpan(rows_List_To_Span[i],0,1,main_Table->columnCount());
	}
}

void Table_Of_Structures::closeEvent(QCloseEvent* event)
{
	event;
	runned_Tables_Of_Structures->remove(table_Key);

	for(int i=0; i<list_Of_Trees.size(); ++i)
	{
		list_Of_Trees[i]->structure_Toolbar->toolbar->setDisabled(false);
	}
}
