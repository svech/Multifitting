#include "independent_variables_editor.h"
#include "multilayer_approach/multilayer/independent_variables/variable_selection.h"
#include "multilayer_approach/multilayer_approach.h"

Independent_Variables_Editor::Independent_Variables_Editor(QTreeWidgetItem* structure_Item, QListWidgetItem* list_Item, QListWidget* variables_List, QString& argument_Type, QWidget *parent) :
	structure_Item(structure_Item),
	list_Item(list_Item),
	variables_List(variables_List),
	indicator(list_Item->data(Qt::UserRole).value<Independent_Indicator>()),
	struct_Data(structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>()),
	argument_Type(argument_Type),
	QDialog(parent)
{
	create_Main_Layout();

	setWindowTitle("Independent Variable");
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

Independent_Variables_Editor::~Independent_Variables_Editor()
{
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

	Global_Variables::create_Shortcuts(this);
	create_Menu();
	create_Standard_Interface();
		main_Layout->addWidget(group_Box);

	// if angle
	if(indicator.parameter_Whats_This == whats_This_Beam_Theta_0_Angle)		create_Angle_Interface();
	// if wavelength
	if(indicator.parameter_Whats_This == whats_This_Wavelength)	create_Wavelength_Interface();

	done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		done_Button->setFocus();
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button, &QPushButton::clicked, this, &Independent_Variables_Editor::close);

	refresh_Show_Data(show_Data);
}

void Independent_Variables_Editor::create_Menu()
{
	Menu* menu = new Menu(window_Type_Independent_Variables_Editor, this);
	main_Layout->setMenuBar(menu->menu_Bar);
	connect(menu, &Menu::refresh, this, &Independent_Variables_Editor::refresh_All);
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

		{
			num_Points = new QSpinBox;
				num_Points->setRange(1, MAX_INTEGER);
				num_Points->setFixedWidth(30);
				num_Points->setProperty(min_Size_Property, 30);
				num_Points->setAccelerated(true);
				num_Points->setButtonSymbols(QAbstractSpinBox::NoButtons);

			layout->addWidget(num_Points);
		}
		{
			val_Label = new QLabel(value_Text);
			layout->addWidget(val_Label);

			val_Edit = new QLineEdit;
				val_Edit->setFixedWidth(40);
				val_Edit->setProperty(min_Size_Property, 40);
				val_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			layout->addWidget(val_Edit);
		}
		{
			from_Label = new QLabel(from_Text);
			layout->addWidget(from_Label);

			min_Edit = new QLineEdit;
				min_Edit->setFixedWidth(40);
				min_Edit->setProperty(min_Size_Property, 40);
				min_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			layout->addWidget(min_Edit);
		}
		{
			to_Label = new QLabel(to_Text);
				to_Label->setSizePolicy(sp_retain);
			layout->addWidget(to_Label);

			max_Edit = new QLineEdit;
				max_Edit->setFixedWidth(40);
				max_Edit->setProperty(min_Size_Property, 40);
				max_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
				max_Edit->setSizePolicy(sp_retain);
			layout->addWidget(max_Edit);
		}

		units_Label = new QLabel(units);
			units_Label->setSizePolicy(sp_retain);
		layout->addWidget(units_Label);

		step_Label = new QLabel(step_Text);
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
		layout->addWidget(active_Check_Box);

		// temporary
		if(struct_Data.item_Type != item_Type_Measurement)
		{
			active_Check_Box->hide();
//			num_Points->setReadOnly(true);
			num_Points->setDisabled(true);
		}

		connect(num_Points, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{Global_Variables::resize_Line_Edit(num_Points);});
		connect(val_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(val_Edit);});
		connect(min_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(min_Edit);});
		connect(max_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(max_Edit);});

		connect(num_Points, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{refresh_Show_Data();});
		connect(val_Edit,	&QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
		connect(min_Edit,	&QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
		connect(max_Edit,	&QLineEdit::textEdited, this, [=]{refresh_Show_Data();});

		connect(step_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(step_Edit);});

		connect(active_Check_Box, &QCheckBox::toggled, this, &Independent_Variables_Editor::activate_Variable);
	}
}

