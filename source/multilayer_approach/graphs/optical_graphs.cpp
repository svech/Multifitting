#include "optical_graphs.h"

Optical_Graphs::Optical_Graphs(QString dimension, QString keep_Splitter, QWidget* parent) :
	total_Number_of_Target_Graphs(global_Multilayer_Approach->multilayer_Tabs->count()),
	total_Number_of_Independent_Graphs(global_Multilayer_Approach->multilayer_Tabs->count()),
	dimension(dimension),
	keep_Splitter(keep_Splitter),
	target_Independent_Splitter_Vec        (global_Multilayer_Approach->multilayer_Tabs->count()),
	target_Vertical_Splitter_Vec		   (global_Multilayer_Approach->multilayer_Tabs->count()),
	target_Horizontal_Splitter_Vec_Vec	   (global_Multilayer_Approach->multilayer_Tabs->count()),
	independent_Vertical_Splitter_Vec	   (global_Multilayer_Approach->multilayer_Tabs->count()),
	independent_Horizontal_Splitter_Vec_Vec(global_Multilayer_Approach->multilayer_Tabs->count()),
	QWidget(parent) // nullptr!
{
		if(	dimension == dim_1D )	setWindowTitle("1D graphs");
		if(	dimension == dim_2D )	setWindowTitle("2D graphs");
		create_Main_Layout();
		set_Window_Geometry();
		setAttribute(Qt::WA_DeleteOnClose);
}

void Optical_Graphs::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu;
	QAction settings_Action("Settings");
	menu.addAction(&settings_Action);

	if(dimension == dim_1D) connect(&settings_Action, &QAction::triggered, this, [=]{ settings_1D();});
	if(dimension == dim_2D) connect(&settings_Action, &QAction::triggered, this, [=]{ settings_2D();});

	menu.exec(event->globalPos());
}

