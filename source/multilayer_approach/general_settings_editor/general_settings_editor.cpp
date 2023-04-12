#include "general_settings_editor.h"

General_Settings_Editor::General_Settings_Editor(QWidget *parent) : QWidget(parent)
{
	setWindowTitle("General settings");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void General_Settings_Editor::closeEvent(QCloseEvent *event)
{
	write_Window_Geometry();
	general_Settings_Tab_Index = main_Tabs->currentIndex();
	global_Multilayer_Approach->runned_General_Settings_Editor.remove(general_Settings_Key);
	event->accept();
}

void General_Settings_Editor::create_Main_Layout()
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
	main_Tabs->setCurrentIndex(general_Settings_Tab_Index);

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
			reflectivity_Threads_Spinbox->setValue(reflectivity_calc_threads);
			reflectivity_Threads_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		groupbox_Layout->addWidget(reflectivity_Threads_Spinbox,row,1);
		connect(reflectivity_Threads_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
		{
			reflectivity_calc_threads = reflectivity_Threads_Spinbox->value();
			global_Workers.resize(reflectivity_calc_threads);
		});
		row++;
		//----------------------------------------------------------------------------
		QLabel* optical_Constants_Read_Threads_Label = new QLabel("Threads to read optical constants");
		groupbox_Layout->addWidget(optical_Constants_Read_Threads_Label,row,0);

		QSpinBox* optical_Constants_Read_Threads_Spinbox = new QSpinBox;
			optical_Constants_Read_Threads_Spinbox->setRange(1,QThread::idealThreadCount());
			optical_Constants_Read_Threads_Spinbox->setSingleStep(1);
			optical_Constants_Read_Threads_Spinbox->setValue(optical_constants_read_threads);
			optical_Constants_Read_Threads_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		groupbox_Layout->addWidget(optical_Constants_Read_Threads_Spinbox,row,1);
		connect(optical_Constants_Read_Threads_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
		{
			optical_constants_read_threads = optical_Constants_Read_Threads_Spinbox->value();
		});
		row++;
	}

	QGroupBox* recalculation_Groupbox = new QGroupBox("Immediate recalculation");
	layout->addWidget(recalculation_Groupbox);
	{
		QVBoxLayout* groupbox_Layout = new QVBoxLayout(recalculation_Groupbox);

		//----------------------------------------------------------------------------

		QCheckBox* global_Recalculation_Checkbox = new QCheckBox("Recalculate on any change");
			global_Recalculation_Checkbox->setChecked(recalculate_spinbox_global);
		groupbox_Layout->addWidget(global_Recalculation_Checkbox);
		connect(global_Recalculation_Checkbox, &QCheckBox::toggled,	[=]{recalculate_spinbox_global = global_Recalculation_Checkbox->isChecked();});

		//----------------------------------------------------------------------------

		structure_Table_Recalculation_Checkbox = new QCheckBox("Recalculate on change in Structure table");
			structure_Table_Recalculation_Checkbox->setChecked(recalculate_spinbox_structure_table);
		groupbox_Layout->addWidget(structure_Table_Recalculation_Checkbox);
		connect(structure_Table_Recalculation_Checkbox, &QCheckBox::toggled,	[=]
		{
			recalculate_spinbox_structure_table = structure_Table_Recalculation_Checkbox->isChecked();
			if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
			{
				for(int i=0; i<global_Multilayer_Approach->table_Of_Structures->main_Tabs->count(); i++)
				{
					global_Multilayer_Approach->table_Of_Structures->main_Tabs->tabBarClicked(i);
				}
			}
		});

//		//----------------------------------------------------------------------------

//		QCheckBox* ignore_1D_Particles_Checkbox = new QCheckBox("Ignore 1D scattering on particles");
//			ignore_1D_Particles_Checkbox->setChecked(ignore_1D_particles_scattering);
//		groupbox_Layout->addWidget(ignore_1D_Particles_Checkbox);
//		connect(ignore_1D_Particles_Checkbox, &QCheckBox::toggled,	[=]{ignore_1D_particles_scattering = ignore_1D_Particles_Checkbox->isChecked();});
	}

	QGroupBox* other_Groupbox = new QGroupBox("Other");
	layout->addWidget(other_Groupbox);
	{
		QVBoxLayout* groupbox_Layout = new QVBoxLayout(other_Groupbox);

		//----------------------------------------------------------------------------

		QCheckBox* ignore_1D_Particles_Checkbox = new QCheckBox("Ignore 1D scattering on particles");
			ignore_1D_Particles_Checkbox->setChecked(ignore_1D_particles_scattering);
		groupbox_Layout->addWidget(ignore_1D_Particles_Checkbox);
		connect(ignore_1D_Particles_Checkbox, &QCheckBox::toggled,	[=]{ignore_1D_particles_scattering = ignore_1D_Particles_Checkbox->isChecked();});

		//----------------------------------------------------------------------------

//		int row = 0;
//		----------------------------------------------------------------------------
//		QCheckBox* use_Beam_Spot_In_Specular_Scattering_Checkbox = new QCheckBox("Use beam shape for specular peak");
//			use_Beam_Spot_In_Specular_Scattering_Checkbox->setChecked(use_beam_spot_in_specular_peak);
//		groupbox_Layout->addWidget(use_Beam_Spot_In_Specular_Scattering_Checkbox,row,0,1,2);
//		connect(use_Beam_Spot_In_Specular_Scattering_Checkbox, &QCheckBox::toggled, this, [=]
//		{
//			use_beam_spot_in_specular_peak = use_Beam_Spot_In_Specular_Scattering_Checkbox->isChecked();
//			global_Multilayer_Approach->global_Recalculate();
//		});
//		row++;
//		----------------------------------------------------------------------------
//		QLabel* n_Max_Series_Label = new QLabel("Number of terms for DWBA, SA, CSA");
//		groupbox_Layout->addWidget(n_Max_Series_Label,row,0);

//		QSpinBox* n_Max_Series_Spinbox = new QSpinBox;
//			n_Max_Series_Spinbox->setRange(1, 150);
//			n_Max_Series_Spinbox->setSingleStep(1);
//			n_Max_Series_Spinbox->setValue(DWBA_n_Max_Series);
//			n_Max_Series_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
//		groupbox_Layout->addWidget(n_Max_Series_Spinbox,row,1);
//		connect(n_Max_Series_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
//		{
//			DWBA_n_Max_Series = n_Max_Series_Spinbox->value();
//			global_Multilayer_Approach->global_Recalculate();
//		});
//		row++;
	}
}

