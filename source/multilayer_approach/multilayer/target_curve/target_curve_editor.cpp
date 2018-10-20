// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "target_curve_editor.h"

Target_Curve_Editor::Target_Curve_Editor(Target_Curve* target_Curve, Multilayer* multilayer, QWidget *parent) :
	target_Curve(target_Curve),
	multilayer_Parent(multilayer),
	QDialog(parent)
{
	create_Main_Layout();
	setWindowTitle("Import Data");
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
	setAcceptDrops(true);
}

void Target_Curve_Editor::closeEvent(QCloseEvent *event)
{	
	multilayer_Parent->runned_Target_Curve_Editors.remove(multilayer_Parent->runned_Target_Curve_Editors.key(this));
	event->accept();
}

void Target_Curve_Editor::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void Target_Curve_Editor::dropEvent(QDropEvent* event)
{
	int counter = 0;
	foreach (const QUrl &url, event->mimeData()->urls())
	{
		if(counter==0)
		{
			QString fileName = url.toLocalFile();
			if (filepath_ComboBox->findText(fileName) == -1)
			{
				filepath_ComboBox->addItem(fileName);
			}
			filepath_ComboBox->setCurrentIndex(filepath_ComboBox->findText(fileName));
			read_Data_File(fileName);
		}
		++counter;
	}
}

void Target_Curve_Editor::set_Window_Geometry()
{
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
//	main_Layout->layout()->setSizeConstraint( QLayout::SetFixedSize );
}

void Target_Curve_Editor::read_Data_File(QString filepath)
{
	refresh_Filepath(filepath);
	if(is_File_Exists)
	{
		target_Curve->import_Data(filepath);
		target_Curve->fill_Measurement_With_Data();
	} else
	{
		target_Curve->loaded_And_Ready = false;
	}

	show_Description_Label();

	// refresh plot
	target_Curve_Plot->plot_Data();
}

void Target_Curve_Editor::show_Description_Label()
{
	if(target_Curve->loaded_And_Ready)
	{
		QString spacer;
		if(target_Curve->curve.angle_Type == angle_Type_Grazing)   target_Curve->ang_Type_For_Label_At_Fixed="(gr)";
		if(target_Curve->curve.angle_Type == angle_Type_Incidence) target_Curve->ang_Type_For_Label_At_Fixed="(in)";

		if(target_Curve->curve.argument_Type == whats_This_Angle)
		{
			target_Curve->arg_Type_For_Label = "Angular "+target_Curve->ang_Type_For_Label_At_Fixed;

			target_Curve->arg_Units = target_Curve->curve.angular_Units;

			double coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);
			target_Curve->at_Fixed = QString::number(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.wavelength.value)/coeff, thumbnail_double_format, thumbnail_wavelength_precision)+" "+target_Curve->curve.spectral_Units;
			spacer = "";
		}
		if(target_Curve->curve.argument_Type == whats_This_Wavelength)
		{
			target_Curve->arg_Type_For_Label = "Spectral";
			target_Curve->arg_Units = target_Curve->curve.spectral_Units;

			double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
			target_Curve->at_Fixed = QString::number(target_Curve->measurement.probe_Angle.value/coeff, thumbnail_double_format, thumbnail_angle_precision)+" "+target_Curve->curve.angular_Units;
			target_Curve->at_Fixed = target_Curve->ang_Type_For_Label_At_Fixed + " " + target_Curve->at_Fixed;
			spacer = " ";
		}

		target_Curve->label_Text =
					target_Curve->arg_Type_For_Label + "; " +
					target_Curve->curve.value_Mode + "; " +
					QString::number(target_Curve->curve.shifted_Argument.first()) + "-" + QString::number(target_Curve->curve.shifted_Argument.last()) + spacer + target_Curve->arg_Units + "; " +
					"at " + target_Curve->at_Fixed;
	} else
	{
		target_Curve->label_Text = "<no description>";
	}

	target_Curve->description_Label->setText(target_Curve->label_Text);
}

void Target_Curve_Editor::browse_Data_File()
{
	QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Find Measured Curve", filepath_ComboBox->lineEdit()->text(), "Text data (*.txt *.dat *.xy);;All files (*.*)"));

	if (!directory.isEmpty())
	{
		if (filepath_ComboBox->findText(directory) == -1)
		{
			filepath_ComboBox->addItem(directory);
		}
		filepath_ComboBox->setCurrentIndex(filepath_ComboBox->findText(directory));
		filepath_ComboBox->lineEdit()->returnPressed();
	}
}

