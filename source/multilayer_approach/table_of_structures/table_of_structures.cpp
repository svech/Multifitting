#include "table_of_structures.h"
#include "algorithm"

Table_Of_Structures::Table_Of_Structures(bool temporary, QWidget *parent) :
	runned_Tables_Of_Structures(global_Multilayer_Approach->runned_Tables_Of_Structures),
	runned_Calculation_Settings_Editor(global_Multilayer_Approach->runned_Calculation_Settings_Editor),
	multilayer_Tabs(global_Multilayer_Approach->multilayer_Tabs),
	temporary(temporary),
	QWidget(parent) // nullptr!
{
	setWindowTitle("Table Of Structures");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

bool Table_Of_Structures::eventFilter(QObject *obj, QEvent *event)
{
	Q_UNUSED(obj);
	if(event->type() == QEvent::Wheel)
	{
		return !mouse_Wheel_Spinbox_Table;
	}
	return false;
}

void Table_Of_Structures::closeEvent(QCloseEvent* event)
{
	if(!temporary) write_Window_Geometry();
	runned_Tables_Of_Structures.remove(table_Of_Structures_Key);
	for(QLineEdit* material_Line_Edit : material_Line_Edits)
	{
		material_Line_Edit->blockSignals(true);
		check_Material(material_Line_Edit);
	}
	if(global_Multilayer_Approach->runned_Regular_Aperiodic_Tables.isEmpty()) global_Multilayer_Approach->unlock_Mainwindow_Interface();
	event->accept();
}

void Table_Of_Structures::emit_Data_Edited()
{
	emit data_Edited();
}

void Table_Of_Structures::create_Main_Layout()
{
	can_Change_Index = false;
	main_Layout = new QHBoxLayout(this);
	main_Layout->setContentsMargins(0,0,0,0);


	global_Multilayer_Approach->lock_Mainwindow_Interface();
	create_Menu();
	create_Tabs();
	main_Layout->addWidget(main_Tabs);

	add_Tabs();

	for(Regular_Aperiodic_Table* regular_Aperiodic_Table : global_Multilayer_Approach->runned_Regular_Aperiodic_Tables.values())
	{
		connect(regular_Aperiodic_Table, &Regular_Aperiodic_Table::regular_Aperiodic_Edited, this, &Table_Of_Structures::reload_From_Regular_Aperiodic);
		connect(this, &Table_Of_Structures::regular_Layer_Edited, regular_Aperiodic_Table, &Regular_Aperiodic_Table::reload_All_Widgets);
	}

	refill_All_Dependent();
	can_Change_Index = tab_synchronization;
}

void Table_Of_Structures::set_Window_Geometry()
{
	setGeometry(structure_table_x_corner,structure_table_y_corner,structure_table_width,structure_table_height);
}

void Table_Of_Structures::write_Window_Geometry()
{
	// usual show
	if(!isMaximized())
	{
		structure_table_x_corner = frameGeometry().x()-corner_x_shift;
		structure_table_y_corner = frameGeometry().y()-corner_y_shift;

		structure_table_width  = geometry().width() +  width_add;
		structure_table_height = geometry().height()+ height_add;
	}
}

void Table_Of_Structures::create_Menu()
{
	menu = new Menu(window_Type_Table_Of_Structures,this);
	main_Layout->setMenuBar(menu->menu_Bar);
	connect(menu, &Menu::refresh, this, [=]{reload_All_Widgets();});
	connect(menu, &Menu::refresh, this, &Table_Of_Structures::emit_Data_Edited);

	if(global_Multilayer_Approach->fitting_Settings->in_Calculation)
	{
		for(QAction* action : menu->calculate_Menu->actions())
		{
			if(action->property(abort_Property).toString() != abort_Property)		{
				action->setDisabled(true);
			}
		}
	}
}

void Table_Of_Structures::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(false);

	connect(main_Tabs,	&QTabWidget::currentChanged, this, [=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);
		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index)main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}

		if(can_Change_Index)
		{
			can_Change_Index = false;
																											{global_Multilayer_Approach->                       multilayer_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key))				{global_Multilayer_Approach->optical_Graphs				->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key))	{global_Multilayer_Approach->calculation_Settings_Editor->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			can_Change_Index = tab_synchronization;
		}

		// CHECK
//		reload_All_Widgets();
	});
}

