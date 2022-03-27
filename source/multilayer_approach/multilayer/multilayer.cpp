#include "multilayer.h"

Multilayer::Multilayer(QWidget *parent) :
	parent(parent),
	num_Target_Rows(1),
	num_Independent_Rows(1),
	QWidget(parent)
{
	graph_Options_2D.show_Current_Coordinate = false;

	imperfections_Model.PSD_1D.PSD_Type = PSD_Type_1D;
	imperfections_Model.PSD_1D.argument_Units = spatial_frequency_units;
	imperfections_Model.PSD_1D.value_Units = PSD_1D_value_units;

	imperfections_Model.PSD_2D.PSD_Type = PSD_Type_2D;
	imperfections_Model.PSD_2D.argument_Units = spatial_frequency_units;
	imperfections_Model.PSD_2D.value_Units = PSD_2D_value_units;

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

	// structure
	create_Structure_Tree();

	// bottom non-stretchable part
	QWidget* bottom_Part_Widget = new QWidget;
	main_Layout->addWidget(bottom_Part_Widget,0,Qt::AlignBottom);
	bottom_Part_Layout = new QVBoxLayout(bottom_Part_Widget);
		bottom_Part_Layout->setContentsMargins(0,0,0,0);
//		bottom_Part_Layout->setSpacing(0);
		bottom_Part_Layout->addWidget(structure_Tree->structure_Toolbar->toolbar);

	// buttons
	create_Main_Tools();

	// separator
	QFrame* separator = new QFrame;
		separator->setFrameShape(QFrame::HLine);
		separator->setFrameShadow(QFrame::Raised);
	bottom_Part_Layout->addWidget(separator);

	// independent data
	create_Independent_Variables_Tabs();

	// target data
	create_Data_Frame();
}

void Multilayer::create_Structure_Tree()
{
	structure_Tree = new Structure_Tree(this, this);
	main_Layout->addWidget(structure_Tree->tree);
}

void Multilayer::create_Main_Tools()
{	
	QWidget* main_Tools_Widget = new QWidget;
	bottom_Part_Layout->addWidget(main_Tools_Widget);

	QGridLayout* layout = new QGridLayout(main_Tools_Widget);
		layout->setContentsMargins(0,0,0,0);
		layout->setAlignment(Qt::AlignVCenter);

	structure_Table_Button = new QPushButton("Structure table");
		layout->addWidget(structure_Table_Button,0,0);
	profile_Plots_Button = new QPushButton("Profile plot");
		layout->addWidget(profile_Plots_Button,0,1);


	optical_Graphs_1D_Button = new QPushButton("1D graphs");
		layout->addWidget(optical_Graphs_1D_Button,1,0);
	optical_Graphs_2D_Button = new QPushButton("2D graphs");
		layout->addWidget(optical_Graphs_2D_Button,1,1);


	roughness_Plots_Button = new QPushButton("Roughness spectrum");
		layout->addWidget(roughness_Plots_Button,2,0);
	particles_Plots_Button = new QPushButton("Particles spectrum");
		layout->addWidget(particles_Plots_Button,2,1);


	calculation_Settings_Button = new QPushButton("Calculation settings");
		layout->addWidget(calculation_Settings_Button,3,0);
	general_Settings_Button = new QPushButton("General settings");
		layout->addWidget(general_Settings_Button,3,1);


	fitting_Settings_Button = new QPushButton("Fitting settings");
		layout->addWidget(fitting_Settings_Button,4,0);
	fits_Selector_Button = new QPushButton("Fits selector");
		layout->addWidget(fits_Selector_Button,4,1);

	Multilayer_Approach* parent_Multilayer_Approach = qobject_cast<Multilayer_Approach*>(parent);
	connect(structure_Table_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Table_Of_Structures);
	connect(profile_Plots_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Profile_Plots);

	connect(optical_Graphs_1D_Button,	 &QPushButton::clicked, parent_Multilayer_Approach, [=]{parent_Multilayer_Approach->open_Optical_Graphs_1D();});
	connect(optical_Graphs_2D_Button,	 &QPushButton::clicked, parent_Multilayer_Approach, [=]{parent_Multilayer_Approach->open_Optical_Graphs_2D();});

	connect(roughness_Plots_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Roughness_Plots);
	connect(particles_Plots_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Particles_Plots);

	connect(calculation_Settings_Button, &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Calculation_Settings);
	connect(general_Settings_Button,	 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_General_Settings);

	connect(fitting_Settings_Button,	 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Fitting_Settings);
	connect(fits_Selector_Button,		 &QPushButton::clicked, parent_Multilayer_Approach, &Multilayer_Approach::open_Fits_Selector);
}

