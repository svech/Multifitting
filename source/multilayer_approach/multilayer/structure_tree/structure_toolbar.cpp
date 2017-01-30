// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "structure_toolbar.h"

Structure_Toolbar::Structure_Toolbar(Structure_Tree* structure_Tree, QWidget *parent) :
	QWidget(parent),
	structure_Tree(structure_Tree)
{
	create_Toolbar();
}

void Structure_Toolbar::create_Toolbar()
{
	QPixmap add_Layer		(icon_path + "add_layer.bmp");
	QPixmap add_Multilayer  (icon_path + "add_multilayer.bmp");
	QPixmap add_Substrate	(icon_path + "add_substrate.bmp");
	QPixmap edit			(icon_path + "roi.bmp");
	QPixmap remove			(icon_path + "delete.bmp");
	QPixmap cut				(icon_path + "cut.bmp");
	QPixmap copy			(icon_path + "copy.bmp");
	QPixmap paste			(icon_path + "paste.bmp");
	QPixmap move_Up			(icon_path + "shift_up.bmp");
	QPixmap move_Down		(icon_path + "shift_down.bmp");
	QPixmap group			(icon_path + "group.bmp");
	QPixmap ungroup			(icon_path + "ungroup.bmp");
	QPixmap thickness_Plot	(icon_path + "zplot.bmp");
	QPixmap sigma_Plot		(icon_path + "sigmaplot.bmp");
	QPixmap destroy			(icon_path + "bomb.bmp");

	toolbar = new QToolBar;
	toolbar->addAction(QIcon(add_Layer),		"Add Layer");						// 0
	toolbar->addAction(QIcon(add_Multilayer),	"Add Multilayer");					// 1
	toolbar->addAction(QIcon(add_Substrate),	"Add Substrate");					// 2
	toolbar->addAction(QIcon(edit),				"Edit");							// 3
	toolbar->addAction(QIcon(remove),			"Remove");							// 4
	toolbar->addAction(QIcon(cut),				"Cut");								// 5
	toolbar->addAction(QIcon(copy),				"Copy");							// 6
	toolbar->addAction(QIcon(paste),			"Paste");							// 7
	toolbar->addAction(QIcon(move_Up),			"Move Up");							// 8
	toolbar->addAction(QIcon(move_Down),		"Move Down");						// 9
	toolbar->addAction(QIcon(group),			"Group");							// 10
	toolbar->addAction(QIcon(ungroup),			"Ungroup");							// 11
	toolbar->addAction(QIcon(thickness_Plot),	"Plot Layer Thickness Profile");	// 12
	toolbar->addAction(QIcon(sigma_Plot),		"Plot Interface Width Profile");	// 13
	toolbar->addAction(QIcon(destroy),			"Remove Substrate and All Layers");	// 14

	toolbar->setIconSize(add_Layer.size());

	connect(toolbar->actions()[0],  SIGNAL(triggered(bool)), this, SLOT(add_Layer(bool)));
	connect(toolbar->actions()[1],  SIGNAL(triggered(bool)), this, SLOT(add_Multilayer(bool)));
	connect(toolbar->actions()[2],  SIGNAL(triggered(bool)), this, SLOT(add_Substrate(bool)));
	connect(toolbar->actions()[3],  SIGNAL(triggered(bool)), this, SLOT(edit(bool)));
	connect(toolbar->actions()[4],  SIGNAL(triggered(bool)), this, SLOT(remove(bool)));
	connect(toolbar->actions()[5],  SIGNAL(triggered(bool)), this, SLOT(cut(bool)));
	connect(toolbar->actions()[6],  SIGNAL(triggered(bool)), this, SLOT(copy(bool)));
	connect(toolbar->actions()[7],  SIGNAL(triggered(bool)), this, SLOT(paste(bool)));
	connect(toolbar->actions()[8],  SIGNAL(triggered(bool)), this, SLOT(move_Up(bool)));
	connect(toolbar->actions()[9],  SIGNAL(triggered(bool)), this, SLOT(move_Down(bool)));
	connect(toolbar->actions()[10], SIGNAL(triggered(bool)), this, SLOT(group(bool)));
	connect(toolbar->actions()[11], SIGNAL(triggered(bool)), this, SLOT(ungroup(bool)));
	connect(toolbar->actions()[12], SIGNAL(triggered(bool)), this, SLOT(thickness_Plot(bool)));
	connect(toolbar->actions()[13], SIGNAL(triggered(bool)), this, SLOT(sigma_Plot(bool)));
	connect(toolbar->actions()[14], SIGNAL(triggered(bool)), this, SLOT(destroy(bool)));

	if_Selected();
}

