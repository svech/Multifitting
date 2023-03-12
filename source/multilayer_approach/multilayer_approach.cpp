#include "multilayer_approach.h"
#include <thread>

struct Old_Independent_Variables
{
	QString tab_Name;
	Data measurement;
	Old_Calculated_Values old_Calculated_Values;
	Calc_Functions calc_Functions;
	Plot_Options plot_Options;
	QString argument_Type;
};
QDataStream& operator >>(QDataStream& stream,		 Old_Independent_Variables old_Independent_Variables )
{
	stream >> old_Independent_Variables.measurement >> old_Independent_Variables.calc_Functions;
	stream >> old_Independent_Variables.old_Calculated_Values;
	stream >> old_Independent_Variables.tab_Name >> old_Independent_Variables.plot_Options;

	if(Global_Variables::check_Loaded_Version(1,7,5))
	{ stream >> old_Independent_Variables.argument_Type ; }

	return stream;
}

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

	connect(multilayer_Tabs->tabBar(),	&QTabBar::customContextMenuRequested,	this, &Multilayer_Approach::tab_Context_Menu);
	connect(add_Tab_Corner_Button,		&QToolButton::clicked,					this, [=]{add_Multilayer();});
	connect(multilayer_Tabs,			&QTabWidget::tabCloseRequested,			this, &Multilayer_Approach::remove_Multilayer);
	connect(multilayer_Tabs,			&QTabWidget::currentChanged,			this, &Multilayer_Approach::change_Tab_Color);
	connect(multilayer_Tabs,			&QTabWidget::tabBarDoubleClicked,		this, &Multilayer_Approach::rename_Multilayer);

	add_Multilayer();

	connect(multilayer_Tabs,		&QTabWidget::currentChanged,	 this, [=]
	{
		if(tab_synchronization)
		{
			if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key)) {table_Of_Structures		  ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))	  {optical_Graphs_2D		  ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))	  {optical_Graphs_1D		  ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Profile_Plots_Window.contains(profile_Plots_Key))		  {profile_Plots_Window		  ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Calculation_Settings_Editor.contains(calc_Settings_Key)){calculation_Settings_Editor->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Roughness_Plots_Window.contains(roughness_Plots_Key))	  {roughness_Plots_Window     ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
			if(runned_Particles_Plots_Window.contains(particles_Plots_Key))	  {particles_Plots_Window     ->main_Tabs->setCurrentIndex(multilayer_Tabs->currentIndex());}
		}
	});
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
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));

		// close independent editors
		for(Independent_Curve_Editor* independent_Curve_Editor : multilayer->runned_Independent_Curve_Editors.values())
		{
			independent_Curve_Editor->close();
		}
		// close target editors
		for(Target_Curve_Editor* target_Curve_Editor : multilayer->runned_Target_Curve_Editors.values())
		{
			target_Curve_Editor->close();
		}
	}
	hide();
	if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key))	{
		table_Of_Structures->close();
	}
	if(runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))	{
		optical_Graphs_2D->close();
	}
	if(runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))	{
		optical_Graphs_1D->close();
	}
	if(runned_Profile_Plots_Window.contains(profile_Plots_Key))	{
		profile_Plots_Window->close();
	}
	if(runned_Roughness_Plots_Window.contains(roughness_Plots_Key))	{
		roughness_Plots_Window->close();
	}
	if(runned_Particles_Plots_Window.contains(particles_Plots_Key))	{
		particles_Plots_Window->close();
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
	if(runned_General_Settings_Editor.contains(general_Settings_Key))	{
		general_Settings_Editor->close();
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

void Multilayer_Approach::duplicate_Structure(const QPoint& pos)
{
	int old_Tab_Index = multilayer_Tabs->tabBar()->tabAt(pos);
	Multilayer* new_Multilayer = add_Multilayer(old_Tab_Index);
	Multilayer* old_Multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(old_Tab_Index));

	*new_Multilayer = *old_Multilayer;	
	new_Multilayer->structure_Tree->tree->expandAll();
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

void Multilayer_Approach::add_Fitted_Structure(QVector<QTreeWidget*>& fitted_Trees_for_Copying, QVector<Imperfections_Model>& imperfections_Models_for_Copying, QString name_Modificator, int run, QString par_Name, double val)
{
	// new instance for storing
	Fitted_Structure new_Fitted_Structure;

	// copy trees
	new_Fitted_Structure.fitted_Trees.resize(fitted_Trees_for_Copying.size());
	new_Fitted_Structure.imperfections_Models.resize(imperfections_Models_for_Copying.size());

	for(int tab_Index=0; tab_Index<fitted_Trees_for_Copying.size(); ++tab_Index)
	{
		/// trees
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

		/// imperfections
		new_Fitted_Structure.imperfections_Models[tab_Index] = imperfections_Models_for_Copying[tab_Index];
	}

	// generate name
	QDateTime date_Time = QDateTime::currentDateTime();
	if( name_Modificator == current_State )	{
		new_Fitted_Structure.name = "# " + Locale.toString(++fits_Positive_Counter) + " state ||  "                                + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss");			}
	if( name_Modificator == before_Fitting_State )	{
		new_Fitted_Structure.name = "# " + Locale.toString(++fits_Positive_Counter) + " fit ||  "                                  + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss") + " || initial";}
	if( name_Modificator == after_Fitting_State )	{
		new_Fitted_Structure.name = "# " + Locale.toString(++fits_Positive_Counter) + " fit ||  "                                  + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss") + " || final";	}
	if( name_Modificator == fit_Run_State )	{
		new_Fitted_Structure.name = "# " + Locale.toString(++fits_Positive_Counter) + " fit run " + Locale.toString(run) + " ||  " + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss");			}
    if( name_Modificator == confidence_Run_State )	{
        new_Fitted_Structure.name = "# " + Locale.toString(++fits_Positive_Counter) + " confidence ||  " + par_Name + " = " + Locale.toString(val) + " ||  " + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss");			}

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

void Multilayer_Approach::open_Profile_Plots(bool profile_Export)
{
	if(!runned_Profile_Plots_Window.contains(profile_Plots_Key))
	{
		runned_Profile_Plots_Window.insert(profile_Plots_Key, profile_Plots_Window);
        profile_Plots_Window = new Profile_Plots_Window(profile_Export);
        profile_Plots_Window->setWindowFlags(Qt::Window);
        if(!profile_Export)
            profile_Plots_Window->show();

		runned_Profile_Plots_Window.clear();
		runned_Profile_Plots_Window.insert(profile_Plots_Key, profile_Plots_Window);
	} else
	{
		profile_Plots_Window->activateWindow();
	}
}

void Multilayer_Approach::open_Optical_Graphs_1D(QString keep_Splitter)
{	
	if(!runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		runned_Optical_Graphs_1D.insert(optical_Graphs_1D_Key, optical_Graphs_1D);
		optical_Graphs_1D = new Optical_Graphs(dim_1D, keep_Splitter);
			optical_Graphs_1D->setWindowFlags(Qt::Window);
			optical_Graphs_1D->show();

		runned_Optical_Graphs_1D.clear();
		runned_Optical_Graphs_1D.insert(optical_Graphs_1D_Key, optical_Graphs_1D);
	} else
	{
		optical_Graphs_1D->activateWindow();
	}
}

void Multilayer_Approach::open_Optical_Graphs_2D(QString keep_Splitter)
{
	if(!runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
	{
		runned_Optical_Graphs_2D.insert(optical_Graphs_2D_Key, optical_Graphs_2D);
		optical_Graphs_2D = new Optical_Graphs(dim_2D, keep_Splitter);
			optical_Graphs_2D->setWindowFlags(Qt::Window);
			optical_Graphs_2D->show();

		runned_Optical_Graphs_2D.clear();
		runned_Optical_Graphs_2D.insert(optical_Graphs_2D_Key, optical_Graphs_2D);
	} else
	{
		optical_Graphs_2D->activateWindow();
	}
}

void Multilayer_Approach::open_Roughness_Plots()
{
	if(!runned_Roughness_Plots_Window.contains(roughness_Plots_Key))
	{
		runned_Roughness_Plots_Window.insert(roughness_Plots_Key, roughness_Plots_Window);

		roughness_Plots_Window = new Roughness_Plots_Window();//(this);
			roughness_Plots_Window->setWindowFlags(Qt::Window);
			roughness_Plots_Window->show();

		runned_Roughness_Plots_Window.clear();
		runned_Roughness_Plots_Window.insert(roughness_Plots_Key, roughness_Plots_Window);
	} else
	{
		roughness_Plots_Window->activateWindow();
	}
}

void Multilayer_Approach::open_Particles_Plots()
{
	if(!runned_Particles_Plots_Window.contains(particles_Plots_Key))
	{
		runned_Particles_Plots_Window.insert(particles_Plots_Key, particles_Plots_Window);

		particles_Plots_Window = new Particles_Plots_Window();//(this);
			particles_Plots_Window->setWindowFlags(Qt::Window);
			particles_Plots_Window->show();

		runned_Particles_Plots_Window.clear();
		runned_Particles_Plots_Window.insert(particles_Plots_Key, particles_Plots_Window);
	} else
	{
		particles_Plots_Window->activateWindow();
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
	if(!runned_General_Settings_Editor.contains(general_Settings_Key))
	{
		runned_General_Settings_Editor.insert(general_Settings_Key, general_Settings_Editor);
		general_Settings_Editor = new General_Settings_Editor;
			general_Settings_Editor->setWindowFlags(Qt::Window);
			general_Settings_Editor->show();

		runned_General_Settings_Editor.clear();
		runned_General_Settings_Editor.insert(general_Settings_Key, general_Settings_Editor);
	} else
	{
		general_Settings_Editor->activateWindow();
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

void Multilayer_Approach::reopen_Table_Of_Structures(bool keep_Tab_Index)
{
	if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
	{
		int active_Tab;
		if(keep_Tab_Index) active_Tab = runned_Tables_Of_Structures.value(table_Of_Structures_Key)->main_Tabs->currentIndex();
		runned_Tables_Of_Structures.value(table_Of_Structures_Key)->close();
		open_Table_Of_Structures();
		if(keep_Tab_Index) runned_Tables_Of_Structures.value(table_Of_Structures_Key)->main_Tabs->setCurrentIndex(active_Tab);
	}
}

void Multilayer_Approach::reopen_Profile_Plots(bool keep_Tab_Index)
{
	if(runned_Profile_Plots_Window.contains(profile_Plots_Key))
	{
		int active_Tab;
		if(keep_Tab_Index) active_Tab = runned_Profile_Plots_Window.value(profile_Plots_Key)->main_Tabs->currentIndex();
		runned_Profile_Plots_Window.value(profile_Plots_Key)->close();
		open_Profile_Plots();
		if(keep_Tab_Index) runned_Profile_Plots_Window.value(profile_Plots_Key)->main_Tabs->setCurrentIndex(active_Tab);
	}
}

void Multilayer_Approach::reopen_Roughness_Plots(bool keep_Tab_Index)
{
	if(runned_Roughness_Plots_Window.contains(roughness_Plots_Key))
	{
		int active_Tab;
		if(keep_Tab_Index) active_Tab = runned_Roughness_Plots_Window.value(roughness_Plots_Key)->main_Tabs->currentIndex();
		runned_Roughness_Plots_Window.value(roughness_Plots_Key)->close();
		open_Roughness_Plots();
		if(keep_Tab_Index) runned_Roughness_Plots_Window.value(roughness_Plots_Key)->main_Tabs->setCurrentIndex(active_Tab);
	}
}

void Multilayer_Approach::reopen_Particles_Plots(bool keep_Tab_Index)
{
	if(runned_Particles_Plots_Window.contains(particles_Plots_Key))
	{
		int active_Tab;
		if(keep_Tab_Index) active_Tab = runned_Particles_Plots_Window.value(particles_Plots_Key)->main_Tabs->currentIndex();
		runned_Particles_Plots_Window.value(particles_Plots_Key)->close();
		open_Particles_Plots();
		if(keep_Tab_Index) runned_Particles_Plots_Window.value(particles_Plots_Key)->main_Tabs->setCurrentIndex(active_Tab);
	}
}

void Multilayer_Approach::reopen_Optical_Graphs_1D(bool keep_Tab_Index, QString keep_Splitter)
{
	if(runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		int active_Tab;
		if(keep_Tab_Index) active_Tab = runned_Optical_Graphs_1D.value(optical_Graphs_1D_Key)->main_Tabs->currentIndex();
		runned_Optical_Graphs_1D.value(optical_Graphs_1D_Key)->close();
		open_Optical_Graphs_1D(keep_Splitter);
		if(keep_Tab_Index) runned_Optical_Graphs_1D.value(optical_Graphs_1D_Key)->main_Tabs->setCurrentIndex(active_Tab);
	}
}

void Multilayer_Approach::reopen_Optical_Graphs_2D(bool keep_Tab_Index, QString keep_Splitter)
{
	if(runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
	{
		int active_Tab;
		if(keep_Tab_Index) active_Tab = runned_Optical_Graphs_2D.value(optical_Graphs_2D_Key)->main_Tabs->currentIndex();
		runned_Optical_Graphs_2D.value(optical_Graphs_2D_Key)->close();
		open_Optical_Graphs_2D(keep_Splitter);
		if(keep_Tab_Index) runned_Optical_Graphs_2D.value(optical_Graphs_2D_Key)->main_Tabs->setCurrentIndex(active_Tab);
	}
}

void Multilayer_Approach::reopen_Calculation_Settings(bool keep_Tab_Index)
{
	if(runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
	{
		int active_Tab;
		if(keep_Tab_Index) active_Tab = runned_Calculation_Settings_Editor.value(calc_Settings_Key)->main_Tabs->currentIndex();
		runned_Calculation_Settings_Editor.value(calc_Settings_Key)->close();
		open_Calculation_Settings();
		if(keep_Tab_Index) runned_Calculation_Settings_Editor.value(calc_Settings_Key)->main_Tabs->setCurrentIndex(active_Tab);
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
	for(Regular_Aperiodic_Table* regular_Aperiodic_Table: runned_Regular_Aperiodic_Tables_List)
	{
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
		   runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key) ||
		   runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key) ||
		   runned_Profile_Plots_Window.contains(profile_Plots_Key) ||
		   runned_Roughness_Plots_Window.contains(roughness_Plots_Key) ||
		   runned_Particles_Plots_Window.contains(particles_Plots_Key) ||
		   runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(true);
			multilayer_Tabs->setMovable(false);
			multilayer_Tabs->cornerWidget()->setDisabled(true);
			disconnect(multilayer_Tabs->tabBar(), &QTabBar::customContextMenuRequested,  this, &Multilayer_Approach::tab_Context_Menu);
		}

		// lock tree
		if(runned_Tables_Of_Structures.contains(table_Of_Structures_Key)/*||
		   runned_Profile_Plots_Window.contains(profile_Plots_Key) ||
		   runned_Roughness_Plots_Window.contains(roughness_Plots_Key)*/)
		{
			// close editors
			for(Item_Editor* editor : multilayer->structure_Tree->list_Editors)	editor->close();

			multilayer->structure_Tree->structure_Toolbar->toolbar->setDisabled(true);
			multilayer->structure_Tree->tree->blockSignals(true);
		}

		// lock independent tabs
		if( runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key) ||
			runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key) ||
			runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
//			for(int i=0; i<multilayer->independent_Curve_Tabs->count(); ++i)
//			{
//				multilayer->independent_Curve_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(true);
//			}
			multilayer->independent_Curve_Tabs->setMovable(false);
//			multilayer->independent_Variables_Corner_Button->setDisabled(true);
		}

		// lock target buttons
//		if( runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key) ||
//			runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key) ||
//			runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
//		{
//			for(int i=0; i<multilayer->data_Target_Profile_Frame_Vector.size(); ++i)
//			{
//				multilayer->add_Buttons_To_Lock[i]->setDisabled(true);
//				multilayer->remove_Buttons_To_Lock[i]->setDisabled(true);
//			}
//		}
	}
}

void Multilayer_Approach::unlock_Mainwindow_Interface()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));

		// unlock multilayer tabs
		if(!runned_Tables_Of_Structures.contains(table_Of_Structures_Key) &&
		   !runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key) &&
		   !runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key) &&
		   !runned_Profile_Plots_Window.contains(profile_Plots_Key) &&
		   !runned_Roughness_Plots_Window.contains(roughness_Plots_Key) &&
		   !runned_Particles_Plots_Window.contains(particles_Plots_Key) &&
		   !runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(false);
			multilayer_Tabs->setMovable(true);
			add_Tab_Corner_Button->setDisabled(false);
			disconnect(multilayer_Tabs->tabBar(), &QTabBar::customContextMenuRequested,  this, &Multilayer_Approach::tab_Context_Menu);
			   connect(multilayer_Tabs->tabBar(), &QTabBar::customContextMenuRequested,  this, &Multilayer_Approach::tab_Context_Menu);
		}

		// unlock tree
		if(!runned_Tables_Of_Structures.contains(table_Of_Structures_Key)/* &&
		   !runned_Profile_Plots_Window.contains(profile_Plots_Key) &&
		   !runned_Roughness_Plots_Window.contains(roughness_Plots_Key)*/)
		{
			multilayer->structure_Tree->structure_Toolbar->toolbar->setDisabled(false);
			multilayer->structure_Tree->tree->blockSignals(false);
		}

		// unlock independent tabs
		if( !runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key) &&
			!runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key) &&
			!runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
//			for(int i=0; i<multilayer->independent_Curve_Tabs->count(); ++i)
//			{
//				multilayer->independent_Curve_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(false);
//			}
			multilayer->independent_Curve_Tabs->setMovable(true);
//			multilayer->independent_Variables_Corner_Button->setDisabled(false);
		}

		// unlock target buttons
//		if( !runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key) &&
//			!runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key) &&
//			!runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
//		{
//				for(int i=0; i<multilayer->data_Target_Profile_Frame_Vector.size(); ++i)
//			{
//				multilayer->add_Buttons_To_Lock[i]->setDisabled(false);
//				multilayer->remove_Buttons_To_Lock[i]->setDisabled(false);
//			}
//		}
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
				preliminary_last_file = filename.absoluteFilePath();
				preliminary_last_directory = filename.absolutePath();

				open(preliminary_last_file);
			}
		}
		++counter;
	}
}

