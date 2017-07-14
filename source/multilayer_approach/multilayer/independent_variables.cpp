// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "independent_variables.h"

Independent_Variables::Independent_Variables(QTreeWidget* real_Struct_Tree, QWidget *parent) :
	QWidget(parent),
	real_Struct_Tree(real_Struct_Tree)
{
	create_Struct_Tree_Copy();
	create_Main_Layout();
	independent_Variables_List_Map = new QMap<QString, QListWidgetItem*>;
}

void Independent_Variables::create_Struct_Tree_Copy()
{
	// creates hidden copy of main structure for plotting

	struct_Tree_Copy = new QTreeWidget(this);
	struct_Tree_Copy->hide();					// can be used for degugging
	for(int i=0; i<real_Struct_Tree->topLevelItemCount(); i++)
	{
		struct_Tree_Copy->addTopLevelItem(real_Struct_Tree->topLevelItem(i)->clone());	// the data are also copied here (check?)
	}

	// add "measurement" item to the top
	QTreeWidgetItem* new_Measurement_Item = new QTreeWidgetItem;
	// set unique id to measurenent
	int id = rand()*RAND_SHIFT+rand();
	new_Measurement_Item->setStatusTip(DEFAULT_COLUMN, QString::number(id));

	Measurement measurement;
	QVariant var; var.setValue(measurement);
	new_Measurement_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	new_Measurement_Item->setWhatsThis(DEFAULT_COLUMN, whats_This_Measurement);
	struct_Tree_Copy->insertTopLevelItem(0, new_Measurement_Item);
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

	connect(independent_Variables_List, SIGNAL(itemSelectionChanged()), this, SLOT(if_Selected()));

	// add angle and wavelength to independent variables list
	Measurement measurement = struct_Tree_Copy->topLevelItem(0)->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
	QString status_Tip = struct_Tree_Copy->topLevelItem(0)->statusTip(DEFAULT_COLUMN);

	// units conversion
	double wavelength_Coeff   = wavelength_Coefficients_Map.value(wavelength_units);
	double angle_Coeff		  = angle_Coefficients_Map.	    value(angle_units);

	QListWidgetItem* angle = new QListWidgetItem;
	angle->setData(Qt::UserRole, true);	// adding "active" status
	if(measurement.probe_Angle.independent.num_Points == 1)
		angle->setText(measurement.angle_Type + " angle, " + Theta_Sym + " [" + QString::number(measurement.probe_Angle.value/angle_Coeff,thumbnail_double_format,thumbnail_angle_precision) + angle_units + "]");
	else
		angle->setText(measurement.angle_Type + " angle, " + Theta_Sym + " [" + QString::number(measurement.probe_Angle.independent.num_Points) + " values: " +
					QString::number(measurement.probe_Angle.independent.min/angle_Coeff,thumbnail_double_format,thumbnail_angle_precision) + " - " +
					QString::number(measurement.probe_Angle.independent.max/angle_Coeff,thumbnail_double_format,thumbnail_angle_precision) + angle_units + "]" + active);
	angle->setWhatsThis(status_Tip + whats_This_Delimiter + QString(whats_This_Measurement) + whats_This_Delimiter + whats_This_Angle);
	angle->setText(angle->text() + active);
	independent_Variables_List->insertItem(0, angle);

	QListWidgetItem* wavelength = new QListWidgetItem;
	wavelength->setData(Qt::UserRole, false);	// adding "passive" status
	if(measurement.wavelength.independent.num_Points == 1)
		wavelength->setText(Global_Variables::wavelength_Energy_Name(wavelength_units) + " [" + QString::number(Global_Variables::wavelength_Energy(wavelength_units, measurement.wavelength.value)/wavelength_Coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " " + wavelength_units + "]");
	else
		wavelength->setText(Global_Variables::wavelength_Energy_Name(wavelength_units) + " [" + QString::number(measurement.wavelength.independent.num_Points) + " values: " +
				QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.min)/wavelength_Coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " - " +
				QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.max)/wavelength_Coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " " + wavelength_units + "]");
	wavelength->setWhatsThis(status_Tip + whats_This_Delimiter + QString(whats_This_Measurement) + whats_This_Delimiter + whats_This_Wavelength);
	independent_Variables_List->insertItem(1, wavelength);

	connect(independent_Variables_List, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(edit_Independent_Variable(QListWidgetItem*)));
}

