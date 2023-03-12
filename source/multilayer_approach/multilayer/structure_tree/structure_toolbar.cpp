#include "structure_toolbar.h"
#include "aperiodic_load_setup.h"

Structure_Toolbar::Structure_Toolbar(Structure_Tree* structure_Tree, QWidget *parent) :
	QWidget(parent),
	structure_Tree(structure_Tree)
{
	create_Toolbar();
}

Structure_Toolbar::~Structure_Toolbar()
{
}

void Structure_Toolbar::create_Toolbar()
{
	QPixmap add_Layer		(Paths_Icon_Path + "add_layer.bmp");
	QPixmap add_Multilayer  (Paths_Icon_Path + "add_multilayer.bmp");
	QPixmap add_Aperiodic	(Paths_Icon_Path + "taj.bmp");
//	QPixmap add_Substrate	(Paths_Icon_Path + "add_substrate.bmp");
	QPixmap edit			(Paths_Icon_Path + "roi.bmp");
	QPixmap remove			(Paths_Icon_Path + "delete.bmp");
	QPixmap cut				(Paths_Icon_Path + "cut.bmp");
	QPixmap copy			(Paths_Icon_Path + "copy.bmp");
	QPixmap paste			(Paths_Icon_Path + "paste.bmp");
	QPixmap move_Up			(Paths_Icon_Path + "shift_up.bmp");
	QPixmap move_Down		(Paths_Icon_Path + "shift_down.bmp");
//	QPixmap group			(Paths_Icon_Path + "group.bmp");
	QPixmap ungroup			(Paths_Icon_Path + "ungroup.bmp");
//	QPixmap thickness_Plot	(Paths_Icon_Path + "zplot.bmp");
//	QPixmap sigma_Plot		(Paths_Icon_Path + "sigmaplot.bmp");
	QPixmap destroy			(Paths_Icon_Path + "bomb.bmp");
//	QPixmap export_Structure(Paths_Icon_Path + "save.bmp");

	toolbar = new QToolBar(this);
	toolbar->addAction(QIcon(add_Layer),		"Add layer");						// 0
	toolbar->addAction(QIcon(add_Multilayer),	"Add multilayer");					// 1
	toolbar->addAction(QIcon(add_Aperiodic),	"Add aperiodic multilayer");		// 2
//	toolbar->addAction(QIcon(add_Substrate),	"Add substrate");					// 3
	toolbar->addAction(QIcon(edit),				"Edit");							// 4
	toolbar->addAction(QIcon(remove),			"Remove\nDel");						// 5
	toolbar->addAction(QIcon(cut),				"Cut\nCtrl+X");						// 6
	toolbar->addAction(QIcon(copy),				"Copy\nCtrl+C");					// 7
	toolbar->addAction(QIcon(paste),			"Paste\nCtrl+V");					// 8
	toolbar->addAction(QIcon(move_Up),			"Move up");							// 9
	toolbar->addAction(QIcon(move_Down),		"Move down");						// 0
//	toolbar->addAction(QIcon(group),			"Group");							// 11
	toolbar->addAction(QIcon(ungroup),			"Ungroup");							// 12
//	toolbar->addAction(QIcon(thickness_Plot),	"Plot layer thickness profile");	// 13
//	toolbar->addAction(QIcon(sigma_Plot),		"Plot interface width profile");	// 14
	toolbar->addAction(QIcon(destroy),			"Remove all layers");				// 15
//	toolbar->addAction(QIcon(export_Structure),	"Save structure as text");			// 16

	toolbar->setIconSize(add_Layer.size());

	connect(toolbar->actions()[Add_Layer],		 &QAction::triggered, this, &Structure_Toolbar::add_Layer);
	connect(toolbar->actions()[Add_Multilayer],  &QAction::triggered, this, &Structure_Toolbar::add_Multilayer);
	connect(toolbar->actions()[Add_Aperiodic],   &QAction::triggered, this, &Structure_Toolbar::add_Aperiodic);
//	connect(toolbar->actions()[Add_Substrate],   &QAction::triggered, this, &Structure_Toolbar::add_Substrate);
	connect(toolbar->actions()[Edit],			 &QAction::triggered, this, &Structure_Toolbar::edit);
	connect(toolbar->actions()[Remove],			 &QAction::triggered, this, &Structure_Toolbar::remove);
	connect(toolbar->actions()[Cut],			 &QAction::triggered, this, &Structure_Toolbar::cut);
	connect(toolbar->actions()[Copy],			 &QAction::triggered, this, &Structure_Toolbar::copy);
	connect(toolbar->actions()[Paste],			 &QAction::triggered, this, &Structure_Toolbar::paste);
	connect(toolbar->actions()[Move_Up],		 &QAction::triggered, this, &Structure_Toolbar::move_Up);
	connect(toolbar->actions()[Move_Down],		 &QAction::triggered, this, &Structure_Toolbar::move_Down);
//	connect(toolbar->actions()[Group],			 &QAction::triggered, this, &Structure_Toolbar::group);
	connect(toolbar->actions()[Ungroup],		 &QAction::triggered, this, &Structure_Toolbar::ungroup);
//	connect(toolbar->actions()[Thickness_Plot],  &QAction::triggered, this, &Structure_Toolbar::thickness_Plot);
//	connect(toolbar->actions()[Sigma_Plot],		 &QAction::triggered, this, &Structure_Toolbar::sigma_Plot);
	connect(toolbar->actions()[Destroy],		 &QAction::triggered, this, &Structure_Toolbar::destroy);
//	connect(toolbar->actions()[Export_Structure],&QAction::triggered, this, &Structure_Toolbar::export_Structure);

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

	for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; interlayer_Index++)
	{
		if(!structure_Tree->multilayer->imperfections_Model.use_Func[interlayer_Index] || !structure_Tree->multilayer->imperfections_Model.use_Interlayer)
		{
			layer.interlayer_Composition[interlayer_Index].enabled = false;
		}
	}

	var.setValue( layer );
	new_Layer->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	buffered_Copy_Type = copy_Type_Cut;
	add_Buffered_Layer(new_Layer);
	Global_Variables::new_Layer_Particles_Model     (layer, structure_Tree->multilayer->imperfections_Model);
	structure_Tree->refresh_Tree_Roughness();
	structure_Tree->refresh_Tree_Particles();
	delete new_Layer;
}

