#include "multilayer_approach.h"
#include <thread>

Multilayer_Approach::Multilayer_Approach(Launcher* launcher, QWidget *parent) :
	launcher(launcher),
	QWidget(parent)
{
	setWindowTitle(multilayer_Approach_Default_Title);
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
	setAcceptDrops(true);
}

Multilayer_Approach::~Multilayer_Approach()
{
}

void Multilayer_Approach::open_Launcher()
{
	launcher->show();
}

void Multilayer_Approach::closeEvent(QCloseEvent* event)
{
	fast_Hide_Windows();
	launcher->runned_Multilayer_Approaches.remove(multilayer_Approach_Key);	
	write_Window_Geometry();
	Settings::save_All_Settings();

	// TODO
	qApp->quit();
	event->accept();
	emit closed();
}

void Multilayer_Approach::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
		main_Layout->setContentsMargins(4,3,2,3);

    create_Multilayer_Tabs ();
	create_Fitting_Settings();
}

void Multilayer_Approach::create_Menu()
{
	menu = new Menu(window_Type_Multilayer_Approach, this);
		main_Layout->setMenuBar(menu->menu_Bar);
}

void Multilayer_Approach::create_Multilayer_Tabs()
{
	multilayer_Tabs = new QTabWidget(this);
		multilayer_Tabs->setMovable(true);
		multilayer_Tabs->setTabsClosable(true);
		multilayer_Tabs->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);

	add_Tab_Corner_Button = new QToolButton;
		add_Tab_Corner_Button->setText("+");
	QFont tmp_Qf; tmp_Qf.setBold(true);
		add_Tab_Corner_Button->setFont(tmp_Qf);
	multilayer_Tabs->setCornerWidget(add_Tab_Corner_Button);
	main_Layout->addWidget(multilayer_Tabs);

	connect(multilayer_Tabs->tabBar(), &QTabBar::customContextMenuRequested,  this, &Multilayer_Approach::tab_Context_Menu);
	connect(add_Tab_Corner_Button,  &QToolButton::clicked,			 this, [=]{add_Multilayer();});
	connect(multilayer_Tabs,		&QTabWidget::tabCloseRequested,  this, &Multilayer_Approach::remove_Multilayer);
	connect(multilayer_Tabs,		&QTabWidget::currentChanged,	 this, &Multilayer_Approach::change_Tab_Color);
	connect(multilayer_Tabs,		&QTabWidget::tabBarDoubleClicked,this, &Multilayer_Approach::rename_Multilayer);

	connect(multilayer_Tabs,		&QTabWidget::currentChanged,	 this, [=]
	{
		if(can_Change_Index)
		{
			can_Change_Index = false;
			if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key)) {table_Of_Structures		  ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key))	  {table_Of_Roughness		  ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Optical_Graphs.contains(optical_Graphs_Key))			  {optical_Graphs			  ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Profile_Plots_Window.contains(profile_Plots_Key))		  {profile_Plots_Window		  ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Calculation_Settings_Editor.contains(calc_Settings_Key)){calculation_Settings_Editor->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			can_Change_Index = tab_synchronization;
		}		
	});

	add_Multilayer();
}

void Multilayer_Approach::create_Fitting_Settings()
{
	fitting_Settings = new Fitting_Settings;
}

void Multilayer_Approach::set_Window_Geometry()
{
	setMinimumWidth(multilayer_min_width);
	setMinimumHeight(multilayer_min_height);
	setGeometry(multilayer_x_corner,multilayer_y_corner,multilayer_width,multilayer_height);
}

void Multilayer_Approach::write_Window_Geometry()
{
	if(!isMaximized())
	{
		multilayer_x_corner = frameGeometry().x()-corner_x_shift;
		multilayer_y_corner = frameGeometry().y()-corner_y_shift;

		multilayer_width  = geometry().width() +  width_add;
		multilayer_height = geometry().height()+ height_add;
	}
}

void Multilayer_Approach::fast_Hide_Windows()
{
	hide();
	if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key))	{
		table_Of_Structures->close();
	}
	if(runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key))	{
		table_Of_Roughness->close();
	}
	if(runned_Optical_Graphs.contains(optical_Graphs_Key))	{
		optical_Graphs->close();
	}
	if(runned_Profile_Plots_Window.contains(profile_Plots_Key))	{
		profile_Plots_Window->close();
	}
	if(runned_Fits_Selectors.contains(fits_Selector_Key))	{
		fits_Selector->close();
	}
	if(runned_Calculation_Settings_Editor.contains(calc_Settings_Key))	{
		calculation_Settings_Editor->close();
	}
	if(runned_Fitting_Settings_Editor.contains(fit_Settings_Key))	{
		fitting_Settings_Editor->close();
	}
	for(Regular_Aperiodic_Table* regular_Aperiodic_Table: runned_Regular_Aperiodic_Tables) {
		regular_Aperiodic_Table->write_Window_Geometry();
		regular_Aperiodic_Table->hide();
	}
}

void Multilayer_Approach::tab_Context_Menu(const QPoint& pos)
{
	QMenu menu;
	QAction duplicate_Action("Duplicate structure");
	menu.addAction(&duplicate_Action);
	connect(&duplicate_Action, &QAction::triggered, this, [=]{duplicate_Structure(pos);});
	menu.exec(QCursor::pos());
}

void Multilayer_Approach::duplicate_Structure(const QPoint &pos)
{
	int old_Tab_Index = multilayer_Tabs->tabBar()->tabAt(pos);
	Multilayer* new_Multilayer = add_Multilayer(old_Tab_Index);
	Multilayer* old_Multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(old_Tab_Index));

	*new_Multilayer = *old_Multilayer;
}

