#include "optical_graphs.h"

Optical_Graphs::Optical_Graphs(QWidget* parent) :
	QWidget(parent)
{
	setWindowTitle("Plots");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Optical_Graphs::closeEvent(QCloseEvent* event)
{
	global_Multilayer_Approach->runned_Optical_Graphs.remove(optical_Graphs_Key);
	event->accept();
}

void Optical_Graphs::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	create_Tabs();
		main_Layout->addWidget(main_Tabs);
	add_Tabs();
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

	QSplitter* splitter = new QSplitter;
	splitter->setOrientation(Qt::Vertical);
	tab_Layout->addWidget(splitter);
	splitter_Vec.append(splitter);

	{
		QGroupBox* target_Group_Box = new QGroupBox("Measured", this);
			target_Group_Box->setObjectName("target_Group_Box");
			target_Group_Box->setStyleSheet("QGroupBox#target_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
													"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		splitter->addWidget(target_Group_Box);

		QHBoxLayout* target_Layout = new QHBoxLayout(target_Group_Box);
		target_Layout->setSpacing(0);
		target_Layout->setContentsMargins(0,2,0,3);

		if( multilayer->enable_Calc_Target_Curves )
		for(Target_Curve* target_Curve : multilayer->target_Profiles_Vector)
		{
			if(target_Curve->loaded_And_Ready)
			if(target_Curve->fit_Params.calc)
			{
				Curve_Plot* new_Curve_Plot = new Curve_Plot(target_Curve, NULL, TARGET, this);
				target_Layout->addWidget(new_Curve_Plot);
			}
		}

		target_GroupBox_Vec.append(target_Group_Box);
	}
	{
		QGroupBox* independent_Group_Box = new QGroupBox("Independent", this);
			independent_Group_Box->setObjectName("independent_Group_Box");
			independent_Group_Box->setStyleSheet("QGroupBox#independent_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
															  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		splitter->addWidget(independent_Group_Box);

		QHBoxLayout* independent_Layout = new QHBoxLayout(independent_Group_Box);
		independent_Layout->setSpacing(0);
		independent_Layout->setContentsMargins(0,2,0,3);

		if(multilayer->enable_Calc_Independent_Curves)
		for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
		{
			Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));
			if( independent_Variables->calc_Functions.check_Enabled	)
			if(	independent_Variables->calc_Functions.if_Something_Enabled() )
			{
				Curve_Plot* new_Curve_Plot = new Curve_Plot(NULL, independent_Variables, INDEPENDENT, this);
				independent_Layout->addWidget(new_Curve_Plot);
			}
		}

		independent_GroupBox_Vec.append(independent_Group_Box);
	}
}

void Optical_Graphs::set_Window_Geometry()
{
	setGeometry(400,400,600,500);
}