void Structure_Toolbar::add_Layer(bool)
{
	QTreeWidgetItem* new_Layer = new QTreeWidgetItem;

	// setting data to new layerItem
	QVariant var;
	Layer layer;
	var.setValue( layer );
	new_Layer->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	add_Buffered_Layer(new_Layer);
	refresh_Toolbar();
}

void Structure_Toolbar::add_Multilayer(bool)
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
		QMessageBox::warning(this, "Warning", text + " is not a integer number");
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
		new_Child_Layer->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		structure_Tree->set_Structure_Item_Text(new_Child_Layer);
	}
	new_Multilayer->addChildren(new_Child_Layers);
	Stack_Content stack_Content;
	QVariant var;
	var.setValue( stack_Content );
	new_Multilayer->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	// if no selected items
	if(structure_Tree->tree->selectedItems().isEmpty())
	{
		// if there is no substrate
		if(structure_Tree->tree->topLevelItem(structure_Tree->tree->topLevelItemCount()-1)->whatsThis(DEFAULT_COLUMN)!=whats_This_Substrate)
		{
			structure_Tree->tree->addTopLevelItem(new_Multilayer);
		} else
		{
			structure_Tree->tree->insertTopLevelItem(structure_Tree->tree->topLevelItemCount()-1,new_Multilayer);
		}
	} else
	{
		// if selected item is top-level
		if(!structure_Tree->tree->currentItem()->parent())
		{
			int position = structure_Tree->tree->indexOfTopLevelItem(structure_Tree->tree->currentItem());

			// place multilayers before substrate
			if(  (structure_Tree->tree->topLevelItem(structure_Tree->tree->topLevelItemCount()-1)->whatsThis(DEFAULT_COLUMN)!=whats_This_Substrate)
			   ||(position != (structure_Tree->tree->topLevelItemCount()-1)))
			{
				structure_Tree->tree->insertTopLevelItem(position+1, new_Multilayer);
			} else
			{
				structure_Tree->tree->insertTopLevelItem(position, new_Multilayer);
			}
		} else
		{
			int position = structure_Tree->tree->currentIndex().row();
			structure_Tree->tree->currentItem()->parent()->insertChild(position+1,new_Multilayer);
		}
	}

	structure_Tree->set_Structure_Item_Text(new_Multilayer);
	refresh_Toolbar();
}

void Structure_Toolbar::add_Substrate(bool)
{
	QTreeWidgetItem* new_Substrate = new QTreeWidgetItem;
		new_Substrate->setWhatsThis(DEFAULT_COLUMN, whats_This_Substrate);

	structure_Tree->tree->addTopLevelItem(new_Substrate);
	structure_Tree->set_Structure_Item_Text(new_Substrate);
	toolbar->actions()[2]->setDisabled(true);

	refresh_Toolbar();
}

void Structure_Toolbar::edit(bool)
{
	structure_Tree->if_DoubleClicked(structure_Tree->tree->currentItem(), DEFAULT_COLUMN);
}

