// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "multilayer.h"

Multilayer::Multilayer(Multilayer_Approach* multilayer_Approach, QWidget *parent) :
	multilayer_Approach(multilayer_Approach),
	QWidget(parent)
{
	create_Main_Layout();
}

void Multilayer::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);

	create_Structure_Frame();
	create_Variables_Frame();
	create_Data_Frame();
}

void Multilayer::create_Structure_Frame()
{
	struct_Frame_Layout = new QVBoxLayout;
		struct_Frame_Layout->setSpacing(0);

	struct_Frame = new QFrame;
        struct_Frame->setContentsMargins(-7,-3,-7,-10);
		struct_Frame->setLayout(struct_Frame_Layout);

	structure_Tree = new Structure_Tree(this, this);
	main_Layout->addWidget(struct_Frame);
}

void Multilayer::create_Variables_Frame()
{
	variables_Frame = new QFrame;
		variables_Frame->setContentsMargins(-7,-8,-9,-10);
	main_Layout->addWidget(variables_Frame);

	variables_Frame_Layout = new QVBoxLayout(variables_Frame);
		variables_Frame_Layout->setSpacing(0);

	create_Variables_Tabs();
		variables_Frame_Layout->addWidget(variables_Tabs);
}

void Multilayer::create_Variables_Tabs()
{
	variables_Tabs = new QTabWidget(this);

	create_Dependent_Variables_Tabs();
	{
		QFrame* frame = new QFrame;
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(dependent_Variables_Tabs);
		frame->setLayout(layout);
		frame->setContentsMargins(0,-5,0,-8);
		variables_Tabs->addTab(frame, "Dependent Variables");
	}

	create_Independent_Variables_Tabs();
	{
		QFrame* frame = new QFrame;
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(independent_Variables_Plot_Tabs);
		layout->setSpacing(0);
		frame->setLayout(layout);
		frame->setContentsMargins(0,-5,0,-8);
		variables_Tabs->addTab(frame, "Independent Variables");
	}

	{
		QFrame* frame = new QFrame;
		QVBoxLayout* layout = new QVBoxLayout;
		structure_Table_Button = new QPushButton("Open structure table");
		layout->addWidget(structure_Table_Button);
		frame->setLayout(layout);
		frame->setContentsMargins(0,-5,0,-8);
		variables_Tabs->addTab(frame, "Structure Table");

		connect(structure_Table_Button, SIGNAL(clicked(bool)), multilayer_Approach, SLOT(open_Table_Of_Structures(bool)));
	}

	variables_Tabs->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
}

void Multilayer::create_Dependent_Variables_Tabs()
{
	dependent_Variables_Tabs = new QTabWidget(this);

	specular_Functions_Layout = new QVBoxLayout;
		specular_Functions_Layout->setSpacing(0);
	create_Specular_Functions();
		specular_Functions_Layout->addWidget(standard_Functions_Group_Box);
		specular_Functions_Layout->addWidget(field_Functions_Group_Box);
		specular_Functions_Layout->addWidget(user_Functions_Group_Box);

	QFrame* specular_Frame = new QFrame(this);
		specular_Frame->setContentsMargins(-5,-5,-5,-9);
		specular_Frame->setLayout(specular_Functions_Layout);
	dependent_Variables_Tabs->addTab(specular_Frame, "Specular Optical Functions");

	// TODO non-specular functions
	nonspecular_Functions_Layout = new QVBoxLayout;
		nonspecular_Functions_Layout->setSpacing(0);
	create_Nonspecular_Functions();
//		nonspecular_Functions_Layout->addWidget(nonspecular_Group_Box);
	QFrame* nonspecular_Frame = new QFrame(this);
//		nonspecular_Frame->setContentsMargins(-5,-5,-5,-9);
		nonspecular_Frame->setLayout(nonspecular_Functions_Layout);
	dependent_Variables_Tabs->addTab(nonspecular_Frame, "Scattering Functions");
}

