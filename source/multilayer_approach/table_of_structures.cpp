// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "table_of_structures.h"
#include "algorithm"

Table_Of_Structures::Table_Of_Structures(QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures, QTabWidget* multilayer_Tabs, QWidget *parent) :
	runned_Tables_Of_Structures(runned_Tables_Of_Structures),
	multilayer_Tabs(multilayer_Tabs),
	QWidget(parent)
{
	setWindowTitle("Table Of Structures");
	create_Main_Layout();

	setAttribute(Qt::WA_DeleteOnClose);
}

void Table_Of_Structures::closeEvent(QCloseEvent* event)
{
	runned_Tables_Of_Structures->remove(table_Key);

	Multilayer_Approach* multilayer_Approach = qobject_cast<Multilayer_Approach*>(multilayer_Tabs->parent());
	multilayer_Approach->menu->setDisabled(false);

	multilayer_Tabs->cornerWidget()->setDisabled(false);
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		list_Of_Trees[i]->structure_Toolbar->toolbar->setDisabled(false);
		list_Of_Trees[i]->tree->blockSignals(false);
		multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setEnabled(true);
	}
	event->accept();
}

void Table_Of_Structures::emit_Data_Edited()
{
	emit data_Edited();
}

void Table_Of_Structures::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
	main_Layout->setContentsMargins(0,0,0,0);
	create_Menu();
	create_Tabs();
	main_Layout->addWidget(main_Tabs);
	//	resize(800,550);
	resize(920,750);
	add_Tabs();

	revise_All_Parameters();
}

void Table_Of_Structures::create_Menu()
{
	Menu* menu = new Menu(Window_Type::Table(),this);
	main_Layout->setMenuBar(menu->menu_Bar);
	connect(menu, SIGNAL(refresh()), this, SLOT(reload_All_Widgets()));
	connect(menu, SIGNAL(refresh()), this, SLOT(emit_Data_Edited()));
}

void Table_Of_Structures::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(true);
}

void Table_Of_Structures::add_Tabs()
{
	read_Trees();
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		My_Table_Widget* new_Table = new My_Table_Widget(basic_Row_Number, basic_Column_Number, coupled_Widgets_Map, main_Tabs, main_Tabs);
		list_Of_Tables.append(new_Table);

		create_Table(new_Table, tab_Index);
		main_Tabs->addTab(new_Table, multilayer_Tabs->tabText(tab_Index));
	}
	table_Is_Created = true;
	reload_All_Widgets();
	reload_All_Widgets();
}

void Table_Of_Structures::create_Table(My_Table_Widget* new_Table, int tab_Index)
{
	// PARAMETER
	rows_List_To_Span.clear();
	{
		// header for each structure
		new_Table->insertRow(new_Table->rowCount());
		int current_Row = new_Table->rowCount()-1;
		rows_List_To_Span.append(current_Row);
		new_Table->setItem(current_Row,0, new QTableWidgetItem(multilayer_Tabs->tabText(tab_Index)));
		new_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);
		new_Table->item   (current_Row,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

		// structure display
		int max_Depth = Global_Variables::get_Tree_Depth(list_Of_Trees[tab_Index]->tree->invisibleRootItem());
		int depth, current_Column;

		// calculate max_Number_Of_Elements for tabulation
		int max_Number_Of_Elements=1;
		{
			QTreeWidgetItem* structure_Item;
			QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);
			while (*it)
			{
				structure_Item=*it;
				QList<Stoichiometry> composition;
				QStringList whats_This_List = structure_Item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);
				item_Type_String = whats_This_List[0];
				if(item_Type_String == whats_This_Ambient)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().composition;	}
				if(item_Type_String == whats_This_Layer)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().composition;	}
				if(item_Type_String == whats_This_Substrate)	{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composition;}

				if(max_Number_Of_Elements<composition.size()) max_Number_Of_Elements=composition.size();
				++it;
			}
		}

		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);
		while (*it)
		{
			structure_Item = *it;

			// common variables
			bool composed_Material = false;

			// load data
			QStringList whats_This_List = structure_Item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);
			item_Type_String = whats_This_List[0];
			if(item_Type_String == whats_This_Ambient)		{composed_Material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().composed_Material;	}
			if(item_Type_String == whats_This_Layer)		{composed_Material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().composed_Material;	}
			if(item_Type_String == whats_This_Multilayer)	{		}
			if(item_Type_String == whats_This_Substrate)	{composed_Material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composed_Material;}

			depth = Global_Variables::get_Item_Depth(structure_Item);

			current_Column = depth-1;
			new_Table->insertRow(new_Table->rowCount());
			new_Table->insertRow(new_Table->rowCount());
			new_Table->insertRow(new_Table->rowCount());

			current_Row = new_Table->rowCount()-2;

			if(item_Type_String != whats_This_Multilayer)
			{
				new_Table->insertRow(new_Table->rowCount());
				new_Table->insertRow(new_Table->rowCount());
				new_Table->insertRow(new_Table->rowCount());
			}


			add_Columns(new_Table,depth);

			// print whatsThis
			new_Table->setItem(current_Row,current_Column, new QTableWidgetItem(structure_Item->whatsThis(DEFAULT_COLUMN)));
			new_Table->item(current_Row,current_Column)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

			current_Column = max_Depth+1;

			///--------------------------------------------------------------------------------------------
			/// creation
			///--------------------------------------------------------------------------------------------

			// material
			if(item_Type_String == whats_This_Ambient || item_Type_String == whats_This_Layer || item_Type_String == whats_This_Substrate)
			{
				if(composed_Material)
				{
					create_Combo_Elements(new_Table, current_Row,   current_Column, structure_Item, item_Type_String);
					create_Stoich		 (new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, VAL);
					create_Stoich		 (new_Table, current_Row+3, current_Column, structure_Item, item_Type_String, MIN);
					create_Stoich		 (new_Table, current_Row+4, current_Column, structure_Item, item_Type_String, MAX);

					// it should be created last
					create_Stoich_Check_Box_Fit (new_Table, current_Row+2, current_Column, structure_Item, item_Type_String, tab_Index, 1, 2, 0, 0);
				}
				else
				{
					create_Material      (new_Table, current_Row,   current_Column, structure_Item, item_Type_String);
					create_Browse_Button (new_Table, current_Row+1, current_Column);
				}
			}
			current_Column += (max_Number_Of_Elements+1);
			///--------------------------------------------------------------------------------------------

			// multilayer
			if(item_Type_String == whats_This_Multilayer)
			{
				// num repetitions
				QString whats_This = whats_This_Num_Repetitions;
				add_Columns		(new_Table, current_Column+5);
				create_Label	(new_Table, current_Row,   current_Column, structure_Item, item_Type_String, tab_Index, whats_This, "N");
				create_Line_Edit(new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				current_Column += 2;

				// period
				whats_This = whats_This_Period;
				add_Columns		(new_Table, current_Column+5);
				create_Label	(new_Table, current_Row,   current_Column,   structure_Item, item_Type_String, tab_Index, whats_This, "d ["+length_units+"]");
				create_Line_Edit(new_Table, current_Row+1, current_Column,   structure_Item, item_Type_String, whats_This, VAL);
				create_Line_Edit(new_Table, current_Row+1, current_Column+2, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit(new_Table, current_Row+1, current_Column+3, structure_Item, item_Type_String, whats_This, MAX);
				// lastcolumn
				create_Check_Box_Fit(new_Table, current_Row+1, current_Column+1, structure_Item, item_Type_String, tab_Index, whats_This, 0, 0, 1, 2);
				current_Column += 5;

				// gamma
				if(structure_Item->childCount()==2)
				{
					whats_This = whats_This_Gamma;
					add_Columns		(new_Table, current_Column+5);
					create_Label	(new_Table, current_Row,   current_Column,   structure_Item, item_Type_String, tab_Index, whats_This, Gamma_Sym);
					create_Line_Edit(new_Table, current_Row+1, current_Column,   structure_Item, item_Type_String, whats_This, VAL);
					create_Line_Edit(new_Table, current_Row+1, current_Column+2, structure_Item, item_Type_String, whats_This, MIN);
					create_Line_Edit(new_Table, current_Row+1, current_Column+3, structure_Item, item_Type_String, whats_This, MAX);
					// last
					create_Check_Box_Fit(new_Table, current_Row+1, current_Column+1, structure_Item, item_Type_String, tab_Index, whats_This, 0, 0, 1, 2);
					current_Column += 5;
				}
			}
			///--------------------------------------------------------------------------------------------

			// density
			if(item_Type_String == whats_This_Ambient || item_Type_String == whats_This_Layer || item_Type_String == whats_This_Substrate)
			{
				QString whats_This;
				add_Columns(new_Table, current_Column+1);
				if(composed_Material)
				{
					whats_This = whats_This_Absolute_Density;
					create_Label(new_Table, current_Row, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, Rho_Sym+" ["+density_units+"]");
				} else
				{
					whats_This = whats_This_Relative_Density;
					create_Label(new_Table, current_Row, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, Rho_Sym+" [r.u.]");
				}
				create_Line_Edit(new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				create_Line_Edit(new_Table, current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit(new_Table, current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, 1, 2, 0, 0);
			}
			current_Column += 2;
			///--------------------------------------------------------------------------------------------

			// thickness
			if(item_Type_String == whats_This_Layer)
			{
				QString whats_This = whats_This_Thickness;
				add_Columns		(new_Table, current_Column+1);
				create_Label	(new_Table, current_Row,   current_Column, structure_Item, item_Type_String, tab_Index, whats_This, "z ["+length_units+"]");
				create_Line_Edit(new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				create_Line_Edit(new_Table, current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit(new_Table, current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, 1, 2, 0, 0);
			}
			current_Column += 2;
			///--------------------------------------------------------------------------------------------

			// common sigma
			if(item_Type_String == whats_This_Layer || item_Type_String == whats_This_Substrate)
			{
				QString whats_This = whats_This_Sigma;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit	(new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				// second
				create_Check_Box_Label(new_Table, current_Row, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, Sigma_Sym+" ["+length_units+"]", 1, 0, 0, 0);
				create_Line_Edit	(new_Table, current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(new_Table, current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, 1, 2, 0, 0);
			}
			current_Column += 2;
			///--------------------------------------------------------------------------------------------

			// interlayers: weights and sigmas
			if(item_Type_String == whats_This_Layer || item_Type_String == whats_This_Substrate)
			{
				add_Columns(new_Table, current_Column+transition_Layer_Functions_Size);

				// weights
				create_Weigts_Interlayer(new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, VAL);
				create_Check_Box_Label_Interlayer(new_Table, current_Row, current_Column, structure_Item, item_Type_String, tab_Index);
				create_Weights_Check_Box_Fit_Interlayer(new_Table, current_Row+2, current_Column, structure_Item, item_Type_String, tab_Index);

				// sigmas
				create_MySigma_Labels_Interlayer(new_Table, current_Row+3, current_Column, structure_Item, item_Type_String, tab_Index);
				create_MySigma_Interlayer		(new_Table, current_Row+4, current_Column, structure_Item, item_Type_String);
			}
			current_Column += (1+transition_Layer_Functions_Size);
			///--------------------------------------------------------------------------------------------



			// thickness linear drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				QString whats_This = whats_This_Thickness_Drift_Line_Value;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit(new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				// second
				create_Check_Box_Label(new_Table, current_Row, current_Column, structure_Item, item_Type_String, tab_Index, whats_This,"dz lin", 1, 1, 0, 0);
				create_Line_Edit	(new_Table, current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(new_Table, current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// thickness random drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				QString whats_This = whats_This_Thickness_Drift_Rand_Rms;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit(new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				// second
				create_Check_Box_Label(new_Table, current_Row, current_Column, structure_Item, item_Type_String, tab_Index, whats_This,"dz ran", 1, 1, 0, 0);
				create_Line_Edit	(new_Table, current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(new_Table, current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// thickness sine drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				add_Columns(new_Table, current_Column+3);

				create_Line_Edit	(new_Table, current_Row+1, current_Column,   structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Amplitude, VAL);
				create_Line_Edit	(new_Table, current_Row+1, current_Column+1, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Frequency, VAL);
				create_Line_Edit	(new_Table, current_Row+1, current_Column+2, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Phase, VAL);

				// second
				create_Check_Box_Label(new_Table, current_Row, current_Column,   structure_Item, item_Type_String, tab_Index, whats_This_Thickness_Drift_Sine,"dz sin : A*sin(2"+Pi_Sym+"n"+Nu_Sym+"+"+Phi_Sym+")", 1, 1, 0, 2);

				create_Line_Edit	(new_Table, current_Row+3, current_Column,   structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Amplitude, MIN);
				create_Line_Edit	(new_Table, current_Row+3, current_Column+1, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Frequency, MIN);
				create_Line_Edit	(new_Table, current_Row+3, current_Column+2, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Phase, MIN);

				create_Line_Edit	(new_Table, current_Row+4, current_Column,	 structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Amplitude, MAX);
				create_Line_Edit	(new_Table, current_Row+4, current_Column+1, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Frequency, MAX);
				create_Line_Edit	(new_Table, current_Row+4, current_Column+2, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Phase, MAX);

				// last
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column,	 structure_Item, item_Type_String, tab_Index, whats_This_Thickness_Drift_Sine_Amplitude, 1, 2, 0, 0);
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column+1, structure_Item, item_Type_String, tab_Index, whats_This_Thickness_Drift_Sine_Frequency, 1, 2, 0, 0);
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column+2, structure_Item, item_Type_String, tab_Index, whats_This_Thickness_Drift_Sine_Phase, 1, 2, 0, 0);

				new_Table->setSpan(current_Row,current_Column,1,3);
			}
			current_Column += 4;
			///--------------------------------------------------------------------------------------------



			// sigma linear drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				QString whats_This = whats_This_Sigma_Drift_Line_Value;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit(new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				// second
				create_Check_Box_Label(new_Table, current_Row, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, "d"+Sigma_Sym+" lin", 1, 1, 0, 0);
				create_Line_Edit	(new_Table, current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(new_Table, current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// sigma random drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				QString whats_This = whats_This_Sigma_Drift_Rand_Rms;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit(new_Table, current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				// second
				create_Check_Box_Label(new_Table, current_Row, current_Column, structure_Item, item_Type_String, tab_Index, whats_This,"d"+Sigma_Sym+" ran", 1, 1, 0, 0);
				create_Line_Edit	(new_Table, current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(new_Table, current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column, structure_Item, item_Type_String, tab_Index, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// sigma sine drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				add_Columns(new_Table, current_Column+3);

				create_Line_Edit	(new_Table, current_Row+1, current_Column,   structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Amplitude, VAL);
				create_Line_Edit	(new_Table, current_Row+1, current_Column+1, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Frequency, VAL);
				create_Line_Edit	(new_Table, current_Row+1, current_Column+2, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Phase, VAL);

				// second
				create_Check_Box_Label(new_Table, current_Row, current_Column,   structure_Item, item_Type_String, tab_Index, whats_This_Sigma_Drift_Sine,"d"+Sigma_Sym+" sin : A*sin(2"+Pi_Sym+"n"+Nu_Sym+"+"+Phi_Sym+")", 1, 1, 0, 2);

				create_Line_Edit	(new_Table, current_Row+3, current_Column,   structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Amplitude, MIN);
				create_Line_Edit	(new_Table, current_Row+3, current_Column+1, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Frequency, MIN);
				create_Line_Edit	(new_Table, current_Row+3, current_Column+2, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Phase, MIN);

				create_Line_Edit	(new_Table, current_Row+4, current_Column,	 structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Amplitude, MAX);
				create_Line_Edit	(new_Table, current_Row+4, current_Column+1, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Frequency, MAX);
				create_Line_Edit	(new_Table, current_Row+4, current_Column+2, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Phase, MAX);

				// last
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column,	 structure_Item, item_Type_String, tab_Index, whats_This_Sigma_Drift_Sine_Amplitude, 1, 2, 0, 0);
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column+1, structure_Item, item_Type_String, tab_Index, whats_This_Sigma_Drift_Sine_Frequency, 1, 2, 0, 0);
				create_Check_Box_Fit(new_Table, current_Row+2, current_Column+2, structure_Item, item_Type_String, tab_Index, whats_This_Sigma_Drift_Sine_Phase, 1, 2, 0, 0);

				new_Table->setSpan(current_Row,current_Column,1,3);
			}
			current_Column += 4;
			///--------------------------------------------------------------------------------------------

			++it;
		}
		new_Table->insertRow(new_Table->rowCount());
	}
	span_Structure_Headers(new_Table);
	span_Structure_Items(new_Table);
	new_Table->resizeColumnsToContents();
	new_Table->resizeRowsToContents();

	// fit initial size of columns
	for(int i=0; i<new_Table->columnCount(); ++i)
		fit_Column(new_Table, new_Table->columnWidth(i), i);
}

void Table_Of_Structures::read_Trees()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Structure_Tree* old_Structure_Tree = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i))->structure_Tree;
		list_Of_Trees.append(old_Structure_Tree);

		// closing editors
		QList<Item_Editor*> local_List_Editors = old_Structure_Tree->list_Editors;
		for(int editor_Index=0; editor_Index<local_List_Editors.size(); ++editor_Index)
			local_List_Editors[editor_Index]->close();

		// disabling main interface
		old_Structure_Tree->structure_Toolbar->toolbar->setDisabled(true);
		old_Structure_Tree->tree->blockSignals(true);
		multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(true);
	}
	multilayer_Tabs->cornerWidget()->setDisabled(true);

	Multilayer_Approach* multilayer_Approach = qobject_cast<Multilayer_Approach*>(multilayer_Tabs->parent());
	multilayer_Approach->menu->setDisabled(true);
}

void Table_Of_Structures::revise_All_Parameters()
{
	for(int tab_Index=0; tab_Index<main_Tabs->count(); ++tab_Index)
	{
		My_Table_Widget* table = qobject_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));

		// search over sheet
		for(int row=0; row<table->rowCount(); row++)
		{
			for(int column=0; column<table->columnCount(); column++)
			{
				QWidget* widget = table->cellWidget(row,column);
				if(widget && coupled_Widgets_Map.contains(widget))
				{
					Parameter parameter = widget->property(parameter_Property).value<Parameter>();

					Parameter_Indicator empty_Indicator;

					// checks anyway, if it marked as "exist" or not

					// check master
					if(loaded_Parameters.indexOf(parameter.coupled.master.id) == -1)
						parameter.coupled.master = empty_Indicator;

					// check slaves
					for(int slave_Index=parameter.coupled.slaves.size()-1; slave_Index>=0; slave_Index--)
					{
						if(loaded_Parameters.indexOf(parameter.coupled.slaves[slave_Index].id) == -1)
							parameter.coupled.slaves.remove(slave_Index);
					}

					refresh_Reload_Core(refresh_Property, widget, parameter, coupled_Widgets_Map);

					if(loaded_Parameters.indexOf(parameter.indicator.id) != -1)
					{
						QLineEdit* param_Line_Edit = coupled_Id_LineEdits_Map.value(parameter.indicator.id);
						if(param_Line_Edit)
						{
//							if()
							param_Line_Edit->setStyleSheet("QLineEdit { background: rgb(0, 255, 255); selection-background-color: rgb(233, 99, 0); }");
						}
					}
				}
			}
		}
	}
}

