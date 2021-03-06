#include "table_of_structures.h"
#include "algorithm"
#include "multilayer_approach/table_of_structures/table_roughness_model_editor.h"

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
		return !mouse_wheel_spinbox_structure_table;
	}
	return false;
}

void Table_Of_Structures::closeEvent(QCloseEvent* event)
{
	if(!temporary) write_Window_Geometry();
	runned_Tables_Of_Structures.remove(table_Of_Structures_Key);
//	bool state = recalculate_spinbox_structure_table;
//	recalculate_spinbox_structure_table = false;
	for(QLineEdit* material_Line_Edit : material_Line_Edits)
	{
		material_Line_Edit->blockSignals(true);
		check_Material(material_Line_Edit, true);
	}
//	recalculate_spinbox_structure_table = state;
	if(global_Multilayer_Approach->runned_Regular_Aperiodic_Tables.isEmpty()) global_Multilayer_Approach->unlock_Mainwindow_Interface();
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
	connect(main_Tabs,	&QTabWidget::currentChanged, this, [=]
	{
		if(tab_synchronization)
		{
																										  {global_Multilayer_Approach->						  multilayer_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))	  {global_Multilayer_Approach->optical_Graphs_2D		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Profile_Plots_Window.contains(profile_Plots_Key))		  {global_Multilayer_Approach->profile_Plots_Window       ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))	  {global_Multilayer_Approach->optical_Graphs_1D		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key)){global_Multilayer_Approach->calculation_Settings_Editor->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Roughness_Plots_Window.contains(roughness_Plots_Key))	  {global_Multilayer_Approach->roughness_Plots_Window     ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Particles_Plots_Window.contains(particles_Plots_Key))	  {global_Multilayer_Approach->particles_Plots_Window	  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
		}
	});
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
	connect(menu, &Menu::refresh, this, [=]
	{
		reload_All_Widgets(true);

		// in other tabs
		for(int tab_Index = 0; tab_Index<main_Tabs->count(); tab_Index++)
		{
			if(tab_Index != main_Tabs->currentIndex())
			{
				for(int i=0; i<all_Widgets_To_Reload[tab_Index].size(); ++i)
				{
					QWidget* widget_To_Reload = all_Widgets_To_Reload[tab_Index][i];
					if(widget_To_Reload != nullptr)
					{
						reload_One_Widget(widget_To_Reload, true);
					}
				}
			}
		}
		Global_Variables::plot_All_Data_in_Roughness();
		Global_Variables::plot_All_Data_in_Particles();
	});
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
	});
}

void Table_Of_Structures::add_Tabs()
{
	read_Trees();
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		My_Table_Widget* new_Table = new My_Table_Widget(basic_Row_Number, basic_Column_Number, this, this);
		all_Widgets_To_Reload.append(QList<QWidget*>());
		check_Boxes_Particles_Usage_List.append(QList<QCheckBox*>());
		regular_Aperiodic_Widgets_To_Reload.append(QList<QWidget*>());

		min_Max_Density_Spin_Boxes_List.append(QList<MyDoubleSpinBox*>());
		min_Max_Thickness_Spin_Boxes_List.append(QList<MyDoubleSpinBox*>());
		min_Max_Sigma_Diffuse_Spin_Boxes_List.append(QList<MyDoubleSpinBox*>());

		create_Table(new_Table, tab_Index);
		main_Tabs->addTab(new_Table, multilayer_Tabs->tabText(tab_Index));
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
	table_Is_Fully_Created = true;
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		for(QCheckBox* check_Box_Usage : check_Boxes_Particles_Usage_List[tab_Index])
		{
			check_Box_Usage->toggled(check_Box_Usage->isChecked());
		}
		My_Table_Widget* table = qobject_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));
		disable_enable_Structure_Items(table);
	}
	main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());
}

