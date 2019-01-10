#include "regular_aperiodic_table.h"

Regular_Aperiodic_Table::Regular_Aperiodic_Table(QTreeWidgetItem *item, QWidget *parent) :
	item(item),
	regular_Aperiodic_Data(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>()),
	QDialog(parent) // nullptr!
{
	global_Multilayer_Approach->runned_Regular_Aperiodic_Tables.insert(regular_Aperiodic_Data.id, this);

	setWindowTitle(Global_Variables::structure_Item_Name(regular_Aperiodic_Data));
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Regular_Aperiodic_Table::keyPressEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key_Shift)
	{
		modifier_Key_Still_Pressed = true;
		modifier_Key_Still_Pressed_Duplicate = true;
	}
}

void Regular_Aperiodic_Table::keyReleaseEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Shift)
	{
		  modifier_Key_Still_Pressed = false;
		  modifier_Key_Still_Pressed_Duplicate = false;
	}
}

bool Regular_Aperiodic_Table::eventFilter(QObject *obj, QEvent *event)
{
	UNUSED(obj);
	if(event->type() == QEvent::Wheel)
	{
		return !aperiodic_Mouse_Wheel_Spinbox_Table;
	}
	return false;
}

void Regular_Aperiodic_Table::closeEvent(QCloseEvent *event)
{
	write_Window_Geometry();
	global_Multilayer_Approach->runned_Regular_Aperiodic_Tables.remove(regular_Aperiodic_Data.id);
	event->accept();
}

void Regular_Aperiodic_Table::set_Window_Geometry()
{
	setGeometry(regular_aperiodic_table_x_corner,regular_aperiodic_table_y_corner,regular_aperiodic_table_width,regular_aperiodic_table_height);
}

void Regular_Aperiodic_Table::write_Window_Geometry()
{
	if(!isMaximized())
	{
		regular_aperiodic_table_x_corner = geometry().x()-WINDOW_BOUNDARY_SHIFT_X;
		regular_aperiodic_table_y_corner = geometry().y()-WINDOW_BOUNDARY_SHIFT_Y;

		regular_aperiodic_table_width  = geometry().width();
		regular_aperiodic_table_height = geometry().height();
	}
}

void Regular_Aperiodic_Table::emit_Regular_Aperiodic_Edited()
{
	emit regular_Aperiodic_Edited();
}

void Regular_Aperiodic_Table::save_And_Emit()
{
	QVariant var;
	var.setValue( regular_Aperiodic_Data );
	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	emit_Regular_Aperiodic_Edited();
}

void Regular_Aperiodic_Table::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(3,0,3,0);

	create_Menu();
	Global_Variables::create_Shortcuts(this);

	create_Table();

	QPushButton* done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		done_Button->setFocus();
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button, &QPushButton::clicked, this, &Regular_Aperiodic_Table::close);
}

void Regular_Aperiodic_Table::create_Menu()
{
	Menu* menu = new Menu(window_Type_Regular_Aperiodic_Table,this);
	main_Layout->setMenuBar(menu->menu_Bar);
	connect(menu, &Menu::refresh, global_Multilayer_Approach, [=]
	{
		reload_All_Widgets();
		global_Multilayer_Approach->refresh_All_Multilayers_View();
		if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Key))
		{
			global_Multilayer_Approach->table_Of_Structures->reload_All_Widgets();
		}
	});
}

