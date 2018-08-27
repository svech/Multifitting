// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "optical_graphs.h"

Optical_Graphs::Optical_Graphs(QString keep_Splitter, QWidget* parent) :
	keep_Splitter(keep_Splitter),
	target_Independent_Splitter_Vec        (global_Multilayer_Approach->multilayer_Tabs->count()),
	target_Vertical_Splitter_Vec		   (global_Multilayer_Approach->multilayer_Tabs->count()),
	target_Horizontal_Splitter_Vec_Vec	   (global_Multilayer_Approach->multilayer_Tabs->count()),
	independent_Vertical_Splitter_Vec	   (global_Multilayer_Approach->multilayer_Tabs->count()),
	independent_Horizontal_Splitter_Vec_Vec(global_Multilayer_Approach->multilayer_Tabs->count()),
	QWidget(parent) // nullptr!
{
	setWindowTitle("Plots");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Optical_Graphs::closeEvent(QCloseEvent* event)
{
	global_Multilayer_Approach->runned_Optical_Graphs.remove(optical_Graphs_Key);
	unlock_Mainwindow_Interface();
	write_Window_Geometry();

	// save splitters geometry
	for(int tab_Index=0; tab_Index<global_Multilayer_Approach->multilayer_Tabs->count(); ++tab_Index)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));

		global_Multilayer_Approach->target_Independent_Splitter_Position_Vec[tab_Index] = target_Independent_Splitter_Vec[tab_Index]->saveState();
		global_Multilayer_Approach->target_Vertical_Splitter_Position_Vec[tab_Index] = target_Vertical_Splitter_Vec[tab_Index]->saveState();
		for(int row=0; row<multilayer->num_Target_Graph_Rows; row++)			{
			global_Multilayer_Approach->target_Horizontal_Splitter_Position_Vec_Vec[tab_Index][row] = target_Horizontal_Splitter_Vec_Vec[tab_Index][row]->saveState();
		}
		global_Multilayer_Approach->independent_Vertical_Splitter_Position_Vec[tab_Index] = independent_Vertical_Splitter_Vec[tab_Index]->saveState();
		for(int row=0; row<multilayer->num_Independent_Graph_Rows; row++)		{
			global_Multilayer_Approach->independent_Horizontal_Splitter_Position_Vec_Vec[tab_Index][row] = independent_Horizontal_Splitter_Vec_Vec[tab_Index][row]->saveState();
		}
	}

	event->accept();
}

void Optical_Graphs::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(0,0,0,0);

	lock_Mainwindow_Interface();
	create_Tabs();
		main_Layout->addWidget(main_Tabs);
	add_Tabs();

	// shortcuts
	{
		QShortcut* save_Shortcut			= new QShortcut(QKeySequence(Qt::Key_S | Qt::CTRL), this);
		QShortcut* open_Shortcut			= new QShortcut(QKeySequence(Qt::Key_O | Qt::CTRL), this);
		QShortcut* fit_Shortcut				= new QShortcut(QKeySequence(Qt::Key_F | Qt::CTRL | Qt::SHIFT), this);
		QShortcut* calc_Specular_Shortcut	= new QShortcut(QKeySequence(Qt::Key_C | Qt::CTRL | Qt::SHIFT), this);

		connect(save_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->save(default_File);});
		connect(open_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->open(default_File);});
		connect(fit_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->start_Fitting();	  });
		connect(calc_Specular_Shortcut, &QShortcut::activated, this, [=]{ global_Multilayer_Approach->calc_Reflection(); });
	}
}

void Optical_Graphs::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(false);

	connect(main_Tabs,	&QTabWidget::currentChanged,
	[=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);

		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index)main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	});
}

