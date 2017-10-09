// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "independent_variables.h"

Independent_Variables::Independent_Variables(QTreeWidget* real_Struct_Tree, QWidget *parent) :
	QWidget(parent),
	measurement(item_Type_Measurement),
	real_Struct_Tree(real_Struct_Tree)
{
	create_Struct_Tree_Copy();
	create_Main_Layout();
	independent_Variables_List_Map = new QMap<int, QListWidgetItem*>;
}

void Independent_Variables::create_Struct_Tree_Copy()
{
	// creates hidden copy of main structure for plotting
	struct_Tree_Copy = new QTreeWidget(this);
	struct_Tree_Copy->hide();					// can be used for degugging
	for(int i=0; i<real_Struct_Tree->topLevelItemCount(); i++)
	{
		struct_Tree_Copy->addTopLevelItem(real_Struct_Tree->topLevelItem(i)->clone());	// the data are also copied here
	}

	// add "measurement" item to the top
	measurement_Item = new QTreeWidgetItem;
	QVariant var; var.setValue(measurement);
	measurement_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	struct_Tree_Copy->insertTopLevelItem(0, measurement_Item);
}

void Independent_Variables::create_Main_Layout()
{
	QVBoxLayout* main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);

	independent_Frame_Layout = new QVBoxLayout;
		independent_Frame_Layout->setSpacing(0);

	independent_Frame = new QFrame;
		independent_Frame->setContentsMargins(-7,-3,-7,-10);
		independent_Frame->setLayout(independent_Frame_Layout);
	main_Layout->addWidget(independent_Frame);

	create_Independent_Variables_List();
		independent_Frame_Layout->addWidget(independent_Variables_List);

	create_Independent_Variables_Toolbar();
		independent_Frame_Layout->addWidget(independent_Variables_Toolbar);
}

void Independent_Variables::create_Independent_Variables_List()
{
	independent_Variables_List = new QListWidget(this);
		independent_Variables_List->setSizeAdjustPolicy(QListWidget::AdjustToContents);

	QSizePolicy sp_Retain = independent_Variables_List->sizePolicy();
	sp_Retain.setRetainSizeWhenHidden(true);
	independent_Variables_List->setSizePolicy(sp_Retain);

	connect(independent_Variables_List, &QListWidget::itemSelectionChanged, this, &Independent_Variables::if_Selected);

	// add angle to independent variables list
	{
		QListWidgetItem* angle_List_Item = new QListWidgetItem;

		Independent_Indicator angle_Indicator;
		angle_Indicator.item_Id				 = measurement.id;
		angle_Indicator.item_Type			 = measurement.item_Type;
		angle_Indicator.parameter_Id		 = measurement.probe_Angle.indicator.id;
		angle_Indicator.parameter_Whats_This = measurement.probe_Angle.indicator.whats_This;
		angle_Indicator.is_Active = true;					// adding "active" status

		QVariant var;
		var.setValue(angle_Indicator);
		angle_List_Item->setData(Qt::UserRole, var);
		independent_Variables_List->addItem(angle_List_Item);

		// refresh text
		Independent_Variables_Editor* editor = new Independent_Variables_Editor(measurement_Item, angle_List_Item, independent_Variables_List);
			editor->close();
	}

	// add wavelength to independent variables list
	{
		QListWidgetItem* wavelength_List_Item = new QListWidgetItem;

		Independent_Indicator wavelength_Indicator;
		wavelength_Indicator.item_Id			  = measurement.id;
		wavelength_Indicator.item_Type			  = measurement.item_Type;
		wavelength_Indicator.parameter_Id		  = measurement.wavelength.indicator.id;
		wavelength_Indicator.parameter_Whats_This = measurement.wavelength.indicator.whats_This;
		wavelength_Indicator.is_Active = false;

		QVariant var;
		var.setValue(wavelength_Indicator);
		wavelength_List_Item->setData(Qt::UserRole, var);
		independent_Variables_List->addItem(wavelength_List_Item);

		// refresh text
		Independent_Variables_Editor* editor = new Independent_Variables_Editor(measurement_Item, wavelength_List_Item, independent_Variables_List);
			editor->close();
	}

	connect(independent_Variables_List, &QListWidget::itemDoubleClicked, this, &Independent_Variables::edit_Independent_Variable);
}

