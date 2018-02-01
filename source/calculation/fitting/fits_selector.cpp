#include "fits_selector.h"

Fits_Selector::Fits_Selector(Multilayer_Approach* multilayer_Approach, QWidget* parent) :
	multilayer_Approach(multilayer_Approach),
	fitted_Structures(multilayer_Approach->fitted_Structures),
	runned_Fits_Selectors(multilayer_Approach->runned_Fits_Selectors),
	QWidget(parent)
{
	setWindowTitle("Fits Selector");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Fits_Selector::closeEvent(QCloseEvent* event)
{
	runned_Fits_Selectors.remove(fits_Selector_Key);
	event->accept();
}

void Fits_Selector::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	create_List();
	load_Data();
	{
		QHBoxLayout* button_Layout = new QHBoxLayout;
			button_Layout->setAlignment(Qt::AlignCenter);
			button_Layout->setSpacing(40);

		done_Button = new QPushButton("Done");
			done_Button->setMaximumWidth(65);
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
			done_Button->setDefault(true);
			done_Button->setShortcut(Qt::Key_D | Qt::CTRL);
		button_Layout->addWidget(done_Button);

		clear_Button = new QPushButton("Clear");
			clear_Button->setMaximumWidth(65);
			clear_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		button_Layout->addWidget(clear_Button);

//		save_Button = new QPushButton("Save");
//			save_Button->setMaximumWidth(65);
//			save_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//		button_Layout->addWidget(save_Button);

		main_Layout->addLayout(button_Layout);

		connect(done_Button,  &QPushButton::clicked, this, &Fits_Selector::close);
		connect(clear_Button, &QPushButton::clicked, this, &Fits_Selector::clear_Fits);
	}
}

void Fits_Selector::set_Window_Geometry()
{
	setFixedWidth(270);
}

void Fits_Selector::create_List()
{
	fits_List = new QListWidget(this);
//		fits_List->setSelectionMode(QAbstractItemView::ExtendedSelection);
	main_Layout->addWidget(fits_List);

	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), fits_List);
	connect(shortcut, &QShortcut::activated, this, &Fits_Selector::delete_Items);
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

void Fits_Selector::delete_Items()
{
	for(QListWidgetItem* item : fits_List->selectedItems())
	{
		fitted_Structures.remove(fits_List->row(item));
		delete item;
	}
}

void Fits_Selector::open_Fit()
{
	bool opened = false;
	int index = fits_List->currentRow();

	qInfo() << "\n";
	for(int tab_Index=0; tab_Index<multilayer_Approach->multilayer_Tabs->count(); ++tab_Index)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Approach->multilayer_Tabs->widget(tab_Index));
		QTreeWidget* struct_Tree = multilayer->structure_Tree->tree;
		id_Type struct_Tree_Id = qvariant_cast<id_Type>(struct_Tree->property(id_Property));

		// look for appropriate tree
		qInfo() << tab_Index<<"size =" << fitted_Structures[index].fitted_Trees.size();
		for(QTreeWidget* loaded_Tree : fitted_Structures[index].fitted_Trees)
		{
			id_Type loaded_Tree_Id = qvariant_cast<id_Type>(loaded_Tree->property(id_Property));

			qInfo() << "loaded_Tree_Id " << loaded_Tree_Id << " ; struct_Tree_Id "<<struct_Tree_Id;
			if(loaded_Tree_Id == struct_Tree_Id)
			{
				qInfo() << "loaded " << fitted_Structures[index].name;
				struct_Tree->clear();
				for(int child_Index=0; child_Index<loaded_Tree->topLevelItemCount(); ++child_Index)
				{
					struct_Tree->addTopLevelItem(loaded_Tree->topLevelItem(child_Index)->clone());
				}
				struct_Tree->expandAll();
				opened = true;
			}
		}
	}

	// refresh table
	if(opened)
	if(multilayer_Approach->runned_Tables_Of_Structures.contains(table_Key))
	{
		multilayer_Approach->runned_Tables_Of_Structures.value(table_Key)->close();
		multilayer_Approach->open_Table_Of_Structures();
	}

	// refresh view
	multilayer_Approach->refresh_All_Multilayers_View();
}