Multilayer* Multilayer_Approach::add_Multilayer(int index)
{
	Multilayer* new_Multilayer = new Multilayer(this);
		new_Multilayer->setContentsMargins(-80,-10,-80,-10);

	connect(new_Multilayer, &Multilayer::refresh_All_Multilayers, this, &Multilayer_Approach::refresh_All_Multilayers_View);

	if(index < 0)
	{
		multilayer_Tabs->addTab(new_Multilayer, default_multilayer_tab_name);
		multilayer_Tabs->setTabText(multilayer_Tabs->count()-1, default_multilayer_tab_name + Locale.toString(multilayer_Tabs->count()));
	} else
	{
		multilayer_Tabs->insertTab(index+1, new_Multilayer, multilayer_Tabs->tabText(index) + " (copy)");
	}

	if(multilayer_Tabs->count()>1)
	{
		multilayer_Tabs->tabBar()->setTabTextColor(multilayer_Tabs->count()-1,Qt::gray);
		multilayer_Tabs->tabBar()->tabButton(multilayer_Tabs->count()-1, QTabBar::RightSide)->hide();
	}
	multilayer_Tabs->currentChanged(multilayer_Tabs->currentIndex());
	return new_Multilayer;
}

void Multilayer_Approach::remove_Multilayer(int index)
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal",
															  "Multilayer \"" + multilayer_Tabs->tabBar()->tabText(index) + "\" will be removed.\nContinue?",
															  QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if (reply == QMessageBox::Yes)
	{
		delete multilayer_Tabs->widget(index);
		if(multilayer_Tabs->count()==0) add_Multilayer();
	}
}

void Multilayer_Approach::change_Tab_Color(int index)
{
	multilayer_Tabs->tabBar()->setTabTextColor(index,Qt::black);
	if(multilayer_Tabs->tabBar()->tabButton(index, QTabBar::RightSide))
	   multilayer_Tabs->tabBar()->tabButton(index, QTabBar::RightSide)->show();

	for(int i = 0; i<multilayer_Tabs->tabBar()->count(); i++)
	{
		if(i!=index)
		{
			multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->hide();
			multilayer_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	}
}

void Multilayer_Approach::rename_Multilayer(int tab_Index)
{
	bool ok;
	QString text = QInputDialog::getText(this, "Rename structure", "New name", QLineEdit::Normal, multilayer_Tabs->tabText(tab_Index), &ok);
	if (ok && !text.isEmpty())
	{
		multilayer_Tabs->setTabText(tab_Index, text);
	}
}

void Multilayer_Approach::add_Fitted_Structure(QVector<QTreeWidget*>& fitted_Trees_for_Copying, QString name_Modificator, int run)
{
	// new instance for storing
	Fitted_Structure new_Fitted_Structure;

	// copy trees
	new_Fitted_Structure.fitted_Trees.resize(fitted_Trees_for_Copying.size());

	for(int tab_Index=0; tab_Index<fitted_Trees_for_Copying.size(); ++tab_Index)
	{
		new_Fitted_Structure.fitted_Trees[tab_Index] = new QTreeWidget(this);
		QTreeWidget* copy = new_Fitted_Structure.fitted_Trees[tab_Index];
		QTreeWidget* old  = fitted_Trees_for_Copying[tab_Index];

		// copy id
		id_Type id = qvariant_cast<id_Type>(old->property(id_Property));
		copy->setProperty(id_Property, id);

		// copy tree with data
		for(int child_Index=0; child_Index<old->topLevelItemCount(); ++child_Index)
		{
			copy->addTopLevelItem(old->topLevelItem(child_Index)->clone());
		}
	}

	// generate name
	QDateTime date_Time = QDateTime::currentDateTime();
	if( name_Modificator == current_State )	{
		new_Fitted_Structure.name = "# " + Locale.toString(++fits_Positive_Counter) + " state ||  "                                + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss");	}
	if( name_Modificator == fitted_State )	{
		new_Fitted_Structure.name = "# " + Locale.toString(++fits_Positive_Counter) + " fit ||  "                                  + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss");	}
	if( name_Modificator == fit_Run_State )	{
		new_Fitted_Structure.name = "# " + Locale.toString(++fits_Positive_Counter) + " fit run " + Locale.toString(run) + " ||  " + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss");	}

	// put new instance to storage
	fitted_Structures.append(new_Fitted_Structure);

	if(runned_Fits_Selectors.contains(fits_Selector_Key))
	{
		fits_Selector->add_Item(new_Fitted_Structure);
	}
}

void Multilayer_Approach::open_Table_Of_Structures()
{
	if(!runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
	{
		runned_Tables_Of_Structures.insert(table_Of_Structures_Key, table_Of_Structures);
		table_Of_Structures = new Table_Of_Structures;
			table_Of_Structures->setWindowFlags(Qt::Window);
			table_Of_Structures->show();

		runned_Tables_Of_Structures.clear();
		runned_Tables_Of_Structures.insert(table_Of_Structures_Key, table_Of_Structures);

		connect(table_Of_Structures, &Table_Of_Structures::data_Edited, this, &Multilayer_Approach::refresh_All_Multilayers_View);

		for(int i=0; i<multilayer_Tabs->count(); ++i)
		{
			Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));
			connect(multilayer, &Multilayer::refresh_All_Multilayers, table_Of_Structures, [=]{table_Of_Structures->reload_All_Widgets();});
		}
	} else
	{
		table_Of_Structures->activateWindow();
	}
}

void Multilayer_Approach::open_Table_Of_Roughness()
{
	if(!runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key))
	{
		runned_Tables_Of_Roughness.insert(table_Of_Roughness_Key, table_Of_Roughness);
		table_Of_Roughness = new Table_Of_Roughness();
			table_Of_Roughness->setWindowFlags(Qt::Window);
			table_Of_Roughness->show();

		runned_Tables_Of_Roughness.clear();
		runned_Tables_Of_Roughness.insert(table_Of_Roughness_Key, table_Of_Roughness);

		// TODO
//		connect(table_Of_Roughness, &Table_Of_Roughness::data_Edited, this, &Multilayer_Approach::refresh_All_Multilayers_View);
//		for(int i=0; i<multilayer_Tabs->count(); ++i)
//		{
//			Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));
//			connect(multilayer, &Multilayer::refresh_All_Multilayers, table_Of_Structures, [=]{table_Of_Roughness->reload_All_Widgets();});
//		}
	} else
	{
		table_Of_Roughness->activateWindow();
	}}

void Multilayer_Approach::open_Optical_Graphs(QString keep_Splitter)
{	
	if(!runned_Optical_Graphs.contains(optical_Graphs_Key))
	{
		runned_Optical_Graphs.insert(optical_Graphs_Key, optical_Graphs);
		optical_Graphs = new Optical_Graphs(keep_Splitter);//(this);
			optical_Graphs->setWindowFlags(Qt::Window);
			optical_Graphs->show();

		runned_Optical_Graphs.clear();
		runned_Optical_Graphs.insert(optical_Graphs_Key, optical_Graphs);

	} else
	{
		optical_Graphs->activateWindow();
	}
}

void Multilayer_Approach::open_Profile_Plots()
{
	if(!runned_Profile_Plots_Window.contains(profile_Plots_Key))
	{
		runned_Profile_Plots_Window.insert(profile_Plots_Key, profile_Plots_Window);
		profile_Plots_Window = new Profile_Plots_Window();//(this);
			profile_Plots_Window->setWindowFlags(Qt::Window);
			profile_Plots_Window->show();

		runned_Profile_Plots_Window.clear();
		runned_Profile_Plots_Window.insert(profile_Plots_Key, profile_Plots_Window);

	} else
	{
		profile_Plots_Window->activateWindow();
	}
}

void Multilayer_Approach::open_Fits_Selector()
{
	if(!runned_Fits_Selectors.contains(fits_Selector_Key))
	{
		runned_Fits_Selectors.insert(fits_Selector_Key, fits_Selector);
		fits_Selector = new Fits_Selector;//(this);
			fits_Selector->setWindowFlags(Qt::Window);
			fits_Selector->show();

		runned_Fits_Selectors.clear();
		runned_Fits_Selectors.insert(fits_Selector_Key, fits_Selector);

	} else
	{
		fits_Selector->activateWindow();
	}
}

void Multilayer_Approach::open_Calculation_Settings()
{
	if(!runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
	{
		runned_Calculation_Settings_Editor.insert(calc_Settings_Key, calculation_Settings_Editor);
		calculation_Settings_Editor = new Calculation_Settings_Editor;//(this);
			calculation_Settings_Editor->setWindowFlags(Qt::Window);
			calculation_Settings_Editor->show();

		runned_Calculation_Settings_Editor.clear();
		runned_Calculation_Settings_Editor.insert(calc_Settings_Key, calculation_Settings_Editor);

	} else
	{
		calculation_Settings_Editor->activateWindow();
	}
}

void Multilayer_Approach::open_General_Settings()
{
	// TODO
	qInfo() << "open general settings" << endl;
}

void Multilayer_Approach::open_Fitting_Settings()
{
	if(!runned_Fitting_Settings_Editor.contains(fit_Settings_Key))
	{
		runned_Fitting_Settings_Editor.insert(fit_Settings_Key, fitting_Settings_Editor);
		fitting_Settings_Editor = new Fitting_Settings_Editor;//(this);
			fitting_Settings_Editor->setWindowFlags(Qt::Window);
			fitting_Settings_Editor->show();

		runned_Fitting_Settings_Editor.clear();
		runned_Fitting_Settings_Editor.insert(fit_Settings_Key, fitting_Settings_Editor);

	} else
	{
		fitting_Settings_Editor->activateWindow();
	}
}

void Multilayer_Approach::open_Regular_Aperiodic_Layers_Table(int tab_Index, QTreeWidgetItem* item)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));
	Data struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(!runned_Regular_Aperiodic_Tables.contains(struct_Data.id))
	{
		Regular_Aperiodic_Table* regular_Aperiodic_Table = new Regular_Aperiodic_Table(item, multilayer);
			regular_Aperiodic_Table->setWindowFlags(Qt::Window);
			regular_Aperiodic_Table->show();

		connect(regular_Aperiodic_Table, &Regular_Aperiodic_Table::regular_Aperiodic_Edited, this, &Multilayer_Approach::refresh_All_Multilayers_View);
	} else
	{
		runned_Regular_Aperiodic_Tables.value(struct_Data.id)->activateWindow();
	}
}