void Independent_Variables::create_Independent_Variables_Toolbar()
{
	QPixmap new_Variable	(icon_path + "new.bmp");
	QPixmap edit			(icon_path + "roi.bmp");
	QPixmap remove			(icon_path + "delete.bmp");

	independent_Variables_Toolbar = new QToolBar;

	independent_Variables_Toolbar->addAction(QIcon(new_Variable),	"Add Independent Variable");// 0
	independent_Variables_Toolbar->addAction(QIcon(edit),			"Edit");					// 1
	independent_Variables_Toolbar->addAction(QIcon(remove),			"Remove");					// 2

	independent_Variables_Toolbar->setIconSize(new_Variable.size());

	connect(independent_Variables_Toolbar->actions()[0], SIGNAL(triggered(bool)), this, SLOT(add_Independent_Variable(bool)));
	connect(independent_Variables_Toolbar->actions()[1], SIGNAL(triggered(bool)), this, SLOT(cause_Editing_Independent_Variable(bool)));
	connect(independent_Variables_Toolbar->actions()[2], SIGNAL(triggered(bool)), this, SLOT(remove_Current_Independent_Variable(bool)));

	if_Selected();
}

void Independent_Variables::add_Independent_Variable(bool)
{
	Variable_Selection* new_Variable = new Variable_Selection(struct_Tree_Copy, independent_Variables_List_Map, independent_Variables_List, Variable_Type::Independent);
		new_Variable->setParent(this);
		new_Variable->setModal(true);
		new_Variable->setWindowFlags(Qt::Window);
		new_Variable->show();

	connect(new_Variable, SIGNAL(closed()), this, SLOT(refresh_Independent_State_All()));
}

void Independent_Variables::cause_Editing_Independent_Variable(bool)
{
	edit_Independent_Variable(independent_Variables_List->currentItem());
}

void Independent_Variables::edit_Independent_Variable(QListWidgetItem* list_Item)
{
	// item search by unique number
	QString whats_This = list_Item->whatsThis();
	QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
	QTreeWidgetItem* structure_Item = NULL;
	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		structure_Item = *it;
		if(structure_Item->statusTip(DEFAULT_COLUMN) == whats_This_List[0])
		{
			break;
		}
		++it;
	}

	// editing itself
	Independent_Variables_Editor* editor = new Independent_Variables_Editor(structure_Item, list_Item, independent_Variables_List);
		editor->setParent(this);
		editor->setModal(true);
		editor->setWindowFlags(Qt::Window);
		editor->show();

	connect(editor, SIGNAL(refresh_Multilayer()), this, SLOT(refresh_All()));
}

void Independent_Variables::remove_Current_Independent_Variable(bool)
{
	remove_Independent_Variable(independent_Variables_List->currentItem());
}

