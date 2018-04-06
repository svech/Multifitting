// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "multilayer_approach.h"

Multilayer_Approach::Multilayer_Approach(Launcher* launcher, QWidget *parent) :
	launcher(launcher),
	QWidget(parent)
{
	setWindowTitle("Multilayer Model");
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
	// TODO
	launcher->runned_Multilayer_Approaches.remove(multilayer_Approach_Key);
	qApp->quit();
	event->accept();
	emit closed();
}


void Multilayer_Approach::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
		main_Layout->setContentsMargins(4,3,2,3);

	create_Multilayer_Tabs();
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

	add_Tab_Corner_Button = new QToolButton;
		add_Tab_Corner_Button->setText("+");
	QFont tmp_Qf; tmp_Qf.setBold(true);
		add_Tab_Corner_Button->setFont(tmp_Qf);
	multilayer_Tabs->setCornerWidget(add_Tab_Corner_Button);
	main_Layout->addWidget(multilayer_Tabs);

	connect(add_Tab_Corner_Button,  &QToolButton::clicked,			 this, &Multilayer_Approach::add_Multilayer);
	connect(multilayer_Tabs,		&QTabWidget::tabCloseRequested,  this, &Multilayer_Approach::remove_Multilayer);
	connect(multilayer_Tabs,		&QTabWidget::currentChanged,	 this, &Multilayer_Approach::change_Tab_Color);
	connect(multilayer_Tabs,		&QTabWidget::tabBarDoubleClicked,this, &Multilayer_Approach::rename_Multilayer);

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

