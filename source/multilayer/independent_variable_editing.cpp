#include "independent_variable_editing.h"

Independent_Variable_Editing::Independent_Variable_Editing(QTreeWidgetItem* structure_Item, QListWidgetItem* item):
	structure_Item(structure_Item),
	item(item),
	gui_Settings(Gui_Settings_Path, QSettings::IniFormat),
	default_Values(Default_Values_Path, QSettings::IniFormat)
{
	create_main_Layout();

	setWindowTitle("Independent Variable");
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Independent_Variable_Editing::create_main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,0,4,0);

	create_Menu();
		main_Layout->setMenuBar(menu_Bar);

	create_Standard_Interface();

}

void Independent_Variable_Editing::create_Menu()
{
	// File
	file_Menu = new QMenu("File", this);

		QAction* act_Quit = new QAction("Done", this);
			connect(act_Quit, SIGNAL(triggered()), this, SLOT(close()));
			file_Menu->addAction(act_Quit);

	// add menu to menu bar
	menu_Bar = new QMenuBar(this);
	menu_Bar->addMenu(file_Menu);
}

void Independent_Variable_Editing::create_Standard_Interface()
{
	QSizePolicy sp_retain;
	sp_retain.setRetainSizeWhenHidden(true);
	{
		group_Box = new QGroupBox;
			group_Box->setObjectName("group_Box");
			group_Box->setStyleSheet("QGroupBox#group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
									 "QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
		main_Layout->addWidget(group_Box);

		QVBoxLayout* group_Box_Layout = new QVBoxLayout(group_Box);
			group_Box_Layout->setAlignment(Qt::AlignLeft);

		QHBoxLayout* layout = new QHBoxLayout;
			layout->setAlignment(Qt::AlignLeft);
		group_Box_Layout->addLayout(layout);

		num_Points = new QLineEdit;
			num_Points->setFixedWidth(30);
			num_Points->setProperty(min_Size, 30);
			num_Points->setValidator(new QIntValidator(1, MAX_INTEGER));
		layout->addWidget(num_Points);

		from_Label = new QLabel(from_Many);
		layout->addWidget(from_Label);

		min = new QLineEdit;
			min->setFixedWidth(40);
			min->setProperty(min_Size, 40);
			min->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
		layout->addWidget(min);

		to_Label = new QLabel(to);
			to_Label->setSizePolicy(sp_retain);
		layout->addWidget(to_Label);

		max = new QLineEdit;
			max->setFixedWidth(40);
			max->setProperty(min_Size, 40);
			max->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			max->setSizePolicy(sp_retain);
		layout->addWidget(max);

		units_Label = new QLabel(units);
			units_Label->setSizePolicy(sp_retain);
		layout->addWidget(units_Label);

		step_Label = new QLabel(step);
			step_Label->setSizePolicy(sp_retain);
		layout->addWidget(step_Label);

		step_Edit = new QLineEdit;
			step_Edit->setFixedWidth(40);
			step_Edit->setProperty(min_Size, 40);
			step_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			step_Edit->setSizePolicy(sp_retain);
		layout->addWidget(step_Edit);

		step_Units_Label = new QLabel(units);
			step_Units_Label->setSizePolicy(sp_retain);
		layout->addWidget(step_Units_Label);

		connect(num_Points, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(min,		SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(max,		SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(step_Edit,	SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		connect(num_Points, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));
		connect(min,		SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));
		connect(max,		SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));
	}
	{
		done_Button = new QPushButton("Done");
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
			done_Button->setDefault(true);
		main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

		connect(done_Button,	   SIGNAL(clicked()), this, SLOT(close()));
	}
	refresh_Show_Data(show_Data);
}

void Independent_Variable_Editing::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
}

void Independent_Variable_Editing::resize_Line_Edit(QString text)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QFontMetrics fm = line_Edit->fontMetrics();
	int width = fm.width(text)+7;
	if(width>line_Edit->property(min_Size).toInt())
	{
		line_Edit->setFixedWidth(width);
		adjustSize();
	}
}