void Table_Of_Structures::create_Table(My_Table_Widget* new_Table, int tab_Index)
{
	// PARAMETER

	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
	rows_List_To_Span.clear();
	{
		// header for each structure
		QFont font_Header;
		font_Header.setBold(true);

		new_Table->insertRow(new_Table->rowCount());
		int current_Row = new_Table->rowCount()-1;
		int steps_Row = 0;

		// structure display
		int max_Depth = Global_Variables::get_Tree_Depth(list_Of_Trees[tab_Index]->tree->invisibleRootItem());
		int depth, current_Column;

		int interlayer_Types_To_Show = 0;
		for(int i=0; i<multilayer->imperfections_Model.use_Func.size(); i++)
		{
			if(multilayer->imperfections_Model.use_Func[i]) interlayer_Types_To_Show++;
		}

		bool has_Layers = false;
		bool has_Boundaries = false;
		bool has_Interlayer_Functions = false;
		bool has_Interlayer_Show = false;
		bool has_Drift = false;
		bool has_Multilayers = false;
		bool has_Gamma = false;
		bool has_Regular_Aperiodic = false;
		bool has_Particle_Height = false;
		bool has_Particle_Distance_Deviation = false;

		// calculate max_Number_Of_Elements for tabulation
		int max_Number_Of_Elements=1;
		{
			QTreeWidgetItem* structure_Item;
			QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);
			while (*it)
			{
				structure_Item=*it;
				Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

				if(struct_Data.item_Type == item_Type_Layer)
				{
					has_Layers = true;

					if( struct_Data.parent_Item_Type == item_Type_Multilayer &&
						multilayer->imperfections_Model.show_Drift &&
					   (multilayer->imperfections_Model.show_Thickness_Drift_Line ||
						multilayer->imperfections_Model.show_Thickness_Drift_Rand ||
						multilayer->imperfections_Model.show_Thickness_Drift_Sine ||
						multilayer->imperfections_Model.show_Sigma_Drift_Line ||
						multilayer->imperfections_Model.show_Sigma_Drift_Rand ||
						multilayer->imperfections_Model.show_Sigma_Drift_Sine ))
					{
						has_Drift = true;
					}

					if( struct_Data.particles_Model.particle_Shape == full_Spheroid ||
						struct_Data.particles_Model.particle_Shape == cylinder )
					{
						has_Particle_Height = true;
					}
					if( struct_Data.particles_Model.particle_Interference_Function == radial_Paracrystal )
					{
						has_Particle_Distance_Deviation = true;

						bool is_Last_Layer = Global_Variables::if_Last_Layer(list_Of_Trees[tab_Index]->tree, structure_Item);
						if( !is_Last_Layer &&
							(multilayer->imperfections_Model.particle_Vertical_Correlation == zero_Correlation &&
						     multilayer->imperfections_Model.use_Common_Particle_Function ||
						     multilayer->imperfections_Model.particle_Vertical_Correlation != zero_Correlation ) )
						{
							has_Particle_Distance_Deviation = false;
						}
					}
				}
				if(struct_Data.item_Type == item_Type_Layer    ||
				   struct_Data.item_Type == item_Type_Substrate )
				{
					has_Boundaries = true;

					if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic ||
					  (interlayer_Types_To_Show>0 && multilayer->imperfections_Model.use_Interlayer))
					{has_Interlayer_Functions = true;}

					if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic ||
					  multilayer->imperfections_Model.use_Interlayer)
					{has_Interlayer_Show = true;}
				}

				if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
				{
					has_Regular_Aperiodic = true;
				}

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

		bool show_Top_Row = false;
		bool show_Bottom_Row = false;
		bool show_Right_Column = false;
		// place for min_max buttons
		{
			if(show_Top_Row)
			{
				rows_List_To_Span.append(current_Row);
				new_Table->setItem(current_Row,0, new QTableWidgetItem("Set min/max"));
				new_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);
				new_Table->item   (current_Row,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				new_Table->item   (current_Row,0)->setFont(font_Header);
				new_Table->item   (current_Row,0)->setSizeHint(QSize(0,0));
				new_Table->insertRow(new_Table->rowCount());
			}

			new_Table->insertRow(new_Table->rowCount());
			current_Row = new_Table->rowCount()-2;
			new_Table->insertRow(new_Table->rowCount());
			new_Table->insertRow(new_Table->rowCount());
//			new_Table->insertRow(new_Table->rowCount());

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
					master_Label->setMinimumWidth(COLOR_LEGEND_LABEL_WIDTH);
				master_Label->setStyleSheet(master_Parameter_Color);
				new_Table->setCellWidget(current_Row+2,0, master_Label);

				// free parameter
				QLabel* free_Label = new QLabel("free parameter");
					free_Label->setAlignment(Qt::AlignCenter);
					free_Label->setMinimumWidth(COLOR_LEGEND_LABEL_WIDTH);
				free_Label->setStyleSheet(free_Parameter_Color);
//				new_Table->setCellWidget(current_Row+3,0, free_Label);
				new_Table->setCellWidget(current_Row+2,1, free_Label);

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

				// z restriction (UNUSED)
//				QLabel* restrict_Z_Label = new QLabel("restrict z");
//					restrict_Z_Label->setAlignment(Qt::AlignCenter);
//					restrict_Z_Label->setMinimumWidth(COLOR_LEGEND_LABEL_WIDTH);
//				restrict_Z_Label->setStyleSheet(soft_Restriction_Color);
//				new_Table->setCellWidget(current_Row+2,1, restrict_Z_Label);

				// active fit
				QLabel* fit_Label = new QLabel("active fit");
					fit_Label->setAlignment(Qt::AlignCenter);
					fit_Label->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
				fit_Label->setStyleSheet(fit_Color);
				new_Table->setCellWidget(current_Row,2, fit_Label);
				new_Table->setSpan(current_Row,2,1,2);

				// common z
				QLabel* common_Z_Label = new QLabel("common z");
					common_Z_Label->setAlignment(Qt::AlignCenter);
					common_Z_Label->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
				common_Z_Label->setStyleSheet(common_Thickness_Color);
				new_Table->setCellWidget(current_Row+1,2, common_Z_Label);
				new_Table->setSpan(current_Row+1,2,1,2);

				// common sigma
				QLabel* common_Sigma_Label = new QLabel("common s");
					common_Sigma_Label->setAlignment(Qt::AlignCenter);
					common_Sigma_Label->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);
				common_Sigma_Label->setStyleSheet(common_Sigma_Color);
				new_Table->setCellWidget(current_Row+2,2, common_Sigma_Label);
				new_Table->setSpan(current_Row+2,2,1,2);
			}

			// density min/max
			create_Simple_Label		(new_Table,	tab_Index, current_Row,   current_Column, "nothing", Rho_Sym+", "+/*Plus_Minus_Sym*/Minus_Sym+"%");
			create_Min_Max_Button	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Density);
			create_Min_Max_Spin_Box	(new_Table, tab_Index, current_Row+2, current_Column, whats_This_Density);
			current_Column += 2;

			// thickness min/max
			if(has_Layers)
			{
				create_Simple_Label		(new_Table,	tab_Index, current_Row,   current_Column, "nothing", "z/d, "+Plus_Minus_Sym+"%");
				create_Min_Max_Button	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Thickness);
				create_Min_Max_Spin_Box	(new_Table, tab_Index, current_Row+2, current_Column, whats_This_Thickness);
			}
			current_Column += 2;

			// sigma min/max
			if(has_Boundaries && has_Interlayer_Show)
			{
				create_Simple_Label		(new_Table,	tab_Index, current_Row,   current_Column, "nothing", "s, "+Plus_Minus_Sym+"%");
				create_Min_Max_Button	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Sigma_Diffuse);
				create_Min_Max_Spin_Box	(new_Table, tab_Index, current_Row+2, current_Column, whats_This_Sigma_Diffuse);
			}
		}

		// place for step buttons
		{
			new_Table->insertRow(new_Table->rowCount());
			current_Row = new_Table->rowCount()-2;

			rows_List_To_Span.append(current_Row);
			new_Table->setItem(current_Row,0, new QTableWidgetItem("Set increase/decrease steps"));
			new_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);
			new_Table->item   (current_Row,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			new_Table->item   (current_Row,0)->setFont(font_Header);
			new_Table->item   (current_Row,0)->setSizeHint(QSize(0,0));

			new_Table->insertRow(new_Table->rowCount());
			current_Row = new_Table->rowCount()-2;
			new_Table->insertRow(new_Table->rowCount());

			// modifications
			{
				current_Column = 0;
				spin_Box_Mouse_Wheel	 (new_Table,			current_Row  , current_Column);
				spin_Box_Recalculate	 (new_Table,			current_Row+1, current_Column);
				spin_Box_Change_Dependent(new_Table,			current_Row  , current_Column+1);
				change_Model			 (new_Table, tab_Index, current_Row+1, current_Column+1);
			}

			add_Columns(new_Table, max_Depth + max_Number_Of_Elements+2);

			// composition step
			if(max_Number_Of_Elements>=2)
			{
				current_Column = max_Depth+1;
				create_Simple_Label		(new_Table,	tab_Index, current_Row,   current_Column, whats_This_Composition, Zeta_Sym);
				create_Step_Spin_Box	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Composition);
			}
			current_Column = max_Depth + max_Number_Of_Elements+2;

			// density step
			create_Simple_Label		(new_Table,	tab_Index, current_Row,   current_Column, whats_This_Density, Rho_Sym);
			create_Step_Spin_Box	(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Density);
			current_Column += 2;

			// thickness step
			if(has_Layers)
			{
				create_Simple_Label	(new_Table,	tab_Index, current_Row,   current_Column, whats_This_Thickness, "z/d ["+length_units+"]");
				create_Step_Spin_Box(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Thickness);
			}
			current_Column += 2;

			// sigma diffuse step
			if(has_Boundaries && has_Interlayer_Show)
			{
				create_Simple_Label	(new_Table,	tab_Index, current_Row,   current_Column, whats_This_Sigma_Diffuse, "s ["+length_units+"]");
				create_Step_Spin_Box(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Sigma_Diffuse);
				current_Column += 1;
			}

			// gamma step
			if(has_Gamma)
			{
				create_Simple_Label	(new_Table,	tab_Index, current_Row,   current_Column, whats_This_Gamma, Gamma_Sym);
				create_Step_Spin_Box(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Gamma);
			}
			current_Column += 1;

			// interlayer step
			if(has_Boundaries && has_Interlayer_Functions)
			{
				create_Simple_Label	(new_Table,	tab_Index, current_Row,   current_Column, whats_This_Interlayer_Composition, "interl");
				create_Step_Spin_Box(new_Table, tab_Index, current_Row+1, current_Column, whats_This_Interlayer_Composition);

				if(has_Regular_Aperiodic && !multilayer->imperfections_Model.use_Interlayer)
				{
					current_Column += 1;
				} else
				{
					current_Column += 1+interlayer_Types_To_Show;
				}
			}

			// roughness steps are done at the end
			steps_Row = current_Row;
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

		int first_Roughness_Column = 0;
		int last_Roughness_Column = 0;

		int first_Particles_Column = 0;
		int last_Particles_Column = 0;

		bool steps_Are_Done_Line_Drift = false;
		bool steps_Are_Done_Rand_Drift = false;
		bool steps_Are_Done_Sine_Drift_Amplitude = false;
		bool steps_Are_Done_Sine_Drift_Frequency = false;
		bool steps_Are_Done_Sine_Drift_Phase = false;

		bool steps_Are_Done_Sigma = false;
		bool steps_Are_Done_Xi = false;		Q_UNUSED(steps_Are_Done_Xi)	  // adaptive step
		bool steps_Are_Done_Alpha = false;
		bool steps_Are_Done_Mu = false;		Q_UNUSED(steps_Are_Done_Mu)	  // adaptive step
		bool steps_Are_Done_Omega = false;  Q_UNUSED(steps_Are_Done_Omega)// adaptive step
		bool steps_Are_Done_Peak_Sigma = false;
		bool steps_Are_Done_Peak_Frequency = false;			Q_UNUSED(steps_Are_Done_Peak_Frequency)// adaptive step
		bool steps_Are_Done_Peak_Frequency_Width = false;	Q_UNUSED(steps_Are_Done_Peak_Frequency_Width)// adaptive step
		bool steps_Are_Done_Sigma_Factor_PSD = false;		
		bool nu0_Is_Done = false;


		bool steps_Are_Done_Particle_Density = false;
		bool steps_Are_Done_Particle_Radius = false;
		bool steps_Are_Done_Particle_Height = false;
		bool steps_Are_Done_Particle_Distance = false;
		bool steps_Are_Done_Particle_Distance_Deviation = false;
		bool steps_Are_Done_Particle_Z_Position = false;
		bool steps_Are_Done_Particle_Z_Position_Deviation = false;

		Data last_Layer_Data;
		while (*it)
		{
			structure_Item = *it;

			int column_Difference = 0;
			depth = Global_Variables::get_Item_Depth(structure_Item);
			current_Column = depth-1;
			new_Table->insertRow(new_Table->rowCount());
			new_Table->insertRow(new_Table->rowCount());
			new_Table->insertRow(new_Table->rowCount());

			current_Row = new_Table->rowCount()-3;

			Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

			bool is_Last_Layer        = Global_Variables::if_Last_Layer       (list_Of_Trees[tab_Index]->tree, structure_Item);
			bool is_Second_Last_Layer = Global_Variables::if_Second_Last_Layer(list_Of_Trees[tab_Index]->tree, structure_Item);
			if(is_Last_Layer) last_Layer_Data = struct_Data;

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
				table_Item->setWhatsThis(struct_Data.item_Type);
				table_Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			new_Table->setItem(current_Row,current_Column, table_Item);
			struct_Table_Map.insert(structure_Item, table_Item);

			// make it checkable
			if(struct_Data.item_Type == item_Type_Substrate)
			{
				table_Item->setText(Global_Variables::structure_Item_Name(struct_Data));
			} else
			{
				QCheckBox* item_CheckBox = new QCheckBox(Global_Variables::structure_Item_Name(struct_Data));
					item_CheckBox->setProperty(item_Table_CheckBox_Property,item_Table_CheckBox_Property);
					item_CheckBox->setProperty(column_Property,current_Column);
				if(struct_Data.item_Type == item_Type_Multilayer)
				{
					item_CheckBox->setProperty(multilayer_Item_Table_CheckBox_Property,multilayer_Item_Table_CheckBox_Property);
								   setProperty(multilayer_Item_Table_CheckBox_Property,multilayer_Item_Table_CheckBox_Property);
					coupled_Back_Widget_and_Struct_Item.insert(item_CheckBox, structure_Item);
				}
				if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
				{
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

					// refresh view in main window
					emit_Data_Edited();
					global_Multilayer_Approach->recalculate_From_Table(true);
				});
			}

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
					create_Combo_Elements	    (new_Table, tab_Index, current_Row,   current_Column, structure_Item, false);
					create_Stoich_Line_Edit	    (new_Table, tab_Index, current_Row+1, current_Column, structure_Item, false, VAL);
					create_Stoich_Line_Edit	    (new_Table, tab_Index, current_Row+3, current_Column, structure_Item, false, MIN);
					create_Stoich_Line_Edit	    (new_Table, tab_Index, current_Row+4, current_Column, structure_Item, false, MAX);

					// it should be created last
					create_Stoich_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, false, 1, 2, 0, 0);
				}
				else
				{
					create_Material_Line_Edit	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, false);
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
				add_Columns			(new_Table, current_Column+1);
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
					add_Columns			(new_Table, current_Column+1);
					create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "d ["+length_units+"]");
					create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
					create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
					create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
					// lastcolumn
					create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

					if(has_Interlayer_Show) current_Column += 3;
					else					current_Column += 2;

					// gamma
					if(structure_Item->childCount()==2)
					{
						whats_This = whats_This_Gamma;
						add_Columns			(new_Table, current_Column+1);
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
			if(multilayer->imperfections_Model.use_Interlayer || struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
			{	current_Column += 2;}
			else
			{	if(has_Gamma) current_Column += 3;}
			///--------------------------------------------------------------------------------------------

			// common sigma
			if(multilayer->imperfections_Model.use_Interlayer || struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
			{
				if( struct_Data.item_Type == item_Type_Layer    ||
					struct_Data.item_Type == item_Type_Substrate )
				{
					QString whats_This = whats_This_Sigma_Diffuse;
					add_Columns(new_Table, current_Column+1);
					create_Line_Edit	  (new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
					// second
					create_Check_Box_Label(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "s ["+length_units+"]", 1, 0, 0, 0);
					create_Line_Edit	  (new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
					create_Line_Edit	  (new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
					// last
					create_Check_Box_Fit  (new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);
				}
				current_Column += 2;
			} else
			{
				if(has_Regular_Aperiodic) current_Column += 2;
				column_Difference--;
			}

			///--------------------------------------------------------------------------------------------

			// interlayers: weights and sigmas
			if(multilayer->imperfections_Model.use_Interlayer || struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
			{
				if( struct_Data.item_Type == item_Type_Layer    ||
					struct_Data.item_Type == item_Type_Substrate )
				{
					int additional_Column = show_Right_Column ? 0 : -1;
					if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic) add_Columns(new_Table, current_Column+transition_Layer_Functions_Size+additional_Column);
					else															add_Columns(new_Table, current_Column+interlayer_Types_To_Show+additional_Column);

					// weights
					create_Weigts_Interlayer		        (new_Table, tab_Index, current_Row+1, current_Column, structure_Item, VAL);
					create_Check_Box_Label_Interlayer	    (new_Table, tab_Index, current_Row,   current_Column, structure_Item);
					create_Weights_Check_Box_Fit_Interlayer (new_Table, tab_Index, current_Row+2, current_Column, structure_Item);

					// sigmas
					if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
					{
						create_Check_Box_Label      (new_Table, tab_Index, current_Row+3, current_Column,  structure_Item, whats_This_Common_Thickness, "common z", -2, -2, -4, -4);
						create_Check_Box_Label      (new_Table, tab_Index, current_Row+4, current_Column,  structure_Item, whats_This_Common_Sigma_Diffuse, "common s", -3, -3, -2, -2);
						create_Check_Box_Label      (new_Table, tab_Index, current_Row+3, current_Column+2,structure_Item, whats_This_Restrict_Thickness, "restrict z: {"+Plus_Minus_Sym+Delta_Big_Sym+", p, Q}", 1, 1, 0, 2);
						create_Thickness_Restriction(new_Table,            current_Row+4, current_Column+2,structure_Item);
					} else
					{
						create_MySigma_Labels_Interlayer	    (new_Table, tab_Index, current_Row+3, current_Column, structure_Item);
						create_MySigma_Line_Edits_Interlayer	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item);
					}
				}

				if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic) current_Column += (1+transition_Layer_Functions_Size);
				else
				{
					if(has_Regular_Aperiodic || interlayer_Types_To_Show>0) current_Column += (1+interlayer_Types_To_Show);
					else													current_Column += (  interlayer_Types_To_Show);
				}
			} else
			{
				if(has_Regular_Aperiodic && !has_Gamma) current_Column += 3;
				if(!has_Regular_Aperiodic)
				{
					if(has_Gamma) current_Column += 0;
					else		  current_Column += 3;
				}
			}
			///--------------------------------------------------------------------------------------------
			/// DRIFTS
			///--------------------------------------------------------------------------------------------

			int additional_Column_Drift = show_Right_Column ? 1 : 0;

			// thickness linear drift
			if(multilayer->imperfections_Model.show_Thickness_Drift_Line && multilayer->imperfections_Model.show_Drift)
			{
				if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
				{
					QString whats_This = whats_This_Thickness_Drift_Line_Value;
					add_Columns(new_Table, current_Column+additional_Column_Drift);
					create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
					// second
					create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This,"dz lin", 1, 1, 0, 0);
					create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
					create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
					// last
					create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

					// drift line step
					if(!steps_Are_Done_Line_Drift)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, "dz lin");
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
						steps_Are_Done_Line_Drift = true;
					}

					current_Column += 1;
				} else
				if(has_Multilayers)
				{
					if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
					{
						if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic) current_Column += 1;
					}
					column_Difference++;
				}
			}
			///--------------------------------------------------------------------------------------------

			// thickness random drift
			if(multilayer->imperfections_Model.show_Thickness_Drift_Rand && multilayer->imperfections_Model.show_Drift)
			{
				if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
				{
					QString whats_This = whats_This_Thickness_Drift_Rand_Rms;
					add_Columns(new_Table, current_Column+additional_Column_Drift);
					create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
					// second
					create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This,"dz ran", 1, 1, 0, 0);
					create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
					create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
					// last
					create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

					// drift rand step
					if(!steps_Are_Done_Rand_Drift)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, "dz ran");
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
						steps_Are_Done_Rand_Drift = true;
					}

					current_Column += 1;
				} else
				if(has_Multilayers)
				{
					if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
					{
						if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic) current_Column += 1;
					}
					column_Difference++;
				}
			}
			///--------------------------------------------------------------------------------------------

			// thickness sine drift
			if(multilayer->imperfections_Model.show_Thickness_Drift_Sine && multilayer->imperfections_Model.show_Drift)
			{
				if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
				{
					add_Columns(new_Table, current_Column+2+additional_Column_Drift);

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

					// sine amplitude step
					if(!steps_Are_Done_Sine_Drift_Amplitude)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This_Thickness_Drift_Sine_Amplitude, "dz:A");
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This_Thickness_Drift_Sine_Amplitude);
						steps_Are_Done_Sine_Drift_Amplitude = true;
					}
					// sine frequency step
					if(!steps_Are_Done_Sine_Drift_Frequency)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column+1, whats_This_Thickness_Drift_Sine_Frequency, "dz:v");
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column+1, whats_This_Thickness_Drift_Sine_Frequency);
						steps_Are_Done_Sine_Drift_Frequency = true;
					}
					// sine phase step
					if(!steps_Are_Done_Sine_Drift_Phase)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column+2, whats_This_Thickness_Drift_Sine_Phase, "dz:"+Phi_Sym);
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column+2, whats_This_Thickness_Drift_Sine_Phase);
						steps_Are_Done_Sine_Drift_Phase = true;
					}

					new_Table->setSpan(current_Row,current_Column,1,3);
					current_Column += 3;
				} else
				if(has_Multilayers)
				{
					if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
					{
						if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic) current_Column += 3;
					}
					column_Difference+=3;
				}
			}
			if(has_Multilayers)
			if(	(multilayer->imperfections_Model.show_Thickness_Drift_Line && multilayer->imperfections_Model.show_Drift) ||
				(multilayer->imperfections_Model.show_Thickness_Drift_Rand && multilayer->imperfections_Model.show_Drift) ||
				(multilayer->imperfections_Model.show_Thickness_Drift_Sine && multilayer->imperfections_Model.show_Drift) )
			{
				column_Difference++;
				if(	struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic ) current_Column += 1;
			}
			///--------------------------------------------------------------------------------------------

			// sigma linear drift
			if(multilayer->imperfections_Model.show_Sigma_Drift_Line  && multilayer->imperfections_Model.show_Drift)
			{
				if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
				{
					QString whats_This = whats_This_Sigma_Drift_Line_Value;
					add_Columns(new_Table, current_Column+additional_Column_Drift);
					create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
					// second
					create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "ds lin", 1, 1, 0, 0);
					create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
					create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
					// last
					create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

					// drift line step
					if(!steps_Are_Done_Line_Drift)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, "ds lin");
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
						steps_Are_Done_Line_Drift = true;
					}

					current_Column += 1;
				} else
				if(has_Multilayers)
				{
					if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
					{
						if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic) current_Column += 1;
					}
					column_Difference++;
				}
			}
			///--------------------------------------------------------------------------------------------

			// sigma random drift
			if(multilayer->imperfections_Model.show_Sigma_Drift_Rand && multilayer->imperfections_Model.show_Drift)
			{
				if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
				{
					QString whats_This = whats_This_Sigma_Drift_Rand_Rms;
					add_Columns(new_Table, current_Column+additional_Column_Drift);
					create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
					// second
					create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "ds ran", 1, 1, 0, 0);
					create_Line_Edit		(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
					create_Line_Edit		(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
					// last
					create_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

					// drift rand step
					if(!steps_Are_Done_Rand_Drift)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, "ds ran");
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
						steps_Are_Done_Rand_Drift = true;
					}

					current_Column += 1;
				} else
				if(has_Multilayers)
				{
					if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
					{
						if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic) current_Column += 1;
					}
					column_Difference++;
				}
			}
			///--------------------------------------------------------------------------------------------

			// sigma sine drift
			if(multilayer->imperfections_Model.show_Sigma_Drift_Sine && multilayer->imperfections_Model.show_Drift)
			{
				if(struct_Data.item_Type == item_Type_Layer && struct_Data.parent_Item_Type == item_Type_Multilayer)
				{
					add_Columns(new_Table, current_Column+2+additional_Column_Drift);

					create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column,   structure_Item, whats_This_Sigma_Drift_Sine_Amplitude, VAL);
					create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column+1, structure_Item, whats_This_Sigma_Drift_Sine_Frequency, VAL);
					create_Line_Edit		(new_Table, tab_Index, current_Row+1, current_Column+2, structure_Item, whats_This_Sigma_Drift_Sine_Phase,	   VAL);

					// second
					create_Check_Box_Label	(new_Table, tab_Index, current_Row,   current_Column,   structure_Item, whats_This_Sigma_Drift_Sine,"ds sin : A*sin(2"+Pi_Sym+"n"+Nu_Sym+"+"+Phi_Sym+")", 1, 1, 0, 2);

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

					// sine amplitude step
					if(!steps_Are_Done_Sine_Drift_Amplitude)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This_Sigma_Drift_Sine_Amplitude, "ds:A");
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This_Sigma_Drift_Sine_Amplitude);
						steps_Are_Done_Sine_Drift_Amplitude = true;
					}
					// sine frequency step
					if(!steps_Are_Done_Sine_Drift_Frequency)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column+1, whats_This_Sigma_Drift_Sine_Frequency, "ds:v");
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column+1, whats_This_Sigma_Drift_Sine_Frequency);
						steps_Are_Done_Sine_Drift_Frequency = true;
					}
					// sine phase step
					if(!steps_Are_Done_Sine_Drift_Phase)
					{
						create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column+2, whats_This_Sigma_Drift_Sine_Phase, "ds:"+Phi_Sym);
						create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column+2, whats_This_Sigma_Drift_Sine_Phase);
						steps_Are_Done_Sine_Drift_Phase = true;
					}
					new_Table->setSpan(current_Row,current_Column,1,3);
					current_Column += 3;
				} else
				if(has_Multilayers)
				{
					if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
					{
						if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic) current_Column += 3;
					}
					column_Difference+=3;
				}
			}
			if(has_Multilayers)
			if(	(multilayer->imperfections_Model.show_Sigma_Drift_Line && multilayer->imperfections_Model.show_Drift) ||
				(multilayer->imperfections_Model.show_Sigma_Drift_Rand && multilayer->imperfections_Model.show_Drift) ||
				(multilayer->imperfections_Model.show_Sigma_Drift_Sine && multilayer->imperfections_Model.show_Drift) )
			{
				column_Difference++;
				if(	struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic ) current_Column += 1;
			}

			///--------------------------------------------------------------------------------------------
			/// ROUGHNESS
			///--------------------------------------------------------------------------------------------

			// align columns
			column_Difference-=(transition_Layer_Functions_Size-interlayer_Types_To_Show);

			if( multilayer->imperfections_Model.show_Drift &&
				struct_Data.item_Type == item_Type_Layer &&
				struct_Data.parent_Item_Type == item_Type_Multilayer)
			{
				int drift_Counter = 0;
				bool thickness_Drift = false;
				if(multilayer->imperfections_Model.show_Thickness_Drift_Line)	{drift_Counter++;  thickness_Drift = true;}
				if(multilayer->imperfections_Model.show_Thickness_Drift_Rand)	{drift_Counter++;  thickness_Drift = true;}
				if(multilayer->imperfections_Model.show_Thickness_Drift_Sine)	{drift_Counter+=3; thickness_Drift = true;}

				bool sigma_Drift = false;
				if(multilayer->imperfections_Model.show_Sigma_Drift_Line)	{drift_Counter++;  sigma_Drift = true;}
				if(multilayer->imperfections_Model.show_Sigma_Drift_Rand)	{drift_Counter++;  sigma_Drift = true;}
				if(multilayer->imperfections_Model.show_Sigma_Drift_Sine)	{drift_Counter+=3; sigma_Drift = true;}

				if(thickness_Drift && sigma_Drift) drift_Counter++;

				column_Difference += drift_Counter;
			}
			if(has_Regular_Aperiodic)
			{
				if(column_Difference<0)
				{
					if(	struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic ) current_Column -= column_Difference;
				} else
				{
					if(	struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic ) current_Column += column_Difference;
				}
			}
			first_Roughness_Column = current_Column;
			int additional_Column = show_Right_Column || (multilayer->imperfections_Model.use_Particles && has_Layers) ? 1 : 0;

			// sigma roughness
			bool show_Sigma = false;
			if(	struct_Data.roughness_Model.is_Enabled )
			{
				if(struct_Data.item_Type == item_Type_Substrate)
				{
					if( multilayer->imperfections_Model.PSD_Model == ABC_Model ||
						multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model )
					{
						show_Sigma = true;
					}
				}
				if(struct_Data.item_Type == item_Type_Layer)
				{
					if((multilayer->imperfections_Model.PSD_Model == ABC_Model ||
						multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model) &&
					   (multilayer->imperfections_Model.vertical_Correlation == zero_Correlation && multilayer->imperfections_Model.use_Common_Roughness_Function == false))
					{
						show_Sigma = true;
					}
				}
			}
			if(show_Sigma)
			{
				QString whats_This = whats_This_Sigma_Roughness;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Sigma_Sym+" ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// sigma roughness step
				if(!steps_Are_Done_Sigma)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Sigma_Sym+" ["+length_units+"]");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Sigma = true;
				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// correlation radius
			bool show_Cor_Radius = false;
			if(	struct_Data.roughness_Model.is_Enabled )
			{
				if(struct_Data.item_Type == item_Type_Substrate)
				{
					if( multilayer->imperfections_Model.PSD_Model == ABC_Model ||
						multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model )
					{
						show_Cor_Radius = true;
					}
				}
				if(struct_Data.item_Type == item_Type_Layer)
				{
					if((multilayer->imperfections_Model.PSD_Model == ABC_Model ||
						multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model) &&
						(multilayer->imperfections_Model.vertical_Correlation == zero_Correlation && multilayer->imperfections_Model.use_Common_Roughness_Function == false))
					{
						show_Cor_Radius = true;
					}
				}
			}
			if(show_Cor_Radius)
			{
				QString whats_This = whats_This_Correlation_Radius;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Xi_Sym+" ["+correlation_length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				/// (adaptive step instead of fixed)
//				// correlation radius step
//				if(!steps_Are_Done_Xi)
//				{
//					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Xi_Sym+" ["+correlation_length_units+"]");
//					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
//					steps_Are_Done_Xi = true;
//				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// omega
			bool show_Omega = false;
			if(	struct_Data.roughness_Model.is_Enabled )
			{
				if( struct_Data.item_Type == item_Type_Layer)
				{
					if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation &&
					   (multilayer->imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
						multilayer->imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model))
					{
						show_Omega = true;
					}
				}
			}
			if(show_Omega)
			{
				QString whats_This = whats_This_Linear_PSD_Omega;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Omega_Big_Sym+" ["+omega_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				/// (adaptive step instead of fixed)
//				// omega step
//				if(!steps_Are_Done_Omega)
//				{
//					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Omega_Big_Sym+" ["+omega_units+"]");
//					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
//					steps_Are_Done_Omega = true;
//				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				if(multilayer->imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model) {
					current_Column+=1;
				} else	{
					current_Column+=2;
				}
			}
			///--------------------------------------------------------------------------------------------

			// mu
			bool show_Mu = false;
			if(	struct_Data.roughness_Model.is_Enabled )
			{
				if( struct_Data.item_Type == item_Type_Layer)
				{
					if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation &&
						(multilayer->imperfections_Model.inheritance_Model == replication_Factor_Inheritance_Model ||
						 multilayer->imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model))
					{
						show_Mu = true;
					}
				}
			}
			if(show_Mu)
			{
				if(multilayer->imperfections_Model.inheritance_Model == replication_Factor_Inheritance_Model) current_Column+=2;
				QString whats_This = whats_This_PSD_Exponenta_Mu;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "L"+Subscript_v_Sym+" ["+mu_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				/// here we put nu0 for correlation depth
				// nu0 value
				if(!nu0_Is_Done)
				{
					create_Nu0_Label	(new_Table,	tab_Index, steps_Row,   current_Column);
					create_Nu0_Spin_Box (new_Table, tab_Index, steps_Row+1, current_Column);
					nu0_Is_Done = true;
				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// fractal alpha and beta
			bool show_Alpha = false;
			if(	struct_Data.roughness_Model.is_Enabled )
			{
				if( struct_Data.item_Type == item_Type_Substrate)
				{
					if( multilayer->imperfections_Model.PSD_Model == ABC_Model ||
						multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model )
					{
						show_Alpha = true;
					}
				}
				if( struct_Data.item_Type == item_Type_Layer)
				{
					if( (multilayer->imperfections_Model.PSD_Model == ABC_Model ||
						 multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model) &&
						 (multilayer->imperfections_Model.vertical_Correlation == zero_Correlation && multilayer->imperfections_Model.use_Common_Roughness_Function == false))
					{
						show_Alpha = true;
					}
					if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation &&
						multilayer->imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model)
					{
						show_Alpha = true;
					}
				}
			}
			bool show_Beta = false;
			if(	struct_Data.roughness_Model.is_Enabled )
			{
				if( struct_Data.item_Type == item_Type_Layer)
				{
					if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation &&
						multilayer->imperfections_Model.inheritance_Model == replication_Factor_Inheritance_Model)
					{
						show_Beta = true;
					}
				}
			}

			if(show_Alpha)
			{
				QString whats_This = whats_This_Fractal_Alpha;
				add_Columns			(new_Table, current_Column+additional_Column);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Alpha_Sym);
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// alpha step
				if(!steps_Are_Done_Alpha && !show_Beta)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Alpha_Sym);
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Alpha = true;
				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			if(show_Beta)
			{
				QString whats_This = whats_This_Fractal_Beta;
				add_Columns			(new_Table, current_Column+additional_Column);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Beta_Sym);
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// alpha step
				if(!steps_Are_Done_Alpha)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Alpha_Sym+"/"+Beta_Sym);
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Alpha = true;
				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// linear growth a1,a2,a3,a4
			bool show_Linear_A = false;
			if(	struct_Data.roughness_Model.is_Enabled )
			{
				if( struct_Data.item_Type == item_Type_Layer)
				{
					if( multilayer->imperfections_Model.vertical_Correlation == partial_Correlation &&
						multilayer->imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model)
					{
						show_Linear_A = true;
					}
				}
			}
			if(show_Linear_A)
			{
				QString whats_This = whats_This_Linear_A1;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "a"+Subscript_1_Sym);
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=1;

				//-----------------------------------

				whats_This = whats_This_Linear_A2;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "a"+Subscript_2_Sym+" ["+linear_a2_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=1;

				//-----------------------------------

				whats_This = whats_This_Linear_A3;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "a"+Subscript_3_Sym+" ["+linear_a3_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=1;

				//-----------------------------------

				whats_This = whats_This_Linear_A4;
				add_Columns			(new_Table, current_Column+additional_Column);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "a"+Subscript_4_Sym+" ["+linear_a4_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------


			// sigma-scaling for loaded 1D PSD
			bool show_Sigma_Factor_PSD_1D = false;
			if(	struct_Data.roughness_Model.is_Enabled && multilayer->imperfections_Model.add_Measured_PSD_1D)
			{
				if( struct_Data.item_Type == item_Type_Substrate)
				{
					show_Sigma_Factor_PSD_1D = true;
				}
			}
			if(show_Sigma_Factor_PSD_1D)
			{
				QString whats_This = whats_This_Sigma_Factor_PSD_1D;
				add_Columns				 (new_Table, current_Column+additional_Column);
				// first
				MyDoubleSpinBox* PSD_Sigma_Lineedit = create_PSD_Sigma_Lineedit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, multilayer, PSD_Type_1D);
				create_PSD_Load_Button	 (new_Table,			current_Row,   current_Column, multilayer, PSD_Type_1D, PSD_Sigma_Lineedit);
				create_Simple_Label		 (new_Table, tab_Index, current_Row+1, current_Column, whats_This_Sigma_Eff_PSD, Sigma_Sym+Subscript_e_Sym+" ["+length_units+"]");
				create_Label			 (new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, "rf 1D");
				create_Line_Edit		 (new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, VAL, PSD_Sigma_Lineedit);
//				create_Line_Edit		 (new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
//				create_Line_Edit		 (new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
//				create_Check_Box_Fit	 (new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 0, 0, 0, 0);

				// PSD sigma factor step
				if(!steps_Are_Done_Sigma_Factor_PSD)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, "rf");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Sigma_Factor_PSD = true;
				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// sigma-scaling for loaded 2D PSD
			bool show_Sigma_Factor_PSD_2D = false;
			if(	struct_Data.roughness_Model.is_Enabled && multilayer->imperfections_Model.add_Measured_PSD_2D)
			{
				if( struct_Data.item_Type == item_Type_Substrate)
				{
					show_Sigma_Factor_PSD_2D = true;
				}
			}
			if(show_Sigma_Factor_PSD_2D)
			{
				QString whats_This = whats_This_Sigma_Factor_PSD_2D;
				add_Columns				 (new_Table, current_Column + additional_Column);
				// first
				MyDoubleSpinBox* PSD_Sigma_Lineedit = create_PSD_Sigma_Lineedit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, multilayer, PSD_Type_2D);
				create_PSD_Load_Button	 (new_Table,		    current_Row,   current_Column, multilayer, PSD_Type_2D, PSD_Sigma_Lineedit);
				create_Simple_Label		 (new_Table, tab_Index, current_Row+1, current_Column, whats_This_Sigma_Eff_PSD, Sigma_Sym+Subscript_e_Sym+" ["+length_units+"]");
				create_Label			 (new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, "rf 2D");
				create_Line_Edit		 (new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, VAL, PSD_Sigma_Lineedit);