void Multilayer::create_Independent_Variables_Tabs()
{
	independent_Curve_Tabs = new QTabWidget;
		independent_Curve_Tabs->setMovable(true);
		independent_Curve_Tabs->setTabsClosable(true);		
		independent_Curve_Tabs->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);

	bottom_Part_Layout->addWidget(independent_Curve_Tabs);

	independent_Variables_Corner_Button = new QToolButton;
		independent_Variables_Corner_Button->setText("+");
	QFont tmp_Qf; tmp_Qf.setBold(true);
		independent_Variables_Corner_Button->setFont(tmp_Qf);
		independent_Curve_Tabs->setCornerWidget(independent_Variables_Corner_Button);

	connect(independent_Variables_Corner_Button,&QToolButton::clicked,					this, [=]{add_Independent_Variables_Tab();});
	connect(independent_Curve_Tabs,				&QTabWidget::currentChanged,			this, &Multilayer::change_Tab_Independent_Variables_Tab_Color);
	connect(independent_Curve_Tabs,				&QTabWidget::tabCloseRequested,			this, &Multilayer::remove_Independent_Variables_Tab);
	connect(independent_Curve_Tabs,				&QTabWidget::tabBarDoubleClicked,		this, &Multilayer::rename_Independent_Variables_Tab);
	connect(independent_Curve_Tabs->tabBar(),	&QTabBar::customContextMenuRequested,	this, &Multilayer::independent_Tab_Context_Menu);

	add_Independent_Variables_Tab();
}

void Multilayer::create_Data_Frame()
{
	data_Target_Profile_Group_Box = new QGroupBox("Target curves");
	bottom_Part_Layout->addWidget(data_Target_Profile_Group_Box);

	layout_Target_Profile_With_Frame_Vector = new QVBoxLayout(data_Target_Profile_Group_Box);
		layout_Target_Profile_With_Frame_Vector->setSpacing(0);
		layout_Target_Profile_With_Frame_Vector->setMargin(0);

	add_Target_Curve(0);
}

//// --------------------------------------------------------------------------------------

Independent_Curve* Multilayer::add_Independent_Variables_Tab(int index)
{
	Independent_Curve* new_Independent_Curve = new Independent_Curve;
	Independent_Curve_Editor* independent_Curve_Editor;
	connect(new_Independent_Curve->setup_Button, &QPushButton::clicked, this, [=]{ open_Editor_Window(new_Independent_Curve, independent_Curve_Editor); });

	// add new tab	
	if(index < 0)
	{
		independent_Curve_Tabs->addTab(new_Independent_Curve, default_independent_curve_tab_name);
		new_Independent_Curve->name = default_independent_curve_tab_name+Locale.toString(independent_Curve_Tabs->count());
		independent_Curve_Tabs->setTabText(independent_Curve_Tabs->count()-1, new_Independent_Curve->name + new_Independent_Curve->enlarge_Tab_Name());
	} else
	{
		independent_Curve_Tabs->insertTab(index+1, new_Independent_Curve, independent_Curve_Tabs->tabText(index) + " (copy)");
	}

	if(independent_Curve_Tabs->count()>1)
	{
		independent_Curve_Tabs->tabBar()->setTabTextColor(independent_Curve_Tabs->count()-1,Qt::gray);
		independent_Curve_Tabs->tabBar()->tabButton(independent_Curve_Tabs->count()-1, QTabBar::RightSide)->hide();
	}
	if(new_Independent_Curve->calc_Functions.if_Something_Enabled_1D()) qobject_cast<Multilayer_Approach*>(parent)->independent_Added_1D = true;
	if(new_Independent_Curve->calc_Functions.if_Something_Enabled_2D()) qobject_cast<Multilayer_Approach*>(parent)->independent_Added_2D = true;

	return new_Independent_Curve;
}