void Regular_Aperiodic_Table::create_Table()
{
	int num_Rows = item->childCount()*regular_Aperiodic_Data.num_Repetition.value()+3;
	regular_Table = new Regular_Aperiodic_Table_Widget(num_Rows,6,this);
	main_Layout->addWidget(regular_Table);

	int current_Row = 0;

	// controls
	/// recalculate
	QCheckBox* checkbox_Recalculate = new QCheckBox("Recalculate");
		checkbox_Recalculate->setChecked(aperiodic_Recalculate_Spinbox_Table);
		checkbox_Recalculate->setProperty(fit_Column_Property, true);
	regular_Table->setCellWidget(current_Row, 0, checkbox_Recalculate);
	connect(checkbox_Recalculate, &QCheckBox::toggled, this, [=]
	{
		aperiodic_Recalculate_Spinbox_Table = checkbox_Recalculate->isChecked();
		if(aperiodic_Recalculate_Spinbox_Table)
			checkbox_Recalculate->setStyleSheet("QWidget { background: rgb(245, 228, 158); }");
		else
			checkbox_Recalculate->setStyleSheet("background-color: white");
	});
	checkbox_Recalculate->toggled(aperiodic_Recalculate_Spinbox_Table);

	/// thickness step label
	create_Simple_Label(current_Row,2," z step ");
	/// sigma step label
	create_Simple_Label(current_Row,4, Sigma_Sym+" step ");
	current_Row++;

	/// thickness step spinbox
	create_Step_Spin_Box(current_Row, 2, whats_This_Thickness);
	/// thickness step spinbox
	create_Step_Spin_Box(current_Row, 4, whats_This_Sigma);

	/// mouse wheel
	QCheckBox* checkbox_Mouse_Wheel = new QCheckBox("Mouse Wheel");
		checkbox_Mouse_Wheel->setChecked(aperiodic_Mouse_Wheel_Spinbox_Table);
		checkbox_Mouse_Wheel->setProperty(fit_Column_Property, true);
	regular_Table->setCellWidget(current_Row, 0, checkbox_Mouse_Wheel);
	connect(checkbox_Mouse_Wheel, &QCheckBox::toggled, this, [=]
	{
		aperiodic_Mouse_Wheel_Spinbox_Table = checkbox_Mouse_Wheel->isChecked();
		if(aperiodic_Mouse_Wheel_Spinbox_Table)
			checkbox_Mouse_Wheel->setStyleSheet("QWidget { background: rgb(255, 170, 137); }");
		else
			checkbox_Mouse_Wheel->setStyleSheet("background-color: white");
	});
	checkbox_Mouse_Wheel->toggled(aperiodic_Mouse_Wheel_Spinbox_Table);
	current_Row++;

	// labels
	create_Simple_Label(current_Row,0," Cell # ");
	create_Simple_Label(current_Row,1," Material ");
	QLabel* thickness_Label = nullptr;
	create_Simple_Label(current_Row,2," z ["+length_units+"] ", thickness_Label);
	labels_List.append(thickness_Label);
	create_Simple_Label(current_Row,3," Fit z ");
	QLabel* sigma_Label = nullptr;
	create_Simple_Label(current_Row,4," "+Sigma_Sym+" ["+length_units+"] ", sigma_Label);
	labels_List.append(sigma_Label);
	create_Simple_Label(current_Row,5," "+Rho_Sym+" ");
	current_Row++;

	for(int n=0; n<regular_Aperiodic_Data.num_Repetition.value(); ++n)
	{
		for(int i=0; i<regular_Aperiodic_Data.regular_Components.size(); ++i)
		{
			int current_Column = 0;

			// cell number
			QLabel* cell_Label = new QLabel(QString::number(n+1));
				cell_Label->setProperty(fit_Column_Property, true);
				cell_Label->setAlignment(Qt::AlignCenter);
			regular_Table->setCellWidget(current_Row, current_Column, cell_Label);

			current_Column++;

			// material
			QLabel* material_Label = new QLabel(regular_Aperiodic_Data.regular_Components[i].components[n].material);
				material_Label->setProperty(fit_Column_Property, true);
				material_Label->setAlignment(Qt::AlignCenter);
				material_Label->setMinimumWidth(55);
			regular_Table->setCellWidget(current_Row, current_Column, material_Label);
			labels_List.append(material_Label);
			current_Column++;

			// thickness value
			MyDoubleSpinBox* thickness_Value_Spinbox = new MyDoubleSpinBox;
				thickness_Value_Spinbox->setDecimals(line_edit_thickness_precision);
				thickness_Value_Spinbox->setRange(0, MAX_DOUBLE);
				thickness_Value_Spinbox->setSingleStep(step_thickness_aperiodic);
				thickness_Value_Spinbox->setAccelerated(true);
				thickness_Value_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				thickness_Value_Spinbox->installEventFilter(this);
				thickness_Value_Spinbox->setValue(regular_Aperiodic_Data.regular_Components[i].components[n].thickness.value/length_Coefficients_Map.value(length_units));
				thickness_Value_Spinbox->setFixedWidth(APERIODIC_TABLE_THICKNESS_VALUE_WIDTH);
				thickness_Value_Spinbox->setProperty(whats_This_Property, whats_This_Thickness);
			regular_Table->setCellWidget(current_Row, current_Column, thickness_Value_Spinbox);
			thickness_Spinboxes_List.append(thickness_Value_Spinbox);
			connect(thickness_Value_Spinbox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				refresh_Regular_Component(regular_Aperiodic_Data.regular_Components[i].components[n], i);
			});
			current_Column++;

			// thickness fit
			QCheckBox* thickness_Fit_CheckBox = new QCheckBox;
				thickness_Fit_CheckBox->setProperty(index_Property, i);
				thickness_Fit_CheckBox->setChecked(regular_Aperiodic_Data.regular_Components[i].components[n].thickness.fit.is_Fitable);
			QWidget* back_Widget = new QWidget;
			QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
				back_Layout->addWidget(thickness_Fit_CheckBox);
				back_Layout->setSpacing(0);
				back_Layout->setContentsMargins(0,0,0,0);
				back_Layout->setAlignment(Qt::AlignCenter);
			regular_Table->setCellWidget(current_Row, current_Column, back_Widget);
			thickness_Checkboxes_List.append(thickness_Fit_CheckBox);
			connect(thickness_Fit_CheckBox, &QCheckBox::toggled, this, [=]
			{
				if(modifier_Key_Still_Pressed)
				{
					// for everybody
					modifier_Key_Still_Pressed = false;
					for(QCheckBox* checkBox : thickness_Checkboxes_List)
					{
						if(checkBox->property(index_Property).toInt() == i)
						{
							checkBox->setChecked(thickness_Fit_CheckBox->isChecked());
						}
						if(checkBox == thickness_Fit_CheckBox)
						{
							thickness_Fit_CheckBox->toggled(thickness_Fit_CheckBox->isChecked());
						}
					}
					modifier_Key_Still_Pressed = modifier_Key_Still_Pressed_Duplicate;

					save_And_Emit();
				} else
				{
					regular_Aperiodic_Data.regular_Components[i].components[n].thickness.fit.is_Fitable = thickness_Fit_CheckBox->isChecked();

					if(thickness_Fit_CheckBox->isChecked())
						back_Widget->setStyleSheet(fit_Color);
					else
						back_Widget->setStyleSheet(white_Color);
				}

				if(!modifier_Key_Still_Pressed_Duplicate)
				{
					save_And_Emit();
				}
			});
			thickness_Fit_CheckBox->toggled(thickness_Fit_CheckBox->isChecked());
			current_Column++;

			// sigma value
			MyDoubleSpinBox* sigma_Value_Spinbox = new MyDoubleSpinBox;
				sigma_Value_Spinbox->setDecimals(line_edit_sigma_precision);
				sigma_Value_Spinbox->setRange(0, MAX_DOUBLE);
				sigma_Value_Spinbox->setSingleStep(step_sigma_aperiodic);
				sigma_Value_Spinbox->setAccelerated(true);
				sigma_Value_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				sigma_Value_Spinbox->installEventFilter(this);
				sigma_Value_Spinbox->setValue(regular_Aperiodic_Data.regular_Components[i].components[n].sigma.value/length_Coefficients_Map.value(length_units));
				sigma_Value_Spinbox->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SIGMA);
				sigma_Value_Spinbox->setProperty(whats_This_Property, whats_This_Sigma);
			regular_Table->setCellWidget(current_Row, current_Column, sigma_Value_Spinbox);
			sigma_Spinboxes_List.append(sigma_Value_Spinbox);
			connect(sigma_Value_Spinbox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				refresh_Regular_Component(regular_Aperiodic_Data.regular_Components[i].components[n], i);
			});
			current_Column++;

			// density value
			MyDoubleSpinBox* density_Value_Spinbox = new MyDoubleSpinBox;
				density_Value_Spinbox->setDecimals(line_edit_density_precision);
				density_Value_Spinbox->setReadOnly(true);
				density_Value_Spinbox->setRange(0, MAX_DOUBLE);
				density_Value_Spinbox->setSingleStep(step_density);
				density_Value_Spinbox->setAccelerated(true);
				density_Value_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				density_Value_Spinbox->installEventFilter(this);
				density_Value_Spinbox->setFixedWidth(APERIODIC_TABLE_DENSITY_VALUE_WIDTH);
			if(!regular_Aperiodic_Data.regular_Components[i].components[n].composed_Material)
			{
				density_Value_Spinbox->setSuffix("  [r.u.]");
				density_Value_Spinbox->setValue(regular_Aperiodic_Data.regular_Components[i].components[n].relative_Density.value);
			} else {
				density_Value_Spinbox->setSuffix("  ["+density_units+"]");
				density_Value_Spinbox->setValue(regular_Aperiodic_Data.regular_Components[i].components[n].absolute_Density.value);
			}
			regular_Table->setCellWidget(current_Row, current_Column, density_Value_Spinbox);
			density_Spinboxes_List.append(density_Value_Spinbox);
			current_Column++;

			current_Row++;
		}
	}

	Table_Of_Structures::fit_Column(regular_Table, 0, 0);
	Table_Of_Structures::fit_Column(regular_Table, 0, 1);
	Table_Of_Structures::fit_Column(regular_Table, 0, 2);
	Table_Of_Structures::fit_Column(regular_Table, 0, 3);
	Table_Of_Structures::fit_Column(regular_Table, 0, 4);
	Table_Of_Structures::fit_Column(regular_Table, 0, 5);

	int width = 0;
	for(int i=0; i<2; i++) {width += regular_Table->columnWidth(i);}
	checkbox_Recalculate->setMinimumWidth(width);
	checkbox_Mouse_Wheel->setMinimumWidth(width);
}

