#include "multilayer.h"

Multilayer::Multilayer():
	gui_Settings(Gui_Settings_Path, QSettings::IniFormat),
	default_Values(Default_Values_Path, QSettings::IniFormat)
{
	create_Main_Layout();
}

void Multilayer::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);

	create_Struct_Frame();
		main_Layout->addWidget(struct_Frame);
	create_Variables_Frame();
		main_Layout->addWidget(variables_Frame);
	create_Data_Frame();
		main_Layout->addWidget(data_Frame);

	main_Layout->setSpacing(0);
	setLayout(main_Layout);
}

void Multilayer::create_Struct_Frame()
{
	struct_Frame = new QFrame;
	struct_Frame_Layout = new QVBoxLayout(struct_Frame);

	create_Struct_Tree();
		struct_Frame_Layout->addWidget(struct_Tree);
	create_Struct_Toolbar();
		struct_Frame_Layout->addWidget(struct_Toolbar);

	struct_Frame_Layout->setSpacing(0);
	struct_Frame->setContentsMargins(-7,-3,-7,-10);
	struct_Frame->setLayout(struct_Frame_Layout);
}

void Multilayer::create_Struct_Tree()
{
	struct_Tree = new QTreeWidget;
	struct_Tree->setColumnCount(1);
	struct_Tree->header()->close();
	struct_Tree->expandAll();

	struct_Tree->setExpandsOnDoubleClick(false);
	connect(struct_Tree, SIGNAL(itemSelectionChanged()), this, SLOT(if_Selected()));
	connect(struct_Tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(if_DoubleClicked(QTreeWidgetItem*, int)));
	add_Layer(true);
}

void Multilayer::create_Struct_Toolbar()
{
	gui_Settings.beginGroup( Paths );
		QString icon_Path = gui_Settings.value( "icon_Path", 0 ).toString();
	gui_Settings.endGroup();

	QPixmap add_Layer		(icon_Path + "add_layer.bmp");
	QPixmap add_Multilayer  (icon_Path + "add_multilayer.bmp");
	QPixmap add_Substrate	(icon_Path + "add_substrate.bmp");
	QPixmap edit			(icon_Path + "roi.bmp");
	QPixmap remove			(icon_Path + "delete.bmp");
	QPixmap cut				(icon_Path + "cut.bmp");
	QPixmap copy			(icon_Path + "copy.bmp");
	QPixmap paste			(icon_Path + "paste.bmp");
	QPixmap move_Up			(icon_Path + "shift_up.bmp");
	QPixmap move_Down		(icon_Path + "shift_down.bmp");
	QPixmap group			(icon_Path + "group.bmp");
	QPixmap ungroup			(icon_Path + "ungroup.bmp");
	QPixmap thickness_Plot	(icon_Path + "zplot.bmp");
	QPixmap sigma_Plot		(icon_Path + "sigmaplot.bmp");
	QPixmap destroy			(icon_Path + "bomb.bmp");

	struct_Toolbar = new QToolBar;
	struct_Toolbar->addAction(QIcon(add_Layer),		"Add Layer");							// 0
	struct_Toolbar->addAction(QIcon(add_Multilayer),"Add Multilayer");						// 1
	struct_Toolbar->addAction(QIcon(add_Substrate), "Add Substrate");						// 2
	struct_Toolbar->addAction(QIcon(edit),			"Edit");								// 3
	struct_Toolbar->addAction(QIcon(remove),		"Remove");								// 4
	struct_Toolbar->addAction(QIcon(cut),			"Cut");									// 5
	struct_Toolbar->addAction(QIcon(copy),			"Copy");								// 6
	struct_Toolbar->addAction(QIcon(paste),			"Paste");								// 7
	struct_Toolbar->addAction(QIcon(move_Up),		"Move Up");								// 8
	struct_Toolbar->addAction(QIcon(move_Down),		"Move Down");							// 9
	struct_Toolbar->addAction(QIcon(group),			"Group");								// 10
	struct_Toolbar->addAction(QIcon(ungroup),		"Ungroup");								// 11
	struct_Toolbar->addAction(QIcon(thickness_Plot),"Plot Layer Thickness Profile");		// 12
	struct_Toolbar->addAction(QIcon(sigma_Plot),	"Plot Interface Width Profile");		// 13
	struct_Toolbar->addAction(QIcon(destroy),		"Remove Substrate and All Layers");		// 14

	struct_Toolbar->setIconSize(add_Layer.size());

	connect(struct_Toolbar->actions()[0], SIGNAL(triggered(bool)), this, SLOT(add_Layer(bool)));
	connect(struct_Toolbar->actions()[1], SIGNAL(triggered(bool)), this, SLOT(add_Multilayer(bool)));
	connect(struct_Toolbar->actions()[2], SIGNAL(triggered(bool)), this, SLOT(add_Substrate(bool)));
	connect(struct_Toolbar->actions()[3], SIGNAL(triggered(bool)), this, SLOT(edit(bool)));
	connect(struct_Toolbar->actions()[4], SIGNAL(triggered(bool)), this, SLOT(remove(bool)));
	connect(struct_Toolbar->actions()[5], SIGNAL(triggered(bool)), this, SLOT(cut(bool)));
	connect(struct_Toolbar->actions()[6], SIGNAL(triggered(bool)), this, SLOT(copy(bool)));
	connect(struct_Toolbar->actions()[7], SIGNAL(triggered(bool)), this, SLOT(paste(bool)));
	connect(struct_Toolbar->actions()[8], SIGNAL(triggered(bool)), this, SLOT(move_Up(bool)));
	connect(struct_Toolbar->actions()[9], SIGNAL(triggered(bool)), this, SLOT(move_Down(bool)));
	connect(struct_Toolbar->actions()[10], SIGNAL(triggered(bool)), this, SLOT(group(bool)));
	connect(struct_Toolbar->actions()[11], SIGNAL(triggered(bool)), this, SLOT(ungroup(bool)));
	connect(struct_Toolbar->actions()[12], SIGNAL(triggered(bool)), this, SLOT(thickness_Plot(bool)));
	connect(struct_Toolbar->actions()[13], SIGNAL(triggered(bool)), this, SLOT(sigma_Plot(bool)));
	connect(struct_Toolbar->actions()[14], SIGNAL(triggered(bool)), this, SLOT(destroy(bool)));

	if_Selected();
}

