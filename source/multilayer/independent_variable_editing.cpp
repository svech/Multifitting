#include "independent_variable_editing.h"

Independent_Variable_Editing::Independent_Variable_Editing(QTreeWidgetItem* structure_Item, QListWidgetItem* item):
	structure_Item(structure_Item),
	item(item),
	gui_Settings(Gui_Settings_Path, QSettings::IniFormat)
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
		main_Layout->addWidget(group_Box);

	// if angle
	if(item->whatsThis() == whats_This_Measurement + whats_This_Delimiter + whats_This_Angle)		create_Angle_Interface();
	// if wavelength
	if(item->whatsThis() == whats_This_Measurement + whats_This_Delimiter + whats_This_Wavelength)	create_Wavelength_Interface();

	done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		done_Button->setFocus();
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button,	   SIGNAL(clicked()), this, SLOT(close()));
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

		group_Box_Layout = new QVBoxLayout(group_Box);
			group_Box_Layout->setAlignment(Qt::AlignLeft);

		QHBoxLayout* layout = new QHBoxLayout;
			layout->setAlignment(Qt::AlignLeft);
		group_Box_Layout->addLayout(layout);

		num_Points = new QLineEdit;
			num_Points->setFixedWidth(30);
			num_Points->setProperty(min_Size, 30);
			num_Points->setValidator(new QIntValidator(1, MAX_INTEGER, this));
		layout->addWidget(num_Points);

		from_Label = new QLabel(from_Many);
		layout->addWidget(from_Label);

		min = new QLineEdit;
			min->setFixedWidth(40);
			min->setProperty(min_Size, 40);
			min->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));			
		layout->addWidget(min);

		to_Label = new QLabel(to);
			to_Label->setSizePolicy(sp_retain);
		layout->addWidget(to_Label);

		max = new QLineEdit;
			max->setFixedWidth(40);
			max->setProperty(min_Size, 40);
			max->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
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
			step_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			step_Edit->setSizePolicy(sp_retain);
			step_Edit->setReadOnly(true);
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
	refresh_Show_Data(show_Data);
}

void Independent_Variable_Editing::create_Angle_Interface()
{
	QSizePolicy sp_retain;
	sp_retain.setRetainSizeWhenHidden(true);

	QHBoxLayout* layout = new QHBoxLayout;
		layout->setAlignment(Qt::AlignLeft);
	group_Box_Layout->addLayout(layout);

	angular_Resolution_Label = new QLabel("Angular Resolution, " + Delta_Big_Sym + Theta_Sym);
		angular_Resolution_Label->setSizePolicy(sp_retain);
	layout->addWidget(angular_Resolution_Label);

	angular_Resolution_Edit = new QLineEdit;
		angular_Resolution_Edit->setFixedWidth(40);
		angular_Resolution_Edit->setProperty(min_Size, 40);
		angular_Resolution_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		angular_Resolution_Edit->setSizePolicy(sp_retain);
	layout->addWidget(angular_Resolution_Edit);

	angle_Units_Label = new QLabel(units);
		angle_Units_Label->setSizePolicy(sp_retain);
	layout->addWidget(angle_Units_Label);

	connect(angular_Resolution_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
	connect(angular_Resolution_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));

	angle_Done = true;
	refresh_Show_Data(show_Data);
}