void Structure_Toolbar::remove(bool)
{
	QTreeWidgetItem* current = structure_Tree->tree->currentItem();
	// TODO show item name before removal
	if((current->parent())&&(current->parent()->childCount()==2))
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Multilayer " + current->parent()->whatsThis(DEFAULT_COLUMN)+" will be disbanded.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			QTreeWidgetItem* parent = current->parent();
			delete current;

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
				int parent_Position = structure_Tree->tree->indexOfTopLevelItem(parent);
				structure_Tree->tree->insertTopLevelItem(parent_Position, survived_Child_Copy);
				delete parent;
			}
		}
	} else
	if(current->whatsThis(DEFAULT_COLUMN) != whats_This_Ambient)
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove " + current->whatsThis(DEFAULT_COLUMN)+"?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			if(current->whatsThis(DEFAULT_COLUMN) == whats_This_Substrate)
			{
				toolbar->actions()[2]->setDisabled(false);		// add_Substrate
			}
			delete current;
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
			new_Ambient->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			new_Ambient->setWhatsThis(DEFAULT_COLUMN,whats_This_Ambient);
			structure_Tree->tree->insertTopLevelItem(1,new_Ambient);

			delete current;
		}
	}

	refresh_Toolbar();
}

void Structure_Toolbar::cut(bool)
{
	QTreeWidgetItem* current = structure_Tree->tree->currentItem();
	buffered = structure_Tree->tree->currentItem()->clone();

	// TODO show item name before cut
	if((current->parent())&&(current->parent()->childCount()==2))
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Cut", "Multilayer " + current->parent()->whatsThis(DEFAULT_COLUMN)+" will be disbanded.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			QTreeWidgetItem* parent = current->parent();
			delete current;

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
				int parent_Position = structure_Tree->tree->indexOfTopLevelItem(parent);
				structure_Tree->tree->insertTopLevelItem(parent_Position, survived_Child_Copy);
				delete parent;
			}
		}
	} else
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Cut", "Really cut " + current->whatsThis(DEFAULT_COLUMN)+"?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			if(current->whatsThis(DEFAULT_COLUMN) == whats_This_Substrate)
			{
				toolbar->actions()[2]->setDisabled(false);
			}
			delete current;
		}
	}
	refresh_Toolbar();
}

void Structure_Toolbar::copy(bool)
{
	buffered = structure_Tree->tree->currentItem()->clone();
	refresh_Toolbar();
}

void Structure_Toolbar::paste(bool)
{
	add_Buffered_Layer(buffered);
}

void Structure_Toolbar::move_Up(bool)
{
	int position = structure_Tree->tree->currentIndex().row();
	QTreeWidgetItem* parent = structure_Tree->tree->currentItem()->parent();

	// if nested
	if(parent)
	{
		QTreeWidgetItem* taken = parent->takeChild(position-1);
		parent->insertChild(position, taken);
	} else
	{
		QTreeWidgetItem* taken = structure_Tree->tree->takeTopLevelItem(position-1);
		structure_Tree->tree->insertTopLevelItem(position, taken);
	}

	refresh_Toolbar();
}

void Structure_Toolbar::move_Down(bool)
{
	int position = structure_Tree->tree->currentIndex().row();
	QTreeWidgetItem* parent = structure_Tree->tree->currentItem()->parent();

	// if nested
	if(parent)
	{
		QTreeWidgetItem* taken = parent->takeChild(position+1);
		parent->insertChild(position, taken);
	} else
	{
		QTreeWidgetItem* taken = structure_Tree->tree->takeTopLevelItem(position+1);
		structure_Tree->tree->insertTopLevelItem(position, taken);
	}

	refresh_Toolbar();
}

void Structure_Toolbar::group(bool)
{
	// TODO group_toolbutton
	qInfo() << "group is not implemented";

	refresh_Toolbar();
}