void Table_Of_Structures::add_Tabs()
{
	read_Trees();
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		My_Table_Widget* new_Table = new My_Table_Widget(basic_Row_Number, basic_Column_Number, this, this);
		all_Widgets_To_Reload.append(QList<QWidget*>());
		regular_Aperiodic_Widgets_To_Reload.append(QList<QWidget*>());

		min_Max_Density_Spin_Boxes_List.append(QList<MyDoubleSpinBox*>());
		min_Max_Thickness_Spin_Boxes_List.append(QList<MyDoubleSpinBox*>());
		min_Max_Sigma_Spin_Boxes_List.append(QList<MyDoubleSpinBox*>());

		create_Table(new_Table, tab_Index);
		main_Tabs->addTab(new_Table, multilayer_Tabs->tabText(tab_Index));
		disable_enable_Structure_Items(new_Table);
	}
	for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
	{
		if(i!=main_Tabs->currentIndex()) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
	}

	table_Is_Created = true;

	// CHECK
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		main_Tabs->setCurrentIndex(tab_Index);
		reload_All_Widgets();
	}
	main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());
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

		bool has_Layers = false;
		bool has_Boundaries = false;
		bool has_Multilayers = false;
		bool has_Gamma = false;

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
				if(struct_Data.item_Type == item_Type_Multilayer)
				{
					has_Multilayers = true;
					if(structure_Item->childCount()==2) has_Gamma = true;
				}

				if( struct_Data.item_Type == item_Type_Ambient  ||
					struct_Data.item_Type == item_Type_Layer    ||
					struct_Data.item_Type == item_Type_Substrate )
				{
					if(struct_Data.composed_Material)
					{	max_Number_Of_Elements = max(struct_Data.composition.size(),max_Number_Of_Elements); }
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
			new_Table->insertRow(new_Table->rowCount());

			add_Columns(new_Table, max_Depth + max_Number_Of_Elements+2);
			current_Column = max_Depth + max_Number_Of_Elements+2;

			/// color legend
			{
				// slave parameter
				QLabel* slave_Label = new QLabel("pure slave");
					slave_Label->setAlignment(Qt::AlignCenter);
					slave_Label->setMinimumWidth(COLOR_LEGEND_LABEL_WIDTH);
				slave_Label->setStyleSheet(slave_Parameter_Color);
				new_Table->setCellWidget(current_Row,0, slave_Label);

				// master and slave parameter
				QLabel* master_Slave_Label = new QLabel("master/slave");
					master_Slave_Label->setAlignment(Qt::AlignCenter);
					master_Slave_Label->setMinimumWidth(COLOR_LEGEND_LABEL_WIDTH);
				master_Slave_Label->setStyleSheet(master_Slave_Parameter_Color);
				new_Table->setCellWidget(current_Row+1,0, master_Slave_Label);

				// master parameter
				QLabel* master_Label = new QLabel("pure master");
					master_Label->setAlignment(Qt::AlignCenter);
				master_Label->setStyleSheet(master_Parameter_Color);
				new_Table->setCellWidget(current_Row+2,0, master_Label);

				// free parameter
				QLabel* free_Label = new QLabel("free parameter");
					free_Label->setAlignment(Qt::AlignCenter);
					free_Label->setMinimumWidth(COLOR_LEGEND_LABEL_WIDTH);
				free_Label->setStyleSheet(free_Parameter_Color);
				new_Table->setCellWidget(current_Row+3,0, free_Label);

				// confidence parameter
				QLabel* confidence_Label = new QLabel("confidence interval");
					confidence_Label->setAlignment(Qt::AlignCenter);
					confidence_Label->setMinimumWidth(COLOR_LEGEND_LABEL_WIDTH);
				confidence_Label->setStyleSheet(confidence_Parameter_Color);
				new_Table->setCellWidget(current_Row,1, confidence_Label);

				// confidence master parameter
				QLabel* confidence_Master_Label = new QLabel("master/confidence");
					confidence_Master_Label->setAlignment(Qt::AlignCenter);
					confidence_Master_Label->setMinimumWidth(COLOR_LEGEND_LABEL_WIDTH);
				confidence_Master_Label->setStyleSheet(master_Confidence_Parameter_Color);
				new_Table->setCellWidget(current_Row+1,1, confidence_Master_Label);

				// z restriction
				QLabel* restrict_Z_Label = new QLabel("restrict z");
					restrict_Z_Label->setAlignment(Qt::AlignCenter);
					restrict_Z_Label->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
				restrict_Z_Label->setStyleSheet(soft_Restriction_Color);
				new_Table->setCellWidget(current_Row+2,1, restrict_Z_Label);

				// active fit
				QLabel* fit_Label = new QLabel("active fit");
					fit_Label->setAlignment(Qt::AlignCenter);
					fit_Label->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
				fit_Label->setStyleSheet(fit_Color);
				new_Table->setCellWidget(current_Row,2, fit_Label);

				// common z
				QLabel* common_Z_Label = new QLabel("common z");
					common_Z_Label->setAlignment(Qt::AlignCenter);
					common_Z_Label->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
				common_Z_Label->setStyleSheet(common_Thickness_Color);
				new_Table->setCellWidget(current_Row+1,2, common_Z_Label);

				// common sigma
				QLabel* common_Sigma_Label = new QLabel("common "+Sigma_Sym);
					common_Sigma_Label->setAlignment(Qt::AlignCenter);
					common_Sigma_Label->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
				common_Sigma_Label->setStyleSheet(common_Sigma_Color);
				new_Table->setCellWidget(current_Row+2,2, common_Sigma_Label);
			}

			// density min/max
			create_Simple_Label		(new_Table,			   current_Row,   current_Column, Rho_Sym+", "+/*Plus_Minus_Sym*/Minus_Sym+"%");
			create_Min_Max_Button	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Density);
			create_Min_Max_Spin_Box	(new_Table, tab_Index, current_Row+2, current_Column, whats_This_Density);
			current_Column += 2;

			// thickness min/max
			if(has_Layers)
			{
				create_Simple_Label		(new_Table,			   current_Row,   current_Column, "z/d, "+Plus_Minus_Sym+"%");
				create_Min_Max_Button	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Thickness);
				create_Min_Max_Spin_Box	(new_Table, tab_Index, current_Row+2, current_Column, whats_This_Thickness);
			}
			current_Column += 2;

			// sigma min/max
			if(has_Boundaries)
			{
				create_Simple_Label		(new_Table,			   current_Row,   current_Column, Sigma_Sym+", "+Plus_Minus_Sym+"%");
				create_Min_Max_Button	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Sigma);
				create_Min_Max_Spin_Box	(new_Table, tab_Index, current_Row+2, current_Column, whats_This_Sigma);
			}
		}

		// place for step buttons
		{
			new_Table->insertRow(new_Table->rowCount());
			current_Row = new_Table->rowCount()-2;

			rows_List_To_Span.append(current_Row);
			new_Table->setItem(current_Row,0, new QTableWidgetItem("Set steps"));
			new_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);
			new_Table->item   (current_Row,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			new_Table->item   (current_Row,0)->setFont(font_Header);

			new_Table->insertRow(new_Table->rowCount());
			current_Row = new_Table->rowCount()-2;
			new_Table->insertRow(new_Table->rowCount());

			// recalculate on spinboxes change
			{
				current_Column = 0;
				spin_Box_Mouse_Wheel	 (new_Table, current_Row  , current_Column);
				spin_Box_Recalculate	 (new_Table, current_Row+1, current_Column);
				spin_Box_Change_Dependent(new_Table, current_Row  , current_Column+1);
			}

			add_Columns(new_Table, max_Depth + max_Number_Of_Elements+2);

			// composition step
			if(max_Number_Of_Elements>=2)
			{
				current_Column = max_Depth+1;
				create_Simple_Label		(new_Table,				current_Row,   current_Column, Zeta_Sym + " step");
				create_Step_Spin_Box	(new_Table, tab_Index,	current_Row+1, current_Column, whats_This_Composition);
			}
			current_Column = max_Depth + max_Number_Of_Elements+2;

			// density step
			create_Simple_Label		(new_Table,				current_Row,   current_Column, Rho_Sym + " step");
			create_Step_Spin_Box	(new_Table, tab_Index,	current_Row+1, current_Column, whats_This_Density);
			current_Column += 2;

			// thickness step
			if(has_Layers)
			{
				create_Simple_Label	(new_Table,			   current_Row,   current_Column, "z/d step");
				create_Step_Spin_Box(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Thickness);
			}
			current_Column += 2;

			// sigma step
			if(has_Boundaries)
			{
				create_Simple_Label	(new_Table,			   current_Row,   current_Column, Sigma_Sym + " step");
				create_Step_Spin_Box(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Sigma);
			}
			current_Column += 1;

			// gamma step
			if(has_Gamma)
			{
				create_Simple_Label	(new_Table,			   current_Row,   current_Column, Gamma_Sym + " step");
				create_Step_Spin_Box(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Gamma);
			}
			current_Column += 4;

			// interlayer step
			if(has_Boundaries)
			{
				create_Simple_Label	(new_Table,			   current_Row,   current_Column, Alpha_Sym + " step");
				create_Step_Spin_Box(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Interlayer_Composition);
			}
			current_Column += 4;

			// drift step
			if(has_Multilayers)
			{
				create_Simple_Label	(new_Table,			   current_Row,   current_Column, "drift step");
				create_Step_Spin_Box(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Drift);
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

			if( struct_Data.item_Type == item_Type_Multilayer	||
				struct_Data.item_Type == item_Type_Ambient		||
				struct_Data.item_Type == item_Type_Layer		||
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
				item_CheckBox->setProperty(item_Table_CheckBox_Property,item_Table_CheckBox_Property);
				item_CheckBox->setProperty(column_Property,current_Column);
			if(struct_Data.item_Type == item_Type_Multilayer)			{
				item_CheckBox->setProperty(multilayer_Item_Table_CheckBox_Property,multilayer_Item_Table_CheckBox_Property);
							   setProperty(multilayer_Item_Table_CheckBox_Property,multilayer_Item_Table_CheckBox_Property);
				coupled_Back_Widget_and_Struct_Item.insert(item_CheckBox, structure_Item);
			}
			if(struct_Data.item_Type == item_Type_Regular_Aperiodic)			{
				item_CheckBox->setProperty(regular_Aperiodic_Item_Table_CheckBox_Property,regular_Aperiodic_Item_Table_CheckBox_Property);
				coupled_Back_Widget_and_Struct_Item.insert(item_CheckBox, structure_Item);
			}
			new_Table->setCellWidget(current_Row, current_Column, item_CheckBox);
			if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
			{
				item_CheckBox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
				item_CheckBox->setFocusPolicy(true ? Qt::NoFocus : Qt::StrongFocus);
			}
			item_CheckBox->setChecked(struct_Data.item_Enabled);
			check_Boxes_Map.insert(item_CheckBox, structure_Item);
			item_Check_Boxes_Map.insert(structure_Item, item_CheckBox);
			connect(item_CheckBox, &QCheckBox::toggled, this, [=]
			{
				// enable or disable widgets
				disable_enable_One_Item(new_Table, table_Item);				
				if(recalculate_Spinbox_Table) {global_Multilayer_Approach->calc_Reflection(true);}

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
			if( struct_Data.item_Type == item_Type_Multilayer ||
				struct_Data.item_Type == item_Type_Regular_Aperiodic)
			{
				// num repetitions
				QString whats_This = whats_This_Num_Repetitions;
				add_Columns			(new_Table, current_Column+5);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "N");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				if( struct_Data.item_Type == item_Type_Multilayer)
				{
					create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
					create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
					// lastcolumn
					create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
					current_Column += 2;


					// period
					whats_This = whats_This_Period;
					add_Columns			(new_Table, current_Column+5);
					create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "d ["+length_units+"]");
					create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
					create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
					create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
					// lastcolumn
					create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
					current_Column += 3;

					// gamma
					if(structure_Item->childCount()==2)
					{
						whats_This = whats_This_Gamma;
						add_Columns			(new_Table, current_Column+5);
						create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Gamma_Sym);
						create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
						create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
						create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
						// last
						create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
						current_Column += 2;
					}
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
				create_Weigts_Interlayer		        (new_Table, tab_Index, current_Row+1, current_Column, structure_Item, VAL);
				create_Check_Box_Label_Interlayer	    (new_Table, tab_Index, current_Row,   current_Column, structure_Item);
				create_Weights_Check_Box_Fit_Interlayer (new_Table, tab_Index, current_Row+2, current_Column, structure_Item);

				// sigmas
				if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
				{
					create_Check_Box_Label      (new_Table, tab_Index, current_Row+3, current_Column,  structure_Item, whats_This_Common_Thickness, "common z", -2, -2, -4, -4);
					create_Check_Box_Label      (new_Table, tab_Index, current_Row+4, current_Column,  structure_Item, whats_This_Common_Sigma, "common "+Sigma_Sym, -3, -3, -2, -2);
					create_Check_Box_Label      (new_Table, tab_Index, current_Row+3, current_Column+2, structure_Item, whats_This_Restrict_Thickness, "restrict z: {"+Plus_Minus_Sym+Delta_Big_Sym+", p, Q}", 1, 1, 0, 2);
					create_Thickness_Restriction(new_Table,            current_Row+4, current_Column+2, structure_Item);
				} else
				{
					create_MySigma_Labels_Interlayer	    (new_Table, tab_Index, current_Row+3, current_Column, structure_Item);
					create_MySigma_Line_Edits_Interlayer	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item);
				}
			}
			current_Column += (1+transition_Layer_Functions_Size);
			///--------------------------------------------------------------------------------------------
			/// DRIFTS
			///--------------------------------------------------------------------------------------------

			// thickness linear drift
			if(struct_Data.thickness_Drift.show_Drift_Line)
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
				current_Column += 1;
			}
			///--------------------------------------------------------------------------------------------

			// thickness random drift
			if(struct_Data.thickness_Drift.show_Drift_Rand)
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
				current_Column += 1;
			}
			///--------------------------------------------------------------------------------------------

			// thickness sine drift
			if(struct_Data.thickness_Drift.show_Drift_Sine)
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
				current_Column += 3;
			}
			if(	struct_Data.thickness_Drift.show_Drift_Line ||
				struct_Data.thickness_Drift.show_Drift_Rand ||
				struct_Data.thickness_Drift.show_Drift_Sine )
			{	current_Column += 1; }
			///--------------------------------------------------------------------------------------------

			// sigma linear drift
			if(struct_Data.sigma_Drift.show_Drift_Line)
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
				current_Column += 1;
			}
			///--------------------------------------------------------------------------------------------

			// sigma random drift
			if(struct_Data.sigma_Drift.show_Drift_Rand)
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
				current_Column += 1;
			}
			///--------------------------------------------------------------------------------------------

			// sigma sine drift
			if(struct_Data.sigma_Drift.show_Drift_Sine)
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
				current_Column += 4;
			}
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
			{
				if(parameter->fit.is_Fitable)
					back_Widget->setStyleSheet(fit_Color);
				else
					back_Widget->setStyleSheet(white_Color);
			}
			else
				back_Widget->setStyleSheet(free_Parameter_Color);
		}

		// has master only
		if(parameter->coupled.master.exist && parameter->coupled.slaves.size()==0)
			back_Widget->setStyleSheet(slave_Parameter_Color);

		// has slaves only
		if(!parameter->coupled.master.exist && parameter->coupled.slaves.size()>0)
			back_Widget->setStyleSheet(master_Parameter_Color);

		// has both
		if(parameter->coupled.master.exist && parameter->coupled.slaves.size()>0)
			back_Widget->setStyleSheet(master_Slave_Parameter_Color);

		// confidence interval. if has no slaves
		if(parameter->confidence.calc_Conf_Interval && parameter->coupled.slaves.size()==0)
			back_Widget->setStyleSheet(confidence_Parameter_Color);

		// confidence interval. if has slaves
		if(parameter->confidence.calc_Conf_Interval && parameter->coupled.slaves.size()>0)
			back_Widget->setStyleSheet(master_Confidence_Parameter_Color);

		// colorize fit
		QCheckBox* fit_Check_Box = check_Boxes_Fit_Map.key(parameter->indicator.id);
		if(fit_Check_Box)
			fit_Check_Box->toggled(fit_Check_Box->isChecked());
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
	if(whats_This == whats_This_Angle)						{precision = line_edit_angle_precision;					coeff = 1;						return struct_Data.probe_Angle;					}
	if(whats_This == whats_This_Angular_Resolution)			{precision = line_edit_angle_precision;					coeff = 1;						return struct_Data.angular_Resolution;			}
	if(whats_This == whats_This_Wavelength)					{precision = line_edit_wavelength_precision;			coeff = 1;						return struct_Data.wavelength;					}
	if(whats_This == whats_This_Spectral_Resolution)		{precision = line_edit_spectral_resolution_precision;	coeff = 1;						return struct_Data.spectral_Resolution;			}
	if(whats_This == whats_This_Polarization)				{precision = 3;											coeff = 1;						return struct_Data.polarization;				}
	if(whats_This == whats_This_Polarization_Sensitivity)	{precision = 3;											coeff = 1;						return struct_Data.polarization_Sensitivity;	}
	if(whats_This == whats_This_Background)					{precision = line_edit_background_precision;			coeff = 1;						return struct_Data.background;					}

	if(whats_This == whats_This_Beam_Size)					{precision = line_edit_beam_size_precision;		coeff = 1;						return struct_Data.beam_Size;					}
	if(whats_This == whats_This_Beam_Profile_Spreading)		{precision = line_edit_beam_size_precision;		coeff = 1;						return struct_Data.beam_Profile_Spreading;		}
	if(whats_This == whats_This_Sample_Size)				{precision = line_edit_sample_size_precision;	coeff = 1;						return struct_Data.sample_Size;					}
	if(whats_This == whats_This_Sample_Shift)				{precision = line_edit_sample_size_precision;	coeff = 1;						return struct_Data.sample_Shift;				}

	// optical properties
	if(whats_This == whats_This_Absolute_Density)			{precision = line_edit_density_precision;	coeff = 1;						return struct_Data.absolute_Density;			}
	if(whats_This == whats_This_Relative_Density)			{precision = line_edit_density_precision;	coeff = 1;						return struct_Data.relative_Density;			}
	if(whats_This == whats_This_Permittivity)				{precision = line_edit_density_precision;	coeff = 1;						return struct_Data.permittivity;				}
	if(whats_This == whats_This_Absorption)					{precision = line_edit_density_precision;	coeff = 1;						return struct_Data.absorption;					}

	// thickness
	if(whats_This == whats_This_Thickness)						{precision = line_edit_thickness_precision;	coeff = length_Coefficients_Map.value(length_units);	return struct_Data.thickness;	}
	if(whats_This == whats_This_Thickness_Drift_Line_Value)		{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Line_Value;		}
	if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Rand_Rms;			}
	if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Sine_Amplitude;	}
	if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Sine_Frequency;	}
	if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		{precision = line_edit_thickness_precision;	coeff = 1;					return struct_Data.thickness_Drift.drift_Sine_Phase;		}

	// interface
	if(whats_This == whats_This_Sigma)						{precision = line_edit_sigma_precision;		coeff = length_Coefficients_Map.value(length_units);	return struct_Data.sigma;		}
	if(whats_This == whats_This_Sigma_Drift_Line_Value)		{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Line_Value;		}
	if(whats_This == whats_This_Sigma_Drift_Rand_Rms)		{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Rand_Rms;			}
	if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)	{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Sine_Amplitude;	}
	if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)	{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Sine_Frequency;	}
	if(whats_This == whats_This_Sigma_Drift_Sine_Phase)		{precision = line_edit_sigma_precision;		coeff = 1;						return struct_Data.sigma_Drift.drift_Sine_Phase;		}

	// multilayer
	if(whats_This == whats_This_Num_Repetitions)			{precision = 0;								coeff = 1;						return struct_Data.num_Repetition.parameter;			}
	if(whats_This == whats_This_Period)						{precision = line_edit_period_precision;	coeff = length_Coefficients_Map.value(length_units);	return struct_Data.period;		}
	if(whats_This == whats_This_Gamma)						{precision = line_edit_gamma_precision;		coeff = 1;												return struct_Data.gamma;		}

	//	qInfo() << "get_Parameter cant find parameter " << whats_This <<"" << endl;
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
					int rows = 5;
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
	if( item_Type == item_Type_Multilayer || item_Type == item_Type_Regular_Aperiodic || item_Type == item_Type_General_Aperiodic)
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
	if( item_Type == item_Type_Layer ||	item_Type == item_Type_Substrate || item_Type == item_Type_Multilayer || item_Type == item_Type_Regular_Aperiodic || item_Type == item_Type_General_Aperiodic)
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
		if( item_Type == item_Type_Layer    ||	item_Type == item_Type_Substrate || item_Type == item_Type_Multilayer)	rows = 5;

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