//				create_Line_Edit		 (new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
//				create_Line_Edit		 (new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
//				create_Check_Box_Fit	 (new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 0, 0, 0, 0);

				// PSD sigma factor step
				if(!steps_Are_Done_Sigma_Factor_PSD)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, "rf");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Sigma_Factor_PSD = true;
				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// peak sigma
			bool show_Peak_Sigma = false;
			if(	struct_Data.roughness_Model.is_Enabled && multilayer->imperfections_Model.add_Gauss_Peak)
			{
				if( struct_Data.item_Type == item_Type_Substrate)
				{
					show_Peak_Sigma = true;
				}
				if(struct_Data.item_Type == item_Type_Layer)
				{
					if(multilayer->imperfections_Model.use_Common_Roughness_Function == false || multilayer->imperfections_Model.vertical_Correlation != partial_Correlation)
					{
						show_Peak_Sigma = true;
					}
				}
			}
			if(show_Peak_Sigma)
			{
				QString whats_This = whats_This_Roughness_Peak_Sigma;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Sigma_Sym+Subscript_v_Sym+" ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// peak sigma step
				if(!steps_Are_Done_Peak_Sigma)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Sigma_Sym+Subscript_v_Sym+" ["+length_units+"]");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Peak_Sigma = true;
				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// peak frequency
			bool show_Peak_Frequency = false;
			if(	struct_Data.roughness_Model.is_Enabled && multilayer->imperfections_Model.add_Gauss_Peak)
			{
				if( struct_Data.item_Type == item_Type_Substrate)
				{
					show_Peak_Frequency = true;
				}
				if(struct_Data.item_Type == item_Type_Layer)
				{
					if(multilayer->imperfections_Model.use_Common_Roughness_Function == false || multilayer->imperfections_Model.vertical_Correlation != partial_Correlation)
					{
						show_Peak_Frequency = true;
					}
				}
			}
			if(show_Peak_Frequency)
			{
				QString whats_This = whats_This_Roughness_Peak_Frequency;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Nu_Sym+" ["+spatial_frequency_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				/// (adaptive step instead of fixed)
//				// peak frequency step
//				if(!steps_Are_Done_Peak_Frequency)
//				{
//					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Nu_Sym+" ["+Mu_Sym+"m"+Minus_One_Sym+"]");
//					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
//					steps_Are_Done_Peak_Frequency = true;
//				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// peak frequency width
			bool show_Peak_Frequency_Width = false;
			if(	struct_Data.roughness_Model.is_Enabled && multilayer->imperfections_Model.add_Gauss_Peak)
			{
				if( struct_Data.item_Type == item_Type_Substrate)
				{
					show_Peak_Frequency_Width = true;
				}
				if(struct_Data.item_Type == item_Type_Layer)
				{
					if(multilayer->imperfections_Model.use_Common_Roughness_Function == false || multilayer->imperfections_Model.vertical_Correlation != partial_Correlation)
					{
						show_Peak_Frequency_Width = true;
					}
				}
			}
			if(show_Peak_Frequency_Width)
			{
				QString whats_This = whats_This_Roughness_Peak_Frequency_Width;
				add_Columns			(new_Table, current_Column + additional_Column);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Delta_Small_Sym+Nu_Sym+" ["+spatial_frequency_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				/// (adaptive step instead of fixed)
//				// peak frequency step
//				if(!steps_Are_Done_Peak_Frequency)
//				{
//					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Nu_Sym+" ["+Mu_Sym+"m"+Minus_One_Sym+"]");
//					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
//					steps_Are_Done_Peak_Frequency = true;
//				}
				last_Roughness_Column = max(current_Column,last_Roughness_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// alignment
			if(	multilayer->imperfections_Model.use_Roughness )
			{
				if( struct_Data.item_Type == item_Type_Layer)
				{
					if( multilayer->imperfections_Model.PSD_Model == ABC_Model || multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model)
					{
						if((multilayer->imperfections_Model.vertical_Correlation != partial_Correlation &&
							multilayer->imperfections_Model.use_Common_Roughness_Function == true) ||
							!struct_Data.roughness_Model.is_Enabled)
						{
							add_Columns	(new_Table, current_Column+4+additional_Column);
							current_Column+=5+additional_Column;
						}
					}
					if( multilayer->imperfections_Model.add_Measured_PSD_1D)
					{
						add_Columns	(new_Table, current_Column+1+additional_Column);
						current_Column+=1+additional_Column;
					}
					if( multilayer->imperfections_Model.add_Measured_PSD_2D)
					{
						add_Columns	(new_Table, current_Column+1+additional_Column);
						current_Column+=1+additional_Column;
					}
					if( multilayer->imperfections_Model.add_Gauss_Peak &&
						multilayer->imperfections_Model.use_Common_Roughness_Function == true)
					{
						add_Columns	(new_Table, current_Column+4+additional_Column);
						current_Column+=5+additional_Column;
					}
				}
			}

			///--------------------------------------------------------------------------------------------
			/// PARTICLES
			///--------------------------------------------------------------------------------------------

			first_Particles_Column = current_Column;

			bool show_Whole_Layer = is_Last_Layer || multilayer->imperfections_Model.particle_Vertical_Correlation == zero_Correlation &&
													 multilayer->imperfections_Model.use_Common_Particle_Function == false;

			bool show_Partial_Common_Layer = is_Second_Last_Layer && multilayer->imperfections_Model.particle_Vertical_Correlation == partial_Correlation &&
																	 multilayer->imperfections_Model.use_Common_Particle_Function == true;

			bool show_Partial_Individual_Layer = !is_Last_Layer && multilayer->imperfections_Model.particle_Vertical_Correlation == partial_Correlation &&
																   multilayer->imperfections_Model.use_Common_Particle_Function == false;

			bool show_Full_Individual_Layer = !is_Last_Layer && multilayer->imperfections_Model.particle_Vertical_Correlation == full_Correlation &&
																multilayer->imperfections_Model.use_Common_Particle_Function == false;


			// usage, shape and interference function
			bool show_Usage_Buttons = false;
			if(	multilayer->imperfections_Model.use_Particles && struct_Data.item_Type == item_Type_Layer)
			{
				show_Usage_Buttons = true;
			}
			if(show_Usage_Buttons)
			{
				add_Columns				 (new_Table,                           current_Column+1);
				if(	struct_Data.particles_Model.is_Enabled )
				{
					if(show_Whole_Layer || show_Partial_Individual_Layer || show_Full_Individual_Layer) {
						create_Shape_Button  (new_Table,            current_Row+2, current_Column, structure_Item);
					}
					if(show_Whole_Layer) {
						create_Pattern_Button(new_Table,            current_Row+3, current_Column, structure_Item);
						create_Model_Button	 (new_Table,            current_Row+4, current_Column, structure_Item);
					}
				} else
				{
					new_Table->setSpan(current_Row+1,current_Column,4,1);
				}
				// last
				create_Check_Box_Usage(new_Table, tab_Index, current_Row, current_Column, structure_Item, "on/off", 0, 4, 0, 2020); // more than table size, it is like inf

				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}

			// particle material
			bool show_Particle_Material = false;
			if(	struct_Data.item_Type == item_Type_Layer && multilayer->imperfections_Model.use_Particles_Material )
			{
				show_Particle_Material = true;
			}
			if(show_Particle_Material)
			{
				if(struct_Data.composed_Material)
				{
					create_Combo_Elements	    (new_Table, tab_Index, current_Row,   current_Column, structure_Item, true);
					create_Stoich_Line_Edit	    (new_Table, tab_Index, current_Row+1, current_Column, structure_Item, true, VAL);
					create_Stoich_Line_Edit	    (new_Table, tab_Index, current_Row+3, current_Column, structure_Item, true, MIN);
					create_Stoich_Line_Edit	    (new_Table, tab_Index, current_Row+4, current_Column, structure_Item, true, MAX);

					// it should be created last
					create_Stoich_Check_Box_Fit	(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, true, 1, 2, 0, 0);
				}
				else
				{
					create_Material_Line_Edit	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, true);
					create_Browse_Button	    (new_Table,	current_Row+1, current_Column, current_Row, current_Column);
				}
				current_Column += (max_Number_Of_Elements+1);
			}
			///--------------------------------------------------------------------------------------------
			///
			// particle density
			bool show_Particle_Density = false;
			if(	struct_Data.particles_Model.is_Enabled)
			{
				show_Particle_Density = true;
			}
			if(show_Particle_Density)
			{
				QString whats_This;
				add_Columns			(new_Table, current_Column+1);
				if(struct_Data.composed_Material)
				{
					whats_This = whats_This_Particle_Absolute_Density;
					create_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Rho_Sym + Subscript_p_Sym + " ["+density_units+"]");
				} else
				{
					whats_This = whats_This_Particle_Relative_Density;
					create_Label	(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Rho_Sym + Subscript_p_Sym + " [r.u.]");
				}
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// particle density step
				if(!steps_Are_Done_Particle_Density)
				{
					if(struct_Data.composed_Material)
					{
						create_Simple_Label(new_Table, tab_Index, steps_Row, current_Column, whats_This, Rho_Sym + Subscript_p_Sym + " ["+density_units+"]");
					} else
					{
						create_Simple_Label(new_Table, tab_Index, steps_Row, current_Column, whats_This, Rho_Sym + Subscript_p_Sym + " [r.u.]");
					}
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This_Particle_Density);
					steps_Are_Done_Particle_Density = true;
				}
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}
			if(!show_Particle_Density && struct_Data.particles_Model.is_Enabled) // keep empty place
			{
				add_Columns	(new_Table, current_Column+1);
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// particle radius
			bool show_Particle_Radius = false;
			if(struct_Data.particles_Model.is_Enabled && struct_Data.particles_Model.is_Independent)
			{
//				if(show_Whole_Layer || show_Partial_Individual_Layer || show_Full_Individual_Layer)
				{
					if( struct_Data.particles_Model.particle_Shape == full_Sphere ||
						struct_Data.particles_Model.particle_Shape == full_Spheroid ||
						struct_Data.particles_Model.particle_Shape == cylinder )
					{
						show_Particle_Radius = true;
					}
				}
			}
			if(show_Particle_Radius)
			{
				QString whats_This = whats_This_Particle_Radius;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "R ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// particle radius step
				if(!steps_Are_Done_Particle_Radius)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, "R ["+length_units+"]");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Particle_Radius = true;
				}
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}
			if(!show_Particle_Radius && struct_Data.particles_Model.is_Enabled) // keep empty place
			{
				add_Columns	(new_Table, current_Column+1);
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// particle height
			bool show_Particle_Height = false;
			if(	struct_Data.particles_Model.is_Enabled && struct_Data.particles_Model.is_Independent)
			{
//				if(show_Whole_Layer || show_Partial_Individual_Layer || show_Full_Individual_Layer)
				{
					if( struct_Data.particles_Model.particle_Shape == full_Spheroid ||
						struct_Data.particles_Model.particle_Shape == cylinder )
					{
						show_Particle_Height = true;
					}
				}
			}
			if(show_Particle_Height)
			{
				QString whats_This = whats_This_Particle_Height;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "H ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// particle radius step
				if(!steps_Are_Done_Particle_Height)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, "H ["+length_units+"]");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Particle_Height = true;
				}
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}
			if(has_Particle_Height && !show_Particle_Height && struct_Data.particles_Model.is_Enabled) // keep empty place
			{
				add_Columns	(new_Table, current_Column+1);
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			// particles correlation depth
			bool show_Particle_Correlation_Depth = false;
			if(	struct_Data.particles_Model.is_Enabled )
			{
				if(!is_Last_Layer && (show_Partial_Common_Layer || show_Partial_Individual_Layer))
				{
					show_Particle_Correlation_Depth = true;
				}
			}
			if(show_Particle_Correlation_Depth)
			{
				QString whats_This = whats_This_Particle_Correlation_Depth;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "L"+Subscript_p_Sym+" ["+mu_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=1;
			}
			///--------------------------------------------------------------------------------------------

			// particle average distance
			bool show_Particle_Average_Distance = false;
			if(	struct_Data.particles_Model.is_Enabled && struct_Data.particles_Model.is_Independent )
			{
				if(show_Whole_Layer)
				{
					if( struct_Data.particles_Model.particle_Interference_Function == disorder)
					{
						show_Particle_Average_Distance = true;
					}
				}
			}
			if(show_Particle_Average_Distance)
			{
				QString whats_This = whats_This_Particle_Average_Distance;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "r" + Subscript_a_Sym + " ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// particle distance step
				if(!steps_Are_Done_Particle_Distance)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This_Particle_Distance, "r ["+length_units+"]");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This_Particle_Distance);
					steps_Are_Done_Particle_Distance = true;
				}
				last_Particles_Column = max(current_Column,last_Particles_Column);
				if(has_Particle_Distance_Deviation)	{current_Column+=2;}
				else								{current_Column+=2;}
			}
			if( !has_Particle_Distance_Deviation &&
				!show_Particle_Average_Distance &&
				struct_Data.particles_Model.is_Enabled) // keep empty place
			{
				add_Columns	(new_Table, current_Column+1);
				last_Particles_Column = max(current_Column,last_Particles_Column);
				if(!show_Particle_Correlation_Depth)	current_Column+=2;
				else									current_Column+=1;
			}
			///--------------------------------------------------------------------------------------------

			// particle radial distance
			bool show_Particle_Radial_Distance = false;
			if(	struct_Data.particles_Model.is_Enabled && struct_Data.particles_Model.is_Independent )
			{
				if(show_Whole_Layer)
				{
					if( struct_Data.particles_Model.particle_Interference_Function == radial_Paracrystal)
					{
						show_Particle_Radial_Distance = true;
					}
				}
			}
			if(show_Particle_Radial_Distance)
			{
				QString whats_This = whats_This_Particle_Radial_Distance;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "r ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// particle distance step
				if(!steps_Are_Done_Particle_Distance)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This_Particle_Distance, "r ["+length_units+"]");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This_Particle_Distance);
					steps_Are_Done_Particle_Distance = true;
				}
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=1;
			}
			if( !show_Particle_Correlation_Depth &&
				!show_Particle_Average_Distance &&
				has_Particle_Distance_Deviation &&
				!show_Particle_Radial_Distance &&
				struct_Data.particles_Model.is_Enabled) // keep empty place
			{
				add_Columns	(new_Table, current_Column+1);
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=1;
			}
			///--------------------------------------------------------------------------------------------

			// particle radial distance deviation
			bool show_Particle_Radial_Distance_Deviation = false;
			if(	struct_Data.particles_Model.is_Enabled && struct_Data.particles_Model.is_Independent )
			{
				if(show_Whole_Layer)
				{
					if( struct_Data.particles_Model.particle_Interference_Function == radial_Paracrystal)
					{
						show_Particle_Radial_Distance_Deviation = true;
					}
				}
			}
			if(show_Particle_Radial_Distance_Deviation)
			{
				QString whats_This = whats_This_Particle_Radial_Distance_Deviation;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Delta_Small_Sym + "r ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// particle radial distance deviation step
				if(!steps_Are_Done_Particle_Distance_Deviation)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Delta_Small_Sym + "r ["+length_units+"]");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Particle_Distance_Deviation = true;
				}
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=1;
			}
			if( has_Particle_Distance_Deviation &&
				!show_Particle_Average_Distance &&
				!show_Particle_Radial_Distance_Deviation &&
				struct_Data.particles_Model.is_Enabled) // keep empty place
			{
				add_Columns	(new_Table, current_Column+1);
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=1;
			}
			///--------------------------------------------------------------------------------------------

			// domain size
			bool show_Domain_Size = false;
			if(	struct_Data.particles_Model.is_Enabled && struct_Data.particles_Model.is_Independent )
			{
				if(show_Whole_Layer)
				{
					if( struct_Data.particles_Model.particle_Interference_Function == radial_Paracrystal)
					{
						show_Domain_Size = true;
					}
				}
			}
			if(show_Domain_Size)
			{
				QString whats_This = whats_This_Domain_Size;
				add_Columns			(new_Table, current_Column+1);
//				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "D ["+length_units+"]");
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, "D ["+correlation_length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				/// (adaptive step instead of fixed)
				// domain size
//				if(!steps_Are_Done_Particle_Distance_Deviation)
//				{
//					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Delta_Small_Sym + "r ["+length_units+"]");
//					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
//					steps_Are_Done_Particle_Distance_Deviation = true;
//				}
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}
			if( has_Particle_Distance_Deviation &&
				!show_Domain_Size &&
				struct_Data.particles_Model.is_Enabled) // keep empty place
			{
				add_Columns	(new_Table, current_Column+1);
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			if( struct_Data.particles_Model.particle_Interference_Function == disorder &&
				multilayer->imperfections_Model.particle_Vertical_Correlation != zero_Correlation &&
			    has_Particle_Distance_Deviation &&
			    last_Layer_Data.particles_Model.particle_Interference_Function != radial_Paracrystal)
			{
				current_Column-=2;
			}

			// particle z position
			bool show_Particle_Z_Position = false;
			if(	struct_Data.particles_Model.is_Enabled && struct_Data.particles_Model.is_Independent )
			{
//				if(show_Whole_Layer || show_Partial_Individual_Layer || show_Full_Individual_Layer)
				{
					show_Particle_Z_Position = true;
				}
			}
			if(show_Particle_Z_Position)
			{
				QString whats_This = whats_This_Particle_Z_Position;
				add_Columns			(new_Table, current_Column+1);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This,"z" + Subscript_p_Sym + " ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// particle radial distance step
				if(!steps_Are_Done_Particle_Z_Position)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, "z" + Subscript_p_Sym + " ["+length_units+"]");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Particle_Z_Position = true;
				}
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=1;
			}
			if(!show_Particle_Z_Position && struct_Data.particles_Model.is_Enabled) // keep empty place
			{
				add_Columns	(new_Table, current_Column+1);
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=1;
			}
			///--------------------------------------------------------------------------------------------

			// particle z position deviation
			bool show_Particle_Z_Position_Deviation = false;
			if(	struct_Data.particles_Model.is_Enabled && struct_Data.particles_Model.is_Independent )
			{
//				if(show_Whole_Layer || show_Partial_Individual_Layer || show_Full_Individual_Layer)
				{
					show_Particle_Z_Position_Deviation = true;
				}
			}
			if(show_Particle_Z_Position_Deviation)
			{
				QString whats_This = whats_This_Particle_Z_Position_Deviation;
				int additional_Column = show_Right_Column ? 1 : 0;
				add_Columns			(new_Table, current_Column + additional_Column);
				create_Label		(new_Table, tab_Index, current_Row,   current_Column, structure_Item, whats_This, Delta_Small_Sym + "z" + Subscript_p_Sym + " ["+length_units+"]");
				create_Line_Edit	(new_Table, tab_Index, current_Row+1, current_Column, structure_Item, whats_This, VAL);
				create_Line_Edit	(new_Table, tab_Index, current_Row+3, current_Column, structure_Item, whats_This, MIN);
				create_Line_Edit	(new_Table, tab_Index, current_Row+4, current_Column, structure_Item, whats_This, MAX);
				// last
				create_Check_Box_Fit(new_Table, tab_Index, current_Row+2, current_Column, structure_Item, whats_This, 1, 2, 0, 0);

				// particle radial distance deviation step
				if(!steps_Are_Done_Particle_Z_Position_Deviation)
				{
					create_Simple_Label	(new_Table,	tab_Index, steps_Row,   current_Column, whats_This, Delta_Small_Sym + "z" + Subscript_p_Sym + " ["+length_units+"]");
					create_Step_Spin_Box(new_Table, tab_Index, steps_Row+1, current_Column, whats_This);
					steps_Are_Done_Particle_Z_Position_Deviation = true;
				}
				last_Particles_Column = max(current_Column,last_Particles_Column);
				current_Column+=2;
			}
			///--------------------------------------------------------------------------------------------

			rows_List_To_Span.append(current_Row-1);
			++it;
		}

		// big label about roughness model
		int first_Row = show_Top_Row ? 1 : 0;
		if(multilayer->imperfections_Model.use_Roughness)
		{
			int broadening = 0;
//			if(multilayer->imperfections_Model.PSD_Model == measured_PSD) broadening = 1;
			int right_Broadening_Factor = show_Right_Column || (multilayer->imperfections_Model.use_Particles && has_Layers) ? 2 : 1;

			// row 1
			QLabel* approximation_Label = new QLabel(multilayer->imperfections_Model.approximation);
				approximation_Label->setAlignment(Qt::AlignCenter);
				approximation_Label->setFont(QFont(approximation_Label->font().family(), 10, QFont::Bold));
			new_Table->setSpan(first_Row,first_Roughness_Column-broadening,1,last_Roughness_Column-first_Roughness_Column+1+right_Broadening_Factor*broadening);
			new_Table->setCellWidget(first_Row, first_Roughness_Column, approximation_Label);

			// row 2
			QLabel* model_Label = new QLabel(multilayer->imperfections_Model.PSD_Model);
				model_Label->setAlignment(Qt::AlignCenter);
				model_Label->setFont(QFont(model_Label->font().family(), 10, QFont::Bold));
			new_Table->setSpan(first_Row+1,first_Roughness_Column-broadening,1,last_Roughness_Column-first_Roughness_Column+1+right_Broadening_Factor*broadening);
			new_Table->setCellWidget(first_Row+1, first_Roughness_Column, model_Label);

			if(multilayer->imperfections_Model.add_Measured_PSD_1D || multilayer->imperfections_Model.add_Measured_PSD_2D)
			{
				model_Label->setText(model_Label->text() + " + external PSD");
			}
			if(multilayer->imperfections_Model.add_Gauss_Peak)
			{
				model_Label->setText(model_Label->text() + " + Gauss peak");
			}

			// row 3
			QLabel* ver_Cor_Label = new QLabel(multilayer->imperfections_Model.vertical_Correlation);
				ver_Cor_Label->setAlignment(Qt::AlignCenter);
				ver_Cor_Label->setFont(QFont(ver_Cor_Label->font().family(), 10, QFont::Bold));
			new_Table->setSpan(first_Row+2,first_Roughness_Column-broadening,1,last_Roughness_Column-first_Roughness_Column+1+right_Broadening_Factor*broadening);
			new_Table->setCellWidget(first_Row+2, first_Roughness_Column, ver_Cor_Label);
		}

		// big label about particles model
		if(multilayer->imperfections_Model.use_Particles && has_Layers)
		{
			// row 1
			QLabel* approximation_Label = new QLabel("DWBA approximation");
				approximation_Label->setAlignment(Qt::AlignCenter);
				approximation_Label->setFont(QFont(approximation_Label->font().family(), 10, QFont::Bold));
			new_Table->setSpan(first_Row,first_Particles_Column,1,last_Particles_Column-first_Particles_Column+1);
			new_Table->setCellWidget(first_Row, first_Particles_Column, approximation_Label);

			// row 2
			bool empty_Model = multilayer->imperfections_Model.particle_Vertical_Correlation == zero_Correlation && multilayer->imperfections_Model.use_Common_Particle_Function == false;
			QString model_Text = "";
			QString geometry_Text = "";
			if(!empty_Model)
			{
				if(last_Layer_Data.particles_Model.particle_Interference_Function == radial_Paracrystal) {
					model_Text = "radial paracrystal";
				}
				if(last_Layer_Data.particles_Model.particle_Interference_Function == disorder) {
					model_Text = "disorder";
				}
				if(last_Layer_Data.particles_Model.geometric_Model == square_Model) {
					geometry_Text = ", square model";
				}
				if(last_Layer_Data.particles_Model.geometric_Model == hexagonal_Model) {
					geometry_Text = ", hexagonal model";
				}
				if(last_Layer_Data.particles_Model.geometric_Model == pure_Radial_Model) {
					geometry_Text = ", pure radial model";
				}
			}
			QLabel* model_Label = new QLabel(model_Text+geometry_Text);//"in-layer interference"
				model_Label->setAlignment(Qt::AlignCenter);
				model_Label->setFont(QFont(model_Label->font().family(), 10, QFont::Bold));
			new_Table->setSpan(first_Row+1,first_Particles_Column,1,last_Particles_Column-first_Particles_Column+1);
			new_Table->setCellWidget(first_Row+1, first_Particles_Column, model_Label);

			// row 3
			QLabel* ver_Cor_Label = new QLabel(multilayer->imperfections_Model.particle_Vertical_Correlation);
				ver_Cor_Label->setAlignment(Qt::AlignCenter);
				ver_Cor_Label->setFont(QFont(ver_Cor_Label->font().family(), 10, QFont::Bold));
			new_Table->setSpan(first_Row+2,first_Particles_Column,1,last_Particles_Column-first_Particles_Column+1);
			new_Table->setCellWidget(first_Row+2, first_Particles_Column, ver_Cor_Label);
		}

		// add or not last row
		if(show_Bottom_Row) rows_List_To_Span.append(new_Table->rowCount()-1);
		else					new_Table->removeRow(new_Table->rowCount()-1);
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
		// colorize fit checkbox
		QWidget* back_Widget_Fit = back_Widgets_Fit_Map.key(parameter->indicator.id);
		if(back_Widget_Fit)
		{
			if(parameter->fit.is_Fitable && !parameter->coupled.master.exist)
			{	back_Widget_Fit->setStyleSheet(fit_Color);}
			else
			{	back_Widget_Fit->setStyleSheet(white_Color);}
		}

		// has no dependencies
		if(!parameter->coupled.master.exist && parameter->coupled.slaves.size()==0)
		{	
			if(back_Widget->property(fit_Text).toString() != fit_Text)
			{
				back_Widget->setStyleSheet(free_Parameter_Color);
			}
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
		{
			fit_Check_Box->toggled(fit_Check_Box->isChecked());
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

	// optical properties
	if(whats_This == whats_This_Absolute_Density)						{precision = line_edit_density_precision;				coeff = 1;						return struct_Data.absolute_Density;			}
	if(whats_This == whats_This_Relative_Density)						{precision = line_edit_density_precision;				coeff = 1;						return struct_Data.relative_Density;			}

	// thickness
	if(whats_This == whats_This_Thickness)								{precision = line_edit_thickness_precision;				coeff = length_Coefficients_Map.value(length_units);	return struct_Data.thickness;	}
	if(whats_This == whats_This_Thickness_Drift_Line_Value)				{precision = line_edit_drift_precision;					coeff = 1;					return struct_Data.thickness_Drift.drift_Line_Value;			}
	if(whats_This == whats_This_Thickness_Drift_Rand_Rms)				{precision = line_edit_drift_precision;					coeff = 1;					return struct_Data.thickness_Drift.drift_Rand_Rms;				}
	if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)			{precision = line_edit_drift_precision;					coeff = 1;					return struct_Data.thickness_Drift.drift_Sine_Amplitude;		}
	if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)			{precision = line_edit_drift_precision;					coeff = 1;					return struct_Data.thickness_Drift.drift_Sine_Frequency;		}
	if(whats_This == whats_This_Thickness_Drift_Sine_Phase)				{precision = line_edit_drift_precision;					coeff = 1;					return struct_Data.thickness_Drift.drift_Sine_Phase;			}

	// interface
	if(whats_This == whats_This_Sigma_Diffuse)							{precision = line_edit_sigma_precision;					coeff = length_Coefficients_Map.value(length_units);	return struct_Data.sigma_Diffuse;		 }
	if(whats_This == whats_This_Sigma_Drift_Line_Value)					{precision = line_edit_drift_precision;					coeff = 1;						return struct_Data.sigma_Diffuse_Drift.drift_Line_Value;		 }
	if(whats_This == whats_This_Sigma_Drift_Rand_Rms)					{precision = line_edit_drift_precision;					coeff = 1;						return struct_Data.sigma_Diffuse_Drift.drift_Rand_Rms;			 }
	if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)				{precision = line_edit_drift_precision;					coeff = 1;						return struct_Data.sigma_Diffuse_Drift.drift_Sine_Amplitude;	 }
	if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)				{precision = line_edit_drift_precision;					coeff = 1;						return struct_Data.sigma_Diffuse_Drift.drift_Sine_Frequency;	 }
	if(whats_This == whats_This_Sigma_Drift_Sine_Phase)					{precision = line_edit_drift_precision;					coeff = 1;						return struct_Data.sigma_Diffuse_Drift.drift_Sine_Phase;		 }

	// roughness
	if(whats_This == whats_This_Sigma_Roughness)						{precision = line_edit_sigma_precision;					coeff = length_Coefficients_Map.value(length_units);						return struct_Data.roughness_Model.sigma;				}
	if(whats_This == whats_This_Correlation_Radius)						{precision = line_edit_cor_radius_precision;			coeff = correlation_Length_Coefficients_Map.value(correlation_length_units);return struct_Data.roughness_Model.cor_radius;			}
	if(whats_This == whats_This_Fractal_Alpha)							{precision = line_edit_fractal_alpha_beta_precision;	coeff = 1;																	return struct_Data.roughness_Model.fractal_alpha;		}
	if(whats_This == whats_This_Fractal_Beta)							{precision = line_edit_fractal_alpha_beta_precision;	coeff = 1;																	return struct_Data.roughness_Model.fractal_beta;		}
	if(whats_This == whats_This_Linear_PSD_Omega)						{precision = line_edit_omega_precision;					coeff = omega_Coefficients_Map.value(omega_units);							return struct_Data.roughness_Model.omega;				}
	if(whats_This == whats_This_PSD_Exponenta_Mu)						{precision = line_edit_mu_precision;					coeff = mu_Coefficients_Map.value(mu_units);								return struct_Data.roughness_Model.mu;					}

	if(whats_This == whats_This_Linear_A1)								{precision = line_edit_linear_a_precision;				coeff = 1;																	return struct_Data.roughness_Model.a1;		}
	if(whats_This == whats_This_Linear_A2)								{precision = line_edit_linear_a_precision;				coeff = linear_a2_Coefficients_Map.value(linear_a2_units);					return struct_Data.roughness_Model.a2;		}
	if(whats_This == whats_This_Linear_A3)								{precision = line_edit_linear_a_precision;				coeff = linear_a3_Coefficients_Map.value(linear_a3_units);					return struct_Data.roughness_Model.a3;		}
	if(whats_This == whats_This_Linear_A4)								{precision = line_edit_linear_a_precision;				coeff = linear_a4_Coefficients_Map.value(linear_a4_units);					return struct_Data.roughness_Model.a4;		}

	if(whats_This == whats_This_Roughness_Peak_Sigma)					{precision = line_edit_sigma_precision;					coeff = length_Coefficients_Map.value(length_units);						return struct_Data.roughness_Model.peak_Sigma;			}
	if(whats_This == whats_This_Roughness_Peak_Frequency)				{precision = line_edit_frequency_precision;				coeff = spatial_Frequency_Coefficients_Map.value(spatial_frequency_units);	return struct_Data.roughness_Model.peak_Frequency;		}
	if(whats_This == whats_This_Roughness_Peak_Frequency_Width)			{precision = line_edit_frequency_precision;				coeff = spatial_Frequency_Coefficients_Map.value(spatial_frequency_units);	return struct_Data.roughness_Model.peak_Frequency_Width;}
	if(whats_This == whats_This_Sigma_Factor_PSD_1D)					{precision = line_edit_psd_factor_precision;			coeff = 1;																	return struct_Data.roughness_Model.sigma_Factor_PSD_1D; }
	if(whats_This == whats_This_Sigma_Factor_PSD_2D)					{precision = line_edit_psd_factor_precision;			coeff = 1;																	return struct_Data.roughness_Model.sigma_Factor_PSD_2D; }

	// particles
	if(whats_This == whats_This_Particle_Absolute_Density)		{precision = line_edit_density_precision;						coeff = 1;																		return struct_Data.particles_Model.particle_Absolute_Density;			}
	if(whats_This == whats_This_Particle_Relative_Density)		{precision = line_edit_density_precision;						coeff = 1;																		return struct_Data.particles_Model.particle_Relative_Density;			}
	if(whats_This == whats_This_Particle_Radius)				{precision = line_edit_particle_size_precision;					coeff = length_Coefficients_Map.value(length_units);							return struct_Data.particles_Model.particle_Radius;						}
	if(whats_This == whats_This_Particle_Height)				{precision = line_edit_particle_size_precision;					coeff = length_Coefficients_Map.value(length_units);							return struct_Data.particles_Model.particle_Height;						}
	if(whats_This == whats_This_Particle_Average_Distance)		{precision = line_edit_particle_lateral_distance_precision;		coeff = length_Coefficients_Map.value(length_units);							return struct_Data.particles_Model.particle_Average_Distance;			}
	if(whats_This == whats_This_Particle_Radial_Distance)		{precision = line_edit_particle_lateral_distance_precision;		coeff = length_Coefficients_Map.value(length_units);							return struct_Data.particles_Model.particle_Radial_Distance;			}
