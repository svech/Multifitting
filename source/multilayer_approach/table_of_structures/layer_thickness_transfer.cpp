#include "layer_thickness_transfer.h"

Layer_Thickness_Transfer::Layer_Thickness_Transfer(QWidget* item_Widget,
												   Table_Of_Structures* table_Of_Structures,
												   QWidget* parent):
	item_Widget(item_Widget),
	table_Of_Structures(table_Of_Structures),
	coeff(length_Coefficients_Map.value(length_units)),
	QDialog(parent)
{
	struct_Item = table_Of_Structures->coupled_Back_Widget_and_Struct_Item.value(item_Widget);
	struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	units_Suffix = "  "+length_units;
	setWindowTitle("<"+table_Of_Structures->main_Tabs->tabText(table_Of_Structures->main_Tabs->currentIndex())+"> "+Global_Variables::structure_Item_Name(struct_Data));
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Layer_Thickness_Transfer::closeEvent(QCloseEvent *)
{
	table_Of_Structures->setProperty(multilayer_Item_Table_CheckBox_Property,multilayer_Item_Table_CheckBox_Property);
	table_Of_Structures->layer_Thickness_Transfer_Is_Created = false;
}

void Layer_Thickness_Transfer::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
}

void Layer_Thickness_Transfer::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(4,4,4,0);

	create_Content_Box();

	// buttons
	QHBoxLayout* button_Layout = new QHBoxLayout;
		button_Layout->setSpacing(0);
		button_Layout->setContentsMargins(4,0,4,0);
	{
		QPushButton* done_Button = new QPushButton("Done");
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//			done_Button->setFocus();
//			done_Button->setDefault(true);
		button_Layout->addWidget(done_Button,0,Qt::AlignCenter);

		connect(done_Button, &QPushButton::clicked, this, &Layer_Thickness_Transfer::close);
	}
	main_Layout->addLayout(button_Layout);

	// shortcuts
	{
		QShortcut* save_Shortcut			= new QShortcut(QKeySequence(Qt::Key_S | Qt::CTRL), this);
		QShortcut* open_Shortcut			= new QShortcut(QKeySequence(Qt::Key_O | Qt::CTRL), this);
		QShortcut* fit_Shortcut				= new QShortcut(QKeySequence(Qt::Key_F | Qt::CTRL | Qt::SHIFT), this);
		QShortcut* calc_Specular_Shortcut	= new QShortcut(QKeySequence(Qt::Key_C | Qt::CTRL | Qt::SHIFT), this);

		connect(save_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->save(default_File);});
		connect(open_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->open(default_File);});
		connect(fit_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->start_Fitting();	  });
		connect(calc_Specular_Shortcut, &QShortcut::activated, this, [=]{ global_Multilayer_Approach->calc_Reflection(); });
	}
}