void Structure_Toolbar::add_Multilayer()
{
	int num_Children = 2;

//	bool ok, ok_Number;
//	QString text = QInputDialog::getText(this, "Multilayer", "Number of different layers in multilayers stack", QLineEdit::Normal, Locale.toString(2), &ok);
//	text.toInt(&ok_Number);
//	if (ok && ok_Number && (text.toInt()>=2))
//	{
//		num_Children = text.toInt();
//	} else
//	if(!ok)
//	{
//		return;
//	}
//	if(!ok_Number)
//	{
//		QMessageBox::warning(this, "Warning", text + " is not a integer number");
//		add_Multilayer();
//	} else
//	if (text.toInt()<2)
//	{
//		QMessageBox::warning(this, "Warning", "There should be at least 2 layers");
//		add_Multilayer();
//	}
//	if(num_Children<2) return;

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

		for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; interlayer_Index++)
		{
			if(!structure_Tree->multilayer->imperfections_Model.use_Func[interlayer_Index] || !structure_Tree->multilayer->imperfections_Model.use_Interlayer)
			{
				layer.interlayer_Composition[interlayer_Index].enabled = false;
			}
		}
        if(i==1)
        {
            layer.material = default_layer_material_second;
            layer.approved_Material = layer.material;
        }
//		Global_Variables::enable_Disable_Roughness_Model(layer, structure_Tree->multilayer->imperfections_Model, is_Last_Layer);
		Global_Variables::new_Layer_Particles_Model     (layer, structure_Tree->multilayer->imperfections_Model);

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
	structure_Tree->refresh_Tree_Roughness();
	structure_Tree->refresh_Tree_Particles();
	delete new_Multilayer;
}

