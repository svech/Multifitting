#include "coupling_editor.h"

Coupling_Editor::Coupling_Editor(QWidget* coupling_Widget,
								 QMap<QWidget*,QTreeWidgetItem*>& coupled_Widgets_Item,
								 QMap<int, QWidget*>& coupled_Widgets_Id,
								 QTabWidget* main_Tabs,
								 bool not_Change_Context_Menu,
								 QWidget* parent):
	coupled_Widgets_Item(coupled_Widgets_Item),
	coupled_Widgets_Id(coupled_Widgets_Id),
	coupling_Widget(coupling_Widget),
	coupling_Parameter(coupling_Widget->property(parameter_Property).value<Parameter>()),
	main_Tabs(main_Tabs),
	not_Change_Context_Menu(not_Change_Context_Menu),
	QDialog(parent)
{
	setWindowTitle("<"+main_Tabs->tabText(coupling_Parameter.indicator.tab_Index)+"> "+coupling_Parameter.indicator.full_Name/*+" "+QString::number(coupling_Parameter.indicator.id)*/);
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Coupling_Editor::closeEvent(QCloseEvent *)
{
	// save data
	{
		// clear for having correct number of slaves
		clear_Nonexisting_Slaves();

		// save myself
		refresh_Reload_Coupled(refresh_Property, coupling_Parameter, coupling_Widget);

		// save external master
		save_External_Master();

		// save external slaves
		save_External_Slaves();

//		qInfo() << "saved" <<     coupling_Parameter.coupled.slaves.size() << "slaves";
//		qInfo() << "saved" << int(coupling_Parameter.coupled.master.exist) << "master";
//		qInfo() << "";
	}

	// enable context menu
	if(!not_Change_Context_Menu)
	for(int tab_Index=0; tab_Index<main_Tabs->count(); ++tab_Index)
	{
		My_Table_Widget* table = qobject_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));
		table->setContextMenuPolicy(Qt::DefaultContextMenu);
	}
}

void Coupling_Editor::set_Window_Geometry()
{
	adjustSize();
//	setFixedHeight(height());
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
	if(!not_Change_Context_Menu)
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

	master_Line_Edit = new QLineEdit(coupling_Parameter.coupled.master.expression);
		master_Line_Edit->setMinimumWidth(MIN_FORMULA_WIDTH_LINE_EDIT);
		master_Line_Edit->setProperty(previous_Expression_Property,master_Line_Edit->text());
		group_Box_Layout->addWidget(master_Line_Edit);

	QHBoxLayout* button_Layout = new QHBoxLayout;
		button_Layout->setAlignment(Qt::AlignRight);
		button_Layout->setSpacing(0);
		group_Box_Layout->addLayout(button_Layout);

	remove_Master_Button = new QPushButton("Clear");
		button_Layout->addWidget(remove_Master_Button,0,Qt::AlignRight);

	connect(remove_Master_Button, &QPushButton::clicked, this, &Coupling_Editor::remove_Master);
	connect(qApp, &QApplication::focusChanged, this, [=](QWidget* old, QWidget* now)
			{
				enable_Getting_Parameter(old, now, master_Label, master_Line_Edit);
			});
	connect(master_Line_Edit, &QLineEdit::editingFinished, this, &Coupling_Editor::check_Expression);

	load_Master();
}

void Coupling_Editor::remove_Master()
{
	coupling_Parameter.coupled.master.exist = false;
	master_Label->setText(no_Master_Text);
	master_Widget = NULL;
}

void Coupling_Editor::load_Master()
{
	bool loaded = false;
	// now check if had master
	if(coupling_Parameter.coupled.master.exist)
	{
		old_Master_Widget = coupled_Widgets_Id.value(coupling_Parameter.coupled.master.id); // old_Master_Widget is set only here

		// if old master widget still exists
		if(old_Master_Widget)
		{
			Parameter old_Master_Parameter = old_Master_Widget->property(parameter_Property).value<Parameter>();

			bool have = false;
			for(Parameter_Indicator& slave_Of_Old_Master : old_Master_Parameter.coupled.slaves)
			{
				if(slave_Of_Old_Master.id == coupling_Parameter.indicator.id)
					have = true;
			}

			if(have)
			{
				master_Widget = old_Master_Widget;
				master_Label->setText("<"+main_Tabs->tabText(old_Master_Parameter.indicator.tab_Index)+"> "+old_Master_Parameter.indicator.full_Name);
				loaded = true;
//				qInfo() << "loaded 1 master";
			}
		}
	}
	if(!loaded)
	{
		master_Widget = NULL;
		coupling_Parameter.coupled.master.exist = false;
		master_Label->setText(no_Master_Text);
//		qInfo() << "loaded 0 masters";
	}
}

