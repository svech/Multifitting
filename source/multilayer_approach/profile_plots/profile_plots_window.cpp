#include "profile_plots_window.h"

Profile_Plots_Window::Profile_Plots_Window(QWidget *parent) : QWidget(parent)
{
	setWindowTitle("Profile Plots");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Profile_Plots_Window::contextMenuEvent(QContextMenuEvent *event)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(main_Tabs->currentIndex()));

	QMenu wavelength_Units("Wavelength units");
	{
		QActionGroup* group_Act_Unit = new QActionGroup(this);
			group_Act_Unit->setExclusive(true);

		for(int index=0; index<wavelength_Units_List.size(); index++)
		{
			QAction* act_Unit = new QAction(wavelength_Units_List[index], this);
			act_Unit->setProperty(index_Property, index);
			act_Unit->setCheckable(true);
			act_Unit->setActionGroup(group_Act_Unit);

			if(wavelength_Units_List[index] == multilayer->profile_Plot_Options.local_wavelength_units) {act_Unit->setChecked(true);}
			wavelength_Units.addAction(act_Unit);
			connect(act_Unit,  &QAction::triggered, this, [=]
			{
				multilayer->profile_Plot_Options.local_wavelength_units = wavelength_Units_List[index];

				at_Wavelength_Label_Vector[main_Tabs->currentIndex()]->setText("At fixed " + Global_Variables::wavelength_Energy_Name(multilayer->profile_Plot_Options.local_wavelength_units));
				at_Wavelength_LineEdit_Vector[main_Tabs->currentIndex()]->setText(Locale.toString(Global_Variables::wavelength_Energy(multilayer->profile_Plot_Options.local_wavelength_units, 1.540562)/wavelength_Coefficients_Map.value(multilayer->profile_Plot_Options.local_wavelength_units),line_edit_double_format,line_edit_wavelength_precision));
				at_Wavelength_Units_Label_Vector[main_Tabs->currentIndex()]->setText(" " + multilayer->profile_Plot_Options.local_wavelength_units);
			});
		}
	}

	QMenu depth_Units("Depth units");
	{
		QActionGroup* group_Act_Unit = new QActionGroup(this);
			group_Act_Unit->setExclusive(true);

		for(int index=0; index<length_Units_List.size(); index++)
		{
			QAction* act_Unit = new QAction(length_Units_List[index], this);
				act_Unit->setProperty(index_Property, index);
				act_Unit->setCheckable(true);
				act_Unit->setActionGroup(group_Act_Unit);

			if(length_Units_List[index] == multilayer->profile_Plot_Options.local_length_units) {act_Unit->setChecked(true);}
			depth_Units.addAction(act_Unit);
			connect(act_Unit,  &QAction::triggered, this, [=]
			{
				multilayer->profile_Plot_Options.local_length_units = length_Units_List[index];

				profile_Plot_Vector[main_Tabs->currentIndex()]->custom_Plot->xAxis->setLabel("Depth, "+multilayer->profile_Plot_Options.local_length_units);
				profile_Plot_Vector[main_Tabs->currentIndex()]->custom_Plot->replot();

				// TODO change argument
			});
		}
	}

	QMenu menu;
	menu.addMenu(&wavelength_Units);
	menu.addMenu(&depth_Units);
	menu.exec(event->globalPos());
}

void Profile_Plots_Window::closeEvent(QCloseEvent *event)
{
	global_Multilayer_Approach->runned_Profile_Plots_Window.remove(profile_Plots_Key);
	global_Multilayer_Approach->unlock_Mainwindow_Interface();
	write_Window_Geometry();
	event->accept();
}

void Profile_Plots_Window::create_Main_Layout()
{
	can_Change_Index = false;
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(0,0,0,0);

	global_Multilayer_Approach->lock_Mainwindow_Interface();
	create_Tabs();
		main_Layout->addWidget(main_Tabs);
	add_Tabs();

	// shortcuts
	Global_Variables::create_Shortcuts(this);
	can_Change_Index = tab_synchronization;
}

void Profile_Plots_Window::set_Window_Geometry()
{
	setGeometry(profile_plots_x_corner,profile_plots_y_corner,profile_plots_width,profile_plots_height);
}