void Multilayer::create_Variables_Frame()
{
	variables_Frame = new QFrame;
	variables_Frame_Layout = new QVBoxLayout(variables_Frame);

	create_Variables_Tabs();
		variables_Frame_Layout->addWidget(variables_Tabs);

	variables_Frame_Layout->setSpacing(0);
	variables_Frame->setContentsMargins(-7,-8,-9,-10);
	variables_Frame->setLayout(variables_Frame_Layout);
}

void Multilayer::create_Variables_Tabs()
{
	variables_Tabs = new QTabWidget;
	create_Dependent_Variables_Tabs();
	{
		QFrame* frame = new QFrame;
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(dependent_Variables_Tabs);
		frame->setLayout(layout);
		frame->setContentsMargins(0,-5,0,-8);
		variables_Tabs->addTab(frame, "Dependent Variables");
	}

	create_Independent_Variables_List();
	create_Independent_Variables_Toolbar();
	{
		QFrame* frame = new QFrame;
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(independent_Variables_List);
		layout->addWidget(independent_Variables_Toolbar);
		layout->setSpacing(0);
		frame->setLayout(layout);
		frame->setContentsMargins(0,-5,0,-12);
		variables_Tabs->addTab(frame, "Independent Variables");
	}

	create_Coupled_Parameters_List();
	create_Coupled_Parameters_Toolbar();
	{
		QFrame* frame = new QFrame;
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(coupled_Parameters_List);
		layout->addWidget(coupled_Parameters_Toolbar);
		layout->setSpacing(0);
		frame->setLayout(layout);
		frame->setContentsMargins(0,-5,0,-12);
		variables_Tabs->addTab(frame, "Coupled Parameters");
	}

	// TODO make other variables tabs
	variables_Tabs->addTab(new QWidget(), "Fitting");
	variables_Tabs->addTab(new QWidget(), "Optimization");
	variables_Tabs->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
}

void Multilayer::create_Dependent_Variables_Tabs()
{
	dependent_Variables_Tabs = new QTabWidget;
	specular_Functions_Layout = new QVBoxLayout;
	nonspecular_Functions_Layout = new QVBoxLayout;

	create_Specular_Functions();
		specular_Functions_Layout->addWidget(standard_Functions_Group_Box);
		specular_Functions_Layout->addWidget(field_Functions_Group_Box);
		specular_Functions_Layout->addWidget(user_Functions_Group_Box);
	specular_Functions_Layout->setSpacing(0);
	QFrame* specular_Frame = new QFrame;
	specular_Frame->setContentsMargins(-5,-5,-5,-9);
		specular_Frame->setLayout(specular_Functions_Layout);
	dependent_Variables_Tabs->addTab(specular_Frame, "Specular Optical Functions");

	// TODO non-specular functions
	create_Nonspecular_Functions();
//		nonspecular_Functions_Layout->addWidget(nonspecular_Group_Box);
	QFrame* nonspecular_Frame = new QFrame;
		nonspecular_Frame->setLayout(nonspecular_Functions_Layout);
	dependent_Variables_Tabs->addTab(nonspecular_Frame, "Scattering Functions");
}

void Multilayer::create_Specular_Functions()
{
	standard_Functions_Group_Box = new QGroupBox("Standard Functions");
	standard_Functions_Group_Box->setContentsMargins(0,8,0,-4);
		QHBoxLayout* standard_Functions_Layout = new QHBoxLayout(standard_Functions_Group_Box);
		standard_Functions_Layout->setAlignment(Qt::AlignLeft);
		reflect_Functions = new QCheckBox("Reflectance");
			standard_Functions_Layout->addWidget(reflect_Functions);
		transmit_Functions = new QCheckBox("Transmittance");
			standard_Functions_Layout->addWidget(transmit_Functions);
		absorp_Functions = new QCheckBox("Absorptance");
			standard_Functions_Layout->addWidget(absorp_Functions);

	field_Functions_Group_Box = new QGroupBox("Field Functions");
	field_Functions_Group_Box->setContentsMargins(0,8,0,-4);
		QHBoxLayout* field_Functions_Layout = new QHBoxLayout(field_Functions_Group_Box);
		field_Functions_Layout->setAlignment(Qt::AlignLeft);
		field_Intensity = new QCheckBox("Intensity");
			field_Functions_Layout->addWidget(field_Intensity);
		joule_Absorption= new QCheckBox("Joule Absorption");
			field_Functions_Layout->addWidget(joule_Absorption);

	user_Functions_Group_Box = new QGroupBox("User-defined Functions");
	user_Functions_Group_Box->setContentsMargins(0,8,0,-4);
		QHBoxLayout* user_Functions_Layout = new QHBoxLayout(user_Functions_Group_Box);
		user_Functions_Layout->setAlignment(Qt::AlignLeft);
		user_Supplied_Functions_Check = new QCheckBox;
			user_Functions_Layout->addWidget(user_Supplied_Functions_Check);
		user_Supplied_Functions = new QLineEdit;
			user_Functions_Layout->addWidget(user_Supplied_Functions);
}

void Multilayer::create_Nonspecular_Functions()
{
	// TODO nonspecular functions
}

void Multilayer::create_Independent_Variables_List()
{
	// TODO invisible if empty
	independent_Variables_List = new QListWidget;
	independent_Variables_List->setSizeAdjustPolicy(QListWidget::AdjustToContents);
	QSizePolicy sp_Retain = independent_Variables_List->sizePolicy();
				sp_Retain.setRetainSizeWhenHidden(true);
	independent_Variables_List->setSizePolicy(sp_Retain);

//	independent_Variables_List->hide();
}

