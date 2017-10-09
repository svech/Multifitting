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

void Independent_Calc_Function_Selection::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);

	{
		standard_Functions_Group_Box = new QGroupBox("Standard Functions", this);
			standard_Functions_Group_Box->setContentsMargins(0,8,0,-4);
		main_Layout->addWidget(standard_Functions_Group_Box);

		QHBoxLayout* standard_Functions_Layout = new QHBoxLayout(standard_Functions_Group_Box);
			standard_Functions_Layout->setAlignment(Qt::AlignLeft);
			reflect_Functions = new QCheckBox("Reflectance", this);
				standard_Functions_Layout->addWidget(reflect_Functions);
			transmit_Functions = new QCheckBox("Transmittance", this);
				standard_Functions_Layout->addWidget(transmit_Functions);
			absorp_Functions = new QCheckBox("Absorptance", this);
				standard_Functions_Layout->addWidget(absorp_Functions);

		connect(reflect_Functions,  &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
		connect(transmit_Functions, &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
		connect(absorp_Functions,   &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
	}
	{
		field_Functions_Group_Box = new QGroupBox("Field Functions", this);
			field_Functions_Group_Box->setContentsMargins(0,8,0,-4);
		main_Layout->addWidget(field_Functions_Group_Box);

		QHBoxLayout* field_Functions_Layout = new QHBoxLayout(field_Functions_Group_Box);
			field_Functions_Layout->setAlignment(Qt::AlignLeft);
			field_Intensity = new QCheckBox("Intensity", this);
				field_Functions_Layout->addWidget(field_Intensity);
			joule_Absorption= new QCheckBox("Joule Absorption", this);
				field_Functions_Layout->addWidget(joule_Absorption);

		connect(field_Intensity,  &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
		connect(joule_Absorption, &QCheckBox::toggled, this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
	}
	{
		user_Functions_Group_Box = new QGroupBox("User-defined Functions");
			user_Functions_Group_Box->setContentsMargins(0,8,0,-4);
		main_Layout->addWidget(user_Functions_Group_Box);

		QHBoxLayout* user_Functions_Layout = new QHBoxLayout(user_Functions_Group_Box);
			user_Functions_Layout->setAlignment(Qt::AlignLeft);
			user_Supplied_Functions_Check = new QCheckBox(this);
				user_Functions_Layout->addWidget(user_Supplied_Functions_Check);
			user_Supplied_Functions = new QLineEdit(this);
				user_Functions_Layout->addWidget(user_Supplied_Functions);

		connect(user_Supplied_Functions_Check,  &QCheckBox::toggled,		 this, &Independent_Calc_Function_Selection::refresh_calc_Functions);
		connect(user_Supplied_Functions,		&QLineEdit::editingFinished, this, [=]{refresh_calc_Functions(true);});
	}
}

void Independent_Calc_Function_Selection::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
}

void Independent_Calc_Function_Selection::refresh_calc_Functions(bool)
{
	qInfo() << "edit";
	independent_Variables->calc_Functions;
}
