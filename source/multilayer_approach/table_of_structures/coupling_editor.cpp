#include "coupling_editor.h"

Coupling_Editor::Coupling_Editor(QWidget* coupling_Widget, QMap<QLineEdit*,QTreeWidgetItem*>& line_Edits_Map, QMap<QWidget*,QTreeWidgetItem*>& coupled_Widgets_Map, QTabWidget *main_Tabs, QWidget* parent):
	line_Edits_Map(line_Edits_Map),
	coupled_Widgets_Map(coupled_Widgets_Map),
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
	// save data
	{
		// coupling parameter
		QVariant var;
		var.setValue( coupling_Parameter );
		coupling_Widget->setProperty(parameter_Property,var);

		// external master parameter
		save_Master();

		// external slaves parameters
		save_Slaves();
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

	// load from tree
	refresh_Reload_Coupled(reload_Property, coupling_Widget);

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

	remove_Master_Button = new QPushButton("Clear");
		button_Layout->addWidget(remove_Master_Button,0,Qt::AlignRight);

	connect(remove_Master_Button, &QPushButton::clicked, this, &Coupling_Editor::remove_Master);
	connect(qApp, &QApplication::focusChanged, this, [=](QWidget* old, QWidget* now){enable_Getting_Parameter(old, now, master_Label, master_Line_Edit);});

	load_Master();
}

void Coupling_Editor::remove_Master()
{

}