void Target_Curve_Editor::fill_Arg_Units_ComboBox(QString arg_Type)
{
	arg_Units_ComboBox->blockSignals(true);
	at_Fixed_Units_ComboBox->blockSignals(true);
	arg_Units_ComboBox->clear();
	at_Fixed_Units_ComboBox->clear();

	if(arg_Type == argument_Types[Grazing_angle] || arg_Type == argument_Types[Incident_angle])	// grazing of incidence angle
	{
		arg_Units_ComboBox->addItems(angle_Units_List);
		arg_Units_ComboBox->setCurrentIndex(arg_Units_ComboBox->findText(target_Curve->curve.angular_Units));

		at_Fixed_Units_ComboBox->addItems(wavelength_Units_List);
		at_Fixed_Units_ComboBox->setCurrentIndex(at_Fixed_Units_ComboBox->findText(target_Curve->curve.spectral_Units));

		angular_Units_Label->setText(arg_Units_ComboBox->currentText());

		if(	target_Curve->curve.spectral_Units == wavelength_Units_List[angstrom] ||
			target_Curve->curve.spectral_Units == wavelength_Units_List[nm]		  ||
			target_Curve->curve.spectral_Units == wavelength_Units_List[mcm]	   )
		{
			at_Fixed_Label->setText("At fixed wavelength:");
		} else
		{
			at_Fixed_Label->setText("At fixed energy:");
		}

		double coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);		// spectral units
		at_Fixed_LineEdit->setText(QString::number(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.wavelength.value)/coeff, line_edit_double_format, line_edit_wavelength_precision));
	}
	if(arg_Type == argument_Types[Wavelength_Energy])									// wavelength or energy
	{
		arg_Units_ComboBox->addItems(wavelength_Units_List);
		arg_Units_ComboBox->setCurrentIndex(arg_Units_ComboBox->findText(target_Curve->curve.spectral_Units));


		at_Fixed_Label->setText("At fixed angle:");
		QStringList list_Grazing, list_Incidence;
		for(int i=0; i<angle_Units_List.size(); ++i)
		{
			list_Grazing.append(angle_Units_List[i]+", Grazing");
			list_Incidence.append(angle_Units_List[i]+", Incidence");
		}

		at_Fixed_Units_ComboBox->addItems(list_Grazing);
		at_Fixed_Units_ComboBox->addItems(list_Incidence);
		at_Fixed_Units_ComboBox->setCurrentIndex(at_Fixed_Units_ComboBox->findText(target_Curve->curve.angular_Units+", "+target_Curve->curve.angle_Type));

		angular_Units_Label->setText(at_Fixed_Units_ComboBox->currentText().split(", ")[0]);

		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);				// angular units
		at_Fixed_LineEdit->setText(QString::number(target_Curve->measurement.probe_Angle.value/coeff, line_edit_double_format, line_edit_angle_precision));
	}
	arg_Units_ComboBox->blockSignals(false);
	at_Fixed_Units_ComboBox->blockSignals(false);

	Global_Variables::resize_Line_Edit(at_Fixed_LineEdit, false);
}

void Target_Curve_Editor::fill_Val_Modes_ComboBox(QString val_Mode)
{
	val_Mode_ComboBox->blockSignals(true);
	val_Mode_ComboBox->clear();

	if(val_Mode == value_Function[Reflectance])	// reflectance
	{
		val_Mode_ComboBox->addItems(value_R_Mode);
	}
	if(val_Mode == value_Function[Transmittance])	// transmittance
	{
		val_Mode_ComboBox->addItems(value_T_Mode);
	}
	val_Mode_ComboBox->blockSignals(false);

	int index = val_Mode_ComboBox->findText(target_Curve->curve.value_Mode);
	if(index>=0) {val_Mode_ComboBox->setCurrentIndex(index);}
	else		 {val_Mode_ComboBox->setCurrentIndex(0);val_Mode_ComboBox->currentTextChanged(val_Mode_ComboBox->currentText());}
}

void Target_Curve_Editor::change_Arg_Units_ComboBox(QString arg_Units)
{
	if(arg_Type_ComboBox->currentText() == argument_Types[Grazing_angle] || arg_Type_ComboBox->currentText() == argument_Types[Incident_angle])	// grazing or incidence angle
	{
		angular_Units_Label->setText(arg_Units);
	}
}

void Target_Curve_Editor::change_At_Fixed_Units_ComboBox(QString fixed_Units)
{
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy])	// wavelength/energy
	{
		angular_Units_Label->setText(fixed_Units.split(", ")[0]);
	}
}

void Target_Curve_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setContentsMargins(4,4,4,0);
		main_Layout->setSpacing(0);

	bottom_Part_Widget = new QWidget;
	bottom_Part_Layout = new QVBoxLayout(bottom_Part_Widget);
		bottom_Part_Layout->setContentsMargins(0,0,0,0);
		bottom_Part_Layout->setSpacing(0);
		bottom_Part_Widget->setFixedHeight(305);
#ifdef __linux__
		bottom_Part_Widget->setFixedHeight(345);
#endif

	create_Plot();
		main_Layout->addWidget(target_Curve_Plot->custom_Plot);
	create_Plot_Options_GroupBox();
		bottom_Part_Layout->addWidget(plot_Options_GroupBox);
	create_Filepath_GroupBox();
		bottom_Part_Layout->addWidget(filepath_GroupBox);
	create_Data_GroupBox();
		bottom_Part_Layout->addWidget(data_GroupBox);
	create_Buttons();

	target_Curve_Plot->custom_Plot->replot();
	main_Layout->addWidget(bottom_Part_Widget);

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

void Target_Curve_Editor::create_Plot()
{
	target_Curve_Plot = new Target_Curve_Plot(target_Curve, this);
}

