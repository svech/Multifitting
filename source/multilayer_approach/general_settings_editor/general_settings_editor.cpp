#include "general_settings_editor.h"

General_Settings_Editor::General_Settings_Editor(QWidget *parent) : QWidget(parent)
{
	setWindowTitle("General Settings");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void General_Settings_Editor::closeEvent(QCloseEvent *event)
{
	write_Window_Geometry();
	global_Multilayer_Approach->runned_General_Settings_Editor.remove(general_Settings_Key);
	event->accept();
}

void General_Settings_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
//		main_Layout->setSizeConstraint(QLayout::SetFixedSize);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(0,0,0,0);

	Global_Variables::create_Shortcuts(this);

	main_Tabs = new QTabWidget(this);
		main_Tabs->setMovable(false);
	main_Layout->addWidget(main_Tabs);

	create_Output_Tab();
	create_Calculation_Tab();
	create_Interface_Tab();

	connect(main_Tabs, &QTabWidget::currentChanged,	[=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);
		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index)main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	});
	main_Tabs->currentChanged(0);

	close_Button = new QPushButton("Close");
		close_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		close_Button->setFocus();
		close_Button->setDefault(true);
	main_Layout->addWidget(close_Button,0,Qt::AlignCenter);
	connect(close_Button, &QPushButton::clicked, this, &General_Settings_Editor::close);
}

void General_Settings_Editor::create_Calculation_Tab()
{
	QWidget* widget = new QWidget;
	main_Tabs->addTab(widget, "Calculation");
	QVBoxLayout* layout = new QVBoxLayout(widget);
		layout->setAlignment(Qt::AlignTop);

	QGroupBox* threads_Groupbox = new QGroupBox("Multithreading");
	layout->addWidget(threads_Groupbox);
	{
		QGridLayout* groupbox_Layout = new QGridLayout(threads_Groupbox);
		int row = 0;
		//----------------------------------------------------------------------------
		QLabel* reflectivity_Threads_Label = new QLabel("Threads to calculate reflectivity");
		groupbox_Layout->addWidget(reflectivity_Threads_Label,row,0);

		QSpinBox* reflectivity_Threads_Spinbox = new QSpinBox;
			reflectivity_Threads_Spinbox->setRange(1,QThread::idealThreadCount());
			reflectivity_Threads_Spinbox->setSingleStep(1);
			reflectivity_Threads_Spinbox->setValue(reflectivity_Calc_Threads);
			reflectivity_Threads_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		groupbox_Layout->addWidget(reflectivity_Threads_Spinbox,row,1);
		connect(reflectivity_Threads_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
		{
			reflectivity_Calc_Threads = reflectivity_Threads_Spinbox->value();
			global_Workers.resize(reflectivity_Calc_Threads);
		});
		row++;
		//----------------------------------------------------------------------------
		QLabel* optical_Constants_Read_Threads_Label = new QLabel("Threads to read optical constants");
		groupbox_Layout->addWidget(optical_Constants_Read_Threads_Label,row,0);

		QSpinBox* optical_Constants_Read_Threads_Spinbox = new QSpinBox;
			optical_Constants_Read_Threads_Spinbox->setRange(1,QThread::idealThreadCount());
			optical_Constants_Read_Threads_Spinbox->setSingleStep(1);
			optical_Constants_Read_Threads_Spinbox->setValue(optical_Constants_Read_Threads);
			optical_Constants_Read_Threads_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		groupbox_Layout->addWidget(optical_Constants_Read_Threads_Spinbox,row,1);
		connect(optical_Constants_Read_Threads_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
		{
			optical_Constants_Read_Threads = optical_Constants_Read_Threads_Spinbox->value();
		});
		row++;
	}

	QGroupBox* recalculation_Groupbox = new QGroupBox("Immediate recalculation");
	layout->addWidget(recalculation_Groupbox);
	{
		QVBoxLayout* groupbox_Layout = new QVBoxLayout(recalculation_Groupbox);

		//----------------------------------------------------------------------------

		QCheckBox* global_Recalculation_Checkbox = new QCheckBox("Recalculate on any change");
			global_Recalculation_Checkbox->setChecked(recalculate_Spinbox_Global);
		groupbox_Layout->addWidget(global_Recalculation_Checkbox);
		connect(global_Recalculation_Checkbox, &QCheckBox::toggled,	[=]{recalculate_Spinbox_Global = global_Recalculation_Checkbox->isChecked();});

		//----------------------------------------------------------------------------

		table_Recalculation_Checkbox = new QCheckBox("Recalculate on change in Table");
			table_Recalculation_Checkbox->setChecked(recalculate_Spinbox_Table);
		groupbox_Layout->addWidget(table_Recalculation_Checkbox);
		connect(table_Recalculation_Checkbox, &QCheckBox::toggled,	[=]
		{
			recalculate_Spinbox_Table = table_Recalculation_Checkbox->isChecked();
			if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
			{
				for(int i=0; i<global_Multilayer_Approach->table_Of_Structures->main_Tabs->count(); i++)
				{
					global_Multilayer_Approach->table_Of_Structures->main_Tabs->tabBarClicked(i);
				}
			}
		});
	}
}