void Table_Of_Structures::refresh_Reload_Core(QString refresh_Reload, QWidget *widget, Parameter& parameter, QMap<QWidget*,QTreeWidgetItem*>& coup_Widgets_Map)
{
	QVariant var;

	// get access to tree item
	parameter = widget->property(parameter_Property).value<Parameter>();
	QTreeWidgetItem* structure_Item = coup_Widgets_Map.value(widget);
	QStringList item_Type_List = structure_Item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);
	QString item_Type_String = item_Type_List[0];
	QString whats_This = parameter.indicator.whats_This;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = data.value<Ambient>();
		if(whats_This == whats_This_Composition)
		{
			for(int i=0; i<ambient.composition.size(); ++i)
			if(ambient.composition[i].composition.indicator.id == parameter.indicator.id)
			{
//				qInfo() << "FOUND " << whats_This << " " << i;
				if(refresh_Reload == reload_Property)	parameter.coupled = ambient.composition[i].composition.coupled;
				if(refresh_Reload == refresh_Property)	ambient.composition[i].composition.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Absolute_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = ambient.absolute_Density.coupled;
			if(refresh_Reload == refresh_Property)	ambient.absolute_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = ambient.relative_Density.coupled;
			if(refresh_Reload == refresh_Property)	ambient.relative_Density.coupled = parameter.coupled;
		}
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = data.value<Layer>();
		if(whats_This == whats_This_Composition)
		{
			for(int i=0; i<layer.composition.size(); ++i)
			if(layer.composition[i].composition.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = layer.composition[i].composition.coupled;
				if(refresh_Reload == refresh_Property)	layer.composition[i].composition.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Interlayer_Composition)
		{
			for(int i=0; i<layer.interlayer_Composition.size(); ++i)
			if(layer.interlayer_Composition[i].interlayer.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = layer.interlayer_Composition[i].interlayer.coupled;
				if(refresh_Reload == refresh_Property)	layer.interlayer_Composition[i].interlayer.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Interlayer_My_Sigma)
		{
			for(int i=0; i<layer.interlayer_Composition.size(); ++i)
			if(layer.interlayer_Composition[i].my_Sigma.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = layer.interlayer_Composition[i].my_Sigma.coupled;
				if(refresh_Reload == refresh_Property)	layer.interlayer_Composition[i].my_Sigma.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Absolute_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.absolute_Density.coupled;
			if(refresh_Reload == refresh_Property)	layer.absolute_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.relative_Density.coupled;
			if(refresh_Reload == refresh_Property)	layer.relative_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma.coupled = parameter.coupled;
		}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Line_Value.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Line_Value.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Rand_Rms.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Rand_Rms.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Sine_Amplitude.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Sine_Amplitude.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Sine_Frequency.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Sine_Frequency.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Sine_Phase.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Sine_Phase.coupled = parameter.coupled;
		}

		if(whats_This == whats_This_Sigma_Drift_Line_Value)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Line_Value.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Line_Value.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Rand_Rms.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Rand_Rms.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Sine_Amplitude.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Sine_Amplitude.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Sine_Frequency.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Sine_Frequency.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Sine_Phase.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Sine_Phase.coupled = parameter.coupled;
		}
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = data.value<Substrate>();
		if(whats_This == whats_This_Composition)
		{
			for(int i=0; i<substrate.composition.size(); ++i)
			if(substrate.composition[i].composition.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = substrate.composition[i].composition.coupled;
				if(refresh_Reload == refresh_Property)	substrate.composition[i].composition.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Interlayer_Composition)
		{
			for(int i=0; i<substrate.interlayer_Composition.size(); ++i)
			if(substrate.interlayer_Composition[i].interlayer.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = substrate.interlayer_Composition[i].interlayer.coupled;
				if(refresh_Reload == refresh_Property)	substrate.interlayer_Composition[i].interlayer.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Interlayer_My_Sigma)
		{
			for(int i=0; i<substrate.interlayer_Composition.size(); ++i)
			if(substrate.interlayer_Composition[i].my_Sigma.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = substrate.interlayer_Composition[i].my_Sigma.coupled;
				if(refresh_Reload == refresh_Property)	substrate.interlayer_Composition[i].my_Sigma.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Absolute_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = substrate.absolute_Density.coupled;
			if(refresh_Reload == refresh_Property)	substrate.absolute_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = substrate.relative_Density.coupled;
			if(refresh_Reload == refresh_Property)	substrate.relative_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = substrate.sigma.coupled;
			if(refresh_Reload == refresh_Property)	substrate.sigma.coupled = parameter.coupled;
		}
		var.setValue( substrate );
	}
	if(item_Type_String == whats_This_Multilayer)
	{
		Stack_Content stack_Content = data.value<Stack_Content>();
		if(whats_This == whats_This_Period)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = stack_Content.period.coupled;
			if(refresh_Reload == refresh_Property)	stack_Content.period.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Gamma)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = stack_Content.gamma.coupled;
			if(refresh_Reload == refresh_Property)	stack_Content.gamma.coupled = parameter.coupled;
		}
		var.setValue( stack_Content );
	}

	// refreshing
	if(refresh_Reload == refresh_Property)
	{
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		var.setValue(parameter);
		widget->setProperty(parameter_Property, var);
	}
}

void Table_Of_Structures::add_Columns(My_Table_Widget* table, int add_After)
{
	while(add_After>=table->columnCount())
	{
		table->insertColumn(table->columnCount());
	}
}

void Table_Of_Structures::get_Parameter(Parameter& parameter, QTreeWidgetItem* structure_Item, QString whats_This)
{
	// PARAMETER
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)
	{
		if(whats_This == whats_This_Absolute_Density)				{parameter = data.value<Ambient>().absolute_Density;}
		if(whats_This == whats_This_Relative_Density)				{parameter = data.value<Ambient>().relative_Density;}
	}
	if(item_Type_String == whats_This_Layer)
	{
		if(whats_This == whats_This_Absolute_Density)				{parameter = data.value<Layer>().absolute_Density;}
		if(whats_This == whats_This_Relative_Density)				{parameter = data.value<Layer>().relative_Density;}
		if(whats_This == whats_This_Thickness)						{parameter = data.value<Layer>().thickness;		}
		if(whats_This == whats_This_Sigma)							{parameter = data.value<Layer>().sigma;					}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)		{parameter = data.value<Layer>().thickness_Drift.drift_Line_Value;	}
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		{parameter = data.value<Layer>().thickness_Drift.drift_Rand_Rms;	}
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	{parameter = data.value<Layer>().thickness_Drift.drift_Sine_Amplitude;}
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	{parameter = data.value<Layer>().thickness_Drift.drift_Sine_Frequency;}
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		{parameter = data.value<Layer>().thickness_Drift.drift_Sine_Phase;	}

		if(whats_This == whats_This_Sigma_Drift_Line_Value)			{parameter = data.value<Layer>().sigma_Drift.drift_Line_Value;	}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			{parameter = data.value<Layer>().sigma_Drift.drift_Rand_Rms;	}
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		{parameter = data.value<Layer>().sigma_Drift.drift_Sine_Amplitude;	}
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		{parameter = data.value<Layer>().sigma_Drift.drift_Sine_Frequency;	}
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			{parameter = data.value<Layer>().sigma_Drift.drift_Sine_Phase;	}
	}
	if(item_Type_String == whats_This_Substrate)
	{
		if(whats_This == whats_This_Absolute_Density)	{parameter = data.value<Substrate>().absolute_Density;	}
		if(whats_This == whats_This_Relative_Density)	{parameter = data.value<Substrate>().relative_Density;	}
		if(whats_This == whats_This_Sigma)				{parameter = data.value<Substrate>().sigma;				}
	}
	if(item_Type_String == whats_This_Multilayer)
	{
		if(whats_This == whats_This_Period)				{parameter = data.value<Stack_Content>().period;	}
		if(whats_This == whats_This_Gamma)				{parameter = data.value<Stack_Content>().gamma;		}
	}
}

