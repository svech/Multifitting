#include "target_curve_editor.h"

Target_Curve_Editor::Target_Curve_Editor(Target_Curve* target_Curve, Multilayer* multilayer, QWidget *parent) :
	target_Curve(target_Curve),
	multilayer_Parent(multilayer),
	QDialog(parent)
{
	setGeometry(target_x_corner, target_y_corner,0,0);
	create_Main_Layout();
	setWindowTitle(target_Curve->measurement.measurement_Type);
	setAttribute(Qt::WA_DeleteOnClose);
	setAcceptDrops(true);
}

void Target_Curve_Editor::write_Window_Geometry()
{
	if(!isMaximized())
	{
		target_x_corner = frameGeometry().x()-corner_x_shift;
		target_y_corner = frameGeometry().y()-corner_y_shift;
	}
}

void Target_Curve_Editor::closeEvent(QCloseEvent *event)
{
	multilayer_Parent->runned_Target_Curve_Editors.remove(multilayer_Parent->runned_Target_Curve_Editors.key(this));
	write_Window_Geometry();
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

void Target_Curve_Editor::read_Data_File(QString filepath)
{
	refresh_Filepath(filepath);	
	if(is_File_Exists)
	{
		target_Curve->import_Data_From_File(filepath);
		target_Curve->parse_Data_From_List();
		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();

		if( target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
		{
			global_Multilayer_Approach->target_Added_1D = true;
		}
		if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
		{
			global_Multilayer_Approach->target_Added_2D = true;
		}

		reset_Subinterval();
		target_Curve->refresh_Description_Label();
		multilayer_Parent->set_Index_To_Target_Curves();
	} else
	{
		target_Curve->loaded_And_Ready = false;
	}
	target_Curve_Plot->plot_Data();
}

void Target_Curve_Editor::binning_Changed()
{
	target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
//	reset_Subinterval();
	target_Curve->refresh_Description_Label();
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
	for(QString line : target_Curve->header)
	{
		out << line << endl;
	}

	if( target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		for(int i=0; i<target_Curve->curve.argument.size(); i++)
		{
			out << Locale.toString(target_Curve->curve.argument[i],'f',16) << "\t" << Locale.toString(target_Curve->curve.values[i],'e',12) << endl;
		}
	}
	if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		for(int i=0; i<target_Curve->curve.value_2D.size(); i++)
		{
			for(int j=0; j<target_Curve->curve.value_2D.first().size(); j++)
			{
				out << Locale.toString(target_Curve->curve.value_2D[i][j],'e',12) << "\t";
			}
			out << endl;
		}
	}
	file.close();
}

void Target_Curve_Editor::create_Main_Layout()
{
	QWidget* main_Widget = this;
	if(make_all_windows_resizeable)
	{
		QVBoxLayout* top_Layout = new QVBoxLayout(this);
			top_Layout->setMargin(0);

		main_Widget = new QWidget;
		QScrollArea* scrollArea = new QScrollArea;
			scrollArea->setWidget(main_Widget);
			scrollArea->setWidgetResizable(true);
		top_Layout->addWidget(scrollArea);
	}

	main_Layout = new QVBoxLayout(main_Widget);
		main_Layout->setContentsMargins(4,4,4,0);
		main_Layout->setSpacing(0);

	bottom_Part_Widget = new QWidget;
	bottom_Part_Layout = new QVBoxLayout(bottom_Part_Widget);
		bottom_Part_Layout->setContentsMargins(0,0,0,0);
		bottom_Part_Layout->setSpacing(0);

	create_Plot();		
	create_Filepath_GroupBox();
	create_Data_GroupBox();
	create_Buttons();

	bottom_Part_Widget->adjustSize();
	bottom_Part_Widget->setFixedHeight(bottom_Part_Widget->height());

	target_Curve_Plot->custom_Plot->replot();
	main_Layout->addWidget(bottom_Part_Widget);

	// shortcuts
	Global_Variables::create_Shortcuts(this);

	if(make_all_windows_resizeable)
	{
		main_Widget->adjustSize();
		resize(main_Widget->width()+2,main_Widget->height()+2);
	}
}

void Target_Curve_Editor::create_Plot()
{
	target_Curve_Plot = new Target_Curve_Plot(target_Curve, this);
	main_Layout->addWidget(target_Curve_Plot);
}

void Target_Curve_Editor::create_Filepath_GroupBox()
{
	measurement_GroupBox = new QGroupBox("Measurement");
	bottom_Part_Layout->addWidget(measurement_GroupBox);

	QHBoxLayout* measurement_GroupBox_Layout = new QHBoxLayout(measurement_GroupBox);
		measurement_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// name
	QLabel* name_Label = new QLabel("Name");
	measurement_GroupBox_Layout->addWidget(name_Label,0,Qt::AlignLeft);

	QLineEdit* name_LineEdit = new QLineEdit;
		name_LineEdit->setText(target_Curve->name);
		name_LineEdit->setProperty(min_Size_Property,90);
		name_LineEdit->setFixedWidth(90);
	measurement_GroupBox_Layout->addWidget(name_LineEdit,0,Qt::AlignLeft);
	connect(name_LineEdit, &QLineEdit::textEdited, this, [=]
	{
		target_Curve->name = name_LineEdit->text();
		if( target_Curve->name.size()>0)
		{	target_Curve->index_Name = target_Curve->index + ": " + target_Curve->name;}
		else
		{	target_Curve->index_Name = target_Curve->index;}


		Global_Variables::resize_Line_Edit(name_LineEdit);
		target_Curve->refresh_Description_Label();

		// refresh plot titles
		if(target_Curve->loaded_And_Ready)
		{
			// 1D case
			if(target_Curve->measurement.measurement_Type != measurement_Types[GISAS_Map])
			{
				if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
				{
					if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
					{
						Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
						curve_Plot_1D->set_Title_Text();
					}
				}
			}
			// 2D case
			if(target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
			{
				if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
				{
					if(global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.contains(target_Curve->measurement.id))
					{
						Curve_Plot_2D* curve_Plot_2D = global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.value(target_Curve->measurement.id);
						curve_Plot_2D->set_Title_Text();
					}
				}
			}
		}
	});

	// filepath
	QLabel* filepath_Label = new QLabel("         File path");
	measurement_GroupBox_Layout->addWidget(filepath_Label,0,Qt::AlignLeft);

	filepath_ComboBox = new QComboBox;
		filepath_ComboBox->setEditable(true);
	measurement_GroupBox_Layout->addWidget(filepath_ComboBox,0,Qt::AlignLeft);

	connect(filepath_ComboBox->lineEdit(), &QLineEdit::textEdited, this, [=]{resize_ComboBox();} );
	connect(filepath_ComboBox, &QComboBox::currentTextChanged, this, [=](QString str){ filepath_ComboBox->lineEdit()->textEdited(str); } );
	connect(filepath_ComboBox->lineEdit(), &QLineEdit::returnPressed, this, [=]
	{
		//////////////////////////////////////////////////////
		read_Data_File(filepath_ComboBox->lineEdit()->text());
		//////////////////////////////////////////////////////

		// reopen Calculation_Settings and Optical_Graphs
		global_Multilayer_Approach->reopen_Calculation_Settings();
		// 1D case
		if(target_Curve->measurement.measurement_Type != measurement_Types[GISAS_Map])
		{
			if(target_Curve->loaded_And_Ready)
			{
				double min = *std::min_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				double max = *std::max_element(target_Curve->curve.shifted_Argument.begin(), target_Curve->curve.shifted_Argument.end());
				target_Curve->curve.subinterval_Left = min;
				target_Curve->curve.subinterval_Right = max;
				reset_Subinterval();
			}
			global_Multilayer_Approach->reopen_Optical_Graphs_1D(true, TARGET);			
		}
		// 2D case
		if(target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
		{
			global_Multilayer_Approach->reopen_Optical_Graphs_2D(true, TARGET);
		}
	});

	// browse
	browse_Button = new QPushButton("Browse...");
		browse_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	measurement_GroupBox_Layout->addWidget(browse_Button,0,Qt::AlignLeft);
	connect(browse_Button, &QPushButton::clicked, this, &Target_Curve_Editor::browse_Data_File);

	// show
	show_Filepath();
}

void Target_Curve_Editor::create_Data_GroupBox()
{
	if(	target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		specular_Target_Curve_Part = new Specular_Target_Curve_Part(target_Curve, this);
			bottom_Part_Layout->addWidget(specular_Target_Curve_Part);
	}
	if(	target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
		detector_Target_Curve_Part = new Detector_Target_Curve_Part(target_Curve, this);
			bottom_Part_Layout->addWidget(detector_Target_Curve_Part);
	}
	if(	target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		rocking_Target_Curve_Part = new Rocking_Target_Curve_Part(target_Curve, this);
			bottom_Part_Layout->addWidget(rocking_Target_Curve_Part);
	}
	if(	target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		offset_Target_Curve_Part = new Offset_Target_Curve_Part(target_Curve, this);
			bottom_Part_Layout->addWidget(offset_Target_Curve_Part);
	}
	if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		gisas_Target_Curve_Part = new GISAS_Target_Curve_Part(target_Curve, this);
			bottom_Part_Layout->addWidget(gisas_Target_Curve_Part);
	}
}

void Target_Curve_Editor::reset_Subinterval()
{
	if(	target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		specular_Target_Curve_Part->reset_Subinterval();
	}
	if(	target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
		detector_Target_Curve_Part->reset_Subinterval();
	}
	if(	target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		rocking_Target_Curve_Part->reset_Subinterval();
		rocking_Target_Curve_Part->reset_Range_Specular_Position();
	}
	if(	target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		offset_Target_Curve_Part->reset_Subinterval();
		offset_Target_Curve_Part->fill_Offset();
	}
	if(	target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{		
		// nothing to do
	}
}

void Target_Curve_Editor::create_Buttons()
{
	QHBoxLayout* button_Layout = new QHBoxLayout();
	button_Layout->setAlignment(Qt::AlignCenter);

	// close
	{
		close_Button = new QPushButton("Close", this);
			close_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			close_Button->setDefault(true);
			connect(close_Button, &QPushButton::clicked, this, [=]{ close(); });
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
