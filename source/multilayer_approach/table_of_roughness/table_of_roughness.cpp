#include "table_of_roughness.h"

Table_Of_Roughness::Table_Of_Roughness(QWidget *parent) : QWidget(parent)
{
	setWindowTitle("Table Of Structures");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Table_Of_Roughness::closeEvent(QCloseEvent *event)
{
	global_Multilayer_Approach->runned_Tables_Of_Roughness.remove(table_Of_Roughness_Key);
	global_Multilayer_Approach->unlock_Mainwindow_Interface();
	write_Window_Geometry();
	event->accept();
}

void Table_Of_Roughness::create_Main_Layout()
{
	can_Change_Index = false;
	main_Layout = new QVBoxLayout(this);
	main_Layout->setContentsMargins(0,0,0,0);

	global_Multilayer_Approach->lock_Mainwindow_Interface();
	create_Menu();
	create_Tabs();
		main_Layout->addWidget(main_Tabs);
	add_Tabs();

	can_Change_Index = tab_synchronization;
}

void Table_Of_Roughness::set_Window_Geometry()
{
	setGeometry(roughness_table_x_corner,roughness_table_y_corner,roughness_table_width,roughness_table_height);
}

void Table_Of_Roughness::write_Window_Geometry()
{
	// usual show
	if(!isMaximized())
	{
		roughness_table_x_corner = frameGeometry().x()-corner_x_shift;
		roughness_table_y_corner = frameGeometry().y()-corner_y_shift;

		roughness_table_width  = geometry().width() +  width_add;
		roughness_table_height = geometry().height()+ height_add;
	}
}

void Table_Of_Roughness::create_Menu()
{
	menu = new Menu(window_Type_Table_Of_Roughness,this);
	main_Layout->setMenuBar(menu->menu_Bar);
	// TODO
//	connect(menu, &Menu::refresh, this, [=]{reload_All_Widgets();});
//	connect(menu, &Menu::refresh, this, &Table_Of_Structures::emit_Data_Edited);

	if(global_Multilayer_Approach->fitting_Settings->in_Calculation)
	{
		for(QAction* action : menu->calculate_Menu->actions())
		{
			if(action->property(abort_Property).toString() != abort_Property)		{
				action->setDisabled(true);
			}
		}
	}
}

void Table_Of_Roughness::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(false);

	connect(main_Tabs,	&QTabWidget::currentChanged, this, [=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);
		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index)main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}

		if(can_Change_Index)
		{
			can_Change_Index = false;
																											{global_Multilayer_Approach->                       multilayer_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key))				{global_Multilayer_Approach->optical_Graphs				->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key))	{global_Multilayer_Approach->calculation_Settings_Editor->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			can_Change_Index = tab_synchronization;
		}
	});
}

void Table_Of_Roughness::add_Tabs()
{
//	read_Trees();
	for(int tab_Index=0; tab_Index<global_Multilayer_Approach->multilayer_Tabs->count(); ++tab_Index)
	{
		QWidget* new_Table = new QWidget;
//		My_Table_Widget* new_Table = new My_Table_Widget(basic_Row_Number, basic_Column_Number, this, this);
//		all_Widgets_To_Reload.append(QList<QWidget*>());
//		regular_Aperiodic_Widgets_To_Reload.append(QList<QWidget*>());

//		min_Max_Density_Spin_Boxes_List.append(QList<MyDoubleSpinBox*>());
//		min_Max_Thickness_Spin_Boxes_List.append(QList<MyDoubleSpinBox*>());
//		min_Max_Sigma_Spin_Boxes_List.append(QList<MyDoubleSpinBox*>());

//		create_Table(new_Table, tab_Index);
		main_Tabs->addTab(new_Table, global_Multilayer_Approach->multilayer_Tabs->tabText(tab_Index));
//		disable_enable_Structure_Items(new_Table);
	}
	for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
	{
		if(i!=main_Tabs->currentIndex()) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
	}
	main_Tabs->setCurrentIndex(global_Multilayer_Approach->multilayer_Tabs->currentIndex());
}

void Table_Of_Roughness::create_Tab_Content(QWidget *new_Widget, int tab_Index)
{

}