void Target_Curve_Editor::create_Plot_Options_GroupBox()
{
	plot_Options_GroupBox = new QGroupBox("Plot options");
#ifdef __linux__
		plot_Options_GroupBox->setStyleSheet("QGroupBox{ border-radius: 2px;  border: 1px solid silver; margin-top: 2ex;}"
											 "QGroupBox::title   { subcontrol-origin: margin;   top: 6px; left: 9px; padding: 0 0px 0 1px;}");
#endif
	QVBoxLayout* plot_Options_GroupBox_Layout = new QVBoxLayout(plot_Options_GroupBox);

	// first row
	{
		QLabel* scale_Label = new QLabel("Scale: ");

		QRadioButton* lin_Radio_Button = new QRadioButton("Lin");
		connect(lin_Radio_Button, &QRadioButton::toggled, this, [=]
		{
			if(lin_Radio_Button->isChecked())
			{
				target_Curve->plot_Options_Experimental.scale = lin_Scale;
				target_Curve->plot_Options_Calculated.scale = lin_Scale;
			}
			target_Curve_Plot->plot_Data();
		});
		connect(lin_Radio_Button, &QRadioButton::clicked, lin_Radio_Button, &QRadioButton::toggled);

		if(target_Curve->plot_Options_Experimental.scale == lin_Scale)
		{
			lin_Radio_Button->setChecked(true);
			lin_Radio_Button->toggled(true);
		}

		QRadioButton* log_Radio_Button = new QRadioButton("Log");
		connect(log_Radio_Button, &QRadioButton::toggled, this, [=]
		{
			if(log_Radio_Button->isChecked())
			{
				target_Curve->plot_Options_Experimental.scale = log_Scale;
				target_Curve->plot_Options_Calculated.scale = log_Scale;
			}
			target_Curve_Plot->plot_Data();
		});
		connect(log_Radio_Button, &QRadioButton::clicked, log_Radio_Button, &QRadioButton::toggled);

		if(target_Curve->plot_Options_Experimental.scale == log_Scale)
		{
			log_Radio_Button->setChecked(true);
			log_Radio_Button->toggled(true);
		}

		QHBoxLayout* radio_Button_Layout = new QHBoxLayout;
		radio_Button_Layout->setAlignment(Qt::AlignLeft);
		radio_Button_Layout->addWidget(scale_Label);
		radio_Button_Layout->addWidget(lin_Radio_Button);
		radio_Button_Layout->addWidget(log_Radio_Button);

		plot_Options_GroupBox_Layout->addLayout(radio_Button_Layout);
	}
}

void Target_Curve_Editor::create_Filepath_GroupBox()
{
	filepath_GroupBox = new QGroupBox("File path");
#ifdef __linux__
		filepath_GroupBox->setStyleSheet("QGroupBox{ border-radius: 2px;  border: 1px solid silver; margin-top: 2ex;}"
										 "QGroupBox::title   { subcontrol-origin: margin;   top: 6px; left: 9px; padding: 0 0px 0 1px;}");
#endif
	QVBoxLayout* filepath_GroupBox_Layout = new QVBoxLayout(filepath_GroupBox);

	// first row
	{
		QHBoxLayout* layout = new QHBoxLayout;

		// filepath
		filepath_ComboBox = new QComboBox;
			filepath_ComboBox->setEditable(true);
			connect(filepath_ComboBox->lineEdit(), &QLineEdit::textEdited, this, [=]{resize_ComboBox();} );
			connect(filepath_ComboBox, &QComboBox::currentTextChanged, this, [=](QString str){ filepath_ComboBox->lineEdit()->textEdited(str); } );
			connect(filepath_ComboBox->lineEdit(), &QLineEdit::returnPressed, this, [=]{ read_Data_File(filepath_ComboBox->lineEdit()->text()); });
		layout->addWidget(filepath_ComboBox,0,Qt::AlignLeft);

		// browse
		browse_Button = new QPushButton("Browse...");
			browse_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(browse_Button, &QPushButton::clicked, this, &Target_Curve_Editor::browse_Data_File);
		layout->addWidget(browse_Button,0,Qt::AlignLeft);

		filepath_GroupBox_Layout->addLayout(layout);
	}
}

