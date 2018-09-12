// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "grading_editor.h"

Grading_Editor::Grading_Editor(QTreeWidgetItem* item, QString drift_Name, QWidget *parent):
    item(item),
	default_Min_Line_Size(100),
	drift_Name(drift_Name),
	layer(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>()),
    QDialog(parent)
{
    create_Main_Layout();

	setWindowTitle(drift_Name + " Grading");
    set_Window_Geometry();
    setAttribute(Qt::WA_DeleteOnClose);
}

void Grading_Editor::emit_Grading_Edited()
{
	// directly to Structure_Tree, over Item_Editor
	emit grading_Edited();
}

void Grading_Editor::refresh_Data()
{
    drift.is_Drift_Line = line_Group_Box->isChecked();
    drift.is_Drift_Sine = sine_Group_Box->isChecked();
    drift.is_Drift_Rand = rand_Group_Box->isChecked();

	drift.show_Drift_Line = show_Line_CheckBox->isChecked();
    drift.show_Drift_Rand = show_Rand_CheckBox->isChecked();
    drift.show_Drift_Sine = show_Sine_CheckBox->isChecked();

	drift.drift_Line_Value.value = line_Value_Line->text().toDouble();
	drift.drift_Sine_Amplitude.value = sine_Amplitude_Line->text().toDouble();
	drift.drift_Sine_Frequency.value = sine_Frequency_Line->text().toDouble();
	drift.drift_Sine_Phase.value = sine_Phase_Line->text().toDouble();
	drift.drift_Rand_Rms.value = rand_Rms_Line->text().toDouble();

    if(drift_Name==whats_This_Thickness) { layer.thickness_Drift = drift;}
    if(drift_Name==whats_This_Sigma)     { layer.sigma_Drift = drift;    }

    QVariant var;
    var.setValue(layer);
    item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	emit_Grading_Edited();
}

void Grading_Editor::create_Main_Layout()
{
    main_Layout = new QVBoxLayout(this);
		main_Layout->setContentsMargins(4,6,4,0);
		main_Layout->setSpacing(8);

    create_Interface();
		main_Layout->addWidget(table_Group_Box);
		main_Layout->addWidget(line_Group_Box);
		main_Layout->addWidget(sine_Group_Box);
		main_Layout->addWidget(rand_Group_Box);

    done_Button = new QPushButton("Done", this);
        done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        done_Button->setFocus();
        done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button, &QPushButton::clicked, this, &Grading_Editor::close);
}

void Grading_Editor::create_Interface()
{
	// table show
	{
		table_Group_Box = new QGroupBox("Show in Structure Table", this);
			table_Group_Box->setObjectName("table_Group_Box");
			table_Group_Box->setStyleSheet("QGroupBox#table_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;} "
										   "QGroupBox::title { subcontrol-origin: margin;	 left: 4px; padding: 0 0px 0 1px;}");
			table_Group_Box->setCheckable(false);

		table_GB_Layout = new QHBoxLayout(table_Group_Box);
			table_GB_Layout->setContentsMargins(10,15,10,10);
			table_GB_Layout->setAlignment(Qt::AlignLeft);
	}
	// linear
	{
		line_Group_Box = new QGroupBox("Linear Drift", this);
			line_Group_Box->setObjectName("line_Group_Box");
			line_Group_Box->setStyleSheet("QGroupBox#line_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;} "
										  "QGroupBox::title { subcontrol-origin: margin;	 left: 4px; padding: 0 0px 0 1px;}");
			line_Group_Box->setCheckable(true);

		line_GB_Layout = new QGridLayout(line_Group_Box);
			line_GB_Layout->setContentsMargins(10,15,10,10);
			line_GB_Layout->setAlignment(Qt::AlignLeft);
	}
	// sine
	{
		sine_Group_Box = new QGroupBox("Sine Drift", this);
			sine_Group_Box->setObjectName("sine_Group_Box");
			sine_Group_Box->setStyleSheet("QGroupBox#sine_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;} "
										  "QGroupBox::title { subcontrol-origin: margin;	 left: 4px; padding: 0 0px 0 1px;}");
			sine_Group_Box->setCheckable(true);

		sine_GB_Layout = new QGridLayout(sine_Group_Box);
			sine_GB_Layout->setContentsMargins(10,15,10,10);
			sine_GB_Layout->setAlignment(Qt::AlignLeft);
	}
	// random
	{
		rand_Group_Box = new QGroupBox("Random Drift", this);
			rand_Group_Box->setObjectName("rand_Group_Box");
			rand_Group_Box->setStyleSheet("QGroupBox#rand_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;} "
										  "QGroupBox::title { subcontrol-origin: margin;	 left: 4px; padding: 0 0px 0 1px;}");
			rand_Group_Box->setCheckable(true);

		rand_GB_Layout = new QGridLayout(rand_Group_Box);
			rand_GB_Layout->setContentsMargins(10,15,10,10);
			rand_GB_Layout->setAlignment(Qt::AlignLeft);
	}

	read_Drift_From_Item();

	connect(show_Line_CheckBox, &QCheckBox::toggled, this, [=]{refresh_Data();});
	connect(show_Rand_CheckBox, &QCheckBox::toggled, this, [=]{refresh_Data();});
	connect(show_Sine_CheckBox, &QCheckBox::toggled, this, [=]{refresh_Data();});

	connect(line_Group_Box,     &QGroupBox::toggled, this, [=]{refresh_Data();});
	connect(sine_Group_Box,     &QGroupBox::toggled, this, [=]{refresh_Data();});
	connect(rand_Group_Box,     &QGroupBox::toggled, this, [=]{refresh_Data();});

	connect(line_Value_Line,    &QLineEdit::textEdited, this, [=]{refresh_Data();});
	connect(sine_Amplitude_Line,&QLineEdit::textEdited, this, [=]{refresh_Data();});
	connect(sine_Frequency_Line,&QLineEdit::textEdited, this, [=]{refresh_Data();});
	connect(sine_Phase_Line,    &QLineEdit::textEdited, this, [=]{refresh_Data();});
	connect(rand_Rms_Line,      &QLineEdit::textEdited, this, [=]{refresh_Data();});
}

