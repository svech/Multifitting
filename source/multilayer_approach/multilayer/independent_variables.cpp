// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "independent_variables.h"

Independent_Variables::Independent_Variables(QTreeWidget* struct_Tree_Copy, QTreeWidget* real_Struct_Tree, QWidget *parent) :
	QWidget(parent),
	struct_Tree_Copy(struct_Tree_Copy),
	real_Struct_Tree(real_Struct_Tree)
{
	create_Main_Layout();
	independent_Variables_List_Map = new QMap<QString, QListWidgetItem*>;
}

void Independent_Variables::clear_Structure()
{
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

void Independent_Variables::clear_Independent_List()
{
	for(int i=independent_Variables_List->count()-1; i>=0; i--)
	{
		QListWidgetItem* temp_Angle_Item = independent_Variables_List->item(0);
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
		if( is_Found || (whats_This_List[2] != whats_This_Composition))
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
			// if was active, set angle active
			if(temp_Item->data(Qt::UserRole).toBool())		temp_Angle_Item->setData(Qt::UserRole, true);
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

		QTreeWidgetItem* structure_Item;
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
	independent_Variables_List = new QListWidget;
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

	connect(new_Variable, SIGNAL(closed()), this, SLOT(refresh_State_All()));
}

void Independent_Variables::cause_Editing_Independent_Variable(bool)
{
	edit_Independent_Variable(independent_Variables_List->currentItem());
}

void Independent_Variables::edit_Independent_Variable(QListWidgetItem* item)
{
	// item search
	QString whats_This = item->whatsThis();
	QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	QTreeWidgetItem* structure_Item;
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

	Independent_Variables_Editor* editor = new Independent_Variables_Editor(structure_Item, item, independent_Variables_List);
		editor->setParent(this);
		editor->setModal(true);
		editor->setWindowFlags(Qt::Window);
		editor->show();

		connect(editor, SIGNAL(refresh()), this, SLOT(emit_Refresh()));
}

void Independent_Variables::remove_Current_Independent_Variable(bool)
{
	QListWidgetItem* item = independent_Variables_List->currentItem();
	remove_Independent_Variable(item);
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
			// check
			if(angle_Item->whatsThis() == whats_This_Measurement + QString(whats_This_Delimiter) + whats_This_Angle)
			{
				angle_Item->setData(Qt::UserRole, true);
				angle_Item->setText(angle_Item->text() + active);
			}
		}

		// sorting
		QString whats_This = item->whatsThis();
		QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
		QStringList whats_This_List_Type = whats_This_List[1].split(item_Type_Delimiter,QString::SkipEmptyParts);
		QVariant var;

		// item search
		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(struct_Tree_Copy);
		while (*it)
		{
			structure_Item = *it;
			if(structure_Item->statusTip(DEFAULT_COLUMN) == whats_This_List[0])
				break;
			++it;
		}
		// item search in original tree
		QTreeWidgetItem* real_Structure_Item;
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
			Ambient ambient      =      structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
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
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if layer
		if(whats_This_List_Type[0] == whats_This_Layer)
		{
			Layer layer      =      structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
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
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if multilayer
		if(whats_This_List_Type[0] == whats_This_Multilayer)
		{
			Stack_Content stack_Content      =      structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
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
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if substrate
		if(whats_This_List_Type[0] == whats_This_Substrate)
		{
			Substrate substrate      =      structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
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
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		Variable_Selection::refresh_State(structure_Item);
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
		if(independent_Variables_List->currentIndex().row()<PERMANENT_INDEPENDENT)
		{
			independent_Variables_Toolbar->actions()[2]->setDisabled(true);	 // remove
		} else
		{
			independent_Variables_Toolbar->actions()[2]->setDisabled(false); // remove
		}
	}
}

void Independent_Variables::refresh_State_All()
{
	// refresh state for all
	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		Variable_Selection::refresh_State(*it);
		++it;
	}
}

void Independent_Variables::emit_Refresh()
{
	emit refresh();
}