void Regular_Aperiodic_Table::create_Simple_Label(int current_Row, int current_Column, QString text, QLabel* label)
{
	label = new QLabel(text);
		label->setAlignment(Qt::AlignCenter);
		label->setStyleSheet("background-color: lightblue");
		label->setProperty(fit_Column_Property, true);

	// add widget to table
		regular_Table->setCellWidget(current_Row, current_Column, label);
}

void Regular_Aperiodic_Table::create_Step_Spin_Box(int current_Row, int current_Column, QString whats_This)
{
	MyDoubleSpinBox* step_SpinBox = new MyDoubleSpinBox;
		step_SpinBox->setDecimals(4);
		step_SpinBox->setRange(0, MAX_DOUBLE);
		step_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		step_SpinBox->setAccelerated(true);
		step_SpinBox->setProperty(column_Property,current_Column);
		step_SpinBox->installEventFilter(this);

	step_Spinboxes_List.append(step_SpinBox);
	regular_Table->setCellWidget(current_Row, current_Column, step_SpinBox);
		if(whats_This == whats_This_Thickness)	step_SpinBox->setFixedWidth(APERIODIC_TABLE_THICKNESS_VALUE_WIDTH);
		if(whats_This == whats_This_Sigma)		step_SpinBox->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SIGMA);

	connect(step_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		double length_Coeff = length_Coefficients_Map.value(length_units);

		if(step_SpinBox->property(reload_Property).toBool())
		{
			int add_Decimals = min(log10(length_Coeff),2.);
			double min_Step = max(0.1/length_Coeff,0.0001);

			if(whats_This == whats_This_Thickness)		{ step_SpinBox->setValue(step_thickness_aperiodic/length_Coeff); }
			if(whats_This == whats_This_Sigma)			{ step_SpinBox->setValue(step_sigma_aperiodic/length_Coeff);	}

			step_SpinBox->setDecimals(2+add_Decimals);
			step_SpinBox->setSingleStep(min_Step);
			step_SpinBox->setSuffix(" "+length_units);
		}

		if(whats_This == whats_This_Thickness)	{ step_thickness_aperiodic = step_SpinBox->value()*length_Coeff;  for(MyDoubleSpinBox* spb : thickness_Spinboxes_List)	spb->setSingleStep(step_thickness_aperiodic/length_Coeff);}
		if(whats_This == whats_This_Sigma)		{ step_sigma_aperiodic = step_SpinBox->value()*length_Coeff;      for(MyDoubleSpinBox* spb : sigma_Spinboxes_List)		spb->setSingleStep(step_sigma_aperiodic/length_Coeff);	}
	});

	// initialize
	step_SpinBox->setProperty(reload_Property, true);
	step_SpinBox->valueChanged(step_SpinBox->value());
	step_SpinBox->setProperty(reload_Property, false);
	connect(qApp, &QApplication::focusChanged, this, [=](QWidget* old, QWidget* now)
	{
		if(now)
		{
			if(now->topLevelWidget()==this)
			{
				if(!old)                             step_SpinBox->valueChanged(step_SpinBox->value());
				else if(old->topLevelWidget()!=this) step_SpinBox->valueChanged(step_SpinBox->value());
			}
		}
	});
}

