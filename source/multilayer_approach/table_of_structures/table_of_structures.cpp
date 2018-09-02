// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "table_of_structures.h"
#include "algorithm"

Table_Of_Structures::Table_Of_Structures(QWidget *parent) :
	runned_Tables_Of_Structures(global_Multilayer_Approach->runned_Tables_Of_Structures),
	runned_Calculation_Settings_Editor(global_Multilayer_Approach->runned_Calculation_Settings_Editor),
	multilayer_Tabs(global_Multilayer_Approach->multilayer_Tabs),
	QWidget(parent) // nullptr!
{
	setWindowTitle("Table Of Structures");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Table_Of_Structures::closeEvent(QCloseEvent* event)
{
	write_Window_Geometry();
	runned_Tables_Of_Structures.remove(table_Key);
	unlock_Mainwindow_Interface();
	event->accept();
	delete this;
}

void Table_Of_Structures::emit_Data_Edited()
{
	emit data_Edited();
}

void Table_Of_Structures::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
	main_Layout->setContentsMargins(0,0,0,0);

	lock_Mainwindow_Interface();
	create_Menu();
	create_Tabs();
	main_Layout->addWidget(main_Tabs);

	add_Tabs();
}

void Table_Of_Structures::set_Window_Geometry()
{
	setGeometry(structure_table_x_corner,structure_table_y_corner,structure_table_width,structure_table_height);
}

void Table_Of_Structures::write_Window_Geometry()
{
	if(!isMaximized())
	{
		structure_table_x_corner = geometry().x()-WINDOW_BOUNDARY_SHIFT_X;
		structure_table_y_corner = geometry().y()-WINDOW_BOUNDARY_SHIFT_Y;

		structure_table_width  = geometry().width();
		structure_table_height = geometry().height();
	}
}

void Table_Of_Structures::lock_Mainwindow_Interface()
{
	// lock part of mainwindow functionality
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Structure_Tree* old_Structure_Tree = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i))->structure_Tree;

		// closing editors
		for(Item_Editor* editor : old_Structure_Tree->list_Editors)	editor->close();

		// disabling main interface
		old_Structure_Tree->structure_Toolbar->toolbar->setDisabled(true);
		old_Structure_Tree->tree->blockSignals(true);
		multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(true);
	}
	multilayer_Tabs->cornerWidget()->setDisabled(true);
	multilayer_Tabs->setMovable(false);
}

void Table_Of_Structures::unlock_Mainwindow_Interface()
{
	// unlock mainwindow functionality
	if(!runned_Tables_Of_Structures.contains(table_Key) && !runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
	{
		multilayer_Tabs->setMovable(true);
		multilayer_Tabs->cornerWidget()->setDisabled(false);
	}
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		list_Of_Trees[i]->structure_Toolbar->toolbar->setDisabled(false);
		list_Of_Trees[i]->tree->blockSignals(false);

		if(!runned_Tables_Of_Structures.contains(table_Key) && !runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setEnabled(true);
		}
	}
}

void Table_Of_Structures::create_Menu()
{
	Menu* menu = new Menu(window_Type_Table,this);
	main_Layout->setMenuBar(menu->menu_Bar);
	connect(menu, &Menu::refresh, this, [=]{reload_All_Widgets();});
	connect(menu, &Menu::refresh, this, &Table_Of_Structures::emit_Data_Edited);
}

void Table_Of_Structures::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(false);

	connect(main_Tabs,	&QTabWidget::currentChanged, [=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);

		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index)main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
		reload_All_Widgets();
	});
}

void Table_Of_Structures::add_Tabs()
{
	read_Trees();
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		My_Table_Widget* new_Table = new My_Table_Widget(basic_Row_Number, basic_Column_Number, this, this);
		all_Widgets_To_Reload.append(QList<QWidget*>());

		min_Max_Density_Line_Edits_List.append(QList<QLineEdit*>());
		min_Max_Thickness_Line_Edits_List.append(QList<QLineEdit*>());
		min_Max_Sigma_Line_Edits_List.append(QList<QLineEdit*>());

		create_Table(new_Table, tab_Index);
		main_Tabs->addTab(new_Table, multilayer_Tabs->tabText(tab_Index));
		disable_enable_Structure_Items(new_Table);
	}
	for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
	{
		if(i!=main_Tabs->currentIndex()) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
	}

	table_Is_Created = true;
	reload_All_Widgets();
}