void Table_Of_Structures::span_Structure_Headers(My_Table_Widget* table)
{
	for(int struct_Index=0; struct_Index<rows_List_To_Span.size(); ++struct_Index)
	{
		table->setSpan(rows_List_To_Span[struct_Index],0,1,table->columnCount());
	}
}

void Table_Of_Structures::span_Structure_Items(My_Table_Widget* table)
{
	for(int row_Index=0; row_Index<table->rowCount(); ++row_Index)
	{
		for(int col_Index=0; col_Index<table->columnCount(); ++col_Index)
		{
			if(table->item(row_Index,col_Index))
			{
				QStringList wtf_List = table->item(row_Index,col_Index)->text().split(item_Type_Delimiter,QString::SkipEmptyParts);
				if(	   wtf_List[0] == whats_This_Ambient
					   || wtf_List[0] == whats_This_Layer
					   || wtf_List[0] == whats_This_Substrate )
				{
					table->setSpan(row_Index,col_Index,5,1);
					//					if(wtf_List[0] != whats_This_Ambient) colorize_Row(row_Index-1);
				}
				if( wtf_List[0] == whats_This_Multilayer )
				{
					table->setSpan(row_Index,col_Index,2,1);
					//					colorize_Row(row_Index-1);
				}
			}
		}
	}
}

void Table_Of_Structures::fit_Column(My_Table_Widget* table, int start_Width, int current_Column)
{
	int max_Width=start_Width;
	for(int row=0; row<table->rowCount(); ++row)
	{
		QLineEdit* current_Line_Edit = qobject_cast<QLineEdit*>(table->cellWidget(row, current_Column));
		if(current_Line_Edit)
		{
			if(max_Width<current_Line_Edit->width())
			{
				max_Width=current_Line_Edit->width()+1;
			}
		}
	}
	table->setColumnWidth(current_Column,max_Width);
}

// creation

// for material only
void Table_Of_Structures::create_Combo_Elements(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QList<Stoichiometry> composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)		{composition = data.value<Ambient>().  composition;}
	if(item_Type_String == whats_This_Layer)		{composition = data.value<Layer>().	   composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = data.value<Substrate>().composition;}

	int current_Column = start_Column;
	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		add_Columns(table, current_Column+1);

		// create combobox
		QComboBox* elements = new QComboBox;
		elements->addItems(sorted_Elements.keys());
		elements->setCurrentIndex(elements->findText(composition[composition_Index].type));

		// number of element is here
		elements->setProperty(num_Chemic_Element_Property, composition_Index);
		elements->setProperty(item_Type_Property, item_Type_String);

		// for reloading
		elements->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(elements);

		// create item
		table->setCellWidget(current_Row, current_Column, elements);
		elements_Map.insert(elements, structure_Item);
		connect(elements, &QComboBox::currentTextChanged, this, [=]{refresh_Element(); });
		//      connect(elements,  SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Element(QString))); old version

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_Stoich(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type)
{
	QList<Stoichiometry> composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)		{composition = data.value<Ambient>().  composition;}
	if(item_Type_String == whats_This_Layer)		{composition = data.value<Layer>().	   composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = data.value<Substrate>().composition;}

	int current_Column = start_Column;
	double value = -2017;
	char format = line_edit_short_double_format;

	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		if(val_Type == VAL)	{value = composition[composition_Index].composition.value;		format = line_edit_short_double_format;}
		if(val_Type == MIN)	{value = composition[composition_Index].composition.fit.min;	format = line_edit_short_double_format;}
		if(val_Type == MAX)	{value = composition[composition_Index].composition.fit.max;	format = line_edit_short_double_format;}

		QString text_Value = QString::number(value, format, line_edit_composition_precision);

		// create lineedit
		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

		// number of element is here
		line_Edit->setProperty(num_Chemic_Element_Property, composition_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(item_Type_Property, item_Type_String);
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setProperty(value_Type_Property, val_Type);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		// for reloading
		line_Edit->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(line_Edit);

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit(table); });

		// create item (set LineEdits_Map)
		table->setCellWidget(current_Row, current_Column, line_Edit);
		line_Edits_Map.insert(line_Edit, structure_Item);
		coupled_Id_LineEdits_Map.insert(composition[composition_Index].composition.indicator.id,line_Edit);

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{refresh_Stoich(); });

//        line_Edit->textEdited(line_Edit->text());
		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

QString Table_Of_Structures::material_From_Composition(QList<Stoichiometry>& composition)
{
	QString text;
	for(int i=0; i<composition.size(); ++i)
	{
		text += composition[i].type;
		if( abs(composition[i].composition.value - 1.) > DBL_EPSILON )
			text += QString::number(composition[i].composition.value, line_edit_short_double_format, thumbnail_composition_precision);
	}
	return text;
}

void Table_Of_Structures::create_Stoich_Check_Box_Fit(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int tab_Index, int r_S, int r_F, int c_S, int c_F)
{
	QList<Stoichiometry> composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)		{composition = data.value<Ambient>().  composition;}
	if(item_Type_String == whats_This_Layer)		{composition = data.value<Layer>().	   composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = data.value<Substrate>().composition;}

	int current_Column = start_Column;

	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		// create check_Box
		QCheckBox* check_Box = new QCheckBox("fit");//;

		// alignment
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		// number of element is here
		check_Box->setProperty(num_Chemic_Element_Property, composition_Index);
		check_Box->setProperty(item_Type_Property, item_Type_String);
		check_Box->setProperty(row_Property, current_Row);
		check_Box->setProperty(column_Property, current_Column);

		// which cells disable and enable
		check_Box->setProperty(relative_Rows_To_Disable_Start_Property, r_S);
		check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, r_F);
		check_Box->setProperty(relative_Columns_To_Disable_Start_Property, c_S);
		check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, c_F);

		// for reloading
		check_Box->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(check_Box);

		// create item
		table->setCellWidget(current_Row, current_Column, back_Widget);
		check_Boxes_Map.insert(check_Box, structure_Item);
		connect(check_Box, &QCheckBox::toggled, this, [=]{refresh_Fit_Element(); });
		connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off(table); });
		check_Box->setChecked(composition[composition_Index].composition.fit.is_Fitable);

		{
			// set parameter id to BACK widget
			QVariant var;
			composition[composition_Index].composition.indicator.tab_Index = tab_Index;
			composition[composition_Index].composition.indicator.full_Name = structure_Item->whatsThis(DEFAULT_COLUMN) + " " + composition[composition_Index].type + " composition";
			var.setValue( composition[composition_Index].composition );
			back_Widget->setProperty(parameter_Property,var);
			back_Widget->setWhatsThis(structure_Item->whatsThis(DEFAULT_COLUMN));
			coupled_Widgets_Map.insert(back_Widget,structure_Item);
			loaded_Parameters.append(composition[composition_Index].composition.indicator.id);
		}

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;

		// disable if 1 element
		if(composition.size()==1)
		{
			check_Box->setProperty(relative_Rows_To_Disable_Start_Property, -1);
			check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, 2);
			check_Box->setProperty(relative_Columns_To_Disable_Start_Property, 0);
			check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, 0);

			check_Box->setChecked(false);
			//			check_Box->toggled(false);
		}
	}
}

void Table_Of_Structures::create_Material(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QString material = "bad";
	QString approved_Material = "very bad";
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)		{		  material = data.value<Ambient>().  material;
		approved_Material = data.value<Ambient>().  approved_Material;}
	if(item_Type_String == whats_This_Layer)		{		  material = data.value<Layer>().    material;
		approved_Material = data.value<Layer>().    approved_Material;}
	if(item_Type_String == whats_This_Substrate)	{		  material = data.value<Substrate>().material;
		approved_Material = data.value<Substrate>().approved_Material;}

	add_Columns(table, current_Column+1);

	QLineEdit* material_Line_Edit = new QLineEdit(material);
	material_Line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);
	material_Line_Edit->setProperty(min_Size_Property, material_Line_Edit->width());
	material_Line_Edit->setProperty(item_Type_Property, item_Type_String);
	material_Line_Edit->setProperty(column_Property, current_Column);

	// for reloading
	material_Line_Edit->setProperty(reload_Property, false);
	all_Widgets_To_Reload.append(material_Line_Edit);

	connect(material_Line_Edit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit(table); });

	table->setCellWidget(current_Row, current_Column, material_Line_Edit);
	line_Edits_Map.insert(material_Line_Edit, structure_Item);	
	connect(material_Line_Edit, &QLineEdit::textEdited, this, [=]{refresh_Material(); });
	connect(material_Line_Edit, SIGNAL(editingFinished()), this, SLOT(check_Material()));
	//	material_Line_Edit->textEdited(material_Line_Edit->text());
}