void Independent_Variables::create_Independent_Variables_Toolbar()
{
	QPixmap new_Variable	(icon_path + "new.bmp");
	QPixmap edit			(icon_path + "roi.bmp");
	QPixmap remove			(icon_path + "delete.bmp");
	QPixmap function		(icon_path + "plot.bmp");

	independent_Variables_Toolbar = new QToolBar;

	independent_Variables_Toolbar->addAction(QIcon(new_Variable),	"Add Independent Variable");// 0
	independent_Variables_Toolbar->addAction(QIcon(edit),			"Edit");					// 1
	independent_Variables_Toolbar->addAction(QIcon(remove),			"Remove");					// 2
	independent_Variables_Toolbar->addAction(QIcon(function),		"Functions");				// 3

	independent_Variables_Toolbar->setIconSize(new_Variable.size());

	connect(independent_Variables_Toolbar->actions()[0], &QAction::triggered, this, &Independent_Variables::add_Independent_Variable);
	connect(independent_Variables_Toolbar->actions()[1], &QAction::triggered, this, &Independent_Variables::cause_Editing_Independent_Variable);
	connect(independent_Variables_Toolbar->actions()[2], &QAction::triggered, this, &Independent_Variables::remove_Current_Independent_Variable);
	connect(independent_Variables_Toolbar->actions()[3], &QAction::triggered, this, &Independent_Variables::choose_Independent_Calc_Function);

	if_Selected();
}

void Independent_Variables::add_Independent_Variable()
{
	Variable_Selection* new_Variable = new Variable_Selection(struct_Tree_Copy, independent_Variables_List_Map, independent_Variables_List);
		new_Variable->setParent(this);
		new_Variable->setModal(true);
		new_Variable->setWindowFlags(Qt::Window);
		new_Variable->show();

	connect(new_Variable, &Variable_Selection::closed, this, &Independent_Variables::refresh_Independent_State_All);
}

void Independent_Variables::cause_Editing_Independent_Variable()
{
	edit_Independent_Variable(independent_Variables_List->currentItem());
}

void Independent_Variables::edit_Independent_Variable(QListWidgetItem* list_Item)
{
	// item search by id
	Independent_Indicator indicator = list_Item->data(Qt::UserRole).value<Independent_Indicator>();

	QTreeWidgetItem* structure_Item = NULL;
	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		structure_Item = *it;
		if(structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>().id == indicator.item_Id)
		{
			break;
		}
		++it;
	}
	if(structure_Item == NULL)
	{
		qInfo() << "Independent_Variables::edit_Independent_Variable : can't find structure_Item in tree copy";
	}

	// editing itself
	Independent_Variables_Editor* editor = new Independent_Variables_Editor(structure_Item, list_Item, independent_Variables_List);
		editor->setParent(this);
		editor->setModal(true);
		editor->setWindowFlags(Qt::Window);
		editor->show();

	connect(editor, &Independent_Variables_Editor::refresh_Multilayer, this, [=]{emit refresh_Multilayer();});
	connect(editor, &Independent_Variables_Editor::refresh_Measurement, this, [=]{measurement = structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>();});
}

void Independent_Variables::remove_Current_Independent_Variable()
{
	remove_Independent_Variable(independent_Variables_List->currentItem());
}

void Independent_Variables::choose_Independent_Calc_Function()
{
	Independent_Calc_Function_Selection* new_Independent_Calc_Function_Selection = new Independent_Calc_Function_Selection(this,this);
		new_Independent_Calc_Function_Selection->setModal(true);
		new_Independent_Calc_Function_Selection->setWindowFlags(Qt::Window);
		new_Independent_Calc_Function_Selection->show();
}

