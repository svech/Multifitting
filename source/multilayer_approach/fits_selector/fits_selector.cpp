#include "fits_selector.h"

Fits_Selector::Fits_Selector(QWidget* parent) :
	runned_Fits_Selectors(global_Multilayer_Approach->runned_Fits_Selectors),
	fitted_Structures(global_Multilayer_Approach->fitted_Structures),
	QWidget(parent) // nullptr!
{
	setWindowTitle("Fits Selector");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Fits_Selector::closeEvent(QCloseEvent* event)
{
	write_Window_Geometry();
	runned_Fits_Selectors.remove(fits_Selector_Key);
	event->accept();
}

void Fits_Selector::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(0,0,0,0);

	create_List();
	load_Data();
	{
		QHBoxLayout* button_Layout = new QHBoxLayout;
			button_Layout->setAlignment(Qt::AlignCenter);
			button_Layout->setSpacing(20);

		done_Button = new QPushButton("Done");
			done_Button->setMaximumWidth(60);
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
			done_Button->setDefault(true);
			done_Button->setShortcut(Qt::Key_D | Qt::CTRL);
		button_Layout->addWidget(done_Button);

		clear_Button = new QPushButton("Clear");
			clear_Button->setMaximumWidth(60);
			clear_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		button_Layout->addWidget(clear_Button);

		save_Button = new QPushButton("Save");
			save_Button->setMaximumWidth(60);
			save_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		button_Layout->addWidget(save_Button);

		main_Layout->addLayout(button_Layout);

		connect(done_Button,  &QPushButton::clicked, this, &Fits_Selector::close);
		connect(clear_Button, &QPushButton::clicked, this, &Fits_Selector::clear_Fits);
		connect(save_Button,  &QPushButton::clicked, this, &Fits_Selector::save_Trees);
	}

	// shortcuts
	Global_Variables::create_Shortcuts(this);
}

void Fits_Selector::set_Window_Geometry()
{
//#ifdef _WIN32
	setGeometry(fits_selector_settings_x_corner,fits_selector_settings_y_corner,fits_selector_settings_width,fits_selector_settings_height);
//#endif
//#ifdef __linux__
//	setGeometry(0,0,235,multilayer_height);
//#endif
}

void Fits_Selector::write_Window_Geometry()
{
	if(!isMaximized())
	{
		fits_selector_settings_x_corner = frameGeometry().x()-corner_x_shift;
		fits_selector_settings_y_corner = frameGeometry().y()-corner_y_shift;

		fits_selector_settings_width  = geometry().width() +  width_add;
		fits_selector_settings_height = geometry().height()+ height_add;
	}
}

void Fits_Selector::create_List()
{
	fits_List = new QListWidget(this);
//		fits_List->setSelectionMode(QAbstractItemView::ExtendedSelection);
	main_Layout->addWidget(fits_List);

	QShortcut* delete_Shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), fits_List);
	QShortcut* rename_Shortcut = new QShortcut(QKeySequence(Qt::Key_F2), fits_List);

	connect(delete_Shortcut, &QShortcut::activated, this, &Fits_Selector::delete_Items);
	connect(rename_Shortcut, &QShortcut::activated, this, &Fits_Selector::rename_Item);

	connect(fits_List, &QListWidget::doubleClicked, this, &Fits_Selector::open_Fit);
}

void Fits_Selector::load_Data()
{
	fits_List->clear();
	for(Fitted_Structure& fitted_Structure : fitted_Structures)
	{
		add_Item(fitted_Structure);
	}
}

void Fits_Selector::add_Item(Fitted_Structure& fitted_Structure)
{
	QListWidgetItem* new_List_Item = new QListWidgetItem(fitted_Structure.name);
	fits_List->addItem(new_List_Item);
}

void Fits_Selector::clear_Fits()
{
	fits_List->clear();
	fitted_Structures.clear();
}

void Fits_Selector::save_Trees()
{
	QVector<QTreeWidget*> current_Trees(global_Multilayer_Approach->multilayer_Tabs->count());

	for(int tab_Index=0; tab_Index<global_Multilayer_Approach->multilayer_Tabs->count(); ++tab_Index)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));
		current_Trees[tab_Index] = multilayer->structure_Tree->tree;
	}
	global_Multilayer_Approach->add_Fitted_Structure(current_Trees, current_State);
}

void Fits_Selector::delete_Items()
{
	for(QListWidgetItem* item : fits_List->selectedItems())
	{
		fitted_Structures.remove(fits_List->row(item));
		delete item;
	}
}

void Fits_Selector::rename_Item()
{
	bool ok;
	QString text = QInputDialog::getText(this, "Rename fit", "New name", QLineEdit::Normal, fits_List->currentItem()->text(), &ok);
	if (ok && !text.isEmpty())
	{
		fits_List->currentItem()->setText(text);
		fitted_Structures[fits_List->currentRow()].name=text;
	}
}

void Fits_Selector::open_Fit()
{
	bool opened = false;
	int index = fits_List->currentRow();

	// close item editors
	global_Multilayer_Approach->close_Item_Editors();

	// close aperiodic tables
	global_Multilayer_Approach->temporarily_Close_Regular_Aperiodics();

	// close table
	bool table_Was_Opened = false;
	int active_Table_Tab = -2018;
	if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
	{
		table_Was_Opened = true;
		active_Table_Tab = global_Multilayer_Approach->table_Of_Structures->main_Tabs->currentIndex();
		global_Multilayer_Approach->table_Of_Structures->close();
	}

	//
	for(int tab_Index=0; tab_Index<global_Multilayer_Approach->multilayer_Tabs->count(); ++tab_Index)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(tab_Index));
		QTreeWidget* struct_Tree = multilayer->structure_Tree->tree;
		id_Type struct_Tree_Id = qvariant_cast<id_Type>(struct_Tree->property(id_Property));

		// look for appropriate tree
		for(QTreeWidget* loaded_Tree : fitted_Structures[index].fitted_Trees)
		{
			id_Type loaded_Tree_Id = qvariant_cast<id_Type>(loaded_Tree->property(id_Property));

			if(loaded_Tree_Id == struct_Tree_Id)
			{
				struct_Tree->clear();
				for(int child_Index=0; child_Index<loaded_Tree->topLevelItemCount(); ++child_Index)
				{
					struct_Tree->addTopLevelItem(loaded_Tree->topLevelItem(child_Index)->clone());
				}
				struct_Tree->expandAll();
				opened = true;
			}
		}

		// TODO INDEPENDENT
		// refresh struct_Tree_Copy
//		for(int independent_Index=0; independent_Index<multilayer->independent_Curve_Tabs->tabBar()->count(); independent_Index++)
//		{
//			Independent_Curve* independent = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(independent_Index));
//			independent->reset_Independent_Variables_Structure();
//		}
	}

	// refresh table
	if(opened && table_Was_Opened)
	{
		global_Multilayer_Approach->open_Table_Of_Structures();
		global_Multilayer_Approach->table_Of_Structures->main_Tabs->setCurrentIndex(active_Table_Tab);
	}

	// reopen aperiodic tables
	global_Multilayer_Approach->reopen_Regular_Aperiodics();

	// recalculate and replot
	if(opened)
	{
		global_Multilayer_Approach->calculate(true);
		if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
		{
			global_Multilayer_Approach->optical_Graphs_1D->activateWindow();
		}
	}

	// refresh view
	global_Multilayer_Approach->refresh_All_Multilayers_View();
}