void Independent_Variables_Editor::create_Angle_Interface()
{
	QSizePolicy sp_retain;
	sp_retain.setRetainSizeWhenHidden(true);

	QHBoxLayout* layout_Ang_Res = new QHBoxLayout;
		layout_Ang_Res->setAlignment(Qt::AlignLeft);
	group_Box_Layout->addLayout(layout_Ang_Res);

	// angular resolution
	{
		angular_Resolution_Label = new QLabel("Angular resolution");
			angular_Resolution_Label->setSizePolicy(sp_retain);
		layout_Ang_Res->addWidget(angular_Resolution_Label);
		angular_Resolution_Edit = new QLineEdit(Locale.toString(struct_Data.beam_Theta_0_Distribution.FWHM_distribution,line_edit_double_format,line_edit_angle_precision));
			angular_Resolution_Edit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			angular_Resolution_Edit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			angular_Resolution_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			angular_Resolution_Edit->setSizePolicy(sp_retain);
		layout_Ang_Res->addWidget(angular_Resolution_Edit);
		angle_Units_Label = new QLabel(units);
			angle_Units_Label->setSizePolicy(sp_retain);
		layout_Ang_Res->addWidget(angle_Units_Label);

		connect(angular_Resolution_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(angular_Resolution_Edit);});
		connect(angular_Resolution_Edit, &QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
	}

	QGridLayout* geometry_Layout = new QGridLayout;
		geometry_Layout->setAlignment(Qt::AlignLeft);
	group_Box_Layout->addLayout(geometry_Layout);

	// beam size
	{
		beam_Size_Label = new QLabel("Beam size");
		geometry_Layout->addWidget(beam_Size_Label,0,0,Qt::AlignRight);
		beam_Size_LineEdit = new QLineEdit(Locale.toString(struct_Data.beam_Geometry.size,line_edit_double_format,line_edit_beam_size_precision));
			beam_Size_LineEdit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			beam_Size_LineEdit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			beam_Size_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		geometry_Layout->addWidget(beam_Size_LineEdit,0,1,Qt::AlignLeft);
		beam_Size_Units_Label = new QLabel(size_Units_List[mm]);
		geometry_Layout->addWidget(beam_Size_Units_Label,0,2,Qt::AlignLeft);

		connect(beam_Size_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(beam_Size_LineEdit);});
		connect(beam_Size_LineEdit, &QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
	}

	// spacer
	{
		QLabel* spacer_1 = new QLabel("      ");
		geometry_Layout->addWidget(spacer_1,0,3,Qt::AlignRight);
	}

	// sample size
	{
		sample_Size_Label = new QLabel("Sample size");
		geometry_Layout->addWidget(sample_Size_Label,0,4,Qt::AlignRight);
		sample_Size_LineEdit = new QLineEdit(Locale.toString(struct_Data.sample_Geometry.size,line_edit_double_format,line_edit_sample_size_precision));
			sample_Size_LineEdit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			sample_Size_LineEdit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			sample_Size_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		geometry_Layout->addWidget(sample_Size_LineEdit,0,5,Qt::AlignLeft);
		sample_Size_Units_Label = new QLabel(size_Units_List[mm]);
		geometry_Layout->addWidget(sample_Size_Units_Label,0,6,Qt::AlignLeft);

		connect(sample_Size_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(sample_Size_LineEdit);});
		connect(sample_Size_LineEdit, &QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
	}

	// beam spreading
	{
		beam_Profile_Spreading_Label = new QLabel("Beam spreading");
		geometry_Layout->addWidget(beam_Profile_Spreading_Label,1,0,Qt::AlignRight);
		beam_Profile_Spreading_LineEdit = new QLineEdit(Locale.toString(struct_Data.beam_Geometry.smoothing,line_edit_double_format,line_edit_beam_size_precision));
			beam_Profile_Spreading_LineEdit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			beam_Profile_Spreading_LineEdit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			beam_Profile_Spreading_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		geometry_Layout->addWidget(beam_Profile_Spreading_LineEdit,1,1,Qt::AlignLeft);

		connect(beam_Profile_Spreading_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(beam_Profile_Spreading_LineEdit);});
		connect(beam_Profile_Spreading_LineEdit, &QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
	}

	// spacer
	{
		QLabel* spacer_2 = new QLabel("      ");
		geometry_Layout->addWidget(spacer_2,1,3,Qt::AlignRight);
	}

	// sample shift
	{
		sample_Shift_Label = new QLabel("Sample shift");
		geometry_Layout->addWidget(sample_Shift_Label,1,4,Qt::AlignRight);
		sample_Shift_LineEdit = new QLineEdit(Locale.toString(struct_Data.sample_Geometry.x_Position,line_edit_double_format,line_edit_sample_size_precision));
			sample_Shift_LineEdit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			sample_Shift_LineEdit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			sample_Shift_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		geometry_Layout->addWidget(sample_Shift_LineEdit,1,5,Qt::AlignLeft);
		sample_Shift_Units_Label = new QLabel(size_Units_List[mm]);
		geometry_Layout->addWidget(sample_Shift_Units_Label,1,6,Qt::AlignLeft);

		connect(sample_Shift_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(sample_Shift_LineEdit);});
		connect(sample_Shift_LineEdit, &QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
	}
}

