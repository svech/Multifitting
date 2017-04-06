// This is a personal academic project. Dear PVS-Studio, please check it.

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
}

void Multilayer_Approach::open_Launcher()
{
	launcher->show();
}

void Multilayer_Approach::closeEvent(QCloseEvent* event)
{
	// TODO
	qApp->quit();
	event;
	emit closed();
}

void Multilayer_Approach::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
		main_Layout->setContentsMargins(4,3,2,3);

	create_Menu();
	create_Multilayer_Tabs();
}

void Multilayer_Approach::create_Menu()
{
	Menu* menu = new Menu(Window_Type::Multilayer_Approach(), this);
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

	connect(add_Tab_Corner_Button,  SIGNAL(clicked()),		  this, SLOT(add_Multilayer()));
	connect(multilayer_Tabs, SIGNAL(tabCloseRequested(int)),  this, SLOT(remove_Multilayer(int)));
	connect(multilayer_Tabs, SIGNAL(currentChanged(int)),	  this, SLOT(change_Tab_Color(int)));
	connect(multilayer_Tabs, SIGNAL(tabBarDoubleClicked(int)),this, SLOT(rename_Multilayer(int)));

	add_Multilayer();
}

void Multilayer_Approach::set_Window_Geometry()
{
	setMinimumWidth(multilayer_min_width);
	setMinimumHeight(multilayer_min_height);
	setGeometry(multilayer_x_corner,multilayer_y_corner,multilayer_width,multilayer_height);
}

// slots

void Multilayer_Approach::add_Multilayer()
{
	Multilayer* new_Multilayer = new Multilayer(this, this);
		new_Multilayer->setContentsMargins(-8,-10,-8,-10);

	connect(new_Multilayer, SIGNAL(refresh_All_Multilayers()),	this,SLOT(refresh_All_Multilayers_View()));

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
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Multilayer \"" + multilayer_Tabs->tabBar()->tabText(index) + "\" will be removed.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
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
		multilayer_Tabs->setTabText(tab_Index, text);
}

void Multilayer_Approach::open_Table_Of_Structures(bool)
{
	// TODO
	if(!runned_Tables_Of_Structures.contains(table_Key))
	{
		table_Of_Structures = new Table_Of_Structures(&runned_Tables_Of_Structures, multilayer_Tabs);
		runned_Tables_Of_Structures.insert(table_Key, table_Of_Structures);
			table_Of_Structures->setWindowFlags(Qt::Window);
			table_Of_Structures->show();
	} else
	{
		runned_Tables_Of_Structures.value(table_Key)->activateWindow();
	}
}

void Multilayer_Approach::refresh_All_Multilayers_View()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = dynamic_cast<Multilayer*>(multilayer_Tabs->widget(i));
		if(multilayer!=sender())
		{
			multilayer->refresh_Structure_And_Independent(sender());
		}
	}
}

void Multilayer_Approach::open()
{
	// TODO
	QFile file("save.fit");
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	int num_Multilayers;
	in >> num_Multilayers;

	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		delete multilayer_Tabs->widget(i);
	}
	multilayer_Tabs->clear();

	for(int i=0; i<num_Multilayers; ++i)
	{		
		// add new multilayer
		add_Multilayer();

		Multilayer* multilayer = dynamic_cast<Multilayer*>(multilayer_Tabs->widget(i));

		// set tab name
		QString multilayer_tab_name;
		in >> multilayer_tab_name;
		multilayer_Tabs->setTabText(multilayer_Tabs->count()-1, multilayer_tab_name);

		// load tree
		Global_Variables::deserialize_Tree(in, multilayer->structure_Tree->tree);

		// save active tab
		int current_Index;
		in >> current_Index;
		multilayer->variables_Tabs->setCurrentIndex(current_Index);

		// load dependent checkboxes state
		bool is_Checked;

		/// standard
			// reflectance
			in >> is_Checked;
			multilayer->reflect_Functions->setChecked(is_Checked);
			// transmittance
			in >> is_Checked;
			multilayer->transmit_Functions->setChecked(is_Checked);
			// transmittance
			in >> is_Checked;
			multilayer->transmit_Functions->setChecked(is_Checked);
		/// field
			// field intensity
			in >> is_Checked;
			multilayer->field_Intensity->setChecked(is_Checked);
			// joule absorption
			in >> is_Checked;
			multilayer->joule_Absorption->setChecked(is_Checked);
		/// user
			in >> is_Checked;
			multilayer->user_Supplied_Functions_Check->setChecked(is_Checked);
			QString user_Text;
			in >> user_Text;
			multilayer->user_Supplied_Functions->setText(user_Text);

		// load number of independent tabs
		int num_Independent;
		in >> num_Independent;

		for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
		{
			delete multilayer->independent_Variables_Plot_Tabs->widget(i);
		}
		multilayer->independent_Variables_Plot_Tabs->clear();

		// load independent tabs
		for(int i=0; i<num_Independent; ++i)
		{
			multilayer->add_Independent_Variables_Tab();
			Independent_Variables* independent = dynamic_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));

			// load plot name
			QString tab_Text;
			in >> tab_Text;
			multilayer->independent_Variables_Plot_Tabs->setTabText(i, tab_Text);

			// load real tree
			independent->real_Struct_Tree = multilayer->structure_Tree->tree;

			// load plot tree
			Global_Variables::deserialize_Tree(in, independent->struct_Tree_Copy);

			// load variables list
			Global_Variables::deserialize_Variables_List(in, independent->independent_Variables_List);
		}

		multilayer->refresh_Structure_And_Independent();

		// disable adding substrate if it already exists
		QStringList whats_This_List = multilayer->structure_Tree->tree->topLevelItem(multilayer->structure_Tree->tree->topLevelItemCount()-1)->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);
		if(whats_This_List[0] == whats_This_Substrate)
			multilayer->structure_Tree->structure_Toolbar->toolbar->actions()[2]->setDisabled(true);
	}
	file.close();
}

