#include "structure_tree.h"

Structure_Tree::Structure_Tree(Multilayer* multilayer, QWidget *parent) :
	QWidget(parent),
	multilayer(multilayer)
{
	create_Tree();
	create_Toolbar();
}

Structure_Tree::~Structure_Tree()
{
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
	structure_Toolbar->add_Substrate();
	connect(structure_Toolbar, &Structure_Toolbar::refresh_Str_And_Independ_signal, multilayer, [=]{multilayer->refresh_Structure_And_Independent();});

	// shortcuts
	{
		QShortcut* remove_Shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
		QShortcut* cut_Shortcut    = new QShortcut(QKeySequence(Qt::Key_X | Qt::CTRL), this);
		QShortcut* copy_Shortcut   = new QShortcut(QKeySequence(Qt::Key_C | Qt::CTRL), this);
		QShortcut* paste_Shortcut  = new QShortcut(QKeySequence(Qt::Key_V | Qt::CTRL), this);

		connect(remove_Shortcut, &QShortcut::activated, this, [=]	{
            if(structure_Toolbar->toolbar->isEnabled())
                if(bool(tree->currentItem()) && structure_Toolbar->toolbar->actions()[Remove]->isEnabled())
                {	structure_Toolbar->remove();	}
        });
		connect(cut_Shortcut, &QShortcut::activated, this, [=]	{
            if(structure_Toolbar->toolbar->isEnabled())
                if(bool(tree->currentItem()) && structure_Toolbar->toolbar->actions()[Cut]->isEnabled())
                {	structure_Toolbar->cut();	}
		});
		connect(copy_Shortcut, &QShortcut::activated, this, [=]	{
            if(structure_Toolbar->toolbar->isEnabled())
                if(bool(tree->currentItem()) && structure_Toolbar->toolbar->actions()[Copy]->isEnabled() )
                {	structure_Toolbar->copy();	}
        });
		connect(paste_Shortcut, &QShortcut::activated, this, [=]	{
            if(structure_Toolbar->toolbar->isEnabled())
                if(bool(tree->currentItem()) && structure_Toolbar->toolbar->actions()[Paste]->isEnabled())
                {	structure_Toolbar->paste();	}
        });
	}
}

void Structure_Tree::get_Total_Thickness(double& thickness, QTreeWidgetItem* item)
{
	for(int i=0; i<item->childCount(); ++i)
	{
		Data struct_Data = item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(struct_Data.item_Enabled)
		{
			// no children
			if(struct_Data.item_Type == item_Type_Layer)
			{
				thickness += struct_Data.thickness.value;
			}
			// have children
			if(item->child(i)->childCount()>0)
			{
				if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
				{
					for(int layer_Index=0; layer_Index<struct_Data.regular_Components.first().components.size(); layer_Index++)
					{
						for(int component_Index=0; component_Index<struct_Data.regular_Components.size(); component_Index++)
						{
							Data& component_Data = struct_Data.regular_Components[component_Index].components[layer_Index];
							thickness += component_Data.thickness.value;
						}
					}
				}
				if(struct_Data.item_Type == item_Type_Multilayer)
				{
					double multilayer_Thickness = 0;
					get_Total_Thickness(multilayer_Thickness, item->child(i));
					thickness += multilayer_Thickness*struct_Data.num_Repetition.value();
				}
				if(struct_Data.item_Type == item_Type_General_Aperiodic)
				{
					get_Total_Thickness(thickness, item->child(i));
				}
			}
		}
	}
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

//	qInfo() << "refresh__StructureTree__Data_and_Text" << endl;
	iterate_Over_Layers();
	iterate_Over_Multilayers();


	/// temporary. delete it
//	qInfo() << "" << endl;
//	QTreeWidgetItem* structure_Item = nullptr;
//	QTreeWidgetItemIterator it(tree);
//	while (*it)
//	{
//		structure_Item = *it;
//		Data data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
//		qInfo() << "       :" << data.item_Type << data.id << endl;
//		++it;
//	}	qInfo() << "" << endl;
}

void Structure_Tree::iterate_Over_Layers(QTreeWidgetItem* item)
{
	// over main tree
	if(item==nullptr)
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
			refresh_If_Layer(item->child(i), i);
		}
	}
}

