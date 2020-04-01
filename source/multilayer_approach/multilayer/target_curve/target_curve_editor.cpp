#include "target_curve_editor.h"

Target_Curve_Editor::Target_Curve_Editor(Target_Curve* target_Curve, Multilayer* multilayer, QWidget *parent) :
	target_Curve(target_Curve),
	multilayer_Parent(multilayer),
	QDialog(parent)
{
	create_Main_Layout();
	setWindowTitle("Import Data: "+target_Curve->target_Data_Type);
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
	if(event->mimeData()->hasUrls())
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
			filepath_ComboBox->lineEdit()->returnPressed();
		}
		++counter;
	}
}

void Target_Curve_Editor::set_Window_Geometry()
{
//	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
//	main_Layout->layout()->setSizeConstraint( QLayout::SetFixedSize );
}

void Target_Curve_Editor::read_Data_File(QString filepath)
{
	refresh_Filepath(filepath);
	if(is_File_Exists)
	{
		target_Curve->import_Data(filepath);
		target_Curve->fill_Measurement_With_Data();
		global_Multilayer_Approach->target_Added = true;
		reset_Subinterval();
	} else
	{
		target_Curve->loaded_And_Ready = false;
	}

	multilayer_Parent->set_Index_To_Target_Curves();

	// refresh plot
	target_Curve_Plot->plot_Data();
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

void Target_Curve_Editor::export_Data_File()
{
	QFile file = QFileDialog::getSaveFileName(this, "Export Loaded Curve", filepath_ComboBox->lineEdit()->text(), "Text data (*.txt *.dat *.xy);;All files (*.*)");
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);
	for(QString line : target_Curve->lines_List)
	{
		out << line << endl;
	}
	file.close();
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
		at_Fixed_LineEdit->setText(Locale.toString(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.wavelength.value)/coeff, line_edit_double_format, line_edit_wavelength_precision));
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
		target_Curve->curve.angle_Type = angle_Type_Grazing;
		at_Fixed_Units_ComboBox->setCurrentIndex(at_Fixed_Units_ComboBox->findText(target_Curve->curve.angular_Units+", "+target_Curve->curve.angle_Type));

		angular_Units_Label->setText(at_Fixed_Units_ComboBox->currentText().split(", ")[0]);

		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);				// angular units
		at_Fixed_LineEdit->setText(Locale.toString(target_Curve->measurement.probe_Angle.value/coeff, line_edit_double_format, line_edit_angle_precision));

		refresh_At_Fixed_Units();
	}
	arg_Units_ComboBox->blockSignals(false);
	at_Fixed_Units_ComboBox->blockSignals(false);

	Global_Variables::resize_Line_Edit(at_Fixed_LineEdit, false);
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

	create_Plot();
		main_Layout->addWidget(target_Curve_Plot);
	create_Filepath_GroupBox();
		bottom_Part_Layout->addWidget(filepath_GroupBox);
	create_Data_GroupBox();
		bottom_Part_Layout->addWidget(data_GroupBox);
	create_Buttons();

	bottom_Part_Widget->adjustSize();
	bottom_Part_Widget->setFixedHeight(bottom_Part_Widget->height());

	target_Curve_Plot->custom_Plot->replot();
	main_Layout->addWidget(bottom_Part_Widget);

	// shortcuts
	Global_Variables::create_Shortcuts(this);
}