void Layer_Thickness_Transfer::create_Content_Box()
{
	content_Group_Box = new QGroupBox;
		content_Group_Box->setObjectName("content_Group_Box");
		content_Group_Box->setStyleSheet("QGroupBox#content_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
										 "QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
	main_Layout->addWidget(content_Group_Box);
	content_Group_Box_Layout = new QVBoxLayout(content_Group_Box);

	QHBoxLayout* top_Layout = new QHBoxLayout;
//		top_Layout->setAlignment(Qt::AlignCenter);
	content_Group_Box_Layout->addLayout(top_Layout);

	// period spinbox
	{
		QHBoxLayout* period_Layout = new QHBoxLayout;
			period_Layout->setAlignment(Qt::AlignCenter);
		top_Layout->addLayout(period_Layout);

		QLabel* pariod_Label = new QLabel("Period");
		period_Layout->addWidget(pariod_Label);

		period_SpinBox = new MyDoubleSpinBox;
			period_SpinBox->setSuffix(units_Suffix);
			period_SpinBox->setRange(0.001/coeff, MAX_DOUBLE);
			period_SpinBox->setValue(struct_Data.period.value/coeff);
			period_SpinBox->setAccelerated(true);
			period_SpinBox->setDecimals(thickness_transfer_precision);
			period_SpinBox->setSingleStep(struct_Data.step_Value_Change/coeff);
			period_SpinBox->setProperty(previous_Value_Property,period_SpinBox->value());
//			period_SpinBox->setProperty(item_Type_Property,struct_Data.item_Type);
			period_SpinBox->setProperty(id_Property,struct_Data.period.indicator.id);
		period_Layout->addWidget(period_SpinBox);
		Global_Variables::resize_Line_Edit(period_SpinBox);

		// mapping
		map_Line_Edit(struct_Data.period.indicator.id,period_SpinBox);
	}

	// step spinbox
	{
		QHBoxLayout* step_Layout = new QHBoxLayout;
			step_Layout->setAlignment(Qt::AlignCenter);
		top_Layout->addLayout(step_Layout);

		QLabel* step_Label = new QLabel("Step");
		step_Layout->addWidget(step_Label);

		step_SpinBox = new MyDoubleSpinBox;
			step_SpinBox->setSuffix(units_Suffix);
			step_SpinBox->setRange(0, MAX_DOUBLE);
			step_SpinBox->setValue(struct_Data.step_Value_Change/coeff);
			step_SpinBox->setAccelerated(true);
			step_SpinBox->setDecimals(2);
			step_SpinBox->setSingleStep(thickness_default_step_value_change/coeff);
		step_Layout->addWidget(step_SpinBox);
		Global_Variables::resize_Line_Edit(step_SpinBox);

		// apply current step value
		step = step_SpinBox->value();

		connect(step_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			period_SpinBox->setSingleStep(step_SpinBox->value());
		});
	}

	create_Layer_Lines();
}