void Table_Of_Structures::create_Table(My_Table_Widget* new_Table, int tab_Index)
{
	// PARAMETER

	rows_List_To_Span.clear();
	{
		// header for each structure
		QFont font_Header;
		font_Header.setBold(true);

		new_Table->insertRow(new_Table->rowCount());
		int current_Row = new_Table->rowCount()-1;

		// structure display
		int max_Depth = Global_Variables::get_Tree_Depth(list_Of_Trees[tab_Index]->tree->invisibleRootItem());
		int depth, current_Column;

		bool has_Layers=false;
		bool has_Boundaries=false;

		// calculate max_Number_Of_Elements for tabulation
		int max_Number_Of_Elements=1;
		{
			QTreeWidgetItem* structure_Item;
			QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);
			while (*it)
			{
				structure_Item=*it;
				Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

				// for min/max buttons
				if(struct_Data.item_Type == item_Type_Layer)		{has_Layers = true;}
				if(struct_Data.item_Type == item_Type_Layer    ||
				   struct_Data.item_Type == item_Type_Substrate )	{has_Boundaries = true;}

				if( struct_Data.item_Type == item_Type_Ambient  ||
					struct_Data.item_Type == item_Type_Layer    ||
					struct_Data.item_Type == item_Type_Substrate )
				{
					max_Number_Of_Elements = max(struct_Data.composition.size(),max_Number_Of_Elements);
				}
				++it;
			}
		}

		// place for min_max buttons
		{
			rows_List_To_Span.append(current_Row);
			new_Table->setItem(current_Row,0, new QTableWidgetItem("Set min/max"));
			new_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);
			new_Table->item   (current_Row,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			new_Table->item   (current_Row,0)->setFont(font_Header);

			new_Table->insertRow(new_Table->rowCount());
			new_Table->insertRow(new_Table->rowCount());

			current_Row = new_Table->rowCount()-2;

			new_Table->insertRow(new_Table->rowCount());
			new_Table->insertRow(new_Table->rowCount());

			add_Columns(new_Table, max_Depth + max_Number_Of_Elements+2);
			current_Column = max_Depth + max_Number_Of_Elements+2;

			// density min/max
			create_Min_Max_Label	(new_Table,			   current_Row,   current_Column, Rho_Sym+", "+Plus_Minus_Sym+"%");
			create_Min_Max_Button	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Density);
			create_Min_Max_Spin_Box	(new_Table, tab_Index, current_Row+2, current_Column, whats_This_Density);
			current_Column += 2;

			// thickness min/max
			if(has_Layers)
			{
				create_Min_Max_Label	(new_Table,			   current_Row,   current_Column, "z/d, "+Plus_Minus_Sym+"%");
				create_Min_Max_Button	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Thickness);
				create_Min_Max_Spin_Box	(new_Table, tab_Index, current_Row+2, current_Column, whats_This_Thickness);
			}
			current_Column += 2;

			// sigma min/max
			if(has_Boundaries)
			{
				create_Min_Max_Label	(new_Table,			   current_Row,   current_Column, Sigma_Sym+", "+Plus_Minus_Sym+"%");
				create_Min_Max_Button	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Sigma);
				create_Min_Max_Spin_Box	(new_Table, tab_Index, current_Row+2, current_Column, whats_This_Sigma);
			}
		}

		// structure name
		{
			current_Row = new_Table->rowCount()-1;
			rows_List_To_Span.append(current_Row);
			new_Table->setItem(current_Row,0, new QTableWidgetItem(multilayer_Tabs->tabText(tab_Index)));
			new_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);
			new_Table->item   (current_Row,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			new_Table->item   (current_Row,0)->setFont(font_Header);
		}

		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);

		// skip ambient
		++it;

		while (*it)
		{
			structure_Item = *it;

			depth = Global_Variables::get_Item_Depth(structure_Item);
			current_Column = depth-1;
			new_Table->insertRow(new_Table->rowCount());
			new_Table->insertRow(new_Table->rowCount());
			new_Table->insertRow(new_Table->rowCount());

			current_Row = new_Table->rowCount()-3;

			Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

			if( struct_Data.item_Type == item_Type_Ambient  ||
				struct_Data.item_Type == item_Type_Layer    ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				new_Table->insertRow(new_Table->rowCount());
				new_Table->insertRow(new_Table->rowCount());
				new_Table->insertRow(new_Table->rowCount());
			}

			add_Columns(new_Table,depth);

			// print item name
			QTableWidgetItem* table_Item = new QTableWidgetItem(/*Global_Variables::structure_Item_Name(struct_Data)*/);
			new_Table->setItem(current_Row,current_Column, table_Item);
			table_Item->setWhatsThis(struct_Data.item_Type);
			table_Item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			struct_Table_Map.insert(structure_Item, table_Item);

			// make it checkable
			QCheckBox* item_CheckBox = new QCheckBox(Global_Variables::structure_Item_Name(struct_Data));
			new_Table->setCellWidget(current_Row, current_Column, item_CheckBox);
			item_CheckBox->setChecked(struct_Data.item_Enabled);
			check_Boxes_Map.insert(item_CheckBox, structure_Item);
			connect(item_CheckBox, &QCheckBox::toggled, this, [=]
			{
				// enable or disable widgets
				disable_enable_One_Item(new_Table, table_Item);

				// refresh view in main window
				emit_Data_Edited();
			});

			current_Column = max_Depth+1;

			///--------------------------------------------------------------------------------------------
			/// creation
			///--------------------------------------------------------------------------------------------

			// material
			if( struct_Data.item_Type == item_Type_Ambient  ||
				struct_Data.item_Type == item_Type_Layer    ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				if(struct_Data.composed_Material)
				{
					create_Combo_Elements	    (new_Table, tab_Index, current_Row,   current_Column, structure_Item);
					create_Stoich_Line_Edit	    (new_Table, tab_Index, current_Row+1, current_Column, structure_Item, VAL);
					create_Stoich_Line_Edit	    (new_Table, tab_Index, current_Row+3, current_Column, structure_Item, MIN);
					create_Stoich_Line_Edit	    (new_Table, tab_Index, current_Row+4, current_Column, structure_Item, MAX);

					// it should be created last
					create_Stoich_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, 1, 2, 0, 0);
				}
				else
				{
					create_Material_Line_Edit	(new_Table, tab_Index, current_Row,   current_Column, structure_Item);
					create_Browse_Button	    (new_Table,	current_Row+1, current_Column, current_Row, current_Column);
				}
			}
			current_Column += (max_Number_Of_Elements+1);
			///--------------------------------------------------------------------------------------------

			// multilayer
			if(struct_Data.item_Type == item_Type_Multilayer)
			{
				// num repetitions
				QString whats_This = whats_This_Num_Repetitions;
				add_Columns		(new_Table, current_Column+5);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "N");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				current_Column += 2;

				// period
				whats_This = whats_This_Period;
				add_Columns		(new_Table, current_Column+5);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column,   structure_Item, whats_This, "d ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column,   structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column+2, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column+3, structure_Item, whats_This, MAX);
				// lastcolumn
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+1, current_Column+1, structure_Item, whats_This, 0, 0, 1, 2);
				current_Column += 5;

				// gamma
				if(structure_Item->childCount()==2)
				{
					whats_This = whats_This_Gamma;
					add_Columns		(new_Table, current_Column+5);
					create_Label		(new_Table, tab_Index, current_Row,   current_Column,   structure_Item, whats_This, Gamma_Sym);
					create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column,   structure_Item, whats_This, VAL);
					create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column+2, structure_Item, whats_This, MIN);
					create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column+3, structure_Item, whats_This, MAX);
					// last
					create_Check_Box_Fit(new_Table, tab_Index, current_Row+1, current_Column+1, structure_Item, whats_This, 0, 0, 1, 2);
					current_Column += 5;
				}
			}
			///--------------------------------------------------------------------------------------------

			// density
			if( struct_Data.item_Type == item_Type_Ambient  ||
				struct_Data.item_Type == item_Type_Layer    ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				QString whats_This;
				add_Columns(new_Table, current_Column+1);
				if(struct_Data.composed_Material)
				{
					whats_This = whats_This_Absolute_Density;
					create_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Rho_Sym+" ["+density_units+"]");
				} else
				{
					whats_This = whats_This_Relative_Density;
					create_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Rho_Sym+" [r.u.]");
				}
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
			}
			current_Column += 2;
			///--------------------------------------------------------------------------------------------

			// thickness
			if(struct_Data.item_Type == item_Type_Layer)
			{
				QString whats_This = whats_This_Thickness;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "z ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
			}
			current_Column += 2;
			///--------------------------------------------------------------------------------------------

			// common sigma
			if( struct_Data.item_Type == item_Type_Layer    ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				QString whats_This = whats_This_Sigma;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit	  (new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				// second
				create_Check_Box_Label(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Sigma_Sym+" ["+length_units+"]", 1, 0, 0, 0);
				create_Line_Edit	  (new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	  (new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit  (new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
			}
			current_Column += 2;
			///--------------------------------------------------------------------------------------------

			// interlayers: weights and sigmas
			if( struct_Data.item_Type == item_Type_Layer    ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				add_Columns(new_Table, current_Column+transition_Layer_Functions_Size);

				// weights
				create_Weigts_Interlayer		(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, VAL);
				create_Check_Box_Label_Interlayer	(new_Table, tab_Index, current_Row,   current_Column, structure_Item);
				create_Weights_Check_Box_Fit_Interlayer (new_Table, tab_Index, current_Row+2, current_Column, structure_Item);

				// sigmas
				create_MySigma_Labels_Interlayer	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item);
				create_MySigma_Line_Edits_Interlayer	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item);
			}
			current_Column += (1+transition_Layer_Functions_Size);
			///--------------------------------------------------------------------------------------------
			/// DRIFTS
			///--------------------------------------------------------------------------------------------

			// thickness linear drift
			if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
			{
				QString whats_This = whats_This_Thickness_Drift_Line_Value;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				// second
				create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This,"dz lin", 1, 1, 0, 0);
				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// thickness random drift
			if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
			{
				QString whats_This = whats_This_Thickness_Drift_Rand_Rms;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				// second
				create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This,"dz ran", 1, 1, 0, 0);
				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// thickness sine drift
			if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
			{
				add_Columns(new_Table, current_Column+3);

				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column,   structure_Item, whats_This_Thickness_Drift_Sine_Amplitude, VAL);
				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column+1, structure_Item, whats_This_Thickness_Drift_Sine_Frequency, VAL);
				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column+2, structure_Item, whats_This_Thickness_Drift_Sine_Phase,	   VAL);

				// second
				create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column,   structure_Item, whats_This_Thickness_Drift_Sine,"dz sin : A*sin(2"+Pi_Sym+"n"+Nu_Sym+"+"+Phi_Sym+")", 1, 1, 0, 2);

				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column,   structure_Item, whats_This_Thickness_Drift_Sine_Amplitude, MIN);
				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column+1, structure_Item, whats_This_Thickness_Drift_Sine_Frequency, MIN);
				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column+2, structure_Item, whats_This_Thickness_Drift_Sine_Phase,	   MIN);

				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column,	structure_Item, whats_This_Thickness_Drift_Sine_Amplitude, MAX);
				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column+1, structure_Item, whats_This_Thickness_Drift_Sine_Frequency, MAX);
				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column+2, structure_Item, whats_This_Thickness_Drift_Sine_Phase,	   MAX);

				// last
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column,	structure_Item, whats_This_Thickness_Drift_Sine_Amplitude, 1, 2, 0, 0);
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column+1, structure_Item, whats_This_Thickness_Drift_Sine_Frequency, 1, 2, 0, 0);
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column+2, structure_Item, whats_This_Thickness_Drift_Sine_Phase,	   1, 2, 0, 0);

				new_Table->setSpan(current_Row,current_Column,1,3);
			}
			current_Column += 4;
			///--------------------------------------------------------------------------------------------

			// sigma linear drift
			if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
			{
				QString whats_This = whats_This_Sigma_Drift_Line_Value;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				// second
				create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "d"+Sigma_Sym+" lin", 1, 1, 0, 0);
				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// sigma random drift
			if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
			{
				QString whats_This = whats_This_Sigma_Drift_Rand_Rms;
				add_Columns(new_Table, current_Column+1);
				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				// second
				create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "d"+Sigma_Sym+" ran", 1, 1, 0, 0);
				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// sigma sine drift
			if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
			{
				add_Columns(new_Table, current_Column+3);

				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column,   structure_Item, whats_This_Sigma_Drift_Sine_Amplitude, VAL);
				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column+1, structure_Item, whats_This_Sigma_Drift_Sine_Frequency, VAL);
				create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column+2, structure_Item, whats_This_Sigma_Drift_Sine_Phase,	   VAL);

				// second
				create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column,   structure_Item, whats_This_Sigma_Drift_Sine,"d"+Sigma_Sym+" sin : A*sin(2"+Pi_Sym+"n"+Nu_Sym+"+"+Phi_Sym+")", 1, 1, 0, 2);

				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column,   structure_Item, whats_This_Sigma_Drift_Sine_Amplitude, MIN);
				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column+1, structure_Item, whats_This_Sigma_Drift_Sine_Frequency, MIN);
				create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column+2, structure_Item, whats_This_Sigma_Drift_Sine_Phase,	   MIN);

				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column,	structure_Item, whats_This_Sigma_Drift_Sine_Amplitude, MAX);
				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column+1, structure_Item, whats_This_Sigma_Drift_Sine_Frequency, MAX);
				create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column+2, structure_Item, whats_This_Sigma_Drift_Sine_Phase,	   MAX);

				// last
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column,	structure_Item, whats_This_Sigma_Drift_Sine_Amplitude, 1, 2, 0, 0);
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column+1, structure_Item, whats_This_Sigma_Drift_Sine_Frequency, 1, 2, 0, 0);
				create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column+2, structure_Item, whats_This_Sigma_Drift_Sine_Phase,	   1, 2, 0, 0);

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
}

void Table_Of_Structures::read_Trees()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Structure_Tree* old_Structure_Tree = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i))->structure_Tree;
		list_Of_Trees.append(old_Structure_Tree);
	}
}

void Table_Of_Structures::refresh_Reload_Colorize(QString refresh_Reload, QWidget* back_Widget, Parameter* parameter)
{
	// PARAMETER

	if(!back_Widget)
	{
		QMessageBox::critical(nullptr, "Table_Of_Structures::refresh_Reload_Colorize", "null back_Widget");
		return;
	}

	// this function saves parameter to and load parameter from structure tree

	id_Type id = coupled_Back_Widget_and_Id.value(back_Widget);
	QTreeWidgetItem* struct_item = coupled_Back_Widget_and_Struct_Item.value(back_Widget);
	Data struct_Data = struct_item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(refresh_Reload == reload_Property)
	{
		parameter = Global_Variables::get_Parameter_From_Struct_Item_by_Id(struct_Data, id);
	}
	if(refresh_Reload == refresh_Property)
	{
		if( id != parameter->indicator.id )
		{
			QMessageBox::critical(this, "Table_Of_Structures::refresh_Reload_Colorize", "id != parameter.indicator.id");
			exit(EXIT_FAILURE);
		}

		Parameter* struct_Parameter_Pointer = Global_Variables::get_Parameter_From_Struct_Item_by_Id(struct_Data, id);

		*struct_Parameter_Pointer = *parameter;

		// refresh tree
		QVariant var;
		var.setValue(struct_Data);
		struct_item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}

	/// colorize dependence
	{
		// has no dependencies
		if(!parameter->coupled.master.exist && parameter->coupled.slaves.size()==0)
		{
			if(back_Widget->property(fit_Text).toString() == fit_Text)
				back_Widget->setStyleSheet("background-color: white");
			else
				back_Widget->setStyleSheet("background-color: lightblue");
		}

		// has master only
		if(parameter->coupled.master.exist && parameter->coupled.slaves.size()==0)
			back_Widget->setStyleSheet("QWidget { background: rgb(255, 50, 50); }");

		// has slaves only
		if(!parameter->coupled.master.exist && parameter->coupled.slaves.size()>0)
			back_Widget->setStyleSheet("QWidget { background: rgb(50, 255, 50); }");

		// has both
		if(parameter->coupled.master.exist && parameter->coupled.slaves.size()>0)
			back_Widget->setStyleSheet("QWidget { background: rgb(255, 255, 50); }");

		if(parameter->confidence.calc_Conf_Interval)
		{
			back_Widget->setStyleSheet("background-color: violet");
		}
	}
}

void Table_Of_Structures::add_Columns(My_Table_Widget* table, int add_After)
{
	while(add_After>=table->columnCount())
	{
		table->insertColumn(table->columnCount());
	}
}