void Coupling_Editor::save_External_Master()
{
	// remove me from previous master (if master was really changed)
	if(old_Master_Widget &&
	   old_Master_Widget != master_Widget) // compare widgets. Be careful!
	{
		// if master widget exists (it should be always true)
		Parameter old_Master_Parameter = old_Master_Widget->property(parameter_Property).value<Parameter>();
		for(int old_Index=old_Master_Parameter.coupled.slaves.size()-1; old_Index>=0; old_Index--)
		{
			if(old_Master_Parameter.coupled.slaves[old_Index].id == coupling_Parameter.indicator.id)
			{
				old_Master_Parameter.coupled.slaves.remove(old_Index);
			}
		}

		// save old master
		refresh_Reload_Coupled(refresh_Property, old_Master_Parameter, old_Master_Widget);

//		qInfo() << "old master " << old_Master_Parameter.indicator.full_Name << " removed me from slaves. Now " << old_Master_Parameter.coupled.slaves.size() << "slaves";
	}

	// add me to current master
	if(master_Widget)
	{
		bool already_Contains = false;
		Parameter master_Parameter = master_Widget->property(parameter_Property).value<Parameter>();
		for(Parameter_Indicator& master_Slave : master_Parameter.coupled.slaves)
		{
			if(master_Slave.id == coupling_Parameter.indicator.id)
			{
				already_Contains = true;
				master_Slave.expression = coupling_Parameter.coupled.master.expression;
				break;
			}
		}
		if(!already_Contains)
		{
			coupling_Parameter.indicator.exist = true; // i should exist as Master's slave
			coupling_Parameter.indicator.expression = coupling_Parameter.coupled.master.expression;
			master_Parameter.coupled.slaves.append(coupling_Parameter.indicator);
		}


		// save master
		refresh_Reload_Coupled(refresh_Property, master_Parameter, master_Widget);

//		qInfo() << "master " << master_Parameter.indicator.full_Name << " added me as slave. Now " << master_Parameter.coupled.slaves.size() << "slaves";
	}
}

void Coupling_Editor::create_Slave_Box()
{
	slave_Group_Box = new QGroupBox("Slaves");
		slave_Group_Box->setObjectName("slave_Group_Box");
		slave_Group_Box->setStyleSheet("QGroupBox#slave_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
										"QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");

	slave_Group_Box_Layout = new QVBoxLayout(slave_Group_Box);

	load_Slaves();
}

void Coupling_Editor::remove_Slave(int index_Pressed)
{
	coupling_Parameter.coupled.slaves[index_Pressed].exist = false;
	slave_Label_Vec[index_Pressed]->setText(no_Slave_Text);
	slave_Widget_Vec[index_Pressed] = NULL;
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

	QLineEdit* slave_Line_Edit = new QLineEdit(coupling_Parameter.coupled.slaves[index_Pressed].expression);
		slave_Line_Edit->setMinimumWidth(MIN_FORMULA_WIDTH_LINE_EDIT);
		frame_Layout->addWidget(slave_Line_Edit);
		slave_Line_Edit->setProperty(previous_Expression_Property,slave_Line_Edit->text());
		slave_Line_Edit_Vec.insert(index_Pressed, slave_Line_Edit);

	QHBoxLayout* button_Layout = new QHBoxLayout;
		button_Layout->setAlignment(Qt::AlignRight);
		button_Layout->setSpacing(0);
		frame_Layout->addLayout(button_Layout);

	QPushButton* remove_Slave_Button = new QPushButton("Clear");
		button_Layout->addWidget(remove_Slave_Button,0,Qt::AlignRight);
	QPushButton* add_Slave_Button = new QPushButton("Add");
		button_Layout->addWidget(add_Slave_Button,0,Qt::AlignRight);

	connect(remove_Slave_Button, &QPushButton::clicked, this, [=]{ remove_Slave(slave_Label_Vec.indexOf(slave_Label));   });
	connect(add_Slave_Button,	 &QPushButton::clicked, this, [=]{ add_Slave   (slave_Label_Vec.indexOf(slave_Label)+1); });
	connect(slave_Line_Edit,	 &QLineEdit::editingFinished, this, &Coupling_Editor::check_Expression);

	connect(qApp, &QApplication::focusChanged, this, [=](QWidget* old, QWidget* now){enable_Getting_Parameter(old, now, slave_Label, slave_Line_Edit);});
}