void Multilayer::create_Independent_Variables_Toolbar()
{
	gui_Settings.beginGroup( Paths );
		QString icon_Path = gui_Settings.value( "icon_Path", 0 ).toString();
	gui_Settings.endGroup();

	QPixmap new_Variable	(icon_Path + "new.bmp");
	QPixmap edit			(icon_Path + "roi.bmp");
	QPixmap remove			(icon_Path + "delete.bmp");

	independent_Variables_Toolbar = new QToolBar;
	independent_Variables_Toolbar->addAction(QIcon(new_Variable),	"Add Independent Variable");// 0
	independent_Variables_Toolbar->addAction(QIcon(edit),			"Edit");					// 1
	independent_Variables_Toolbar->addAction(QIcon(remove),			"Remove");					// 2

	independent_Variables_Toolbar->setIconSize(new_Variable.size());

	// TODO connect actions
}

void Multilayer::create_Coupled_Parameters_List()
{
	// TODO invisible if empty
	coupled_Parameters_List = new QListWidget;
	coupled_Parameters_List->setSizeAdjustPolicy(QListWidget::AdjustToContents);
	QSizePolicy sp_Retain = coupled_Parameters_List->sizePolicy();
				sp_Retain.setRetainSizeWhenHidden(true);
	coupled_Parameters_List->setSizePolicy(sp_Retain);

//	coupled_Parameters_List->hide();
}

void Multilayer::create_Coupled_Parameters_Toolbar()
{
	gui_Settings.beginGroup( Paths );
		QString icon_Path = gui_Settings.value( "icon_Path", 0 ).toString();
	gui_Settings.endGroup();

	QPixmap new_Variable	(icon_Path + "new.bmp");
	QPixmap edit			(icon_Path + "roi.bmp");
	QPixmap remove			(icon_Path + "delete.bmp");

	coupled_Parameters_Toolbar = new QToolBar;
	coupled_Parameters_Toolbar->addAction(QIcon(new_Variable),	"Add Coupled Parameter");	// 0
	coupled_Parameters_Toolbar->addAction(QIcon(edit),			"Edit");					// 1
	coupled_Parameters_Toolbar->addAction(QIcon(remove),		"Remove");					// 2

	coupled_Parameters_Toolbar->setIconSize(new_Variable.size());

	// TODO connect actions
}

void Multilayer::create_Fitting_Parameters_List()
{
// TODO
}

void Multilayer::create_Fitting_Parameters_Toolbar()
{
// TODO
}

void Multilayer::create_Optimization_Parameters_List()
{
// TODO
}

void Multilayer::create_Optimization_Parameters_Toolbar()
{
// TODO
}

void Multilayer::create_Data_Frame()
{
	data_Frame = new QFrame;
	data_Frame_Layout = new QVBoxLayout(data_Frame);
	data_Frame_Layout->setSpacing(0);
	data_Frame->setContentsMargins(-8,-8,-8,-7);

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

	add_Measured_Data();
	add_Target_Profile();
}

// slots

// structure toolbar

void Multilayer::add_Layer(bool)
{
	QTreeWidgetItem* new_Layer = new QTreeWidgetItem;
	add_Buffered_Layer(new_Layer);
	refresh_Over_Struct();
}

void Multilayer::add_Multilayer(bool)
{
	int num_Children = 0;

	bool ok, ok_Number;
	QString text = QInputDialog::getText(this, "Multilayer", "Number of different layers in multilayers stack", QLineEdit::Normal, QString::number(2), &ok);
	text.toInt(&ok_Number);
	if (ok && ok_Number && (text.toInt()>=2))
	{
		num_Children = text.toInt();
	} else
	if(!ok)
	{
		return;
	}
	if(!ok_Number)
	{
		QMessageBox::warning(this, "Warning", text+" is not a integer number");
		add_Multilayer(true);
	} else
	if (text.toInt()<2)
	{
		QMessageBox::warning(this, "Warning", "There should be at least 2 layers");
		add_Multilayer(true);
	}
	if(num_Children<2) return;

	QTreeWidgetItem* new_Multilayer = new QTreeWidgetItem;
	QList<QTreeWidgetItem*> new_Child_Layers;
	for(int i=0; i<num_Children; i++)
	{
		QTreeWidgetItem* new_Child_Layer = new QTreeWidgetItem;
		new_Child_Layers << new_Child_Layer;

		Layer layer;
		QVariant var;
		var.setValue( layer );
		new_Child_Layer->setData(default_Column, Qt::UserRole, var);

		set_Structure_Item_Text(new_Child_Layer);
	}
	new_Multilayer->addChildren(new_Child_Layers);
	Stack_Content stack_Content;
	QVariant var;
	var.setValue( stack_Content );
	new_Multilayer->setData(default_Column, Qt::UserRole, var);

	// if no selected items
	if(struct_Tree->selectedItems().isEmpty())
	{
		// if there is no substrate
		if(struct_Tree->topLevelItem(struct_Tree->topLevelItemCount()-1)->whatsThis(default_Column)!=what_is_This_Substrate)
		{
			struct_Tree->addTopLevelItem(new_Multilayer);
		} else
		{
			struct_Tree->insertTopLevelItem(struct_Tree->topLevelItemCount()-1,new_Multilayer);
		}
	} else
	{
		// if selected item is top-level
		if(!struct_Tree->currentItem()->parent())
		{
			int position = struct_Tree->indexOfTopLevelItem(struct_Tree->currentItem());

			// place multilayers before substrate
			if(  (struct_Tree->topLevelItem(struct_Tree->topLevelItemCount()-1)->whatsThis(default_Column)!=what_is_This_Substrate)
			   ||(position != (struct_Tree->topLevelItemCount()-1)))
			{
				struct_Tree->insertTopLevelItem(position+1, new_Multilayer);
			} else
			{
				struct_Tree->insertTopLevelItem(position, new_Multilayer);
			}
		} else
		{
			int position = struct_Tree->currentIndex().row();
			struct_Tree->currentItem()->parent()->insertChild(position+1,new_Multilayer);
		}
	}

	set_Structure_Item_Text(new_Multilayer);
	refresh_Toolbar();
	refresh_Over_Struct();
}

void Multilayer::add_Substrate(bool)
{
	QTreeWidgetItem* new_Substrate = new QTreeWidgetItem;
	new_Substrate->setWhatsThis(default_Column, what_is_This_Substrate);
	struct_Tree->addTopLevelItem(new_Substrate);
	set_Structure_Item_Text(new_Substrate);

	struct_Toolbar->actions()[2]->setDisabled(true);

	refresh_Toolbar();
}

