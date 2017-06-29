// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "target_curve_editor.h"

Target_Curve_Editor::Target_Curve_Editor(Target_Curve* target_Curve, QWidget *parent) :
	target_Curve(target_Curve),
	QWidget(parent)
{
	create_Main_Layout();

	setWindowTitle("Import Data");
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Target_Curve_Editor::closeEvent(QCloseEvent *event)
{
	event->accept();
}

void Target_Curve_Editor::set_Window_Geometry()
{
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
	main_Layout->layout()->setSizeConstraint( QLayout::SetFixedSize );
}

void Target_Curve_Editor::read_Data_File(QString filepath)
{
	refresh_Filepath(filepath);
}

void Target_Curve_Editor::browse_Data_File()
{
	QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Find File", filepath_ComboBox->lineEdit()->text(), "Text data (*.txt *.dat *.xy);;All files (*.*)"));

	if (!directory.isEmpty())
	{
		if (filepath_ComboBox->findText(directory) == -1)
			filepath_ComboBox->addItem(directory);
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

	if(arg_Type == argument_Types[0] || arg_Type == argument_Types[1])	// grazing of incidence angle
	{
		arg_Units_ComboBox->addItems(angle_Units_List);
		arg_Units_ComboBox->setCurrentIndex(arg_Units_ComboBox->findText(target_Curve->curve.angular_Units));

		at_Fixed_Units_ComboBox->addItems(wavelength_Units_List);
		at_Fixed_Units_ComboBox->setCurrentIndex(at_Fixed_Units_ComboBox->findText(target_Curve->curve.spectral_Units));

		angular_Units_Label->setText(arg_Units_ComboBox->currentText());

		if(	target_Curve->curve.spectral_Units == wavelength_Units_List[0] ||
			target_Curve->curve.spectral_Units == wavelength_Units_List[1] ||
			target_Curve->curve.spectral_Units == wavelength_Units_List[2] )
		{
			at_Fixed_Label->setText("At fixed wavelength:");
		} else
		{
			at_Fixed_Label->setText("At fixed energy:");
		}

		double coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);		// spectral units
		at_Fixed_LineEdit->setText(QString::number(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.lambda_Value)/coeff, line_edit_double_format, line_edit_wavelength_precision));
	}
	if(arg_Type == argument_Types[2])									// wavelength or energy
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
		at_Fixed_LineEdit->setText(QString::number(target_Curve->measurement.angle_Value/coeff, line_edit_double_format, line_edit_angle_precision));
	}
	arg_Units_ComboBox->blockSignals(false);
	at_Fixed_Units_ComboBox->blockSignals(false);

	resize_Line_Edit(at_Fixed_LineEdit);
}

void Target_Curve_Editor::fill_Val_Modes_ComboBox(QString val_Mode)
{
	val_Mode_ComboBox->blockSignals(true);
	val_Mode_ComboBox->clear();

	if(val_Mode == value_Function[0])	// reflectance
	{
		val_Mode_ComboBox->addItems(value_R_Mode);
		val_Mode_ComboBox->setCurrentIndex(val_Mode_ComboBox->findText(target_Curve->curve.value_Mode));
	}
	if(val_Mode == value_Function[1])	// transmittance
	{
		val_Mode_ComboBox->addItems(value_T_Mode);
	}
	if(val_Mode == value_Function[2])	// absorptance
	{
		val_Mode_ComboBox->addItems(value_A_Mode);
	}
	val_Mode_ComboBox->blockSignals(false);
}

void Target_Curve_Editor::change_Arg_Units_ComboBox(QString arg_Units)
{
	if(arg_Type_ComboBox->currentText() == argument_Types[0] || arg_Type_ComboBox->currentText() == argument_Types[1])	// grazing or incidence angle
	{
		angular_Units_Label->setText(arg_Units);
	}
}

void Target_Curve_Editor::change_At_Fixed_Units_ComboBox(QString fixed_Units)
{
	if(arg_Type_ComboBox->currentText() == argument_Types[2])	// wavelength/energy
	{
		angular_Units_Label->setText(fixed_Units.split(", ")[0]);
	}
}