void Multilayer_Approach::add_Multilayer()
{
	Multilayer* new_Multilayer = new Multilayer(this);
		new_Multilayer->setContentsMargins(-80,-10,-80,-10);

	connect(new_Multilayer, &Multilayer::refresh_All_Multilayers, this, &Multilayer_Approach::refresh_All_Multilayers_View);

	multilayer_Tabs->addTab(new_Multilayer, default_multilayer_tab_name);
	multilayer_Tabs->setTabText(multilayer_Tabs->count()-1, default_multilayer_tab_name + QString::number(multilayer_Tabs->count()));

	if(multilayer_Tabs->count()>1)
	{
		multilayer_Tabs->tabBar()->setTabTextColor(multilayer_Tabs->count()-1,Qt::gray);
		multilayer_Tabs->tabBar()->tabButton(multilayer_Tabs->count()-1, QTabBar::RightSide)->hide();
	}
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
		new_Fitted_Structure.name = "# " + QString::number(++fits_Positive_Counter) + " state ||  "                                + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss");	}
	if( name_Modificator == fitted_State )	{
		new_Fitted_Structure.name = "# " + QString::number(++fits_Positive_Counter) + " fit ||  "                                  + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss");	}
	if( name_Modificator == fit_Run_State )	{
		new_Fitted_Structure.name = "# " + QString::number(++fits_Positive_Counter) + " fit run " + QString::number(run) + " ||  " + date_Time.toString("dd.MM.yyyy  ||  hh:mm:ss");	}

	// put new instance to storage
	fitted_Structures.append(new_Fitted_Structure);

	if(runned_Fits_Selectors.contains(fits_Selector_Key))
	{
		fits_Selector->add_Item(new_Fitted_Structure);
	}
}

void Multilayer_Approach::open_Table_Of_Structures()
{
	if(!runned_Tables_Of_Structures.contains(table_Key))
	{
		table_Of_Structures = new Table_Of_Structures;//(this);
		runned_Tables_Of_Structures.insert(table_Key, table_Of_Structures);
			table_Of_Structures->setWindowFlags(Qt::Window);
			table_Of_Structures->show();

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

void Multilayer_Approach::open_Optical_Graphs()
{
	if(!runned_Optical_Graphs.contains(optical_Graphs_Key))
	{
		optical_Graphs = new Optical_Graphs;//(this);
		runned_Optical_Graphs.insert(optical_Graphs_Key, optical_Graphs);
			optical_Graphs->setWindowFlags(Qt::Window);
			optical_Graphs->show();

//		for(int i=0; i<multilayer_Tabs->count(); ++i)
//		{
//			Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));
//			connect(multilayer, &Multilayer::refresh_All_Multilayers, table_Of_Structures, [=]{table_Of_Structures->reload_All_Widgets();});
//		}
	} else
	{
		optical_Graphs->activateWindow();
	}
}

void Multilayer_Approach::open_Fits_Selector()
{
	if(!runned_Fits_Selectors.contains(fits_Selector_Key))
	{
		fits_Selector = new Fits_Selector;//(this);
		runned_Fits_Selectors.insert(fits_Selector_Key, fits_Selector);
			fits_Selector->setWindowFlags(Qt::Window);
			fits_Selector->show();
	} else
	{
		fits_Selector->activateWindow();
	}
}

void Multilayer_Approach::open_Calculation_Settings()
{
	if(!runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
	{
		calculation_Settings_Editor = new Calculation_Settings_Editor;//(this);
		runned_Calculation_Settings_Editor.insert(calc_Settings_Key, calculation_Settings_Editor);
			calculation_Settings_Editor->setWindowFlags(Qt::Window);
			calculation_Settings_Editor->show();
	} else
	{
		calculation_Settings_Editor->activateWindow();
	}
}

void Multilayer_Approach::open_Fitting_Settings()
{
	if(!runned_Fitting_Settings_Editor.contains(fit_Settings_Key))
	{
		fitting_Settings_Editor = new Fitting_Settings_Editor;//(this);
		runned_Fitting_Settings_Editor.insert(fit_Settings_Key, fitting_Settings_Editor);
			fitting_Settings_Editor->setWindowFlags(Qt::Window);
			fitting_Settings_Editor->show();
	} else
	{
		fitting_Settings_Editor->activateWindow();
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
			QMessageBox::StandardButton reply = QMessageBox::question(nullptr,"Open", "Open file " + filename.fileName() + " ?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
			if (reply == QMessageBox::Yes)
			{
				open(filename.absoluteFilePath());
			}
		}
		++counter;
	}
}

void Multilayer_Approach::open(QString filename)
{
	// TODO
	int max_Num_Targets=0;

	// check extension
	QFileInfo file_Info(filename);
	if(file_Info.suffix() != file_Extension)
	{
		QMessageBox::information(nullptr, "Wrong file type", "Only \"." + QString(file_Extension) + "\" files can be opened");
		return;
	}

	// check filepath
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(nullptr, "File not found", "Nothing to open");
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

	if((loaded_Version_Major <VERSION_MAJOR) ||
	   (loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor <VERSION_MINOR) ||
	   (loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor==VERSION_MINOR) && (loaded_Version_Build<VERSION_BUILD) )
	{
		QMessageBox::StandardButton reply = QMessageBox::warning(this,"Opening old file","Do you want to open file,\ncreated by v."
							 + QString::number(loaded_Version_Major) + "."
							 + QString::number(loaded_Version_Minor) + "."
							 + QString::number(loaded_Version_Build) + " in v."
							 + QString::number(VERSION_MAJOR) + "."
							 + QString::number(VERSION_MINOR) + "."
							 + QString::number(VERSION_BUILD) + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::No) return;
	}
	if((loaded_Version_Major >VERSION_MAJOR) ||
	   (loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor >VERSION_MINOR) ||
	   (loaded_Version_Major==VERSION_MAJOR) && (loaded_Version_Minor==VERSION_MINOR) && (loaded_Version_Build>VERSION_BUILD) )
	{
		QMessageBox::warning(this,"Opening old file","File, created by newer version "
							 + QString::number(loaded_Version_Major) + "."
							 + QString::number(loaded_Version_Minor) + "."
							 + QString::number(loaded_Version_Build) + "\ncan't be opened in "
							 + QString::number(VERSION_MAJOR) + "."
							 + QString::number(VERSION_MINOR) + "."
							 + QString::number(VERSION_BUILD));
	}

	// close table of structures
	bool reopen_Table = runned_Tables_Of_Structures.contains(table_Key);
	if(reopen_Table)
	{
		runned_Tables_Of_Structures.value(table_Key)->close();
	}

	// close table of structures
	bool reopen_Calc_Settings = runned_Calculation_Settings_Editor.contains(calc_Settings_Key);
	if(reopen_Calc_Settings)
	{
		runned_Calculation_Settings_Editor.value(calc_Settings_Key)->close();
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
				target_Curve->set_Text_To_Label();
			}

			// load calc property for target plots
			in >> multilayer->enable_Calc_Target_Curves;
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
	file.close();

	// resizing
	if(max_Num_Targets>=2 && height()<=multilayer_height)
	{
		resize(width(), multilayer_height + (max_Num_Targets-1)*multilayer_height_additive);
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

	// reopen fits selector
	if(reopen_Fits_Selector)
	{
		open_Fits_Selector();
	}

	qInfo() << "opened";
}

void Multilayer_Approach::save(QString filename)
{
	// TODO
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

		}
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

	file.close();

	qInfo() << "saved";
}

void Multilayer_Approach::calc_Reflection()
{
	// TODO
	qInfo() << "\n\n-------------------------------------------------------"
				 "\ncalc specular functions..."
				 "\n-------------------------------------------------------\n";
	Main_Calculation_Module* main_Calculation_Module = new Main_Calculation_Module(CALCULATION);
	main_Calculation_Module->single_Calculation();
	delete main_Calculation_Module;
}

void Multilayer_Approach::start_Fitting()
{
	// TODO
	qInfo() << "\n\n-------------------------------------------------------"
				 "\nfitting..."
				 "\n-------------------------------------------------------\n";
	Main_Calculation_Module* main_Calculation_Module = new Main_Calculation_Module(FITTING);
	main_Calculation_Module->fitting();
	delete main_Calculation_Module;

	if(lambda_Out_Of_Range) return;

	main_Calculation_Module = new Main_Calculation_Module(CALCULATION);
	main_Calculation_Module->single_Calculation();
	delete main_Calculation_Module;
}

void Multilayer_Approach::reload_Optical_Constants()
{
	// TODO
	qInfo() << "\n\n-------------------------------------------------------"
				 "\nreload optical constants..."
				 "\n-------------------------------------------------------\n";
	optical_Constants->reload();
}