void Structure_Toolbar::add_Aperiodic()
{
	// presettings window
	Aperiodic_Settings aperiodic_Settings;
		aperiodic_Settings.length_Units = aperiodic_default_units_import;
	Aperiodic_Load_Setup* aperiodic_Load_Setup = new Aperiodic_Load_Setup(aperiodic_Settings, item_Type_General_Aperiodic, this);
		aperiodic_Load_Setup->exec();
	if(!aperiodic_Settings.contin)
		return;

	{
		bool loaded = false;
		QStringList lines_List;

		QVector<QString> materials;
		QVector<double> thicknesses;
		QVector<double> sigmas;
		QVector<double> densities;
		QList<QTreeWidgetItem*> new_Child_Layers;

		// create aperiodic item
		QTreeWidgetItem* new_Aperiodic = new QTreeWidgetItem;

		// imd-styled file
		QFileInfo filename = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Find Aperiodic Multulayer", last_aperiodic_data_directory, "Thicknesses " + QString("*.txt") + ";;All files (*.*)"));
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
					bool column_3_Double_Format = false;
					bool column_4_Double_Format = true;
					bool column_5_Double_Format = true;

					int good_Size = 3;
					if(aperiodic_Settings.column_4 != "") {good_Size = 4; column_4_Double_Format = false;}
					if(aperiodic_Settings.column_5 != "") {good_Size = 5; column_5_Double_Format = false;}

					if(words.size()>=good_Size)
					{
						size_Format = true;
						QString(words[0]).toInt(&int_Format);
						QString(words[2]).replace(",", ".").toDouble(&column_3_Double_Format);

						if(words.size()>=4) QString(words[3]).replace(",", ".").toDouble(&column_4_Double_Format);
						if(words.size()>=5) QString(words[4]).replace(",", ".").toDouble(&column_5_Double_Format);
					}
					if(!int_Format || !column_3_Double_Format || !column_4_Double_Format || !column_5_Double_Format || !size_Format)
					{
						QString addition_1 = "";
						QString addition_2 = "";

						if(aperiodic_Settings.column_4 == whats_This_Sigma_Diffuse)		addition_1 = "  <sigma>";
						if(aperiodic_Settings.column_4 == whats_This_Density)	addition_1 = "  <density>";
						if(aperiodic_Settings.column_5 == whats_This_Sigma_Diffuse)		addition_2 = "  <sigma>";
						if(aperiodic_Settings.column_5 == whats_This_Density)	addition_2 = "  <density>";

						QMessageBox::information(nullptr, "Bad format", "Row " + Locale.toString(line_Index) + " has wrong format.\n\nData should be styled:\n <period index>  <material>  <thickness>" + addition_1 + addition_2);
						return;
					}

					materials.append(words[1]);
					thicknesses.append(QString(words[2]).replace(",", ".").toDouble());
					if(aperiodic_Settings.column_4 == whats_This_Sigma_Diffuse)		sigmas.   append(QString(words[3]).replace(",", ".").toDouble());
					if(aperiodic_Settings.column_4 == whats_This_Density)	densities.append(QString(words[3]).replace(",", ".").toDouble());
					if(aperiodic_Settings.column_5 == whats_This_Sigma_Diffuse)		sigmas.   append(QString(words[4]).replace(",", ".").toDouble());
					if(aperiodic_Settings.column_5 == whats_This_Density)	densities.append(QString(words[4]).replace(",", ".").toDouble());
				}
			}
			for(int layer_Index=0; layer_Index<materials.size(); ++layer_Index)
			{
				double length_Coeff = length_Coefficients_Map.value(aperiodic_Settings.length_Units);
				Data layer(item_Type_Layer);

				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; interlayer_Index++)
				{
					if(!structure_Tree->multilayer->imperfections_Model.use_Func[interlayer_Index] || !structure_Tree->multilayer->imperfections_Model.use_Interlayer)
					{
						layer.interlayer_Composition[interlayer_Index].enabled = false;
					}
				}
//				Global_Variables::enable_Disable_Roughness_Model(layer, structure_Tree->multilayer->imperfections_Model);
				Global_Variables::new_Layer_Particles_Model     (layer, structure_Tree->multilayer->imperfections_Model);

				layer.common_Sigma_Diffuse = true;
				layer.material = materials[layer_Index];
				layer.thickness.value = thicknesses[layer_Index]*length_Coeff;

				if( aperiodic_Settings.column_4 == whats_This_Sigma_Diffuse ||
					aperiodic_Settings.column_5 == whats_This_Sigma_Diffuse	)
				{
					layer.sigma_Diffuse.value = sigmas[layer_Index]*length_Coeff;
					for(Interlayer& interlayer : layer.interlayer_Composition)
					{
						interlayer.my_Sigma_Diffuse.value = layer.sigma_Diffuse.value;
					}
				}
				if( aperiodic_Settings.column_4 == whats_This_Density ||
					aperiodic_Settings.column_5 == whats_This_Density )
				{
					layer.relative_Density.value = densities[layer_Index];
				}

				layer.composed_Material = false;

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

			Data aperiodic(item_Type_General_Aperiodic );
			aperiodic.num_Repetition.parameter.value = 1;
			QVariant var;
			var.setValue( aperiodic );
			new_Aperiodic->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			// insert aperiodic item to tree
			buffered_Copy_Type = copy_Type_Cut;
			add_Buffered_Layer(new_Aperiodic);
			delete new_Aperiodic;
			last_aperiodic_data_directory = filename.absolutePath();
			structure_Tree->refresh_Tree_Roughness();
			structure_Tree->refresh_Tree_Particles();
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

	for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; interlayer_Index++)
	{
		if(!structure_Tree->multilayer->imperfections_Model.use_Func[interlayer_Index] || !structure_Tree->multilayer->imperfections_Model.use_Interlayer)
		{
			data.interlayer_Composition[interlayer_Index].enabled = false;
		}
	}
//	Global_Variables::enable_Disable_Roughness_Model(data, structure_Tree->multilayer->imperfections_Model);

	var.setValue( data );
	new_Substrate->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	structure_Tree->tree->addTopLevelItem(new_Substrate);