void Independent_Variables::remove_Independent_Variable(QListWidgetItem* item)
{
	// PARAMETER

	if(item)
	{
		// if active then make active angle
		if(item->data(Qt::UserRole).toBool())
		{
			QListWidgetItem* angle_Item = independent_Variables_List->item(0);
			angle_Item->setData(Qt::UserRole, true);
			angle_Item->setText(angle_Item->text() + active);
		}

		// sorting
		QString whats_This = item->whatsThis();
		QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
		QStringList whats_This_List_Type = whats_This_List[1].split(item_Type_Delimiter,QString::SkipEmptyParts);
		QVariant var;

		// item search
		QTreeWidgetItem* copy_Structure_Item = NULL;
		QTreeWidgetItemIterator it(struct_Tree_Copy);
		while (*it)
		{
			copy_Structure_Item = *it;
			if(copy_Structure_Item->statusTip(DEFAULT_COLUMN) == whats_This_List[0])
				break;
			++it;
		}
		// item search in original tree
		QTreeWidgetItem* real_Structure_Item = NULL;
		QTreeWidgetItemIterator real_It(real_Struct_Tree);
		while (*real_It)
		{
			real_Structure_Item = *real_It;
			if(real_Structure_Item->statusTip(DEFAULT_COLUMN) == whats_This_List[0])
				break;
			++real_It;
		}

		// if ambient
		if(whats_This_List_Type[0] == whats_This_Ambient)
		{
			Ambient ambient      = copy_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
			Ambient real_Ambient = real_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

			/// optical constants
			if(whats_This_List[2] == whats_This_Absolute_Density)
			{
				ambient.absolute_Density.independent.is_Independent = false;
				ambient.absolute_Density.value = real_Ambient.absolute_Density.value;
			}
			if(whats_This_List[2] == whats_This_Relative_Density)
			{
				ambient.relative_Density.independent.is_Independent = false;
				ambient.relative_Density.value = real_Ambient.relative_Density.value;
			}
			if(whats_This_List[2] == whats_This_Permittivity)
			{
				ambient.permittivity.independent.is_Independent = false;
				ambient.permittivity.value = real_Ambient.permittivity.value;
			}
			if(whats_This_List[2] == whats_This_Absorption)
			{
				ambient.absorption.independent.is_Independent = false;
				ambient.absorption.value = real_Ambient.absorption.value;
			}
			if(whats_This_List[2] == whats_This_Composition)
			{
				int index = QString(whats_This_List[2]).toInt();
				ambient.composition[index].composition.independent.is_Independent = false;
				ambient.composition[index].composition.value = real_Ambient.composition[index].composition.value;
			}

			var.setValue(ambient);
			copy_Structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if layer
		if(whats_This_List_Type[0] == whats_This_Layer)
		{
			Layer layer      = copy_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
			Layer real_Layer = real_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

			/// optical constants
			if(whats_This_List[2] == whats_This_Absolute_Density)
			{
				layer.absolute_Density.independent.is_Independent = false;
				layer.absolute_Density.value = real_Layer.absolute_Density.value;
			}
			if(whats_This_List[2] == whats_This_Relative_Density)
			{
				layer.relative_Density.independent.is_Independent = false;
				layer.relative_Density.value = real_Layer.relative_Density.value;
			}
			if(whats_This_List[2] == whats_This_Permittivity)
			{
				layer.permittivity.independent.is_Independent = false;
				layer.permittivity.value = real_Layer.permittivity.value;
			}
			if(whats_This_List[2] == whats_This_Absorption)
			{
				layer.absorption.independent.is_Independent = false;
				layer.absorption.value = real_Layer.absorption.value;
			}
			if(whats_This_List[2] == whats_This_Composition)
			{
				int index = QString(whats_This_List[2]).toInt();
				layer.composition[index].composition.independent.is_Independent = false;
				layer.composition[index].composition.value = real_Layer.composition[index].composition.value;
			}

			/// thickness parameters
			// layer thickness
			if(whats_This_List[2] == whats_This_Thickness)
			{
				layer.thickness.independent.is_Independent = false;
				layer.thickness.value = real_Layer.thickness.value;
			}

			/// interface parameters
			// layer sigma
			if(whats_This_List[2] == whats_This_Sigma)
			{
				layer.sigma.independent.is_Independent = false;
				layer.sigma.value = real_Layer.sigma.value;
			}
			// layer interlayer composition (if enabled and >=2 elements)
			if(whats_This_List[2] == whats_This_Interlayer_Composition)
			{
				int index = QString(whats_This_List.last()).toInt();
				layer.interlayer_Composition[index].interlayer.independent.is_Independent = false;
				layer.interlayer_Composition[index].interlayer.value = real_Layer.interlayer_Composition[index].interlayer.value;
			}

			var.setValue(layer);
			copy_Structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if multilayer
		if(whats_This_List_Type[0] == whats_This_Multilayer)
		{
			Stack_Content stack_Content      = copy_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
			Stack_Content real_Stack_Content = real_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();

			// multilayer num_repetitions
			if(whats_This_List[2] == whats_This_Num_Repetitions)
			{
				stack_Content.num_Repetition.is_Independent = false;
				stack_Content.num_Repetition.value = real_Stack_Content.num_Repetition.value;
			}
			// multilayer period
			if(whats_This_List[2] == whats_This_Period)
			{
				stack_Content.period.independent.is_Independent = false;
				stack_Content.period.value = real_Stack_Content.period.value;
			}
			// multilayer gamma
			if(whats_This_List[2] == whats_This_Gamma)
			{
				stack_Content.gamma.independent.is_Independent = false;
				stack_Content.gamma.value = real_Stack_Content.gamma.value;
			}

			var.setValue(stack_Content);
			copy_Structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if substrate
		if(whats_This_List_Type[0] == whats_This_Substrate)
		{
			Substrate substrate      = copy_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
			Substrate real_Substrate = real_Structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

			/// optical constants

			// substrate density
			if(whats_This_List[2] == whats_This_Absolute_Density)
			{
				substrate.absolute_Density.independent.is_Independent = false;
				substrate.absolute_Density.value = real_Substrate.absolute_Density.value;
			}
			if(whats_This_List[2] == whats_This_Relative_Density)
			{
				substrate.relative_Density.independent.is_Independent = false;
				substrate.relative_Density.value = real_Substrate.relative_Density.value;
			}
			// substrate permittivity
			if(whats_This_List[2] == whats_This_Permittivity)
			{
				substrate.permittivity.independent.is_Independent = false;
				substrate.permittivity.value = real_Substrate.permittivity.value;
			}
			// substrate absorption
			if(whats_This_List[2] == whats_This_Absorption)
			{
				substrate.absorption.independent.is_Independent = false;
				substrate.absorption.value = real_Substrate.absorption.value;
			}
			// substrate composition
			if(whats_This_List[2] == whats_This_Composition)
			{
				int index = QString(whats_This_List[2]).toInt();
				substrate.composition[index].composition.independent.is_Independent = false;
				substrate.composition[index].composition.value = real_Substrate.composition[index].composition.value;
			}

			/// interface parameters

			// substrate sigma
			if(whats_This_List[2] == whats_This_Sigma)
			{
				substrate.sigma.independent.is_Independent = false;
				substrate.sigma.value = real_Substrate.sigma.value;
			}
			// substrate interlayer composition (if enabled and >=2 elements)
			if(whats_This_List[2] == whats_This_Interlayer_Composition)
			{
				int index = QString(whats_This_List.last()).toInt();
				substrate.interlayer_Composition[index].interlayer.independent.is_Independent = false;
				substrate.interlayer_Composition[index].interlayer.value = real_Substrate.interlayer_Composition[index].interlayer.value;
			}

			var.setValue(substrate);
			copy_Structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		Variable_Selection::refresh_Independent_State_Of_Struct_Tree_Copy_Item(copy_Structure_Item);
		independent_Variables_List_Map->remove(whats_This);
		delete item;
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

	// temporary copy
	QTreeWidget* temp_Tree = new QTreeWidget;
	for(int i=0; i<struct_Tree_Copy->topLevelItemCount(); i++)
	{
		temp_Tree->addTopLevelItem(struct_Tree_Copy->topLevelItem(i)->clone());
	};

	// clear
	clear_Structure_Copy();

	// reset hidden copy of main structure
	for(int i=0; i<real_Struct_Tree->topLevelItemCount(); i++)
	{
		struct_Tree_Copy->addTopLevelItem(real_Struct_Tree->topLevelItem(i)->clone());
	}

	// search for previously existing items
	QTreeWidgetItem* new_Copy_Item;
	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		new_Copy_Item = *it;

		QString whats_This = new_Copy_Item->whatsThis(DEFAULT_COLUMN);
		QStringList whats_This_List = whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);
		QVariant var;

		// iterate over copy of old tree
		QTreeWidgetItem* old_Copy_Item;
		QTreeWidgetItemIterator old_It(temp_Tree);
		while (*old_It)
		{
			old_Copy_Item = *old_It;

			if(new_Copy_Item->statusTip(DEFAULT_COLUMN) == old_Copy_Item->statusTip(DEFAULT_COLUMN))
			{
				// if measurement
				if(whats_This_List[0] == whats_This_Measurement)
				{
					Measurement measurement     = new_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
					Measurement old_Measurement = old_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();

					measurement = old_Measurement; //-V519

					var.setValue(measurement);
					new_Copy_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
				}
				// if ambient
				if(whats_This_List[0] == whats_This_Ambient)
				{
					Ambient ambient     = new_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
					Ambient old_Ambient = old_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

					/// optical
					ambient.absolute_Density.independent = old_Ambient.absolute_Density.independent;
					ambient.relative_Density.independent = old_Ambient.relative_Density.independent;
					ambient.permittivity.independent	 = old_Ambient.permittivity.independent;
					ambient.absorption.independent		 = old_Ambient.absorption.independent;

					for(int index=0; index<ambient.composition.size(); ++index)
					{
						ambient.composition[index].composition.independent.min = 0;
						ambient.composition[index].composition.independent.max = 1;
						ambient.composition[index].composition.independent.num_Points = 1;
					}

					var.setValue(ambient);
					new_Copy_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
				}
				// if layer
				if(whats_This_List[0] == whats_This_Layer)
				{
					Layer layer     = new_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
					Layer old_Layer = old_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

					/// optical
					layer.absolute_Density.independent	= old_Layer.absolute_Density.independent;
					layer.relative_Density.independent	= old_Layer.relative_Density.independent;
					layer.permittivity.independent		= old_Layer.permittivity.independent;
					layer.absorption.independent		= old_Layer.absorption.independent;

					for(int index=0; index<layer.composition.size(); ++index)
					{
						layer.composition[index].composition.independent.min = 0;
						layer.composition[index].composition.independent.max = 1;
						layer.composition[index].composition.independent.num_Points = 1;
					}

					/// thickness
					layer.thickness.independent = old_Layer.thickness.independent;

					/// interface parameters
					layer.sigma.independent = old_Layer.sigma.independent;

					for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
					{
						layer.interlayer_Composition[func_Index].interlayer.independent = old_Layer.interlayer_Composition[func_Index].interlayer.independent;
					}

					var.setValue(layer);
					new_Copy_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
				}
				// if multilayer
				if(whats_This_List[0] == whats_This_Multilayer)
				{
					Stack_Content stack_Content     = new_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
					Stack_Content old_Stack_Content = old_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();

					// renew value of num_Repetition, other things remain old
					int temp_Value = stack_Content.num_Repetition.value;
					stack_Content.num_Repetition     = old_Stack_Content.num_Repetition;
					stack_Content.num_Repetition.value = temp_Value;

					stack_Content.period.independent = old_Stack_Content.period.independent;
					stack_Content.gamma.independent  = old_Stack_Content.gamma.independent;

					var.setValue(stack_Content);
					new_Copy_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
				}
				// if substrate
				if(whats_This_List[0] == whats_This_Substrate)
				{
					Substrate substrate     = new_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
					Substrate old_Substrate = old_Copy_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

					/// optical
					substrate.absolute_Density.independent	= old_Substrate.absolute_Density.independent;
					substrate.relative_Density.independent	= old_Substrate.relative_Density.independent;
					substrate.permittivity.independent		= old_Substrate.permittivity.independent;
					substrate.absorption.independent		= old_Substrate.absorption.independent;

					for(int index=0; index<substrate.composition.size(); ++index)
					{
						substrate.composition[index].composition.independent.min = 0;
						substrate.composition[index].composition.independent.max = 1;
						substrate.composition[index].composition.independent.num_Points = 1;
					}

					/// interface parameters
					substrate.sigma.independent = old_Substrate.sigma.independent;

					for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
					{
						substrate.interlayer_Composition[func_Index].interlayer.independent = old_Substrate.interlayer_Composition[func_Index].interlayer.independent;
					}

					var.setValue(substrate);
					new_Copy_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
				}
			}
			++old_It;
		}
		++it;
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
		if(structure_Item->whatsThis(DEFAULT_COLUMN) != whats_This_Measurement)
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
	for(int i=independent_Variables_List->count()-1; i>=0; i--)
	{
		QListWidgetItem* temp_Item       = independent_Variables_List->item(i);

		QString whats_This = temp_Item->whatsThis();
		QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
		QStringList whats_This_List_Type = whats_This_List[1].split(item_Type_Delimiter,QString::SkipEmptyParts);

		// search for target item
		bool is_Found = false;
		QTreeWidgetItem* struct_Tree_Copy_item;
		QTreeWidgetItemIterator it(struct_Tree_Copy);
		while (*it)
		{
			struct_Tree_Copy_item = *it;
			if(struct_Tree_Copy_item->statusTip(DEFAULT_COLUMN) == whats_This_List[0])
			{
				is_Found = true;
				break;
			}
			++it;
		}

		// if found but deprecated
		if( is_Found && (whats_This_List[2] != whats_This_Composition))
		{
			if(whats_This_List_Type[0] == whats_This_Ambient)
			{
				Ambient ambient = struct_Tree_Copy_item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

				if(		((whats_This_List[2] == whats_This_Relative_Density) && (ambient.composed_Material))
				   ||	((whats_This_List[2] == whats_This_Absolute_Density) && (!ambient.composed_Material))   )
				{ is_Found=false; }

			} else
			if(whats_This_List_Type[0] == whats_This_Layer)
			{
				Layer layer = struct_Tree_Copy_item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

				if(		((whats_This_List[2] == whats_This_Relative_Density) && (layer.composed_Material))
				   ||	((whats_This_List[2] == whats_This_Absolute_Density) && (!layer.composed_Material))   )
				{ is_Found=false; }

			} else
			if(whats_This_List_Type[0] == whats_This_Substrate)
			{
				Substrate substrate = struct_Tree_Copy_item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

				if(		((whats_This_List[2] == whats_This_Relative_Density) && (substrate.composed_Material))
				   ||	((whats_This_List[2] == whats_This_Absolute_Density) && (!substrate.composed_Material))   )
				{ is_Found=false; }
			}
		}
		// if not found
		if( (!is_Found) || (whats_This_List[2] == whats_This_Composition))
		{
			remove_Independent_Variable(temp_Item);
		}
	}
}

void Independent_Variables::refresh_Text()
{
	for(int i=0; i<independent_Variables_List->count(); ++i)
	{
		QListWidgetItem* item = independent_Variables_List->item(i);

		// item search
		QString whats_This = item->whatsThis();
		QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

		QTreeWidgetItem* structure_Item = NULL;
		QTreeWidgetItemIterator it(struct_Tree_Copy);
		while (*it)
		{
			structure_Item = *it;
			if(structure_Item->statusTip(DEFAULT_COLUMN) == whats_This_List[0])
				break;
			++it;
		}

		Independent_Variables_Editor* editor = new Independent_Variables_Editor(structure_Item, item, independent_Variables_List);
			editor->close();
	}
}

void Independent_Variables::refresh_All()
{
	emit refresh_Multilayer();
}