void Profile_Plots_Window::write_Window_Geometry()
{
	// usual show
	if(!isMaximized())
	{
		profile_plots_x_corner = frameGeometry().x()-corner_x_shift;
		profile_plots_y_corner = frameGeometry().y()-corner_y_shift;

		profile_plots_width  = geometry().width() +  width_add;
		profile_plots_height = geometry().height()+ height_add;
	}
}

void Profile_Plots_Window::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(false);

	connect(main_Tabs,	&QTabWidget::currentChanged, this,
	[=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);
		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}

		if(can_Change_Index)
		{
			can_Change_Index = false;
																										  {global_Multilayer_Approach->						  multilayer_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key)) {global_Multilayer_Approach->table_Of_Structures		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key))	  {global_Multilayer_Approach->table_Of_Roughness		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key))			  {global_Multilayer_Approach->optical_Graphs			  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key)){global_Multilayer_Approach->calculation_Settings_Editor->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			can_Change_Index = tab_synchronization;
		}
	});
}

void Profile_Plots_Window::add_Tabs()
{
	at_Wavelength_Label_Vector.resize(global_Multilayer_Approach->multilayer_Tabs->count());
	at_Wavelength_LineEdit_Vector.resize(global_Multilayer_Approach->multilayer_Tabs->count());
	at_Wavelength_Units_Label_Vector.resize(global_Multilayer_Approach->multilayer_Tabs->count());
	profile_Plot_Vector.resize(global_Multilayer_Approach->multilayer_Tabs->count());

	for(int tab_Index=0; tab_Index<global_Multilayer_Approach->multilayer_Tabs->count(); ++tab_Index)
	{
		QWidget* new_Widget = new QWidget;
		create_Tab_Content(new_Widget, tab_Index);
		main_Tabs->addTab(new_Widget, global_Multilayer_Approach->multilayer_Tabs->tabText(tab_Index));
	}
	for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
	{
		if(i!=main_Tabs->currentIndex()) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
	}
	main_Tabs->setCurrentIndex(global_Multilayer_Approach->multilayer_Tabs->currentIndex());
}