Parameter& Table_Of_Structures::get_Parameter(Data& struct_Data, QString whats_This, int& precision, double& coeff)
{
	// PARAMETER

	// measurement
	if(whats_This == whats_This_Angle)						{precision = line_edit_angle_precision;		coeff = 1;						return struct_Data.probe_Angle;					}
	if(whats_This == whats_This_Angular_Resolution)			{precision = line_edit_angle_precision;		coeff = 1;						return struct_Data.angular_Resolution;			}
	if(whats_This == whats_This_Wavelength)					{precision = line_edit_wavelength_precision;coeff = 1;						return struct_Data.wavelength;					}
	if(whats_This == whats_This_Spectral_Resolution)		{precision = line_edit_wavelength_precision;coeff = 1;						return struct_Data.spectral_Resolution;			}
	if(whats_This == whats_This_Polarization)				{precision = 3;								coeff = 1;						return struct_Data.polarization;				}
	if(whats_This == whats_This_Polarization_Sensitivity)	{precision = 3;								coeff = 1;						return struct_Data.polarization_Sensitivity;	}
	if(whats_This == whats_This_Background)					{precision = line_edit_background_precision;coeff = 1;						return struct_Data.background;					}

	if(whats_This == whats_This_Beam_Size)					{precision = line_edit_beam_size_precision;	coeff = 1;						return struct_Data.beam_Size;					}
	if(whats_This == whats_This_Beam_Profile_Spreading)		{precision = line_edit_beam_size_precision;	coeff = 1;						return struct_Data.beam_Profile_Spreading;		}
	if(whats_This == whats_This_Sample_Size)				{precision = line_edit_sample_size_precision;coeff = 1;						return struct_Data.sample_Size;					}
	if(whats_This == whats_This_Sample_Shift)				{precision = line_edit_sample_size_precision;coeff = 1;						return struct_Data.sample_Shift;				}

	// optical properties
	if(whats_This == whats_This_Absolute_Density)			{precision = line_edit_density_precision;	coeff = 1;						return struct_Data.absolute_Density;			}
	if(whats_This == whats_This_Relative_Density)			{precision = line_edit_density_precision;	coeff = 1;						return struct_Data.relative_Density;			}
	if(whats_This == whats_This_Permittivity)				{precision = line_edit_density_precision;	coeff = 1;						return struct_Data.permittivity;				}
	if(whats_This == whats_This_Absorption)					{precision = line_edit_density_precision;	coeff = 1;						return struct_Data.absorption;					}

	// thickness
	if(whats_This == whats_This_Thickness)						{precision = line_edit_thickness_precision;	coeff = length_Coefficients_Map.value(length_units);	return struct_Data.thickness;				}
	if(whats_This == whats_This_Thickness_Drift_Line_Value)		{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Line_Value;	}
	if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Rand_Rms;	}
	if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Sine_Amplitude;}
	if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Sine_Frequency;}
	if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Sine_Phase;	}

	// interface
	if(whats_This == whats_This_Sigma)						{precision = line_edit_sigma_precision;		coeff = length_Coefficients_Map.value(length_units);	return struct_Data.sigma;				}
	if(whats_This == whats_This_Sigma_Drift_Line_Value)		{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Line_Value;	}
	if(whats_This == whats_This_Sigma_Drift_Rand_Rms)		{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Rand_Rms;		}
	if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)	{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Sine_Amplitude;	}
	if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)	{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Sine_Frequency;	}
	if(whats_This == whats_This_Sigma_Drift_Sine_Phase)		{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Sine_Phase;	}

	// multilayer
	if(whats_This == whats_This_Period)						{precision = line_edit_period_precision;	coeff = length_Coefficients_Map.value(length_units);	return struct_Data.period;				}
	if(whats_This == whats_This_Gamma)						{precision = line_edit_gamma_precision;		coeff = 1;						return struct_Data.gamma;				}

	//	qInfo() << "get_Parameter cant find parameter " << whats_This <<"";
	Parameter* parameter = new Parameter;
	parameter->indicator.id=0;
	precision = 1; coeff = 1;

	return *parameter;
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
				QString item_Type = table->item(row_Index,col_Index)->whatsThis();
				if( item_Type == item_Type_Ambient  ||
					item_Type == item_Type_Layer    ||
					item_Type == item_Type_Substrate )
				{
					int rows = 5;
					table->setSpan(row_Index,col_Index,rows,1);
				}
				if( item_Type == item_Type_Multilayer )
				{
					int rows = 2;
					table->setSpan(row_Index,col_Index,rows,1);
				}
			}
		}
	}
}

void Table_Of_Structures::disable_enable_Structure_Items(My_Table_Widget* table)
{
	for(int row_Index=0; row_Index<table->rowCount(); ++row_Index)
	{
		for(int col_Index=0; col_Index<table->columnCount(); ++col_Index)
		{
			QTableWidgetItem* table_Item = table->item(row_Index,col_Index);
			if(table_Item)
			{
				disable_enable_One_Item(table, table_Item);
			}
		}
	}
}

void Table_Of_Structures::disable_enable_One_Item(My_Table_Widget* table, QTableWidgetItem* table_Item)
{
	int row_Index = table_Item->row();
	int col_Index = table_Item->column();

	QTreeWidgetItem* struct_Item = struct_Table_Map.key(table_Item);
	if(struct_Item)
	{
		// parental data
		QTreeWidgetItem* parent = struct_Item->parent();
		if(parent)
		{
			Data parent_Struct_Data = parent->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			QTableWidgetItem* parent_Table_Item = struct_Table_Map.value(parent);
			QWidget* parent_Widget = table->cellWidget(parent_Table_Item->row(), parent_Table_Item->column());

			bool enabled = true;
			// if parent inactive or manually disabled
			if(!parent_Struct_Data.item_Enabled || !parent_Widget->isEnabled())
				enabled = false;

			if(enabled)
			{
				disable_enable_One_Item_Content(table, table_Item, true);
			} else
			{
				disable_enable_One_Item_Content(table, table_Item, false, false);
			}
		} else
		{
			disable_enable_One_Item_Content(table, table_Item, true);
		}
	}

	QString item_Type = table_Item->whatsThis();
	if( item_Type == item_Type_Multilayer || item_Type == item_Type_Aperiodic)
	{
		QCheckBox* check_Box = qobject_cast<QCheckBox*>(table->cellWidget(row_Index, col_Index));
		QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
		disable_enable_Multilayers(table, structure_Item);
	}
	reload_All_Widgets();
}

void Table_Of_Structures::disable_enable_One_Item_Content(My_Table_Widget* table, QTableWidgetItem* table_Item, bool save, bool enable)
{
	int row_Index = table_Item->row();
	int col_Index = table_Item->column();

	QString item_Type = table_Item->whatsThis();
	if( item_Type == item_Type_Layer ||	item_Type == item_Type_Substrate || item_Type == item_Type_Multilayer || item_Type == item_Type_Aperiodic)
	{
		QCheckBox* check_Box = qobject_cast<QCheckBox*>(table->cellWidget(row_Index, col_Index));
		QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
		Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		if(save)
		{
			struct_Data.item_Enabled = check_Box->isChecked();
			QVariant var;
			var.setValue( struct_Data );
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		} else
		{
			struct_Data.item_Enabled = enable;
		}

		int rows = 0;
		if( item_Type == item_Type_Layer    ||	item_Type == item_Type_Substrate )	rows = 5;
		if( item_Type == item_Type_Multilayer )										rows = 2;

		for(int disable_Row_Index=row_Index  ; disable_Row_Index<row_Index+rows      ; ++disable_Row_Index)
		for(int disable_Col_Index=col_Index+1; disable_Col_Index<table->columnCount(); ++disable_Col_Index)
		{
			QWidget* widget = table->cellWidget(disable_Row_Index, disable_Col_Index);
			if(widget)
			{
				QLineEdit* line_Edit = qobject_cast<QLineEdit*>(widget);

				if(!struct_Data.item_Enabled)
				{
					if(line_Edit) line_Edit->setStyleSheet("border: 0px solid gray");
					widget->setDisabled(true);
					widget->setProperty(enabled_Property, false);
				} else
				{
					if(line_Edit) line_Edit->setStyleSheet("border: 1px solid gray");
					widget->setDisabled(false);
					widget->setProperty(enabled_Property, true);
				}
			}
		}
	}
}