void Structure_Tree::refresh_If_Layer(QTreeWidgetItem* this_Item, int i)
{
	Data data = this_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(data.item_Enabled) this_Item->setDisabled(false);
	else				  this_Item->setDisabled(true);


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
			no_Good_Parent = (parent_Data.item_Type != item_Type_Multilayer && parent_Data.item_Type != item_Type_Regular_Aperiodic);
		}
		if(no_Good_Parent)
		{
			data.thickness_Drift.is_Drift_Line = false;
			data.thickness_Drift.is_Drift_Rand = false;
			data.thickness_Drift.is_Drift_Sine = false;

			data.sigma_Diffuse_Drift.is_Drift_Line = false;
			data.sigma_Diffuse_Drift.is_Drift_Rand = false;
			data.sigma_Diffuse_Drift.is_Drift_Sine = false;
		}

		QVariant var; var.setValue( data );
		this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		set_Structure_Item_Text(this_Item, i);
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
	if(item==nullptr)
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
			iterate_Over_Multilayers(this_Item);
			find_Period_And_Gamma(this_Item);
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
		double first_Thickness = 0;
		// iterate over childs
		for(int i=0; i<this_Item->childCount(); i++)
		{
			QTreeWidgetItem* child = this_Item->child(i);
			Data data_i = child->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			// if layer
			if(data_i.item_Type == item_Type_Layer)
			{
				period += data_i.thickness.value;
				if(i==0) first_Thickness += data_i.thickness.value;
			}
			if(data_i.item_Type == item_Type_Multilayer)
			{
				find_Period_And_Gamma(child);
				period += data_i.period.value * data_i.num_Repetition.value();
				if(i==0) first_Thickness += data_i.period.value * data_i.num_Repetition.value();
			}
			if(data_i.item_Type == item_Type_General_Aperiodic)
			{
				for(int k=0; k<child->childCount(); k++)
				{
					find_Period_And_Gamma(child);
				}
			}
			if(data_i.item_Type == item_Type_Regular_Aperiodic)
			{
				for(int k=0; k<data_i.regular_Components.size(); k++)
				{
					for(int n=0; n<data_i.num_Repetition.value(); n++)
					{
						period += data_i.regular_Components[k].components[n].thickness.value;
						if(i==0) first_Thickness += data_i.regular_Components[k].components[n].thickness.value;
					}
				}
			}
		}

		// gamma update
		double gamma=-4;
		if(this_Item->childCount()==2)
		{
			gamma = first_Thickness/period;
		}

		// refresh data
		stack_Data.period.value = period;
		stack_Data.gamma.value  = gamma;
		QVariant var; var.setValue(stack_Data);
		this_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
}

////-----------------------------------------------------------------------------------

void Structure_Tree::if_DoubleClicked(QTreeWidgetItem* item)
{
	if(item == nullptr) item = tree->currentItem();
	if(runned_Editors.contains(item))
	{
		if(runned_Editors.value(item)->isVisible())
		{
			runned_Editors.value(item)->activateWindow();
		} else
		{
			delete runned_Editors.value(item);
			runned_Editors.remove(item);
		}
	}
	if(!runned_Editors.contains(item))
	{
		Item_Editor* item_Editor = new Item_Editor(list_Editors, item, this, this);
			item_Editor->setWindowFlags(Qt::Window);
			item_Editor->show();

		connect(item_Editor, &Item_Editor::closed, this, [=]
		{
			runned_Editors.remove(item);
			editor_Close();
		});
		connect(item_Editor, &Item_Editor::item_Data_Edited, multilayer, [=]{multilayer->refresh_Structure_And_Independent();});

		list_Editors.append(item_Editor);
		structure_Toolbar->toolbar->setDisabled(true);
		runned_Editors.insert(item,item_Editor);
	}
	refresh_Tree_Roughness();
	refresh_Tree_Particles();
}