//	toolbar->actions()[Add_Substrate]->setDisabled(true);
	structure_Tree->set_Structure_Item_Text(new_Substrate);
	structure_Tree->set_Item_Parent_Type(new_Substrate);
	structure_Tree->refresh_Tree_Roughness();
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
//	if(current->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type == item_Type_Multilayer) // for regular and general aperiodic too
	if(current->parent()->childCount()==2)
	{
		QString multilayer_Text = current->parent()->text(DEFAULT_COLUMN).split(", N")[0];
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", multilayer_Text + " will be disbanded.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if(reply == QMessageBox::Yes)
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
		} else
		{
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
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove layer " + Locale.toString(data.layer_Index) + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
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
	if( data.item_Type == item_Type_Regular_Aperiodic ||
		data.item_Type == item_Type_General_Aperiodic )
	{
		QString aperiodic_Text = current->text(DEFAULT_COLUMN);
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove " + aperiodic_Text + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			if(!ask_Parent_Multilayer()) delete current;
		}
	} /*else
	if(data.item_Type == item_Type_Substrate)
	{
		QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Really remove substrate?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			toolbar->actions()[Add_Substrate]->setDisabled(false);		// add_Substrate button
			// cppcheck-suppress doubleFree
			delete current;
		}
	}*/

	structure_Tree->refresh_Tree_Roughness();
	structure_Tree->refresh_Tree_Particles();
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
		reply = QMessageBox::question(this,"Removal", "Really cut layer " + Locale.toString(data.layer_Index) + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

	} else
	// if multilayer
	if(data.item_Type == item_Type_Multilayer)
	{
		QString multilayer_Text = current->text(DEFAULT_COLUMN).split(", N")[0];
		reply = QMessageBox::question(this,"Removal", "Really cut " + multilayer_Text + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	} else
	// if aperiodic
	if( data.item_Type == item_Type_Regular_Aperiodic ||
		data.item_Type == item_Type_General_Aperiodic )
	{
		QString aperiodic_Text = current->text(DEFAULT_COLUMN);
		reply = QMessageBox::question(this,"Removal", "Really cut " + aperiodic_Text + "?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	}

	// cppcheck-suppress doubleFree
	if (reply == QMessageBox::Yes)
	{
		if(!ask_Parent_Multilayer()) delete current;
	}

	structure_Tree->refresh_Tree_Roughness();
	structure_Tree->refresh_Tree_Particles();
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
	if(buffered)
	{
		add_Buffered_Layer(buffered);
		buffered_Copy_Type = copy_Type_Copy;	 // next copies should have changed IDs
		structure_Tree->refresh_Tree_Roughness();
		structure_Tree->refresh_Tree_Particles();
	}
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

		Data parent_Data = parent->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(parent_Data.item_Type == item_Type_Regular_Aperiodic)
		{
			parent_Data.regular_Components.move(position-1,position);
			QVariant var;
			var.setValue( parent_Data );
			parent->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
	} else
	{
		QTreeWidgetItem* taken = structure_Tree->tree->takeTopLevelItem(position-1);
		structure_Tree->tree->insertTopLevelItem(position, taken);
	}

	structure_Tree->refresh_Tree_Roughness();
	structure_Tree->refresh_Tree_Particles();
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

		Data parent_Data = parent->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(parent_Data.item_Type == item_Type_Regular_Aperiodic)
		{
			parent_Data.regular_Components.move(position+1,position);
			QVariant var;
			var.setValue( parent_Data );
			parent->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
	} else
	{
		QTreeWidgetItem* taken = structure_Tree->tree->takeTopLevelItem(position+1);
		structure_Tree->tree->insertTopLevelItem(position, taken);
	}

	structure_Tree->refresh_Tree_Roughness();
	structure_Tree->refresh_Tree_Particles();
	refresh_Toolbar();
}

void Structure_Toolbar::group()
{
	// TODO group_toolbutton
	qInfo() << "group is not implemented" << endl;
	structure_Tree->refresh_Tree_Roughness();
	structure_Tree->refresh_Tree_Particles();
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

	structure_Tree->refresh_Tree_Roughness();
	structure_Tree->refresh_Tree_Particles();
	refresh_Toolbar();
}

void Structure_Toolbar::destroy()
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Remove all layers?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if (reply == QMessageBox::Yes)
	{
        QMessageBox::information(this,"Removal", "Caution: the project remains the same!");

		structure_Tree->tree->clear();
		add_Ambient();
		add_Substrate();
//		toolbar->actions()[Add_Substrate]->setDisabled(false);		// add_Substrate
		refresh_Toolbar();
		structure_Tree->refresh_Tree_Roughness();
		structure_Tree->refresh_Tree_Particles();
	}
}

void Structure_Toolbar::export_Structure()
{
    // point as decimal separator
    QLocale tempLoc = QLocale::c();

	QString tab_Text = "";
	for(int i=0; i<global_Multilayer_Approach->multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(i));
		if(multilayer == structure_Tree->multilayer)
		{
			tab_Text = global_Multilayer_Approach->multilayer_Tabs->tabText(i);
		}
	}
    QString path = Global_Variables::working_Directory() + "/";
    QString name = "structure_"+/*tempLoc.toString(index)+"_"+*/tab_Text+".txt";
    QFile file(path + name);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		out.setFieldAlignment(QTextStream::AlignLeft);

		look_Over_Tree();
		print_Structure_Header(out);
		iterate_Over_Tree(out, structure_Tree->tree->invisibleRootItem());
		qInfo() << "structure saved as text :"<< name << endl;

		file.close();
	} else
	{
		QMessageBox::critical(nullptr, "Structure_Toolbar::export_Structure", "Can't write file " + name);
		exit(EXIT_FAILURE);
	}

	// export all regular aperiodics
	int aperiodics_Counter = 1;
	int index_Width = 8;
	int my_material_Width = max(material_Width,10);
	int my_thickness_Width = max(thickness_Width+thickness_Precision+1,8);
	int my_sigma_Width = max(sigma_Width+sigma_Precision+1,7);

	QTreeWidgetItemIterator it(structure_Tree->tree);
	while (*it)
    {
		QTreeWidgetItem* structure_Item = *it;
		const Data struct_Data = structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>();
		if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
		{
            QString name = "structure_"+/*tempLoc.toString(index)+"_"+*/tab_Text+"_Aperiodic_#"+QString::number(aperiodics_Counter)+".txt";
            QFile file(Global_Variables::working_Directory() + "/" + name);
			if (file.open(QIODevice::WriteOnly))
			{
				QTextStream out(&file);
				out.setFieldAlignment(QTextStream::AlignLeft);

				// header
                out << "; " << Global_Variables::date_Time() << qSetFieldWidth(0) << endl;
                out << "; " << Global_Variables::multifitting_Version() << qSetFieldWidth(0) << endl;
				out << qSetFieldWidth(index_Width) << ";cell" << qSetFieldWidth(0) << " " << qSetFieldWidth(my_material_Width) << "material" << qSetFieldWidth(0);
				out.setFieldAlignment(QTextStream::AlignRight);
				out << qSetFieldWidth(my_thickness_Width-3) << "d ("      << qSetFieldWidth(0) << thickness_units << ")" << "    ";
				out << qSetFieldWidth(my_sigma_Width+3)     << "sigma ("  << qSetFieldWidth(0) << thickness_units << ")";
				out.setFieldAlignment(QTextStream::AlignLeft);
				out	<< qSetFieldWidth(0) << endl;

				// data
				for(int n=0; n<struct_Data.num_Repetition.value(); n++)
				{
					for(int k=0; k<struct_Data.regular_Components.size(); k++)
					{
						const Data& regular_Data = struct_Data.regular_Components[k].components[n];

						out << qSetFieldWidth(index_Width) << QString::number(n+1) << qSetFieldWidth(0) << " " << qSetFieldWidth(my_material_Width) << regular_Data.material << qSetFieldWidth(0);
						out.setFieldAlignment(QTextStream::AlignRight);
                        out << qSetFieldWidth(my_thickness_Width) << tempLoc.toString(regular_Data.thickness.value,'f',thickness_Precision) << qSetFieldWidth(0) << "    ";
                        out << qSetFieldWidth(my_sigma_Width)     << tempLoc.toString(regular_Data.sigma_Diffuse.value, 'f',sigma_Precision) << qSetFieldWidth(0);
						out.setFieldAlignment(QTextStream::AlignLeft);
						out	<< qSetFieldWidth(0) << endl;
					}
				}

				file.close();
			} else
			{
				QMessageBox::critical(nullptr, "Structure_Toolbar::export_Structure", "Can't write file " + name);
				exit(EXIT_FAILURE);
			}

			aperiodics_Counter++;
		}
		++it;
	}
}