void Multilayer_Approach::temporarily_Close_Regular_Aperiodics()
{
	rerun_Of_Regular_Aperiodic_Tables_List.clear();
	for(Regular_Aperiodic_Table* regular_Aperiodic_Table: runned_Regular_Aperiodic_Tables_List) {

		int tab_Index = -2019;
		for(int i=0; i<multilayer_Tabs->count(); i++)
		{
			Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));
			if(multilayer == regular_Aperiodic_Table->multilayer)	{tab_Index = i;}
		}
		Data item_Data = regular_Aperiodic_Table->item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>();
		Regular_Aperiodic_Table_Launch lunch = {item_Data.id, tab_Index};
		rerun_Of_Regular_Aperiodic_Tables_List.append(lunch);
		regular_Aperiodic_Table->close();
	}
}

void Multilayer_Approach::close_Item_Editors()
{
	// close item editors
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

		for(Item_Editor* item_Editor : multilayer->structure_Tree->list_Editors)
		{
			item_Editor->close();
		}
		multilayer->structure_Tree->list_Editors.clear();
	}
}

void Multilayer_Approach::reopen_Regular_Aperiodics()
{
	for(Regular_Aperiodic_Table_Launch& lunch: rerun_Of_Regular_Aperiodic_Tables_List)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(lunch.tab_Index));
		QTreeWidgetItem* structure_Item = Global_Variables::get_Item_From_Tree_By_Id(multilayer->structure_Tree->tree, lunch.item_Id);
		Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		open_Regular_Aperiodic_Layers_Table(lunch.tab_Index, structure_Item);
		if(table_Of_Structures)
		{
			Regular_Aperiodic_Table* regular_Aperiodic_Table = runned_Regular_Aperiodic_Tables.value(struct_Data.id);

			disconnect(regular_Aperiodic_Table, &Regular_Aperiodic_Table::regular_Aperiodic_Edited, table_Of_Structures, &Table_Of_Structures::reload_From_Regular_Aperiodic);
			   connect(regular_Aperiodic_Table, &Regular_Aperiodic_Table::regular_Aperiodic_Edited, table_Of_Structures, &Table_Of_Structures::reload_From_Regular_Aperiodic);

			disconnect(table_Of_Structures, &Table_Of_Structures::regular_Layer_Edited, regular_Aperiodic_Table, &Regular_Aperiodic_Table::reload_All_Widgets);
			   connect(table_Of_Structures, &Table_Of_Structures::regular_Layer_Edited, regular_Aperiodic_Table, &Regular_Aperiodic_Table::reload_All_Widgets);
		}
	}
}