void Table_Of_Structures::fit_Column(QTableWidget* table, int start_Width, int current_Column)
{
	// fit column
	int max_Width = start_Width;
	for(int row = 0; row<table->rowCount(); ++row)
	{
		QLineEdit* current_Line_Edit = qobject_cast<QLineEdit*>(table->cellWidget(row, current_Column));
		MyDoubleSpinBox* current_Spin_Box = qobject_cast<MyDoubleSpinBox*>(table->cellWidget(row, current_Column));
		QCheckBox* current_CheckBox = qobject_cast<QCheckBox*>(table->cellWidget(row, current_Column));

		QLabel* current_Label = qobject_cast<QLabel*>(table->cellWidget(row, current_Column));
		bool fit_Labels = false;
		if(current_Label) fit_Labels = current_Label->property(fit_Column_Property).toBool();

		bool not_Fit_Spinbox = false;
		if(current_Spin_Box) not_Fit_Spinbox = current_Spin_Box->property(fit_Column_Property).toBool();

		if(current_Line_Edit || current_Spin_Box || fit_Labels)
		{			
			int width = 0;
			if(current_Line_Edit) width = current_Line_Edit->width();
			if(current_Spin_Box && !not_Fit_Spinbox) width = current_Spin_Box->width();
			if(fit_Labels) width = current_Label->width();
			if(max_Width<width)
			{
				max_Width = width+1;
			}
		} else
		// for item names
		{
			bool not_Fit_CheckBox = false;
			if(current_CheckBox) not_Fit_CheckBox = current_CheckBox->property(fit_Column_Property).toBool();

			if(current_CheckBox && !not_Fit_CheckBox)
			{
				const QFont& myFont = current_CheckBox->font();
				QFontMetrics fm(myFont);
				int check_Box_Width=fm.width(current_CheckBox->text());
				int shift = 25;
				int current_Width = 0;
				if(current_Column<=1) current_Width = max(COLOR_LEGEND_LABEL_WIDTH, check_Box_Width + shift);
				else                  current_Width = check_Box_Width + shift + 1;

				if(max_Width<current_Width)
				{
					max_Width = current_Width + 1;
				}
			}
		}
	}
	table->setColumnWidth(current_Column,max_Width);

	// fit other line edits
//	for(int row = 0; row<table->rowCount(); ++row)
//	{
//		QLineEdit* current_Line_Edit = qobject_cast<QLineEdit*>(table->cellWidget(row, current_Column));
//		if(current_Line_Edit)
//		if(current_Line_Edit != line_Edit)
//		{
//			current_Line_Edit->setFixedWidth(max_Width - 1);
//		}
//	}
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
		elements->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);

		elements->addItems(sorted_Elements.keys());
		elements->setCurrentIndex(elements->findText(composition[composition_Index].type));

		// number of element is here
		elements->setProperty(num_Chemic_Element_Property, composition_Index);

		// storage
		elements_Map.insert(elements, structure_Item);

		// add widget to table
		table->setCellWidget(current_Row, current_Column, elements);

		connect(elements, &QComboBox::currentTextChanged, this, [=](QString str){refresh_Element(table, str);});

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_Stoich_Line_Edit(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString val_Type)
{
	QList<Stoichiometry> composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().composition;

	int current_Column = start_Column;
	double value = -2017;

	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		Parameter& comp = composition[composition_Index].composition;
		if(val_Type == VAL)	{value = comp.value;	}
		if(val_Type == MIN)	{value = comp.fit.min;	}
		if(val_Type == MAX)	{value = comp.fit.max;	}

		// create lineedit
		MyDoubleSpinBox* spin_Box = new MyDoubleSpinBox;
			spin_Box->setRange(0, MAX_DOUBLE);
			spin_Box->setDecimals(line_edit_composition_precision);
			spin_Box->setValue(value);
			spin_Box->setAccelerated(true);
			spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
			spin_Box->setButtonSymbols(QAbstractSpinBox::NoButtons);
			spin_Box->installEventFilter(this);

		// number of element is here
		spin_Box->setProperty(num_Chemic_Element_Property, composition_Index);
		spin_Box->setProperty(min_Size_Property, spin_Box->width());
		spin_Box->setProperty(column_Property, current_Column);
		spin_Box->setProperty(value_Type_Property, val_Type);

		// for reloading
		spin_Box->setProperty(reload_Property, false);
		spin_Box->setProperty(tab_Index_Property,tab_Index);
		if(val_Type == VAL) {
			spin_Box->setProperty(id_Property, comp.indicator.id);
		}

		// storage
		spin_Boxes_ID_Map.insert(spin_Box, comp.indicator.id);
		spin_Boxes_Map.insert(spin_Box, structure_Item);
		all_Widgets_To_Reload[tab_Index].append(spin_Box);
		if(val_Type == VAL)	{
			reload_Show_Dependence_Map.insertMulti(spin_Box, comp.indicator.id);
		}

		// add widget to table
		table->setCellWidget(current_Row, current_Column, spin_Box);

		connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]{resize_Line_Edit(table,spin_Box); });
		connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]{refresh_Stoich();});
		connect(spin_Box, &MyDoubleSpinBox::editingFinished, this, [=]
		{
			for(QCheckBox* item_Check_Box : check_Boxes_Map.keys(structure_Item))
			{
				if(item_Check_Box->property(item_Table_CheckBox_Property).toString() == item_Table_CheckBox_Property)
				{
					int current_Column = item_Check_Box->property(column_Property).toInt();
					fit_Column(table, 0, current_Column);
				}
			}
		});

		spin_Box->setSingleStep(step_composition);
		composition_Spin_Boxes_List.append(spin_Box);

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
			text += Locale.toString(composition[i].composition.value, line_edit_short_double_format, thumbnail_composition_precision);
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
		check_Boxes_Fit_Map.	   insert	   (check_Box, comp.indicator.id); // here duplicated by reload_Show_Dependence_Map
		check_Boxes_Map.		   insert      (check_Box, structure_Item);
		reload_Show_Dependence_Map.insertMulti (check_Box, comp.indicator.id);
		all_Widgets_To_Reload[tab_Index].append(check_Box);

		// set up BACK widget
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		{
			back_Widget->setProperty(coupling_Editor_Property, true);	// for opening Coupling_Editor
			back_Widget->setProperty(fit_Text, fit_Text);				// for coloring
			all_Widgets_To_Reload[tab_Index].append(back_Widget);

			coupled_Back_Widget_and_Struct_Item.insert(back_Widget, structure_Item);
			coupled_Back_Widget_and_Id.			insert(back_Widget, comp.indicator.id);

			// colorize
			refresh_Reload_Colorize(colorize_Property, back_Widget, &comp);
		}	else
		{
			back_Widget->setStyleSheet("background-color: lightblue");
		}


		// add widget to table
		table->setCellWidget(current_Row, current_Column, back_Widget);

		connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Fit_Element);
		connect(check_Box, &QCheckBox::toggled, this, [=]
		{
			cells_On_Off(table);

			// colorizing working fits
			QTreeWidgetItem* item = check_Boxes_Map.value(check_Box);
			Data data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			Parameter& com = data.composition[composition_Index].composition;

			if( !com.coupled.master.exist &&
				 com.coupled.slaves.size()==0 &&
				!com.confidence.calc_Conf_Interval)
			{
				if(com.fit.is_Fitable)
				{
					back_Widget->setStyleSheet(fit_Color);
				} else
				{
					back_Widget->setStyleSheet(white_Color);
				}
			}
		});

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
		material_Line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
		material_Line_Edit->setProperty(min_Size_Property, material_Line_Edit->width());
		material_Line_Edit->setProperty(column_Property, current_Column);

	// storage
	material_Line_Edits.append(material_Line_Edit);
	line_Edits_Map.insert(material_Line_Edit, structure_Item);
	all_Widgets_To_Reload[tab_Index].append(material_Line_Edit);

	// add widget to table
	table->setCellWidget(current_Row, current_Column, material_Line_Edit);

	connect(material_Line_Edit, &QLineEdit::textEdited,	     this, [=]{resize_Line_Edit(table,material_Line_Edit); });
	connect(material_Line_Edit, &QLineEdit::textEdited,	     this, [=](QString str){ refresh_Material(table, str);} );
	connect(material_Line_Edit, &QLineEdit::editingFinished, this, [=]{check_Material();} );
}

void Table_Of_Structures::create_Browse_Button(My_Table_Widget* table, int current_Row, int start_Column, int material_LineEdit_Row, int material_LineEdit_Column)
{
	QPushButton* browse_Button = new QPushButton("Browse");
		browse_Button->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
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
	if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
	{
		if(parameter.indicator.id!=0 && whats_This!=whats_This_Num_Repetitions)
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

		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			regular_Aperiodic_Widgets_To_Reload[tab_Index].append(check_Box);

			Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			int my_I = -2019;
			for(int i = 0; i<structure_Item->parent()->childCount(); i++)
			{
				if(structure_Item->parent()->child(i)==structure_Item)
				{
					my_I = i;
					check_Box->setProperty(index_Property, i);
				}
			}

			if(whats_This == whats_This_Common_Thickness)	{bool_Check = parent_Data.regular_Components[my_I].is_Common_Thickness;}
			if(whats_This == whats_This_Common_Sigma)		{bool_Check = parent_Data.regular_Components[my_I].is_Common_Sigma;}
			if(whats_This == whats_This_Restrict_Thickness)	{bool_Check = parent_Data.regular_Components[my_I].use_Soft_Restrictions;}
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
	if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
	{
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
	} else
	{
		back_Widget->setStyleSheet("background-color: lightblue");
	}


	connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off(table); });

	if( struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
	{
		if( whats_This == whats_This_Sigma)
		{
			check_Box->setAttribute(Qt::WA_TransparentForMouseEvents);
			check_Box->setFocusPolicy(Qt::NoFocus);
		}
		if( whats_This == whats_This_Common_Thickness || whats_This == whats_This_Common_Sigma )
		{
			table->setSpan(current_Row,current_Column,1,2);
			back_Widget->setMinimumWidth(2*TABLE_FIX_WIDTH_LINE_EDIT_SHORT+1);
		}
		if( whats_This == whats_This_Restrict_Thickness )
		{
			table->setSpan(current_Row,current_Column,1,4);
			back_Widget->setMinimumWidth(4*TABLE_FIX_WIDTH_LINE_EDIT_SHORT+3);
		}
	}

	connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Check_Box_Header);

	check_Box->setProperty(reload_Property,true);
	check_Box->toggled(check_Box->isChecked());
	check_Box->setProperty(reload_Property,false);

	table->setCellWidget(current_Row, current_Column, back_Widget);
}

void Table_Of_Structures::create_Thickness_Restriction(My_Table_Widget *table, int current_Row, int current_Column, QTreeWidgetItem *structure_Item)
{
	Data regular_Aperiodic_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	int my_I = -2019;
	for(int i = 0; i<structure_Item->parent()->childCount(); i++)
	{
		if(structure_Item->parent()->child(i)==structure_Item)
		{
			my_I = i;
		}
	}

	// threshold
	QSpinBox* soft_Restriction_Threshold_SpinBox = new QSpinBox;
		soft_Restriction_Threshold_SpinBox->setRange(0, 100);
		soft_Restriction_Threshold_SpinBox->setSuffix("%");
		soft_Restriction_Threshold_SpinBox->setPrefix(Plus_Minus_Sym);
		soft_Restriction_Threshold_SpinBox->setValue(regular_Aperiodic_Data.regular_Components[my_I].threshold);
		soft_Restriction_Threshold_SpinBox->setAccelerated(true);
		soft_Restriction_Threshold_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		soft_Restriction_Threshold_SpinBox->installEventFilter(this);
	table->setCellWidget(current_Row, current_Column, soft_Restriction_Threshold_SpinBox);

	// power
	QSpinBox* soft_Restriction_Power_SpinBox = new QSpinBox;
		soft_Restriction_Power_SpinBox->setRange(0, 100);
		soft_Restriction_Power_SpinBox->setPrefix("p = ");
		soft_Restriction_Power_SpinBox->setValue(regular_Aperiodic_Data.regular_Components[my_I].power);
		soft_Restriction_Power_SpinBox->setAccelerated(true);
		soft_Restriction_Power_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		soft_Restriction_Power_SpinBox->installEventFilter(this);
	table->setCellWidget(current_Row, current_Column+1, soft_Restriction_Power_SpinBox);

	// Q value
	MyDoubleSpinBox* soft_Restriction_Q_SpinBox = new MyDoubleSpinBox;
		soft_Restriction_Q_SpinBox->setPrefix("Q = ");
		soft_Restriction_Q_SpinBox->setRange(0, MAX_DOUBLE);
		soft_Restriction_Q_SpinBox->setDecimals(8);
		soft_Restriction_Q_SpinBox->setSingleStep(Global_Variables::get_Order_Of_Magnitude(regular_Aperiodic_Data.regular_Components[my_I].Q_factor));
//		soft_Restriction_Q_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
		soft_Restriction_Q_SpinBox->setValue(regular_Aperiodic_Data.regular_Components[my_I].Q_factor);
		soft_Restriction_Q_SpinBox->setAccelerated(true);
		soft_Restriction_Q_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		soft_Restriction_Q_SpinBox->installEventFilter(this);
		soft_Restriction_Q_SpinBox->setProperty(min_Size_Property,2*TABLE_FIX_WIDTH_LINE_EDIT_SHORT+1);
		soft_Restriction_Q_SpinBox->setProperty(fit_Column_Property,true);
	table->setSpan(current_Row,current_Column+2,1,2);
	table->setCellWidget(current_Row, current_Column+2, soft_Restriction_Q_SpinBox);
	Global_Variables::resize_Line_Edit(soft_Restriction_Q_SpinBox);

	connect(soft_Restriction_Threshold_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
	{
		Data regular_Aperiodic_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		regular_Aperiodic_Data.regular_Components[my_I].threshold = soft_Restriction_Threshold_SpinBox->value();

		QVariant var;
		var.setValue( regular_Aperiodic_Data );
		structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		emit regular_Layer_Edited("especially_wrong"); // to reaload regular aperiodic item without reloading widgets
	});
	connect(soft_Restriction_Power_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
	{
		Data regular_Aperiodic_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		regular_Aperiodic_Data.regular_Components[my_I].power = soft_Restriction_Power_SpinBox->value();

		QVariant var;
		var.setValue( regular_Aperiodic_Data );
		structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		emit regular_Layer_Edited("especially_wrong"); // to reaload regular aperiodic item without reloading widgets
	});
	connect(soft_Restriction_Q_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		Data regular_Aperiodic_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		regular_Aperiodic_Data.regular_Components[my_I].Q_factor = soft_Restriction_Q_SpinBox->value();
		Global_Variables::resize_Line_Edit(soft_Restriction_Q_SpinBox);

		// adaptive step
		double order = Global_Variables::get_Order_Of_Magnitude(regular_Aperiodic_Data.regular_Components[my_I].Q_factor);;
		double difference = regular_Aperiodic_Data.regular_Components[my_I].Q_factor - order;
		if(Global_Variables::get_Order_Of_Magnitude(difference)<order)
		{
			soft_Restriction_Q_SpinBox->setSingleStep(order/10);
		} else
		{
			soft_Restriction_Q_SpinBox->setSingleStep(order);
		}

		QVariant var;
		var.setValue( regular_Aperiodic_Data );
		structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		emit regular_Layer_Edited("especially_wrong"); // to reaload regular aperiodic item without reloading widgets
	});
}