void Independent_Variable_Editing::create_Wavelength_Interface()
{
	QSizePolicy sp_retain;
	sp_retain.setRetainSizeWhenHidden(true);

	{
		QHBoxLayout* up_Layout = new QHBoxLayout;
			up_Layout->setAlignment(Qt::AlignLeft);
		group_Box_Layout->addLayout(up_Layout);

		spectral_Resolution_Label = new QLabel("Spectral Resolution, " + Delta_Big_Sym + Lambda_Sym);
			spectral_Resolution_Label->setSizePolicy(sp_retain);
		up_Layout->addWidget(spectral_Resolution_Label);

		spectral_Resolution_Edit = new QLineEdit;
			spectral_Resolution_Edit->setFixedWidth(40);
			spectral_Resolution_Edit->setProperty(min_Size, 40);
			spectral_Resolution_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			spectral_Resolution_Edit->setSizePolicy(sp_retain);
		up_Layout->addWidget(spectral_Resolution_Edit);

		wavelength_Units_Label = new QLabel(units);
			wavelength_Units_Label->setSizePolicy(sp_retain);
		up_Layout->addWidget(wavelength_Units_Label);
	}
	{
		QHBoxLayout* down_Layout = new QHBoxLayout;
			down_Layout->setAlignment(Qt::AlignLeft);
		group_Box_Layout->addLayout(down_Layout);

		polarization_Label = new QLabel("Incident Polarization");
			polarization_Label->setSizePolicy(sp_retain);
		down_Layout->addWidget(polarization_Label);

		polarization_Edit = new QLineEdit;
			polarization_Edit->setFixedWidth(40);
			polarization_Edit->setProperty(min_Size, 40);
			polarization_Edit->setValidator(new QDoubleValidator(-1, 1, MAX_PRECISION, this));
			polarization_Edit->setSizePolicy(sp_retain);
		down_Layout->addWidget(polarization_Edit);

		analyzer_Label = new QLabel("Polarization Analyzer Sensitivity");
			analyzer_Label->setSizePolicy(sp_retain);
		down_Layout->addWidget(analyzer_Label);

		analyzer_Edit = new QLineEdit;
			analyzer_Edit->setFixedWidth(40);
			analyzer_Edit->setProperty(min_Size, 40);
			analyzer_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			analyzer_Edit->setSizePolicy(sp_retain);
		down_Layout->addWidget(analyzer_Edit);

	}

	connect(spectral_Resolution_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
	connect(spectral_Resolution_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));

	connect(polarization_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
	connect(polarization_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));

	connect(analyzer_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
	connect(analyzer_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));

	wavelength_Done = true;
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
	QString whats_This = item->whatsThis();
	QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
	QStringList whats_This_List_Type = whats_This_List[0].split(item_Type_Delimiter,QString::SkipEmptyParts);
	QVariant var;	

	// if ambient
	if(whats_This_List_Type[0] == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(default_Column, Qt::UserRole).value<Ambient>();

		/// optical constants

		// ambient density
		if(whats_This_List[1] == whats_This_Density)
		{
			name = ambient.material + " (ambient) " + whats_This_List[1] + ", " + Rho_Sym;
			group_Box->setTitle(name);
			units = "g/cm" + Cube_Sym;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.density.independent.num_Points));
				min->setText(		QString::number(ambient.density.independent.min,line_edit_double_format,line_edit_density_precision));
				max->setText(		QString::number(ambient.density.independent.max,line_edit_double_format,line_edit_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(ambient.density.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					ambient.density.independent.num_Points = num_Points->text().toInt();
					ambient.density.independent.min = min->text().toDouble();
					ambient.density.independent.max = max->text().toDouble();

					if(ambient.density.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(ambient.density.value,thumbnail_double_format,thumbnail_density_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(ambient.density.independent.num_Points) + " values: " +
									  QString::number(ambient.density.independent.min,thumbnail_double_format,thumbnail_density_precision) + " - " +
									  QString::number(ambient.density.independent.max,thumbnail_double_format,thumbnail_density_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((ambient.density.independent.max-ambient.density.independent.min)/ambient.density.independent.num_Points,line_edit_double_format,line_edit_density_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(ambient.density.independent.num_Points, show);
		}
		// ambient permittivity
		if(whats_This_List[1] == whats_This_Permittivity)
		{
			min->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
			max->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));

			name = ambient.material + " (ambient) " + whats_This_List[1] + ", " + "1-" + Epsilon_Sym;
			group_Box->setTitle(name);
			units = "% of nominal";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.permittivity.independent.num_Points));
				min->setText(		QString::number(ambient.permittivity.independent.min,line_edit_double_format,line_edit_permittivity_precision));
				max->setText(		QString::number(ambient.permittivity.independent.max,line_edit_double_format,line_edit_permittivity_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(ambient.permittivity.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					ambient.permittivity.independent.num_Points = num_Points->text().toInt();
					ambient.permittivity.independent.min = min->text().toDouble();
					ambient.permittivity.independent.max = max->text().toDouble();

					if(ambient.permittivity.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(ambient.permittivity.value,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(ambient.permittivity.independent.num_Points) + " values: " +
									  QString::number(ambient.permittivity.independent.min,thumbnail_double_format,thumbnail_permittivity_precision) + " - " +
									  QString::number(ambient.permittivity.independent.max,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((ambient.permittivity.independent.max-ambient.permittivity.independent.min)/ambient.permittivity.independent.num_Points,line_edit_double_format,line_edit_permittivity_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(ambient.permittivity.independent.num_Points, show);
		}
		// ambient absorption
		if(whats_This_List[1] == whats_This_Absorption)
		{
			name = ambient.material + " (ambient) " + whats_This_List[1] + ", " + Cappa_Sym;
			group_Box->setTitle(whats_This_List_Type[0] + " Absorption, " + Cappa_Sym);
			units = "% of nominal";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.absorption.independent.num_Points));
				min->setText(		QString::number(ambient.absorption.independent.min,line_edit_double_format,line_edit_absorption_precision));
				max->setText(		QString::number(ambient.absorption.independent.max,line_edit_double_format,line_edit_absorption_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(ambient.absorption.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					ambient.absorption.independent.num_Points = num_Points->text().toInt();
					ambient.absorption.independent.min = min->text().toDouble();
					ambient.absorption.independent.max = max->text().toDouble();

					if(ambient.absorption.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(ambient.absorption.value,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(ambient.absorption.independent.num_Points) + " values: " +
									  QString::number(ambient.absorption.independent.min,thumbnail_double_format,thumbnail_absorption_precision) + " - " +
									  QString::number(ambient.absorption.independent.max,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((ambient.absorption.independent.max-ambient.absorption.independent.min)/ambient.absorption.independent.num_Points,line_edit_double_format,line_edit_absorption_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(ambient.absorption.independent.num_Points, show);
		}
		//ambient composition
		if(whats_This_List[1] == whats_This_Composition)
		{
			int index = QString(whats_This_List[2]).toInt();
			name = ambient.material + " (ambient) " + ambient.composition[index].type + " " + whats_This_List[1] + ", " + Zeta_Sym + "_" + ambient.composition[index].type;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.composition[index].composition.independent.num_Points));
				min->setText(		QString::number(ambient.composition[index].composition.independent.min,line_edit_double_format,line_edit_composition_precision));
				max->setText(		QString::number(ambient.composition[index].composition.independent.max,line_edit_double_format,line_edit_composition_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(ambient.composition[index].composition.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					ambient.composition[index].composition.independent.num_Points = num_Points->text().toInt();
					ambient.composition[index].composition.independent.min = min->text().toDouble();
					ambient.composition[index].composition.independent.max = max->text().toDouble();

					if(ambient.composition[index].composition.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(ambient.composition[index].composition.value,thumbnail_double_format,thumbnail_composition_precision) + "]");
					else
						item->setText(name + " [" + QString::number(ambient.composition[index].composition.independent.num_Points) + " values: " +
									  QString::number(ambient.composition[index].composition.independent.min,thumbnail_double_format,thumbnail_composition_precision) + " - " +
									  QString::number(ambient.composition[index].composition.independent.max,thumbnail_double_format,thumbnail_composition_precision) + "]");
				}
			}
			step_Edit->setText(QString::number((ambient.composition[index].composition.independent.max-ambient.composition[index].composition.independent.min)/ambient.composition[index].composition.independent.num_Points,line_edit_double_format,line_edit_composition_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(ambient.composition[index].composition.independent.num_Points, show);
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
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + whats_This_List[1] + ", " + Rho_Sym;
			group_Box->setTitle(name);
			units = "g/cm" + Cube_Sym;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.density.independent.num_Points));
				min->setText(		QString::number(layer.density.independent.min,line_edit_double_format,line_edit_density_precision));
				max->setText(		QString::number(layer.density.independent.max,line_edit_double_format,line_edit_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(layer.density.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					layer.density.independent.num_Points = num_Points->text().toInt();
					layer.density.independent.min = min->text().toDouble();
					layer.density.independent.max = max->text().toDouble();

					if(layer.density.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.density.value,thumbnail_double_format,thumbnail_density_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(layer.density.independent.num_Points) + " values: " +
									  QString::number(layer.density.independent.min,thumbnail_double_format,thumbnail_density_precision) + " - " +
									  QString::number(layer.density.independent.max,thumbnail_double_format,thumbnail_density_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((layer.density.independent.max-layer.density.independent.min)/layer.density.independent.num_Points,line_edit_double_format,line_edit_density_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(layer.density.independent.num_Points, show);
		}
		// layer permittivity
		if(whats_This_List[1] == whats_This_Permittivity)
		{
			min->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
			max->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));

			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + whats_This_List[1] + ", " + "1-" + Epsilon_Sym;
			group_Box->setTitle(name);
			units = "% of nominal";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.permittivity.independent.num_Points));
				min->setText(		QString::number(layer.permittivity.independent.min,line_edit_double_format,line_edit_permittivity_precision));
				max->setText(		QString::number(layer.permittivity.independent.max,line_edit_double_format,line_edit_permittivity_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(layer.permittivity.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					layer.permittivity.independent.num_Points = num_Points->text().toInt();
					layer.permittivity.independent.min = min->text().toDouble();
					layer.permittivity.independent.max = max->text().toDouble();

					if(layer.permittivity.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.permittivity.value,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(layer.permittivity.independent.num_Points) + " values: " +
									  QString::number(layer.permittivity.independent.min,thumbnail_double_format,thumbnail_permittivity_precision) + " - " +
									  QString::number(layer.permittivity.independent.max,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((layer.permittivity.independent.max-layer.permittivity.independent.min)/layer.permittivity.independent.num_Points,line_edit_double_format,line_edit_permittivity_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(layer.permittivity.independent.num_Points, show);
		}
		// layer absorption
		if(whats_This_List[1] == whats_This_Absorption)
		{
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + whats_This_List[1] + ", " + Cappa_Sym;
			group_Box->setTitle(name);
			units = "% of nominal";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.absorption.independent.num_Points));
				min->setText(		QString::number(layer.absorption.independent.min,line_edit_double_format,line_edit_absorption_precision));
				max->setText(		QString::number(layer.absorption.independent.max,line_edit_double_format,line_edit_absorption_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(layer.absorption.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					layer.absorption.independent.num_Points = num_Points->text().toInt();
					layer.absorption.independent.min = min->text().toDouble();
					layer.absorption.independent.max = max->text().toDouble();

					if(layer.absorption.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.absorption.value,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(layer.absorption.independent.num_Points) + " values: " +
									  QString::number(layer.absorption.independent.min,thumbnail_double_format,thumbnail_absorption_precision) + " - " +
									  QString::number(layer.absorption.independent.max,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((layer.absorption.independent.max-layer.absorption.independent.min)/layer.absorption.independent.num_Points,line_edit_double_format,line_edit_absorption_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(layer.absorption.independent.num_Points, show);
		}
		// layer composition
		if(whats_This_List[1] == whats_This_Composition)
		{
			int index = QString(whats_This_List[2]).toInt();
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + layer.composition[index].type + " " + whats_This_List[1] + ", " + Zeta_Sym + "_" + layer.composition[index].type;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.composition[index].composition.independent.num_Points));
				min->setText(		QString::number(layer.composition[index].composition.independent.min,line_edit_double_format,line_edit_composition_precision));
				max->setText(		QString::number(layer.composition[index].composition.independent.max,line_edit_double_format,line_edit_composition_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(layer.composition[index].composition.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					layer.composition[index].composition.independent.num_Points = num_Points->text().toInt();
					layer.composition[index].composition.independent.min = min->text().toDouble();
					layer.composition[index].composition.independent.max = max->text().toDouble();

					if(layer.composition[index].composition.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.composition[index].composition.value,thumbnail_double_format,thumbnail_composition_precision) + "]");
					else
						item->setText(name + " [" + QString::number(layer.composition[index].composition.independent.num_Points) + " values: " +
									  QString::number(layer.composition[index].composition.independent.min,thumbnail_double_format,thumbnail_composition_precision) + " - " +
									  QString::number(layer.composition[index].composition.independent.max,thumbnail_double_format,thumbnail_composition_precision) + "]");
				}
			}
			step_Edit->setText(QString::number((layer.composition[index].composition.independent.max-layer.composition[index].composition.independent.min)/layer.composition[index].composition.independent.num_Points,line_edit_double_format,line_edit_composition_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(layer.composition[index].composition.independent.num_Points, show);
		}

		/// thickness parameters

		// layer thickness
		if(whats_This_List[1] == whats_This_Thickness)
		{
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + whats_This_List[1] + ", z";
			group_Box->setTitle(name);
			units = Angstrom_Sym;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.thickness.independent.num_Points));
				min->setText(		QString::number(layer.thickness.independent.min,line_edit_double_format,line_edit_thickness_precision));
				max->setText(		QString::number(layer.thickness.independent.max,line_edit_double_format,line_edit_thickness_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(layer.thickness.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					layer.thickness.independent.num_Points = num_Points->text().toInt();
					layer.thickness.independent.min = min->text().toDouble();
					layer.thickness.independent.max = max->text().toDouble();

					if(layer.thickness.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.thickness.value,thumbnail_double_format,thumbnail_thickness_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(layer.thickness.independent.num_Points) + " values: " +
									  QString::number(layer.thickness.independent.min,thumbnail_double_format,thumbnail_thickness_precision) + " - " +
									  QString::number(layer.thickness.independent.max,thumbnail_double_format,thumbnail_thickness_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((layer.thickness.independent.max-layer.thickness.independent.min)/layer.thickness.independent.num_Points,line_edit_double_format,line_edit_thickness_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(layer.thickness.independent.num_Points, show);
		}

		/// interface parameters

		// layer sigma
		if(whats_This_List[1] == whats_This_Sigma)
		{
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + "Roughness/Diffuseness, " + Sigma_Sym;
			group_Box->setTitle(name);
			units = Angstrom_Sym;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.sigma.independent.num_Points));
				min->setText(		QString::number(layer.sigma.independent.min,line_edit_double_format,line_edit_sigma_precision));
				max->setText(		QString::number(layer.sigma.independent.max,line_edit_double_format,line_edit_sigma_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(layer.sigma.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					layer.sigma.independent.num_Points = num_Points->text().toInt();
					layer.sigma.independent.min = min->text().toDouble();
					layer.sigma.independent.max = max->text().toDouble();

					if(layer.sigma.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.sigma.value,thumbnail_double_format,thumbnail_sigma_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(layer.sigma.independent.num_Points) + " values: " +
									  QString::number(layer.sigma.independent.min,thumbnail_double_format,thumbnail_sigma_precision) + " - " +
									  QString::number(layer.sigma.independent.max,thumbnail_double_format,thumbnail_sigma_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((layer.sigma.independent.max-layer.sigma.independent.min)/layer.sigma.independent.num_Points,line_edit_double_format,line_edit_sigma_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(layer.sigma.independent.num_Points, show);
		}
		// layer interlayer composition (if enabled and >=2 elements)
		if(whats_This_List[1] == whats_This_Interlayer_Composition)
		{
			int index = QString(whats_This_List.last()).toInt();
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + "Interlayer Composition, " + transition_Layer_Functions[index];
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.interlayer_Composition[index].interlayer.independent.num_Points));
				min->setText(		QString::number(layer.interlayer_Composition[index].interlayer.independent.min,line_edit_double_format,line_edit_interlayer_precision));
				max->setText(		QString::number(layer.interlayer_Composition[index].interlayer.independent.max,line_edit_double_format,line_edit_interlayer_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(layer.interlayer_Composition[index].interlayer.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					layer.interlayer_Composition[index].interlayer.independent.num_Points = num_Points->text().toInt();
					layer.interlayer_Composition[index].interlayer.independent.min = min->text().toDouble();
					layer.interlayer_Composition[index].interlayer.independent.max = max->text().toDouble();

					if(layer.interlayer_Composition[index].interlayer.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.interlayer_Composition[index].interlayer.value,thumbnail_double_format,thumbnail_interlayer_precision) + "]");
					else
						item->setText(name + " [" + QString::number(layer.interlayer_Composition[index].interlayer.independent.num_Points) + " values: " +
									  QString::number(layer.interlayer_Composition[index].interlayer.independent.min,thumbnail_double_format,thumbnail_interlayer_precision) + " - " +
									  QString::number(layer.interlayer_Composition[index].interlayer.independent.max,thumbnail_double_format,thumbnail_interlayer_precision) + "]");
				}
			}
			step_Edit->setText(QString::number((layer.interlayer_Composition[index].interlayer.independent.max-layer.interlayer_Composition[index].interlayer.independent.min)/layer.interlayer_Composition[index].interlayer.independent.num_Points,line_edit_double_format,line_edit_interlayer_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(layer.interlayer_Composition[index].interlayer.independent.num_Points, show);
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
			name = whats_This_List[0] + " Number of repetitions, N";
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			min->setValidator(new QIntValidator(0, MAX_INTEGER, this));
			max->setValidator(new QIntValidator(1, MAX_INTEGER, this));

			// show data
			if(show)
			{
				num_Points->setText(QString::number(stack_Content.num_Repetition.num_steps));
				min->setText(		QString::number(stack_Content.num_Repetition.start));
				max->setText(		QString::number(stack_Content.num_Repetition.step));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(stack_Content.num_Repetition.num_steps));
					num_Points->textEdited(num_Points->text());
				} else
				if(max->text().toInt()<1)
				{
					max->setText(QString::number(stack_Content.num_Repetition.step));
					max->textEdited(max->text());
				} else
				{
					stack_Content.num_Repetition.num_steps = num_Points->text().toInt();
					stack_Content.num_Repetition.start = min->text().toDouble();
					stack_Content.num_Repetition.step  = max->text().toDouble();

					if(stack_Content.num_Repetition.num_steps == 1)
						item->setText(name + " [" + QString::number(stack_Content.num_Repetition.value) + "]");
					else
						item->setText(name + " [" + QString::number(stack_Content.num_Repetition.num_steps) + " values: " +
									  QString::number(stack_Content.num_Repetition.start) + " - " +
									  QString::number(stack_Content.num_Repetition.start + stack_Content.num_Repetition.step*stack_Content.num_Repetition.num_steps) + "]");
				}
			}
			show_Hide_Elements(stack_Content.num_Repetition.num_steps, show);
			if(stack_Content.num_Repetition.num_steps > 1)
			{
				from_Label->setText("values; start:");
				to_Label->setText(" step size:");
				step_Edit->hide();
				step_Label->hide();
				units_Label->hide();
			}
		}
		// multilayer period
		if(whats_This_List[1] == whats_This_Period)
		{
			name = whats_This_List[0] + " " + whats_This_List[1] + ", d";
			group_Box->setTitle(name);
			units = Angstrom_Sym;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(stack_Content.period.independent.num_Points));
				min->setText(		QString::number(stack_Content.period.independent.min,line_edit_double_format,line_edit_period_precision));
				max->setText(		QString::number(stack_Content.period.independent.max,line_edit_double_format,line_edit_period_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(stack_Content.period.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					stack_Content.period.independent.num_Points = num_Points->text().toInt();
					stack_Content.period.independent.min = min->text().toDouble();
					stack_Content.period.independent.max = max->text().toDouble();

					if(stack_Content.period.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(stack_Content.period.value,thumbnail_double_format,thumbnail_period_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(stack_Content.period.independent.num_Points) + " values: " +
									  QString::number(stack_Content.period.independent.min,thumbnail_double_format,thumbnail_period_precision) + " - " +
									  QString::number(stack_Content.period.independent.max,thumbnail_double_format,thumbnail_period_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((stack_Content.period.independent.max-stack_Content.period.independent.min)/stack_Content.period.independent.num_Points,line_edit_double_format,line_edit_period_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(stack_Content.period.independent.num_Points, show);
		}
		// multilayer gamma
		if(whats_This_List[1] == whats_This_Gamma)
		{
			name = whats_This_List[0] + " Thickness Ratio, " + Gamma_Sym;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(stack_Content.gamma.independent.num_Points));
				min->setText(		QString::number(stack_Content.gamma.independent.min,line_edit_double_format,line_edit_gamma_precision));
				max->setText(		QString::number(stack_Content.gamma.independent.max,line_edit_double_format,line_edit_gamma_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(stack_Content.gamma.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				if(min->text().toDouble()>1)
				{
					min->setText(QString::number(stack_Content.gamma.independent.min,line_edit_double_format,line_edit_gamma_precision));
					min->textEdited(min->text());
				} else
				if(max->text().toDouble()>1)
				{
					max->setText(QString::number(stack_Content.gamma.independent.max,line_edit_double_format,line_edit_gamma_precision));
					max->textEdited(max->text());
				} else
				{
					stack_Content.gamma.independent.num_Points = num_Points->text().toInt();
					stack_Content.gamma.independent.min = min->text().toDouble();
					stack_Content.gamma.independent.max = max->text().toDouble();

					if(stack_Content.gamma.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(stack_Content.gamma.value,thumbnail_double_format,thumbnail_gamma_precision) + "]");
					else
						item->setText(name + " [" + QString::number(stack_Content.gamma.independent.num_Points) + " values: " +
									  QString::number(stack_Content.gamma.independent.min,thumbnail_double_format,thumbnail_gamma_precision) + " - " +
									  QString::number(stack_Content.gamma.independent.max,thumbnail_double_format,thumbnail_gamma_precision) + "]");
				}
			}
			step_Edit->setText(QString::number((stack_Content.gamma.independent.max-stack_Content.gamma.independent.min)/stack_Content.gamma.independent.num_Points,line_edit_double_format,line_edit_gamma_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(stack_Content.gamma.independent.num_Points, show);
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
			name = substrate.material + " (substrate) " + whats_This_List[1] + ", " + Rho_Sym;
			group_Box->setTitle(name);
			units = "g/cm" + Cube_Sym;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.density.independent.num_Points));
				min->setText(		QString::number(substrate.density.independent.min,line_edit_double_format,line_edit_density_precision));
				max->setText(		QString::number(substrate.density.independent.max,line_edit_double_format,line_edit_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(substrate.density.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					substrate.density.independent.num_Points = num_Points->text().toInt();
					substrate.density.independent.min = min->text().toDouble();
					substrate.density.independent.max = max->text().toDouble();

					if(substrate.density.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.density.value,thumbnail_double_format,thumbnail_density_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(substrate.density.independent.num_Points) + " values: " +
									  QString::number(substrate.density.independent.min,thumbnail_double_format,thumbnail_density_precision) + " - " +
									  QString::number(substrate.density.independent.max,thumbnail_double_format,thumbnail_density_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((substrate.density.independent.max-substrate.density.independent.min)/substrate.density.independent.num_Points,line_edit_double_format,line_edit_density_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(substrate.density.independent.num_Points, show);
		}
		// substrate permittivity
		if(whats_This_List[1] == whats_This_Permittivity)
		{
			min->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
			max->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));

			name = substrate.material + " (substrate) " + whats_This_List[1] + ", " + "1-" + Epsilon_Sym;
			group_Box->setTitle(name);
			units = "% of nominal";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.permittivity.independent.num_Points));
				min->setText(		QString::number(substrate.permittivity.independent.min,line_edit_double_format,line_edit_permittivity_precision));
				max->setText(		QString::number(substrate.permittivity.independent.max,line_edit_double_format,line_edit_permittivity_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(substrate.permittivity.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					substrate.permittivity.independent.num_Points = num_Points->text().toInt();
					substrate.permittivity.independent.min = min->text().toDouble();
					substrate.permittivity.independent.max = max->text().toDouble();

					if(substrate.permittivity.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.permittivity.value,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(substrate.permittivity.independent.num_Points) + " values: " +
									  QString::number(substrate.permittivity.independent.min,thumbnail_double_format,thumbnail_permittivity_precision) + " - " +
									  QString::number(substrate.permittivity.independent.max,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((substrate.permittivity.independent.max-substrate.permittivity.independent.min)/substrate.permittivity.independent.num_Points,line_edit_double_format,line_edit_permittivity_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(substrate.permittivity.independent.num_Points, show);
		}
		// substrate absorption
		if(whats_This_List[1] == whats_This_Absorption)
		{
			name = substrate.material + " (substrate) " + whats_This_List[1] + ", " + Cappa_Sym;
			group_Box->setTitle(name);
			units = "% of nominal";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.absorption.independent.num_Points));
				min->setText(		QString::number(substrate.absorption.independent.min,line_edit_double_format,line_edit_absorption_precision));
				max->setText(		QString::number(substrate.absorption.independent.max,line_edit_double_format,line_edit_absorption_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(substrate.absorption.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					substrate.absorption.independent.num_Points = num_Points->text().toInt();
					substrate.absorption.independent.min = min->text().toDouble();
					substrate.absorption.independent.max = max->text().toDouble();

					if(substrate.absorption.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.absorption.value,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(substrate.absorption.independent.num_Points) + " values: " +
									  QString::number(substrate.absorption.independent.min,thumbnail_double_format,thumbnail_absorption_precision) + " - " +
									  QString::number(substrate.absorption.independent.max,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((substrate.absorption.independent.max-substrate.absorption.independent.min)/substrate.absorption.independent.num_Points,line_edit_double_format,line_edit_absorption_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(substrate.absorption.independent.num_Points, show);
		}
		// substrate composition
		if(whats_This_List[1] == whats_This_Composition)
		{
			int index = QString(whats_This_List[2]).toInt();
			name = substrate.material + " (substrate) " + substrate.composition[index].type + " " + whats_This_List[1] + ", " + Zeta_Sym;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.composition[index].composition.independent.num_Points));
				min->setText(		QString::number(substrate.composition[index].composition.independent.min,line_edit_double_format,line_edit_composition_precision));
				max->setText(		QString::number(substrate.composition[index].composition.independent.max,line_edit_double_format,line_edit_composition_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(substrate.composition[index].composition.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					substrate.composition[index].composition.independent.num_Points = num_Points->text().toInt();
					substrate.composition[index].composition.independent.min = min->text().toDouble();
					substrate.composition[index].composition.independent.max = max->text().toDouble();

					if(substrate.composition[index].composition.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.composition[index].composition.value,thumbnail_double_format,thumbnail_composition_precision) + "]");
					else
						item->setText(name + " [" + QString::number(substrate.composition[index].composition.independent.num_Points) + " values: " +
									  QString::number(substrate.composition[index].composition.independent.min,thumbnail_double_format,thumbnail_composition_precision) + " - " +
									  QString::number(substrate.composition[index].composition.independent.max,thumbnail_double_format,thumbnail_composition_precision) + "]");
				}
			}
			step_Edit->setText(QString::number((substrate.composition[index].composition.independent.max-substrate.composition[index].composition.independent.min)/substrate.composition[index].composition.independent.num_Points,line_edit_double_format,line_edit_composition_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(substrate.composition[index].composition.independent.num_Points, show);
		}

		/// interface parameters

		// substrate sigma
		if(whats_This_List[1] == whats_This_Sigma)
		{
			name = substrate.material + " (substrate) " + "Roughness/Diffuseness, " + Sigma_Sym;
			group_Box->setTitle(name);
			units = Angstrom_Sym;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.sigma.independent.num_Points));
				min->setText(		QString::number(substrate.sigma.independent.min,line_edit_double_format,line_edit_sigma_precision));
				max->setText(		QString::number(substrate.sigma.independent.max,line_edit_double_format,line_edit_sigma_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(substrate.sigma.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					substrate.sigma.independent.num_Points = num_Points->text().toInt();
					substrate.sigma.independent.min = min->text().toDouble();
					substrate.sigma.independent.max = max->text().toDouble();

					if(substrate.sigma.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.sigma.value,thumbnail_double_format,thumbnail_sigma_precision) + " " + units + "]");
					else
						item->setText(name + " [" + QString::number(substrate.sigma.independent.num_Points) + " values: " +
									  QString::number(substrate.sigma.independent.min,thumbnail_double_format,thumbnail_sigma_precision) + " - " +
									  QString::number(substrate.sigma.independent.max,thumbnail_double_format,thumbnail_sigma_precision) + " " + units + "]");
				}
			}
			step_Edit->setText(QString::number((substrate.sigma.independent.max-substrate.sigma.independent.min)/substrate.sigma.independent.num_Points,line_edit_double_format,line_edit_sigma_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(substrate.sigma.independent.num_Points, show);
		}
		// substrate interlayer composition (if enabled and >=2 elements)
		if(whats_This_List[1] == whats_This_Interlayer_Composition)
		{
			int index = QString(whats_This_List.last()).toInt();
			name = substrate.material + " (substrate) " + "Interlayer Composition, " + transition_Layer_Functions[index];
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.interlayer_Composition[index].interlayer.independent.num_Points));
				min->setText(		QString::number(substrate.interlayer_Composition[index].interlayer.independent.min,line_edit_double_format,line_edit_interlayer_precision));
				max->setText(		QString::number(substrate.interlayer_Composition[index].interlayer.independent.max,line_edit_double_format,line_edit_interlayer_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(substrate.interlayer_Composition[index].interlayer.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					substrate.interlayer_Composition[index].interlayer.independent.num_Points = num_Points->text().toInt();
					substrate.interlayer_Composition[index].interlayer.independent.min = min->text().toDouble();
					substrate.interlayer_Composition[index].interlayer.independent.max = max->text().toDouble();

					if(substrate.interlayer_Composition[index].interlayer.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.interlayer_Composition[index].interlayer.value,thumbnail_double_format,thumbnail_interlayer_precision) + "]");
					else
						item->setText(name + " [" + QString::number(substrate.interlayer_Composition[index].interlayer.independent.num_Points) + " values: " +
									  QString::number(substrate.interlayer_Composition[index].interlayer.independent.min,thumbnail_double_format,thumbnail_interlayer_precision) + " - " +
									  QString::number(substrate.interlayer_Composition[index].interlayer.independent.max,thumbnail_double_format,thumbnail_interlayer_precision) + "]");
				}
			}
			step_Edit->setText(QString::number((substrate.interlayer_Composition[index].interlayer.independent.max-substrate.interlayer_Composition[index].interlayer.independent.min)/substrate.interlayer_Composition[index].interlayer.independent.num_Points,line_edit_double_format,line_edit_interlayer_precision));
			step_Edit->textEdited(step_Edit->text());
			show_Hide_Elements(substrate.interlayer_Composition[index].interlayer.independent.num_Points, show);
		}

		var.setValue(substrate);
		structure_Item->setData(default_Column, Qt::UserRole, var);
	}

	// if angle
	if(angle_Done)
	if(whats_This_List[1] == whats_This_Angle)
	{
		Measurement measurement = structure_Item->data(default_Column, Qt::UserRole).value<Measurement>();		

		QString angle_Type;
		if(measurement.angle_Type == Angle_Type::Grazing)   angle_Type = "Grazing";
		if(measurement.angle_Type == Angle_Type::Incidence) angle_Type = "Incidence";

		name = angle_Type + " angle, " + Theta_Sym;
		group_Box->setTitle(name);
		units = Degree_Sym;
		units_Label->setText(units);
		step_Units_Label->setText(units);
		angle_Units_Label->setText(units);

		// show data
		if(show)
		{
			num_Points->setText(QString::number(measurement.probe_Angle.independent.num_Points));
			min->setText(		QString::number(measurement.probe_Angle.independent.min,line_edit_double_format,line_edit_angle_precision));
			max->setText(		QString::number(measurement.probe_Angle.independent.max,line_edit_double_format,line_edit_angle_precision));

			if(measurement.probe_Angle.independent.num_Points >= MIN_ANGULAR_RESOLUTION_POINTS)
				angular_Resolution_Edit->setText(QString::number(measurement.angular_Resolution.value,line_edit_double_format,line_edit_angle_precision));
		} else
		// refresh data
		{
			if(num_Points->text().toInt()<1)
			{
				num_Points->setText(QString::number(measurement.probe_Angle.independent.num_Points));
				num_Points->textEdited(num_Points->text());
			} else
			if(min->text().toDouble()>90)
			{
				min->setText(QString::number(measurement.probe_Angle.independent.min,line_edit_double_format,line_edit_angle_precision));
				min->textEdited(min->text());
			} else
			if(max->text().toDouble()>90)
			{
				max->setText(QString::number(measurement.probe_Angle.independent.max,line_edit_double_format,line_edit_angle_precision));
				max->textEdited(max->text());
			} else
			{
				measurement.probe_Angle.independent.num_Points = num_Points->text().toInt();
				measurement.probe_Angle.independent.min = min->text().toDouble();
				measurement.probe_Angle.independent.max = max->text().toDouble();

				if(measurement.probe_Angle.independent.num_Points == 1)
					item->setText(name + " [" + QString::number(measurement.probe_Angle.value,thumbnail_double_format,thumbnail_angle_precision) + units + "]");
				else
					item->setText(name + " [" + QString::number(measurement.probe_Angle.independent.num_Points) + " values: " +
								  QString::number(measurement.probe_Angle.independent.min,thumbnail_double_format,thumbnail_angle_precision) + " - " +
								  QString::number(measurement.probe_Angle.independent.max,thumbnail_double_format,thumbnail_angle_precision)  + units + "]");

				if(measurement.probe_Angle.independent.num_Points >= MIN_ANGULAR_RESOLUTION_POINTS)
					measurement.angular_Resolution.value = angular_Resolution_Edit->text().toDouble();
				else
					measurement.angular_Resolution.value = default_angular_resolution;
			}
		}
		if(measurement.probe_Angle.independent.num_Points >= MIN_ANGULAR_RESOLUTION_POINTS)
			angular_Resolution_Edit->setText(QString::number(measurement.angular_Resolution.value,line_edit_double_format,line_edit_angle_precision));
		step_Edit->setText(QString::number((measurement.probe_Angle.independent.max-measurement.probe_Angle.independent.min)/measurement.probe_Angle.independent.num_Points,line_edit_double_format,line_edit_angle_precision));
		step_Edit->textEdited(step_Edit->text());
		show_Hide_Elements(measurement.probe_Angle.independent.num_Points, show);
		show_Hide_Angular_Elements(measurement.probe_Angle.independent.num_Points, show);

		var.setValue(measurement);
		structure_Item->setData(default_Column, Qt::UserRole, var);
	}

	// if wavelength
	if(wavelength_Done)
	if(whats_This_List[1] == whats_This_Wavelength)
	{
		Measurement measurement = structure_Item->data(default_Column, Qt::UserRole).value<Measurement>();

		name = "Wavelength, " + Lambda_Sym;
		group_Box->setTitle(name);
		units = Angstrom_Sym;
		units_Label->setText(units);
		step_Units_Label->setText(units);
		wavelength_Units_Label->setText(units);

		// show data
		if(show)
		{
			num_Points->setText(QString::number(measurement.wavelength.independent.num_Points));
			min->setText(		QString::number(measurement.wavelength.independent.min,line_edit_double_format,line_edit_wavelength_precision));
			max->setText(		QString::number(measurement.wavelength.independent.max,line_edit_double_format,line_edit_wavelength_precision));

			polarization_Edit->setText(QString::number(measurement.polarization.value,line_edit_double_format,line_edit_wavelength_precision));
			analyzer_Edit->setText(QString::number(measurement.polarization_Sensitivity.value,line_edit_double_format,line_edit_wavelength_precision));

			if(measurement.wavelength.independent.num_Points >= MIN_SPECTRAL_RESOLUTION_POINTS)
				spectral_Resolution_Edit->setText(QString::number(measurement.spectral_Resolution.value,line_edit_double_format,line_edit_wavelength_precision));
		} else
		// refresh data
		{
			if(num_Points->text().toInt()<1)
			{
				num_Points->setText(QString::number(measurement.wavelength.independent.num_Points));
				num_Points->textEdited(num_Points->text());
			} else
			if(polarization_Edit->text().toDouble()>1)
			{
				polarization_Edit->setText(QString::number(measurement.polarization.value,line_edit_double_format,line_edit_wavelength_precision));
				polarization_Edit->textEdited(min->text());
			} else
			if(polarization_Edit->text().toDouble()<-1)
			{
				polarization_Edit->setText(QString::number(measurement.polarization.value,line_edit_double_format,line_edit_wavelength_precision));
				polarization_Edit->textEdited(max->text());
			} else
			{
				measurement.wavelength.independent.num_Points = num_Points->text().toInt();
				measurement.wavelength.independent.min = min->text().toDouble();
				measurement.wavelength.independent.max = max->text().toDouble();

				measurement.polarization.value = polarization_Edit->text().toDouble();
				measurement.polarization_Sensitivity.value = analyzer_Edit->text().toDouble();

				if(measurement.wavelength.independent.num_Points == 1)
					item->setText(name + " [" + QString::number(measurement.wavelength.value,thumbnail_double_format,thumbnail_wavelength_precision) + " " + units + "]");
				else
					item->setText(name + " [" + QString::number(measurement.wavelength.independent.num_Points) + " values: " +
								  QString::number(measurement.wavelength.independent.min,thumbnail_double_format,thumbnail_wavelength_precision) + " - " +
								  QString::number(measurement.wavelength.independent.max,thumbnail_double_format,thumbnail_wavelength_precision)  + " " + units + "]");

				if(measurement.wavelength.independent.num_Points >= MIN_SPECTRAL_RESOLUTION_POINTS)
					measurement.spectral_Resolution.value = spectral_Resolution_Edit->text().toDouble();
				else
					measurement.spectral_Resolution.value = default_spectral_resolution;
			}
		}
		if(measurement.wavelength.independent.num_Points >= MIN_SPECTRAL_RESOLUTION_POINTS)
			spectral_Resolution_Edit->setText(QString::number(measurement.spectral_Resolution.value,line_edit_double_format,line_edit_wavelength_precision));
		step_Edit->setText(QString::number((measurement.wavelength.independent.max-measurement.wavelength.independent.min)/measurement.wavelength.independent.num_Points,line_edit_double_format,line_edit_wavelength_precision));
		step_Edit->textEdited(step_Edit->text());
		show_Hide_Elements(measurement.wavelength.independent.num_Points, show);
		show_Hide_Spectral_Elements(measurement.wavelength.independent.num_Points, show);

		var.setValue(measurement);
		structure_Item->setData(default_Column, Qt::UserRole, var);
	}
}

void Independent_Variable_Editing::show_Hide_Angular_Elements(int points, bool show)
{
	if(show)
	{
		angular_Resolution_Edit->textEdited(angular_Resolution_Edit->text());
	}

	if(points < MIN_ANGULAR_RESOLUTION_POINTS)
	{
		angular_Resolution_Label->hide();
		angular_Resolution_Edit->hide();
		angle_Units_Label->hide();
	}
	else
	{
		angular_Resolution_Label->show();
		angular_Resolution_Edit->show();
		angle_Units_Label->show();
	}
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Independent_Variable_Editing::show_Hide_Spectral_Elements(int points, bool show)
{
	if(show)
	{
		spectral_Resolution_Edit->textEdited(spectral_Resolution_Edit->text());
	}

	if(points < MIN_ANGULAR_RESOLUTION_POINTS)
	{
		spectral_Resolution_Label->hide();
		spectral_Resolution_Edit->hide();
		wavelength_Units_Label->hide();
	}
	else
	{
		spectral_Resolution_Label->show();
		spectral_Resolution_Edit->show();
		wavelength_Units_Label->show();
	}
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}