void Multilayer_Approach::lock_Mainwindow_Interface()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));

		// lock multilayer tabs
		if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key) ||
		   runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key) ||
		   runned_Optical_Graphs.contains(optical_Graphs_Key) ||
		   runned_Profile_Plots_Window.contains(profile_Plots_Key) ||
		   runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(true);
			multilayer_Tabs->setMovable(false);
			multilayer_Tabs->cornerWidget()->setDisabled(true);
			disconnect(multilayer_Tabs->tabBar(), &QTabBar::customContextMenuRequested,  this, &Multilayer_Approach::tab_Context_Menu);
		}

		// lock tree
		if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key) ||
		   runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key) /*||
		   runned_Profile_Plots_Window.contains(profile_Plots_Key)*/)
		{
			// close editors
			for(Item_Editor* editor : multilayer->structure_Tree->list_Editors)	editor->close();

			multilayer->structure_Tree->structure_Toolbar->toolbar->setDisabled(true);
			multilayer->structure_Tree->tree->blockSignals(true);
		}

		// lock independent tabs
		if(runned_Optical_Graphs.contains(optical_Graphs_Key) ||
		   runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
			{
				Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));
				if(runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
				{independent_Variables->independent_Variables_Toolbar->setDisabled(true);}

				multilayer->independent_Variables_Plot_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(true);
			}
			multilayer->independent_Variables_Plot_Tabs->setMovable(false);
			multilayer->independent_Variables_Corner_Button->setDisabled(true);
		}

		// lock target buttons
		if(runned_Optical_Graphs.contains(optical_Graphs_Key) ||
		   runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			for(int i=0; i<multilayer->data_Target_Profile_Frame_Vector.size(); ++i)
			{
//				multilayer->add_Buttons_To_Lock[i]->setDisabled(true);
				multilayer->remove_Buttons_To_Lock[i]->setDisabled(true);
			}
		}
	}
}

void Multilayer_Approach::unlock_Mainwindow_Interface()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));

		// unlock multilayer tabs
		if(!runned_Tables_Of_Structures.contains(table_Of_Structures_Key) &&
		   !runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key) &&
		   !runned_Optical_Graphs.contains(optical_Graphs_Key) &&
		   !runned_Profile_Plots_Window.contains(profile_Plots_Key) &&
		   !runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(false);
			multilayer_Tabs->setMovable(true);
			add_Tab_Corner_Button->setDisabled(false);
			disconnect(multilayer_Tabs->tabBar(), &QTabBar::customContextMenuRequested,  this, &Multilayer_Approach::tab_Context_Menu);
			   connect(multilayer_Tabs->tabBar(), &QTabBar::customContextMenuRequested,  this, &Multilayer_Approach::tab_Context_Menu);
		}

		// unlock tree
		if(!runned_Tables_Of_Structures.contains(table_Of_Structures_Key) &&
		   !runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key)/* &&
		   !runned_Profile_Plots_Window.contains(profile_Plots_Key)*/)
		{
			multilayer->structure_Tree->structure_Toolbar->toolbar->setDisabled(false);
			multilayer->structure_Tree->tree->blockSignals(false);
		}

		// unlock independent tabs
		if(!runned_Optical_Graphs.contains(optical_Graphs_Key) &&
		   !runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
			{
				Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));
				independent_Variables->independent_Variables_Toolbar->setDisabled(false);

				multilayer->independent_Variables_Plot_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(false);
			}
			multilayer->independent_Variables_Plot_Tabs->setMovable(true);
			multilayer->independent_Variables_Corner_Button->setDisabled(false);
		} else
		if(!runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
			{
				Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));
				independent_Variables->independent_Variables_Toolbar->setDisabled(false);
			}
		}

		// unlock target buttons
		if(!runned_Optical_Graphs.contains(optical_Graphs_Key) &&
		   !runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
				for(int i=0; i<multilayer->data_Target_Profile_Frame_Vector.size(); ++i)
			{
//				multilayer->add_Buttons_To_Lock[i]->setDisabled(false);
				multilayer->remove_Buttons_To_Lock[i]->setDisabled(false);
			}
		}
	}
}

void Multilayer_Approach::refresh_All_Multilayers_View()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));
		if(multilayer!=sender())
		{
			multilayer->refresh_Structure_And_Independent(sender());
		}
	}
}