void Target_Curve_Editor::create_Plot()
{
	target_Curve_Plot = new Target_Curve_Plot(target_Curve, this);
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
			connect(filepath_ComboBox->lineEdit(), &QLineEdit::returnPressed, this, [=]
			{
				// close graphs
				bool reopen_Graphs = global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key);
				int active_Tab_Optical_Graphs = -2019;
				if(reopen_Graphs)
				{
					active_Tab_Optical_Graphs = global_Multilayer_Approach->runned_Optical_Graphs.value(optical_Graphs_Key)->main_Tabs->currentIndex();
					global_Multilayer_Approach->runned_Optical_Graphs.value(optical_Graphs_Key)->close();
				}
				// close calculation settings
				bool reopen_Calc_Settings = global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key);
				int active_Tab_Calculation_Settings_Editor = -2019;
				if(reopen_Calc_Settings)
				{
					active_Tab_Calculation_Settings_Editor = global_Multilayer_Approach->runned_Calculation_Settings_Editor.value(calc_Settings_Key)->main_Tabs->currentIndex();
					global_Multilayer_Approach->runned_Calculation_Settings_Editor.value(calc_Settings_Key)->close();
				}

				//////////////////////////////////////////////////////
				read_Data_File(filepath_ComboBox->lineEdit()->text());				
				//////////////////////////////////////////////////////

				// reopen graphs
				if(reopen_Graphs)
				{
					global_Multilayer_Approach->open_Optical_Graphs(TARGET);
					global_Multilayer_Approach->runned_Optical_Graphs.value(optical_Graphs_Key)->main_Tabs->setCurrentIndex(active_Tab_Optical_Graphs);
				}				
				// reopen calculation settings
				if(reopen_Calc_Settings)
				{
					global_Multilayer_Approach->open_Calculation_Settings();
					global_Multilayer_Approach->runned_Calculation_Settings_Editor.value(calc_Settings_Key)->main_Tabs->setCurrentIndex(active_Tab_Calculation_Settings_Editor);
				}
			});
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
	create_Value_GroupBox();
	create_Argument_GroupBox();
	create_Beam_GroupBox();
	create_Detector_GroupBox();
	create_Sample_GroupBox();
	create_Footptint_GroupBox();


	// specifying interval for fitting
	{
		QHBoxLayout* layout = new QHBoxLayout;
			layout->setAlignment(Qt::AlignLeft);
		data_GroupBox_Layout->addLayout(layout);

		main_Subinterval_Checkbox = new QCheckBox("Use only data between argument");
			main_Subinterval_Checkbox->setCheckable(true);
			main_Subinterval_Checkbox->setChecked(target_Curve->curve.use_Subinterval);
		layout->addWidget(main_Subinterval_Checkbox);

		from_Subinterval_SpinBox = new MyDoubleSpinBox;
			from_Subinterval_SpinBox->setAccelerated(true);
			if(target_Curve->loaded_And_Ready)
			{
				double min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				double max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				from_Subinterval_SpinBox->setRange(min,max);
			} else
			{
				from_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
			}

			from_Subinterval_SpinBox->setDecimals(4);
			from_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Start);
			from_Subinterval_SpinBox->setSingleStep(0.01);
			from_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		layout->addWidget(from_Subinterval_SpinBox);
		Global_Variables::resize_Line_Edit(from_Subinterval_SpinBox);

		and_Subinterval_Label = new QLabel("  and  ");
		layout->addWidget(and_Subinterval_Label);

		to_Subinterval_SpinBox = new MyDoubleSpinBox;
			to_Subinterval_SpinBox->setAccelerated(true);
			if(target_Curve->loaded_And_Ready)
			{
				double min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				double max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				to_Subinterval_SpinBox->setRange(min,max);
			} else
			{
				to_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
			}
			to_Subinterval_SpinBox->setDecimals(4);
			to_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_End);
			to_Subinterval_SpinBox->setSingleStep(0.01);
			to_Subinterval_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		layout->addWidget(to_Subinterval_SpinBox);
		Global_Variables::resize_Line_Edit(to_Subinterval_SpinBox);

		connect(main_Subinterval_Checkbox, &QCheckBox::toggled, this, [=]
		{
			target_Curve->curve.use_Subinterval = main_Subinterval_Checkbox->isChecked();
			from_Subinterval_SpinBox->setDisabled(!target_Curve->curve.use_Subinterval);
			and_Subinterval_Label   ->setDisabled(!target_Curve->curve.use_Subinterval);
			to_Subinterval_SpinBox  ->setDisabled(!target_Curve->curve.use_Subinterval);

			// show/hide subinterval on plot
			target_Curve_Plot->start_Rect->setVisible(target_Curve->curve.use_Subinterval);
			target_Curve_Plot->end_Rect->setVisible(target_Curve->curve.use_Subinterval);

			target_Curve_Plot->subinterval_Changed_Replot();
			Global_Variables::replot_All_Graphs();
		});

		connect(from_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			target_Curve->curve.subinterval_Start = from_Subinterval_SpinBox->value();
			to_Subinterval_SpinBox->setRange(target_Curve->curve.subinterval_Start,to_Subinterval_SpinBox->maximum());
			to_Subinterval_SpinBox->setValue(max(target_Curve->curve.subinterval_Start,target_Curve->curve.subinterval_End));

			target_Curve_Plot->subinterval_Changed_Replot();
			Global_Variables::replot_All_Graphs();
		});
		connect(to_Subinterval_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			target_Curve->curve.subinterval_End = to_Subinterval_SpinBox->value();
			from_Subinterval_SpinBox->setRange(from_Subinterval_SpinBox->minimum(),target_Curve->curve.subinterval_End);
			from_Subinterval_SpinBox->setValue(min(target_Curve->curve.subinterval_Start,target_Curve->curve.subinterval_End));

			target_Curve_Plot->subinterval_Changed_Replot();
			Global_Variables::replot_All_Graphs();
		});
		main_Subinterval_Checkbox->toggled(target_Curve->curve.use_Subinterval);
	}

	QHBoxLayout* hor_Layout = new QHBoxLayout;
		hor_Layout->setAlignment(Qt::AlignLeft);

	// 1st column
	{
		QVBoxLayout* layout = new QVBoxLayout;
		hor_Layout->addLayout(layout);

		arg_Type_Label = new QLabel("Argument");
			layout->addWidget(arg_Type_Label);

		val_Function_Label = new QLabel("Function");
			layout->addWidget(val_Function_Label);
	}
	// 2nd column
	{
		QVBoxLayout* layout = new QVBoxLayout;
		hor_Layout->addLayout(layout);

		arg_Type_ComboBox = new QComboBox;
			arg_Type_ComboBox->addItems(argument_Types);
			arg_Type_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			arg_Type_ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(arg_Type_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::fill_Arg_Units_ComboBox);
			layout->addWidget(arg_Type_ComboBox);

		val_Function_ComboBox = new QComboBox;
			val_Function_ComboBox->addItems(specular_Value_Function);
//			val_Function_ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			val_Function_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			layout->addWidget(val_Function_ComboBox);
	}
	// 3rd column
	{
		QVBoxLayout* layout = new QVBoxLayout;
		hor_Layout->addLayout(layout);

		arg_Units_Label = new QLabel("Units");
		layout->addWidget(arg_Units_Label);
	}
	// 4th column
	{
		QVBoxLayout* layout = new QVBoxLayout;
		hor_Layout->addLayout(layout);

		arg_Units_ComboBox = new QComboBox;
			arg_Units_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			arg_Units_ComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		connect(arg_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::change_Arg_Units_ComboBox);
		layout->addWidget(arg_Units_ComboBox);

	}
	// 5th column
	{
		QVBoxLayout* layout = new QVBoxLayout;
		hor_Layout->addLayout(layout);

		arg_Offset_Label = new QLabel("Offset");
		layout->addWidget(arg_Offset_Label);

		val_Offset_Label = new QLabel("Offset");
		layout->addWidget(val_Offset_Label);
	}
	// 6th column
	{
		QVBoxLayout* layout = new QVBoxLayout;
		hor_Layout->addLayout(layout);

		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
		arg_Offset_SpinBox = new MyDoubleSpinBox;
			arg_Offset_SpinBox->setAccelerated(true);
			arg_Offset_SpinBox->setRange(-100000, MAX_DOUBLE);
			arg_Offset_SpinBox->setDecimals(6);
			arg_Offset_SpinBox->setSingleStep(0.001/coeff);
			arg_Offset_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		layout->addWidget(arg_Offset_SpinBox);

		val_Offset_SpinBox = new MyDoubleSpinBox;
			val_Offset_SpinBox->setAccelerated(true);
			val_Offset_SpinBox->setRange(-100000, MAX_DOUBLE);
			val_Offset_SpinBox->setDecimals(6);
			val_Offset_SpinBox->setSingleStep(0.001);
			val_Offset_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		layout->addWidget(val_Offset_SpinBox);
	}
	// 7th column
	{
		QVBoxLayout* layout = new QVBoxLayout;
		hor_Layout->addLayout(layout);

		arg_Factor_Label = new QLabel("Scale factor");
		layout->addWidget(arg_Factor_Label);

		val_Factor_Label = new QLabel("Scale factor");
		layout->addWidget(val_Factor_Label);
	}
	// 8th column
	{
		QVBoxLayout* layout = new QVBoxLayout;
		hor_Layout->addLayout(layout);

		arg_Factor_SpinBox = new MyDoubleSpinBox;
			arg_Factor_SpinBox->setAccelerated(true);
			arg_Factor_SpinBox->setRange(0, MAX_DOUBLE);
			arg_Factor_SpinBox->setDecimals(5);
			arg_Factor_SpinBox->setValue(target_Curve->curve.arg_Factor);
			arg_Factor_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
//			arg_Factor_SpinBox->setSingleStep(0.001);
			arg_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		layout->addWidget(arg_Factor_SpinBox);

		val_Factor_SpinBox = new MyDoubleSpinBox;
			val_Factor_SpinBox->setAccelerated(true);
			val_Factor_SpinBox->setRange(0, MAX_DOUBLE);
			val_Factor_SpinBox->setDecimals(5);
			val_Factor_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			val_Factor_SpinBox->setValue(target_Curve->curve.val_Factor.value);
//			val_Factor_SpinBox->setSingleStep(0.001);
			val_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		layout->addWidget(val_Factor_SpinBox);
	}
	// 9th column
	{
		QVBoxLayout* layout = new QVBoxLayout;
		hor_Layout->addLayout(layout);

		// top
		QHBoxLayout* top_Sub_Layout = new QHBoxLayout;
			top_Sub_Layout->setAlignment(Qt::AlignLeft);
		layout->addLayout(top_Sub_Layout);

		norm_On_Beam_Intensity = new QCheckBox("Divide on beam intensity,");
			norm_On_Beam_Intensity->setChecked(target_Curve->curve.divide_On_Beam_Intensity);
		top_Sub_Layout->addWidget(norm_On_Beam_Intensity);

		beam_Intensity_Start_Label = new QLabel("start:");
			beam_Intensity_Start_Label->setDisabled(true);
		top_Sub_Layout->addWidget(beam_Intensity_Start_Label);

		beam_Intensity_Start_SpinBox = new MyDoubleSpinBox;
			beam_Intensity_Start_SpinBox->setAccelerated(true);
			beam_Intensity_Start_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Intensity_Start_SpinBox->setDecimals(0);
			beam_Intensity_Start_SpinBox->setValue(target_Curve->curve.beam_Intensity_Start);
			beam_Intensity_Start_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
//			beam_Intensity_Start_SpinBox->setSingleStep(10);
			beam_Intensity_Start_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Intensity_Start_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		Global_Variables::resize_Line_Edit(beam_Intensity_Start_SpinBox, false);
		top_Sub_Layout->addWidget(beam_Intensity_Start_SpinBox);

		beam_Intensity_Final_Label = new QLabel("  final:");
		top_Sub_Layout->addWidget(beam_Intensity_Final_Label);

		beam_Intensity_Final_SpinBox = new MyDoubleSpinBox;
			beam_Intensity_Final_SpinBox->setAccelerated(true);
			beam_Intensity_Final_SpinBox->setRange(0, MAX_DOUBLE);
			beam_Intensity_Final_SpinBox->setDecimals(0);
			beam_Intensity_Final_SpinBox->setValue(target_Curve->curve.beam_Intensity_Final);
			beam_Intensity_Final_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
//			beam_Intensity_Final_SpinBox->setSingleStep(10);
			beam_Intensity_Final_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			beam_Intensity_Final_SpinBox->setProperty(min_Size_Property, TARGET_BEAM_INTENSITY_WIDTH);
		Global_Variables::resize_Line_Edit(beam_Intensity_Final_SpinBox, false);
		top_Sub_Layout->addWidget(beam_Intensity_Final_SpinBox);

		// bottom
		QHBoxLayout* bottom_Sub_Layout = new QHBoxLayout;
			bottom_Sub_Layout->setAlignment(Qt::AlignLeft);
		layout->addLayout(bottom_Sub_Layout);

		// min
		QLabel* scale_Factor_Min_Label = new QLabel(", min");
			bottom_Sub_Layout->addWidget(scale_Factor_Min_Label);
		MyDoubleSpinBox* scale_Factor_Min_SpinBox = new MyDoubleSpinBox(this);
			scale_Factor_Min_SpinBox->setRange(1E-5,MAX_DOUBLE);
			scale_Factor_Min_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			scale_Factor_Min_SpinBox->setDecimals(5);
//			scale_Factor_Min_SpinBox->setSingleStep(0.01);
			scale_Factor_Min_SpinBox->setValue(target_Curve->curve.val_Factor.fit.min);
			scale_Factor_Min_SpinBox->setAccelerated(true);
			scale_Factor_Min_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		Global_Variables::resize_Line_Edit(scale_Factor_Min_SpinBox, false);
		connect(scale_Factor_Min_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			target_Curve->curve.val_Factor.fit.min = scale_Factor_Min_SpinBox->value();
		});
		bottom_Sub_Layout->addWidget(scale_Factor_Min_SpinBox);

		// max
		QLabel* scale_Factor_Max_Label = new QLabel("max");
			bottom_Sub_Layout->addWidget(scale_Factor_Max_Label);
		MyDoubleSpinBox* scale_Factor_Max_SpinBox = new MyDoubleSpinBox(this);
			scale_Factor_Max_SpinBox->setRange(1E-5,MAX_DOUBLE);
			scale_Factor_Max_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			scale_Factor_Max_SpinBox->setDecimals(5);
