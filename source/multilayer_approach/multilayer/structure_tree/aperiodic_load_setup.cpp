#include "aperiodic_load_setup.h"

Aperiodic_Load_Setup::Aperiodic_Load_Setup(Aperiodic_Settings& aperiodic_Settings, QString item_Type, QWidget* parent) :
	aperiodic_Settings(aperiodic_Settings),
	item_Type(item_Type),
	QDialog(parent)
{
	setWindowTitle("Aperiodic settings");
	setAttribute(Qt::WA_DeleteOnClose);
	create_Main_Layout();
	set_Window_Geometry();
}

void Aperiodic_Load_Setup::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
//	move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

void Aperiodic_Load_Setup::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(3,0,3,0);

	create_GroupBox();

	QHBoxLayout* button_Layout = new QHBoxLayout;
	main_Layout->addLayout(button_Layout);
	{
		load_Button = new QPushButton("Load");
			load_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			load_Button->setFocus();
			load_Button->setDefault(true);
		button_Layout->addWidget(load_Button,0,Qt::AlignCenter);

		connect(load_Button, &QPushButton::clicked, this, [=]{aperiodic_Settings.contin = true; close();});
	}
	{
		close_Button = new QPushButton("Close");
			close_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		button_Layout->addWidget(close_Button,0,Qt::AlignCenter);

		connect(close_Button, &QPushButton::clicked, this, [=]{aperiodic_Settings.contin = false; close();});
	}
}

void Aperiodic_Load_Setup::create_GroupBox()
{
	main_GroupBox = new QGroupBox;
		main_GroupBox->setObjectName("main_GroupBox");
		main_GroupBox->setStyleSheet("QGroupBox#main_GroupBox { border-radius: 2px;  border: 1px solid gray;}");
	main_Layout->addWidget(main_GroupBox);

	QHBoxLayout* main_Group_Box_Layout = new QHBoxLayout(main_GroupBox);

	{
		QCheckBox* index_CheckBox = new QCheckBox("Index");
			index_CheckBox->setChecked(true);
			index_CheckBox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
			index_CheckBox->setFocusPolicy(true ? Qt::NoFocus : Qt::StrongFocus);
			main_Group_Box_Layout->addWidget(index_CheckBox);

		QCheckBox* material_CheckBox = new QCheckBox("Material");
			material_CheckBox->setChecked(true);
			material_CheckBox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
			material_CheckBox->setFocusPolicy(true ? Qt::NoFocus : Qt::StrongFocus);
			main_Group_Box_Layout->addWidget(material_CheckBox);

		QCheckBox* thickness_CheckBox = new QCheckBox("Thickness");
			thickness_CheckBox->setChecked(true);
			thickness_CheckBox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
			thickness_CheckBox->setFocusPolicy(true ? Qt::NoFocus : Qt::StrongFocus);
			main_Group_Box_Layout->addWidget(thickness_CheckBox);

		QCheckBox* sigma_CheckBox = new QCheckBox("Sigma");
			sigma_CheckBox->setChecked(aperiodic_default_sigma_import);
			main_Group_Box_Layout->addWidget(sigma_CheckBox);

		QCheckBox* density_CheckBox = new QCheckBox("Density");
		if(item_Type == item_Type_General_Aperiodic)
		{
			density_CheckBox->setChecked(aperiodic_default_density_import);
			main_Group_Box_Layout->addWidget(density_CheckBox);
		}
		if(item_Type == item_Type_Regular_Aperiodic)
		{
			density_CheckBox->setChecked(false);
		}

		QComboBox* length_Units = new QComboBox;
			length_Units->addItems(length_Units_List);
			length_Units->setCurrentText(aperiodic_default_units_import);
			main_Group_Box_Layout->addWidget(length_Units);

		connect(sigma_CheckBox, &QCheckBox::toggled, this, [=]
		{
			if(sigma_CheckBox->isChecked() && density_CheckBox->isChecked())
			{
				aperiodic_Settings.column_4 = whats_This_Sigma;
				aperiodic_Settings.column_5 = whats_This_Density;
			} else
			if(sigma_CheckBox->isChecked() && !density_CheckBox->isChecked())
			{
				aperiodic_Settings.column_4 = whats_This_Sigma;
				aperiodic_Settings.column_5 = "";
			} else
			if(!sigma_CheckBox->isChecked() && density_CheckBox->isChecked())
			{
				aperiodic_Settings.column_4 = whats_This_Density;
				aperiodic_Settings.column_5 = "";
			} else
			if(!sigma_CheckBox->isChecked() && !density_CheckBox->isChecked())
			{
				aperiodic_Settings.column_4 = "";
				aperiodic_Settings.column_5 = "";
			}

			aperiodic_default_sigma_import   = sigma_CheckBox->isChecked();
			aperiodic_default_density_import = density_CheckBox->isChecked();
		});
		connect(density_CheckBox, &QCheckBox::toggled, this, [=]{sigma_CheckBox->toggled(sigma_CheckBox->isChecked());});
		connect(length_Units, &QComboBox::currentTextChanged, this, [=]
		{
			aperiodic_Settings.length_Units = length_Units->currentText();
			aperiodic_default_units_import = aperiodic_Settings.length_Units;
		});

		// initialize
		sigma_CheckBox->toggled(sigma_CheckBox->isChecked());
		length_Units->currentTextChanged(length_Units->currentText());
	}
}
