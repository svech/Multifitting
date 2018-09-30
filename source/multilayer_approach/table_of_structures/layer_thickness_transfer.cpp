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
	item_Widget->setProperty(multilayer_Item_Table_CheckBox_Property,multilayer_Item_Table_CheckBox_Property);
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
			period_SpinBox->setRange(0.1/coeff, MAX_DOUBLE);
			period_SpinBox->setValue(struct_Data.period.value/coeff);
			period_SpinBox->setAccelerated(true);
			period_SpinBox->setDecimals(thickness_transfer_precision);
			period_SpinBox->setSingleStep(struct_Data.step_Value_Change/coeff);
			period_SpinBox->setProperty(previous_Value_Property,period_SpinBox->value());
		period_Layout->addWidget(period_SpinBox);
		Global_Variables::resize_Line_Edit(period_SpinBox);
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

		double current_Child_Value=-2018;
		if(current_Child_Data.item_Type == item_Type_Layer)		 current_Child_Value = current_Child_Data.thickness.value;
		if(current_Child_Data.item_Type == item_Type_Multilayer) current_Child_Value = current_Child_Data.period.value;
		if(current_Child_Data.item_Type == item_Type_Aperiodic)  current_Child_Value = current_Child_Data.period.value;

		for(int partner_Index = child_Index+1; partner_Index<struct_Item->childCount(); partner_Index++)
		{
			QHBoxLayout* child_Partner_Layout = new QHBoxLayout;
				child_Partner_Layout->setAlignment(Qt::AlignLeft);
			current_Child_Group_Box_Layout->addLayout(child_Partner_Layout);

				// partner layer or multilayer or aperiodic
			QTreeWidgetItem* partner_Item = struct_Item->child(partner_Index);
			Data partner_Data = partner_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

			double partner_Value=-2018;
			if(partner_Data.item_Type == item_Type_Layer)	   partner_Value = partner_Data.thickness.value;
			if(partner_Data.item_Type == item_Type_Multilayer) partner_Value = partner_Data.period.value;
			if(partner_Data.item_Type == item_Type_Aperiodic)  partner_Value = partner_Data.period.value;

			QLabel* current_Child_Label = new QLabel(Global_Variables::structure_Item_Name(current_Child_Data));
			child_Partner_Layout->addWidget(current_Child_Label);

			MyDoubleSpinBox* current_Child_SpinBox = new MyDoubleSpinBox;
				current_Child_SpinBox->setSuffix(units_Suffix);
				current_Child_SpinBox->setRange(0, MAX_DOUBLE);
				current_Child_SpinBox->setValue(current_Child_Value/coeff);
				current_Child_SpinBox->setAccelerated(true);
				current_Child_SpinBox->setDecimals(thickness_transfer_precision);
				current_Child_SpinBox->setSingleStep(step);
				current_Child_SpinBox->setProperty(previous_Value_Property,current_Child_SpinBox->value());
			child_Partner_Layout->addWidget(current_Child_SpinBox);
			Global_Variables::resize_Line_Edit(current_Child_SpinBox);

			map_Of_Identicals.insert(current_Child_SpinBox, current_Child_Data.id);

			QLabel* arrow_Label = new QLabel("<---->");
			child_Partner_Layout->addWidget(arrow_Label);

			MyDoubleSpinBox* partner_SpinBox = new MyDoubleSpinBox;
				partner_SpinBox->setSuffix(units_Suffix);
				partner_SpinBox->setRange(0, MAX_DOUBLE);
				partner_SpinBox->setValue(partner_Value/coeff);
				partner_SpinBox->setAccelerated(true);
				partner_SpinBox->setDecimals(thickness_transfer_precision);
				partner_SpinBox->setSingleStep(step);
				partner_SpinBox->setProperty(previous_Value_Property,partner_SpinBox->value());
			child_Partner_Layout->addWidget(partner_SpinBox);
			Global_Variables::resize_Line_Edit(partner_SpinBox);

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
		double sum=0;

		for(MyDoubleSpinBox* spinboxes : map_Of_Partners.keys())
		{
			spinboxes->disconnect();
			spinboxes->setValue(spinboxes->value()*period_Coeff);
			sum+=spinboxes->value()/2;
		}
		for(MyDoubleSpinBox* spinboxes : map_Of_Partners.keys())
		{
			spinboxes->setValue(spinboxes->value()*period_SpinBox->value()/sum);
			Global_Variables::resize_Line_Edit(spinboxes);
			spinboxes->create_Text_Change_Connection();
			spinboxes->setProperty(previous_Value_Property,spinboxes->value());
			connect(spinboxes,static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				spinBox_Lambda(spinboxes,map_Of_Partners.value(spinboxes));
			});
		}
		period_SpinBox->setProperty(previous_Value_Property,period_SpinBox->value());
		Global_Variables::resize_Line_Edit(period_SpinBox);
	});
}

void Layer_Thickness_Transfer::spinBox_Lambda(MyDoubleSpinBox* current_Child_SpinBox, MyDoubleSpinBox* partner_SpinBox)
{
	double previous_Current_Child_SpinBox_Value = current_Child_SpinBox->property(previous_Value_Property).toDouble();
	double delta = current_Child_SpinBox->value() - previous_Current_Child_SpinBox_Value;

	if(partner_SpinBox->value()-delta >= partner_SpinBox->minimum()*coeff)
	{
		// partner
		partner_SpinBox->disconnect();
		partner_SpinBox->setValue(partner_SpinBox->value()-delta);
		Global_Variables::resize_Line_Edit(partner_SpinBox);
		partner_SpinBox->create_Text_Change_Connection();
		partner_SpinBox->setProperty(previous_Value_Property,partner_SpinBox->value());
		connect(partner_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			spinBox_Lambda(partner_SpinBox,current_Child_SpinBox);
		});
		current_Child_SpinBox->setProperty(previous_Value_Property,current_Child_SpinBox->value());

		// current_Child identicals
		for(MyDoubleSpinBox* identical_Current_Child : map_Of_Identicals.keys(map_Of_Identicals.value(current_Child_SpinBox)))
		if(identical_Current_Child!=current_Child_SpinBox)
		{
			identical_Current_Child->disconnect();
			identical_Current_Child->setValue(current_Child_SpinBox->value());
			Global_Variables::resize_Line_Edit(identical_Current_Child);
			identical_Current_Child->create_Text_Change_Connection();
			identical_Current_Child->setProperty(previous_Value_Property,identical_Current_Child->value());
			connect(identical_Current_Child, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				spinBox_Lambda(identical_Current_Child,map_Of_Partners.value(identical_Current_Child));
			});
		}

		// partner identicals
		for(MyDoubleSpinBox* identical_Partner : map_Of_Identicals.keys(map_Of_Identicals.value(partner_SpinBox)))
		if(identical_Partner!=partner_SpinBox)
		{
			identical_Partner->disconnect();
			identical_Partner->setValue(partner_SpinBox->value());
			Global_Variables::resize_Line_Edit(identical_Partner);
			identical_Partner->create_Text_Change_Connection();
			identical_Partner->setProperty(previous_Value_Property,identical_Partner->value());
			connect(identical_Partner, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				spinBox_Lambda(identical_Partner,map_Of_Partners.key(identical_Partner));
			});
		}
	} else
	{
		current_Child_SpinBox->setValue(previous_Current_Child_SpinBox_Value);
	}
	Global_Variables::resize_Line_Edit(current_Child_SpinBox);
}