//			scale_Factor_Max_SpinBox->setSingleStep(0.01);
			scale_Factor_Max_SpinBox->setValue(target_Curve->curve.val_Factor.fit.max);
			scale_Factor_Max_SpinBox->setAccelerated(true);
			scale_Factor_Max_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		Global_Variables::resize_Line_Edit(scale_Factor_Max_SpinBox, false);
		connect(scale_Factor_Max_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			target_Curve->curve.val_Factor.fit.max = scale_Factor_Max_SpinBox->value();
		});
		bottom_Sub_Layout->addWidget(scale_Factor_Max_SpinBox);
	}
	data_GroupBox_Layout->addLayout(hor_Layout);

	// measurement
	{
		QGridLayout* layout = new QGridLayout;
		layout->setAlignment(Qt::AlignLeft);

		// at fixed
		{
			at_Fixed_Label = new QLabel("At fixed wavelength");
			layout->addWidget(at_Fixed_Label,0,0,5,1,Qt::AlignRight);
			at_Fixed_LineEdit = new QLineEdit;
				at_Fixed_LineEdit->setProperty(min_Size_Property, 85);
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
			polarization_LineEdit = new QLineEdit(Locale.toString(target_Curve->measurement.polarization.value,line_edit_double_format,line_edit_polarization_precision));
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

			spectral_Resolution_SpinBox = new MyDoubleSpinBox;
				spectral_Resolution_SpinBox->setAccelerated(true);
				spectral_Resolution_SpinBox->setRange(0, MAX_DOUBLE);
				spectral_Resolution_SpinBox->setDecimals(8);
				spectral_Resolution_SpinBox->setValue(target_Curve->measurement.spectral_Resolution.value);
				spectral_Resolution_SpinBox->setSingleStep(0.0001);
				spectral_Resolution_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			Global_Variables::resize_Line_Edit(spectral_Resolution_SpinBox, false);
			layout->addWidget(spectral_Resolution_SpinBox,0,7,Qt::AlignLeft);
		}

//		// polarization sensitivity
//		{
//			polarization_Sensitivity_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Polarization_Sensitivity, 0));
//			layout->addWidget(polarization_Sensitivity_Label,1,3,Qt::AlignRight);
//			polarization_Sensitivity_LineEdit = new QLineEdit(Locale.toString(target_Curve->measurement.polarization_Sensitivity.value,line_edit_double_format,line_edit_polarization_precision));
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
			background_LineEdit = new QLineEdit(Locale.toString(target_Curve->measurement.background.value,line_edit_short_double_format,line_edit_background_precision));
				background_LineEdit->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
				background_LineEdit->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
				background_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(background_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(background_LineEdit, false);} );
			layout->addWidget(background_LineEdit,1,4,Qt::AlignLeft);
		}

		// angular resolution
		{
			double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
			angular_Resolution_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Angular_Resolution, 0));
			layout->addWidget(angular_Resolution_Label,1,6,Qt::AlignRight);

			angular_Resolution_SpinBox = new MyDoubleSpinBox;
				angular_Resolution_SpinBox->setAccelerated(true);
				angular_Resolution_SpinBox->setRange(0, MAX_DOUBLE);
				angular_Resolution_SpinBox->setDecimals(8);
				angular_Resolution_SpinBox->setValue(target_Curve->measurement.angular_Resolution.value/coeff);
				angular_Resolution_SpinBox->setSingleStep(0.001/coeff);
				angular_Resolution_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			Global_Variables::resize_Line_Edit(angular_Resolution_SpinBox, false);

			layout->addWidget(angular_Resolution_SpinBox,1,7,Qt::AlignLeft);
			angular_Units_Label = new QLabel("<ang unit>");
			layout->addWidget(angular_Units_Label,1,8,Qt::AlignLeft);
		}

		/// next group
		// beam size
		{
			beam_Size_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Beam_Size, 0));
			layout->addWidget(beam_Size_Label,3,3,Qt::AlignRight);

			beam_Size_SpinBox = new MyDoubleSpinBox;
				beam_Size_SpinBox->setAccelerated(true);
				beam_Size_SpinBox->setRange(0, MAX_DOUBLE);
				beam_Size_SpinBox->setDecimals(line_edit_beam_size_precision);
				beam_Size_SpinBox->setValue(target_Curve->measurement.beam_Size.value);
				beam_Size_SpinBox->setSingleStep(0.001);
				beam_Size_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				beam_Size_SpinBox->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
			Global_Variables::resize_Line_Edit(beam_Size_SpinBox, false);
			layout->addWidget(beam_Size_SpinBox,3,4,Qt::AlignLeft);
			beam_Size_Units_Label = new QLabel(size_Units_List[mm]);
			layout->addWidget(beam_Size_Units_Label,3,5,Qt::AlignLeft);
		}

		// sample size
		{
			sample_Size_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Sample_Size, 0));
			layout->addWidget(sample_Size_Label,3,6,Qt::AlignRight);

			sample_Size_SpinBox = new MyDoubleSpinBox;
				sample_Size_SpinBox->setAccelerated(true);
				sample_Size_SpinBox->setRange(0, MAX_DOUBLE);
				sample_Size_SpinBox->setDecimals(line_edit_sample_size_precision);
				sample_Size_SpinBox->setValue(target_Curve->measurement.sample_Size.value);
				sample_Size_SpinBox->setSingleStep(0.1);
				sample_Size_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				sample_Size_SpinBox->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
			Global_Variables::resize_Line_Edit(sample_Size_SpinBox, false);
			layout->addWidget(sample_Size_SpinBox,3,7,Qt::AlignLeft);
			sample_Size_Units_Label = new QLabel(size_Units_List[mm]);
			layout->addWidget(sample_Size_Units_Label,3,8,Qt::AlignLeft);
		}

		// beam spreading
		{
			beam_Profile_Spreading_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Beam_Profile_Spreading, 0));
			layout->addWidget(beam_Profile_Spreading_Label,4,3,Qt::AlignRight);
			beam_Profile_Spreading_SpinBox = new MyDoubleSpinBox;
				beam_Profile_Spreading_SpinBox->setAccelerated(true);
				beam_Profile_Spreading_SpinBox->setRange(0, MAX_DOUBLE);
				beam_Profile_Spreading_SpinBox->setDecimals(line_edit_beam_size_precision);
				beam_Profile_Spreading_SpinBox->setValue(target_Curve->measurement.beam_Profile_Spreading.value);
				beam_Profile_Spreading_SpinBox->setSingleStep(0.1);
				beam_Profile_Spreading_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				beam_Profile_Spreading_SpinBox->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
			Global_Variables::resize_Line_Edit(beam_Profile_Spreading_SpinBox, false);
			layout->addWidget(beam_Profile_Spreading_SpinBox,4,4,Qt::AlignLeft);
		}

		// sample shift
		{
			sample_Shift_Label = new QLabel(Global_Variables::parameter_Name(target_Curve->measurement, whats_This_Sample_Shift, 0));
			layout->addWidget(sample_Shift_Label,4,6,Qt::AlignRight);
			sample_Shift_SpinBox = new MyDoubleSpinBox;
				sample_Shift_SpinBox->setAccelerated(true);
				sample_Shift_SpinBox->setRange(0, MAX_DOUBLE);
				sample_Shift_SpinBox->setDecimals(line_edit_sample_size_precision);
				sample_Shift_SpinBox->setValue(target_Curve->measurement.sample_Shift.value);
				sample_Shift_SpinBox->setSingleStep(0.1);
				sample_Shift_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				sample_Shift_SpinBox->setProperty(min_Size_Property, TARGET_LINE_EDIT_WIDTH);
			Global_Variables::resize_Line_Edit(sample_Shift_SpinBox, false);
			layout->addWidget(sample_Shift_SpinBox,4,7,Qt::AlignLeft);
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
	connect(norm_On_Beam_Intensity, &QCheckBox::toggled, this, [=]
	{
		target_Curve->curve.divide_On_Beam_Intensity = norm_On_Beam_Intensity->isChecked();
		beam_Intensity_Start_Label  ->setDisabled(!target_Curve->curve.divide_On_Beam_Intensity);
		beam_Intensity_Start_SpinBox->setDisabled(!target_Curve->curve.divide_On_Beam_Intensity);
		beam_Intensity_Final_Label  ->setDisabled(!target_Curve->curve.divide_On_Beam_Intensity);
		beam_Intensity_Final_SpinBox->setDisabled(!target_Curve->curve.divide_On_Beam_Intensity);

	});
	connect(norm_On_Beam_Intensity, &QCheckBox::toggled, this, &Target_Curve_Editor::refresh_Beam_Intensity);
	norm_On_Beam_Intensity->toggled(target_Curve->curve.divide_On_Beam_Intensity);

	// value line
	connect(val_Function_ComboBox,	&QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_Value_Type );
	connect(val_Offset_SpinBox,     static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Offsets);
	connect(val_Factor_SpinBox,     static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Factors);
	connect(beam_Intensity_Start_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Beam_Intensity);
	connect(beam_Intensity_Final_SpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Beam_Intensity);

	// measurement
	connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::refresh_At_Fixed_Units);
	connect(at_Fixed_Units_ComboBox, &QComboBox::currentTextChanged, this, &Target_Curve_Editor::show_Unit_Dependent_Data);

	connect(at_Fixed_LineEdit, &QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_At_Fixed_Value);

	connect(polarization_LineEdit,				&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Polarization);