void Target_Curve_Editor::create_Data_GroupBox()
{
	data_GroupBox = new QGroupBox("Data", this);
#ifdef __linux__
		data_GroupBox->setStyleSheet("QGroupBox{ border-radius: 2px;  border: 1px solid silver; margin-top: 2ex;}"
									 "QGroupBox::title   { subcontrol-origin: margin;   top: 6px; left: 9px; padding: 0 0px 0 1px;}");
#endif
	QVBoxLayout* data_GroupBox_Layout = new QVBoxLayout(data_GroupBox);

	// argument
	{
		QHBoxLayout* layout = new QHBoxLayout;
		layout->setAlignment(Qt::AlignLeft);

		arg_Type_Label = new QLabel("Argument");
			arg_Type_Label->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
		layout->addWidget(arg_Type_Label,0,Qt::AlignLeft);
		arg_Type_ComboBox = new QComboBox;
			arg_Type_ComboBox->addItems(argument_Types);
			arg_Type_ComboBox->setFixedWidth(120);
			arg_Type_ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(arg_Type_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::fill_Arg_Units_ComboBox);
		layout->addWidget(arg_Type_ComboBox,0,Qt::AlignLeft);

		arg_Units_Label = new QLabel("Units");
			arg_Units_Label->setFixedWidth(25);
		layout->addWidget(arg_Units_Label,0,Qt::AlignLeft);
		arg_Units_ComboBox = new QComboBox;
			arg_Units_ComboBox->setFixedWidth(70);
			arg_Units_ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(arg_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::change_Arg_Units_ComboBox);
		layout->addWidget(arg_Units_ComboBox,0,Qt::AlignLeft);

		arg_Offset_Label = new QLabel("Offset");
		layout->addWidget(arg_Offset_Label,0,Qt::AlignLeft);
		arg_Offset_SpinBox = new QDoubleSpinBox;
			arg_Offset_SpinBox->setAccelerated(true);
			arg_Offset_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
			arg_Offset_SpinBox->setDecimals(5);
			arg_Offset_SpinBox->setSingleStep(0.0001);
			arg_Offset_SpinBox->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
			arg_Offset_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//			arg_Offset_Spinbox->setProperty(min_Size_Property, arg_Offset_Spinbox->width());
		layout->addWidget(arg_Offset_SpinBox,0,Qt::AlignLeft);

		arg_Factor_Label = new QLabel("Scale factor");
		layout->addWidget(arg_Factor_Label,0,Qt::AlignLeft);
		arg_Factor_SpinBox = new QDoubleSpinBox;
			arg_Factor_SpinBox->setValue(1);
			arg_Factor_SpinBox->setAccelerated(true);
			arg_Factor_SpinBox->setRange(0, MAX_DOUBLE);
			arg_Factor_SpinBox->setDecimals(5);
			arg_Factor_SpinBox->setSingleStep(0.001);
			arg_Factor_SpinBox->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
			arg_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//			arg_Factor_LineEdit->setProperty(min_Size_Property, arg_Factor_LineEdit->width());
		layout->addWidget(arg_Factor_SpinBox,0,Qt::AlignLeft);

		norm_On_Beam_Intensity = new QCheckBox("Divide on beam intensity");
			norm_On_Beam_Intensity->setChecked(target_Curve->curve.divide_On_Beam_Intensity);
		layout->addWidget(norm_On_Beam_Intensity,0,Qt::AlignLeft);

		data_GroupBox_Layout->addLayout(layout);
	}

	// value
	{
		QHBoxLayout* layout = new QHBoxLayout;
		layout->setAlignment(Qt::AlignLeft);

		val_Function_Label = new QLabel("Function");
			val_Function_Label->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
			layout->addWidget(val_Function_Label,0,Qt::AlignLeft);
		val_Function_ComboBox = new QComboBox;
			val_Function_ComboBox->addItems(value_Function);
			val_Function_ComboBox->setFixedWidth(120);
			val_Function_ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(val_Function_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::fill_Val_Modes_ComboBox );
		layout->addWidget(val_Function_ComboBox,0,Qt::AlignLeft);

		val_Mode_Label = new QLabel("Mode");
			val_Mode_Label->setFixedWidth(25);
		layout->addWidget(val_Mode_Label,0,Qt::AlignLeft);
		val_Mode_ComboBox = new QComboBox;
			val_Mode_ComboBox->setFixedWidth(70);
			val_Mode_ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		layout->addWidget(val_Mode_ComboBox,0,Qt::AlignLeft);

		val_Offset_Label = new QLabel("Offset");
			layout->addWidget(val_Offset_Label,0,Qt::AlignLeft);
		val_Offset_SpinBox = new QDoubleSpinBox;
			val_Offset_SpinBox->setAccelerated(true);
			val_Offset_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
			val_Offset_SpinBox->setDecimals(5);
			val_Offset_SpinBox->setSingleStep(0.001);
			val_Offset_SpinBox->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
			val_Offset_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//			val_Offset_LineEdit->setProperty(min_Size_Property, val_Offset_LineEdit->width());
//			connect(val_Offset_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_Line_Edit );
		layout->addWidget(val_Offset_SpinBox,0,Qt::AlignLeft);

		val_Factor_Label = new QLabel("Scale factor");
			layout->addWidget(val_Factor_Label,0,Qt::AlignLeft);
		val_Factor_SpinBox = new QDoubleSpinBox;
			val_Factor_SpinBox->setValue(1);
			val_Factor_SpinBox->setAccelerated(true);
			val_Factor_SpinBox->setRange(0, MAX_DOUBLE);
			val_Factor_SpinBox->setDecimals(5);
			val_Factor_SpinBox->setSingleStep(0.001);
			val_Factor_SpinBox->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
			val_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//			val_Factor_LineEdit->setProperty(min_Size_Property, val_Factor_LineEdit->width());
//			connect(val_Factor_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_Line_Edit );
		layout->addWidget(val_Factor_SpinBox,0,Qt::AlignLeft);

		beam_Intensity_Label = new QLabel("Incident photons per point");
			layout->addWidget(beam_Intensity_Label,0,Qt::AlignLeft);
		beam_Intensity_LineEdit = new QLineEdit(QString::number(target_Curve->curve.beam_Intensity/*,line_edit_double_format,line_edit_polarization_precision*/));
			beam_Intensity_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
			beam_Intensity_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
			beam_Intensity_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		connect(beam_Intensity_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(beam_Intensity_LineEdit, false);} );
		layout->addWidget(beam_Intensity_LineEdit,0,Qt::AlignLeft);

		data_GroupBox_Layout->addLayout(layout);
	}

	// measurement
	{
		QGridLayout* layout = new QGridLayout;
		layout->setAlignment(Qt::AlignLeft);

		// at fixed
		{
			at_Fixed_Label = new QLabel("At fixed wavelength");
			layout->addWidget(at_Fixed_Label,0,0,5,1,Qt::AlignRight);
			at_Fixed_LineEdit = new QLineEdit;
				at_Fixed_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				at_Fixed_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				at_Fixed_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(at_Fixed_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(at_Fixed_LineEdit, false);} );
			layout->addWidget(at_Fixed_LineEdit,0,1,5,1,Qt::AlignLeft);
			at_Fixed_Units_ComboBox = new QComboBox;
				at_Fixed_Units_ComboBox->addItems(argument_Types);
				at_Fixed_Units_ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
				at_Fixed_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::change_At_Fixed_Units_ComboBox);
			layout->addWidget(at_Fixed_Units_ComboBox,0,2,5,1,Qt::AlignLeft);
		}

		// polarization
		{
			polarization_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Polarization, 0));
			layout->addWidget(polarization_Label,0,3,Qt::AlignRight);
			polarization_LineEdit = new QLineEdit(QString::number(target_Curve->measurement.polarization.value,line_edit_double_format,line_edit_polarization_precision));
				polarization_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				polarization_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				polarization_LineEdit->setValidator(new QDoubleValidator(-1, 1, MAX_PRECISION, this));
			connect(polarization_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(polarization_LineEdit, false);} );
			layout->addWidget(polarization_LineEdit,0,4,Qt::AlignLeft);
		}

		// spectral resolution
		{
			spectral_Resolution_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Spectral_Resolution, 0));
			layout->addWidget(spectral_Resolution_Label,0,6,Qt::AlignRight);
			spectral_Resolution_LineEdit = new QLineEdit(QString::number(target_Curve->measurement.spectral_Resolution.value,line_edit_double_format,line_edit_spectral_resolution_precision));
				spectral_Resolution_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				spectral_Resolution_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				spectral_Resolution_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(spectral_Resolution_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(spectral_Resolution_LineEdit, false);} );
			layout->addWidget(spectral_Resolution_LineEdit,0,7,Qt::AlignLeft);
		}

