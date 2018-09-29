#include "layer_thickness_transfer.h"

Layer_Thickness_Transfer::Layer_Thickness_Transfer(QWidget* item_Widget,
												   Table_Of_Structures* table_Of_Structures,
												   QWidget* parent):
	item_Widget(item_Widget),
	table_Of_Structures(table_Of_Structures),
	QDialog(parent)
{
	struct_Item = table_Of_Structures->coupled_Back_Widget_and_Struct_Item.value(item_Widget);
	struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	setWindowTitle("<"+table_Of_Structures->main_Tabs->tabText(table_Of_Structures->main_Tabs->currentIndex())+"> "+Global_Variables::structure_Item_Name(struct_Data));
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Layer_Thickness_Transfer::closeEvent(QCloseEvent *)
{

}

void Layer_Thickness_Transfer::set_Window_Geometry()
{

}

void Layer_Thickness_Transfer::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(4,4,4,0);

	create_Content_Box();
		main_Layout->addWidget(content_Group_Box);

	// buttons
	QHBoxLayout* button_Layout = new QHBoxLayout;
		button_Layout->setSpacing(0);
		button_Layout->setContentsMargins(4,0,4,0);
	{
		QPushButton* done_Button = new QPushButton("Done");
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
			done_Button->setDefault(true);
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

		content_Group_Box_Layout = new QVBoxLayout(content_Group_Box);

	create_Layer_Lines();
}

void Layer_Thickness_Transfer::create_Layer_Lines()
{
	// PARAMETER

	double coeff = length_Coefficients_Map.value(length_units);

	for(int child_Index = 0; child_Index<struct_Item->childCount()-1; child_Index++)
	{
		// current layer or multilayer or aperiodic
		QTreeWidgetItem* current_Child_Item = struct_Item->child(child_Index);
		Data current_Child_Data = current_Child_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		QGroupBox* current_Child_Group_Box = new QGroupBox(Global_Variables::structure_Item_Name(current_Child_Data));
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

			QDoubleSpinBox* current_Child_SpinBox = new QDoubleSpinBox;
				current_Child_SpinBox->setSuffix(" "+length_units);
				current_Child_SpinBox->setValue(current_Child_Value/coeff);
			child_Partner_Layout->addWidget(current_Child_SpinBox);

			QLabel* arrow_Label = new QLabel("<---->");
			child_Partner_Layout->addWidget(arrow_Label);

			QDoubleSpinBox* partner_SpinBox = new QDoubleSpinBox;
				partner_SpinBox->setSuffix(" "+length_units);
				partner_SpinBox->setValue(partner_Value/coeff);
			child_Partner_Layout->addWidget(partner_SpinBox);

			QLabel* partner_Label = new QLabel(Global_Variables::structure_Item_Name(partner_Data));
			child_Partner_Layout->addWidget(partner_Label);
		}

//		QDoubleSpinBox* layer_DSB = new QDoubleSpinBox;
//			layer_DSB->setAccelerated(true);
//			layer_DSB->setRange(-MAX_DOUBLE, MAX_DOUBLE);
//			layer_DSB->setDecimals(5);
//			layer_DSB->setSingleStep(0.0001);
//			layer_DSB->setFixedWidth(TARGET_LINE_EDIT_WIDTH);
//			layer_DSB->setButtonSymbols(QAbstractSpinBox::NoButtons);
	}
}