//	connect(polarization_Sensitivity_LineEdit,	&QLineEdit::textEdited, this, &Target_Curve_Editor::refresh_Polarization);
	connect(background_LineEdit,				&QLineEdit::textEdited, this, [=]
	{
		target_Curve->measurement.background.value = Locale.toDouble(background_LineEdit->text());
//		global_Multilayer_Approach->calc_Reflection(true);
	});
	connect(spectral_Resolution_SpinBox,	static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Resolution);
	connect(angular_Resolution_SpinBox,		static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Resolution);

	connect(beam_Size_SpinBox,				static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Measurement_Geometry);
	connect(beam_Profile_Spreading_SpinBox,	static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Measurement_Geometry);
	connect(sample_Size_SpinBox,			static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Measurement_Geometry);
	connect(sample_Shift_SpinBox,			static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Target_Curve_Editor::refresh_Measurement_Geometry);
}

void Target_Curve_Editor::create_Value_GroupBox()
{
	QGroupBox* value_GroupBox = new QGroupBox("Value", this);
#ifdef __linux__
		value_GroupBox->setStyleSheet("QGroupBox{ border-radius: 2px;  border: 1px solid silver; margin-top: 2ex;}"
									 "QGroupBox::title   { subcontrol-origin: margin;   top: 6px; left: 9px; padding: 0 0px 0 1px;}");
#endif
	QGridLayout* value_GroupBox_Layout = new QVBoxLayout(value_GroupBox);

	QComboBox* value_Type_ComboBox = new QComboBox;
	if( target_Curve->target_Data_Type == target_Data_Types[Specular_Scan] ) {value_Type_ComboBox->addItems(specular_Value_Function);}
		target_Curve->target_Data_Type == target_Data_Types[Detector_Scan] ||
		target_Curve->target_Data_Type == target_Data_Types[Rocking_Curve] ||
		target_Curve->target_Data_Type == target_Data_Types[Offset_Scan] )
	{
		create_Subinterval_Rectangle();
		create_Plot_Options_GroupBox();
	}
	if( target_Curve->target_Data_Type == target_Data_Types[GISAS] )
	{
	}

		value_Type_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		layout->addWidget(val_Function_ComboBox);

}

