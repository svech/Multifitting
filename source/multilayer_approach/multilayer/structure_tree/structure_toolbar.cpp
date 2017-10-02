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

	connect(toolbar->actions()[0],  &QAction::triggered, this, &Structure_Toolbar::add_Layer);
	connect(toolbar->actions()[1],  &QAction::triggered, this, &Structure_Toolbar::add_Multilayer);
	connect(toolbar->actions()[2],  &QAction::triggered, this, &Structure_Toolbar::add_Substrate);
	connect(toolbar->actions()[3],  &QAction::triggered, this, &Structure_Toolbar::edit);
	connect(toolbar->actions()[4],  &QAction::triggered, this, &Structure_Toolbar::remove);
	connect(toolbar->actions()[5],  &QAction::triggered, this, &Structure_Toolbar::cut);
	connect(toolbar->actions()[6],  &QAction::triggered, this, &Structure_Toolbar::copy);
	connect(toolbar->actions()[7],  &QAction::triggered, this, &Structure_Toolbar::paste);
	connect(toolbar->actions()[8],  &QAction::triggered, this, &Structure_Toolbar::move_Up);
	connect(toolbar->actions()[9],  &QAction::triggered, this, &Structure_Toolbar::move_Down);
	connect(toolbar->actions()[10], &QAction::triggered, this, &Structure_Toolbar::group);
	connect(toolbar->actions()[11], &QAction::triggered, this, &Structure_Toolbar::ungroup);
	connect(toolbar->actions()[12], &QAction::triggered, this, &Structure_Toolbar::thickness_Plot);
	connect(toolbar->actions()[13], &QAction::triggered, this, &Structure_Toolbar::sigma_Plot);
	connect(toolbar->actions()[14], &QAction::triggered, this, &Structure_Toolbar::destroy);

	if_Selected();
}

void Structure_Toolbar::change_IDs_Of_Subtree(QTreeWidgetItem* item)
{
	// parent
	Data data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	data.reset_All_IDs();
	QVariant var; var.setValue( data );
	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	// children
	for(int i=0; i<item->childCount(); i++)
	{
		change_IDs_Of_Subtree(item->child(i));
	}
}

void Structure_Toolbar::add_Ambient()
{
	// item
	QTreeWidgetItem* new_Ambient = new QTreeWidgetItem;

	// data
	QVariant var;
	Data ambient(item_Type_Ambient);
	var.setValue( ambient );
	new_Ambient->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	structure_Tree->tree->addTopLevelItem(new_Ambient);
	structure_Tree->set_Structure_Item_Text(new_Ambient);

//	refresh_Toolbar(); // doesn't work here. I dont know why
}

void Structure_Toolbar::add_Layer()
{
	// item
	QTreeWidgetItem* new_Layer = new QTreeWidgetItem;

	// data
	QVariant var;
	Data layer(item_Type_Layer);
	var.setValue( layer );
	new_Layer->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	buffered_Copy_Type = copy_Type_Cut;
	add_Buffered_Layer(new_Layer);
}

void Structure_Toolbar::add_Multilayer()
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
		add_Multilayer();
	} else
	if (text.toInt()<2)
	{
		QMessageBox::warning(this, "Warning", "There should be at least 2 layers");
		add_Multilayer();
	}
	if(num_Children<2) return;

	// item
	QTreeWidgetItem* new_Multilayer = new QTreeWidgetItem;

	// children
	QList<QTreeWidgetItem*> new_Child_Layers;
	for(int i=0; i<num_Children; i++)
	{
		// item
		QTreeWidgetItem* new_Child_Layer = new QTreeWidgetItem;

		// data
		Data layer(item_Type_Layer);
		QVariant var;
		var.setValue( layer );
		new_Child_Layer->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		new_Child_Layers << new_Child_Layer;
	}
	new_Multilayer->addChildren(new_Child_Layers);

	// data
	Data multilayer(item_Type_Multilayer);
	QVariant var;
	var.setValue( multilayer );
	new_Multilayer->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	buffered_Copy_Type = copy_Type_Cut;
	add_Buffered_Layer(new_Multilayer);
}

void Structure_Toolbar::add_Substrate()
{
	// item
	QTreeWidgetItem* new_Substrate = new QTreeWidgetItem;

	// data
	QVariant var;
	Data data(item_Type_Substrate);
	var.setValue( data );
	new_Substrate->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	structure_Tree->tree->addTopLevelItem(new_Substrate);

	toolbar->actions()[2]->setDisabled(true);
	refresh_Toolbar();
}

void Structure_Toolbar::edit()
{
	structure_Tree->if_DoubleClicked();
}