void Layer_Thickness_Transfer::create_Layer_Lines()
{
	// PARAMETER

	for(int child_Index = 0; child_Index<struct_Item->childCount()-1; child_Index++)
	{
		// current layer or multilayer or aperiodic
		QTreeWidgetItem* current_Child_Item = struct_Item->child(child_Index);
		Data current_Child_Data = current_Child_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		QGroupBox* current_Child_Group_Box = new QGroupBox(/*Global_Variables::structure_Item_Name(current_Child_Data)*/);
			current_Child_Group_Box->setObjectName("current_Child_Group_Box");
			current_Child_Group_Box->setStyleSheet("QGroupBox#current_Child_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
												   "QGroupBox::title                  { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
		QVBoxLayout* current_Child_Group_Box_Layout = new QVBoxLayout(current_Child_Group_Box);
		content_Group_Box_Layout->addWidget(current_Child_Group_Box);

		double current_Child_Min = 0.001;
		Parameter& current_Child_Parameter = current_Child_Data.thickness;
		if(current_Child_Data.item_Type == item_Type_Layer)		 { current_Child_Parameter = current_Child_Data.thickness; current_Child_Min = 0; }
		if(current_Child_Data.item_Type == item_Type_Multilayer) { current_Child_Parameter = current_Child_Data.period;    current_Child_Min = 0.001; }
		if(current_Child_Data.item_Type == item_Type_Aperiodic)  { current_Child_Parameter = current_Child_Data.period;    current_Child_Min = 0.001; }

		for(int partner_Index = child_Index+1; partner_Index<struct_Item->childCount(); partner_Index++)
		{
			QHBoxLayout* child_Partner_Layout = new QHBoxLayout;
				child_Partner_Layout->setAlignment(Qt::AlignLeft);
			current_Child_Group_Box_Layout->addLayout(child_Partner_Layout);

			// partner layer or multilayer or aperiodic
			QTreeWidgetItem* partner_Item = struct_Item->child(partner_Index);
			Data partner_Data = partner_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

			double partner_Min = 0.001;
			Parameter& partner_Parameter = partner_Data.thickness;
			if(partner_Data.item_Type == item_Type_Layer)	   { partner_Parameter = partner_Data.thickness; partner_Min = 0; }
			if(partner_Data.item_Type == item_Type_Multilayer) { partner_Parameter = partner_Data.period;    partner_Min = 0.001;}
			if(partner_Data.item_Type == item_Type_Aperiodic)  { partner_Parameter = partner_Data.period;    partner_Min = 0.001;}

			QLabel* current_Child_Label = new QLabel(Global_Variables::structure_Item_Name(current_Child_Data));
			child_Partner_Layout->addWidget(current_Child_Label);

			MyDoubleSpinBox* current_Child_SpinBox = new MyDoubleSpinBox;
				current_Child_SpinBox->setSuffix(units_Suffix);
				current_Child_SpinBox->setRange(current_Child_Min, MAX_DOUBLE);
				current_Child_SpinBox->setValue(current_Child_Parameter.value/coeff);
				current_Child_SpinBox->setAccelerated(true);
				current_Child_SpinBox->setDecimals(thickness_transfer_precision);
				current_Child_SpinBox->setSingleStep(step);
				current_Child_SpinBox->setProperty(previous_Value_Property,current_Child_SpinBox->value());
//				current_Child_SpinBox->setProperty(item_Type_Property,current_Child_Data.item_Type);
				current_Child_SpinBox->setProperty(id_Property,current_Child_Parameter.indicator.id);
			child_Partner_Layout->addWidget(current_Child_SpinBox);
			Global_Variables::resize_Line_Edit(current_Child_SpinBox);

			// mapping
			map_Line_Edit(current_Child_Parameter.indicator.id,current_Child_SpinBox);
			map_Of_Items.insert(current_Child_SpinBox,current_Child_Item);
			map_Of_Identicals.insert(current_Child_SpinBox, current_Child_Data.id);

			QLabel* arrow_Label = new QLabel("<---->");
			child_Partner_Layout->addWidget(arrow_Label);

			MyDoubleSpinBox* partner_SpinBox = new MyDoubleSpinBox;
				partner_SpinBox->setSuffix(units_Suffix);
				partner_SpinBox->setRange(partner_Min, MAX_DOUBLE);
				partner_SpinBox->setValue(partner_Parameter.value/coeff);
				partner_SpinBox->setAccelerated(true);
				partner_SpinBox->setDecimals(thickness_transfer_precision);
				partner_SpinBox->setSingleStep(step);
				partner_SpinBox->setProperty(previous_Value_Property,partner_SpinBox->value());
//				partner_SpinBox->setProperty(item_Type_Property,partner_Data.item_Type);
				partner_SpinBox->setProperty(id_Property,partner_Parameter.indicator.id);
			child_Partner_Layout->addWidget(partner_SpinBox);
			Global_Variables::resize_Line_Edit(partner_SpinBox);

			// mapping
			map_Line_Edit(partner_Parameter.indicator.id,partner_SpinBox);
			map_Of_Items.insert(partner_SpinBox,partner_Item);
			map_Of_Identicals.insert(partner_SpinBox, partner_Data.id);

			QLabel* partner_Label = new QLabel(Global_Variables::structure_Item_Name(partner_Data));
			child_Partner_Layout->addWidget(partner_Label);

			map_Of_Partners.insert(current_Child_SpinBox,partner_SpinBox);
			map_Of_Partners.insert(partner_SpinBox,current_Child_SpinBox);

			// changing step
			connect(step_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				current_Child_SpinBox->setSingleStep(step_SpinBox->value());
				partner_SpinBox->      setSingleStep(step_SpinBox->value());
				Global_Variables::resize_Line_Edit(step_SpinBox);
			});
		}
	}

	// maps filled now
	for(MyDoubleSpinBox* identical_Current_Child : map_Of_Partners.keys())
	{
		connect(identical_Current_Child, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			spinBox_Lambda(identical_Current_Child, map_Of_Partners.value(identical_Current_Child));
		});
	}
	for(MyDoubleSpinBox* identical_Partner : map_Of_Partners.values())
	{
		connect(identical_Partner, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			spinBox_Lambda(identical_Partner, map_Of_Partners.key(identical_Partner));
		});
	}

	// period influence
	connect(period_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double period_Coeff = period_SpinBox->value()/period_SpinBox->property(previous_Value_Property).toDouble();

		// correction of round errors
		double sum = 0;
		double n = (1+sqrt(1+4*map_Of_Partners.keys().size()))/2-1;

		for(MyDoubleSpinBox* spinboxes : map_Of_Partners.keys())
		{
			int N=1;
			const Data data = map_Of_Items.value(spinboxes)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			if(data.item_Type == item_Type_Multilayer) N=data.num_Repetition.value;

			spinboxes->blockSignals(true);
			spinboxes->setValue(spinboxes->value()*period_Coeff);
			sum+=spinboxes->value()*N/n;
		}
		for(MyDoubleSpinBox* spinboxes : map_Of_Partners.keys())
		{
			spinboxes->setValue(spinboxes->value()*period_SpinBox->value()/sum);
			Global_Variables::resize_Line_Edit(spinboxes);
			spinboxes->setProperty(previous_Value_Property,spinboxes->value());
			spinboxes->blockSignals(false);
		}
		period_SpinBox->setProperty(previous_Value_Property,period_SpinBox->value());
		Global_Variables::resize_Line_Edit(period_SpinBox);

		// refresh
		refresh(period_SpinBox, line_edit_period_precision);

		// correction of round errors
		reload();

		// recalculate
		global_Multilayer_Approach->calc_Reflection(true);
	});
}