//		// polarization sensitivity
//		{
//			polarization_Sensitivity_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Polarization_Sensitivity, 0));
//			layout->addWidget(polarization_Sensitivity_Label,1,3,Qt::AlignRight);
//			polarization_Sensitivity_LineEdit = new QLineEdit(QString::number(target_Curve->measurement.polarization_Sensitivity.value,line_edit_double_format,line_edit_polarization_precision));
//				polarization_Sensitivity_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
//				polarization_Sensitivity_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
//				polarization_Sensitivity_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
//				connect(polarization_Sensitivity_LineEdit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit();} );
//			layout->addWidget(polarization_Sensitivity_LineEdit,1,4,Qt::AlignLeft);
//		}
		// background
		{
			background_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Background, 0));
			layout->addWidget(background_Label,1,3,Qt::AlignRight);
			background_LineEdit = new QLineEdit(QString::number(target_Curve->measurement.background.value,line_edit_short_double_format,line_edit_background_precision));
				background_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				background_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				background_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(background_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(background_LineEdit, false);} );
			layout->addWidget(background_LineEdit,1,4,Qt::AlignLeft);
		}

		// angular resolution
		{
			angular_Resolution_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Angular_Resolution, 0));
			layout->addWidget(angular_Resolution_Label,1,6,Qt::AlignRight);
			angular_Resolution_LineEdit = new QLineEdit(QString::number(target_Curve->measurement.angular_Resolution.value,line_edit_double_format,line_edit_angle_precision));
				angular_Resolution_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				angular_Resolution_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				angular_Resolution_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(angular_Resolution_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(angular_Resolution_LineEdit, false);} );
			layout->addWidget(angular_Resolution_LineEdit,1,7,Qt::AlignLeft);
			angular_Units_Label = new QLabel("<ang unit>");
			layout->addWidget(angular_Units_Label,1,8,Qt::AlignLeft);
		}

		/// next group
		// beam size
		{
			beam_Size_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Beam_Size, 0));
			layout->addWidget(beam_Size_Label,3,3,Qt::AlignRight);
			beam_Size_LineEdit = new QLineEdit(QString::number(target_Curve->measurement.beam_Size.value,line_edit_double_format,line_edit_beam_size_precision));
				beam_Size_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				beam_Size_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				beam_Size_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(beam_Size_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(beam_Size_LineEdit, false);} );
			layout->addWidget(beam_Size_LineEdit,3,4,Qt::AlignLeft);
			beam_Size_Units_Label = new QLabel(size_Units_List[mm]);
			layout->addWidget(beam_Size_Units_Label,3,5,Qt::AlignLeft);
		}

		// sample size
		{
			sample_Size_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Sample_Size, 0));
			layout->addWidget(sample_Size_Label,3,6,Qt::AlignRight);
			sample_Size_LineEdit = new QLineEdit(QString::number(target_Curve->measurement.sample_Size.value,line_edit_double_format,line_edit_sample_size_precision));
				sample_Size_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				sample_Size_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				sample_Size_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(sample_Size_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(sample_Size_LineEdit, false);} );
			layout->addWidget(sample_Size_LineEdit,3,7,Qt::AlignLeft);
			sample_Size_Units_Label = new QLabel(size_Units_List[mm]);
			layout->addWidget(sample_Size_Units_Label,3,8,Qt::AlignLeft);
		}

		// beam spreading
		{
			beam_Profile_Spreading_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Beam_Profile_Spreading, 0));
			layout->addWidget(beam_Profile_Spreading_Label,4,3,Qt::AlignRight);
			beam_Profile_Spreading_LineEdit = new QLineEdit(QString::number(target_Curve->measurement.beam_Profile_Spreading.value,line_edit_double_format,line_edit_beam_size_precision));
				beam_Profile_Spreading_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				beam_Profile_Spreading_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				beam_Profile_Spreading_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(beam_Profile_Spreading_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(beam_Profile_Spreading_LineEdit, false);} );
			layout->addWidget(beam_Profile_Spreading_LineEdit,4,4,Qt::AlignLeft);
		}

		// sample shift
		{
			sample_Shift_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Sample_Shift, 0));
			layout->addWidget(sample_Shift_Label,4,6,Qt::AlignRight);
			sample_Shift_LineEdit = new QLineEdit(QString::number(target_Curve->measurement.sample_Shift.value,line_edit_double_format,line_edit_sample_size_precision));
				sample_Shift_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				sample_Shift_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				sample_Shift_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(sample_Shift_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(sample_Shift_LineEdit, false);} );
			layout->addWidget(sample_Shift_LineEdit,4,7,Qt::AlignLeft);
			sample_Shift_Units_Label = new QLabel(size_Units_List[mm]);
			layout->addWidget(sample_Shift_Units_Label,4,8,Qt::AlignLeft);
		}

		data_GroupBox_Layout->addLayout(layout);
	}

	// set up after creation (at the end!)
	arg_Type_ComboBox->currentTextChanged(arg_Type_ComboBox->currentText());
	val_Function_ComboBox->currentTextChanged(val_Function_ComboBox->currentText());

	show_All();

	// at the end!
	// argument line
	connect(arg_Type_ComboBox,	&QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_Argument_Type);
	connect(arg_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_Argument_Units);
	connect(arg_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::show_Angular_Resolution);
	connect(arg_Offset_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Offsets);
	connect(arg_Factor_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Factors);
	connect(norm_On_Beam_Intensity, &QCheckBox::toggled, this, [=]{target_Curve->curve.divide_On_Beam_Intensity = norm_On_Beam_Intensity->isChecked(); });
	connect(norm_On_Beam_Intensity, &QCheckBox::toggled, this, &Target_Curve_Editor::refresh_Beam_Intensity);

	// value line
	connect(val_Function_ComboBox,	&QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_Value_Type );
	connect(val_Mode_ComboBox,		&QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_Value_Mode);
	connect(val_Offset_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Offsets);
	connect(val_Factor_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Factors);
	connect(beam_Intensity_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Beam_Intensity);

	// measurement
	connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_At_Fixed_Units);
	connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::show_Unit_Dependent_Data);

	connect(at_Fixed_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_At_Fixed_Value);

	connect(polarization_LineEdit,				&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Polarization);
