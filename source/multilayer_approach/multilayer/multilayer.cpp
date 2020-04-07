#include "multilayer.h"

Multilayer::Multilayer(QWidget *parent) :
	parent(parent),
	num_Target_Rows(1),
	num_Independent_Rows(1),
	QWidget(parent)
{
	// initialization. later it will be loaded from .fit
	profile_Plot_Options.local_length_units = length_units;
	profile_Plot_Options.local_wavelength_units = wavelength_units;

	create_Main_Layout();
}

Multilayer::~Multilayer()
{
}

void Multilayer::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(4,4,4,4);

	create_Splitter();
	create_Structure_Frame();
	create_Variables_Frame();
	create_Data_Frame();
}

void Multilayer::create_Splitter()
{
	multilayer_Splitter = new QSplitter;
	multilayer_Splitter->setOrientation(Qt::Vertical);
	multilayer_Splitter->setStyleSheet("QSplitter::handle{border: 0px none; background: none;}");
	main_Layout->addWidget(multilayer_Splitter);

	top_Widget_Split = new QWidget;
	top_Widget_Layout = new QVBoxLayout(top_Widget_Split);
		top_Widget_Layout->setSpacing(0);
		top_Widget_Layout->setContentsMargins(0,0,0,0);
	multilayer_Splitter->addWidget(top_Widget_Split);

	bottom_Widget_Split = new QWidget;
	bottom_Widget_Layout = new QVBoxLayout(bottom_Widget_Split);
		bottom_Widget_Layout->setSpacing(0);
		bottom_Widget_Layout->setContentsMargins(0,0,0,0);
	multilayer_Splitter->addWidget(bottom_Widget_Split);
}

void Multilayer::create_Structure_Frame()
{
	structure_Tree = new Structure_Tree(this, this);
	top_Widget_Layout->addWidget(structure_Tree->tree);
	top_Widget_Layout->addWidget(structure_Tree->structure_Toolbar->toolbar);
}

void Multilayer::create_Variables_Frame()
{
	create_Variables_Tabs();
	bottom_Widget_Layout->addWidget(variables_Tabs);
}

void Multilayer::create_Variables_Tabs()
{
	variables_Tabs = new QTabWidget(this);

	create_Independent_Variables_Tabs();
	{
		variables_Tabs->addTab(independent_Variables_Plot_Tabs, "Independent Variables");
	}
	{
		QWidget* frame = new QWidget;
		QGridLayout* layout = new QGridLayout;
			layout->setAlignment(Qt::AlignVCenter);

		structure_Table_Button = new QPushButton("Structure table");
			layout->addWidget(structure_Table_Button,0,0);
		roughness_Table_Button = new QPushButton("Roughness table");
			layout->addWidget(roughness_Table_Button,0,1);

		optical_Graphs_Button = new QPushButton("Graphs");
			layout->addWidget(optical_Graphs_Button,1,0);
		profile_Plots_Button = new QPushButton("Profile plot");
			layout->addWidget(profile_Plots_Button,1,1);

		calculation_Settings_Button = new QPushButton("Calculation settings");
			layout->addWidget(calculation_Settings_Button,2,0);
		general_Settings_Button = new QPushButton("General settings");
			layout->addWidget(general_Settings_Button,2,1);

		fitting_Settings_Button = new QPushButton("Fitting settings");
			layout->addWidget(fitting_Settings_Button,3,0);
		fits_Selector_Button = new QPushButton("Fits selector");
			layout->addWidget(fits_Selector_Button,3,1);

		frame->setLayout(layout);
		frame->setContentsMargins(0,0,0,0);
		variables_Tabs->addTab(frame, "Main Tools");

		// TODO
		roughness_Table_Button->setDisabled(true);
		general_Settings_Button->setDisabled(true);

		Multilayer_Approach* parent_Multilayer_Approach = qobject_cast<Multilayer_Approach*>(parent);
		connect(structure_Table_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Table_Of_Structures);
		connect(roughness_Table_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Table_Of_Roughness);

		connect(optical_Graphs_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, [=]{parent_Multilayer_Approach->open_Optical_Graphs();});
		connect(profile_Plots_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, [=]{parent_Multilayer_Approach->open_Profile_Plots();});

		connect(calculation_Settings_Button, &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Calculation_Settings);
		connect(general_Settings_Button,	 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_General_Settings);

		connect(fits_Selector_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Fits_Selector);
		connect(fitting_Settings_Button,	 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Fitting_Settings);
	}
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

	connect(independent_Variables_Corner_Button,&QToolButton::clicked,			 this, &Multilayer::add_Independent_Variables_Tab);
	connect(independent_Variables_Plot_Tabs,	&QTabWidget::currentChanged,	 this, &Multilayer::change_Tab_Independent_Variables_Tab_Color);
	connect(independent_Variables_Plot_Tabs,	&QTabWidget::tabCloseRequested,  this, &Multilayer::remove_Independent_Variables_Tab);
	connect(independent_Variables_Plot_Tabs,	&QTabWidget::tabBarDoubleClicked,this, &Multilayer::rename_Independent_Variables_Tab);

	add_Independent_Variables_Tab();
}