//// for all parameters
void Table_Of_Structures::create_Line_Edit(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString val_Type)
{
	// PARAMETER

	int precision = 4;
	double value = -2017;
	double coeff = 1;
	id_Type id = 0;

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	// create spinbox
	MyDoubleSpinBox* spin_Box = new MyDoubleSpinBox;
		spin_Box->setAccelerated(true);
		spin_Box->setButtonSymbols(QAbstractSpinBox::NoButtons);
		spin_Box->setRange(-100000, MAX_DOUBLE); // by default
		spin_Box->installEventFilter(this);

	if(whats_This == whats_This_Num_Repetitions)
	{
		value = struct_Data.num_Repetition.value();

		spin_Box->setDecimals(0);
		spin_Box->setSingleStep(1);
		spin_Box->setRange(0, MAX_INTEGER);

		if(struct_Data.item_Type != item_Type_Regular_Aperiodic)
		{
			id = struct_Data.num_Repetition.parameter.indicator.id;
			if(val_Type == VAL) reload_Show_Dependence_Map.insertMulti(spin_Box, id_Of_Thicknesses);
			spin_Boxes_ID_Map.insert(spin_Box,id);
		} else
		{
			spin_Box->setDisabled(true);
		}
	} else
	{
		Parameter& parameter = get_Parameter(struct_Data, whats_This, precision, coeff);
		spin_Box->setDecimals(precision);
		if(val_Type == VAL)	{value = parameter.value;	/*format = line_edit_double_format;*/	}
		if(val_Type == MIN)	{value = parameter.fit.min;	/*format = line_edit_short_double_format;*/	}
		if(val_Type == MAX)	{value = parameter.fit.max;	/*format = line_edit_short_double_format;*/	}

		if(	whats_This == whats_This_Thickness_Drift_Line_Value ||
			whats_This == whats_This_Sigma_Drift_Line_Value	)
		{
			spin_Box->setRange(-100, MAX_DOUBLE);
		} else
		{
			if(whats_This == whats_This_Gamma)
			{
				spin_Box->setRange(0, 1);
			} else
			{
				spin_Box->setRange(0, MAX_DOUBLE);
			}
		}
		id = parameter.indicator.id;

		if(val_Type == VAL) spin_Boxes_ID_Map.insert(spin_Box,id);

		if( whats_This == whats_This_Thickness	||
			whats_This == whats_This_Period		||
			whats_This == whats_This_Gamma		)
		{
			if(val_Type == VAL) reload_Show_Dependence_Map.insertMulti(spin_Box,  id_Of_Thicknesses);
		}

		if( whats_This == whats_This_Sigma )
		{
			for(Interlayer& interlayer : struct_Data.interlayer_Composition)
			{
				reload_Show_Dependence_Map.insertMulti(spin_Box,  interlayer.my_Sigma.indicator.id);
			}
			spin_Box->setProperty(forced_Reload_Property,true);
		}
	}

#ifdef _WIN32
	QFont font(spin_Box->font());
	font.setPointSize(8.25);
	font.setFamily("MS Shell Dlg 2");
	spin_Box->setFont(font);
#endif
#ifdef __linux__
#endif

	spin_Box->setValue(value/coeff);
	spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
	if( whats_This == whats_This_Absolute_Density || whats_This == whats_This_Relative_Density || whats_This == whats_This_Num_Repetitions ) spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_DENSITY);
	if( whats_This == whats_This_Sigma )                                       spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SIGMA);
	if( whats_This == whats_This_Thickness || whats_This == whats_This_Period) spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_THICKNESS);

	if(	whats_This == whats_This_Thickness_Drift_Line_Value     ||
		whats_This == whats_This_Thickness_Drift_Rand_Rms       ||
		whats_This == whats_This_Sigma_Drift_Line_Value			||
		whats_This == whats_This_Sigma_Drift_Rand_Rms			||
		whats_This == whats_This_Gamma	)						spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
	if(	whats_This == whats_This_Thickness_Drift_Sine_Amplitude ||
		whats_This == whats_This_Thickness_Drift_Sine_Frequency ||
		whats_This == whats_This_Thickness_Drift_Sine_Phase		||
		whats_This == whats_This_Sigma_Drift_Sine_Amplitude		||
		whats_This == whats_This_Sigma_Drift_Sine_Frequency		||
		whats_This == whats_This_Sigma_Drift_Sine_Phase	)		spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SIGMA);

	spin_Box->setProperty(min_Size_Property, spin_Box->width());
	spin_Box->setProperty(column_Property, current_Column);
	spin_Box->setProperty(whats_This_Property, whats_This);
	spin_Box->setProperty(value_Type_Property, val_Type);

	// for reloading
	spin_Box->setProperty(reload_Property, false);
	spin_Box->setProperty(tab_Index_Property, tab_Index);
	if(val_Type == VAL)
	{
		spin_Box->setProperty(id_Property, id);
		if(whats_This == whats_This_Period ||
		   whats_This == whats_This_Gamma )
		{
			spin_Box->setProperty(period_Gamma_Property, true);
		}
	}

	// storage
	spin_Boxes_Map.insert(spin_Box, structure_Item);
	all_Widgets_To_Reload[tab_Index].append(spin_Box);
	if(val_Type == VAL && struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
	{
		reload_Show_Dependence_Map.insertMulti(spin_Box, id);
	}
	if( val_Type == VAL &&
	   (whats_This == whats_This_Sigma || whats_This == whats_This_Thickness) &&
		struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic	)
	{
		regular_Aperiodic_Widgets_To_Reload[tab_Index].append(spin_Box);
	}

	// storage for fast min/max refreshing
	if( val_Type == MIN || val_Type == MAX )
	{
		if( whats_This == whats_This_Absolute_Density || whats_This == whats_This_Relative_Density )
		{
			min_Max_Density_Spin_Boxes_List[tab_Index].append(spin_Box);
		}
		if( whats_This == whats_This_Thickness || whats_This == whats_This_Period )
		{
			min_Max_Thickness_Spin_Boxes_List[tab_Index].append(spin_Box);
		}
		if( whats_This == whats_This_Sigma )
		{
			min_Max_Sigma_Spin_Boxes_List[tab_Index].append(spin_Box);
		}
	}

	// create item (set LineEdits_Map)
	table->setCellWidget(current_Row, current_Column, spin_Box);

	connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged),   this, [=]
	{
		refresh_Parameter(table);
		resize_Line_Edit (table,spin_Box);
	});
//	connect(spin_Box, &MyDoubleSpinBox::editingFinished, this, [=]{refresh_Parameter(table); });

	// steps
	if(whats_This == whats_This_Absolute_Density
	|| whats_This == whats_This_Relative_Density)			{ spin_Box->setSingleStep(step_density);	density_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Thickness
	|| whats_This == whats_This_Period)						{ spin_Box->setSingleStep(step_thickness);	thickness_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Sigma)						{ spin_Box->setSingleStep(step_sigma);		sigma_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Gamma)						{ spin_Box->setSingleStep(step_gamma);		gamma_Spin_Boxes_List.append(spin_Box); }
	if(	whats_This == whats_This_Thickness_Drift_Line_Value     ||
		whats_This == whats_This_Thickness_Drift_Rand_Rms       ||
		whats_This == whats_This_Thickness_Drift_Sine_Amplitude ||
		whats_This == whats_This_Sigma_Drift_Line_Value			||
		whats_This == whats_This_Sigma_Drift_Rand_Rms			||
		whats_This == whats_This_Sigma_Drift_Sine_Amplitude		)  { spin_Box->setSingleStep(step_drift);		drift_Spin_Boxes_List.append(spin_Box); }
	if(	whats_This == whats_This_Thickness_Drift_Sine_Frequency ||
		whats_This == whats_This_Thickness_Drift_Sine_Phase		||
		whats_This == whats_This_Sigma_Drift_Sine_Frequency		||
		whats_This == whats_This_Sigma_Drift_Sine_Phase	)	{ spin_Box->setSingleStep(0.1);	}
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
	if( whats_This == whats_This_Thickness &&
		struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic	)
	{
		regular_Aperiodic_Widgets_To_Reload[tab_Index].append(check_Box);
	}

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
		check_Boxes_Fit_Map.	   insert	   (check_Box, parameter.indicator.id);

		coupled_Back_Widget_and_Struct_Item.insert(back_Widget, structure_Item);
		coupled_Back_Widget_and_Id.			insert(back_Widget, parameter.indicator.id);

		// colorize
		refresh_Reload_Colorize(colorize_Property, back_Widget, &parameter);
	}

	// create item
	table->setCellWidget(current_Row, current_Column, back_Widget);

	connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Fit_Parameter);
	connect(check_Box, &QCheckBox::toggled, this, [=]
	{
		if( struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic ||
			whats_This != whats_This_Thickness)
		{
			cells_On_Off(table);
		}

		// colorizing working fits
		QTreeWidgetItem* item = check_Boxes_Map.value(check_Box);
		Data data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		Parameter& param = get_Parameter(data, whats_This);

		if(param.fit.is_Fitable && !param.coupled.master.exist)
		{
			// for all params
			if(	(param.indicator.whats_This != whats_This_Thickness_Drift_Line_Value	) &&
				(param.indicator.whats_This != whats_This_Thickness_Drift_Rand_Rms		) &&
				(param.indicator.whats_This != whats_This_Thickness_Drift_Sine_Amplitude) &&
				(param.indicator.whats_This != whats_This_Thickness_Drift_Sine_Frequency) &&
				(param.indicator.whats_This != whats_This_Thickness_Drift_Sine_Phase	) &&

				(param.indicator.whats_This != whats_This_Sigma_Drift_Line_Value		) &&
				(param.indicator.whats_This != whats_This_Sigma_Drift_Rand_Rms			) &&
				(param.indicator.whats_This != whats_This_Sigma_Drift_Sine_Amplitude	) &&
				(param.indicator.whats_This != whats_This_Sigma_Drift_Sine_Frequency	) &&
				(param.indicator.whats_This != whats_This_Sigma_Drift_Sine_Phase		)
			  )
			{
				back_Widget->setStyleSheet(fit_Color);
			} else
			// special cases
			{
				if(	(param.indicator.whats_This == whats_This_Thickness_Drift_Line_Value	&& data.thickness_Drift.is_Drift_Line) ||
					(param.indicator.whats_This == whats_This_Thickness_Drift_Rand_Rms		&& data.thickness_Drift.is_Drift_Rand) ||

					(param.indicator.whats_This == whats_This_Sigma_Drift_Line_Value		&& data.sigma_Drift.is_Drift_Line)	   ||
					(param.indicator.whats_This == whats_This_Sigma_Drift_Rand_Rms			&& data.sigma_Drift.is_Drift_Rand)
				  )
				{
					back_Widget->setStyleSheet(fit_Color);
				} else
				{
					if(	(param.indicator.whats_This == whats_This_Thickness_Drift_Sine_Amplitude&& data.thickness_Drift.is_Drift_Sine) ||
						(param.indicator.whats_This == whats_This_Thickness_Drift_Sine_Frequency&& data.thickness_Drift.is_Drift_Sine) ||
						(param.indicator.whats_This == whats_This_Thickness_Drift_Sine_Phase	&& data.thickness_Drift.is_Drift_Sine) ||

						(param.indicator.whats_This == whats_This_Sigma_Drift_Sine_Amplitude	&& data.sigma_Drift.is_Drift_Sine) ||
						(param.indicator.whats_This == whats_This_Sigma_Drift_Sine_Frequency	&& data.sigma_Drift.is_Drift_Sine) ||
						(param.indicator.whats_This == whats_This_Sigma_Drift_Sine_Phase		&& data.sigma_Drift.is_Drift_Sine)
					  )
					{
						if(	!param.coupled.master.exist &&
							 param.coupled.slaves.size()==0 &&
							!param.confidence.calc_Conf_Interval)
						{
							back_Widget->setStyleSheet(fit_Color);
						}
					} else
					{
						if(	!param.coupled.master.exist &&
							 param.coupled.slaves.size()==0 &&
							!param.confidence.calc_Conf_Interval)
						{
							back_Widget->setStyleSheet(white_Color);
						}
					}
				}
			}
		} else
		{
			// special cases
			if(back_Widget->property(fit_Text).toString() == fit_Text)
			{
				if(	!param.coupled.master.exist &&
					 param.coupled.slaves.size()==0 &&
					!param.confidence.calc_Conf_Interval)
				{
					back_Widget->setStyleSheet(white_Color);
				}
			} else
			// all parameters
			{
				back_Widget->setStyleSheet(white_Color);
			}
		}
	});

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
		reload_Show_Dependence_Map.insertMulti(check_Box,  inter_Comp.my_Sigma.indicator.id);

		check_Box->setChecked(inter_Comp.enabled);

		// alignment
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		// set up BACK widget
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		{
			back_Widget->setProperty(coupling_Editor_Property, true);		// for opening Coupling_Editor
			all_Widgets_To_Reload[tab_Index].append(back_Widget);

			coupled_Back_Widget_and_Struct_Item.insert(back_Widget, structure_Item);
			coupled_Back_Widget_and_Id.			insert(back_Widget, inter_Comp.interlayer.indicator.id);

			// colorize
			refresh_Reload_Colorize(colorize_Property, back_Widget, &inter_Comp.interlayer);
		} else
		{
			back_Widget->setStyleSheet("background-color: lightblue");
		}

		table->setCellWidget(current_Row, current_Column, back_Widget);

		connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Check_Box_Label_Interlayer);
		connect(check_Box, &QCheckBox::toggled, this, [=]
		{
			cells_On_Off(table);

			// colorizing working fits
			QTreeWidgetItem* item = check_Boxes_Map.value(check_Box);
			Data data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			Interlayer& inter_Com = data.interlayer_Composition[interlayer_Index];
			QCheckBox* fit_Check = check_Boxes_Fit_Map.key(-inter_Com.interlayer.indicator.id);// negaive "id"!
			if(fit_Check)
				fit_Check->released();
		});
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		{
			connect(check_Box, &QCheckBox::toggled, this, [=]{cells_On_Off_2(table, structure_Item); });
		}
		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_Weigts_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString val_Type)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;
	double value = -2017;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		Interlayer& inter_Comp = interlayer_Composition[interlayer_Index];

		if(val_Type == VAL)	value = inter_Comp.interlayer.value;
		if(val_Type == MIN)	value = inter_Comp.interlayer.fit.min;
		if(val_Type == MAX)	value = inter_Comp.interlayer.fit.max;

		MyDoubleSpinBox* spin_Box = new MyDoubleSpinBox;
			spin_Box->setRange(0, MAX_DOUBLE);
			spin_Box->setDecimals(line_edit_interlayer_precision);
			spin_Box->setValue(value);
			spin_Box->setAccelerated(true);
			spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
			spin_Box->setButtonSymbols(QAbstractSpinBox::NoButtons);
			spin_Box->installEventFilter(this);

		spin_Box->setProperty(interlayer_Index_Property, interlayer_Index);
		spin_Box->setProperty(min_Size_Property, spin_Box->width());
		spin_Box->setProperty(column_Property, current_Column);
		spin_Box->setProperty(value_Type_Property, val_Type);

		// for reloading
		spin_Box->setProperty(reload_Property, false);
		spin_Box->setProperty(tab_Index_Property, tab_Index);
		if(val_Type == VAL) {
			spin_Box->setProperty(id_Property, inter_Comp.interlayer.indicator.id);
		}
		// storage
		spin_Boxes_ID_Map.insert(spin_Box,inter_Comp.interlayer.indicator.id);
		spin_Boxes_Map.insert(spin_Box, structure_Item);
		all_Widgets_To_Reload[tab_Index].append(spin_Box);
		if(val_Type == VAL) {
			reload_Show_Dependence_Map.insertMulti(spin_Box, struct_Data.sigma.indicator.id);
		}

		// create item
		table->setCellWidget(current_Row, current_Column, spin_Box);

		connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]{resize_Line_Edit(table,spin_Box); });
		connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]{refresh_Weigts_Interlayer();});

		spin_Box->setSingleStep(step_interlayer);
		interlayer_Spin_Boxes_List.append(spin_Box);

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
		check_Boxes_Fit_Map.			 insert(check_Box, -inter_Comp.interlayer.indicator.id); // negaive "id"!
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
		connect(check_Box, &QCheckBox::released, this, [=]
		{
			// colorizing working fits
			QTreeWidgetItem* item = check_Boxes_Map.value(check_Box);
			Data data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			Interlayer& inter_Com = data.interlayer_Composition[interlayer_Index];

			if(inter_Com.interlayer.fit.is_Fitable && inter_Com.enabled && !inter_Com.interlayer.coupled.master.exist)
			{
				back_Widget->setStyleSheet(fit_Color);
			} else
			{
				back_Widget->setStyleSheet(white_Color);
			}
		});

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
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		{
			label->setProperty(coupling_Editor_Property, true);

			coupled_Back_Widget_and_Struct_Item.insert(label, structure_Item);
			coupled_Back_Widget_and_Id.			insert(label, my_Sigma.indicator.id);

			// colorize
			refresh_Reload_Colorize(colorize_Property, label, &my_Sigma);
		} else
		{
			label->setStyleSheet("background-color: lightblue");
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

		MyDoubleSpinBox* spin_Box = new MyDoubleSpinBox;
			spin_Box->setRange(0, MAX_DOUBLE);
			spin_Box->setDecimals(line_edit_sigma_precision);
			spin_Box->setValue(sigma_Comp.value);
			spin_Box->setAccelerated(true);
			spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
			spin_Box->setButtonSymbols(QAbstractSpinBox::NoButtons);
			spin_Box->installEventFilter(this);

		spin_Box->setProperty(interlayer_Index_Property, interlayer_Index);
		spin_Box->setProperty(min_Size_Property, spin_Box->width());
		spin_Box->setProperty(column_Property, current_Column);

		// for reloading
		spin_Box->setProperty(reload_Property, false);
		spin_Box->setProperty(tab_Index_Property, tab_Index);
		spin_Box->setProperty(id_Property, sigma_Comp.indicator.id);

		// storage
		spin_Boxes_ID_Map.insert(spin_Box,sigma_Comp.indicator.id);
		spin_Boxes_Map.insert(spin_Box, structure_Item);
		all_Widgets_To_Reload[tab_Index].append(spin_Box);
		reload_Show_Dependence_Map.insertMulti(spin_Box, sigma_Comp.indicator.id);

		// create item (set LineEdits_Map)
		table->setCellWidget(current_Row, current_Column, spin_Box);

		connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]{resize_Line_Edit(table,spin_Box); });
		connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]{refresh_MySigma_Interlayer();});

		spin_Box->setSingleStep(step_sigma);
		sigma_Spin_Boxes_List.append(spin_Box);

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_Simple_Label(My_Table_Widget* table, int current_Row, int current_Column, QString text)
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
	// PARAMETER

	add_Columns(table,current_Column);

	QPushButton* min_Max_Button = new QPushButton("Reset");
		min_Max_Button->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
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
						struct_Data.absolute_Density.fit.max = struct_Data.absolute_Density.value;//*(100+multilayer->min_Max_Density)/100;