if(whats_This == whats_This_Particle_Radial_Distance_Deviation)	{precision = line_edit_particle_lateral_distance_precision;		coeff = length_Coefficients_Map.value(length_units);							return struct_Data.particles_Model.particle_Radial_Distance_Deviation;	}
	if(whats_This == whats_This_Domain_Size)					{precision = line_edit_cor_radius_precision;					coeff = correlation_Length_Coefficients_Map.value(correlation_length_units);	return struct_Data.particles_Model.domain_Size;							}
	if(whats_This == whats_This_Particle_Correlation_Depth)		{precision = line_edit_mu_precision;							coeff = mu_Coefficients_Map.value(mu_units);									return struct_Data.particles_Model.particle_Correlation_Depth;			}
	if(whats_This == whats_This_Particle_Z_Position)			{precision = line_edit_particle_z_position_precision;			coeff = length_Coefficients_Map.value(length_units);							return struct_Data.particles_Model.particle_Z_Position;					}
	if(whats_This == whats_This_Particle_Z_Position_Deviation)	{precision = line_edit_particle_z_position_precision;			coeff = length_Coefficients_Map.value(length_units);							return struct_Data.particles_Model.particle_Z_Position_Deviation;		}

	// multilayer
	if(whats_This == whats_This_Num_Repetitions)			{precision = 0;								coeff = 1;						return struct_Data.num_Repetition.parameter;			}
	if(whats_This == whats_This_Period)						{precision = line_edit_thickness_precision;	coeff = length_Coefficients_Map.value(length_units);	return struct_Data.period;		}
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
				if( item_Type == item_Type_Regular_Aperiodic )
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
	if( item_Type == item_Type_Layer ||	/*item_Type == item_Type_Substrate || */item_Type == item_Type_Multilayer || item_Type == item_Type_Regular_Aperiodic || item_Type == item_Type_General_Aperiodic)
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
		if( item_Type == item_Type_Layer    ||	/*item_Type == item_Type_Substrate || */item_Type == item_Type_Multilayer)	rows = 5;

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
	int max_Width = start_Width;
	for(int row = 0; row<table->rowCount(); ++row)
	{
		QLineEdit* current_Line_Edit = qobject_cast<QLineEdit*>(table->cellWidget(row, current_Column));
		MyDoubleSpinBox* current_Spin_Box = qobject_cast<MyDoubleSpinBox*>(table->cellWidget(row, current_Column));
		QCheckBox* current_CheckBox = qobject_cast<QCheckBox*>(table->cellWidget(row, current_Column));
		QLabel* current_Label = qobject_cast<QLabel*>(table->cellWidget(row, current_Column));

		bool fit_Labels = false;
		if(current_Label)
		{
			fit_Labels = current_Label->property(fit_Column_Property).toBool();
		}

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
	table->setColumnWidth(current_Column, max_Width);

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
void Table_Of_Structures::create_Combo_Elements(My_Table_Widget* table, int, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, bool is_Particle)
{
	QList<Stoichiometry> composition;
	if(is_Particle)	{
		composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().particles_Model.particle_Composition;
	} else {
		composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().composition;
	}

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

		connect(elements, &QComboBox::currentTextChanged, this, [=]{refresh_Element(table, is_Particle);});

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_Stoich_Line_Edit(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, bool is_Particle, QString val_Type)
{
	QList<Stoichiometry> composition;
	if(is_Particle)	{
		composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().particles_Model.particle_Composition;
	} else {
		composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().composition;
	}

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
		connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]{refresh_Stoich(is_Particle);});
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

void Table_Of_Structures::create_Stoich_Check_Box_Fit(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, bool is_Particle, int r_S, int r_F, int c_S, int c_F)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QList<Stoichiometry>& composition = is_Particle ? struct_Data.particles_Model.particle_Composition : struct_Data.composition;

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

		connect(check_Box, &QCheckBox::toggled, this, [=]{refresh_Fit_Element(is_Particle);});
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

void Table_Of_Structures::create_Material_Line_Edit(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, bool is_Particle)
{
	QString material;
	if(is_Particle)	{
		material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().particles_Model.particle_Material;
	} else {
		material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().material;
	}

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
	connect(material_Line_Edit, &QLineEdit::textEdited,	     this, [=]{refresh_Material(table, is_Particle);} );
	connect(material_Line_Edit, &QLineEdit::editingFinished, this, [=]{check_Material(material_Line_Edit, false, is_Particle);} );
}

void Table_Of_Structures::create_Browse_Button(My_Table_Widget* table, int current_Row, int start_Column, int material_LineEdit_Row, int material_LineEdit_Column)
{
	QPushButton* browse_Button = new QPushButton("Browse");
		browse_Button->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
	table->setCellWidget(current_Row, start_Column, browse_Button);

	QLineEdit* material_Line_Edit = qobject_cast<QLineEdit*>(table->cellWidget(material_LineEdit_Row, material_LineEdit_Column));
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
			bool_Check = struct_Data.sigma_Diffuse_Drift.is_Drift_Line;
			id = struct_Data.sigma_Diffuse_Drift.drift_Line_Value.indicator.id;
		}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)
		{
			bool_Check = struct_Data.sigma_Diffuse_Drift.is_Drift_Rand;
			id = struct_Data.sigma_Diffuse_Drift.drift_Rand_Rms.indicator.id;
		}
		if(whats_This == whats_This_Sigma_Drift_Sine)
		{
			bool_Check = struct_Data.sigma_Diffuse_Drift.is_Drift_Sine;
			id = struct_Data.sigma_Diffuse_Drift.drift_Sine_Amplitude.indicator.id;
			id_Type id_2 = struct_Data.sigma_Diffuse_Drift.drift_Sine_Frequency.indicator.id;
			id_Type id_3 = struct_Data.sigma_Diffuse_Drift.drift_Sine_Phase.indicator.id;
			reload_Show_Dependence_Map.insertMulti(check_Box, id_2);
			reload_Show_Dependence_Map.insertMulti(check_Box, id_3);
		}

		if(whats_This == whats_This_Sigma_Diffuse)
		{
			bool_Check = struct_Data.common_Sigma_Diffuse;
			id = struct_Data.sigma_Diffuse.indicator.id;
			for(Interlayer& interlayer : struct_Data.interlayer_Composition)
			{
				reload_Show_Dependence_Map.insertMulti(check_Box, interlayer.my_Sigma_Diffuse.indicator.id);
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

			if(whats_This == whats_This_Common_Thickness)		{bool_Check = parent_Data.regular_Components[my_I].is_Common_Thickness;}
			if(whats_This == whats_This_Common_Sigma_Diffuse)	{bool_Check = parent_Data.regular_Components[my_I].is_Common_Sigma;}
			if(whats_This == whats_This_Restrict_Thickness)		{bool_Check = parent_Data.regular_Components[my_I].use_Soft_Restrictions;}
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
		if( whats_This == whats_This_Sigma_Diffuse)
		{
			check_Box->setAttribute(Qt::WA_TransparentForMouseEvents);
			check_Box->setFocusPolicy(Qt::NoFocus);
		}
		if( whats_This == whats_This_Common_Thickness || whats_This == whats_This_Common_Sigma_Diffuse )
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

void Table_Of_Structures::create_Check_Box_Usage(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString text, int r_S, int r_F, int c_S, int c_F)
{
	QCheckBox* check_Box = new QCheckBox(text);

	Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	check_Box->setChecked(layer_Data.particles_Model.is_Used);

	// enable/disable function
	check_Box->setProperty(row_Property, current_Row);
	check_Box->setProperty(column_Property, current_Column);
	check_Box->setProperty(relative_Rows_To_Disable_Start_Property, r_S);
	check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, r_F);
	check_Box->setProperty(relative_Columns_To_Disable_Start_Property, c_S);
	check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, c_F);

	// for reloading
	check_Box->setProperty(reload_Property, false);
	check_Box->setProperty(tab_Index_Property, tab_Index);

	// storage
	check_Boxes_Map.	       insert	   (check_Box, structure_Item);
	all_Widgets_To_Reload[tab_Index].append(check_Box);
	check_Boxes_Particles_Usage_List[tab_Index].append(check_Box);

	// set up BACK widget
	// alignment
	QWidget* back_Widget = new QWidget;
	QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
	back_Layout->addWidget(check_Box);
	back_Layout->setSpacing(0);
	back_Layout->setContentsMargins(0,0,0,0);
	back_Layout->setAlignment(Qt::AlignCenter);

	connect(check_Box, &QCheckBox::toggled, this, [=]
	{
		if(table_Is_Fully_Created)
		{
			Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
			if(multilayer->imperfections_Model.particle_Vertical_Correlation == zero_Correlation &&
			   multilayer->imperfections_Model.use_Common_Particle_Function  == false)
			{
				cells_On_Off(table, false);
			}
		}

		Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		layer_Data.particles_Model.is_Used = check_Box->isChecked();

		if(layer_Data.particles_Model.is_Used)
			back_Widget->setStyleSheet("QWidget { background: rgb(100, 255, 220); }");
		else
			back_Widget->setStyleSheet("background-color: white");

		QVariant var;
		var.setValue(layer_Data);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		emit_Data_Edited();
	});

	// only by user
	connect(check_Box, &QCheckBox::clicked, this, [=]
	{		
		// recalculation at change
		global_Multilayer_Approach->recalculate_From_Table(true);
	});

	table->setCellWidget(current_Row, current_Column, back_Widget);
}

void Table_Of_Structures::create_Shape_Button(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item)
{
	Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	QPushButton* shape_Button = new QPushButton(layer_Data.particles_Model.particle_Shape);
		shape_Button->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_THICKNESS);
	table->setCellWidget(current_Row, current_Column, shape_Button);

	connect(shape_Button, &QPushButton::clicked, this, [=]{open_Shape_Pattern_Model_Dialog(structure_Item, "shape");});
}

void Table_Of_Structures::create_Pattern_Button(My_Table_Widget* table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item)
{
	Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	QPushButton* pattern_Button = new QPushButton(layer_Data.particles_Model.particle_Interference_Function);
		pattern_Button->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_THICKNESS);
	table->setCellWidget(current_Row, current_Column, pattern_Button);

	connect(pattern_Button, &QPushButton::clicked, this, [=]{open_Shape_Pattern_Model_Dialog(structure_Item, "pattern");});
}

void Table_Of_Structures::create_Model_Button(My_Table_Widget *table, int current_Row, int current_Column, QTreeWidgetItem* structure_Item)
{
	Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	QPushButton* model_Button = new QPushButton(layer_Data.particles_Model.geometric_Model);
		model_Button->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_THICKNESS);
	table->setCellWidget(current_Row, current_Column, model_Button);

	connect(model_Button, &QPushButton::clicked, this, [=]{open_Shape_Pattern_Model_Dialog(structure_Item, "model");});
}

