// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "structure_tree.h"

Structure_Tree::Structure_Tree(Multilayer* multilayer, QWidget *parent) :
	QWidget(parent),
	multilayer(multilayer)
{
	create_Tree();
	create_Toolbar();
}

void Structure_Tree::create_Tree()
{
	tree = new QTreeWidget(this);
		tree->setColumnCount(1);
		tree->header()->close();
		tree->expandAll();
		tree->setExpandsOnDoubleClick(false);
	multilayer->struct_Frame_Layout->addWidget(tree);

	connect(tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(if_DoubleClicked(QTreeWidgetItem*, int)));
}

void Structure_Tree::create_Toolbar()
{
	structure_Toolbar = new Structure_Toolbar(this, this);
		multilayer->struct_Frame_Layout->addWidget(structure_Toolbar->toolbar);

	connect(tree, SIGNAL(itemSelectionChanged()), structure_Toolbar, SLOT(if_Selected()));
	structure_Toolbar->add_Layer(false);
	connect(structure_Toolbar, SIGNAL(refresh_Str_And_Independ_signal()), multilayer, SLOT(refresh_Structure_And_Independent()));
}

//-----------------------------------------------------------------------------------
// refresh text and data of tree
//-----------------------------------------------------------------------------------

void Structure_Tree::refresh__StructureTree__Data_and_Text()
{
	/// emergency kit
	//	refresh__StructureTree__Data_and_Text(); //-V760
	//	QMetaObject::invokeMethod(this, "refreshStructureTreeDataAndText", Qt::QueuedConnection);
	//	refresh__StructureTree__Data_and_Text();
	//	QMetaObject::invokeMethod(this, "refreshStructureTreeDataAndText", Qt::QueuedConnection);

	different_Layers_Counter=0;
	refresh_Layers();
	refresh_Multilayers();
}