void Multilayer::change_Tab_Independent_Variables_Tab_Color(int index)
{
	independent_Curve_Tabs->tabBar()->setTabTextColor(index,Qt::black);
	if(independent_Curve_Tabs->tabBar()->tabButton(index, QTabBar::RightSide))
	   independent_Curve_Tabs->tabBar()->tabButton(index, QTabBar::RightSide)->show();

	for(int i = 0; i<independent_Curve_Tabs->tabBar()->count(); i++)
	{
		if(i!=index)
		{
			independent_Curve_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->hide();
			independent_Curve_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	}
}

void Multilayer::remove_Independent_Variables_Tab(int index)
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Variables tab \"" + independent_Curve_Tabs->tabBar()->tabText(index)
															  + "\" will be removed.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if (reply == QMessageBox::Yes)
	{
		Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(independent_Curve_Tabs->widget(index));
		// reopening
		bool reopen_Windows = (independent_Curve->measurement.measurement_Type != no_Measurement_Type);

		if(runned_Independent_Curve_Editors.contains(independent_Curve))
		{
			runned_Independent_Curve_Editors.value(independent_Curve)->close();
			runned_Independent_Curve_Editors.remove(independent_Curve);
		}
		delete independent_Curve;	//detele independent_Curve_Tabs->widget(index);
		if(independent_Curve_Tabs->count()==0) add_Independent_Variables_Tab();

		// correcting number of rows
		int counter_1D_Curves = 0, counter_2D_Curves = 0;
		for(int i=0; i<independent_Curve_Tabs->count(); i++)
		{
			Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(independent_Curve_Tabs->widget(i));

			if( independent_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan])
			{
				counter_1D_Curves++;
			}
			if( independent_Curve->measurement.measurement_Type == measurement_Types[GISAS])
			{
				counter_2D_Curves++;
			}
		}
		graph_Options_1D.num_Independent_Graph_Rows = min(graph_Options_1D.num_Independent_Graph_Rows,counter_1D_Curves);
		graph_Options_1D.num_Independent_Graph_Rows = max(graph_Options_1D.num_Independent_Graph_Rows,1);

		graph_Options_2D.num_Independent_Graph_Rows = min(graph_Options_2D.num_Independent_Graph_Rows,counter_2D_Curves);
		graph_Options_2D.num_Independent_Graph_Rows = max(graph_Options_2D.num_Independent_Graph_Rows,1);

		// for calculation_Settings
		int total_Curves = counter_1D_Curves + counter_2D_Curves;
		num_Independent_Rows = min(num_Independent_Rows,total_Curves);
		num_Independent_Rows = max(num_Independent_Rows,1);

		if(reopen_Windows)
		{
			global_Multilayer_Approach->reopen_Calculation_Settings();
			global_Multilayer_Approach->reopen_Optical_Graphs_1D();
			global_Multilayer_Approach->reopen_Optical_Graphs_2D();
		}
	}
}

void Multilayer::rename_Independent_Variables_Tab(int tab_Index)
{
	bool ok;
	Independent_Curve* independent = qobject_cast<Independent_Curve*>(independent_Curve_Tabs->widget(tab_Index));

	QString text = QInputDialog::getText(this, "Rename plot", "New name", QLineEdit::Normal, independent->name, &ok);
	if (ok && !text.isEmpty())
	{
		independent->name = text;
		independent_Curve_Tabs->setTabText(tab_Index, independent->name + independent->enlarge_Tab_Name());
	}
}

void Multilayer::independent_Tab_Context_Menu(const QPoint& pos)
{
	QMenu menu;
	QAction duplicate_Action("Duplicate curve");
	menu.addAction(&duplicate_Action);
	connect(&duplicate_Action, &QAction::triggered, this, [=]{duplicate_Independent_Curve(pos);});
	menu.exec(QCursor::pos());
}