void Multilayer::edit(bool)
{
	if(runned_Editors.contains(struct_Tree->currentItem()))
	{
		if(runned_Editors.value(struct_Tree->currentItem())->isVisible())
		{
			runned_Editors.value(struct_Tree->currentItem())->activateWindow();
		} else
		{
			delete runned_Editors.value(struct_Tree->currentItem());
			runned_Editors.remove(struct_Tree->currentItem());
		}
	}
	if(!runned_Editors.contains(struct_Tree->currentItem()))
	{
		Item_Editing* item_Editing = new Item_Editing(struct_Tree->currentItem());
		connect(item_Editing, SIGNAL(is_Closed()), this, SLOT(editor_Close()));
		connect(item_Editing, SIGNAL(is_Edited()), this, SLOT(editors_Refresh()));
		item_Editing->show();

		list_Editors.append(item_Editing);
		struct_Toolbar->setDisabled(true);
		runned_Editors.insert(struct_Tree->currentItem(),item_Editing);
	}
}

void Multilayer::remove(bool)
{
	// TODO show item name before removal
	if((struct_Tree->currentItem()->parent())&&(struct_Tree->currentItem()->parent()->childCount()==2))
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Multilayer " + struct_Tree->currentItem()->parent()->whatsThis(default_Column)+" will be disbanded.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			QTreeWidgetItem* parent = struct_Tree->currentItem()->parent();
			delete struct_Tree->currentItem();

			QTreeWidgetItem* survived_Child_Copy = new QTreeWidgetItem();
			*survived_Child_Copy = *parent->child(0);

			// if multilayer is already nested
			if(parent->parent())
			{
				int parent_Position = parent->parent()->indexOfChild(parent);
				parent->parent()->insertChild(parent_Position, survived_Child_Copy);
				delete parent;
			} else
			{
				int parent_Position = struct_Tree->indexOfTopLevelItem(parent);
				struct_Tree->insertTopLevelItem(parent_Position, survived_Child_Copy);
				delete parent;
			}
		}
	} else
	if(struct_Tree->currentItem()->whatsThis(default_Column) != what_is_This_Ambient)
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove "+struct_Tree->currentItem()->whatsThis(default_Column)+"?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			if(struct_Tree->currentItem()->whatsThis(default_Column) == what_is_This_Substrate)
			{
				struct_Toolbar->actions()[2]->setDisabled(false);		// add_Substrate
			}
			delete struct_Tree->currentItem();
		}
	} else
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really reset ambient?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			QVariant var;
			QTreeWidgetItem* new_Ambient = new QTreeWidgetItem;
			Ambient ambient;
			var.setValue( ambient );
			new_Ambient->setData(default_Column, Qt::UserRole, var);
			new_Ambient->setWhatsThis(default_Column,what_is_This_Ambient);
			struct_Tree->insertTopLevelItem(1,new_Ambient);

			delete struct_Tree->currentItem();
		}
	}

	refresh_Toolbar();
}

void Multilayer::cut(bool)
{
	buffered = struct_Tree->currentItem()->clone();

	// TODO show item name before cut
	if((struct_Tree->currentItem()->parent())&&(struct_Tree->currentItem()->parent()->childCount()==2))
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Cut", "Multilayer "+struct_Tree->currentItem()->parent()->whatsThis(default_Column)+" will be disbanded.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			QTreeWidgetItem* parent = struct_Tree->currentItem()->parent();
			delete struct_Tree->currentItem();

			QTreeWidgetItem* survived_Child_Copy = new QTreeWidgetItem();
			*survived_Child_Copy = *parent->child(0);

			// if multilayer is already nested
			if(parent->parent())
			{
				int parent_Position = parent->parent()->indexOfChild(parent);
				parent->parent()->insertChild(parent_Position, survived_Child_Copy);
				delete parent;
			} else
			{
				int parent_Position = struct_Tree->indexOfTopLevelItem(parent);
				struct_Tree->insertTopLevelItem(parent_Position, survived_Child_Copy);
				delete parent;
			}
		}
	} else
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Cut", "Really cut "+struct_Tree->currentItem()->whatsThis(default_Column)+"?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			if(struct_Tree->currentItem()->whatsThis(default_Column) == what_is_This_Substrate)
			{
				struct_Toolbar->actions()[2]->setDisabled(false);
			}
			delete struct_Tree->currentItem();
		}
	}
	refresh_Toolbar();
}

void Multilayer::copy(bool)
{
	buffered = struct_Tree->currentItem()->clone();
	refresh_Toolbar();
}

void Multilayer::paste(bool)
{
	add_Buffered_Layer(buffered);
}

void Multilayer::move_Up(bool)
{
	int position = struct_Tree->currentIndex().row();
	QTreeWidgetItem* parent = struct_Tree->currentItem()->parent();

	// if nested
	if(parent)
	{
		QTreeWidgetItem* taken = parent->takeChild(position-1);
		parent->insertChild(position, taken);
	} else
	{
		QTreeWidgetItem* taken = struct_Tree->takeTopLevelItem(position-1);
		struct_Tree->insertTopLevelItem(position, taken);
	}

	refresh_Toolbar();
}

void Multilayer::move_Down(bool)
{
	int position = struct_Tree->currentIndex().row();
	QTreeWidgetItem* parent = struct_Tree->currentItem()->parent();

	// if nested
	if(parent)
	{
		QTreeWidgetItem* taken = parent->takeChild(position+1);
		parent->insertChild(position, taken);
	} else
	{
		QTreeWidgetItem* taken = struct_Tree->takeTopLevelItem(position+1);
		struct_Tree->insertTopLevelItem(position, taken);
	}

	refresh_Toolbar();
}

void Multilayer::group(bool)
{
	// TODO group_toolbutton
	qInfo() << "group is not implemented";

	refresh_Toolbar();
}

void Multilayer::ungroup(bool)
{
	int position = struct_Tree->currentIndex().row();

	// deep copy of children
	QList<QTreeWidgetItem*> child_List;
	for(int i=0; i<struct_Tree->currentItem()->childCount(); i++)
	{
		child_List.append(struct_Tree->currentItem()->child(i)->clone());
	}

	// if nested
	if(struct_Tree->currentItem()->parent())
	{
		struct_Tree->currentItem()->parent()->insertChildren(position+1, child_List);
		delete struct_Tree->currentItem();
	} else
	{
		struct_Tree->insertTopLevelItems(position+1, child_List);
		delete struct_Tree->currentItem();
	}

	refresh_Toolbar();
}