void Table_Of_Structures::disable_enable_Multilayers(My_Table_Widget* table, QTreeWidgetItem* parent)
{
	// parental data
	Data parent_Struct_Data = parent->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QTableWidgetItem* parent_Table_Item = struct_Table_Map.value(parent);
	QWidget* parent_Widget = table->cellWidget(parent_Table_Item->row(), parent_Table_Item->column());

	bool enabled = true;
	// if inactive or manually disabled
	if(!parent_Struct_Data.item_Enabled || !parent_Widget->isEnabled())
		enabled = false;

	// children's data
	for(int child_Index=0; child_Index<parent->childCount(); ++child_Index)
	{
		QTreeWidgetItem* child_Struct_Item = parent->child(child_Index);
		Data child_Struct_Data = child_Struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		QTableWidgetItem* child_Table_Item = struct_Table_Map.value(child_Struct_Item);

		QWidget* widget = table->cellWidget(child_Table_Item->row(), child_Table_Item->column());
		widget->setDisabled(!enabled);

		bool child_Enabled = enabled && child_Struct_Data.item_Enabled;
		disable_enable_One_Item_Content(table, child_Table_Item, false, child_Enabled);

		// go deeper
		disable_enable_Multilayers(table, child_Struct_Item);
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

//// creation

//// for material only
void Table_Of_Structures::create_Combo_Elements(My_Table_Widget* table, int, int current_Row, int start_Column, QTreeWidgetItem* structure_Item)
{
	QList<Stoichiometry> composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().composition;

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

		// storage
		elements_Map.insert(elements, structure_Item);

		// add widget to table
		table->setCellWidget(current_Row, current_Column, elements);

		connect(elements, &QComboBox::currentTextChanged, this, &Table_Of_Structures::refresh_Element);

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_Stoich_Line_Edit(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString val_Type)
{
	QList<Stoichiometry> composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().composition;

	int current_Column = start_Column;
	double value = -2017;
	char format = line_edit_short_double_format;

	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		Parameter& comp = composition[composition_Index].composition;
		if(val_Type == VAL)	{value = comp.value;		format = line_edit_short_double_format;}
		if(val_Type == MIN)	{value = comp.fit.min;	format = line_edit_short_double_format;}
		if(val_Type == MAX)	{value = comp.fit.max;	format = line_edit_short_double_format;}

		// create lineedit
		QLineEdit* line_Edit = new QLineEdit(QString::number(value, format, line_edit_composition_precision));
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);

		// number of element is here
		line_Edit->setProperty(num_Chemic_Element_Property, composition_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setProperty(value_Type_Property, val_Type);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		// for reloading
		line_Edit->setProperty(reload_Property, false);
		line_Edit->setProperty(tab_Index_Property,tab_Index);

		// storage
		line_Edits_Map.		       insert      (line_Edit, structure_Item);
		reload_Show_Dependence_Map.insertMulti (line_Edit, comp.indicator.id);
		all_Widgets_To_Reload[tab_Index].append(line_Edit);

		// add widget to table
		table->setCellWidget(current_Row, current_Column, line_Edit);

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit(table); });
		connect(line_Edit, &QLineEdit::textEdited, this, &Table_Of_Structures::refresh_Stoich);

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

QString Table_Of_Structures::material_From_Composition(const QList<Stoichiometry>& composition)
{
	QString text;
	for(int i=0; i<composition.size(); ++i)
	{
		text += composition[i].type;
		if( abs(composition[i].composition.value - 1) > DBL_EPSILON )
		{
			text += QString::number(composition[i].composition.value, line_edit_short_double_format, thumbnail_composition_precision);
		}
	}
	return text;
}

void Table_Of_Structures::create_Stoich_Check_Box_Fit(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, int r_S, int r_F, int c_S, int c_F)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QList<Stoichiometry>& composition = struct_Data.composition;

	int current_Column = start_Column;

	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		Parameter& comp = composition[composition_Index].composition;

		// update tab_Index and full_Name
		comp.indicator.tab_Index = tab_Index;
		comp.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Composition, composition_Index);
		QVariant var;
		var.setValue(struct_Data);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// create check_Box
		QCheckBox* check_Box = new QCheckBox(fit_Text);

		// alignment
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		// number of element is here
		check_Box->setProperty(num_Chemic_Element_Property, composition_Index);
		check_Box->setProperty(row_Property, current_Row);
		check_Box->setProperty(column_Property, current_Column);

		// which cells disable and enable
		check_Box->setProperty(relative_Rows_To_Disable_Start_Property, r_S);
		check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, r_F);
		check_Box->setProperty(relative_Columns_To_Disable_Start_Property, c_S);
		check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, c_F);

		// for reloading
		check_Box->setProperty(reload_Property, false);
		check_Box->setProperty(tab_Index_Property, tab_Index);

		// storage
		check_Boxes_Map.		   insert      (check_Box, structure_Item);
		reload_Show_Dependence_Map.insertMulti (check_Box, comp.indicator.id);
		all_Widgets_To_Reload[tab_Index].append(check_Box);

		// set up BACK widget
		{
			back_Widget->setProperty(coupling_Editor_Property, true);	// for opening Coupling_Editor
			back_Widget->setProperty(fit_Text, fit_Text);				// for coloring
			all_Widgets_To_Reload[tab_Index].append(back_Widget);

			coupled_Back_Widget_and_Struct_Item.insert(back_Widget, structure_Item);
			coupled_Back_Widget_and_Id.			insert(back_Widget, comp.indicator.id);

			// colorize
			refresh_Reload_Colorize(colorize_Property, back_Widget, &comp);
		}

		// add widget to table
		table->setCellWidget(current_Row, current_Column, back_Widget);

		connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Fit_Element);
		connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off(table); });

		check_Box->setChecked(comp.fit.is_Fitable);

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;

		// disable if 1 element
		if(composition.size()==1)
		{
			check_Box->setProperty(relative_Rows_To_Disable_Start_Property, -1);
			check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, 2);
			check_Box->setProperty(relative_Columns_To_Disable_Start_Property, 0);
			check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, 0);

			check_Box->setChecked(false);
		}
	}
}

void Table_Of_Structures::create_Material_Line_Edit(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item)
{
	QString material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().material;

	add_Columns(table, current_Column+1);

	QLineEdit* material_Line_Edit = new QLineEdit(material);
	material_Line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
	material_Line_Edit->setProperty(min_Size_Property, material_Line_Edit->width());
	material_Line_Edit->setProperty(column_Property, current_Column);

	// storage
	line_Edits_Map.insert(material_Line_Edit, structure_Item);
	all_Widgets_To_Reload[tab_Index].append(material_Line_Edit);

	// add widget to table
	table->setCellWidget(current_Row, current_Column, material_Line_Edit);

	connect(material_Line_Edit, &QLineEdit::textEdited,	     this, [=]{resize_Line_Edit(table); });
	connect(material_Line_Edit, &QLineEdit::textEdited,	     this, &Table_Of_Structures::refresh_Material);
	connect(material_Line_Edit, &QLineEdit::editingFinished, this, &Table_Of_Structures::check_Material  );
}

void Table_Of_Structures::create_Browse_Button(My_Table_Widget* table, int current_Row, int start_Column, int material_LineEdit_Row, int material_LineEdit_Column)
{
	QPushButton* browse_Button = new QPushButton("Browse...");
	table->setCellWidget(current_Row, start_Column, browse_Button);

	QLineEdit* material_Line_Edit = qobject_cast<QLineEdit*>(table->cellWidget(material_LineEdit_Row,material_LineEdit_Column));
	connect(browse_Button, &QPushButton::clicked, this, [=]{browse_Material(material_Line_Edit);});
}

//// for several parameters
void Table_Of_Structures::create_Label(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString text)
{
	QLabel* label = new QLabel(text);
	label->setAlignment(Qt::AlignCenter);

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& parameter = get_Parameter(struct_Data, whats_This);

	// update tab_Index and full_Name
	parameter.indicator.tab_Index = tab_Index;
	parameter.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This);
	QVariant var;
	var.setValue(struct_Data);
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	// for reloading
	label->setProperty(reload_Property, false);
	label->setProperty(tab_Index_Property, tab_Index);
	label->setProperty(item_Type_Property, struct_Data.item_Type);
	label->setProperty(whats_This_Property, whats_This);

	// storage
	reload_Show_Dependence_Map.insertMulti(label, parameter.indicator.id);
	all_Widgets_To_Reload[tab_Index].append(label);

	// set up BACK widget
	if(parameter.indicator.id!=0)
	{
		label->setProperty(coupling_Editor_Property, true);			// for opening Coupling_Editor

		coupled_Back_Widget_and_Struct_Item.insert(label, structure_Item);
		coupled_Back_Widget_and_Id.			insert(label, parameter.indicator.id);

		// colorize
		refresh_Reload_Colorize(colorize_Property, label, &parameter);
	} else
	{
		label->setStyleSheet("background-color: lightblue");
	}

	// add widget to table
	table->setCellWidget(current_Row, current_Column, label);

	connect(label, &QLabel::windowTitleChanged, this, &Table_Of_Structures::refresh_Header);
}

void Table_Of_Structures::create_Check_Box_Label(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString text, int r_S, int r_F, int c_S, int c_F)
{
	// PARAMETER

	bool bool_Check = false;
	id_Type id = 0;
	QCheckBox* check_Box = new QCheckBox(text);

	// update tab_Index and full_Name
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& parameter = get_Parameter(struct_Data, whats_This);
	parameter.indicator.tab_Index = tab_Index;
	parameter.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This);
	QVariant var;
	var.setValue(struct_Data);
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	// special cases
	{
		if(whats_This == whats_This_Thickness_Drift_Line_Value)
		{
			bool_Check = struct_Data.thickness_Drift.is_Drift_Line;
			id = struct_Data.thickness_Drift.drift_Line_Value.indicator.id;
		}
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)
		{
			bool_Check = struct_Data.thickness_Drift.is_Drift_Rand;
			id = struct_Data.thickness_Drift.drift_Rand_Rms.indicator.id;
		}
		if(whats_This == whats_This_Thickness_Drift_Sine)
		{
			bool_Check = struct_Data.thickness_Drift.is_Drift_Sine;
			id = struct_Data.thickness_Drift.drift_Sine_Amplitude.indicator.id;
			id_Type id_2 = struct_Data.thickness_Drift.drift_Sine_Frequency.indicator.id;
			id_Type id_3 = struct_Data.thickness_Drift.drift_Sine_Phase.indicator.id;
			reload_Show_Dependence_Map.insertMulti(check_Box, id_2);
			reload_Show_Dependence_Map.insertMulti(check_Box, id_3);
		}

		if(whats_This == whats_This_Sigma_Drift_Line_Value)
		{
			bool_Check = struct_Data.sigma_Drift.is_Drift_Line;
			id = struct_Data.sigma_Drift.drift_Line_Value.indicator.id;
		}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)
		{
			bool_Check = struct_Data.sigma_Drift.is_Drift_Rand;
			id = struct_Data.sigma_Drift.drift_Rand_Rms.indicator.id;
		}
		if(whats_This == whats_This_Sigma_Drift_Sine)
		{
			bool_Check = struct_Data.sigma_Drift.is_Drift_Sine;
			id = struct_Data.sigma_Drift.drift_Sine_Amplitude.indicator.id;
			id_Type id_2 = struct_Data.sigma_Drift.drift_Sine_Frequency.indicator.id;
			id_Type id_3 = struct_Data.sigma_Drift.drift_Sine_Phase.indicator.id;
			reload_Show_Dependence_Map.insertMulti(check_Box, id_2);
			reload_Show_Dependence_Map.insertMulti(check_Box, id_3);
		}

		if(whats_This == whats_This_Sigma)
		{
			bool_Check = struct_Data.common_Sigma;
			id = struct_Data.sigma.indicator.id;
			for(Interlayer& interlayer : struct_Data.interlayer_Composition)
			{
				reload_Show_Dependence_Map.insertMulti(check_Box, interlayer.my_Sigma.indicator.id);
			}
		}
	}

	// enable/disable function
	check_Box->setProperty(row_Property, current_Row);
	check_Box->setProperty(column_Property, current_Column);
	check_Box->setProperty(relative_Rows_To_Disable_Start_Property, r_S);
	check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, r_F);
	check_Box->setProperty(relative_Columns_To_Disable_Start_Property, c_S);
	check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, c_F);
	check_Box->setProperty(whats_This_Property, whats_This);

	// for reloading
	check_Box->setProperty(reload_Property, false);
	check_Box->setProperty(tab_Index_Property, tab_Index);

	// storage
	check_Boxes_Map.	       insert	   (check_Box, structure_Item);
	reload_Show_Dependence_Map.insertMulti (check_Box, id);
	all_Widgets_To_Reload[tab_Index].append(check_Box);

	check_Box->setChecked(bool_Check);

	// alignment
	QWidget* back_Widget = new QWidget;
	QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
	back_Layout->addWidget(check_Box);
	back_Layout->setSpacing(0);
	back_Layout->setContentsMargins(0,0,0,0);
	back_Layout->setAlignment(Qt::AlignCenter);

	// set up BACK widget
	if(parameter.indicator.id!=0)
	{
		back_Widget->setProperty(coupling_Editor_Property, true);		// for opening Coupling_Editor
		all_Widgets_To_Reload[tab_Index].append(back_Widget);

		coupled_Back_Widget_and_Struct_Item.insert(back_Widget, structure_Item);
		coupled_Back_Widget_and_Id.			insert(back_Widget, parameter.indicator.id);

		// colorize
		refresh_Reload_Colorize(colorize_Property, back_Widget, &parameter);
	} else
	{
		back_Widget->setStyleSheet("background-color: lightblue");
	}

	connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off(table); });
	connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Check_Box_Header);

	table->setCellWidget(current_Row, current_Column, back_Widget);
}