void Multilayer_Approach::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void Multilayer_Approach::dropEvent(QDropEvent* event)
{
	int counter = 0;
	foreach (const QUrl &url, event->mimeData()->urls())
	{
		if(counter==0)
		{
			QFileInfo filename = url.toLocalFile();
			QMessageBox::StandardButton reply = QMessageBox::question(this, "Open", "Open file " + filename.fileName() + " ?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
			if (reply == QMessageBox::Yes)
			{
				last_file = filename.absoluteFilePath();
				last_directory = filename.absolutePath();

				open(last_file);
			}
		}
		++counter;
	}
}

void Multilayer_Approach::open(QString filename)
{
	// TODO
	int max_Num_Targets=0;

	// check filepath
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(nullptr, "File not found", "Nothing to open");
		return;
	}

	// check extension
	QFileInfo file_Info(filename);
	if(file_Info.suffix() != file_Extension)
	{
		QMessageBox::information(nullptr, "Wrong file type", "\"" + file_Info.fileName() + "\" has wrong type.\nOnly \"." + QString(file_Extension) + "\" files can be opened");
		return;
	}

	QDataStream in(&file);

	// check control string
	QString control = "";
	in >> control;
	if(control!=control_String)
	{
		QMessageBox::information(nullptr, "General Failor", "Can't read your data");
		file.close();
		return;
	}

	// read version
	in >> loaded_Version_Major;
	in >> loaded_Version_Minor;
	in >> loaded_Version_Build;

	if( (loaded_Version_Major <VERSION_MAJOR) ||
	   ((loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor <VERSION_MINOR)) ||
	   ((loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor==VERSION_MINOR) && (loaded_Version_Build<VERSION_BUILD)) )
	{
		QMessageBox::StandardButton reply = QMessageBox::warning(this,"Opening old file","Do you want to open file,\ncreated by v."
							 + Locale.toString(loaded_Version_Major) + "."
							 + Locale.toString(loaded_Version_Minor) + "."
							 + Locale.toString(loaded_Version_Build) + " in v."
							 + Locale.toString(VERSION_MAJOR) + "."
							 + Locale.toString(VERSION_MINOR) + "."
							 + Locale.toString(VERSION_BUILD) + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::No) return;
	}
	if( (loaded_Version_Major >VERSION_MAJOR) ||
	   ((loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor >VERSION_MINOR)) ||
	   ((loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor==VERSION_MINOR) && (loaded_Version_Build>VERSION_BUILD)) )
	{
		QMessageBox::warning(this,"Opening old file","File, created by newer version "
							 + Locale.toString(loaded_Version_Major) + "."
							 + Locale.toString(loaded_Version_Minor) + "."
							 + Locale.toString(loaded_Version_Build) + "\ncan't be opened in "
							 + Locale.toString(VERSION_MAJOR) + "."
							 + Locale.toString(VERSION_MINOR) + "."
							 + Locale.toString(VERSION_BUILD));
		return;
	}

	// close table of structures
	bool reopen_Table = runned_Tables_Of_Structures.contains(table_Of_Structures_Key);
	int active_Tab_Tables_Of_Structures = -2019;
	if(reopen_Table)
	{
		active_Tab_Tables_Of_Structures = runned_Tables_Of_Structures.value(table_Of_Structures_Key)->main_Tabs->currentIndex();
		runned_Tables_Of_Structures.value(table_Of_Structures_Key)->close();
	}

	// close calculation settings
	bool reopen_Calc_Settings = runned_Calculation_Settings_Editor.contains(calc_Settings_Key);
	int active_Tab_Calculation_Settings_Editor = -2019;
	if(reopen_Calc_Settings)
	{
		active_Tab_Calculation_Settings_Editor = runned_Calculation_Settings_Editor.value(calc_Settings_Key)->main_Tabs->currentIndex();
		runned_Calculation_Settings_Editor.value(calc_Settings_Key)->close();
	}

	// close graphs
	bool reopen_Graphs = runned_Optical_Graphs.contains(optical_Graphs_Key);
	int active_Tab_Optical_Graphs = -2019;
	if(reopen_Graphs)
	{
		active_Tab_Optical_Graphs = runned_Optical_Graphs.value(optical_Graphs_Key)->main_Tabs->currentIndex();
		runned_Optical_Graphs.value(optical_Graphs_Key)->close();
	}

	// close profile
	bool reopen_Profile_Plots = runned_Profile_Plots_Window.contains(profile_Plots_Key);
	int active_Tab_Profile_Plots = -2019;
	if(reopen_Profile_Plots)
	{
		active_Tab_Profile_Plots = runned_Profile_Plots_Window.value(profile_Plots_Key)->main_Tabs->currentIndex();
		runned_Profile_Plots_Window.value(profile_Plots_Key)->close();
	}

	// close fitting settings
	bool reopen_Fit_Settings = runned_Fitting_Settings_Editor.contains(fit_Settings_Key);
	if(reopen_Fit_Settings)
	{
		runned_Fitting_Settings_Editor.value(fit_Settings_Key)->close();
	}

	// close fits selector
	bool reopen_Fits_Selector = runned_Fits_Selectors.contains(fits_Selector_Key);
	if(reopen_Fits_Selector)
	{
		runned_Fits_Selectors.value(fits_Selector_Key)->close();
	}

	// close target editors
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));
		for(Target_Curve_Editor* target_Curve_Editor : multilayer->runned_Target_Curve_Editors.values())
		{
			target_Curve_Editor->close();
		}
	}

	// close aperiodic tables
	temporarily_Close_Regular_Aperiodics();

	// read previous id
	in >> previous_ID;

	// clear existing tabs
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		// for not growing in size
//		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));
//		for(int i=multilayer->data_Target_Profile_Frame_Vector.size()-1; i>=0; --i)
//		{
//			multilayer->remove_Target_Curve(i,true);
//		}

		delete multilayer_Tabs->widget(i);
	}
	multilayer_Tabs->clear();

	// read number of tabs
	int num_Multilayers;
	in >> num_Multilayers;

	// load multilayer tabs
	for(int i=0; i<num_Multilayers; ++i)
	{
		// add new multilayer
		add_Multilayer();

		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));

		// set tab name
		QString multilayer_tab_name;
		in >> multilayer_tab_name;
		multilayer_Tabs->setTabText(i, multilayer_tab_name);

		// load tree
		Global_Variables::deserialize_Tree(in, multilayer->structure_Tree->tree);

		/// variables
		{
			// load index of active variables tab
			int current_Variable_Tab_Index;
			in >> current_Variable_Tab_Index;
			multilayer->variables_Tabs->setCurrentIndex(current_Variable_Tab_Index);
		}
		/// independent
		{
			// clear existing independent tabs
			for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
			{
				delete multilayer->independent_Variables_Plot_Tabs->widget(i);
			}
			multilayer->independent_Variables_Plot_Tabs->clear();

			// read number of independent tabs
			int num_Independent;
			in >> num_Independent;

			// load independent tabs
			for(int i=0; i<num_Independent; ++i)
			{
				multilayer->add_Independent_Variables_Tab();
				Independent_Variables* independent = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));

				// load plot name
				QString tab_Text;
				in >> tab_Text;
				multilayer->independent_Variables_Plot_Tabs->setTabText(i, tab_Text);

				// load main data
				in >> independent;

				// load real tree
				independent->real_Struct_Tree = multilayer->structure_Tree->tree;

				// load plot tree
				Global_Variables::deserialize_Tree(in, independent->struct_Tree_Copy);

				// load variables list
				Global_Variables::deserialize_Variables_List(in, independent->independent_Variables_List);
			}

			// load index of active independent tab
			int current_Independent_Tab_Index;
			in >> current_Independent_Tab_Index;
			multilayer->independent_Variables_Plot_Tabs->setCurrentIndex(current_Independent_Tab_Index);

			// load calc property for independent plots
			in >> multilayer->enable_Calc_Independent_Curves;

			// load number of calculation settings independent rows
			if(Global_Variables::check_Loaded_Version(1,9,3))
			{in >> multilayer->num_Independent_Rows;}

			// load number of independent graph rows
			if(Global_Variables::check_Loaded_Version(1,7,8) && !Global_Variables::check_Loaded_Version(1,9,3))
			{in >> multilayer->graph_Options.num_Independent_Graph_Rows;}	// since 1.7.8 and before 1.9.3
		}
		/// target
		{
			// clear existing targets
			for(int i=multilayer->data_Target_Profile_Frame_Vector.size()-1; i>=0; --i)
			{
				multilayer->remove_Target_Curve(i,true);
			}

			// load number of target curves/frames
			int num_Target;
			in >> num_Target;

			// for window resizing
			if(num_Target>max_Num_Targets) max_Num_Targets=num_Target;

			// load target profiles
			for(int i=0; i<num_Target; ++i)
			{
				// create default target
				multilayer->add_Target_Curve(i, true);

				// work with it
				Target_Curve* target_Curve = multilayer->target_Profiles_Vector[i];

				// load main data
				in >> target_Curve;
				multilayer->set_Index_To_Target_Curves();
			}

			// load calc property for target plots
			in >> multilayer->enable_Calc_Target_Curves;

			// load number of calculation settings target rows
			if(Global_Variables::check_Loaded_Version(1,7,8))
			{in >> multilayer->num_Target_Rows;}

			// load number of target graph rows
			if(Global_Variables::check_Loaded_Version(1,7,8) && !Global_Variables::check_Loaded_Version(1,9,3))
			{in >> multilayer->graph_Options.num_Target_Graph_Rows;}	// since 1.7.8 and before 1.9.3
		}

		/// graph options
		if(Global_Variables::check_Loaded_Version(1,9,3))
		{in >> multilayer->graph_Options;}	              // since 1.9.3

		/// profile plot
		if(Global_Variables::check_Loaded_Version(1,10,2))
		{in >> multilayer->profile_Plot_Options >> multilayer->profile_Plot_Data;}	  // since 1.10.2

		/// discretization
		if(Global_Variables::check_Loaded_Version(1,10,2))
		{in >> multilayer->discretization_Parameters;}	  // since 1.10.2

		/// fitting settings
		{
			in >> fitting_Settings;
		}

		// load min_max values for fitting
		in >> multilayer->min_Max_Density;
		in >> multilayer->min_Max_Thickness;
		in >> multilayer->min_Max_Sigma;

		// refresh all
		multilayer->structure_Tree->structure_Toolbar->refresh_Toolbar();

		// disable adding substrate if it already exists
		QString item_Type = multilayer->structure_Tree->tree->topLevelItem(multilayer->structure_Tree->tree->topLevelItemCount()-1)->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>().item_Type;
		if(item_Type == item_Type_Substrate)
		{
			multilayer->structure_Tree->structure_Toolbar->toolbar->actions()[Add_Substrate]->setDisabled(true);
		}
	}

	// load index of active multilayer tab
	int multilayer_Tab_Index;
	in >> multilayer_Tab_Index;
	multilayer_Tabs->setCurrentIndex(multilayer_Tab_Index);

	/// load fits
	{
		// load counter
		in >> fits_Positive_Counter;

		// load number of fitted structures
		int num_Fit_Struc = 0;
		in >> num_Fit_Struc;

		// clear existing fitted_Structures
		fitted_Structures.clear();
		fitted_Structures.resize(num_Fit_Struc);

		// load fitted structures
		for(Fitted_Structure& fitted_Structure : fitted_Structures)
		{
			// load name
			in >> fitted_Structure.name;

			// load number of structure trees
			int num_Trees = 0;
			in >> num_Trees;

			// clear existing trees and create empty trees
			fitted_Structure.fitted_Trees.clear();
			fitted_Structure.fitted_Trees.resize(num_Trees);

			// load structure trees
			for(int tree_Index=0;  tree_Index<num_Trees; ++tree_Index)
			{
				fitted_Structure.fitted_Trees[tree_Index] = new QTreeWidget(this);
				Global_Variables::deserialize_Tree(in, fitted_Structure.fitted_Trees[tree_Index]);
			}
		}
	}

	/// load plot splitters state
	if(Global_Variables::check_Loaded_Version(1,9,5))
	{
		in >> target_Independent_Splitter_Position_Vec;
		in >> target_Vertical_Splitter_Position_Vec;
		in >> target_Horizontal_Splitter_Position_Vec_Vec;
		in >> independent_Vertical_Splitter_Position_Vec;
		in >> independent_Horizontal_Splitter_Position_Vec_Vec;
	}

	file.close();

	// resizing
	if(max_Num_Targets>=2 && height()<=multilayer_height)
	{
		resize(width(), multilayer_height /*+ (max_Num_Targets-1)*multilayer_height_additive*/);
	}

	// reopen table of structures
	if(reopen_Table)
	{
		open_Table_Of_Structures();
		runned_Tables_Of_Structures.value(table_Of_Structures_Key)->main_Tabs->setCurrentIndex(active_Tab_Tables_Of_Structures);
	}

	// reopen calculation settings
	if(reopen_Calc_Settings)
	{
		open_Calculation_Settings();
		runned_Calculation_Settings_Editor.value(calc_Settings_Key)->main_Tabs->setCurrentIndex(active_Tab_Calculation_Settings_Editor);
	}

	// reopen graphs
	if(reopen_Graphs)
	{
		open_Optical_Graphs();
		runned_Optical_Graphs.value(optical_Graphs_Key)->main_Tabs->setCurrentIndex(active_Tab_Optical_Graphs);
	}

	// close profile
	if(reopen_Profile_Plots)
	{
		open_Profile_Plots();
		runned_Profile_Plots_Window.value(profile_Plots_Key)->main_Tabs->setCurrentIndex(active_Tab_Profile_Plots);
	}

	// reopen fitting settings
	if(reopen_Fit_Settings)
	{
		open_Fitting_Settings();
	}

	// reopen fits selector
	if(reopen_Fits_Selector)
	{
		open_Fits_Selector();
	}

	// reopen aperiodic tables
	reopen_Regular_Aperiodics();

	file_Was_Opened_or_Saved = true;
	last_file = filename;

	setWindowTitle(multilayer_Approach_Default_Title + ": .../" + file_Info.dir().dirName() + "/" + file_Info.fileName());

	qInfo() << "opened" << endl;
}