void Regular_Aperiodic_Table::refresh_Regular_Component(Data& current_Layer, int i)
{
	// PARAMETER
	MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(QObject::sender());
	QString whats_This = spin_Box->property(whats_This_Property).toString();

	int precision = 4;
	double coeff = 1;

	// for reloading
	bool reload = spin_Box->property(reload_Property).toBool();

	Parameter& parameter = Table_Of_Structures::get_Parameter(current_Layer, whats_This, precision, coeff);

	if(reload)
	{
		spin_Box->blockSignals(true);
		spin_Box->setDecimals(precision);
		spin_Box->setValue(parameter.value/coeff);
		spin_Box->blockSignals(false);

		// special cases
		if(whats_This == whats_This_Sigma)
		{
			for(Interlayer& interlayer : current_Layer.interlayer_Composition)	{
				interlayer.my_Sigma.value = current_Layer.sigma.value;
			}
		}
		return;
	}
	// if refresh
	{
		parameter.value = spin_Box->value()*coeff;

		// special cases
		if(whats_This == whats_This_Sigma)
		{
			for(Interlayer& interlayer : current_Layer.interlayer_Composition)	{
				interlayer.my_Sigma.value = current_Layer.sigma.value;
			}
		}
		regular_Aperiodic_Data.regular_Components[i].find_Min_Max_Values();
	}
	save_And_Emit();

	// recalculation at change
	if(aperiodic_Recalculate_Spinbox_Table && !reload) {global_Multilayer_Approach->calc_Reflection(true);}
}