void Multilayer_Approach::save()
{
	// TODO
	QFile file("save.fit");
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);

	// save number of multilayers
	out << multilayer_Tabs->count();

	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = dynamic_cast<Multilayer*>(multilayer_Tabs->widget(i));

		// save tab name
		out << multilayer_Tabs->tabText(i);

		// save tree
		Global_Variables::serialize_Tree(out, multilayer->structure_Tree->tree);

		// save active tab
		out << multilayer->variables_Tabs->currentIndex();

		// save dependent checkboxes state
		// specular
		/// standard
			// reflectance
			out <<  multilayer->reflect_Functions->isChecked();
			// transmittance
			out <<  multilayer->transmit_Functions->isChecked();
			// absorptance
			out <<  multilayer->absorp_Functions->isChecked();
		/// field
			// field intensity
			out <<  multilayer->field_Intensity->isChecked();
			// joule absorption
			out <<  multilayer->joule_Absorption->isChecked();
		/// user
			out <<  multilayer->user_Supplied_Functions_Check->isChecked();
			out <<  multilayer->user_Supplied_Functions->text();

		// save number of independent tabs
		out << multilayer->independent_Variables_Plot_Tabs->count();

		// save independent tabs
		for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
		{
			Independent_Variables* independent = dynamic_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));

			// save plot name
			out << multilayer->independent_Variables_Plot_Tabs->tabText(i);

			// save plot tree
			Global_Variables::serialize_Tree(out, independent->struct_Tree_Copy);

			// save variables list
			Global_Variables::serialize_Variables_List(out, independent->independent_Variables_List);
		}
	}
	file.close();
}

void Multilayer_Approach::calc_Reflection()
{
	// TODO
	qInfo() << "\n\n------------------------------------------------------------\ncalc specular functions...\n-------------------------------------------------------\n";
	Main_Calculation_Module* main_Calculation_Module = new Main_Calculation_Module(multilayer_Tabs);
	connect(main_Calculation_Module, SIGNAL(critical(QString)),    this, SLOT(catch_Critical(QString)));
	connect(main_Calculation_Module, SIGNAL(warning(QString)),     this, SLOT(catch_Warning(QString)));
	connect(main_Calculation_Module, SIGNAL(information(QString)), this, SLOT(catch_Information(QString)));
	main_Calculation_Module->run_All();
	delete main_Calculation_Module;
}

void Multilayer_Approach::reload_Optical_Constants()
{
	// TODO
	qInfo() << "\n\n------------------------------------------------------------\nreload optical constants...\n-------------------------------------------------------\n";
	optical_Constants->reload();
}

void Multilayer_Approach::catch_Critical(QString critical_Text)
{
	QMessageBox::critical(this, "Error", critical_Text);
}

void Multilayer_Approach::catch_Warning(QString warning_Text)
{
	QMessageBox::warning(this, "Warning", warning_Text);
}

void Multilayer_Approach::catch_Information(QString information_Text)
{
	QMessageBox::information(this, "Information", information_Text);
}