void Target_Curve_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setContentsMargins(4,10,4,0);

	create_Filepath_GroupBox();
		main_Layout->addWidget(filepath_GroupBox);
	create_Data_GroupBox();
		main_Layout->addWidget(data_GroupBox);

	// close
	{
		close_Button = new QPushButton("Done", this);
			close_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			close_Button->setFocus();
			close_Button->setDefault(true);
			connect(close_Button, SIGNAL(clicked()), this, SLOT(close()));
		main_Layout->addWidget(close_Button,0,Qt::AlignCenter);
	}
}

void Target_Curve_Editor::create_Filepath_GroupBox()
{
	filepath_GroupBox = new QGroupBox("File path");
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
	data_GroupBox->setContentsMargins(0,8,0,-4);
	QVBoxLayout* data_GroupBox_Layout = new QVBoxLayout(data_GroupBox);

	// argument
	{
		QHBoxLayout* layout = new QHBoxLayout;
		layout->setAlignment(Qt::AlignLeft);

		arg_Type_Label = new QLabel("Argument");
			arg_Type_Label->setFixedWidth(50);
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
			arg_Offset_SpinBox->setFixedWidth(65);
//			arg_Offset_Spinbox->setProperty(min_Size_Property, arg_Offset_Spinbox->width());

		layout->addWidget(arg_Offset_SpinBox,0,Qt::AlignLeft);

		arg_Factor_Label = new QLabel("Scale factor");
		layout->addWidget(arg_Factor_Label,0,Qt::AlignLeft);
		arg_Factor_SpinBox = new QDoubleSpinBox;
			arg_Factor_SpinBox->setValue(1);
			arg_Factor_SpinBox->setAccelerated(true);
			arg_Factor_SpinBox->setRange(0, MAX_DOUBLE);
			arg_Factor_SpinBox->setDecimals(5);
			arg_Factor_SpinBox->setSingleStep(0.0001);
			arg_Factor_SpinBox->setFixedWidth(65);
//			arg_Factor_LineEdit->setProperty(min_Size_Property, arg_Factor_LineEdit->width());
		layout->addWidget(arg_Factor_SpinBox,0,Qt::AlignLeft);

		data_GroupBox_Layout->addLayout(layout);
	}

	// value
	{
		QHBoxLayout* layout = new QHBoxLayout;
		layout->setAlignment(Qt::AlignLeft);

		val_Function_Label = new QLabel("Function");
			val_Function_Label->setFixedWidth(50);
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
			val_Offset_SpinBox->setSingleStep(0.0001);
			val_Offset_SpinBox->setFixedWidth(65);
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
			val_Factor_SpinBox->setSingleStep(0.0001);
			val_Factor_SpinBox->setFixedWidth(65);
//			val_Factor_LineEdit->setProperty(min_Size_Property, val_Factor_LineEdit->width());
//			connect(val_Factor_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_Line_Edit );
		layout->addWidget(val_Factor_SpinBox,0,Qt::AlignLeft);

		data_GroupBox_Layout->addLayout(layout);
	}

	// measurement
	{
		QGridLayout* layout = new QGridLayout;
		layout->setAlignment(Qt::AlignLeft);

		at_Fixed_Label = new QLabel("At fixed wavelength");
		layout->addWidget(at_Fixed_Label,0,0,2,1,Qt::AlignRight);
		at_Fixed_LineEdit = new QLineEdit;
			at_Fixed_LineEdit->setFixedWidth(50);
			at_Fixed_LineEdit->setProperty(min_Size_Property, 50);
			at_Fixed_LineEdit->setValidator(new QDoubleValidator(-1, 1, MAX_PRECISION, this));
			connect(at_Fixed_LineEdit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit();} );
		layout->addWidget(at_Fixed_LineEdit,0,1,2,1,Qt::AlignLeft);
		at_Fixed_Units_ComboBox = new QComboBox;
			at_Fixed_Units_ComboBox->addItems(argument_Types);
			at_Fixed_Units_ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			at_Fixed_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::change_At_Fixed_Units_ComboBox);
		layout->addWidget(at_Fixed_Units_ComboBox,0,2,2,1,Qt::AlignLeft);


		polarization_Label = new QLabel("Polarization");
		layout->addWidget(polarization_Label,0,3,Qt::AlignRight);
		polarization_LineEdit = new QLineEdit;
			polarization_LineEdit->setText(QString::number(default_polarization,line_edit_double_format,line_edit_wavelength_precision));
			polarization_LineEdit->setFixedWidth(50);
			polarization_LineEdit->setProperty(min_Size_Property, 50);
			polarization_LineEdit->setValidator(new QDoubleValidator(-1, 1, MAX_PRECISION, this));
			connect(polarization_LineEdit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit();} );
		layout->addWidget(polarization_LineEdit,0,4,Qt::AlignLeft);

		polarization_Sensitivity_Label = new QLabel("Polarization sensitivity");
		layout->addWidget(polarization_Sensitivity_Label,1,3,Qt::AlignRight);
		polarization_Sensitivity_LineEdit = new QLineEdit;
			polarization_Sensitivity_LineEdit->setText(QString::number(default_polarization_sensitivity,line_edit_double_format,line_edit_wavelength_precision));
			polarization_Sensitivity_LineEdit->setFixedWidth(50);
			polarization_Sensitivity_LineEdit->setProperty(min_Size_Property, 50);
			polarization_Sensitivity_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(polarization_Sensitivity_LineEdit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit();} );
		layout->addWidget(polarization_Sensitivity_LineEdit,1,4,Qt::AlignLeft);


		spectral_Resolution_Label = new QLabel("Spectral resolution");
		layout->addWidget(spectral_Resolution_Label,0,5,Qt::AlignRight);
		spectral_Resolution_LineEdit = new QLineEdit;
			spectral_Resolution_LineEdit->setText(QString::number(default_spectral_resolution,line_edit_double_format,line_edit_wavelength_precision));
			spectral_Resolution_LineEdit->setFixedWidth(50);
			spectral_Resolution_LineEdit->setProperty(min_Size_Property, 50);
			spectral_Resolution_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(spectral_Resolution_LineEdit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit();} );
		layout->addWidget(spectral_Resolution_LineEdit,0,6,Qt::AlignLeft);

		angular_Resolution_Label = new QLabel("Angular resolution");
		layout->addWidget(angular_Resolution_Label,1,5,Qt::AlignRight);
		angular_Resolution_LineEdit = new QLineEdit;
			angular_Resolution_LineEdit->setText(QString::number(default_angular_resolution,line_edit_double_format,line_edit_angle_precision));
			angular_Resolution_LineEdit->setFixedWidth(50);
			angular_Resolution_LineEdit->setProperty(min_Size_Property, 50);
			angular_Resolution_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(angular_Resolution_LineEdit, &QLineEdit::textEdited, this, [=]{resize_Line_Edit();} );
		layout->addWidget(angular_Resolution_LineEdit,1,6,Qt::AlignLeft);
		angular_Units_Label = new QLabel("<ang unit>");
		layout->addWidget(angular_Units_Label,1,7,Qt::AlignRight);

		data_GroupBox_Layout->addLayout(layout);
	}

	// set up after creation (at the end!)
	arg_Type_ComboBox->currentTextChanged(arg_Type_ComboBox->currentText());
	val_Function_ComboBox->currentTextChanged(val_Function_ComboBox->currentText());

	show_All();

	// at the end!
	// argument line
	connect(arg_Type_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_Argument_Type);
	connect(arg_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_Argument_Units);
	connect(arg_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::show_Angular_Resolution);
	connect(arg_Offset_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Offsets);
	connect(arg_Factor_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Factors);

	// value line
	connect(val_Function_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_Value_Type );
	connect(val_Mode_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_Value_Mode);
	connect(val_Offset_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Offsets);
	connect(val_Factor_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Factors);

	// measurement
	connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_At_Fixed_Units);
	connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::show_Unit_Dependent_Data);
	connect(at_Fixed_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_At_Fixed_Value);

	connect(polarization_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Polarization);
	connect(polarization_Sensitivity_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Polarization);

	connect(spectral_Resolution_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Spectral_Resolution);
	connect(angular_Resolution_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Angular_Resolution);
}