void Optical_Graphs::add_Tabs()
{
	global_Multilayer_Approach->target_Independent_Splitter_Position_Vec.resize		   (global_Multilayer_Approach->multilayer_Tabs->count());
	global_Multilayer_Approach->target_Vertical_Splitter_Position_Vec.resize		   (global_Multilayer_Approach->multilayer_Tabs->count());
	global_Multilayer_Approach->target_Horizontal_Splitter_Position_Vec_Vec.resize	   (global_Multilayer_Approach->multilayer_Tabs->count());
	global_Multilayer_Approach->independent_Vertical_Splitter_Position_Vec.resize	   (global_Multilayer_Approach->multilayer_Tabs->count());
	global_Multilayer_Approach->independent_Horizontal_Splitter_Position_Vec_Vec.resize(global_Multilayer_Approach->multilayer_Tabs->count());

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

	// target
	{
		QGroupBox* target_Group_Box = new QGroupBox("Measured", this);
			target_Group_Box->setObjectName("target_Group_Box");
			target_Group_Box->setStyleSheet("QGroupBox#target_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
													"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		target_Independent_Splitter->addWidget(target_Group_Box);

		QHBoxLayout* target_Layout = new QHBoxLayout(target_Group_Box);
		target_Layout->setSpacing(0);
		target_Layout->setContentsMargins(0,2,0,3);

		QSplitter* target_Vertical_Splitter = new QSplitter;
		target_Vertical_Splitter->setOrientation(Qt::Vertical);
		target_Vertical_Splitter->setHandleWidth(1);
		target_Vertical_Splitter->setStyleSheet("QSplitter::handle{border: 0px solid gray; background: gray;}");
		target_Layout->addWidget(target_Vertical_Splitter);
		target_Vertical_Splitter_Vec[tab_Index] = target_Vertical_Splitter;
		for(int row=0; row<multilayer->num_Target_Graph_Rows; row++)
		{
			QSplitter* target_Horizontal_Splitter = new QSplitter;
			target_Horizontal_Splitter->setOrientation(Qt::Horizontal);
			target_Horizontal_Splitter->setHandleWidth(1);
			target_Horizontal_Splitter->setStyleSheet("QSplitter::handle{border: 0px solid gray; background: gray;}");
			target_Horizontal_Splitter_Vec_Vec[tab_Index][row] = target_Horizontal_Splitter;
			target_Vertical_Splitter->addWidget(target_Horizontal_Splitter);
		}

		// fill splitters with graphs
		int current_Graph = 0;
		if( multilayer->enable_Calc_Target_Curves )
		for(Target_Curve* target_Curve : multilayer->target_Profiles_Vector)
		{
			if(target_Curve->loaded_And_Ready)
			if(target_Curve->fit_Params.calc)
			{
				Curve_Plot* new_Curve_Plot = new Curve_Plot(target_Curve, nullptr, TARGET, this);

				int current_Row = current_Graph/multilayer->num_Target_Graph_Rows;
				qInfo() << "current_Row="<<current_Row;
				current_Graph++;

				target_Horizontal_Splitter_Vec_Vec[tab_Index][current_Row]->addWidget(new_Curve_Plot);
				plots.append(new_Curve_Plot);
			}
		}
		target_GroupBox_Vec.append(target_Group_Box);
	}

	// independent
	{
		QGroupBox* independent_Group_Box = new QGroupBox("Independent", this);
			independent_Group_Box->setObjectName("independent_Group_Box");
			independent_Group_Box->setStyleSheet("QGroupBox#independent_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
															  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		target_Independent_Splitter->addWidget(independent_Group_Box);

		QHBoxLayout* independent_Layout = new QHBoxLayout(independent_Group_Box);
		independent_Layout->setSpacing(0);
		independent_Layout->setContentsMargins(0,2,0,3);

		QSplitter* independent_Vertical_Splitter = new QSplitter;
		independent_Vertical_Splitter->setOrientation(Qt::Horizontal);
		independent_Vertical_Splitter->setHandleWidth(1);
		independent_Vertical_Splitter->setStyleSheet("QSplitter::handle{border: 0px solid gray; background: gray;}");
		independent_Layout->addWidget(independent_Vertical_Splitter);
		independent_Vertical_Splitter_Vec[tab_Index] = independent_Vertical_Splitter;
		for(int row=0; row<multilayer->num_Independent_Graph_Rows; row++)
		{
			QSplitter* independent_Horizontal_Splitter = new QSplitter;
			independent_Horizontal_Splitter->setOrientation(Qt::Horizontal);
			independent_Horizontal_Splitter->setHandleWidth(1);
			independent_Horizontal_Splitter->setStyleSheet("QSplitter::handle{border: 0px solid gray; background: gray;}");
			independent_Horizontal_Splitter_Vec_Vec[tab_Index][row] = independent_Horizontal_Splitter;
			independent_Vertical_Splitter->addWidget(independent_Horizontal_Splitter);
		}

		// fill splitters with graphs
		int current_Graph = 0;
		if(multilayer->enable_Calc_Independent_Curves)
		for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
		{
			Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));
			if( independent_Variables->calc_Functions.check_Enabled	)
			if(	independent_Variables->calc_Functions.if_Something_Enabled() )
			{
				Curve_Plot* new_Curve_Plot = new Curve_Plot(nullptr, independent_Variables, INDEPENDENT, this);

				int current_Row = current_Graph/multilayer->num_Independent_Graph_Rows;
				current_Graph++;

				independent_Horizontal_Splitter_Vec_Vec[tab_Index][current_Row]->addWidget(new_Curve_Plot);
				plots.append(new_Curve_Plot);
			}
		}
		independent_GroupBox_Vec.append(independent_Group_Box);
	}

	// restore splitter positions
//	global_Multilayer_Approach->target_Independent_Splitter_Position_Vec.resize		   (global_Multilayer_Approach->multilayer_Tabs->count());
//	global_Multilayer_Approach->target_Vertical_Splitter_Position_Vec.resize		   (global_Multilayer_Approach->multilayer_Tabs->count());
//	global_Multilayer_Approach->target_Horizontal_Splitter_Position_Vec_Vec.resize	   (global_Multilayer_Approach->multilayer_Tabs->count());
//	global_Multilayer_Approach->independent_Vertical_Splitter_Position_Vec.resize	   (global_Multilayer_Approach->multilayer_Tabs->count());
//	global_Multilayer_Approach->independent_Horizontal_Splitter_Position_Vec_Vec.resize(global_Multilayer_Approach->multilayer_Tabs->count());

	global_Multilayer_Approach->target_Horizontal_Splitter_Position_Vec_Vec     [tab_Index].resize(multilayer->num_Target_Graph_Rows);
	global_Multilayer_Approach->independent_Horizontal_Splitter_Position_Vec_Vec[tab_Index].resize(multilayer->num_Independent_Graph_Rows);

	{
										  target_Independent_Splitter_Vec  [tab_Index]->restoreState(global_Multilayer_Approach->target_Independent_Splitter_Position_Vec  [tab_Index]);
		if(keep_Splitter != TARGET)
		{
			target_Vertical_Splitter_Vec[tab_Index]->restoreState(global_Multilayer_Approach->target_Vertical_Splitter_Position_Vec[tab_Index]);

			for(int row=0; row<multilayer->num_Target_Graph_Rows; row++)
			{
				target_Horizontal_Splitter_Vec_Vec[tab_Index][row]->restoreState(global_Multilayer_Approach->target_Horizontal_Splitter_Position_Vec_Vec[tab_Index][row]);
			}
		}
		if(keep_Splitter != INDEPENDENT)
		{
			independent_Vertical_Splitter_Vec[tab_Index]->restoreState(global_Multilayer_Approach->independent_Vertical_Splitter_Position_Vec[tab_Index]);

			for(int row=0; row<multilayer->num_Independent_Graph_Rows; row++)
			{
				independent_Horizontal_Splitter_Vec_Vec[tab_Index][row]->restoreState(global_Multilayer_Approach->independent_Horizontal_Splitter_Position_Vec_Vec[tab_Index][row]);
			}
		}
	}
}

void Optical_Graphs::set_Window_Geometry()
{
	setGeometry(graphs_x_corner,graphs_y_corner,graphs_width,graphs_height);
}

void Optical_Graphs::write_Window_Geometry()
{
	if(!isMaximized())
	{
		graphs_x_corner = geometry().x()-WINDOW_BOUNDARY_SHIFT_X;
		graphs_y_corner = geometry().y()-WINDOW_BOUNDARY_SHIFT_Y;

		graphs_width  = geometry().width();
		graphs_height = geometry().height();
	}
}

void Optical_Graphs::lock_Mainwindow_Interface()
{
	global_Multilayer_Approach->calculation_Settings_Editor->lock_Mainwindow_Interface();
}

void Optical_Graphs::unlock_Mainwindow_Interface()
{
	global_Multilayer_Approach->calculation_Settings_Editor->unlock_Mainwindow_Interface();
}