void Multilayer::duplicate_Independent_Curve(const QPoint& pos)
{
	int old_Tab_Index = independent_Curve_Tabs->tabBar()->tabAt(pos);
	Independent_Curve* new_Independent_Curve = add_Independent_Variables_Tab(old_Tab_Index);
	Independent_Curve* old_Independent_Curve = qobject_cast<Independent_Curve*>(independent_Curve_Tabs->widget(old_Tab_Index));
	*new_Independent_Curve = *old_Independent_Curve;
	new_Independent_Curve->name += " (copy)";
	new_Independent_Curve->refresh_Description_Label();

	// reopening windows
	if(new_Independent_Curve->measurement.measurement_Type != no_Measurement_Type)
	{
		global_Multilayer_Approach->reopen_Calculation_Settings();
		global_Multilayer_Approach->reopen_Optical_Graphs_1D();
		global_Multilayer_Approach->reopen_Optical_Graphs_2D();
	}
}

void Multilayer::refresh_Structure_And_Independent(QObject* my_Sender)
{
	structure_Tree->refresh__StructureTree__Data_and_Text();
	structure_Tree->editors_Edit(sender());

	// refresh other multilayers
	if(!my_Sender) {emit refresh_All_Multilayers();}
}

//// --------------------------------------------------------------------------------------

Target_Curve* Multilayer::add_Target_Curve(int index_Pressed, bool opening)
{
	// window resizing
	if(!data_Target_Profile_Frame_Vector.isEmpty())
	{
		if(!opening)
		{
			QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() + data_Target_Profile_Frame_Vector.first()->height());
		}
	}

	QPushButton* new_Import_Button = new QPushButton("Import");
	QLabel* new_Description_Label  = new QLabel("<no description>");
		new_Description_Label->setContextMenuPolicy(Qt::CustomContextMenu);
	QPushButton* new_Add_Button    = new QPushButton("Add Row");
//	QToolButton* new_Add_Button    = new QToolButton;	new_Add_Button   ->setText("+");
//	QPushButton* new_Remove_Button = new QPushButton("x");//("Remove");
	QToolButton* new_Remove_Button = new QToolButton;	new_Remove_Button->setText("x");
		QFont remove_Font; remove_Font.setBold(true);
		new_Remove_Button->setFont(remove_Font);
//		QPalette remove_Palette = new_Remove_Button->palette();
//		QColor remove_Color = QColor(226,143,102);
//		remove_Palette.setColor(QPalette::NoRole, remove_Color);
//		new_Remove_Button->setPalette(remove_Palette);
		new_Remove_Button->setStyleSheet("color: white; background-color: rgb(226,143,102)");

	QFrame* new_Frame = new QFrame;
		layout_Target_Profile_With_Frame_Vector->insertWidget(index_Pressed,new_Frame);

	Target_Curve* new_Target_Curve = new Target_Curve(new_Description_Label, this);
	Target_Curve_Editor* new_Target_Curve_Editor;

	data_Target_Profile_Frame_Vector.insert(index_Pressed,new_Frame);
	description_Label_Vector.insert(index_Pressed,new_Description_Label);
	target_Profiles_Vector.insert(index_Pressed, new_Target_Curve);

	QHBoxLayout* new_Frame_Layout = new QHBoxLayout(new_Frame);
		new_Frame_Layout->setMargin(0);
		new_Frame_Layout->setSpacing(0);
	QHBoxLayout* left_Layout  = new QHBoxLayout;
	QHBoxLayout* right_Layout = new QHBoxLayout;
	left_Layout->setSpacing(10);
	right_Layout->setSpacing(0);

	new_Import_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Description_Label->	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Add_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Remove_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	new_Import_Button->		setFixedWidth(60);
	new_Add_Button->		setFixedWidth(63);
	new_Remove_Button->		setFixedSize(21,21);

	left_Layout->addWidget(new_Import_Button);
	left_Layout->addWidget(new_Description_Label, 0, Qt::AlignLeft);
	right_Layout->addWidget(new_Add_Button);
	right_Layout->addWidget(new_Remove_Button,Qt::AlignHCenter);
	new_Frame_Layout->addLayout(left_Layout, Qt::AlignLeft);
	new_Frame_Layout->addLayout(right_Layout, Qt::AlignRight);

	connect(new_Description_Label,&QLabel::customContextMenuRequested, this, &Multilayer::target_Curve_Context_Menu);
	connect(new_Import_Button,	  &QPushButton::clicked, this, [=]{ open_Editor_Window (new_Target_Curve, new_Target_Curve_Editor); });
	connect(new_Add_Button,		  &QToolButton::clicked, this, [=]{ add_Target_Curve   (data_Target_Profile_Frame_Vector.indexOf(new_Frame)+1); });
	connect(new_Remove_Button,	  &QPushButton::clicked, this, [=]{ remove_Target_Curve(data_Target_Profile_Frame_Vector.indexOf(new_Frame)  ); });

	return new_Target_Curve;
}