void Table_Of_Structures::open_Shape_Pattern_Model_Dialog(QTreeWidgetItem* structure_Item, QString shape_Pattern_Model)
{
	Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	QDialog* choice_Window = new QDialog(this);
		choice_Window->setWindowTitle(Global_Variables::structure_Item_Name(layer_Data));
		choice_Window->setWindowModality(Qt::ApplicationModal);
		choice_Window->setAttribute(Qt::WA_DeleteOnClose);
		choice_Window->setWindowFlags(Qt::Tool);
		choice_Window->show();
		choice_Window->activateWindow();

	QVBoxLayout* choice_Layout = new QVBoxLayout(choice_Window);
		choice_Layout->setSizeConstraint(QLayout::SetFixedSize);
		choice_Layout->setSpacing(5);
		choice_Layout->setContentsMargins(5,5,5,5);

	// settings group box
	QGroupBox*  choice_Group_Box = new QGroupBox();
//	QGroupBox*  choice_Group_Box = new QGroupBox;
//		choice_Group_Box->setObjectName("choice_Group_Box");
//		choice_Group_Box->setStyleSheet("QGroupBox#choice_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 0ex;}"
//												"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
	choice_Layout->addWidget(choice_Group_Box);

	QVBoxLayout* choice_Group_Box_Layout = new QVBoxLayout(choice_Group_Box);
		choice_Group_Box_Layout->setSpacing(5);
		choice_Group_Box_Layout->setContentsMargins(5,9,5,5);


	if(shape_Pattern_Model == "shape")
	{
		choice_Group_Box->setTitle("Particle shape");

		// buttons
		QRadioButton* full_Sphere_Radiobutton = new QRadioButton("Sphere");
			full_Sphere_Radiobutton->setChecked(layer_Data.particles_Model.particle_Shape == full_Sphere);
		choice_Group_Box_Layout->addWidget(full_Sphere_Radiobutton);

		QRadioButton* full_Spheroid_Radiobutton = new QRadioButton("Spheroid");
			full_Spheroid_Radiobutton->setChecked(layer_Data.particles_Model.particle_Shape == full_Spheroid);
		choice_Group_Box_Layout->addWidget(full_Spheroid_Radiobutton);

		QRadioButton* full_Cylinder_Radiobutton = new QRadioButton("Cylinder");
			full_Cylinder_Radiobutton->setChecked(layer_Data.particles_Model.particle_Shape == cylinder);
		choice_Group_Box_Layout->addWidget(full_Cylinder_Radiobutton);

		QButtonGroup* shape_Group = new QButtonGroup;
			shape_Group->addButton(full_Sphere_Radiobutton);
			shape_Group->addButton(full_Spheroid_Radiobutton);
			shape_Group->addButton(full_Cylinder_Radiobutton);

		// connections
		connect(full_Sphere_Radiobutton, &QRadioButton::toggled, this, [=]
		{
			if(full_Sphere_Radiobutton->isChecked())
			{
				Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				layer_Data.particles_Model.particle_Shape = full_Sphere;

				QVariant var;
				var.setValue(layer_Data);
				structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			}
		});
		connect(full_Spheroid_Radiobutton, &QRadioButton::toggled, this, [=]
		{
			if(full_Spheroid_Radiobutton->isChecked())
			{
				Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				layer_Data.particles_Model.particle_Shape = full_Spheroid;

				QVariant var;
				var.setValue(layer_Data);
				structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			}
		});
		connect(full_Cylinder_Radiobutton, &QRadioButton::toggled, this, [=]
		{
			if(full_Cylinder_Radiobutton->isChecked())
			{
				Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				layer_Data.particles_Model.particle_Shape = cylinder;

				QVariant var;
				var.setValue(layer_Data);
				structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			}
		});
	}
	if(shape_Pattern_Model == "pattern")
	{
		// settings group box
		choice_Group_Box->setTitle("Lateral order");

		// buttons
		QRadioButton* disorder_Radiobutton = new QRadioButton("Disorder");
			disorder_Radiobutton->setChecked(layer_Data.particles_Model.particle_Interference_Function == disorder);
		choice_Group_Box_Layout->addWidget(disorder_Radiobutton);

		QRadioButton* radial_Paracrystal_Radiobutton = new QRadioButton("Radial paracrystal");
			radial_Paracrystal_Radiobutton->setChecked(layer_Data.particles_Model.particle_Interference_Function == radial_Paracrystal);
		choice_Group_Box_Layout->addWidget(radial_Paracrystal_Radiobutton);

		QButtonGroup* interference_Function_Group = new QButtonGroup;
			interference_Function_Group->addButton(disorder_Radiobutton);
			interference_Function_Group->addButton(radial_Paracrystal_Radiobutton);


		// connections
		connect(disorder_Radiobutton, &QRadioButton::toggled, this, [=]
		{
			if(disorder_Radiobutton->isChecked())
			{
				Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				layer_Data.particles_Model.particle_Interference_Function = disorder;

				QVariant var;
				var.setValue(layer_Data);
				structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			}
		});
		connect(radial_Paracrystal_Radiobutton, &QRadioButton::toggled, this, [=]
		{
			if(radial_Paracrystal_Radiobutton->isChecked())
			{
				Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				layer_Data.particles_Model.particle_Interference_Function = radial_Paracrystal;

				QVariant var;
				var.setValue(layer_Data);
				structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			}
		});
	}
	if(shape_Pattern_Model == "model")
	{
		// settings group box
		choice_Group_Box->setTitle("Model");

		// buttons
		QRadioButton* hexagonal_Radiobutton = new QRadioButton("Hexagonal");
			hexagonal_Radiobutton->setChecked(layer_Data.particles_Model.geometric_Model == hexagonal_Model);
		choice_Group_Box_Layout->addWidget(hexagonal_Radiobutton);

		QRadioButton* square_Radiobutton = new QRadioButton("Square");
			square_Radiobutton->setChecked(layer_Data.particles_Model.geometric_Model == square_Model);
		choice_Group_Box_Layout->addWidget(square_Radiobutton);

		QRadioButton* pure_Radial_Radiobutton = new QRadioButton("Radial");
			pure_Radial_Radiobutton->setChecked(layer_Data.particles_Model.geometric_Model == pure_Radial_Model);
//		choice_Group_Box_Layout->addWidget(pure_Radial_Radiobutton);

		QButtonGroup* model_Group = new QButtonGroup;
			model_Group->addButton(hexagonal_Radiobutton);
			model_Group->addButton(square_Radiobutton);
			model_Group->addButton(pure_Radial_Radiobutton);


		// connections
		connect(hexagonal_Radiobutton, &QRadioButton::toggled, this, [=]
		{
			if(hexagonal_Radiobutton->isChecked())
			{
				Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				layer_Data.particles_Model.geometric_Model = hexagonal_Model;

				QVariant var;
				var.setValue(layer_Data);
				structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			}
		});
		connect(square_Radiobutton, &QRadioButton::toggled, this, [=]
		{
			if(square_Radiobutton->isChecked())
			{
				Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				layer_Data.particles_Model.geometric_Model = square_Model;

				QVariant var;
				var.setValue(layer_Data);
				structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			}
		});
		connect(pure_Radial_Radiobutton, &QRadioButton::toggled, this, [=]
		{
			if(pure_Radial_Radiobutton->isChecked())
			{
				Data layer_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				layer_Data.particles_Model.geometric_Model = pure_Radial_Model;

				QVariant var;
				var.setValue(layer_Data);
				structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			}
		});
	}

	connect(choice_Window, &QDialog::finished, this, [=]
	{
		global_Multilayer_Approach->reopen_Table_Of_Structures(true);
	});
}

void Table_Of_Structures::create_PSD_Load_Button(My_Table_Widget* table, int current_Row, int current_Column, Multilayer* multilayer, QString PSD_Type, MyDoubleSpinBox* PSD_Sigma_Lineedit)
{
	QPushButton* PSD_Button = new QPushButton(PSD_Type);
	table->setCellWidget(current_Row, current_Column, PSD_Button);
	PSD_Button->setProperty(is_Opened_Property,false);
	if( (PSD_Type == PSD_Type_1D && multilayer->imperfections_Model.PSD_1D.argument.size()>2) ||
		(PSD_Type == PSD_Type_2D && multilayer->imperfections_Model.PSD_2D.argument.size()>2) )
		PSD_Button->setStyleSheet("QWidget { background: rgb(100, 255, 220); }");
	else
		PSD_Button->setStyleSheet("background-color: white");

	connect(PSD_Button, &QPushButton::clicked, this, [=]
	{
		if(!(PSD_Button->property(is_Opened_Property).toBool()))
		{
			PSD_Button->setProperty(is_Opened_Property,true);
			Measured_PSD_Editor* measured_PSD_Editor = new Measured_PSD_Editor(multilayer, PSD_Type, PSD_Button, PSD_Sigma_Lineedit, this);
				measured_PSD_Editor->show();
		}
	});
}

MyDoubleSpinBox* Table_Of_Structures::create_PSD_Sigma_Lineedit(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, Multilayer* multilayer, QString PSD_Type)
{
	Data substrate_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	MyDoubleSpinBox* spin_Box = new MyDoubleSpinBox;
		spin_Box->setReadOnly(true);
		spin_Box->setButtonSymbols(QAbstractSpinBox::NoButtons);
		spin_Box->setRange(-MAX_DOUBLE, MAX_DOUBLE);
		spin_Box->setDecimals(line_edit_sigma_precision);
		spin_Box->installEventFilter(this);
		spin_Box->setStyleSheet("background-color: white");

#ifdef _WIN32
	QFont font(spin_Box->font());
	font.setPointSize(8.25);
	font.setFamily("MS Shell Dlg 2");
	spin_Box->setFont(font);
#endif
#ifdef __linux__
#endif

	PSD_Data psd_Data	  (PSD_Type == PSD_Type_1D ? multilayer->imperfections_Model.PSD_1D : multilayer->imperfections_Model.PSD_2D);
	Parameter sigma_Factor(PSD_Type == PSD_Type_1D ? substrate_Data.roughness_Model.sigma_Factor_PSD_1D : substrate_Data.roughness_Model.sigma_Factor_PSD_2D);

	double coeff(length_Coefficients_Map.value(length_units));

	spin_Box->setValue(psd_Data.calc_Sigma_Full()*sigma_Factor.value/coeff);
	spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);

	spin_Box->setProperty(min_Size_Property, spin_Box->width());
	spin_Box->setProperty(column_Property, current_Column);
	spin_Box->setProperty(whats_This_Property, whats_This_Sigma_Eff_PSD);
	spin_Box->setProperty(value_Type_Property, VAL);

	// for reloading
	spin_Box->setProperty(reload_Property, false);
	spin_Box->setProperty(tab_Index_Property, tab_Index);

	// storage
	all_Widgets_To_Reload[tab_Index].append(spin_Box);
	connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged),   this, [=]
	{
		double coeff(length_Coefficients_Map.value(length_units));
		Data substrate_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		PSD_Data psd_Data	  (PSD_Type == PSD_Type_1D ? multilayer->imperfections_Model.PSD_1D : multilayer->imperfections_Model.PSD_2D);
		Parameter sigma_Factor(PSD_Type == PSD_Type_1D ? substrate_Data.roughness_Model.sigma_Factor_PSD_1D : substrate_Data.roughness_Model.sigma_Factor_PSD_2D);

		spin_Box->setValue(psd_Data.calc_Sigma_Full()*sigma_Factor.value/coeff);
		resize_Line_Edit (table,spin_Box);
	});

	// create item (set LineEdits_Map)
	table->setCellWidget(current_Row, current_Column, spin_Box);

	return spin_Box;
}

void Table_Of_Structures::create_Nu0_Label(My_Table_Widget *table, int tab_Index, int current_Row, int current_Column)
{
	// PARAMETER

	add_Columns(table,current_Column);

	QLabel* label = new QLabel(Nu_Sym+Zero_Subscript_Sym+" ["+spatial_frequency_units+"]");
		label->setAlignment(Qt::AlignCenter);
		label->setStyleSheet("background-color: lightblue");
	all_Widgets_To_Reload[tab_Index].append(label);

	// add widget to table
	table->setCellWidget(current_Row, current_Column, label);

	// for reloading
	label->setProperty(reload_Property, false);
	label->setProperty(tab_Index_Property, tab_Index);

	connect(label, &QLabel::windowTitleChanged, this, [=]
	{
		label->setText(Nu_Sym+Zero_Subscript_Sym+" ["+spatial_frequency_units+"]");
	});
}

void Table_Of_Structures::create_Nu0_Spin_Box(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column)
{
	add_Columns(table,current_Column);
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	double arg_Coeff = spatial_Frequency_Coefficients_Map.value(spatial_frequency_units);

	MyDoubleSpinBox* vertical_Inheritance_Roughness_Frequency_Spinbox = new MyDoubleSpinBox;//(nullptr,false);
		vertical_Inheritance_Roughness_Frequency_Spinbox->setAccelerated(true);
		vertical_Inheritance_Roughness_Frequency_Spinbox->setRange(1E-7/arg_Coeff, MAX_DOUBLE);
		vertical_Inheritance_Roughness_Frequency_Spinbox->setValue(multilayer->imperfections_Model.vertical_Inheritance_Frequency/arg_Coeff); // from A^-1 to spatial_frequency_units
		vertical_Inheritance_Roughness_Frequency_Spinbox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
		vertical_Inheritance_Roughness_Frequency_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		vertical_Inheritance_Roughness_Frequency_Spinbox->setProperty(reload_Property, false);
		vertical_Inheritance_Roughness_Frequency_Spinbox->setProperty(tab_Index_Property, tab_Index);
		vertical_Inheritance_Roughness_Frequency_Spinbox->setProperty(min_Size_Property,TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
	all_Widgets_To_Reload[tab_Index].append(vertical_Inheritance_Roughness_Frequency_Spinbox);


	{
		double ff = vertical_Inheritance_Roughness_Frequency_Spinbox->value();
		if(            ff<1E-5)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(7);
		if(ff>=1E-5 && ff<1E-4)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(6);
		if(ff>=1E-4 && ff<1E-3)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(5);
		if(ff>=1E-3 && ff<1E0)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(4);
		if(ff>=1    && ff<10)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(3);
		if(ff>=10)				vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(2);
		fit_Column(table, 0, current_Column);
	}

	// add widget to table
	table->setCellWidget(current_Row, current_Column, vertical_Inheritance_Roughness_Frequency_Spinbox);

	connect(vertical_Inheritance_Roughness_Frequency_Spinbox, static_cast<void (MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double arg_Coeff = spatial_Frequency_Coefficients_Map.value(spatial_frequency_units);
		if(vertical_Inheritance_Roughness_Frequency_Spinbox->property(reload_Property).toBool())
		{
			vertical_Inheritance_Roughness_Frequency_Spinbox->blockSignals(true);
			vertical_Inheritance_Roughness_Frequency_Spinbox->setRange(1E-8/arg_Coeff, MAX_DOUBLE);
			vertical_Inheritance_Roughness_Frequency_Spinbox->setValue(multilayer->imperfections_Model.vertical_Inheritance_Frequency/arg_Coeff); // from A^-1 to spatial_frequency_units
			vertical_Inheritance_Roughness_Frequency_Spinbox->blockSignals(false);
		} else
		{
			multilayer->imperfections_Model.vertical_Inheritance_Frequency = vertical_Inheritance_Roughness_Frequency_Spinbox->value()*arg_Coeff; // from spatial_frequency_units to A^-1
			global_Multilayer_Approach->recalculate_From_Table(true);
		}
		{
			double ff = vertical_Inheritance_Roughness_Frequency_Spinbox->value();
			if(            ff<1E-6)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(8);
			if(ff>=1E-6 && ff<1E-5)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(7);
			if(ff>=1E-5 && ff<1E-4)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(6);
			if(ff>=1E-4 && ff<1E-3)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(5);
			if(ff>=1E-3 && ff<1E0)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(4);
			if(ff>=1    && ff<10)	vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(3);
			if(ff>=10)				vertical_Inheritance_Roughness_Frequency_Spinbox->setDecimals(2);
			fit_Column(table, 0, current_Column);
		}
	});
//	vertical_Inheritance_Roughness_Frequency_Spinbox->valueChanged(multilayer->imperfections_Model.vertical_Inheritance_Frequency/arg_Coeff);
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
void Table_Of_Structures::create_Line_Edit(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString whats_This, QString val_Type, MyDoubleSpinBox* PSD_Sigma_Lineedit)
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
		spin_Box->setStyleSheet("background-color: white");

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

		spin_Box->setRange(0, MAX_DOUBLE);
		// but
		if(	whats_This == whats_This_Thickness_Drift_Line_Value ||
			whats_This == whats_This_Sigma_Drift_Line_Value	)
		{
			spin_Box->setRange(-100, MAX_DOUBLE);
		}
		if(whats_This == whats_This_Gamma)
		{
			spin_Box->setRange(0, 1);
		}

		// roughness
		if(whats_This == whats_This_Fractal_Alpha)
		{
			spin_Box->setRange(0.1, 1);
		}
		if(whats_This == whats_This_Fractal_Beta )
		{
			spin_Box->setRange(0, 99);
		}
		if(whats_This == whats_This_Correlation_Radius)
		{
			spin_Box->setMinimum(10/coeff);
			spin_Box->setMaximum(1e10/coeff);
		}
		if( whats_This == whats_This_PSD_Exponenta_Mu ||
			whats_This == whats_This_Particle_Correlation_Depth)
		{
			spin_Box->setMinimum(1e-1/coeff);
			spin_Box->setMaximum(1e10/coeff);
		}
		if(whats_This == whats_This_Roughness_Peak_Frequency_Width)
		{
			spin_Box->setMinimum(pow(10.,-line_edit_frequency_precision));
		}
		if( whats_This == whats_This_Correlation_Radius ||
			whats_This == whats_This_PSD_Exponenta_Mu ||
			whats_This == whats_This_Linear_PSD_Omega ||

			whats_This == whats_This_Linear_A1 ||
			whats_This == whats_This_Linear_A2 ||
			whats_This == whats_This_Linear_A3 ||
			whats_This == whats_This_Linear_A4 ||

			whats_This == whats_This_Roughness_Peak_Frequency ||
			whats_This == whats_This_Roughness_Peak_Frequency_Width ||
			whats_This == whats_This_Domain_Size ||
			whats_This == whats_This_Particle_Correlation_Depth )
		{
			spin_Box->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
		}

		// particles
		if( whats_This == whats_This_Particle_Average_Distance ||
			whats_This == whats_This_Particle_Radial_Distance)
		{
			spin_Box->setMinimum(1/coeff);
		}
		if( whats_This == whats_This_Particle_Radial_Distance_Deviation)
		{
			spin_Box->setMinimum(0.5/coeff);
		}
		if( whats_This == whats_This_Domain_Size)
		{
			spin_Box->setMinimum(1E2/coeff);
			spin_Box->setMaximum(1E8/coeff);
		}
		if( whats_This == whats_This_Particle_Z_Position)
		{
			spin_Box->setMinimum(-10000/coeff);
		}

		id = parameter.indicator.id;

		if(val_Type == VAL) spin_Boxes_ID_Map.insert(spin_Box,id);

		if( whats_This == whats_This_Thickness	||
			whats_This == whats_This_Period		||
			whats_This == whats_This_Gamma		)
		{
			if(val_Type == VAL) reload_Show_Dependence_Map.insertMulti(spin_Box, id_Of_Thicknesses);
		}

		if( whats_This == whats_This_Sigma_Diffuse )
		{
			for(Interlayer& interlayer : struct_Data.interlayer_Composition)
			{
				reload_Show_Dependence_Map.insertMulti(spin_Box,  interlayer.my_Sigma_Diffuse.indicator.id);
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

	if(whats_This == whats_This_Particle_Absolute_Density ||
	   whats_This == whats_This_Particle_Relative_Density)	spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_DENSITY);

	if(whats_This == whats_This_Particle_Radius)					spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
	if(whats_This == whats_This_Particle_Height)					spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
	if(whats_This == whats_This_Particle_Average_Distance ||
	   whats_This == whats_This_Particle_Radial_Distance)			spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
	if(whats_This == whats_This_Particle_Radial_Distance_Deviation)	spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
	if(whats_This == whats_This_Domain_Size)						spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_DENSITY);
	if(whats_This == whats_This_Particle_Z_Position)				spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
	if(whats_This == whats_This_Particle_Z_Position_Deviation)		spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);

	if( whats_This == whats_This_Absolute_Density  ||
		whats_This == whats_This_Relative_Density  ||
		whats_This == whats_This_Num_Repetitions   ||
		whats_This == whats_This_Correlation_Radius)	spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_DENSITY);

	if( whats_This == whats_This_Sigma_Diffuse ||
		whats_This == whats_This_Sigma_Roughness ||
		whats_This == whats_This_Roughness_Peak_Sigma)	spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SIGMA);

	if( whats_This == whats_This_Linear_PSD_Omega ||
		whats_This == whats_This_PSD_Exponenta_Mu ||
		whats_This == whats_This_Particle_Correlation_Depth ||
		whats_This == whats_This_Linear_A1 ||
		whats_This == whats_This_Linear_A2 ||
		whats_This == whats_This_Linear_A3 ||
		whats_This == whats_This_Linear_A4 ||
		whats_This == whats_This_Roughness_Peak_Frequency_Width ||
		whats_This == whats_This_Sigma_Factor_PSD_1D ||
		whats_This == whats_This_Sigma_Factor_PSD_2D )       spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
	if( whats_This == whats_This_Thickness || whats_This == whats_This_Period) spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_THICKNESS);

	if(	whats_This == whats_This_Gamma	)						spin_Box->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_LONG);

	if(	whats_This == whats_This_Thickness_Drift_Line_Value     ||
		whats_This == whats_This_Thickness_Drift_Rand_Rms       ||
		whats_This == whats_This_Sigma_Drift_Line_Value			||
		whats_This == whats_This_Sigma_Drift_Rand_Rms			||
		whats_This == whats_This_Thickness_Drift_Sine_Amplitude ||
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
	   (whats_This == whats_This_Sigma_Diffuse || whats_This == whats_This_Thickness) &&
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
		if( whats_This == whats_This_Sigma_Diffuse )
		{
			min_Max_Sigma_Diffuse_Spin_Boxes_List[tab_Index].append(spin_Box);
		}
	}

	// create item (set LineEdits_Map)
	table->setCellWidget(current_Row, current_Column, spin_Box);

	connect(spin_Box, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged),   this, [=]
	{
		refresh_Parameter(table);
		resize_Line_Edit (table,spin_Box);

		// special case for reloading non-parameter
		if( whats_This == whats_This_Sigma_Factor_PSD_1D ||
			whats_This == whats_This_Sigma_Factor_PSD_2D )
		{
			PSD_Sigma_Lineedit->valueChanged(spin_Box->value());
		}
	});
//	connect(spin_Box, &MyDoubleSpinBox::editingFinished, this, [=]{refresh_Parameter(table); });

	// steps
	if(whats_This == whats_This_Absolute_Density
	|| whats_This == whats_This_Relative_Density)				{ spin_Box->setSingleStep(step_density);					density_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Thickness
	|| whats_This == whats_This_Period)							{ spin_Box->setSingleStep(step_thickness);					thickness_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Sigma_Diffuse)					{ spin_Box->setSingleStep(step_sigma_diffuse);				sigma_Diffuse_Spin_Boxes_List.append(spin_Box); }

	if(whats_This == whats_This_Sigma_Roughness)				{ spin_Box->setSingleStep(step_sigma_roughness);			sigma_Roughness_Spin_Boxes_List.append(spin_Box); }
//	if(whats_This == whats_This_Correlation_Radius)				{ spin_Box->setSingleStep(step_cor_radius);					correlation_Radius_Roughness_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Fractal_Alpha)					{ spin_Box->setSingleStep(step_fractal_alpha_beta);			fractal_Alpha_Beta_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Fractal_Beta)					{ spin_Box->setSingleStep(step_fractal_alpha_beta);			fractal_Alpha_Beta_Spin_Boxes_List.append(spin_Box); }
//	if(whats_This == whats_This_Linear_PSD_Omega)				{ spin_Box->setSingleStep(step_omega);						linear_PSD_Omega_Spin_Boxes_List.append(spin_Box); }
//	if(whats_This == whats_This_PSD_Exponenta_Mu)				{ spin_Box->setSingleStep(step_mu);							linear_PSD_Exponenta_Mu_Spin_Boxes_List.append(spin_Box); }