void Structure_Tree::refresh_Layers(QTreeWidgetItem* item)
{
	// over main tree
	if(item==NULL)
	{
		for(int i=0; i<tree->topLevelItemCount(); i++)
		{
			refresh_If_Layer(tree->topLevelItem(i));
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

void Structure_Tree::set_Layer_Text_And_WhatsThis(QTreeWidgetItem* item, int dif_Layers_Counter)
{
	set_Structure_Item_Text(item);
	QStringList list = item->text(DEFAULT_COLUMN).split("layer");
	item->setText(DEFAULT_COLUMN, list[0] + "layer (" + QString::number(different_Layers_Counter) + ")" + list[1]);
	item->setWhatsThis(DEFAULT_COLUMN, QString(whats_This_Layer) + item_Type_Delimiter + "(" + QString::number(dif_Layers_Counter) + ")");
}

void Structure_Tree::refresh_If_Layer(QTreeWidgetItem* this_Item)
{
	// if not multilayer
	if(this_Item->childCount()==0)
	{
		// just refresh name if ambient or substrate
		if((this_Item->whatsThis(DEFAULT_COLUMN)==whats_This_Ambient)||(this_Item->whatsThis(DEFAULT_COLUMN)==whats_This_Substrate))
		{
			set_Structure_Item_Text(this_Item);
		} else
		// assign number if layer
		{
			different_Layers_Counter++;
			set_Layer_Text_And_WhatsThis(this_Item, different_Layers_Counter);

			Layer layer = this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
			layer.layer_Index = different_Layers_Counter;
			QVariant var; var.setValue( layer );
			this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
	} else
	// if multilayer
	{
		refresh_Layers(this_Item);
	}
}

void Structure_Tree::refresh_Multilayers(QTreeWidgetItem* item)
{
	// over main tree
	if(item==NULL)
	{
		for(int i=0; i<tree->topLevelItemCount(); i++)
		{
			refresh_If_Multilayer(tree->topLevelItem(i));
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

void Structure_Tree::set_Multilayer_Text_And_WhatsThis(QTreeWidgetItem* item, int first, int last)
{
	set_Structure_Item_Text(item);
	QStringList list = item->text(DEFAULT_COLUMN).split("Multilayer");
	item->setText(DEFAULT_COLUMN, list[0] + "Multilayer (" + QString::number(first) + " - " + QString::number(last) + ")" + list[1]);
	item->setWhatsThis(DEFAULT_COLUMN, QString(whats_This_Multilayer) + item_Type_Delimiter + "(" + QString::number(first) + " - " + QString::number(last) + ")");
}

void Structure_Tree::refresh_If_Multilayer(QTreeWidgetItem* this_Item)
{
	// if multilayer
	if(this_Item->childCount()>0)
	{
		// first and last layers
		int first =-1, last = -2;
		find_First_Layer(this_Item, first);
		find_Last_Layer(this_Item, last);

		// period
		find_Period(this_Item);

		set_Multilayer_Text_And_WhatsThis(this_Item, first, last);

		refresh_Multilayers(this_Item);
	}
}

void Structure_Tree::find_First_Layer(QTreeWidgetItem* this_Item, int& first)
{
	first=-2016;
	// if first is pure layer
	if(this_Item->child(0)->childCount()==0)
	{
		first = this_Item->child(0)->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().layer_Index;
	} else
	// go deeper
	{
		find_First_Layer(this_Item->child(0), first);
	}
}

void Structure_Tree::find_Last_Layer(QTreeWidgetItem* this_Item, int& last)
{
	last=-1025;
	// if last is pure layer
	if(this_Item->child(this_Item->childCount()-1)->childCount()==0)
	{
		last = this_Item->child(this_Item->childCount()-1)->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().layer_Index;
	} else
	// go deeper
	{
		find_Last_Layer(this_Item->child(this_Item->childCount()-1), last);
	}
}

void Structure_Tree::find_Period(QTreeWidgetItem* this_Item)
{
	double period=0;
	// iterate over childs
	for(int i=0; i<this_Item->childCount(); i++)
	{
		// if layer
		if(this_Item->child(i)->childCount()==0) {period += this_Item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().thickness.value;}
		else
		// go deeper
		{
			find_Period(this_Item->child(i));
			period +=   this_Item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().period.value
					  * this_Item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().num_Repetition.value;
		}
	}

	// gamma update
	double gamma=-4;
	if(this_Item->childCount()==2)
	{
		// layer
		if(this_Item->child(0)->childCount()==0)
		{
			gamma =   this_Item->child(0)->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().thickness.value
					/ period;
		} else
		// multilayer
		{
			gamma =   this_Item->child(0)->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().period.value
					* this_Item->child(0)->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().num_Repetition.value
					/ period;
		}
	}

	// refresh data
	Stack_Content stack_Content = this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
	stack_Content.period.value = period;
	stack_Content.gamma.value  = gamma;
	QVariant var; var.setValue(stack_Content);
	this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

//-----------------------------------------------------------------------------------

void Structure_Tree::if_DoubleClicked(QTreeWidgetItem*, int)
{
	if(runned_Editors.contains(tree->currentItem()))
	{
		if(runned_Editors.value(tree->currentItem())->isVisible())
		{
			runned_Editors.value(tree->currentItem())->activateWindow();
		} else
		{
			delete runned_Editors.value(tree->currentItem());
			runned_Editors.remove(tree->currentItem());
		}
	}
	if(!runned_Editors.contains(tree->currentItem()))
	{
		Item_Editor* item_Editor = new Item_Editor(list_Editors, tree->currentItem(), this);
			item_Editor->setWindowFlags(Qt::Window);
			item_Editor->show();

		connect(item_Editor, SIGNAL(closed()), this, SLOT(editor_Close()));
		connect(item_Editor, SIGNAL(item_Data_Edited()), multilayer, SLOT(refresh_Structure_And_Independent()));

		list_Editors.append(item_Editor);
		structure_Toolbar->toolbar->setDisabled(true);
		runned_Editors.insert(tree->currentItem(),item_Editor);
	}
}

void Structure_Tree::set_Structure_Item_Text(QTreeWidgetItem* item)
{
	double length_Coeff = length_Coefficients_Map.value(length_units);

	// if ambient
	if(item->whatsThis(DEFAULT_COLUMN)==whats_This_Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		if(ambient.material!="Vacuum")
		{
			if(ambient.composed_Material)
			{
				item->setText(DEFAULT_COLUMN, "ambient: " + ambient.material + ", " + Rho_Sym + "=" +
							  QString::number(ambient.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + density_units);
			} else
			{
				item->setText(DEFAULT_COLUMN, "ambient: " + ambient.material + ", " + Rho_Sym + "=" +
							  QString::number(ambient.relative_Density.value,thumbnail_double_format,thumbnail_density_precision));
			}
		}
	} else
	{
		QString temp_Sigma_Sym;
		// if substrate
		if(item->whatsThis(DEFAULT_COLUMN)==whats_This_Substrate)
		{
			Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
			if(substrate.composed_Material)
			{
				item->setText(DEFAULT_COLUMN, substrate.material + " substrate" + ", " + Rho_Sym + "=" +
							  QString::number(substrate.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + density_units);
			} else
			{
				item->setText(DEFAULT_COLUMN, substrate.material + " substrate" + ", " + Rho_Sym + "=" +
							  QString::number(substrate.relative_Density.value,thumbnail_double_format,thumbnail_density_precision));
			}

			if(substrate.sigma.value>0)
			{
				// average sigma
				if(substrate.common_Sigma)
					temp_Sigma_Sym = Sigma_Sym;
				else
					temp_Sigma_Sym = "<"+Sigma_Sym+">";

				item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + temp_Sigma_Sym + "=" +
							  QString::number(substrate.sigma.value/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision) + length_units);
			}
		} else
		{
			// if multilayer
			if(item->childCount()>0)
			{
				Stack_Content stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
				item->setText(DEFAULT_COLUMN, "Multilayer, N=" + QString::number(stack_Content.num_Repetition.value)
							  + ", d=" + QString::number(stack_Content.period.value/length_Coeff,thumbnail_double_format,thumbnail_period_precision) + length_units);

				if(item->childCount()==2)
				{
					item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + Gamma_Sym + "=" +
								  QString::number(stack_Content.gamma.value,thumbnail_double_format,thumbnail_gamma_precision));
				}
			} else
			// if layer
			{
				Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
				QString thickness_Text = ", z=" +  QString::number(layer.thickness.value/length_Coeff,thumbnail_double_format,thumbnail_thickness_precision) + length_units;

				if(layer.composed_Material)
				{
					item->setText(DEFAULT_COLUMN, layer.material + " layer"
								  + thickness_Text
								  + ", " + Rho_Sym + "=" +  QString::number(layer.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + density_units);
				} else
				{
					item->setText(DEFAULT_COLUMN, layer.material + " layer"
								  + thickness_Text
								  + ", " + Rho_Sym + "=" +  QString::number(layer.relative_Density.value,thumbnail_double_format,thumbnail_density_precision));
				}

				if(layer.sigma.value>0)
				{
					// average sigma
					if(layer.common_Sigma)
						temp_Sigma_Sym = Sigma_Sym;
					else
						temp_Sigma_Sym = "<"+Sigma_Sym+">";

					item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + temp_Sigma_Sym + "=" +
								  QString::number(layer.sigma.value/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision) + length_units);

					// reflect sigma drift
					Drift sigma_Drift = layer.sigma_Drift;
					if(sigma_Drift.is_Drift_Line || sigma_Drift.is_Drift_Sine || sigma_Drift.is_Drift_Rand)
						item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + " || d" + Sigma_Sym + " = on");
				}

				// reflect thickness drift
				Drift thick_Drift = layer.thickness_Drift;
				QString thick_Drift_Text = "";
				if(thick_Drift.is_Drift_Line || thick_Drift.is_Drift_Sine || thick_Drift.is_Drift_Rand)
					thick_Drift_Text += "\n---> dz =";
				if(thick_Drift.is_Drift_Line) thick_Drift_Text += " {line, " + QString::number(thick_Drift.drift_Line_Value.value,thumbnail_double_format,thumbnail_thickness_precision) + "%}";
				if(thick_Drift.is_Drift_Sine) thick_Drift_Text += " {sine, " + QString::number(thick_Drift.drift_Sine_Amplitude.value,thumbnail_double_format,thumbnail_thickness_precision) + "%,"
																			 + QString::number(thick_Drift.drift_Sine_Frequency.value,thumbnail_double_format,thumbnail_thickness_precision) + ","
																			 + QString::number(thick_Drift.drift_Sine_Phase.value,thumbnail_double_format,thumbnail_thickness_precision) + "}";
				if(thick_Drift.is_Drift_Rand) thick_Drift_Text += " {rand, " + QString::number(thick_Drift.drift_Rand_Rms.value,thumbnail_double_format,thumbnail_thickness_precision) + "%}";

				item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) +  thick_Drift_Text);
			}
		}
	}
}

void Structure_Tree::editor_Close()
{
	// remove closed editor from list
	for(int i=0; i<list_Editors.size(); ++i)
	{
		if(list_Editors[i]==sender())
		{
			list_Editors.removeAt(i);
		}
	}

	if(list_Editors.isEmpty())
	{
		structure_Toolbar->toolbar->setDisabled(false);
	}
}

void Structure_Tree::editors_Edit(QObject* sender)
{
	// broadcast to other editors
	for(int i=0; i<list_Editors.size(); ++i)
	{
		if(list_Editors[i]!=sender)
		{
			list_Editors[i]->show_All();
			list_Editors[i]->initial_Radio_Check(false);
		}
	}
}