//						struct_Data.absolute_Density.confidence.min = struct_Data.absolute_Density.fit.min;
//						struct_Data.absolute_Density.confidence.max = struct_Data.absolute_Density.fit.max;
					}
					if(struct_Data.relative_Density.fit.is_Fitable)
					{
						struct_Data.relative_Density.fit.min = struct_Data.relative_Density.value*(100-multilayer->min_Max_Density)/100;
						struct_Data.relative_Density.fit.max = struct_Data.relative_Density.value;//*(100+multilayer->min_Max_Density)/100;
//						struct_Data.relative_Density.confidence.min = struct_Data.relative_Density.fit.min;
//						struct_Data.relative_Density.confidence.max = struct_Data.relative_Density.fit.max;
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
//						struct_Data.thickness.confidence.min = struct_Data.thickness.fit.min;
//						struct_Data.thickness.confidence.max = struct_Data.thickness.fit.max;
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
//						struct_Data.period.confidence.min = struct_Data.period.fit.min;
//						struct_Data.period.confidence.max = struct_Data.period.fit.max;
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
//						struct_Data.sigma.confidence.min = struct_Data.sigma.fit.min;
//						struct_Data.sigma.confidence.max = struct_Data.sigma.fit.max;
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
		spin_Box->setRange(0, 100);
		spin_Box->setDecimals(3);
		spin_Box->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);

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
		spin_Box->setSingleStep(1);
		spin_Box->setButtonSymbols(QAbstractSpinBox::NoButtons);

	table->setCellWidget(current_Row, current_Column, spin_Box);
	connect(spin_Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		if(whats_This == whats_This_Density)	{ multilayer->min_Max_Density = spin_Box->value();	 }
		if(whats_This == whats_This_Thickness)	{ multilayer->min_Max_Thickness = spin_Box->value(); }
		if(whats_This == whats_This_Sigma)		{ multilayer->min_Max_Sigma = spin_Box->value();	 }
	});
}

void Table_Of_Structures::create_Step_Spin_Box(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This)
{
	// PARAMETER

	add_Columns(table,current_Column);

	QDoubleSpinBox* step_SpinBox = new QDoubleSpinBox;
		step_SpinBox->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
		step_SpinBox->setRange(0, MAX_DOUBLE);
		step_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		step_SpinBox->setAccelerated(true);
		step_SpinBox->setProperty(column_Property,current_Column);
		step_SpinBox->installEventFilter(this);
		step_SpinBox->setDecimals(10);

	table->setCellWidget(current_Row, current_Column, step_SpinBox);
	Global_Variables::resize_Line_Edit(step_SpinBox);

		int min_Width = 0;
		if(whats_This == whats_This_Composition)			{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;	}
		if(whats_This == whats_This_Density)				{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_DENSITY;}
		if(whats_This == whats_This_Thickness)				{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_THICKNESS;	}
		if(whats_This == whats_This_Sigma)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SIGMA;	}
		if(whats_This == whats_This_Interlayer_Composition)	{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;	}
		if(whats_This == whats_This_Gamma)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_LONG; step_SpinBox->setRange(0, 0.5);}
		if(whats_This == whats_This_Drift)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_LONG;	}
		step_SpinBox->setFixedWidth(min_Width);
		step_SpinBox->setProperty(min_Size_Property, step_SpinBox->width());

	connect(step_SpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		double length_Coeff = length_Coefficients_Map.value(length_units);

		if(whats_This == whats_This_Composition)			{ step_composition = step_SpinBox->value();				for(MyDoubleSpinBox* spb : composition_Spin_Boxes_List) spb->setSingleStep(step_composition);			}
		if(whats_This == whats_This_Density)				{ step_density = step_SpinBox->value();					for(MyDoubleSpinBox* spb : density_Spin_Boxes_List)     spb->setSingleStep(step_density);				}
		if(whats_This == whats_This_Thickness)				{ step_thickness = step_SpinBox->value()*length_Coeff;  for(MyDoubleSpinBox* spb : thickness_Spin_Boxes_List)	spb->setSingleStep(step_thickness/length_Coeff);}
		if(whats_This == whats_This_Sigma)					{ step_sigma = step_SpinBox->value()*length_Coeff;      for(MyDoubleSpinBox* spb : sigma_Spin_Boxes_List)		spb->setSingleStep(step_sigma/length_Coeff);	}
		if(whats_This == whats_This_Interlayer_Composition)	{ step_interlayer = step_SpinBox->value();				for(MyDoubleSpinBox* spb : interlayer_Spin_Boxes_List)	spb->setSingleStep(step_interlayer);			}
		if(whats_This == whats_This_Gamma)					{ step_gamma = step_SpinBox->value();					for(MyDoubleSpinBox* spb : gamma_Spin_Boxes_List)		spb->setSingleStep(step_gamma);					}
		if(whats_This == whats_This_Drift)					{ step_drift = step_SpinBox->value();					for(MyDoubleSpinBox* spb : drift_Spin_Boxes_List)		spb->setSingleStep(step_drift);					}

		resize_Line_Edit(table, step_SpinBox);
	});

	// refresh in each table
	connect(main_Tabs, &QTabWidget::tabBarClicked, this,  [=]
	{
		double length_Coeff = length_Coefficients_Map.value(length_units);
		int add_Decimals = min(log10(length_Coeff),2.);
		double min_Step = max(0.1/length_Coeff,0.0001);

		if(whats_This == whats_This_Composition)			{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_composition);				step_SpinBox->setDecimals(3);					step_SpinBox->setSingleStep(0.1);		}
		if(whats_This == whats_This_Density)				{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_density);					step_SpinBox->setDecimals(3);					step_SpinBox->setSingleStep(0.1);		}
		if(whats_This == whats_This_Thickness)				{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_thickness/length_Coeff);	step_SpinBox->setDecimals(2+add_Decimals);	    step_SpinBox->setSingleStep(min_Step);	step_SpinBox->setSuffix(" "+length_units);}
		if(whats_This == whats_This_Sigma)					{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_sigma/length_Coeff);		step_SpinBox->setDecimals(2+add_Decimals);	    step_SpinBox->setSingleStep(min_Step);	step_SpinBox->setSuffix(" "+length_units);}
		if(whats_This == whats_This_Interlayer_Composition)	{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_interlayer);				step_SpinBox->setDecimals(3);					step_SpinBox->setSingleStep(0.1);		}
		if(whats_This == whats_This_Gamma)					{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_gamma);						step_SpinBox->setDecimals(3);					step_SpinBox->setSingleStep(0.01);		}
		if(whats_This == whats_This_Drift)					{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_drift);						step_SpinBox->setDecimals(4);					step_SpinBox->setSingleStep(0.001);		}

		step_SpinBox->valueChanged(step_SpinBox->value());
	});

	// initialize
	main_Tabs->tabBarClicked(tab_Index);
}

void Table_Of_Structures::spin_Box_Recalculate(My_Table_Widget *table, int current_Row, int current_Column)
{
	add_Columns(table,current_Column);

	QCheckBox* checkbox_Recalculate = new QCheckBox("Recalculate");
		checkbox_Recalculate->setChecked(recalculate_Spinbox_Table);
	table->setCellWidget(current_Row, current_Column, checkbox_Recalculate);

	connect(checkbox_Recalculate, &QCheckBox::toggled, this, [=]
	{
		recalculate_Spinbox_Table = checkbox_Recalculate->isChecked();
		if(recalculate_Spinbox_Table)
			checkbox_Recalculate->setStyleSheet("QWidget { background: rgb(245, 228, 158); }");
		else
			checkbox_Recalculate->setStyleSheet("background-color: white");
	});

	// refresh in each table
	connect(main_Tabs, &QTabWidget::tabBarClicked, this, [=]
	{
		checkbox_Recalculate->setChecked(recalculate_Spinbox_Table);
	});
	checkbox_Recalculate->toggled(recalculate_Spinbox_Table);
}

void Table_Of_Structures::spin_Box_Mouse_Wheel(My_Table_Widget *table, int current_Row, int current_Column)
{
	add_Columns(table,current_Column);

	QCheckBox* checkbox_Mouse_Wheel = new QCheckBox("Mouse Wheel");
		checkbox_Mouse_Wheel->setChecked(mouse_Wheel_Spinbox_Table);
	table->setCellWidget(current_Row, current_Column, checkbox_Mouse_Wheel);

	connect(checkbox_Mouse_Wheel, &QCheckBox::toggled, this, [=]
	{
		mouse_Wheel_Spinbox_Table = checkbox_Mouse_Wheel->isChecked();
		if(mouse_Wheel_Spinbox_Table)
			checkbox_Mouse_Wheel->setStyleSheet("QWidget { background: rgb(255, 170, 137); }");
		else
			checkbox_Mouse_Wheel->setStyleSheet("background-color: white");
	});

	// refresh in each table
	connect(main_Tabs, &QTabWidget::tabBarClicked, this, [=]
	{
		checkbox_Mouse_Wheel->setChecked(mouse_Wheel_Spinbox_Table);
	});
	checkbox_Mouse_Wheel->toggled(mouse_Wheel_Spinbox_Table);
}

void Table_Of_Structures::spin_Box_Change_Dependent(My_Table_Widget* table, int current_Row, int current_Column)
{
	add_Columns(table,current_Column);

	QCheckBox* checkbox_Dependent = new QCheckBox("Change Dependent");
		checkbox_Dependent->setChecked(refill_Dependent_Table);
	table->setCellWidget(current_Row, current_Column, checkbox_Dependent);

	connect(checkbox_Dependent, &QCheckBox::toggled, this, [=]
	{
		refill_Dependent_Table = checkbox_Dependent->isChecked();

		// immediately refill all tables
		refill_All_Dependent();

		if(refill_Dependent_Table)
			checkbox_Dependent->setStyleSheet("QWidget { background: rgb(180, 255, 150); }");
		else
			checkbox_Dependent->setStyleSheet("background-color: white");
	});

	// refresh in each table
	connect(main_Tabs, &QTabWidget::tabBarClicked, this, [=]
	{
		checkbox_Dependent->setChecked(refill_Dependent_Table);
	});

	// just colorize
	if(refill_Dependent_Table)
		checkbox_Dependent->setStyleSheet("QWidget { background: rgb(180, 255, 150); }");
	else
		checkbox_Dependent->setStyleSheet("background-color: white");
}

void Table_Of_Structures::refill_All_Dependent()
{
	QVector<Parameter> master_Parameters; // real master values should be changed before all refilling operations. Here just copies

	// find all top masters
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
		real_Tree_Iteration(multilayer->structure_Tree->tree, master_Parameters);
	}

	// change dependent chain
	QVector<id_Type> ids;
	for(Parameter& master_Parameter : master_Parameters)
	{
		change_Slaves_in_Structure_Tree(master_Parameter, master_Parameter.coupled.slaves, ids);
	}

//	// refresh table and independent in all tabs
	refresh_Dependents(ids, true);

//	// lock and unlock
	lock_Unlock_Dependents(ids);
}

void Table_Of_Structures::real_Tree_Iteration(QTreeWidget* real_Struct_Tree, QVector<Parameter>& master_Parameters)
{
	QTreeWidgetItem* structure_Item;
	QTreeWidgetItemIterator it(real_Struct_Tree);
	while (*it)
	{
		structure_Item = *it;
		Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		if(struct_Data.parent_Item_Type!=item_Type_Regular_Aperiodic)
		{
			// look for pure masters
			struct_Data.fill_Potentially_Fitable_Parameters_Vector();
			for(Parameter* parameter : struct_Data.potentially_Fitable_Parameters)
			{
				// pure masters only
				if(!parameter->coupled.master.exist && parameter->coupled.slaves.size()>0)
				{
					master_Parameters.append(*parameter);
				}
			}
		}
		++it;
	}
}