void General_Settings_Editor::create_Interface_Tab()
{
	QWidget* widget = new QWidget;
	main_Tabs->addTab(widget, "Interface");
	QVBoxLayout* layout = new QVBoxLayout(widget);
		layout->setAlignment(Qt::AlignTop);

	//----------------------------------------------------------------------------

	QGroupBox* names_Groupbox = new QGroupBox("Default titles");
	layout->addWidget(names_Groupbox);
	QGridLayout* names_Layout = new QGridLayout(names_Groupbox);
	{
		QLabel* multilayer_Tab_Name_Label = new QLabel("New structure tab name");
		names_Layout->addWidget(multilayer_Tab_Name_Label,0,0,Qt::AlignLeft);

		QLineEdit* multilayer_Tab_Name_LineEdit = new QLineEdit;
			multilayer_Tab_Name_LineEdit->setText(default_multilayer_tab_name);
			multilayer_Tab_Name_LineEdit->setFixedWidth(95);
		names_Layout->addWidget(multilayer_Tab_Name_LineEdit,0,1/*,Qt::AlignLeft*/);
		connect(multilayer_Tab_Name_LineEdit, &QLineEdit::textEdited, this, [=]
		{
			default_multilayer_tab_name = multilayer_Tab_Name_LineEdit->text();
		});

		//----------------------------------------------------------------------------

		QLabel* independent_Tab_Name_Label = new QLabel("New curve tab name");
		names_Layout->addWidget(independent_Tab_Name_Label,1,0,Qt::AlignLeft);

		QLineEdit* independent_Tab_Name_LineEdit = new QLineEdit;
			independent_Tab_Name_LineEdit->setText(default_independent_curve_tab_name);
			independent_Tab_Name_LineEdit->setFixedWidth(95);
		names_Layout->addWidget(independent_Tab_Name_LineEdit,1,1/*,Qt::AlignLeft*/);
		connect(independent_Tab_Name_LineEdit, &QLineEdit::textEdited, this, [=]
		{
			default_independent_curve_tab_name = independent_Tab_Name_LineEdit->text();
		});
	}

	//----------------------------------------------------------------------------

	QGroupBox* graphs_Groupbox = new QGroupBox("Graphs");
	layout->addWidget(graphs_Groupbox);
	QGridLayout* graphs_Layout = new QGridLayout(graphs_Groupbox);
	graphs_Layout->setAlignment(Qt::AlignLeft);
	{
		QLabel* profile_Plot_Thickness_Label = new QLabel("Profile line thickness");
		graphs_Layout->addWidget(profile_Plot_Thickness_Label,0,0,Qt::AlignLeft);

		MyDoubleSpinBox* profile_Line_Thickness_SpinBox = new MyDoubleSpinBox(nullptr, false);
			profile_Line_Thickness_SpinBox->setAccelerated(true);
			profile_Line_Thickness_SpinBox->setRange(0, 10);
			profile_Line_Thickness_SpinBox->setDecimals(1);
			profile_Line_Thickness_SpinBox->setValue(default_Profile_Line_Thickness);
			profile_Line_Thickness_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			profile_Line_Thickness_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			profile_Line_Thickness_SpinBox->setFixedWidth(30);
		graphs_Layout->addWidget(profile_Line_Thickness_SpinBox,0,1,Qt::AlignLeft);
		connect(profile_Line_Thickness_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			default_Profile_Line_Thickness = profile_Line_Thickness_SpinBox->value();
			Global_Variables::plot_All_Data_in_Profiles();
		});


		//----------------------------------------------------------------------------

		QCheckBox* replot_Graphs_During_Fitting_CheckBox = new QCheckBox("Replot 1D graphs while fitting");
			replot_Graphs_During_Fitting_CheckBox->setChecked(replot_graphs_during_fitting_1D);
		graphs_Layout->addWidget(replot_Graphs_During_Fitting_CheckBox,1,0,1,2,Qt::AlignLeft);
		connect(replot_Graphs_During_Fitting_CheckBox, &QCheckBox::toggled,	[=]{replot_graphs_during_fitting_1D = replot_Graphs_During_Fitting_CheckBox->isChecked();});
	}

        //----------------------------------------------------------------------------

        QGroupBox* console_Groupbox = new QGroupBox("Console");
        layout->addWidget(console_Groupbox);
        QGridLayout* console_Layout = new QGridLayout(console_Groupbox);
        console_Layout->setAlignment(Qt::AlignLeft);
        {
                QLabel* run_SO_Factor_Label = new QLabel("Print step for SwarmOps fitting:");
                console_Layout->addWidget(run_SO_Factor_Label,0,0,Qt::AlignLeft);

                QSpinBox* run_SO_Factor_SpinBox = new QSpinBox;
                    run_SO_Factor_SpinBox->setRange(1, 1000);
                    run_SO_Factor_SpinBox->setValue(print_swarmops_state_step);
                    run_SO_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
                    run_SO_Factor_SpinBox->setAccelerated(true);
                    run_SO_Factor_SpinBox->setFixedWidth(40);
                console_Layout->addWidget(run_SO_Factor_SpinBox,0,1,Qt::AlignLeft);
                connect(run_SO_Factor_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=]
                { print_swarmops_state_step = run_SO_Factor_SpinBox->value(); });

                //----------------------------------------------------------------------------

                QLabel* run_GSL_Factor_Label = new QLabel("Print step for GSL fitting:");
                console_Layout->addWidget(run_GSL_Factor_Label,1,0,Qt::AlignLeft);

                QSpinBox* run_GSL_Factor_SpinBox = new QSpinBox;
                    run_GSL_Factor_SpinBox->setRange(1, 1000);
                    run_GSL_Factor_SpinBox->setValue(print_gsl_state_step);
                    run_GSL_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
                    run_GSL_Factor_SpinBox->setAccelerated(true);
                    run_GSL_Factor_SpinBox->setFixedWidth(40);
                console_Layout->addWidget(run_GSL_Factor_SpinBox,1,1,Qt::AlignLeft);
                connect(run_GSL_Factor_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=]
                { print_gsl_state_step = run_GSL_Factor_SpinBox->value(); });

                //----------------------------------------------------------------------------

                QCheckBox* calculation_Time_CheckBox = new QCheckBox("Show single calculation time");
                calculation_Time_CheckBox->setChecked(calculation_time);
                console_Layout->addWidget(calculation_Time_CheckBox,2,0,1,2,Qt::AlignLeft);
                connect(calculation_Time_CheckBox, &QCheckBox::toggled,	[=]{calculation_time = calculation_Time_CheckBox->isChecked();});

                //----------------------------------------------------------------------------

                QCheckBox* individual_Residuals_CheckBox = new QCheckBox("Show individual residuals");
                individual_Residuals_CheckBox->setChecked(show_residuals);
                console_Layout->addWidget(individual_Residuals_CheckBox,3,0,1,2,Qt::AlignLeft);
                connect(individual_Residuals_CheckBox, &QCheckBox::toggled,	[=]{show_residuals = individual_Residuals_CheckBox->isChecked();});
        }
	//----------------------------------------------------------------------------

	QGroupBox* other_Groupbox = new QGroupBox("Other");
	layout->addWidget(other_Groupbox);
	QVBoxLayout* other_Layout = new QVBoxLayout(other_Groupbox);
	{
		//----------------------------------------------------------------------------

		QCheckBox* resizeable_Windows_CheckBox = new QCheckBox("Make all windows resizable");
			resizeable_Windows_CheckBox->setChecked(make_all_windows_resizeable);
		other_Layout->addWidget(resizeable_Windows_CheckBox);
		connect(resizeable_Windows_CheckBox, &QCheckBox::toggled, [=]
		{
			previous_all_windows_resizeable = make_all_windows_resizeable;
			make_all_windows_resizeable = resizeable_Windows_CheckBox->isChecked();

			global_Multilayer_Approach->reopen_Profile_Plots(true);
			global_Multilayer_Approach->reopen_Optical_Graphs_1D(true);
			global_Multilayer_Approach->reopen_Optical_Graphs_2D(true);
			global_Multilayer_Approach->reopen_Calculation_Settings(true);
		});

		//----------------------------------------------------------------------------

		QCheckBox* synchronize_Tabs_CheckBox = new QCheckBox("Structural tabs synchronization");
			synchronize_Tabs_CheckBox->setChecked(tab_synchronization);
		other_Layout->addWidget(synchronize_Tabs_CheckBox);
		connect(synchronize_Tabs_CheckBox, &QCheckBox::toggled,	[=]{tab_synchronization = synchronize_Tabs_CheckBox->isChecked();});
	}
}