void Independent_Variables_Editor::create_Wavelength_Interface()
{
	QSizePolicy sp_retain;
	sp_retain.setRetainSizeWhenHidden(true);

	{
		QHBoxLayout* up_Layout = new QHBoxLayout;
			up_Layout->setAlignment(Qt::AlignLeft);
		group_Box_Layout->addLayout(up_Layout);

		spectral_Resolution_Label = new QLabel("Spectral resolution");
			spectral_Resolution_Label->setSizePolicy(sp_retain);
		up_Layout->addWidget(spectral_Resolution_Label);

		spectral_Resolution_Edit = new QLineEdit;
			spectral_Resolution_Edit->setFixedWidth(40);
			spectral_Resolution_Edit->setProperty(min_Size_Property, 40);
			spectral_Resolution_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			spectral_Resolution_Edit->setSizePolicy(sp_retain);
		up_Layout->addWidget(spectral_Resolution_Edit);
	}
	{
		QHBoxLayout* down_Layout = new QHBoxLayout;
			down_Layout->setAlignment(Qt::AlignLeft);
		group_Box_Layout->addLayout(down_Layout);

		polarization_Label = new QLabel("Polarization");
			polarization_Label->setSizePolicy(sp_retain);
		down_Layout->addWidget(polarization_Label);

		polarization_Edit = new QLineEdit;
			polarization_Edit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			polarization_Edit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			polarization_Edit->setValidator(new QDoubleValidator(-1, 1, MAX_PRECISION, this));
			polarization_Edit->setSizePolicy(sp_retain);
		down_Layout->addWidget(polarization_Edit);

		background_Label = new QLabel("Background");
			background_Label->setSizePolicy(sp_retain);
		down_Layout->addWidget(background_Label);

		background_Edit = new QLineEdit;
			background_Edit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			background_Edit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			background_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			background_Edit->setSizePolicy(sp_retain);
		down_Layout->addWidget(background_Edit);
	}

	connect(spectral_Resolution_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(spectral_Resolution_Edit);});
	connect(spectral_Resolution_Edit, &QLineEdit::textEdited, this, [=]{refresh_Show_Data();});

	connect(polarization_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(polarization_Edit);});
	connect(polarization_Edit, &QLineEdit::textEdited, this, [=]{refresh_Show_Data();});

	connect(background_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(background_Edit);});
	connect(background_Edit, &QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
}

void Independent_Variables_Editor::set_Window_Geometry()
{
	adjustSize();
//	main_Layout->setSizeConstraint( QLayout::SetFixedSize );
}