void Table_Of_Structures::change_Slaves_in_Structure_Tree(Parameter& master, const QVector<Parameter_Indicator>& slaves, QVector<id_Type>& ids)
{
	for(const Parameter_Indicator& slave_Indicator : slaves)
	{
		ids.append(slave_Indicator.id);

		QTreeWidgetItem* slave_Structure_Item = get_Struct_Item_From_Multilayer_by_Id (slave_Indicator.id);
		Data slave_Struct_Data = slave_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		Parameter* slave_Parameter = Global_Variables::get_Parameter_From_Struct_Item_by_Id(slave_Struct_Data, slave_Indicator.id);

		if(refill_Dependent_Table)
		{
#ifdef EXPRTK
			// local parsing
			exprtk::parser<double> parser;
			exprtk::symbol_table<double> symbol_table;
			exprtk::expression<double> expression_Exprtk;
			symbol_table.add_variable(expression_Master_Slave_Variable, master.value);
			symbol_table.add_constants();

			expression_Exprtk.register_symbol_table(symbol_table);
			parser.compile(slave_Parameter->coupled.master.expression.toStdString(), expression_Exprtk);

			slave_Parameter->value = expression_Exprtk.value();
#else
			slave_Parameter->value = master.value;
#endif
			// refresh my_sigmas if you are common_sigma
			if(slave_Parameter->indicator.whats_This == whats_This_Sigma && slave_Struct_Data.common_Sigma)
			{
				for(Interlayer& interlayer : slave_Struct_Data.interlayer_Composition)
				{
					interlayer.my_Sigma.value = slave_Struct_Data.sigma.value;
				}
			}

			// refresh common_sigma if you are my_sigma
			if(slave_Parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma && !slave_Struct_Data.common_Sigma )
			{
				{
					slave_Struct_Data.sigma.value = recalculate_Sigma_From_Individuals(slave_Struct_Data.interlayer_Composition);
				}
			}

			// save
			QVariant var;
			var.setValue( slave_Struct_Data );
			slave_Structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			if(slave_Parameter->indicator.whats_This == whats_This_Thickness &&
				slave_Struct_Data.parent_Item_Type == item_Type_Multilayer	)
			{
				Global_Variables::change_Parent_Period_Gamma_Thickness(slave_Structure_Item);
			}
		}

		// we need to go deeper
		change_Slaves_in_Structure_Tree(*slave_Parameter, slave_Parameter->coupled.slaves, ids);
	}
}

void Table_Of_Structures::refresh_Dependents(const QVector<id_Type>& ids, bool manual_Refresh, bool reload_Period_Gamma)
{
	for(int tab_Index=0; tab_Index<main_Tabs->count(); ++tab_Index)
	{
		for(int i=0; i<all_Widgets_To_Reload[tab_Index].size(); ++i)
		{
			QWidget* widget_To_Reload = all_Widgets_To_Reload[tab_Index][i];
			MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(widget_To_Reload);

			if(spin_Box)
			{
				// reload only lineedits-slaves;
				id_Type slave_ID = widget_To_Reload->property(id_Property).toInt();
				if(ids.contains(slave_ID))
				{
					if(refill_Dependent_Table)
					{
						reload_One_Widget(widget_To_Reload);
					}
				}

				// reload period and gamma
				if(widget_To_Reload->property(period_Gamma_Property).toBool() && reload_Period_Gamma)
				{
					if(refill_Dependent_Table)
					{
						reload_One_Widget(widget_To_Reload);
					}
				}
			}
		}

		if(refill_Dependent_Table && manual_Refresh)
		{
			Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
			multilayer->structure_Tree->refresh__StructureTree__Data_and_Text();
			for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
			{
				Independent_Variables* independent = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));
				independent->reset_Independent_Variables_Structure();
			}
		}
	}
}

void Table_Of_Structures::lock_Unlock_Dependents(const QVector<id_Type>& ids)
{
	for(int tab_Index=0; tab_Index<main_Tabs->count(); ++tab_Index)
	{
		for(int i=0; i<all_Widgets_To_Reload[tab_Index].size(); ++i)
		{
			QWidget* widget_To_Reload = all_Widgets_To_Reload[tab_Index][i];
			MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(widget_To_Reload);

			if(spin_Box)
			{
				// lock, unlock and reload only lineedits-slaves;
				id_Type id = widget_To_Reload->property(id_Property).toInt();
				if(ids.contains(id))
				{
					if(refill_Dependent_Table)	spin_Box->setReadOnly(true);
					else						spin_Box->setReadOnly(false);
				} else
				{
					// make editable ex-slaves
					if(id>0 && !widget_To_Reload->property(period_Gamma_Property).toBool())
					{
						spin_Box->setReadOnly(false);
					}
				}

				// lock unlock period and gamma
				if(widget_To_Reload->property(period_Gamma_Property).toBool())
				{
					QString whats_This = spin_Box->property(whats_This_Property).toString();
					if(whats_This == whats_This_Period || whats_This == whats_This_Gamma)
					{
						QTreeWidgetItem* multilayer_Item = get_Struct_Item_From_Multilayer_by_Id(id);
						bool has_Dependent = false;
						check_Multilayer_Item_For_Dependent_Thicknesses(multilayer_Item, has_Dependent);

						QCheckBox* item_Check_Box = item_Check_Boxes_Map.value(multilayer_Item);
						if(has_Dependent) {spin_Box->setReadOnly(true ); item_Check_Box->setProperty(multilayer_Item_Table_CheckBox_Property, "wrong");                                }
						else			  {spin_Box->setReadOnly(false); item_Check_Box->setProperty(multilayer_Item_Table_CheckBox_Property, multilayer_Item_Table_CheckBox_Property);}
					}
				}
			}
		}
	}
}

void Table_Of_Structures::check_Multilayer_Item_For_Dependent_Thicknesses(QTreeWidgetItem* multilayer_Item, bool& has_Dependent)
{
	for(int i=0; i<multilayer_Item->childCount(); i++)
	{
		QTreeWidgetItem* child_Item = multilayer_Item->child(i);
		if(child_Item->childCount()==0)
		{
			Data child_Data = child_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			if(child_Data.item_Type == item_Type_Layer)
			{
				if(child_Data.thickness.coupled.master.exist && refill_Dependent_Table)
				{
					has_Dependent = true;
					return;
				}
			}
		} else
		// go deeper
		{
			check_Multilayer_Item_For_Dependent_Thicknesses(child_Item, has_Dependent);
		}
	}
}

QTreeWidgetItem* Table_Of_Structures::get_Struct_Item_From_Multilayer_by_Id(id_Type id)
{
	MyDoubleSpinBox* spin_Box = spin_Boxes_ID_Map.key(id);
	QTreeWidgetItem* structure_Item = spin_Boxes_Map.value(spin_Box);
	return structure_Item;

	// old style, slow. reserve realization
//	for(int tab_Index=0; tab_Index<global_Multilayer_Approach->multilayer_Tabs->count(); ++tab_Index)
//	{
//		Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));

//		QTreeWidgetItem* structure_Item;
//		QTreeWidgetItemIterator it(multilayer->structure_Tree->tree);
//		while (*it)
//		{
//			structure_Item = *it;
//			Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
//			Parameter* parameter = Global_Variables::get_Parameter_From_Struct_Item_by_Id(struct_Data, id);
//			if(parameter!=nullptr)
//			{
//				return structure_Item;
//			}
//			++it;
//		}
//	}
//	return nullptr;
}

//// refresh

//// for material only
void Table_Of_Structures::refresh_Element(My_Table_Widget* table, QString)
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
		for(QCheckBox* item_Check_Box : check_Boxes_Map.keys(structure_Item))
		{
			if(item_Check_Box->property(item_Table_CheckBox_Property).toString() == item_Table_CheckBox_Property)
			{
				item_Check_Box->setText(Global_Variables::structure_Item_Name(struct_Data));
				int current_Column = item_Check_Box->property(column_Property).toInt();
				fit_Column(table, 0, current_Column);
			}
		}

		// full name update
		comp.composition.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Composition, composition_Index);

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			int my_I = -2019;
			for(int i=0; i<structure_Item->parent()->childCount(); i++)	{
				if(structure_Item->parent()->child(i)==structure_Item)	{
					my_I = i;
				}
			}

			Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			for(int n=0; n<parent_Data.num_Repetition.value(); n++)
			{
				Data& regular_Data = parent_Data.regular_Components[my_I].components[n];
				Parameter& regular_Comp = regular_Data.composition[composition_Index].composition;
				regular_Data.composition[composition_Index].type = combo_Box->currentText();
				regular_Data.material = material_From_Composition(regular_Data.composition);

				// full name update
				regular_Comp.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Composition, composition_Index);
			}

			QVariant var;
			var.setValue( parent_Data );
			structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			emit regular_Layer_Edited(material_Identifier);
		}

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
	}
}

void Table_Of_Structures::refresh_Stoich()
{
	MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = spin_Boxes_Map.value(spin_Box);
	int composition_Index = spin_Box->property(num_Chemic_Element_Property).toInt();
	QString value_Type = spin_Box->property(value_Type_Property).toString();

	// for reloading
	bool reload = spin_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& comp = struct_Data.composition[composition_Index].composition;

	// reload state from struct_Tree
	if(reload)
	{
		spin_Box->blockSignals(true);
		if(value_Type == VAL)	spin_Box->setValue(comp.value);
		if(value_Type == MIN)	spin_Box->setValue(comp.fit.min);
		if(value_Type == MAX)	spin_Box->setValue(comp.fit.max);
		spin_Box->blockSignals(false);

		return;
	}
	// refresh struct_Tree
	{
		if(value_Type == VAL)	{comp.value   = spin_Box->value(); }
		if(value_Type == MIN)	{comp.fit.min = spin_Box->value(); /*comp.confidence.min = comp.fit.min;*/}
		if(value_Type == MAX)	{comp.fit.max = spin_Box->value(); /*comp.confidence.max = comp.fit.max;*/}

		struct_Data.material = material_From_Composition(struct_Data.composition);
		for(QCheckBox* item_Check_Box : check_Boxes_Map.keys(structure_Item))
		{
			if(item_Check_Box->property(item_Table_CheckBox_Property).toString() == item_Table_CheckBox_Property)
			{
				item_Check_Box->setText(Global_Variables::structure_Item_Name(struct_Data));
//				int current_Column = item_Check_Box->property(column_Property).toInt();
//				fit_Column(table, 0, current_Column);
			}
		}

		// full name update
		comp.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Composition, composition_Index);

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			int my_I = -2019;
			for(int i=0; i<structure_Item->parent()->childCount(); i++)	{
				if(structure_Item->parent()->child(i)==structure_Item)	{
					my_I = i;
				}
			}

			Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			for(int n=0; n<parent_Data.num_Repetition.value(); n++)
			{
				Data& regular_Data = parent_Data.regular_Components[my_I].components[n];
				Parameter& regular_Comp = regular_Data.composition[composition_Index].composition;
				if(value_Type == VAL)	regular_Comp.value   = spin_Box->value();
				if(value_Type == MIN)	regular_Comp.fit.min = spin_Box->value();
				if(value_Type == MAX)	regular_Comp.fit.max = spin_Box->value();

				regular_Data.material = material_From_Composition(regular_Data.composition);

				// full name update
				regular_Comp.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Composition, composition_Index);
			}

			QVariant var;
			var.setValue( parent_Data );
			structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			emit regular_Layer_Edited(material_Identifier);
		}

		// we should save before changing dependent
		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// change dependent if necessary
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		if( refill_Dependent_Table &&
			!comp.coupled.master.exist && comp.coupled.slaves.size()>0 )
		{
			// change dependent chain
			QVector<id_Type> ids;
			change_Slaves_in_Structure_Tree(comp, comp.coupled.slaves, ids);

			// refresh table and independent in all tabs
			refresh_Dependents(ids);
		}
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());

		// recalculation at change
		if(recalculate_Spinbox_Table && !reload && value_Type == VAL) {global_Multilayer_Approach->calc_Reflection(true);}
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

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			int my_I = -2019;
			for(int i=0; i<structure_Item->parent()->childCount(); i++)	{
				if(structure_Item->parent()->child(i)==structure_Item)	{
					my_I = i;
				}
			}

			Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			for(int n=0; n<parent_Data.num_Repetition.value(); n++)
			{
				Data& regular_Data = parent_Data.regular_Components[my_I].components[n];
				Parameter& regular_Comp = regular_Data.composition[composition_Index].composition;

				regular_Comp.fit.is_Fitable = check_Box->isChecked();
			}

			QVariant var;
			var.setValue( parent_Data );
			structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			emit regular_Layer_Edited("especially_wrong"); // to reaload regular aperiodic item without reloading widgets
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

void Table_Of_Structures::refresh_Material(My_Table_Widget* table, QString)
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
		for(QCheckBox* item_Check_Box : check_Boxes_Map.keys(structure_Item))
		{
			if(item_Check_Box->property(item_Table_CheckBox_Property).toString() == item_Table_CheckBox_Property)
			{
				item_Check_Box->setText(Global_Variables::structure_Item_Name(struct_Data));
				int current_Column = item_Check_Box->property(column_Property).toInt();
				fit_Column(table, 0, current_Column);
			}
		}

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			int my_I = -2019;
			for(int i=0; i<structure_Item->parent()->childCount(); i++)	{
				if(structure_Item->parent()->child(i)==structure_Item)	{
					my_I = i;
				}
			}

			Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			for(int n=0; n<parent_Data.num_Repetition.value(); n++)
			{
				Data& regular_Data = parent_Data.regular_Components[my_I].components[n];
				regular_Data.material = line_Edit->text();
			}

			QVariant var;
			var.setValue( parent_Data );
			structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			emit regular_Layer_Edited(material_Identifier);
		}

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
	}
}