void Independent_Variables::remove_Independent_Variable(QListWidgetItem* list_Item)
{
	// PARAMETER

	if(list_Item)
	{
		QVariant var;
		Independent_Indicator indicator = list_Item->data(Qt::UserRole).value<Independent_Indicator>();

		// if active then make active angle
		if(indicator.is_Active)
		{
			QListWidgetItem* angle_Item = independent_Variables_List->item(0);

			Independent_Indicator angle_Indicator = angle_Item->data(Qt::UserRole).value<Independent_Indicator>();
			angle_Indicator.is_Active = true;
			var.setValue(angle_Indicator);
			angle_Item->setData(Qt::UserRole, var);

			angle_Item->setText(angle_Item->text() + active);
		}

		// item search in copy
		QTreeWidgetItem* copy_Structure_Item = NULL;
		QTreeWidgetItemIterator it(struct_Tree_Copy);
		while (*it)
		{
			copy_Structure_Item = *it;
			if(copy_Structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>().id == indicator.item_Id)
				break;
			++it;
		}
		// item search in original tree
		QTreeWidgetItem* real_Structure_Item = NULL;
		QTreeWidgetItemIterator real_It(real_Struct_Tree);
		while (*real_It)
		{
			real_Structure_Item = *real_It;
			if(real_Structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>().id == indicator.item_Id)
				break;
			++real_It;
		}

		// change data to default

		Data copy = copy_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		Data real = real_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		int index = indicator.index;

		/// optical constants
		if(indicator.parameter_Whats_This == whats_This_Absolute_Density)				{copy.absolute_Density							= real.absolute_Density;							}
		if(indicator.parameter_Whats_This == whats_This_Relative_Density)				{copy.relative_Density							= real.relative_Density;							}
		if(indicator.parameter_Whats_This == whats_This_Permittivity)					{copy.permittivity								= real.permittivity;								}
		if(indicator.parameter_Whats_This == whats_This_Absorption)						{copy.absorption								= real.absorption;									}
		if(indicator.parameter_Whats_This == whats_This_Composition)					{copy.composition[index].composition			= real.composition[index].composition;				}
		/// thickness parameters
		if(indicator.parameter_Whats_This == whats_This_Thickness)						{copy.thickness									= real.thickness;									}
		if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Line_Value)		{copy.thickness_Drift.drift_Line_Value			= real.thickness_Drift.drift_Line_Value;			}
		if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Rand_Rms)		{copy.thickness_Drift.drift_Rand_Rms			= real.thickness_Drift.drift_Rand_Rms;				}
		if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	{copy.thickness_Drift.drift_Sine_Amplitude		= real.thickness_Drift.drift_Sine_Amplitude;		}
		if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Sine_Frequency)	{copy.thickness_Drift.drift_Sine_Frequency		= real.thickness_Drift.drift_Sine_Frequency;		}
		if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Sine_Phase)		{copy.thickness_Drift.drift_Sine_Phase			= real.thickness_Drift.drift_Sine_Phase;			}
		/// interface parameters
		if(indicator.parameter_Whats_This == whats_This_Sigma)							{copy.sigma										= real.sigma;										}
		if(indicator.parameter_Whats_This == whats_This_Interlayer_Composition)			{copy.interlayer_Composition[index].interlayer	= real.interlayer_Composition[index].interlayer;	}
		if(indicator.parameter_Whats_This == whats_This_Interlayer_My_Sigma)			{copy.interlayer_Composition[index].my_Sigma	= real.interlayer_Composition[index].my_Sigma;		}
		if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Line_Value)			{copy.sigma_Drift.drift_Line_Value				= real.sigma_Drift.drift_Line_Value;				}
		if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Rand_Rms)			{copy.sigma_Drift.drift_Rand_Rms				= real.sigma_Drift.drift_Rand_Rms;					}
		if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		{copy.sigma_Drift.drift_Sine_Amplitude			= real.sigma_Drift.drift_Sine_Amplitude;			}
		if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Sine_Frequency)		{copy.sigma_Drift.drift_Sine_Frequency			= real.sigma_Drift.drift_Sine_Frequency;			}
		if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Sine_Phase)			{copy.sigma_Drift.drift_Sine_Phase				= real.sigma_Drift.drift_Sine_Phase;				}
		/// stack parameters
		if(indicator.parameter_Whats_This == whats_This_Num_Repetitions)				{copy.num_Repetition							= real.num_Repetition;								}
		if(indicator.parameter_Whats_This == whats_This_Period)							{copy.period									= real.period;										}
		if(indicator.parameter_Whats_This == whats_This_Gamma)							{copy.gamma										= real.gamma;										}

		var.setValue(copy);
		copy_Structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		Variable_Selection::refresh_Independent_State_Of_Struct_Tree_Copy_Item(copy_Structure_Item);
		independent_Variables_List_Map->remove(indicator.parameter_Id);
		delete list_Item;
	}
}

void Independent_Variables::if_Selected()
{
	if(independent_Variables_List->selectedItems().isEmpty())
	{
		independent_Variables_Toolbar->actions()[1]->setDisabled(true);		 // edit
		independent_Variables_Toolbar->actions()[2]->setDisabled(true);		 // remove
	} else
	{
		independent_Variables_Toolbar->actions()[1]->setDisabled(false);	 // edit
		if(independent_Variables_List->currentIndex().row()<PERMANENT_INDEPENDENT)	// make angle and wavelength nonremovable
		{
			independent_Variables_Toolbar->actions()[2]->setDisabled(true);	 // remove
		} else
		{
			independent_Variables_Toolbar->actions()[2]->setDisabled(false); // remove
		}
	}
}

