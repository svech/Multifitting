#include "coupling_editor.h"

Coupling_Editor::Coupling_Editor(QWidget* coupling_Widget, QTabWidget *main_Tabs, QWidget* parent):
	coupling_Widget(coupling_Widget),
	coupling_Parameter(coupling_Widget->property(parameter_Property).value<Parameter>()),
	main_Tabs(main_Tabs),
	QDialog(parent)
{
	setWindowTitle("<"+main_Tabs->tabText(coupling_Parameter.indicator.tab_Index)+"> "+coupling_Parameter.indicator.full_Name/*+" "+QString::number(coupling_Parameter.indicator.id)*/);
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Coupling_Editor::closeEvent(QCloseEvent *)
{
	// save data to table
	{
		// coupling parameter
		QVariant varvar;
		varvar.setValue( coupling_Parameter );
		coupling_Widget->setProperty(parameter_Property,varvar);

		// external master parameter
		if(master_Widget)
		{
			bool already_Contains = false;
			QVariant var;
			Parameter master_Parameter = master_Widget->property(parameter_Property).value<Parameter>();
			for(Parameter_Indicator master_Slave : master_Parameter.coupled.slaves)
			{
				if(master_Slave.id == coupling_Parameter.indicator.id)
				{
					already_Contains = true;
					break;
				}
			}
			if(!already_Contains)
			{
				master_Parameter.coupled.slaves.append(coupling_Parameter.indicator);
				var.setValue( master_Parameter );
				master_Widget->setProperty(parameter_Property,var);
				qInfo() << "master " << master_Parameter.indicator.full_Name << " added me as slave";
			}
		}

		// external slaves parameters
		for(QWidget* slave_Widget : slave_Widget_Vec)
		{
			if(slave_Widget)
			{
				QVariant var;
				Parameter slave_Parameter = slave_Widget->property(parameter_Property).value<Parameter>();
				slave_Parameter.coupled.master = coupling_Parameter.indicator;
				var.setValue( slave_Parameter );
				slave_Widget->setProperty(parameter_Property,var);
				qInfo() << "slave " << slave_Parameter.indicator.full_Name << " added me as master";
			}
		}
	}

	// save data to structure
	{

	}

	// enable context menu
	for(int tab_Index=0; tab_Index<main_Tabs->count(); ++tab_Index)
	{
		My_Table_Widget* table = qobject_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));
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
		My_Table_Widget* table = qobject_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));
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

	master_Label = new QLabel(no_Master_Text);
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

	load_Master();
}

void Coupling_Editor::load_Master()
{
	// if had master
	if(coupling_Parameter.coupled.has_Master)
	{
		old_Master_Widget = search_Widget_By_Id(coupling_Parameter.coupled.master.id);

		// if master still exists
		if(old_Master_Widget)
		{
			Parameter old_Master_Parameter = old_Master_Widget->property(parameter_Property).value<Parameter>();
			master_Label->setText("<"+main_Tabs->tabText(old_Master_Parameter.indicator.tab_Index)+"> "+old_Master_Parameter.indicator.full_Name);
		} else
		{
			coupling_Parameter.coupled.has_Master = false;
			master_Label->setText(no_Master_Text);
		}
	}
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
	Parameter_Indicator slave_Indicator;
	coupling_Parameter.coupled.slaves.insert(index_Pressed, slave_Indicator);

	QHBoxLayout* frame_Layout = new QHBoxLayout;
		slave_Group_Box_Layout->insertLayout(index_Pressed, frame_Layout);

	QWidget* slave_Widget = NULL;
		slave_Widget_Vec.insert(index_Pressed, slave_Widget);

	QLabel* slave_Label = new QLabel(no_Slave_Text);
		frame_Layout->addWidget(slave_Label);
		slave_Label_Vec.insert(index_Pressed, slave_Label);

	QLineEdit* slave_Line_Edit = new QLineEdit("1");
		slave_Line_Edit->setMinimumWidth(MIN_FORMULA_WIDTH_LINE_EDIT);
		frame_Layout->addWidget(slave_Line_Edit);
		slave_Line_Edit_Vec.insert(index_Pressed, slave_Line_Edit);

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
			My_Table_Widget* table = qobject_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));
			disconnect(table, &My_Table_Widget::customContextMenuRequested, this, nullptr);
			   connect(table, &My_Table_Widget::customContextMenuRequested, this, [=]{get_Parameter(label);});
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
			label->setText("<"+main_Tabs->tabText(parameter.indicator.tab_Index)+"> "+parameter.indicator.full_Name/* + " " + QString::number(parameter.indicator.id)*/);

			// Name and tab are reloading inside the Parameter at Table opening, so they should be correct after changing the structure and tab order.

			// set master
			if(label == master_Label)
			{
				// slave's side
				coupling_Parameter.coupled.has_Master = true;
				coupling_Parameter.coupled.master = parameter.indicator;

				// master's side
				master_Widget = widget;	// remember widget. data will be saved at close.
			} else
			// set slave
			{
				// slave's side
				int index = slave_Label_Vec.indexOf(label);
				slave_Widget_Vec[index] = widget;		// remember widget. data will be saved at close.

				// master's side
				coupling_Parameter.coupled.slaves[index] = parameter.indicator;
			}

			qInfo() << "parameter id = " << parameter.indicator.id << "\n" << main_Tabs->tabText(parameter.indicator.tab_Index) << " " << parameter.indicator.full_Name << endl;
//			search_Widget_By_Id(parameter.indicator.id);
		}
	}
}

QWidget* Coupling_Editor::search_Widget_By_Id(int id)
{
	for(int tab_Index = 0; tab_Index<main_Tabs->count(); tab_Index++)
	{
		My_Table_Widget* table = qobject_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));

		// search over sheet
		for(int row=0; row<table->rowCount(); row++)
		{
			for(int column=0; column<table->columnCount(); column++)
			{
				QWidget* widget = table->cellWidget(row,column);
				if(widget)
				{
					Parameter parameter = widget->property(parameter_Property).value<Parameter>();
					if(parameter.indicator.id == id)
					{
//						qInfo() << "id = " << id << " found at " << "tab " << tab_Index << "; row " << row << "; column " << column;
						return widget;
					}
				}
			}
		}
	}
	return NULL;
}