void Structure_Toolbar::ungroup(bool)
{
	QTreeWidgetItem* current = structure_Tree->tree->currentItem();
	int position = structure_Tree->tree->currentIndex().row();

	// deep copy of children
	QList<QTreeWidgetItem*> child_List;
	for(int i=0; i<current->childCount(); i++)
	{
		child_List.append(current->child(i)->clone());
	}

	// if nested
	if(current->parent())
	{
		current->parent()->insertChildren(position+1, child_List);
		delete current;
	} else
	{
		structure_Tree->tree->insertTopLevelItems(position+1, child_List);
		delete current;
	}

	refresh_Toolbar();
}

void Structure_Toolbar::thickness_Plot(bool)
{
	// TODO thickness_Plot_toolbutton
	qInfo() << "thickness_Plot is not implemented";
}

void Structure_Toolbar::sigma_Plot(bool)
{
	// TODO sigma_Plot_toolbutton
	qInfo() << "sigma_Plot is not implemented";
}

void Structure_Toolbar::destroy(bool)
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Remove substrate and all layers?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		structure_Tree->tree->clear();
		add_Layer(true);
		toolbar->actions()[2]->setDisabled(false);		// add_Substrate
		refresh_Toolbar();
	}
}

void Structure_Toolbar::if_Selected()
{
	if(buffered!=NULL)
	{
		toolbar->actions()[7]->setDisabled(false);		 // paste
	} else
	{
		toolbar->actions()[7]->setDisabled(true);		 // paste
	}

	toolbar->actions()[11]->setDisabled(true);			 // ungroup

	if(structure_Tree->tree->selectedItems().isEmpty())
	{
		toolbar->actions()[3]->setDisabled(true);		 // edit
		toolbar->actions()[4]->setDisabled(true);		 // remove
		toolbar->actions()[5]->setDisabled(true);		 // cut
		toolbar->actions()[6]->setDisabled(true);		 // copy
		toolbar->actions()[8]->setDisabled(true);		 // move_Up
		toolbar->actions()[9]->setDisabled(true);		 // move_Down
		toolbar->actions()[10]->setDisabled(true);		 // group
		toolbar->actions()[12]->setDisabled(true);		 // thickness_Plot
		toolbar->actions()[13]->setDisabled(true);		 // sigma_Plot
	} else
	{
		bool if_Substrate = structure_Tree->tree->currentItem()->whatsThis(DEFAULT_COLUMN)==whats_This_Substrate;

		toolbar->actions()[3]->setDisabled(false);		 // edit
		toolbar->actions()[4]->setDisabled(false);		 // remove

		// if multilayer
		if(structure_Tree->tree->currentItem()->childCount()>0)
		{
			toolbar->actions()[11]->setDisabled(false);		 // ungroup
		}

		int position = structure_Tree->tree->currentIndex().row();
		QTreeWidgetItem* parent = structure_Tree->tree->currentItem()->parent();

		// if nested
		if(parent)
		{
			toolbar->actions()[5]->setDisabled(false);		 // cut
			toolbar->actions()[6]->setDisabled(false);		 // copy

			if(position == 0)
			{
				toolbar->actions()[8]->setDisabled(true);	// move_Up
				toolbar->actions()[9]->setDisabled(false);	// move_Down
			} else
			if(position == (parent->childCount()-1))
			{
				toolbar->actions()[8]->setDisabled(false);	// move_Up
				toolbar->actions()[9]->setDisabled(true);	// move_Down
			} else
			{
				toolbar->actions()[8]->setDisabled(false);	// move_Up
				toolbar->actions()[9]->setDisabled(false);	// move_Down
			}
		} else
		{
			// if ambient
			if(position == 0)
			{
				toolbar->actions()[5]->setDisabled(true);	// cut
				toolbar->actions()[6]->setDisabled(true);	// copy
				toolbar->actions()[8]->setDisabled(true);	// move_Up
				toolbar->actions()[9]->setDisabled(true);	// move_Down
				toolbar->actions()[10]->setDisabled(true);	// group
			} else
			// if not at end
			if(position<structure_Tree->tree->topLevelItemCount()-1)
			{
				toolbar->actions()[5]->setDisabled(false);	// cut
				toolbar->actions()[6]->setDisabled(false);	// copy

				// if next is not substrate
				if(structure_Tree->tree->topLevelItem(position+1)->whatsThis(DEFAULT_COLUMN)!=whats_This_Substrate)
				{
					toolbar->actions()[9]->setDisabled(false);	// move_Down
				}
				// if second
				if(position == 1)
				{
					toolbar->actions()[8]->setDisabled(true);	// move_Up
				} else
				{
					toolbar->actions()[8]->setDisabled(false);	// move_Up
				}
			} else
			{ // if at end
				if(if_Substrate)
				{
					toolbar->actions()[5]->setDisabled(true);	// cut
					toolbar->actions()[6]->setDisabled(true);	// copy
					toolbar->actions()[8]->setDisabled(true);	// move_Up
				} else
				{
					// if second
					if(position == 1)
					{
						toolbar->actions()[8]->setDisabled(true);	// move_Up
					} else
					{
						toolbar->actions()[8]->setDisabled(false);	// move_Up
					}
					toolbar->actions()[5]->setDisabled(false);	// cut
					toolbar->actions()[6]->setDisabled(false);	// copy
				}
				toolbar->actions()[9]->setDisabled(true);	// move_Down
			}
		}
	}
}