//	if(whats_This == whats_This_Linear_A1)						{ spin_Box->setSingleStep(step_linear_a1);					linear_A1_Spin_Boxes_List.append(spin_Box); }
//	if(whats_This == whats_This_Linear_A2)						{ spin_Box->setSingleStep(step_linear_a2);					linear_A2_Spin_Boxes_List.append(spin_Box); }
//	if(whats_This == whats_This_Linear_A3)						{ spin_Box->setSingleStep(step_linear_a3);					linear_A3_Spin_Boxes_List.append(spin_Box); }
//	if(whats_This == whats_This_Linear_A4)						{ spin_Box->setSingleStep(step_linear_a4);					linear_A4_Spin_Boxes_List.append(spin_Box); }

	if(whats_This == whats_This_Roughness_Peak_Sigma)			{ spin_Box->setSingleStep(step_sigma_roughness_peak);		sigma_Roughness_Peak_Spin_Boxes_List.append(spin_Box); }
//	if(whats_This == whats_This_Roughness_Peak_Frequency)		{ spin_Box->setSingleStep(step_peak_frequency);				peak_Frequency_Spin_Boxes_List.append(spin_Box); }
//	if(whats_This == whats_This_Roughness_Peak_Frequency_Width)	{ spin_Box->setSingleStep(step_peak_frequency_width);		peak_Frequency_Width_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Sigma_Factor_PSD_1D ||
	   whats_This == whats_This_Sigma_Factor_PSD_2D )			{ spin_Box->setSingleStep(step_psd_sigma_factor);			PSD_Sigma_Factor_Spin_Boxes_List.append(spin_Box); }

	if(whats_This == whats_This_Particle_Absolute_Density ||
	   whats_This == whats_This_Particle_Relative_Density)			{ spin_Box->setSingleStep(step_particle_density);			  particle_Density_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Particle_Radius)					{ spin_Box->setSingleStep(step_particle_radius);			  particle_Radius_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Particle_Height)					{ spin_Box->setSingleStep(step_particle_height);			  particle_Height_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Particle_Average_Distance ||
	   whats_This == whats_This_Particle_Radial_Distance)			{ spin_Box->setSingleStep(step_particle_distance);			  particle_Distance_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Particle_Radial_Distance_Deviation)	{ spin_Box->setSingleStep(step_particle_distance_deviation);  particle_Distance_Deviation_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Domain_Size)						{ spin_Box->setSingleStep(step_domain_size);				  domain_Size_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Particle_Z_Position)				{ spin_Box->setSingleStep(step_particle_z_position);		  particle_Z_Position_Spin_Boxes_List.append(spin_Box); }
	if(whats_This == whats_This_Particle_Z_Position_Deviation)		{ spin_Box->setSingleStep(step_particle_z_position_deviation);particle_Z_Position_Deviation_Spin_Boxes_List.append(spin_Box); }

	if( whats_This == whats_This_Gamma)								{ spin_Box->setSingleStep(step_gamma);					gamma_Spin_Boxes_List.append(spin_Box); }

	if(	whats_This == whats_This_Thickness_Drift_Line_Value ||
		whats_This == whats_This_Sigma_Drift_Line_Value		)		{ spin_Box->setSingleStep(step_drift_linear);		drift_Linear_Spin_Boxes_List.append(spin_Box); }

	if(	whats_This == whats_This_Thickness_Drift_Rand_Rms ||
		whats_This == whats_This_Sigma_Drift_Rand_Rms		)		{ spin_Box->setSingleStep(step_drift_random);		drift_Random_Spin_Boxes_List.append(spin_Box); }

	if(	whats_This == whats_This_Thickness_Drift_Sine_Amplitude ||
		whats_This == whats_This_Sigma_Drift_Sine_Amplitude		)	{ spin_Box->setSingleStep(step_drift_amplitude);	drift_Amplitude_Spin_Boxes_List.append(spin_Box); }

	if(	whats_This == whats_This_Thickness_Drift_Sine_Frequency ||
		whats_This == whats_This_Sigma_Drift_Sine_Frequency		)	{ spin_Box->setSingleStep(step_drift_frequency);	drift_Frequency_Spin_Boxes_List.append(spin_Box); }

	if(	whats_This == whats_This_Thickness_Drift_Sine_Phase		||
		whats_This == whats_This_Sigma_Drift_Sine_Phase	)			{ spin_Box->setSingleStep(step_drift_phase);		drift_Phase_Spin_Boxes_List.append(spin_Box); }
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
	} else
	{
		back_Widgets_Fit_Map.insert(back_Widget, parameter.indicator.id);
	}

	// create item
	table->setCellWidget(current_Row, current_Column, back_Widget);

	connect(check_Box, &QCheckBox::toggled, this, &Table_Of_Structures::refresh_Fit_Parameter);
	connect(check_Box, &QCheckBox::toggled, this, [=]
	{
		if( struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic ||
			whats_This != whats_This_Thickness)
		{
			cells_On_Off(table, true, !check_Box->isChecked(), true); // mark disabled widgets as non-enablable, but we can enable them from here
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

					(param.indicator.whats_This == whats_This_Sigma_Drift_Line_Value		&& data.sigma_Diffuse_Drift.is_Drift_Line)	   ||
					(param.indicator.whats_This == whats_This_Sigma_Drift_Rand_Rms			&& data.sigma_Diffuse_Drift.is_Drift_Rand)
				  )
				{
					back_Widget->setStyleSheet(fit_Color);
				} else
				{
					if(	(param.indicator.whats_This == whats_This_Thickness_Drift_Sine_Amplitude&& data.thickness_Drift.is_Drift_Sine) ||
						(param.indicator.whats_This == whats_This_Thickness_Drift_Sine_Frequency&& data.thickness_Drift.is_Drift_Sine) ||
						(param.indicator.whats_This == whats_This_Thickness_Drift_Sine_Phase	&& data.thickness_Drift.is_Drift_Sine) ||

						(param.indicator.whats_This == whats_This_Sigma_Drift_Sine_Amplitude	&& data.sigma_Diffuse_Drift.is_Drift_Sine) ||
						(param.indicator.whats_This == whats_This_Sigma_Drift_Sine_Frequency	&& data.sigma_Diffuse_Drift.is_Drift_Sine) ||
						(param.indicator.whats_This == whats_This_Sigma_Drift_Sine_Phase		&& data.sigma_Diffuse_Drift.is_Drift_Sine)
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

	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		if(multilayer->imperfections_Model.use_Func[interlayer_Index] || struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
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
			reload_Show_Dependence_Map.insertMulti (check_Box, struct_Data.sigma_Diffuse.indicator.id);
			all_Widgets_To_Reload[tab_Index].append(check_Box);
			reload_Show_Dependence_Map.insertMulti(check_Box,  inter_Comp.my_Sigma_Diffuse.indicator.id);

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
}

void Table_Of_Structures::create_Weigts_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString val_Type)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;
	double value = -2017;

	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		if(multilayer->imperfections_Model.use_Func[interlayer_Index] || struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
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
				reload_Show_Dependence_Map.insertMulti(spin_Box, struct_Data.sigma_Diffuse.indicator.id);
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
}

void Table_Of_Structures::create_Weights_Check_Box_Fit_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;

	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		if(multilayer->imperfections_Model.use_Func[interlayer_Index] || struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
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

			// alignment (back_Widget for alignment and color)
			QWidget* back_Widget = new QWidget;
			back_Widgets_Fit_Map.insert(back_Widget, inter_Comp.interlayer.indicator.id);
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
}

void Table_Of_Structures::create_MySigma_Labels_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		if(multilayer->imperfections_Model.use_Func[interlayer_Index])
		{
			Parameter& my_Sigma = interlayer_Composition[interlayer_Index].my_Sigma_Diffuse;

			// update tab_Index and full_Name
			my_Sigma.indicator.tab_Index = tab_Index;
			my_Sigma.indicator.full_Name = Global_Variables::parameter_Name(struct_Data, whats_This_Interlayer_My_Sigma_Diffuse, interlayer_Index);
			QVariant var;
			var.setValue(struct_Data);
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			QLabel* label = new QLabel("s "+transition_Layer_Functions[interlayer_Index]);
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
}

void Table_Of_Structures::create_MySigma_Line_Edits_Interlayer(My_Table_Widget* table, int tab_Index, int current_Row, int start_Column, QTreeWidgetItem* structure_Item)
{
	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QVector<Interlayer>& interlayer_Composition = struct_Data.interlayer_Composition;

	int current_Column = start_Column;

	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		if(multilayer->imperfections_Model.use_Func[interlayer_Index])
		{
			Parameter& sigma_Comp = interlayer_Composition[interlayer_Index].my_Sigma_Diffuse;

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

			spin_Box->setSingleStep(step_sigma_diffuse);
			sigma_Diffuse_Spin_Boxes_List.append(spin_Box);

			current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
		}
	}
}

void Table_Of_Structures::create_Simple_Label(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This, QString text)
{
	// PARAMETER

	add_Columns(table,current_Column);

	QLabel* label = new QLabel(text);
		label->setAlignment(Qt::AlignCenter);
		label->setStyleSheet("background-color: lightgray");
	all_Widgets_To_Reload[tab_Index].append(label);

	// add widget to table
	table->setCellWidget(current_Row, current_Column, label);

	// for reloading
	label->setProperty(reload_Property, false);
	label->setProperty(tab_Index_Property, tab_Index);
	label->setProperty(whats_This_Property, whats_This);

	connect(label, &QLabel::windowTitleChanged, this, [=]
	{
		if(whats_This == whats_This_Thickness)							label->setText("z/d ["+length_units+"]");
		if(whats_This == whats_This_Sigma_Diffuse)						label->setText("s ["+length_units+"]");
		if(whats_This == whats_This_Sigma_Roughness)					label->setText(Sigma_Sym+" ["+length_units+"]");
		if(whats_This == whats_This_Correlation_Radius)					label->setText(Xi_Sym+" ["+correlation_length_units+"]");
		if(whats_This == whats_This_Linear_PSD_Omega)					label->setText(Omega_Big_Sym+" ["+omega_units+"]");
		if(whats_This == whats_This_PSD_Exponenta_Mu)					label->setText("L"+Subscript_v_Sym+" ["+mu_units+"]");
		if(whats_This == whats_This_Linear_A2)							label->setText("a"+Subscript_2_Sym+" ["+linear_a2_units+"]");
		if(whats_This == whats_This_Linear_A3)							label->setText("a"+Subscript_3_Sym+" ["+linear_a3_units+"]");
		if(whats_This == whats_This_Linear_A4)							label->setText("a"+Subscript_4_Sym+" ["+linear_a4_units+"]");
		if(whats_This == whats_This_Roughness_Peak_Sigma)				label->setText(Sigma_Sym+Subscript_v_Sym+" ["+length_units+"]");
		if(whats_This == whats_This_Roughness_Peak_Frequency)			label->setText(                Nu_Sym+" ["+spatial_frequency_units+"]");
		if(whats_This == whats_This_Roughness_Peak_Frequency_Width)		label->setText(Delta_Small_Sym+Nu_Sym+" ["+spatial_frequency_units+"]");
		if(whats_This == whats_This_Sigma_Eff_PSD)						label->setText(Sigma_Sym+Subscript_e_Sym+" ["+length_units+"]");


		if(whats_This == whats_This_Particle_Radius)					label->setText("R ["+length_units+"]");
		if(whats_This == whats_This_Particle_Height)					label->setText("H ["+length_units+"]");
		if(whats_This == whats_This_Particle_Distance)					label->setText("r ["+length_units+"]");
		if(whats_This == whats_This_Particle_Radial_Distance_Deviation)	label->setText(Delta_Small_Sym + "r ["+length_units+"]");
		if(whats_This == whats_This_Domain_Size)						label->setText("D ["+correlation_length_units+"]");
		if(whats_This == whats_This_Particle_Correlation_Depth)			label->setText("L"+ Subscript_p_Sym+" ["+mu_units+"]");
		if(whats_This == whats_This_Particle_Z_Position)				label->setText("z" + Subscript_p_Sym + " ["+length_units+"]");
		if(whats_This == whats_This_Particle_Z_Position_Deviation)		label->setText(Delta_Small_Sym + "z" + Subscript_p_Sym + " ["+length_units+"]");
		if(whats_This == whats_This_Particle_Z_Position_Deviation)		label->setText(Delta_Small_Sym + "z" + Subscript_p_Sym + " ["+length_units+"]");
	});
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
				if(whats_This == whats_This_Sigma_Diffuse)
				{
					if(struct_Data.sigma_Diffuse.fit.is_Fitable)
					{
						struct_Data.sigma_Diffuse.fit.min = struct_Data.sigma_Diffuse.value*(100-multilayer->min_Max_Sigma)/100;
						struct_Data.sigma_Diffuse.fit.max = struct_Data.sigma_Diffuse.value*(100+multilayer->min_Max_Sigma)/100;
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
		if(whats_This == whats_This_Sigma_Diffuse)
		{
			spin_Box->setValue(multilayer->min_Max_Sigma);
		}
		spin_Box->setSingleStep(1);
		spin_Box->setButtonSymbols(QAbstractSpinBox::NoButtons);

	table->setCellWidget(current_Row, current_Column, spin_Box);
	connect(spin_Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [=]
	{
		if(whats_This == whats_This_Density)		{ multilayer->min_Max_Density	= spin_Box->value(); }
		if(whats_This == whats_This_Thickness)		{ multilayer->min_Max_Thickness = spin_Box->value(); }
		if(whats_This == whats_This_Sigma_Diffuse)	{ multilayer->min_Max_Sigma		= spin_Box->value(); }
	});
}

void Table_Of_Structures::create_Step_Spin_Box(My_Table_Widget* table, int tab_Index, int current_Row, int current_Column, QString whats_This)
{
	// PARAMETER

	add_Columns(table,current_Column);

	MyDoubleSpinBox* step_SpinBox = new MyDoubleSpinBox;
		step_SpinBox->setMinimumWidth(TABLE_FIX_WIDTH_LINE_EDIT_SHORT);
		step_SpinBox->setRange(0, MAX_DOUBLE);
		step_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		step_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
		step_SpinBox->setAccelerated(true);
		step_SpinBox->setProperty(column_Property,current_Column);
		step_SpinBox->installEventFilter(this);
		step_SpinBox->setDecimals(10);

	table->setCellWidget(current_Row, current_Column, step_SpinBox);
	Global_Variables::resize_Line_Edit(step_SpinBox);

		int min_Width = 0;
		if(whats_This == whats_This_Composition)			{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;	}
		if(whats_This == whats_This_Density)				{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_DENSITY;}
		if(whats_This == whats_This_Thickness)				{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_THICKNESS;}
		if(whats_This == whats_This_Sigma_Diffuse)			{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SIGMA;	}
		if(whats_This == whats_This_Interlayer_Composition)	{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;	}

		if(whats_This == whats_This_Sigma_Roughness)				{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SIGMA;	}
//		if(whats_This == whats_This_Correlation_Radius)				{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_DENSITY;}
		if(whats_This == whats_This_Fractal_Alpha)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;	}
//		if(whats_This == whats_This_Linear_PSD_Omega)				{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SIGMA;	}
//		if(whats_This == whats_This_PSD_Exponenta_Mu)				{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SIGMA;	}
		if(whats_This == whats_This_Fractal_Beta)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;	}
		if(whats_This == whats_This_Roughness_Peak_Sigma)			{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SIGMA;	}
//		if(whats_This == whats_This_Roughness_Peak_Frequency)		{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;	}
//		if(whats_This == whats_This_Roughness_Peak_Frequency_Width)	{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;	}
		if(whats_This == whats_This_Sigma_Factor_PSD_1D ||
		   whats_This == whats_This_Sigma_Factor_PSD_2D)			{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;	}

		if(whats_This == whats_This_Particle_Density)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_DENSITY;}
		if(whats_This == whats_This_Particle_Radius)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;}
		if(whats_This == whats_This_Particle_Height)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;}
		if(whats_This == whats_This_Particle_Distance)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;}
		if(whats_This == whats_This_Particle_Radial_Distance_Deviation)	{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;}
		if(whats_This == whats_This_Domain_Size)						{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_DENSITY;}
		if(whats_This == whats_This_Particle_Z_Position)				{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;}
		if(whats_This == whats_This_Particle_Z_Position_Deviation)		{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SHORT;}

		if(whats_This == whats_This_Gamma)					{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_LONG; step_SpinBox->setRange(0, 0.5);}

		if(	whats_This == whats_This_Thickness_Drift_Line_Value     ||
			whats_This == whats_This_Thickness_Drift_Rand_Rms       ||
			whats_This == whats_This_Sigma_Drift_Line_Value			||
			whats_This == whats_This_Sigma_Drift_Rand_Rms			||
			whats_This == whats_This_Thickness_Drift_Sine_Amplitude ||
			whats_This == whats_This_Thickness_Drift_Sine_Frequency ||
			whats_This == whats_This_Thickness_Drift_Sine_Phase		||
			whats_This == whats_This_Sigma_Drift_Sine_Amplitude		||
			whats_This == whats_This_Sigma_Drift_Sine_Frequency		||
			whats_This == whats_This_Sigma_Drift_Sine_Phase	)		{ min_Width = TABLE_FIX_WIDTH_LINE_EDIT_SIGMA;	}

		step_SpinBox->setFixedWidth(min_Width);
		step_SpinBox->setProperty(min_Size_Property, step_SpinBox->width());

	connect(step_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double length_Coeff = length_Coefficients_Map.value(length_units);
		double correlation_Length_Coeff = length_Coefficients_Map.value(correlation_length_units);
//		double omega_Coeff = omega_Coefficients_Map.value(omega_units);
//		double mu_Coeff = mu_Coefficients_Map.value(mu_units);
//		double linear_a2_Coeff = linear_a2_Coefficients_Map.value(linear_a2_units);
//		double linear_a3_Coeff = linear_a3_Coefficients_Map.value(linear_a3_units);
//		double linear_a4_Coeff = linear_a4_Coefficients_Map.value(linear_a4_units);

		if(whats_This == whats_This_Composition)			{ step_composition = step_SpinBox->value();					for(MyDoubleSpinBox* spb : composition_Spin_Boxes_List)		spb->setSingleStep(step_composition);				}
		if(whats_This == whats_This_Density)				{ step_density = step_SpinBox->value();						for(MyDoubleSpinBox* spb : density_Spin_Boxes_List)			spb->setSingleStep(step_density);					}
		if(whats_This == whats_This_Thickness)				{ step_thickness = step_SpinBox->value()*length_Coeff;		for(MyDoubleSpinBox* spb : thickness_Spin_Boxes_List)		spb->setSingleStep(step_thickness/length_Coeff);	}
		if(whats_This == whats_This_Sigma_Diffuse)			{ step_sigma_diffuse = step_SpinBox->value()*length_Coeff;  for(MyDoubleSpinBox* spb : sigma_Diffuse_Spin_Boxes_List)	spb->setSingleStep(step_sigma_diffuse/length_Coeff);}
		if(whats_This == whats_This_Interlayer_Composition)	{ step_interlayer = step_SpinBox->value();					for(MyDoubleSpinBox* spb : interlayer_Spin_Boxes_List)		spb->setSingleStep(step_interlayer);				}

		if(whats_This == whats_This_Sigma_Roughness)				{ step_sigma_roughness = step_SpinBox->value()*length_Coeff;				for(MyDoubleSpinBox* spb : sigma_Roughness_Spin_Boxes_List)				spb->setSingleStep(step_sigma_roughness/length_Coeff);}
//		if(whats_This == whats_This_Correlation_Radius)				{ step_cor_radius = step_SpinBox->value()*correlation_Length_Coeff;	for(MyDoubleSpinBox* spb : correlation_Radius_Roughness_Spin_Boxes_List)spb->setSingleStep(step_cor_radius/correlation_Length_Coeff);}
		if(whats_This == whats_This_Fractal_Alpha)					{ step_fractal_alpha_beta = step_SpinBox->value();					for(MyDoubleSpinBox* spb : fractal_Alpha_Beta_Spin_Boxes_List)			spb->setSingleStep(step_fractal_alpha_beta);}
		if(whats_This == whats_This_Fractal_Beta)					{ step_fractal_alpha_beta = step_SpinBox->value();					for(MyDoubleSpinBox* spb : fractal_Alpha_Beta_Spin_Boxes_List)			spb->setSingleStep(step_fractal_alpha_beta);}
//		if(whats_This == whats_This_Linear_PSD_Omega)				{ step_omega = step_SpinBox->value()*omega_Coeff;					for(MyDoubleSpinBox* spb : linear_PSD_Omega_Spin_Boxes_List)			spb->setSingleStep(step_omega/omega_Coeff);}
//		if(whats_This == whats_This_PSD_Exponenta_Mu)				{ step_mu = step_SpinBox->value()*mu_Coeff;							for(MyDoubleSpinBox* spb : linear_PSD_Exponenta_Mu_Spin_Boxes_List)		spb->setSingleStep(step_mu/mu_Coeff);}

//		if(whats_This == whats_This_Linear_A1)						{ step_linear_a1 = step_SpinBox->value();							for(MyDoubleSpinBox* spb : linear_A1_Spin_Boxes_List)		spb->setSingleStep(step_linear_a1);}
//		if(whats_This == whats_This_Linear_A2)						{ step_linear_a2 = step_SpinBox->value()*linear_a2_Coeff;			for(MyDoubleSpinBox* spb : linear_A2_Spin_Boxes_List)		spb->setSingleStep(step_linear_a2/linear_a2_Coeff);}
//		if(whats_This == whats_This_Linear_A3)						{ step_linear_a3 = step_SpinBox->value()*linear_a3_Coeff;			for(MyDoubleSpinBox* spb : linear_A3_Spin_Boxes_List)		spb->setSingleStep(step_linear_a3/linear_a3_Coeff);}
//		if(whats_This == whats_This_Linear_A4)						{ step_linear_a4 = step_SpinBox->value()*linear_a4_Coeff;			for(MyDoubleSpinBox* spb : linear_A4_Spin_Boxes_List)		spb->setSingleStep(step_linear_a4/linear_a4_Coeff);}

		if(whats_This == whats_This_Roughness_Peak_Sigma)			{ step_sigma_roughness_peak = step_SpinBox->value()*length_Coeff;			for(MyDoubleSpinBox* spb : sigma_Roughness_Peak_Spin_Boxes_List)		spb->setSingleStep(step_sigma_roughness_peak/length_Coeff);}
//		if(whats_This == whats_This_Roughness_Peak_Frequency)		{ step_peak_frequency = step_SpinBox->value();								for(MyDoubleSpinBox* spb : peak_Frequency_Spin_Boxes_List)				spb->setSingleStep(step_peak_frequency);}
//		if(whats_This == whats_This_Roughness_Peak_Frequency_Width)	{ step_peak_frequency_width = step_SpinBox->value();						for(MyDoubleSpinBox* spb : peak_Frequency_Width_Spin_Boxes_List)		spb->setSingleStep(step_peak_frequency_width);}
		if(whats_This == whats_This_Sigma_Factor_PSD_1D ||
		   whats_This == whats_This_Sigma_Factor_PSD_2D )			{ step_psd_sigma_factor = step_SpinBox->value();							for(MyDoubleSpinBox* spb : PSD_Sigma_Factor_Spin_Boxes_List)			spb->setSingleStep(step_psd_sigma_factor);}

		if(whats_This == whats_This_Particle_Density)					{ step_particle_density  = step_SpinBox->value();						  for(MyDoubleSpinBox* spb : particle_Density_Spin_Boxes_List)				spb->setSingleStep(step_particle_density);	}
		if(whats_This == whats_This_Particle_Radius)					{ step_particle_radius   = step_SpinBox->value()*length_Coeff;			  for(MyDoubleSpinBox* spb : particle_Radius_Spin_Boxes_List)				spb->setSingleStep(step_particle_radius/length_Coeff);	}
		if(whats_This == whats_This_Particle_Height)					{ step_particle_height   = step_SpinBox->value()*length_Coeff;			  for(MyDoubleSpinBox* spb : particle_Height_Spin_Boxes_List)				spb->setSingleStep(step_particle_height/length_Coeff);	}
		if(whats_This == whats_This_Particle_Distance)					{ step_particle_distance = step_SpinBox->value()*length_Coeff;			  for(MyDoubleSpinBox* spb : particle_Distance_Spin_Boxes_List)				spb->setSingleStep(step_particle_distance/length_Coeff);	}
		if(whats_This == whats_This_Particle_Radial_Distance_Deviation)	{ step_particle_distance_deviation = step_SpinBox->value()*length_Coeff;  for(MyDoubleSpinBox* spb : particle_Distance_Deviation_Spin_Boxes_List)	spb->setSingleStep(step_particle_distance_deviation/length_Coeff);	}
		if(whats_This == whats_This_Domain_Size)						{ step_domain_size		 = step_SpinBox->value()*correlation_Length_Coeff;for(MyDoubleSpinBox* spb : domain_Size_Spin_Boxes_List)					spb->setSingleStep(step_domain_size/correlation_Length_Coeff);	}
		if(whats_This == whats_This_Particle_Z_Position)				{ step_particle_z_position = step_SpinBox->value()*length_Coeff;		  for(MyDoubleSpinBox* spb : particle_Z_Position_Spin_Boxes_List)			spb->setSingleStep(step_particle_z_position/length_Coeff);	}
		if(whats_This == whats_This_Particle_Z_Position_Deviation)		{ step_particle_z_position_deviation = step_SpinBox->value()*length_Coeff;for(MyDoubleSpinBox* spb : particle_Z_Position_Deviation_Spin_Boxes_List)	spb->setSingleStep(step_particle_z_position_deviation/length_Coeff);	}

		if(whats_This == whats_This_Gamma)								{ step_gamma = step_SpinBox->value();				for(MyDoubleSpinBox* spb : gamma_Spin_Boxes_List)			spb->setSingleStep(step_gamma);				}

		if(	whats_This == whats_This_Thickness_Drift_Line_Value ||
			whats_This == whats_This_Sigma_Drift_Line_Value		)		{ step_drift_linear = step_SpinBox->value(); 		for(MyDoubleSpinBox* spb : drift_Linear_Spin_Boxes_List)	spb->setSingleStep(step_drift_linear);	}

		if(	whats_This == whats_This_Thickness_Drift_Rand_Rms ||
			whats_This == whats_This_Sigma_Drift_Rand_Rms		)		{ step_drift_random = step_SpinBox->value();		for(MyDoubleSpinBox* spb : drift_Random_Spin_Boxes_List)	spb->setSingleStep(step_drift_random);	}

		if(	whats_This == whats_This_Thickness_Drift_Sine_Amplitude ||
			whats_This == whats_This_Sigma_Drift_Sine_Amplitude		)	{ step_drift_amplitude = step_SpinBox->value(); 	for(MyDoubleSpinBox* spb : drift_Amplitude_Spin_Boxes_List)	spb->setSingleStep(step_drift_amplitude);	}

		if(	whats_This == whats_This_Thickness_Drift_Sine_Frequency ||
			whats_This == whats_This_Sigma_Drift_Sine_Frequency		)	{ step_drift_frequency = step_SpinBox->value();		for(MyDoubleSpinBox* spb : drift_Frequency_Spin_Boxes_List)	spb->setSingleStep(step_drift_frequency);	}

		if(	whats_This == whats_This_Thickness_Drift_Sine_Phase		||
			whats_This == whats_This_Sigma_Drift_Sine_Phase	)			{ step_drift_phase = step_SpinBox->value();			for(MyDoubleSpinBox* spb : drift_Phase_Spin_Boxes_List)		spb->setSingleStep(step_drift_phase);		}

		resize_Line_Edit(table, step_SpinBox);
	});

	// refresh in each table
	connect(main_Tabs, &QTabWidget::tabBarClicked, this,  [=]
	{
		double length_Coeff = length_Coefficients_Map.value(length_units);
		double correlation_Length_Coeff = length_Coefficients_Map.value(correlation_length_units);
//		double omega_Coeff = omega_Coefficients_Map.value(omega_units);
//		double mu_Coeff = mu_Coefficients_Map.value(mu_units);
//		double linear_a2_Coeff = linear_a2_Coefficients_Map.value(linear_a2_units);
//		double linear_a3_Coeff = linear_a3_Coefficients_Map.value(linear_a3_units);
//		double linear_a4_Coeff = linear_a4_Coefficients_Map.value(linear_a4_units);

//		int add_Decimals = /*min(*/log10(length_Coeff)/*,2.)*/;
//		double min_Step = max(0.1/length_Coeff,0.0001);

		if(whats_This == whats_This_Composition)			{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_composition);				step_SpinBox->setDecimals(line_edit_composition_precision);		}
		if(whats_This == whats_This_Density)				{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_density);					step_SpinBox->setDecimals(line_edit_density_precision);			}
		if(whats_This == whats_This_Thickness)				{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_thickness/length_Coeff);	step_SpinBox->setDecimals(line_edit_thickness_precision);		}
		if(whats_This == whats_This_Sigma_Diffuse)			{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_sigma_diffuse/length_Coeff);step_SpinBox->setDecimals(line_edit_sigma_precision);			}
		if(whats_This == whats_This_Interlayer_Composition)	{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_interlayer);				step_SpinBox->setDecimals(line_edit_interlayer_precision);		}

		if(whats_This == whats_This_Sigma_Roughness)				{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_sigma_roughness/length_Coeff);			step_SpinBox->setDecimals(line_edit_sigma_precision);			}