void Structure_Tree::set_Structure_Item_Text(QTreeWidgetItem* item, int i)
{
	double length_Coeff = length_Coefficients_Map.value(length_units);

	const Data data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	QString density_Text;

	// if ambient
	if(data.item_Type == item_Type_Ambient)
	{
		if(data.material!=Vacuum)
		{
			if(data.composed_Material)	density_Text = Locale.toString(data.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + density_units;
			else						density_Text = Locale.toString(data.relative_Density.value,thumbnail_double_format,thumbnail_density_precision);
			item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data) + ", " + Rho_Sym + "=" + density_Text);
		} else
		{
			item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data));
		}
	} else
	{
		// average sigma
		QString temp_Sigma_Sym;
		if(data.common_Sigma_Diffuse)	{temp_Sigma_Sym =  "s";	}
		else					{temp_Sigma_Sym = "<s>";}


		// if substrate
		if(data.item_Type == item_Type_Substrate)
		{
			if(data.composed_Material)	density_Text = Locale.toString(data.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + density_units;
			else						density_Text = Locale.toString(data.relative_Density.value,thumbnail_double_format,thumbnail_density_precision);
			item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data) + ", " + Rho_Sym + "=" + density_Text);

			if(data.sigma_Diffuse.value>0)
			{
				item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + temp_Sigma_Sym + "=" +
							  Locale.toString(data.sigma_Diffuse.value/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision) + length_units);
			}
		} else
		{
			// if multilayer/aperiodic
			if(item->childCount()>0)
			{
				if(data.item_Type == item_Type_Multilayer)
				{
					item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data) + ", N=" + Locale.toString(data.num_Repetition.value())
								  + ", d="  + Locale.toString(data.period.value/length_Coeff,thumbnail_double_format,thumbnail_thickness_precision) + length_units);

					if(item->childCount()==2 && abs(data.period.value)>DBL_EPSILON)
					{
						item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + ", " + Gamma_Sym + "=" +
									  Locale.toString(data.gamma.value,thumbnail_double_format,thumbnail_gamma_precision));
					}
				} else
				if( data.item_Type == item_Type_Regular_Aperiodic ||
					data.item_Type == item_Type_General_Aperiodic )
				{
					item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data));
				}
			} else
			// if layer
			{
				QString thickness_Text, sigma_Text;
				Data parent_Data;
				if(item->parent())
				{
					parent_Data = item->parent()->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				}
				if(item->parent() && parent_Data.item_Type == item_Type_Regular_Aperiodic)
				{
					parent_Data.regular_Components[i].find_Min_Max_Values();
					thickness_Text = ", z=<" + Locale.toString(parent_Data.regular_Components[i].min_Max_Values.thickness_Min/length_Coeff,thumbnail_double_format,thumbnail_thickness_precision)
									   + "-" + Locale.toString(parent_Data.regular_Components[i].min_Max_Values.thickness_Max/length_Coeff,thumbnail_double_format,thumbnail_thickness_precision)
									   + ">" + length_units;

					if(abs(parent_Data.regular_Components[i].min_Max_Values.sigma_Diffuse_Max)>2*DBL_EPSILON)
					{
						sigma_Text     = ", " + temp_Sigma_Sym + "=<" + Locale.toString(parent_Data.regular_Components[i].min_Max_Values.sigma_Diffuse_Min/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision)
																+ "-" + Locale.toString(parent_Data.regular_Components[i].min_Max_Values.sigma_Diffuse_Max/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision)
																+ ">" + length_units;
					} else {
						sigma_Text = "";
					}
				} else
				{
					thickness_Text = ", z=" +  Locale.toString(data.thickness.value/length_Coeff,thumbnail_double_format,thumbnail_thickness_precision) + length_units;
					if(abs(data.sigma_Diffuse.value)>2*DBL_EPSILON) { sigma_Text = ", " + temp_Sigma_Sym + "=" + Locale.toString(data.sigma_Diffuse.value/length_Coeff,thumbnail_double_format,thumbnail_sigma_precision) + length_units; }
				}

				QString left_Bracket = "";
				QString right_Bracket = "";
				if(multilayer->imperfections_Model.use_Particles && data.particles_Model.is_Used) {left_Bracket = "<"; right_Bracket = ">";}
				if(data.composed_Material)	{density_Text = Locale.toString(data.average_Layer_density(),thumbnail_double_format,thumbnail_density_precision) + density_units;	}
				else						{density_Text = Locale.toString(data.average_Layer_density(),thumbnail_double_format,thumbnail_density_precision);					}

				item->setText(DEFAULT_COLUMN, Global_Variables::structure_Item_Name(data) + thickness_Text + ", " + left_Bracket + Rho_Sym + right_Bracket + "=" + density_Text);


				item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + sigma_Text);

				if(!item->parent() || parent_Data.item_Type != item_Type_Regular_Aperiodic)
				{
					if(data.sigma_Diffuse.value>0)
					{
						// show sigma drift
						Drift sigma_Drift = data.sigma_Diffuse_Drift;
						if(sigma_Drift.is_Drift_Line || sigma_Drift.is_Drift_Sine || sigma_Drift.is_Drift_Rand)
						{
							item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) + " || ds = on");
						}
					}

					// reflect thickness drift
					Drift thick_Drift = data.thickness_Drift;
					QString thick_Drift_Text = "";
					if(thick_Drift.is_Drift_Line || thick_Drift.is_Drift_Sine || thick_Drift.is_Drift_Rand)
					   thick_Drift_Text += "\n---> dz =";
					if(thick_Drift.is_Drift_Line) thick_Drift_Text += " {line, " + Locale.toString(thick_Drift.drift_Line_Value.value,		thumbnail_double_format,thumbnail_drift_precision) + "%}";
					if(thick_Drift.is_Drift_Sine) thick_Drift_Text += " {sine, " + Locale.toString(thick_Drift.drift_Sine_Amplitude.value,	thumbnail_double_format,thumbnail_drift_precision) + "%,"
																				 + Locale.toString(thick_Drift.drift_Sine_Frequency.value,	thumbnail_double_format,thumbnail_drift_precision) + ","
																				 + Locale.toString(thick_Drift.drift_Sine_Phase.value,		thumbnail_double_format,thumbnail_drift_precision) + "}";
					if(thick_Drift.is_Drift_Rand) thick_Drift_Text += " {rand, " + Locale.toString(thick_Drift.drift_Rand_Rms.value,		thumbnail_double_format,thumbnail_drift_precision) + "%}";

					item->setText(DEFAULT_COLUMN, item->text(DEFAULT_COLUMN) +  thick_Drift_Text);
				}
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