void Multilayer_Approach::open(QString filename)
{
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
	// only here approve last file
//	last_file = preliminary_last_file;
//	last_directory = preliminary_last_directory;
	QFileInfo info(file);
	last_file = info.absoluteFilePath();
	last_directory = info.absolutePath();

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

	// close graphs 1D
	bool reopen_Graphs_1D = runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key);
	int active_Tab_Optical_Graphs_1D = -2019;
	if(reopen_Graphs_1D)
	{
		active_Tab_Optical_Graphs_1D = runned_Optical_Graphs_1D.value(optical_Graphs_1D_Key)->main_Tabs->currentIndex();
		runned_Optical_Graphs_1D.value(optical_Graphs_1D_Key)->close();
	}

	// close graphs 2D
	bool reopen_Graphs_2D = runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key);
	int active_Tab_Optical_Graphs_2D = -2019;
	if(reopen_Graphs_2D)
	{
		active_Tab_Optical_Graphs_2D = runned_Optical_Graphs_2D.value(optical_Graphs_2D_Key)->main_Tabs->currentIndex();
		runned_Optical_Graphs_2D.value(optical_Graphs_2D_Key)->close();
	}

	// close profile
	bool reopen_Profile_Plots = runned_Profile_Plots_Window.contains(profile_Plots_Key);
	int active_Tab_Profile_Plots = -2019;
	if(reopen_Profile_Plots)
	{
		active_Tab_Profile_Plots = runned_Profile_Plots_Window.value(profile_Plots_Key)->main_Tabs->currentIndex();
		runned_Profile_Plots_Window.value(profile_Plots_Key)->close();
	}

	// close roughness
	bool reopen_Roughness_Plots = runned_Roughness_Plots_Window.contains(roughness_Plots_Key);
	int active_Tab_Roughness_Plots = -2019;
	if(reopen_Roughness_Plots)
	{
		active_Tab_Roughness_Plots = runned_Roughness_Plots_Window.value(roughness_Plots_Key)->main_Tabs->currentIndex();
		runned_Roughness_Plots_Window.value(roughness_Plots_Key)->close();
	}

	// close particles
	bool reopen_Particles_Plots = runned_Particles_Plots_Window.contains(particles_Plots_Key);
	int active_Tab_Particles_Plots = -2019;
	if(reopen_Particles_Plots)
	{
		active_Tab_Particles_Plots = runned_Particles_Plots_Window.value(particles_Plots_Key)->main_Tabs->currentIndex();
		runned_Particles_Plots_Window.value(particles_Plots_Key)->close();
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

	// close independent editors
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));
		for(Independent_Curve_Editor* independent_Curve_Editor : multilayer->runned_Independent_Curve_Editors.values())
		{
			independent_Curve_Editor->close();
		}
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

        // if old file has no substrate
        Global_Variables::add_Substrate(multilayer->structure_Tree->tree);

		if(!Global_Variables::check_Loaded_Version(1,11,0))
		{
			int current_Variable_Tab_Index;
			in >> current_Variable_Tab_Index;
		}
		/// independent
		{
			// clear existing independent tabs
			for(int i=0; i<multilayer->independent_Curve_Tabs->count(); ++i)
			{
				delete multilayer->independent_Curve_Tabs->widget(i);
			}
			multilayer->independent_Curve_Tabs->clear();

			// read number of independent tabs
			int num_Independent;
			in >> num_Independent;

			// load independent tabs
			for(int i=0; i<num_Independent; ++i)
			{
				multilayer->add_Independent_Variables_Tab();

				// load plot name
				QString tab_Text;
				in >> tab_Text;
				multilayer->independent_Curve_Tabs->setTabText(i, tab_Text);

				if(!Global_Variables::check_Loaded_Version(1,11,0))
				{
					Old_Independent_Variables old_independent;
					in >> old_independent;
					Global_Variables::pseudo_Deserialize_Tree(in);
					Global_Variables::pseudo_Deserialize_Variables_List(in);
				} else
				{
					Independent_Curve* independent = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(i));

					// load main data
					in >> independent;
				}
			}

			// load index of active independent tab
			int current_Independent_Tab_Index;
			in >> current_Independent_Tab_Index;
			multilayer->independent_Curve_Tabs->setCurrentIndex(current_Independent_Tab_Index);

			// load calc property for independent plots
			in >> multilayer->enable_Calc_Independent_Curves;

			// load number of calculation settings independent rows
			if(Global_Variables::check_Loaded_Version(1,9,3))
			{in >> multilayer->num_Independent_Rows;}

			// load number of independent graph rows
			if(Global_Variables::check_Loaded_Version(1,7,8) && !Global_Variables::check_Loaded_Version(1,9,3))
			{in >> multilayer->graph_Options_1D.num_Independent_Graph_Rows;}	// since 1.7.8 and before 1.9.3
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
			{in >> multilayer->graph_Options_1D.num_Target_Graph_Rows;}	// since 1.7.8 and before 1.9.3
		}

		/// graph options
		if(Global_Variables::check_Loaded_Version(1,9,3))
		{in >> multilayer->graph_Options_1D;}	            // since 1.9.3

		/// profile plot
		if(Global_Variables::check_Loaded_Version(1,10,2))
		{in >> multilayer->profile_Plot_Options;}			// since 1.10.2

		/// roughness plot
		if(Global_Variables::check_Loaded_Version(1,11,18))
		{in >> multilayer->roughness_Plot_Options;}			// since 1.11.18

		/// particles plot
		if(Global_Variables::check_Loaded_Version(1,11,23))
		{in >> multilayer->particles_Plot_Options;}			// since 1.11.23

		/// discretization
		if(Global_Variables::check_Loaded_Version(1,10,2))
		{in >> multilayer->discretization_Parameters;}		// since 1.10.2

		/// roughness and other
		if(Global_Variables::check_Loaded_Version(1,11,0))
		{in >> multilayer->imperfections_Model;}			// since 1.11.0
		else
		{
			for(int ii=0; ii<multilayer->imperfections_Model.use_Func.size(); ii++)
			{multilayer->imperfections_Model.use_Func[ii] = true;}
		}

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
			fitted_Structure.imperfections_Models.clear();

			fitted_Structure.fitted_Trees.resize(num_Trees);
			fitted_Structure.imperfections_Models.resize(num_Trees);

			// load structure trees
			for(int tree_Index=0;  tree_Index<num_Trees; ++tree_Index)
			{
				// trees
				fitted_Structure.fitted_Trees[tree_Index] = new QTreeWidget(this);
				Global_Variables::deserialize_Tree(in, fitted_Structure.fitted_Trees[tree_Index]);

                // if old file has no substrate
                Global_Variables::add_Substrate(fitted_Structure.fitted_Trees[tree_Index]);

				// imperfections
				if(Global_Variables::check_Loaded_Version(1,11,6))
				{in >> fitted_Structure.imperfections_Models[tree_Index];}	// since 1.11.6
			}
		}
	}

	/// load plot splitters state
	if(Global_Variables::check_Loaded_Version(1,9,5))
	{
		in >> target_Independent_Splitter_Position_1D_Vec;
		in >> target_Vertical_Splitter_Position_1D_Vec;
		in >> target_Horizontal_Splitter_Position_1D_Vec_Vec;
		in >> independent_Vertical_Splitter_Position_1D_Vec;
		in >> independent_Horizontal_Splitter_Position_1D_Vec_Vec;
	}
	if(Global_Variables::check_Loaded_Version(1,11,0))
	{
		in >> target_Independent_Splitter_Position_2D_Vec;
		in >> target_Vertical_Splitter_Position_2D_Vec;
		in >> target_Horizontal_Splitter_Position_2D_Vec_Vec;
		in >> independent_Vertical_Splitter_Position_2D_Vec;
		in >> independent_Horizontal_Splitter_Position_2D_Vec_Vec;
	}

	file.close();

	global_Multilayer_Approach->calculate(true); // to obtain calculated curves that are not loaded

	// resizing
	if(max_Num_Targets>=2 && height()<=multilayer_height)
	{
		resize(width(), multilayer_height /*+ (max_Num_Targets-1)*multilayer_height_additive*/);
	}

	// reopen table of structures
	if(reopen_Table)
	{
		open_Table_Of_Structures();
	}

	// reopen calculation settings
	if(reopen_Calc_Settings)
	{
		open_Calculation_Settings();
	}

	// reopen 1D graphs
	if(reopen_Graphs_1D)
	{
		open_Optical_Graphs_1D();
	}

	// reopen 2D graphs
	if(reopen_Graphs_2D)
	{
		open_Optical_Graphs_2D();
	}

	// reopen profile
	if(reopen_Profile_Plots)
	{
		open_Profile_Plots();
	}

	// reopen roughness
	if(reopen_Roughness_Plots)
	{
		open_Roughness_Plots();
	}

	// reopen particles
	if(reopen_Particles_Plots)
	{
		open_Particles_Plots();
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
    QString directory_To_Open = Global_Variables::working_Directory();

	QFileInfo filename = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Open saved file", directory_To_Open, "Multifitting files " + QString("*.fit") + ";;All files (*.*)"));
	if(!filename.completeBaseName().isEmpty())
	{	
		preliminary_last_file = filename.absoluteFilePath();
		preliminary_last_directory = filename.absolutePath();

		open(preliminary_last_file);
    }
}

