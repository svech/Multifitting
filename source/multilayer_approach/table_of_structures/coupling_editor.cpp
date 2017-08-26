#include "coupling_editor.h"

Coupling_Editor::Coupling_Editor(Parameter parameter, QTabWidget *main_Tabs, QWidget* parent):
	main_Tabs(main_Tabs),
	QDialog(parent)
{
	setWindowTitle("<"+main_Tabs->tabText(parameter.indicator.tab_Index)+"> "+parameter.indicator.full_Name/*+" "+QString::number(parameter.indicator.id)*/);
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Coupling_Editor::closeEvent(QCloseEvent *)
{
	// enable context menu
	for(int tab_Index=0; tab_Index<main_Tabs->count(); ++tab_Index)
	{
		My_Table_Widget* table = dynamic_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));
		table->setContextMenuPolicy(Qt::DefaultContextMenu);
	}
}

void Coupling_Editor::set_Window_Geometry()
{
	adjustSize();
//	setFixedSize(size());
}

void Coupling_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,0,4,0);

	create_Master_Box();
		main_Layout->addWidget(master_Group_Box);
	create_Slave_Box();
		main_Layout->addWidget(slave_Group_Box);

	done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		done_Button->setFocus();
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button, &QPushButton::clicked, this, &Coupling_Editor::close);

	// disable context menu
	for(int tab_Index=0; tab_Index<main_Tabs->count(); ++tab_Index)
	{
		My_Table_Widget* table = dynamic_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));
		table->setContextMenuPolicy(Qt::CustomContextMenu);
	}
}

void Coupling_Editor::create_Master_Box()
{
	master_Group_Box = new QGroupBox("Master");
		master_Group_Box->setObjectName("master_Group_Box");
		master_Group_Box->setStyleSheet("QGroupBox#master_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
										"QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");

	QHBoxLayout* group_Box_Layout = new QHBoxLayout(master_Group_Box);

	master_Label = new QLabel(".........<no master>.........");
		group_Box_Layout->addWidget(master_Label);

	master_Line_Edit = new QLineEdit("1");
		master_Line_Edit->setMinimumWidth(MIN_FORMULA_WIDTH_LINE_EDIT);
		group_Box_Layout->addWidget(master_Line_Edit);

	QHBoxLayout* button_Layout = new QHBoxLayout;
		button_Layout->setAlignment(Qt::AlignRight);
		button_Layout->setSpacing(0);
		group_Box_Layout->addLayout(button_Layout);

	set_Master_Button = new QPushButton("Set");
		button_Layout->addWidget(set_Master_Button,0,Qt::AlignRight);
	remove_Master_Button = new QPushButton("Remove");
		button_Layout->addWidget(remove_Master_Button,0,Qt::AlignRight);

	connect(qApp, &QApplication::focusChanged, this, [=](QWidget* old, QWidget* now){enable_Getting_Parameter(old, now, master_Label, master_Line_Edit);});
}

void Coupling_Editor::create_Slave_Box()
{
	slave_Group_Box = new QGroupBox("Slaves");
		slave_Group_Box->setObjectName("slave_Group_Box");
		slave_Group_Box->setStyleSheet("QGroupBox#slave_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
										"QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");

	slave_Group_Box_Layout = new QVBoxLayout(slave_Group_Box);

	add_Slave(0);
}

void Coupling_Editor::set_Slave(int index_Pressed)
{

}

void Coupling_Editor::remove_Slave(int index_Pressed)
{

}

void Coupling_Editor::add_Slave(int index_Pressed)
{
	QHBoxLayout* frame_Layout = new QHBoxLayout;
		slave_Group_Box_Layout->insertLayout(index_Pressed, frame_Layout);

	QLabel* slave_Label = new QLabel(".........<no slave>..........");
		frame_Layout->addWidget(slave_Label);
		slave_Label_Vec.insert(index_Pressed, slave_Label);

	QLineEdit* slave_Line_Edit = new QLineEdit("1");
		slave_Line_Edit->setMinimumWidth(MIN_FORMULA_WIDTH_LINE_EDIT);
		frame_Layout->addWidget(slave_Line_Edit);
		slave_Line_Edit_Vec.insert(index_Pressed, slave_Line_Edit);

	bool slave_Connected = false;
		slave_Connected_Vec.insert(index_Pressed, slave_Connected);

	QHBoxLayout* button_Layout = new QHBoxLayout;
		button_Layout->setAlignment(Qt::AlignRight);
		button_Layout->setSpacing(0);
		frame_Layout->addLayout(button_Layout);

	QPushButton* set_Slave_Button = new QPushButton("Set");
		button_Layout->addWidget(set_Slave_Button,0,Qt::AlignRight);
	QPushButton* remove_Slave_Button = new QPushButton("Remove");
		button_Layout->addWidget(remove_Slave_Button,0,Qt::AlignRight);
	QPushButton* add_Slave_Button = new QPushButton("Add");
		button_Layout->addWidget(add_Slave_Button,0,Qt::AlignRight);


	qInfo() << "index_Pressed of created = " << index_Pressed;
	connect(set_Slave_Button,    &QPushButton::clicked, this, [=]{ set_Slave   (slave_Label_Vec.indexOf(slave_Label)); });
	connect(remove_Slave_Button, &QPushButton::clicked, this, [=]{ remove_Slave(slave_Label_Vec.indexOf(slave_Label)); });
	connect(add_Slave_Button,	 &QPushButton::clicked, this, [=]{ add_Slave   (slave_Label_Vec.indexOf(slave_Label)+1); });

	connect(qApp, &QApplication::focusChanged, this, [=](QWidget* old, QWidget* now){enable_Getting_Parameter(old, now, slave_Label, slave_Line_Edit);});
}

void Coupling_Editor::enable_Getting_Parameter(QWidget* old, QWidget* now, QLabel *label, QLineEdit* line_Edit)
{
	// got focus
	if( line_Edit != qobject_cast<QLineEdit*>(old) &&
		line_Edit == qobject_cast<QLineEdit*>(now) )
	{
		for(int tab_Index=0; tab_Index<main_Tabs->count(); ++tab_Index)
		{
			My_Table_Widget* table = dynamic_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));
			disconnect(table, &My_Table_Widget::customContextMenuRequested, this, nullptr);
//			if(line_Edit == master_Line_Edit)
			   connect(table, &My_Table_Widget::customContextMenuRequested, this, [=]{get_Parameter(label);});
//			else
//			   connect(table, &My_Table_Widget::customContextMenuRequested, this, [=]{get_Parameter_Slave(index);});
		}
	}
}

void Coupling_Editor::get_Parameter(QLabel* label)
{
	My_Table_Widget* table = qobject_cast<My_Table_Widget*>(QObject::sender());
	QWidget* widget = table->get_Cell_Widget();

	if(widget)
	{
		if(!widget->whatsThis().isEmpty())
		{
			Parameter parameter = widget->property(parameter_Property).value<Parameter>();
			qInfo() << "parameter id = " << parameter.indicator.id << "\n" << main_Tabs->tabText(parameter.indicator.tab_Index) << " " << parameter.indicator.full_Name << endl;
			label->setText("<"+main_Tabs->tabText(parameter.indicator.tab_Index)+"> "+parameter.indicator.full_Name/* + " " + QString::number(parameter.indicator.id)*/);
		}
	}
}