void Independent_Variables_Editor::show_Hide_Elements()
{
	int points = num_Points->value();

	if(points == 1)
	{
		val_Label->show();
		val_Edit->show();

		from_Label->hide();
		min_Edit->hide();

		to_Label->setText(units);
		max_Edit->hide();

		units_Label->hide();

		step_Label->hide();
		step_Edit->hide();
		step_Units_Label->hide();
	} else
	if(points > 1)
	{
		val_Label->hide();
		val_Edit->hide();

		from_Label->show();
		min_Edit->show();

		to_Label->setText(to_Text);
		to_Label->show();
		max_Edit->show();

		units_Label->show();
		units_Label->setText(units+" ;");

		step_Label->show();
		step_Edit->show();
		step_Units_Label->show();
	}
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Independent_Variables_Editor::refresh_Show_Data(bool show)
{
	// PARAMETER

	int line_edit_precision = line_edit_density_precision;
	int thumbnail_precision = thumbnail_density_precision;
	double coeff = 1; // should be 1 by default!
	Parameter* param_Pointer = Global_Variables::get_Parameter_From_Struct_Item_by_Whats_This(
							   struct_Data,
							   indicator.parameter_Whats_This,
							   &line_edit_precision,
							   &thumbnail_precision,
							   &units,
							   &coeff);

	// special cases
	if(indicator.parameter_Whats_This == whats_This_Permittivity)
	{
		val_Edit->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
		min_Edit->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
		max_Edit->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
	}
	if(indicator.parameter_Whats_This == whats_This_Interlayer_Composition)
	{
		param_Pointer = &struct_Data.interlayer_Composition[indicator.index].interlayer;
	}
	if(indicator.parameter_Whats_This == whats_This_Interlayer_My_Sigma)
	{
		param_Pointer = &struct_Data.interlayer_Composition[indicator.index].my_Sigma;
	}
	if(indicator.parameter_Whats_This == whats_This_Wavelength)
	{
		param_Pointer = nullptr;
	}

	if(param_Pointer)
	{
		Parameter& parameter = (*param_Pointer);
		name = Global_Variables::parameter_Name(struct_Data, indicator.parameter_Whats_This, indicator.index);
		group_Box->setTitle(name);

		units_Label->setText(units);
		step_Units_Label->setText(units);

		if(indicator.parameter_Whats_This == whats_This_Beam_Theta_0_Angle)
		{
			angle_Units_Label->setText(units);
		}

		// show data
		if(show)
		{
			val_Edit->setText(	Locale.toString(parameter.value/coeff,			line_edit_double_format,line_edit_precision));
			min_Edit->setText(	Locale.toString(parameter.independent.min/coeff,line_edit_double_format,line_edit_precision));
			max_Edit->setText(	Locale.toString(parameter.independent.max/coeff,line_edit_double_format,line_edit_precision));

			Global_Variables::resize_Line_Edit(val_Edit);
			Global_Variables::resize_Line_Edit(min_Edit);
			Global_Variables::resize_Line_Edit(max_Edit);

			if(indicator.parameter_Whats_This == whats_This_Beam_Theta_0_Angle)
			{
				angular_Resolution_Edit->		 setText(Locale.toString(struct_Data.beam_Theta_0_Distribution.FWHM_distribution/coeff,line_edit_double_format,line_edit_angle_precision));
				beam_Size_LineEdit->			 setText(Locale.toString(struct_Data.beam_Geometry.size,						 line_edit_double_format,line_edit_beam_size_precision));
				sample_Size_LineEdit->			 setText(Locale.toString(struct_Data.sample_Geometry.size,						 line_edit_double_format,line_edit_sample_size_precision));
				beam_Profile_Spreading_LineEdit->setText(Locale.toString(struct_Data.beam_Geometry.smoothing,					 line_edit_double_format,line_edit_beam_size_precision));
				sample_Shift_LineEdit->			 setText(Locale.toString(struct_Data.sample_Geometry.x_Position,				 line_edit_double_format,line_edit_sample_size_precision));

				Global_Variables::resize_Line_Edit(angular_Resolution_Edit);
				Global_Variables::resize_Line_Edit(beam_Size_LineEdit);
				Global_Variables::resize_Line_Edit(sample_Size_LineEdit);
				Global_Variables::resize_Line_Edit(beam_Profile_Spreading_LineEdit);
				Global_Variables::resize_Line_Edit(sample_Shift_LineEdit);
			}

			// LAST! causes refresh
			num_Points->setValue(parameter.independent.num_Points);
			Global_Variables::resize_Line_Edit(num_Points);
		} else
		// refresh data
		{
			// special cases
			if(indicator.parameter_Whats_This == whats_This_Gamma)
			{
				if(Locale.toDouble(val_Edit->text())>1)
				{
					val_Edit->setText(Locale.toString(parameter.value,line_edit_double_format,line_edit_gamma_precision));
					Global_Variables::resize_Line_Edit(val_Edit);
				}
				if(Locale.toDouble(min_Edit->text())>1)
				{
					min_Edit->setText(Locale.toString(parameter.independent.min,line_edit_double_format,line_edit_gamma_precision));
					Global_Variables::resize_Line_Edit(min_Edit);
				}
				if(Locale.toDouble(max_Edit->text())>1)
				{
					max_Edit->setText(Locale.toString(parameter.independent.max,line_edit_double_format,line_edit_gamma_precision));
					Global_Variables::resize_Line_Edit(max_Edit);
				}
			}
			if(indicator.parameter_Whats_This == whats_This_Beam_Theta_0_Angle)
			{
				if(Locale.toDouble(val_Edit->text())*coeff>90.+3*pow(10.,-line_edit_angle_precision+1))
				{
					val_Edit->setText(Locale.toString(parameter.value/coeff,line_edit_double_format,line_edit_angle_precision));
					Global_Variables::resize_Line_Edit(val_Edit);
				}
				if(Locale.toDouble(min_Edit->text())*coeff>90.+3*pow(10.,-line_edit_angle_precision+1))
				{					
					min_Edit->setText(Locale.toString(parameter.independent.min/coeff,line_edit_double_format,line_edit_angle_precision));
					Global_Variables::resize_Line_Edit(min_Edit);
				}
				if(Locale.toDouble(max_Edit->text())*coeff>90.+3*pow(10.,-line_edit_angle_precision+1))
				{
					max_Edit->setText(Locale.toString(parameter.independent.max/coeff,line_edit_double_format,line_edit_angle_precision));
					Global_Variables::resize_Line_Edit(max_Edit);
				}

				if(parameter.independent.num_Points >= MIN_ANGULAR_RESOLUTION_POINTS)
				{
					struct_Data.beam_Theta_0_Distribution.FWHM_distribution = Locale.toDouble(angular_Resolution_Edit->text())*coeff;
				}
				struct_Data.beam_Geometry.size			= Locale.toDouble(beam_Size_LineEdit->text());
				struct_Data.sample_Geometry.size		= Locale.toDouble(sample_Size_LineEdit->text());
				struct_Data.beam_Geometry.smoothing		= Locale.toDouble(beam_Profile_Spreading_LineEdit->text());
				struct_Data.sample_Geometry.x_Position	= Locale.toDouble(sample_Shift_LineEdit->text());
			}

			parameter.independent.num_Points = num_Points->value();
			parameter.value			  = Locale.toDouble(val_Edit->text())*coeff;
			parameter.independent.min = Locale.toDouble(min_Edit->text())*coeff;
			parameter.independent.max = Locale.toDouble(max_Edit->text())*coeff;

			QVariant var; var.setValue(struct_Data);
			structure_Item->setData(DEFAULT_COLUMN,Qt::UserRole,var);
		}
		if(parameter.independent.num_Points == 1)
			list_Item->setText(name + " [" +Locale.toString(parameter.value/coeff,thumbnail_double_format,thumbnail_precision) + units + end_Bracket_Text);
		else
			list_Item->setText(name + " [" +Locale.toString(parameter.independent.num_Points) + " values: " +
											Locale.toString(parameter.independent.min/coeff,thumbnail_double_format,thumbnail_precision) + " - " +
											Locale.toString(parameter.independent.max/coeff,thumbnail_double_format,thumbnail_precision) + units + end_Bracket_Text);
		if(parameter.independent.num_Points>1)
		{
			step_Edit->setText(Locale.toString((parameter.independent.max-parameter.independent.min)/(parameter.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_precision));
			Global_Variables::resize_Line_Edit(step_Edit);
		}
		show_Hide_Elements();
		if(indicator.parameter_Whats_This == whats_This_Beam_Theta_0_Angle)	show_Hide_Angular_Elements();
	}

	// special cases
	if(indicator.parameter_Whats_This == whats_This_Num_Repetitions)
	{
		name = Global_Variables::parameter_Name(struct_Data, indicator.parameter_Whats_This, indicator.index);
		group_Box->setTitle(name);

		units_Label->setText(units);
		step_Units_Label->setText(units);

		val_Edit->setValidator(new QIntValidator(0, MAX_INTEGER, this));
		min_Edit->setValidator(new QIntValidator(0, MAX_INTEGER, this));
		max_Edit->setValidator(new QIntValidator(1, MAX_INTEGER, this));

		// show data
		if(show)
		{
			val_Edit->setText(	Locale.toString(struct_Data.num_Repetition.value()));
			min_Edit->setText(	Locale.toString(struct_Data.num_Repetition.start));
			max_Edit->setText(	Locale.toString(struct_Data.num_Repetition.step));

			Global_Variables::resize_Line_Edit(val_Edit);
			Global_Variables::resize_Line_Edit(min_Edit);
			Global_Variables::resize_Line_Edit(max_Edit);

			// LAST! causes refresh
			num_Points->setValue(struct_Data.num_Repetition.num_Steps);
			Global_Variables::resize_Line_Edit(num_Points);
		} else
		// refresh data
		{
			if(Locale.toInt(max_Edit->text())<1)
			{
				max_Edit->setText(Locale.toString(struct_Data.num_Repetition.step));
				Global_Variables::resize_Line_Edit(max_Edit);
			}

			struct_Data.num_Repetition.num_Steps		= num_Points->value();
			struct_Data.num_Repetition.parameter.value	= Locale.toDouble(val_Edit->text());
			struct_Data.num_Repetition.start			= Locale.toInt   (min_Edit->text());
			struct_Data.num_Repetition.step				= Locale.toInt   (max_Edit->text());

			QVariant var; var.setValue(struct_Data);
			structure_Item->setData(DEFAULT_COLUMN,Qt::UserRole,var);
		}
		if(struct_Data.num_Repetition.num_Steps == 1)
			list_Item->setText(name + " [" +Locale.toString(struct_Data.num_Repetition.value()) + end_Bracket_Text);
		else
			list_Item->setText(name + " [" +Locale.toString(struct_Data.num_Repetition.num_Steps) + " values: " +
											Locale.toString(struct_Data.num_Repetition.start) + " - " +
											Locale.toString(struct_Data.num_Repetition.start + struct_Data.num_Repetition.step*struct_Data.num_Repetition.num_Steps) + end_Bracket_Text);
		if(struct_Data.num_Repetition.num_Steps > 1)
		{
			from_Label->setText("values; start:");
			to_Label->setText(" step size:");
		}
		step_Edit->hide();
		step_Label->hide();
		step_Edit->hide();
		units_Label->hide();
		active_Check_Box->hide();
		show_Hide_Elements();
	}
	if(indicator.parameter_Whats_This == whats_This_Wavelength)
	{
		units = " " + wavelength_units;
		coeff = wavelength_Coefficients_Map.value(wavelength_units);

		name = Global_Variables::parameter_Name(struct_Data, indicator.parameter_Whats_This, indicator.index);
		group_Box->setTitle(name);

		units_Label->setText(units);
		step_Units_Label->setText(units);
		spectral_Resolution_Label->setText("Spectral resolution");

		// show data
		if(show)
		{
			val_Edit->setText(	Locale.toString(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.value)/coeff,line_edit_double_format,line_edit_wavelength_precision));
			min_Edit->setText(	Locale.toString(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.min)/coeff,line_edit_double_format,line_edit_wavelength_precision));
			max_Edit->setText(	Locale.toString(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.max)/coeff,line_edit_double_format,line_edit_wavelength_precision));

			Global_Variables::resize_Line_Edit(val_Edit);
			Global_Variables::resize_Line_Edit(min_Edit);
			Global_Variables::resize_Line_Edit(max_Edit);

			spectral_Resolution_Edit->setText(Locale.toString(struct_Data.spectral_Distribution.FWHM_distribution,line_edit_double_format,line_edit_wavelength_precision));
			polarization_Edit->setText(Locale.toString(struct_Data.polarization,line_edit_double_format,line_edit_polarization_precision));
			background_Edit->setText(Locale.toString(struct_Data.background,line_edit_short_double_format,line_edit_background_precision));

			Global_Variables::resize_Line_Edit(spectral_Resolution_Edit);
			Global_Variables::resize_Line_Edit(polarization_Edit);
			Global_Variables::resize_Line_Edit(background_Edit);

			// LAST! causes refresh
			num_Points->setValue(struct_Data.wavelength.independent.num_Points);
			Global_Variables::resize_Line_Edit(num_Points);
		} else
		// refresh data
		{
			// no check for zero wavelength
			if(Locale.toDouble(polarization_Edit->text())>1 || Locale.toDouble(polarization_Edit->text())<-1)
			{
				polarization_Edit->setText(Locale.toString(struct_Data.polarization,line_edit_double_format,line_edit_polarization_precision));
				Global_Variables::resize_Line_Edit(polarization_Edit);
			}

			struct_Data.wavelength.independent.num_Points = num_Points->value();
			struct_Data.wavelength.value		   = Global_Variables::wavelength_Energy(wavelength_units,Locale.toDouble(val_Edit->text())*coeff);
			struct_Data.wavelength.independent.min = Global_Variables::wavelength_Energy(wavelength_units,Locale.toDouble(min_Edit->text())*coeff);
			struct_Data.wavelength.independent.max = Global_Variables::wavelength_Energy(wavelength_units,Locale.toDouble(max_Edit->text())*coeff);

			struct_Data.polarization = Locale.toDouble(polarization_Edit->text());
			struct_Data.background = Locale.toDouble(background_Edit->text());

			if(struct_Data.wavelength.independent.num_Points >= MIN_SPECTRAL_RESOLUTION_POINTS)
				struct_Data.spectral_Distribution.FWHM_distribution = Locale.toDouble(spectral_Resolution_Edit->text());

			QVariant var; var.setValue(struct_Data);
			structure_Item->setData(DEFAULT_COLUMN,Qt::UserRole,var);
		}
		if(struct_Data.wavelength.independent.num_Points == 1)
			list_Item->setText(name + " [" +Locale.toString(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.value)/coeff,thumbnail_double_format,thumbnail_wavelength_precision) + units + end_Bracket_Text);
		else
			list_Item->setText(name + " [" +Locale.toString(struct_Data.wavelength.independent.num_Points) + " values: " +
											Locale.toString(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.min)/coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " - " +
											Locale.toString(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.max)/coeff,thumbnail_double_format,thumbnail_wavelength_precision) + units + end_Bracket_Text);

		if(struct_Data.wavelength.independent.num_Points>1)
		{
			step_Edit->setText(Locale.toString((Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.max)-
												Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.min))/
											   (struct_Data.wavelength.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_wavelength_precision));
			Global_Variables::resize_Line_Edit(step_Edit);
		}
		show_Hide_Elements();
		show_Hide_Spectral_Elements();
	}

	show_Active_Check_Box();

	// refresh internal measurement of Independent_Variables
	if(struct_Data.item_Type == item_Type_Measurement) emit refresh_Measurement();
}