void Multilayer_Approach::open_Action()
{
    if(file_Was_Opened_or_Saved || open_last_file) {
        open(last_file);
    } else {
        open(Global_Variables::working_Directory() + "/" + default_File);
    }
}

void Multilayer_Approach::save(QString filename)
{	
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

	// mark as new version now
	loaded_Version_Major = VERSION_MAJOR;
	loaded_Version_Minor = VERSION_MINOR;
	loaded_Version_Build = VERSION_BUILD;

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

		/// independent
		{
			// save number of independent tabs
			out << multilayer->independent_Curve_Tabs->count();

			// save independent tabs
			for(int i=0; i<multilayer->independent_Curve_Tabs->count(); ++i)
			{
				// save plot name
				out << multilayer->independent_Curve_Tabs->tabText(i);

				Independent_Curve* independent = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(i));

				// save main data
				out << independent;
			}

			// save index of active independent tab
			out << multilayer->independent_Curve_Tabs->currentIndex();

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
		out << multilayer->graph_Options_1D;		  // since 1.9.3

		/// profile plot
		out << multilayer->profile_Plot_Options;	  // since 1.10.2

		/// roughness plot
		out << multilayer->roughness_Plot_Options;		// since 1.11.18

		/// particles plot
		out << multilayer->particles_Plot_Options;		// since 1.11.23

		/// discretization
		out << multilayer->discretization_Parameters;	  // since 1.10.2

		/// roughness and other
		out << multilayer->imperfections_Model;	  // since 1.11.0

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
			for(int tree_Index =0; tree_Index<fitted_Structure.fitted_Trees.size(); tree_Index++)
			{
				// trees
				Global_Variables::serialize_Tree(out, fitted_Structure.fitted_Trees[tree_Index]);

				// imperfections
				out << fitted_Structure.imperfections_Models[tree_Index];
			}
		}
	}

	/// save plot splitters state
	{
		// 1D
		if(runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			runned_Optical_Graphs_1D.value(optical_Graphs_1D_Key)->save_Geometry();
		}
		out << target_Independent_Splitter_Position_1D_Vec;
		out << target_Vertical_Splitter_Position_1D_Vec;
		out << target_Horizontal_Splitter_Position_1D_Vec_Vec;
		out << independent_Vertical_Splitter_Position_1D_Vec;
		out << independent_Horizontal_Splitter_Position_1D_Vec_Vec;

		// 2D
		if(runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
		{
			runned_Optical_Graphs_2D.value(optical_Graphs_2D_Key)->save_Geometry();
		}
		out << target_Independent_Splitter_Position_2D_Vec;
		out << target_Vertical_Splitter_Position_2D_Vec;
		out << target_Horizontal_Splitter_Position_2D_Vec_Vec;
		out << independent_Vertical_Splitter_Position_2D_Vec;
		out << independent_Horizontal_Splitter_Position_2D_Vec_Vec;
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
    QString directory_To_Save = Global_Variables::working_Directory();

	QFileInfo filename = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, "Save as", directory_To_Save, "Multifitting files " + QString("*.fit") + ";;All files (*.*)"));
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

