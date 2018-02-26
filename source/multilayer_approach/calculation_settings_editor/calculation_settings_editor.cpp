#include "calculation_settings_editor.h"

Calculation_Settings_Editor::Calculation_Settings_Editor(QWidget* parent) :
	multilayer_Tabs(global_Multilayer_Approach->multilayer_Tabs),
	QWidget(parent)
{
	setWindowTitle("Calculation Settings");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Calculation_Settings_Editor::closeEvent(QCloseEvent* event)
{
	global_Multilayer_Approach->runned_Calculation_Settings_Editor.remove(calc_Settings_Key);
	unlock_Mainwindow_Interface();
	event->accept();
}

void Calculation_Settings_Editor::set_Window_Geometry()
{
	setGeometry(0,525,width(),height());
}

void Calculation_Settings_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	lock_Mainwindow_Interface();
	create_Menu();
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
	{
		fitting_Settings_Button = new QPushButton("Fitting Settings");
			fitting_Settings_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			fitting_Settings_Button->setFocus();
			fitting_Settings_Button->setDefault(true);
		button_Layout->addWidget(fitting_Settings_Button);
	}
	main_Layout->addLayout(button_Layout);

	// TODO
	connect(done_Button,			 &QPushButton::clicked, this, &Calculation_Settings_Editor::close);
	global_Norm_Button->setDisabled(true);
	connect(fitting_Settings_Button, &QPushButton::clicked, global_Multilayer_Approach, &Multilayer_Approach::open_Fitting_Settings);
}

void Calculation_Settings_Editor::create_Menu()
{
	Menu* menu = new Menu(window_Type_Calculation_Settings_Editor,this);
	main_Layout->setMenuBar(menu->menu_Bar);
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
	});
}

void Calculation_Settings_Editor::lock_Mainwindow_Interface()
{
	// lock part of mainwindow functionality
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));
//		multilayer->structure_Tree->structure_Toolbar->toolbar->setDisabled(true);
		multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(true);

		// independent tabs
		for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
		{
			Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));
			independent_Variables->independent_Variables_Toolbar->setDisabled(true);

			multilayer->independent_Variables_Plot_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(true);
		}
		multilayer->independent_Variables_Plot_Tabs->setMovable(false);
		multilayer->independent_Variables_Corner_Button->setDisabled(true);

		// close target editors
		for(Target_Curve_Editor* target_Curve_Editor : multilayer->runned_Target_Curve_Editors.values())
		{
			target_Curve_Editor->close();
		}
		// target buttons
		for(QFrame* frame : multilayer->data_Target_Profile_Frame_Vector)
		{
			frame->setDisabled(true);
		}

	}
	multilayer_Tabs->cornerWidget()->setDisabled(true);
	multilayer_Tabs->setMovable(false);
}

void Calculation_Settings_Editor::unlock_Mainwindow_Interface()
{
	// unlock mainwindow functionality
	if(!global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Key) && !global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
	{
		multilayer_Tabs->setMovable(true);
		multilayer_Tabs->cornerWidget()->setDisabled(false);
	}
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(i));

		if(!global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Key) && !global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
//			multilayer->structure_Tree->structure_Toolbar->toolbar->setDisabled(false);
			multilayer_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setEnabled(true);
		}

		// independent tabs
		for(int i=0; i<multilayer->independent_Variables_Plot_Tabs->count(); ++i)
		{
			Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(i));
			independent_Variables->independent_Variables_Toolbar->setDisabled(false);

			multilayer->independent_Variables_Plot_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->setDisabled(false);
		}
		multilayer->independent_Variables_Plot_Tabs->setMovable(true);
		multilayer->independent_Variables_Corner_Button->setDisabled(false);

		// target buttons
		for(QFrame* frame : multilayer->data_Target_Profile_Frame_Vector)
		{
			frame->setDisabled(false);
		}
	}
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
}