void Multilayer::create_Specular_Functions()
{
	standard_Functions_Group_Box = new QGroupBox("Standard Functions", this);
		standard_Functions_Group_Box->setContentsMargins(0,8,0,-4);

	QHBoxLayout* standard_Functions_Layout = new QHBoxLayout(standard_Functions_Group_Box);
		standard_Functions_Layout->setAlignment(Qt::AlignLeft);
		reflect_Functions = new QCheckBox("Reflectance", this);
			standard_Functions_Layout->addWidget(reflect_Functions);
		transmit_Functions = new QCheckBox("Transmittance", this);
			standard_Functions_Layout->addWidget(transmit_Functions);
		absorp_Functions = new QCheckBox("Absorptance", this);
			standard_Functions_Layout->addWidget(absorp_Functions);

	field_Functions_Group_Box = new QGroupBox("Field Functions", this);
		field_Functions_Group_Box->setContentsMargins(0,8,0,-4);

	QHBoxLayout* field_Functions_Layout = new QHBoxLayout(field_Functions_Group_Box);
		field_Functions_Layout->setAlignment(Qt::AlignLeft);
		field_Intensity = new QCheckBox("Intensity", this);
			field_Functions_Layout->addWidget(field_Intensity);
		joule_Absorption= new QCheckBox("Joule Absorption", this);
			field_Functions_Layout->addWidget(joule_Absorption);

	user_Functions_Group_Box = new QGroupBox("User-defined Functions");
		user_Functions_Group_Box->setContentsMargins(0,8,0,-4);

	QHBoxLayout* user_Functions_Layout = new QHBoxLayout(user_Functions_Group_Box);
		user_Functions_Layout->setAlignment(Qt::AlignLeft);
		user_Supplied_Functions_Check = new QCheckBox(this);
			user_Functions_Layout->addWidget(user_Supplied_Functions_Check);
		user_Supplied_Functions = new QLineEdit(this);
			user_Functions_Layout->addWidget(user_Supplied_Functions);
}

void Multilayer::create_Nonspecular_Functions()
{
	// TODO nonspecular functions
}

void Multilayer::create_Independent_Variables_Tabs()
{
	independent_Variables_Plot_Tabs = new QTabWidget;
		independent_Variables_Plot_Tabs->setMovable(true);
		independent_Variables_Plot_Tabs->setTabsClosable(true);

	independent_Variables_Corner_Button = new QToolButton;
		independent_Variables_Corner_Button->setText("+");
	QFont tmp_Qf; tmp_Qf.setBold(true);
		independent_Variables_Corner_Button->setFont(tmp_Qf);
		independent_Variables_Plot_Tabs->setCornerWidget(independent_Variables_Corner_Button);

	connect(independent_Variables_Corner_Button,SIGNAL(clicked()),				 this, SLOT(add_Independent_Variables_Tab()));
	connect(independent_Variables_Plot_Tabs,	SIGNAL(currentChanged(int)),	 this, SLOT(change_Tab_Independent_Variables_Tab_Color(int)));
	connect(independent_Variables_Plot_Tabs,	SIGNAL(tabCloseRequested(int)),  this, SLOT(remove_Independent_Variables_Tab(int)));
	connect(independent_Variables_Plot_Tabs,	SIGNAL(tabBarDoubleClicked(int)),this, SLOT(rename_Independent_Variables_Tab(int)));

	add_Independent_Variables_Tab();
}

void Multilayer::create_Data_Frame()
{
	data_Frame = new QFrame;
		data_Frame->setContentsMargins(-8,-8,-8,-7);
	main_Layout->addWidget(data_Frame);

	data_Frame_Layout = new QVBoxLayout(data_Frame);
		data_Frame_Layout->setSpacing(0);

	data_Measured_Data_Group_Box = new QGroupBox("Measured Data");
	data_Target_Profile_Group_Box= new QGroupBox("Target Profile");

	data_Frame_Layout->addWidget(data_Measured_Data_Group_Box);
	data_Frame_Layout->addWidget(data_Target_Profile_Group_Box);

	layout_Measured_Data_With_Frame_Vector = new QVBoxLayout(data_Measured_Data_Group_Box);
	layout_Target_Profile_With_Frame_Vector= new QVBoxLayout(data_Target_Profile_Group_Box);

	layout_Measured_Data_With_Frame_Vector->setSpacing(0);
	layout_Target_Profile_With_Frame_Vector->setSpacing(0);

	layout_Measured_Data_With_Frame_Vector->setMargin(0);
	layout_Target_Profile_With_Frame_Vector->setMargin(0);

	add_Target_Curve(0, MEASURED);
	add_Target_Curve(0, OPTIMIZE);
}

// --------------------------------------------------------------------------------------

void Multilayer::add_Independent_Variables_Tab()
{
	Independent_Variables* new_Independent = new Independent_Variables(structure_Tree->tree, this);
		new_Independent->setContentsMargins(-8,-10,-8,-10);

	independent_Variables_Vector.append(new_Independent);

	connect(new_Independent, SIGNAL(refresh_Multilayer()), this, SLOT(refresh_Structure_And_Independent()));

	// add new tab
	independent_Variables_Plot_Tabs->addTab(new_Independent, default_independent_variable_tab_name);
	independent_Variables_Plot_Tabs->setTabText(independent_Variables_Plot_Tabs->count()-1, default_independent_variable_tab_name+QString::number(independent_Variables_Plot_Tabs->count()));

	if(independent_Variables_Plot_Tabs->count()>1)
	{
		independent_Variables_Plot_Tabs->tabBar()->setTabTextColor(independent_Variables_Plot_Tabs->count()-1,Qt::gray);
		independent_Variables_Plot_Tabs->tabBar()->tabButton(independent_Variables_Plot_Tabs->count()-1, QTabBar::RightSide)->hide();
	}
}