void Multilayer_Approach::save_Action()
{
    if(file_Was_Opened_or_Saved) {
        save(last_file);
    } else {
        save(Global_Variables::working_Directory() + "/" + default_File);
    }
}

void Multilayer_Approach::calculate(bool silent)
{
	if(!fitting_Settings->in_Calculation)
	{
		Main_Calculation_Module* main_Calculation_Module = new Main_Calculation_Module(CALCULATION);
		main_Calculation_Module->single_Calculation(!silent);
		delete main_Calculation_Module;
	}
}

void Multilayer_Approach::global_Recalculate(bool silent)
{
	if(recalculate_spinbox_global)
	{
		calculate(silent);
	}
}

void Multilayer_Approach::recalculate_From_Table(bool silent)
{
	if(recalculate_spinbox_structure_table)
	{
		global_Multilayer_Approach->calculate(silent);
	} else
	{
		Global_Variables::plot_All_Data_in_Profiles();
		Global_Variables::plot_All_Data_in_Roughness();
		Global_Variables::plot_All_Data_in_Particles();
	}
}

void Multilayer_Approach::start_Fitting()
{
	if(!fitting_Settings->in_Calculation)
	{
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
		qInfo() << "\n\n-------------------------------------------------------"
					 "\nreload optical constants..."
					 "\n-------------------------------------------------------\n\n";
		optical_Constants->reload();
    }
}

