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
	id_Type tree_Id = Global_Definitions::generate_Id();
	tree->setProperty(id_Property, tree_Id);

		tree->setColumnCount(1);
		tree->header()->close();
		tree->expandAll();
		tree->setExpandsOnDoubleClick(false);

	connect(tree, &QTreeWidget::itemDoubleClicked, this, &Structure_Tree::if_DoubleClicked);
}

void Structure_Tree::create_Toolbar()
{
	structure_Toolbar = new Structure_Toolbar(this, this);

	connect(tree, &QTreeWidget::itemSelectionChanged, structure_Toolbar, &Structure_Toolbar::if_Selected);
	structure_Toolbar->add_Ambient();
	connect(structure_Toolbar, &Structure_Toolbar::refresh_Str_And_Independ_signal, multilayer, [=]{multilayer->refresh_Structure_And_Independent();});
}

////-----------------------------------------------------------------------------------
//// refresh text and data of tree
////-----------------------------------------------------------------------------------

void Structure_Tree::refresh__StructureTree__Data_and_Text()
{
	/// emergency kit
	//	refresh__StructureTree__Data_and_Text(); //-V760
	//	QMetaObject::invokeMethod(this, "refreshStructureTreeDataAndText", Qt::QueuedConnection);
	//	refresh__StructureTree__Data_and_Text();
	//	QMetaObject::invokeMethod(this, "refreshStructureTreeDataAndText", Qt::QueuedConnection);

//	qInfo() << "refresh__StructureTree__Data_and_Text";
	iterate_Over_Layers();
	iterate_Over_Multilayers();


	/// temporary. delete it
//	qInfo() << "";
//	QTreeWidgetItem* structure_Item = NULL;
//	QTreeWidgetItemIterator it(tree);
//	while (*it)
//	{
//		structure_Item = *it;
//		Data data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
//		qInfo() << "       :" << data.item_Type << data.id;
//		++it;
//	}	qInfo() << "";
}

void Structure_Tree::iterate_Over_Layers(QTreeWidgetItem* item)
{
	// over main tree
	if(item==NULL)
	{
		// set counter to zero
		different_Layers_Counter=0;
		for(int i=0; i<tree->topLevelItemCount(); i++)
		{
			refresh_If_Layer(tree->topLevelItem(i));
		}
	} else
	// over child multilayer/aperiodic
	{
		for(int i=0; i<item->childCount(); i++)
		{
			refresh_If_Layer(item->child(i));
		}
	}
}

void Structure_Tree::refresh_If_Layer(QTreeWidgetItem* this_Item)
{
	Data data = this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	// if not multilayer/aperiodic
	if(this_Item->childCount()==0)
	{
		// assign number if layer
		if(data.item_Type == item_Type_Layer)
		{
			different_Layers_Counter++;
			data.layer_Index = different_Layers_Counter;
			if(this_Item->parent())
			{
				data.has_Parent = true;
			} else
			{
				data.has_Parent = false;
			}
		}

		// remove depth and sigma grading if independent layer
		bool no_Good_Parent = !this_Item->parent();
		if(this_Item->parent())
		{
			Data parent_Data = this_Item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			no_Good_Parent = (parent_Data.item_Type != item_Type_Multilayer);
		}
		if(no_Good_Parent)
		{
			data.thickness_Drift.is_Drift_Line = false;
			data.thickness_Drift.is_Drift_Rand = false;
			data.thickness_Drift.is_Drift_Sine = false;

			data.sigma_Drift.is_Drift_Line = false;
			data.sigma_Drift.is_Drift_Rand = false;
			data.sigma_Drift.is_Drift_Sine = false;
		}

		QVariant var; var.setValue( data );
		this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		set_Structure_Item_Text(this_Item);
		set_Item_Parent_Type(this_Item);
	} else
	// if multilayer, go deeper
	{
		iterate_Over_Layers(this_Item);
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
	} else
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
	// if multilayer/aperiodic
	if(this_Item->childCount()>0)
	{
		// find and save first and last layers
		int first =-1, last = -2;
		find_First_Layer(this_Item, first);
		find_Last_Layer(this_Item, last);

		// find and save period and gamma
		// check if periodic multilayer inside functions
		{
			find_Period_And_Gamma(this_Item);
			iterate_Over_Multilayers(this_Item);
		}

		set_Structure_Item_Text(this_Item);
		set_Item_Parent_Type(this_Item);
	}
}