void Structure_Toolbar::iterate_Over_Tree(QTextStream& out, QTreeWidgetItem* parent_Item, QString parent_Index_String)
{
	int addition = 1;
	if(parent_Item==structure_Tree->tree->invisibleRootItem()) addition=0;
	int index = 0;
	for(int i=0; i<parent_Item->childCount(); i++)
	{
		QString current_Index_String;
		if(parent_Index_String=="") current_Index_String = QString::number(i+addition);
		else						current_Index_String = parent_Index_String+"."+QString::number(i+addition);

		QTreeWidgetItem* structure_Item = parent_Item->child(i);
		int item_Depth = Global_Variables::get_Item_Depth(structure_Item);
		const Data struct_Data = structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>();
		const Data substrate = structure_Tree->tree->invisibleRootItem()->child(structure_Tree->tree->invisibleRootItem()->childCount()-1)->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>();

		if(struct_Data.item_Type!=item_Type_Ambient && struct_Data.item_Enabled)
		{

			if(i>=1) if(parent_Item->child(i-1)->childCount()>0) index=0;
			print_Structure_Item(out, structure_Item, current_Index_String, item_Depth, index, substrate);
			index++;
		}

		// go deeper, if you need
		if( (struct_Data.item_Type==item_Type_Multilayer ||
			struct_Data.item_Type==item_Type_General_Aperiodic) && struct_Data.item_Enabled)
		{
			iterate_Over_Tree(out, structure_Item, current_Index_String);
		}
		if( struct_Data.item_Type==item_Type_Regular_Aperiodic && struct_Data.item_Enabled)
		{
			index = 0;
			QString temp_Parent_Index_String = current_Index_String;
			for(int n=0; n<struct_Data.num_Repetition.value(); n++)
			{
				for(int k=0; k<struct_Data.regular_Components.size(); k++)
				{
					const Data& regular_Data = struct_Data.regular_Components[k].components[n];

					QTreeWidgetItem* temp_Item = new QTreeWidgetItem;
					QVariant var; var.setValue( regular_Data );
					temp_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

					current_Index_String = temp_Parent_Index_String+"."+QString::number(index+1);
					print_Structure_Item(out, temp_Item, current_Index_String, item_Depth+1, index, substrate);
					index++;

					delete temp_Item;
				}
			}
		}
	}
}