void Multilayer::target_Curve_Context_Menu(const QPoint& pos)
{
	QLabel* description_Label = qobject_cast<QLabel*>(QObject::sender());

	QMenu menu;
	QAction duplicate_Action("Duplicate curve");
	menu.addAction(&duplicate_Action);
	connect(&duplicate_Action, &QAction::triggered, this, [=]
	{
		int old_Index = description_Label_Vector.indexOf(description_Label);
		duplicate_Target_Curve(old_Index);
	});
	menu.exec(QCursor::pos());
}

void Multilayer::duplicate_Target_Curve(int old_Index)
{
	Target_Curve* new_Target_Curve = add_Target_Curve(old_Index+1);
	Target_Curve* old_Target_Curve = target_Profiles_Vector[old_Index];
	*new_Target_Curve = *old_Target_Curve;
	set_Index_To_Target_Curves();

	// reopening windows
	if(new_Target_Curve->loaded_And_Ready)
	{
		global_Multilayer_Approach->reopen_Calculation_Settings();
		// 1D case
		if(new_Target_Curve->measurement.measurement_Type != measurement_Types[GISAS_Map])
		{
			global_Multilayer_Approach->reopen_Optical_Graphs_1D(true, TARGET);
		}
		// 2D case
		if(new_Target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
		{
			global_Multilayer_Approach->reopen_Optical_Graphs_2D(true, TARGET);
		}
	}
}

void Multilayer::remove_Target_Curve(int index_Pressed, bool clean)
{	
	// delete frame	
	delete data_Target_Profile_Frame_Vector[index_Pressed];
	data_Target_Profile_Frame_Vector.removeAt(index_Pressed);
	description_Label_Vector.removeAt(index_Pressed);

	// reopening
	bool reopen_Windows = target_Profiles_Vector[index_Pressed]->loaded_And_Ready;

	// delete data
	if(runned_Target_Curve_Editors.contains(target_Profiles_Vector[index_Pressed]))
	{
		runned_Target_Curve_Editors.value(target_Profiles_Vector[index_Pressed])->close();
		runned_Target_Curve_Editors.remove(target_Profiles_Vector[index_Pressed]);
	}
	delete target_Profiles_Vector[index_Pressed];
	target_Profiles_Vector.removeAt(index_Pressed);

	// window resizing
	if( !data_Target_Profile_Frame_Vector.isEmpty() )
	{
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() - data_Target_Profile_Frame_Vector.first()->height());
	}

	if(data_Target_Profile_Frame_Vector.isEmpty() && !clean)	add_Target_Curve(0);

	set_Index_To_Target_Curves();

	// correcting number of rows
	int counter_1D_Curves = 0, counter_2D_Curves = 0;
	for(Target_Curve* target_Curve : target_Profiles_Vector)
	{
		if( target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan])
		{
			counter_1D_Curves++;
		}
		if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS])
		{
			counter_2D_Curves++;
		}
	}
	graph_Options_1D.num_Target_Graph_Rows = min(graph_Options_1D.num_Target_Graph_Rows,counter_1D_Curves);
	graph_Options_1D.num_Target_Graph_Rows = max(graph_Options_1D.num_Target_Graph_Rows,1);

	graph_Options_2D.num_Target_Graph_Rows = min(graph_Options_2D.num_Target_Graph_Rows,counter_2D_Curves);
	graph_Options_2D.num_Target_Graph_Rows = max(graph_Options_2D.num_Target_Graph_Rows,1);

	// for calculation_Settings
	int total_Curves = counter_1D_Curves + counter_2D_Curves;
	num_Target_Rows = min(num_Target_Rows,total_Curves);
	num_Target_Rows = max(num_Target_Rows,1);

	if(reopen_Windows)
	{
		global_Multilayer_Approach->reopen_Calculation_Settings();
		global_Multilayer_Approach->reopen_Optical_Graphs_1D();
		global_Multilayer_Approach->reopen_Optical_Graphs_2D();
	}
}