//// for all parameters
void Table_Of_Structures::create_Line_Edit(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString val_Type)
{
	// PARAMETER

	int precision = 4;
	char format = line_edit_short_double_format;
	double value = -2017;
	double coeff = 1;
	id_Type id = 0;

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QString text_Value = "-2017";
	QValidator* validator = nullptr;

	// create lineedit
	QLineEdit* line_Edit = new QLineEdit;

	if(whats_This == whats_This_Num_Repetitions)
	{
		value = struct_Data.num_Repetition.value;
		text_Value = QString::number(value);
		validator = new QIntValidator(0, MAX_INTEGER, this);
		id = struct_Data.num_Repetition.id;
		reload_Show_Dependence_Map.insertMulti(line_Edit, id_Of_Thicknesses);
	} else
	{
		Parameter& parameter = get_Parameter(struct_Data, whats_This, precision, coeff);
		if(val_Type == VAL)	{value = parameter.value;	format = line_edit_double_format;	}
		if(val_Type == MIN)	{value = parameter.fit.min;	format = line_edit_short_double_format;	}
		if(val_Type == MAX)	{value = parameter.fit.max;	format = line_edit_short_double_format;	}

		text_Value = QString::number(value/coeff, format, precision);
		if(	whats_This == whats_This_Thickness_Drift_Line_Value ||
			whats_This == whats_This_Sigma_Drift_Line_Value	)
		{
			validator = new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this);
		} else
		{
			validator = new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this);
		}
		id = parameter.indicator.id;

		if( whats_This == whats_This_Thickness	||
			whats_This == whats_This_Period		||
			whats_This == whats_This_Gamma		)
		{
			reload_Show_Dependence_Map.insertMulti(line_Edit, id_Of_Thicknesses);
		}

		if( whats_This == whats_This_Sigma )
		{
			for(Interlayer& interlayer : struct_Data.interlayer_Composition)
			{
				reload_Show_Dependence_Map.insertMulti(line_Edit, interlayer.my_Sigma.indicator.id);
			}
		}
	}

#ifdef _WIN32
	QFont font(line_Edit->font());
	font.setPointSize(8.25);
	font.setFamily("MS Shell Dlg 2");
	line_Edit->setFont(font);
#endif
#ifdef __linux__
#endif

	line_Edit->setText(text_Value);
	line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
	line_Edit->setValidator(validator);

	line_Edit->setProperty(min_Size_Property, line_Edit->width());
	line_Edit->setProperty(column_Property, current_Column);
	line_Edit->setProperty(whats_This_Property, whats_This);
	line_Edit->setProperty(value_Type_Property, val_Type);

	// for reloading
	line_Edit->setProperty(reload_Property, false);
	line_Edit->setProperty(tab_Index_Property, tab_Index);

	// storage
	line_Edits_Map.			   insert	   (line_Edit, structure_Item);
	reload_Show_Dependence_Map.insertMulti (line_Edit, id);
	all_Widgets_To_Reload[tab_Index].append(line_Edit);

	// storage for fast min/max refreshing
	if( val_Type == MIN || val_Type == MAX )
	{
		if( whats_This == whats_This_Absolute_Density || whats_This == whats_This_Relative_Density )
		{
			min_Max_Density_Line_Edits_List[tab_Index].append(line_Edit);
		}
		if( whats_This == whats_This_Thickness || whats_This == whats_This_Period )
		{
			min_Max_Thickness_Line_Edits_List[tab_Index].append(line_Edit);
		}
		if( whats_This == whats_This_Sigma )
		{
			min_Max_Sigma_Line_Edits_List[tab_Index].append(line_Edit);
		}
	}

	// create item (set LineEdits_Map)
	table->setCellWidget(current_Row, current_Column, line_Edit);

	connect(line_Edit, &QLineEdit::textEdited,	    this, [=]{resize_Line_Edit (table); });
	connect(line_Edit, &QLineEdit::textEdited,	    this, [=]{refresh_Parameter(table); });
	connect(line_Edit, &QLineEdit::editingFinished, this, [=]{refresh_Parameter(table); });
}

void Table_Of_Structures::create_Check_Box_Fit(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, int r_S, int r_F, int c_S, int c_F)
{
	// PARAMETER

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& parameter = get_Parameter(struct_Data, whats_This);

	// update tab_Index and full_Name
	parameter.indicator.tab_Index = tab_Index;
	parameter.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This);
	QVariant var;
	var.setValue(struct_Data);
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	// create check_Box
	QCheckBox* check_Box = new QCheckBox(fit_Text);

	// alignment
	QWidget* back_Widget = new QWidget;
	QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
	back_Layout->addWidget(check_Box);
	back_Layout->setSpacing(0);
	back_Layout->setContentsMargins(0,0,0,0);
	back_Layout->setAlignment(Qt::AlignCenter);

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
	check_Box->setProperty(tab_Index_Property, tab_Index);

	// storage
	check_Boxes_Map.		   insert	   (check_Box, structure_Item);
	reload_Show_Dependence_Map.insertMulti (check_Box, parameter.indicator.id);
	all_Widgets_To_Reload[tab_Index].append(check_Box);

	// set up BACK widget
	if(	whats_This == whats_This_Thickness_Drift_Sine_Amplitude ||
		whats_This == whats_This_Thickness_Drift_Sine_Frequency ||
		whats_This == whats_This_Thickness_Drift_Sine_Phase		||

		whats_This == whats_This_Sigma_Drift_Sine_Amplitude		||
		whats_This == whats_This_Sigma_Drift_Sine_Frequency		||
		whats_This == whats_This_Sigma_Drift_Sine_Phase			)
	{
		back_Widget->setProperty(coupling_Editor_Property, true);		// for opening Coupling_Editor
		back_Widget->setProperty(fit_Text, fit_Text);					// for coloring
		all_Widgets_To_Reload[tab_Index].append(back_Widget);

		coupled_Back_Widget_and_Struct_Item.insert(back_Widget, structure_Item);
		coupled_Back_Widget_and_Id.			insert(back_Widget, parameter.indicator.id);

		// colorize
		refresh_Reload_Colorize(colorize_Property, back_Widget, &parameter);
	}

	// create item
	table->setCellWidget(current_Row, current_Column, back_Widget);

	connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Fit_Parameter);
	connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off(table);});

	check_Box->setChecked(parameter.fit.is_Fitable);
}

//// for interlayers
void Table_Of_Structures::create_Check_Box_Label_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;
	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		Interlayer& inter_Comp = interlayer_Composition[interlayer_Index];

		// update tab_Index and full_Name
		inter_Comp.interlayer.indicator.tab_Index = tab_Index;
		inter_Comp.interlayer.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Interlayer_Composition, interlayer_Index);
		QVariant var;
		var.setValue(struct_Data);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

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

		// for reloading
		check_Box->setProperty(reload_Property, false);
		check_Box->setProperty(tab_Index_Property, tab_Index);

		// storage
		check_Boxes_Map.		   insert	   (check_Box, structure_Item);
		reload_Show_Dependence_Map.insertMulti (check_Box, struct_Data.sigma.indicator.id);
		all_Widgets_To_Reload[tab_Index].append(check_Box);

		check_Box->setChecked(inter_Comp.enabled);

		// alignment
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		// set up BACK widget
		{
			back_Widget->setProperty(coupling_Editor_Property, true);		// for opening Coupling_Editor
			all_Widgets_To_Reload[tab_Index].append(back_Widget);

			coupled_Back_Widget_and_Struct_Item.insert(back_Widget, structure_Item);
			coupled_Back_Widget_and_Id.			insert(back_Widget, inter_Comp.interlayer.indicator.id);

			// colorize
			refresh_Reload_Colorize(colorize_Property, back_Widget, &inter_Comp.interlayer);
		}

		table->setCellWidget(current_Row, current_Column, back_Widget);

		connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Check_Box_Label_Interlayer);
		connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off(table); });
		connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off_2(table, structure_Item); });

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_Weigts_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString val_Type)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;
	double value = -2017;
	char format = line_edit_short_double_format;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		Interlayer& inter_Comp = interlayer_Composition[interlayer_Index];

		if(val_Type == VAL)	value = inter_Comp.interlayer.value;
		if(val_Type == MIN)	value = inter_Comp.interlayer.fit.min;
		if(val_Type == MAX)	value = inter_Comp.interlayer.fit.max;

		QString text_Value = QString::number(value, format, line_edit_interlayer_precision);

		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		line_Edit->setProperty(interlayer_Index_Property, interlayer_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setProperty(value_Type_Property, val_Type);

		// for reloading
		line_Edit->setProperty(reload_Property, false);
		line_Edit->setProperty(tab_Index_Property, tab_Index);

		// storage
		line_Edits_Map.			   insert	   (line_Edit, structure_Item);
		reload_Show_Dependence_Map.insertMulti (line_Edit, struct_Data.sigma.indicator.id);
		all_Widgets_To_Reload[tab_Index].append(line_Edit);

		// create item
		table->setCellWidget(current_Row, current_Column, line_Edit);

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit(table); });
		connect(line_Edit, &QLineEdit::textEdited, this, &Table_Of_Structures::refresh_Weigts_Interlayer);

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_Weights_Check_Box_Fit_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		Interlayer& inter_Comp = interlayer_Composition[interlayer_Index];

		QCheckBox* check_Box = new QCheckBox("fit");
		check_Box->setProperty(interlayer_Index_Property, interlayer_Index);

		// for reloading
		check_Box->setProperty(reload_Property, false);
		check_Box->setProperty(tab_Index_Property, tab_Index);

		// storage
		check_Boxes_Map.				 insert(check_Box, structure_Item);
		all_Widgets_To_Reload[tab_Index].append(check_Box);

		check_Box->setChecked(inter_Comp.interlayer.fit.is_Fitable);

		// alignment (back_Widget only for alignment)
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		table->setCellWidget(current_Row, current_Column, back_Widget);

		connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Weights_Check_Box_Fit_Interlayer);

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_MySigma_Labels_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;
	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		Parameter& my_Sigma = interlayer_Composition[interlayer_Index].my_Sigma;

		// update tab_Index and full_Name
		my_Sigma.indicator.tab_Index = tab_Index;
		my_Sigma.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Interlayer_My_Sigma, interlayer_Index);
		QVariant var;
		var.setValue(struct_Data);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		QLabel* label = new QLabel(Sigma_Sym+" "+transition_Layer_Functions[interlayer_Index]);
		label->setAlignment(Qt::AlignCenter);
		all_Widgets_To_Reload[tab_Index].append(label);

		// set up BACK widget
		{
			label->setProperty(coupling_Editor_Property, true);

			coupled_Back_Widget_and_Struct_Item.insert(label, structure_Item);
			coupled_Back_Widget_and_Id.			insert(label, my_Sigma.indicator.id);

			// colorize
			refresh_Reload_Colorize(colorize_Property, label, &my_Sigma);
		}

		table->setCellWidget(current_Row, current_Column, label);

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_MySigma_Line_Edits_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		Parameter& sigma_Comp = interlayer_Composition[interlayer_Index].my_Sigma;

		QString text_Value = QString::number(sigma_Comp.value, line_edit_double_format, line_edit_sigma_precision);

		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);

		line_Edit->setProperty(interlayer_Index_Property, interlayer_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		// for reloading
		line_Edit->setProperty(reload_Property, false);
		line_Edit->setProperty(tab_Index_Property, tab_Index);

		// storage
		line_Edits_Map.			   insert	   (line_Edit, structure_Item);
		reload_Show_Dependence_Map.insertMulti (line_Edit, sigma_Comp.indicator.id);
		all_Widgets_To_Reload[tab_Index].append(line_Edit);

		// create item (set LineEdits_Map)
		table->setCellWidget(current_Row, current_Column, line_Edit);

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit(table); });
		connect(line_Edit, &QLineEdit::textEdited, this, &Table_Of_Structures::refresh_MySigma_Interlayer);

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_Min_Max_Label(My_Table_Widget* table, int current_Row, int current_Column, QString text)
{
	add_Columns(table,current_Column);

	QLabel* label = new QLabel(text);
	label->setAlignment(Qt::AlignCenter);
	label->setStyleSheet("background-color: lightblue");

	// add widget to table
	table->setCellWidget(current_Row, current_Column, label);
}