void Coupling_Editor::load_Slaves()
{
	// make copy of slaves
	old_Slaves = coupling_Parameter.coupled.slaves; // old slaves are set only here
	coupling_Parameter.coupled.slaves.clear();
	add_Slave(0);
	int counter = 0;

	for(Parameter_Indicator& old_Slave : old_Slaves)
	{
		QWidget* old_Slave_Widget = coupled_Widgets_Id.value(old_Slave.id);

		// if old slave widget still exists
		if(old_Slave_Widget)
		{
			Parameter old_Slave_Parameter = old_Slave_Widget->property(parameter_Property).value<Parameter>();
			if( old_Slave_Parameter.coupled.master.exist &&
				old_Slave_Parameter.coupled.master.id == coupling_Parameter.indicator.id)
			{
				if(counter>=slave_Label_Vec.size())
					add_Slave(counter);

				slave_Label_Vec[counter]->setText("<"+main_Tabs->tabText(old_Slave_Parameter.indicator.tab_Index)+"> "+old_Slave_Parameter.indicator.full_Name);
				slave_Widget_Vec[counter] = old_Slave_Widget;
				coupling_Parameter.coupled.slaves[counter] = old_Slave_Parameter.indicator;
				coupling_Parameter.coupled.slaves[counter].exist = true;
				coupling_Parameter.coupled.slaves[counter].expression = old_Slave_Parameter.coupled.master.expression;
				slave_Line_Edit_Vec[counter]->setText(coupling_Parameter.coupled.slaves[counter].expression);

				counter++;
			}
		}
	}
//	qInfo() << "loaded" << counter << " of " << old_Slaves.size() << "slaves";
}

void Coupling_Editor::save_External_Slaves()
{
	// ex-slaves remove me from their master place
	for(Parameter_Indicator& old_Slave : old_Slaves)
	{
		unsigned long long int id = old_Slave.id;
		bool keep = false;

		// search in actual base
		for(Parameter_Indicator& slave : coupling_Parameter.coupled.slaves)
		{
			if(slave.exist && (slave.id == id))
				keep = true;
		}

		// if not found, mark and remove from list
		if(!keep)
		{
			QWidget* deprecated_Widget = coupled_Widgets_Id.value(id);
			if(deprecated_Widget)
			{				
				Parameter deprecated_Parameter = deprecated_Widget->property(parameter_Property).value<Parameter>();
				deprecated_Parameter.coupled.master.exist = false;

				// save old slave
				refresh_Reload_Coupled(refresh_Property, deprecated_Parameter, deprecated_Widget);
			}
		}
	}

	int index=0;
	// current slaves add me as master
	for(QWidget* slave_Widget : slave_Widget_Vec)
	{
		if(slave_Widget)
		{
			Parameter slave_Parameter = slave_Widget->property(parameter_Property).value<Parameter>();
			slave_Parameter.coupled.master = coupling_Parameter.indicator;
			slave_Parameter.coupled.master.exist = true;
			slave_Parameter.coupled.master.expression = coupling_Parameter.coupled.slaves[index].expression;

			// save slave
			refresh_Reload_Coupled(refresh_Property, slave_Parameter, slave_Widget);
//			qInfo() << "slave " << slave_Parameter.indicator.full_Name << " added me as master";
		}
		index++;
	}
}

void Coupling_Editor::clear_Nonexisting_Slaves()
{
	int index_To_Remove = -2018;

	// remove onn-ral parameter indicators from the list
	for(int i=0; i<coupling_Parameter.coupled.slaves.size(); ++i)
	{
		if(!coupling_Parameter.coupled.slaves[i].exist)
		{
			index_To_Remove=i;
			break;
		}
	}

	if(index_To_Remove<0)
	{
		return;
	} else
	{
		coupling_Parameter.coupled.slaves.remove(index_To_Remove);
		slave_Widget_Vec.remove(index_To_Remove);
		slave_Label_Vec.remove(index_To_Remove);
		slave_Line_Edit_Vec.remove(index_To_Remove);
		clear_Nonexisting_Slaves();
	}
}

