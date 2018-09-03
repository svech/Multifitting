#include "confidence_interval_editor.h"

Confidence_Interval_Editor::Confidence_Interval_Editor(Coupling_Editor* coupling_Editor, QWidget *parent) :
	coupling_Editor(coupling_Editor),
	QWidget(parent)
{
	create_Main_Layout();

	setWindowTitle("Confidence Interval");
	set_Window_Geometry();

	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose);
}

void Confidence_Interval_Editor::closeEvent(QCloseEvent*)
{
	coupling_Editor->close();
}

void Confidence_Interval_Editor::set_Window_Geometry()
{
	move(coupling_Editor->pos());
	adjustSize();
	setFixedSize(size());
}

void Confidence_Interval_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(4,4,4,0);

	create_Group_Box();

	{
		done_Button = new QPushButton("Done");
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
			done_Button->setDefault(true);
		main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

		connect(done_Button, &QPushButton::clicked, this, &Independent_Variables_Editor::close);
	}
}

void Confidence_Interval_Editor::create_Group_Box()
{
	main_GroupBox = new QGroupBox("Calculate Confidence Interval");
		main_GroupBox->setCheckable(true);
		main_GroupBox->setObjectName("global_Group_Box");
		main_GroupBox->setStyleSheet("QGroupBox#global_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
											   "QGroupBox::title { subcontrol-origin: margin;	left: 9px; padding: 0 0px 0 1px;}");
		main_GroupBox->setChecked(coupling_Editor->coupled_Parameter->confidence.calc_Conf_Interval);

	connect(main_GroupBox, &QGroupBox::toggled, this, [=]{coupling_Editor->coupled_Parameter->confidence.calc_Conf_Interval = main_GroupBox->isChecked();});
	main_Layout->addWidget(main_GroupBox);

	// create content
	groupBox_Layout = new QVBoxLayout(main_GroupBox);
		groupBox_Layout->setAlignment(Qt::AlignLeft);
		groupBox_Layout->setContentsMargins(5,5,5,5);

	QHBoxLayout* layout = new QHBoxLayout;
	groupBox_Layout->addLayout(layout);
	QSizePolicy sp_retain;
	sp_retain.setRetainSizeWhenHidden(true);
	{
		num_Points = new QLineEdit;
			num_Points->setFixedWidth(30);
			num_Points->setProperty(min_Size_Property, 30);
			num_Points->setValidator(new QIntValidator(1, MAX_INTEGER, this));
		layout->addWidget(num_Points);
	}
	{
		from_Label = new QLabel(from_Text);
		layout->addWidget(from_Label);

		min_Edit = new QLineEdit;
			min_Edit->setFixedWidth(40);
			min_Edit->setProperty(min_Size_Property, 40);
			min_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		layout->addWidget(min_Edit);
	}
	{
		to_Label = new QLabel(to_Text);
		layout->addWidget(to_Label);

		max_Edit = new QLineEdit;
			max_Edit->setFixedWidth(40);
			max_Edit->setProperty(min_Size_Property, 40);
			max_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			max_Edit->setSizePolicy(sp_retain);
		layout->addWidget(max_Edit);
	}

	units_Label = new QLabel(units);
		units_Label->setSizePolicy(sp_retain);
	layout->addWidget(units_Label);

	step_Label = new QLabel(step_Text);
		step_Label->setSizePolicy(sp_retain);
	layout->addWidget(step_Label);

	step_Edit = new QLineEdit;
		step_Edit->setFixedWidth(40);
		step_Edit->setProperty(min_Size_Property, 40);
		step_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		step_Edit->setSizePolicy(sp_retain);
		step_Edit->setReadOnly(true);
	layout->addWidget(step_Edit);

	step_Units_Label = new QLabel(units);
		step_Units_Label->setSizePolicy(sp_retain);
	layout->addWidget(step_Units_Label);

//	num_Points->setText(QString::number(coupling_Editor->coupled_Parameter->confidence.num_Points));
//	min_Edit->setText  (QString::number(coupling_Editor->coupled_Parameter->confidence.min, ));
//	max_Edit->setText  (QString::number(coupling_Editor->coupled_Parameter->confidence.max));

	connect(num_Points, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(num_Points);});
	connect(min_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(min_Edit);});
	connect(max_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(max_Edit);});

	connect(num_Points, &QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
	connect(min_Edit,	&QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
	connect(max_Edit,	&QLineEdit::textEdited, this, [=]{refresh_Show_Data();});

	connect(step_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(step_Edit);});
}

void Confidence_Interval_Editor::refresh_Show_Data()
{

}