void Target_Curve_Editor::create_Argument_GroupBox()
{

}

void Target_Curve_Editor::create_Beam_GroupBox()
{

}

void Target_Curve_Editor::create_Detector_GroupBox()
{

}

void Target_Curve_Editor::create_Sample_GroupBox()
{

}

void Target_Curve_Editor::create_Footptint_GroupBox()
{

}

void Target_Curve_Editor::reset_Subinterval()
{
	if(target_Curve->loaded_And_Ready)
	{
		double min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
		double max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
		from_Subinterval_SpinBox->setRange(min,max);
		to_Subinterval_SpinBox->setRange(min,max);

		from_Subinterval_SpinBox->setValue(min);
		to_Subinterval_SpinBox->setValue(max);
	} else
	{
		from_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
		to_Subinterval_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);

		from_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_Start);
		to_Subinterval_SpinBox->setValue(target_Curve->curve.subinterval_End);
	}
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
		read_Data_Button = new QPushButton("Read data", this);
			read_Data_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(read_Data_Button, &QPushButton::clicked, this, [=]{ filepath_ComboBox->lineEdit()->returnPressed(); });
		button_Layout->addWidget(read_Data_Button,0,Qt::AlignCenter);
	}
	// export data
	{
		export_Data_Button = new QPushButton("Export data", this);
			export_Data_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(export_Data_Button, &QPushButton::clicked, this, [=]{ export_Data_File(); });
		button_Layout->addWidget(export_Data_Button,0,Qt::AlignCenter);
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

	// offsets
	{
		arg_Offset_SpinBox->setValue(target_Curve->curve.arg_Offset); Global_Variables::resize_Line_Edit(arg_Offset_SpinBox, false);
		val_Offset_SpinBox->setValue(target_Curve->curve.val_Offset); Global_Variables::resize_Line_Edit(val_Offset_SpinBox, false);
	}

	// scale factors
	{
		arg_Factor_SpinBox->setValue(target_Curve->curve.arg_Factor      ); Global_Variables::resize_Line_Edit(arg_Factor_SpinBox, false);
		val_Factor_SpinBox->setValue(target_Curve->curve.val_Factor.value); Global_Variables::resize_Line_Edit(val_Factor_SpinBox, false);
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

//	polarization_LineEdit->setText(Locale.toString(target_Curve->measurement.polarization.value, line_edit_double_format, line_edit_polarization_precision));
////	polarization_Sensitivity_LineEdit->setText(Locale.toString(target_Curve->measurement.polarization_Sensitivity.value, line_edit_double_format, line_edit_polarization_precision));
//	spectral_Resolution_LineEdit->setText(Locale.toString(target_Curve->measurement.spectral_Resolution.value, line_edit_double_format, line_edit_wavelength_precision));

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
		at_Fixed_LineEdit->setText(Locale.toString(Global_Variables::wavelength_Energy(target_Curve->curve.spectral_Units,target_Curve->measurement.wavelength.value)/coeff, line_edit_double_format, line_edit_wavelength_precision));
	}
	if(target_Curve->curve.argument_Type == whats_This_Wavelength)
	{
		double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);				// angular units
		at_Fixed_LineEdit->setText(Locale.toString(target_Curve->measurement.probe_Angle.value/coeff, line_edit_double_format, line_edit_angle_precision));
	}

	show_Angular_Resolution();
	Global_Variables::resize_Line_Edit(at_Fixed_LineEdit, false);
}