template<typename Type_Curve, typename Type_Curve_Editor>
void Multilayer::open_Editor_Window(Type_Curve* type_Curve, Type_Curve_Editor* type_Curve_Editor)
{
	// check in map if open
	bool check = false;
	if(typeid(Type_Curve) == typeid(Independent_Curve))
	{
		Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(type_Curve);
		if(runned_Independent_Curve_Editors.contains(independent_Curve))
		{
			runned_Independent_Curve_Editors.value(independent_Curve)->activateWindow();
			check = false;
		} else
		{
			check = true;
		}
	}
	if(typeid(Type_Curve) == typeid(Target_Curve))
	{
		Target_Curve* target_Curve = qobject_cast<Target_Curve*>(type_Curve);
		if(runned_Target_Curve_Editors.contains(target_Curve))
		{
			runned_Target_Curve_Editors.value(target_Curve)->activateWindow();
			check = false;
		} else
		{
			check = true;
		}
	}

	if(check)
	{
		if(type_Curve->measurement.measurement_Type == no_Measurement_Type)
		{
			QDialog* choice_Data_Type_Window = new QDialog(this);
				choice_Data_Type_Window->setWindowTitle("Data");
				choice_Data_Type_Window->setWindowModality(Qt::ApplicationModal);
				choice_Data_Type_Window->setAttribute(Qt::WA_DeleteOnClose);
				choice_Data_Type_Window->setWindowFlags(Qt::Tool);
				choice_Data_Type_Window->show();

			QVBoxLayout* choice_Data_Type_Layout = new QVBoxLayout(choice_Data_Type_Window);
				choice_Data_Type_Layout->setSizeConstraint(QLayout::SetFixedSize);
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
			connect(specular_Button,  &QPushButton::clicked, this, [=]{type_Curve->measurement.measurement_Type = measurement_Types[Specular_Scan]; type_Curve->measurement.detector_1D.slit_Width = default_detector_1D_slit_width_specular;  type_Curve->measurement.detector_1D.slit_Width = 0.8; choice_Data_Type_Window->close(); open_Editor_Window(type_Curve, type_Curve_Editor);});
				choice_Data_Type_Group_Box_Layout->addWidget(specular_Button);
			QPushButton* detector_Button = new QPushButton(measurement_Types[Detector_Scan]);
			connect(detector_Button,  &QPushButton::clicked, this, [=]{type_Curve->measurement.measurement_Type = measurement_Types[Detector_Scan]; type_Curve->measurement.detector_1D.slit_Width = default_detector_1D_slit_width_scattering; choice_Data_Type_Window->close(); open_Editor_Window(type_Curve, type_Curve_Editor);});
				choice_Data_Type_Group_Box_Layout->addWidget(detector_Button);
			QPushButton* rocking_Button  = new QPushButton(measurement_Types[Rocking_Curve]);
			connect(rocking_Button,   &QPushButton::clicked, this, [=]{type_Curve->measurement.measurement_Type = measurement_Types[Rocking_Curve]; type_Curve->measurement.detector_1D.slit_Width = default_detector_1D_slit_width_scattering; type_Curve->measurement.spectral_Distribution.use_Sampling = true; choice_Data_Type_Window->close(); open_Editor_Window(type_Curve, type_Curve_Editor);});
				choice_Data_Type_Group_Box_Layout->addWidget(rocking_Button);
			QPushButton* offset_Button   = new QPushButton(measurement_Types[Offset_Scan]);
			connect(offset_Button,    &QPushButton::clicked, this, [=]{type_Curve->measurement.measurement_Type = measurement_Types[Offset_Scan];   type_Curve->measurement.detector_1D.slit_Width = default_detector_1D_slit_width_scattering; type_Curve->measurement.spectral_Distribution.use_Sampling = true; choice_Data_Type_Window->close(); open_Editor_Window(type_Curve, type_Curve_Editor);});
				choice_Data_Type_Group_Box_Layout->addWidget(offset_Button);
			QPushButton* gisas_Button    = new QPushButton(measurement_Types[GISAS_Map]);
			connect(gisas_Button,     &QPushButton::clicked, this, [=]{type_Curve->measurement.measurement_Type = measurement_Types[GISAS_Map];		type_Curve->calc_Functions.instrumental_Smoothing = false; choice_Data_Type_Window->close(); open_Editor_Window(type_Curve, type_Curve_Editor);});
				choice_Data_Type_Group_Box_Layout->addWidget(gisas_Button);
		} else
		{
			type_Curve_Editor = new Type_Curve_Editor(type_Curve, this, this);
				type_Curve_Editor->setWindowFlags(Qt::Window);
				type_Curve_Editor->show();

			// add to map
			if(typeid(Type_Curve) == typeid(Independent_Curve))
			{
				Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(type_Curve);
				if(independent_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
				{
					if( //imperfections_Model.PSD_Model == fractal_Gauss_Model ||
						imperfections_Model.approximation == DWBA_approximation ||
						imperfections_Model.approximation == SA_approximation ||
						imperfections_Model.approximation == CSA_approximation )
					{
						QString text;
//						if( imperfections_Model.PSD_Model == fractal_Gauss_Model ) text = "2D scattering can't be simulated with fractal Gauss model of roughness.\nChange the model in Structure Table.";
						if( imperfections_Model.approximation == DWBA_approximation ) text = "2D scattering can't be simulated in DWBA approximation.\nChange the approximation in Structure Table.";
						if( imperfections_Model.approximation == SA_approximation   ) text = "2D scattering can't be simulated in SA approximation.\nChange the approximation in Structure Table.";
						if( imperfections_Model.approximation == CSA_approximation  ) text = "2D scattering can't be simulated in CSA approximation.\nChange the approximation in Structure Table.";

						QMessageBox::information(this,"GISAS simulation", text);
						independent_Curve->calc_Functions.check_Enabled = false;

						global_Multilayer_Approach->reopen_Calculation_Settings();
						global_Multilayer_Approach->reopen_Optical_Graphs_2D();
					}
				}
				Independent_Curve_Editor* independent_Curve_Editor = qobject_cast<Independent_Curve_Editor*>(type_Curve_Editor);
				runned_Independent_Curve_Editors.insert(independent_Curve, independent_Curve_Editor);
			}
			if(typeid(Type_Curve) == typeid(Target_Curve))
			{
				Target_Curve* target_Curve = qobject_cast<Target_Curve*>(type_Curve);			
				if(target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
				{
					if( //imperfections_Model.PSD_Model == fractal_Gauss_Model ||
						imperfections_Model.approximation == DWBA_approximation ||
						imperfections_Model.approximation == SA_approximation ||
						imperfections_Model.approximation == CSA_approximation )
					{
						QString text;
//						if( imperfections_Model.PSD_Model == fractal_Gauss_Model ) text = "2D scattering can't be simulated with fractal Gauss model of roughness.\nChange the model in Structure Table.";
						if( imperfections_Model.approximation == DWBA_approximation ) text = "2D scattering can't be simulated in DWBA approximation.\nChange the approximation in Structure Table.";
						if( imperfections_Model.approximation == SA_approximation   ) text = "2D scattering can't be simulated in SA approximation.\nChange the approximation in Structure Table.";
						if( imperfections_Model.approximation == CSA_approximation  ) text = "2D scattering can't be simulated in CSA approximation.\nChange the approximation in Structure Table.";

						QMessageBox::information(this,"GISAS simulation", text);
						target_Curve->fit_Params.calculate = false;
						global_Multilayer_Approach->reopen_Calculation_Settings();
						global_Multilayer_Approach->reopen_Optical_Graphs_2D();
					}
				}
				Target_Curve_Editor* target_Curve_Editor = qobject_cast<Target_Curve_Editor*>(type_Curve_Editor);
				runned_Target_Curve_Editors.insert(target_Curve, target_Curve_Editor);
			}
		}
	}
}
template void Multilayer::open_Editor_Window<Independent_Curve, Independent_Curve_Editor>(Independent_Curve*, Independent_Curve_Editor*);
template void Multilayer::open_Editor_Window<Target_Curve,      Target_Curve_Editor>	 (Target_Curve*,      Target_Curve_Editor*);

void Multilayer::set_Index_To_Target_Curves()
{
	int serial_Index = 0;
	for(int i=0; i<target_Profiles_Vector.size(); i++)
	{
		if( target_Profiles_Vector[i]->loaded_And_Ready )
		{
			serial_Index+=1;
			target_Profiles_Vector[i]->index = QString::number(serial_Index);
			if(target_Profiles_Vector[i]->name.size()>0)
			{	target_Profiles_Vector[i]->index_Name = target_Profiles_Vector[i]->index + ": " + target_Profiles_Vector[i]->name;}
			else
			{	target_Profiles_Vector[i]->index_Name = target_Profiles_Vector[i]->index;}

			target_Profiles_Vector[i]->refresh_Description_Label();
		}
	}
}

Multilayer& Multilayer::operator =(const Multilayer& referent_Multilayer)
{
	// structure tree
	*structure_Tree = *referent_Multilayer.structure_Tree;

	// independent
	for(int independent_Index=0; independent_Index< independent_Curve_Tabs->count(); independent_Index++)
	{
		Independent_Curve* old_Independent = qobject_cast<Independent_Curve*>(independent_Curve_Tabs->widget(independent_Index));
		delete old_Independent;
	}
	independent_Curve_Tabs->clear();
	for(int independent_Index=0; independent_Index<referent_Multilayer.independent_Curve_Tabs->tabBar()->count(); independent_Index++)
	{
		add_Independent_Variables_Tab();
		independent_Curve_Tabs->setTabText(independent_Index, referent_Multilayer.independent_Curve_Tabs->tabText(independent_Index));

		Independent_Curve* referent_Independent = qobject_cast<Independent_Curve*>(referent_Multilayer.independent_Curve_Tabs->widget(independent_Index));
		Independent_Curve* new_Independent = qobject_cast<Independent_Curve*>(independent_Curve_Tabs->widget(independent_Index));

		*new_Independent = *referent_Independent;
		new_Independent->refresh_Description_Label();
	}

	// target profiles	

	// clear existing targets
	for(int i=data_Target_Profile_Frame_Vector.size()-1; i>=0; --i)
	{
		remove_Target_Curve(i,true);
	}

	QLayoutItem* child;
	while ((child = layout_Target_Profile_With_Frame_Vector->takeAt(0)) != 0){delete child;}

	for(int target_Index=0; target_Index<referent_Multilayer.target_Profiles_Vector.size(); target_Index++)
	{
		add_Target_Curve(target_Index, true);
		target_Profiles_Vector[target_Index]->mark_As_Copy = false; // without marking as copy
		*target_Profiles_Vector[target_Index] = *referent_Multilayer.target_Profiles_Vector[target_Index];
		target_Profiles_Vector[target_Index]->mark_As_Copy = true;
		target_Profiles_Vector[target_Index]->refresh_Description_Label();
	}

	// other data
	num_Target_Rows		 = referent_Multilayer.num_Target_Rows;		 // rows in Calculation settings
	num_Independent_Rows = referent_Multilayer.num_Independent_Rows; // rows in Calculation settings

	graph_Options_1D = referent_Multilayer.graph_Options_1D;
	graph_Options_2D = referent_Multilayer.graph_Options_2D;
	profile_Plot_Options = referent_Multilayer.profile_Plot_Options;
	roughness_Plot_Options = referent_Multilayer.roughness_Plot_Options;
	particles_Plot_Options = referent_Multilayer.particles_Plot_Options;

	discretization_Parameters = referent_Multilayer.discretization_Parameters;
	imperfections_Model = referent_Multilayer.imperfections_Model;

	enable_Calc_Target_Curves = referent_Multilayer.enable_Calc_Target_Curves;
	enable_Calc_Independent_Curves = referent_Multilayer.enable_Calc_Independent_Curves;

	min_Max_Density = referent_Multilayer.min_Max_Density;
	min_Max_Thickness = referent_Multilayer.min_Max_Thickness;
	min_Max_Sigma = referent_Multilayer.min_Max_Sigma;

	return *this;
}