void Independent_Variables::reset_Independent_Variables_Structure()
{
	// PARAMETER

	/// -----------------------------------------------------

	// temporary copy of copy
	QTreeWidget* temp_Tree = new QTreeWidget;
	for(int i=0; i<struct_Tree_Copy->topLevelItemCount(); i++)
	{
		temp_Tree->addTopLevelItem(struct_Tree_Copy->topLevelItem(i)->clone());
	};

	// clear
	clear_Structure_Copy();

	// reset hidden copy of main structure. Measurement
	for(int i=0; i<real_Struct_Tree->topLevelItemCount(); i++)
	{
		struct_Tree_Copy->addTopLevelItem(real_Struct_Tree->topLevelItem(i)->clone());
	}

	// search for previously existing items
	QVariant var;
	QTreeWidgetItem* new_Copy_Item = NULL;
	QTreeWidgetItemIterator new_It(struct_Tree_Copy);
	while (*new_It)
	{
		new_Copy_Item = *new_It;

		Data new_Copy = new_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		// iterate over temp copy of old tree
		QTreeWidgetItem* old_Copy_Item;
		QTreeWidgetItemIterator old_It(temp_Tree);
		while (*old_It)
		{
			old_Copy_Item = *old_It;

			Data old_Copy = old_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

			// if corresponding items, copy all
			if(new_Copy.id == old_Copy.id)
			{
				new_Copy.absolute_Density.independent = old_Copy.absolute_Density.independent;
				new_Copy.relative_Density.independent = old_Copy.relative_Density.independent;
				new_Copy.permittivity.independent	  = old_Copy.permittivity.independent;
				new_Copy.absorption.independent		  = old_Copy.absorption.independent;
				new_Copy.thickness.independent		  = old_Copy.thickness.independent;

				new_Copy.thickness_Drift.drift_Line_Value.independent		= old_Copy.thickness_Drift.drift_Line_Value.independent;
				new_Copy.thickness_Drift.drift_Rand_Rms.independent			= old_Copy.thickness_Drift.drift_Rand_Rms.independent;
				new_Copy.thickness_Drift.drift_Sine_Amplitude.independent	= old_Copy.thickness_Drift.drift_Sine_Amplitude.independent;
				new_Copy.thickness_Drift.drift_Sine_Frequency.independent	= old_Copy.thickness_Drift.drift_Sine_Frequency.independent;
				new_Copy.thickness_Drift.drift_Sine_Phase.independent		= old_Copy.thickness_Drift.drift_Sine_Phase.independent;

				new_Copy.sigma.independent			  = old_Copy.sigma.independent;

				// composition is not inherited
//				for(int i=0; i<nmin(new_Copy.composition.size(),old_Copy.composition.size()); ++i)
//				{
//					new_Copy.composition[i].composition.independent = old_Copy.composition[i].composition.independent;
//				}
				for(int i=0; i<new_Copy.interlayer_Composition.size(); ++i)
				{
					new_Copy.interlayer_Composition[i].interlayer.independent = old_Copy.interlayer_Composition[i].interlayer.independent;
					new_Copy.interlayer_Composition[i].my_Sigma.independent	  = old_Copy.interlayer_Composition[i].my_Sigma.independent;
				}

				new_Copy.sigma_Drift.drift_Line_Value.independent		= old_Copy.sigma_Drift.drift_Line_Value.independent;
				new_Copy.sigma_Drift.drift_Rand_Rms.independent			= old_Copy.sigma_Drift.drift_Rand_Rms.independent;
				new_Copy.sigma_Drift.drift_Sine_Amplitude.independent	= old_Copy.sigma_Drift.drift_Sine_Amplitude.independent;
				new_Copy.sigma_Drift.drift_Sine_Frequency.independent	= old_Copy.sigma_Drift.drift_Sine_Frequency.independent;
				new_Copy.sigma_Drift.drift_Sine_Phase.independent		= old_Copy.sigma_Drift.drift_Sine_Phase.independent;

				// renew value of num_Repetition, other things remain old
				int temp_Value = new_Copy.num_Repetition.value;
				new_Copy.num_Repetition       = old_Copy.num_Repetition;
				new_Copy.num_Repetition.value = temp_Value;

				new_Copy.period.independent	= old_Copy.period.independent;
				new_Copy.gamma.independent	= old_Copy.gamma.independent;
				var.setValue(new_Copy);
				new_Copy_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			}
			++old_It;
		}
		++new_It;
	}

	delete temp_Tree;

	/// -----------------------------------------------------

	// clear irrelevant variables
	clear_Unused_Independent_List();
	refresh_Text();
}