void Target_Curve_Editor::show_Angular_Resolution()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
	angular_Resolution_SpinBox->setValue(target_Curve->measurement.angular_Resolution.value/coeff);
	angular_Resolution_SpinBox->setSingleStep(0.001/coeff);

	if(target_Curve->curve.argument_Type == whats_This_Wavelength)
	{
		coeff = wavelength_Coefficients_Map.value(target_Curve->curve.spectral_Units);
		arg_Offset_SpinBox->setSingleStep(0.01/coeff);
	} else
	{
		arg_Offset_SpinBox->setSingleStep(0.001/coeff);
	}
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

		if(target_Curve->fit_Params.maximize_Integral)
		{
			target_Curve->fit_Params.maximize_Integral = false;
			if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
			{
				Calculation_Settings_Editor* calculation_Settings_Editor = global_Multilayer_Approach->runned_Calculation_Settings_Editor.value(calc_Settings_Key);
				QCheckBox* maximize_Integral_Checkbox = calculation_Settings_Editor->max_Integral_Map.value(target_Curve);
				maximize_Integral_Checkbox->toggled(false);
			}
		}
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[Incident_angle])	// Incident angle
	{
		target_Curve->curve.argument_Type = whats_This_Angle;
		target_Curve->curve.angle_Type = angle_Type_Incidence;
		target_Curve->measurement.angle_Type = target_Curve->curve.angle_Type;

		if(target_Curve->fit_Params.maximize_Integral)
		{
			target_Curve->fit_Params.maximize_Integral = false;
			if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
			{
				Calculation_Settings_Editor* calculation_Settings_Editor = global_Multilayer_Approach->runned_Calculation_Settings_Editor.value(calc_Settings_Key);
				QCheckBox* maximize_Integral_Checkbox = calculation_Settings_Editor->max_Integral_Map.value(target_Curve);
				maximize_Integral_Checkbox->toggled(false);
			}
		}
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy])	// Wavelength/energy
	{
		target_Curve->curve.argument_Type = whats_This_Wavelength;
	}
	target_Curve->fill_Measurement_With_Data();
	target_Curve->show_Description_Label();
	target_Curve_Plot->refresh_Labels();
	show_Angular_Resolution();
}