void Multilayer::change_Tab_Independent_Variables_Tab_Color(int index)
{
	independent_Variables_Plot_Tabs->tabBar()->setTabTextColor(index,Qt::black);
	if(independent_Variables_Plot_Tabs->tabBar()->tabButton(index, QTabBar::RightSide))
	   independent_Variables_Plot_Tabs->tabBar()->tabButton(index, QTabBar::RightSide)->show();

	for(int i = 0; i<independent_Variables_Plot_Tabs->tabBar()->count(); i++)
	{
		if(i!=index)
		{
			independent_Variables_Plot_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->hide();
			independent_Variables_Plot_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	}
}

void Multilayer::remove_Independent_Variables_Tab(int index)
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Variables tab \"" + independent_Variables_Plot_Tabs->tabBar()->tabText(index) + "\" will be removed.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		independent_Variables_Vector.remove(index);
		delete independent_Variables_Plot_Tabs->widget(index);
		if(independent_Variables_Plot_Tabs->count()==0) add_Independent_Variables_Tab();
	}
}

void Multilayer::rename_Independent_Variables_Tab(int tab_Index)
{
	bool ok;
	QString text = QInputDialog::getText(this, "Rename plot", "New name", QLineEdit::Normal, independent_Variables_Plot_Tabs->tabText(tab_Index), &ok);
	if (ok && !text.isEmpty())
		independent_Variables_Plot_Tabs->setTabText(tab_Index, text);
}

void Multilayer::refresh_Structure_And_Independent(QObject* my_Sender)
{
	structure_Tree->refresh__StructureTree__Data_and_Text();
	structure_Tree->editors_Edit(sender());

	for(int i=0; i<independent_Variables_Plot_Tabs->count(); ++i)
	{
		Independent_Variables* independent = dynamic_cast<Independent_Variables*>(independent_Variables_Plot_Tabs->widget(i));
		if(independent!=sender())
		{
			independent->reset_Independent_Variables_Structure();
		}
	}

	// refresh other multilayers
	if(!my_Sender) emit refresh_All_Multilayers();
}

// --------------------------------------------------------------------------------------

void Multilayer::add_Target_Curve(int index_Pressed, QString target_Curve_Type)
{
	// window resizing
	if( (target_Curve_Type == MEASURED && !data_Measured_Data_Frame_Vector.isEmpty()) ||
		(target_Curve_Type == OPTIMIZE && !data_Target_Profile_Frame_Vector.isEmpty())		)
	{
		struct_Frame->setFixedSize(struct_Frame->size());
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() + multilayer_height_additive);
	}


	QPushButton* new_Import_Button = new QPushButton("Import");
	QLabel* new_Description_Label  = new QLabel("<no description>");
	QPushButton* new_Add_Button    = new QPushButton("Add Row");
	QPushButton* new_Remove_Button = new QPushButton("Remove");

	QFrame* new_Frame = new QFrame;
	Target_Curve* new_Target_Curve = new Target_Curve(new_Description_Label, structure_Tree->tree);

	// -----
	// TEMPORARY
	new_Target_Curve->open_Window();
	// ------

	if(target_Curve_Type == MEASURED)
	{
		data_Measured_Data_Frame_Vector.insert(index_Pressed, new_Frame);
		measured_Data_Vector.insert(index_Pressed, new_Target_Curve);
	} else
	if(target_Curve_Type == OPTIMIZE)
	{
		data_Target_Profile_Frame_Vector.insert(index_Pressed,new_Frame);
		target_Profiles_Vector.insert(index_Pressed, new_Target_Curve);
	}else	{qInfo() << "Multilayer::add_Target_Curve  :  target_Curve_Type should be \"MEASURED\" or \"OPTIMIZE\"";}

	QHBoxLayout* new_Frame_Layout = new QHBoxLayout(new_Frame);
	new_Frame_Layout->setMargin(0);
	QHBoxLayout* left_Layout  = new QHBoxLayout;
	QHBoxLayout* right_Layout = new QHBoxLayout;
	left_Layout->setSpacing(10);
	right_Layout->setSpacing(0);