void Multilayer::thickness_Plot(bool)
{
	// TODO thickness_Plot_toolbutton
	qInfo() << "thickness_Plot is not implemented";
}

void Multilayer::sigma_Plot(bool)
{
	// TODO sigma_Plot_toolbutton
	qInfo() << "sigma_Plot is not implemented";
}

void Multilayer::destroy(bool)
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Remove substrate and all layers?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		struct_Tree->clear();
		add_Layer(true);		
		struct_Toolbar->actions()[2]->setDisabled(false);		// add_Substrate
		refresh_Toolbar();
	}
}

// service slots

void Multilayer::if_Selected()
{
	if(buffered!=NULL)
	{
		struct_Toolbar->actions()[7]->setDisabled(false);		 // paste
	} else
	{
		struct_Toolbar->actions()[7]->setDisabled(true);		 // paste
	}

	struct_Toolbar->actions()[11]->setDisabled(true);		 // ungroup

	if(struct_Tree->selectedItems().isEmpty())
	{
		struct_Toolbar->actions()[3]->setDisabled(true);		 // edit
		struct_Toolbar->actions()[4]->setDisabled(true);		 // remove
		struct_Toolbar->actions()[5]->setDisabled(true);		 // cut
		struct_Toolbar->actions()[6]->setDisabled(true);		 // copy
		struct_Toolbar->actions()[8]->setDisabled(true);		 // move_Up
		struct_Toolbar->actions()[9]->setDisabled(true);		 // move_Down
		struct_Toolbar->actions()[10]->setDisabled(true);		 // group
		struct_Toolbar->actions()[12]->setDisabled(true);		 // thickness_Plot
		struct_Toolbar->actions()[13]->setDisabled(true);		 // sigma_Plot
	} else
	{
		bool if_Substrate = struct_Tree->currentItem()->whatsThis(default_Column)==what_is_This_Substrate;

		struct_Toolbar->actions()[3]->setDisabled(false);		 // edit
		struct_Toolbar->actions()[4]->setDisabled(false);		 // remove

		// if multilayer
		if(struct_Tree->currentItem()->childCount()>0)
		{
			struct_Toolbar->actions()[11]->setDisabled(false);		 // ungroup
		}

		int position = struct_Tree->currentIndex().row();
		QTreeWidgetItem* parent = struct_Tree->currentItem()->parent();

		// if nested
		if(parent)
		{
			struct_Toolbar->actions()[5]->setDisabled(false);		 // cut
			struct_Toolbar->actions()[6]->setDisabled(false);		 // copy

			if(position == 0)
			{
				struct_Toolbar->actions()[8]->setDisabled(true);	// move_Up
				struct_Toolbar->actions()[9]->setDisabled(false);	// move_Down
			} else
			if(position == (parent->childCount()-1))
			{
				struct_Toolbar->actions()[8]->setDisabled(false);	// move_Up
				struct_Toolbar->actions()[9]->setDisabled(true);	// move_Down
			} else
			{
				struct_Toolbar->actions()[8]->setDisabled(false);	// move_Up
				struct_Toolbar->actions()[9]->setDisabled(false);	// move_Down
			}
		} else
		{
			// if ambient
			if(position == 0)
			{
				struct_Toolbar->actions()[5]->setDisabled(true);	// cut
				struct_Toolbar->actions()[6]->setDisabled(true);	// copy
				struct_Toolbar->actions()[8]->setDisabled(true);	// move_Up
				struct_Toolbar->actions()[9]->setDisabled(true);	// move_Down
				struct_Toolbar->actions()[10]->setDisabled(true);	// group
			} else
			// if not at end
			if(position<struct_Tree->topLevelItemCount()-1)
			{
				struct_Toolbar->actions()[5]->setDisabled(false);	// cut
				struct_Toolbar->actions()[6]->setDisabled(false);	// copy

				// if next is not substrate
				if(struct_Tree->topLevelItem(position+1)->whatsThis(default_Column)!=what_is_This_Substrate)
				{
					struct_Toolbar->actions()[9]->setDisabled(false);	// move_Down
				}
				// if second
				if(position == 1)
				{
					struct_Toolbar->actions()[8]->setDisabled(true);	// move_Up
				} else
				{
					struct_Toolbar->actions()[8]->setDisabled(false);	// move_Up
				}
			} else
			{ // if at end
				if(if_Substrate)
				{
					struct_Toolbar->actions()[5]->setDisabled(true);	// cut
					struct_Toolbar->actions()[6]->setDisabled(true);	// copy
					struct_Toolbar->actions()[8]->setDisabled(true);	// move_Up
				} else
				{
					struct_Toolbar->actions()[5]->setDisabled(false);	// cut
					struct_Toolbar->actions()[6]->setDisabled(false);	// copy
					struct_Toolbar->actions()[8]->setDisabled(false);	// move_Up
				}
				struct_Toolbar->actions()[9]->setDisabled(true);	// move_Down
			}
		}
	}
}

void Multilayer::if_DoubleClicked(QTreeWidgetItem* item, int column)
{
	item;column;
	edit(true);
}

