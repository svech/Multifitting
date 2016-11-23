#include "structure_tree.h"

Structure_Tree::Structure_Tree(Multilayer* multilayer, QWidget *parent) :
	QWidget(parent),
	multilayer(multilayer)
{
	create_Tree();
	create_Toolbar();
}

void Structure_Tree::emit_Refresh()
{
	emit refresh();
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

	structure_Toolbar->add_Layer(true);
}

void Structure_Tree::multiple_Refresh_Over_Struct()
{
	refresh_Over_Struct();
	QMetaObject::invokeMethod(this, "refreshOverStruct", Qt::QueuedConnection);
	refresh_Over_Struct();
	QMetaObject::invokeMethod(this, "refreshOverStruct", Qt::QueuedConnection);
}

void Structure_Tree::refresh_Over_Struct()
{
	different_Layers_Counter=0;
	iterate_Over_Struct();
	iterate_Over_Multilayers();
	multilayer->reset_Independent_Variables_Structure();
}

void Structure_Tree::refresh_Text()
{
	different_Layers_Counter=0;
	iterate_Over_Struct();
	iterate_Over_Multilayers();
}

void Structure_Tree::iterate_Over_Struct(QTreeWidgetItem* item)
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
			set_Structure_Item_Text(this_Item);
			QStringList list = this_Item->text(DEFAULT_COLUMN).split("layer");
			this_Item->setText(DEFAULT_COLUMN, list[0] + "layer (" + QString::number(different_Layers_Counter) + ")" + list[1]);
			this_Item->setWhatsThis(DEFAULT_COLUMN, QString(whats_This_Layer) + item_Type_Delimiter + "(" + QString::number(different_Layers_Counter) + ")");

			Layer layer = this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
			layer.layer_Index = different_Layers_Counter;
			QVariant var; var.setValue( layer );
			this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
	} else
	// if multilayer
	{
		iterate_Over_Struct(this_Item);
	}
}

void Structure_Tree::iterate_Over_Multilayers(QTreeWidgetItem* item)
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

void Structure_Tree::refresh_If_Multilayer(QTreeWidgetItem* this_Item)
{
	// if multilayer
	if(this_Item->childCount()>0)
	{
		// first and last layers
		int first =-1, last = -2;
		find_First_Layer(this_Item, first);
		find_Last_Layer(this_Item, last);

		set_Structure_Item_Text(this_Item);
		QStringList list = this_Item->text(DEFAULT_COLUMN).split("Multilayer");
		this_Item->setText(DEFAULT_COLUMN, list[0] + "Multilayer (" + QString::number(first) + " - " + QString::number(last) + ")" + list[1]);
		this_Item->setWhatsThis(DEFAULT_COLUMN, QString(whats_This_Multilayer) + item_Type_Delimiter + "(" + QString::number(first) + " - " + QString::number(last) + ")");

		// period
		find_Period(this_Item);

		iterate_Over_Multilayers(this_Item);
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
					/ this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().period.value;
		} else
		// multilayer
		{
			gamma =   this_Item->child(0)->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().period.value
					* this_Item->child(0)->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().num_Repetition.value
					/ this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().period.value;
		}
	}

	// refresh data
	Stack_Content stack_Content = this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
	stack_Content.period.value = period;
	stack_Content.gamma.value  = gamma;
	QVariant var; var.setValue(stack_Content);
	this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

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
		Item_Editor* item_Editor = new Item_Editor(tree->currentItem(),this);
			item_Editor->setWindowFlags(Qt::Window);
			item_Editor->show();

		connect(item_Editor, SIGNAL(closed()), this, SLOT(editor_Close()));
		connect(item_Editor, SIGNAL(refresh()),this, SLOT(editors_Edit()));	// TODO ? (or editors_refresh?)
		connect(item_Editor, SIGNAL(refresh()),this, SLOT(emit_Refresh()));
		connect(item_Editor, SIGNAL(edited()), this, SLOT(editors_Edit()));


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
		item->setText(DEFAULT_COLUMN, "ambient: " + item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().material);
		if(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().material!="Vacuum")
		{
			item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + Rho_Sym + "=" +
						  QString::number(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().density.value,thumbnail_double_format,thumbnail_density_precision) + density_units);
		}
	} else
	{
		// if substrate
		if(item->whatsThis(DEFAULT_COLUMN)==whats_This_Substrate)
		{
			item->setText(DEFAULT_COLUMN, item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().material + " substrate" + ", " + Rho_Sym + "=" +
						  QString::number(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().density.value,thumbnail_double_format,thumbnail_density_precision) + density_units);

			if(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().sigma.value>0)
			{
				item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + Sigma_Sym + "=" +
							  QString::number(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().sigma.value/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision) + length_units);
			}
		} else
		{
			// if multilayer
			if(item->childCount()>0)
			{
				item->setText(DEFAULT_COLUMN, "Multilayer, N=" + QString::number(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().num_Repetition.value)
							  + ", d=" + QString::number(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().period.value/length_Coeff,thumbnail_double_format,thumbnail_period_precision) + length_units);

				if(item->childCount()==2)
				{
					item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + Gamma_Sym + "=" +
								  QString::number(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>().gamma.value,thumbnail_double_format,thumbnail_gamma_precision));
				}
			} else
			// if layer
			{
				item->setText(DEFAULT_COLUMN, item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().material + " layer"
							  + ", z=" +  QString::number(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().thickness.value/length_Coeff,thumbnail_double_format,thumbnail_thickness_precision) + length_units
							  + ", " + Rho_Sym + "=" +  QString::number(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().density.value,thumbnail_double_format,thumbnail_density_precision) + density_units);

				if(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().sigma.value>0)
				{
					item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + Sigma_Sym + "=" +
								  QString::number(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().sigma.value/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision) + length_units);
				}
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
	multiple_Refresh_Over_Struct();
}

void Structure_Tree::editors_Refresh()
{
	for(int i=0; i<list_Editors.size(); ++i)
	{
		list_Editors[i]->show_All();
		list_Editors[i]->initial_Radio_Check(false);
	}
}

void Structure_Tree::editors_Edit()
{
	multiple_Refresh_Over_Struct();

	// broadcast to other editors
	for(int i=0; i<list_Editors.size(); ++i)
	{
		if(list_Editors[i]!=sender())
		{
			list_Editors[i]->show_All();
			list_Editors[i]->initial_Radio_Check(false);
		}
	}
}