void Layer_Thickness_Transfer::spinBox_Lambda(MyDoubleSpinBox* current_Child_SpinBox, MyDoubleSpinBox* partner_SpinBox)
{
	const Data current_Child_Data = map_Of_Items.value(current_Child_SpinBox)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	const Data partner_Data       = map_Of_Items.value(partner_SpinBox)      ->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	double previous_Current_Child_SpinBox_Value = current_Child_SpinBox->property(previous_Value_Property).toDouble();
	double delta = current_Child_SpinBox->value() - previous_Current_Child_SpinBox_Value;
	int N = 1;
	int precision = 8;
	if(current_Child_Data.item_Type == item_Type_Layer)	     { precision = line_edit_thickness_precision; N = 1; }
	if(current_Child_Data.item_Type == item_Type_Multilayer) { precision = line_edit_period_precision;    N = current_Child_Data.num_Repetition.value;}
	if(current_Child_Data.item_Type == item_Type_Aperiodic)  { precision = line_edit_period_precision;    N = 1; }

	current_Child_SpinBox->setDisabled(false);
	partner_SpinBox->setDisabled(false);

	if(partner_SpinBox->value()-delta >= partner_SpinBox->minimum()*coeff)
	{
		// partner
		partner_SpinBox->blockSignals(true);
		partner_SpinBox->setValue(partner_SpinBox->value()-delta*N);
		refresh(partner_SpinBox, precision);
		Global_Variables::resize_Line_Edit(partner_SpinBox);
		partner_SpinBox->setProperty(previous_Value_Property,partner_SpinBox->value());
		partner_SpinBox->blockSignals(false);
		current_Child_SpinBox->setProperty(previous_Value_Property,current_Child_SpinBox->value());

		// current_Child identicals
		for(MyDoubleSpinBox* identical_Current_Child : map_Of_Identicals.keys(map_Of_Identicals.value(current_Child_SpinBox)))
		if(identical_Current_Child!=current_Child_SpinBox)
		{
			identical_Current_Child->blockSignals(true);
			identical_Current_Child->setValue(current_Child_SpinBox->value());
			Global_Variables::resize_Line_Edit(identical_Current_Child);
			identical_Current_Child->create_Text_Change_Connection();
			identical_Current_Child->setProperty(previous_Value_Property,identical_Current_Child->value());
			identical_Current_Child->blockSignals(false);
		}

		// partner identicals
		for(MyDoubleSpinBox* identical_Partner : map_Of_Identicals.keys(map_Of_Identicals.value(partner_SpinBox)))
		if(identical_Partner!=partner_SpinBox)
		{
			identical_Partner->blockSignals(true);
			identical_Partner->setValue(partner_SpinBox->value());
			Global_Variables::resize_Line_Edit(identical_Partner);
			identical_Partner->create_Text_Change_Connection();
			identical_Partner->setProperty(previous_Value_Property,identical_Partner->value());
			identical_Partner->blockSignals(false);
		}
	} else
	{
		current_Child_SpinBox->setValue(previous_Current_Child_SpinBox_Value);
	}

	refresh(current_Child_SpinBox, precision);
	Global_Variables::resize_Line_Edit(current_Child_SpinBox);

	// recalculate
	global_Multilayer_Approach->calc_Reflection(true);
}