void Profile_Plots_Window::create_Tab_Content(QWidget* new_Widget, int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));

	Profile_Plot* new_Plot = new Profile_Plot(multilayer);
		new_Plot->custom_Plot->xAxis->setLabel("Depth, "+multilayer->profile_Plot_Options.local_length_units);
		profile_Plot_Vector[tab_Index] = new_Plot;

	QHBoxLayout* tab_Layout = new QHBoxLayout(new_Widget);

	// left side
	///==============================================================================================
		QVBoxLayout* left_Layout = new QVBoxLayout;
			left_Layout->setSpacing(2);
			tab_Layout->addLayout(left_Layout);

		///------------------------------------------------------------------------------------------
		// top part
		///------------------------------------------------------------------------------------------
		QGroupBox* value_Type_GroupBox = new QGroupBox;
			left_Layout->addWidget(value_Type_GroupBox);
			left_Layout->setAlignment(Qt::AlignTop);
		QVBoxLayout* value_Type_Layout = new QVBoxLayout(value_Type_GroupBox);
			value_Type_Layout->setSpacing(2);
			value_Type_Layout->setContentsMargins(8,5,8,5);

		// ----------------------------------------------------
		// permittivity
		// ----------------------------------------------------
		QGridLayout* permittivity_Layout = new QGridLayout;
			value_Type_Layout->addLayout(permittivity_Layout);

		QRadioButton* permittivity_RadioButton = new QRadioButton("Permittivity");
			permittivity_Layout->addWidget(permittivity_RadioButton,0,0,1,6);

		QLabel* at_Wavelength_Label = new QLabel("At fixed " + Global_Variables::wavelength_Energy_Name(multilayer->profile_Plot_Options.local_wavelength_units));
			permittivity_Layout->addWidget(at_Wavelength_Label,1,2,1,4);
			at_Wavelength_Label_Vector[tab_Index] = at_Wavelength_Label;
		QLineEdit* at_Wavelength_LineEdit = new QLineEdit(Locale.toString(Global_Variables::wavelength_Energy(multilayer->profile_Plot_Options.local_wavelength_units, multilayer->profile_Plot_Data.local_Wavelength)/wavelength_Coefficients_Map.value(multilayer->profile_Plot_Options.local_wavelength_units),line_edit_double_format,line_edit_wavelength_precision));
			at_Wavelength_LineEdit->setFixedWidth(80);
			at_Wavelength_LineEdit->setProperty(min_Size_Property, 80);
			at_Wavelength_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(at_Wavelength_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(at_Wavelength_LineEdit, false);});
			at_Wavelength_LineEdit_Vector[tab_Index] = at_Wavelength_LineEdit;
		QLabel* at_Wavelength_Unints_Label = new QLabel(" " + multilayer->profile_Plot_Options.local_wavelength_units);
			at_Wavelength_Units_Label_Vector[tab_Index] = at_Wavelength_Unints_Label;

		QHBoxLayout* wavelength_Layout = new QHBoxLayout;
			wavelength_Layout->setAlignment(Qt::AlignLeft);
			wavelength_Layout->addWidget(at_Wavelength_LineEdit,0,Qt::AlignLeft);
			wavelength_Layout->addWidget(at_Wavelength_Unints_Label,0,Qt::AlignLeft);
			permittivity_Layout->addLayout(wavelength_Layout,2,5);

		QRadioButton* delta_RadioButton = new QRadioButton("Re(1-"+Epsilon_Sym+")");
			permittivity_Layout->addWidget(delta_RadioButton,3,2,1,4);
			connect(delta_RadioButton, &QRadioButton::toggled, this, [=]
			{

				bool checked = delta_RadioButton->isChecked();
				if(checked)
				{
					qInfo() << "delta_RadioButton\n\n";
					multilayer->profile_Plot_Options.permittivity_Type = DELTA_EPS;
					new_Plot->custom_Plot->yAxis->setLabel("Re(1-"+Epsilon_Sym+")");
//					new_Plot->custom_Plot->replot();
					new_Plot->plot_Data(true);
				}
			});

		QRadioButton* beta_RadioButton = new QRadioButton("Im("+Epsilon_Sym+")");
			permittivity_Layout->addWidget(beta_RadioButton,4,2,1,4);
			connect(beta_RadioButton, &QRadioButton::toggled, this, [=]
			{
				bool checked = beta_RadioButton->isChecked();
				if(checked)
				{
					qInfo() << "beta_RadioButton\n\n";
					multilayer->profile_Plot_Options.permittivity_Type = BETA_EPS;
					new_Plot->custom_Plot->yAxis->setLabel("Im("+Epsilon_Sym+")");
//					new_Plot->custom_Plot->replot();
					new_Plot->plot_Data(true);
				}
			});

		QButtonGroup* delta_Beta_ButtonGroup = new QButtonGroup;
			delta_Beta_ButtonGroup->addButton(delta_RadioButton);
			delta_Beta_ButtonGroup->addButton(beta_RadioButton);


		connect(permittivity_RadioButton, &QRadioButton::toggled, this, [=]
		{
			bool checked = permittivity_RadioButton->isChecked();
			at_Wavelength_Label->setEnabled(checked);
			at_Wavelength_LineEdit->setEnabled(checked);
			at_Wavelength_Unints_Label->setEnabled(checked);

			if(checked)
			{
				multilayer->profile_Plot_Options.type = PERMITTIVITY;
				delta_RadioButton->toggled(delta_RadioButton->isChecked());
				beta_RadioButton ->toggled(beta_RadioButton ->isChecked());
			}

			delta_RadioButton->setEnabled(checked);
			beta_RadioButton->setEnabled(checked);
		});

		// ----------------------------------------------------
		// materials
		// ----------------------------------------------------
		QRadioButton* materials_RadioButton = new QRadioButton("Materials");
			value_Type_Layout->addWidget(materials_RadioButton);
			connect(materials_RadioButton, &QRadioButton::toggled, this, [=]
			{
				bool checked = materials_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.type = MATERIAL;
					new_Plot->custom_Plot->yAxis->setLabel("Relative density");
					new_Plot->custom_Plot->replot();
				}
				permittivity_RadioButton->toggled(permittivity_RadioButton->isChecked());
			});

		// ----------------------------------------------------
		// elements
		// ----------------------------------------------------
		QRadioButton* elements_RadioButton = new QRadioButton("Elements");
			// TODO
			elements_RadioButton->setDisabled(true);
			value_Type_Layout->addWidget(elements_RadioButton);
			connect(elements_RadioButton, &QRadioButton::toggled, this, [=]
			{
				bool checked = elements_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.type = ELEMENTS;
					new_Plot->custom_Plot->yAxis->setLabel("Absolute concentration, "+Multiply_Sym+"10"+Power_23_Sym+" cm"+Minus_Three_Sym);
					new_Plot->custom_Plot->replot();
				}
				permittivity_RadioButton->toggled(permittivity_RadioButton->isChecked());
			});

		// ----------------------------------------------------
		QButtonGroup* radio_ButtonGroup = new QButtonGroup;
			radio_ButtonGroup->addButton(permittivity_RadioButton);
			radio_ButtonGroup->addButton(materials_RadioButton);
			radio_ButtonGroup->addButton(elements_RadioButton);
		// ----------------------------------------------------
		value_Type_GroupBox->adjustSize();
		value_Type_GroupBox->setFixedSize(value_Type_GroupBox->size());

		///------------------------------------------------------------------------------------------
		// middle part
		///------------------------------------------------------------------------------------------
		QGroupBox* line_Type_GroupBox = new QGroupBox;
			left_Layout->addWidget(line_Type_GroupBox);
		QVBoxLayout* line_Type_Layout = new QVBoxLayout(line_Type_GroupBox);
			line_Type_Layout->setSpacing(2);
			line_Type_Layout->setContentsMargins(8,5,8,5);

		// ----------------------------------------------------
		QCheckBox* use_Roughness_CheckBox = new QCheckBox("Apply roughness");
			line_Type_Layout->addWidget(use_Roughness_CheckBox);
			connect(use_Roughness_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.apply_Roughness = use_Roughness_CheckBox->isChecked();
			});

		QCheckBox* use_Diffusiness_CheckBox = new QCheckBox("Apply diffusiness");
			line_Type_Layout->addWidget(use_Diffusiness_CheckBox);
			connect(use_Diffusiness_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.apply_Diffusiness = use_Diffusiness_CheckBox->isChecked();
			});

		QCheckBox* show_Sharp_CheckBox = new QCheckBox("Show sharp profile");
			line_Type_Layout->addWidget(show_Sharp_CheckBox);
			connect(show_Sharp_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.show_Sharp_Profile = show_Sharp_CheckBox->isChecked();
			});

		QCheckBox* discretization_CheckBox = new QCheckBox("Show discretization");
			line_Type_Layout->addWidget(discretization_CheckBox);
			connect(discretization_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.show_Discretization = discretization_CheckBox->isChecked();
			});


		// ----------------------------------------------------
		line_Type_GroupBox->adjustSize();
		line_Type_GroupBox->setFixedSize(line_Type_GroupBox->size());

		///------------------------------------------------------------------------------------------
		// bottom part
		///------------------------------------------------------------------------------------------
		QGroupBox* scale_GroupBox = new QGroupBox;
			left_Layout->addWidget(scale_GroupBox);
		QVBoxLayout* scale_Layout = new QVBoxLayout(scale_GroupBox);
			scale_Layout->setSpacing(2);
			scale_Layout->setContentsMargins(8,5,2,5);

		// ----------------------------------------------------
		QCheckBox* rescale_X_CheckBox = new QCheckBox("Rescale X");
			scale_Layout->addWidget(rescale_X_CheckBox);
			connect(rescale_X_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.rescale_X = rescale_X_CheckBox->isChecked();
			});

		QCheckBox* rescale_Y_CheckBox = new QCheckBox("Rescale Y");
			scale_Layout->addWidget(rescale_Y_CheckBox);
			connect(rescale_Y_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.rescale_Y = rescale_Y_CheckBox->isChecked();
			});

		// scaling
		QGridLayout* scale_Y_Layout = new QGridLayout;
			scale_Layout->addLayout(scale_Y_Layout);

		QLabel* scale_Y_Label = new QLabel("Scale Y: ");
			scale_Y_Layout->addWidget(scale_Y_Label,0,0);

		// lin
		QRadioButton* lin_Y_RadioButton = new QRadioButton("Lin");
			scale_Y_Layout->addWidget(lin_Y_RadioButton,0,1);
