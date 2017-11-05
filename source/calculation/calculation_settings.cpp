#include "calculation_settings.h"

Calculation_Settings::Calculation_Settings(QMap<QString, Calculation_Settings*>* runned_Calculation_Settings, QTabWidget* multilayer_Tabs, QWidget* parent) :
	runned_Calculation_Settings(runned_Calculation_Settings),
	multilayer_Tabs(multilayer_Tabs),
	QWidget(parent)
{
	setWindowTitle("Calculation Settings");
	create_Main_Layout();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Calculation_Settings::closeEvent(QCloseEvent* event)
{
	runned_Calculation_Settings->remove(calc_Settings_Key);
	event->accept();
}

void Calculation_Settings::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

//	create_Menu();
	create_Tabs();
	main_Layout->addWidget(main_Tabs);

	adjustSize();
	add_Tabs();

	done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button, &QPushButton::clicked, this, &Calculation_Settings::close);
}

void Calculation_Settings::create_Tabs()
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
//		reload_All_Widgets();
	});
}

void Calculation_Settings::add_Tabs()
{
//	read_Trees();
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		QWidget* new_Widget = new QWidget;
//		all_Widgets_To_Reload.append(QList<QWidget*>());

		create_Tab_Content(new_Widget, tab_Index);
		main_Tabs->addTab(new_Widget, multilayer_Tabs->tabText(tab_Index));
	}
	for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
	{
		if(i!=main_Tabs->currentIndex()) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
	}

//	table_Is_Created = true;
//	reload_All_Widgets();
}

void Calculation_Settings::create_Tab_Content(QWidget* new_Widget, int tab_Index)
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

void Calculation_Settings::load_Target_Parameters(int tab_Index)
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

			box->setChecked(multilayer->target_Profiles_Vector[target_Index]->calc);
			connect(box,  &QGroupBox::toggled, this, [=]{ multilayer->target_Profiles_Vector[target_Index]->calc = box->isChecked(); });
		}
	}
}

void Calculation_Settings::load_Independent_Parameters(int tab_Index)
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
			}
		}
	}
}

void Calculation_Settings::refresh_Independent_Calc_Properties(int tab_Index, int independent_Index, QGroupBox* box)
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