void Multilayer::create_Data_Frame()
{
	data_Target_Profile_Group_Box = new QGroupBox("Target Curve");
	bottom_Widget_Layout->addWidget(data_Target_Profile_Group_Box);

	layout_Target_Profile_With_Frame_Vector = new QVBoxLayout(data_Target_Profile_Group_Box);
	layout_Target_Profile_With_Frame_Vector->setSpacing(0);
	layout_Target_Profile_With_Frame_Vector->setMargin(0);

	add_Target_Curve(0);
}

//// --------------------------------------------------------------------------------------

void Multilayer::add_Independent_Variables_Tab()
{
	Independent_Variables* new_Independent = new Independent_Variables(structure_Tree->tree, this);
		new_Independent->setContentsMargins(-8,-10,-8,-10);

	connect(new_Independent, &Independent_Variables::refresh_Multilayer, this, [=]{refresh_Structure_And_Independent();});

	// add new tab
	independent_Variables_Plot_Tabs->addTab(new_Independent, default_independent_variable_tab_name);
	new_Independent->tab_Name = default_independent_variable_tab_name+Locale.toString(independent_Variables_Plot_Tabs->count());
	independent_Variables_Plot_Tabs->setTabText(independent_Variables_Plot_Tabs->count()-1, new_Independent->tab_Name + new_Independent->enlarge_Tab_Name());

	if(independent_Variables_Plot_Tabs->count()>1)
	{
		independent_Variables_Plot_Tabs->tabBar()->setTabTextColor(independent_Variables_Plot_Tabs->count()-1,Qt::gray);
		independent_Variables_Plot_Tabs->tabBar()->tabButton(independent_Variables_Plot_Tabs->count()-1, QTabBar::RightSide)->hide();
	}

	qobject_cast<Multilayer_Approach*>(parent)->independent_Added = true;
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
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Variables tab \"" + independent_Variables_Plot_Tabs->tabBar()->tabText(index)
															  + "\" will be removed.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if (reply == QMessageBox::Yes)
	{
		delete independent_Variables_Plot_Tabs->widget(index);
		if(independent_Variables_Plot_Tabs->count()==0) add_Independent_Variables_Tab();
	}
}

void Multilayer::rename_Independent_Variables_Tab(int tab_Index)
{
	bool ok;
	Independent_Variables* independent = qobject_cast<Independent_Variables*>(independent_Variables_Plot_Tabs->widget(tab_Index));

	QString text = QInputDialog::getText(this, "Rename plot", "New name", QLineEdit::Normal, independent->tab_Name, &ok);
	if (ok && !text.isEmpty())
	{
		independent->tab_Name = text;
		independent_Variables_Plot_Tabs->setTabText(tab_Index, independent->tab_Name + independent->enlarge_Tab_Name());
	}
}