void Coupling_Editor::load_Master()
{
	// now check if had master
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

void Coupling_Editor::save_Master()
{
	// external master parameter
	if(master_Widget)
	{
		bool already_Contains = false;
		Parameter master_Parameter = master_Widget->property(parameter_Property).value<Parameter>();
		for(Parameter_Indicator& master_Slave : master_Parameter.coupled.slaves)
		{
			if(master_Slave.id == coupling_Parameter.indicator.id)
			{
				already_Contains = true;
				break;
			}
		}
		if(!already_Contains)
		{
			QVariant var;
			master_Parameter.coupled.slaves.append(coupling_Parameter.indicator);
			var.setValue( master_Parameter );
			master_Widget->setProperty(parameter_Property,var);
			refresh_Reload_Coupled(refresh_Property, master_Widget);
			qInfo() << "master " << master_Parameter.indicator.full_Name << " added me as slave. Now " << master_Parameter.coupled.slaves.size() << "slaves";
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

	load_Slaves();
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

	QPushButton* remove_Slave_Button = new QPushButton("Clear");
		button_Layout->addWidget(remove_Slave_Button,0,Qt::AlignRight);
	QPushButton* add_Slave_Button = new QPushButton("Add");
		button_Layout->addWidget(add_Slave_Button,0,Qt::AlignRight);

	connect(remove_Slave_Button, &QPushButton::clicked, this, [=]{ remove_Slave(slave_Label_Vec.indexOf(slave_Label)); });
	connect(add_Slave_Button,	 &QPushButton::clicked, this, [=]{ add_Slave   (slave_Label_Vec.indexOf(slave_Label)+1); });

	connect(qApp, &QApplication::focusChanged, this, [=](QWidget* old, QWidget* now){enable_Getting_Parameter(old, now, slave_Label, slave_Line_Edit);});
}

void Coupling_Editor::load_Slaves()
{
	QVector<Parameter_Indicator> copy_Old_Slaved_Indicator_Vec = coupling_Parameter.coupled.slaves;
	coupling_Parameter.coupled.slaves.clear();
	add_Slave(0);
	int counter = 0;

	qInfo() << "loaded " << copy_Old_Slaved_Indicator_Vec.size() << " slaves";
	for(Parameter_Indicator& slave_Indicator : copy_Old_Slaved_Indicator_Vec)
	{
		QWidget* old_Slave_Widget = search_Widget_By_Id(slave_Indicator.id);

		if(old_Slave_Widget)
		{
			Parameter old_Slave_Parameter = old_Slave_Widget->property(parameter_Property).value<Parameter>();
			if( old_Slave_Parameter.coupled.has_Master &&
				old_Slave_Parameter.coupled.master.id == coupling_Parameter.indicator.id)
			{
				qInfo() << "counter = " << counter;
				if(counter>=slave_Label_Vec.size())
					add_Slave(counter);

				slave_Label_Vec[counter]->setText("<"+main_Tabs->tabText(old_Slave_Parameter.indicator.tab_Index)+"> "+old_Slave_Parameter.indicator.full_Name);
				coupling_Parameter.coupled.slaves[counter] = slave_Indicator;

				counter++;
			}
		}
	}

	loaded_Slaves = coupling_Parameter.coupled.slaves;

	refresh_Reload_Coupled(refresh_Property, coupling_Widget);
}

void Coupling_Editor::save_Slaves()
{
	// external slaves parameters
	for(QWidget* slave_Widget : slave_Widget_Vec)
	{
		if(slave_Widget)
		{
			QVariant var;
			Parameter slave_Parameter = slave_Widget->property(parameter_Property).value<Parameter>();
			slave_Parameter.coupled.has_Master = true;
			slave_Parameter.coupled.master = coupling_Parameter.indicator;
			var.setValue( slave_Parameter );
			slave_Widget->setProperty(parameter_Property,var);
			refresh_Reload_Coupled(refresh_Property, slave_Widget);
			qInfo() << "slave " << slave_Parameter.indicator.full_Name << " added me as master";
		}
	}

	// remove unused slaves on slave's side
	for(int loaded_Index=0; loaded_Index<loaded_Slaves.size(); ++loaded_Index)
	{
		int id=loaded_Slaves[loaded_Index].id;
		bool keep = false;

		// search in actual base
		for(Parameter_Indicator slave : coupling_Parameter.coupled.slaves)
		{
			if(slave.id == id)
				keep=true;
		}

		// if not found, delete
		if(!keep)
		{
			QVariant var;
			QWidget* deprecated_Widget = search_Widget_By_Id(id);
			if(deprecated_Widget)
			{
				Parameter deprecated_Parameter = deprecated_Widget->property(parameter_Property).value<Parameter>();
				deprecated_Parameter.coupled.has_Master=false;
				var.setValue( deprecated_Parameter );
				deprecated_Widget->setProperty(parameter_Property,var);
				refresh_Reload_Coupled(refresh_Property, deprecated_Widget);
			}
		}
	}
}

void Coupling_Editor::refresh_Reload_Coupled(QString refresh_Reload, QWidget* widget)
{
	if(!widget) qInfo() << "Coupling_Editor::refresh_Reload_Coupled : null widget";
	QVariant var;

	// save what we have
	if(refresh_Reload == refresh_Property)
	{
		var.setValue( coupling_Parameter );
		coupling_Widget->setProperty(parameter_Property,var);
	}

	// get access to tree item
	Parameter parameter = widget->property(parameter_Property).value<Parameter>();
	QTreeWidgetItem* structure_Item = coupled_Widgets_Map.value(widget);
	QStringList item_Type_List = structure_Item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);
	QString item_Type_String = item_Type_List[0];
	QString whats_This = parameter.indicator.whats_This;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = data.value<Ambient>();
		if(whats_This == whats_This_Composition)
		{
			for(int i=0; i<ambient.composition.size(); ++i)
			if(ambient.composition[i].composition.indicator.id == parameter.indicator.id)
			{
//				qInfo() << "FOUND " << whats_This << " " << i;
				if(refresh_Reload == reload_Property)	parameter.coupled = ambient.composition[i].composition.coupled;
				if(refresh_Reload == refresh_Property)	ambient.composition[i].composition.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Absolute_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = ambient.absolute_Density.coupled;
			if(refresh_Reload == refresh_Property)	ambient.absolute_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = ambient.relative_Density.coupled;
			if(refresh_Reload == refresh_Property)	ambient.relative_Density.coupled = parameter.coupled;
		}
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = data.value<Layer>();
		if(whats_This == whats_This_Composition)
		{
			for(int i=0; i<layer.composition.size(); ++i)
			if(layer.composition[i].composition.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = layer.composition[i].composition.coupled;
				if(refresh_Reload == refresh_Property)	layer.composition[i].composition.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Interlayer_Composition)
		{
			for(int i=0; i<layer.interlayer_Composition.size(); ++i)
			if(layer.interlayer_Composition[i].interlayer.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = layer.interlayer_Composition[i].interlayer.coupled;
				if(refresh_Reload == refresh_Property)	layer.interlayer_Composition[i].interlayer.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Interlayer_My_Sigma)
		{
			for(int i=0; i<layer.interlayer_Composition.size(); ++i)
			if(layer.interlayer_Composition[i].my_Sigma.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = layer.interlayer_Composition[i].my_Sigma.coupled;
				if(refresh_Reload == refresh_Property)	layer.interlayer_Composition[i].my_Sigma.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Absolute_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.absolute_Density.coupled;
			if(refresh_Reload == refresh_Property)	layer.absolute_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.relative_Density.coupled;
			if(refresh_Reload == refresh_Property)	layer.relative_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma.coupled = parameter.coupled;
		}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Line_Value.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Line_Value.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Rand_Rms.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Rand_Rms.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Sine_Amplitude.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Sine_Amplitude.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Sine_Frequency.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Sine_Frequency.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.thickness_Drift.drift_Sine_Phase.coupled;
			if(refresh_Reload == refresh_Property)	layer.thickness_Drift.drift_Sine_Phase.coupled = parameter.coupled;
		}

		if(whats_This == whats_This_Sigma_Drift_Line_Value)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Line_Value.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Line_Value.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Rand_Rms.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Rand_Rms.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Sine_Amplitude.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Sine_Amplitude.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Sine_Frequency.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Sine_Frequency.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = layer.sigma_Drift.drift_Sine_Phase.coupled;
			if(refresh_Reload == refresh_Property)	layer.sigma_Drift.drift_Sine_Phase.coupled = parameter.coupled;
		}
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = data.value<Substrate>();
		if(whats_This == whats_This_Composition)
		{
			for(int i=0; i<substrate.composition.size(); ++i)
			if(substrate.composition[i].composition.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = substrate.composition[i].composition.coupled;
				if(refresh_Reload == refresh_Property)	substrate.composition[i].composition.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Interlayer_Composition)
		{
			for(int i=0; i<substrate.interlayer_Composition.size(); ++i)
			if(substrate.interlayer_Composition[i].interlayer.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = substrate.interlayer_Composition[i].interlayer.coupled;
				if(refresh_Reload == refresh_Property)	substrate.interlayer_Composition[i].interlayer.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Interlayer_My_Sigma)
		{
			for(int i=0; i<substrate.interlayer_Composition.size(); ++i)
			if(substrate.interlayer_Composition[i].my_Sigma.indicator.id == parameter.indicator.id)
			{
				if(refresh_Reload == reload_Property)	parameter.coupled = substrate.interlayer_Composition[i].my_Sigma.coupled;
				if(refresh_Reload == refresh_Property)	substrate.interlayer_Composition[i].my_Sigma.coupled = parameter.coupled;
				break;
			}
		}
		if(whats_This == whats_This_Absolute_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = substrate.absolute_Density.coupled;
			if(refresh_Reload == refresh_Property)	substrate.absolute_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = substrate.relative_Density.coupled;
			if(refresh_Reload == refresh_Property)	substrate.relative_Density.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Sigma)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = substrate.sigma.coupled;
			if(refresh_Reload == refresh_Property)	substrate.sigma.coupled = parameter.coupled;
		}
		var.setValue( substrate );
	}
	if(item_Type_String == whats_This_Multilayer)
	{
		Stack_Content stack_Content = data.value<Stack_Content>();
		if(whats_This == whats_This_Period)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = stack_Content.period.coupled;
			if(refresh_Reload == refresh_Property)	stack_Content.period.coupled = parameter.coupled;
		}
		if(whats_This == whats_This_Gamma)
		{
			if(refresh_Reload == reload_Property)	parameter.coupled = stack_Content.gamma.coupled;
			if(refresh_Reload == refresh_Property)	stack_Content.gamma.coupled = parameter.coupled;
		}
		var.setValue( stack_Content );
	}

	// refreshing
	if(refresh_Reload == refresh_Property)
	{
		structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		var.setValue(parameter);
		widget->setProperty(parameter_Property, var);
	}

	// reloading
	if(refresh_Reload == reload_Property)
	{
		if(coupling_Widget == widget)
		{
			coupling_Parameter = parameter;
		}
	}
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
				master_Widget = widget;					// remember widget. data will be saved at close.
			} else
			// set slave
			{
				// slave's side
				int index = slave_Label_Vec.indexOf(label);
				slave_Widget_Vec[index] = widget;		// remember widget. data will be saved at close.

				// master's side
				coupling_Parameter.coupled.slaves[index] = parameter.indicator;
			}

			refresh_Reload_Coupled(refresh_Property, coupling_Widget);

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