void Structure_Tree::refresh_Tree_Roughness()
{
	QTreeWidgetItemIterator it(tree);
	while(*it)
	{
		QTreeWidgetItem* item = *it;
		Data struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if( struct_Data.item_Type == item_Type_Layer ||
			struct_Data.item_Type == item_Type_Substrate )
		{
			bool is_Last_Layer = false;
			if( struct_Data.item_Type == item_Type_Layer)
			{
				QTreeWidgetItemIterator next_It = it;
				++next_It;
				QTreeWidgetItem* next_Item = *next_It;
				Data next_Struct_Data = next_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				if(next_Struct_Data.item_Type == item_Type_Substrate)
				{
					is_Last_Layer = true;
				}
			}
			Global_Variables::enable_Disable_Roughness_Model(struct_Data, multilayer->imperfections_Model, is_Last_Layer);

			QVariant var;
			var.setValue( struct_Data );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
		++it;
	}
}

void Structure_Tree::refresh_Tree_Particles(bool refresh_Shape, bool refresh_Interference_Function, bool refresh_Geometry)
{
	QTreeWidgetItemIterator it(multilayer->structure_Tree->tree);
	while(*it)
	{
		QTreeWidgetItem* item = *it;
		Data struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if( struct_Data.item_Type == item_Type_Layer )
		{
			bool is_Last_Layer = false;
			// if last layer
			{
				QTreeWidgetItemIterator next_It = it;
				++next_It;
				QTreeWidgetItem* next_Item = *next_It;
				Data next_Struct_Data = next_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				if(next_Struct_Data.item_Type == item_Type_Substrate)
				{
					is_Last_Layer = true;
				}
			}
			Global_Variables::enable_Disable_Particles_Model(struct_Data, multilayer->imperfections_Model, is_Last_Layer);

			if(refresh_Shape)
			{
				struct_Data.particles_Model.particle_Shape = multilayer->imperfections_Model.initial_Particle_Shape;
			}
			if(refresh_Interference_Function)
			{
				struct_Data.particles_Model.particle_Interference_Function = multilayer->imperfections_Model.initial_Interference_Function;
			}
			if(refresh_Geometry)
			{
				struct_Data.particles_Model.geometric_Model = multilayer->imperfections_Model.initial_Geometric_Model;
			}

			QVariant var;
			var.setValue( struct_Data );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
		++it;
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

	if(list_Editors.isEmpty() && global_Multilayer_Approach->runned_Regular_Aperiodic_Tables.isEmpty())
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

void Structure_Tree::lock_Tree()
{
	structure_Toolbar->toolbar->setDisabled(true);
	tree->blockSignals(true);
}

void Structure_Tree::unlock_Tree()
{
	structure_Toolbar->toolbar->setDisabled(false);
	tree->blockSignals(false);
}

Structure_Tree& Structure_Tree::operator =(const Structure_Tree& referent_Structure_Tree)
{
	tree->clear();
	for(int i=0; i<referent_Structure_Tree.tree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = referent_Structure_Tree.tree->topLevelItem(i)->clone(); // Real copy! The data are also copied here.
		Structure_Toolbar::change_IDs_Of_Subtree(item);
		tree->addTopLevelItem(item);
	}
	return *this;
}