void Multilayer_Approach::export_Structures()
{
    for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
    {
        Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
        multilayer->structure_Tree->structure_Toolbar->export_Structure();
    }
}

void Multilayer_Approach::export_Curves()
{
    bool state_Print_1D = print_1D_Data_On_Recalculation;
    bool state_Print_2D = print_2D_Data_On_Recalculation;
    bool state_Print_1D_PSD = print_1D_PSD_From_Scattering_On_Recalculation;

    print_1D_Data_On_Recalculation = true;
    print_2D_Data_On_Recalculation = true;
    print_1D_PSD_From_Scattering_On_Recalculation = false; // true

    calculate();

    print_1D_Data_On_Recalculation = state_Print_1D;
    print_2D_Data_On_Recalculation = state_Print_2D;
    print_1D_PSD_From_Scattering_On_Recalculation = state_Print_1D_PSD;
}

void Multilayer_Approach::export_Profiles()
{
    bool opened_Especially = false;
    if(!runned_Profile_Plots_Window.contains(profile_Plots_Key)) {
        open_Profile_Plots(true);
        opened_Especially = true;
    }

    for(Profile_Plot* profile_Plot : profile_Plots_Window->profile_Plot_Vector) {
        profile_Plot->export_Profile();
    }

    if(opened_Especially)
        profile_Plots_Window->close();
}