//			connect(lin_Y_RadioButton, &QRadioButton::toggled, this, [&]
//			{
//				if(lin_Y_RadioButton->isChecked())
//				{
//					multilayer->profile_Plot_Options.y_Scale  = lin_Scale;
//				}
//				plot_All_Data();
//			});
//			connect(lin_Y_RadioButton, &QRadioButton::clicked, lin_Y_RadioButton, &QRadioButton::toggled);

		// log
		QRadioButton* log_Y_RadioButton = new QRadioButton("Log");
			scale_Y_Layout->addWidget(log_Y_RadioButton,0,2);
			qInfo() << log_Y_RadioButton << endl;
			connect(log_Y_RadioButton, &QRadioButton::toggled, this, [&]
			{
				qInfo() << log_Y_RadioButton->isChecked();
//				if(log_Y_RadioButton->isChecked())
//				{
//					multilayer->profile_Plot_Options.y_Scale = log_Scale;
//				}
//				plot_All_Data();
			});
//			connect(log_Y_RadioButton, &QRadioButton::clicked, log_Y_RadioButton, &QRadioButton::toggled);

		QButtonGroup* Y_ButtonGroup = new QButtonGroup;
			Y_ButtonGroup->addButton(lin_Y_RadioButton);
			Y_ButtonGroup->addButton(log_Y_RadioButton);

		// ----------------------------------------------------
		scale_GroupBox->adjustSize();
		scale_GroupBox->setFixedSize(scale_GroupBox->size());

		value_Type_GroupBox->setFixedWidth(scale_GroupBox->width());
		line_Type_GroupBox->setFixedWidth(scale_GroupBox->width());

	///==============================================================================================

	// right side: plot
	////////////////////////////////////////////////
		QVBoxLayout* right_Layout = new QVBoxLayout;
			tab_Layout->addLayout(right_Layout);
		right_Layout->addWidget(new_Plot);
	////////////////////////////////////////////////

	///==============================================================================================
	// ----------------------------------------------------
	// initialization
	// ----------------------------------------------------
	if(multilayer->profile_Plot_Options.type == PERMITTIVITY) {permittivity_RadioButton->setChecked(true);}
	if(multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS) {delta_RadioButton->setChecked(true);}
	if(multilayer->profile_Plot_Options.permittivity_Type == BETA_EPS)  {beta_RadioButton->setChecked(true);}
	if(multilayer->profile_Plot_Options.type == MATERIAL)	  {materials_RadioButton->setChecked(true);}
	if(multilayer->profile_Plot_Options.type == ELEMENTS)	  {elements_RadioButton->setChecked(true);}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - -

	use_Roughness_CheckBox->setChecked(multilayer->profile_Plot_Options.apply_Roughness);
	use_Diffusiness_CheckBox->setChecked(multilayer->profile_Plot_Options.apply_Diffusiness);
	show_Sharp_CheckBox->setChecked(multilayer->profile_Plot_Options.show_Sharp_Profile);
	discretization_CheckBox->setChecked(multilayer->profile_Plot_Options.show_Discretization);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - -

	rescale_X_CheckBox->setChecked(multilayer->profile_Plot_Options.rescale_X);
	rescale_Y_CheckBox->setChecked(multilayer->profile_Plot_Options.rescale_Y);