void Target_Curve_Editor::refresh_Value_Type()
{
	target_Curve->curve.value_Function = val_Function_ComboBox->currentText();
	target_Curve->show_Description_Label();
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
	target_Curve->show_Description_Label();
	target_Curve_Plot->refresh_Labels();
}

void Target_Curve_Editor::refresh_Value_Mode()
{
	target_Curve_Plot->refresh_Labels();
}

void Target_Curve_Editor::refresh_At_Fixed_Value()
{
	QString local_Unit = at_Fixed_Units_ComboBox->currentText().split(", ")[0];
	if(arg_Type_ComboBox->currentText() == argument_Types[Grazing_angle] || arg_Type_ComboBox->currentText() == argument_Types[Incident_angle])	// angle
	{
		double coeff = wavelength_Coefficients_Map.value(local_Unit);		// spectral units
		target_Curve->measurement.wavelength.value = Global_Variables::wavelength_Energy(local_Unit, Locale.toDouble(at_Fixed_LineEdit->text())*coeff);
	} else
	if(arg_Type_ComboBox->currentText() == argument_Types[Wavelength_Energy])															// Wavelength/energy
	{
		double coeff = angle_Coefficients_Map.value(local_Unit);			// angular units

		// be ready to have a bug next line!
		if(Locale.toDouble(at_Fixed_LineEdit->text())*coeff<=90)//.+3*pow(10.,-line_edit_angle_precision+1))	// be ready to have a bug!
		{
			target_Curve->measurement.probe_Angle.value = Locale.toDouble(at_Fixed_LineEdit->text())*coeff;
		} else
		{
			at_Fixed_LineEdit->setText(Locale.toString(target_Curve->measurement.probe_Angle.value/coeff, line_edit_double_format, line_edit_angle_precision));
		}
	}
	target_Curve->show_Description_Label();
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
	target_Curve->show_Description_Label();
	target_Curve_Plot->refresh_Labels();
}