void Table_Of_Structures::create_Browse_Button(My_Table_Widget* table, int current_Row, int start_Column)
{
	// TODO
	QPushButton* browse_Button = new QPushButton("Browse");
	browse_Button->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);
	table->setCellWidget(current_Row, start_Column, browse_Button);
	connect(browse_Button, &QPushButton::clicked, this, [=]{browse_Material();});
	// connect(browse_Button, SIGNAL(clicked(bool)), this, SLOT(browse_Material(bool))); old version
}

// for several parameters
void Table_Of_Structures::create_Label(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int tab_Index, QString whats_This, QString text)
{
	QLabel* label = new QLabel(text);
	label->setStyleSheet("background-color: lightblue");
	label->setAlignment(Qt::AlignCenter);
	table->setCellWidget(current_Row, current_Column, label);

	// set parameter id to widget
	Parameter parameter; parameter.indicator.id=0;
	get_Parameter(parameter, structure_Item, whats_This);
	if(parameter.indicator.id!=0)
	{
		QVariant var;
		parameter.indicator.tab_Index = tab_Index;
		parameter.indicator.full_Name = structure_Item->whatsThis(DEFAULT_COLUMN) + " " + whats_This;
		var.setValue( parameter );
		label->setProperty(parameter_Property,var);
		label->setWhatsThis(structure_Item->whatsThis(DEFAULT_COLUMN));
		coupled_Widgets_Map.insert(label,structure_Item);
		loaded_Parameters.append(parameter.indicator.id);
	}

	// for reloading
	label->setProperty(reload_Property, false);
	label->setProperty(item_Type_Property, item_Type_String);
	label->setProperty(whats_This_Property, whats_This);
	all_Widgets_To_Reload.append(label);

	connect(label, &QLabel::windowTitleChanged, this, [=]{refresh_Header(); });
}

void Table_Of_Structures::create_Check_Box_Label(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int tab_Index, QString whats_This, QString text, int r_S, int r_F, int c_S, int c_F)
{
	// PARAMETER
	bool bool_Check = false;

	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)
	{
		if(whats_This == whats_This_Thickness_Drift_Line_Value)	bool_Check = data.value<Layer>().thickness_Drift.is_Drift_Line;
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)	bool_Check = data.value<Layer>().thickness_Drift.is_Drift_Rand;
		if(whats_This == whats_This_Thickness_Drift_Sine)		bool_Check = data.value<Layer>().thickness_Drift.is_Drift_Sine;

		if(whats_This == whats_This_Sigma_Drift_Line_Value)		bool_Check = data.value<Layer>().sigma_Drift.is_Drift_Line;
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)		bool_Check = data.value<Layer>().sigma_Drift.is_Drift_Rand;
		if(whats_This == whats_This_Sigma_Drift_Sine)			bool_Check = data.value<Layer>().sigma_Drift.is_Drift_Sine;

		if(whats_This == whats_This_Sigma)						bool_Check = data.value<Layer>().common_Sigma;
	}
	if(item_Type_String == whats_This_Substrate)
	{
		if(whats_This == whats_This_Sigma)						bool_Check = data.value<Substrate>().common_Sigma;
	}

	QCheckBox* check_Box = new QCheckBox(text);

	// enable/disable function
	check_Box->setProperty(row_Property, current_Row);
	check_Box->setProperty(column_Property, current_Column);
	check_Box->setProperty(relative_Rows_To_Disable_Start_Property, r_S);
	check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, r_F);
	check_Box->setProperty(relative_Columns_To_Disable_Start_Property, c_S);
	check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, c_F);

	check_Box->setProperty(item_Type_Property, item_Type_String);
	check_Box->setProperty(whats_This_Property, whats_This);
	check_Boxes_Map.insert(check_Box, structure_Item);

	// for reloading
	check_Box->setProperty(reload_Property, false);
	all_Widgets_To_Reload.append(check_Box);

	check_Box->setChecked(bool_Check);
	connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off(table); });
	//  connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(cells_On_Off(bool))); old version

	connect(check_Box, &QCheckBox::toggled, this, [=]{refresh_Check_Box_Header(); });
	//	connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(refresh_Check_Box_Header(bool)));  old version
	//    check_Box->toggled(false);

	// alignment
	QWidget* back_Widget = new QWidget;
	QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
	back_Layout->addWidget(check_Box);
	back_Layout->setSpacing(0);
	back_Layout->setContentsMargins(0,0,0,0);
	back_Layout->setAlignment(Qt::AlignCenter);

	// set parameter id to BACK widget
	Parameter parameter; parameter.indicator.id=0;
	get_Parameter(parameter, structure_Item, whats_This);
	if(parameter.indicator.id!=0)
	{
		QVariant var;
		parameter.indicator.tab_Index = tab_Index;
		parameter.indicator.full_Name = structure_Item->whatsThis(DEFAULT_COLUMN) + " " + whats_This;
		var.setValue( parameter );
		back_Widget->setProperty(parameter_Property,var);
		back_Widget->setWhatsThis(structure_Item->whatsThis(DEFAULT_COLUMN));
		coupled_Widgets_Map.insert(back_Widget,structure_Item);
		loaded_Parameters.append(parameter.indicator.id);
	}

	back_Widget->setStyleSheet("background-color: lightblue");
	table->setCellWidget(current_Row, current_Column, back_Widget);
}

// for all parameters
void Table_Of_Structures::create_Line_Edit(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString val_Type)
{
	// PARAMETER
	Parameter parameter;
	int precision = 4;
	char format = line_edit_short_double_format;
	double value = -2017;

	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)
	{
		if(whats_This == whats_This_Absolute_Density)	{parameter = data.value<Ambient>().absolute_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Relative_Density)	{parameter = data.value<Ambient>().relative_Density;		precision = line_edit_density_precision;}
	}
	if(item_Type_String == whats_This_Layer)
	{
		if(whats_This == whats_This_Absolute_Density)				{parameter = data.value<Layer>().absolute_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Relative_Density)				{parameter = data.value<Layer>().relative_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Thickness)						{parameter = data.value<Layer>().thickness;				precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Sigma)							{parameter = data.value<Layer>().sigma;					precision = line_edit_sigma_precision;}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)		{parameter = data.value<Layer>().thickness_Drift.drift_Line_Value;			precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		{parameter = data.value<Layer>().thickness_Drift.drift_Rand_Rms;			precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	{parameter = data.value<Layer>().thickness_Drift.drift_Sine_Amplitude;		precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	{parameter = data.value<Layer>().thickness_Drift.drift_Sine_Frequency;		precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		{parameter = data.value<Layer>().thickness_Drift.drift_Sine_Phase;			precision = line_edit_thickness_precision;}

		if(whats_This == whats_This_Sigma_Drift_Line_Value)			{parameter = data.value<Layer>().sigma_Drift.drift_Line_Value;		precision = line_edit_sigma_precision;}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			{parameter = data.value<Layer>().sigma_Drift.drift_Rand_Rms;		precision = line_edit_sigma_precision;}
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		{parameter = data.value<Layer>().sigma_Drift.drift_Sine_Amplitude;	precision = line_edit_sigma_precision;}
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		{parameter = data.value<Layer>().sigma_Drift.drift_Sine_Frequency;	precision = line_edit_sigma_precision;}
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			{parameter = data.value<Layer>().sigma_Drift.drift_Sine_Phase;		precision = line_edit_sigma_precision;}
	}
	if(item_Type_String == whats_This_Substrate)
	{
		if(whats_This == whats_This_Absolute_Density)	{parameter = data.value<Substrate>().absolute_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Relative_Density)	{parameter = data.value<Substrate>().relative_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Sigma)				{parameter = data.value<Substrate>().sigma;					precision = line_edit_sigma_precision;	}
	}
	if(item_Type_String == whats_This_Multilayer)
	{
		if(whats_This == whats_This_Num_Repetitions)	{value = data.value<Stack_Content>().num_Repetition.value;}
		if(whats_This == whats_This_Period)				{parameter = data.value<Stack_Content>().period;	precision = line_edit_period_precision;}
		if(whats_This == whats_This_Gamma)				{parameter = data.value<Stack_Content>().gamma;		precision = line_edit_gamma_precision;}
	}

	if(val_Type == VAL)
	{
		if(whats_This != whats_This_Num_Repetitions)	value = parameter.value;
		format = line_edit_double_format;
	}
	if(val_Type == MIN)	{value = parameter.fit.min; format = line_edit_short_double_format;}
	if(val_Type == MAX)	{value = parameter.fit.max; format = line_edit_short_double_format;}

	QString text_Value;
	if(whats_This != whats_This_Num_Repetitions)	text_Value = QString::number(value, format, precision);
	else											text_Value = QString::number(value);

	// create lineedit
	QLineEdit* line_Edit = new QLineEdit(text_Value);
	line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

	line_Edit->setProperty(min_Size_Property, line_Edit->width());
	line_Edit->setProperty(item_Type_Property, item_Type_String);
	line_Edit->setProperty(column_Property, current_Column);
	line_Edit->setProperty(whats_This_Property, whats_This);
	line_Edit->setProperty(value_Type_Property, val_Type);

	if(whats_This == whats_This_Absolute_Density)				line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Relative_Density)				line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

	if(whats_This == whats_This_Thickness)                      line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

	if(whats_This == whats_This_Sigma)                          line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

	if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

	if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

	if(whats_This == whats_This_Num_Repetitions)				line_Edit->setValidator(new QIntValidator(0, MAX_INTEGER));
	if(whats_This == whats_This_Period)							line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Gamma)							line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

	//    // this lines should be reloaded after every editing
	//    if(whats_This == whats_This_Thickness ||
	//       whats_This == whats_This_Sigma     ||
	//       whats_This == whats_This_Period    ||
	//       whats_This == whats_This_Gamma     ||
	//       whats_This == whats_This_Num_Repetitions)     line_Edit->setProperty(master_Reload_Property, true);

	// for reloading
	line_Edit->setProperty(reload_Property, false);
	all_Widgets_To_Reload.append(line_Edit);

	connect(line_Edit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit(table); });

	// create item (set LineEdits_Map)
	table->setCellWidget(current_Row, current_Column, line_Edit);
	line_Edits_Map.insert(line_Edit, structure_Item);
	coupled_Id_LineEdits_Map.insert(parameter.indicator.id,line_Edit);

	connect(line_Edit, &QLineEdit::textEdited, this, [=]{refresh_Parameter(table); });
	connect(line_Edit, &QLineEdit::editingFinished, this, [=]{refresh_Parameter(table); });

//    line_Edit->textEdited(line_Edit->text());
}