//		if(whats_This == whats_This_Correlation_Radius)				{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_cor_radius/correlation_Length_Coeff);	step_SpinBox->setDecimals(line_edit_cor_radius_precision);		}
		if(whats_This == whats_This_Fractal_Alpha)					{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_fractal_alpha_beta);					step_SpinBox->setDecimals(line_edit_fractal_alpha_beta_precision);	}
		if(whats_This == whats_This_Fractal_Beta)					{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_fractal_alpha_beta);					step_SpinBox->setDecimals(line_edit_fractal_alpha_beta_precision);	}
//		if(whats_This == whats_This_Linear_PSD_Omega)				{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_omega/omega_Coeff);						step_SpinBox->setDecimals(line_edit_omega_precision);			}
//		if(whats_This == whats_This_PSD_Exponenta_Mu)				{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_mu/mu_Coeff);							step_SpinBox->setDecimals(line_edit_mu_precision);				}
//		if(whats_This == whats_This_Linear_A1)						{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_linear_a1);								step_SpinBox->setDecimals(line_edit_linear_a1_precision);		}
//		if(whats_This == whats_This_Linear_A2)						{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_linear_a2/linear_a2_Coeff);				step_SpinBox->setDecimals(line_edit_linear_a2_precision);		}
//		if(whats_This == whats_This_Linear_A3)						{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_linear_a3/linear_a3_Coeff);				step_SpinBox->setDecimals(line_edit_linear_a3_precision);		}
//		if(whats_This == whats_This_Linear_A4)						{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_linear_a4/linear_a4_Coeff);				step_SpinBox->setDecimals(line_edit_linear_a4_precision);		}

		if(whats_This == whats_This_Roughness_Peak_Sigma)			{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_sigma_roughness_peak/length_Coeff);			step_SpinBox->setDecimals(line_edit_sigma_precision);			}
//		if(whats_This == whats_This_Roughness_Peak_Frequency)		{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_peak_frequency);							step_SpinBox->setDecimals(line_edit_frequency_precision);		}
//		if(whats_This == whats_This_Roughness_Peak_Frequency_Width)	{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_peak_frequency_width);						step_SpinBox->setDecimals(line_edit_frequency_precision);		}
		if(whats_This == whats_This_Sigma_Factor_PSD_1D ||
		   whats_This == whats_This_Sigma_Factor_PSD_2D )			{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_psd_sigma_factor);							step_SpinBox->setDecimals(line_edit_psd_factor_precision);		}

		if(whats_This == whats_This_Particle_Density)					{ step_SpinBox->setDecimals(10); step_SpinBox->setValue(step_particle_density);								step_SpinBox->setDecimals(line_edit_density_precision);	}
		if(whats_This == whats_This_Particle_Radius)					{ step_SpinBox->setDecimals(10); step_SpinBox->setValue(step_particle_radius/length_Coeff);					step_SpinBox->setDecimals(line_edit_particle_size_precision);	}
		if(whats_This == whats_This_Particle_Height)					{ step_SpinBox->setDecimals(10); step_SpinBox->setValue(step_particle_height/length_Coeff);					step_SpinBox->setDecimals(line_edit_particle_size_precision);	}
		if(whats_This == whats_This_Particle_Distance)					{ step_SpinBox->setDecimals(10); step_SpinBox->setValue(step_particle_distance/length_Coeff);				step_SpinBox->setDecimals(line_edit_particle_lateral_distance_precision);	}
		if(whats_This == whats_This_Particle_Radial_Distance_Deviation)	{ step_SpinBox->setDecimals(10); step_SpinBox->setValue(step_particle_distance_deviation/length_Coeff);  	step_SpinBox->setDecimals(line_edit_particle_lateral_distance_precision);	}
		if(whats_This == whats_This_Domain_Size)						{ step_SpinBox->setDecimals(10); step_SpinBox->setValue(step_domain_size/correlation_Length_Coeff);			step_SpinBox->setDecimals(line_edit_cor_radius_precision);	}
		if(whats_This == whats_This_Particle_Z_Position)				{ step_SpinBox->setDecimals(10); step_SpinBox->setValue(step_particle_z_position/length_Coeff);				step_SpinBox->setDecimals(line_edit_particle_z_position_precision);	}
		if(whats_This == whats_This_Particle_Z_Position_Deviation)		{ step_SpinBox->setDecimals(10); step_SpinBox->setValue(step_particle_z_position_deviation/length_Coeff);   step_SpinBox->setDecimals(line_edit_particle_z_position_precision);	}

		if(whats_This == whats_This_Gamma)								{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_gamma);						step_SpinBox->setDecimals(line_edit_gamma_precision);			}

		if(	whats_This == whats_This_Thickness_Drift_Line_Value ||
			whats_This == whats_This_Sigma_Drift_Line_Value		)		{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_drift_linear);		step_SpinBox->setDecimals(line_edit_drift_precision);	}

		if(	whats_This == whats_This_Thickness_Drift_Rand_Rms ||
			whats_This == whats_This_Sigma_Drift_Rand_Rms		)		{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_drift_random);		step_SpinBox->setDecimals(line_edit_drift_precision);	}

		if(	whats_This == whats_This_Thickness_Drift_Sine_Amplitude ||
			whats_This == whats_This_Sigma_Drift_Sine_Amplitude		)	{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_drift_amplitude);	step_SpinBox->setDecimals(line_edit_drift_precision);	}

		if(	whats_This == whats_This_Thickness_Drift_Sine_Frequency ||
			whats_This == whats_This_Sigma_Drift_Sine_Frequency		)	{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_drift_frequency);	step_SpinBox->setDecimals(line_edit_drift_precision);	}

		if(	whats_This == whats_This_Thickness_Drift_Sine_Phase		||
			whats_This == whats_This_Sigma_Drift_Sine_Phase	)			{ step_SpinBox->setDecimals(10);	step_SpinBox->setValue(step_drift_phase);		step_SpinBox->setDecimals(line_edit_drift_precision);	}

		step_SpinBox->setRange(pow(10,-step_SpinBox->decimals()), MAX_DOUBLE);
		step_SpinBox->valueChanged(step_SpinBox->value());
	});

	// initialize
	main_Tabs->tabBarClicked(tab_Index);
}

void Table_Of_Structures::spin_Box_Recalculate(My_Table_Widget *table, int current_Row, int current_Column)
{
	add_Columns(table,current_Column);

	QCheckBox* checkbox_Recalculate = new QCheckBox("Recalculate");
		checkbox_Recalculate->setChecked(recalculate_spinbox_structure_table);
	table->setCellWidget(current_Row, current_Column, checkbox_Recalculate);

	connect(checkbox_Recalculate, &QCheckBox::toggled, this, [=]
	{
		recalculate_spinbox_structure_table = checkbox_Recalculate->isChecked();
		if(recalculate_spinbox_structure_table)	checkbox_Recalculate->setStyleSheet("QWidget { background: rgb(245, 228, 158); }");
		else							checkbox_Recalculate->setStyleSheet("background-color: white");

		if(global_Multilayer_Approach->runned_General_Settings_Editor.contains(general_Settings_Key))
		{
			global_Multilayer_Approach->general_Settings_Editor->structure_Table_Recalculation_Checkbox->setChecked(recalculate_spinbox_structure_table);
		}
	});

	// refresh in each table
	connect(main_Tabs, &QTabWidget::tabBarClicked, this, [=]
	{
		checkbox_Recalculate->setChecked(recalculate_spinbox_structure_table);
	});
	checkbox_Recalculate->toggled(recalculate_spinbox_structure_table);
}

void Table_Of_Structures::spin_Box_Mouse_Wheel(My_Table_Widget *table, int current_Row, int current_Column)
{
	add_Columns(table,current_Column);

	QCheckBox* checkbox_Mouse_Wheel = new QCheckBox("Mouse Wheel");
		checkbox_Mouse_Wheel->setChecked(mouse_wheel_spinbox_structure_table);
	table->setCellWidget(current_Row, current_Column, checkbox_Mouse_Wheel);

	connect(checkbox_Mouse_Wheel, &QCheckBox::toggled, this, [=]
	{
		mouse_wheel_spinbox_structure_table = checkbox_Mouse_Wheel->isChecked();
		if(mouse_wheel_spinbox_structure_table)
			checkbox_Mouse_Wheel->setStyleSheet("QWidget { background: rgb(255, 170, 137); }");
		else
			checkbox_Mouse_Wheel->setStyleSheet("background-color: white");
	});

	// refresh in each table
	connect(main_Tabs, &QTabWidget::tabBarClicked, this, [=]
	{
		checkbox_Mouse_Wheel->setChecked(mouse_wheel_spinbox_structure_table);
	});
	checkbox_Mouse_Wheel->toggled(mouse_wheel_spinbox_structure_table);
}

void Table_Of_Structures::spin_Box_Change_Dependent(My_Table_Widget* table, int current_Row, int current_Column)
{
	add_Columns(table,current_Column);

	QCheckBox* checkbox_Dependent = new QCheckBox("Change dependent");
		checkbox_Dependent->setChecked(refill_dependent_structure_table);
	table->setCellWidget(current_Row, current_Column, checkbox_Dependent);

	connect(checkbox_Dependent, &QCheckBox::toggled, this, [=]
	{
		refill_dependent_structure_table = checkbox_Dependent->isChecked();

		// immediately refill all tables
		refill_All_Dependent();

		if(refill_dependent_structure_table)
			checkbox_Dependent->setStyleSheet("QWidget { background: rgb(180, 255, 150); }");
		else
			checkbox_Dependent->setStyleSheet("background-color: white");
	});

	// refresh in each table
	connect(main_Tabs, &QTabWidget::tabBarClicked, this, [=]
	{
		checkbox_Dependent->setChecked(refill_dependent_structure_table);
	});

	// just colorize
	if(refill_dependent_structure_table)
		checkbox_Dependent->setStyleSheet("QWidget { background: rgb(180, 255, 150); }");
	else
		checkbox_Dependent->setStyleSheet("background-color: white");
}

void Table_Of_Structures::change_Model(My_Table_Widget *table, int tab_Index, int current_Row, int current_Column)
{
	add_Columns(table,current_Column);

	QPushButton* change_Button = new QPushButton("Set model");
	table->setCellWidget(current_Row, current_Column, change_Button);

	connect(change_Button, &QPushButton::clicked, this, [=]
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
		Table_Roughness_Model_Editor* table_Roughness_Model_Editor = new Table_Roughness_Model_Editor(multilayer);
			table_Roughness_Model_Editor->show();
	});
}

void Table_Of_Structures::refill_All_Dependent()
{
	QVector<Parameter> master_Parameters; // real master values should be changed before all refilling operations. Here just copies

	// find all top masters
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
		real_Tree_Iteration(multilayer, multilayer->structure_Tree->tree, master_Parameters);
	}

	// change dependent chain
	QVector<id_Type> ids;
	for(Parameter& master_Parameter : master_Parameters)
	{
		change_Slaves_in_Structure_Tree(master_Parameter, master_Parameter.coupled.slaves, ids);
	}

	// refresh table and independent in all tabs
	refresh_Dependents(ids, true);

	// lock and unlock
	lock_Unlock_Dependents(ids);
}

void Table_Of_Structures::real_Tree_Iteration(Multilayer* multilayer, QTreeWidget* real_Struct_Tree, QVector<Parameter>& master_Parameters)
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
//			struct_Data.fill_Potentially_Fitable_Parameters_Vector();
			struct_Data.fill_Table_Showed_Parameters_Vector(multilayer->imperfections_Model);