void Independent_Variable_Editing::show_Hide_Elements(int points, bool show)
{
	if(show)
	{
		num_Points->textEdited(num_Points->text());
		min->textEdited(min->text());
		max->textEdited(max->text());
		step_Edit->textEdited(step_Edit->text());
	}

	if(points == 1)
	{
		max->hide();
		units_Label->hide();
		step_Edit->hide();
		step_Label->hide();
		step_Units_Label->hide();

		from_Label->setText(from_One);
		to_Label->setText(units);
	}
	else
	{
		max->show();
		units_Label->show();
		step_Edit->show();
		step_Label->show();
		step_Units_Label->show();

		from_Label->setText(from_Many);
		to_Label->setText(to);
		units_Label->setText(units+" ;");
	}
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Independent_Variable_Editing::refresh_Data(QString)
{
	refresh_Show_Data();
}

void Independent_Variable_Editing::refresh_Show_Data(bool show)
{
	default_Values.beginGroup( Structure_Values_Representation );
		int default_density_precision = default_Values.value( "default_density_precision", 0 ).toInt();
	default_Values.endGroup();

	QString whats_This = item->whatsThis();
	QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
	QStringList whats_This_List_Type = whats_This_List[0].split(item_Type_Delimiter,QString::SkipEmptyParts);
	QVariant var;

	group_Box->setTitle(whats_This);

	// if ambient
	if(whats_This_List_Type[0] == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(default_Column, Qt::UserRole).value<Ambient>();

		/// optical constants

		// ambient density
		if(whats_This_List[1] == whats_This_Density)
		{
			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.density.independent.num_Points));
				min->setText(		QString::number(ambient.density.independent.min,'f',default_density_precision));
				max->setText(		QString::number(ambient.density.independent.max,'f',default_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()==0)	num_Points->setText("1");
				if(min->text().toDouble()>=90)		min->setText("90");
				if(max->text().toDouble()>=90)		max->setText("90");

				ambient.density.independent.num_Points = num_Points->text().toInt();
				ambient.density.independent.min = min->text().toDouble();
				ambient.density.independent.max = max->text().toDouble();

				if(ambient.density.independent.num_Points == 1)
					item->setText(ambient.material + " (ambient)" + whats_This_List[1] + ", " + Rho_Sym + " [" + QString::number(ambient.density.value,'f',default_density_precision) + " g/cm" + Cube_Sym + "]");
				else
					item->setText(ambient.material + " (ambient)" + whats_This_List[1] + ", " + Rho_Sym + " [" + QString::number(ambient.density.independent.num_Points) + " values: " +
								  QString::number(ambient.density.independent.min,'f',default_density_precision) + " - " +
								  QString::number(ambient.density.independent.max,'f',default_density_precision) + " g/cm" + Cube_Sym + "]");
			}
			step_Edit->setText(QString::number((ambient.density.independent.max-ambient.density.independent.min)/ambient.density.independent.num_Points/*,'g',default_step_size_precision*/));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(ambient.density.independent.num_Points, show);
		}
		// ambient permittivity
		if(whats_This_List[1] == whats_This_Permittivity)
		{
//			ambient.permittivity.independent.is_Independent = true;
//			ambient.permittivity.independent.num_Points = 1;
//			ambient.permittivity.independent.min = ambient.permittivity.value;
//			ambient.permittivity.independent.max = ambient.permittivity.value;

//			item->setText(item->text() + " [" + QString::number(ambient.permittivity.value,'f',default_density_precision) + " % of nominal]");
		}
		// ambient absorption
		if(whats_This_List[1] == whats_This_Absorption)
		{
//			ambient.absorption.independent.is_Independent = true;
//			ambient.absorption.independent.num_Points = 1;
//			ambient.absorption.independent.min = ambient.absorption.value;
//			ambient.absorption.independent.max = ambient.absorption.value;

//			item->setText(item->text() + " [" + QString::number(ambient.absorption.value,'f',default_density_precision) + " % of nominal]");
		}
		//ambient composition
		if(whats_This_List[1] == whats_This_Composition)
		{
//			int index = QString(whats_This_List[2]).toInt();
//			ambient.composition[index].composition.independent.is_Independent = true;
//			ambient.composition[index].composition.independent.num_Points = 1;
//			ambient.composition[index].composition.independent.min = ambient.composition[index].composition.value;
//			ambient.composition[index].composition.independent.max = ambient.composition[index].composition.value;

//			item->setText(item->text() + " [" + QString::number(ambient.composition[index].composition.value,'f',default_density_precision) + "]");
		}

		var.setValue(ambient);
		structure_Item->setData(default_Column, Qt::UserRole, var);
	}

	// if layer
	if(whats_This_List_Type[0] == whats_This_Layer)
	{
		Layer layer = structure_Item->data(default_Column, Qt::UserRole).value<Layer>();

		/// optical constants

		// layer density
		if(whats_This_List[1] == whats_This_Density)
		{
//			layer.density.independent.is_Independent = true;
//			layer.density.independent.num_Points = 1;
//			layer.density.independent.min = layer.density.value;
//			layer.density.independent.max = layer.density.value;

//			item->setText(item->text() + " [" + QString::number(layer.density.value,'f',default_density_precision) + " g/cm" + Cube_Sym + "]");
		}
		// layer permittivity
		if(whats_This_List[1] == whats_This_Permittivity)
		{
//			layer.permittivity.independent.is_Independent = true;
//			layer.permittivity.independent.num_Points = 1;
//			layer.permittivity.independent.min = layer.permittivity.value;
//			layer.permittivity.independent.max = layer.permittivity.value;

//			item->setText(item->text() + " [" + QString::number(layer.permittivity.value,'f',default_density_precision) + " % of nominal]");
		}
		// layer absorption
		if(whats_This_List[1] == whats_This_Absorption)
		{
//			layer.absorption.independent.is_Independent = true;
//			layer.absorption.independent.num_Points = 1;
//			layer.absorption.independent.min = layer.absorption.value;
//			layer.absorption.independent.max = layer.absorption.value;

//			item->setText(item->text() + " [" + QString::number(layer.absorption.value,'f',default_density_precision) + " % of nominal]");
		}
		// layer composition
		if(whats_This_List[1] == whats_This_Composition)
		{
//			int index = QString(whats_This_List[2]).toInt();
//			layer.composition[index].composition.independent.is_Independent = true;
//			layer.composition[index].composition.independent.num_Points = 1;
//			layer.composition[index].composition.independent.min = layer.composition[index].composition.value;
//			layer.composition[index].composition.independent.max = layer.composition[index].composition.value;

//			item->setText(item->text() + " [" + QString::number(layer.composition[index].composition.value,'f',default_density_precision) + "]");
		}

		/// thickness parameters

		// layer thickness
		if(whats_This_List[1] == whats_This_Thickness)
		{
//			layer.thickness.independent.is_Independent = true;
//			layer.thickness.independent.num_Points = 1;
//			layer.thickness.independent.min = layer.thickness.value;
//			layer.thickness.independent.max = layer.thickness.value;

//			item->setText(item->text() + " [" + QString::number(layer.thickness.value,'f',default_density_precision) + " " + Angstrom_Sym + "]");
		}

		/// interface parameters

		// layer sigma
		if(whats_This_List[1] == whats_This_Sigma)
		{
//			layer.sigma.independent.is_Independent = true;
//			layer.sigma.independent.num_Points = 1;
//			layer.sigma.independent.min = layer.sigma.value;
//			layer.sigma.independent.max = layer.sigma.value;

//			item->setText(item->text() + " [" + QString::number(layer.sigma.value,'f',default_density_precision) + " " + Angstrom_Sym + "]");
		}
		// layer interlayer composition (if enabled and >=2 elements)
		if(whats_This_List[1] == whats_This_Interlayer_Composition)
		{
//			int index = QString(whats_This_List.last()).toInt();
//			layer.interlayer_Composition[index].interlayer.independent.is_Independent = true;
//			layer.interlayer_Composition[index].interlayer.independent.num_Points = 1;
//			layer.interlayer_Composition[index].interlayer.independent.min = layer.interlayer_Composition[index].interlayer.value;
//			layer.interlayer_Composition[index].interlayer.independent.max = layer.interlayer_Composition[index].interlayer.value;

//			item->setText(item->text() + " [" + QString::number(layer.interlayer_Composition[index].interlayer.value,'f',default_density_precision) + "]");
		}

		var.setValue(layer);
		structure_Item->setData(default_Column, Qt::UserRole, var);
	}

	// if multilayer
	if(whats_This_List_Type[0] == whats_This_Multilayer)
	{
		Stack_Content stack_Content = structure_Item->data(default_Column, Qt::UserRole).value<Stack_Content>();

		// multilayer num_repetitions
		if(whats_This_List[1] == whats_This_Num_Repetitions)
		{
//			stack_Content.num_Repetition.is_Independent = true;
//			stack_Content.num_Repetition.num_steps = 0;
//			stack_Content.num_Repetition.start = stack_Content.num_Repetition.value;
//			stack_Content.num_Repetition.step = 1;

//			item->setText(item->text() + " [" + QString::number(stack_Content.num_Repetition.value) + "]");
		}
		// multilayer period
		if(whats_This_List[1] == whats_This_Period)
		{
//			stack_Content.period.independent.is_Independent = true;
//			stack_Content.period.independent.num_Points = 1;
//			stack_Content.period.independent.min = stack_Content.period.value;
//			stack_Content.period.independent.max = stack_Content.period.value;

//			item->setText(item->text() + " [" + QString::number(stack_Content.period.value,'f',default_density_precision) + " " + Angstrom_Sym + "]");
		}
		// multilayer gamma
		if(whats_This_List[1] == whats_This_Gamma)
		{
//			stack_Content.gamma.independent.is_Independent = true;
//			stack_Content.gamma.independent.num_Points = 1;
//			stack_Content.gamma.independent.min = stack_Content.gamma.value;
//			stack_Content.gamma.independent.max = stack_Content.gamma.value;

//			item->setText(item->text() + " [" + QString::number(stack_Content.gamma.value,'f',default_density_precision) + "]");
		}

		var.setValue(stack_Content);
		structure_Item->setData(default_Column, Qt::UserRole, var);
	}

	// if substrate
	if(whats_This_List_Type[0] == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(default_Column, Qt::UserRole).value<Substrate>();

		/// optical constants

		// substrate density
		if(whats_This_List[1] == whats_This_Density)
		{
//			substrate.density.independent.is_Independent = true;
//			substrate.density.independent.num_Points = 1;
//			substrate.density.independent.min = substrate.density.value;
//			substrate.density.independent.max = substrate.density.value;

//			item->setText(item->text() + " [" + QString::number(substrate.density.value,'f',default_density_precision) + " g/cm" + Cube_Sym + "]");
		}
		// substrate permittivity
		if(whats_This_List[1] == whats_This_Permittivity)
		{
//			substrate.permittivity.independent.is_Independent = true;
//			substrate.permittivity.independent.num_Points = 1;
//			substrate.permittivity.independent.min = substrate.permittivity.value;
//			substrate.permittivity.independent.max = substrate.permittivity.value;

//			item->setText(item->text() + " [" + QString::number(substrate.permittivity.value,'f',default_density_precision) + " % of nominal]");
		}
		// substrate absorption
		if(whats_This_List[1] == whats_This_Absorption)
		{
//			substrate.absorption.independent.is_Independent = true;
//			substrate.absorption.independent.num_Points = 1;
//			substrate.absorption.independent.min = substrate.absorption.value;
//			substrate.absorption.independent.max = substrate.absorption.value;

//			item->setText(item->text() + " [" + QString::number(substrate.absorption.value,'f',default_density_precision) + " % of nominal]");
		}
		// substrate composition
		if(whats_This_List[1] == whats_This_Composition)
		{
//			int index = QString(whats_This_List[2]).toInt();
//			substrate.composition[index].composition.independent.is_Independent = true;
//			substrate.composition[index].composition.independent.num_Points = 1;
//			substrate.composition[index].composition.independent.min = substrate.composition[index].composition.value;
//			substrate.composition[index].composition.independent.max = substrate.composition[index].composition.value;

//			item->setText(item->text() + " [" + QString::number(substrate.composition[index].composition.value,'f',default_density_precision) + "]");
		}

		/// interface parameters

		// substrate sigma
		if(whats_This_List[1] == whats_This_Sigma)
		{
//			substrate.sigma.independent.is_Independent = true;
//			substrate.sigma.independent.num_Points = 1;
//			substrate.sigma.independent.min = substrate.sigma.value;
//			substrate.sigma.independent.max = substrate.sigma.value;

//			item->setText(item->text() + " [" + QString::number(substrate.sigma.value,'f',default_density_precision) + " " + Angstrom_Sym + "]");
		}
		// substrate interlayer composition (if enabled and >=2 elements)
		if(whats_This_List[1] == whats_This_Interlayer_Composition)
		{
//			int index = QString(whats_This_List.last()).toInt();
//			substrate.interlayer_Composition[index].interlayer.independent.is_Independent = true;
//			substrate.interlayer_Composition[index].interlayer.independent.num_Points = 1;
//			substrate.interlayer_Composition[index].interlayer.independent.min = substrate.interlayer_Composition[index].interlayer.value;
//			substrate.interlayer_Composition[index].interlayer.independent.max = substrate.interlayer_Composition[index].interlayer.value;

//			item->setText(item->text() + " [" + QString::number(substrate.interlayer_Composition[index].interlayer.value,'f',default_density_precision) + "]");
		}

		var.setValue(substrate);
		structure_Item->setData(default_Column, Qt::UserRole, var);
	}

//	close();
}