void Table_Of_Structures::create_Check_Box_Fit(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int tab_Index, QString whats_This, int r_S, int r_F, int c_S, int c_F)
{
	// PARAMETER
	Parameter parameter;

	get_Parameter(parameter, structure_Item, whats_This);

	/*
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);
	if(item_Type_String == whats_This_Ambient)
	{
		if(whats_This == whats_This_Absolute_Density)	parameter = data.value<Ambient>().absolute_Density;
		if(whats_This == whats_This_Relative_Density)	parameter = data.value<Ambient>().relative_Density;
	}
	if(item_Type_String == whats_This_Layer)
	{
		if(whats_This == whats_This_Absolute_Density)				parameter = data.value<Layer>().absolute_Density;
		if(whats_This == whats_This_Relative_Density)				parameter = data.value<Layer>().relative_Density;
		if(whats_This == whats_This_Thickness)						parameter = data.value<Layer>().thickness;
		if(whats_This == whats_This_Sigma)							parameter = data.value<Layer>().sigma;

		if(whats_This == whats_This_Thickness_Drift_Line_Value)		parameter = data.value<Layer>().thickness_Drift.drift_Line_Value;
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		parameter = data.value<Layer>().thickness_Drift.drift_Rand_Rms;
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	parameter = data.value<Layer>().thickness_Drift.drift_Sine_Amplitude;
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	parameter = data.value<Layer>().thickness_Drift.drift_Sine_Frequency;
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		parameter = data.value<Layer>().thickness_Drift.drift_Sine_Phase;

		if(whats_This == whats_This_Sigma_Drift_Line_Value)			parameter = data.value<Layer>().sigma_Drift.drift_Line_Value;
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			parameter = data.value<Layer>().sigma_Drift.drift_Rand_Rms;
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		parameter = data.value<Layer>().sigma_Drift.drift_Sine_Amplitude;
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		parameter = data.value<Layer>().sigma_Drift.drift_Sine_Frequency;
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			parameter = data.value<Layer>().sigma_Drift.drift_Sine_Phase;
	}
	if(item_Type_String == whats_This_Substrate)
	{
		if(whats_This == whats_This_Absolute_Density)	parameter = data.value<Substrate>().absolute_Density;
		if(whats_This == whats_This_Relative_Density)	parameter = data.value<Substrate>().relative_Density;
		if(whats_This == whats_This_Sigma)				parameter = data.value<Substrate>().sigma;
	}
	if(item_Type_String == whats_This_Multilayer)
	{
		if(whats_This == whats_This_Period)				parameter = data.value<Stack_Content>().period;
		if(whats_This == whats_This_Gamma)				parameter = data.value<Stack_Content>().gamma;
	}*/

	// create check_Box
	QCheckBox* check_Box = new QCheckBox("fit");

	// alignment
	QWidget* back_Widget = new QWidget;
	QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
	back_Layout->addWidget(check_Box);
	back_Layout->setSpacing(0);
	back_Layout->setContentsMargins(0,0,0,0);
	back_Layout->setAlignment(Qt::AlignCenter);

	check_Box->setProperty(item_Type_Property, item_Type_String);
	check_Box->setProperty(whats_This_Property, whats_This);
	check_Box->setProperty(row_Property, current_Row);
	check_Box->setProperty(column_Property, current_Column);

	// which cells disable and enable
	check_Box->setProperty(relative_Rows_To_Disable_Start_Property, r_S);
	check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, r_F);
	check_Box->setProperty(relative_Columns_To_Disable_Start_Property, c_S);
	check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, c_F);

	// for reloading
	check_Box->setProperty(reload_Property, false);
	all_Widgets_To_Reload.append(check_Box);

	// create item
	table->setCellWidget(current_Row, current_Column, back_Widget);
	check_Boxes_Map.insert(check_Box, structure_Item);

	// set parameter id to BACK widget
	if(	whats_This == whats_This_Thickness_Drift_Sine_Amplitude ||
			whats_This == whats_This_Thickness_Drift_Sine_Frequency ||
			whats_This == whats_This_Thickness_Drift_Sine_Phase		||

			whats_This == whats_This_Sigma_Drift_Sine_Amplitude		||
			whats_This == whats_This_Sigma_Drift_Sine_Frequency		||
			whats_This == whats_This_Sigma_Drift_Sine_Phase			)
	{
		QVariant var;
		parameter.indicator.tab_Index = tab_Index;
		parameter.indicator.full_Name = structure_Item->whatsThis(DEFAULT_COLUMN) + " " + whats_This;
		var.setValue( parameter );
		back_Widget->setProperty(parameter_Property,var);
		back_Widget->setWhatsThis(structure_Item->whatsThis(DEFAULT_COLUMN));
		coupled_Widgets_Map.insert(back_Widget,structure_Item);
		loaded_Parameters.append(parameter.indicator.id);
	}

	connect(check_Box, &QCheckBox::toggled, this, [=]{refresh_Fit_Parameter(); });
	//	connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(refresh_Fit_Parameter(bool))); old version

	connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off(table); });
	//	connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(cells_On_Off(bool))); old version
	check_Box->setChecked(parameter.fit.is_Fitable);
	//	check_Box->toggled(false);
}

// for interlayers
void Table_Of_Structures::create_Check_Box_Label_Interlayer(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int tab_Index)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;
	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		QCheckBox* check_Box = new QCheckBox(transition_Layer_Functions[interlayer_Index]);

		// enable/disable function
		check_Box->setProperty(row_Property, current_Row);
		check_Box->setProperty(column_Property, current_Column);

		// first group
		check_Box->setProperty(relative_Rows_To_Disable_Start_Property, 1);
		check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, 1);
		check_Box->setProperty(relative_Columns_To_Disable_Start_Property, 0);
		check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, 0);

		// second group
		check_Box->setProperty(relative_Rows_To_Disable_Start_Property_2, 4);
		check_Box->setProperty(relative_Rows_To_Disable_Finish_Property_2, 4);
		check_Box->setProperty(relative_Columns_To_Disable_Start_Property_2, 0);
		check_Box->setProperty(relative_Columns_To_Disable_Finish_Property_2, 0);

		check_Box->setProperty(interlayer_Index_Property, interlayer_Index);
		check_Box->setProperty(item_Type_Property, item_Type_String);
		check_Boxes_Map.insert(check_Box, structure_Item);

		// for reloading
		check_Box->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(check_Box);

		check_Box->setChecked(interlayer_Composition[interlayer_Index].enabled);

		connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off(table); });
		//		connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(cells_On_Off(bool)));  old version

		connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off_2(table); });
		//		connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(cells_On_Off_2(bool))); old version

		connect(check_Box, &QCheckBox::toggled, this, [=]{refresh_Check_Box_Label_Interlayer(); });
		//		connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(refresh_Check_Box_Label_Interlayer(bool)));  old version

		//		check_Box->toggled(false);

		// alignment
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		{
			// set parameter id to BACK widget
			QVariant var;
			interlayer_Composition[interlayer_Index].interlayer.indicator.tab_Index = tab_Index;
			interlayer_Composition[interlayer_Index].interlayer.indicator.full_Name = structure_Item->whatsThis(DEFAULT_COLUMN) + " " + "Weight "+transition_Layer_Functions[interlayer_Index];
			var.setValue( interlayer_Composition[interlayer_Index].interlayer );
			back_Widget->setProperty(parameter_Property,var);
			back_Widget->setWhatsThis(structure_Item->whatsThis(DEFAULT_COLUMN));
			coupled_Widgets_Map.insert(back_Widget,structure_Item);
			loaded_Parameters.append(interlayer_Composition[interlayer_Index].interlayer.indicator.id);
		}

		back_Widget->setStyleSheet("background-color: lightblue");
		table->setCellWidget(current_Row, current_Column, back_Widget);

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_Weigts_Interlayer(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;
	double value = -2017;
	char format = line_edit_short_double_format;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		if(val_Type == VAL)	{value = interlayer_Composition[interlayer_Index].interlayer.value;		format = line_edit_short_double_format;}
		if(val_Type == MIN)	{value = interlayer_Composition[interlayer_Index].interlayer.fit.min;	format = line_edit_short_double_format;}
		if(val_Type == MAX)	{value = interlayer_Composition[interlayer_Index].interlayer.fit.max;	format = line_edit_short_double_format;}

		QString text_Value = QString::number(value, format, line_edit_interlayer_precision);

		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

		line_Edit->setProperty(interlayer_Index_Property, interlayer_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(item_Type_Property, item_Type_String);
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setProperty(value_Type_Property, val_Type);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		// for reloading
		line_Edit->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(line_Edit);

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit(table); });

		// create item (set LineEdits_Map)
		table->setCellWidget(current_Row, current_Column, line_Edit);
		line_Edits_Map.insert(line_Edit, structure_Item);
		coupled_Id_LineEdits_Map.insert(interlayer_Composition[interlayer_Index].interlayer.indicator.id,line_Edit);

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{refresh_Weigts_Interlayer(); });

//		line_Edit->textEdited(line_Edit->text());

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_Weights_Check_Box_Fit_Interlayer(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int tab_Index)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		QCheckBox* check_Box = new QCheckBox("fit");

		check_Box->setProperty(interlayer_Index_Property, interlayer_Index);
		check_Box->setProperty(item_Type_Property, item_Type_String);
		check_Boxes_Map.insert(check_Box, structure_Item);

		// for reloading
		check_Box->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(check_Box);

		check_Box->setChecked(interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable);

		connect(check_Box, &QCheckBox::toggled, this, [=]{refresh_Weights_Check_Box_Fit_Interlayer(); });
		//		connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(refresh_Weights_Check_Box_Fit_Interlayer(bool))); old version

		//		check_Box->toggled(false);

		// alignment
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		{
			// set parameter id to BACK widget
			QVariant var;
			interlayer_Composition[interlayer_Index].interlayer.indicator.tab_Index = tab_Index;
			interlayer_Composition[interlayer_Index].interlayer.indicator.full_Name = structure_Item->whatsThis(DEFAULT_COLUMN) + " " + "Weight "+transition_Layer_Functions[interlayer_Index];
			var.setValue( interlayer_Composition[interlayer_Index].interlayer );
			///	unused for non-duplicating parameter
			//	back_Widget->setProperty(parameter_Property,var);
			//	back_Widget->setWhatsThis(structure_Item->whatsThis(DEFAULT_COLUMN));
			//	coupled_Widgets_Map.insert(back_Widget,structure_Item);
			//	loaded_Parameters.append(interlayer_Composition[interlayer_Index].interlayer.indicator.id);
		}

		table->setCellWidget(current_Row, current_Column, back_Widget);

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_MySigma_Labels_Interlayer(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int tab_Index)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;
	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		QLabel* label = new QLabel(Sigma_Sym+" "+transition_Layer_Functions[interlayer_Index]);
		label->setStyleSheet("background-color: lightblue");
		label->setAlignment(Qt::AlignCenter);

		{
			// set parameter id to widget
			QVariant var;
			interlayer_Composition[interlayer_Index].my_Sigma.indicator.tab_Index = tab_Index;
			interlayer_Composition[interlayer_Index].my_Sigma.indicator.full_Name = structure_Item->whatsThis(DEFAULT_COLUMN) + " " + "Sigma "+transition_Layer_Functions[interlayer_Index];
			var.setValue( interlayer_Composition[interlayer_Index].my_Sigma );
			label->setProperty(parameter_Property,var);
			label->setWhatsThis(structure_Item->whatsThis(DEFAULT_COLUMN));
			coupled_Widgets_Map.insert(label,structure_Item);
			loaded_Parameters.append(interlayer_Composition[interlayer_Index].my_Sigma.indicator.id);
		}

		table->setCellWidget(current_Row, current_Column, label);

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_MySigma_Interlayer(My_Table_Widget* table, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;
	double value;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		value = interlayer_Composition[interlayer_Index].my_Sigma.value;

		QString text_Value = QString::number(value, line_edit_double_format, line_edit_sigma_precision);

		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

		line_Edit->setProperty(interlayer_Index_Property, interlayer_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(item_Type_Property, item_Type_String);
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));


		// for reloading
		line_Edit->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(line_Edit);

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit(table); });

		// create item (set LineEdits_Map)
		table->setCellWidget(current_Row, current_Column, line_Edit);
		line_Edits_Map.insert(line_Edit, structure_Item);
		coupled_Id_LineEdits_Map.insert(interlayer_Composition[interlayer_Index].my_Sigma.indicator.id,line_Edit);

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{refresh_MySigma_Interlayer(); });

//		line_Edit->textEdited(line_Edit->text());

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

// refresh