//			for(Parameter* parameter : struct_Data.potentially_Fitable_Parameters)
			for(Parameter* parameter : struct_Data.table_Showed_Parameters)
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

		if(refill_dependent_structure_table)
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
			if(slave_Parameter->indicator.whats_This == whats_This_Sigma_Diffuse && slave_Struct_Data.common_Sigma_Diffuse)
			{
				for(Interlayer& interlayer : slave_Struct_Data.interlayer_Composition)
				{
					interlayer.my_Sigma_Diffuse.value = slave_Struct_Data.sigma_Diffuse.value;
				}
			}

			// refresh common_sigma if you are my_sigma
			if(slave_Parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma_Diffuse && !slave_Struct_Data.common_Sigma_Diffuse )
			{
				{
					slave_Struct_Data.sigma_Diffuse.value = recalculate_Sigma_From_Individuals(slave_Struct_Data.interlayer_Composition);
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
					if(refill_dependent_structure_table)
					{
						reload_One_Widget(widget_To_Reload);
					}
				}

				// reload period and gamma
				if(widget_To_Reload->property(period_Gamma_Property).toBool() && reload_Period_Gamma)
				{
					if(refill_dependent_structure_table)
					{
						reload_One_Widget(widget_To_Reload);
					}
				}
			}
		}

		if(refill_dependent_structure_table && manual_Refresh)
		{
			Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
			multilayer->structure_Tree->refresh__StructureTree__Data_and_Text();
			// TODO INDEPENDENT
//			for(int i=0; i<multilayer->independent_Curve_Tabs->count(); ++i)
//			{
//				Independent_Curve* independent = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(i));
//				independent->reset_Independent_Variables_Structure();
//			}
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
					if(refill_dependent_structure_table)	{spin_Box->setReadOnly(true); }
					else						{spin_Box->setReadOnly(false);}
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
				if(child_Data.thickness.coupled.master.exist && refill_dependent_structure_table)
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
void Table_Of_Structures::refresh_Element(My_Table_Widget* table, bool is_Particle)
{
	QComboBox* combo_Box = qobject_cast<QComboBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = elements_Map.value(combo_Box);
	int composition_Index = combo_Box->property(num_Chemic_Element_Property).toInt();

	// for reloading
	bool reload = combo_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Stoichiometry& comp = is_Particle ? struct_Data.particles_Model.particle_Composition[composition_Index] : struct_Data.composition[composition_Index];

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
		if(is_Particle)	{
			struct_Data.particles_Model.particle_Material = Global_Variables::material_From_Composition(struct_Data.particles_Model.particle_Composition);
		} else {
			struct_Data.material = Global_Variables::material_From_Composition(struct_Data.composition);
		}
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
				Parameter& regular_Comp = is_Particle ? regular_Data.particles_Model.particle_Composition[composition_Index].composition : regular_Data.composition[composition_Index].composition;
				if(is_Particle)	{
					regular_Data.particles_Model.particle_Composition[composition_Index].type = combo_Box->currentText();
				} else {
					regular_Data.composition[composition_Index].type = combo_Box->currentText();
				}
				regular_Data.material = Global_Variables::material_From_Composition(regular_Data.composition);

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

void Table_Of_Structures::refresh_Stoich(bool is_Particle)
{
	MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = spin_Boxes_Map.value(spin_Box);
	int composition_Index = spin_Box->property(num_Chemic_Element_Property).toInt();
	QString value_Type = spin_Box->property(value_Type_Property).toString();

	// for reloading
	bool reload = spin_Box->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	Parameter& comp = is_Particle ? struct_Data.particles_Model.particle_Composition[composition_Index].composition : struct_Data.composition[composition_Index].composition;

	// reload state from struct_Tree
	if(reload)
	{
		spin_Box->blockSignals(true);
		spin_Box->setDecimals(line_edit_composition_precision);
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

		if(is_Particle) {
			struct_Data.particles_Model.particle_Material = Global_Variables::material_From_Composition(struct_Data.particles_Model.particle_Composition);
		} else {
			struct_Data.material = Global_Variables::material_From_Composition(struct_Data.composition);
		}
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
				Parameter& regular_Comp = is_Particle ? regular_Data.particles_Model.particle_Composition[composition_Index].composition : regular_Data.composition[composition_Index].composition;
				if(value_Type == VAL)	regular_Comp.value   = spin_Box->value();
				if(value_Type == MIN)	regular_Comp.fit.min = spin_Box->value();
				if(value_Type == MAX)	regular_Comp.fit.max = spin_Box->value();

				if(is_Particle) {
					regular_Data.particles_Model.particle_Material = Global_Variables::material_From_Composition(regular_Data.particles_Model.particle_Composition);
				} else {
					regular_Data.material = Global_Variables::material_From_Composition(regular_Data.composition);
				}

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
		if( refill_dependent_structure_table &&
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
		if(!reload && value_Type == VAL) {global_Multilayer_Approach->recalculate_From_Table(true);}
	}
}

void Table_Of_Structures::refresh_Fit_Element(bool is_Particle)
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
		if(is_Particle) {
			check_Box->setChecked(struct_Data.particles_Model.particle_Composition[composition_Index].composition.fit.is_Fitable);
		} else {
			check_Box->setChecked(struct_Data.composition[composition_Index].composition.fit.is_Fitable);
		}
		return;
	}
	// refresh struct_Tree
	{
		if(is_Particle) {
			struct_Data.particles_Model.particle_Composition[composition_Index].composition.fit.is_Fitable = check_Box->isChecked();
		} else {
			struct_Data.composition[composition_Index].composition.fit.is_Fitable = check_Box->isChecked();
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
				Parameter& regular_Comp = is_Particle ? regular_Data.particles_Model.particle_Composition[composition_Index].composition : regular_Data.composition[composition_Index].composition;

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

void Table_Of_Structures::refresh_Material(My_Table_Widget* table, bool is_Particle)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(reload)
	{
		if(is_Particle) {
			line_Edit->setText(struct_Data.particles_Model.particle_Material);
		} else {
			line_Edit->setText(struct_Data.material);
		}
		return;
	}
	{
		if(is_Particle) {
			struct_Data.particles_Model.particle_Material = line_Edit->text();
		} else {
			struct_Data.material = line_Edit->text();
		}

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
			for(int i=0; i<structure_Item->parent()->childCount(); i++)
			{
				if(structure_Item->parent()->child(i)==structure_Item)
				{
					my_I = i;
				}
			}

			Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			for(int n=0; n<parent_Data.num_Repetition.value(); n++)
			{
				Data& regular_Data = parent_Data.regular_Components[my_I].components[n];
				if(is_Particle) {
					regular_Data.particles_Model.particle_Material = line_Edit->text();
				} else {
					regular_Data.material = line_Edit->text();
				}

				// checking here for regular aperiodic components
				if(optical_Constants->material_Map.contains(regular_Data.material + nk_Ext))
				{
					if(is_Particle) {
						regular_Data.particles_Model.particle_Approved_Material = regular_Data.particles_Model.particle_Material;
					} else {
						regular_Data.approved_Material = regular_Data.material;
					}
				} else
				{
					if(is_Particle) {
						regular_Data.particles_Model.particle_Material = regular_Data.particles_Model.particle_Approved_Material;
					} else {
						regular_Data.material = regular_Data.approved_Material;
					}
				}
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

void Table_Of_Structures::check_Material(QLineEdit* line_Edit, bool close, bool is_Particle)
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
		QString material;
		if(is_Particle) {
			material = struct_Data.particles_Model.particle_Material;
		} else {
			material = struct_Data.material;
		}
		if(optical_Constants->material_Map.contains(material + nk_Ext))
		{
			if(is_Particle) {
				struct_Data.particles_Model.particle_Approved_Material = struct_Data.particles_Model.particle_Material;
			} else {
				struct_Data.approved_Material = struct_Data.material;
			}
		} else
		{
			line_Edit->blockSignals(true);
			if(is_Particle) {
				QMessageBox::information(this, "Wrong material", "File \"" + struct_Data.particles_Model.particle_Material + nk_Ext + "\" not found");
			} else {
				QMessageBox::information(this, "Wrong material", "File \"" + struct_Data.material + nk_Ext + "\" not found");
			}
			line_Edit->blockSignals(false);
			if(is_Particle) {
				struct_Data.particles_Model.particle_Material = struct_Data.particles_Model.particle_Approved_Material;
				line_Edit->setText(struct_Data.particles_Model.particle_Material);
			} else {
				struct_Data.material = struct_Data.approved_Material;
				line_Edit->setText(struct_Data.material);
			}
			line_Edit->textEdited(line_Edit->text());
		}
		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	{
		emit_Data_Edited();
		if(!close)
		{
			global_Multilayer_Approach->recalculate_From_Table(true);
			reload_Related_Widgets(QObject::sender());
		}
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

	if(item_Type == item_Type_Layer || item_Type == item_Type_Substrate)
	{
		if(reload)
		{
			if(whats_This == whats_This_Thickness)							label->setText(         "z ["+length_units+"]");
			if(whats_This == whats_This_Sigma_Roughness)					label->setText(Sigma_Sym+" ["+length_units+"]");
			if(whats_This == whats_This_Correlation_Radius)					label->setText(Xi_Sym+   " ["+correlation_length_units+"]");
			if(whats_This == whats_This_Linear_PSD_Omega)					label->setText(Omega_Big_Sym+" ["+omega_units+"]");
			if(whats_This == whats_This_PSD_Exponenta_Mu)					label->setText("L"+Subscript_v_Sym+" ["+mu_units+"]");
			if(whats_This == whats_This_Linear_A2)							label->setText("a"+Subscript_2_Sym+" ["+linear_a2_units+"]");
			if(whats_This == whats_This_Linear_A3)							label->setText("a"+Subscript_3_Sym+" ["+linear_a3_units+"]");
			if(whats_This == whats_This_Linear_A4)							label->setText("a"+Subscript_4_Sym+" ["+linear_a4_units+"]");
			if(whats_This == whats_This_Roughness_Peak_Sigma)				label->setText(Sigma_Sym+Subscript_v_Sym+" ["+length_units+"]");
			if(whats_This == whats_This_Roughness_Peak_Frequency 	)		label->setText(                Nu_Sym+" ["+spatial_frequency_units+"]");
			if(whats_This == whats_This_Roughness_Peak_Frequency_Width 	)	label->setText(Delta_Small_Sym+Nu_Sym+" ["+spatial_frequency_units+"]");

			if(whats_This == whats_This_Particle_Radius)					label->setText("R ["+length_units+"]");
			if(whats_This == whats_This_Particle_Height)					label->setText("H ["+length_units+"]");
			if(whats_This == whats_This_Particle_Average_Distance)			label->setText("r" + Subscript_a_Sym + " ["+length_units+"]");
			if(whats_This == whats_This_Particle_Radial_Distance)			label->setText("r ["+length_units+"]");
			if(whats_This == whats_This_Particle_Radial_Distance_Deviation)	label->setText(Delta_Small_Sym + "r ["+length_units+"]");
			if(whats_This == whats_This_Domain_Size)						label->setText("D ["+correlation_length_units+"]");
			if(whats_This == whats_This_Particle_Correlation_Depth)			label->setText("L"+ Subscript_p_Sym+" ["+mu_units+"]");
			if(whats_This == whats_This_Particle_Z_Position)				label->setText("z" + Subscript_p_Sym + " ["+length_units+"]");
			if(whats_This == whats_This_Particle_Z_Position_Deviation)		label->setText(Delta_Small_Sym + "z" + Subscript_p_Sym + " ["+length_units+"]");

			return;
		}
	}
	if(item_Type == item_Type_Multilayer)
	{
		if(reload)
		{
			if(whats_This == whats_This_Period)				label->setText("d ["+length_units+"]");
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

		if(whats_This == whats_This_Sigma_Drift_Line_Value)	check_Box->setChecked(struct_Data.sigma_Diffuse_Drift.is_Drift_Line);
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )	check_Box->setChecked(struct_Data.sigma_Diffuse_Drift.is_Drift_Rand);
		if(whats_This == whats_This_Sigma_Drift_Sine	  )	check_Box->setChecked(struct_Data.sigma_Diffuse_Drift.is_Drift_Sine);

		if(whats_This == whats_This_Sigma_Diffuse)
		{
			check_Box->setChecked(struct_Data.common_Sigma_Diffuse);
			check_Box->setText("s ["+length_units+"]");
		}

		if(struct_Data.parent_Item_Type == item_Type_Regular_Aperiodic)
		{
			Data parent_Data = structure_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			int i = check_Box->property(index_Property).toInt();
			if(whats_This == whats_This_Common_Thickness)		{check_Box->setChecked(parent_Data.regular_Components[i].is_Common_Thickness);	}
			if(whats_This == whats_This_Common_Sigma_Diffuse)	{check_Box->setChecked(parent_Data.regular_Components[i].is_Common_Sigma);		}
			if(whats_This == whats_This_Restrict_Thickness)		{check_Box->setChecked(parent_Data.regular_Components[i].use_Soft_Restrictions);}
			colorize_Regular_Aperiodic_Check_Box_Header(check_Box);
		}

		return;
	}
	// if refresh
	{
		if(whats_This == whats_This_Thickness_Drift_Line_Value)	struct_Data.thickness_Drift.is_Drift_Line = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms  )	struct_Data.thickness_Drift.is_Drift_Rand = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine      )	struct_Data.thickness_Drift.is_Drift_Sine = check_Box->isChecked();

		if(whats_This == whats_This_Sigma_Drift_Line_Value)	struct_Data.sigma_Diffuse_Drift.is_Drift_Line = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )	struct_Data.sigma_Diffuse_Drift.is_Drift_Rand = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine	  )	struct_Data.sigma_Diffuse_Drift.is_Drift_Sine = check_Box->isChecked();

		if(whats_This == whats_This_Sigma_Diffuse)					struct_Data.common_Sigma_Diffuse = check_Box->isChecked();

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
			if(whats_This == whats_This_Common_Sigma_Diffuse)
			{
				parent_Data.regular_Components[i].is_Common_Sigma = check_Box->isChecked();

				for(int n=0; n<parent_Data.num_Repetition.value(); n++)
				{
					Data& regular_Data = parent_Data.regular_Components[i].components[n];

					if(parent_Data.regular_Components[i].is_Common_Sigma)
					{
						// set common value
						regular_Data.sigma_Diffuse.value = struct_Data.sigma_Diffuse.value;
						for(Interlayer& interlayer : regular_Data.interlayer_Composition)
						{
							interlayer.my_Sigma_Diffuse.value = struct_Data.sigma_Diffuse.value;
						}

						// add to slave list (if still not)
						if(!struct_Data.sigma_Diffuse.coupled.slaves.contains(regular_Data.sigma_Diffuse.indicator)) {
							struct_Data.sigma_Diffuse.coupled.slaves.append  (regular_Data.sigma_Diffuse.indicator);
						}
						// set as master (if still not)
						if(!(regular_Data.sigma_Diffuse.coupled.master == struct_Data.sigma_Diffuse.indicator)) {
							 regular_Data.sigma_Diffuse.coupled.master =  struct_Data.sigma_Diffuse.indicator;
							 regular_Data.sigma_Diffuse.coupled.master.exist = true;
						}
					} else
					// remove slaves and master
					{
						// remove from slave list (if still there)
						if( struct_Data.sigma_Diffuse.coupled.slaves.contains (regular_Data.sigma_Diffuse.indicator)) {
							struct_Data.sigma_Diffuse.coupled.slaves.removeOne(regular_Data.sigma_Diffuse.indicator);
						}
						// remove master (if still there)
						if( regular_Data.sigma_Diffuse.coupled.master == struct_Data.sigma_Diffuse.indicator) {
							regular_Data.sigma_Diffuse.coupled.master.id = 0;
							regular_Data.sigma_Diffuse.coupled.master.exist = false;
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
		if(!reload) {global_Multilayer_Approach->recalculate_From_Table(true);}
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
	if(whats_This == whats_This_Common_Sigma_Diffuse)
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
		if( refill_dependent_structure_table &&
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
	if( refill_dependent_structure_table &&
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
				if(whats_This == whats_This_Sigma_Diffuse)
				{
					spin_Box->setValue(parameter.value/coeff);
					spin_Box->setDisabled(!struct_Data.common_Sigma_Diffuse);
					spin_Box->setStyleSheet("border: 1px solid grey");

					for(Interlayer& interlayer : struct_Data.interlayer_Composition)
					{
						if(struct_Data.common_Sigma_Diffuse)
						{
							interlayer.my_Sigma_Diffuse.value = struct_Data.sigma_Diffuse.value;
						}
						interlayer.my_Sigma_Diffuse.fit.min = struct_Data.sigma_Diffuse.fit.min;
						interlayer.my_Sigma_Diffuse.fit.max = struct_Data.sigma_Diffuse.fit.max;
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

			// VAL MIN MAX
			if(whats_This == whats_This_Correlation_Radius)
			{
				spin_Box->setMinimum(10/coeff);
				spin_Box->setMaximum(1e10/coeff);
			}			
			if(whats_This == whats_This_PSD_Exponenta_Mu ||
			   whats_This == whats_This_Particle_Correlation_Depth )
			{
				spin_Box->setMinimum(1e-1/coeff);
				spin_Box->setMaximum(1e10/coeff);
			}
			// particles
			if( whats_This == whats_This_Particle_Average_Distance ||
				whats_This == whats_This_Particle_Radial_Distance)
			{
				spin_Box->setMinimum(1/coeff);
			}
			if( whats_This == whats_This_Particle_Radial_Distance_Deviation)
			{
				spin_Box->setMinimum(0.5/coeff);
			}
			if(whats_This == whats_This_Domain_Size)
			{
				spin_Box->setMinimum(1e2/coeff);
				spin_Box->setMaximum(1e8/coeff);
			}
			if( whats_This == whats_This_Particle_Z_Position)
			{
				spin_Box->setMinimum(-10000/coeff);
			}

			spin_Box->blockSignals(false);
			return;
		}
		// if refresh
		{
			// special cases
			if(whats_This == whats_This_Sigma_Diffuse)
			{
				if(value_Type == VAL)	 parameter.value   = spin_Box->value()*coeff;
				if(value_Type == MIN)	{parameter.fit.min = spin_Box->value()*coeff; /*parameter.confidence.min = parameter.fit.min;*/}
				if(value_Type == MAX)	{parameter.fit.max = spin_Box->value()*coeff; /*parameter.confidence.max = parameter.fit.max;*/}

				for(Interlayer& interlayer : struct_Data.interlayer_Composition)
				{
					if(struct_Data.common_Sigma_Diffuse)
					{
						interlayer.my_Sigma_Diffuse.value = struct_Data.sigma_Diffuse.value;
					}
					interlayer.my_Sigma_Diffuse.fit.min = struct_Data.sigma_Diffuse.fit.min;
					interlayer.my_Sigma_Diffuse.fit.max = struct_Data.sigma_Diffuse.fit.max;
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
			if(whats_This == whats_This_Sigma_Factor_PSD_1D ||
			   whats_This == whats_This_Sigma_Factor_PSD_2D)
			{
				if(value_Type == VAL)
				{
					parameter.value   = spin_Box->value();
					parameter.fit.min = parameter.value*0.1;
					parameter.fit.max = parameter.value*10;
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

						if(whats_This == whats_This_Sigma_Diffuse)
						{
							for(Interlayer& interlayer : parent_Data.regular_Components[i].components[n].interlayer_Composition)
							{
								interlayer.my_Sigma_Diffuse.value   = parent_Data.regular_Components[i].components[n].sigma_Diffuse.value;
								interlayer.my_Sigma_Diffuse.fit.min = parent_Data.regular_Components[i].components[n].sigma_Diffuse.fit.min;
								interlayer.my_Sigma_Diffuse.fit.max = parent_Data.regular_Components[i].components[n].sigma_Diffuse.fit.max;
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
		if( refill_dependent_structure_table &&
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
		if(!reload && value_Type == VAL) {global_Multilayer_Approach->recalculate_From_Table(true);}
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
		if(whats_This == whats_This_Sigma_Diffuse)
		{
			for(Interlayer& interlayer : struct_Data.interlayer_Composition)
			{
				interlayer.my_Sigma_Diffuse.fit.is_Fitable = check_Box->isChecked();
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
		if(!struct_Data.common_Sigma_Diffuse)
		{
			struct_Data.sigma_Diffuse.value = recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
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

		MyDoubleSpinBox* spin_Box;
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		{
			spin_Box = spin_Boxes_ID_Map.key(struct_Data.interlayer_Composition[interlayer_Index].my_Sigma_Diffuse.indicator.id);
			spin_Box->setProperty(forced_Reload_Property, struct_Data.interlayer_Composition[interlayer_Index].enabled);
		}

		// refresh widget
		//	var.setValue(struct_Data.interlayer_Composition[interlayer_Index].interlayer);
		//	QWidget* back_Widget = coupled_Back_Widget_and_Id.key(struct_Data.interlayer_Composition[interlayer_Index].interlayer.indicator.id);
		//	back_Widget->setProperty(parameter_Property, var);

		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());

		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		{
			spin_Box->setProperty(forced_Reload_Property, false);
		}

		// recalculation at change
		if(!reload) {global_Multilayer_Approach->recalculate_From_Table(true);}
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
		spin_Box->setDecimals(line_edit_interlayer_precision);
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
			if(!struct_Data.common_Sigma_Diffuse)
			{
				struct_Data.sigma_Diffuse.value = recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
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
		if( refill_dependent_structure_table &&
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
		if(!reload && value_Type == VAL) {global_Multilayer_Approach->recalculate_From_Table(true);}
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
		spin_Box->setDecimals(line_edit_sigma_precision);
		spin_Box->setValue(interlayer.my_Sigma_Diffuse.value/length_Coeff);
		bool disable = struct_Data.common_Sigma_Diffuse || !interlayer.enabled;
		spin_Box->setDisabled(disable);
		spin_Box->blockSignals(false);
		return;
	}
	// if refresh
	{
		interlayer.my_Sigma_Diffuse.value = spin_Box->value()*length_Coeff;
		if(!struct_Data.common_Sigma_Diffuse)
		{
			struct_Data.sigma_Diffuse.value = recalculate_Sigma_From_Individuals(struct_Data.interlayer_Composition);
		}

		// we should save before changing dependent
		QVariant var;
		var.setValue( struct_Data );
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// change dependent if necessary
		if(struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		if( refill_dependent_structure_table &&
			!interlayer.my_Sigma_Diffuse.coupled.master.exist && interlayer.my_Sigma_Diffuse.coupled.slaves.size()>0 )
		{
			// change dependent chain
			QVector<id_Type> ids;
			change_Slaves_in_Structure_Tree(interlayer.my_Sigma_Diffuse, interlayer.my_Sigma_Diffuse.coupled.slaves, ids);

			// refresh table and independent in all tabs
			refresh_Dependents(ids);
		}
	}
	{
		emit_Data_Edited();
		reload_Related_Widgets(QObject::sender());

		// recalculation at change
		if(!reload) {global_Multilayer_Approach->recalculate_From_Table(true);}
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
			temp_Sigma_Square += pow(interlayer.my_Sigma_Diffuse.value,2) * interlayer.interlayer.value;
		}
	}

	if(abs(sum)<DBL_EPSILON) sum = DBL_EPSILON;
	return sqrt(temp_Sigma_Square/sum);
}

//// general
void Table_Of_Structures::cells_On_Off(My_Table_Widget* table, bool borders, bool keep_Disabled, bool can_Enable)
{
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());

	int current_Row = check_Box->property(row_Property).toInt();
	int current_Column = check_Box->property(column_Property).toInt();

	int row_Start = check_Box->property(relative_Rows_To_Disable_Start_Property).toInt();
	int row_Finish = check_Box->property(relative_Rows_To_Disable_Finish_Property).toInt();
	int column_Start = check_Box->property(relative_Columns_To_Disable_Start_Property).toInt();
	int column_Finish = min(check_Box->property(relative_Columns_To_Disable_Finish_Property).toInt(), table->columnCount()-1);

	for(int row=row_Start; row<=row_Finish; ++row)
	{
		for(int col=column_Start; col<=column_Finish; ++col)
		{
			QWidget* widget = table->cellWidget(current_Row+row,current_Column+col);

			if(widget && check_Box->parent()!=widget /* do not disable itself */)
			{
				// god mode always satisfies condition
				if(can_Enable) {
					widget->setProperty("keep_Disabled_Property", keep_Disabled);
				}

				// non-god mode should check
				if(!widget->property("keep_Disabled_Property").toBool()){
					widget->setDisabled(!check_Box->isChecked());
				} else {
					widget->setDisabled(true);
				}

//				widget->setDisabled(!check_Box->isChecked()); // plain old version

				if(borders)
				{
					if(!check_Box->isChecked())
						widget->setStyleSheet("border: 0px solid grey");
					else
						widget->setStyleSheet("border: 1px solid grey");
				}
			}
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
				widget->setDisabled(!check_Box->isChecked() || struct_Data.common_Sigma_Diffuse);

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

void Table_Of_Structures::reload_All_Widgets(bool reload_Disabled, QObject* sender)
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
				reload_One_Widget(widget_To_Reload, reload_Disabled);
			}
		}
	}
}

void Table_Of_Structures::reload_One_Widget(QWidget* widget_To_Reload, bool reload_Disabled)
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
	if(!reload_Disabled && !widget_To_Reload->property(enabled_Property).toBool())	return;

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
	if( whats_This == whats_This_Sigma_Diffuse )list_Pointer = &min_Max_Sigma_Diffuse_Spin_Boxes_List[current_Tab_Index];

	for(MyDoubleSpinBox* spin_Box : *list_Pointer)
	{
		spin_Box->setProperty(reload_Property, true);
		spin_Box->valueChanged(spin_Box->value());
		spin_Box->setProperty(reload_Property, false);
	}
}