void Multilayer::add_Buffered_Layer(QTreeWidgetItem* new_Layer_Passed)
{
	// setting data to new layerItem
	QVariant var;

	// copy of new_Layer
	QTreeWidgetItem* new_Layer = new QTreeWidgetItem;
	new_Layer = new_Layer_Passed->clone();

	// if no tree at all (at the beginning)
	if(struct_Tree->topLevelItemCount()==0)
	{
		Ambient ambient;
		var.setValue( ambient );
		new_Layer->setData(default_Column, Qt::UserRole, var);
		new_Layer->setWhatsThis(default_Column,what_is_This_Ambient);
		struct_Tree->addTopLevelItem(new_Layer);
	} else
	{
		Layer layer;
		var.setValue( layer );
		new_Layer->setData(default_Column, Qt::UserRole, var);

		// if no selected items
		if(struct_Tree->selectedItems().isEmpty())
		{
			// if there is no substrate
			if(struct_Tree->topLevelItem(struct_Tree->topLevelItemCount()-1)->whatsThis(default_Column)!=what_is_This_Substrate)
			{
				struct_Tree->addTopLevelItem(new_Layer);
			} else
			{
				struct_Tree->insertTopLevelItem(struct_Tree->topLevelItemCount()-1,new_Layer);
			}
		} else
		{
			// if selected item is top-level
			if(!struct_Tree->currentItem()->parent())
			{
				int position = struct_Tree->indexOfTopLevelItem(struct_Tree->currentItem());

				// place layers before substrate
				if(  (struct_Tree->topLevelItem(struct_Tree->topLevelItemCount()-1)->whatsThis(default_Column)!=what_is_This_Substrate)
				   ||(position != (struct_Tree->topLevelItemCount()-1)))
				{
					struct_Tree->insertTopLevelItem(position+1, new_Layer);
				} else
				{
					struct_Tree->insertTopLevelItem(position, new_Layer);
				}
			} else
			{
				int position = struct_Tree->currentIndex().row();
				struct_Tree->currentItem()->parent()->insertChild(position+1,new_Layer);
			}
		}
	}

	set_Structure_Item_Text(new_Layer);
	refresh_Toolbar();
}

void Multilayer::refresh_Toolbar()
{
	if(!struct_Tree->selectedItems().isEmpty())
	{
		struct_Tree->currentItem()->setSelected(false);
		struct_Tree->currentItem()->setSelected(true);
	}
	struct_Tree->expandAll();

	multiple_Refresh_Over_Struct();
}

void Multilayer::set_Structure_Item_Text(QTreeWidgetItem* item)
{
	default_Values.beginGroup( Structure_Values_Representation );
		int precision = default_Values.value( "default_precision", 0 ).toInt();
	default_Values.endGroup();

	// if ambient
	if(item->whatsThis(default_Column)==what_is_This_Ambient)
	{
		item->setText(default_Column, "ambient: " + item->data(default_Column, Qt::UserRole).value<Ambient>().material);
		if(item->data(default_Column, Qt::UserRole).value<Ambient>().material!="Vacuum")
		{
			item->setText(default_Column, item->text(default_Column) + ", " + Rho_Sym + "=" +  QString::number(item->data(default_Column, Qt::UserRole).value<Ambient>().density,'f',precision) + "g/cm" + Cube_Sym);
		}
	} else
	{
		// if substrate
		if(item->whatsThis(default_Column)==what_is_This_Substrate)
		{
			item->setText(default_Column, item->data(default_Column, Qt::UserRole).value<Substrate>().material + " substrate"
						  + ", " + Rho_Sym + "=" +  QString::number(item->data(default_Column, Qt::UserRole).value<Substrate>().density,'f',precision) + "g/cm" + Cube_Sym);

			if(item->data(default_Column, Qt::UserRole).value<Substrate>().sigma>0)
			{
				item->setText(default_Column, item->text(default_Column) + ", " + Sigma_Sym + "=" +  QString::number(item->data(default_Column, Qt::UserRole).value<Substrate>().sigma,'f',precision) + Angstrom_Sym);
			}
		} else
		{
			// if multilayer
			if(item->childCount()>0)
			{
				item->setText(default_Column, "Multilayer, N=" + QString::number(item->data(default_Column, Qt::UserRole).value<Stack_Content>().num_Repetition)
							  + ", d=" + QString::number(item->data(default_Column, Qt::UserRole).value<Stack_Content>().period,'f',precision) + Angstrom_Sym);

				if(item->childCount()==2)
				{
					item->setText(default_Column, item->text(default_Column) + ", " + Gamma_Sym + "=" + QString::number(item->data(default_Column, Qt::UserRole).value<Stack_Content>().gamma,'f',precision));
				}
			} else
			// if layer
			{
				item->setText(default_Column, item->data(default_Column, Qt::UserRole).value<Layer>().material + " layer"
							  + ", z=" +  QString::number(item->data(default_Column, Qt::UserRole).value<Layer>().thickness,'f',precision) + Angstrom_Sym
							  + ", " + Rho_Sym + "=" +  QString::number(item->data(default_Column, Qt::UserRole).value<Layer>().density,'f',precision) + "g/cm" + Cube_Sym);

				if(item->data(default_Column, Qt::UserRole).value<Layer>().sigma>0)
				{
					item->setText(default_Column, item->text(default_Column) + ", " + Sigma_Sym + "=" +  QString::number(item->data(default_Column, Qt::UserRole).value<Layer>().sigma,'f',precision) + Angstrom_Sym);
				}
			}
		}
	}
}

void Multilayer::editor_Close()
{
	// remove closed editor from list
	for(int i=0; i<list_Editors.size(); ++i)
	{
		if(list_Editors[i]==sender())
			list_Editors.removeAt(i);
	}

	if(list_Editors.isEmpty())
	{
		struct_Toolbar->setDisabled(false);
	}
	multiple_Refresh_Over_Struct();
}

void Multilayer::editors_Refresh()
{
	multiple_Refresh_Over_Struct();

	// broadcast to other editors
	for(int i=0; i<list_Editors.size(); ++i)
	{
		if(list_Editors[i]!=sender())
		{
			list_Editors[i]->show_Material();
			list_Editors[i]->show_Density();
			list_Editors[i]->show_Thickness();
			list_Editors[i]->show_Sigma();
			list_Editors[i]->show_Stack_Parameters();
		}
	}
}

void Multilayer::multiple_Refresh_Over_Struct()
{
	refresh_Over_Struct();
	QMetaObject::invokeMethod(this, "refreshOverStruct", Qt::QueuedConnection);
	refresh_Over_Struct();
	QMetaObject::invokeMethod(this, "refreshOverStruct", Qt::QueuedConnection);
}

void Multilayer::refresh_Over_Struct()
{
	different_Layers_Counter=0;
	iterate_Over_Struct();
	iterate_Over_Multilayers();
}

