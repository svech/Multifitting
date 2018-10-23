// This is an open source non-commercial project. Dear PVS-Studio, please check it.
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
	QPixmap add_Aperiodic	(icon_path + "taj.bmp");
	QPixmap add_Substrate	(icon_path + "add_substrate.bmp");
	QPixmap edit			(icon_path + "roi.bmp");
	QPixmap remove			(icon_path + "delete.bmp");
	QPixmap cut				(icon_path + "cut.bmp");
	QPixmap copy			(icon_path + "copy.bmp");
	QPixmap paste			(icon_path + "paste.bmp");
	QPixmap move_Up			(icon_path + "shift_up.bmp");
	QPixmap move_Down		(icon_path + "shift_down.bmp");
//	QPixmap group			(icon_path + "group.bmp");
	QPixmap ungroup			(icon_path + "ungroup.bmp");
//	QPixmap thickness_Plot	(icon_path + "zplot.bmp");
//	QPixmap sigma_Plot		(icon_path + "sigmaplot.bmp");
	QPixmap destroy			(icon_path + "bomb.bmp");

	toolbar = new QToolBar;
	toolbar->addAction(QIcon(add_Layer),		"Add Layer");						// 0
	toolbar->addAction(QIcon(add_Multilayer),	"Add Multilayer");					// 1
	toolbar->addAction(QIcon(add_Aperiodic),	"Add Aperiodic Multilayer");		// 2
	toolbar->addAction(QIcon(add_Substrate),	"Add Substrate");					// 3
	toolbar->addAction(QIcon(edit),				"Edit");							// 4
	toolbar->addAction(QIcon(remove),			"Remove");							// 5
	toolbar->addAction(QIcon(cut),				"Cut");								// 6
	toolbar->addAction(QIcon(copy),				"Copy");							// 7
	toolbar->addAction(QIcon(paste),			"Paste");							// 8
	toolbar->addAction(QIcon(move_Up),			"Move Up");							// 9
	toolbar->addAction(QIcon(move_Down),		"Move Down");						// 0
//	toolbar->addAction(QIcon(group),			"Group");							// 11
	toolbar->addAction(QIcon(ungroup),			"Ungroup");							// 12
//	toolbar->addAction(QIcon(thickness_Plot),	"Plot Layer Thickness Profile");	// 13
//	toolbar->addAction(QIcon(sigma_Plot),		"Plot Interface Width Profile");	// 14
	toolbar->addAction(QIcon(destroy),			"Remove Substrate and All Layers");	// 15

	toolbar->setIconSize(add_Layer.size());

	connect(toolbar->actions()[Add_Layer],		&QAction::triggered, this, &Structure_Toolbar::add_Layer);
	connect(toolbar->actions()[Add_Multilayer], &QAction::triggered, this, &Structure_Toolbar::add_Multilayer);
	connect(toolbar->actions()[Add_Aperiodic],  &QAction::triggered, this, &Structure_Toolbar::add_Aperiodic);
	connect(toolbar->actions()[Add_Substrate],  &QAction::triggered, this, &Structure_Toolbar::add_Substrate);
	connect(toolbar->actions()[Edit],			&QAction::triggered, this, &Structure_Toolbar::edit);
	connect(toolbar->actions()[Remove],			&QAction::triggered, this, &Structure_Toolbar::remove);
	connect(toolbar->actions()[Cut],			&QAction::triggered, this, &Structure_Toolbar::cut);
	connect(toolbar->actions()[Copy],			&QAction::triggered, this, &Structure_Toolbar::copy);
	connect(toolbar->actions()[Paste],			&QAction::triggered, this, &Structure_Toolbar::paste);
	connect(toolbar->actions()[Move_Up],		&QAction::triggered, this, &Structure_Toolbar::move_Up);
	connect(toolbar->actions()[Move_Down],		&QAction::triggered, this, &Structure_Toolbar::move_Down);
