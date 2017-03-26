// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "independent_variables_editor.h"

Independent_Variables_Editor::Independent_Variables_Editor(QTreeWidgetItem* structure_Item, QListWidgetItem* item, QListWidget* variables_List, QWidget *parent) :
	QDialog(parent),
	structure_Item(structure_Item),
	item(item),
	variables_List(variables_List)
{
	create_Main_Layout();

	setWindowTitle("Independent Variable");
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Independent_Variables_Editor::refresh_All()
{
	refresh_Show_Data(show_Data);
	emit refresh_Multilayer();
}

void Independent_Variables_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,0,4,0);

	create_Menu();

	create_Standard_Interface();
		main_Layout->addWidget(group_Box);

	QStringList whats_This_List = item->whatsThis().split(whats_This_Delimiter,QString::SkipEmptyParts);
	// if angle
	if(whats_This_List.last() == whats_This_Angle)		create_Angle_Interface();
	// if wavelength
	if(whats_This_List.last() == whats_This_Wavelength)	create_Wavelength_Interface();

	done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		done_Button->setFocus();
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button,	   SIGNAL(clicked()), this, SLOT(close()));
}

void Independent_Variables_Editor::create_Menu()
{
	Menu* menu = new Menu(Window_Type::Independent_Variables_Editor(), this);
	main_Layout->setMenuBar(menu->menu_Bar);
	connect(menu, SIGNAL(refresh()), this, SLOT(refresh_All()));
}

void Independent_Variables_Editor::create_Standard_Interface()
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
			num_Points->setProperty(min_Size_Property, 30);
			num_Points->setValidator(new QIntValidator(1, MAX_INTEGER, this));
		layout->addWidget(num_Points);

		from_Label = new QLabel(from_Many);
		layout->addWidget(from_Label);

		min = new QLineEdit;
			min->setFixedWidth(40);
			min->setProperty(min_Size_Property, 40);
			min->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		layout->addWidget(min);

		to_Label = new QLabel(to);
			to_Label->setSizePolicy(sp_retain);
		layout->addWidget(to_Label);

		max = new QLineEdit;
			max->setFixedWidth(40);
			max->setProperty(min_Size_Property, 40);
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
			step_Edit->setProperty(min_Size_Property, 40);
			step_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			step_Edit->setSizePolicy(sp_retain);
			step_Edit->setReadOnly(true);
		layout->addWidget(step_Edit);

		step_Units_Label = new QLabel(units);
			step_Units_Label->setSizePolicy(sp_retain);
		layout->addWidget(step_Units_Label);

		active_Check_Box = new QCheckBox("Active");
			show_Active_Check_Box();
		layout->addWidget(active_Check_Box);

		connect(num_Points, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(num_Points, SIGNAL(textEdited(QString)), this, SLOT(refresh_Special_Line_Edit(QString)));
		connect(min,		SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(max,		SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(step_Edit,	SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		connect(num_Points, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));
		connect(min,		SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));
		connect(max,		SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));

		connect(active_Check_Box, SIGNAL(toggled(bool)), this, SLOT(activate_Variable(bool)));
	}
	refresh_Show_Data(show_Data);
}

void Independent_Variables_Editor::create_Angle_Interface()
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
		angular_Resolution_Edit->setProperty(min_Size_Property, 40);
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

void Independent_Variables_Editor::create_Wavelength_Interface()
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
			spectral_Resolution_Edit->setProperty(min_Size_Property, 40);
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
			polarization_Edit->setProperty(min_Size_Property, 40);
			polarization_Edit->setValidator(new QDoubleValidator(-1, 1, MAX_PRECISION, this));
			polarization_Edit->setSizePolicy(sp_retain);
		down_Layout->addWidget(polarization_Edit);

		analyzer_Label = new QLabel("Polarization Analyzer Sensitivity");
			analyzer_Label->setSizePolicy(sp_retain);
		down_Layout->addWidget(analyzer_Label);

		analyzer_Edit = new QLineEdit;
			analyzer_Edit->setFixedWidth(40);
			analyzer_Edit->setProperty(min_Size_Property, 40);
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

void Independent_Variables_Editor::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
}

void Independent_Variables_Editor::resize_Line_Edit(QString text, QLineEdit* line_Edit)
{
	if(line_Edit == NULL) line_Edit = qobject_cast<QLineEdit*>(QObject::sender());

	text = line_Edit->text();
	QFontMetrics fm = line_Edit->fontMetrics();
	int width = fm.width(text) + QLINEEDIT_RESIZE_MARGIN;
	if(width>line_Edit->property(min_Size_Property).toInt())
	{
		line_Edit->setFixedWidth(width);
		QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
	} else
	{
		line_Edit->setFixedWidth(line_Edit->property(min_Size_Property).toInt());
		QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
	}
}

void Independent_Variables_Editor::refresh_Special_Line_Edit(QString text)
{
	QString whats_This = item->whatsThis();
	QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// if angle
	if(angle_Done)
	if(whats_This_List[1] == whats_This_Angle)
	{
		Measurement measurement = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
		double coeff = angle_Coefficients_Map.value(angle_units);

		angular_Resolution_Edit->setText(QString::number(measurement.angular_Resolution.value/coeff,line_edit_double_format,line_edit_angle_precision));
	}

	// if wavelength
	if(wavelength_Done)
	if(whats_This_List[1] == whats_This_Wavelength)
	{
		Measurement measurement = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
		double coeff = wavelength_Coefficients_Map.value(wavelength_units);

		spectral_Resolution_Edit->setText(QString::number(measurement.spectral_Resolution.value/coeff,line_edit_double_format,line_edit_wavelength_precision));
	}
}

void Independent_Variables_Editor::show_Hide_Elements(int points, bool show)
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

void Independent_Variables_Editor::refresh_Data(QString)
{
	refresh_Show_Data();
}