void Target_Curve_Editor::refresh_Offsets()
{
	target_Curve->curve.arg_Offset = arg_Offset_SpinBox->value();
	target_Curve->curve.val_Offset = val_Offset_SpinBox->value();
	target_Curve->fill_Measurement_With_Data();
	target_Curve->show_Description_Label();
	target_Curve_Plot->plot_Data(true);
	global_Multilayer_Approach->calc_Reflection(true);
}

void Target_Curve_Editor::refresh_Factors()
{
	if(arg_Factor_SpinBox->value() > DBL_EPSILON) target_Curve->curve.arg_Factor       = arg_Factor_SpinBox->value();
	if(val_Factor_SpinBox->value() > DBL_EPSILON) target_Curve->curve.val_Factor.value = val_Factor_SpinBox->value();
	target_Curve->fill_Measurement_With_Data();
	target_Curve->show_Description_Label();
	target_Curve_Plot->plot_Data(true);
	global_Multilayer_Approach->calc_Reflection(true);
}

void Target_Curve_Editor::refresh_Beam_Intensity()
{
	target_Curve->curve.beam_Intensity_Start = beam_Intensity_Start_SpinBox->value();
	target_Curve->curve.beam_Intensity_Final = beam_Intensity_Final_SpinBox->value();
	target_Curve->fill_Measurement_With_Data();
	target_Curve->show_Description_Label();
	target_Curve_Plot->plot_Data(true);
	global_Multilayer_Approach->calc_Reflection(true);
}

void Target_Curve_Editor::refresh_Polarization()
{
	// polarization
	if(abs(Locale.toDouble(polarization_LineEdit->text()))<=1)
	{
		target_Curve->measurement.polarization.value = Locale.toDouble(polarization_LineEdit->text());
	} else
	{
		polarization_LineEdit->setText(Locale.toString(target_Curve->measurement.polarization.value, line_edit_double_format, line_edit_polarization_precision));
	}
	// polarization sensitivity
//	target_Curve->measurement.polarization_Sensitivity.value = Locale.toDouble(polarization_Sensitivity_LineEdit->text());
}

void Target_Curve_Editor::refresh_Resolution()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->curve.angular_Units);
	target_Curve->measurement.angular_Resolution.value = angular_Resolution_SpinBox->value()*coeff;
	target_Curve->measurement.spectral_Resolution.value = spectral_Resolution_SpinBox->value();
	global_Multilayer_Approach->calc_Reflection(true);
}

void Target_Curve_Editor::refresh_Measurement_Geometry()
{
	target_Curve->measurement.beam_Size.value = beam_Size_SpinBox->value();
	target_Curve->measurement.beam_Profile_Spreading.value = beam_Profile_Spreading_SpinBox->value();
	target_Curve->measurement.sample_Size.value = sample_Size_SpinBox->value();
	target_Curve->measurement.sample_Shift.value = sample_Shift_SpinBox->value();
	global_Multilayer_Approach->calc_Reflection(true);
}