void Table_Of_Structures::check_Material(QLineEdit* line_Edit)
{
	if(line_Edit == nullptr)
	{
		line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	}
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
			line_Edit->blockSignals(true);
			QMessageBox::information(this, "Wrong material", "File \"" + struct_Data.material + nk_Ext + "\" not found");
			line_Edit->blockSignals(false);
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

		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			int i = check_Box->property(index_Property).toInt();
			if(whats_This == whats_This_Common_Thickness)	{check_Box->setChecked(parent_Data.regular_Components[i].is_Common_Thickness);	}
			if(whats_This == whats_This_Common_Sigma)		{check_Box->setChecked(parent_Data.regular_Components[i].is_Common_Sigma);		}
			if(whats_This == whats_This_Restrict_Thickness)	{check_Box->setChecked(parent_Data.regular_Components[i].use_Soft_Restrictions);}
			colorize_Regular_Aperiodic_Check_Box_Header(check_Box);
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

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			int i = check_Box->property(index_Property).toInt();
			if(whats_This == whats_This_Common_Thickness)
			{
				parent_Data.regular_Components[i].is_Common_Thickness   = check_Box->isChecked();

				for(int n=0; n<parent_Data.num_Repetition.value(); n++)
				{
					Data& regular_Data = parent_Data.regular_Components[i].components[n];

					if(parent_Data.regular_Components[i].is_Common_Thickness)
					{
						// set common value
						regular_Data.thickness.value = struct_Data.thickness.value;

						// add to slave list (if still not)
						if(!struct_Data.thickness.coupled.slaves.contains(regular_Data.thickness.indicator)) {
							struct_Data.thickness.coupled.slaves.append  (regular_Data.thickness.indicator);
						}
						// set as master (if still not)
						if(!(regular_Data.thickness.coupled.master == struct_Data.thickness.indicator)) {
							 regular_Data.thickness.coupled.master =  struct_Data.thickness.indicator;
							 regular_Data.thickness.coupled.master.exist = true;
						}
					} else
					// remove slaves and master
					{
						// remove from slave list (if still there)
						if( struct_Data.thickness.coupled.slaves.contains (regular_Data.thickness.indicator)) {
							struct_Data.thickness.coupled.slaves.removeOne(regular_Data.thickness.indicator);
						}
						// remove master (if still there)
						if( regular_Data.thickness.coupled.master == struct_Data.thickness.indicator) {
							regular_Data.thickness.coupled.master.id = 0;
							regular_Data.thickness.coupled.master.exist = false;
						}
					}
				}
			}
			if(whats_This == whats_This_Common_Sigma)
			{
				parent_Data.regular_Components[i].is_Common_Sigma       = check_Box->isChecked();

				for(int n=0; n<parent_Data.num_Repetition.value(); n++)
				{
					Data& regular_Data = parent_Data.regular_Components[i].components[n];

					if(parent_Data.regular_Components[i].is_Common_Sigma)
					{
						// set common value
						regular_Data.sigma.value = struct_Data.sigma.value;

						// add to slave list (if still not)
						if(!struct_Data.sigma.coupled.slaves.contains(regular_Data.sigma.indicator)) {
							struct_Data.sigma.coupled.slaves.append  (regular_Data.sigma.indicator);
						}
						// set as master (if still not)
						if(!(regular_Data.sigma.coupled.master == struct_Data.sigma.indicator)) {
							 regular_Data.sigma.coupled.master =  struct_Data.sigma.indicator;
							 regular_Data.sigma.coupled.master.exist = true;
						}
					} else
					// remove slaves and master
					{
						// remove from slave list (if still there)
						if( struct_Data.sigma.coupled.slaves.contains (regular_Data.sigma.indicator)) {
							struct_Data.sigma.coupled.slaves.removeOne(regular_Data.sigma.indicator);
						}
						// remove master (if still there)
						if( regular_Data.sigma.coupled.master == struct_Data.sigma.indicator) {
							regular_Data.sigma.coupled.master.id = 0;
							regular_Data.sigma.coupled.master.exist = false;
						}
					}
				}
			}
			if(whats_This == whats_This_Restrict_Thickness)	{parent_Data.regular_Components[i].use_Soft_Restrictions = check_Box->isChecked();}
			colorize_Regular_Aperiodic_Check_Box_Header(check_Box);

			QVariant var;
			var.setValue( parent_Data );
			structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			emit regular_Layer_Edited(colorize_Property);
		}

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());

		//	recalculation at change
		if(recalculate_Spinbox_Table && !reload) {global_Multilayer_Approach->calc_Reflection(true);}
	}
}

void Table_Of_Structures::colorize_Regular_Aperiodic_Check_Box_Header(QCheckBox* check_Box)
{
	QString whats_This = check_Box->property(whats_This_Property).toString();

	if(whats_This == whats_This_Common_Thickness)
	{
		if(check_Box->isChecked())
			check_Box->parentWidget()->setStyleSheet(common_Thickness_Color);
		else
			check_Box->parentWidget()->setStyleSheet(white_Color);
	}
	if(whats_This == whats_This_Common_Sigma)
	{
		if(check_Box->isChecked())
			check_Box->parentWidget()->setStyleSheet(common_Sigma_Color);
		else
			check_Box->parentWidget()->setStyleSheet(white_Color);
	}
	if(whats_This == whats_This_Restrict_Thickness)
	{
		if(check_Box->isChecked())
			check_Box->parentWidget()->setStyleSheet(soft_Restriction_Color);
		else
			check_Box->parentWidget()->setStyleSheet(white_Color);
	}
}

void Table_Of_Structures::change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, const double factor)
{
	Data struct_Data = multilayer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if( multilayer_Item->childCount())
	{
		struct_Data.period.value *= factor;

		if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
		{
			for(int i=0; i<multilayer_Item->childCount(); ++i)
			{
				for(int n=0; n<struct_Data.num_Repetition.value(); n++)
				{
					struct_Data.regular_Components[i].components[n].thickness.value *= factor;
				}
				struct_Data.regular_Components[i].find_Min_Max_Values();
			}
		}

		for(int i=0; i<multilayer_Item->childCount(); ++i)
		{
			change_Child_Layers_Thickness(multilayer_Item->child(i), factor);
		}
	}
	if(struct_Data.item_Type == item_Type_Layer)
	{
		struct_Data.thickness.value *= factor;
	}

	QVariant var;
	var.setValue( struct_Data );
	multilayer_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	// change dependent if necessary
	if(struct_Data.item_Type == item_Type_Layer)
	{
		if( refill_Dependent_Table &&
			!struct_Data.thickness.coupled.master.exist && struct_Data.thickness.coupled.slaves.size()>0 )
		{
			// change dependent chain
			QVector<id_Type> ids;
			change_Slaves_in_Structure_Tree(struct_Data.thickness, struct_Data.thickness.coupled.slaves, ids);

			// refresh table and independent in all tabs
			refresh_Dependents(ids, false, false);
		}
	}

	emit regular_Layer_Edited(QString(whats_This_Thickness)+VAL);
}

void Table_Of_Structures::reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness)
{
	Data stack_Content = multilayer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	double factor=1;
	if( abs(stack_Content.period.value) > DBL_EPSILON && stack_Content.num_Repetition.value()!=0)
	{
		factor = new_Thickness/(  stack_Content.period.value*stack_Content.num_Repetition.value()  );
		change_Child_Layers_Thickness(multilayer_Item, factor);
	}
}

void Table_Of_Structures::reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness)
{
	QVariant var;
	Data layer = layer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	layer.thickness.value = new_Thickness;
	var.setValue( layer );
	layer_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	// change dependent if necessary
	if( refill_Dependent_Table &&
		!layer.thickness.coupled.master.exist && layer.thickness.coupled.slaves.size()>0 )
	{
		// change dependent chain
		QVector<id_Type> ids;
		change_Slaves_in_Structure_Tree(layer.thickness, layer.thickness.coupled.slaves, ids);

		// refresh table and independent in all tabs
		refresh_Dependents(ids, false, false);
	}
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
	// save data individually! be careful

	MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = spin_Boxes_Map.value(spin_Box);
	QString value_Type = spin_Box->property(value_Type_Property).toString();
	QString whats_This = spin_Box->property(whats_This_Property).toString();

	int precision = 4;
	double coeff = 1;

	// for reloading
	bool reload = spin_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(whats_This == whats_This_Num_Repetitions)
	{
		if(reload)
		{
			spin_Box->blockSignals(true);
			if(value_Type == VAL)    spin_Box->setValue(struct_Data.num_Repetition.value());
			if(value_Type == MIN)    spin_Box->setValue(round(struct_Data.num_Repetition.parameter.fit.min));
			if(value_Type == MAX)    spin_Box->setValue(round(struct_Data.num_Repetition.parameter.fit.max));
			spin_Box->blockSignals(false);
			return;
		}
		// if refresh
		{
			if(value_Type == VAL)   struct_Data.num_Repetition.parameter.value   = spin_Box->value();
			if(value_Type == MIN)	struct_Data.num_Repetition.parameter.fit.min = spin_Box->value();
			if(value_Type == MAX)	struct_Data.num_Repetition.parameter.fit.max = spin_Box->value();

			QVariant var;
			var.setValue( struct_Data );
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			// parents
			Global_Variables::change_Parent_Period_Gamma_Thickness(structure_Item);

			if(layer_Thickness_Transfer_Is_Created)
				layer_Thickness_Transfer->lock_Unlock_Thickness_Transfer(structure_Item);
		}

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	} else
	{
		Parameter& parameter = get_Parameter(struct_Data, whats_This, precision, coeff);

		if(reload)
		{
			spin_Box->blockSignals(true);
			spin_Box->setDecimals(precision);

			if(value_Type == VAL)
			{
				// special cases
				if(whats_This == whats_This_Sigma)
				{
					spin_Box->setValue(parameter.value/coeff);
					spin_Box->setDisabled(!struct_Data.common_Sigma);
					spin_Box->setStyleSheet("border: 1px solid grey");

					for(Interlayer& interlayer : struct_Data.interlayer_Composition)
					{
						if(struct_Data.common_Sigma)
						{
							interlayer.my_Sigma.value = struct_Data.sigma.value;
						}
						interlayer.my_Sigma.fit.min = struct_Data.sigma.fit.min;
						interlayer.my_Sigma.fit.max = struct_Data.sigma.fit.max;
//						interlayer.my_Sigma.confidence.min = interlayer.my_Sigma.fit.min;
//						interlayer.my_Sigma.confidence.max = interlayer.my_Sigma.fit.max;
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
						spin_Box->setValue(struct_Data.gamma.value);
						spin_Box->setDisabled(false);
						spin_Box->setStyleSheet("border: 1px solid grey");
					} else
					{
						spin_Box->setDisabled(true);
						spin_Box->setStyleSheet("border: 0px solid grey");
					}
				} else
				{
					spin_Box->setValue(parameter.value/coeff);
				}
			}
			if(value_Type == MIN)   spin_Box->setValue(parameter.fit.min/coeff);
			if(value_Type == MAX)   spin_Box->setValue(parameter.fit.max/coeff);

			spin_Box->blockSignals(false);
			return;
		}
		// if refresh
		{
			// special cases
			if(whats_This == whats_This_Sigma)
			{
				if(value_Type == VAL)	 parameter.value   = spin_Box->value()*coeff;
				if(value_Type == MIN)	{parameter.fit.min = spin_Box->value()*coeff; /*parameter.confidence.min = parameter.fit.min;*/}
				if(value_Type == MAX)	{parameter.fit.max = spin_Box->value()*coeff; /*parameter.confidence.max = parameter.fit.max;*/}

				for(Interlayer& interlayer : struct_Data.interlayer_Composition)
				{
					if(struct_Data.common_Sigma)
					{
						interlayer.my_Sigma.value = struct_Data.sigma.value;
					}
					interlayer.my_Sigma.fit.min = struct_Data.sigma.fit.min;
					interlayer.my_Sigma.fit.max = struct_Data.sigma.fit.max;
//					interlayer.my_Sigma.confidence.min = interlayer.my_Sigma.fit.min;
//					interlayer.my_Sigma.confidence.max = interlayer.my_Sigma.fit.max;
				}
			} else
			if(whats_This == whats_This_Thickness)
			{
				if(value_Type == VAL)
				{
					parameter.value = spin_Box->value()*coeff;
					QVariant var;
					var.setValue( struct_Data );
					structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

					// parents
					if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic) {
						Global_Variables::change_Parent_Period_Gamma_Thickness(structure_Item);
					}
				}
				if(value_Type == MIN)	{parameter.fit.min = spin_Box->value()*coeff; /*parameter.confidence.min = parameter.fit.min;*/}
				if(value_Type == MAX)	{parameter.fit.max = spin_Box->value()*coeff; /*parameter.confidence.max = parameter.fit.max;*/}
			} else
			if(whats_This == whats_This_Period)
			{
				if(value_Type == VAL)
				{
					// children
					double new_Period_Value = spin_Box->value()*coeff;
					if(	qApp->focusWidget() != spin_Box ||
						abs(new_Period_Value) > DBL_EPSILON )
					{
						double factor = 1;
						if( abs(struct_Data.period.value) > DBL_EPSILON )
						{
							factor = new_Period_Value / struct_Data.period.value;
						}
						change_Child_Layers_Thickness(structure_Item, factor);
					}

					// parents
					Global_Variables::change_Parent_Period_Gamma_Thickness(structure_Item);
				}
				if(value_Type == MIN)	{parameter.fit.min = spin_Box->value()*coeff; /*parameter.confidence.min = parameter.fit.min;*/}
				if(value_Type == MAX)	{parameter.fit.max = spin_Box->value()*coeff; /*parameter.confidence.max = parameter.fit.max;*/}
			} else
			if(whats_This == whats_This_Gamma)
			{
				double new_Value = spin_Box->value();

				if(value_Type == VAL)
				{
					if(new_Value>1)
					{
						spin_Box->setValue(struct_Data.gamma.value);
						resize_Line_Edit(table, spin_Box);
					} else
					{
						if( qApp->focusWidget() != spin_Box ||
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
						spin_Box->setValue(struct_Data.gamma.fit.min);
						resize_Line_Edit(table, spin_Box);
					} else
					{
						struct_Data.gamma.fit.min = spin_Box->value();
//						struct_Data.gamma.confidence.min = struct_Data.gamma.fit.min;
					}
				}
				if(value_Type == MAX)
				{
					if(new_Value>1)
					{
						spin_Box->setValue(struct_Data.gamma.fit.max);
						resize_Line_Edit(table, spin_Box);
					} else
					{
						struct_Data.gamma.fit.max = spin_Box->value();
//						struct_Data.gamma.confidence.max = struct_Data.gamma.fit.max;
					}
				}
			} else
			{
				if(value_Type == VAL)	 parameter.value   = spin_Box->value()*coeff;
				if(value_Type == MIN)	{parameter.fit.min = spin_Box->value()*coeff; /*parameter.confidence.min = parameter.fit.min;*/}
				if(value_Type == MAX)	{parameter.fit.max = spin_Box->value()*coeff; /*parameter.confidence.max = parameter.fit.max;*/}
			}
		}

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			for(int i=0; i<structure_Item->parent()->childCount(); i++)
			{
				if(structure_Item->parent()->child(i)==structure_Item)
				{
					Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
					for(int n=0; n<parent_Data.num_Repetition.value(); n++)
					{
						Parameter& regular_Parameter = get_Parameter(parent_Data.regular_Components[i].components[n], whats_This);
						if(value_Type == VAL)	regular_Parameter.value   = spin_Box->value()*coeff;
						if(value_Type == MIN)	regular_Parameter.fit.min = spin_Box->value()*coeff;
						if(value_Type == MAX)	regular_Parameter.fit.max = spin_Box->value()*coeff;

						if(whats_This == whats_This_Sigma)
						{
							for(Interlayer& interlayer : parent_Data.regular_Components[i].components[n].interlayer_Composition)
							{
								interlayer.my_Sigma.value   = parent_Data.regular_Components[i].components[n].sigma.value;
								interlayer.my_Sigma.fit.min = parent_Data.regular_Components[i].components[n].sigma.fit.min;
								interlayer.my_Sigma.fit.max = parent_Data.regular_Components[i].components[n].sigma.fit.max;
							}
						}
					}

					parent_Data.regular_Components[i].find_Min_Max_Values();

					QVariant var;
					var.setValue( parent_Data );
					structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

					break;
				}
			}
			Global_Variables::change_Parent_Period_Gamma_Thickness(structure_Item);
			emit regular_Layer_Edited(whats_This+value_Type);
		}

		// we should save before changing dependent
		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// change dependent if necessary
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		if( refill_Dependent_Table &&
			!parameter.coupled.master.exist && parameter.coupled.slaves.size()>0 )
		{

			// change dependent chain
			QVector<id_Type> ids;
			change_Slaves_in_Structure_Tree(parameter, parameter.coupled.slaves, ids);

			// refresh table and independent in all tabs
			refresh_Dependents(ids);
		}
	}
	{
		reload_Related_Widgets(QObject::sender());
		if(layer_Thickness_Transfer_Is_Created && !layer_Thickness_Transfer_Reload_Block)	{
			layer_Thickness_Transfer->reload();
		}
		emit_Data_Edited();

		// recalculation at change
		if(recalculate_Spinbox_Table && !reload && value_Type == VAL) {global_Multilayer_Approach->calc_Reflection(true);}
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

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			for(int i=0; i<structure_Item->parent()->childCount(); i++)
			{
				if(structure_Item->parent()->child(i)==structure_Item)
				{
					Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
					for(int n=0; n<parent_Data.num_Repetition.value(); n++)
					{
						Parameter& regular_Parameter = get_Parameter(parent_Data.regular_Components[i].components[n], whats_This);
						regular_Parameter.fit.is_Fitable = check_Box->isChecked();
					}

					QVariant var;
					var.setValue( parent_Data );
					structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

					break;
				}
			}

			if(whats_This == whats_This_Thickness)
				emit regular_Layer_Edited(fit_Thickness);
			else
				emit regular_Layer_Edited("especially_wrong"); // to reaload regular aperiodic item without reloading widgets
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

void Table_Of_Structures::reload_From_Regular_Aperiodic()
{	
	if(table_Is_Created)
	{
		// reloading for widgets on current tab
		int current_Tab_Index = main_Tabs->currentIndex();
		for(int i=0; i<regular_Aperiodic_Widgets_To_Reload[current_Tab_Index].size(); ++i)
		{
			QWidget* widget_To_Reload = regular_Aperiodic_Widgets_To_Reload[current_Tab_Index][i];

			// reload even disabled widgets!
			if(widget_To_Reload->property(enabled_Property).toBool())
			{
				widget_To_Reload->setProperty(reload_Property, true);

				QCheckBox*		check_Box = qobject_cast<QCheckBox*>	  (widget_To_Reload);
				MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(widget_To_Reload);

				if(check_Box)
				{
					check_Box->toggled(check_Box->isChecked());
				}
				if(spin_Box)
				{
					spin_Box->valueChanged(spin_Box->value());
					Global_Variables::resize_Line_Edit(spin_Box);
					reload_Related_Widgets(spin_Box);
				}
				widget_To_Reload->setProperty(reload_Property, false);
			} else {
				reload_Related_Widgets(widget_To_Reload);
			}
		}
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

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			for(int i=0; i<structure_Item->parent()->childCount(); i++)
			{
				if(structure_Item->parent()->child(i)==structure_Item)
				{
					Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
					for(int n=0; n<parent_Data.num_Repetition.value(); n++)
					{
						parent_Data.regular_Components[i].components[n].interlayer_Composition[interlayer_Index].enabled = check_Box->isChecked();
					}

					QVariant var;
					var.setValue( parent_Data );
					structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

					break;
				}
			}
			emit regular_Layer_Edited("especially_wrong"); // to reaload regular aperiodic item without reloading widgets
		}

		// here we save non-parapetric changes

		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		MyDoubleSpinBox* spin_Box = spin_Boxes_ID_Map.key(struct_Data.interlayer_Composition[interlayer_Index].my_Sigma.indicator.id);
		spin_Box->setProperty(forced_Reload_Property, struct_Data.interlayer_Composition[interlayer_Index].enabled);

		// refresh widget
		//	var.setValue(struct_Data.interlayer_Composition[interlayer_Index].interlayer);
		//	QWidget* back_Widget = coupled_Back_Widget_and_Id.key(struct_Data.interlayer_Composition[interlayer_Index].interlayer.indicator.id);
		//	back_Widget->setProperty(parameter_Property, var);
//	}
//	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());

		spin_Box->setProperty(forced_Reload_Property, false);

		// recalculation at change
		if(recalculate_Spinbox_Table && !reload) {global_Multilayer_Approach->calc_Reflection(true);}
	}
}