//	connect(polarization_Sensitivity_LineEdit,	&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Polarization);
	connect(background_LineEdit,			&QLineEdit::textEdited, [=]{target_Curve->measurement.background.value = background_LineEdit->text().toDouble();});
	connect(spectral_Resolution_LineEdit,	&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Resolution);
	connect(angular_Resolution_LineEdit,	&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Resolution);

	connect(beam_Size_LineEdit,				&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Measurement_Geometry);
	connect(beam_Profile_Spreading_LineEdit,&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Measurement_Geometry);
	connect(sample_Size_LineEdit,			&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Measurement_Geometry);
	connect(sample_Shift_LineEdit,			&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Measurement_Geometry);
}

void Target_Curve_Editor::create_Buttons()
{
	QHBoxLayout* button_Layout = new QHBoxLayout();
	button_Layout->setAlignment(Qt::AlignCenter);

	// close
	{
		close_Button = new QPushButton("Done", this);
			close_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//			close_Button->setFocus();
			close_Button->setDefault(true);
			connect(close_Button, SIGNAL(clicked()), this, SLOT(close()));
		button_Layout->addWidget(close_Button,0,Qt::AlignCenter);
	}

	// read data
	{
		read_Data_Button = new QPushButton("Read Data", this);
			read_Data_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(read_Data_Button, &QPushButton::clicked, this, [=]{ filepath_ComboBox->lineEdit()->returnPressed(); });
		button_Layout->addWidget(read_Data_Button,0,Qt::AlignCenter);
	}

	bottom_Part_Layout->addLayout(button_Layout);
}