void Optical_Graphs::settings_1D()
{
	int index = main_Tabs->currentIndex();
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(index));

	QWidget* settings_Window = new QWidget(this);
		settings_Window->setWindowTitle("Graphs Settings");
		settings_Window->setWindowModality(Qt::ApplicationModal);
		settings_Window->setAttribute(Qt::WA_DeleteOnClose);
		settings_Window->setWindowFlags(Qt::Window);
		settings_Window->show();

	QVBoxLayout* settings_Main_Layout = new QVBoxLayout(settings_Window);
		settings_Main_Layout->setSizeConstraint(QLayout::SetFixedSize);
		settings_Main_Layout->setSpacing(5);
		settings_Main_Layout->setContentsMargins(5,5,5,5);

	// settings group box
	QGroupBox* settings_Group_Box = new QGroupBox;
		settings_Group_Box->setObjectName("settings_Group_Box");
		settings_Group_Box->setStyleSheet("QGroupBox#settings_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 0ex;}"
													"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
	settings_Main_Layout->addWidget(settings_Group_Box);

	QGridLayout* settings_Group_Box_Layout = new QGridLayout(settings_Group_Box);
		settings_Group_Box_Layout->setContentsMargins(5,5,5,5);

	// num target rows
	QLabel* target_Rows_Label = new QLabel("Number of \"Measured\" rows");
	QSpinBox* target_Rows_SpinBox = new QSpinBox;
		target_Rows_SpinBox->setRange(1, total_Number_of_Target_Graphs[index]);
		target_Rows_SpinBox->setValue(multilayer->graph_Options_1D.num_Target_Graph_Rows);
		target_Rows_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		target_Rows_SpinBox->setAccelerated(true);
		target_Rows_SpinBox->setFixedWidth(25);

	settings_Group_Box_Layout->addWidget(target_Rows_Label,		0,0,1,1);
	settings_Group_Box_Layout->addWidget(target_Rows_SpinBox,	0,1,1,1);

	// num independent rows
	QLabel* independent_Rows_Label = new QLabel("Number of \"Independent\" rows");
	QSpinBox* independent_Rows_SpinBox = new QSpinBox;
		independent_Rows_SpinBox->setRange(1, total_Number_of_Independent_Graphs[index]);
		independent_Rows_SpinBox->setValue(multilayer->graph_Options_1D.num_Independent_Graph_Rows);
		independent_Rows_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		independent_Rows_SpinBox->setAccelerated(true);
		independent_Rows_SpinBox->setFixedWidth(25);

	settings_Group_Box_Layout->addWidget(independent_Rows_Label,	1,0,1,1);
	settings_Group_Box_Layout->addWidget(independent_Rows_SpinBox,	1,1,1,1);

	// additional settings group box
	QGroupBox* plots_Settings_Group_Box = new QGroupBox;
		plots_Settings_Group_Box->setObjectName("plots_Settings_Group_Box");
		plots_Settings_Group_Box->setStyleSheet("QGroupBox#plots_Settings_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 0ex;}"
																"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
	settings_Main_Layout->addWidget(plots_Settings_Group_Box);

	QVBoxLayout* plots_Settings_Group_Box_Layout = new QVBoxLayout(plots_Settings_Group_Box);
		plots_Settings_Group_Box_Layout->setContentsMargins(5,5,5,5);

	QCheckBox* show_Scatter_Size_CheckBox = new QCheckBox("Show plot symbol size");
		show_Scatter_Size_CheckBox->setChecked(multilayer->graph_Options_1D.show_Scatter);
//		show_Scatter_Size_CheckBox->setLayoutDirection(Qt::RightToLeft);
		plots_Settings_Group_Box_Layout->addWidget(show_Scatter_Size_CheckBox);

	QCheckBox* show_Thickness_CheckBox = new QCheckBox("Show plot line thickness");
		show_Thickness_CheckBox->setChecked(multilayer->graph_Options_1D.show_Thickness);
		plots_Settings_Group_Box_Layout->addWidget(show_Thickness_CheckBox);

	QCheckBox* show_X_Scale_CheckBox = new QCheckBox("Show X scale");
		show_X_Scale_CheckBox->setChecked(multilayer->graph_Options_1D.show_X_Scale);
		plots_Settings_Group_Box_Layout->addWidget(show_X_Scale_CheckBox);

	QCheckBox* show_Max_Value_CheckBox = new QCheckBox("Show max calc value");
		show_Max_Value_CheckBox->setChecked(multilayer->graph_Options_1D.show_Max_Value);
		plots_Settings_Group_Box_Layout->addWidget(show_Max_Value_CheckBox);

	QCheckBox* show_Range_CheckBox = new QCheckBox("Show Y range");
		show_Range_CheckBox->setChecked(multilayer->graph_Options_1D.show_Range);
		plots_Settings_Group_Box_Layout->addWidget(show_Range_CheckBox);

	QCheckBox* show_Current_Coordinate_CheckBox = new QCheckBox("Show cursor position");
		show_Current_Coordinate_CheckBox->setChecked(multilayer->graph_Options_1D.show_Current_Coordinate);
		plots_Settings_Group_Box_Layout->addWidget(show_Current_Coordinate_CheckBox);

	QCheckBox* show_Title_CheckBox = new QCheckBox("Show plot title");
		show_Title_CheckBox->setChecked(multilayer->graph_Options_1D.show_Title);
		plots_Settings_Group_Box_Layout->addWidget(show_Title_CheckBox);

	// buttons
	QHBoxLayout* buttons_Layout = new QHBoxLayout;
		buttons_Layout->setSpacing(10);
		buttons_Layout->setContentsMargins(0,0,0,0);

	QPushButton* ok_Button = new QPushButton ("OK");
	QPushButton* cancel_Button = new QPushButton("Cancel");
		buttons_Layout->addWidget(ok_Button);
		buttons_Layout->addWidget(cancel_Button);
	settings_Main_Layout->addLayout(buttons_Layout);

	connect(cancel_Button,  &QPushButton::clicked, this, [=]{settings_Window->close();});
	connect(ok_Button,      &QPushButton::clicked, this, [=]
	{
		multilayer->graph_Options_1D.num_Target_Graph_Rows = target_Rows_SpinBox->value();
		multilayer->graph_Options_1D.num_Independent_Graph_Rows = independent_Rows_SpinBox->value();

		// additional
		multilayer->graph_Options_1D.show_Scatter = show_Scatter_Size_CheckBox->isChecked();
		multilayer->graph_Options_1D.show_Thickness = show_Thickness_CheckBox->isChecked();
		multilayer->graph_Options_1D.show_X_Scale = show_X_Scale_CheckBox->isChecked();
		multilayer->graph_Options_1D.show_Max_Value = show_Max_Value_CheckBox->isChecked();
		multilayer->graph_Options_1D.show_Range = show_Range_CheckBox->isChecked();
		multilayer->graph_Options_1D.show_Current_Coordinate = show_Current_Coordinate_CheckBox->isChecked();
		multilayer->graph_Options_1D.show_Title = show_Title_CheckBox->isChecked();

		global_Multilayer_Approach->reopen_Optical_Graphs_1D(true);
		settings_Window->close();
	});
}

void Optical_Graphs::settings_2D()
{
	int index = main_Tabs->currentIndex();
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(index));

	QWidget* settings_Window = new QWidget(this);
		settings_Window->setWindowTitle("Graphs Settings");
		settings_Window->setWindowModality(Qt::ApplicationModal);
		settings_Window->setAttribute(Qt::WA_DeleteOnClose);
		settings_Window->setWindowFlags(Qt::Window);
		settings_Window->show();

	QVBoxLayout* settings_Main_Layout = new QVBoxLayout(settings_Window);
		settings_Main_Layout->setSizeConstraint(QLayout::SetFixedSize);
		settings_Main_Layout->setSpacing(5);
		settings_Main_Layout->setContentsMargins(5,5,5,5);

	// settings group box
	QGroupBox* settings_Group_Box = new QGroupBox;
		settings_Group_Box->setObjectName("settings_Group_Box_2D");
		settings_Group_Box->setStyleSheet("QGroupBox#settings_Group_Box_2D { border-radius: 2px;  border: 1px solid gray; margin-top: 0ex;}"
													"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
	settings_Main_Layout->addWidget(settings_Group_Box);

	QGridLayout* settings_Group_Box_Layout = new QGridLayout(settings_Group_Box);
		settings_Group_Box_Layout->setContentsMargins(5,5,5,5);

	// num target rows
	QLabel* target_Rows_Label = new QLabel("Number of \"Measured\" rows");
	QSpinBox* target_Rows_SpinBox = new QSpinBox;
		target_Rows_SpinBox->setRange(1, total_Number_of_Target_Graphs[index]);
		target_Rows_SpinBox->setValue(multilayer->graph_Options_2D.num_Target_Graph_Rows);
		target_Rows_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		target_Rows_SpinBox->setAccelerated(true);
		target_Rows_SpinBox->setFixedWidth(25);

	settings_Group_Box_Layout->addWidget(target_Rows_Label,		0,0,1,1);
	settings_Group_Box_Layout->addWidget(target_Rows_SpinBox,	0,1,1,1);

	// num independent rows
	QLabel* independent_Rows_Label = new QLabel("Number of \"Independent\" rows");
	QSpinBox* independent_Rows_SpinBox = new QSpinBox;
		independent_Rows_SpinBox->setRange(1, total_Number_of_Independent_Graphs[index]);
		independent_Rows_SpinBox->setValue(multilayer->graph_Options_2D.num_Independent_Graph_Rows);
		independent_Rows_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		independent_Rows_SpinBox->setAccelerated(true);
		independent_Rows_SpinBox->setFixedWidth(25);

	settings_Group_Box_Layout->addWidget(independent_Rows_Label,	1,0,1,1);
	settings_Group_Box_Layout->addWidget(independent_Rows_SpinBox,	1,1,1,1);

	// additional settings group box
	QGroupBox* plots_Settings_Group_Box = new QGroupBox;
		plots_Settings_Group_Box->setObjectName("plots_Settings_Group_Box_2D");
		plots_Settings_Group_Box->setStyleSheet("QGroupBox#plots_Settings_Group_Box_2D { border-radius: 2px;  border: 1px solid gray; margin-top: 0ex;}"
																"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
	settings_Main_Layout->addWidget(plots_Settings_Group_Box);

	QVBoxLayout* plots_Settings_Group_Box_Layout = new QVBoxLayout(plots_Settings_Group_Box);
		plots_Settings_Group_Box_Layout->setContentsMargins(5,5,5,5);

	QCheckBox* show_Current_Coordinate_CheckBox = new QCheckBox("Show value near cursor");
		show_Current_Coordinate_CheckBox->setChecked(multilayer->graph_Options_2D.show_Current_Coordinate);
	plots_Settings_Group_Box_Layout->addWidget(show_Current_Coordinate_CheckBox);

	QCheckBox* show_Title_CheckBox = new QCheckBox("Show plot title");
		show_Title_CheckBox->setChecked(multilayer->graph_Options_2D.show_Title);
	plots_Settings_Group_Box_Layout->addWidget(show_Title_CheckBox);

	// buttons
	QHBoxLayout* buttons_Layout = new QHBoxLayout;
		buttons_Layout->setSpacing(10);
		buttons_Layout->setContentsMargins(0,0,0,0);

	QPushButton* ok_Button = new QPushButton ("OK");
	QPushButton* cancel_Button = new QPushButton("Cancel");
		buttons_Layout->addWidget(ok_Button);
		buttons_Layout->addWidget(cancel_Button);
	settings_Main_Layout->addLayout(buttons_Layout);

	connect(cancel_Button,  &QPushButton::clicked, this, [=]{settings_Window->close();});
	connect(ok_Button,      &QPushButton::clicked, this, [=]
	{
		multilayer->graph_Options_2D.num_Target_Graph_Rows = target_Rows_SpinBox->value();
		multilayer->graph_Options_2D.num_Independent_Graph_Rows = independent_Rows_SpinBox->value();

		// additional
		multilayer->graph_Options_2D.show_Current_Coordinate = show_Current_Coordinate_CheckBox->isChecked();
		multilayer->graph_Options_2D.show_Title = show_Title_CheckBox->isChecked();

		global_Multilayer_Approach->reopen_Optical_Graphs_2D(true);
		settings_Window->close();
	});
}

void Optical_Graphs::closeEvent(QCloseEvent* event)
{
	if(dimension == dim_1D)	global_Multilayer_Approach->runned_Optical_Graphs_1D.remove(optical_Graphs_1D_Key);
	if(dimension == dim_2D)	global_Multilayer_Approach->runned_Optical_Graphs_2D.remove(optical_Graphs_2D_Key);
	global_Multilayer_Approach->unlock_Mainwindow_Interface();

	save_Geometry();
	event->accept();
}

void Optical_Graphs::save_Geometry()
{
	// frame geometry
	write_Window_Geometry();

	// splitters geometry
	for(int tab_Index=0; tab_Index<global_Multilayer_Approach->multilayer_Tabs->count(); ++tab_Index)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));

		// 1D
		if(dimension == dim_1D)
		{
			global_Multilayer_Approach->target_Independent_Splitter_Position_1D_Vec[tab_Index] = target_Independent_Splitter_Vec[tab_Index]->saveState();

			if(multilayer->graph_Options_1D.num_Target_Graph_Rows == global_Multilayer_Approach->target_Horizontal_Splitter_Position_1D_Vec_Vec[tab_Index].size())
			{
				global_Multilayer_Approach->target_Vertical_Splitter_Position_1D_Vec[tab_Index] = target_Vertical_Splitter_Vec[tab_Index]->saveState();
				for(int row=0; row<multilayer->graph_Options_1D.num_Target_Graph_Rows; row++)
				{
					global_Multilayer_Approach->target_Horizontal_Splitter_Position_1D_Vec_Vec[tab_Index][row] = target_Horizontal_Splitter_Vec_Vec[tab_Index][row]->saveState();
				}
			}

			if(multilayer->graph_Options_1D.num_Independent_Graph_Rows == global_Multilayer_Approach->independent_Horizontal_Splitter_Position_1D_Vec_Vec[tab_Index].size())
			{
				global_Multilayer_Approach->independent_Vertical_Splitter_Position_1D_Vec[tab_Index] = independent_Vertical_Splitter_Vec[tab_Index]->saveState();
				for(int row=0; row<multilayer->graph_Options_1D.num_Independent_Graph_Rows; row++)
				{
					global_Multilayer_Approach->independent_Horizontal_Splitter_Position_1D_Vec_Vec[tab_Index][row] = independent_Horizontal_Splitter_Vec_Vec[tab_Index][row]->saveState();
				}
			}
		}
		// 2D
		if(dimension == dim_2D)
		{
			global_Multilayer_Approach->target_Independent_Splitter_Position_2D_Vec[tab_Index] = target_Independent_Splitter_Vec[tab_Index]->saveState();

			if(multilayer->graph_Options_2D.num_Target_Graph_Rows == global_Multilayer_Approach->target_Horizontal_Splitter_Position_2D_Vec_Vec[tab_Index].size())
			{
				global_Multilayer_Approach->target_Vertical_Splitter_Position_2D_Vec[tab_Index] = target_Vertical_Splitter_Vec[tab_Index]->saveState();
				for(int row=0; row<multilayer->graph_Options_2D.num_Target_Graph_Rows; row++)
				{
					global_Multilayer_Approach->target_Horizontal_Splitter_Position_2D_Vec_Vec[tab_Index][row] = target_Horizontal_Splitter_Vec_Vec[tab_Index][row]->saveState();
				}
			}

			if(multilayer->graph_Options_2D.num_Independent_Graph_Rows == global_Multilayer_Approach->independent_Horizontal_Splitter_Position_2D_Vec_Vec[tab_Index].size())
			{
				global_Multilayer_Approach->independent_Vertical_Splitter_Position_2D_Vec[tab_Index] = independent_Vertical_Splitter_Vec[tab_Index]->saveState();
				for(int row=0; row<multilayer->graph_Options_2D.num_Independent_Graph_Rows; row++)
				{
					global_Multilayer_Approach->independent_Horizontal_Splitter_Position_2D_Vec_Vec[tab_Index][row] = independent_Horizontal_Splitter_Vec_Vec[tab_Index][row]->saveState();
				}
			}
		}
	}
}