//	if(target_Curve_Type == MEASURED) {new_Description_Label->setText(new_Description_Label->text() + " " + QString::number(measured_Counter)); measured_Counter++;} else
//	if(target_Curve_Type == OPTIMIZE) {new_Description_Label->setText(new_Description_Label->text() + " " + QString::number(target_Counter));   target_Counter++; }

	new_Import_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Description_Label->	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Add_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Remove_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	left_Layout->addWidget(new_Import_Button);
	left_Layout->addWidget(new_Description_Label, 0, Qt::AlignLeft);
	right_Layout->addWidget(new_Add_Button);
	right_Layout->addWidget(new_Remove_Button);
	new_Frame_Layout->addLayout(left_Layout, Qt::AlignLeft);
	new_Frame_Layout->addLayout(right_Layout, Qt::AlignRight);

	new_Frame->setLayout(new_Frame_Layout);

	if(target_Curve_Type == MEASURED) layout_Measured_Data_With_Frame_Vector->insertWidget(index_Pressed,new_Frame); else
	if(target_Curve_Type == OPTIMIZE) layout_Target_Profile_With_Frame_Vector->insertWidget(index_Pressed,new_Frame);

	connect(new_Import_Button,    &QPushButton::clicked, this, [=, this]{ open_Import_Window(new_Target_Curve); });

	if(target_Curve_Type == MEASURED)
	{
		connect(new_Add_Button,    &QPushButton::clicked, this, [=, this]{ add_Target_Curve   (data_Measured_Data_Frame_Vector.indexOf(new_Frame)+1, target_Curve_Type); });
		connect(new_Remove_Button, &QPushButton::clicked, this, [=, this]{ remove_Target_Curve(data_Measured_Data_Frame_Vector.indexOf(new_Frame),   target_Curve_Type); });
	} else
	if(target_Curve_Type == OPTIMIZE)
	{
		connect(new_Add_Button,    &QPushButton::clicked, this, [=, this]{ add_Target_Curve   (data_Target_Profile_Frame_Vector.indexOf(new_Frame)+1, target_Curve_Type); });
		connect(new_Remove_Button, &QPushButton::clicked, this, [=, this]{ remove_Target_Curve(data_Target_Profile_Frame_Vector.indexOf(new_Frame),   target_Curve_Type); });
	}

	// hiding add button
//	QPushButton* add_Button;
//	if(data_Measured_Data_Frame_Vector.size()>1)
//	{
//		add_Button = data_Measured_Data_Frame_Vector[data_Measured_Data_Frame_Vector.size()-2]->findChild<QPushButton*>(new_Add_Button->objectName());
//		add_Button->hide();
//	}
}

void Multilayer::remove_Target_Curve(int index_Pressed, QString target_Curve_Type)
{
	if(target_Curve_Type == MEASURED)
	{
		// delete frame
		delete data_Measured_Data_Frame_Vector[index_Pressed];
		data_Measured_Data_Frame_Vector.removeAt(index_Pressed);

		// delete data
		delete measured_Data_Vector[index_Pressed];
		measured_Data_Vector.removeAt(index_Pressed);
	} else
	if(target_Curve_Type == OPTIMIZE)
	{
		// delete frame
		delete data_Target_Profile_Frame_Vector[index_Pressed];
		data_Target_Profile_Frame_Vector.removeAt(index_Pressed);

		// delete data
		delete target_Profiles_Vector[index_Pressed];
		target_Profiles_Vector.removeAt(index_Pressed);
	}

	// window resizing
	if( (target_Curve_Type == MEASURED && !data_Measured_Data_Frame_Vector.isEmpty())  ||
		(target_Curve_Type == OPTIMIZE && !data_Target_Profile_Frame_Vector.isEmpty()) )
	{
		struct_Frame->setFixedSize(struct_Frame->size());
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() - multilayer_height_additive);
	}

	if(target_Curve_Type == MEASURED && data_Measured_Data_Frame_Vector.isEmpty())	add_Target_Curve(0, MEASURED);
	if(target_Curve_Type == OPTIMIZE && data_Target_Profile_Frame_Vector.isEmpty())	add_Target_Curve(0, OPTIMIZE);
}

void Multilayer::open_Import_Window(Target_Curve* target_Curve)
{
	Target_Curve_Editor* new_Target_Curve_Editor = new Target_Curve_Editor(target_Curve, this);
		new_Target_Curve_Editor->setParent(this);
		new_Target_Curve_Editor->setWindowFlags(Qt::Window);
		new_Target_Curve_Editor->show();

//	connect(depth_Grading, SIGNAL(grading_Edited()), this, SLOT(emit_Item_Data_Edited()));
}