void Structure_Toolbar::print_Structure_Header(QTextStream &out)
{
    out << Global_Variables::date_Time() << qSetFieldWidth(0) << endl;
    out << Global_Variables::multifitting_Version() << qSetFieldWidth(0) << endl;

	// legend
	out << "< legend: >" << qSetFieldWidth(0) << endl;
    out << "\tN    - number of elementary cells" << qSetFieldWidth(0) << endl;
    out << "\td    - period or thickness" << qSetFieldWidth(0) << endl;
    out << "\ts_d  - root-mean-square interlayer thickness (sigma diffuse)" << qSetFieldWidth(0) << endl;
    out << "\ts_r  - root-mean-square roughness (sigma roughness)" << qSetFieldWidth(0) << endl;
    out << "\tr    - material density (rho), absolute or relative" << qSetFieldWidth(0) << endl;
    out << "\tA    - angstroms" << qSetFieldWidth(0) << endl;
    out << "\tr.u. - relative units" << qSetFieldWidth(0) << endl;

	out << "-------------------------------------------------------------------------------" << qSetFieldWidth(0) << endl << endl;
}

void Structure_Toolbar::look_Over_Tree()
{
	material_Width = 0;
	thickness_Width = 0;
	sigma_Width = 0;
	density_Width = 0;

	QTreeWidgetItemIterator it(structure_Tree->tree);
	while (*it)
	{
		QTreeWidgetItem* structure_Item = *it;
		const Data struct_Data = structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>();
		if(struct_Data.item_Type == item_Type_Layer && struct_Data.item_Enabled)
		{
			// material
			material_Width = max(material_Width, struct_Data.material.size());

			// density
			QString density_String;
			if(struct_Data.composed_Material)	density_String = QString::number(int(struct_Data.absolute_Density.value));
			else								density_String = QString::number(int(struct_Data.relative_Density.value));
			density_Width = max(density_Width, density_String.size());

			// thickness_Width
			QString thickness_String = QString::number(int(struct_Data.thickness.value));
			thickness_Width = max(thickness_Width, thickness_String.size());

			// sigma_Width
			QString sigma_String = QString::number(int(struct_Data.sigma_Diffuse.value));
			sigma_Width = max(sigma_Width, sigma_String.size());
		}

		if(struct_Data.item_Type == item_Type_Regular_Aperiodic && struct_Data.item_Enabled)
		{
			for(int n=0; n<struct_Data.num_Repetition.value(); n++)
			{
				for(int k=0; k<struct_Data.regular_Components.size(); k++)
				{
					const Data& regular_Data = struct_Data.regular_Components[k].components[n];

					// thickness_Width
					QString thickness_String = QString::number(int(regular_Data.thickness.value));
					thickness_Width = max(thickness_Width, thickness_String.size());

					// sigma_Width
					QString sigma_String = QString::number(int(regular_Data.sigma_Diffuse.value));
					sigma_Width = max(sigma_Width, sigma_String.size());
				}
			}
		}
		++it;
	}
}