// for material only
void Table_Of_Structures::refresh_Element()
{
	QComboBox* combo_Box = qobject_cast<QComboBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = elements_Map.value(combo_Box);
	QString item_Type_String = combo_Box->property(item_Type_Property).toString();
	int composition_Index = combo_Box->property(num_Chemic_Element_Property).toInt();

	// for reloading
	bool reload = combo_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)	{combo_Box->setCurrentIndex(combo_Box->findText(ambient.composition[composition_Index].type));	return;	}

		ambient.composition[composition_Index].type = combo_Box->currentText();
		ambient.material = material_From_Composition(ambient.composition);
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{combo_Box->setCurrentIndex(combo_Box->findText(layer.composition[composition_Index].type));	return;	}

		layer.composition[composition_Index].type = combo_Box->currentText();
		layer.material = material_From_Composition(layer.composition);
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{combo_Box->setCurrentIndex(combo_Box->findText(substrate.composition[composition_Index].type));return;	}

		substrate.composition[composition_Index].type = combo_Box->currentText();
		substrate.material = material_From_Composition(substrate.composition);
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Stoich()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();
	int composition_Index = line_Edit->property(num_Chemic_Element_Property).toInt();
	QString value_Type = line_Edit->property(value_Type_Property).toString();

	char format = line_edit_short_double_format;
	int precision = line_edit_composition_precision;

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(ambient.composition[composition_Index].composition.value,   format, precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(ambient.composition[composition_Index].composition.fit.min, format, precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(ambient.composition[composition_Index].composition.fit.max, format, precision));

			return;
		}

		if(value_Type == VAL)	ambient.composition[composition_Index].composition.value = line_Edit->text().toDouble();
		if(value_Type == MIN)	ambient.composition[composition_Index].composition.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	ambient.composition[composition_Index].composition.fit.max = line_Edit->text().toDouble();

		ambient.material = material_From_Composition(ambient.composition);
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(layer.composition[composition_Index].composition.value,   format, precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(layer.composition[composition_Index].composition.fit.min, format, precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(layer.composition[composition_Index].composition.fit.max, format, precision));

			return;
		}

		if(value_Type == VAL)	layer.composition[composition_Index].composition.value = line_Edit->text().toDouble();
		if(value_Type == MIN)	layer.composition[composition_Index].composition.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	layer.composition[composition_Index].composition.fit.max = line_Edit->text().toDouble();

		layer.material = material_From_Composition(layer.composition);
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(substrate.composition[composition_Index].composition.value,   format, precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.composition[composition_Index].composition.fit.min, format, precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.composition[composition_Index].composition.fit.max, format, precision));

			return;
		}

		if(value_Type == VAL)	substrate.composition[composition_Index].composition.value = line_Edit->text().toDouble();
		if(value_Type == MIN)	substrate.composition[composition_Index].composition.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	substrate.composition[composition_Index].composition.fit.max = line_Edit->text().toDouble();

		substrate.material = material_From_Composition(substrate.composition);
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Fit_Element()
{
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	int composition_Index = check_Box->property(num_Chemic_Element_Property).toInt();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)	{check_Box->setChecked(ambient.composition[composition_Index].composition.fit.is_Fitable);	return;	}

		ambient.composition[composition_Index].composition.fit.is_Fitable = check_Box->isChecked();
		ambient.material = material_From_Composition(ambient.composition);
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{check_Box->setChecked(layer.composition[composition_Index].composition.fit.is_Fitable);	return;	}

		layer.composition[composition_Index].composition.fit.is_Fitable = check_Box->isChecked();
		layer.material = material_From_Composition(layer.composition);
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{check_Box->setChecked(substrate.composition[composition_Index].composition.fit.is_Fitable);	return;	}

		substrate.composition[composition_Index].composition.fit.is_Fitable = check_Box->isChecked();
		substrate.material = material_From_Composition(substrate.composition);
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}


void Table_Of_Structures::refresh_Material()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)	{line_Edit->setText(ambient.material);	return;	}

		ambient.material = line_Edit->text();
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{line_Edit->setText(layer.material);	return;	}

		layer.material = line_Edit->text();
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{line_Edit->setText(substrate.material);	return;	}

		substrate.material = line_Edit->text();
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::check_Material()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();

	// no cheking for reloading
	bool reload = line_Edit->property(reload_Property).toBool();
	if(reload)	{return;}

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		line_Edit->textEdited(line_Edit->text());
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		if(optical_Constants->material_Map.contains(ambient.material + nk_Ext))
		{
			ambient.approved_Material = ambient.material;
		} else
		{
			QMessageBox::information(this, "Wrong material", "File \"" + ambient.material + nk_Ext + "\" not found");
			ambient.material = ambient.approved_Material;
			line_Edit->setText(ambient.material);
			line_Edit->textEdited(line_Edit->text());
		}
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		line_Edit->textEdited(line_Edit->text());

		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		if(optical_Constants->material_Map.contains(layer.material + nk_Ext))
		{
			layer.approved_Material = layer.material;
		} else
		{
			QMessageBox::information(this, "Wrong material", "File \"" + layer.material + nk_Ext + "\" not found");
			layer.material = layer.approved_Material;
			line_Edit->setText(layer.material);
			line_Edit->textEdited(line_Edit->text());
		}
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		line_Edit->textEdited(line_Edit->text());
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		if(optical_Constants->material_Map.contains(substrate.material + nk_Ext))
		{
			substrate.approved_Material = substrate.material;
		} else
		{
			QMessageBox::information(this, "Wrong material", "File \"" + substrate.material + nk_Ext + "\" not found");
			substrate.material = substrate.approved_Material;
			line_Edit->setText(substrate.material);
			line_Edit->textEdited(line_Edit->text());
		}
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::browse_Material()
{
	// TODO
	qInfo() << "browse...";
}

void Table_Of_Structures::refresh_Header()
{
	// PARAMETER
	QLabel* label = qobject_cast<QLabel*>(QObject::sender());
	QString item_Type_String = label->property(item_Type_Property).toString();
	QString whats_This = label->property(whats_This_Property).toString();

	// for reloading
	bool reload = label->property(reload_Property).toBool();

	if(item_Type_String == whats_This_Layer)
	{
		if(reload)
		{
			if(whats_This == whats_This_Thickness)
			{
				label->setText("z ["+length_units+"]");
			}
			return;
		}
	}
	if(item_Type_String == whats_This_Multilayer)
	{
		if(reload)
		{
			if(whats_This == whats_This_Period)
			{
				label->setText("d ["+length_units+"]");
			}
			return;
		}
	}
}

// for several parameters
void Table_Of_Structures::refresh_Check_Box_Header()
{
	// PARAMETER
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	QString whats_This = check_Box->property(whats_This_Property).toString();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);
	QVariant var;
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = data.value<Layer>();

		if(reload)
		{
			if(whats_This == whats_This_Thickness_Drift_Line_Value)	check_Box->setChecked(layer.thickness_Drift.is_Drift_Line);
			if(whats_This == whats_This_Thickness_Drift_Rand_Rms  )	check_Box->setChecked(layer.thickness_Drift.is_Drift_Rand);
			if(whats_This == whats_This_Thickness_Drift_Sine	  )	check_Box->setChecked(layer.thickness_Drift.is_Drift_Sine);

			if(whats_This == whats_This_Sigma_Drift_Line_Value)		check_Box->setChecked(layer.sigma_Drift.is_Drift_Line);
			if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )		check_Box->setChecked(layer.sigma_Drift.is_Drift_Rand);
			if(whats_This == whats_This_Sigma_Drift_Sine	  )		check_Box->setChecked(layer.sigma_Drift.is_Drift_Sine);

			if(whats_This == whats_This_Sigma)
			{
				check_Box->setChecked(layer.common_Sigma);
				check_Box->setText(Sigma_Sym+" ["+length_units+"]");
			}
			return;
		}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)	layer.thickness_Drift.is_Drift_Line = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms  )	layer.thickness_Drift.is_Drift_Rand = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine	  )	layer.thickness_Drift.is_Drift_Sine = check_Box->isChecked();

		if(whats_This == whats_This_Sigma_Drift_Line_Value)		layer.sigma_Drift.is_Drift_Line = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )		layer.sigma_Drift.is_Drift_Rand = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine	  )		layer.sigma_Drift.is_Drift_Sine = check_Box->isChecked();

		if(whats_This == whats_This_Sigma)			layer.common_Sigma = check_Box->isChecked();

		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = data.value<Substrate>();

		if(reload)
		{
			if(whats_This == whats_This_Sigma)
			{
				check_Box->setChecked(substrate.common_Sigma);
				check_Box->setText(Sigma_Sym+" ["+length_units+"]");
			}
			return;
		}
		if(whats_This == whats_This_Sigma)	substrate.common_Sigma = check_Box->isChecked();

		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor)
{
	QVariant var;
	for(int i=0; i<multilayer_Item->childCount(); ++i)
	{
		// if layer
		if(multilayer_Item->child(i)->childCount()==0)
		{
			Layer layer = multilayer_Item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
			layer.thickness.value = layer.thickness.value*factor;
			var.setValue( layer );
			multilayer_Item->child(i)->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		} else
			// if multilayer
		{
			change_Child_Layers_Thickness(multilayer_Item->child(i), factor);
		}
	}
}

void Table_Of_Structures::reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness)
{
	Stack_Content stack_Content = multilayer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
	double factor=1;
	if( abs(stack_Content.period.value) > DBL_EPSILON && stack_Content.num_Repetition.value!=0)
	{
		factor = new_Thickness/(  stack_Content.period.value*stack_Content.num_Repetition.value  );
		change_Child_Layers_Thickness(multilayer_Item, factor);
	}
}