void Table_Of_Structures::refresh_Weigts_Interlayer()
{
	MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = spin_Boxes_Map.value(spin_Box);
	int interlayer_Index = spin_Box->property(interlayer_Index_Property).toInt();
	QString value_Type = spin_Box->property(value_Type_Property).toString();

	// for reloading
	bool reload = spin_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& interlayer = struct_Data.interlayer_Composition[interlayer_Index].interlayer;

	if(reload)
	{
		spin_Box->blockSignals(true);
		if(value_Type == VAL)	spin_Box->setValue(interlayer.value);
		if(value_Type == MIN)	spin_Box->setValue(interlayer.fit.min);
		if(value_Type == MAX)	spin_Box->setValue(interlayer.fit.max);
		spin_Box->blockSignals(false);
		return;
	}
	// if refresh
	{
		if(value_Type == VAL)
		{
			interlayer.value = spin_Box->value();
			if(!struct_Data.common_Sigma)
			{
				struct_Data.sigma.value = recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
			}
		}
		if(value_Type == MIN)	{interlayer.fit.min = spin_Box->value(); /*interlayer.confidence.min = interlayer.fit.min;*/}
		if(value_Type == MAX)	{interlayer.fit.max = spin_Box->value(); /*interlayer.confidence.max = interlayer.fit.max;*/}

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			for(int i=0; i<structure_Item->parent()->childCount(); i++)
			{
				if(structure_Item->parent()->child(i)==structure_Item)
				{
					Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
					for(int n=0; n<parent_Data.num_Repetition.value(); n++)
					{
						Parameter& regular_Interlayer = parent_Data.regular_Components[i].components[n].interlayer_Composition[interlayer_Index].interlayer;
						if(value_Type == VAL)	regular_Interlayer.value   = spin_Box->value();
						if(value_Type == MIN)	regular_Interlayer.fit.min = spin_Box->value();
						if(value_Type == MAX)	regular_Interlayer.fit.max = spin_Box->value();
					}

					QVariant var;
					var.setValue( parent_Data );
					structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

					break;
				}
			}
			emit regular_Layer_Edited("especially_wrong"); // to reaload regular aperiodic item without reloading widgets
		}

		// we should save before changing dependent
		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// change dependent if necessary
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		if( refill_Dependent_Table &&
			!interlayer.coupled.master.exist && interlayer.coupled.slaves.size()>0 )
		{
			// change dependent chain
			QVector<id_Type> ids;
			change_Slaves_in_Structure_Tree(interlayer, interlayer.coupled.slaves, ids);

			// refresh table and independent in all tabs
			refresh_Dependents(ids);
		}
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());

		// recalculation at change
		if(recalculate_Spinbox_Table && !reload && value_Type == VAL) {global_Multilayer_Approach->calc_Reflection(true);}
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

		// regular aperiodic
		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			for(int i=0; i<structure_Item->parent()->childCount(); i++)
			{
				if(structure_Item->parent()->child(i)==structure_Item)
				{
					Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
					for(int n=0; n<parent_Data.num_Repetition.value(); n++)
					{
						Parameter& regular_Parameter = parent_Data.regular_Components[i].components[n].interlayer_Composition[interlayer_Index].interlayer;
						regular_Parameter.fit.is_Fitable = check_Box->isChecked();
					}

					QVariant var;
					var.setValue( parent_Data );
					structure_Item->parent()->setData(DEFAULT_COLUMN, Qt::UserRole, var);

					break;
				}
			}
			emit regular_Layer_Edited("especially_wrong"); // to reaload regular aperiodic item without reloading widgets
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

void Table_Of_Structures::refresh_MySigma_Interlayer()
{
	double length_Coeff = length_Coefficients_Map.value(length_units);

	MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = spin_Boxes_Map.value(spin_Box);
	int interlayer_Index = spin_Box->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = spin_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Interlayer& interlayer = struct_Data.interlayer_Composition[interlayer_Index];

	if(reload)
	{
		spin_Box->blockSignals(true);
		spin_Box->setValue(interlayer.my_Sigma.value/length_Coeff);
		bool disable = struct_Data.common_Sigma || !interlayer.enabled;
		spin_Box->setDisabled(disable);
		spin_Box->blockSignals(false);
		return;
	}
	// if refresh
	{
		interlayer.my_Sigma.value = spin_Box->value()*length_Coeff;
		if(!struct_Data.common_Sigma)
		{
			struct_Data.sigma.value = recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
		}

		// we should save before changing dependent
		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// change dependent if necessary
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		if( refill_Dependent_Table &&
			!interlayer.my_Sigma.coupled.master.exist && interlayer.my_Sigma.coupled.slaves.size()>0 )
		{
			// change dependent chain
			QVector<id_Type> ids;
			change_Slaves_in_Structure_Tree(interlayer.my_Sigma, interlayer.my_Sigma.coupled.slaves, ids);

			// refresh table and independent in all tabs
			refresh_Dependents(ids);
		}
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());

		// recalculation at change
		if(recalculate_Spinbox_Table && !reload) {global_Multilayer_Approach->calc_Reflection(true);}
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

template<typename Type>
void Table_Of_Structures::resize_Line_Edit(My_Table_Widget* table, Type* line_Edit)
{
	if(!line_Edit) line_Edit = qobject_cast<Type*>(QObject::sender());
	Global_Variables::resize_Line_Edit(line_Edit);

	int current_Column = line_Edit->property(column_Property).toInt();
	fit_Column(table, 0, current_Column);
}
template void Table_Of_Structures::resize_Line_Edit<QLineEdit>		 (My_Table_Widget* , QLineEdit* );
template void Table_Of_Structures::resize_Line_Edit<MyDoubleSpinBox> (My_Table_Widget* , MyDoubleSpinBox* );

void Table_Of_Structures::reload_All_Widgets(QObject* sender)
{
	if(table_Is_Created)
	{
//		qInfo() << "reload_All_Widgets" << endl;
		// reloading for widgets on current tab
		int current_Tab_Index = main_Tabs->currentIndex();
//		qInfo() << "all_Widgets_To_Reload[current_Tab_Index].size() " << all_Widgets_To_Reload[current_Tab_Index].size() << endl;

		main_Tabs->tabBarClicked(current_Tab_Index);
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
//	qInfo() << "reload_One_Widget"<<++temp_Counter << endl;
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

	QLabel*				label = qobject_cast<QLabel*>		  (widget_To_Reload);
	QCheckBox*		check_Box = qobject_cast<QCheckBox*>	  (widget_To_Reload);
	QLineEdit*		line_Edit = qobject_cast<QLineEdit*>	  (widget_To_Reload);
	MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(widget_To_Reload);
	QComboBox*		combo_Box = qobject_cast<QComboBox*>	  (widget_To_Reload);

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
	if(spin_Box)
	{
		spin_Box->valueChanged(spin_Box->value());
		spin_Box->valueChanged(spin_Box->value());
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
		for(id_Type id : reload_Show_Dependence_Map.values(qobject_cast<QWidget*>(sender)))
		{
			for(QWidget* related: reload_Show_Dependence_Map.keys(id))
			{
				if(related != sender)
				{
					if(related->property(enabled_Property).toBool() || related->property(forced_Reload_Property).toBool())
					{
						related->setProperty(reload_Property, true);

						QLabel*				label = qobject_cast<QLabel*>		  (related);
						QCheckBox*		check_Box = qobject_cast<QCheckBox*>	  (related);
						QLineEdit*		line_Edit = qobject_cast<QLineEdit*>	  (related);
						MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(related);
						QComboBox*		combo_Box = qobject_cast<QComboBox*>	  (related);

						if(label)
						{
							label->windowTitleChanged("temp");
						}
						if(check_Box)
						{
							check_Box->toggled(check_Box->isChecked());
						}
						if(line_Edit)
						{
							line_Edit->textEdited("temp");
							line_Edit->textEdited("temp");
						}
						if(spin_Box)
						{
							spin_Box->valueChanged(spin_Box->value());
							spin_Box->valueChanged(spin_Box->value());
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
}

void Table_Of_Structures::reload_Min_Max_Line_Edits(QString whats_This)
{
	// reloading for widgets on current tab
	int current_Tab_Index = main_Tabs->currentIndex();

	QList<MyDoubleSpinBox*>* list_Pointer;

	if( whats_This == whats_This_Density )		list_Pointer = &min_Max_Density_Spin_Boxes_List[current_Tab_Index];
	if( whats_This == whats_This_Thickness )	list_Pointer = &min_Max_Thickness_Spin_Boxes_List[current_Tab_Index];
	if( whats_This == whats_This_Sigma )		list_Pointer = &min_Max_Sigma_Spin_Boxes_List[current_Tab_Index];

	for(MyDoubleSpinBox* spin_Box : *list_Pointer)
	{
		spin_Box->setProperty(reload_Property, true);
		spin_Box->valueChanged(spin_Box->value());
		spin_Box->setProperty(reload_Property, false);
	}
}