void Optical_Graphs::create_Main_Layout()
{
	QWidget* main_Widget = this;
	if(make_all_windows_resizeable)
	{
		QVBoxLayout* top_Layout = new QVBoxLayout(this);
			top_Layout->setMargin(0);

		main_Widget = new QWidget;
		QScrollArea* scrollArea = new QScrollArea;
			scrollArea->setWidget(main_Widget);
			scrollArea->setWidgetResizable(true);
		top_Layout->addWidget(scrollArea);
	}

	main_Layout = new QVBoxLayout(main_Widget);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(0,0,0,0);

	global_Multilayer_Approach->lock_Mainwindow_Interface();
	create_Tabs();
		main_Layout->addWidget(main_Tabs);
	add_Tabs();

	// shortcuts
	Global_Variables::create_Shortcuts(this);
	connect(main_Tabs,	&QTabWidget::currentChanged, this, [=]
	{
		if(tab_synchronization)
		{
																										  {global_Multilayer_Approach->						  multilayer_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key)) {global_Multilayer_Approach->table_Of_Structures		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))	  {global_Multilayer_Approach->optical_Graphs_2D		  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Profile_Plots_Window.contains(profile_Plots_Key))		  {global_Multilayer_Approach->profile_Plots_Window       ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key)){global_Multilayer_Approach->calculation_Settings_Editor->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
		}
	});
}