void Target_Curve_Editor::resize_Line_Edit(QLineEdit* line_Edit)
{
	if(!line_Edit) line_Edit = qobject_cast<QLineEdit*>(QObject::sender());

	QString text = line_Edit->text();
	QFontMetrics fm = line_Edit->fontMetrics();
	int width = fm.width(text) + QLINEEDIT_RESIZE_MARGIN;
	if(width>line_Edit->property(min_Size_Property).toInt())
	{
		line_Edit->setFixedWidth(width);
	} else
	{
		line_Edit->setFixedWidth(line_Edit->property(min_Size_Property).toInt());
	}
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
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
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Target_Curve_Editor::show_All()
{
	show_Filepath();
	show_Curve_Data();
	show_Measurement_Data();
}

void Target_Curve_Editor::show_Filepath()
{
	QString current_Filepath;

	// show directory
	if(target_Curve->filepath.isEmpty()) // check for previous directory
	{
		// default directory
		current_Filepath = QDir::toNativeSeparators(QDir::currentPath());
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
			if(target_Curve->curve.angle_Type == Angle_Type::Grazing())
				arg_Type_ComboBox->setCurrentIndex(arg_Type_ComboBox->findText(argument_Types[0]));	// Grazing angle
			if(target_Curve->curve.angle_Type == Angle_Type::Incidence())
				arg_Type_ComboBox->setCurrentIndex(arg_Type_ComboBox->findText(argument_Types[1]));	// Incidence angle
		} else
		if(target_Curve->curve.argument_Type == whats_This_Wavelength)
		{
			arg_Type_ComboBox->setCurrentIndex(arg_Type_ComboBox->findText(argument_Types[2]));		// Wavelength/energy
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
	// at fixed units
	if(target_Curve->curve.argument_Type == whats_This_Angle)
	{
		at_Fixed_Units_ComboBox->setCurrentIndex(at_Fixed_Units_ComboBox->findText(target_Curve->curve.spectral_Units));
	}
	if(target_Curve->curve.argument_Type == whats_This_Wavelength)
	{
		at_Fixed_Units_ComboBox->setCurrentIndex(at_Fixed_Units_ComboBox->findText(target_Curve->curve.angular_Units+", "+target_Curve->curve.angle_Type));
	}

	polarization_LineEdit->setText(QString::number(target_Curve->measurement.polarization.value, line_edit_double_format, line_edit_wavelength_precision));
	polarization_Sensitivity_LineEdit->setText(QString::number(target_Curve->measurement.polarization_Sensitivity.value, line_edit_double_format, line_edit_wavelength_precision));
	spectral_Resolution_LineEdit->setText(QString::number(target_Curve->measurement.spectral_Resolution.value, line_edit_double_format, line_edit_wavelength_precision));

	show_Unit_Dependent_Data();

	resize_Line_Edit(at_Fixed_LineEdit);
	resize_Line_Edit(polarization_LineEdit);
	resize_Line_Edit(polarization_Sensitivity_LineEdit);
	resize_Line_Edit(spectral_Resolution_LineEdit);
	resize_Line_Edit(angular_Resolution_LineEdit);
}

void Target_Curve_Editor::show_Unit_Dependent_Data()
{
	// at fixed value
	if(target_Curve->curve.argument_Type == whats_This_Angle)
	{
		double coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);		// spectral units
		at_Fixed_LineEdit->setText(QString::number(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.lambda_Value)/coeff, line_edit_double_format, line_edit_wavelength_precision));
	}
	if(target_Curve->curve.argument_Type == whats_This_Wavelength)
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);				// angular units
		at_Fixed_LineEdit->setText(QString::number(target_Curve->measurement.angle_Value/coeff, line_edit_double_format, line_edit_angle_precision));
	}

	show_Angular_Resolution();
	resize_Line_Edit(at_Fixed_LineEdit);
}