//	connect(toolbar->actions()[Group],			&QAction::triggered, this, &Structure_Toolbar::group);
	connect(toolbar->actions()[Ungroup],		&QAction::triggered, this, &Structure_Toolbar::ungroup);
//	connect(toolbar->actions()[Thickness_Plot], &QAction::triggered, this, &Structure_Toolbar::thickness_Plot);
//	connect(toolbar->actions()[Sigma_Plot],		&QAction::triggered, this, &Structure_Toolbar::sigma_Plot);
	connect(toolbar->actions()[Destroy],		&QAction::triggered, this, &Structure_Toolbar::destroy);

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
	structure_Tree->set_Item_Parent_Type(new_Ambient);
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
	delete new_Layer;
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
	delete new_Multilayer;
}

void Structure_Toolbar::add_Aperiodic()
{
	// TODO
	bool loaded = false;
	QStringList lines_List;

	QVector<QString> materials;
	QVector<double> thicknesses;
	QList<QTreeWidgetItem*> new_Child_Layers;

	// create aperiodic item
	QTreeWidgetItem* new_Aperiodic = new QTreeWidgetItem;

	// imd-styled file
	QFileInfo filename = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Find Aperiodic Multulayer", QDir::currentPath(), "Thicknesses " + QString("*.txt") + ";;All files (*.*)"));
	if (!filename.completeBaseName().isEmpty())
	{
		/// reading data
		QFile input_File(filename.absoluteFilePath());
		QString temp_Line = "not empty now";

		if (input_File.open(QIODevice::ReadOnly))
		{
			QTextStream input_Stream(&input_File);
			while ( !input_Stream.atEnd() )
			{
				temp_Line=input_Stream.readLine();
				lines_List.append(temp_Line);
			}
			input_File.close();
			loaded = true;
		} else
		{
			QMessageBox::information(this, "Error", "Can't open file filename \"" + filename.fileName() + "\"");
			return;
		}
	}
	if(loaded)
	{
		/// parsing data
		for(int line_Index=0; line_Index<lines_List.size(); ++line_Index)
		{
			QString temp_Line = lines_List[line_Index];
			QStringList words = temp_Line.split(delimiters,QString::SkipEmptyParts);
			if(temp_Line[0]!=';' && temp_Line[0]!='#' && words.size()>0)
			{
				bool size_Format = false;
				bool int_Format = false;
				bool double_Format = false;

				if(words.size()>=3)
				{
					size_Format = true;
					QString(words[0]).toInt(&int_Format);
					QString(words[2]).toDouble(&double_Format);
				}
				if(!int_Format || !double_Format || !size_Format)
				{
					QMessageBox::information(nullptr, "Bad format", "Row " + QString::number(line_Index) + " has wrong format.\n\nData should be IMD-styled:\n <period index>  <material>  <thickness>");
					return;
				}

				materials.append(words[1]);
				thicknesses.append(QString(words[2]).toDouble());
			}
		}
		for(int layer_Index=0; layer_Index<materials.size(); ++layer_Index)
		{
			Data layer(item_Type_Layer);
			layer.material = materials[layer_Index];
			layer.thickness.value = thicknesses[layer_Index];

			// TODO temporary
			if(layer.material == "Mo")
			{
//				layer.thickness.value = layer.thickness.value*0.92;
				layer.sigma.value = 5;
				layer.interlayer_Composition[Erf].interlayer.value  = 1;
				layer.interlayer_Composition[Lin].interlayer.value  = 0;
				layer.interlayer_Composition[Exp].interlayer.value  = 0;
				layer.interlayer_Composition[Tanh].interlayer.value = 0;
				layer.interlayer_Composition[Sin].interlayer.value  = 0;
				layer.interlayer_Composition[Step].interlayer.value = 0;
			}
			if(layer.material == "Si")
			{
//				layer.thickness.value = layer.thickness.value*0.875;
				layer.sigma.value = 6;
				layer.interlayer_Composition[Erf].interlayer.value  = 1;
				layer.interlayer_Composition[Lin].interlayer.value  = 0;
				layer.interlayer_Composition[Exp].interlayer.value  = 0;
				layer.interlayer_Composition[Tanh].interlayer.value = 0;
				layer.interlayer_Composition[Sin].interlayer.value  = 0;
				layer.interlayer_Composition[Step].interlayer.value = 0;
			}
//			if(layer.material == "Zr")
//			{
////				layer.relative_Density.value=0.97;
////				layer.thickness.value = layer.thickness.value*1.15;
//				layer.sigma.value = 6;
//				layer.interlayer_Composition[Erf].interlayer.value  = 1;
//				layer.interlayer_Composition[Lin].interlayer.value  = 1;
//				layer.interlayer_Composition[Exp].interlayer.value  = 1;
//				layer.interlayer_Composition[Tanh].interlayer.value = 1;
//				layer.interlayer_Composition[Sin].interlayer.value  = 1;
//				layer.interlayer_Composition[Step].interlayer.value = 1;
//			}
			for(Interlayer& inter : layer.interlayer_Composition)
			{
				inter.my_Sigma.value = layer.sigma.value;
//				inter.enabled = true;
			}

			if(!layer.composed_Material)
			{
				if(optical_Constants->material_Map.contains(layer.material + nk_Ext))
				{
					layer.approved_Material = layer.material;
				} else
				{
					QMessageBox::information(this, "Wrong material", "Material \"" + layer.material + "\" not found");
					layer.material = layer.approved_Material;
				}
			}

			// save data
			QTreeWidgetItem* new_Layer = new QTreeWidgetItem;
			QVariant var;
			var.setValue( layer );
			new_Layer->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			new_Child_Layers << new_Layer;
		}

		// insert child layers
		new_Aperiodic->addChildren(new_Child_Layers);

		Data aperiodic(item_Type_Aperiodic);
		aperiodic.num_Repetition.parameter.value = 1;
		QVariant var;
		var.setValue( aperiodic );
		new_Aperiodic->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		// insert aperiodic item to tree
		buffered_Copy_Type = copy_Type_Cut;
		QTreeWidgetItem* item_Aperiodic = add_Buffered_Layer(new_Aperiodic);
		delete new_Aperiodic;

		// TODO temporary
		/// set dependences

		// top layers
		QVector<QTreeWidgetItem*> top_Layer(2);
		QVector<Data> top_Layer_Data(top_Layer.size());
		qInfo() << "top layers:";
		for(int i=0; i<top_Layer.size(); ++i)
		{
			top_Layer[i] = item_Aperiodic->child(i);
			top_Layer_Data[i] = top_Layer[i]->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			qInfo() << top_Layer_Data[i].material;

			top_Layer_Data[i].thickness.fit.is_Fitable = true;
			top_Layer_Data[i].sigma.fit.is_Fitable = true;
		}

		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(structure_Tree->tree);
		while (*it)
		{
			structure_Item=*it;
			Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

			if(struct_Data.item_Type == item_Type_Layer)
			{
				struct_Data.thickness.fit.is_Fitable = true;
				for(int i=0; i<top_Layer.size(); ++i)
				{
					// sigma
					if(struct_Data.material == top_Layer_Data[i].material && struct_Data.id != top_Layer_Data[i].id)
					{
						// add slave
						struct_Data.sigma.indicator.exist = true;
						top_Layer_Data[i].sigma.coupled.slaves.append(struct_Data.sigma.indicator);

						// add master
						top_Layer_Data[i].sigma.indicator.exist = true;
						struct_Data.sigma.coupled.master = top_Layer_Data[i].sigma.indicator;
					}

					// thickness
					if(struct_Data.material == top_Layer_Data[i].material && struct_Data.id != top_Layer_Data[i].id &&
					   abs(struct_Data.thickness.value - top_Layer_Data[i].thickness.value) < DBL_EPSILON)
					{
						// add slave
						struct_Data.thickness.indicator.exist = true;
						top_Layer_Data[i].thickness.coupled.slaves.append(struct_Data.thickness.indicator);

						// add master
						top_Layer_Data[i].thickness.indicator.exist = true;
						struct_Data.thickness.coupled.master = top_Layer_Data[i].thickness.indicator;
					}
				}
			}

			QVariant var;
			var.setValue( struct_Data );
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			++it;
		}

		for(int i=0; i<top_Layer.size(); ++i)
		{
			QVariant var;
			var.setValue( top_Layer_Data[i] );
			top_Layer[i]->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
	}
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

	toolbar->actions()[Add_Substrate]->setDisabled(true);
	refresh_Toolbar();
}

void Structure_Toolbar::edit()
{
	structure_Tree->if_DoubleClicked();
}

bool Structure_Toolbar::ask_Parent_Multilayer()
{
	QTreeWidgetItem* current = structure_Tree->tree->currentItem();

	// if parent periodic multilayer has 2 childs
	if(current->parent())
	if(current->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type == item_Type_Multilayer)
	if(current->parent()->childCount()==2)
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
			return true;
		}
	}
	return false;
}