void Multilayer_Approach::open_As()
{
	QFileInfo filename = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Open saved file", last_directory, "Multifitting files " + QString("*.fit") + ";;All files (*.*)"));
	if(!filename.completeBaseName().isEmpty())
	{
		last_file = filename.absoluteFilePath();
		last_directory = filename.absolutePath();

		open(last_file);
	}
}

void Multilayer_Approach::save(QString filename)
{	
	// TODO

	// resaving file from older version
	if(filename == last_file)
	{
		if( (loaded_Version_Major <VERSION_MAJOR) ||
		   ((loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor <VERSION_MINOR)) ||
		   ((loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor==VERSION_MINOR) && (loaded_Version_Build<VERSION_BUILD)) )
		{
			QMessageBox::StandardButton reply = QMessageBox::warning(this,"Resaving old file","Do you want to rewrite old file,\ncreated by v."
								 + Locale.toString(loaded_Version_Major) + "."
								 + Locale.toString(loaded_Version_Minor) + "."
								 + Locale.toString(loaded_Version_Build) + " by new version v."
								 + Locale.toString(VERSION_MAJOR) + "."
								 + Locale.toString(VERSION_MINOR) + "."
								 + Locale.toString(VERSION_BUILD) + "?\n If yes, it cannot be opened in older version.", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
			if (reply == QMessageBox::No) return;
		}
	}

	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);

	// save control string
	out << QString(control_String);

	// save version
	out << VERSION_MAJOR;
	out << VERSION_MINOR;
	out << VERSION_BUILD;

	// save previous id
	++previous_ID;
	out << previous_ID;

	// save number of multilayer tabs
	out << multilayer_Tabs->count();

	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));

		// save tab name
		out << multilayer_Tabs->tabText(i);

		// save tree		
		Global_Variables::serialize_Tree(out, multilayer->structure_Tree->tree);

		/// variables
		{
			// save index of active variables tab
			out << multilayer->variables_Tabs->currentIndex();
		}
		/// independent
		{
			// save number of independent tabs
			out << multilayer->independent_Variables_Plot_Tabs->count();

			// save independent tabs
			for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
			{
				Independent_Variables* independent = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));

				// save plot name
				out << multilayer->independent_Variables_Plot_Tabs->tabText(i);

				// save main data
				out << independent;

				// save plot tree
				Global_Variables::serialize_Tree(out, independent->struct_Tree_Copy);

				// save variables list
				Global_Variables::serialize_Variables_List(out, independent->independent_Variables_List);
			}

			// save index of active independent tab
			out << multilayer->independent_Variables_Plot_Tabs->currentIndex();

			// save calc property for independent plots
			out << multilayer->enable_Calc_Independent_Curves;

			// save number of independent calculation settings rows
			out << multilayer->num_Independent_Rows; // since 1.9.3
		}
		/// target
		{
			// save number of target curves/frames
			out << multilayer->target_Profiles_Vector.size();

			// save target profiles
			for(Target_Curve* target_Curve : multilayer->target_Profiles_Vector)
			{
				out << target_Curve;
			}

			// save calc property for target plots
			out << multilayer->enable_Calc_Target_Curves;

			// save number of calculation settings target rows
			out << multilayer->num_Target_Rows;   // since 1.7.8
		}		
		/// graph options
		out << multilayer->graph_Options;		  // since 1.9.3

		/// profile plot
		out << multilayer->profile_Plot_Options << multilayer->profile_Plot_Data;	  // since 1.10.2

		/// discretization
		out << multilayer->discretization_Parameters;	  // since 1.10.2

		/// fitting settings
		{
			out << fitting_Settings;
		}

		// save min_max values for fitting
		out << multilayer->min_Max_Density;
		out << multilayer->min_Max_Thickness;
		out << multilayer->min_Max_Sigma;		
	}	

	// save index of active multilayer tab
	out << multilayer_Tabs->currentIndex();

	/// save fits
	{
		// save counter
		out << fits_Positive_Counter;

		// save number of fitted structures
		out << fitted_Structures.size();

		// save fitted structures
		for(Fitted_Structure& fitted_Structure : fitted_Structures)
		{
			// save name
			out << fitted_Structure.name;

			// save number of structure trees
			out << fitted_Structure.fitted_Trees.size();

			// save structure trees
			for(QTreeWidget* treee : fitted_Structure.fitted_Trees)
			{
				Global_Variables::serialize_Tree(out, treee);
			}
		}
	}

	/// save plot splitters state
	{
		out << target_Independent_Splitter_Position_Vec;
		out << target_Vertical_Splitter_Position_Vec;
		out << target_Horizontal_Splitter_Position_Vec_Vec;
		out << independent_Vertical_Splitter_Position_Vec;
		out << independent_Horizontal_Splitter_Position_Vec_Vec;
	}

	file.close();
	file_Was_Opened_or_Saved = true;
	last_file = filename;

	QFileInfo file_Info(filename);
	setWindowTitle(multilayer_Approach_Default_Title + ": .../" + file_Info.dir().dirName() + "/" + file_Info.fileName());

	qInfo() << "saved" << endl;
}