void General_Settings_Editor::create_Interface_Tab()
{
	QWidget* widget = new QWidget;
	main_Tabs->addTab(widget, "Interface");
	QVBoxLayout* layout = new QVBoxLayout(widget);
		layout->setAlignment(Qt::AlignTop);

	//----------------------------------------------------------------------------

	QCheckBox* synchronize_Tabs = new QCheckBox("Structural tabs synchronization");
		synchronize_Tabs->setChecked(tab_synchronization);
	layout->addWidget(synchronize_Tabs);
	connect(synchronize_Tabs, &QCheckBox::toggled,	[=]{tab_synchronization = synchronize_Tabs->isChecked();});
}

void General_Settings_Editor::create_Output_Tab()
{
	QWidget* widget = new QWidget;
	main_Tabs->addTab(widget, "Output");
	QVBoxLayout* layout = new QVBoxLayout(widget);
		layout->setAlignment(Qt::AlignTop);

	QGroupBox* print_Groupbox = new QGroupBox("Print data");
	layout->addWidget(print_Groupbox);
	{
		QVBoxLayout* groupbox_Layout = new QVBoxLayout(print_Groupbox);

		QCheckBox* print_1D_Data = new QCheckBox("Print to file 1D data on recalculation");
			print_1D_Data->setChecked(print_1D_Data_On_Recalculation);
		groupbox_Layout->addWidget(print_1D_Data);
		connect(print_1D_Data, &QCheckBox::toggled,	[=]{print_1D_Data_On_Recalculation = print_1D_Data->isChecked();});

		//----------------------------------------------------------------------------

		QCheckBox* print_2D_Data = new QCheckBox("Print to file 2D data on recalculation");
			print_2D_Data->setChecked(print_2D_Data_On_Recalculation);
		groupbox_Layout->addWidget(print_2D_Data);
		connect(print_2D_Data, &QCheckBox::toggled,	[=]{print_2D_Data_On_Recalculation = print_2D_Data->isChecked();});
	}

	QGroupBox* path_Groupbox = new QGroupBox("Paths");
	layout->addWidget(path_Groupbox);
	{
		QVBoxLayout* groupbox_Layout = new QVBoxLayout(path_Groupbox);

		QRadioButton* multifitting_Radiobitton = new QRadioButton("Save/output to Multifitting directory");
			multifitting_Radiobitton->setChecked(use_multifitting_directory);
		groupbox_Layout->addWidget(multifitting_Radiobitton);

		QLineEdit* multifitting_LineEdit = new QLineEdit;
			multifitting_LineEdit->setText(QDir::currentPath());
			multifitting_LineEdit->setReadOnly(true);
			multifitting_LineEdit->setEnabled(use_multifitting_directory);
		groupbox_Layout->addWidget(multifitting_LineEdit);

		//----------------------------------------------------------------------------

		QRadioButton* working_Radiobitton = new QRadioButton("Save/output to chosen directory");
			working_Radiobitton->setChecked(use_working_directory);
		groupbox_Layout->addWidget(working_Radiobitton);

		QHBoxLayout* working_Layout = new QHBoxLayout;
		groupbox_Layout->addLayout(working_Layout);
		QPushButton* working_Button = new QPushButton("Set up");
			working_Button->setEnabled(use_working_directory);
			working_Button->setFixedWidth(55);
		working_Layout->addWidget(working_Button);
		QLineEdit* working_LineEdit = new QLineEdit;
			working_LineEdit->setText(working_directory);
			working_LineEdit->setEnabled(use_working_directory);
		working_Layout->addWidget(working_LineEdit);

		connect(working_Button, &QPushButton::clicked, this, [=]
		{
			QDir temp_Directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, "Set working directory", working_directory, QFileDialog::ShowDirsOnly));
			if(temp_Directory.exists())	working_directory = temp_Directory.absolutePath();
			working_LineEdit->setText(working_directory);
		});
		connect(working_LineEdit, &QLineEdit::returnPressed, this, [=]
		{
			QDir temp_Directory = QDir::toNativeSeparators(working_LineEdit->text());
			if(temp_Directory.exists())
			{
				working_directory = temp_Directory.absolutePath();
			} else
			{
				QMessageBox::information(nullptr,"No such directory", "Directory\n\""+working_directory+"\"\nis not exist");
			}
		});

		//----------------------------------------------------------------------------

		QRadioButton* last_Radiobitton = new QRadioButton("Save/output to last .fit file directory");
			last_Radiobitton->setChecked(use_last_directory);
		groupbox_Layout->addWidget(last_Radiobitton);

		QLineEdit* last_LineEdit = new QLineEdit;
			last_LineEdit->setText(last_directory);
			last_LineEdit->setReadOnly(true);
			last_LineEdit->setEnabled(use_last_directory);
		groupbox_Layout->addWidget(last_LineEdit);

		//----------------------------------------------------------------------------

		QButtonGroup* data_ButtonGroup = new QButtonGroup;
			data_ButtonGroup->addButton(multifitting_Radiobitton);
			data_ButtonGroup->addButton(working_Radiobitton);
			data_ButtonGroup->addButton(last_Radiobitton);

		connect(multifitting_Radiobitton, &QCheckBox::clicked, this, [=]
		{
			use_multifitting_directory = multifitting_Radiobitton->isChecked();
			use_working_directory = working_Radiobitton->isChecked();
			use_last_directory = last_Radiobitton->isChecked();

			multifitting_LineEdit->setEnabled(use_multifitting_directory);
			working_LineEdit->setEnabled(use_working_directory);
			working_Button->setEnabled(use_working_directory);
			last_LineEdit->setEnabled(use_last_directory);
		});
		connect(last_Radiobitton, &QCheckBox::clicked, this, [=]
		{
			use_multifitting_directory = multifitting_Radiobitton->isChecked();
			use_working_directory = working_Radiobitton->isChecked();
			use_last_directory = last_Radiobitton->isChecked();

			multifitting_LineEdit->setEnabled(use_multifitting_directory);
			working_LineEdit->setEnabled(use_working_directory);
			working_Button->setEnabled(use_working_directory);
			last_LineEdit->setEnabled(use_last_directory);
		});
		connect(working_Radiobitton, &QCheckBox::clicked, this, [=]
		{
			use_multifitting_directory = multifitting_Radiobitton->isChecked();
			use_working_directory = working_Radiobitton->isChecked();
			use_last_directory = last_Radiobitton->isChecked();

			multifitting_LineEdit->setEnabled(use_multifitting_directory);
			working_LineEdit->setEnabled(use_working_directory);
			working_Button->setEnabled(use_working_directory);
			last_LineEdit->setEnabled(use_last_directory);
		});
	}
}

void General_Settings_Editor::set_Window_Geometry()
{
	setGeometry(general_settings_x_corner,general_settings_y_corner,general_settings_width,general_settings_height);
	adjustSize();
}

void General_Settings_Editor::write_Window_Geometry()
{
	if(!isMaximized())
	{
		general_settings_x_corner = frameGeometry().x()-corner_x_shift;
		general_settings_y_corner = frameGeometry().y()-corner_y_shift;

		general_settings_width  = geometry().width() +  width_add;
		general_settings_height = geometry().height()+ height_add;
	}
}