void Layer_Thickness_Transfer::refresh(MyDoubleSpinBox* spinbox, int precision)
{
	QLineEdit* line_Edit = map_Of_Line_Edits.value(spinbox);
	line_Edit->setText(QString::number(spinbox->value()*coeff/coeff, line_edit_double_format, precision));
	table_Of_Structures->layer_Thickness_Transfer_Reload_Block = true;
	line_Edit->textEdited(line_Edit->text());
	table_Of_Structures->layer_Thickness_Transfer_Reload_Block = false;
}

void Layer_Thickness_Transfer::map_Line_Edit(id_Type id, MyDoubleSpinBox* spinbox)
{
	for(int i=0; i<table_Of_Structures->line_Edits_ID_Map.keys(id).size(); i++)
	{
		QLineEdit* line_Edit = table_Of_Structures->line_Edits_ID_Map.keys(id)[i];
		QString value_Type = line_Edit->property(value_Type_Property).toString();
		if(value_Type == VAL)
		{
			map_Of_Line_Edits.insert(spinbox,line_Edit);
			break;
		}
	}
}

void Layer_Thickness_Transfer::reload()
{
	for(MyDoubleSpinBox* spinbox : map_Of_Line_Edits.keys())
	{
		QLineEdit* line_Edit = map_Of_Line_Edits.value(spinbox);
		spinbox->blockSignals(true);
		spinbox->setValue(spinbox->valueFromText(line_Edit->text()));
		spinbox->blockSignals(false);
		Global_Variables::resize_Line_Edit(spinbox);
	}
}

void Layer_Thickness_Transfer::lock_Unlock_Thickness_Transfer(QTreeWidgetItem* item)
{
	const Data data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(data.item_Type == item_Type_Multilayer)
	{
		if(data.num_Repetition.value == 0)
		{
			for(MyDoubleSpinBox* spinbox : map_Of_Items.keys(item))
			{
				spinbox->setDisabled(true);
				map_Of_Partners.value(spinbox)->setDisabled(true);
			}
		} else
		{
			for(MyDoubleSpinBox* spinbox : map_Of_Items.keys(item))
			{
				MyDoubleSpinBox* partner = map_Of_Partners.value(spinbox);
				QTreeWidgetItem* partner_Item = map_Of_Items.value(partner);
				const Data partner_Data = partner_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
				if(partner_Data.item_Type == item_Type_Multilayer)
				{
					if(partner_Data.num_Repetition.value == 0)
					{
						for(MyDoubleSpinBox* spinbox1 : map_Of_Items.keys(partner_Item))
						{
							spinbox1->setDisabled(true);
							map_Of_Partners.value(spinbox1)->setDisabled(true);
						}
					} else
					{
						for(MyDoubleSpinBox* spinbox1 : map_Of_Items.keys(partner_Item))
						{
							spinbox1->setDisabled(false);
							map_Of_Partners.value(spinbox1)->setDisabled(false);
						}
					}
				} else
				{
					for(MyDoubleSpinBox* spinbox1 : map_Of_Items.keys(partner_Item))
					{
						spinbox1->setDisabled(false);
						map_Of_Partners.value(spinbox1)->setDisabled(false);
					}
				}
			}
		}
	}
}