void Regular_Aperiodic_Table::reload_One_Widget(QWidget* widget_To_Reload)
{
	widget_To_Reload->setProperty(reload_Property, true);

	QLabel*				label = qobject_cast<QLabel*>		  (widget_To_Reload);
	QCheckBox*		check_Box = qobject_cast<QCheckBox*>	  (widget_To_Reload);
	MyDoubleSpinBox* spin_Box = qobject_cast<MyDoubleSpinBox*>(widget_To_Reload);

	if(label)
	{
		label->windowTitleChanged(label->text());
	}
	if(check_Box)
	{
		check_Box->toggled(check_Box->isChecked());
	}
	if(spin_Box)
	{
		spin_Box->valueChanged(spin_Box->value());
	}

	widget_To_Reload->setProperty(reload_Property, false);
}

void Regular_Aperiodic_Table::reload_All_Widgets()
{
	QWidget* sender = qobject_cast<QWidget*>(QObject::sender());
	reload_Labels(sender);
	reload_Thicknesses(sender);
	reload_Sigmas(sender);
	reload_Densities(sender);
	reload_Steps(sender);
	reload_Checkboxes(sender);
}

void Regular_Aperiodic_Table::reload_Labels(QWidget* sender)
{
	for(QWidget* widget_To_Reload : labels_List)
	{
		if(widget_To_Reload != sender)
		{
			reload_One_Widget(widget_To_Reload);
		}
	}
}

void Regular_Aperiodic_Table::reload_Thicknesses(QWidget* sender)
{
	for(QWidget* widget_To_Reload : thickness_Spinboxes_List)
	{
		if(widget_To_Reload != sender)
		{
			reload_One_Widget(widget_To_Reload);
		}
	}
}

void Regular_Aperiodic_Table::reload_Sigmas(QWidget* sender)
{
	for(QWidget* widget_To_Reload : sigma_Spinboxes_List)
	{
		if(widget_To_Reload != sender)
		{
			reload_One_Widget(widget_To_Reload);
		}
	}
}

void Regular_Aperiodic_Table::reload_Densities(QWidget* sender)
{
	for(QWidget* widget_To_Reload : density_Spinboxes_List)
	{
		if(widget_To_Reload != sender)
		{
			reload_One_Widget(widget_To_Reload);
		}
	}
}

void Regular_Aperiodic_Table::reload_Steps(QWidget *sender)
{
	for(QWidget* widget_To_Reload : step_Spinboxes_List)
	{
		if(widget_To_Reload != sender)
		{
			reload_One_Widget(widget_To_Reload);
		}
	}
}

void Regular_Aperiodic_Table::reload_Checkboxes(QWidget* sender)
{
	for(QWidget* widget_To_Reload : thickness_Checkboxes_List)
	{
		if(widget_To_Reload != sender)
		{
			reload_One_Widget(widget_To_Reload);
		}
	}
}