void Structure_Toolbar::remove()
{
	QTreeWidgetItem* current = structure_Tree->tree->currentItem();
	Data data = current->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	// if parent multilayer has 2 childs
	if((current->parent())&&(current->parent()->childCount()==2))
	{
		QString multilayer_Text = current->parent()->text(DEFAULT_COLUMN).split(", N")[0];
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", multilayer_Text + " will be disbanded.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			QTreeWidgetItem* parent = current->parent();
			delete current;

			QTreeWidgetItem* survived_Child = parent->child(0)->clone();

			// if multilayer is already nested
			if(parent->parent())
			{
				int parent_Position = parent->parent()->indexOfChild(parent);
				parent->parent()->insertChild(parent_Position, survived_Child);
				delete parent;
			} else
			{
				int parent_Position = structure_Tree->tree->indexOfTopLevelItem(parent);
				structure_Tree->tree->insertTopLevelItem(parent_Position, survived_Child);
				delete parent;
			}
		}
	} else
	// if ambient
	if(data.item_Type == item_Type_Ambient)
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really reset ambient?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			QVariant var;

			Data ambient(item_Type_Ambient);
			var.setValue( ambient );
			current->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
	} else
	// if layer
	if(data.item_Type == item_Type_Layer)
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove layer " + QString::number(data.layer_Index) + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)	delete current;
	} else
	// if multilayer
	if(data.item_Type == item_Type_Multilayer)
	{
		QString multilayer_Text = current->text(DEFAULT_COLUMN).split(", N")[0];
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove " + multilayer_Text + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)	delete current;
	} else
	if(data.item_Type == item_Type_Substrate)
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove substrate?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			toolbar->actions()[2]->setDisabled(false);		// add_Substrate button
			delete current;
		}
	}

	refresh_Toolbar();
}

void Structure_Toolbar::cut()
{
	QTreeWidgetItem* current = structure_Tree->tree->currentItem();
	buffered_Copy_Type = copy_Type_Cut;
	buffered = structure_Tree->tree->currentItem()->clone();
	Data data = current->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	// if parent multilayer has 2 childs
	if((current->parent())&&(current->parent()->childCount()==2))
	{
		QString multilayer_Text = current->parent()->text(DEFAULT_COLUMN).split(", N")[0];
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Cut", multilayer_Text + " will be disbanded.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			QTreeWidgetItem* parent = current->parent();
			delete current;

			QTreeWidgetItem* survived_Child_Copy = new QTreeWidgetItem;
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
	// if layer
	if(data.item_Type == item_Type_Layer)
	{
			QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really cut layer " + QString::number(data.layer_Index) + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
			if (reply == QMessageBox::Yes) delete current;
	} else
	// if multilayer
	if(data.item_Type == item_Type_Multilayer)
	{
		QString multilayer_Text = current->text(DEFAULT_COLUMN).split(", N")[0];
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really cut " + multilayer_Text + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)	delete current;
	}
	refresh_Toolbar();
}

void Structure_Toolbar::copy()
{
	buffered_Copy_Type = copy_Type_Copy;
	buffered = structure_Tree->tree->currentItem()->clone();
	refresh_Toolbar();
}

void Structure_Toolbar::paste()
{
	add_Buffered_Layer(buffered);
	buffered_Copy_Type = copy_Type_Copy;	 // next copies should have changed IDs
}

void Structure_Toolbar::move_Up()
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

void Structure_Toolbar::move_Down()
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

void Structure_Toolbar::group()
{
	// TODO group_toolbutton
	qInfo() << "group is not implemented";

	refresh_Toolbar();
}

void Structure_Toolbar::ungroup()
{
	QTreeWidgetItem* current = structure_Tree->tree->currentItem();
	int position = structure_Tree->tree->currentIndex().row();

	QList<QTreeWidgetItem*> child_List;
	for(int i=0; i<current->childCount(); i++)
	{
		// deep copy of children
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

void Structure_Toolbar::thickness_Plot()
{
	// TODO thickness_Plot_toolbutton
	qInfo() << "thickness_Plot is not implemented";
}

void Structure_Toolbar::sigma_Plot()
{
	// TODO sigma_Plot_toolbutton
	qInfo() << "sigma_Plot is not implemented";
}

void Structure_Toolbar::destroy()
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Remove substrate and all layers?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if (reply == QMessageBox::Yes)
	{
		structure_Tree->tree->clear();
		add_Ambient();
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
		bool if_Substrate = (structure_Tree->tree->currentItem()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type) == item_Type_Substrate;

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
				if(structure_Tree->tree->topLevelItem(position+1)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type!=item_Type_Substrate)
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

//----------------------------------------------------------------------

void Structure_Toolbar::refresh_Toolbar()
{
	if(!structure_Tree->tree->selectedItems().isEmpty())
	{
		structure_Tree->tree->currentItem()->setSelected(false);
		structure_Tree->tree->currentItem()->setSelected(true);
	}
	structure_Tree->tree->expandAll();

	emit refresh_Str_And_Independ_signal();
}

void Structure_Toolbar::add_Buffered_Layer(QTreeWidgetItem* new_Layer_Passed)
{
	QTreeWidgetItem* new_Layer = new_Layer_Passed->clone();

	// need to change IDs
	if(buffered_Copy_Type == copy_Type_Copy)
	{
		change_IDs_Of_Subtree(new_Layer);
	}

	// if we have some items in tree
	int last_Top_Index = structure_Tree->tree->topLevelItemCount()-1;
	if(last_Top_Index>=0)
	{
		// if no selected items
		if(structure_Tree->tree->selectedItems().isEmpty())
		{
			// if there is no substrate
			if(structure_Tree->tree->topLevelItem(last_Top_Index)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type!=item_Type_Substrate)
			{
				structure_Tree->tree->addTopLevelItem(new_Layer);
			} else
			{
				structure_Tree->tree->insertTopLevelItem(last_Top_Index,new_Layer);
			}
		} else
		{
			// if selected item is top-level
			if(!structure_Tree->tree->currentItem()->parent())
			{
				int position = structure_Tree->tree->indexOfTopLevelItem(structure_Tree->tree->currentItem());

				// place layers before substrate
				if(  (structure_Tree->tree->topLevelItem(last_Top_Index)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type!=item_Type_Substrate)
				   ||(position != last_Top_Index))
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

	refresh_Toolbar();
}