void General_Settings_Editor::create_Output_Tab()
{
	QWidget* widget = new QWidget;
	main_Tabs->addTab(widget, "Input/Output");
	QVBoxLayout* layout = new QVBoxLayout(widget);
		layout->setAlignment(Qt::AlignTop);

	QGroupBox* print_Groupbox = new QGroupBox("Print data to file");
	layout->addWidget(print_Groupbox);
	{
		QVBoxLayout* groupbox_Layout = new QVBoxLayout(print_Groupbox);

		QCheckBox* print_1D_Data = new QCheckBox("Print 1D data on recalculation");
			print_1D_Data->setChecked(print_1D_Data_On_Recalculation);
		groupbox_Layout->addWidget(print_1D_Data);
		connect(print_1D_Data, &QCheckBox::toggled,	[=]{print_1D_Data_On_Recalculation = print_1D_Data->isChecked();});

		//----------------------------------------------------------------------------

		QCheckBox* print_2D_Data = new QCheckBox("Print 2D data on recalculation");
			print_2D_Data->setChecked(print_2D_Data_On_Recalculation);
		groupbox_Layout->addWidget(print_2D_Data);
		connect(print_2D_Data, &QCheckBox::toggled,	[=]{print_2D_Data_On_Recalculation = print_2D_Data->isChecked();});

		//----------------------------------------------------------------------------

		QCheckBox* print_1D_PSD = new QCheckBox("Print 1D PSD from scattering on recalculation");
			print_1D_PSD->setChecked(print_1D_PSD_From_Scattering_On_Recalculation);
		groupbox_Layout->addWidget(print_1D_PSD);
		connect(print_1D_PSD, &QCheckBox::toggled,	[=]{print_1D_PSD_From_Scattering_On_Recalculation = print_1D_PSD->isChecked();});
	}

	QGroupBox* PSD_Units_Groupbox = new QGroupBox("PSD units");
	layout->addWidget(PSD_Units_Groupbox);
	{
		QHBoxLayout* groupbox_Layout = new QHBoxLayout(PSD_Units_Groupbox);
			groupbox_Layout->setAlignment(Qt::AlignLeft);
			groupbox_Layout->setSpacing(30);

		QVBoxLayout* groupbox_1_Layout = new QVBoxLayout;
			groupbox_1_Layout->setSpacing(4);
			groupbox_1_Layout->setAlignment(Qt::AlignLeft);
		groupbox_Layout->addLayout(groupbox_1_Layout);

		QLabel* PSD_Argument_Units_Label = new QLabel("Argument");
		groupbox_1_Layout->addWidget(PSD_Argument_Units_Label);

		QComboBox* PSD_Argument_Units_Combobox = new QComboBox;
			PSD_Argument_Units_Combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			PSD_Argument_Units_Combobox->addItems(spatial_Frequency_Units_List);
			PSD_Argument_Units_Combobox->setCurrentText(spatial_frequency_units_export);
		groupbox_1_Layout->addWidget(PSD_Argument_Units_Combobox);
		connect(PSD_Argument_Units_Combobox, &QComboBox::currentTextChanged, this, [=]
		{
			spatial_frequency_units_export = PSD_Argument_Units_Combobox->currentText();
		});

		//----------------------------------------------------------------------------

		QVBoxLayout* groupbox_2_Layout = new QVBoxLayout;
			groupbox_2_Layout->setSpacing(4);
			groupbox_2_Layout->setAlignment(Qt::AlignLeft);
		groupbox_Layout->addLayout(groupbox_2_Layout);

		QLabel* PSD_1D_Value_Units_Label = new QLabel("1D value");
		groupbox_2_Layout->addWidget(PSD_1D_Value_Units_Label);

		QComboBox* PSD_1D_Value_Units_Combobox = new QComboBox;
			PSD_1D_Value_Units_Combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			PSD_1D_Value_Units_Combobox->addItems(PSD_1D_Value_Units_List);
			PSD_1D_Value_Units_Combobox->setCurrentText(PSD_1D_value_units);
		groupbox_2_Layout->addWidget(PSD_1D_Value_Units_Combobox);
		connect(PSD_1D_Value_Units_Combobox, &QComboBox::currentTextChanged, this, [=]
		{
			PSD_1D_value_units = PSD_1D_Value_Units_Combobox->currentText();
		});

		//----------------------------------------------------------------------------

		QVBoxLayout* groupbox_3_Layout = new QVBoxLayout;
			groupbox_3_Layout->setSpacing(4);
			groupbox_3_Layout->setAlignment(Qt::AlignLeft);
		groupbox_Layout->addLayout(groupbox_3_Layout);

		QLabel* PSD_2D_Value_Units_Label = new QLabel("2D value");
		groupbox_3_Layout->addWidget(PSD_2D_Value_Units_Label);

		QComboBox* PSD_2D_Value_Units_Combobox = new QComboBox;
			PSD_2D_Value_Units_Combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			PSD_2D_Value_Units_Combobox->addItems(PSD_2D_Value_Units_List);
			PSD_2D_Value_Units_Combobox->setCurrentText(PSD_2D_value_units);
		groupbox_3_Layout->addWidget(PSD_2D_Value_Units_Combobox);
		connect(PSD_2D_Value_Units_Combobox, &QComboBox::currentTextChanged, this, [=]
		{
			PSD_2D_value_units = PSD_2D_Value_Units_Combobox->currentText();
		});
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
				QMessageBox::information(nullptr,"No such directory", "Directory\n\""+working_directory+"\"\ndoes not exist");
			}
		});

		//----------------------------------------------------------------------------

		QRadioButton* last_Radiobitton = new QRadioButton("Save/output to last .fit file directory");
			last_Radiobitton->setChecked(use_last_directory);
		groupbox_Layout->addWidget(last_Radiobitton);

                last_LineEdit = new QLineEdit;
			last_LineEdit->setText(last_directory);
			last_LineEdit->setReadOnly(true);
			last_LineEdit->setEnabled(use_last_directory);
		groupbox_Layout->addWidget(last_LineEdit);

		//----------------------------------------------------------------------------

		QButtonGroup* data_ButtonGroup = new QButtonGroup;
			data_ButtonGroup->addButton(multifitting_Radiobitton);
			data_ButtonGroup->addButton(working_Radiobitton);
			data_ButtonGroup->addButton(last_Radiobitton);

		connect(multifitting_Radiobitton, &QRadioButton::toggled, this, [=]
		{
			use_multifitting_directory = multifitting_Radiobitton->isChecked();
			use_working_directory = working_Radiobitton->isChecked();
			use_last_directory = last_Radiobitton->isChecked();

			multifitting_LineEdit->setEnabled(use_multifitting_directory);
			working_LineEdit->setEnabled(use_working_directory);
			working_Button->setEnabled(use_working_directory);
			last_LineEdit->setEnabled(use_last_directory);
		});
		connect(last_Radiobitton, &QRadioButton::toggled, this, [=]
		{
			use_multifitting_directory = multifitting_Radiobitton->isChecked();
			use_working_directory = working_Radiobitton->isChecked();
			use_last_directory = last_Radiobitton->isChecked();

			multifitting_LineEdit->setEnabled(use_multifitting_directory);
			working_LineEdit->setEnabled(use_working_directory);
			working_Button->setEnabled(use_working_directory);
			last_LineEdit->setEnabled(use_last_directory);
		});
		connect(working_Radiobitton, &QRadioButton::toggled, this, [=]
		{
			use_multifitting_directory = multifitting_Radiobitton->isChecked();
			use_working_directory = working_Radiobitton->isChecked();
			use_last_directory = last_Radiobitton->isChecked();

			multifitting_LineEdit->setEnabled(use_multifitting_directory);
			working_LineEdit->setEnabled(use_working_directory);
			working_Button->setEnabled(use_working_directory);
			last_LineEdit->setEnabled(use_last_directory);
		});

		//-----------------------------------------------------------

		QCheckBox* open_Last_File_At_The_Beginning = new QCheckBox("Always open last file");
			open_Last_File_At_The_Beginning->setChecked(open_last_file);
		groupbox_Layout->addWidget(open_Last_File_At_The_Beginning);
		connect(open_Last_File_At_The_Beginning, &QCheckBox::clicked, this, [=]
		{
			open_last_file = open_Last_File_At_The_Beginning->isChecked();
		});
	}
}

void General_Settings_Editor::set_Window_Geometry()
{
	int width_add=0,height_add=0;
	if(make_all_windows_resizeable && !previous_all_windows_resizeable) {width_add+=2; height_add+=2;}

	setGeometry(general_settings_x_corner,general_settings_y_corner,general_settings_width+width_add,general_settings_height+height_add);
	if(!make_all_windows_resizeable) { adjustSize(); }
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
