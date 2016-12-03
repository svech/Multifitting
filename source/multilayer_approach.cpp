#include "multilayer_approach.h"

Multilayer_Approach::Multilayer_Approach(QWidget *parent) :
	QWidget(parent)
{
	setWindowTitle("Multilayer Model");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
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
	Multilayer* new_Multilayer = new Multilayer(this);
		new_Multilayer->setContentsMargins(-8,-10,-8,-10);

	connect(new_Multilayer, SIGNAL(refresh()),	this,SLOT(refresh_All_Multilayers_View()));

	multilayer_Tabs->addTab(new_Multilayer, default_multilayer_tab_name);
	multilayer_Tabs->setTabText(multilayer_Tabs->count()-1, default_multilayer_tab_name + QString::number(multilayer_Tabs->count()));

	if(multilayer_Tabs->count()>1)
	{
		multilayer_Tabs->tabBar()->setTabTextColor(multilayer_Tabs->count()-1,Qt::gray);
		multilayer_Tabs->tabBar()->tabButton(multilayer_Tabs->count()-1, QTabBar::RightSide)->hide();
	}
	multilayer_List.append(new_Multilayer);
}

void Multilayer_Approach::remove_Multilayer(int index)
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Multilayer \"" + multilayer_Tabs->tabBar()->tabText(index) + "\" will be removed.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		Multilayer* temp_Multilayer;
		for(int i=0; i<multilayer_List.size(); i++)
		{
			if(multilayer_List[i] == multilayer_Tabs->widget(index))
			{
				temp_Multilayer = multilayer_List[i];
				multilayer_List.removeAt(i);
			}
		}
		delete temp_Multilayer;
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

void Multilayer_Approach::refresh_All_Multilayers_View()
{
	for(int i=0; i<multilayer_List.size(); ++i)
	{
		multilayer_List[i]->refresh_Text();
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

	for(int i=0; i<multilayer_List.size(); ++i)
	{
		delete multilayer_List[i];
	}
	multilayer_List.clear();
	multilayer_List.reserve(num_Multilayers);

	for(int i=0; i<num_Multilayers; ++i)
	{
		add_Multilayer();
		Global_Variables::deserialize_Tree(in, multilayer_List[i]->structure_Tree->tree);
		multilayer_List[i]->refresh_State();
	}
	file.close();
}

void Multilayer_Approach::save()
{
	// TODO
	QFile file("save.fit");
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << multilayer_List.size();

	for(int i=0; i<multilayer_List.size(); ++i)
	{
		Global_Variables::serialize_Tree(out, multilayer_List[i]->structure_Tree->tree);
	}
	file.close();
}

void Multilayer_Approach::calc_Reflection()
{
	// TODO
	qInfo() << "\n\n------------------------------------------------------------\ncalc specular functions...\n-------------------------------------------------------\n";
	Main_Calculation_Module* main_Calculation_Module = new Main_Calculation_Module(multilayer_List);
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