void Multilayer::refresh_Structure_And_Independent(QObject* my_Sender)
{
	structure_Tree->refresh__StructureTree__Data_and_Text();
	structure_Tree->editors_Edit(sender());
	for(int i=0; i<independent_Variables_Plot_Tabs->count(); ++i)
	{
		Independent_Variables* independent = qobject_cast<Independent_Variables*>(independent_Variables_Plot_Tabs->widget(i));
		if(independent!=sender())
		{
			independent->reset_Independent_Variables_Structure();
			independent->refresh_Text();
		}
	}

	// refresh other multilayers
	if(!my_Sender) {emit refresh_All_Multilayers();}
}

//// --------------------------------------------------------------------------------------

void Multilayer::add_Target_Curve(int index_Pressed, bool opening)
{
	// window resizing
	if(!data_Target_Profile_Frame_Vector.isEmpty())
	{
//#ifdef __linux__
//		data_Target_Profile_Group_Box->adjustSize();
//#endif
		if(!opening)
		{
			QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() + data_Target_Profile_Frame_Vector.first()->height());
		}
	}

	QPushButton* new_Import_Button = new QPushButton("Import");
	QLabel* new_Description_Label  = new QLabel("<no description>");
	QPushButton* new_Add_Button    = new QPushButton("Add Row");
	QPushButton* new_Remove_Button = new QPushButton("Remove");

	QFrame* new_Frame = new QFrame;
	Target_Curve* new_Target_Curve = new Target_Curve(new_Description_Label, structure_Tree->tree);

	add_Buttons_To_Lock.insert(index_Pressed,new_Add_Button);
	remove_Buttons_To_Lock.insert(index_Pressed,new_Remove_Button);
	data_Target_Profile_Frame_Vector.insert(index_Pressed,new_Frame);
	target_Profiles_Vector.insert(index_Pressed, new_Target_Curve);

	QHBoxLayout* new_Frame_Layout = new QHBoxLayout(new_Frame);
		new_Frame_Layout->setMargin(0);
		new_Frame_Layout->setSpacing(0);
	QHBoxLayout* left_Layout  = new QHBoxLayout;
	QHBoxLayout* right_Layout = new QHBoxLayout;
	left_Layout->setSpacing(10);
	right_Layout->setSpacing(0);

//	{new_Description_Label->setText(new_Description_Label->text() + " " + Locale.toString(target_Counter));   target_Counter++; }

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

	layout_Target_Profile_With_Frame_Vector->insertWidget(index_Pressed,new_Frame);

	connect(new_Import_Button, &QPushButton::clicked, this, [=]{ open_Import_Window (new_Target_Curve); });
	connect(new_Add_Button,    &QPushButton::clicked, this, [=]{ add_Target_Curve   (data_Target_Profile_Frame_Vector.indexOf(new_Frame)+1); });
	connect(new_Remove_Button, &QPushButton::clicked, this, [=]{ remove_Target_Curve(data_Target_Profile_Frame_Vector.indexOf(new_Frame)  ); });

	// hiding add button
//	QPushButton* add_Button;
//	if(data_Measured_Data_Frame_Vector.size()>1)
//	{
//		add_Button = data_Measured_Data_Frame_Vector[data_Measured_Data_Frame_Vector.size()-2]->findChild<QPushButton*>(new_Add_Button->objectName());
//		add_Button->hide();
//	}
}

void Multilayer::remove_Target_Curve(int index_Pressed, bool clean)
{
	// delete frame
	delete data_Target_Profile_Frame_Vector[index_Pressed];
	data_Target_Profile_Frame_Vector.removeAt(index_Pressed);
	add_Buttons_To_Lock.removeAt(index_Pressed);
	remove_Buttons_To_Lock.removeAt(index_Pressed);

	// delete data
	delete target_Profiles_Vector[index_Pressed];
	target_Profiles_Vector.removeAt(index_Pressed);

	// window resizing
	if( !data_Target_Profile_Frame_Vector.isEmpty() )
	{
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height()      - data_Target_Profile_Frame_Vector.first()->height());
	}

	if(data_Target_Profile_Frame_Vector.isEmpty() && !clean)	add_Target_Curve(0);

	set_Index_To_Target_Curves();
}

