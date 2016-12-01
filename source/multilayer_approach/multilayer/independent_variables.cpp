#include "independent_variables.h"

Independent_Variables::Independent_Variables(QTreeWidget* struct_Tree_Copy, QWidget *parent) :
	QWidget(parent),
	struct_Tree_Copy(struct_Tree_Copy)
{
	create_Main_Layout();
	independent_Variables_List_Map = new QMap<QString, QListWidgetItem*>;
}

void Independent_Variables::clear_Parameters()
{
	// clear structure
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

	// clear independent list
	bool wave_Active = false;
	for(int i=independent_Variables_List->count()-1; i>=0; i--)
	{
		QListWidgetItem* temp_Item = independent_Variables_List->item(i);
		if((temp_Item->whatsThis() != whats_This_Measurement + QString(whats_This_Delimiter) + whats_This_Angle) && (temp_Item->whatsThis() != whats_This_Measurement + QString(whats_This_Delimiter) + whats_This_Wavelength))
		{
			delete temp_Item;
		} else
		if(temp_Item->whatsThis() == whats_This_Measurement + QString(whats_This_Delimiter) + whats_This_Wavelength)
		{
			if(temp_Item->data(Qt::UserRole).toBool()) wave_Active = true;

		} else
		if(temp_Item->whatsThis() == whats_This_Measurement + QString(whats_This_Delimiter) + whats_This_Angle)
		{
			if(!wave_Active)
			{
				temp_Item->setData(Qt::UserRole, true);

				QStringList item_Text_List = temp_Item->text().split(active, QString::SkipEmptyParts);
				temp_Item->setText(item_Text_List[0] + active);
			}
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
			if(structure_Item->whatsThis(DEFAULT_COLUMN) == whats_This_List[0])
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
		angle->setWhatsThis(QString(whats_This_Measurement) + whats_This_Delimiter + whats_This_Angle);
	independent_Variables_List->insertItem(0, angle);

	QListWidgetItem* wavelength = new QListWidgetItem;
	wavelength->setData(Qt::UserRole, false);	// adding "passive" status
	if(measurement.wavelength.independent.num_Points == 1)
		wavelength->setText(Global_Variables::wavelength_Energy_Name(wavelength_units) + " [" + QString::number(Global_Variables::wavelength_Energy(wavelength_units, measurement.wavelength.value)/wavelength_Coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " " + wavelength_units + "]");
	else
		wavelength->setText(Global_Variables::wavelength_Energy_Name(wavelength_units) + " [" + QString::number(measurement.wavelength.independent.num_Points) + " values: " +
				QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.min)/wavelength_Coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " - " +
				QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.max)/wavelength_Coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " " + wavelength_units + "]");
		wavelength->setWhatsThis(QString(whats_This_Measurement) + whats_This_Delimiter + whats_This_Wavelength);
	independent_Variables_List->insertItem(1, wavelength);

	connect(independent_Variables_List, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(edit_Independent_Variable(QListWidgetItem*)));
}

void Independent_Variables::create_Independent_Variables_Toolbar()
{
	QPixmap new_Variable	(icon_path + "new.bmp");
	QPixmap edit			(icon_path + "roi.bmp");
	QPixmap remove		(icon_path + "delete.bmp");

	independent_Variables_Toolbar = new QToolBar;

	independent_Variables_Toolbar->addAction(QIcon(new_Variable),	"Add Independent Variable");// 0
	independent_Variables_Toolbar->addAction(QIcon(edit),			"Edit");					// 1
	independent_Variables_Toolbar->addAction(QIcon(remove),			"Remove");					// 2

	independent_Variables_Toolbar->setIconSize(new_Variable.size());

	connect(independent_Variables_Toolbar->actions()[0], SIGNAL(triggered(bool)), this, SLOT(add_Independent_Variable(bool)));
	connect(independent_Variables_Toolbar->actions()[1], SIGNAL(triggered(bool)), this, SLOT(cause_Editing_Independent_Variable(bool)));
	connect(independent_Variables_Toolbar->actions()[2], SIGNAL(triggered(bool)), this, SLOT(remove_Independent_Variable(bool)));

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
		if(structure_Item->whatsThis(DEFAULT_COLUMN) == whats_This_List[0])
			break;
		++it;
	}

	Independent_Variables_Editor* editor = new Independent_Variables_Editor(structure_Item, item, independent_Variables_List);
		editor->setParent(this);
		editor->setModal(true);
		editor->setWindowFlags(Qt::Window);
		editor->show();

	connect(editor, SIGNAL(refresh()), this, SLOT(emit_Refresh()));
}

