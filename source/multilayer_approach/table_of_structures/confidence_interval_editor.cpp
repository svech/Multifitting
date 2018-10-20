#include "confidence_interval_editor.h"

Confidence_Interval_Editor::Confidence_Interval_Editor(Coupling_Editor* coupling_Editor, QWidget *parent) :
	coupling_Editor(coupling_Editor),
	QGroupBox(parent)
{
	setTitle("Calculate Confidence Interval");
	setCheckable(true);
	setObjectName("global_Group_Box");
	setStyleSheet("QGroupBox#global_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
						   "QGroupBox::title { subcontrol-origin: margin;	left: 9px; padding: 0 0px 0 1px;}");

	setChecked(coupling_Editor->coupled_Parameter->confidence.calc_Conf_Interval);
	connect(this, &Confidence_Interval_Editor::toggled, this, [=]{coupling_Editor->coupled_Parameter->confidence.calc_Conf_Interval = isChecked();});

	create_Content();
}

void Confidence_Interval_Editor::closeEvent(QCloseEvent*)
{
	coupling_Editor->close();
}

void Confidence_Interval_Editor::create_Content()
{
	// create content
	groupBox_Layout = new QVBoxLayout(this);
		groupBox_Layout->setContentsMargins(5,10,5,5);

	QHBoxLayout* layout = new QHBoxLayout;
	groupBox_Layout->addLayout(layout);
	QSizePolicy sp_retain;
	sp_retain.setRetainSizeWhenHidden(true);
	{	
		num_Points = new QSpinBox;
			num_Points->setFixedWidth(30);
			num_Points->setProperty(min_Size_Property, 30);
			num_Points->setRange(MIN_CONFIDENCE_POINTS, MAX_INTEGER);
			num_Points->setAccelerated(true);
			num_Points->setButtonSymbols(QAbstractSpinBox::NoButtons);
		layout->addWidget(num_Points,0,Qt::AlignLeft);
	}
	{
		from_Label = new QLabel(from_Text);
		layout->addWidget(from_Label);

		min_Edit = new QLineEdit;
			min_Edit->setFixedWidth(40);
			min_Edit->setProperty(min_Size_Property, 40);
			min_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		layout->addWidget(min_Edit,0,Qt::AlignLeft);
	}
	{
		to_Label = new QLabel(to_Text);
		layout->addWidget(to_Label,0,Qt::AlignLeft);

		max_Edit = new QLineEdit;
			max_Edit->setFixedWidth(40);
			max_Edit->setProperty(min_Size_Property, 40);
			max_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			max_Edit->setSizePolicy(sp_retain);
		layout->addWidget(max_Edit,0,Qt::AlignLeft);
	}

	units_Label = new QLabel(units);
		units_Label->setSizePolicy(sp_retain);
	layout->addWidget(units_Label,0,Qt::AlignLeft);

	step_Label = new QLabel(step_Text);
		step_Label->setSizePolicy(sp_retain);
	layout->addWidget(step_Label,0,Qt::AlignLeft);

	step_Edit = new QLineEdit;
		step_Edit->setFixedWidth(40);
		step_Edit->setProperty(min_Size_Property, 40);
		step_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		step_Edit->setSizePolicy(sp_retain);
		step_Edit->setReadOnly(true);
	layout->addWidget(step_Edit,0,Qt::AlignLeft);

	step_Units_Label = new QLabel(units);
		step_Units_Label->setSizePolicy(sp_retain);
	layout->addWidget(step_Units_Label,0,Qt::AlignLeft);

	refresh_Show_Data(true);

	connect(num_Points, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{Global_Variables::resize_Line_Edit(num_Points);});
	connect(min_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(min_Edit);});
	connect(max_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(max_Edit);});
	connect(step_Edit,	&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(step_Edit);});

	connect(num_Points, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{refresh_Show_Data();});
	connect(min_Edit,	&QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
	connect(max_Edit,	&QLineEdit::textEdited, this, [=]{refresh_Show_Data();});
}

void Confidence_Interval_Editor::refresh_Show_Data(bool show)
{
	// PARAMETER

	Parameter* parameter = coupling_Editor->coupled_Parameter;

	int line_edit_precision = line_edit_density_precision;
	int thumbnail_precision = thumbnail_density_precision;
	double coeff = 1; // should be 1 by default!
	Global_Variables::get_Parameter_From_Struct_Item_by_Whats_This(
							   coupling_Editor->struct_Data,
							   coupling_Editor->coupled_Parameter->indicator.whats_This,
							   &line_edit_precision,
							   &thumbnail_precision,
							   &units,
							   &coeff);

	// getting index
	int index = -1;
	if(parameter->indicator.whats_This == whats_This_Composition ||
	   parameter->indicator.whats_This == whats_This_Interlayer_Composition ||
	   parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma )
	{
		for(int i=0; i<coupling_Editor->struct_Data.composition.size(); ++i)
		{
			if(parameter->indicator.id == coupling_Editor->struct_Data.composition[i].composition.indicator.id) index = i;
		}
		for(int i=0; i<transition_Layer_Functions_Size; ++i)
		{
			if(parameter->indicator.id == coupling_Editor->struct_Data.interlayer_Composition[i].interlayer.indicator.id)	index = i;
			if(parameter->indicator.id == coupling_Editor->struct_Data.interlayer_Composition[i].my_Sigma.indicator.id)	index = i;
		}
	}

	units_Label->setText(units);
	step_Units_Label->setText(units);

	// show data
	if(show)
	{
		min_Edit->setText(QString::number(parameter->confidence.min/coeff,line_edit_double_format,line_edit_precision));
		max_Edit->setText(QString::number(parameter->confidence.max/coeff,line_edit_double_format,line_edit_precision));

		Global_Variables::resize_Line_Edit(min_Edit);
		Global_Variables::resize_Line_Edit(max_Edit);

		// LAST! causes refresh
		num_Points->setValue(parameter->confidence.num_Points);
		Global_Variables::resize_Line_Edit(num_Points);
	} else
	// refresh data
	{
		// special cases
		if(parameter->indicator.whats_This == whats_This_Gamma)
		{
			if(min_Edit->text().toDouble()>1)
			{
				min_Edit->setText(QString::number(parameter->confidence.min,line_edit_double_format,line_edit_gamma_precision));
				Global_Variables::resize_Line_Edit(min_Edit);
			}
			if(max_Edit->text().toDouble()>1)
			{
				max_Edit->setText(QString::number(parameter->confidence.max,line_edit_double_format,line_edit_gamma_precision));
				Global_Variables::resize_Line_Edit(max_Edit);
			}
		}

		parameter->confidence.num_Points = num_Points->value();
		parameter->confidence.min = min_Edit->text().toDouble()*coeff;
		parameter->confidence.max = max_Edit->text().toDouble()*coeff;
	}
	step_Edit->setText( QString::number((parameter->confidence.max-parameter->confidence.min)/(parameter->confidence.num_Points-1)/coeff,line_edit_double_format,line_edit_precision));
	Global_Variables::resize_Line_Edit(step_Edit);
}