void Structure_Toolbar::refresh_Toolbar()
{
	if(!structure_Tree->tree->selectedItems().isEmpty())
	{
		structure_Tree->tree->currentItem()->setSelected(false);
		structure_Tree->tree->currentItem()->setSelected(true);
	}
	structure_Tree->tree->expandAll();

	structure_Tree->multiple_Refresh_Over_Struct();
}

void Structure_Toolbar::add_Buffered_Layer(QTreeWidgetItem* new_Layer_Passed)
{
	// copy of new_Layer
	QTreeWidgetItem* new_Layer = new QTreeWidgetItem;
	new_Layer = new_Layer_Passed->clone(); //-V519

	// if no structure_Tree->tree at all (at the beginning)
	if(structure_Tree->tree->topLevelItemCount()==0)
	{
		QVariant var;
		Ambient ambient;
		var.setValue( ambient );
		new_Layer->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		new_Layer->setWhatsThis(DEFAULT_COLUMN,whats_This_Ambient);
		structure_Tree->tree->addTopLevelItem(new_Layer);
	} else
	{
		// if no selected items
		if(structure_Tree->tree->selectedItems().isEmpty())
		{
			// if there is no substrate
			if(structure_Tree->tree->topLevelItem(structure_Tree->tree->topLevelItemCount()-1)->whatsThis(DEFAULT_COLUMN)!=whats_This_Substrate)
			{
				structure_Tree->tree->addTopLevelItem(new_Layer);
			} else
			{
				structure_Tree->tree->insertTopLevelItem(structure_Tree->tree->topLevelItemCount()-1,new_Layer);
			}
		} else
		{
			// if selected item is top-level
			if(!structure_Tree->tree->currentItem()->parent())
			{
				int position = structure_Tree->tree->indexOfTopLevelItem(structure_Tree->tree->currentItem());

				// place layers before substrate
				if(  (structure_Tree->tree->topLevelItem(structure_Tree->tree->topLevelItemCount()-1)->whatsThis(DEFAULT_COLUMN)!=whats_This_Substrate)
				   ||(position != (structure_Tree->tree->topLevelItemCount()-1)))
				{
					structure_Tree->tree->insertTopLevelItem(position+1, new_Layer);
				} else
				{
					structure_Tree->tree->insertTopLevelItem(position, new_Layer);
				}
			} else
			{
				int position = structure_Tree->tree->currentIndex().row();
				structure_Tree->tree->currentItem()->parent()->insertChild(position+1,new_Layer);
			}
		}
	}

	structure_Tree->set_Structure_Item_Text(new_Layer);
	refresh_Toolbar();
}
