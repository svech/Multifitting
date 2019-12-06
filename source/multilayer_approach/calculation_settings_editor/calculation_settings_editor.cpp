#include "calculation_settings_editor.h"

Calculation_Settings_Editor::Calculation_Settings_Editor(QWidget* parent) :
	total_Number_of_Targets(0),
	total_Number_of_Independents(0),
	multilayer_Tabs(global_Multilayer_Approach->multilayer_Tabs),
	QWidget(parent) // nullptr!
{
	setWindowTitle("Calculation Settings");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Calculation_Settings_Editor::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu;
	QAction settings_Action("Settings");
	menu.addAction(&settings_Action);

	connect(&settings_Action, &QAction::triggered, this, [=]{ settings();});

	menu.exec(event->globalPos());
}

void Calculation_Settings_Editor::settings()
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(main_Tabs->currentIndex()));

	QWidget* settings_Window = new QWidget(this);
		settings_Window->setWindowTitle("Calc Editor Settings");
		settings_Window->setWindowModality(Qt::ApplicationModal);
		settings_Window->setAttribute(Qt::WA_DeleteOnClose);
		settings_Window->setWindowFlags(Qt::Window);
		settings_Window->show();

	QVBoxLayout* settings_Main_Layout = new QVBoxLayout(settings_Window);
		settings_Main_Layout->setSpacing(5);
		settings_Main_Layout->setContentsMargins(5,5,5,5);

	// settings group box
	QGroupBox* settings_Group_Box = new QGroupBox;
		settings_Group_Box->setObjectName("settings_Group_Box");
		settings_Group_Box->setStyleSheet("QGroupBox#settings_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 0ex;}"
													"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
	settings_Main_Layout->addWidget(settings_Group_Box);

	QGridLayout* settings_Group_Box_Layout = new QGridLayout(settings_Group_Box);
		settings_Group_Box_Layout->setContentsMargins(5,5,5,5);
	settings_Main_Layout->addWidget(settings_Group_Box);

	// num target rows
	QLabel* target_Rows_Label = new QLabel("Number of \"Measured\" rows");
	QSpinBox* target_Rows_SpinBox = new QSpinBox;
		target_Rows_SpinBox->setRange(1, total_Number_of_Targets);
		target_Rows_SpinBox->setValue(multilayer->num_Target_Rows);
		target_Rows_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		target_Rows_SpinBox->setAccelerated(true);
		target_Rows_SpinBox->setFixedWidth(25);

	settings_Group_Box_Layout->addWidget(target_Rows_Label,		0,0,1,1);
	settings_Group_Box_Layout->addWidget(target_Rows_SpinBox,	0,1,1,1);

	// num independent rows
	QLabel* independent_Rows_Label = new QLabel("Number of \"Independent\" rows");
	QSpinBox* independent_Rows_SpinBox = new QSpinBox;
		independent_Rows_SpinBox->setRange(1, total_Number_of_Independents);
		independent_Rows_SpinBox->setValue(multilayer->num_Independent_Rows);
		independent_Rows_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		independent_Rows_SpinBox->setAccelerated(true);
		independent_Rows_SpinBox->setFixedWidth(25);

	settings_Group_Box_Layout->addWidget(independent_Rows_Label,	1,0,1,1);
	settings_Group_Box_Layout->addWidget(independent_Rows_SpinBox,	1,1,1,1);

	// buttons
	QHBoxLayout* buttons_Layout = new QHBoxLayout;
		buttons_Layout->setSpacing(10);
		buttons_Layout->setContentsMargins(0,0,0,0);

	QPushButton* ok_Button = new QPushButton ("OK");
	QPushButton* cancel_Button = new QPushButton("Cancel");
		buttons_Layout->addWidget(ok_Button);
		buttons_Layout->addWidget(cancel_Button);
	settings_Main_Layout->addLayout(buttons_Layout);

	connect(cancel_Button,  &QPushButton::clicked, this, [=]{settings_Window->close();});
	connect(ok_Button,      &QPushButton::clicked, this, [=]
	{
		int active_Tab = main_Tabs->currentIndex();
		close();
		multilayer->num_Target_Rows = target_Rows_SpinBox->value();
		multilayer->num_Independent_Rows = independent_Rows_SpinBox->value();
		global_Multilayer_Approach->open_Calculation_Settings();
		global_Multilayer_Approach->calculation_Settings_Editor->main_Tabs->setCurrentIndex(active_Tab);
		settings_Window->close();
	});

	settings_Window->adjustSize();
	settings_Window->setFixedSize(settings_Window->size());
}