void Multilayer_Approach::save_As()
{
	QFileInfo filename = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, "Save as", last_directory, "Multifitting files " + QString("*.fit") + ";;All files (*.*)"));
	if(!filename.completeBaseName().isEmpty())
	{
		QFileInfo filename2;
		if(filename.suffix() != "fit") { filename2 = QFileInfo(filename.absoluteFilePath()+".fit");	}
		else						   { filename2 = QFileInfo(filename.absoluteFilePath());		}

		QString last_file_0 = filename2.absoluteFilePath();
		last_directory = filename2.absolutePath();

		save(last_file_0);
		last_file = last_file_0;
	}
}

void Multilayer_Approach::calc_Reflection(bool silent)
{
	if(!fitting_Settings->in_Calculation)
	{
		// TODO
		if(!silent)
		{
			qInfo() << "\n\n-------------------------------------------------------"
						 "\ncalc specular functions..." //<< temp_Counter++ <<
						 "\n-------------------------------------------------------\n\n";
		}
		Main_Calculation_Module* main_Calculation_Module = new Main_Calculation_Module(CALCULATION);
		main_Calculation_Module->single_Calculation(!silent);
		delete main_Calculation_Module;
	}
}

void Multilayer_Approach::start_Fitting()
{
	if(!fitting_Settings->in_Calculation)
	{
		// TODO
		qInfo() << "\n\n-------------------------------------------------------"
					 "\nfitting..."
					 "\n-------------------------------------------------------\n\n";

		if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
		{
			if(table_Of_Structures->layer_Thickness_Transfer_Is_Created)
			{
				table_Of_Structures->layer_Thickness_Transfer->close();
			}
		}

		Main_Calculation_Module* main_Calculation_Module = new Main_Calculation_Module(FITTING);
		main_Calculation_Module->fitting_and_Confidence();
		delete main_Calculation_Module;

		if(lambda_Out_Of_Range) return;

		main_Calculation_Module = new Main_Calculation_Module(CALCULATION);
		main_Calculation_Module->single_Calculation();
		delete main_Calculation_Module;
	}
}