void Table_Of_Structures::reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness)
{
	QVariant var;
	Layer layer = layer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
	layer.thickness.value = new_Thickness;
	var.setValue( layer );
	layer_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

void Table_Of_Structures::change_Stack_Gamma(QTreeWidgetItem* structure_Item, double new_Gamma_Value)
{
	QVariant var;
	Stack_Content stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
	int i=0;
	{
		double new_Thickness = stack_Content.period.value*new_Gamma_Value;
		if(structure_Item->child(i)->childCount()==0)	// children are simple layers
		{
			reset_Layer_Thickness(structure_Item->child(i), new_Thickness);
		}
		if(structure_Item->child(i)->childCount()>0)	// children are multilayers
		{
			reset_Multilayer_Thickness(structure_Item->child(i), new_Thickness);
		}
	}
	i=1;
	{
		double new_Thickness = stack_Content.period.value*(1-new_Gamma_Value);
		if(structure_Item->child(i)->childCount()==0)	// children are simple layers
		{
			reset_Layer_Thickness(structure_Item->child(i), new_Thickness);
		}
		if(structure_Item->child(i)->childCount()>0)	// children are multilayers
		{
			reset_Multilayer_Thickness(structure_Item->child(i), new_Thickness);
		}
	}
	var.setValue( stack_Content );
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

// for all parameters
void Table_Of_Structures::refresh_Parameter(My_Table_Widget* table)
{
	double length_Coeff = length_Coefficients_Map.value(length_units);

	// PARAMETER
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();
	QString value_Type = line_Edit->property(value_Type_Property).toString();
	QString whats_This = line_Edit->property(whats_This_Property).toString();

	char min_Max_Format = line_edit_short_double_format;

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(ambient.absolute_Density.value,	 line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(ambient.absolute_Density.fit.min, min_Max_Format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(ambient.absolute_Density.fit.max, min_Max_Format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Relative_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(ambient.relative_Density.value,	 line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(ambient.relative_Density.fit.min, min_Max_Format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(ambient.relative_Density.fit.max, min_Max_Format, line_edit_density_precision));
			}
			return;
		}

		if(whats_This == whats_This_Absolute_Density)
		{
			if(value_Type == VAL)	ambient.absolute_Density.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	ambient.absolute_Density.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	ambient.absolute_Density.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(value_Type == VAL)	ambient.relative_Density.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	ambient.relative_Density.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	ambient.relative_Density.fit.max = line_Edit->text().toDouble();
		}

		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.absolute_Density.value,   line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.absolute_Density.fit.min, min_Max_Format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.absolute_Density.fit.max, min_Max_Format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Relative_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.relative_Density.value,   line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.relative_Density.fit.min, min_Max_Format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.relative_Density.fit.max, min_Max_Format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Thickness)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness.value/length_Coeff,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness.fit.min/length_Coeff, min_Max_Format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness.fit.max/length_Coeff, min_Max_Format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Sigma)
			{
				if(value_Type == VAL)
				{
					line_Edit->setText(QString::number(layer.sigma.value/length_Coeff,   line_edit_double_format, line_edit_sigma_precision));
					line_Edit->setDisabled(!layer.common_Sigma);
					line_Edit->setStyleSheet("border: 1px solid grey");
				}
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma.fit.min/length_Coeff, min_Max_Format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma.fit.max/length_Coeff, min_Max_Format, line_edit_sigma_precision));
			}

			if(whats_This == whats_This_Thickness_Drift_Line_Value)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Line_Value.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Line_Value.fit.min, min_Max_Format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Line_Value.fit.max, min_Max_Format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Thickness_Drift_Rand_Rms)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Rand_Rms.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Rand_Rms.fit.min, min_Max_Format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Rand_Rms.fit.max, min_Max_Format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Amplitude.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Amplitude.fit.min, min_Max_Format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Amplitude.fit.max, min_Max_Format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Frequency.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Frequency.fit.min, min_Max_Format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Frequency.fit.max, min_Max_Format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Thickness_Drift_Sine_Phase)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Phase.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Phase.fit.min, min_Max_Format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Phase.fit.max, min_Max_Format, line_edit_thickness_precision));
			}

			if(whats_This == whats_This_Sigma_Drift_Line_Value)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Line_Value.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Line_Value.fit.min, min_Max_Format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Line_Value.fit.max, min_Max_Format, line_edit_sigma_precision));
			}
			if(whats_This == whats_This_Sigma_Drift_Rand_Rms)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Rand_Rms.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Rand_Rms.fit.min, min_Max_Format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Rand_Rms.fit.max, min_Max_Format, line_edit_sigma_precision));
			}
			if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Amplitude.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Amplitude.fit.min, min_Max_Format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Amplitude.fit.max, min_Max_Format, line_edit_sigma_precision));
			}
			if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Frequency.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Frequency.fit.min, min_Max_Format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Frequency.fit.max, min_Max_Format, line_edit_sigma_precision));
			}
			if(whats_This == whats_This_Sigma_Drift_Sine_Phase)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Phase.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Phase.fit.min, min_Max_Format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Phase.fit.max, min_Max_Format, line_edit_sigma_precision));
			}
			return;
		}

		if(whats_This == whats_This_Absolute_Density)
		{
			if(value_Type == VAL)	layer.absolute_Density.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.absolute_Density.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.absolute_Density.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(value_Type == VAL)	layer.relative_Density.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.relative_Density.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.relative_Density.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness)
		{
			if(value_Type == VAL)	layer.thickness.value = line_Edit->text().toDouble()*length_Coeff;
			if(value_Type == MIN)	layer.thickness.fit.min = line_Edit->text().toDouble()*length_Coeff;
			if(value_Type == MAX)	layer.thickness.fit.max = line_Edit->text().toDouble()*length_Coeff;
		}
		if(whats_This == whats_This_Sigma)
		{
			if(value_Type == VAL)
			{
				layer.sigma.value = line_Edit->text().toDouble()*length_Coeff;
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					layer.interlayer_Composition[interlayer_Index].my_Sigma.value = layer.sigma.value;
				}
			}
			if(value_Type == MIN)
			{
				layer.sigma.fit.min = line_Edit->text().toDouble()*length_Coeff;
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					layer.interlayer_Composition[interlayer_Index].my_Sigma.fit.min = layer.sigma.fit.min;
				}
			}
			if(value_Type == MAX)
			{
				layer.sigma.fit.max = line_Edit->text().toDouble()*length_Coeff;
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					layer.interlayer_Composition[interlayer_Index].my_Sigma.fit.max = layer.sigma.fit.max;
				}
			}
		}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Line_Value.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Line_Value.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Line_Value.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Rand_Rms.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Rand_Rms.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Rand_Rms.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Sine_Amplitude.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Sine_Amplitude.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Sine_Amplitude.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Sine_Frequency.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Sine_Frequency.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Sine_Frequency.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Sine_Phase.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Sine_Phase.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Sine_Phase.fit.max = line_Edit->text().toDouble();
		}

		if(whats_This == whats_This_Sigma_Drift_Line_Value)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Line_Value.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Line_Value.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Line_Value.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Rand_Rms.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Rand_Rms.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Rand_Rms.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Sine_Amplitude.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Sine_Amplitude.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Sine_Amplitude.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Sine_Frequency.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Sine_Frequency.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Sine_Frequency.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Sine_Phase.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Sine_Phase.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Sine_Phase.fit.max = line_Edit->text().toDouble();
		}

		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(substrate.absolute_Density.value,   line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.absolute_Density.fit.min, min_Max_Format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.absolute_Density.fit.max, min_Max_Format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Relative_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(substrate.relative_Density.value,   line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.relative_Density.fit.min, min_Max_Format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.relative_Density.fit.max, min_Max_Format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Sigma)
			{
				if(value_Type == VAL)
				{
					line_Edit->setText(QString::number(substrate.sigma.value/length_Coeff,   line_edit_double_format, line_edit_sigma_precision));
					line_Edit->setDisabled(!substrate.common_Sigma);
					line_Edit->setStyleSheet("border: 1px solid grey");
				}
				if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.sigma.fit.min/length_Coeff, min_Max_Format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.sigma.fit.max/length_Coeff, min_Max_Format, line_edit_sigma_precision));
			}
			return;
		}
		if(whats_This == whats_This_Absolute_Density)
		{
			if(value_Type == VAL)	{substrate.absolute_Density.value = line_Edit->text().toDouble();	}
			if(value_Type == MIN)	{substrate.absolute_Density.fit.min = line_Edit->text().toDouble();}
			if(value_Type == MAX)	{substrate.absolute_Density.fit.max = line_Edit->text().toDouble();}
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(value_Type == VAL)	{substrate.relative_Density.value = line_Edit->text().toDouble();	}
			if(value_Type == MIN)	{substrate.relative_Density.fit.min = line_Edit->text().toDouble();}
			if(value_Type == MAX)	{substrate.relative_Density.fit.max = line_Edit->text().toDouble();}
		}
		if(whats_This == whats_This_Sigma)
		{
			if(value_Type == VAL)
			{
				substrate.sigma.value = line_Edit->text().toDouble()*length_Coeff;
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					substrate.interlayer_Composition[interlayer_Index].my_Sigma.value = substrate.sigma.value;
				}
			}
			if(value_Type == MIN)
			{
				substrate.sigma.fit.min = line_Edit->text().toDouble()*length_Coeff;
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					substrate.interlayer_Composition[interlayer_Index].my_Sigma.fit.min = substrate.sigma.fit.min;
				}
			}
			if(value_Type == MAX)
			{
				substrate.sigma.fit.max = line_Edit->text().toDouble()*length_Coeff;
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					substrate.interlayer_Composition[interlayer_Index].my_Sigma.fit.max = substrate.sigma.fit.max;
				}
			}
		}

		var.setValue( substrate );
	}
	if(item_Type_String == whats_This_Multilayer)
	{
		Stack_Content stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();

		if(reload)
		{
			if(whats_This == whats_This_Num_Repetitions)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(stack_Content.num_Repetition.value));
			}
			if(whats_This == whats_This_Period)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(stack_Content.period.value/length_Coeff,   line_edit_double_format, line_edit_period_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(stack_Content.period.fit.min/length_Coeff, min_Max_Format, line_edit_period_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(stack_Content.period.fit.max/length_Coeff, min_Max_Format, line_edit_period_precision));
			}
			if(whats_This == whats_This_Gamma)
			{
				if(value_Type == VAL)
				{
					if( abs(stack_Content.period.value) > DBL_EPSILON )
					{
						line_Edit->setText(QString::number(stack_Content.gamma.value, min_Max_Format, line_edit_gamma_precision));
						line_Edit->setDisabled(false);
						line_Edit->setStyleSheet("border: 1px solid grey");
					} else
					{
						line_Edit->setDisabled(true);
						line_Edit->setStyleSheet("border: 0px solid grey");
					}
				}
				if(value_Type == MIN)	line_Edit->setText(QString::number(stack_Content.gamma.fit.min, min_Max_Format, line_edit_gamma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(stack_Content.gamma.fit.max, min_Max_Format, line_edit_gamma_precision));
			}
			return;
		}

		if(whats_This == whats_This_Num_Repetitions)
		{
			if(value_Type == VAL)	stack_Content.num_Repetition.value = line_Edit->text().toInt();
		}
		if(whats_This == whats_This_Period)
		{
			if(value_Type == VAL)
			{
				double new_Period_Value = line_Edit->text().toDouble()*length_Coeff;
				if(		qApp->focusWidget() != line_Edit
						||	abs(new_Period_Value) > DBL_EPSILON )
				{
					double factor = 1;
					if( abs(stack_Content.period.value) > DBL_EPSILON )	factor = new_Period_Value / stack_Content.period.value;
					change_Child_Layers_Thickness(structure_Item, factor);
				}
			}
			if(value_Type == MIN)	stack_Content.period.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	stack_Content.period.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Gamma)
		{
			if(value_Type == VAL)
			{
				double new_Gamma_Value = line_Edit->text().toDouble();
				if(new_Gamma_Value>1)
				{
					line_Edit->setText(QString::number(stack_Content.gamma.value));
					resize_Line_Edit(table, line_Edit);
				} else
				{
					if(		qApp->focusWidget() != line_Edit
							||	(abs(new_Gamma_Value) > DBL_EPSILON && abs(new_Gamma_Value-1) > DBL_EPSILON) )
					{
						if( abs(stack_Content.period.value) > DBL_EPSILON )	change_Stack_Gamma(structure_Item, new_Gamma_Value);
					}
				}
			}
			if(value_Type == MIN)	stack_Content.gamma.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	stack_Content.gamma.fit.max = line_Edit->text().toDouble();
		}
		var.setValue( stack_Content );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Fit_Parameter()
{
	// PARAMETER
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	QString whats_This = check_Box->property(whats_This_Property).toString();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)	check_Box->setChecked(ambient.absolute_Density.fit.is_Fitable);
			if(whats_This == whats_This_Relative_Density)	check_Box->setChecked(ambient.absolute_Density.fit.is_Fitable);

			return;
		}

		if(whats_This == whats_This_Absolute_Density)	ambient.absolute_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Relative_Density)	ambient.relative_Density.fit.is_Fitable = check_Box->isChecked();

		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)				check_Box->setChecked(layer.absolute_Density.fit.is_Fitable);
			if(whats_This == whats_This_Relative_Density)				check_Box->setChecked(layer.relative_Density.fit.is_Fitable);
			if(whats_This == whats_This_Thickness)						check_Box->setChecked(layer.thickness.fit.is_Fitable);
			if(whats_This == whats_This_Sigma)                  		check_Box->setChecked(layer.sigma.fit.is_Fitable);

			if(whats_This == whats_This_Thickness_Drift_Line_Value)		check_Box->setChecked(layer.thickness_Drift.drift_Line_Value.fit.is_Fitable);
			if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		check_Box->setChecked(layer.thickness_Drift.drift_Rand_Rms.fit.is_Fitable);
			if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	check_Box->setChecked(layer.thickness_Drift.drift_Sine_Amplitude.fit.is_Fitable);
			if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	check_Box->setChecked(layer.thickness_Drift.drift_Sine_Frequency.fit.is_Fitable);
			if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		check_Box->setChecked(layer.thickness_Drift.drift_Sine_Phase.fit.is_Fitable);

			if(whats_This == whats_This_Sigma_Drift_Line_Value)			check_Box->setChecked(layer.sigma_Drift.drift_Line_Value.fit.is_Fitable);
			if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			check_Box->setChecked(layer.sigma_Drift.drift_Rand_Rms.fit.is_Fitable);
			if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		check_Box->setChecked(layer.sigma_Drift.drift_Sine_Amplitude.fit.is_Fitable);
			if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		check_Box->setChecked(layer.sigma_Drift.drift_Sine_Frequency.fit.is_Fitable);
			if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			check_Box->setChecked(layer.sigma_Drift.drift_Sine_Phase.fit.is_Fitable);

			return;
		}

		if(whats_This == whats_This_Absolute_Density)				layer.absolute_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Relative_Density)				layer.relative_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness)						layer.thickness.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma)
		{
			layer.sigma.fit.is_Fitable = check_Box->isChecked();
			for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
			{
				layer.interlayer_Composition[interlayer_Index].my_Sigma.fit.is_Fitable = check_Box->isChecked();
			}
		}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)		layer.thickness_Drift.drift_Line_Value.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		layer.thickness_Drift.drift_Rand_Rms.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	layer.thickness_Drift.drift_Sine_Amplitude.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	layer.thickness_Drift.drift_Sine_Frequency.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		layer.thickness_Drift.drift_Sine_Phase.fit.is_Fitable = check_Box->isChecked();

		if(whats_This == whats_This_Sigma_Drift_Line_Value)			layer.sigma_Drift.drift_Line_Value.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			layer.sigma_Drift.drift_Rand_Rms.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		layer.sigma_Drift.drift_Sine_Amplitude.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		layer.sigma_Drift.drift_Sine_Frequency.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			layer.sigma_Drift.drift_Sine_Phase.fit.is_Fitable = check_Box->isChecked();

		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)	check_Box->setChecked(substrate.absolute_Density.fit.is_Fitable);
			if(whats_This == whats_This_Relative_Density)	check_Box->setChecked(substrate.relative_Density.fit.is_Fitable);
			if(whats_This == whats_This_Sigma)              check_Box->setChecked(substrate.sigma.fit.is_Fitable);

			return;
		}

		if(whats_This == whats_This_Absolute_Density)	substrate.absolute_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Relative_Density)	substrate.relative_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma)
		{
			substrate.sigma.fit.is_Fitable = check_Box->isChecked();
			for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
			{
				substrate.interlayer_Composition[interlayer_Index].my_Sigma.fit.is_Fitable = check_Box->isChecked();
			}
		}

		var.setValue( substrate );
	}
	if(item_Type_String == whats_This_Multilayer)
	{
		Stack_Content stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();

		if(reload)
		{
			if(whats_This == whats_This_Period)	check_Box->setChecked(stack_Content.period.fit.is_Fitable);
			if(whats_This == whats_This_Gamma)	check_Box->setChecked(stack_Content.gamma.fit.is_Fitable);
			return;
		}
		if(whats_This == whats_This_Period)	stack_Content.period.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Gamma)	stack_Content.gamma.fit.is_Fitable = check_Box->isChecked();

		var.setValue( stack_Content );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