void Table_Of_Structures::create_Min_Max_Button(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This)
{
	add_Columns(table,current_Column);

	QPushButton* min_Max_Button = new QPushButton("Reset");
	table->setCellWidget(current_Row, current_Column, min_Max_Button);

	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
	connect(min_Max_Button, &QPushButton::clicked, this, [=]
	{
		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);
		while (*it)
		{
			structure_Item=*it;
			Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

			if( struct_Data.item_Type == item_Type_Ambient  ||
				struct_Data.item_Type == item_Type_Layer    ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				if(whats_This == whats_This_Density)
				{
					if(struct_Data.absolute_Density.fit.is_Fitable)
					{
						struct_Data.absolute_Density.fit.min = struct_Data.absolute_Density.value*(100-multilayer->min_Max_Density)/100;
						struct_Data.absolute_Density.fit.max = struct_Data.absolute_Density.value*(100+multilayer->min_Max_Density)/100;
						struct_Data.absolute_Density.confidence.min = struct_Data.absolute_Density.fit.min;
						struct_Data.absolute_Density.confidence.max = struct_Data.absolute_Density.fit.max;
					}
					if(struct_Data.relative_Density.fit.is_Fitable)
					{
						struct_Data.relative_Density.fit.min = struct_Data.relative_Density.value*(100-multilayer->min_Max_Density)/100;
						struct_Data.relative_Density.fit.max = struct_Data.relative_Density.value*(100+multilayer->min_Max_Density)/100;
						struct_Data.relative_Density.confidence.min = struct_Data.relative_Density.fit.min;
						struct_Data.relative_Density.confidence.max = struct_Data.relative_Density.fit.max;
					}
				}
			}
			if( struct_Data.item_Type == item_Type_Layer)
			{
				if(whats_This == whats_This_Thickness)
				{
					if(struct_Data.thickness.fit.is_Fitable)
					{
						struct_Data.thickness.fit.min = struct_Data.thickness.value*(100-multilayer->min_Max_Thickness)/100;
						struct_Data.thickness.fit.max = struct_Data.thickness.value*(100+multilayer->min_Max_Thickness)/100;
						struct_Data.thickness.confidence.min = struct_Data.thickness.fit.min;
						struct_Data.thickness.confidence.max = struct_Data.thickness.fit.max;
					}
				}
			}
			if( struct_Data.item_Type == item_Type_Multilayer)
			{
				if(whats_This == whats_This_Thickness) // It is correct! NOT whats_This_Period, but thickness!
				{
					if(struct_Data.period.fit.is_Fitable)
					{
						struct_Data.period.fit.min = struct_Data.period.value*(100-multilayer->min_Max_Thickness)/100;
						struct_Data.period.fit.max = struct_Data.period.value*(100+multilayer->min_Max_Thickness)/100;
						struct_Data.period.confidence.min = struct_Data.period.fit.min;
						struct_Data.period.confidence.max = struct_Data.period.fit.max;
					}
				}
			}
			if( struct_Data.item_Type == item_Type_Layer    ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				if(whats_This == whats_This_Sigma)
				{
					if(struct_Data.sigma.fit.is_Fitable)
					{
						struct_Data.sigma.fit.min = struct_Data.sigma.value*(100-multilayer->min_Max_Sigma)/100;
						struct_Data.sigma.fit.max = struct_Data.sigma.value*(100+multilayer->min_Max_Sigma)/100;
						struct_Data.sigma.confidence.min = struct_Data.sigma.fit.min;
						struct_Data.sigma.confidence.max = struct_Data.sigma.fit.max;
					}
				}
			}

			QVariant var;
			var.setValue( struct_Data );
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			++it;
		}
		reload_Min_Max_Line_Edits(whats_This);
	});
}

void Table_Of_Structures::create_Min_Max_Spin_Box(My_Table_Widget* table, int tab_Index,  int current_Row, int current_Column, QString whats_This)
{
	// PARAMETER

	add_Columns(table,current_Column);
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	QDoubleSpinBox* spin_Box = new QDoubleSpinBox;
		spin_Box->setAccelerated(true);
		if(whats_This == whats_This_Density)
		{
			spin_Box->setValue(multilayer->min_Max_Density);
		}
		if(whats_This == whats_This_Thickness)
		{
			spin_Box->setValue(multilayer->min_Max_Thickness);
		}
		if(whats_This == whats_This_Sigma)
		{
			spin_Box->setValue(multilayer->min_Max_Sigma);
		}
		spin_Box->setRange(0, 100);
		spin_Box->setDecimals(3);
		spin_Box->setSingleStep(1);
		spin_Box->setButtonSymbols(QAbstractSpinBox::NoButtons);

	table->setCellWidget(current_Row, current_Column, spin_Box);
	connect(spin_Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		if(whats_This == whats_This_Density)
		{
			multilayer->min_Max_Density = spin_Box->value();
		}
		if(whats_This == whats_This_Thickness)
		{
			multilayer->min_Max_Thickness = spin_Box->value();
		}
		if(whats_This == whats_This_Sigma)
		{
			multilayer->min_Max_Sigma = spin_Box->value();
		}
	});
}

//// refresh

//// for material only
void Table_Of_Structures::refresh_Element(QString)
{
	QComboBox* combo_Box = qobject_cast<QComboBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = elements_Map.value(combo_Box);
	int composition_Index = combo_Box->property(num_Chemic_Element_Property).toInt();

	// for reloading
	bool reload = combo_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Stoichiometry& comp = struct_Data.composition[composition_Index];

	// reload state from struct_Tree
	if(reload)
	{
		combo_Box->setCurrentIndex(combo_Box->findText(comp.type));
		return;
	}
	// refresh struct_Tree
	{
		// state update
		comp.type = combo_Box->currentText();
		struct_Data.material = material_From_Composition(struct_Data.composition);

		// full name update
		comp.composition.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Composition, composition_Index);

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
	}
}