void Multilayer_Approach::calc_Confidence_Intervals()
{
	if(!fitting_Settings->in_Calculation)
	{
		// TODO
		qInfo() << "\n\n-------------------------------------------------------"
					 "\ncalc confidence intervals..."
					 "\n-------------------------------------------------------\n\n";

		Main_Calculation_Module* main_Calculation_Module = new Main_Calculation_Module(CONFIDENCE);
		main_Calculation_Module->fitting_and_Confidence();
		delete main_Calculation_Module;
	}
}

void Multilayer_Approach::abort_Calculations()
{
	if(fitting_Settings->in_Calculation) {
		fitting_Settings->abort = true;
	}
}

void Multilayer_Approach::calculation_Started()
{
	fitting_Settings->in_Calculation = true;
	if(runned_Fitting_Settings_Editor.contains(fit_Settings_Key))
	{
		fitting_Settings_Editor->abort_Button->setEnabled(fitting_Settings->in_Calculation);
	}

	// menu enabling/disabling
	for(QAction* action : menu->calculate_Menu->actions())	{
		if(action->property(abort_Property).toString() != abort_Property)
		{
			action->setDisabled(true);
		}
	}
	if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
	{
		for(QAction* action : table_Of_Structures->menu->calculate_Menu->actions())
		{
			if(action->property(abort_Property).toString() != abort_Property)		{
				action->setDisabled(true);
			}
		}
	}
}

void Multilayer_Approach::calculation_Finished()
{
	fitting_Settings->abort = false;
	fitting_Settings->in_Calculation = false;
	if(runned_Fitting_Settings_Editor.contains(fit_Settings_Key))
	{
		fitting_Settings_Editor->abort_Button->setEnabled(fitting_Settings->in_Calculation);
	}

	// menu enabling/disabling
	for(QAction* action : menu->calculate_Menu->actions())	{
		action->setDisabled(false);
	}
	if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
	{
		for(QAction* action : table_Of_Structures->menu->calculate_Menu->actions())	{
			action->setDisabled(false);
		}
	}
}

void Multilayer_Approach::reload_Optical_Constants()
{
	if(!fitting_Settings->in_Calculation)
	{
		// TODO
		qInfo() << "\n\n-------------------------------------------------------"
					 "\nreload optical constants..."
					 "\n-------------------------------------------------------\n\n";
		optical_Constants->reload();
	}
}