void Optical_Graphs::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
		main_Tabs->setMovable(false);

	connect(main_Tabs,	&QTabWidget::currentChanged, this, [=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);
		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	});
}

void Optical_Graphs::add_Tabs()
{
	// 1D
	if(dimension == dim_1D)
	{
		global_Multilayer_Approach->target_Independent_Splitter_Position_1D_Vec.resize		   (global_Multilayer_Approach->multilayer_Tabs->count());
		global_Multilayer_Approach->target_Vertical_Splitter_Position_1D_Vec.resize			   (global_Multilayer_Approach->multilayer_Tabs->count());
		global_Multilayer_Approach->target_Horizontal_Splitter_Position_1D_Vec_Vec.resize	   (global_Multilayer_Approach->multilayer_Tabs->count());
		global_Multilayer_Approach->independent_Vertical_Splitter_Position_1D_Vec.resize	   (global_Multilayer_Approach->multilayer_Tabs->count());
		global_Multilayer_Approach->independent_Horizontal_Splitter_Position_1D_Vec_Vec.resize (global_Multilayer_Approach->multilayer_Tabs->count());
	}
	// 2D
	if(dimension == dim_2D)
	{
		global_Multilayer_Approach->target_Independent_Splitter_Position_2D_Vec.resize		   (global_Multilayer_Approach->multilayer_Tabs->count());
		global_Multilayer_Approach->target_Vertical_Splitter_Position_2D_Vec.resize			   (global_Multilayer_Approach->multilayer_Tabs->count());
		global_Multilayer_Approach->target_Horizontal_Splitter_Position_2D_Vec_Vec.resize	   (global_Multilayer_Approach->multilayer_Tabs->count());
		global_Multilayer_Approach->independent_Vertical_Splitter_Position_2D_Vec.resize	   (global_Multilayer_Approach->multilayer_Tabs->count());
		global_Multilayer_Approach->independent_Horizontal_Splitter_Position_2D_Vec_Vec.resize (global_Multilayer_Approach->multilayer_Tabs->count());
	}

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

void Optical_Graphs::create_Tab_Content(QWidget* new_Widget, int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));

	QVBoxLayout* tab_Layout = new QVBoxLayout(new_Widget);

	QSplitter* target_Independent_Splitter = new QSplitter;
		target_Independent_Splitter->setOrientation(Qt::Vertical);
		target_Independent_Splitter->setStyleSheet("QSplitter::handle{border: 0px solid light gray; background: none;}");
	tab_Layout->addWidget(target_Independent_Splitter);
	target_Independent_Splitter_Vec[tab_Index] = target_Independent_Splitter;

	QVector<Curve_Plot_1D*> tab_Plots_1D;
	QVector<Curve_Plot_2D*> tab_Plots_2D;
	// target
	{
		// calculate total number of graphs
		total_Number_of_Target_Graphs[tab_Index] = 0;
		QVector<Target_Curve*> target_Profiles_to_Show;

		if( multilayer->enable_Calc_Target_Curves )
		for(Target_Curve* target_Curve : multilayer->target_Profiles_Vector)
		{
			if(target_Curve->loaded_And_Ready)
			if(target_Curve->fit_Params.calculate)
			{
				if(dimension == dim_1D && target_Curve->measurement.measurement_Type != measurement_Types[GISAS_Map])
				{
					target_Profiles_to_Show.append(target_Curve);
					total_Number_of_Target_Graphs[tab_Index]++;
				}
				if(dimension == dim_2D && target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
				{
					target_Profiles_to_Show.append(target_Curve);
					total_Number_of_Target_Graphs[tab_Index]++;
				}
			}
		}

		// prepare box
		QGroupBox* target_Group_Box = new QGroupBox("Measured", this);
			target_Group_Box->setObjectName("target_Group_Box");
			target_Group_Box->setStyleSheet("QGroupBox#target_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
													"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		target_Independent_Splitter->addWidget(target_Group_Box);

		QHBoxLayout* target_Layout = new QHBoxLayout(target_Group_Box);
			target_Layout->setSpacing(0);
			target_Layout->setContentsMargins(0,3,0,3);

		MySplitter* target_Vertical_Splitter = new MySplitter;
			target_Vertical_Splitter->setOrientation(Qt::Vertical);
			target_Vertical_Splitter->setChildrenCollapsible(false);
			target_Vertical_Splitter->setHandleWidth(1);
			target_Vertical_Splitter->setStyleSheet("QSplitter::handle{border: 0px solid gray; background: gray;}");
		target_Layout->addWidget(target_Vertical_Splitter);	
		target_Vertical_Splitter_Vec[tab_Index] = target_Vertical_Splitter;

		int num_Target_Rows = dimension == dim_1D ? multilayer->graph_Options_1D.num_Target_Graph_Rows : multilayer->graph_Options_2D.num_Target_Graph_Rows;

		target_Horizontal_Splitter_Vec_Vec[tab_Index].resize(num_Target_Rows);
		for(int row=0; row<num_Target_Rows; row++)
		{
			QSplitter* target_Horizontal_Splitter = new QSplitter;
				target_Horizontal_Splitter->setOrientation(Qt::Horizontal);
				target_Horizontal_Splitter->setChildrenCollapsible(false);
				target_Horizontal_Splitter->setHandleWidth(1);
				target_Horizontal_Splitter->setStyleSheet("QSplitter::handle{border: 0px solid gray; background: gray;}");
			target_Horizontal_Splitter_Vec_Vec[tab_Index][row] = target_Horizontal_Splitter;
			target_Vertical_Splitter->addWidget(target_Horizontal_Splitter);
		}

		// fill splitters with graphs
		int graphs_in_Short_Row = total_Number_of_Target_Graphs[tab_Index]/num_Target_Rows;
		int additional_Graphs = total_Number_of_Target_Graphs[tab_Index]%num_Target_Rows;
		int first_Long_Row_Index = num_Target_Rows-additional_Graphs;

		int current_Row = 0;
		int graphs_in_Filled_Rows = 0;
		int length = -2018;

		for(int graph_Index=0; graph_Index<total_Number_of_Target_Graphs[tab_Index]; graph_Index++)
		{
			if(	dimension == dim_1D )
			{
				Curve_Plot_1D* new_Curve_Plot_1D = new Curve_Plot_1D(multilayer, target_Profiles_to_Show[graph_Index], nullptr, TARGET);
				meas_Id_Curve_1D_Map.insert(target_Profiles_to_Show[graph_Index]->measurement.id, new_Curve_Plot_1D);
				tab_Plots_1D.append(new_Curve_Plot_1D);
				target_Horizontal_Splitter_Vec_Vec[tab_Index][current_Row]->addWidget(new_Curve_Plot_1D);
			}
			if(	dimension == dim_2D )
			{
				Curve_Plot_2D* new_Curve_Plot_2D = new Curve_Plot_2D(multilayer, target_Profiles_to_Show[graph_Index], nullptr, TARGET);
				meas_Id_Curve_2D_Map.insert(target_Profiles_to_Show[graph_Index]->measurement.id, new_Curve_Plot_2D);
				tab_Plots_2D.append(new_Curve_Plot_2D);
				target_Horizontal_Splitter_Vec_Vec[tab_Index][current_Row]->addWidget(new_Curve_Plot_2D);
			}
			if(current_Row < first_Long_Row_Index) {length = graphs_in_Short_Row;} else {length = graphs_in_Short_Row + 1;}

			if(graph_Index == graphs_in_Filled_Rows+length-1)
			{
				graphs_in_Filled_Rows += length;
				current_Row++;
			}
		}
		target_GroupBox_Vec.append(target_Group_Box);
	}
	// independent
	{
		// calculate total number of graphs
		total_Number_of_Independent_Graphs[tab_Index] = 0;
		QVector<Independent_Curve*> independent_Profiles_to_Show;

		if(multilayer->enable_Calc_Independent_Curves)
		for(int i=0; i<multilayer->independent_Curve_Tabs->count(); ++i)
		{
			Independent_Curve* independent_Variables = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(i));
			if( independent_Variables->calc_Functions.check_Enabled	)
			{
				if(	dimension == dim_1D && independent_Variables->calc_Functions.if_Something_Enabled_1D() )
				{
					independent_Profiles_to_Show.append(independent_Variables);
					total_Number_of_Independent_Graphs[tab_Index]++;
				}
				if(	dimension == dim_2D && independent_Variables->calc_Functions.if_Something_Enabled_2D() )
				{
					independent_Profiles_to_Show.append(independent_Variables);
					total_Number_of_Independent_Graphs[tab_Index]++;
				}
			}
		}

		// prepare box
		QGroupBox* independent_Group_Box = new QGroupBox("Independent", this);
			independent_Group_Box->setObjectName("independent_Group_Box");
			independent_Group_Box->setStyleSheet("QGroupBox#independent_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
															  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		target_Independent_Splitter->addWidget(independent_Group_Box);

		QHBoxLayout* independent_Layout = new QHBoxLayout(independent_Group_Box);
			independent_Layout->setSpacing(0);
			independent_Layout->setContentsMargins(0,8,0,3);

		QSplitter* independent_Vertical_Splitter = new QSplitter;
			independent_Vertical_Splitter->setOrientation(Qt::Vertical);
			independent_Vertical_Splitter->setChildrenCollapsible(false);
			independent_Vertical_Splitter->setHandleWidth(1);
			independent_Vertical_Splitter->setStyleSheet("QSplitter::handle{border: 0px solid gray; background: gray;}");
		independent_Layout->addWidget(independent_Vertical_Splitter);
		independent_Vertical_Splitter_Vec[tab_Index] = independent_Vertical_Splitter;

		int num_Independent_Rows = dimension == dim_1D ? multilayer->graph_Options_1D.num_Independent_Graph_Rows : multilayer->graph_Options_2D.num_Independent_Graph_Rows;

		independent_Horizontal_Splitter_Vec_Vec[tab_Index].resize(num_Independent_Rows);
		for(int row=0; row<num_Independent_Rows; row++)
		{
			QSplitter* independent_Horizontal_Splitter = new QSplitter;
				independent_Horizontal_Splitter->setOrientation(Qt::Horizontal);
				independent_Horizontal_Splitter->setChildrenCollapsible(false);
				independent_Horizontal_Splitter->setHandleWidth(1);
				independent_Horizontal_Splitter->setStyleSheet("QSplitter::handle{border: 0px solid gray; background: gray;}");
			independent_Horizontal_Splitter_Vec_Vec[tab_Index][row] = independent_Horizontal_Splitter;
			independent_Vertical_Splitter->addWidget(independent_Horizontal_Splitter);
		}

		// fill splitters with graphs
		int graphs_in_Short_Row = total_Number_of_Independent_Graphs[tab_Index]/num_Independent_Rows;
		int additional_Graphs = total_Number_of_Independent_Graphs[tab_Index]%num_Independent_Rows;
		int first_Long_Row_Index = num_Independent_Rows-additional_Graphs;

		int current_Row = 0;
		int graphs_in_Filled_Rows = 0;
		int length = -2018;

		for(int graph_Index=0; graph_Index<total_Number_of_Independent_Graphs[tab_Index]; graph_Index++)
		{
			if(	dimension == dim_1D )
			{
				Curve_Plot_1D* new_Curve_Plot_1D = new Curve_Plot_1D(multilayer, nullptr, independent_Profiles_to_Show[graph_Index], INDEPENDENT);
				meas_Id_Curve_1D_Map.insert(independent_Profiles_to_Show[graph_Index]->measurement.id, new_Curve_Plot_1D);
				tab_Plots_1D.append(new_Curve_Plot_1D);
				independent_Horizontal_Splitter_Vec_Vec[tab_Index][current_Row]->addWidget(new_Curve_Plot_1D);
			}
			if(	dimension == dim_2D )
			{
				Curve_Plot_2D* new_Curve_Plot_2D = new Curve_Plot_2D(multilayer, nullptr, independent_Profiles_to_Show[graph_Index], INDEPENDENT);
				meas_Id_Curve_2D_Map.insert(independent_Profiles_to_Show[graph_Index]->measurement.id, new_Curve_Plot_2D);
				tab_Plots_2D.append(new_Curve_Plot_2D);
				independent_Horizontal_Splitter_Vec_Vec[tab_Index][current_Row]->addWidget(new_Curve_Plot_2D);
			}
			if(current_Row < first_Long_Row_Index) {length = graphs_in_Short_Row;} else {length = graphs_in_Short_Row + 1;}

			if(graph_Index == graphs_in_Filled_Rows+length-1)
			{
				graphs_in_Filled_Rows += length;
				current_Row++;
			}
		}
		independent_GroupBox_Vec.append(independent_Group_Box);
	}

	if(	dimension == dim_1D ) plots_1D.append(tab_Plots_1D);
	if(	dimension == dim_2D ) plots_2D.append(tab_Plots_2D);

	// restore splitter positions
	if(	dimension == dim_1D ) global_Multilayer_Approach->target_Horizontal_Splitter_Position_1D_Vec_Vec     [tab_Index].resize(multilayer->graph_Options_1D.num_Target_Graph_Rows);
	if(	dimension == dim_2D ) global_Multilayer_Approach->target_Horizontal_Splitter_Position_2D_Vec_Vec     [tab_Index].resize(multilayer->graph_Options_2D.num_Target_Graph_Rows);
	if(	dimension == dim_1D ) global_Multilayer_Approach->independent_Horizontal_Splitter_Position_1D_Vec_Vec[tab_Index].resize(multilayer->graph_Options_1D.num_Independent_Graph_Rows);
	if(	dimension == dim_2D ) global_Multilayer_Approach->independent_Horizontal_Splitter_Position_2D_Vec_Vec[tab_Index].resize(multilayer->graph_Options_2D.num_Independent_Graph_Rows);

	if(keep_Splitter != TARGET_AND_INDEPENDENT)
	{
		if(	dimension == dim_1D )
		{
			if(!global_Multilayer_Approach->independent_Target_Added_1D)
			{
				target_Independent_Splitter_Vec[tab_Index]->restoreState(global_Multilayer_Approach->target_Independent_Splitter_Position_1D_Vec[tab_Index]);
			}
			if(total_Number_of_Target_Graphs[tab_Index] == 0)		target_Independent_Splitter_Vec[tab_Index]->widget(0)->setHidden(true);
			if(total_Number_of_Independent_Graphs[tab_Index] == 0)	target_Independent_Splitter_Vec[tab_Index]->widget(1)->setHidden(true);
		}
		if(	dimension == dim_2D )
		{
			if(!global_Multilayer_Approach->independent_Target_Added_2D)
			{
				target_Independent_Splitter_Vec[tab_Index]->restoreState(global_Multilayer_Approach->target_Independent_Splitter_Position_2D_Vec[tab_Index]);
			}
			if(total_Number_of_Target_Graphs[tab_Index] == 0)		target_Independent_Splitter_Vec[tab_Index]->widget(0)->setHidden(true);
			if(total_Number_of_Independent_Graphs[tab_Index] == 0)	target_Independent_Splitter_Vec[tab_Index]->widget(1)->setHidden(true);
		}

		if(keep_Splitter != TARGET)
		{
			if(	dimension == dim_1D )
			{
				target_Vertical_Splitter_Vec[tab_Index]->restoreState(global_Multilayer_Approach->target_Vertical_Splitter_Position_1D_Vec[tab_Index]);
				for(int row=0; row<multilayer->graph_Options_1D.num_Target_Graph_Rows; row++)
				{
					if(!global_Multilayer_Approach->target_Added_1D)
					{
						target_Horizontal_Splitter_Vec_Vec[tab_Index][row]->restoreState(global_Multilayer_Approach->target_Horizontal_Splitter_Position_1D_Vec_Vec[tab_Index][row]);
					}
				}
			}
			if(	dimension == dim_2D )
			{
				target_Vertical_Splitter_Vec[tab_Index]->restoreState(global_Multilayer_Approach->target_Vertical_Splitter_Position_2D_Vec[tab_Index]);
				for(int row=0; row<multilayer->graph_Options_2D.num_Target_Graph_Rows; row++)
				{
					if(!global_Multilayer_Approach->target_Added_2D)
					{
						target_Horizontal_Splitter_Vec_Vec[tab_Index][row]->restoreState(global_Multilayer_Approach->target_Horizontal_Splitter_Position_2D_Vec_Vec[tab_Index][row]);
					}
				}
			}
		}
		if(keep_Splitter != INDEPENDENT)
		{
			if(	dimension == dim_1D )
			{
				independent_Vertical_Splitter_Vec[tab_Index]->restoreState(global_Multilayer_Approach->independent_Vertical_Splitter_Position_1D_Vec[tab_Index]);
				for(int row=0; row<multilayer->graph_Options_1D.num_Independent_Graph_Rows; row++)
				{
					if(!global_Multilayer_Approach->independent_Added_1D)
					{
						independent_Horizontal_Splitter_Vec_Vec[tab_Index][row]->restoreState(global_Multilayer_Approach->independent_Horizontal_Splitter_Position_1D_Vec_Vec[tab_Index][row]);
					}
				}
			}
			if(	dimension == dim_2D )
			{
				independent_Vertical_Splitter_Vec[tab_Index]->restoreState(global_Multilayer_Approach->independent_Vertical_Splitter_Position_2D_Vec[tab_Index]);
				for(int row=0; row<multilayer->graph_Options_2D.num_Independent_Graph_Rows; row++)
				{
					if(!global_Multilayer_Approach->independent_Added_2D)
					{
						independent_Horizontal_Splitter_Vec_Vec[tab_Index][row]->restoreState(global_Multilayer_Approach->independent_Horizontal_Splitter_Position_2D_Vec_Vec[tab_Index][row]);
					}
				}
			}
		}
	}
	if(	dimension == dim_1D )
	{
		global_Multilayer_Approach->independent_Target_Added_1D = false;
		global_Multilayer_Approach->independent_Added_1D = false;
		global_Multilayer_Approach->target_Added_1D = false;
	}
	if(	dimension == dim_2D )
	{
		global_Multilayer_Approach->independent_Target_Added_2D = false;
		global_Multilayer_Approach->independent_Added_2D = false;
		global_Multilayer_Approach->target_Added_2D = false;
	}
}

void Optical_Graphs::set_Window_Geometry()
{
	int width_add = 0, height_add = 0;
	if(make_all_windows_resizeable && !previous_all_windows_resizeable) {width_add+=2; height_add+=2;}

	if(dimension == dim_1D) setGeometry(graphs_x_corner_1D,graphs_y_corner_1D,graphs_width_1D+width_add,graphs_height_1D+height_add);
	if(dimension == dim_2D) setGeometry(graphs_x_corner_2D,graphs_y_corner_2D,graphs_width_2D+width_add,graphs_height_2D+height_add);
}

void Optical_Graphs::write_Window_Geometry()
{
	if(!isMaximized())
	{
		if(dimension == dim_1D)
		{
			graphs_x_corner_1D = frameGeometry().x()-corner_x_shift;
			graphs_y_corner_1D = frameGeometry().y()-corner_y_shift;

			graphs_width_1D  = geometry().width() +  width_add;
			graphs_height_1D = geometry().height()+ height_add;
		}
		if(dimension == dim_2D)
		{
			graphs_x_corner_2D = frameGeometry().x()-corner_x_shift;
			graphs_y_corner_2D = frameGeometry().y()-corner_y_shift;

			graphs_width_2D  = geometry().width() +  width_add;
			graphs_height_2D = geometry().height()+ height_add;
		}
	}
}