void Calculation_Settings_Editor::create_Tab_Content(QWidget* new_Widget, int tab_Index)
{
	QVBoxLayout* tab_Layout = new QVBoxLayout(new_Widget);

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
	target_Group_Box_Vec[tab_Index]->setChecked(multilayer->enable_Calc_Target_Curves);
	connect(target_Group_Box_Vec[tab_Index],  &QGroupBox::toggled, this, [=]{ multilayer->enable_Calc_Target_Curves = target_Group_Box_Vec[tab_Index]->isChecked(); });

	QHBoxLayout* box_Layout = new QHBoxLayout(target_Group_Box_Vec[tab_Index]);

	for(int target_Index=0; target_Index<multilayer->target_Profiles_Vector.size(); ++target_Index)
	{
		if(multilayer->target_Profiles_Vector[target_Index]->loaded_And_Ready)
		{
			QFrame* frame = new QFrame;
			target_Frame_Vec.append(frame);
			box_Layout->addWidget(frame);

			QVBoxLayout* frame_Layout = new QVBoxLayout(frame);

			QGroupBox* box = new QGroupBox(multilayer->target_Profiles_Vector[target_Index]->label_Text);
				box->setCheckable(true);
				box->setObjectName("box");
				box->setStyleSheet("QGroupBox#box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
							  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
			frame_Layout->addWidget(box);

			box->setChecked(multilayer->target_Profiles_Vector[target_Index]->fit_Params.calc);
			connect(box,  &QGroupBox::toggled, this, [=]{ multilayer->target_Profiles_Vector[target_Index]->fit_Params.calc = box->isChecked(); });

			QVBoxLayout* box_Layout = new QVBoxLayout(box);

			// content
			{
				{
					QCheckBox* fit = new QCheckBox("Fit");
						box_Layout->addWidget(fit);
						fit->setChecked(multilayer->target_Profiles_Vector[target_Index]->fit_Params.fit);
					connect(fit,  &QCheckBox::toggled, this, [=]{ multilayer->target_Profiles_Vector[target_Index]->fit_Params.fit = fit->isChecked(); });

					QLabel* weight_Label = new QLabel("Weight");
					QLineEdit* weight_Line_Edit = new QLineEdit(QString::number(multilayer->target_Profiles_Vector[target_Index]->fit_Params.weight));
						weight_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

					QHBoxLayout* weight_Layout = new QHBoxLayout;
						weight_Layout->addWidget(weight_Label);
						weight_Layout->addWidget(weight_Line_Edit);

					box_Layout->addLayout(weight_Layout);
					connect(weight_Line_Edit,  &QLineEdit::textEdited, this, [=]
					{
						multilayer->target_Profiles_Vector[target_Index]->fit_Params.weight      =      weight_Line_Edit->text().toDouble();
						multilayer->target_Profiles_Vector[target_Index]->fit_Params.weight_Sqrt = sqrt(weight_Line_Edit->text().toDouble());
					});
					different_Lines.append(weight_Line_Edit);
				}
				{
					QCheckBox* norm = new QCheckBox("Divide by N");
						box_Layout->addWidget(norm);
					norm->setChecked(multilayer->target_Profiles_Vector[target_Index]->fit_Params.norm);
					connect(norm,  &QCheckBox::toggled, this, [=]{ multilayer->target_Profiles_Vector[target_Index]->fit_Params.norm = norm->isChecked(); });
				}
				{
					QLabel* function_Label = new QLabel("Function");
					QLineEdit* fit_Function_Line_Edit = new QLineEdit(multilayer->target_Profiles_Vector[target_Index]->fit_Params.fit_Function);

					QHBoxLayout* function_Layout = new QHBoxLayout;
						function_Layout->addWidget(function_Label);
						function_Layout->addWidget(fit_Function_Line_Edit);

					box_Layout->addLayout(function_Layout);
					connect(fit_Function_Line_Edit,  &QLineEdit::editingFinished, this, [=]
					{
						QStringList var_List = {fit_Function_Variable};
						if(Global_Variables::expression_Is_Valid(fit_Function_Line_Edit->text(), var_List) || fit_Function_Line_Edit->text().isEmpty())
						{
							multilayer->target_Profiles_Vector[target_Index]->fit_Params.fit_Function = fit_Function_Line_Edit->text();
						} else
						{
							fit_Function_Line_Edit->setText(multilayer->target_Profiles_Vector[target_Index]->fit_Params.fit_Function);
							QMessageBox::information(this, "Wrong expression", "Expression has wrong syntax");
						}
					});
					different_Lines.append(fit_Function_Line_Edit);
				}
			}
		}
	}
}

void Calculation_Settings_Editor::load_Independent_Parameters(int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	independent_Group_Box_Vec[tab_Index]->setChecked(multilayer->enable_Calc_Independent_Curves);
	connect(independent_Group_Box_Vec[tab_Index],  &QGroupBox::toggled, this, [=]{ multilayer->enable_Calc_Independent_Curves = independent_Group_Box_Vec[tab_Index]->isChecked(); });

	QHBoxLayout* box_Layout = new QHBoxLayout(independent_Group_Box_Vec[tab_Index]);

	for(int independent_Index=0; independent_Index<multilayer->independent_Variables_Plot_Tabs->count(); ++independent_Index)
	{
		Independent_Variables* independent_Variables = qobject_cast<Independent_Variables*>(multilayer->independent_Variables_Plot_Tabs->widget(independent_Index));

		QFrame* frame = new QFrame;
		independent_Frame_Vec.append(frame);
		box_Layout->addWidget(frame);

		QVBoxLayout* frame_Layout = new QVBoxLayout(frame);

		QGroupBox* box = new QGroupBox(multilayer->independent_Variables_Plot_Tabs->tabText(independent_Index));
			box->setCheckable(true);
			box->setObjectName("box");
			box->setStyleSheet("QGroupBox#box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
						  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		frame_Layout->addWidget(box);

		box->setChecked(independent_Variables->calc_Functions.check_Enabled);
		connect(box,  &QGroupBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });

		QVBoxLayout* box_Layout = new QVBoxLayout(box);

		// content
		{
			{
				QGroupBox* standard_Functions_Group_Box = new QGroupBox("Standard Functions");
					standard_Functions_Group_Box->setContentsMargins(0,8,0,-4);
					standard_Functions_Group_Box->setObjectName("standard_Functions_Group_Box");
					standard_Functions_Group_Box->setStyleSheet("QGroupBox#standard_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
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
					field_Functions_Group_Box->setContentsMargins(0,8,0,-4);
					field_Functions_Group_Box->setObjectName("field_Functions_Group_Box");
					field_Functions_Group_Box->setStyleSheet("QGroupBox#field_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
															 "QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
				box_Layout->addWidget(field_Functions_Group_Box);

				QHBoxLayout* field_Functions_Layout = new QHBoxLayout(field_Functions_Group_Box);
					field_Functions_Layout->setAlignment(Qt::AlignLeft);
					QCheckBox* field_Intensity = new QCheckBox(intensity_Function);
						field_Functions_Layout->addWidget(field_Intensity);
						field_Intensity->setChecked(independent_Variables->calc_Functions.check_Field);
					QCheckBox* joule_Absorption= new QCheckBox(joule_Function);
						field_Functions_Layout->addWidget(joule_Absorption);
						joule_Absorption->setChecked(independent_Variables->calc_Functions.check_Joule);

				connect(field_Intensity,  &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
				connect(joule_Absorption, &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
			}
			{
				QGroupBox* user_Functions_Group_Box = new QGroupBox("User-defined Functions");
					user_Functions_Group_Box->setContentsMargins(0,8,0,-4);
					user_Functions_Group_Box->setObjectName("user_Functions_Group_Box");
					user_Functions_Group_Box->setStyleSheet("QGroupBox#user_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
															"QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
				box_Layout->addWidget(user_Functions_Group_Box);

				QHBoxLayout* user_Functions_Layout = new QHBoxLayout(user_Functions_Group_Box);
					user_Functions_Layout->setAlignment(Qt::AlignLeft);
					QCheckBox* user_Supplied_Functions_Check = new QCheckBox(user_Function);
						user_Functions_Layout->addWidget(user_Supplied_Functions_Check);
						user_Supplied_Functions_Check->setChecked(independent_Variables->calc_Functions.check_User);
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