void Multilayer::iterate_Over_Struct(QTreeWidgetItem* item)
{
	// over main tree
	if(item==NULL)
	{
		for(int i=0; i<struct_Tree->topLevelItemCount(); i++)
		{
			refresh_If_Layer(struct_Tree->topLevelItem(i));
		}
	}else
	// over child multilayer
	{
		for(int i=0; i<item->childCount(); i++)
		{
			refresh_If_Layer(item->child(i));
		}
	}
}

void Multilayer::refresh_If_Layer(QTreeWidgetItem* this_Item)
{
	// if not multilayer
	if(this_Item->childCount()==0)
	{
		// just refresh name if ambient or substrate
		if((this_Item->whatsThis(default_Column)==what_is_This_Ambient)||(this_Item->whatsThis(default_Column)==what_is_This_Substrate))
		{
			set_Structure_Item_Text(this_Item);
		} else
		// assign number if layer
		{
			different_Layers_Counter++;
			set_Structure_Item_Text(this_Item);
			QStringList list = this_Item->text(default_Column).split("layer");
			this_Item->setText(default_Column, list[0] + "layer (" + QString::number(different_Layers_Counter) + ")" + list[1]);
			this_Item->setWhatsThis(default_Column, "Layer (" + QString::number(different_Layers_Counter) + ")");

			// TODO works?
			Layer layer = this_Item->data(default_Column, Qt::UserRole).value<Layer>();
			layer.layer_Index = different_Layers_Counter;
			QVariant var; var.setValue( layer );
			this_Item->setData(default_Column, Qt::UserRole, var);
		}
	} else
	// if multilayer
	{
		iterate_Over_Struct(this_Item);
	}
}

void Multilayer::iterate_Over_Multilayers(QTreeWidgetItem* item)
{
	// over main tree
	if(item==NULL)
	{
		for(int i=0; i<struct_Tree->topLevelItemCount(); i++)
		{
			refresh_If_Multilayer(struct_Tree->topLevelItem(i));
		}
	}else
	// over child multilayer
	{
		for(int i=0; i<item->childCount(); i++)
		{
			refresh_If_Multilayer(item->child(i));
		}
	}
}

void Multilayer::refresh_If_Multilayer(QTreeWidgetItem* this_Item)
{
	// if multilayer
	if(this_Item->childCount()>0)
	{
		// first and last layers
		int first =-1, last = -2;
		find_First_Layer(this_Item, first);
		find_Last_Layer(this_Item, last);

		set_Structure_Item_Text(this_Item);
		QStringList list = this_Item->text(default_Column).split("Multilayer");
		this_Item->setText(default_Column, list[0] + "Multilayer: (" + QString::number(first) + " - " + QString::number(last) + ")" + list[1]);
		this_Item->setWhatsThis(default_Column, "Multilayer: (" + QString::number(first) + " - " + QString::number(last) + ")");

		// period
		find_Period(this_Item);

		iterate_Over_Multilayers(this_Item);
	}

}

void Multilayer::find_First_Layer(QTreeWidgetItem* this_Item, int& first)
{
	first=-2016;
	// if first is pure layer
	if(this_Item->child(0)->childCount()==0)
	{
		first = this_Item->child(0)->data(default_Column, Qt::UserRole).value<Layer>().layer_Index;
	} else
	// go deeper
	{
		find_First_Layer(this_Item->child(0), first);
	}
}

void Multilayer::find_Last_Layer(QTreeWidgetItem* this_Item, int& last)
{
	last=-1025;
	// if last is pure layer
	if(this_Item->child(this_Item->childCount()-1)->childCount()==0)
	{
		last = this_Item->child(this_Item->childCount()-1)->data(default_Column, Qt::UserRole).value<Layer>().layer_Index;
	} else
	// go deeper
	{
		find_Last_Layer(this_Item->child(this_Item->childCount()-1), last);
	}
}

void Multilayer::find_Period(QTreeWidgetItem* this_Item)
{
	double period=0;
	// iterate over childs
	for(int i=0; i<this_Item->childCount(); i++)
	{
		// if layer
		if(this_Item->child(i)->childCount()==0) {period += this_Item->child(i)->data(default_Column, Qt::UserRole).value<Layer>().thickness;}
		else
		// go deeper
		{
			find_Period(this_Item->child(i));
			period +=   this_Item->child(i)->data(default_Column, Qt::UserRole).value<Stack_Content>().period
					  * this_Item->child(i)->data(default_Column, Qt::UserRole).value<Stack_Content>().num_Repetition;
		}
	}

	// gamma update
	double gamma=-4;
	if(this_Item->childCount()==2)
	{
		// layer
		if(this_Item->child(0)->childCount()==0)
		{
			gamma =   this_Item->child(0)->data(default_Column, Qt::UserRole).value<Layer>().thickness
					/ this_Item->data(default_Column, Qt::UserRole).value<Stack_Content>().period;
		} else
		// multilayer
		{
			gamma =   this_Item->child(0)->data(default_Column, Qt::UserRole).value<Stack_Content>().period
					* this_Item->child(0)->data(default_Column, Qt::UserRole).value<Stack_Content>().num_Repetition
					/ this_Item->data(default_Column, Qt::UserRole).value<Stack_Content>().period;
		}
	}

	// refresh data
	Stack_Content stack_Content = this_Item->data(default_Column, Qt::UserRole).value<Stack_Content>();
	stack_Content.period = period;
	stack_Content.gamma  = gamma;
	QVariant var; var.setValue(stack_Content);
	this_Item->setData(default_Column, Qt::UserRole, var);
}

// data management