void Structure_Toolbar::print_Structure_Item(QTextStream& out, QTreeWidgetItem* structure_Item, QString current_Index_String, int item_Depth, int item_Index, const Data& substrate)
{
	const Data struct_Data = structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>();

	int index_Width = 7;
	if(struct_Data.parent_Item_Type == NOPARENT) index_Width = 0;

	int my_material_Width = material_Width;
	int my_thickness_Width = thickness_Width+thickness_Precision+1;
	int my_sigma_Width = sigma_Width+sigma_Precision+1;
	int my_density_Width = density_Width+density_Precision+1;

	QString temp_thickness_units = thickness_units;

	bool if_Full = (item_Index/*%10*/==0);// || (struct_Data.parent_Item_Type == NOPARENT);
	// indentation
	for(int y=0; y<item_Depth-1; y++) {	out << "\t";}

	if(struct_Data.item_Type == item_Type_Layer && struct_Data.item_Enabled)
	{
		out << qSetFieldWidth(index_Width) << current_Index_String << qSetFieldWidth(0) << " " << qSetFieldWidth(my_material_Width) << struct_Data.material << qSetFieldWidth(0);

		if(if_Full) {out << "   : d=";	temp_thickness_units = thickness_units;}
		else	    {out << "       "; 	temp_thickness_units = empty_Thickness_units;}
		out.setFieldAlignment(QTextStream::AlignRight);
		out << qSetFieldWidth(my_thickness_Width) << Locale.toString(struct_Data.thickness.value,'f',thickness_Precision)  << qSetFieldWidth(0) << " " << temp_thickness_units;

		out.setFieldAlignment(QTextStream::AlignLeft);
		if(if_Full) {out << "    s_d=";}
		else	    {out << "        ";}
		out.setFieldAlignment(QTextStream::AlignRight);
		out << qSetFieldWidth(my_sigma_Width) << Locale.toString(struct_Data.sigma_Diffuse.value,'f',sigma_Precision)  << qSetFieldWidth(0) << " " << temp_thickness_units;

		out.setFieldAlignment(QTextStream::AlignLeft);
		if(if_Full) {out << "    s_r=";}
		else	    {out << "        ";}
		out.setFieldAlignment(QTextStream::AlignRight);
		if(!structure_Tree->multilayer->imperfections_Model.use_Common_Roughness_Function)	{
		out << qSetFieldWidth(my_sigma_Width) << Locale.toString(struct_Data.roughness_Model.sigma.value,'f',sigma_Precision)  << qSetFieldWidth(0) << " " << temp_thickness_units;
		} else {
		out << qSetFieldWidth(my_sigma_Width) << Locale.toString(substrate.roughness_Model.sigma.value,'f',sigma_Precision)  << qSetFieldWidth(0) << " " << temp_thickness_units;
		}
		out.setFieldAlignment(QTextStream::AlignLeft);
		if(if_Full) {out << "    r=";}
		else	    {out << "      ";}
		out.setFieldAlignment(QTextStream::AlignRight);
		if(struct_Data.composed_Material) {
		out	<< qSetFieldWidth(my_density_Width) << Locale.toString(struct_Data.absolute_Density.value,'f',density_Precision) << qSetFieldWidth(0) << " " << absolute_Density_Units;
		} else {
		out	<< qSetFieldWidth(my_density_Width) << Locale.toString(struct_Data.relative_Density.value,'f',density_Precision) << qSetFieldWidth(0) << " " << relative_Density_Units;
		}
		out.setFieldAlignment(QTextStream::AlignLeft);
		out	<< qSetFieldWidth(0) << endl;
	}
	if(struct_Data.item_Type == item_Type_Substrate && struct_Data.item_Enabled)
	{
		out << qSetFieldWidth(0) << current_Index_String << " Substrate " << qSetFieldWidth(my_material_Width) << struct_Data.material << qSetFieldWidth(0)
			<< "    s_d="   << Locale.toString(struct_Data.sigma_Diffuse.value,'f',sigma_Precision) << " " << thickness_units
			<< "    s_r="   << Locale.toString(struct_Data.roughness_Model.sigma.value,'f',sigma_Precision) << " " << thickness_units;

		if(struct_Data.composed_Material) {
		out	<< "    r="   << Locale.toString(struct_Data.absolute_Density.value,'f',density_Precision) << " " << absolute_Density_Units;
		} else {
		out	<< "    r="   << Locale.toString(struct_Data.relative_Density.value,'f',density_Precision) << " " << relative_Density_Units;
		}
		out	<< qSetFieldWidth(0) << endl;
	}
	if(struct_Data.item_Type == item_Type_Multilayer && struct_Data.item_Enabled)
	{
		out << qSetFieldWidth(0) << current_Index_String << qSetFieldWidth(0) << "  Periodic   ";
		for(int i=structure_Item->childCount()-1; i>=0; i--)
		{
			const Data child_Data = structure_Item->child(i)->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>();
			if(child_Data.item_Type == item_Type_Layer)				out << child_Data.material;
			if(child_Data.item_Type == item_Type_Multilayer)		out << "<periodic>";
			if(child_Data.item_Type == item_Type_Regular_Aperiodic)	out << "<aperiodic>";
			if(child_Data.item_Type == item_Type_General_Aperiodic)	out << "<aperiodic>";
			if(i!=0) out << "/";
		}
		out << "   N="<< Locale.toString(struct_Data.num_Repetition.value()) << qSetFieldWidth(0) << endl;
	}
	if(struct_Data.item_Type == item_Type_Regular_Aperiodic && struct_Data.item_Enabled)
	{
		out << qSetFieldWidth(0) << current_Index_String << qSetFieldWidth(0) << "  Aperiodic   ";
		for(int i=structure_Item->childCount()-1; i>=0; i--)
		{
			const Data child_Data = structure_Item->child(i)->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>();
			out << child_Data.material;
			if(i!=0) out << "/";
		}
		out << "   N="<< Locale.toString(struct_Data.num_Repetition.value()) << qSetFieldWidth(0) << endl;
	}
	if(struct_Data.item_Type == item_Type_General_Aperiodic && struct_Data.item_Enabled)
	{
		out << qSetFieldWidth(0) << current_Index_String  << qSetFieldWidth(0) << "  Aperiodic  " << endl;
	}
}