void Calculation_Settings_Editor::closeEvent(QCloseEvent* event)
{
	write_Window_Geometry();
	global_Multilayer_Approach->runned_Calculation_Settings_Editor.remove(calc_Settings_Key);
	global_Multilayer_Approach->unlock_Mainwindow_Interface();
	event->accept();
}

void Calculation_Settings_Editor::set_Window_Geometry()
{
	setGeometry(calculation_settings_x_corner,calculation_settings_y_corner,calculation_settings_width,calculation_settings_height);
	adjustSize();
}

void Calculation_Settings_Editor::write_Window_Geometry()
{
	if(!isMaximized())
	{
		calculation_settings_x_corner = frameGeometry().x()-corner_x_shift;
		calculation_settings_y_corner = frameGeometry().y()-corner_y_shift;

		calculation_settings_width  = geometry().width() +  width_add;
		calculation_settings_height = geometry().height()+ height_add;
	}
}

void Calculation_Settings_Editor::create_Main_Layout()
{
	can_Change_Index = false;
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(0,0,0,0);

	global_Multilayer_Approach->lock_Mainwindow_Interface();
	Global_Variables::create_Shortcuts(this);
	create_Tabs();
		main_Layout->addWidget(main_Tabs);

	adjustSize();
	add_Tabs();

	QHBoxLayout* button_Layout = new QHBoxLayout;
		button_Layout->setAlignment(Qt::AlignCenter);
		button_Layout->setSpacing(54);
	{
		done_Button = new QPushButton("Done");
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
			done_Button->setDefault(true);
		button_Layout->addWidget(done_Button);
	}
	{
		global_Norm_Button = new QPushButton("Calculate Weights");
			global_Norm_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			global_Norm_Button->setFocus();
			global_Norm_Button->setDefault(true);
		button_Layout->addWidget(global_Norm_Button);
	}
	main_Layout->addLayout(button_Layout);

	// TODO
	connect(done_Button,			 &QPushButton::clicked, this, &Calculation_Settings_Editor::close);
	global_Norm_Button->setDisabled(true);
	can_Change_Index = tab_synchronization;
}

void Calculation_Settings_Editor::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(false);

	connect(main_Tabs,	&QTabWidget::currentChanged,
	[=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);
		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index)main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}

		if(can_Change_Index)
		{
			can_Change_Index = false;
																										 {global_Multilayer_Approach->                multilayer_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key)){global_Multilayer_Approach->table_Of_Structures ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Tables_Of_Roughness.contains(table_Of_Roughness_Key))	 {global_Multilayer_Approach->table_Of_Roughness  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key))			 {global_Multilayer_Approach->optical_Graphs	  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Profile_Plots_Window.contains(profile_Plots_Key))		 {global_Multilayer_Approach->profile_Plots_Window->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			can_Change_Index = tab_synchronization;
		}
	});
}

void Calculation_Settings_Editor::add_Tabs()
{
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		QWidget* new_Widget = new QWidget;

		create_Tab_Content(new_Widget, tab_Index);
		main_Tabs->addTab(new_Widget, multilayer_Tabs->tabText(tab_Index));
	}
	for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
	{
		if(i!=main_Tabs->currentIndex()) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
	}
	main_Tabs->setCurrentIndex(global_Multilayer_Approach->multilayer_Tabs->currentIndex());
}