void Structure_Tree::find_First_Layer(QTreeWidgetItem* this_Item, int& first)
{
	first=-2016;
	// if first is pure layer
	if(this_Item->child(0)->childCount()==0)
	{
		first = this_Item->child(0)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().layer_Index;
	} else
	// go deeper
	{
		find_First_Layer(this_Item->child(0), first);
	}

	Data data = this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	data.first_Layer_Index = first;
	QVariant var; var.setValue( data );
	this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

void Structure_Tree::find_Last_Layer(QTreeWidgetItem* this_Item, int& last)
{
	last=-1025;
	int last_Index = this_Item->childCount()-1;
	// if last is pure layer
	if(this_Item->child(last_Index)->childCount()==0)
	{
		last = this_Item->child(this_Item->childCount()-1)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().layer_Index;
	} else
	// go deeper
	{
		find_Last_Layer(this_Item->child(last_Index), last);
	}

	Data data = this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	data.last_Layer_Index = last;
	QVariant var; var.setValue( data );
	this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

void Structure_Tree::find_Period_And_Gamma(QTreeWidgetItem* this_Item)
{
	Data stack_Data = this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(stack_Data.item_Type == item_Type_Multilayer)
	{
		double period=0;
		// iterate over childs
		for(int i=0; i<this_Item->childCount(); i++)
		{
			Data data_i = this_Item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			// if layer
			if(this_Item->child(i)->childCount()==0)
			{
				period += data_i.thickness.value;
			}
			else
			// if multilayer, go deeper
			{
				find_Period_And_Gamma(this_Item->child(i));
				period += data_i.period.value * data_i.num_Repetition.value;
			}
		}

		// gamma update
		double gamma=-4;
		if(this_Item->childCount()==2)
		{
			Data data_0 = this_Item->child(0)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			// layer
			if(this_Item->child(0)->childCount()==0)
			{
				gamma = data_0.thickness.value/period;
			} else
			// multilayer
			{
				gamma = data_0.period.value*data_0.num_Repetition.value/period;
			}
		}

		// refresh data
		stack_Data.period.value = period;
		stack_Data.gamma.value  = gamma;
		QVariant var; var.setValue(stack_Data);
		this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
}

////-----------------------------------------------------------------------------------

void Structure_Tree::if_DoubleClicked()
{
	const Data data = tree->currentItem()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(data.item_Type == item_Type_Aperiodic) return;

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

		connect(item_Editor, &Item_Editor::closed, this, &Structure_Tree::editor_Close);
		connect(item_Editor, &Item_Editor::item_Data_Edited, multilayer, [=]{multilayer->refresh_Structure_And_Independent();});

		list_Editors.append(item_Editor);
		structure_Toolbar->toolbar->setDisabled(true);
		runned_Editors.insert(tree->currentItem(),item_Editor);
	}
}

void Structure_Tree::set_Structure_Item_Text(QTreeWidgetItem* item)
{
	double length_Coeff = length_Coefficients_Map.value(length_units);

	const Data data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QString density_Text;

	// if ambient
	if(data.item_Type == item_Type_Ambient)
	{
		if(data.material!="Vacuum")
		{
			if(data.composed_Material)	density_Text = QString::number(data.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + density_units;
			else						density_Text = QString::number(data.relative_Density.value,thumbnail_double_format,thumbnail_density_precision);
			item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data) + ", " + Rho_Sym + "=" + density_Text);
		}
	} else
	{
		// average sigma
		QString temp_Sigma_Sym;
		if(data.common_Sigma)	{temp_Sigma_Sym =     Sigma_Sym;	}
		else					{temp_Sigma_Sym = "<"+Sigma_Sym+">";}


		// if substrate
		if(data.item_Type == item_Type_Substrate)
		{
			if(data.composed_Material)	density_Text = QString::number(data.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + density_units;
			else						density_Text = QString::number(data.relative_Density.value,thumbnail_double_format,thumbnail_density_precision);
			item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data) + ", " + Rho_Sym + "=" + density_Text);

			if(data.sigma.value>0)
			{
				item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + temp_Sigma_Sym + "=" +
							  QString::number(data.sigma.value/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision) + length_units);
			}
		} else
		{
			// if multilayer/aperiodic
			if(item->childCount()>0)
			{
				if(data.item_Type == item_Type_Multilayer)
				{
					item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data) + ", N=" + QString::number(data.num_Repetition.value)
								  + ", d="  + QString::number(data.period.value/length_Coeff,thumbnail_double_format,thumbnail_period_precision) + length_units);

					if(item->childCount()==2 && abs(data.period.value)>DBL_EPSILON)
					{
						item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + Gamma_Sym + "=" +
									  QString::number(data.gamma.value,thumbnail_double_format,thumbnail_gamma_precision));
					}
				} else
				if(data.item_Type == item_Type_Aperiodic)
				{
					item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data));
				}
			} else
			// if layer
			{
				QString thickness_Text = ", z=" +  QString::number(data.thickness.value/length_Coeff,thumbnail_double_format,thumbnail_thickness_precision) + length_units;

				if(data.composed_Material)	{density_Text = QString::number(data.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + density_units;	}
				else						{density_Text = QString::number(data.relative_Density.value,thumbnail_double_format,thumbnail_density_precision);					}

				item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data) + thickness_Text + ", " + Rho_Sym + "=" + density_Text);

				if(data.sigma.value>0)
				{
					item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + temp_Sigma_Sym + "=" +
								  QString::number(data.sigma.value/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision) + length_units);

					// show sigma drift
					Drift sigma_Drift = data.sigma_Drift;
					if(sigma_Drift.is_Drift_Line || sigma_Drift.is_Drift_Sine || sigma_Drift.is_Drift_Rand)
					{
						item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + " || d" + Sigma_Sym + " = on");
					}
				}

				// reflect thickness drift
				Drift thick_Drift = data.thickness_Drift;
				QString thick_Drift_Text = "";
				if(thick_Drift.is_Drift_Line || thick_Drift.is_Drift_Sine || thick_Drift.is_Drift_Rand)
					thick_Drift_Text += "\n---> dz =";
				if(thick_Drift.is_Drift_Line) thick_Drift_Text += " {line, " + QString::number(thick_Drift.drift_Line_Value.value,		thumbnail_double_format,thumbnail_drift_precision) + "%}";
				if(thick_Drift.is_Drift_Sine) thick_Drift_Text += " {sine, " + QString::number(thick_Drift.drift_Sine_Amplitude.value,	thumbnail_double_format,thumbnail_drift_precision) + "%,"
																			 + QString::number(thick_Drift.drift_Sine_Frequency.value,	thumbnail_double_format,thumbnail_drift_precision) + ","
																			 + QString::number(thick_Drift.drift_Sine_Phase.value,		thumbnail_double_format,thumbnail_drift_precision) + "}";
				if(thick_Drift.is_Drift_Rand) thick_Drift_Text += " {rand, " + QString::number(thick_Drift.drift_Rand_Rms.value,		thumbnail_double_format,thumbnail_drift_precision) + "%}";

				item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) +  thick_Drift_Text);
			}
		}
	}
}

void Structure_Tree::set_Item_Parent_Type(QTreeWidgetItem *item)
{
	Data data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(item->parent())
	{
		Data parent_Data = item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		data.parent_Item_Type = parent_Data.item_Type;
	} else
	{
		data.parent_Item_Type = NOPARENT;
	}
	QVariant var; var.setValue(data);
	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
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
			list_Editors[i]->reload_And_Show_All();
		}
	}
}