// for interlayers
void Table_Of_Structures::refresh_Check_Box_Label_Interlayer()
{
	// PARAMETER
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	int interlayer_Index = check_Box->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{check_Box->setChecked(layer.interlayer_Composition[interlayer_Index].enabled);	return;	}

		layer.interlayer_Composition[interlayer_Index].enabled = check_Box->isChecked();
		if(!layer.common_Sigma)
		{
			layer.sigma.value = recalculate_Sigma_From_Individuals(layer.interlayer_Composition);
		}
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{check_Box->setChecked(substrate.interlayer_Composition[interlayer_Index].enabled);	return;	}

		substrate.interlayer_Composition[interlayer_Index].enabled = check_Box->isChecked();
		if(!substrate.common_Sigma)
		{
			substrate.sigma.value = recalculate_Sigma_From_Individuals(substrate.interlayer_Composition);
		}
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Weigts_Interlayer()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();
	int interlayer_Index = line_Edit->property(interlayer_Index_Property).toInt();
	QString value_Type = line_Edit->property(value_Type_Property).toString();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;

	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(layer.interlayer_Composition[interlayer_Index].interlayer.value,	  line_edit_short_double_format, line_edit_interlayer_precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(layer.interlayer_Composition[interlayer_Index].interlayer.fit.min, line_edit_short_double_format, line_edit_interlayer_precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(layer.interlayer_Composition[interlayer_Index].interlayer.fit.max, line_edit_short_double_format, line_edit_interlayer_precision));

			return;
		}

		if(value_Type == VAL)
		{
			layer.interlayer_Composition[interlayer_Index].interlayer.value = line_Edit->text().toDouble();
			if(!layer.common_Sigma)
			{
				layer.sigma.value = recalculate_Sigma_From_Individuals(layer.interlayer_Composition);
			}
		}
		if(value_Type == MIN)	layer.interlayer_Composition[interlayer_Index].interlayer.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	layer.interlayer_Composition[interlayer_Index].interlayer.fit.max = line_Edit->text().toDouble();

		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(substrate.interlayer_Composition[interlayer_Index].interlayer.value,	  line_edit_short_double_format, line_edit_interlayer_precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.interlayer_Composition[interlayer_Index].interlayer.fit.min, line_edit_short_double_format, line_edit_interlayer_precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.interlayer_Composition[interlayer_Index].interlayer.fit.max, line_edit_short_double_format, line_edit_interlayer_precision));

			return;
		}

		if(value_Type == VAL)
		{
			substrate.interlayer_Composition[interlayer_Index].interlayer.value = line_Edit->text().toDouble();
			if(!substrate.common_Sigma)
			{
				substrate.sigma.value = recalculate_Sigma_From_Individuals(substrate.interlayer_Composition);
			}
		}
		if(value_Type == MIN)	substrate.interlayer_Composition[interlayer_Index].interlayer.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	substrate.interlayer_Composition[interlayer_Index].interlayer.fit.max = line_Edit->text().toDouble();

		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Weights_Check_Box_Fit_Interlayer()
{
	// PARAMETER
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	int interlayer_Index = check_Box->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{check_Box->setChecked(layer.interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable);	return;	}

		layer.interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable = check_Box->isChecked();
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{check_Box->setChecked(substrate.interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable);	return;	}

		substrate.interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable = check_Box->isChecked();
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_MySigma_Interlayer()
{
	double length_Coeff = length_Coefficients_Map.value(length_units);

	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();
	int interlayer_Index = line_Edit->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;

	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)
		{
			line_Edit->setText(QString::number(layer.interlayer_Composition[interlayer_Index].my_Sigma.value/length_Coeff, line_edit_double_format, line_edit_sigma_precision));
			bool disable = layer.common_Sigma || !layer.interlayer_Composition[interlayer_Index].enabled;
			line_Edit->setDisabled(disable);
			return;
		}

		layer.interlayer_Composition[interlayer_Index].my_Sigma.value = line_Edit->text().toDouble()*length_Coeff;
		if(!layer.common_Sigma)
		{
			layer.sigma.value = recalculate_Sigma_From_Individuals(layer.interlayer_Composition);
		}

		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)
		{
			line_Edit->setText(QString::number(substrate.interlayer_Composition[interlayer_Index].my_Sigma.value/length_Coeff, line_edit_double_format, line_edit_sigma_precision));
			bool disable = substrate.common_Sigma || !substrate.interlayer_Composition[interlayer_Index].enabled;
			line_Edit->setDisabled(disable);
			return;
		}

		substrate.interlayer_Composition[interlayer_Index].my_Sigma.value = line_Edit->text().toDouble()*length_Coeff;
		if(!substrate.common_Sigma)
		{
			substrate.sigma.value = recalculate_Sigma_From_Individuals(substrate.interlayer_Composition);
		}

		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	{
		emit_Data_Edited();
		reload_All_Widgets(QObject::sender());
	}
}

double Table_Of_Structures::recalculate_Sigma_From_Individuals(QVector<Interlayer>& interlayer_Composition)
{
	double temp_Sigma_Square=0;
	double sum = 0;
	for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
	{
		if(interlayer_Composition[interlayer_Index].enabled)
		{
			sum += interlayer_Composition[interlayer_Index].interlayer.value;
			temp_Sigma_Square += pow(interlayer_Composition[interlayer_Index].my_Sigma.value,2) * interlayer_Composition[interlayer_Index].interlayer.value;
		}
	}
	if(sum!=0)    return sqrt(temp_Sigma_Square/sum);
	else          return 0;
}



// general
void Table_Of_Structures::cells_On_Off(My_Table_Widget* table)
{
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());

	int current_Row = check_Box->property(row_Property).toInt();
	int current_Column = check_Box->property(column_Property).toInt();

	int row_Start = check_Box->property(relative_Rows_To_Disable_Start_Property).toInt();
	int row_Finish = check_Box->property(relative_Rows_To_Disable_Finish_Property).toInt();
	int column_Start = check_Box->property(relative_Columns_To_Disable_Start_Property).toInt();
	int column_Finish = check_Box->property(relative_Columns_To_Disable_Finish_Property).toInt();

	for(int row=row_Start; row<=row_Finish; ++row)
		for(int col=column_Start; col<=column_Finish; ++col)
		{
			QWidget* widget = table->cellWidget(current_Row+row,current_Column+col);
			if(widget)
			{
				widget->setDisabled(!check_Box->isChecked());

				if(!check_Box->isChecked())
					widget->setStyleSheet("border: 0px solid grey");
				else
					widget->setStyleSheet("border: 1px solid grey");
			}
		}
}

void Table_Of_Structures::cells_On_Off_2(My_Table_Widget* table)
{
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());

	int current_Row = check_Box->property(row_Property).toInt();
	int current_Column = check_Box->property(column_Property).toInt();

	int row_Start = check_Box->property(relative_Rows_To_Disable_Start_Property_2).toInt();
	int row_Finish = check_Box->property(relative_Rows_To_Disable_Finish_Property_2).toInt();
	int column_Start = check_Box->property(relative_Columns_To_Disable_Start_Property_2).toInt();
	int column_Finish = check_Box->property(relative_Columns_To_Disable_Finish_Property_2).toInt();

	for(int row=row_Start; row<=row_Finish; ++row)
		for(int col=column_Start; col<=column_Finish; ++col)
		{
			QWidget* widget = table->cellWidget(current_Row+row,current_Column+col);
			if(widget)
			{
				widget->setDisabled(!check_Box->isChecked());

				if(!check_Box->isChecked())
					widget->setStyleSheet("border: 0px solid red");
				else
					widget->setStyleSheet("border: 1px solid grey");
			}
		}
}

void Table_Of_Structures::resize_Line_Edit(My_Table_Widget* table, QLineEdit* line_Edit)
{
	if(!line_Edit) line_Edit = qobject_cast<QLineEdit*>(QObject::sender());

	QString text = line_Edit->text();
	QFontMetrics fm = line_Edit->fontMetrics();
	int width = fm.width(text) + QLINEEDIT_RESIZE_MARGIN;
	if(width<line_Edit->property(min_Size_Property).toInt())	{width=line_Edit->property(min_Size_Property).toInt();}
	line_Edit->setFixedWidth(width);

	int current_Column = line_Edit->property(column_Property).toInt();
	fit_Column(table, 0, current_Column);
}

void Table_Of_Structures::reload_All_Widgets(QObject* sender)
{
	if(table_Is_Created)
	{
//        qInfo() << "reload_All_Widgets " << ++temp_Counter;
		for(int i=0; i<all_Widgets_To_Reload.size(); ++i)
		{
			if(all_Widgets_To_Reload[i] != sender)
			{
				all_Widgets_To_Reload[i]->setProperty(reload_Property, true);

				QLabel*        label = qobject_cast<QLabel*>   (all_Widgets_To_Reload[i]);
				QCheckBox* check_Box = qobject_cast<QCheckBox*>(all_Widgets_To_Reload[i]);
				QLineEdit* line_Edit = qobject_cast<QLineEdit*>(all_Widgets_To_Reload[i]);
				QComboBox* combo_Box = qobject_cast<QComboBox*>(all_Widgets_To_Reload[i]);

				if(label)
				{
					label->windowTitleChanged("temp");
				}
				if(check_Box)
				{
					check_Box->toggled(false);
				}
				if(line_Edit)
				{
					line_Edit->textEdited("temp");			//
					line_Edit->textEdited("temp");			// repeated intentionally!
				}
				if(combo_Box)
				{
					combo_Box->currentTextChanged("temp");
				}

				all_Widgets_To_Reload[i]->setProperty(reload_Property, false);
			}
		}
	}
}