void Independent_Variables_Editor::activate_Variable()
{
	QVariant var;

	// all to passive
	for(int i = 0; i < variables_List->count(); ++i)
	{
		// set inactive
		Independent_Indicator temp_Ind = variables_List->item(i)->data(Qt::UserRole).value<Independent_Indicator>();
		temp_Ind.is_Active = false;
		var.setValue(temp_Ind);
		variables_List->item(i)->setData(Qt::UserRole, var);

		// change text
		QString current_Item_Text = variables_List->item(i)->text();
		QStringList item_Text_List = current_Item_Text.split(active, QString::SkipEmptyParts);
		variables_List->item(i)->setText(item_Text_List[0]);
	}
	// this to active
	indicator.is_Active = true;
	var.setValue(indicator);
	list_Item->setData(Qt::UserRole, var);
	list_Item->setText(list_Item->text() + active);
	active_Check_Box->setDisabled(active_Check_Box->isChecked());

	// for further use
	Independent_Indicator item_Indicator = list_Item->data(Qt::UserRole).value<Independent_Indicator>();
	argument_Type = item_Indicator.parameter_Whats_This;
}

void Independent_Variables_Editor::show_Active_Check_Box()
{
	active_Check_Box->setChecked(indicator.is_Active);
	if(indicator.is_Active) activate_Variable();
}

void Independent_Variables_Editor::show_Hide_Angular_Elements()
{
	int points = num_Points->value();

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

void Independent_Variables_Editor::show_Hide_Spectral_Elements()
{
	int points = num_Points->value();

	if(points < MIN_ANGULAR_RESOLUTION_POINTS)
	{
		spectral_Resolution_Label->hide();
		spectral_Resolution_Edit->hide();
	}
	else
	{
		spectral_Resolution_Label->show();
		spectral_Resolution_Edit->show();
	}
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}
