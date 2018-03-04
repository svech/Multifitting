#include "independent_calc_function_selection.h"

Independent_Calc_Function_Selection::Independent_Calc_Function_Selection(Independent_Variables* independent_Variables, QWidget *parent) :
	independent_Variables(independent_Variables),
	QDialog(parent)
{
	create_Main_Layout();

	setWindowTitle("Independent Variable");
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Independent_Calc_Function_Selection::closeEvent(QCloseEvent *event)
{
	QStringList var_List = {symbol_R, symbol_T, symbol_A, symbol_F, symbol_J};
	if(Global_Variables::expression_Is_Valid(user_Supplied_Functions->text(), var_List) || user_Supplied_Functions->text().isEmpty())
	{
		independent_Variables->calc_Functions.user_Functions = user_Supplied_Functions->text();
		event->accept();
	} else
	{
		event->ignore();
		user_Supplied_Functions->setText(independent_Variables->calc_Functions.user_Functions);
		done_Button->blockSignals(true);
		QMessageBox::information(this, "Wrong expression", "Expression has wrong syntax");
	}
}

void Independent_Calc_Function_Selection::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	{
		global_Group_Box = new QGroupBox("Enable", this);
			global_Group_Box->setCheckable(true);
			global_Group_Box->setObjectName("global_Group_Box");
			global_Group_Box->setStyleSheet("QGroupBox#global_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
														"QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
			global_Group_Box->setChecked(independent_Variables->calc_Functions.check_Enabled);
		main_Layout->addWidget(global_Group_Box);

		global_Layout = new QVBoxLayout;
		global_Layout->setSpacing(0);
		global_Layout->setContentsMargins(4,4,4,4);
		global_Group_Box->setLayout(global_Layout);

		connect(global_Group_Box,  &QGroupBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
	}
	{
		standard_Functions_Group_Box = new QGroupBox("Standard Functions", this);
			standard_Functions_Group_Box->setContentsMargins(0,8,0,-4);
			standard_Functions_Group_Box->setObjectName("standard_Functions_Group_Box");
			standard_Functions_Group_Box->setStyleSheet("QGroupBox#standard_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
														"QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
		global_Layout->addWidget(standard_Functions_Group_Box);

		QHBoxLayout* standard_Functions_Layout = new QHBoxLayout(standard_Functions_Group_Box);
			standard_Functions_Layout->setAlignment(Qt::AlignLeft);
			reflect_Functions = new QCheckBox(reflectance_Function, this);
				standard_Functions_Layout->addWidget(reflect_Functions);
				reflect_Functions->setChecked(independent_Variables->calc_Functions.check_Reflectance);
			transmit_Functions = new QCheckBox(transmittance_Function, this);
				standard_Functions_Layout->addWidget(transmit_Functions);
				transmit_Functions->setChecked(independent_Variables->calc_Functions.check_Transmittance);
			absorp_Functions = new QCheckBox(absorptance_Function, this);
				standard_Functions_Layout->addWidget(absorp_Functions);
				absorp_Functions->setChecked(independent_Variables->calc_Functions.check_Absorptance);

		connect(reflect_Functions,  &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
		connect(transmit_Functions, &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
		connect(absorp_Functions,   &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
	}
	{
		field_Functions_Group_Box = new QGroupBox("Field Functions", this);
			field_Functions_Group_Box->setContentsMargins(0,8,0,-4);
			field_Functions_Group_Box->setObjectName("field_Functions_Group_Box");
			field_Functions_Group_Box->setStyleSheet("QGroupBox#field_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
													 "QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
		global_Layout->addWidget(field_Functions_Group_Box);

		QHBoxLayout* field_Functions_Layout = new QHBoxLayout(field_Functions_Group_Box);
			field_Functions_Layout->setAlignment(Qt::AlignLeft);
			field_Intensity = new QCheckBox(intensity_Function, this);
				field_Functions_Layout->addWidget(field_Intensity);
				field_Intensity->setChecked(independent_Variables->calc_Functions.check_Field);
			joule_Absorption= new QCheckBox(joule_Function, this);
				field_Functions_Layout->addWidget(joule_Absorption);
				joule_Absorption->setChecked(independent_Variables->calc_Functions.check_Joule);

		connect(field_Intensity,  &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
		connect(joule_Absorption, &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
	}
	{
		user_Functions_Group_Box = new QGroupBox(user_Function);
			user_Functions_Group_Box->setContentsMargins(0,8,0,-4);
			user_Functions_Group_Box->setObjectName("user_Functions_Group_Box");
			user_Functions_Group_Box->setStyleSheet("QGroupBox#user_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
													"QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
		global_Layout->addWidget(user_Functions_Group_Box);

		QHBoxLayout* user_Functions_Layout = new QHBoxLayout(user_Functions_Group_Box);
			user_Functions_Layout->setAlignment(Qt::AlignLeft);
			user_Supplied_Functions_Check = new QCheckBox(this);
				user_Functions_Layout->addWidget(user_Supplied_Functions_Check);
				user_Supplied_Functions_Check->setChecked(independent_Variables->calc_Functions.check_User);
			user_Supplied_Functions = new QLineEdit(this);
				user_Functions_Layout->addWidget(user_Supplied_Functions);
				user_Supplied_Functions->setText(independent_Variables->calc_Functions.user_Functions);
				user_Supplied_Functions->setEnabled(independent_Variables->calc_Functions.check_User);

		connect(user_Supplied_Functions_Check,  &QCheckBox::toggled,		 this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
		connect(user_Supplied_Functions,		&QLineEdit::returnPressed,   this, &Independent_Calc_Function_Selection::close);
	}

	done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		done_Button->setFocus();
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button, &QPushButton::clicked, this, &Independent_Calc_Function_Selection::close);
}

void Independent_Calc_Function_Selection::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
}

void Independent_Calc_Function_Selection::refresh_calc_Functions(bool)
{
	independent_Variables->calc_Functions.check_Enabled = global_Group_Box->isChecked();

	independent_Variables->calc_Functions.check_Reflectance = reflect_Functions->isChecked();
	independent_Variables->calc_Functions.check_Transmittance = transmit_Functions->isChecked();
	independent_Variables->calc_Functions.check_Absorptance = absorp_Functions->isChecked();

	independent_Variables->calc_Functions.check_Field = field_Intensity->isChecked();
	independent_Variables->calc_Functions.check_Joule = joule_Absorption->isChecked();
	independent_Variables->calc_Functions.check_User = user_Supplied_Functions_Check->isChecked();

	user_Supplied_Functions->setEnabled(independent_Variables->calc_Functions.check_User);
	done_Button->blockSignals(false);

	// rename tab
	QTabWidget* independent_Variables_Plot_Tabs = qobject_cast<QTabWidget*>(independent_Variables->parent()->parent());
	for(int tab_Index=0; tab_Index<independent_Variables_Plot_Tabs->count(); ++tab_Index)
	{
		Independent_Variables* independent = qobject_cast<Independent_Variables*>(independent_Variables_Plot_Tabs->widget(tab_Index));
		if(independent == independent_Variables)
			independent_Variables_Plot_Tabs->setTabText(tab_Index, independent->tab_Name + independent->enlarge_Tab_Name());
	}
}