void Calculation_Settings_Editor::create_Tab_Content(QWidget* new_Widget, int tab_Index)
{
	QVBoxLayout* tab_Layout = new QVBoxLayout(new_Widget);
		tab_Layout->setSpacing(5);
		tab_Layout->setContentsMargins(2,2,2,2);

	{
		QGroupBox* target_Group_Box = new QGroupBox("Measured", this);
			target_Group_Box->setCheckable(true);
			target_Group_Box->setObjectName("target_Group_Box");
			target_Group_Box->setStyleSheet("QGroupBox#target_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
														"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		tab_Layout->addWidget(target_Group_Box);

//		QHBoxLayout* target_Layout = new QHBoxLayout(target_Group_Box);
//		measured_Layout->setSpacing(0);
//		measured_Layout->setContentsMargins(4,4,4,4);
//		connect(measured_Layout,  &QGroupBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);

		target_Group_Box_Vec.append(target_Group_Box);
		load_Target_Parameters(tab_Index);
	}
	{
		QGroupBox* independent_Group_Box = new QGroupBox("Independent", this);
			independent_Group_Box->setCheckable(true);
			independent_Group_Box->setObjectName("independent_Group_Box");
			independent_Group_Box->setStyleSheet("QGroupBox#independent_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
														"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		tab_Layout->addWidget(independent_Group_Box);

//		QHBoxLayout* independent_Layout = new QHBoxLayout(independent_Group_Box);
//		independent_Layout->setSpacing(0);
//		independent_Layout->setContentsMargins(4,4,4,4);
//		connect(independent_Layout,  &QGroupBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);

		independent_Group_Box_Vec.append(independent_Group_Box);
		load_Independent_Parameters(tab_Index);
	}
}

void Calculation_Settings_Editor::load_Target_Parameters(int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	// enable/disable target groupbox
	target_Group_Box_Vec[tab_Index]->setChecked(multilayer->enable_Calc_Target_Curves);
	connect(target_Group_Box_Vec[tab_Index],  &QGroupBox::toggled, this, [=]{
		multilayer->enable_Calc_Target_Curves = target_Group_Box_Vec[tab_Index]->isChecked();
		global_Multilayer_Approach->independent_Target_Added = target_Group_Box_Vec[tab_Index]->isChecked();
		reopen_Optical_Graphs(TARGET);
		activateWindow();
	});

	QVBoxLayout* target_Vertical_Box_Layout = new QVBoxLayout(target_Group_Box_Vec[tab_Index]);
		target_Vertical_Box_Layout->setSpacing(20);
		target_Vertical_Box_Layout->setContentsMargins(7,14,7,7);

	// calculate total number of targets
	total_Number_of_Targets = 0;
	QVector<Target_Curve*> targets;

	for(Target_Curve* target_Curve: multilayer->target_Profiles_Vector)
	{
		if(target_Curve->loaded_And_Ready)
		{
			targets.append(target_Curve);
			total_Number_of_Targets++;
		}
	}

	int targets_in_Short_Row = total_Number_of_Targets/multilayer->num_Target_Rows;
	int additional_Targets = total_Number_of_Targets%multilayer->num_Target_Rows;
	int first_Long_Row_Index = multilayer->num_Target_Rows-additional_Targets;

	int current_Row = 0;
	int targets_in_Filled_Rows = 0;
	int length = -2018;

	QHBoxLayout* horizontal_Layout = new QHBoxLayout;

	int target_Index = 0;
	for(Target_Curve* target_Curve: targets)
	{
		QGroupBox* box = new QGroupBox(target_Curve->label_Text);
			box->setCheckable(true);
			box->setObjectName("box");
			box->setStyleSheet("QGroupBox#box { border-radius: 2px; border: 1px solid gray; margin-top: 2ex;}"
							"QGroupBox::title { subcontrol-origin: margin;  left: 9px; padding: 0 0px 0 1px;}");

		horizontal_Layout->addWidget(box);
			horizontal_Layout->setSpacing(20);
		// switching is here
		if(current_Row < first_Long_Row_Index) length = targets_in_Short_Row; else length = targets_in_Short_Row + 1;
		if(target_Index == targets_in_Filled_Rows+length-1)
		{
			targets_in_Filled_Rows += length;
			current_Row++;
			target_Vertical_Box_Layout->addLayout(horizontal_Layout);
			horizontal_Layout = new QHBoxLayout; // may be memory leak at the end
		}

		box->setChecked(target_Curve->fit_Params.calc);
		connect(box,  &QGroupBox::toggled, this, [=]{
			target_Curve->fit_Params.calc = qobject_cast<QGroupBox*>(sender())->isChecked();
			global_Multilayer_Approach->target_Added = qobject_cast<QGroupBox*>(sender())->isChecked();
			reopen_Optical_Graphs(TARGET);
			activateWindow();
		});

		QVBoxLayout* box_Layout = new QVBoxLayout(box);
			box_Layout->setSpacing(5);
			box_Layout->setContentsMargins(5,10,5,5);

		// content
		{
			{
				QCheckBox* fit = new QCheckBox("Fit");
					box_Layout->addWidget(fit);
					fit->setChecked(target_Curve->fit_Params.fit);
				connect(fit,  &QCheckBox::toggled, this, [=]{ target_Curve->fit_Params.fit = fit->isChecked(); });

				QLabel* weight_Label = new QLabel("Weight");
				QLineEdit* weight_Line_Edit = new QLineEdit(Locale.toString(target_Curve->fit_Params.weight));
					weight_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

				QHBoxLayout* weight_Layout = new QHBoxLayout;
					weight_Layout->addWidget(weight_Label);
					weight_Layout->addWidget(weight_Line_Edit);

				box_Layout->addLayout(weight_Layout);
				connect(weight_Line_Edit,  &QLineEdit::textEdited, this, [=]
				{
					target_Curve->fit_Params.weight      =      Locale.toDouble(weight_Line_Edit->text());
					target_Curve->fit_Params.weight_Sqrt = sqrt(Locale.toDouble(weight_Line_Edit->text()));
				});
				different_Lines.append(weight_Line_Edit);
			}
			{
				QCheckBox* norm_Checkbox = new QCheckBox("Divide by N");
				norm_Checkbox->setChecked(target_Curve->fit_Params.norm);
				connect(norm_Checkbox,  &QCheckBox::toggled, this, [=]{ target_Curve->fit_Params.norm = norm_Checkbox->isChecked(); });

				QCheckBox* maximize_Integral_Checkbox = new QCheckBox("Maximize integral");
					maximize_Integral_Checkbox->setChecked(target_Curve->fit_Params.maximize_Integral);
					max_Integral_Map.insert(target_Curve, maximize_Integral_Checkbox);

				QHBoxLayout* checkboxes_Layout = new QHBoxLayout;
					checkboxes_Layout->addWidget(norm_Checkbox);
					checkboxes_Layout->addWidget(maximize_Integral_Checkbox);
					box_Layout->addLayout(checkboxes_Layout);

				// -------------------------------------------------------
				QLabel* function_Label = new QLabel("Function:");
				QLineEdit* fit_Function_Line_Edit = new QLineEdit(target_Curve->fit_Params.fit_Function);

				QHBoxLayout* function_Layout = new QHBoxLayout;
					function_Layout->addWidget(function_Label);
					function_Layout->addWidget(fit_Function_Line_Edit);

				box_Layout->addLayout(function_Layout);

				connect(fit_Function_Line_Edit,  &QLineEdit::textEdited, this, [=]
				{
					QStringList var_List = {fit_Function_Variable};
					if(Global_Variables::expression_Is_Valid(fit_Function_Line_Edit->text(), var_List) || fit_Function_Line_Edit->text().isEmpty())
					{
						target_Curve->fit_Params.fit_Function = fit_Function_Line_Edit->text();
					}
				});
				connect(fit_Function_Line_Edit,  &QLineEdit::editingFinished, this, [=]
				{
					QStringList var_List = {fit_Function_Variable};
					if(Global_Variables::expression_Is_Valid(fit_Function_Line_Edit->text(), var_List) || fit_Function_Line_Edit->text().isEmpty())
					{
						target_Curve->fit_Params.fit_Function = fit_Function_Line_Edit->text();
					} else
					{
						fit_Function_Line_Edit->setText(target_Curve->fit_Params.fit_Function);
						QMessageBox::information(this, "Wrong expression", "Expression has wrong syntax");
					}
				});
				// -------------------------------------------------------

				QLabel* power_Label = new QLabel("Power:");
				function_Layout->addWidget(power_Label);

				MyDoubleSpinBox* power_SpinBox = new MyDoubleSpinBox;
					power_SpinBox->setRange(0, 42);
					power_SpinBox->setDecimals(1);
					power_SpinBox->setSingleStep(1);
					power_SpinBox->setValue(target_Curve->fit_Params.power);
					power_SpinBox->setAccelerated(true);
					power_SpinBox->setFixedWidth(25);
					power_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				function_Layout->addWidget(power_SpinBox);
				connect(power_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
				{
					target_Curve->fit_Params.power = power_SpinBox->value();
				});
				// -------------------------------------------------------

				QCheckBox* chi2_CheckBox = new QCheckBox("Use "+Chi2_Sym);
					chi2_CheckBox->setChecked(target_Curve->fit_Params.use_Chi2);
				function_Layout->addWidget(chi2_CheckBox);
				connect(chi2_CheckBox,  &QCheckBox::toggled, this, [=]
				{
					power_Label           ->setEnabled(!chi2_CheckBox->isChecked() && !maximize_Integral_Checkbox->isChecked());
					power_SpinBox         ->setEnabled(!chi2_CheckBox->isChecked() && !maximize_Integral_Checkbox->isChecked());
					norm_Checkbox         ->setEnabled(!chi2_CheckBox->isChecked() && !maximize_Integral_Checkbox->isChecked());

					function_Label        ->setDisabled(chi2_CheckBox->isChecked() && !target_Curve->fit_Params.maximize_Integral);
					fit_Function_Line_Edit->setDisabled(chi2_CheckBox->isChecked() && !target_Curve->fit_Params.maximize_Integral);

					target_Curve->fit_Params.use_Chi2 = chi2_CheckBox->isChecked();
				});

				connect(maximize_Integral_Checkbox,  &QCheckBox::toggled, this, [=]
				{
					if(target_Curve->curve.argument_Type == whats_This_Wavelength)
					{
						target_Curve->fit_Params.maximize_Integral = maximize_Integral_Checkbox->isChecked();
					} else
					{
						maximize_Integral_Checkbox->blockSignals(true);
						maximize_Integral_Checkbox->setChecked(false);
						target_Curve->fit_Params.maximize_Integral = false;
						maximize_Integral_Checkbox->blockSignals(false);
						QMessageBox::information(this,"Wrong curve type", "Only spectral curve\nshoud be maximized");
					}
					chi2_CheckBox->setEnabled(!maximize_Integral_Checkbox->isChecked());

					power_Label  ->setEnabled(!maximize_Integral_Checkbox->isChecked() && !chi2_CheckBox->isChecked());
					power_SpinBox->setEnabled(!maximize_Integral_Checkbox->isChecked() && !chi2_CheckBox->isChecked());
					norm_Checkbox->setEnabled(!maximize_Integral_Checkbox->isChecked() && !chi2_CheckBox->isChecked());

					function_Label        ->setEnabled(maximize_Integral_Checkbox->isChecked() || !target_Curve->fit_Params.use_Chi2);
					fit_Function_Line_Edit->setEnabled(maximize_Integral_Checkbox->isChecked() || !target_Curve->fit_Params.use_Chi2);
				});

				// initialize
				chi2_CheckBox->toggled(chi2_CheckBox->isChecked());
				if(target_Curve->fit_Params.maximize_Integral)
				{maximize_Integral_Checkbox->toggled(maximize_Integral_Checkbox->isChecked());}
			}
		}
		target_Index++;
	}
	if(!horizontal_Layout->parent()) delete horizontal_Layout;
}

void Calculation_Settings_Editor::load_Independent_Parameters(int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	independent_Group_Box_Vec[tab_Index]->setChecked(multilayer->enable_Calc_Independent_Curves);
	connect(independent_Group_Box_Vec[tab_Index],  &QGroupBox::toggled, this, [=]{
		multilayer->enable_Calc_Independent_Curves = independent_Group_Box_Vec[tab_Index]->isChecked();
		global_Multilayer_Approach->independent_Target_Added = independent_Group_Box_Vec[tab_Index]->isChecked();
		reopen_Optical_Graphs(INDEPENDENT);
		activateWindow();
	});

	QVBoxLayout* independent_Vertical_Box_Layout = new QVBoxLayout(independent_Group_Box_Vec[tab_Index]);
		independent_Vertical_Box_Layout->setSpacing(20);
		independent_Vertical_Box_Layout->setContentsMargins(7,14,7,7);

	// total number of independents
	total_Number_of_Independents = multilayer->independent_Variables_Plot_Tabs->count();

	int independents_in_Short_Row = total_Number_of_Independents/multilayer->num_Independent_Rows;
	int additional_Independents = total_Number_of_Independents%multilayer->num_Independent_Rows;
	int first_Long_Row_Index = multilayer->num_Independent_Rows-additional_Independents;

	int current_Row = 0;
	int independents_in_Filled_Rows = 0;
	int length = -2018;

	QHBoxLayout* horizontal_Layout = new QHBoxLayout;

	for(int independent_Index=0; independent_Index<multilayer->independent_Variables_Plot_Tabs->count(); ++independent_Index)
	{
		Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(independent_Index));

		QGroupBox* box = new QGroupBox(multilayer->independent_Variables_Plot_Tabs->tabText(independent_Index));
			box->setCheckable(true);
			box->setObjectName("box");
			box->setStyleSheet("QGroupBox#box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
						  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");

		horizontal_Layout->addWidget(box);
			horizontal_Layout->setSpacing(20);

		// switching is here
		if(current_Row < first_Long_Row_Index) length = independents_in_Short_Row; else length = independents_in_Short_Row + 1;

		if(independent_Index == independents_in_Filled_Rows+length-1)
		{
			independents_in_Filled_Rows += length;
			current_Row++;
			independent_Vertical_Box_Layout->addLayout(horizontal_Layout);
			horizontal_Layout = new QHBoxLayout; // may be memory leak at the end
		}

		box->setChecked(independent_Variables->calc_Functions.check_Enabled);
		connect(box,  &QGroupBox::toggled, this, [=]{
			refresh_Independent_Calc_Properties(tab_Index, independent_Index, box);
			global_Multilayer_Approach->independent_Added = qobject_cast<QGroupBox*>(sender())->isChecked();
			reopen_Optical_Graphs(INDEPENDENT);
			activateWindow();
		});

		QVBoxLayout* box_Layout = new QVBoxLayout(box);
			box_Layout->setSpacing(5);
			box_Layout->setContentsMargins(5,10,5,5);
		// content
		{
			{
				QGroupBox* standard_Functions_Group_Box = new QGroupBox("Standard Functions");
					standard_Functions_Group_Box->setObjectName("standard_Functions_Group_Box");
					standard_Functions_Group_Box->setStyleSheet("QGroupBox#standard_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 1ex;}"
																"QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
				box_Layout->addWidget(standard_Functions_Group_Box);

				QHBoxLayout* standard_Functions_Layout = new QHBoxLayout(standard_Functions_Group_Box);
					standard_Functions_Layout->setAlignment(Qt::AlignLeft);
					QCheckBox* reflect_Functions = new QCheckBox(reflectance_Function);
						standard_Functions_Layout->addWidget(reflect_Functions);
						reflect_Functions->setChecked(independent_Variables->calc_Functions.check_Reflectance);
					QCheckBox* transmit_Functions = new QCheckBox(transmittance_Function);
						standard_Functions_Layout->addWidget(transmit_Functions);
						transmit_Functions->setChecked(independent_Variables->calc_Functions.check_Transmittance);
					QCheckBox* absorp_Functions = new QCheckBox(absorptance_Function);
						standard_Functions_Layout->addWidget(absorp_Functions);
						absorp_Functions->setChecked(independent_Variables->calc_Functions.check_Absorptance);

				connect(reflect_Functions,  &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
				connect(transmit_Functions, &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
				connect(absorp_Functions,   &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
			}
			{
				QGroupBox* field_Functions_Group_Box = new QGroupBox("Field Functions");
					field_Functions_Group_Box->setObjectName("field_Functions_Group_Box");
					field_Functions_Group_Box->setStyleSheet("QGroupBox#field_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 1ex;}"
															 "QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
				box_Layout->addWidget(field_Functions_Group_Box);

				QHBoxLayout* field_Functions_Layout = new QHBoxLayout(field_Functions_Group_Box);
					field_Functions_Layout->setAlignment(Qt::AlignLeft);
					QCheckBox* field_Intensity = new QCheckBox(intensity_Function);
						field_Functions_Layout->addWidget(field_Intensity);
						field_Intensity->setChecked(independent_Variables->calc_Functions.check_Field);
/* TODO */				field_Intensity->setDisabled(true);
					QCheckBox* joule_Absorption= new QCheckBox(joule_Function);
						field_Functions_Layout->addWidget(joule_Absorption);
						joule_Absorption->setChecked(independent_Variables->calc_Functions.check_Joule);
/* TODO */				joule_Absorption->setDisabled(true);

				connect(field_Intensity,  &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
				connect(joule_Absorption, &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
			}
			{
				QGroupBox* user_Functions_Group_Box = new QGroupBox("User-defined Functions");
					user_Functions_Group_Box->setObjectName("user_Functions_Group_Box");
					user_Functions_Group_Box->setStyleSheet("QGroupBox#user_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 1ex;}"
															"QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
				box_Layout->addWidget(user_Functions_Group_Box);

				QHBoxLayout* user_Functions_Layout = new QHBoxLayout(user_Functions_Group_Box);
					user_Functions_Layout->setAlignment(Qt::AlignLeft);
					QCheckBox* user_Supplied_Functions_Check = new QCheckBox(user_Function);
						user_Functions_Layout->addWidget(user_Supplied_Functions_Check);
						user_Supplied_Functions_Check->setChecked(independent_Variables->calc_Functions.check_User);
/* TODO */				user_Supplied_Functions_Check->setDisabled(true);
					QLineEdit* user_Supplied_Functions = new QLineEdit;
						user_Functions_Layout->addWidget(user_Supplied_Functions);
						user_Supplied_Functions->setText(independent_Variables->calc_Functions.user_Functions);
						user_Supplied_Functions->setEnabled(independent_Variables->calc_Functions.check_User);


				connect(user_Supplied_Functions_Check,  &QCheckBox::toggled,		 this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); user_Supplied_Functions->setEnabled(independent_Variables->calc_Functions.check_User);});
				connect(user_Supplied_Functions,		&QLineEdit::editingFinished, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
				different_Lines.append(user_Supplied_Functions);
			}
		}
	}
	if(!horizontal_Layout->parent()) delete horizontal_Layout;
}

void Calculation_Settings_Editor::refresh_Independent_Calc_Properties(int tab_Index, int independent_Index, QGroupBox* box)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
	Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(independent_Index));

	independent_Variables->calc_Functions.check_Enabled = box->isChecked();

	if(qobject_cast<QCheckBox*>(sender()))
	{
		QCheckBox* check_Box = qobject_cast<QCheckBox*>(sender());
		if(check_Box->text() == reflectance_Function)	independent_Variables->calc_Functions.check_Reflectance = check_Box->isChecked();
		if(check_Box->text() == transmittance_Function) independent_Variables->calc_Functions.check_Transmittance = check_Box->isChecked();
		if(check_Box->text() == absorptance_Function)	independent_Variables->calc_Functions.check_Absorptance = check_Box->isChecked();

		if(check_Box->text() == intensity_Function)		independent_Variables->calc_Functions.check_Field = check_Box->isChecked();
		if(check_Box->text() == joule_Function)			independent_Variables->calc_Functions.check_Joule = check_Box->isChecked();
		if(check_Box->text() == user_Function)			independent_Variables->calc_Functions.check_User = check_Box->isChecked();

		reopen_Optical_Graphs();
		activateWindow();
	}

	if(qobject_cast<QLineEdit*>(sender()))
	{
		QLineEdit* user_Supplied_Functions = qobject_cast<QLineEdit*>(sender());
		QStringList var_List = {symbol_R, symbol_T, symbol_A, symbol_F, symbol_J};
		if(Global_Variables::expression_Is_Valid(user_Supplied_Functions->text(), var_List) || user_Supplied_Functions->text().isEmpty())
		{
			independent_Variables->calc_Functions.user_Functions = user_Supplied_Functions->text();
		} else
		{
			user_Supplied_Functions->setText(independent_Variables->calc_Functions.user_Functions);
			QMessageBox::information(this, "Wrong expression", "Expression has wrong syntax");
		}
	}

	multilayer->independent_Variables_Plot_Tabs->setTabText(independent_Index, independent_Variables->tab_Name + independent_Variables->enlarge_Tab_Name());
	box->setTitle(multilayer->independent_Variables_Plot_Tabs->tabText(independent_Index));
}

void Calculation_Settings_Editor::reopen_Optical_Graphs(QString keep_Splitter)
{
	// refresh graphs (anyway)
	if(global_Multilayer_Approach->runned_Optical_Graphs.contains(optical_Graphs_Key))
	{
		int active_Tab = global_Multilayer_Approach->optical_Graphs->main_Tabs->currentIndex();
		global_Multilayer_Approach->optical_Graphs->close();
		global_Multilayer_Approach->open_Optical_Graphs(keep_Splitter);
		global_Multilayer_Approach->optical_Graphs->main_Tabs->setCurrentIndex(active_Tab);
	}
}