void Table_Of_Structures::refresh_Stoich(QString)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	int composition_Index = line_Edit->property(num_Chemic_Element_Property).toInt();
	QString value_Type = line_Edit->property(value_Type_Property).toString();

	char format = line_edit_short_double_format;
	int precision = line_edit_composition_precision;

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& comp = struct_Data.composition[composition_Index].composition;

	// reload state from struct_Tree
	if(reload)
	{
		if(value_Type == VAL)	line_Edit->setText(QString::number(comp.value,   format, precision));
		if(value_Type == MIN)	line_Edit->setText(QString::number(comp.fit.min, format, precision));
		if(value_Type == MAX)	line_Edit->setText(QString::number(comp.fit.max, format, precision));

		return;
	}
	// refresh struct_Tree
	{
		if(value_Type == VAL)	 comp.value   = line_Edit->text().toDouble();
		if(value_Type == MIN)	{comp.fit.min = line_Edit->text().toDouble(); comp.confidence.min = comp.fit.min;}
		if(value_Type == MAX)	{comp.fit.max = line_Edit->text().toDouble(); comp.confidence.max = comp.fit.max;}

		struct_Data.material = material_From_Composition(struct_Data.composition);

		// full name update
		comp.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Composition, composition_Index);

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Fit_Element(bool)
{
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	int composition_Index = check_Box->property(num_Chemic_Element_Property).toInt();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	// reload state from struct_Tree
	if(reload)
	{
		check_Box->setChecked(struct_Data.composition[composition_Index].composition.fit.is_Fitable);
		return;
	}
	// refresh struct_Tree
	{
		struct_Data.composition[composition_Index].composition.fit.is_Fitable = check_Box->isChecked();

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Material(QString)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(reload)
	{
		line_Edit->setText(struct_Data.material);
		return;
	}
	{
		struct_Data.material = line_Edit->text();
		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
	}
}

void Table_Of_Structures::check_Material()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);

	// no cheking for reloading
	bool reload = line_Edit->property(reload_Property).toBool();
	if(reload)
	{
		return;
	}
	{
		line_Edit->textEdited(line_Edit->text());
		Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(optical_Constants->material_Map.contains(struct_Data.material + nk_Ext))
		{
			struct_Data.approved_Material = struct_Data.material;
		} else
		{
			QMessageBox::information(this, "Wrong material", "File \"" + struct_Data.material + nk_Ext + "\" not found");
			struct_Data.material = struct_Data.approved_Material;
			line_Edit->setText(struct_Data.material);
			line_Edit->textEdited(line_Edit->text());
		}
		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::browse_Material(QLineEdit* material_Line_Edit)
{
	QFileInfo filename = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Find Material", nk_Path, "Optical constants " + QString(nk_Filter) + ";;All files (*->*)"));
	if (!filename.completeBaseName().isEmpty() || filename.completeSuffix() == nk_Ext)
	{
		material_Line_Edit->setText(filename.completeBaseName());
		material_Line_Edit->editingFinished();
	}
}

void Table_Of_Structures::refresh_Header(QString)
{
	// PARAMETER

	QLabel* label = qobject_cast<QLabel*>(QObject::sender());
	QString whats_This = label->property(whats_This_Property).toString();
	QString item_Type = label->property(item_Type_Property).toString();

	// for reloading
	bool reload = label->property(reload_Property).toBool();

	if(item_Type == item_Type_Layer)
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
	if(item_Type == item_Type_Multilayer)
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

//// for several parameters
void Table_Of_Structures::refresh_Check_Box_Header(bool)
{
	// PARAMETER

	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString whats_This = check_Box->property(whats_This_Property).toString();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(reload)
	{
		if(whats_This == whats_This_Thickness_Drift_Line_Value)	check_Box->setChecked(struct_Data.thickness_Drift.is_Drift_Line);
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms  )	check_Box->setChecked(struct_Data.thickness_Drift.is_Drift_Rand);
		if(whats_This == whats_This_Thickness_Drift_Sine      )	check_Box->setChecked(struct_Data.thickness_Drift.is_Drift_Sine);

		if(whats_This == whats_This_Sigma_Drift_Line_Value)	check_Box->setChecked(struct_Data.sigma_Drift.is_Drift_Line);
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )	check_Box->setChecked(struct_Data.sigma_Drift.is_Drift_Rand);
		if(whats_This == whats_This_Sigma_Drift_Sine	  )	check_Box->setChecked(struct_Data.sigma_Drift.is_Drift_Sine);

		if(whats_This == whats_This_Sigma)
		{
			check_Box->setChecked(struct_Data.common_Sigma);
			check_Box->setText(Sigma_Sym+" ["+length_units+"]");
		}
		return;
	}
	// if refresh
	{
		if(whats_This == whats_This_Thickness_Drift_Line_Value)	struct_Data.thickness_Drift.is_Drift_Line = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms  )	struct_Data.thickness_Drift.is_Drift_Rand = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine      )	struct_Data.thickness_Drift.is_Drift_Sine = check_Box->isChecked();

		if(whats_This == whats_This_Sigma_Drift_Line_Value)	struct_Data.sigma_Drift.is_Drift_Line = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )	struct_Data.sigma_Drift.is_Drift_Rand = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine	  )	struct_Data.sigma_Drift.is_Drift_Sine = check_Box->isChecked();

		if(whats_This == whats_This_Sigma)					struct_Data.common_Sigma = check_Box->isChecked();

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor)
{
	Data struct_Data = multilayer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(struct_Data.item_Type == item_Type_Multilayer)
	{
		struct_Data.period.value *= factor;
		for(int i=0; i<multilayer_Item->childCount(); ++i)
		{
			change_Child_Layers_Thickness(multilayer_Item->child(i), factor);
		}
	} else
	{
		if(struct_Data.item_Type == item_Type_Layer)
		{
			struct_Data.thickness.value *= factor;
		}
	}
	QVariant var;
	var.setValue( struct_Data );
	multilayer_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

void Table_Of_Structures::reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness)
{
	Data stack_Content = multilayer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	double factor=1;
	if( abs(stack_Content.period.value) > DBL_EPSILON && stack_Content.num_Repetition.value!=0)
	{
		factor = new_Thickness/(  stack_Content.period.value*stack_Content.num_Repetition.value  );
		change_Child_Layers_Thickness(multilayer_Item, factor);
	}
	//    stack_Content = multilayer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
}