void Independent_Variables_Editor::refresh_Show_Data(bool show)
{
	// PARAMETER

	QString whats_This = item->whatsThis();
	QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
	QStringList whats_This_List_Type = whats_This_List[1].split(item_Type_Delimiter,QString::SkipEmptyParts);
	QVariant var;

	int wtl_index = 2;

	// if ambient
	if(whats_This_List_Type[0] == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		/// optical constants

		// ambient absolute density
		if(whats_This_List[wtl_index] == whats_This_Absolute_Density)
		{
			name = ambient.material + " (ambient) " + whats_This_List[wtl_index] + ", " + Rho_Sym;
			group_Box->setTitle(name);
			units = density_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.absolute_Density.independent.num_Points));
				if(ambient.absolute_Density.independent.num_Points == 1)
					min->setText(	QString::number(ambient.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
				else
					min->setText(	QString::number(ambient.absolute_Density.independent.min,line_edit_double_format,line_edit_density_precision));
				max->setText(		QString::number(ambient.absolute_Density.independent.max,line_edit_double_format,line_edit_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(ambient.absolute_Density.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					ambient.absolute_Density.independent.num_Points = num_Points->text().toInt();
					if(ambient.absolute_Density.independent.num_Points == 1)
						ambient.absolute_Density.value = min->text().toDouble();
					else
						ambient.absolute_Density.independent.min = min->text().toDouble();
					ambient.absolute_Density.independent.max = max->text().toDouble();

					if(ambient.absolute_Density.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(ambient.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(ambient.absolute_Density.independent.num_Points) + " values: " +
									  QString::number(ambient.absolute_Density.independent.min,thumbnail_double_format,thumbnail_density_precision) + " - " +
									  QString::number(ambient.absolute_Density.independent.max,thumbnail_double_format,thumbnail_density_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((ambient.absolute_Density.independent.max-ambient.absolute_Density.independent.min)/(ambient.absolute_Density.independent.num_Points-1),line_edit_double_format,line_edit_density_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(ambient.absolute_Density.independent.num_Points, show);
		}
		// ambient relative density
		if(whats_This_List[wtl_index] == whats_This_Relative_Density)
		{
			name = ambient.material + " (ambient) " + whats_This_List[wtl_index] + ", " + Rho_Sym;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.relative_Density.independent.num_Points));
				if(ambient.relative_Density.independent.num_Points == 1)
					min->setText(	QString::number(ambient.relative_Density.value,line_edit_double_format,line_edit_density_precision));
				else
					min->setText(	QString::number(ambient.relative_Density.independent.min,line_edit_double_format,line_edit_density_precision));
				max->setText(		QString::number(ambient.relative_Density.independent.max,line_edit_double_format,line_edit_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(ambient.relative_Density.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					ambient.relative_Density.independent.num_Points = num_Points->text().toInt();
					if(ambient.relative_Density.independent.num_Points == 1)
						ambient.relative_Density.value = min->text().toDouble();
					else
						ambient.relative_Density.independent.min = min->text().toDouble();
					ambient.relative_Density.independent.max = max->text().toDouble();

					if(ambient.relative_Density.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(ambient.relative_Density.value,thumbnail_double_format,thumbnail_density_precision) + end_Bracket);
					else
						item->setText(name + " [" + QString::number(ambient.relative_Density.independent.num_Points) + " values: " +
									  QString::number(ambient.relative_Density.independent.min,thumbnail_double_format,thumbnail_density_precision) + " - " +
									  QString::number(ambient.relative_Density.independent.max,thumbnail_double_format,thumbnail_density_precision) + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((ambient.relative_Density.independent.max-ambient.relative_Density.independent.min)/(ambient.relative_Density.independent.num_Points-1),line_edit_double_format,line_edit_density_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(ambient.relative_Density.independent.num_Points, show);
		}
		// ambient permittivity
		if(whats_This_List[wtl_index] == whats_This_Permittivity)
		{
			min->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
			max->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));

			name = ambient.material + " (ambient) " + whats_This_List[wtl_index] + ", " + "1-" + Epsilon_Sym;
			group_Box->setTitle(name);
			units = opt_const_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.permittivity.independent.num_Points));
				if(ambient.permittivity.independent.num_Points == 1)
					min->setText(	QString::number(ambient.permittivity.value,line_edit_double_format,line_edit_permittivity_precision));
				else
					min->setText(	QString::number(ambient.permittivity.independent.min,line_edit_double_format,line_edit_permittivity_precision));
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
					if(ambient.permittivity.independent.num_Points == 1)
						ambient.permittivity.value = min->text().toDouble();
					else
						ambient.permittivity.independent.min = min->text().toDouble();
					ambient.permittivity.independent.max = max->text().toDouble();

					if(ambient.permittivity.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(ambient.permittivity.value,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(ambient.permittivity.independent.num_Points) + " values: " +
									  QString::number(ambient.permittivity.independent.min,thumbnail_double_format,thumbnail_permittivity_precision) + " - " +
									  QString::number(ambient.permittivity.independent.max,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((ambient.permittivity.independent.max-ambient.permittivity.independent.min)/(ambient.permittivity.independent.num_Points-1),line_edit_double_format,line_edit_permittivity_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(ambient.permittivity.independent.num_Points, show);
		}
		// ambient absorption
		if(whats_This_List[wtl_index] == whats_This_Absorption)
		{
			name = ambient.material + " (ambient) " + whats_This_List[wtl_index] + ", " + Cappa_Sym;
			group_Box->setTitle(whats_This_List_Type[0] + " Absorption, " + Cappa_Sym);
			units = opt_const_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.absorption.independent.num_Points));
				if(ambient.absorption.independent.num_Points == 1)
					min->setText(	QString::number(ambient.absorption.value,line_edit_double_format,line_edit_absorption_precision));
				else
					min->setText(	QString::number(ambient.absorption.independent.min,line_edit_double_format,line_edit_absorption_precision));
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
					if(ambient.absorption.independent.num_Points == 1)
						ambient.absorption.value = min->text().toDouble();
					else
						ambient.absorption.independent.min = min->text().toDouble();
					ambient.absorption.independent.max = max->text().toDouble();

					if(ambient.absorption.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(ambient.absorption.value,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(ambient.absorption.independent.num_Points) + " values: " +
									  QString::number(ambient.absorption.independent.min,thumbnail_double_format,thumbnail_absorption_precision) + " - " +
									  QString::number(ambient.absorption.independent.max,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((ambient.absorption.independent.max-ambient.absorption.independent.min)/(ambient.absorption.independent.num_Points-1),line_edit_double_format,line_edit_absorption_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(ambient.absorption.independent.num_Points, show);
		}
		//ambient composition
		if(whats_This_List[wtl_index] == whats_This_Composition)
		{
			int index = QString(whats_This_List[wtl_index+1]).toInt();
			name = ambient.material + " (ambient) " + ambient.composition[index].type + " " + whats_This_List[wtl_index] + ", " + Zeta_Sym + "_" + ambient.composition[index].type;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(ambient.composition[index].composition.independent.num_Points));
				if(ambient.composition[index].composition.independent.num_Points == 1)
					min->setText(	QString::number(ambient.composition[index].composition.value,line_edit_double_format,line_edit_composition_precision));
				else
					min->setText(	QString::number(ambient.composition[index].composition.independent.min,line_edit_double_format,line_edit_composition_precision));
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
					if(ambient.composition[index].composition.independent.num_Points == 1)
						ambient.composition[index].composition.value = min->text().toDouble();
					else
						ambient.composition[index].composition.independent.min = min->text().toDouble();
					ambient.composition[index].composition.independent.max = max->text().toDouble();

					if(ambient.composition[index].composition.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(ambient.composition[index].composition.value,thumbnail_double_format,thumbnail_composition_precision) + end_Bracket);
					else
						item->setText(name + " [" + QString::number(ambient.composition[index].composition.independent.num_Points) + " values: " +
									  QString::number(ambient.composition[index].composition.independent.min,thumbnail_double_format,thumbnail_composition_precision) + " - " +
									  QString::number(ambient.composition[index].composition.independent.max,thumbnail_double_format,thumbnail_composition_precision) + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((ambient.composition[index].composition.independent.max-ambient.composition[index].composition.independent.min)/(ambient.composition[index].composition.independent.num_Points-1),line_edit_double_format,line_edit_composition_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(ambient.composition[index].composition.independent.num_Points, show);
		}

		var.setValue(ambient);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	// if layer
	if(whats_This_List_Type[0] == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		/// optical constants

		// layer absolute density
		if(whats_This_List[wtl_index] == whats_This_Absolute_Density)
		{
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + whats_This_List[wtl_index] + ", " + Rho_Sym;
			group_Box->setTitle(name);
			units = density_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.absolute_Density.independent.num_Points));
				if(layer.absolute_Density.independent.num_Points == 1)
					min->setText(	QString::number(layer.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
				else
					min->setText(	QString::number(layer.absolute_Density.independent.min,line_edit_double_format,line_edit_density_precision));
				max->setText(		QString::number(layer.absolute_Density.independent.max,line_edit_double_format,line_edit_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(layer.absolute_Density.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					layer.absolute_Density.independent.num_Points = num_Points->text().toInt();
					if(layer.absolute_Density.independent.num_Points == 1)
						layer.absolute_Density.value = min->text().toDouble();
					else
						layer.absolute_Density.independent.min = min->text().toDouble();
					layer.absolute_Density.independent.max = max->text().toDouble();

					if(layer.absolute_Density.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(layer.absolute_Density.independent.num_Points) + " values: " +
									  QString::number(layer.absolute_Density.independent.min,thumbnail_double_format,thumbnail_density_precision) + " - " +
									  QString::number(layer.absolute_Density.independent.max,thumbnail_double_format,thumbnail_density_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((layer.absolute_Density.independent.max-layer.absolute_Density.independent.min)/(layer.absolute_Density.independent.num_Points-1),line_edit_double_format,line_edit_density_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(layer.absolute_Density.independent.num_Points, show);
		}
		// layer relative density
		if(whats_This_List[wtl_index] == whats_This_Relative_Density)
		{
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + whats_This_List[wtl_index] + ", " + Rho_Sym;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.relative_Density.independent.num_Points));
				if(layer.relative_Density.independent.num_Points == 1)
					min->setText(	QString::number(layer.relative_Density.value,line_edit_double_format,line_edit_density_precision));
				else
					min->setText(	QString::number(layer.relative_Density.independent.min,line_edit_double_format,line_edit_density_precision));
				max->setText(		QString::number(layer.relative_Density.independent.max,line_edit_double_format,line_edit_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(layer.relative_Density.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					layer.relative_Density.independent.num_Points = num_Points->text().toInt();
					if(layer.relative_Density.independent.num_Points == 1)
						layer.relative_Density.value = min->text().toDouble();
					else
						layer.relative_Density.independent.min = min->text().toDouble();
					layer.relative_Density.independent.max = max->text().toDouble();

					if(layer.relative_Density.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.relative_Density.value,thumbnail_double_format,thumbnail_density_precision) + end_Bracket);
					else
						item->setText(name + " [" + QString::number(layer.relative_Density.independent.num_Points) + " values: " +
									  QString::number(layer.relative_Density.independent.min,thumbnail_double_format,thumbnail_density_precision) + " - " +
									  QString::number(layer.relative_Density.independent.max,thumbnail_double_format,thumbnail_density_precision) + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((layer.relative_Density.independent.max-layer.relative_Density.independent.min)/(layer.relative_Density.independent.num_Points-1),line_edit_double_format,line_edit_density_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(layer.relative_Density.independent.num_Points, show);
		}
		// layer permittivity
		if(whats_This_List[wtl_index] == whats_This_Permittivity)
		{
			min->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
			max->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));

			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + whats_This_List[wtl_index] + ", " + "1-" + Epsilon_Sym;
			group_Box->setTitle(name);
			units = opt_const_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.permittivity.independent.num_Points));
				if(layer.permittivity.independent.num_Points == 1)
					min->setText(	QString::number(layer.permittivity.value,line_edit_double_format,line_edit_permittivity_precision));
				else
					min->setText(	QString::number(layer.permittivity.independent.min,line_edit_double_format,line_edit_permittivity_precision));
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
					if(layer.permittivity.independent.num_Points == 1)
						layer.permittivity.value = min->text().toDouble();
					else
						layer.permittivity.independent.min = min->text().toDouble();
					layer.permittivity.independent.max = max->text().toDouble();

					if(layer.permittivity.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.permittivity.value,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(layer.permittivity.independent.num_Points) + " values: " +
									  QString::number(layer.permittivity.independent.min,thumbnail_double_format,thumbnail_permittivity_precision) + " - " +
									  QString::number(layer.permittivity.independent.max,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((layer.permittivity.independent.max-layer.permittivity.independent.min)/(layer.permittivity.independent.num_Points-1),line_edit_double_format,line_edit_permittivity_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(layer.permittivity.independent.num_Points, show);
		}
		// layer absorption
		if(whats_This_List[wtl_index] == whats_This_Absorption)
		{
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + whats_This_List[wtl_index] + ", " + Cappa_Sym;
			group_Box->setTitle(name);
			units = opt_const_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.absorption.independent.num_Points));
				if(layer.absorption.independent.num_Points == 1)
					min->setText(	QString::number(layer.absorption.value,line_edit_double_format,line_edit_absorption_precision));
				else
					min->setText(	QString::number(layer.absorption.independent.min,line_edit_double_format,line_edit_absorption_precision));
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
					if(layer.absorption.independent.num_Points == 1)
						layer.absorption.value = min->text().toDouble();
					else
						layer.absorption.independent.min = min->text().toDouble();
					layer.absorption.independent.max = max->text().toDouble();

					if(layer.absorption.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.absorption.value,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(layer.absorption.independent.num_Points) + " values: " +
									  QString::number(layer.absorption.independent.min,thumbnail_double_format,thumbnail_absorption_precision) + " - " +
									  QString::number(layer.absorption.independent.max,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((layer.absorption.independent.max-layer.absorption.independent.min)/(layer.absorption.independent.num_Points-1),line_edit_double_format,line_edit_absorption_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(layer.absorption.independent.num_Points, show);
		}
		// layer composition
		if(whats_This_List[wtl_index] == whats_This_Composition)
		{
			int index = QString(whats_This_List[wtl_index+1]).toInt();
			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + layer.composition[index].type + " " + whats_This_List[wtl_index] + ", " + Zeta_Sym + "_" + layer.composition[index].type;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.composition[index].composition.independent.num_Points));
				if(layer.composition[index].composition.independent.num_Points == 1)
					min->setText(	QString::number(layer.composition[index].composition.value,line_edit_double_format,line_edit_composition_precision));
				else
					min->setText(	QString::number(layer.composition[index].composition.independent.min,line_edit_double_format,line_edit_composition_precision));
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
					if(layer.composition[index].composition.independent.num_Points == 1)
						layer.composition[index].composition.value = min->text().toDouble();
					else
						layer.composition[index].composition.independent.min = min->text().toDouble();
					layer.composition[index].composition.independent.max = max->text().toDouble();

					if(layer.composition[index].composition.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.composition[index].composition.value,thumbnail_double_format,thumbnail_composition_precision) + end_Bracket);
					else
						item->setText(name + " [" + QString::number(layer.composition[index].composition.independent.num_Points) + " values: " +
									  QString::number(layer.composition[index].composition.independent.min,thumbnail_double_format,thumbnail_composition_precision) + " - " +
									  QString::number(layer.composition[index].composition.independent.max,thumbnail_double_format,thumbnail_composition_precision) + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((layer.composition[index].composition.independent.max-layer.composition[index].composition.independent.min)/(layer.composition[index].composition.independent.num_Points-1),line_edit_double_format,line_edit_composition_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(layer.composition[index].composition.independent.num_Points, show);
		}

		/// thickness parameters

		// layer thickness
		if(whats_This_List[wtl_index] == whats_This_Thickness)
		{
			double coeff = length_Coefficients_Map.value(length_units);

			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + whats_This_List[wtl_index] + ", z";
			group_Box->setTitle(name);
			units = length_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.thickness.independent.num_Points));
				if(layer.thickness.independent.num_Points == 1)
					min->setText(	QString::number(layer.thickness.value/coeff,line_edit_double_format,line_edit_thickness_precision));
				else
					min->setText(	QString::number(layer.thickness.independent.min/coeff,line_edit_double_format,line_edit_thickness_precision));
				max->setText(		QString::number(layer.thickness.independent.max/coeff,line_edit_double_format,line_edit_thickness_precision));
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
					if(layer.thickness.independent.num_Points == 1)
						layer.thickness.value = min->text().toDouble()*coeff;
					else
						layer.thickness.independent.min = min->text().toDouble()*coeff;
					layer.thickness.independent.max = max->text().toDouble()*coeff;

					if(layer.thickness.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.thickness.value/coeff,thumbnail_double_format,thumbnail_thickness_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(layer.thickness.independent.num_Points) + " values: " +
									  QString::number(layer.thickness.independent.min/coeff,thumbnail_double_format,thumbnail_thickness_precision) + " - " +
									  QString::number(layer.thickness.independent.max/coeff,thumbnail_double_format,thumbnail_thickness_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((layer.thickness.independent.max-layer.thickness.independent.min)/(layer.thickness.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_thickness_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(layer.thickness.independent.num_Points, show);
		}

		/// interface parameters

		// layer sigma
		if(whats_This_List[wtl_index] == whats_This_Sigma)
		{
			double coeff = length_Coefficients_Map.value(length_units);

			name = layer.material + " (layer " +  QString::number(layer.layer_Index) + ") " + "Roughness/Diffuseness, " + Sigma_Sym;
			group_Box->setTitle(name);
			units = length_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(layer.sigma.independent.num_Points));
				if(layer.sigma.independent.num_Points == 1)
					min->setText(	QString::number(layer.sigma.value/coeff,line_edit_double_format,line_edit_sigma_precision));
				else
					min->setText(	QString::number(layer.sigma.independent.min/coeff,line_edit_double_format,line_edit_sigma_precision));
				max->setText(		QString::number(layer.sigma.independent.max/coeff,line_edit_double_format,line_edit_sigma_precision));
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
					if(layer.sigma.independent.num_Points == 1)
						layer.sigma.value = min->text().toDouble()*coeff;
					else
						layer.sigma.independent.min = min->text().toDouble()*coeff;
					layer.sigma.independent.max = max->text().toDouble()*coeff;

					if(layer.sigma.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.sigma.value/coeff,thumbnail_double_format,thumbnail_sigma_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(layer.sigma.independent.num_Points) + " values: " +
									  QString::number(layer.sigma.independent.min/coeff,thumbnail_double_format,thumbnail_sigma_precision) + " - " +
									  QString::number(layer.sigma.independent.max/coeff,thumbnail_double_format,thumbnail_sigma_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((layer.sigma.independent.max-layer.sigma.independent.min)/(layer.sigma.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_sigma_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(layer.sigma.independent.num_Points, show);
		}
		// layer interlayer composition (if enabled and >=2 elements)
		if(whats_This_List[wtl_index] == whats_This_Interlayer_Composition)
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
				if(layer.interlayer_Composition[index].interlayer.independent.num_Points == 1)
					min->setText(	QString::number(layer.interlayer_Composition[index].interlayer.value,line_edit_double_format,line_edit_interlayer_precision));
				else
					min->setText(	QString::number(layer.interlayer_Composition[index].interlayer.independent.min,line_edit_double_format,line_edit_interlayer_precision));
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
					if(layer.interlayer_Composition[index].interlayer.independent.num_Points == 1)
						layer.interlayer_Composition[index].interlayer.value = min->text().toDouble();
					else
						layer.interlayer_Composition[index].interlayer.independent.min = min->text().toDouble();
					layer.interlayer_Composition[index].interlayer.independent.max = max->text().toDouble();

					if(layer.interlayer_Composition[index].interlayer.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(layer.interlayer_Composition[index].interlayer.value,thumbnail_double_format,thumbnail_interlayer_precision) + end_Bracket);
					else
						item->setText(name + " [" + QString::number(layer.interlayer_Composition[index].interlayer.independent.num_Points) + " values: " +
									  QString::number(layer.interlayer_Composition[index].interlayer.independent.min,thumbnail_double_format,thumbnail_interlayer_precision) + " - " +
									  QString::number(layer.interlayer_Composition[index].interlayer.independent.max,thumbnail_double_format,thumbnail_interlayer_precision) + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((layer.interlayer_Composition[index].interlayer.independent.max-layer.interlayer_Composition[index].interlayer.independent.min)/(layer.interlayer_Composition[index].interlayer.independent.num_Points-1),line_edit_double_format,line_edit_interlayer_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(layer.interlayer_Composition[index].interlayer.independent.num_Points, show);
		}

		var.setValue(layer);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	// if multilayer
	if(whats_This_List_Type[0] == whats_This_Multilayer)
	{
		Stack_Content stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();

		// multilayer num_repetitions
		if(whats_This_List[wtl_index] == whats_This_Num_Repetitions)
		{			
			name = whats_This_List[wtl_index-1] + " Number of repetitions, N";
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
				if(stack_Content.num_Repetition.num_steps == 1)
					min->setText(	QString::number(stack_Content.num_Repetition.value));
				else
					min->setText(	QString::number(stack_Content.num_Repetition.start));
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
					if(stack_Content.num_Repetition.num_steps == 1)
						stack_Content.num_Repetition.value = min->text().toDouble();
					else
						stack_Content.num_Repetition.start = min->text().toDouble();
					stack_Content.num_Repetition.step  = max->text().toDouble();

					if(stack_Content.num_Repetition.num_steps == 1)
					{
						item->setText(name + " [" + QString::number(stack_Content.num_Repetition.value) + end_Bracket);
					}
					else
						item->setText(name + " [" + QString::number(stack_Content.num_Repetition.num_steps) + " values: " +
									  QString::number(stack_Content.num_Repetition.start) + " - " +
									  QString::number(stack_Content.num_Repetition.start + stack_Content.num_Repetition.step*stack_Content.num_Repetition.num_steps) + end_Bracket);
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
		if(whats_This_List[wtl_index] == whats_This_Period)
		{
			double coeff = length_Coefficients_Map.value(length_units);

			name = whats_This_List[wtl_index-1] + " " + whats_This_List[wtl_index] + ", d";
			group_Box->setTitle(name);
			units = length_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(stack_Content.period.independent.num_Points));
				if(stack_Content.period.independent.num_Points == 1)
					min->setText(	QString::number(stack_Content.period.value/coeff,line_edit_double_format,line_edit_period_precision));
				else
					min->setText(	QString::number(stack_Content.period.independent.min/coeff,line_edit_double_format,line_edit_period_precision));
				max->setText(		QString::number(stack_Content.period.independent.max/coeff,line_edit_double_format,line_edit_period_precision));
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
					if(stack_Content.period.independent.num_Points == 1)
						stack_Content.period.value = min->text().toDouble()*coeff;
					else
						stack_Content.period.independent.min = min->text().toDouble()*coeff;
					stack_Content.period.independent.max = max->text().toDouble()*coeff;

					if(stack_Content.period.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(stack_Content.period.value/coeff,thumbnail_double_format,thumbnail_period_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(stack_Content.period.independent.num_Points) + " values: " +
									  QString::number(stack_Content.period.independent.min/coeff,thumbnail_double_format,thumbnail_period_precision) + " - " +
									  QString::number(stack_Content.period.independent.max/coeff,thumbnail_double_format,thumbnail_period_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((stack_Content.period.independent.max-stack_Content.period.independent.min)/(stack_Content.period.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_period_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(stack_Content.period.independent.num_Points, show);
		}
		// multilayer gamma
		if(whats_This_List[wtl_index] == whats_This_Gamma)
		{
			name = whats_This_List[wtl_index-1] + " Thickness Ratio, " + Gamma_Sym;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(stack_Content.gamma.independent.num_Points));
				if(stack_Content.gamma.independent.num_Points == 1)
					min->setText(	QString::number(stack_Content.gamma.value,line_edit_double_format,line_edit_gamma_precision));
				else
					min->setText(	QString::number(stack_Content.gamma.independent.min,line_edit_double_format,line_edit_gamma_precision));
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
					if(stack_Content.gamma.independent.num_Points == 1)
						min->setText(QString::number(stack_Content.gamma.value,line_edit_double_format,line_edit_gamma_precision));
					else
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
					if(stack_Content.gamma.independent.num_Points == 1)
						stack_Content.gamma.value = min->text().toDouble();
					else
						stack_Content.gamma.independent.min = min->text().toDouble();
					stack_Content.gamma.independent.max = max->text().toDouble();

					if(stack_Content.gamma.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(stack_Content.gamma.value,thumbnail_double_format,thumbnail_gamma_precision) + end_Bracket);
					else
						item->setText(name + " [" + QString::number(stack_Content.gamma.independent.num_Points) + " values: " +
									  QString::number(stack_Content.gamma.independent.min,thumbnail_double_format,thumbnail_gamma_precision) + " - " +
									  QString::number(stack_Content.gamma.independent.max,thumbnail_double_format,thumbnail_gamma_precision) + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((stack_Content.gamma.independent.max-stack_Content.gamma.independent.min)/(stack_Content.gamma.independent.num_Points-1),line_edit_double_format,line_edit_gamma_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(stack_Content.gamma.independent.num_Points, show);
		}

		var.setValue(stack_Content);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	// if substrate
	if(whats_This_List_Type[0] == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		/// optical constants

		// substrate absolute density
		if(whats_This_List[wtl_index] == whats_This_Absolute_Density)
		{
			name = substrate.material + " (substrate) " + whats_This_List[wtl_index] + ", " + Rho_Sym;
			group_Box->setTitle(name);
			units = density_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.absolute_Density.independent.num_Points));
				if(substrate.absolute_Density.independent.num_Points == 1)
					min->setText(	QString::number(substrate.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
				else
					min->setText(	QString::number(substrate.absolute_Density.independent.min,line_edit_double_format,line_edit_density_precision));
				max->setText(		QString::number(substrate.absolute_Density.independent.max,line_edit_double_format,line_edit_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(substrate.absolute_Density.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					substrate.absolute_Density.independent.num_Points = num_Points->text().toInt();
					if(substrate.absolute_Density.independent.num_Points == 1)
						substrate.absolute_Density.value = min->text().toDouble();
					else
						substrate.absolute_Density.independent.min = min->text().toDouble();
					substrate.absolute_Density.independent.max = max->text().toDouble();

					if(substrate.absolute_Density.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.absolute_Density.value,thumbnail_double_format,thumbnail_density_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(substrate.absolute_Density.independent.num_Points) + " values: " +
									  QString::number(substrate.absolute_Density.independent.min,thumbnail_double_format,thumbnail_density_precision) + " - " +
									  QString::number(substrate.absolute_Density.independent.max,thumbnail_double_format,thumbnail_density_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((substrate.absolute_Density.independent.max-substrate.absolute_Density.independent.min)/(substrate.absolute_Density.independent.num_Points-1),line_edit_double_format,line_edit_density_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(substrate.absolute_Density.independent.num_Points, show);
		}
		// substrate relative density
		if(whats_This_List[wtl_index] == whats_This_Relative_Density)
		{
			name = substrate.material + " (substrate) " + whats_This_List[wtl_index] + ", " + Rho_Sym;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.relative_Density.independent.num_Points));
				if(substrate.relative_Density.independent.num_Points == 1)
					min->setText(	QString::number(substrate.relative_Density.value,line_edit_double_format,line_edit_density_precision));
				else
					min->setText(	QString::number(substrate.relative_Density.independent.min,line_edit_double_format,line_edit_density_precision));
				max->setText(		QString::number(substrate.relative_Density.independent.max,line_edit_double_format,line_edit_density_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(substrate.relative_Density.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				{
					substrate.relative_Density.independent.num_Points = num_Points->text().toInt();
					if(substrate.relative_Density.independent.num_Points == 1)
						substrate.relative_Density.value = min->text().toDouble();
					else
						substrate.relative_Density.independent.min = min->text().toDouble();
					substrate.relative_Density.independent.max = max->text().toDouble();

					if(substrate.relative_Density.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.relative_Density.value,thumbnail_double_format,thumbnail_density_precision) + end_Bracket);
					else
						item->setText(name + " [" + QString::number(substrate.relative_Density.independent.num_Points) + " values: " +
									  QString::number(substrate.relative_Density.independent.min,thumbnail_double_format,thumbnail_density_precision) + " - " +
									  QString::number(substrate.relative_Density.independent.max,thumbnail_double_format,thumbnail_density_precision) + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((substrate.relative_Density.independent.max-substrate.relative_Density.independent.min)/(substrate.relative_Density.independent.num_Points-1),line_edit_double_format,line_edit_density_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(substrate.relative_Density.independent.num_Points, show);
		}
		// substrate permittivity
		if(whats_This_List[wtl_index] == whats_This_Permittivity)
		{
			min->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
			max->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));

			name = substrate.material + " (substrate) " + whats_This_List[wtl_index] + ", " + "1-" + Epsilon_Sym;
			group_Box->setTitle(name);
			units = opt_const_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.permittivity.independent.num_Points));
				if(substrate.permittivity.independent.num_Points == 1)
					min->setText(	QString::number(substrate.permittivity.value,line_edit_double_format,line_edit_permittivity_precision));
				else
					min->setText(	QString::number(substrate.permittivity.independent.min,line_edit_double_format,line_edit_permittivity_precision));
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
					if(substrate.permittivity.independent.num_Points == 1)
						substrate.permittivity.value = min->text().toDouble();
					else
						substrate.permittivity.independent.min = min->text().toDouble();
					substrate.permittivity.independent.max = max->text().toDouble();

					if(substrate.permittivity.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.permittivity.value,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(substrate.permittivity.independent.num_Points) + " values: " +
									  QString::number(substrate.permittivity.independent.min,thumbnail_double_format,thumbnail_permittivity_precision) + " - " +
									  QString::number(substrate.permittivity.independent.max,thumbnail_double_format,thumbnail_permittivity_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((substrate.permittivity.independent.max-substrate.permittivity.independent.min)/(substrate.permittivity.independent.num_Points-1),line_edit_double_format,line_edit_permittivity_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(substrate.permittivity.independent.num_Points, show);
		}
		// substrate absorption
		if(whats_This_List[wtl_index] == whats_This_Absorption)
		{
			name = substrate.material + " (substrate) " + whats_This_List[wtl_index] + ", " + Cappa_Sym;
			group_Box->setTitle(name);
			units = opt_const_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.absorption.independent.num_Points));
				if(substrate.absorption.independent.num_Points == 1)
					min->setText(	QString::number(substrate.absorption.value,line_edit_double_format,line_edit_absorption_precision));
				else
					min->setText(	QString::number(substrate.absorption.independent.min,line_edit_double_format,line_edit_absorption_precision));
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
					if(substrate.absorption.independent.num_Points == 1)
						substrate.absorption.value = min->text().toDouble();
					else
						substrate.absorption.independent.min = min->text().toDouble();
					substrate.absorption.independent.max = max->text().toDouble();

					if(substrate.absorption.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.absorption.value,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(substrate.absorption.independent.num_Points) + " values: " +
									  QString::number(substrate.absorption.independent.min,thumbnail_double_format,thumbnail_absorption_precision) + " - " +
									  QString::number(substrate.absorption.independent.max,thumbnail_double_format,thumbnail_absorption_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((substrate.absorption.independent.max-substrate.absorption.independent.min)/(substrate.absorption.independent.num_Points-1),line_edit_double_format,line_edit_absorption_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(substrate.absorption.independent.num_Points, show);
		}
		// substrate composition
		if(whats_This_List[wtl_index] == whats_This_Composition)
		{
			int index = QString(whats_This_List[wtl_index+1]).toInt();
			name = substrate.material + " (substrate) " + substrate.composition[index].type + " " + whats_This_List[wtl_index] + ", " + Zeta_Sym;
			group_Box->setTitle(name);
			units = "";
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.composition[index].composition.independent.num_Points));
				if(substrate.composition[index].composition.independent.num_Points == 1)
					min->setText(	QString::number(substrate.composition[index].composition.value,line_edit_double_format,line_edit_composition_precision));
				else
					min->setText(	QString::number(substrate.composition[index].composition.independent.min,line_edit_double_format,line_edit_composition_precision));
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
					if(substrate.composition[index].composition.independent.num_Points == 1)
						substrate.composition[index].composition.value = min->text().toDouble();
					else
						substrate.composition[index].composition.independent.min = min->text().toDouble();
					substrate.composition[index].composition.independent.max = max->text().toDouble();

					if(substrate.composition[index].composition.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.composition[index].composition.value,thumbnail_double_format,thumbnail_composition_precision) + end_Bracket);
					else
						item->setText(name + " [" + QString::number(substrate.composition[index].composition.independent.num_Points) + " values: " +
									  QString::number(substrate.composition[index].composition.independent.min,thumbnail_double_format,thumbnail_composition_precision) + " - " +
									  QString::number(substrate.composition[index].composition.independent.max,thumbnail_double_format,thumbnail_composition_precision) + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((substrate.composition[index].composition.independent.max-substrate.composition[index].composition.independent.min)/(substrate.composition[index].composition.independent.num_Points-1),line_edit_double_format,line_edit_composition_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(substrate.composition[index].composition.independent.num_Points, show);
		}

		/// interface parameters

		// substrate sigma
		if(whats_This_List[wtl_index] == whats_This_Sigma)
		{
			double coeff = length_Coefficients_Map.value(length_units);

			name = substrate.material + " (substrate) " + "Roughness/Diffuseness, " + Sigma_Sym;
			group_Box->setTitle(name);
			units = length_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(substrate.sigma.independent.num_Points));
				if(substrate.sigma.independent.num_Points == 1)
					min->setText(	QString::number(substrate.sigma.value/coeff,line_edit_double_format,line_edit_sigma_precision));
				else
					min->setText(	QString::number(substrate.sigma.independent.min/coeff,line_edit_double_format,line_edit_sigma_precision));
				max->setText(		QString::number(substrate.sigma.independent.max/coeff,line_edit_double_format,line_edit_sigma_precision));
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
					if(substrate.sigma.independent.num_Points == 1)
						substrate.sigma.value = min->text().toDouble()*coeff;
					else
						substrate.sigma.independent.min = min->text().toDouble()*coeff;
					substrate.sigma.independent.max = max->text().toDouble()*coeff;

					if(substrate.sigma.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.sigma.value/coeff,thumbnail_double_format,thumbnail_sigma_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(substrate.sigma.independent.num_Points) + " values: " +
									  QString::number(substrate.sigma.independent.min/coeff,thumbnail_double_format,thumbnail_sigma_precision) + " - " +
									  QString::number(substrate.sigma.independent.max/coeff,thumbnail_double_format,thumbnail_sigma_precision) + " " + units + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((substrate.sigma.independent.max-substrate.sigma.independent.min)/(substrate.sigma.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_sigma_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(substrate.sigma.independent.num_Points, show);
		}
		// substrate interlayer composition (if enabled and >=2 elements)
		if(whats_This_List[wtl_index] == whats_This_Interlayer_Composition)
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
				if(substrate.interlayer_Composition[index].interlayer.independent.num_Points == 1)
					min->setText(	QString::number(substrate.interlayer_Composition[index].interlayer.value,line_edit_double_format,line_edit_interlayer_precision));
				else
					min->setText(	QString::number(substrate.interlayer_Composition[index].interlayer.independent.min,line_edit_double_format,line_edit_interlayer_precision));
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
					if(substrate.interlayer_Composition[index].interlayer.independent.num_Points == 1)
						substrate.interlayer_Composition[index].interlayer.value = min->text().toDouble();
					else
						substrate.interlayer_Composition[index].interlayer.independent.min = min->text().toDouble();
					substrate.interlayer_Composition[index].interlayer.independent.max = max->text().toDouble();

					if(substrate.interlayer_Composition[index].interlayer.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(substrate.interlayer_Composition[index].interlayer.value,thumbnail_double_format,thumbnail_interlayer_precision) + end_Bracket);
					else
						item->setText(name + " [" + QString::number(substrate.interlayer_Composition[index].interlayer.independent.num_Points) + " values: " +
									  QString::number(substrate.interlayer_Composition[index].interlayer.independent.min,thumbnail_double_format,thumbnail_interlayer_precision) + " - " +
									  QString::number(substrate.interlayer_Composition[index].interlayer.independent.max,thumbnail_double_format,thumbnail_interlayer_precision) + end_Bracket);
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((substrate.interlayer_Composition[index].interlayer.independent.max-substrate.interlayer_Composition[index].interlayer.independent.min)/(substrate.interlayer_Composition[index].interlayer.independent.num_Points-1),line_edit_double_format,line_edit_interlayer_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(substrate.interlayer_Composition[index].interlayer.independent.num_Points, show);
		}

		var.setValue(substrate);
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	// if measurement
	if(whats_This_List_Type[0] == whats_This_Measurement)
	{
		Measurement measurement = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();

		// if angle
		if(angle_Done)
		if(whats_This_List[wtl_index] == whats_This_Angle)
		{
			double coeff = angle_Coefficients_Map.value(angle_units);

			name = measurement.angle_Type + " angle, " + Theta_Sym;
			group_Box->setTitle(name);
			units = angle_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);
			angle_Units_Label->setText(units);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(measurement.probe_Angle.independent.num_Points));
				if(measurement.probe_Angle.independent.num_Points == 1)
					min->setText(	QString::number(measurement.probe_Angle.value/coeff,line_edit_double_format,line_edit_angle_precision));
				else
					min->setText(	QString::number(measurement.probe_Angle.independent.min/coeff,line_edit_double_format,line_edit_angle_precision));
				max->setText(		QString::number(measurement.probe_Angle.independent.max/coeff,line_edit_double_format,line_edit_angle_precision));

//				if(measurement.probe_Angle.independent.num_Points >= MIN_ANGULAR_RESOLUTION_POINTS)
					angular_Resolution_Edit->setText(QString::number(measurement.angular_Resolution.value/coeff,line_edit_double_format,line_edit_angle_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(measurement.probe_Angle.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				} else
				if(min->text().toDouble()*coeff>90)
				{
					if(measurement.probe_Angle.independent.num_Points == 1)
						min->setText(QString::number(measurement.probe_Angle.value/coeff,line_edit_double_format,line_edit_angle_precision));
					else
						min->setText(QString::number(measurement.probe_Angle.independent.min/coeff,line_edit_double_format,line_edit_angle_precision));
					min->textEdited(min->text());
				} else
				if(max->text().toDouble()*coeff>90)
				{
					max->setText(QString::number(measurement.probe_Angle.independent.max/coeff,line_edit_double_format,line_edit_angle_precision));
					max->textEdited(max->text());
				} else
				{
					measurement.probe_Angle.independent.num_Points = num_Points->text().toInt();
					if(measurement.probe_Angle.independent.num_Points == 1)
						measurement.probe_Angle.value = min->text().toDouble()*coeff;
					else
						measurement.probe_Angle.independent.min = min->text().toDouble()*coeff;
					measurement.probe_Angle.independent.max = max->text().toDouble()*coeff;

					if(measurement.probe_Angle.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(measurement.probe_Angle.value/coeff,thumbnail_double_format,thumbnail_angle_precision) + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(measurement.probe_Angle.independent.num_Points) + " values: " +
									  QString::number(measurement.probe_Angle.independent.min/coeff,thumbnail_double_format,thumbnail_angle_precision) + " - " +
									  QString::number(measurement.probe_Angle.independent.max/coeff,thumbnail_double_format,thumbnail_angle_precision)  + units + end_Bracket);

					if(measurement.probe_Angle.independent.num_Points >= MIN_ANGULAR_RESOLUTION_POINTS)
						measurement.angular_Resolution.value = angular_Resolution_Edit->text().toDouble()*coeff;
					// TODO decide reset to default or not
	//				else
	//					measurement.angular_Resolution.value = default_angular_resolution*coeff;
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((measurement.probe_Angle.independent.max-measurement.probe_Angle.independent.min)/(measurement.probe_Angle.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_angle_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(measurement.probe_Angle.independent.num_Points, show);
			show_Hide_Angular_Elements(measurement.probe_Angle.independent.num_Points, show);

			var.setValue(measurement);
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if wavelength
		if(wavelength_Done)
		if(whats_This_List[wtl_index] == whats_This_Wavelength)
		{
			double coeff = wavelength_Coefficients_Map.value(wavelength_units);

			name = Global_Variables::wavelength_Energy_Name(wavelength_units);
			group_Box->setTitle(name);
			units = wavelength_units;
			units_Label->setText(units);
			step_Units_Label->setText(units);
			wavelength_Units_Label->setText(units);
			spectral_Resolution_Label->setText("Spectral Resolution, " + Delta_Big_Sym + name[name.size()-1]);

			// show data
			if(show)
			{
				num_Points->setText(QString::number(measurement.wavelength.independent.num_Points));
				if(measurement.wavelength.independent.num_Points == 1)
					min->setText(	QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.value)/coeff,line_edit_double_format,line_edit_wavelength_precision));
				else
					min->setText(	QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.min)/coeff,line_edit_double_format,line_edit_wavelength_precision));
				max->setText(		QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.max)/coeff,line_edit_double_format,line_edit_wavelength_precision));

				polarization_Edit->setText(QString::number(measurement.polarization.value/coeff,line_edit_double_format,line_edit_wavelength_precision));
				polarization_Edit->textEdited(polarization_Edit->text());
				analyzer_Edit->setText(QString::number(measurement.polarization_Sensitivity.value/coeff,line_edit_double_format,line_edit_wavelength_precision));
				analyzer_Edit->textEdited(analyzer_Edit->text());

//				if(measurement.wavelength.independent.num_Points >= MIN_SPECTRAL_RESOLUTION_POINTS)
					spectral_Resolution_Edit->setText(QString::number(measurement.spectral_Resolution.value/coeff,line_edit_double_format,line_edit_wavelength_precision));
			} else
			// refresh data
			{
				if(num_Points->text().toInt()<1)
				{
					num_Points->setText(QString::number(measurement.wavelength.independent.num_Points));
					num_Points->textEdited(num_Points->text());
				}
				// don't uncomment
	/*			else
				if(min->text().toDouble()==0)
				{
					if(measurement.wavelength.independent.num_Points == 1)
						min->setText(QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.value)/coeff,line_edit_double_format,line_edit_wavelength_precision));
					else
						min->setText(QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.min)/coeff,line_edit_double_format,line_edit_wavelength_precision));
					min->textEdited(min->text());
				} else
				if(max->text().toDouble()==0)
				{
					max->setText(QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.max)/coeff,line_edit_double_format,line_edit_wavelength_precision));
					max->textEdited(max->text());
				} else*/
				if(polarization_Edit->text().toDouble()>1)
				{
					polarization_Edit->setText(QString::number(measurement.polarization.value,line_edit_double_format,line_edit_wavelength_precision));
					polarization_Edit->textEdited(polarization_Edit->text());
				} else
				if(polarization_Edit->text().toDouble()<-1)
				{
					polarization_Edit->setText(QString::number(measurement.polarization.value,line_edit_double_format,line_edit_wavelength_precision));
					polarization_Edit->textEdited(polarization_Edit->text());
				} else
				{
					measurement.wavelength.independent.num_Points = num_Points->text().toInt();
					if(measurement.wavelength.independent.num_Points == 1)
						measurement.wavelength.value = Global_Variables::wavelength_Energy(wavelength_units,min->text().toDouble()*coeff);
					else
						measurement.wavelength.independent.min = Global_Variables::wavelength_Energy(wavelength_units,min->text().toDouble()*coeff);
					measurement.wavelength.independent.max = Global_Variables::wavelength_Energy(wavelength_units,max->text().toDouble()*coeff);

					measurement.polarization.value = polarization_Edit->text().toDouble();
					measurement.polarization_Sensitivity.value = analyzer_Edit->text().toDouble();

					if(measurement.wavelength.independent.num_Points == 1)
						item->setText(name + " [" + QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.value)/coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " " + units + end_Bracket);
					else
						item->setText(name + " [" + QString::number(measurement.wavelength.independent.num_Points) + " values: " +
									  QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.min)/coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " - " +
									  QString::number(Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.max)/coeff,thumbnail_double_format,thumbnail_wavelength_precision)  + " " + units + end_Bracket);

					if(measurement.wavelength.independent.num_Points >= MIN_SPECTRAL_RESOLUTION_POINTS)
						measurement.spectral_Resolution.value = spectral_Resolution_Edit->text().toDouble()*coeff;
					// TODO decide reset to default or not
	//				else
	//					measurement.spectral_Resolution.value = default_spectral_resolution*coeff;
				}
			}
			if(num_Points->text().toInt()>1)
			{
				step_Edit->setText(QString::number((Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.max)-Global_Variables::wavelength_Energy(wavelength_units,measurement.wavelength.independent.min))/(measurement.wavelength.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_wavelength_precision));
				step_Edit->textEdited(step_Edit->text());
			}
			show_Hide_Elements(measurement.wavelength.independent.num_Points, show);
			show_Hide_Spectral_Elements(measurement.wavelength.independent.num_Points, show);

			var.setValue(measurement);
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
	}

	show_Active_Check_Box();
}

void Independent_Variables_Editor::activate_Variable(bool)
{
	// all to passive
	for(int i = 0; i < variables_List->count(); ++i)
	{
		variables_List->item(i)->setData(Qt::UserRole, false);
		QString current_Item_Text = variables_List->item(i)->text();
		QStringList item_Text_List = current_Item_Text.split(active, QString::SkipEmptyParts);
		variables_List->item(i)->setText(item_Text_List[0]);
	}
	// this to active
	item->setData(Qt::UserRole, true);
	item->setText(item->text() + active);
	active_Check_Box->setDisabled(active_Check_Box->isChecked());
}

void Independent_Variables_Editor::show_Active_Check_Box()
{
	active_Check_Box->setChecked(item->data(Qt::UserRole).toBool());
	if(item->data(Qt::UserRole).toBool()) activate_Variable(true);
}

void Independent_Variables_Editor::show_Hide_Angular_Elements(int points, bool show)
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

void Independent_Variables_Editor::show_Hide_Spectral_Elements(int points, bool show)
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
