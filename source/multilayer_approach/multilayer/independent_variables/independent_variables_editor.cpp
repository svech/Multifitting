// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "independent_variables_editor.h"
#include "multilayer_approach/multilayer/independent_variables/variable_selection.h"
#include "multilayer_approach/multilayer_approach.h"

Independent_Variables_Editor::Independent_Variables_Editor(QTreeWidgetItem* structure_Item, QListWidgetItem* list_Item, QListWidget* variables_List, QWidget *parent) :
	structure_Item(structure_Item),
	list_Item(list_Item),
	variables_List(variables_List),
	indicator(list_Item->data(Qt::UserRole).value<Independent_Indicator>()),
	struct_Data(structure_Item->data(DEFAULT_COLUMN,Qt::UserRole).value<Data>()),
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

	create_Shortcuts();
	create_Menu();
	create_Standard_Interface();
		main_Layout->addWidget(group_Box);

	// if angle
	if(indicator.parameter_Whats_This == whats_This_Angle)		create_Angle_Interface();
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

void Independent_Variables_Editor::create_Shortcuts()
{
	// shortcuts
	{
		QShortcut* save_Shortcut			= new QShortcut(QKeySequence(Qt::Key_S | Qt::CTRL), this);
		QShortcut* open_Shortcut			= new QShortcut(QKeySequence(Qt::Key_O | Qt::CTRL), this);
		QShortcut* fit_Shortcut				= new QShortcut(QKeySequence(Qt::Key_F | Qt::CTRL | Qt::SHIFT), this);
		QShortcut* calc_Specular_Shortcut	= new QShortcut(QKeySequence(Qt::Key_C | Qt::CTRL | Qt::SHIFT), this);

		connect(save_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->save(default_File);});
		connect(open_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->open(default_File);});
		connect(fit_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->start_Fitting();	  });
		connect(calc_Specular_Shortcut, &QShortcut::activated, this, [=]{ global_Multilayer_Approach->calc_Reflection(); });
	}
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
			num_Points = new QLineEdit;
				num_Points->setFixedWidth(30);
				num_Points->setProperty(min_Size_Property, 30);
				num_Points->setValidator(new QIntValidator(1, MAX_INTEGER, this));
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


		connect(num_Points, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(val_Edit,	SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(min_Edit,	SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(max_Edit,	SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		connect(num_Points, &QLineEdit::textEdited, [=]{refresh_Show_Data();});
		connect(val_Edit,	&QLineEdit::textEdited, [=]{refresh_Show_Data();});
		connect(min_Edit,	&QLineEdit::textEdited, [=]{refresh_Show_Data();});
		connect(max_Edit,	&QLineEdit::textEdited, [=]{refresh_Show_Data();});

		connect(step_Edit,	SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

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
		angular_Resolution_Label = new QLabel(Global_Variables::parameter_Name(struct_Data, whats_This_Angular_Resolution, 0));
			angular_Resolution_Label->setSizePolicy(sp_retain);
		layout_Ang_Res->addWidget(angular_Resolution_Label);
		angular_Resolution_Edit = new QLineEdit(QString::number(struct_Data.angular_Resolution.value,line_edit_double_format,line_edit_angle_precision));
			angular_Resolution_Edit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			angular_Resolution_Edit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			angular_Resolution_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			angular_Resolution_Edit->setSizePolicy(sp_retain);
		layout_Ang_Res->addWidget(angular_Resolution_Edit);
		angle_Units_Label = new QLabel(units);
			angle_Units_Label->setSizePolicy(sp_retain);
		layout_Ang_Res->addWidget(angle_Units_Label);

		connect(angular_Resolution_Edit, &QLineEdit::textEdited, [=](QString text){resize_Line_Edit(text,angular_Resolution_Edit);} );
		connect(angular_Resolution_Edit, &QLineEdit::textEdited, [=]{refresh_Show_Data();});
	}

	QGridLayout* geometry_Layout = new QGridLayout;
		geometry_Layout->setAlignment(Qt::AlignLeft);
	group_Box_Layout->addLayout(geometry_Layout);

	// beam size
	{
		beam_Size_Label = new QLabel(Global_Variables::parameter_Name(struct_Data, whats_This_Beam_Size, 0));
		geometry_Layout->addWidget(beam_Size_Label,0,0,Qt::AlignRight);
		beam_Size_LineEdit = new QLineEdit(QString::number(struct_Data.beam_Size.value,line_edit_double_format,line_edit_beam_size_precision));
			beam_Size_LineEdit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			beam_Size_LineEdit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			beam_Size_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		geometry_Layout->addWidget(beam_Size_LineEdit,0,1,Qt::AlignLeft);
		beam_Size_Units_Label = new QLabel(size_Units_List[mm]);
		geometry_Layout->addWidget(beam_Size_Units_Label,0,2,Qt::AlignLeft);

		connect(beam_Size_LineEdit, &QLineEdit::textEdited, this, [=](QString text){resize_Line_Edit(text,beam_Size_LineEdit);} );
		connect(beam_Size_LineEdit, &QLineEdit::textEdited, [=]{refresh_Show_Data();});
	}

	// spacer
	{
		QLabel* spacer_1 = new QLabel("      ");
		geometry_Layout->addWidget(spacer_1,0,3,Qt::AlignRight);
	}

	// sample size
	{
		sample_Size_Label = new QLabel(Global_Variables::parameter_Name(struct_Data, whats_This_Sample_Size, 0));
		geometry_Layout->addWidget(sample_Size_Label,0,4,Qt::AlignRight);
		sample_Size_LineEdit = new QLineEdit(QString::number(struct_Data.sample_Size.value,line_edit_double_format,line_edit_sample_size_precision));
			sample_Size_LineEdit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			sample_Size_LineEdit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			sample_Size_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		geometry_Layout->addWidget(sample_Size_LineEdit,0,5,Qt::AlignLeft);
		sample_Size_Units_Label = new QLabel(size_Units_List[mm]);
		geometry_Layout->addWidget(sample_Size_Units_Label,0,6,Qt::AlignLeft);

		connect(sample_Size_LineEdit, &QLineEdit::textEdited, this, [=](QString text){resize_Line_Edit(text,sample_Size_LineEdit);} );
		connect(sample_Size_LineEdit, &QLineEdit::textEdited, [=]{refresh_Show_Data();});
	}

	// beam spreading
	{
		beam_Profile_Spreading_Label = new QLabel(Global_Variables::parameter_Name(struct_Data, whats_This_Beam_Profile_Spreading, 0));
		geometry_Layout->addWidget(beam_Profile_Spreading_Label,1,0,Qt::AlignRight);
		beam_Profile_Spreading_LineEdit = new QLineEdit(QString::number(struct_Data.beam_Profile_Spreading.value,line_edit_double_format,line_edit_beam_size_precision));
			beam_Profile_Spreading_LineEdit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			beam_Profile_Spreading_LineEdit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			beam_Profile_Spreading_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		geometry_Layout->addWidget(beam_Profile_Spreading_LineEdit,1,1,Qt::AlignLeft);

		connect(beam_Profile_Spreading_LineEdit, &QLineEdit::textEdited, this, [=](QString text){resize_Line_Edit(text,beam_Profile_Spreading_LineEdit);} );
		connect(beam_Profile_Spreading_LineEdit, &QLineEdit::textEdited, [=]{refresh_Show_Data();});
	}

	// spacer
	{
		QLabel* spacer_2 = new QLabel("      ");
		geometry_Layout->addWidget(spacer_2,1,3,Qt::AlignRight);
	}

	// sample shift
	{
		sample_Shift_Label = new QLabel(Global_Variables::parameter_Name(struct_Data, whats_This_Sample_Shift, 0));
		geometry_Layout->addWidget(sample_Shift_Label,1,4,Qt::AlignRight);
		sample_Shift_LineEdit = new QLineEdit(QString::number(struct_Data.sample_Shift.value,line_edit_double_format,line_edit_sample_size_precision));
			sample_Shift_LineEdit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			sample_Shift_LineEdit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			sample_Shift_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		geometry_Layout->addWidget(sample_Shift_LineEdit,1,5,Qt::AlignLeft);
		sample_Shift_Units_Label = new QLabel(size_Units_List[mm]);
		geometry_Layout->addWidget(sample_Shift_Units_Label,1,6,Qt::AlignLeft);

		connect(sample_Shift_LineEdit, &QLineEdit::textEdited, this, [=](QString text){resize_Line_Edit(text,sample_Shift_LineEdit);} );
		connect(sample_Shift_LineEdit, &QLineEdit::textEdited, [=]{refresh_Show_Data();});
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

		spectral_Resolution_Label = new QLabel(Global_Variables::parameter_Name(struct_Data, whats_This_Spectral_Resolution, 0));
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

		polarization_Label = new QLabel(Global_Variables::parameter_Name(struct_Data, whats_This_Polarization, 0));
			polarization_Label->setSizePolicy(sp_retain);
		down_Layout->addWidget(polarization_Label);

		polarization_Edit = new QLineEdit;
			polarization_Edit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			polarization_Edit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			polarization_Edit->setValidator(new QDoubleValidator(-1, 1, MAX_PRECISION, this));
			polarization_Edit->setSizePolicy(sp_retain);
		down_Layout->addWidget(polarization_Edit);

		background_Label = new QLabel(Global_Variables::parameter_Name(struct_Data, whats_This_Background, 0));
			background_Label->setSizePolicy(sp_retain);
		down_Layout->addWidget(background_Label);

		background_Edit = new QLineEdit;
			background_Edit->setFixedWidth(INDEPENDENT_LINE_EDIT_WIDTH);
			background_Edit->setProperty(min_Size_Property, INDEPENDENT_LINE_EDIT_WIDTH);
			background_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			background_Edit->setSizePolicy(sp_retain);
		down_Layout->addWidget(background_Edit);
	}

	connect(spectral_Resolution_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
	connect(spectral_Resolution_Edit, &QLineEdit::textEdited, [=]{refresh_Show_Data();});

	connect(polarization_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
	connect(polarization_Edit, &QLineEdit::textEdited, [=]{refresh_Show_Data();});

	connect(background_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
	connect(background_Edit, &QLineEdit::textEdited, [=]{refresh_Show_Data();});
}

void Independent_Variables_Editor::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
}

void Independent_Variables_Editor::resize_Line_Edit(QString text, QLineEdit* line_Edit)
{
	if(line_Edit == nullptr) line_Edit = qobject_cast<QLineEdit*>(QObject::sender());

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

void Independent_Variables_Editor::show_Hide_Elements()
{
	int points = num_Points->text().toInt();

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
	Parameter* param_Pointer = nullptr;

	/// optical constants
	if(indicator.parameter_Whats_This == whats_This_Absolute_Density)
	{
		param_Pointer = &struct_Data.absolute_Density;
		line_edit_precision = line_edit_density_precision;
		thumbnail_precision = thumbnail_density_precision;
		units = " " + density_units;
	}
	if(indicator.parameter_Whats_This == whats_This_Relative_Density)
	{
		param_Pointer = &struct_Data.relative_Density;
		line_edit_precision = line_edit_density_precision;
		thumbnail_precision = thumbnail_density_precision;
		units = "" ;
	}
	if(indicator.parameter_Whats_This == whats_This_Permittivity)
	{
		val_Edit->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
		min_Edit->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));
		max_Edit->setValidator(new QDoubleValidator(-MAX_DOUBLE, MAX_DOUBLE, MAX_PRECISION, this));

		param_Pointer = &struct_Data.permittivity;
		line_edit_precision = line_edit_permittivity_precision;
		thumbnail_precision = thumbnail_permittivity_precision;
		units = " " + opt_const_units;
	}
	if(indicator.parameter_Whats_This == whats_This_Absorption)
	{
		param_Pointer = &struct_Data.absorption;
		line_edit_precision = line_edit_absorption_precision;
		thumbnail_precision = thumbnail_absorption_precision;
		units = " " + opt_const_units;
	}
	if(indicator.parameter_Whats_This == whats_This_Composition)
	{
		param_Pointer = &struct_Data.composition[indicator.index].composition;
		line_edit_precision = line_edit_composition_precision;
		thumbnail_precision = thumbnail_composition_precision;
		units = "";
	}
	/// thickness parameters
	if(indicator.parameter_Whats_This == whats_This_Thickness)
	{
		param_Pointer = &struct_Data.thickness;
		line_edit_precision = line_edit_thickness_precision;
		thumbnail_precision = thumbnail_thickness_precision;
		units = " " + length_units;
		coeff = length_Coefficients_Map.value(length_units);
	}
	if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Line_Value)
	{
		param_Pointer = &struct_Data.thickness_Drift.drift_Line_Value;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = " " + length_units;
	}
	if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Rand_Rms)
	{
		param_Pointer = &struct_Data.thickness_Drift.drift_Rand_Rms;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = " " + length_units;
	}
	if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Sine_Amplitude)
	{
		param_Pointer = &struct_Data.thickness_Drift.drift_Sine_Amplitude;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = " " + length_units;
	}
	if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Sine_Frequency)
	{
		param_Pointer = &struct_Data.thickness_Drift.drift_Sine_Frequency;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = " " + length_units;
	}
	if(indicator.parameter_Whats_This == whats_This_Thickness_Drift_Sine_Phase)
	{
		param_Pointer = &struct_Data.thickness_Drift.drift_Sine_Phase;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = " " + length_units;
	}
	/// interface parameters
	if(indicator.parameter_Whats_This == whats_This_Sigma)
	{
		param_Pointer = &struct_Data.sigma;
		line_edit_precision = line_edit_sigma_precision;
		thumbnail_precision = thumbnail_sigma_precision;
		units = " " + length_units;
		coeff = length_Coefficients_Map.value(length_units);
	}
	if(indicator.parameter_Whats_This == whats_This_Interlayer_Composition)
	{
		param_Pointer = &struct_Data.interlayer_Composition[indicator.index].interlayer;
		line_edit_precision = line_edit_interlayer_precision;
		thumbnail_precision = thumbnail_interlayer_precision;
		units = "";
	}
	if(indicator.parameter_Whats_This == whats_This_Interlayer_My_Sigma)
	{
		param_Pointer = &struct_Data.interlayer_Composition[indicator.index].my_Sigma;
		line_edit_precision = line_edit_sigma_precision;
		thumbnail_precision = thumbnail_sigma_precision;
		units = " " + length_units;
		coeff = length_Coefficients_Map.value(length_units);
	}
	if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Line_Value)
	{
		param_Pointer = &struct_Data.sigma_Drift.drift_Line_Value;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = "";
	}
	if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Rand_Rms)
	{
		param_Pointer = &struct_Data.sigma_Drift.drift_Rand_Rms;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = "";
	}
	if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Sine_Amplitude)
	{
		param_Pointer = &struct_Data.sigma_Drift.drift_Sine_Amplitude;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = "";
	}
	if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Sine_Frequency)
	{
		param_Pointer = &struct_Data.sigma_Drift.drift_Sine_Frequency;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = "";
	}
	if(indicator.parameter_Whats_This == whats_This_Sigma_Drift_Sine_Phase)
	{
		param_Pointer = &struct_Data.sigma_Drift.drift_Sine_Phase;
		line_edit_precision = line_edit_drift_precision;
		thumbnail_precision = thumbnail_drift_precision;
		units = "";
	}
	/// stack parameters (without num_Repetitions)
	if(indicator.parameter_Whats_This == whats_This_Num_Repetitions)
	{
		param_Pointer = nullptr;
	}
	if(indicator.parameter_Whats_This == whats_This_Period)
	{
		param_Pointer = &struct_Data.period;
		line_edit_precision = line_edit_period_precision;
		thumbnail_precision = thumbnail_period_precision;
		units = " " + length_units;
		coeff = length_Coefficients_Map.value(length_units);
	}
	if(indicator.parameter_Whats_This == whats_This_Gamma)
	{
		param_Pointer = &struct_Data.gamma;
		line_edit_precision = line_edit_gamma_precision;
		thumbnail_precision = thumbnail_gamma_precision;
		units = "";
	}
	/// measurement parameters
	if(indicator.parameter_Whats_This == whats_This_Angle)
	{
		param_Pointer = &struct_Data.probe_Angle;
		line_edit_precision = line_edit_angle_precision;
		thumbnail_precision = thumbnail_angle_precision;
		units = angle_units;
		coeff = angle_Coefficients_Map.value(angle_units);
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

		if(indicator.parameter_Whats_This == whats_This_Angle)
		{
			angle_Units_Label->setText(units);
		}

		// show data
		if(show)
		{
			num_Points->setText(QString::number(parameter.independent.num_Points));
			val_Edit->setText(	QString::number(parameter.value/coeff,			line_edit_double_format,line_edit_precision));
			min_Edit->setText(	QString::number(parameter.independent.min/coeff,line_edit_double_format,line_edit_precision));
			max_Edit->setText(	QString::number(parameter.independent.max/coeff,line_edit_double_format,line_edit_precision));

			resize_Line_Edit(num_Points->text(),num_Points);
			resize_Line_Edit(val_Edit->text(),val_Edit);
			resize_Line_Edit(min_Edit->text(),min_Edit);
			resize_Line_Edit(max_Edit->text(),max_Edit);

			if(indicator.parameter_Whats_This == whats_This_Angle)
			{
				angular_Resolution_Edit->		 setText(QString::number(struct_Data.angular_Resolution.value/coeff,line_edit_double_format,line_edit_angle_precision));
				beam_Size_LineEdit->			 setText(QString::number(struct_Data.beam_Size.value,				line_edit_double_format,line_edit_beam_size_precision));
				sample_Size_LineEdit->			 setText(QString::number(struct_Data.sample_Size.value,				line_edit_double_format,line_edit_sample_size_precision));
				beam_Profile_Spreading_LineEdit->setText(QString::number(struct_Data.beam_Profile_Spreading.value,	line_edit_double_format,line_edit_beam_size_precision));
				sample_Shift_LineEdit->			 setText(QString::number(struct_Data.sample_Shift.value,			line_edit_double_format,line_edit_sample_size_precision));

				resize_Line_Edit(angular_Resolution_Edit->text(),		 angular_Resolution_Edit);
				resize_Line_Edit(beam_Size_LineEdit->text(),			 beam_Size_LineEdit);
				resize_Line_Edit(sample_Size_LineEdit->text(),			 sample_Size_LineEdit);
				resize_Line_Edit(beam_Profile_Spreading_LineEdit->text(),beam_Profile_Spreading_LineEdit);
				resize_Line_Edit(sample_Shift_LineEdit->text(),			 sample_Shift_LineEdit);
			}
		} else
		// refresh data
		{
			if(num_Points->text().toInt()<1)
			{
				num_Points->setText(QString::number(parameter.independent.num_Points));
				resize_Line_Edit(num_Points->text(),num_Points);
			}
			// special cases
			if(indicator.parameter_Whats_This == whats_This_Gamma)
			{
				if(val_Edit->text().toDouble()>1)
				{
					val_Edit->setText(QString::number(parameter.value,line_edit_double_format,line_edit_gamma_precision));
					resize_Line_Edit(val_Edit->text(),val_Edit);
				}
				if(min_Edit->text().toDouble()>1)
				{
					min_Edit->setText(QString::number(parameter.independent.min,line_edit_double_format,line_edit_gamma_precision));
					resize_Line_Edit(min_Edit->text(),min_Edit);
				}
				if(max_Edit->text().toDouble()>1)
				{
					max_Edit->setText(QString::number(parameter.independent.max,line_edit_double_format,line_edit_gamma_precision));
					resize_Line_Edit(max_Edit->text(),max_Edit);
				}
			}
			if(indicator.parameter_Whats_This == whats_This_Angle)
			{
				if(val_Edit->text().toDouble()*coeff>90.+3*pow(10.,-line_edit_angle_precision+1))
				{
					val_Edit->setText(QString::number(parameter.value/coeff,line_edit_double_format,line_edit_angle_precision));
					resize_Line_Edit(val_Edit->text(),val_Edit);
				}
				if(min_Edit->text().toDouble()*coeff>90.+3*pow(10.,-line_edit_angle_precision+1))
				{
					min_Edit->setText(QString::number(parameter.independent.min/coeff,line_edit_double_format,line_edit_angle_precision));
					resize_Line_Edit(min_Edit->text(),min_Edit);
				}
				if(max_Edit->text().toDouble()*coeff>90.+3*pow(10.,-line_edit_angle_precision+1))
				{
					max_Edit->setText(QString::number(parameter.independent.max/coeff,line_edit_double_format,line_edit_angle_precision));
					resize_Line_Edit(max_Edit->text(),max_Edit);
				}

				if(parameter.independent.num_Points >= MIN_ANGULAR_RESOLUTION_POINTS)
				{
					struct_Data.angular_Resolution.value = angular_Resolution_Edit->text().toDouble()*coeff;
				}
				struct_Data.beam_Size.value				= beam_Size_LineEdit->text().toDouble();
				struct_Data.sample_Size.value			= sample_Size_LineEdit->text().toDouble();
				struct_Data.beam_Profile_Spreading.value= beam_Profile_Spreading_LineEdit->text().toDouble();
				struct_Data.sample_Shift.value			= sample_Shift_LineEdit->text().toDouble();
			}

			parameter.independent.num_Points = num_Points->text().toInt();
			parameter.value			  = val_Edit->text().toDouble()*coeff;
			parameter.independent.min = min_Edit->text().toDouble()*coeff;
			parameter.independent.max = max_Edit->text().toDouble()*coeff;

			QVariant var; var.setValue(struct_Data);
			structure_Item->setData(DEFAULT_COLUMN,Qt::UserRole,var);
		}
		if(parameter.independent.num_Points == 1)
			list_Item->setText(name + " [" +QString::number(parameter.value/coeff,thumbnail_double_format,thumbnail_precision) + units + end_Bracket_Text);
		else
			list_Item->setText(name + " [" +QString::number(parameter.independent.num_Points) + " values: " +
											QString::number(parameter.independent.min/coeff,thumbnail_double_format,thumbnail_precision) + " - " +
											QString::number(parameter.independent.max/coeff,thumbnail_double_format,thumbnail_precision) + units + end_Bracket_Text);
		if(parameter.independent.num_Points>1)
		{
			step_Edit->setText(QString::number((parameter.independent.max-parameter.independent.min)/(parameter.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_precision));
			resize_Line_Edit(step_Edit->text(),step_Edit);
		}
		show_Hide_Elements();
		if(indicator.parameter_Whats_This == whats_This_Angle)	show_Hide_Angular_Elements();
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
			num_Points->setText(QString::number(struct_Data.num_Repetition.num_Steps));
			val_Edit->setText(	QString::number(struct_Data.num_Repetition.value));
			min_Edit->setText(	QString::number(struct_Data.num_Repetition.start));
			max_Edit->setText(	QString::number(struct_Data.num_Repetition.step));

			resize_Line_Edit(num_Points->text(),num_Points);
			resize_Line_Edit(val_Edit->text(),val_Edit);
			resize_Line_Edit(min_Edit->text(),min_Edit);
			resize_Line_Edit(max_Edit->text(),max_Edit);
		} else
		// refresh data
		{
			if(num_Points->text().toInt()<1)
			{
				num_Points->setText(QString::number(struct_Data.num_Repetition.num_Steps));
				resize_Line_Edit(num_Points->text(),num_Points);
			}
			if(max_Edit->text().toInt()<1)
			{
				max_Edit->setText(QString::number(struct_Data.num_Repetition.step));
				resize_Line_Edit(max_Edit->text(),max_Edit);
			}

			struct_Data.num_Repetition.num_Steps = num_Points->text().toInt();
			struct_Data.num_Repetition.value	 =   val_Edit->text().toInt();
			struct_Data.num_Repetition.start	 =   min_Edit->text().toInt();
			struct_Data.num_Repetition.step		 =   max_Edit->text().toInt();

			QVariant var; var.setValue(struct_Data);
			structure_Item->setData(DEFAULT_COLUMN,Qt::UserRole,var);
		}
		if(struct_Data.num_Repetition.num_Steps == 1)
			list_Item->setText(name + " [" +QString::number(struct_Data.num_Repetition.value) + end_Bracket_Text);
		else
			list_Item->setText(name + " [" +QString::number(struct_Data.num_Repetition.num_Steps) + " values: " +
											QString::number(struct_Data.num_Repetition.start) + " - " +
											QString::number(struct_Data.num_Repetition.start + struct_Data.num_Repetition.step*struct_Data.num_Repetition.num_Steps) + end_Bracket_Text);
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
		spectral_Resolution_Label->setText(Global_Variables::parameter_Name(struct_Data, whats_This_Spectral_Resolution, indicator.index));

		// show data
		if(show)
		{
			num_Points->setText(QString::number(struct_Data.wavelength.independent.num_Points));
			val_Edit->setText(	QString::number(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.value)/coeff,line_edit_double_format,line_edit_wavelength_precision));
			min_Edit->setText(	QString::number(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.min)/coeff,line_edit_double_format,line_edit_wavelength_precision));
			max_Edit->setText(	QString::number(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.max)/coeff,line_edit_double_format,line_edit_wavelength_precision));

			resize_Line_Edit(num_Points->text(),num_Points);
			resize_Line_Edit(val_Edit->text(),val_Edit);
			resize_Line_Edit(min_Edit->text(),min_Edit);
			resize_Line_Edit(max_Edit->text(),max_Edit);

			spectral_Resolution_Edit->setText(QString::number(struct_Data.spectral_Resolution.value,line_edit_double_format,line_edit_wavelength_precision));
			polarization_Edit->setText(QString::number(struct_Data.polarization.value/coeff,line_edit_double_format,line_edit_polarization_precision));
			background_Edit->setText(QString::number(struct_Data.background.value/coeff,line_edit_short_double_format,line_edit_background_precision));

			resize_Line_Edit(spectral_Resolution_Edit->text(),spectral_Resolution_Edit);
			resize_Line_Edit(polarization_Edit->text(),polarization_Edit);
			resize_Line_Edit(background_Edit->text(),background_Edit);

		} else
		// refresh data
		{
			if(num_Points->text().toInt()<1)
			{
				num_Points->setText(QString::number(struct_Data.wavelength.independent.num_Points));
				resize_Line_Edit(num_Points->text(),num_Points);
			}
			// no check for zero wavelength
			if(polarization_Edit->text().toDouble()>1 || polarization_Edit->text().toDouble()<-1)
			{
				polarization_Edit->setText(QString::number(struct_Data.polarization.value,line_edit_double_format,line_edit_polarization_precision));
				resize_Line_Edit(polarization_Edit->text(),polarization_Edit);
			}

			struct_Data.wavelength.independent.num_Points = num_Points->text().toInt();
			struct_Data.wavelength.value		   = Global_Variables::wavelength_Energy(wavelength_units,val_Edit->text().toDouble()*coeff);
			struct_Data.wavelength.independent.min = Global_Variables::wavelength_Energy(wavelength_units,min_Edit->text().toDouble()*coeff);
			struct_Data.wavelength.independent.max = Global_Variables::wavelength_Energy(wavelength_units,max_Edit->text().toDouble()*coeff);

			struct_Data.polarization.value = polarization_Edit->text().toDouble();
			struct_Data.background.value = background_Edit->text().toDouble();

			if(struct_Data.wavelength.independent.num_Points >= MIN_SPECTRAL_RESOLUTION_POINTS)
				struct_Data.spectral_Resolution.value = spectral_Resolution_Edit->text().toDouble();

			QVariant var; var.setValue(struct_Data);
			structure_Item->setData(DEFAULT_COLUMN,Qt::UserRole,var);
		}
		if(struct_Data.wavelength.independent.num_Points == 1)
			list_Item->setText(name + " [" +QString::number(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.value)/coeff,thumbnail_double_format,thumbnail_wavelength_precision) + units + end_Bracket_Text);
		else
			list_Item->setText(name + " [" +QString::number(struct_Data.wavelength.independent.num_Points) + " values: " +
											QString::number(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.min)/coeff,thumbnail_double_format,thumbnail_wavelength_precision) + " - " +
											QString::number(Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.max)/coeff/coeff,thumbnail_double_format,thumbnail_wavelength_precision) + units + end_Bracket_Text);

		if(struct_Data.wavelength.independent.num_Points>1)
		{
			step_Edit->setText(QString::number((Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.max)-
												Global_Variables::wavelength_Energy(wavelength_units,struct_Data.wavelength.independent.min))/
											   (struct_Data.wavelength.independent.num_Points-1)/coeff,line_edit_double_format,line_edit_wavelength_precision));
			resize_Line_Edit(step_Edit->text(),step_Edit);
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
}

void Independent_Variables_Editor::show_Active_Check_Box()
{
	active_Check_Box->setChecked(indicator.is_Active);
	if(indicator.is_Active) activate_Variable();
}

void Independent_Variables_Editor::show_Hide_Angular_Elements()
{
	int points = num_Points->text().toInt();

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
	int points = num_Points->text().toInt();

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
