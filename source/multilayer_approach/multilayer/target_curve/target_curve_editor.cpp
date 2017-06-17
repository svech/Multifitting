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
//	target_Curve->import_Data();
}

void Target_Curve_Editor::browse_Data_File()
{
	QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Find File", filepath_ComboBox->lineEdit()->text(), "Text data (*.txt *.dat *.xy);;All files (*.*)"));

	if (!directory.isEmpty())
	{
		if (filepath_ComboBox->findText(directory) == -1)
			filepath_ComboBox->addItem(directory);
		filepath_ComboBox->setCurrentIndex(filepath_ComboBox->findText(directory));
	}
}

void Target_Curve_Editor::fill_Arg_Units_ComboBox(QString arg_Type)
{
	arg_Units_ComboBox->clear();

	if(arg_Type == argument_Types[0] || arg_Type == argument_Types[1])	// grazing of incidence angle
	{
		arg_Units_ComboBox->addItems(angle_Units_List);
	}
	if(arg_Type == argument_Types[2])									// wavelength or energy
	{
		arg_Units_ComboBox->addItems(wavelength_Units_List);
	}
}

void Target_Curve_Editor::fill_Val_Modes_ComboBox(QString val_Mode)
{
	val_Mode_ComboBox->clear();

	if(val_Mode == value_Function[0])	// reflectance
	{
		val_Mode_ComboBox->addItems(value_R_Mode);
	}
	if(val_Mode == value_Function[1])	// transmittance
	{
		val_Mode_ComboBox->addItems(value_T_Mode);
	}
	if(val_Mode == value_Function[2])	// absorptance
	{
		val_Mode_ComboBox->addItems(value_A_Mode);
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
			filepath_ComboBox->addItem(QDir::toNativeSeparators(QDir::currentPath()));
				QFontMetrics fm = filepath_ComboBox->lineEdit()->fontMetrics();
				int width = fm.width(QDir::currentPath()) + QLINEEDIT_RESIZE_MARGIN;
			filepath_ComboBox->lineEdit()->setProperty(min_Size_Property, max(width, width/*600*/));
			connect(filepath_ComboBox->lineEdit(), &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_ComboBox );
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
//			connect(arg_Offset_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_Line_Edit );
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
//			connect(arg_Factor_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_Line_Edit );
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

	// set up after creation
	arg_Type_ComboBox->currentTextChanged(arg_Type_ComboBox->currentText());
	val_Function_ComboBox->currentTextChanged(val_Function_ComboBox->currentText());

	// measurement
	{
		QGridLayout* layout = new QGridLayout;
		layout->setAlignment(Qt::AlignLeft);

		polarization_Label = new QLabel("Polarization");
		layout->addWidget(polarization_Label,0,0,Qt::AlignRight);
		polarization_LineEdit = new QLineEdit;
			polarization_LineEdit->setText(QString::number(default_polarization,line_edit_double_format,line_edit_wavelength_precision));
			polarization_LineEdit->setFixedWidth(50);
			polarization_LineEdit->setProperty(min_Size_Property, 50);
			polarization_LineEdit->setValidator(new QDoubleValidator(-1, 1, MAX_PRECISION, this));
			connect(polarization_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_Line_Edit );
			polarization_LineEdit->textEdited(polarization_LineEdit->text());
		layout->addWidget(polarization_LineEdit,0,1,Qt::AlignLeft);

		polarization_Sensitivity_Label = new QLabel("Polarization sensitivity");
		layout->addWidget(polarization_Sensitivity_Label,1,0,Qt::AlignRight);
		polarization_Sensitivity_LineEdit = new QLineEdit;
			polarization_Sensitivity_LineEdit->setText(QString::number(default_polarization_sensitivity,line_edit_double_format,line_edit_wavelength_precision));
			polarization_Sensitivity_LineEdit->setFixedWidth(50);
			polarization_Sensitivity_LineEdit->setProperty(min_Size_Property, 50);
			polarization_Sensitivity_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(polarization_Sensitivity_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_Line_Edit );
			polarization_Sensitivity_LineEdit->textEdited(polarization_Sensitivity_LineEdit->text());
		layout->addWidget(polarization_Sensitivity_LineEdit,1,1,Qt::AlignLeft);


		spectral_Resolution_Label = new QLabel("Spectral resolution");
		layout->addWidget(spectral_Resolution_Label,0,2,Qt::AlignRight);
		spectral_Resolution_LineEdit = new QLineEdit;
			spectral_Resolution_LineEdit->setText(QString::number(default_spectral_resolution,line_edit_double_format,line_edit_wavelength_precision));
			spectral_Resolution_LineEdit->setFixedWidth(50);
			spectral_Resolution_LineEdit->setProperty(min_Size_Property, 50);
			spectral_Resolution_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(spectral_Resolution_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_Line_Edit );
			spectral_Resolution_LineEdit->textEdited(spectral_Resolution_LineEdit->text());
		layout->addWidget(spectral_Resolution_LineEdit,0,3,Qt::AlignLeft);

		angular_Resolution_Label = new QLabel("Angular resolution");
		layout->addWidget(angular_Resolution_Label,1,2,Qt::AlignRight);
		angular_Resolution_LineEdit = new QLineEdit;
			angular_Resolution_LineEdit->setText(QString::number(default_angular_resolution,line_edit_double_format,line_edit_angle_precision));
			angular_Resolution_LineEdit->setFixedWidth(50);
			angular_Resolution_LineEdit->setProperty(min_Size_Property, 50);
			angular_Resolution_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(angular_Resolution_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::resize_Line_Edit );
			angular_Resolution_LineEdit->textEdited(angular_Resolution_LineEdit->text());
		layout->addWidget(angular_Resolution_LineEdit,1,3,Qt::AlignLeft);
		angular_Units_Label = new QLabel("<ang unit>");
		layout->addWidget(angular_Units_Label,1,4,Qt::AlignRight);

		data_GroupBox_Layout->addLayout(layout);
	}
}

void Target_Curve_Editor::resize_Line_Edit()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());

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

void Target_Curve_Editor::resize_ComboBox()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
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