void Target_Curve_Editor::show_Angular_Resolution()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
	angular_Resolution_LineEdit->setText(QString::number(target_Curve->measurement.angular_Resolution.value/coeff, line_edit_double_format, line_edit_angle_precision));
	resize_Line_Edit(angular_Resolution_LineEdit);
}

void Target_Curve_Editor::refresh_Data()
{
	// UNUSED
}

void Target_Curve_Editor::refresh_Filepath(QString filepath)
{
	QFile file(filepath);
	QFileInfo file_Info(file.fileName());

	if(file.exists() && !file_Info.isDir())
	{
		target_Curve->filename = file_Info.fileName();
		target_Curve->filepath = file_Info.absolutePath();
	} else
	{
		QMessageBox::information(this, "Wrong filename", "File \"" + file_Info.fileName() + "\" doesn't exist");
	}
}

void Target_Curve_Editor::refresh_Curve_Data()
{
	// UNUSED
	refresh_Argument_Type();
	refresh_Value_Type();
	refresh_Argument_Units();
	refresh_Value_Mode();
	refresh_At_Fixed_Value();
	refresh_At_Fixed_Units();
	refresh_Offsets();
	refresh_Factors();
}

void Target_Curve_Editor::refresh_Argument_Type()
{
	if(arg_Type_ComboBox->currentText() == argument_Types[0])	// Grazing angle
	{
		target_Curve->curve.argument_Type = whats_This_Angle;
		target_Curve->curve.angle_Type = Angle_Type::Grazing();
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[1])	// Incident angle
	{
		target_Curve->curve.argument_Type = whats_This_Angle;
		target_Curve->curve.angle_Type = Angle_Type::Incidence();
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[2])	// Wavelength/energy
	{
		target_Curve->curve.argument_Type = whats_This_Wavelength;
	}
}