void Target_Curve_Editor::resize_ComboBox(QLineEdit* line_Edit)
{
	if(!line_Edit) line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QComboBox* comboBox = qobject_cast<QComboBox*>(line_Edit->parent());

	QString text = line_Edit->text();
	QFontMetrics fm = line_Edit->fontMetrics();
	int width = fm.width(text) + QLINEEDIT_RESIZE_MARGIN;
	if(width>line_Edit->property(min_Size_Property).toInt())
	{
		line_Edit->setFixedWidth(width);
		comboBox->setFixedWidth(line_Edit->width()+QLINEEDIT_TO_QCOMBOBOX_DIFFERENCE);
	} else
	{
		line_Edit->setFixedWidth(line_Edit->property(min_Size_Property).toInt());
		comboBox->setFixedWidth(line_Edit->width()+QLINEEDIT_TO_QCOMBOBOX_DIFFERENCE);
	}
//	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Target_Curve_Editor::show_All()
{
	show_Filepath();
	show_Curve_Data();
//	show_Measurement_Data();
}

void Target_Curve_Editor::show_Filepath()
{
	QString current_Filepath;

	// show directory
	if(target_Curve->filepath.isEmpty()) // check for previous directory
	{
		// default directory
		current_Filepath = QDir::toNativeSeparators(QDir::currentPath()+"/"+default_Measured_Filename);
	} else
	{
		// previous directory
		current_Filepath = QDir::toNativeSeparators(target_Curve->filepath);
	}

	// show filename
	if(!target_Curve->filename.isEmpty()) // check for filename
	{
		current_Filepath += QDir::toNativeSeparators("/"+target_Curve->filename);
	}

	filepath_ComboBox->addItem(current_Filepath);
		QFontMetrics fm = filepath_ComboBox->lineEdit()->fontMetrics();
		int width = fm.width(current_Filepath) + QLINEEDIT_RESIZE_MARGIN;
	filepath_ComboBox->lineEdit()->setProperty(min_Size_Property, width);

	resize_ComboBox(filepath_ComboBox->lineEdit());
}

void Target_Curve_Editor::show_Curve_Data()
{
	// argument type
	{
		if(target_Curve->curve.argument_Type == whats_This_Angle)
		{
			if(target_Curve->curve.angle_Type == angle_Type_Grazing)
				arg_Type_ComboBox->setCurrentIndex(arg_Type_ComboBox->findText(argument_Types[Grazing_angle]));	// Grazing angle
			if(target_Curve->curve.angle_Type == angle_Type_Incidence)
				arg_Type_ComboBox->setCurrentIndex(arg_Type_ComboBox->findText(argument_Types[Incident_angle]));	// Incidence angle
		} else
		if(target_Curve->curve.argument_Type == whats_This_Wavelength)
		{
			arg_Type_ComboBox->setCurrentIndex(arg_Type_ComboBox->findText(argument_Types[Wavelength_Energy]));		// Wavelength/energy
		}
	}

	// value type
	{
		val_Function_ComboBox->setCurrentIndex(val_Function_ComboBox->findText(target_Curve->curve.value_Function));
	}

	// argument units
	{
		if(target_Curve->curve.argument_Type == whats_This_Angle)
			arg_Units_ComboBox->setCurrentIndex(arg_Units_ComboBox->findText(target_Curve->curve.angular_Units));
		if(target_Curve->curve.argument_Type == whats_This_Wavelength)
			arg_Units_ComboBox->setCurrentIndex(arg_Units_ComboBox->findText(target_Curve->curve.spectral_Units));
	}

	// value mode
	{
		val_Mode_ComboBox->setCurrentIndex(val_Mode_ComboBox->findText(target_Curve->curve.value_Mode));
	}

	// offsets
	{
		arg_Offset_SpinBox->setValue(target_Curve->curve.arg_Offset);
		val_Offset_SpinBox->setValue(target_Curve->curve.val_Offset);
	}

	// scale factors
	{
		arg_Factor_SpinBox->setValue(target_Curve->curve.arg_Factor);
		val_Factor_SpinBox->setValue(target_Curve->curve.val_Factor);
	}
}

void Target_Curve_Editor::show_Measurement_Data()
{
//	// at fixed units
//	if(target_Curve->curve.argument_Type == whats_This_Angle)
//	{
//		at_Fixed_Units_ComboBox->setCurrentIndex(at_Fixed_Units_ComboBox->findText(target_Curve->curve.spectral_Units));
//	}
//	if(target_Curve->curve.argument_Type == whats_This_Wavelength)
//	{
//		at_Fixed_Units_ComboBox->setCurrentIndex(at_Fixed_Units_ComboBox->findText(target_Curve->curve.angular_Units+", "+target_Curve->curve.angle_Type));
//	}

//	polarization_LineEdit->setText(QString::number(target_Curve->measurement.polarization.value, line_edit_double_format, line_edit_polarization_precision));
////	polarization_Sensitivity_LineEdit->setText(QString::number(target_Curve->measurement.polarization_Sensitivity.value, line_edit_double_format, line_edit_polarization_precision));
//	spectral_Resolution_LineEdit->setText(QString::number(target_Curve->measurement.spectral_Resolution.value, line_edit_double_format, line_edit_wavelength_precision));

//	show_Unit_Dependent_Data();

//	resize_Line_Edit(at_Fixed_LineEdit);
//	resize_Line_Edit(polarization_LineEdit);
////	resize_Line_Edit(polarization_Sensitivity_LineEdit);
//	resize_Line_Edit(background_LineEdit);
//	resize_Line_Edit(spectral_Resolution_LineEdit);
//	resize_Line_Edit(angular_Resolution_LineEdit);

//	resize_Line_Edit(beam_Size_LineEdit);
//	resize_Line_Edit(beam_Profile_Spreading_LineEdit);
//	resize_Line_Edit(sample_Size_LineEdit);
//	resize_Line_Edit(sample_Shift_LineEdit);
}

void Target_Curve_Editor::show_Unit_Dependent_Data()
{
	// at fixed value
	if(target_Curve->curve.argument_Type == whats_This_Angle)
	{
		double coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);		// spectral units
		at_Fixed_LineEdit->setText(QString::number(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.wavelength.value)/coeff, line_edit_double_format, line_edit_wavelength_precision));
	}
	if(target_Curve->curve.argument_Type == whats_This_Wavelength)
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);				// angular units
		at_Fixed_LineEdit->setText(QString::number(target_Curve->measurement.probe_Angle.value/coeff, line_edit_double_format, line_edit_angle_precision));
	}

	show_Angular_Resolution();
	Global_Variables::resize_Line_Edit(at_Fixed_LineEdit, false);
}

void Target_Curve_Editor::show_Angular_Resolution()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
	angular_Resolution_LineEdit->setText(QString::number(target_Curve->measurement.angular_Resolution.value/coeff, line_edit_double_format, line_edit_angle_precision));
	Global_Variables::resize_Line_Edit(angular_Resolution_LineEdit, false);
}

void Target_Curve_Editor::refresh_Filepath(QString filepath)
{
	QFile file(filepath);
	QFileInfo file_Info(file.fileName());

	if(file.exists() && !file_Info.isDir())
	{
		target_Curve->filename = file_Info.fileName();
		target_Curve->filepath = file_Info.absolutePath();
		is_File_Exists = true;
	} else
	{
		is_File_Exists = false;
		QMessageBox::information(this, "Wrong filename", "File \"" + file_Info.fileName() + "\" doesn't exist");
	}
}

void Target_Curve_Editor::refresh_Argument_Type()
{
	if(arg_Type_ComboBox->currentText() == argument_Types[Grazing_angle])	// Grazing angle
	{
		target_Curve->curve.argument_Type = whats_This_Angle;
		target_Curve->curve.angle_Type = angle_Type_Grazing;
		target_Curve->measurement.angle_Type = target_Curve->curve.angle_Type;
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[Incident_angle])	// Incident angle
	{
		target_Curve->curve.argument_Type = whats_This_Angle;
		target_Curve->curve.angle_Type = angle_Type_Incidence;
		target_Curve->measurement.angle_Type = target_Curve->curve.angle_Type;
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy])	// Wavelength/energy
	{
		target_Curve->curve.argument_Type = whats_This_Wavelength;
	}
	target_Curve->fill_Measurement_With_Data();
	show_Description_Label();
	target_Curve_Plot->refresh_Labels();
}