//	if(multilayer->profile_Plot_Options.y_Scale == lin_Scale) {lin_Y_RadioButton->setChecked(true);}
//	if(multilayer->profile_Plot_Options.y_Scale == log_Scale) {log_Y_RadioButton->setChecked(true);}


//	use_Roughness_CheckBox->toggled(multilayer->profile_Plot_Options.apply_Roughness);		// already toggled when ->setChecked(true)
//	use_Diffusiness_CheckBox->toggled(multilayer->profile_Plot_Options.apply_Diffusiness);	// already toggled when ->setChecked(true)
//	show_Sharp_CheckBox->toggled(multilayer->profile_Plot_Options.show_Sharp_Profile);		// already toggled when ->setChecked(true)
//	discretization_CheckBox->toggled(multilayer->profile_Plot_Options.show_Discretization);	// already toggled when ->setChecked(true)

//	delta_RadioButton->toggled(multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS);// already toggled when ->setChecked(true)
//	beta_RadioButton->toggled(multilayer->profile_Plot_Options.permittivity_Type == BETA_EPS);	// already toggled when ->setChecked(true)
//	materials_RadioButton->toggled(multilayer->profile_Plot_Options.type == MATERIAL);			// already toggled when ->setChecked(true)
//	elements_RadioButton->toggled(multilayer->profile_Plot_Options.type == ELEMENTS);			// already toggled when ->setChecked(true)

	new_Plot->custom_Plot->replot();
}

void Profile_Plots_Window::refresh_Plots()
{
	for(Profile_Plot* profile_Plot: profile_Plot_Vector)
	{
		profile_Plot->plot_Data(true);
	}
}