void Target_Curve_Editor::refresh_Value_Type()
{
	target_Curve->curve.value_Function = val_Function_ComboBox->currentText();
}

void Target_Curve_Editor::refresh_Argument_Units()
{
	if(arg_Type_ComboBox->currentText() == argument_Types[0] || arg_Type_ComboBox->currentText() == argument_Types[1])	// angle
		target_Curve->curve.angular_Units = arg_Units_ComboBox->currentText();
	if(arg_Type_ComboBox->currentText() == argument_Types[2])							// Wavelength/energy
		target_Curve->curve.spectral_Units = arg_Units_ComboBox->currentText();
}

void Target_Curve_Editor::refresh_Value_Mode()
{
	target_Curve->curve.value_Mode = val_Mode_ComboBox->currentText();
}

void Target_Curve_Editor::refresh_At_Fixed_Value()
{
	QString local_Unit = at_Fixed_Units_ComboBox->currentText().split(", ")[0];
	if(arg_Type_ComboBox->currentText() == argument_Types[0] || arg_Type_ComboBox->currentText() == argument_Types[1])	// angle
	{
		double coeff = wavelength_Coefficients_Map.value(local_Unit);		// spectral units
		target_Curve->measurement.lambda_Value = Global_Variables::wavelength_Energy(local_Unit, at_Fixed_LineEdit->text().toDouble()*coeff);
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[2])															// Wavelength/energy
	{
		double coeff = angle_Coefficients_Map.value(local_Unit);			// angular units
		target_Curve->measurement.angle_Value = at_Fixed_LineEdit->text().toDouble()*coeff;
	}
}

void Target_Curve_Editor::refresh_At_Fixed_Units()
{
	if(arg_Type_ComboBox->currentText() == argument_Types[0] || arg_Type_ComboBox->currentText() == argument_Types[1])	// angle
	{
		target_Curve->curve.spectral_Units = at_Fixed_Units_ComboBox->currentText();
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[2])	// Wavelength/energy
	{
		target_Curve->curve.angular_Units = at_Fixed_Units_ComboBox->currentText().split(", ")[0];
		target_Curve->curve.angle_Type = at_Fixed_Units_ComboBox->currentText().split(", ")[1];
	}
}

void Target_Curve_Editor::refresh_Offsets()
{
	target_Curve->curve.arg_Offset = arg_Offset_SpinBox->value();
	target_Curve->curve.val_Offset = val_Offset_SpinBox->value();
}

void Target_Curve_Editor::refresh_Factors()
{
	target_Curve->curve.arg_Factor = arg_Factor_SpinBox->value();
	target_Curve->curve.val_Factor = val_Factor_SpinBox->value();
}

void Target_Curve_Editor::refresh_Polarization()
{
	// polarization
	if(abs(polarization_LineEdit->text().toDouble())<=1)
		target_Curve->measurement.polarization.value = polarization_LineEdit->text().toDouble();
	else
		polarization_LineEdit->setText(QString::number(target_Curve->measurement.polarization.value, line_edit_double_format, line_edit_wavelength_precision));

	// polarization sensitivity
	target_Curve->measurement.polarization_Sensitivity.value = polarization_Sensitivity_LineEdit->text().toDouble();
}

void Target_Curve_Editor::refresh_Spectral_Resolution()
{
	target_Curve->measurement.spectral_Resolution.value = spectral_Resolution_LineEdit->text().toDouble();
}

void Target_Curve_Editor::refresh_Angular_Resolution()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
	target_Curve->measurement.angular_Resolution.value = angular_Resolution_LineEdit->text().toDouble()*coeff;
}