void Grading_Editor::read_Drift_From_Item()
{
    if(drift_Name==whats_This_Thickness) { drift = layer.thickness_Drift;}
	if(drift_Name==whats_This_Sigma)     { drift = layer.sigma_Drift;    }

	// table show
	{
		show_Line_CheckBox = new QCheckBox("Linear drift");
			show_Line_CheckBox->setChecked(drift.show_Drift_Line);
		table_GB_Layout->addWidget(show_Line_CheckBox);

		show_Rand_CheckBox = new QCheckBox("Random drift");
			show_Rand_CheckBox->setChecked(drift.show_Drift_Rand);
		table_GB_Layout->addWidget(show_Rand_CheckBox);

		show_Sine_CheckBox = new QCheckBox("Sine drift");
			show_Sine_CheckBox->setChecked(drift.show_Drift_Sine);
		table_GB_Layout->addWidget(show_Sine_CheckBox);
	}

	// linear
	{
		line_Group_Box->setChecked(drift.is_Drift_Line);

		line_Value_Label = new QLabel("Drift per period, %",this);
		line_Value_Line = new QLineEdit(this);
			line_Value_Line->setFixedWidth(default_Min_Line_Size);
			line_Value_Line->setProperty(min_Size_Property, default_Min_Line_Size);
			line_Value_Line->setText(QString::number(drift.drift_Line_Value.value));

		line_GB_Layout->addWidget(line_Value_Label, 0,0);
		line_GB_Layout->addWidget(line_Value_Line,  1,0);
	}
	// sine
	{
		sine_Group_Box->setChecked(drift.is_Drift_Sine);

		sine_Amplitude_Label = new QLabel("Amplitude, %",this);
		sine_Amplitude_Line = new QLineEdit(this);
			sine_Amplitude_Line->setFixedWidth(default_Min_Line_Size);
			sine_Amplitude_Line->setProperty(min_Size_Property, default_Min_Line_Size);
			sine_Amplitude_Line->setText(QString::number(drift.drift_Sine_Amplitude.value));

		sine_Frequency_Label = new QLabel("Frequency, period" + Minus_One_Sym,this);
		sine_Frequency_Line = new QLineEdit(this);
			sine_Frequency_Line->setFixedWidth(default_Min_Line_Size);
			sine_Frequency_Line->setProperty(min_Size_Property, default_Min_Line_Size);
			sine_Frequency_Line->setText(QString::number(drift.drift_Sine_Frequency.value));

		sine_Phase_Label = new QLabel("Phase, part of 1",this);
		sine_Phase_Line = new QLineEdit(this);
			sine_Phase_Line->setFixedWidth(default_Min_Line_Size);
			sine_Phase_Line->setProperty(min_Size_Property, default_Min_Line_Size);
			sine_Phase_Line->setText(QString::number(drift.drift_Sine_Phase.value));

		sine_GB_Layout->addWidget(sine_Amplitude_Label, 0,0);
		sine_GB_Layout->addWidget(sine_Amplitude_Line,  1,0);

		sine_GB_Layout->addWidget(sine_Frequency_Label, 0,1);
		sine_GB_Layout->addWidget(sine_Frequency_Line,  1,1);

		sine_GB_Layout->addWidget(sine_Phase_Label, 0,2);
		sine_GB_Layout->addWidget(sine_Phase_Line,  1,2);
	}
	// random
	{
		rand_Group_Box->setChecked(drift.is_Drift_Rand);

		rand_Rms_Label = new QLabel("RMS, %",this);
		rand_Rms_Line = new QLineEdit(this);
			rand_Rms_Line->setFixedWidth(default_Min_Line_Size);
			rand_Rms_Line->setProperty(min_Size_Property, default_Min_Line_Size);
			rand_Rms_Line->setText(QString::number(drift.drift_Rand_Rms.value));

		rand_GB_Layout->addWidget(rand_Rms_Label, 0,0);
		rand_GB_Layout->addWidget(rand_Rms_Line,  1,0);
	}
}

void Grading_Editor::set_Window_Geometry()
{
    adjustSize();
    setFixedSize(size());
}