void Independent_Variables::remove_Independent_Variable(bool)
{
	// PARAMETER

	QListWidgetItem* item = independent_Variables_List->currentItem();
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
		QStringList whats_This_List_Type = whats_This_List[0].split(item_Type_Delimiter,QString::SkipEmptyParts);
		QVariant var;

		// item search
		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(struct_Tree_Copy);
		while (*it)
		{
			structure_Item = *it;
			if(structure_Item->whatsThis(DEFAULT_COLUMN) == whats_This_List[0])
				break;
			++it;
		}

	  // if ambient
	  if(whats_This_List_Type[0] == whats_This_Ambient)
	  {
		  Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		  /// optical constants
		  if(whats_This_List[1] == whats_This_Absolute_Density)
		  {
			  ambient.absolute_Density.independent.is_Independent = false;
		  }
		  if(whats_This_List[1] == whats_This_Relative_Density)
		  {
			  ambient.relative_Density.independent.is_Independent = false;
		  }
		  if(whats_This_List[1] == whats_This_Permittivity)
		  {
			  ambient.permittivity.independent.is_Independent = false;
		  }
		  if(whats_This_List[1] == whats_This_Absorption)
		  {
			  ambient.absorption.independent.is_Independent = false;
		  }
		  if(whats_This_List[1] == whats_This_Composition)
		  {
			  int index = QString(whats_This_List[2]).toInt();
			  ambient.composition[index].composition.independent.is_Independent = false;
		  }

		  var.setValue(ambient);
		  structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	  }

	  // if layer
	  if(whats_This_List_Type[0] == whats_This_Layer)
	  {
		  Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		  /// optical constants
		  if(whats_This_List[1] == whats_This_Absolute_Density)
		  {
			  layer.absolute_Density.independent.is_Independent = false;
		  }
		  if(whats_This_List[1] == whats_This_Relative_Density)
		  {
			  layer.relative_Density.independent.is_Independent = false;
		  }
		  if(whats_This_List[1] == whats_This_Permittivity)
		  {
			  layer.permittivity.independent.is_Independent = false;
		  }
		  if(whats_This_List[1] == whats_This_Absorption)
		  {
			  layer.absorption.independent.is_Independent = false;
		  }
		  if(whats_This_List[1] == whats_This_Composition)
		  {
			  int index = QString(whats_This_List[2]).toInt();
			  layer.composition[index].composition.independent.is_Independent = false;
		  }

		  /// thickness parameters
		  // layer thickness
		  if(whats_This_List[1] == whats_This_Thickness)
		  {
			  layer.thickness.independent.is_Independent = false;
		  }

		  /// interface parameters
		  // layer sigma
		  if(whats_This_List[1] == whats_This_Sigma)
		  {
			  layer.sigma.independent.is_Independent = false;
		  }
		  // layer interlayer composition (if enabled and >=2 elements)
		  if(whats_This_List[1] == whats_This_Interlayer_Composition)
		  {
			  int index = QString(whats_This_List.last()).toInt();
			  layer.interlayer_Composition[index].interlayer.independent.is_Independent = false;
		  }

		  var.setValue(layer);
		  structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	  }

	  // if multilayer
	  if(whats_This_List_Type[0] == whats_This_Multilayer)
	  {
		  Stack_Content stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();

		  // multilayer num_repetitions
		  if(whats_This_List[1] == whats_This_Num_Repetitions)
		  {
			  stack_Content.num_Repetition.is_Independent = false;
		  }
		  // multilayer period
		  if(whats_This_List[1] == whats_This_Period)
		  {
			  stack_Content.period.independent.is_Independent = false;
		  }
		  // multilayer gamma
		  if(whats_This_List[1] == whats_This_Gamma)
		  {
			  stack_Content.gamma.independent.is_Independent = false;
		  }

		  var.setValue(stack_Content);
		  structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	  }

	  // if substrate
	  if(whats_This_List_Type[0] == whats_This_Substrate)
	  {
		  Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		  /// optical constants

		  // substrate density
		  if(whats_This_List[1] == whats_This_Absolute_Density)
		  {
			  substrate.absolute_Density.independent.is_Independent = false;
		  }
		  if(whats_This_List[1] == whats_This_Relative_Density)
		  {
			  substrate.relative_Density.independent.is_Independent = false;
		  }
		  // substrate permittivity
		  if(whats_This_List[1] == whats_This_Permittivity)
		  {
			  substrate.permittivity.independent.is_Independent = false;
		  }
		  // substrate absorption
		  if(whats_This_List[1] == whats_This_Absorption)
		  {
			  substrate.absorption.independent.is_Independent = false;
		  }
		  // substrate composition
		  if(whats_This_List[1] == whats_This_Composition)
		  {
			  int index = QString(whats_This_List[2]).toInt();
			  substrate.composition[index].composition.independent.is_Independent = false;
		  }

		  /// interface parameters

		  // substrate sigma
		  if(whats_This_List[1] == whats_This_Sigma)
		  {
			  substrate.sigma.independent.is_Independent = false;
		  }
		  // substrate interlayer composition (if enabled and >=2 elements)
		  if(whats_This_List[1] == whats_This_Interlayer_Composition)
		  {
			  int index = QString(whats_This_List.last()).toInt();
			  substrate.interlayer_Composition[index].interlayer.independent.is_Independent = false;
		  }

		  var.setValue(substrate);
		  structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	  }

	  refresh_State(structure_Item);
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

void Independent_Variables::refresh_State(QTreeWidgetItem* structure_Item)
{
	// PARAMETER
	QVariant var;
	QString whats_This = structure_Item->whatsThis(DEFAULT_COLUMN);
	QStringList whats_This_List_Type = whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

	if(whats_This_List_Type[0] == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if( ambient.absolute_Density.independent.is_Independent == true ) {ambient.separate_Optical_Constants = TRIL_FALSE;} else
		if( ambient.relative_Density.independent.is_Independent == true ) {ambient.separate_Optical_Constants = TRIL_FALSE;} else
		if( ambient.permittivity.	 independent.is_Independent == true ) {ambient.separate_Optical_Constants = TRIL_TRUE;}  else
		if( ambient.absorption.		 independent.is_Independent == true ) {ambient.separate_Optical_Constants = TRIL_TRUE;}  else

		   {ambient.separate_Optical_Constants = TRIL_NOT_DEFINED;}

		var.setValue(ambient);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	if(whats_This_List_Type[0] == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if( layer.absolute_Density.independent.is_Independent == true ) {layer.separate_Optical_Constants = TRIL_FALSE;} else
		if( layer.relative_Density.independent.is_Independent == true ) {layer.separate_Optical_Constants = TRIL_FALSE;} else
		if( layer.permittivity.	   independent.is_Independent == true ) {layer.separate_Optical_Constants = TRIL_TRUE;}  else
		if( layer.absorption.	   independent.is_Independent == true ) {layer.separate_Optical_Constants = TRIL_TRUE;}  else

		   {layer.separate_Optical_Constants = TRIL_NOT_DEFINED;}

		var.setValue(layer);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	if(whats_This_List_Type[0] == whats_This_Multilayer)
	{
	}
	if(whats_This_List_Type[0] == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if( substrate.absolute_Density.independent.is_Independent == true ) {substrate.separate_Optical_Constants = TRIL_FALSE;} else
		if( substrate.relative_Density.independent.is_Independent == true ) {substrate.separate_Optical_Constants = TRIL_FALSE;} else
		if( substrate.permittivity.	   independent.is_Independent == true ) {substrate.separate_Optical_Constants = TRIL_TRUE;}  else
		if( substrate.absorption.	   independent.is_Independent == true ) {substrate.separate_Optical_Constants = TRIL_TRUE;}  else

		   {substrate.separate_Optical_Constants = TRIL_NOT_DEFINED;}

		var.setValue(substrate);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
}

void Independent_Variables::refresh_State_All()
{
	// refresh state for all
	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		refresh_State(*it);
		++it;
	}
}

void Independent_Variables::emit_Refresh()
{
emit refresh();
}