void Target_Curve_Editor::refresh_Value_Type()
{
	target_Curve->curve.value_Function = val_Function_ComboBox->currentText();
	show_Description_Label();
	target_Curve_Plot->refresh_Labels();
}

void Target_Curve_Editor::refresh_Argument_Units()
{
	if(arg_Type_ComboBox->currentText() == argument_Types[Grazing_angle] || arg_Type_ComboBox->currentText() == argument_Types[Incident_angle])	// angle
	{
		target_Curve->curve.angular_Units = arg_Units_ComboBox->currentText();
	}
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy])							// Wavelength/energy
	{
		target_Curve->curve.spectral_Units = arg_Units_ComboBox->currentText();
	}
	target_Curve->fill_Measurement_With_Data();
	show_Description_Label();
	target_Curve_Plot->refresh_Labels();
}

void Target_Curve_Editor::refresh_Value_Mode()
{
	target_Curve->curve.value_Mode = val_Mode_ComboBox->currentText();
	target_Curve_Plot->refresh_Labels();
}

void Target_Curve_Editor::refresh_At_Fixed_Value()
{
	QString local_Unit = at_Fixed_Units_ComboBox->currentText().split(", ")[0];
	if(arg_Type_ComboBox->currentText() == argument_Types[Grazing_angle] || arg_Type_ComboBox->currentText() == argument_Types[Incident_angle])	// angle
	{
		double coeff = wavelength_Coefficients_Map.value(local_Unit);		// spectral units
		target_Curve->measurement.wavelength.value = Global_Variables::wavelength_Energy(local_Unit, at_Fixed_LineEdit->text().toDouble()*coeff);
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy])															// Wavelength/energy
	{
		double coeff = angle_Coefficients_Map.value(local_Unit);			// angular units

		// be ready to have a bug next line!
		if(at_Fixed_LineEdit->text().toDouble()*coeff<=90)//.+3*pow(10.,-line_edit_angle_precision+1))	// be ready to have a bug!
		{
			target_Curve->measurement.probe_Angle.value = at_Fixed_LineEdit->text().toDouble()*coeff;
		} else
		{
			at_Fixed_LineEdit->setText(QString::number(target_Curve->measurement.probe_Angle.value/coeff, line_edit_double_format, line_edit_angle_precision));
		}
	}
	show_Description_Label();
}

void Target_Curve_Editor::refresh_At_Fixed_Units()
{
	if(arg_Type_ComboBox->currentText() == argument_Types[Grazing_angle] || arg_Type_ComboBox->currentText() == argument_Types[Incident_angle])	// angle
	{
		target_Curve->curve.spectral_Units = at_Fixed_Units_ComboBox->currentText();
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy])	// Wavelength/energy
	{
		target_Curve->curve.angular_Units = at_Fixed_Units_ComboBox->currentText().split(", ")[0];
		target_Curve->curve.angle_Type = at_Fixed_Units_ComboBox->currentText().split(", ")[1];
		target_Curve->measurement.angle_Type = target_Curve->curve.angle_Type;
	}
	show_Description_Label();
	target_Curve_Plot->refresh_Labels();
}

void Target_Curve_Editor::refresh_Offsets()
{
	target_Curve->curve.arg_Offset = arg_Offset_SpinBox->value();
	target_Curve->curve.val_Offset = val_Offset_SpinBox->value();
	target_Curve->fill_Measurement_With_Data();
	show_Description_Label();
	target_Curve_Plot->plot_Data(true);
}

void Target_Curve_Editor::refresh_Factors()
{
	target_Curve->curve.arg_Factor = arg_Factor_SpinBox->value();
	target_Curve->curve.val_Factor = val_Factor_SpinBox->value();
	target_Curve->fill_Measurement_With_Data();
	show_Description_Label();
	target_Curve_Plot->plot_Data(true);
}

void Target_Curve_Editor::refresh_Beam_Intensity()
{
	target_Curve->curve.beam_Intensity = beam_Intensity_LineEdit->text().toDouble();
	target_Curve->fill_Measurement_With_Data();
	show_Description_Label();
	target_Curve_Plot->plot_Data(true);

}

void Target_Curve_Editor::refresh_Polarization()
{
	// polarization
	if(abs(polarization_LineEdit->text().toDouble())<=1)
	{
		target_Curve->measurement.polarization.value = polarization_LineEdit->text().toDouble();
	} else
	{
		polarization_LineEdit->setText(QString::number(target_Curve->measurement.polarization.value, line_edit_double_format, line_edit_polarization_precision));
	}
	// polarization sensitivity
//	target_Curve->measurement.polarization_Sensitivity.value = polarization_Sensitivity_LineEdit->text().toDouble();
}

void Target_Curve_Editor::refresh_Resolution()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
	target_Curve->measurement.angular_Resolution.value = angular_Resolution_LineEdit->text().toDouble()*coeff;
	target_Curve->measurement.spectral_Resolution.value = spectral_Resolution_LineEdit->text().toDouble();
}

void Target_Curve_Editor::refresh_Measurement_Geometry()
{
	target_Curve->measurement.beam_Size.value = beam_Size_LineEdit->text().toDouble();
	target_Curve->measurement.beam_Profile_Spreading.value = beam_Profile_Spreading_LineEdit->text().toDouble();
	target_Curve->measurement.sample_Size.value = sample_Size_LineEdit->text().toDouble();
	target_Curve->measurement.sample_Shift.value = sample_Shift_LineEdit->text().toDouble();
}