void Table_Of_Structures::reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness)
{
	QVariant var;
	Data layer = layer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	layer.thickness.value = new_Thickness;
	var.setValue( layer );
	layer_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

void Table_Of_Structures::change_Stack_Gamma(QTreeWidgetItem* structure_Item, double new_Gamma_Value)
{
	QVariant var;
	Data stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	stack_Content.gamma.value = new_Gamma_Value;
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
		stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
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

//// for all parameters
void Table_Of_Structures::refresh_Parameter(My_Table_Widget* table)
{
	// PARAMETER
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString value_Type = line_Edit->property(value_Type_Property).toString();
	QString whats_This = line_Edit->property(whats_This_Property).toString();

	char min_Max_Format = line_edit_short_double_format;	
	int precision = 4;
	double coeff = 1;

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(whats_This == whats_This_Num_Repetitions)
	{
		if(reload)
		{
			if(value_Type == VAL)   line_Edit->setText(QString::number(struct_Data.num_Repetition.value));
			return;
		}
		// if refresh
		{
			if(value_Type == VAL)   struct_Data.num_Repetition.value = line_Edit->text().toInt();
		}

	} else
	{
		Parameter& parameter = get_Parameter(struct_Data, whats_This, precision, coeff);

		if(reload)
		{
			if(value_Type == VAL)
			{
				// special cases
				if(whats_This == whats_This_Sigma)
				{
					line_Edit->setText(QString::number(parameter.value/coeff, line_edit_double_format, precision));
					line_Edit->setDisabled(!struct_Data.common_Sigma);
					line_Edit->setStyleSheet("border: 1px solid grey");

					for(Interlayer& interlayer : struct_Data.interlayer_Composition)
					{
						interlayer.my_Sigma.value	= struct_Data.sigma.value;
						interlayer.my_Sigma.fit.min = struct_Data.sigma.fit.min;
						interlayer.my_Sigma.fit.max = struct_Data.sigma.fit.max;
						interlayer.my_Sigma.confidence.min = interlayer.my_Sigma.fit.min;
						interlayer.my_Sigma.confidence.max = interlayer.my_Sigma.fit.max;
					}
					{
						QVariant var;
						var.setValue( struct_Data );
						structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
					}
				} else
				if(whats_This == whats_This_Gamma)
				{
					if( abs(struct_Data.period.value) > DBL_EPSILON )
					{
						line_Edit->setText(QString::number(struct_Data.gamma.value, min_Max_Format, line_edit_gamma_precision));
						line_Edit->setDisabled(false);
						line_Edit->setStyleSheet("border: 1px solid grey");
					} else
					{
						line_Edit->setDisabled(true);
						line_Edit->setStyleSheet("border: 0px solid grey");
					}
				} else
				{
					line_Edit->setText(QString::number(parameter.value/coeff, line_edit_double_format, precision));
				}
			}
			if(value_Type == MIN)   line_Edit->setText(QString::number(parameter.fit.min/coeff, min_Max_Format,	precision));
			if(value_Type == MAX)   line_Edit->setText(QString::number(parameter.fit.max/coeff, min_Max_Format,	precision));

			return;
		}
		// if refresh
		{
			// special cases
			if(whats_This == whats_This_Sigma)
			{
				if(value_Type == VAL)	 parameter.value   = line_Edit->text().toDouble()*coeff;
				if(value_Type == MIN)	{parameter.fit.min = line_Edit->text().toDouble()*coeff; parameter.confidence.min = parameter.fit.min;}
				if(value_Type == MAX)	{parameter.fit.max = line_Edit->text().toDouble()*coeff; parameter.confidence.max = parameter.fit.max;}

				for(Interlayer& interlayer : struct_Data.interlayer_Composition)
				{
					interlayer.my_Sigma.value	= struct_Data.sigma.value;
					interlayer.my_Sigma.fit.min = struct_Data.sigma.fit.min;
					interlayer.my_Sigma.fit.max = struct_Data.sigma.fit.max;
					interlayer.my_Sigma.confidence.min = interlayer.my_Sigma.fit.min;
					interlayer.my_Sigma.confidence.max = interlayer.my_Sigma.fit.max;
				}
			} else
			if(whats_This == whats_This_Period)
			{
				if(value_Type == VAL)
				{
					double new_Period_Value = line_Edit->text().toDouble()*coeff;
					if(	qApp->focusWidget() != line_Edit ||
						abs(new_Period_Value) > DBL_EPSILON )
					{
						double factor = 1;
						if( abs(struct_Data.period.value) > DBL_EPSILON )
						{
							factor = new_Period_Value / struct_Data.period.value;
						}
						change_Child_Layers_Thickness(structure_Item, factor);
					}
				}
				if(value_Type == MIN)	{parameter.fit.min = line_Edit->text().toDouble()*coeff; parameter.confidence.min = parameter.fit.min;}
				if(value_Type == MAX)	{parameter.fit.max = line_Edit->text().toDouble()*coeff; parameter.confidence.max = parameter.fit.max;}
			} else
			if(whats_This == whats_This_Gamma)
			{
				double new_Value = line_Edit->text().toDouble();

				if(value_Type == VAL)
				{
					if(new_Value>1)
					{
						line_Edit->setText(QString::number(struct_Data.gamma.value));
						resize_Line_Edit(table, line_Edit);
					} else
					{
						if( qApp->focusWidget() != line_Edit ||
							(abs(new_Value) > DBL_EPSILON && abs(new_Value-1) > DBL_EPSILON) )
						{
							if( abs(struct_Data.period.value) > DBL_EPSILON )
							{
								change_Stack_Gamma(structure_Item, new_Value);
							}
						}
					}
				}
				if(value_Type == MIN)
				{
					if(new_Value>1)
					{
						line_Edit->setText(QString::number(struct_Data.gamma.fit.min));
						resize_Line_Edit(table, line_Edit);
					} else
					{
						struct_Data.gamma.fit.min = line_Edit->text().toDouble();
						struct_Data.gamma.confidence.min = struct_Data.gamma.fit.min;
					}
				}
				if(value_Type == MAX)
				{
					if(new_Value>1)
					{
						line_Edit->setText(QString::number(struct_Data.gamma.fit.max));
						resize_Line_Edit(table, line_Edit);
					} else
					{
						struct_Data.gamma.fit.max = line_Edit->text().toDouble();
						struct_Data.gamma.confidence.max = struct_Data.gamma.fit.max;
					}
				}
			} else
			{
				if(value_Type == VAL)	 parameter.value   = line_Edit->text().toDouble()*coeff;
				if(value_Type == MIN)	{parameter.fit.min = line_Edit->text().toDouble()*coeff; parameter.confidence.min = parameter.fit.min;}
				if(value_Type == MAX)	{parameter.fit.max = line_Edit->text().toDouble()*coeff; parameter.confidence.max = parameter.fit.max;}
			}
		}
	}
	{
		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Fit_Parameter(bool)
{
	// PARAMETER

	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());

	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString whats_This = check_Box->property(whats_This_Property).toString();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& parameter = get_Parameter(struct_Data, whats_This);

	if(reload)
	{
		check_Box->setChecked(parameter.fit.is_Fitable);
		return;
	}
	// if refresh
	{
		parameter.fit.is_Fitable = check_Box->isChecked();

		// special case
		if(whats_This == whats_This_Sigma)
		{
			for(Interlayer& interlayer : struct_Data.interlayer_Composition)
			{
				interlayer.my_Sigma.fit.is_Fitable = check_Box->isChecked();
			}
		}

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

//// for interlayers
void Table_Of_Structures::refresh_Check_Box_Label_Interlayer(bool)
{
	// PARAMETER

	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	int interlayer_Index = check_Box->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(reload)
	{
		check_Box->setChecked(struct_Data.interlayer_Composition[interlayer_Index].enabled);
		return;
	}
	// if refresh
	{
		struct_Data.interlayer_Composition[interlayer_Index].enabled = check_Box->isChecked();
		if(!struct_Data.common_Sigma)
		{
			struct_Data.sigma.value = recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
		}

		// here we save non-parapetric changes

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// refresh widget
		//	var.setValue(struct_Data.interlayer_Composition[interlayer_Index].interlayer);
		//	QWidget* back_Widget = coupled_Back_Widget_and_Id.key(struct_Data.interlayer_Composition[interlayer_Index].interlayer.indicator.id);
		//	back_Widget->setProperty(parameter_Property, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Weigts_Interlayer(QString)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	int interlayer_Index = line_Edit->property(interlayer_Index_Property).toInt();
	QString value_Type = line_Edit->property(value_Type_Property).toString();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& interlayer = struct_Data.interlayer_Composition[interlayer_Index].interlayer;

	if(reload)
	{
		if(value_Type == VAL)	line_Edit->setText(QString::number(interlayer.value,   line_edit_short_double_format, line_edit_interlayer_precision));
		if(value_Type == MIN)	line_Edit->setText(QString::number(interlayer.fit.min, line_edit_short_double_format, line_edit_interlayer_precision));
		if(value_Type == MAX)	line_Edit->setText(QString::number(interlayer.fit.max, line_edit_short_double_format, line_edit_interlayer_precision));
		return;
	}
	// if refresh
	{
		if(value_Type == VAL)
		{
			interlayer.value = line_Edit->text().toDouble();
			if(!struct_Data.common_Sigma)
			{
				struct_Data.sigma.value = recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
			}
		}
		if(value_Type == MIN)	{interlayer.fit.min = line_Edit->text().toDouble(); interlayer.confidence.min = interlayer.fit.min;}
		if(value_Type == MAX)	{interlayer.fit.max = line_Edit->text().toDouble(); interlayer.confidence.max = interlayer.fit.max;}

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_Weights_Check_Box_Fit_Interlayer(bool)
{
	// PARAMETER

	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	int interlayer_Index = check_Box->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& parameter = struct_Data.interlayer_Composition[interlayer_Index].interlayer;
	if(reload)
	{
		check_Box->setChecked(parameter.fit.is_Fitable);
		return;
	}
	// if refresh
	{
		parameter.fit.is_Fitable = check_Box->isChecked();

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

void Table_Of_Structures::refresh_MySigma_Interlayer(QString)
{
	double length_Coeff = length_Coefficients_Map.value(length_units);

	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	int interlayer_Index = line_Edit->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Interlayer& interlayer = struct_Data.interlayer_Composition[interlayer_Index];

	if(reload)
	{
		line_Edit->setText(QString::number(interlayer.my_Sigma.value/length_Coeff, line_edit_double_format, line_edit_sigma_precision));
		bool disable = struct_Data.common_Sigma || !interlayer.enabled;
		line_Edit->setDisabled(disable);
		return;
	}
	// if refresh
	{
		interlayer.my_Sigma.value = line_Edit->text().toDouble()*length_Coeff;
		if(!struct_Data.common_Sigma)
		{
			struct_Data.sigma.value = recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
		}

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());
	}
}

double Table_Of_Structures::recalculate_Sigma_From_Individuals(QVector<Interlayer>& interlayer_Composition)
{
	double temp_Sigma_Square=0;
	double sum = 0;
	for(Interlayer& interlayer : interlayer_Composition)
	{
		if(interlayer.enabled)
		{
			sum += interlayer.interlayer.value;
			temp_Sigma_Square += pow(interlayer.my_Sigma.value,2) * interlayer.interlayer.value;
		}
	}

	if(abs(sum)<DBL_EPSILON) sum = DBL_EPSILON;
	return sqrt(temp_Sigma_Square/sum);
}

//// general
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

void Table_Of_Structures::cells_On_Off_2(My_Table_Widget* table, QTreeWidgetItem* structure_Item)
{
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	int current_Row = check_Box->property(row_Property).toInt();
	int current_Column = check_Box->property(column_Property).toInt();

	int row_Start = check_Box->property(relative_Rows_To_Disable_Start_Property_2).toInt();
	int row_Finish = check_Box->property(relative_Rows_To_Disable_Finish_Property_2).toInt();
	int column_Start = check_Box->property(relative_Columns_To_Disable_Start_Property_2).toInt();
	int column_Finish = check_Box->property(relative_Columns_To_Disable_Finish_Property_2).toInt();

	for(int row=row_Start; row<=row_Finish; ++row)
	{
		for(int col=column_Start; col<=column_Finish; ++col)
		{
			QWidget* widget = table->cellWidget(current_Row+row,current_Column+col);

			if(widget)
			{
				widget->setDisabled(!check_Box->isChecked() || struct_Data.common_Sigma);

				if(!check_Box->isChecked())
					widget->setStyleSheet("border: 0px solid red");
				else
					widget->setStyleSheet("border: 1px solid grey");
			}
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
		// reloading for widgets on current tab
		int current_Tab_Index = main_Tabs->currentIndex();
//		qInfo() << "all_Widgets_To_Reload[current_Tab_Index].size() " << all_Widgets_To_Reload[current_Tab_Index].size();

		for(int i=0; i<all_Widgets_To_Reload[current_Tab_Index].size(); ++i)
		{
			QWidget* widget_To_Reload = all_Widgets_To_Reload[current_Tab_Index][i];
			if(widget_To_Reload != sender)
			{
				reload_One_Widget(widget_To_Reload);
			}
		}
	}
}

void Table_Of_Structures::reload_One_Widget(QWidget* widget_To_Reload)
{
	// reload dependences and color
	if(widget_To_Reload->property(coupling_Editor_Property).toBool())
	{
		id_Type id = coupled_Back_Widget_and_Id.value(widget_To_Reload);
		QTreeWidgetItem* struct_Item = coupled_Back_Widget_and_Struct_Item.value(widget_To_Reload);
		Data struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		Parameter* parameter = Global_Variables::get_Parameter_From_Struct_Item_by_Id(struct_Data, id);

		// if had some dependences
		if(parameter->coupled.master.exist || parameter->coupled.slaves.size()>0)
		{
			Coupling_Editor* new_Coupling_Editor = new Coupling_Editor(widget_To_Reload, this, nullptr); // last nullptr is essential for menu disabling!
				new_Coupling_Editor->close();
		}
	}

	// do not reload disabled widgets
	if(!widget_To_Reload->property(enabled_Property).toBool())	return;

	widget_To_Reload->setProperty(reload_Property, true);

	QLabel*        label = qobject_cast<QLabel*>   (widget_To_Reload);
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(widget_To_Reload);
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(widget_To_Reload);
	QComboBox* combo_Box = qobject_cast<QComboBox*>(widget_To_Reload);

	if(label)
	{
		label->windowTitleChanged("temp");
	}
	if(check_Box)
	{
		check_Box->toggled(false);				//
		check_Box->toggled(false);				// repeated intentionally!
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

	widget_To_Reload->setProperty(reload_Property, false);
}

void Table_Of_Structures::reload_Related_Widgets(QObject* sender)
{
	if(table_Is_Created)
	{
//		qInfo() << "reload_Related_Widgets " << ++temp_Counter_1;
		for(id_Type id : reload_Show_Dependence_Map.values(qobject_cast<QWidget*>(sender)))
		{
			for(QWidget* related: reload_Show_Dependence_Map.keys(id))
			{
				if(related != sender)
				{
					related->setProperty(reload_Property, true);

					QLabel*        label = qobject_cast<QLabel*>   (related);
					QCheckBox* check_Box = qobject_cast<QCheckBox*>(related);
					QLineEdit* line_Edit = qobject_cast<QLineEdit*>(related);
					QComboBox* combo_Box = qobject_cast<QComboBox*>(related);

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
						line_Edit->textEdited("temp");
						line_Edit->textEdited("temp");
					}
					if(combo_Box)
					{
						combo_Box->currentTextChanged("temp");
					}

					related->setProperty(reload_Property, false);
				}
			}
		}
	}
}

void Table_Of_Structures::reload_Min_Max_Line_Edits(QString whats_This)
{
	// reloading for widgets on current tab
	int current_Tab_Index = main_Tabs->currentIndex();

	QList<QLineEdit*>* list_Pointer;

	if( whats_This == whats_This_Density )		list_Pointer = &min_Max_Density_Line_Edits_List[current_Tab_Index];
	if( whats_This == whats_This_Thickness )	list_Pointer = &min_Max_Thickness_Line_Edits_List[current_Tab_Index];
	if( whats_This == whats_This_Sigma )		list_Pointer = &min_Max_Sigma_Line_Edits_List[current_Tab_Index];

	for(QLineEdit* line_Edit : *list_Pointer)
	{
		line_Edit->setProperty(reload_Property, true);
		line_Edit->textEdited("temp");
		line_Edit->setProperty(reload_Property, false);
	}
}