void Multilayer::add_Measured_Data()
{
	gui_Settings.beginGroup( Multilayer_Window_Geometry );
		int multilayer_Height_Additive = gui_Settings.value( "multilayer_Height_Additive", 0 ).toInt();
	gui_Settings.endGroup();

	setUpdatesEnabled(false);

	// window resizing
	if(!data_Measured_Data_Frame_Vector.isEmpty())
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() + multilayer_Height_Additive);

	QFrame* new_Frame = new QFrame;	data_Measured_Data_Frame_Vector.append(new_Frame);
	QHBoxLayout* new_Frame_Layout = new QHBoxLayout(new_Frame);
	new_Frame_Layout->setMargin(0);
	QHBoxLayout* left_Layout  = new QHBoxLayout;
	QHBoxLayout* right_Layout = new QHBoxLayout;
	left_Layout->setSpacing(10);
	right_Layout->setSpacing(0);

	QPushButton* new_Import_Button = new QPushButton("Import");
	QLabel* new_Description_Label  = new QLabel("<no description>");
	QPushButton* new_Add_Button    = new QPushButton("Add Row");
	QPushButton* new_Remove_Button = new QPushButton("Remove");

	new_Import_Button->		setObjectName("Import");
	new_Description_Label->	setObjectName("Description");
	new_Add_Button->		setObjectName("Add Row");
	new_Remove_Button->		setObjectName("Remove Row");

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
	layout_Measured_Data_With_Frame_Vector->addWidget(new_Frame);

	//connect(new_Import_Button, SIGNAL(clicked()), this, SLOT(     ));
	connect(new_Add_Button,    SIGNAL(clicked()), this, SLOT(add_Measured_Data()));
	connect(new_Remove_Button, SIGNAL(clicked()), this, SLOT(remove_Measured_Data()));

	// hiding add button
	QPushButton* add_Button;
	if(data_Measured_Data_Frame_Vector.size()>1)
	{
		add_Button = data_Measured_Data_Frame_Vector[data_Measured_Data_Frame_Vector.size()-2]->findChild<QPushButton*>(new_Add_Button->objectName());
		add_Button->hide();
	}

	setUpdatesEnabled(true);
}

void Multilayer::remove_Measured_Data()
{
	gui_Settings.beginGroup( Multilayer_Window_Geometry );
		int multilayer_Height_Additive = gui_Settings.value( "multilayer_Height_Additive", 0 ).toInt();
	gui_Settings.endGroup();

	setUpdatesEnabled(false);

	QString add_Name = data_Measured_Data_Frame_Vector.first()->findChildren<QPushButton*>().end()[-2]->objectName();	// add button is the second from the end

	int i0=-1;
	for(int i=0; i<data_Measured_Data_Frame_Vector.size(); i++)
		if(QObject::sender()->parent() == data_Measured_Data_Frame_Vector[i]) i0=i;
	if(i0!=-1) data_Measured_Data_Frame_Vector.remove(i0);
	delete QObject::sender()->parent();

	// window resizing
	if(!data_Measured_Data_Frame_Vector.isEmpty())
	{
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() - multilayer_Height_Additive);
		adjustSize();
	}

	if(data_Measured_Data_Frame_Vector.isEmpty())
		add_Measured_Data();

	setUpdatesEnabled(false);

	// showing add button
	QPushButton* add_Button;
	add_Button = data_Measured_Data_Frame_Vector.last()->findChild<QPushButton*>(add_Name);
	add_Button->show();

	setUpdatesEnabled(true);
}

void Multilayer::add_Target_Profile()
{
	gui_Settings.beginGroup( Multilayer_Window_Geometry );
		int multilayer_Height_Additive = gui_Settings.value( "multilayer_Height_Additive", 0 ).toInt();
	gui_Settings.endGroup();

	setUpdatesEnabled(false);

	// window resizing
	if(!data_Target_Profile_Frame_Vector.isEmpty())
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() + multilayer_Height_Additive);

	QFrame* new_Frame = new QFrame;	data_Target_Profile_Frame_Vector.append(new_Frame);


	QHBoxLayout* new_Frame_Layout = new QHBoxLayout(new_Frame);
	new_Frame_Layout->setMargin(0);
	QHBoxLayout* left_Layout  = new QHBoxLayout;
	QHBoxLayout* right_Layout = new QHBoxLayout;
	left_Layout->setSpacing(10);
	right_Layout->setSpacing(0);

	QPushButton* new_Import_Button = new QPushButton("Import");
	QLabel* new_Description_Label  = new QLabel("<no description>");
	QPushButton* new_Add_Button    = new QPushButton("Add Row");
	QPushButton* new_Remove_Button = new QPushButton("Remove");

	new_Import_Button->		setObjectName("Import");
	new_Description_Label->	setObjectName("Description");
	new_Add_Button->		setObjectName("Add Row");
	new_Remove_Button->		setObjectName("Remove Row");

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
	layout_Target_Profile_With_Frame_Vector->addWidget(new_Frame);

	//connect(new_Import_Button, SIGNAL(clicked()), this, SLOT(     ));
	connect(new_Add_Button,    SIGNAL(clicked()), this, SLOT(add_Target_Profile()));
	connect(new_Remove_Button, SIGNAL(clicked()), this, SLOT(remove_Target_Profile()));

	// hiding add button
	QPushButton* add_Button;
	if(data_Target_Profile_Frame_Vector.size()>1)
	{
		add_Button = data_Target_Profile_Frame_Vector[data_Target_Profile_Frame_Vector.size()-2]->findChild<QPushButton*>(new_Add_Button->objectName());
		add_Button->hide();
	}

	setUpdatesEnabled(true);
}

void Multilayer::remove_Target_Profile()
{
	gui_Settings.beginGroup( Multilayer_Window_Geometry );
		int multilayer_Height_Additive = gui_Settings.value( "multilayer_Height_Additive", 0 ).toInt();
	gui_Settings.endGroup();

	setUpdatesEnabled(false);

	QString add_Name = data_Target_Profile_Frame_Vector.first()->findChildren<QPushButton*>().end()[-2]->objectName();	// add_button is the second from the end

	int i0=-1;
	for(int i=0; i<data_Target_Profile_Frame_Vector.size(); i++)
		if(QObject::sender()->parent() == data_Target_Profile_Frame_Vector[i]) i0=i;
	if(i0!=-1) data_Target_Profile_Frame_Vector.remove(i0);
	delete QObject::sender()->parent();

	// window resizing
	if(!data_Target_Profile_Frame_Vector.isEmpty())
	{
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() - multilayer_Height_Additive);
		adjustSize();
	}

	if(data_Target_Profile_Frame_Vector.isEmpty())
		add_Target_Profile();

	setUpdatesEnabled(false);

	// showing add button
	QPushButton* add_Button;
	add_Button = data_Target_Profile_Frame_Vector.last()->findChild<QPushButton*>(add_Name);
	add_Button->show();

	setUpdatesEnabled(true);
}