void Structure_Toolbar::remove()
{
	QTreeWidgetItem* current = structure_Tree->tree->currentItem();
	Data data = current->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

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
		// cppcheck-suppress doubleFree
		if (reply == QMessageBox::Yes)
		{
			if(!ask_Parent_Multilayer()) delete current;
		}
	} else
	// if multilayer
	if(data.item_Type == item_Type_Multilayer)
	{
		QString multilayer_Text = current->text(DEFAULT_COLUMN).split(", N")[0];
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove " + multilayer_Text + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			if(!ask_Parent_Multilayer()) delete current;
		}
	} else
	// if aperiodic
	if(data.item_Type == item_Type_Aperiodic)
	{
		QString aperiodic_Text = current->text(DEFAULT_COLUMN);
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove " + aperiodic_Text + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			if(!ask_Parent_Multilayer()) delete current;
		}
	} else
	if(data.item_Type == item_Type_Substrate)
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove substrate?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			toolbar->actions()[Add_Substrate]->setDisabled(false);		// add_Substrate button
			// cppcheck-suppress doubleFree
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

	QMessageBox::StandardButton reply = QMessageBox::No;

	// if layer
	if(data.item_Type == item_Type_Layer)
	{
		reply = QMessageBox::question(this,"Removal", "Really cut layer " + QString::number(data.layer_Index) + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

	} else
	// if multilayer
	if(data.item_Type == item_Type_Multilayer)
	{
		QString multilayer_Text = current->text(DEFAULT_COLUMN).split(", N")[0];
		reply = QMessageBox::question(this,"Removal", "Really cut " + multilayer_Text + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	} else
	// if aperiodic
	if(data.item_Type == item_Type_Aperiodic)
	{
		QString aperiodic_Text = current->text(DEFAULT_COLUMN);
		reply = QMessageBox::question(this,"Removal", "Really cut " + aperiodic_Text + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	}

	// cppcheck-suppress doubleFree
	if (reply == QMessageBox::Yes)
	{
		if(!ask_Parent_Multilayer()) delete current;
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
		toolbar->actions()[Add_Substrate]->setDisabled(false);		// add_Substrate
		refresh_Toolbar();
	}
}

void Structure_Toolbar::if_Selected()
{
	if(buffered!=nullptr)
	{
		toolbar->actions()[Paste]->setDisabled(false);			// paste
	} else
	{
		toolbar->actions()[Paste]->setDisabled(true);			// paste
	}

	toolbar->actions()[Ungroup]->setDisabled(true);				// ungroup

	if(structure_Tree->tree->selectedItems().isEmpty())
	{
		toolbar->actions()[Edit]->setDisabled(true);			// edit
		toolbar->actions()[Remove]->setDisabled(true);			// remove
		toolbar->actions()[Cut]->setDisabled(true);				// cut
		toolbar->actions()[Copy]->setDisabled(true);			// copy
		toolbar->actions()[Move_Up]->setDisabled(true);			// move_Up
		toolbar->actions()[Move_Down]->setDisabled(true);		// move_Down
//		toolbar->actions()[Group]->setDisabled(true);			// group
//		toolbar->actions()[Thickness_Plot]->setDisabled(true);	// thickness_Plot
//		toolbar->actions()[Sigma_Plot]->setDisabled(true);		// sigma_Plot
	} else
	{
		bool if_Substrate  = (structure_Tree->tree->currentItem()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type) == item_Type_Substrate;
//		bool if_Layer      = (structure_Tree->tree->currentItem()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type) == item_Type_Layer;
		bool if_Multilayer = (structure_Tree->tree->currentItem()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type) == item_Type_Multilayer;

		toolbar->actions()[Edit]->setDisabled(false);			// edit
		toolbar->actions()[Remove]->setDisabled(false);			// remove

		// if multilayer
		if(if_Multilayer)
		{
			toolbar->actions()[Ungroup]->setDisabled(false);	// ungroup
		}

		int position = structure_Tree->tree->currentIndex().row();
		QTreeWidgetItem* parent = structure_Tree->tree->currentItem()->parent();

		// if nested
		if(parent)
		{
			toolbar->actions()[Cut]->setDisabled(false);			// cut
			toolbar->actions()[Copy]->setDisabled(false);			// copy

			if(position == 0)
			{
				toolbar->actions()[Move_Up]->setDisabled(true);		// move_Up
				toolbar->actions()[Move_Down]->setDisabled(false);	// move_Down
			} else
			if(position == (parent->childCount()-1))
			{
				toolbar->actions()[Move_Up]->setDisabled(false);	// move_Up
				toolbar->actions()[Move_Down]->setDisabled(true);	// move_Down
			} else
			{
				toolbar->actions()[Move_Up]->setDisabled(false);	// move_Up
				toolbar->actions()[Move_Down]->setDisabled(false);	// move_Down
			}
		} else
		{
			// if ambient
			if(position == 0)
			{
				toolbar->actions()[Cut]->setDisabled(true);			// cut
				toolbar->actions()[Copy]->setDisabled(true);		// copy
				toolbar->actions()[Move_Up]->setDisabled(true);		// move_Up
				toolbar->actions()[Move_Down]->setDisabled(true);	// move_Down
//				toolbar->actions()[Group]->setDisabled(true);		// group
			} else
			// if not at end
			if(position<structure_Tree->tree->topLevelItemCount()-1)
			{
				toolbar->actions()[Cut]->setDisabled(false);		// cut
				toolbar->actions()[Copy]->setDisabled(false);		// copy

				// if next is not substrate
				if(structure_Tree->tree->topLevelItem(position+1)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type!=item_Type_Substrate)
				{
					toolbar->actions()[Move_Down]->setDisabled(false);	// move_Down
				}
				// if second
				if(position == 1)
				{
					toolbar->actions()[Move_Up]->setDisabled(true);	// move_Up
				} else
				{
					toolbar->actions()[Move_Up]->setDisabled(false);// move_Up
				}
			} else
			{ // if at end
				if(if_Substrate)
				{
					toolbar->actions()[Cut]->setDisabled(true);		// cut
					toolbar->actions()[Copy]->setDisabled(true);	// copy
					toolbar->actions()[Move_Up]->setDisabled(true);	// move_Up
				} else
				{
					// if second
					if(position == 1)
					{
						toolbar->actions()[Move_Up]->setDisabled(true);	// move_Up
					} else
					{
						toolbar->actions()[Move_Up]->setDisabled(false);	// move_Up
					}
					toolbar->actions()[Cut]->setDisabled(false);	// cut
					toolbar->actions()[Copy]->setDisabled(false);	// copy
				}
				toolbar->actions()[Move_Down]->setDisabled(true);	// move_Down
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

QTreeWidgetItem* Structure_Toolbar::add_Buffered_Layer(QTreeWidgetItem* new_Layer_Passed)
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
	return new_Layer;
}