void Coupling_Editor::refresh_Reload_Coupled(QString refresh_Reload, Parameter& parameter, QWidget* widget)
{
	if(!widget)
	{
		qInfo() << "Coupling_Editor::refresh_Reload_Coupled : null widget";
		QMessageBox::critical(NULL, "Coupling_Editor::refresh_Reload_Coupled", "null widget");
		return;
	}

	// refresh and reload directly from structure tree
	Table_Of_Structures::refresh_Reload_Core(refresh_Reload, widget, parameter, coupled_Widgets_Item);
}

void Coupling_Editor::check_Expression()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(sender());
	QString expression = line_Edit->text();

	if(!Global_Variables::expression_Is_Valid(expression, QStringList(expression_Master_Slave_Variable)))
	{
		QMessageBox::information(this, "Wrong expression", "Expression has wrong syntax");
		line_Edit->setText(line_Edit->property(previous_Expression_Property).toString());
	} else
	{
		if(line_Edit == master_Line_Edit)
		{
			coupling_Parameter.coupled.master.expression = expression;
		} else
		{
			int index = slave_Line_Edit_Vec.indexOf(line_Edit);
			coupling_Parameter.coupled.slaves[index].expression = expression;
		}
		line_Edit->setProperty(previous_Expression_Property, expression);
	}
}

void Coupling_Editor::enable_Getting_Parameter(QWidget* old, QWidget* now, QLabel* label, QLineEdit* line_Edit)
{
	// got focus
	if( line_Edit != qobject_cast<QLineEdit*>(old) &&
		line_Edit == qobject_cast<QLineEdit*>(now) )
	{
		for(int tab_Index=0; tab_Index<main_Tabs->count(); ++tab_Index)
		{
			My_Table_Widget* table = qobject_cast<My_Table_Widget*>(main_Tabs->widget(tab_Index));
			disconnect(table, &My_Table_Widget::customContextMenuRequested, this, nullptr);
			   connect(table, &My_Table_Widget::customContextMenuRequested, this, [=]{ get_Parameter(label); });
		}
	}
}

void Coupling_Editor::get_Parameter(QLabel* label)
{
	My_Table_Widget* table = qobject_cast<My_Table_Widget*>(QObject::sender());
	QWidget* widget = table->get_Cell_Widget();

	if(widget)
	if(widget->property(coupling_Editor_Property).toBool())
	{
		Parameter parameter = widget->property(parameter_Property).value<Parameter>();

		// period and gamma can't be connected
		if(parameter.indicator.whats_This == whats_This_Period || parameter.indicator.whats_This == whats_This_Gamma) return;

		// Name and tab are reloading inside the Parameter at Table opening, so they will be corrected after changing the structure and tab order from outside.

		// set master
		if(label == master_Label)
		{
			// slave's side (ME)
			QString expression = coupling_Parameter.coupled.master.expression;
			coupling_Parameter.coupled.master = parameter.indicator;
			coupling_Parameter.coupled.master.exist = true;
			coupling_Parameter.coupled.master.expression = expression;

			// master's side
			master_Widget = widget;					// remember widget. data will be saved at close.
			label->setText("<"+main_Tabs->tabText(parameter.indicator.tab_Index)+"> "+parameter.indicator.full_Name/* + " " + QString::number(parameter.indicator.id)*/);
		} else
		// set slave
		{
			// look for this slave in our slaves
			bool have = false;
			for(Parameter_Indicator& slave : coupling_Parameter.coupled.slaves)
			{
				if(slave.id == parameter.indicator.id)
					have = true;
			}

			// if we dont have such slave
			if(!have)
			{
				// slave's side
				int index = slave_Label_Vec.indexOf(label);
				slave_Widget_Vec[index] = widget;		// remember widget. data will be saved at close.

				// master's side (ME)

				QString expression = coupling_Parameter.coupled.slaves[index].expression;
				coupling_Parameter.coupled.slaves[index] = parameter.indicator;
				coupling_Parameter.coupled.slaves[index].exist = true;
				coupling_Parameter.coupled.slaves[index].expression = expression;

				label->setText("<"+main_Tabs->tabText(parameter.indicator.tab_Index)+"> "+parameter.indicator.full_Name/* + " " + QString::number(parameter.indicator.id)*/);
			}
		}
//		qInfo() << "parameter id = " << parameter.indicator.id << "\n" << main_Tabs->tabText(parameter.indicator.tab_Index) << " " << parameter.indicator.full_Name << endl;
	}
}