void Structure_Toolbar::if_Selected()
{
	if(buffered)
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
		QString item_Type = (structure_Tree->tree->currentItem()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().item_Type);
		bool if_Substrate  = (item_Type == item_Type_Substrate);
//		bool if_Layer      = (item_Type == item_Type_Layer);
		bool if_Multilayer = (item_Type == item_Type_Multilayer);
		bool if_Aperiodic  = (item_Type == item_Type_Regular_Aperiodic) || (item_Type == item_Type_General_Aperiodic);

		toolbar->actions()[Edit]->setDisabled(false);			// edit
		toolbar->actions()[Remove]->setDisabled(false);			// remove

		// if multilayer or aperiodic
		if(if_Multilayer || if_Aperiodic)
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

			// special parents
			Data parent_Data = parent->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			if(parent_Data.item_Type == item_Type_Regular_Aperiodic)
			{
				toolbar->actions()[Add_Layer]->setDisabled(true);
				toolbar->actions()[Add_Multilayer]->setDisabled(true);
				toolbar->actions()[Add_Aperiodic]->setDisabled(true);
				toolbar->actions()[Remove]->setDisabled(true);
				toolbar->actions()[Cut]->setDisabled(true);
				if(buffered)
				{
					QTreeWidgetItem* buffered_Item = buffered->clone();
					Data buffered_Data = buffered_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
					if(buffered_Data.item_Type != item_Type_Layer)
					{
						toolbar->actions()[Paste]->setDisabled(true);
					}
				}
			} else
			{
				toolbar->actions()[Add_Layer]->setDisabled(false);
				toolbar->actions()[Add_Multilayer]->setDisabled(false);
				toolbar->actions()[Add_Aperiodic]->setDisabled(false);
				toolbar->actions()[Remove]->setDisabled(false);
			}
		} else
		{
			// always accessible if no parents
			toolbar->actions()[Add_Layer]->setDisabled(false);
			toolbar->actions()[Add_Multilayer]->setDisabled(false);
			toolbar->actions()[Add_Aperiodic]->setDisabled(false);
			toolbar->actions()[Remove]->setDisabled(false);

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
				} else
				{
					toolbar->actions()[Move_Down]->setDisabled(true);	// move_Down
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
				/// substrate only can be at the end
//				if(if_Substrate)
//				{
					toolbar->actions()[Cut]->setDisabled(true);		// cut
					toolbar->actions()[Copy]->setDisabled(true);	// copy
					toolbar->actions()[Move_Up]->setDisabled(true);	// move_Up
					toolbar->actions()[Remove]->setDisabled(true);	// remove
//				} else
//				{
//					// if second
//					if(position == 1)
//					{
//						toolbar->actions()[Move_Up]->setDisabled(true);	// move_Up
//					} else
//					{
//						toolbar->actions()[Move_Up]->setDisabled(false);	// move_Up
//					}
//					toolbar->actions()[Cut]->setDisabled(false);	// cut
//					toolbar->actions()[Copy]->setDisabled(false);	// copy
//				}
//				toolbar->actions()[Move_Down]->setDisabled(true);	// move_Down
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
				QTreeWidgetItem* parent = structure_Tree->tree->currentItem()->parent();
				parent->insertChild(position+1,new_Layer);

				Data parent_Data = parent->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				if(parent_Data.item_Type == item_Type_Regular_Aperiodic)
				{
					Data current_Data = new_Layer->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
					current_Data.prepare_Layer_For_Regular_Component();

					Regular_Component new_Regular_Component = parent_Data.regular_Components.first(); // similar to 1st
						new_Regular_Component.is_Common_Sigma = true;
						new_Regular_Component.components.fill(current_Data);
						new_Regular_Component.top_Id = current_Data.id;
						new_Regular_Component.find_Min_Max_Values();
					for(Data& inserted_Child : new_Regular_Component.components) { inserted_Child.reset_All_IDs(); }
					parent_Data.regular_Components.insert(position+1, new_Regular_Component);

					QVariant var;
					var.setValue( current_Data );
					new_Layer->setData(DEFAULT_COLUMN, Qt::UserRole, var);

					var.setValue( parent_Data );
					parent->setData(DEFAULT_COLUMN, Qt::UserRole, var);
				}
			}
		}
	}
	refresh_Toolbar();
	return new_Layer;
}
