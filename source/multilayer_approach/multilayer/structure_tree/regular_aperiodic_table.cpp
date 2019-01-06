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
	//	connect(menu, &Menu::refresh, this, &Item_Editor::emit_Item_Data_Edited);
}

void Regular_Aperiodic_Table::create_Table()
{
	int num_Rows = item->childCount()*regular_Aperiodic_Data.num_Repetition.value()+1;
	regular_Table = new Regular_Aperiodic_Table_Widget(num_Rows,6,this);
	main_Layout->addWidget(regular_Table);

	int current_Row = 0;

	// labels
	create_Simple_Label(current_Row,0," Cell # ");
	create_Simple_Label(current_Row,1," Material ");
	create_Simple_Label(current_Row,2," z ["+length_units+"] ");
	create_Simple_Label(current_Row,3," Fit z ");
	create_Simple_Label(current_Row,4," "+Sigma_Sym+" ["+length_units+"] ");
	create_Simple_Label(current_Row,5," "+Rho_Sym+" ");
	current_Row++;

	for(int n=0; n<regular_Aperiodic_Data.num_Repetition.value(); ++n)
	{
		for(int i=0; i<regular_Aperiodic_Data.regular_Components.size(); ++i)
		{
			int current_Column = 0;

			// cell number
			QLabel* cell_Label = new QLabel(QString::number(n));
				cell_Label->setProperty(fit_Column_Property, true);
				cell_Label->setAlignment(Qt::AlignCenter);
			regular_Table->setCellWidget(current_Row, current_Column, cell_Label);

			current_Column++;

			// material
			QLabel* material_Label = new QLabel(regular_Aperiodic_Data.regular_Components[i].components[n].material);
				material_Label->setProperty(fit_Column_Property, true);
				material_Label->setAlignment(Qt::AlignCenter);
			regular_Table->setCellWidget(current_Row, current_Column, material_Label);
			labels_List.append(material_Label);
			current_Column++;

			// thickness value
			MyDoubleSpinBox* thickness_Value_Spinbox = new MyDoubleSpinBox;
				thickness_Value_Spinbox->setDecimals(line_edit_thickness_precision);
				thickness_Value_Spinbox->setRange(0, MAX_DOUBLE);
				thickness_Value_Spinbox->setSingleStep(step_thickness);
				thickness_Value_Spinbox->setAccelerated(true);
				thickness_Value_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				thickness_Value_Spinbox->installEventFilter(this);
				thickness_Value_Spinbox->setValue(regular_Aperiodic_Data.regular_Components[i].components[n].thickness.value/length_Coefficients_Map.value(length_units));
				thickness_Value_Spinbox->setFixedWidth(APERIODIC_TABLE_THICKNESS_VALUE_WIDTH);
			regular_Table->setCellWidget(current_Row, current_Column, thickness_Value_Spinbox);
			spinboxes_List.append(thickness_Value_Spinbox);
			current_Column++;

			// thickness fit
			QCheckBox* thickness_Fit_CheckBox = new QCheckBox;
			QWidget* back_Widget = new QWidget;
			QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
				back_Layout->addWidget(thickness_Fit_CheckBox);
				back_Layout->setSpacing(0);
				back_Layout->setContentsMargins(0,0,0,0);
				back_Layout->setAlignment(Qt::AlignCenter);
			regular_Table->setCellWidget(current_Row, current_Column, back_Widget);
			checkboxes_List.append(thickness_Fit_CheckBox);
			current_Column++;

			// sigma value
			MyDoubleSpinBox* sigma_Value_Spinbox = new MyDoubleSpinBox;
				sigma_Value_Spinbox->setDecimals(line_edit_sigma_precision);
				sigma_Value_Spinbox->setRange(0, MAX_DOUBLE);
				sigma_Value_Spinbox->setSingleStep(step_sigma);
				sigma_Value_Spinbox->setAccelerated(true);
				sigma_Value_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				sigma_Value_Spinbox->installEventFilter(this);
				sigma_Value_Spinbox->setValue(regular_Aperiodic_Data.regular_Components[i].components[n].sigma.value/length_Coefficients_Map.value(length_units));
				sigma_Value_Spinbox->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT_SIGMA);
			regular_Table->setCellWidget(current_Row, current_Column, sigma_Value_Spinbox);
			spinboxes_List.append(sigma_Value_Spinbox);
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
				if(regular_Aperiodic_Data.regular_Components[i].components[n].composed_Material)
				{
					density_Value_Spinbox->setSuffix("  [r.u.]");
					density_Value_Spinbox->setValue(regular_Aperiodic_Data.regular_Components[i].components[n].relative_Density.value);
				} else {
					density_Value_Spinbox->setSuffix("  ["+density_units+"]");
					density_Value_Spinbox->setValue(regular_Aperiodic_Data.regular_Components[i].components[n].absolute_Density.value);
				}
			regular_Table->setCellWidget(current_Row, current_Column, density_Value_Spinbox);
			spinboxes_List.append(density_Value_Spinbox);
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
}

void Regular_Aperiodic_Table::create_Simple_Label(int current_Row, int current_Column, QString text)
{
	QLabel* label = new QLabel(text);
		label->setAlignment(Qt::AlignCenter);
		label->setStyleSheet("background-color: lightblue");
		label->setProperty(fit_Column_Property, true);

	// add widget to table
	regular_Table->setCellWidget(current_Row, current_Column, label);
}