void Multilayer::open_Import_Window(Target_Curve* target_Curve)
{
	if(runned_Target_Curve_Editors.contains(target_Curve))
	{
		runned_Target_Curve_Editors.value(target_Curve)->activateWindow();
	}
	if(!runned_Target_Curve_Editors.contains(target_Curve))
	{		
		if(target_Curve->measurement.measurement_Type == no_Measurement_Type)
		{
			QDialog* choice_Data_Type_Window = new QDialog(this);
				choice_Data_Type_Window->setWindowTitle("Data");
				choice_Data_Type_Window->setWindowModality(Qt::ApplicationModal);
				choice_Data_Type_Window->setAttribute(Qt::WA_DeleteOnClose);
				choice_Data_Type_Window->setWindowFlags(Qt::Tool);
				choice_Data_Type_Window->show();

			QVBoxLayout* choice_Data_Type_Layout = new QVBoxLayout(choice_Data_Type_Window);
				choice_Data_Type_Layout->setSpacing(5);
				choice_Data_Type_Layout->setContentsMargins(5,5,5,5);

			// settings group box
			QGroupBox*  choice_Data_Type_Group_Box = new QGroupBox;
				choice_Data_Type_Group_Box->setObjectName("choice_Data_Type_Group_Box");
				choice_Data_Type_Group_Box->setStyleSheet("QGroupBox#choice_Data_Type_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 0ex;}"
															"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
			choice_Data_Type_Layout->addWidget(choice_Data_Type_Group_Box);

			QVBoxLayout* choice_Data_Type_Group_Box_Layout = new QVBoxLayout(choice_Data_Type_Group_Box);
				choice_Data_Type_Group_Box_Layout->setSpacing(5);
				choice_Data_Type_Group_Box_Layout->setContentsMargins(5,5,5,5);

			// buttons
			QPushButton* specular_Button = new QPushButton(measurement_Types[Specular_Scan]);
			connect(specular_Button,  &QPushButton::clicked, this, [=]{target_Curve->measurement.measurement_Type = measurement_Types[Specular_Scan]; choice_Data_Type_Window->close(); open_Import_Window(target_Curve);});
				choice_Data_Type_Group_Box_Layout->addWidget(specular_Button);
			QPushButton* detector_Button = new QPushButton(measurement_Types[Detector_Scan]);
			connect(detector_Button,  &QPushButton::clicked, this, [=]{target_Curve->measurement.measurement_Type = measurement_Types[Detector_Scan]; choice_Data_Type_Window->close(); open_Import_Window(target_Curve);});
				choice_Data_Type_Group_Box_Layout->addWidget(detector_Button);
			QPushButton* rocking_Button  = new QPushButton(measurement_Types[Rocking_Curve]);
			connect(rocking_Button,   &QPushButton::clicked, this, [=]{target_Curve->measurement.measurement_Type = measurement_Types[Rocking_Curve]; choice_Data_Type_Window->close(); open_Import_Window(target_Curve);});
				choice_Data_Type_Group_Box_Layout->addWidget(rocking_Button);
			QPushButton* offset_Button   = new QPushButton(measurement_Types[Offset_Scan]);
			connect(offset_Button,    &QPushButton::clicked, this, [=]{target_Curve->measurement.measurement_Type = measurement_Types[Offset_Scan];   choice_Data_Type_Window->close(); open_Import_Window(target_Curve);});
				choice_Data_Type_Group_Box_Layout->addWidget(offset_Button);
			QPushButton* gisas_Button    = new QPushButton(measurement_Types[GISAS]);
			connect(gisas_Button,     &QPushButton::clicked, this, [=]{target_Curve->measurement.measurement_Type = measurement_Types[GISAS];		  choice_Data_Type_Window->close(); open_Import_Window(target_Curve);});
				choice_Data_Type_Group_Box_Layout->addWidget(gisas_Button);

			choice_Data_Type_Window->adjustSize();
			choice_Data_Type_Window->setFixedSize(choice_Data_Type_Window->size());
		} else
		{
			Target_Curve_Editor* new_Target_Curve_Editor = new Target_Curve_Editor(target_Curve, this, this);
				new_Target_Curve_Editor->setWindowFlags(Qt::Window);
				new_Target_Curve_Editor->show();
			runned_Target_Curve_Editors.insert(target_Curve, new_Target_Curve_Editor);
		}
	}
}

void Multilayer::set_Index_To_Target_Curves()
{
	int serial_Index = 0;
	for(int i=0; i<target_Profiles_Vector.size(); i++)
	{
		if( target_Profiles_Vector[i]->loaded_And_Ready )
		{
			serial_Index+=1;
			target_Profiles_Vector[i]->index = QString::number(serial_Index);
			target_Profiles_Vector[i]->show_Description_Label();
		}
	}
}

Multilayer& Multilayer::operator =(const Multilayer& referent_Multilayer)
{
	// structure tree
	*structure_Tree = *referent_Multilayer.structure_Tree;

	// independent plots
	for(int independent_Index=0; independent_Index< independent_Variables_Plot_Tabs->count(); independent_Index++)
	{
		Independent_Variables* old_Independent = qobject_cast<Independent_Variables*>(independent_Variables_Plot_Tabs->widget(independent_Index));
		delete old_Independent;
	}
	independent_Variables_Plot_Tabs->clear();
	for(int independent_Index=0; independent_Index<referent_Multilayer.independent_Variables_Plot_Tabs->tabBar()->count(); independent_Index++)
	{
		add_Independent_Variables_Tab();
		independent_Variables_Plot_Tabs->setTabText(independent_Index, referent_Multilayer.independent_Variables_Plot_Tabs->tabText(independent_Index));

		Independent_Variables* referent_Independent = qobject_cast<Independent_Variables*>(referent_Multilayer.independent_Variables_Plot_Tabs->widget(independent_Index));
		Independent_Variables* new_Independent = qobject_cast<Independent_Variables*>(independent_Variables_Plot_Tabs->widget(independent_Index));

		*new_Independent = *referent_Independent;
	}

	// target profiles
	data_Target_Profile_Frame_Vector.clear();
	add_Buttons_To_Lock.clear();
	remove_Buttons_To_Lock.clear();
	target_Profiles_Vector.clear();
	QLayoutItem* child;
	while ((child = layout_Target_Profile_With_Frame_Vector->takeAt(0)) != 0){delete child;}

	for(int target_Index=0; target_Index<referent_Multilayer.target_Profiles_Vector.size(); target_Index++)
	{
		add_Target_Curve(target_Index, true);
		*target_Profiles_Vector[target_Index] = *referent_Multilayer.target_Profiles_Vector[target_Index];
	}

	// other data
	num_Target_Rows		 = referent_Multilayer.num_Target_Rows;		 // rows in Calculation Settings
	num_Independent_Rows = referent_Multilayer.num_Independent_Rows; // rows in Calculation Settings

	graph_Options = referent_Multilayer.graph_Options;
	profile_Plot_Options = referent_Multilayer.profile_Plot_Options;

	enable_Calc_Target_Curves = referent_Multilayer.enable_Calc_Target_Curves;
	enable_Calc_Independent_Curves = referent_Multilayer.enable_Calc_Independent_Curves;

	min_Max_Density = referent_Multilayer.min_Max_Density;
	min_Max_Thickness = referent_Multilayer.min_Max_Thickness;
	min_Max_Sigma = referent_Multilayer.min_Max_Sigma;

	return *this;
}