void Independent_Variables::refresh_Independent_State_All()
{
	// refresh state for all struct_Tree_Copy items
	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		Variable_Selection::refresh_Independent_State_Of_Struct_Tree_Copy_Item(*it);
		++it;
	}
}

void Independent_Variables::clear_Structure_Copy()
{
	// delete ALL items from struct_Tree_Copy except Measurement

	QList<QTreeWidgetItem*> struct_In_List;
	QTreeWidgetItem* structure_Item;
	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		structure_Item = *it;
		if(structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>().item_Type != item_Type_Measurement)
		{
			struct_In_List.append(structure_Item);
		}
		++it;
	}
	for(int i=struct_In_List.size()-1; i>=0; i--)
	{
		delete struct_In_List[i];
	}
}

void Independent_Variables::clear_Unused_Independent_List()
{
	// PARAMETER

	for(int i=independent_Variables_List->count()-1; i>=0; i--)
	{
		QListWidgetItem* temp_List_Item = independent_Variables_List->item(i);
		Independent_Indicator indicator = temp_List_Item->data(Qt::UserRole).value<Independent_Indicator>();

		// search for target item
		bool is_Found = false;
		QTreeWidgetItem* struct_Tree_Copy_item;
		QTreeWidgetItemIterator it(struct_Tree_Copy);
		while (*it)
		{
			struct_Tree_Copy_item = *it;
			if(indicator.item_Id == struct_Tree_Copy_item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().id)
			{
				is_Found = true;
				break;
			}
			++it;
		}

		// if found but deprecated
		if( is_Found/* && (whats_This_List[2] != whats_This_Composition)*/)
		{
			Data data = struct_Tree_Copy_item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			if(	(indicator.parameter_Whats_This == whats_This_Relative_Density &&  data.composed_Material) ||
				(indicator.parameter_Whats_This == whats_This_Absolute_Density && !data.composed_Material) ||

				(!(struct_Tree_Copy_item->parent()) && (
														 indicator.parameter_Whats_This == whats_This_Thickness_Drift_Line_Value	||
														 indicator.parameter_Whats_This == whats_This_Thickness_Drift_Rand_Rms		||
														 indicator.parameter_Whats_This == whats_This_Thickness_Drift_Sine_Amplitude||
														 indicator.parameter_Whats_This == whats_This_Thickness_Drift_Sine_Frequency||
														 indicator.parameter_Whats_This == whats_This_Thickness_Drift_Sine_Phase	||

														 indicator.parameter_Whats_This == whats_This_Sigma_Drift_Line_Value		||
														 indicator.parameter_Whats_This == whats_This_Sigma_Drift_Rand_Rms			||
														 indicator.parameter_Whats_This == whats_This_Sigma_Drift_Sine_Amplitude	||
														 indicator.parameter_Whats_This == whats_This_Sigma_Drift_Sine_Frequency	||
														 indicator.parameter_Whats_This == whats_This_Sigma_Drift_Sine_Phase
														)
				 ))
			{
				is_Found=false;
			}
		}

		// if not found
		if( !is_Found /*|| (whats_This_List[2] == whats_This_Composition)*/)
		{
			remove_Independent_Variable(temp_List_Item);
		}
	}
}

void Independent_Variables::refresh_Text()
{
	for(int i=0; i<independent_Variables_List->count(); ++i)
	{
		QListWidgetItem* temp_List_Item = independent_Variables_List->item(i);
		QTreeWidgetItem* struct_Tree_Copy_item = NULL;
		QTreeWidgetItemIterator it(struct_Tree_Copy);
		while (*it)
		{
			struct_Tree_Copy_item = *it;
			if(temp_List_Item->data(Qt::UserRole).value<Independent_Indicator>().item_Id == struct_Tree_Copy_item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().id)
				break;
			++it;
		}

		Independent_Variables_Editor* editor = new Independent_Variables_Editor(struct_Tree_Copy_item, temp_List_Item, independent_Variables_List);
			editor->close();
	}
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Independent_Variables* independent_Variables )
{
	return stream	<< independent_Variables->measurement << independent_Variables->calc_Functions;
}
QDataStream& operator >>(QDataStream& stream,		 Independent_Variables* independent_Variables )
{
	return stream	>> independent_Variables->measurement >> independent_Variables->calc_Functions;
}
