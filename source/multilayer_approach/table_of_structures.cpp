#include "table_of_structures.h"
#include "algorithm"

Table_Of_Structures::Table_Of_Structures(QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures, QTabWidget* multilayer_Tabs, QWidget *parent) :
	runned_Tables_Of_Structures(runned_Tables_Of_Structures),
	multilayer_Tabs(multilayer_Tabs),
	QWidget(parent)
{
	setWindowTitle("Table Of Structures");
	create_Main_Layout();
	read_Trees();

	setAttribute(Qt::WA_DeleteOnClose);
}

void Table_Of_Structures::closeEvent(QCloseEvent* event)
{
	event;
	runned_Tables_Of_Structures->remove(table_Key);

	for(int i=0; i<list_Of_Trees.size(); ++i)
	{
		list_Of_Trees[i]->structure_Toolbar->toolbar->setDisabled(false);
	}
}

void Table_Of_Structures::emit_Data_Edited()
{
	emit data_Edited();	
}

void Table_Of_Structures::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
		main_Layout->setContentsMargins(0,0,0,0);
	create_Table();
	main_Layout->addWidget(main_Table);
	resize(1450,800);
}

void Table_Of_Structures::create_Table()
{
	// PARAMETER

	read_Trees();
	main_Table = new QTableWidget(basic_Row_Number, basic_Column_Number, this);
	rows_List_To_Span.clear();

	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		// header for each structure
		main_Table->insertRow(main_Table->rowCount());
		int current_Row = main_Table->rowCount()-1;
		rows_List_To_Span.append(current_Row);
		main_Table->setItem(current_Row,0, new QTableWidgetItem(multilayer_Tabs->tabText(tab_Index)));
		main_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);
		main_Table->item   (current_Row,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

		// structure display
		int max_Depth = Global_Variables::get_Tree_Depth(list_Of_Trees[tab_Index]->tree->invisibleRootItem());
		int depth, current_Column;

		int max_Number_Of_Elements=1;
		{
			QTreeWidgetItem* structure_Item;
			QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);
			while (*it)
			{
				structure_Item=*it;
				QList<Stoichiometry> composition;
				QStringList whats_This_List = structure_Item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);
				item_Type_String = whats_This_List[0];
				if(item_Type_String == whats_This_Ambient)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().composition;	}
				if(item_Type_String == whats_This_Layer)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().composition;	}
				if(item_Type_String == whats_This_Substrate)	{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composition;}

				if(max_Number_Of_Elements<composition.size()) max_Number_Of_Elements=composition.size();
				++it;
			}
		}

		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);
		while (*it)
		{
			structure_Item = *it;
			depth = Global_Variables::get_Item_Depth(structure_Item);

			current_Column = depth-1;
			main_Table->insertRow(main_Table->rowCount());
			main_Table->insertRow(main_Table->rowCount());
			main_Table->insertRow(main_Table->rowCount());
			main_Table->insertRow(main_Table->rowCount());

			current_Row = main_Table->rowCount()-3;
			main_Table->insertRow(main_Table->rowCount());
			main_Table->insertRow(main_Table->rowCount());

			add_Columns(depth);

			// print whatsThis
			main_Table->setItem(current_Row,current_Column, new QTableWidgetItem(structure_Item->whatsThis(DEFAULT_COLUMN)));
			main_Table->item(current_Row,current_Column)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

			// common variables
			bool composed_Material;

			// load data
			QStringList whats_This_List = structure_Item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);
			item_Type_String = whats_This_List[0];
			if(item_Type_String == whats_This_Ambient)		{composed_Material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().composed_Material;	}
			if(item_Type_String == whats_This_Layer)		{composed_Material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().composed_Material;	}
			if(item_Type_String == whats_This_Multilayer)	{		}
			if(item_Type_String == whats_This_Substrate)	{composed_Material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composed_Material;}

			current_Column = max_Depth+1;

			///--------------------------------------------------------------------------------------------
			/// creation
			///--------------------------------------------------------------------------------------------

			// material
			if(item_Type_String == whats_This_Ambient || item_Type_String == whats_This_Layer || item_Type_String == whats_This_Substrate)
			if(composed_Material)
			{
				create_Combo_Elements(current_Row,   current_Column, structure_Item, item_Type_String);
				create_Stoich		 (current_Row+1, current_Column, structure_Item, item_Type_String, VAL);
				create_Stoich		 (current_Row+3, current_Column, structure_Item, item_Type_String, MIN);
				create_Stoich		 (current_Row+4, current_Column, structure_Item, item_Type_String, MAX);

				// it should be created last
				create_Stoich_Check_Box_Fit (current_Row+2, current_Column, structure_Item, item_Type_String, 1, 2, 0, 0);
			}
			else
			{
				create_Material      (current_Row,   current_Column, structure_Item, item_Type_String);
				create_Browse_Button (current_Row+1, current_Column);
			}
			current_Column += (max_Number_Of_Elements+1);
			///--------------------------------------------------------------------------------------------

			// density
			if(item_Type_String == whats_This_Ambient || item_Type_String == whats_This_Layer || item_Type_String == whats_This_Substrate)
			{
				QString whats_This;
				add_Columns(current_Column+1);
				if(composed_Material)
				{
					whats_This = whats_This_Absolute_Density;
					create_Label(current_Row, current_Column, Rho_Sym+" ["+density_units+"]");
				} else
				{
					whats_This = whats_This_Relative_Density;
					create_Label(current_Row, current_Column, Rho_Sym+" [r.u.]");
				}
				create_Line_Edit(current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				create_Line_Edit(current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit(current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(current_Row+2, current_Column, structure_Item, item_Type_String, whats_This, 1, 2, 0, 0);
			}
			current_Column += 2;
			///--------------------------------------------------------------------------------------------

			// thickness
			if(item_Type_String == whats_This_Layer)
			{
				QString whats_This = whats_This_Thickness;
				add_Columns(current_Column+1);
				create_Label(current_Row, current_Column, "z ["+Angstrom_Sym+"]");
				create_Line_Edit(current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				create_Line_Edit(current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit(current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(current_Row+2, current_Column, structure_Item, item_Type_String, whats_This, 1, 2, 0, 0);
			}
			current_Column += 2;
			///--------------------------------------------------------------------------------------------

			// common sigma
			if(item_Type_String == whats_This_Layer || item_Type_String == whats_This_Substrate)
			{
				QString whats_This = whats_This_Sigma;
				add_Columns(current_Column+1);
				create_Label(current_Row, current_Column, Sigma_Sym+" ["+Angstrom_Sym+"]");
				create_Line_Edit	(current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				create_Line_Edit	(current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(current_Row+2, current_Column, structure_Item, item_Type_String, whats_This, 1, 2, 0, 0);
			}
			current_Column += 2;
			///--------------------------------------------------------------------------------------------

			// interlayers: weights and sigmas
			if(item_Type_String == whats_This_Layer || item_Type_String == whats_This_Substrate)
			{
				add_Columns(current_Column+transition_Layer_Functions_Size);

				// weights
				create_Weigts_Interlayer(current_Row+1, current_Column, structure_Item, item_Type_String, VAL);
				create_Check_Box_Label_Interlayer(current_Row, current_Column, structure_Item, item_Type_String);
				create_Weights_Check_Box_Fit_Interlayer(current_Row+2, current_Column, structure_Item, item_Type_String);

				// sigmas
				create_MySigma_Labels_Interlayer(current_Row+3, current_Column, structure_Item, item_Type_String);
				create_MySigma_Interlayer		(current_Row+4, current_Column, structure_Item, item_Type_String);
			}
			current_Column += (1+transition_Layer_Functions_Size);
			///--------------------------------------------------------------------------------------------



			// thickness linear drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				QString whats_This = whats_This_Thickness_Drift_Line_Value;
				add_Columns(current_Column+1);
				create_Line_Edit(current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				// second
				create_Check_Box_Label(current_Row, current_Column, structure_Item, item_Type_String, whats_This,"dz lin", 1, 1, 0, 0);
				create_Line_Edit	(current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(current_Row+2, current_Column, structure_Item, item_Type_String, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// thickness random drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				QString whats_This = whats_This_Thickness_Drift_Rand_Rms;
				add_Columns(current_Column+1);
				create_Line_Edit(current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				// second
				create_Check_Box_Label(current_Row, current_Column, structure_Item, item_Type_String, whats_This,"dz ran", 1, 1, 0, 0);
				create_Line_Edit	(current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(current_Row+2, current_Column, structure_Item, item_Type_String, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// thickness sine drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				add_Columns(current_Column+3);

				create_Line_Edit	(current_Row+1, current_Column,   structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Amplitude, VAL);
				create_Line_Edit	(current_Row+1, current_Column+1, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Frequency, VAL);
				create_Line_Edit	(current_Row+1, current_Column+2, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Phase, VAL);

				// second
				create_Check_Box_Label(current_Row, current_Column,   structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine,"dz sin : A*sin(2"+Pi_Sym+"n"+Nu_Sym+"+"+Phi_Sym+")", 1, 1, 0, 2);

				create_Line_Edit	(current_Row+3, current_Column,   structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Amplitude, MIN);
				create_Line_Edit	(current_Row+3, current_Column+1, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Frequency, MIN);
				create_Line_Edit	(current_Row+3, current_Column+2, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Phase, MIN);

				create_Line_Edit	(current_Row+4, current_Column,	  structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Amplitude, MAX);
				create_Line_Edit	(current_Row+4, current_Column+1, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Frequency, MAX);
				create_Line_Edit	(current_Row+4, current_Column+2, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Phase, MAX);

				// last
				create_Check_Box_Fit(current_Row+2, current_Column,	  structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Amplitude, 1, 2, 0, 0);
				create_Check_Box_Fit(current_Row+2, current_Column+1, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Frequency, 1, 2, 0, 0);
				create_Check_Box_Fit(current_Row+2, current_Column+2, structure_Item, item_Type_String, whats_This_Thickness_Drift_Sine_Phase, 1, 2, 0, 0);

				main_Table->setSpan(current_Row,current_Column,1,3);
			}
			current_Column += 4;
			///--------------------------------------------------------------------------------------------



			// sigma linear drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				QString whats_This = whats_This_Sigma_Drift_Line_Value;
				add_Columns(current_Column+1);
				create_Line_Edit(current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				// second
				create_Check_Box_Label(current_Row, current_Column, structure_Item, item_Type_String, whats_This, "d"+Sigma_Sym+" lin", 1, 1, 0, 0);
				create_Line_Edit	(current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(current_Row+2, current_Column, structure_Item, item_Type_String, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// sigma random drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				QString whats_This = whats_This_Sigma_Drift_Rand_Rms;
				add_Columns(current_Column+1);
				create_Line_Edit(current_Row+1, current_Column, structure_Item, item_Type_String, whats_This, VAL);
				// second
				create_Check_Box_Label(current_Row, current_Column, structure_Item, item_Type_String, whats_This,"d"+Sigma_Sym+" ran", 1, 1, 0, 0);
				create_Line_Edit	(current_Row+3, current_Column, structure_Item, item_Type_String, whats_This, MIN);
				create_Line_Edit	(current_Row+4, current_Column, structure_Item, item_Type_String, whats_This, MAX);
				// last
				create_Check_Box_Fit(current_Row+2, current_Column, structure_Item, item_Type_String, whats_This, 1, 2, 0, 0);
			}
			current_Column += 1;
			///--------------------------------------------------------------------------------------------

			// sigma sine drift
			if(item_Type_String == whats_This_Layer && depth >=2)
			{
				add_Columns(current_Column+3);

				create_Line_Edit	(current_Row+1, current_Column,   structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Amplitude, VAL);
				create_Line_Edit	(current_Row+1, current_Column+1, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Frequency, VAL);
				create_Line_Edit	(current_Row+1, current_Column+2, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Phase, VAL);

				// second
				create_Check_Box_Label(current_Row, current_Column,   structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine,"d"+Sigma_Sym+" sin : A*sin(2"+Pi_Sym+"n"+Nu_Sym+"+"+Phi_Sym+")", 1, 1, 0, 2);

				create_Line_Edit	(current_Row+3, current_Column,   structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Amplitude, MIN);
				create_Line_Edit	(current_Row+3, current_Column+1, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Frequency, MIN);
				create_Line_Edit	(current_Row+3, current_Column+2, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Phase, MIN);

				create_Line_Edit	(current_Row+4, current_Column,	  structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Amplitude, MAX);
				create_Line_Edit	(current_Row+4, current_Column+1, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Frequency, MAX);
				create_Line_Edit	(current_Row+4, current_Column+2, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Phase, MAX);

				// last
				create_Check_Box_Fit(current_Row+2, current_Column,	  structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Amplitude, 1, 2, 0, 0);
				create_Check_Box_Fit(current_Row+2, current_Column+1, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Frequency, 1, 2, 0, 0);
				create_Check_Box_Fit(current_Row+2, current_Column+2, structure_Item, item_Type_String, whats_This_Sigma_Drift_Sine_Phase, 1, 2, 0, 0);

				main_Table->setSpan(current_Row,current_Column,1,3);
			}
			current_Column += 4;
			///--------------------------------------------------------------------------------------------

			++it;
		}
		main_Table->insertRow(main_Table->rowCount());
	}
	span_Structure_Headers();
	span_Structure_Items();
	main_Table->resizeColumnsToContents();
	main_Table->resizeRowsToContents();

	table_Is_Created = true;
	reload_All_Widgets();

	// fit initial size of columns
	for(int i=0; i<main_Table->columnCount(); ++i)
		fit_Column(main_Table->columnWidth(i), i);
}

void Table_Of_Structures::read_Trees()
{
	for(int i=0; i<multilayer_Tabs->count(); ++i)
	{
		Structure_Tree* old_Structure_Tree = dynamic_cast<Multilayer*>(multilayer_Tabs->widget(i))->structure_Tree;
		list_Of_Trees.append(old_Structure_Tree);
		old_Structure_Tree->structure_Toolbar->toolbar->setDisabled(true);
	}
}

void Table_Of_Structures::add_Columns(int add_After)
{
	while(add_After>=main_Table->columnCount())
	{
		main_Table->insertColumn(main_Table->columnCount());
	}
}

void Table_Of_Structures::span_Structure_Headers()
{
	for(int struct_Index=0; struct_Index<rows_List_To_Span.size(); ++struct_Index)
	{
		main_Table->setSpan(rows_List_To_Span[struct_Index],0,1,main_Table->columnCount());
	}
}

void Table_Of_Structures::span_Structure_Items()
{
	for(int row_Index=0; row_Index<main_Table->rowCount(); ++row_Index)
	{
		for(int col_Index=0; col_Index<main_Table->columnCount(); ++col_Index)
		{
			if(main_Table->item(row_Index,col_Index))
			{
				QStringList wtf_List = main_Table->item(row_Index,col_Index)->text().split(item_Type_Delimiter,QString::SkipEmptyParts);
				if(	   wtf_List[0] == whats_This_Ambient
					|| wtf_List[0] == whats_This_Layer
					|| wtf_List[0] == whats_This_Multilayer
					|| wtf_List[0] == whats_This_Substrate )
				{
					main_Table->setSpan(row_Index,col_Index,5,1);
				}
			}
		}
	}
}

void Table_Of_Structures::fit_Column(int start_Width, int current_Column)
{
	int max_Width=start_Width;
	for(int row=0; row<main_Table->rowCount(); ++row)
	{
		QLineEdit* current_Line_Edit = qobject_cast<QLineEdit*>(main_Table->cellWidget(row, current_Column));
		if(current_Line_Edit)
		{
			if(max_Width<current_Line_Edit->width())
			{
				max_Width=current_Line_Edit->width()+1;
			}
		}
	}
	main_Table->setColumnWidth(current_Column,max_Width);
}

void Table_Of_Structures::reload_All_Widgets(QObject* sender)
{
	if(table_Is_Created)
	{
		for(int i=0; i<all_Widgets_To_Reload.size(); ++i)
		{
			if(all_Widgets_To_Reload[i] != sender)
			{
				all_Widgets_To_Reload[i]->setProperty(reload_Property, true);

				QCheckBox* check_Box = qobject_cast<QCheckBox*>(all_Widgets_To_Reload[i]);
				QLineEdit* line_Edit = qobject_cast<QLineEdit*>(all_Widgets_To_Reload[i]);
				QComboBox* combo_Box = qobject_cast<QComboBox*>(all_Widgets_To_Reload[i]);

				if(check_Box)	check_Box->toggled(false);
				if(line_Edit)	line_Edit->textEdited("temp");
				if(combo_Box)	combo_Box->currentTextChanged("temp");

				all_Widgets_To_Reload[i]->setProperty(reload_Property, false);
			}
		}
	}
}

// creation

// for material only
void Table_Of_Structures::create_Combo_Elements(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QList<Stoichiometry> composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)		{composition = data.value<Ambient>().  composition;}
	if(item_Type_String == whats_This_Layer)		{composition = data.value<Layer>().	   composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = data.value<Substrate>().composition;}

	int current_Column = start_Column;
	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		add_Columns(current_Column+1);

		// create combobox
		QComboBox* elements = new QComboBox;
		elements->addItems(sorted_Elements.keys());
		elements->setCurrentIndex(elements->findText(composition[composition_Index].type));

		// number of element is here
		elements->setProperty(num_Chemic_Element_Property, composition_Index);
		elements->setProperty(item_Type_Property, item_Type_String);

		// for reloading
		elements->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(elements);

		// create item
		main_Table->setCellWidget(current_Row, current_Column, elements);
		elements_Map.insert(elements, structure_Item);
		connect(elements,  SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Element(QString)));

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_Stoich(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type)
{
	QList<Stoichiometry> composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)		{composition = data.value<Ambient>().	 composition;}
	if(item_Type_String == whats_This_Layer)		{composition = data.value<Layer>().	 composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = data.value<Substrate>().composition;}

	int current_Column = start_Column;
	double value;

	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		if(val_Type == VAL)	{value = composition[composition_Index].composition.value;	}
		if(val_Type == MIN)	{value = composition[composition_Index].composition.fit.min;}
		if(val_Type == MAX)	{value = composition[composition_Index].composition.fit.max;}

		QString text_Value = QString::number(value, line_edit_short_double_format, line_edit_composition_precision);

		// create lineedit
		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

		// number of element is here
		line_Edit->setProperty(num_Chemic_Element_Property, composition_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(item_Type_Property, item_Type_String);
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setProperty(value_Type_Property, val_Type);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		// for reloading
		line_Edit->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(line_Edit);

		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		// create item
		main_Table->setCellWidget(current_Row, current_Column, line_Edit);
		line_Edits_Map.insert(line_Edit, structure_Item);
		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Stoich(QString)));
		line_Edit->textEdited(line_Edit->text());
		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

QString Table_Of_Structures::material_From_Composition(QList<Stoichiometry>& composition)
{
	QString text;
	for(int i=0; i<composition.size(); ++i)
	{
		text += composition[i].type;
		if( abs(composition[i].composition.value - 1.) > DBL_EPSILON )
			text += QString::number(composition[i].composition.value, line_edit_short_double_format, thumbnail_composition_precision);
	}
	return text;
}

void Table_Of_Structures::create_Stoich_Check_Box_Fit(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, int r_S, int r_F, int c_S, int c_F)
{
	QList<Stoichiometry> composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)		{composition = data.value<Ambient>().  composition;}
	if(item_Type_String == whats_This_Layer)		{composition = data.value<Layer>().	   composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = data.value<Substrate>().composition;}

	int current_Column = start_Column;

	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		// create check_Box
		QCheckBox* check_Box = new QCheckBox("fit");//;

		// alignment
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		// number of element is here
		check_Box->setProperty(num_Chemic_Element_Property, composition_Index);
		check_Box->setProperty(item_Type_Property, item_Type_String);
		check_Box->setProperty(row_Property, current_Row);
		check_Box->setProperty(column_Property, current_Column);

		// which cells disable and enable
		check_Box->setProperty(relative_Rows_To_Disable_Start_Property, r_S);
		check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, r_F);
		check_Box->setProperty(relative_Columns_To_Disable_Start_Property, c_S);
		check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, c_F);

		// for reloading
		check_Box->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(check_Box);

		// create item
		main_Table->setCellWidget(current_Row, current_Column, back_Widget);
		check_Boxes_Map.insert(check_Box, structure_Item);
		connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(refresh_Fit_Element(bool)));
		connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(cells_On_Off(bool)));
		check_Box->setChecked(composition[composition_Index].composition.fit.is_Fitable);
		check_Box->toggled(false);

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;

		// disable if 1 element
		if(composition.size()==1)
		{
			check_Box->setProperty(relative_Rows_To_Disable_Start_Property, -1);
			check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, 2);
			check_Box->setProperty(relative_Columns_To_Disable_Start_Property, 0);
			check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, 0);

			check_Box->setChecked(false);
			check_Box->toggled(false);
		}
	}
}

void Table_Of_Structures::create_Material(int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QString material = "bad";
	QString approved_Material = "very bad";
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)		{		  material = data.value<Ambient>().  material;
													 approved_Material = data.value<Ambient>().  approved_Material;}
	if(item_Type_String == whats_This_Layer)		{		  material = data.value<Layer>().    material;
													 approved_Material = data.value<Layer>().    approved_Material;}
	if(item_Type_String == whats_This_Substrate)	{		  material = data.value<Substrate>().material;
													 approved_Material = data.value<Substrate>().approved_Material;}

	add_Columns(current_Column+1);

	QLineEdit* material_Line_Edit = new QLineEdit(material);
	material_Line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);
	material_Line_Edit->setProperty(min_Size_Property, material_Line_Edit->width());
	material_Line_Edit->setProperty(item_Type_Property, item_Type_String);
	material_Line_Edit->setProperty(column_Property, current_Column);

	// for reloading
	material_Line_Edit->setProperty(reload_Property, false);
	all_Widgets_To_Reload.append(material_Line_Edit);

	connect(material_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

	main_Table->setCellWidget(current_Row, current_Column, material_Line_Edit);
	line_Edits_Map.insert(material_Line_Edit, structure_Item);
	connect(material_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Material(QString)));
	connect(material_Line_Edit, SIGNAL(editingFinished()), this, SLOT(check_Material()));
	material_Line_Edit->textEdited(material_Line_Edit->text());
}

void Table_Of_Structures::create_Browse_Button(int current_Row, int start_Column)
{
	// TODO
	QPushButton* browse_Button = new QPushButton("Browse");
	browse_Button->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);
	main_Table->setCellWidget(current_Row, start_Column, browse_Button);
	connect(browse_Button, SIGNAL(clicked(bool)), this, SLOT(browse_Material(bool)));
}

// for several parameters
void Table_Of_Structures::create_Label(int current_Row, int current_Column, QString text)
{
	QLabel* label = new QLabel(text);
	label->setStyleSheet("background-color: lightblue");
	label->setAlignment(Qt::AlignCenter);
	main_Table->setCellWidget(current_Row, current_Column, label);
}

void Table_Of_Structures::create_Check_Box_Label(int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString text, int r_S, int r_F, int c_S, int c_F)
{
	// PARAMETER
	bool is_Drift;

	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)
	{
		if(whats_This == whats_This_Thickness_Drift_Line_Value)	is_Drift = data.value<Layer>().thickness_Drift.is_Drift_Line;
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)	is_Drift = data.value<Layer>().thickness_Drift.is_Drift_Rand;
		if(whats_This == whats_This_Thickness_Drift_Sine)		is_Drift = data.value<Layer>().thickness_Drift.is_Drift_Sine;

		if(whats_This == whats_This_Sigma_Drift_Line_Value)		is_Drift = data.value<Layer>().sigma_Drift.is_Drift_Line;
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)		is_Drift = data.value<Layer>().sigma_Drift.is_Drift_Rand;
		if(whats_This == whats_This_Sigma_Drift_Sine)			is_Drift = data.value<Layer>().sigma_Drift.is_Drift_Sine;

	}

	QCheckBox* check_Box = new QCheckBox(text);

	// enable/disable function
	check_Box->setProperty(row_Property, current_Row);
	check_Box->setProperty(column_Property, current_Column);
	check_Box->setProperty(relative_Rows_To_Disable_Start_Property, r_S);
	check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, r_F);
	check_Box->setProperty(relative_Columns_To_Disable_Start_Property, c_S);
	check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, c_F);

	check_Box->setProperty(item_Type_Property, item_Type_String);
	check_Box->setProperty(whats_This_Property, whats_This);
	check_Boxes_Map.insert(check_Box, structure_Item);

	// for reloading
	check_Box->setProperty(reload_Property, false);
	all_Widgets_To_Reload.append(check_Box);

	check_Box->setChecked(is_Drift);
	connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(cells_On_Off(bool)));
	connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(refresh_Drift_Header(bool)));
	check_Box->toggled(false);

	// alignment
	QWidget* back_Widget = new QWidget;
	QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
	back_Layout->addWidget(check_Box);
	back_Layout->setSpacing(0);
	back_Layout->setContentsMargins(0,0,0,0);
	back_Layout->setAlignment(Qt::AlignCenter);

	back_Widget->setStyleSheet("background-color: lightblue");
	main_Table->setCellWidget(current_Row, current_Column, back_Widget);
}

// for all parameters
void Table_Of_Structures::create_Line_Edit(int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString val_Type)
{
	// PARAMETER
	Parameter parameter;
	int precision;

	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)
	{
		if(whats_This == whats_This_Absolute_Density)	{parameter = data.value<Ambient>().absolute_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Relative_Density)	{parameter = data.value<Ambient>().relative_Density;		precision = line_edit_density_precision;}
	}
	if(item_Type_String == whats_This_Layer)
	{
		if(whats_This == whats_This_Absolute_Density)				{parameter = data.value<Layer>().absolute_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Relative_Density)				{parameter = data.value<Layer>().relative_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Thickness)						{parameter = data.value<Layer>().thickness;				precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Sigma)							{parameter = data.value<Layer>().sigma;					precision = line_edit_sigma_precision;}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)		{parameter = data.value<Layer>().thickness_Drift.drift_Line_Value;			precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		{parameter = data.value<Layer>().thickness_Drift.drift_Rand_Rms;			precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	{parameter = data.value<Layer>().thickness_Drift.drift_Sine_Amplitude;		precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	{parameter = data.value<Layer>().thickness_Drift.drift_Sine_Frequency;		precision = line_edit_thickness_precision;}
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		{parameter = data.value<Layer>().thickness_Drift.drift_Sine_Phase;			precision = line_edit_thickness_precision;}

		if(whats_This == whats_This_Sigma_Drift_Line_Value)			{parameter = data.value<Layer>().sigma_Drift.drift_Line_Value;		precision = line_edit_sigma_precision;}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			{parameter = data.value<Layer>().sigma_Drift.drift_Rand_Rms;		precision = line_edit_sigma_precision;}
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		{parameter = data.value<Layer>().sigma_Drift.drift_Sine_Amplitude;	precision = line_edit_sigma_precision;}
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		{parameter = data.value<Layer>().sigma_Drift.drift_Sine_Frequency;	precision = line_edit_sigma_precision;}
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			{parameter = data.value<Layer>().sigma_Drift.drift_Sine_Phase;		precision = line_edit_sigma_precision;}
	}
	if(item_Type_String == whats_This_Substrate)
	{
		if(whats_This == whats_This_Absolute_Density)	{parameter = data.value<Substrate>().absolute_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Relative_Density)	{parameter = data.value<Substrate>().relative_Density;		precision = line_edit_density_precision;}
		if(whats_This == whats_This_Sigma)				{parameter = data.value<Substrate>().sigma;					precision = line_edit_sigma_precision;	}
	}

	double value;

	if(val_Type == VAL)	{value = parameter.value;  }
	if(val_Type == MIN)	{value = parameter.fit.min;}
	if(val_Type == MAX)	{value = parameter.fit.max;}

	QString text_Value = QString::number(value, line_edit_double_format, precision);

	// create lineedit
	QLineEdit* line_Edit = new QLineEdit(text_Value);
	line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

	// number of element is here
	line_Edit->setProperty(min_Size_Property, line_Edit->width());
	line_Edit->setProperty(item_Type_Property, item_Type_String);
	line_Edit->setProperty(column_Property, current_Column);
	line_Edit->setProperty(whats_This_Property, whats_This);
	line_Edit->setProperty(value_Type_Property, val_Type);

	if(whats_This == whats_This_Absolute_Density)				line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Relative_Density)				line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Thickness)						line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Sigma)							line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

	if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

	if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

	// for reloading
	line_Edit->setProperty(reload_Property, false);
	all_Widgets_To_Reload.append(line_Edit);

	connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

	// create item
	main_Table->setCellWidget(current_Row, current_Column, line_Edit);
	line_Edits_Map.insert(line_Edit, structure_Item);
	connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Parameter(QString)));
	line_Edit->textEdited(line_Edit->text());
}

void Table_Of_Structures::create_Check_Box_Fit(int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, int r_S, int r_F, int c_S, int c_F)
{
	// PARAMETER
	Parameter parameter;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Ambient)
	{
		if(whats_This == whats_This_Absolute_Density)	parameter = data.value<Ambient>().absolute_Density;
		if(whats_This == whats_This_Relative_Density)	parameter = data.value<Ambient>().relative_Density;
	}
	if(item_Type_String == whats_This_Layer)
	{
		if(whats_This == whats_This_Absolute_Density)				parameter = data.value<Layer>().absolute_Density;
		if(whats_This == whats_This_Relative_Density)				parameter = data.value<Layer>().relative_Density;
		if(whats_This == whats_This_Thickness)						parameter = data.value<Layer>().thickness;
		if(whats_This == whats_This_Sigma)							parameter = data.value<Layer>().sigma;

		if(whats_This == whats_This_Thickness_Drift_Line_Value)		parameter = data.value<Layer>().thickness_Drift.drift_Line_Value;
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		parameter = data.value<Layer>().thickness_Drift.drift_Rand_Rms;
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	parameter = data.value<Layer>().thickness_Drift.drift_Sine_Amplitude;
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	parameter = data.value<Layer>().thickness_Drift.drift_Sine_Frequency;
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		parameter = data.value<Layer>().thickness_Drift.drift_Sine_Phase;

		if(whats_This == whats_This_Sigma_Drift_Line_Value)			parameter = data.value<Layer>().sigma_Drift.drift_Line_Value;
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			parameter = data.value<Layer>().sigma_Drift.drift_Rand_Rms;
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		parameter = data.value<Layer>().sigma_Drift.drift_Sine_Amplitude;
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		parameter = data.value<Layer>().sigma_Drift.drift_Sine_Frequency;
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			parameter = data.value<Layer>().sigma_Drift.drift_Sine_Phase;
	}
	if(item_Type_String == whats_This_Substrate)
	{
		if(whats_This == whats_This_Absolute_Density)	parameter = data.value<Substrate>().absolute_Density;
		if(whats_This == whats_This_Relative_Density)	parameter = data.value<Substrate>().relative_Density;
		if(whats_This == whats_This_Sigma)				parameter = data.value<Substrate>().sigma;
	}

	// create check_Box
	QCheckBox* check_Box = new QCheckBox("fit");

	// alignment
	QWidget* back_Widget = new QWidget;
	QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
	back_Layout->addWidget(check_Box);
	back_Layout->setSpacing(0);
	back_Layout->setContentsMargins(0,0,0,0);
	back_Layout->setAlignment(Qt::AlignCenter);

	check_Box->setProperty(item_Type_Property, item_Type_String);
	check_Box->setProperty(whats_This_Property, whats_This);
	check_Box->setProperty(row_Property, current_Row);
	check_Box->setProperty(column_Property, current_Column);

	// which cells disable and enable
	check_Box->setProperty(relative_Rows_To_Disable_Start_Property, r_S);
	check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, r_F);
	check_Box->setProperty(relative_Columns_To_Disable_Start_Property, c_S);
	check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, c_F);

	// for reloading
	check_Box->setProperty(reload_Property, false);
	all_Widgets_To_Reload.append(check_Box);

	// create item
	main_Table->setCellWidget(current_Row, current_Column, back_Widget);
	check_Boxes_Map.insert(check_Box, structure_Item);
	connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(refresh_Fit_Parameter(bool)));
	connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(cells_On_Off(bool)));
	check_Box->setChecked(parameter.fit.is_Fitable);
	check_Box->toggled(false);
}

// for interlayers
void Table_Of_Structures::create_Check_Box_Label_Interlayer(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;
	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		QCheckBox* check_Box = new QCheckBox(transition_Layer_Functions[interlayer_Index]);

		// enable/disable function
		check_Box->setProperty(row_Property, current_Row);
		check_Box->setProperty(column_Property, current_Column);
		check_Box->setProperty(relative_Rows_To_Disable_Start_Property, 1);
		check_Box->setProperty(relative_Rows_To_Disable_Finish_Property, 1);
		check_Box->setProperty(relative_Columns_To_Disable_Start_Property, 0);
		check_Box->setProperty(relative_Columns_To_Disable_Finish_Property, 0);

		check_Box->setProperty(interlayer_Index_Property, interlayer_Index);
		check_Box->setProperty(item_Type_Property, item_Type_String);
		check_Boxes_Map.insert(check_Box, structure_Item);

		// for reloading
		check_Box->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(check_Box);

		check_Box->setChecked(interlayer_Composition[interlayer_Index].enabled);
		connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(cells_On_Off(bool)));
		connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(refresh_Check_Box_Label_Interlayer(bool)));
		check_Box->toggled(false);

		// alignment
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		back_Widget->setStyleSheet("background-color: lightblue");
		main_Table->setCellWidget(current_Row, current_Column, back_Widget);

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_Weigts_Interlayer(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;
	double value;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		if(val_Type == VAL)	{value = interlayer_Composition[interlayer_Index].interlayer.value;	}
		if(val_Type == MIN)	{value = interlayer_Composition[interlayer_Index].interlayer.fit.min;}
		if(val_Type == MAX)	{value = interlayer_Composition[interlayer_Index].interlayer.fit.max;}

		QString text_Value = QString::number(value, line_edit_short_double_format, line_edit_interlayer_precision);

		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

		line_Edit->setProperty(interlayer_Index_Property, interlayer_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(item_Type_Property, item_Type_String);
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setProperty(value_Type_Property, val_Type);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		// for reloading
		line_Edit->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(line_Edit);

		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		// create item
		main_Table->setCellWidget(current_Row, current_Column, line_Edit);
		line_Edits_Map.insert(line_Edit, structure_Item);
		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Weigts_Interlayer(QString)));
		line_Edit->textEdited(line_Edit->text());

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_Weights_Check_Box_Fit_Interlayer(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		QCheckBox* check_Box = new QCheckBox("fit");

		check_Box->setProperty(interlayer_Index_Property, interlayer_Index);
		check_Box->setProperty(item_Type_Property, item_Type_String);
		check_Boxes_Map.insert(check_Box, structure_Item);

		// for reloading
		check_Box->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(check_Box);

		check_Box->setChecked(interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable);
		connect(check_Box, SIGNAL(toggled(bool)), this, SLOT(refresh_Weights_Check_Box_Fit_Interlayer(bool)));
		check_Box->toggled(false);

		// alignment
		QWidget* back_Widget = new QWidget;
		QVBoxLayout* back_Layout = new QVBoxLayout(back_Widget);
		back_Layout->addWidget(check_Box);
		back_Layout->setSpacing(0);
		back_Layout->setContentsMargins(0,0,0,0);
		back_Layout->setAlignment(Qt::AlignCenter);

		main_Table->setCellWidget(current_Row, current_Column, back_Widget);

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_MySigma_Labels_Interlayer(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;
	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		QLabel* label = new QLabel(Sigma_Sym+" "+transition_Layer_Functions[interlayer_Index]);
		label->setStyleSheet("background-color: lightblue");
		label->setAlignment(Qt::AlignCenter);

		main_Table->setCellWidget(current_Row, current_Column, label);

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

void Table_Of_Structures::create_MySigma_Interlayer(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QVector<Interlayer> interlayer_Composition;
	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);

	if(item_Type_String == whats_This_Layer)		{interlayer_Composition = data.value<Layer>().	  interlayer_Composition;}
	if(item_Type_String == whats_This_Substrate)	{interlayer_Composition = data.value<Substrate>().interlayer_Composition;}

	int current_Column = start_Column;
	double value;

	for(int interlayer_Index=0; interlayer_Index<interlayer_Composition.size(); ++interlayer_Index)
	{
		value = interlayer_Composition[interlayer_Index].my_Sigma.value;

		QString text_Value = QString::number(value, line_edit_double_format, line_edit_sigma_precision);

		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

		line_Edit->setProperty(interlayer_Index_Property, interlayer_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(item_Type_Property, item_Type_String);
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		// for reloading
		line_Edit->setProperty(reload_Property, false);
		all_Widgets_To_Reload.append(line_Edit);

		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		// create item
		main_Table->setCellWidget(current_Row, current_Column, line_Edit);
		line_Edits_Map.insert(line_Edit, structure_Item);
		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_MySigma_Interlayer(QString)));
		line_Edit->textEdited(line_Edit->text());

		current_Column+=TABLE_COLUMN_INTERLAYERS_SHIFT;
	}
}

// refresh

// for material only
void Table_Of_Structures::refresh_Element(QString temp)
{
	QComboBox* combo_Box = qobject_cast<QComboBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = elements_Map.value(combo_Box);
	QString item_Type_String = combo_Box->property(item_Type_Property).toString();
	int composition_Index = combo_Box->property(num_Chemic_Element_Property).toInt();

	// for reloading
	bool reload = combo_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)	{combo_Box->setCurrentIndex(combo_Box->findText(ambient.composition[composition_Index].type));	return;	}

		ambient.composition[composition_Index].type = combo_Box->currentText();
		ambient.material = material_From_Composition(ambient.composition);
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{combo_Box->setCurrentIndex(combo_Box->findText(layer.composition[composition_Index].type));	return;	}

		layer.composition[composition_Index].type = combo_Box->currentText();
		layer.material = material_From_Composition(layer.composition);
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{combo_Box->setCurrentIndex(combo_Box->findText(substrate.composition[composition_Index].type));return;	}

		substrate.composition[composition_Index].type = combo_Box->currentText();
		substrate.material = material_From_Composition(substrate.composition);
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

void Table_Of_Structures::refresh_Stoich(QString temp)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();
	int composition_Index = line_Edit->property(num_Chemic_Element_Property).toInt();
	QString value_Type = line_Edit->property(value_Type_Property).toString();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(ambient.composition[composition_Index].composition.value,   line_edit_short_double_format, line_edit_composition_precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(ambient.composition[composition_Index].composition.fit.min, line_edit_short_double_format, line_edit_composition_precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(ambient.composition[composition_Index].composition.fit.max, line_edit_short_double_format, line_edit_composition_precision));

			return;
		}

		if(value_Type == VAL)	ambient.composition[composition_Index].composition.value = line_Edit->text().toDouble();
		if(value_Type == MIN)	ambient.composition[composition_Index].composition.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	ambient.composition[composition_Index].composition.fit.max = line_Edit->text().toDouble();

		ambient.material = material_From_Composition(ambient.composition);
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(layer.composition[composition_Index].composition.value,   line_edit_short_double_format, line_edit_composition_precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(layer.composition[composition_Index].composition.fit.min, line_edit_short_double_format, line_edit_composition_precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(layer.composition[composition_Index].composition.fit.max, line_edit_short_double_format, line_edit_composition_precision));

			return;
		}

		if(value_Type == VAL)	layer.composition[composition_Index].composition.value = line_Edit->text().toDouble();
		if(value_Type == MIN)	layer.composition[composition_Index].composition.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	layer.composition[composition_Index].composition.fit.max = line_Edit->text().toDouble();

		layer.material = material_From_Composition(layer.composition);
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(substrate.composition[composition_Index].composition.value,   line_edit_short_double_format, line_edit_composition_precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.composition[composition_Index].composition.fit.min, line_edit_short_double_format, line_edit_composition_precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.composition[composition_Index].composition.fit.max, line_edit_short_double_format, line_edit_composition_precision));

			return;
		}

		if(value_Type == VAL)	substrate.composition[composition_Index].composition.value = line_Edit->text().toDouble();
		if(value_Type == MIN)	substrate.composition[composition_Index].composition.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	substrate.composition[composition_Index].composition.fit.max = line_Edit->text().toDouble();

		substrate.material = material_From_Composition(substrate.composition);
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

void Table_Of_Structures::refresh_Fit_Element(bool b)
{
	b;
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	int composition_Index = check_Box->property(num_Chemic_Element_Property).toInt();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)	{check_Box->setChecked(ambient.composition[composition_Index].composition.fit.is_Fitable);	return;	}

		ambient.composition[composition_Index].composition.fit.is_Fitable = check_Box->isChecked();
		ambient.material = material_From_Composition(ambient.composition);
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{check_Box->setChecked(layer.composition[composition_Index].composition.fit.is_Fitable);	return;	}

		layer.composition[composition_Index].composition.fit.is_Fitable = check_Box->isChecked();
		layer.material = material_From_Composition(layer.composition);
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{check_Box->setChecked(substrate.composition[composition_Index].composition.fit.is_Fitable);	return;	}

		substrate.composition[composition_Index].composition.fit.is_Fitable = check_Box->isChecked();
		substrate.material = material_From_Composition(substrate.composition);
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}


void Table_Of_Structures::refresh_Material(QString temp)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)	{line_Edit->setText(ambient.material);	return;	}

		ambient.material = line_Edit->text();
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{line_Edit->setText(layer.material);	return;	}

		layer.material = line_Edit->text();
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{line_Edit->setText(substrate.material);	return;	}

		substrate.material = line_Edit->text();
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

void Table_Of_Structures::check_Material()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();

	// no cheking for reloading
	bool reload = line_Edit->property(reload_Property).toBool();
	if(reload)	{return;}

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		line_Edit->textEdited(line_Edit->text());
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		if(optical_Constants->material_Map.contains(ambient.material + nk_Ext))
		{
			ambient.approved_Material = ambient.material;
		} else
		{
			QMessageBox::information(this, "Wrong material", "File \"" + ambient.material + nk_Ext + "\" not found");
			ambient.material = ambient.approved_Material;
			line_Edit->setText(ambient.material);
			line_Edit->textEdited(line_Edit->text());
		}
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		line_Edit->textEdited(line_Edit->text());

		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		if(optical_Constants->material_Map.contains(layer.material + nk_Ext))
		{
			layer.approved_Material = layer.material;
		} else
		{
			QMessageBox::information(this, "Wrong material", "File \"" + layer.material + nk_Ext + "\" not found");
			layer.material = layer.approved_Material;
			line_Edit->setText(layer.material);
			line_Edit->textEdited(line_Edit->text());
		}
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		line_Edit->textEdited(line_Edit->text());
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		if(optical_Constants->material_Map.contains(substrate.material + nk_Ext))
		{
			substrate.approved_Material = substrate.material;
		} else
		{
			QMessageBox::information(this, "Wrong material", "File \"" + substrate.material + nk_Ext + "\" not found");
			substrate.material = substrate.approved_Material;
			line_Edit->setText(substrate.material);
			line_Edit->textEdited(line_Edit->text());
		}
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

void Table_Of_Structures::browse_Material(bool b)
{
	// TODO
	b;
	qInfo() << "browse...";
}

// for several parameters
void Table_Of_Structures::refresh_Drift_Header(bool b)
{
	// PARAMETER
	b;
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	QString whats_This = check_Box->property(whats_This_Property).toString();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole);
	QVariant var;
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = data.value<Layer>();

		if(reload)
		{
			if(whats_This == whats_This_Thickness_Drift_Line_Value)	check_Box->setChecked(layer.thickness_Drift.is_Drift_Line);
			if(whats_This == whats_This_Thickness_Drift_Rand_Rms  )	check_Box->setChecked(layer.thickness_Drift.is_Drift_Rand);
			if(whats_This == whats_This_Thickness_Drift_Sine	  )	check_Box->setChecked(layer.thickness_Drift.is_Drift_Sine);

			if(whats_This == whats_This_Sigma_Drift_Line_Value)		check_Box->setChecked(layer.sigma_Drift.is_Drift_Line);
			if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )		check_Box->setChecked(layer.sigma_Drift.is_Drift_Rand);
			if(whats_This == whats_This_Sigma_Drift_Sine	  )		check_Box->setChecked(layer.sigma_Drift.is_Drift_Sine);

			return;
		}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)	layer.thickness_Drift.is_Drift_Line = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms  )	layer.thickness_Drift.is_Drift_Rand = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine	  )	layer.thickness_Drift.is_Drift_Sine = check_Box->isChecked();

		if(whats_This == whats_This_Sigma_Drift_Line_Value)		layer.sigma_Drift.is_Drift_Line = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms  )		layer.sigma_Drift.is_Drift_Rand = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine	  )		layer.sigma_Drift.is_Drift_Sine = check_Box->isChecked();

		var.setValue( layer );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

// for all parameters
void Table_Of_Structures::refresh_Parameter(QString temp)
{
	// PARAMETER
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();
	QString value_Type = line_Edit->property(value_Type_Property).toString();
	QString whats_This = line_Edit->property(whats_This_Property).toString();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(ambient.absolute_Density.value,	 line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(ambient.absolute_Density.fit.min, line_edit_double_format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(ambient.absolute_Density.fit.max, line_edit_double_format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Relative_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(ambient.relative_Density.value,	 line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(ambient.relative_Density.fit.min, line_edit_double_format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(ambient.relative_Density.fit.max, line_edit_double_format, line_edit_density_precision));
			}
			return;
		}

		if(whats_This == whats_This_Absolute_Density)
		{
			if(value_Type == VAL)	ambient.absolute_Density.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	ambient.absolute_Density.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	ambient.absolute_Density.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(value_Type == VAL)	ambient.relative_Density.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	ambient.relative_Density.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	ambient.relative_Density.fit.max = line_Edit->text().toDouble();
		}

		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.absolute_Density.value,   line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.absolute_Density.fit.min, line_edit_double_format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.absolute_Density.fit.max, line_edit_double_format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Relative_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.relative_Density.value,   line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.relative_Density.fit.min, line_edit_double_format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.relative_Density.fit.max, line_edit_double_format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Thickness)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness.fit.min, line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness.fit.max, line_edit_double_format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Sigma)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma.fit.min, line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma.fit.max, line_edit_double_format, line_edit_sigma_precision));
			}

			if(whats_This == whats_This_Thickness_Drift_Line_Value)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Line_Value.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Line_Value.fit.min, line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Line_Value.fit.max, line_edit_double_format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Thickness_Drift_Rand_Rms)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Rand_Rms.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Rand_Rms.fit.min, line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Rand_Rms.fit.max, line_edit_double_format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Amplitude.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Amplitude.fit.min, line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Amplitude.fit.max, line_edit_double_format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Frequency.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Frequency.fit.min, line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Frequency.fit.max, line_edit_double_format, line_edit_thickness_precision));
			}
			if(whats_This == whats_This_Thickness_Drift_Sine_Phase)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Phase.value,   line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Phase.fit.min, line_edit_double_format, line_edit_thickness_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.thickness_Drift.drift_Sine_Phase.fit.max, line_edit_double_format, line_edit_thickness_precision));
			}

			if(whats_This == whats_This_Sigma_Drift_Line_Value)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Line_Value.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Line_Value.fit.min, line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Line_Value.fit.max, line_edit_double_format, line_edit_sigma_precision));
			}
			if(whats_This == whats_This_Sigma_Drift_Rand_Rms)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Rand_Rms.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Rand_Rms.fit.min, line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Rand_Rms.fit.max, line_edit_double_format, line_edit_sigma_precision));
			}
			if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Amplitude.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Amplitude.fit.min, line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Amplitude.fit.max, line_edit_double_format, line_edit_sigma_precision));
			}
			if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Frequency.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Frequency.fit.min, line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Frequency.fit.max, line_edit_double_format, line_edit_sigma_precision));
			}
			if(whats_This == whats_This_Sigma_Drift_Sine_Phase)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Phase.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Phase.fit.min, line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(layer.sigma_Drift.drift_Sine_Phase.fit.max, line_edit_double_format, line_edit_sigma_precision));
			}
			return;
		}

		if(whats_This == whats_This_Absolute_Density)
		{
			if(value_Type == VAL)	layer.absolute_Density.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.absolute_Density.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.absolute_Density.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(value_Type == VAL)	layer.relative_Density.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.relative_Density.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.relative_Density.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness)
		{
			if(value_Type == VAL)	layer.thickness.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Sigma)
		{
			if(value_Type == VAL)	layer.sigma.value = line_Edit->text().toDouble();
			if(value_Type == MIN)
			{
				layer.sigma.fit.min = line_Edit->text().toDouble();
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					layer.interlayer_Composition[interlayer_Index].interlayer.fit.min = line_Edit->text().toDouble();
				}
			}
			if(value_Type == MAX)
			{
				layer.sigma.fit.max = line_Edit->text().toDouble();
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					layer.interlayer_Composition[interlayer_Index].interlayer.fit.max = line_Edit->text().toDouble();
				}
			}
		}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Line_Value.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Line_Value.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Line_Value.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Rand_Rms.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Rand_Rms.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Rand_Rms.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Sine_Amplitude.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Sine_Amplitude.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Sine_Amplitude.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Sine_Frequency.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Sine_Frequency.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Sine_Frequency.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)
		{
			if(value_Type == VAL)	layer.thickness_Drift.drift_Sine_Phase.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.thickness_Drift.drift_Sine_Phase.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.thickness_Drift.drift_Sine_Phase.fit.max = line_Edit->text().toDouble();
		}

		if(whats_This == whats_This_Sigma_Drift_Line_Value)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Line_Value.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Line_Value.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Line_Value.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Rand_Rms.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Rand_Rms.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Rand_Rms.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Sine_Amplitude.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Sine_Amplitude.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Sine_Amplitude.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Sine_Frequency.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Sine_Frequency.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Sine_Frequency.fit.max = line_Edit->text().toDouble();
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)
		{
			if(value_Type == VAL)	layer.sigma_Drift.drift_Sine_Phase.value = line_Edit->text().toDouble();
			if(value_Type == MIN)	layer.sigma_Drift.drift_Sine_Phase.fit.min = line_Edit->text().toDouble();
			if(value_Type == MAX)	layer.sigma_Drift.drift_Sine_Phase.fit.max = line_Edit->text().toDouble();
		}

		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(substrate.absolute_Density.value,   line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.absolute_Density.fit.min, line_edit_double_format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.absolute_Density.fit.max, line_edit_double_format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Relative_Density)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(substrate.relative_Density.value,   line_edit_double_format, line_edit_density_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.relative_Density.fit.min, line_edit_double_format, line_edit_density_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.relative_Density.fit.max, line_edit_double_format, line_edit_density_precision));
			}
			if(whats_This == whats_This_Sigma)
			{
				if(value_Type == VAL)	line_Edit->setText(QString::number(substrate.sigma.value,   line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.sigma.fit.min, line_edit_double_format, line_edit_sigma_precision));
				if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.sigma.fit.max, line_edit_double_format, line_edit_sigma_precision));
			}
			return;
		}
		if(whats_This == whats_This_Absolute_Density)
		{
			if(value_Type == VAL)	{substrate.absolute_Density.value = line_Edit->text().toDouble();	}
			if(value_Type == MIN)	{substrate.absolute_Density.fit.min = line_Edit->text().toDouble();}
			if(value_Type == MAX)	{substrate.absolute_Density.fit.max = line_Edit->text().toDouble();}
		}
		if(whats_This == whats_This_Relative_Density)
		{
			if(value_Type == VAL)	{substrate.relative_Density.value = line_Edit->text().toDouble();	}
			if(value_Type == MIN)	{substrate.relative_Density.fit.min = line_Edit->text().toDouble();}
			if(value_Type == MAX)	{substrate.relative_Density.fit.max = line_Edit->text().toDouble();}
		}
		if(whats_This == whats_This_Sigma)
		{
			if(value_Type == VAL)	{substrate.sigma.value = line_Edit->text().toDouble();	}
			if(value_Type == MIN)
			{
				substrate.sigma.fit.min = line_Edit->text().toDouble();
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					substrate.interlayer_Composition[interlayer_Index].interlayer.fit.min = line_Edit->text().toDouble();
				}
			}
			if(value_Type == MAX)
			{
				substrate.sigma.fit.max = line_Edit->text().toDouble();
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
				{
					substrate.interlayer_Composition[interlayer_Index].interlayer.fit.max = line_Edit->text().toDouble();
				}
			}
		}

		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

void Table_Of_Structures::refresh_Fit_Parameter(bool b)
{
	// PARAMETER
	b;
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	QString whats_This = check_Box->property(whats_This_Property).toString();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)	check_Box->setChecked(ambient.absolute_Density.fit.is_Fitable);
			if(whats_This == whats_This_Relative_Density)	check_Box->setChecked(ambient.absolute_Density.fit.is_Fitable);

			return;
		}

		if(whats_This == whats_This_Absolute_Density)	ambient.absolute_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Relative_Density)	ambient.relative_Density.fit.is_Fitable = check_Box->isChecked();

		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)				check_Box->setChecked(layer.absolute_Density.fit.is_Fitable);
			if(whats_This == whats_This_Relative_Density)				check_Box->setChecked(layer.relative_Density.fit.is_Fitable);
			if(whats_This == whats_This_Thickness)						check_Box->setChecked(layer.thickness.fit.is_Fitable);
			if(whats_This == whats_This_Sigma)                  		check_Box->setChecked(layer.sigma.fit.is_Fitable);

			if(whats_This == whats_This_Thickness_Drift_Line_Value)		check_Box->setChecked(layer.thickness_Drift.drift_Line_Value.fit.is_Fitable);
			if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		check_Box->setChecked(layer.thickness_Drift.drift_Rand_Rms.fit.is_Fitable);
			if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	check_Box->setChecked(layer.thickness_Drift.drift_Sine_Amplitude.fit.is_Fitable);
			if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	check_Box->setChecked(layer.thickness_Drift.drift_Sine_Frequency.fit.is_Fitable);
			if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		check_Box->setChecked(layer.thickness_Drift.drift_Sine_Phase.fit.is_Fitable);

			if(whats_This == whats_This_Sigma_Drift_Line_Value)			check_Box->setChecked(layer.sigma_Drift.drift_Line_Value.fit.is_Fitable);
			if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			check_Box->setChecked(layer.sigma_Drift.drift_Rand_Rms.fit.is_Fitable);
			if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		check_Box->setChecked(layer.sigma_Drift.drift_Sine_Amplitude.fit.is_Fitable);
			if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		check_Box->setChecked(layer.sigma_Drift.drift_Sine_Frequency.fit.is_Fitable);
			if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			check_Box->setChecked(layer.sigma_Drift.drift_Sine_Phase.fit.is_Fitable);

			return;
		}

		if(whats_This == whats_This_Absolute_Density)				layer.absolute_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Relative_Density)				layer.relative_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness)						layer.thickness.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma)
		{
			layer.sigma.fit.is_Fitable = check_Box->isChecked();
			for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
			{
				layer.interlayer_Composition[interlayer_Index].my_Sigma.fit.is_Fitable = check_Box->isChecked();
			}
		}

		if(whats_This == whats_This_Thickness_Drift_Line_Value)		layer.thickness_Drift.drift_Line_Value.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)		layer.thickness_Drift.drift_Rand_Rms.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)	layer.thickness_Drift.drift_Sine_Amplitude.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)	layer.thickness_Drift.drift_Sine_Frequency.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)		layer.thickness_Drift.drift_Sine_Phase.fit.is_Fitable = check_Box->isChecked();

		if(whats_This == whats_This_Sigma_Drift_Line_Value)			layer.sigma_Drift.drift_Line_Value.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)			layer.sigma_Drift.drift_Rand_Rms.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)		layer.sigma_Drift.drift_Sine_Amplitude.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)		layer.sigma_Drift.drift_Sine_Frequency.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)			layer.sigma_Drift.drift_Sine_Phase.fit.is_Fitable = check_Box->isChecked();

		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)
		{
			if(whats_This == whats_This_Absolute_Density)	check_Box->setChecked(substrate.absolute_Density.fit.is_Fitable);
			if(whats_This == whats_This_Relative_Density)	check_Box->setChecked(substrate.relative_Density.fit.is_Fitable);
			if(whats_This == whats_This_Sigma)              check_Box->setChecked(substrate.sigma.fit.is_Fitable);

			return;
		}

		if(whats_This == whats_This_Absolute_Density)	substrate.absolute_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Relative_Density)	substrate.relative_Density.fit.is_Fitable = check_Box->isChecked();
		if(whats_This == whats_This_Sigma)
		{
			substrate.sigma.fit.is_Fitable = check_Box->isChecked();
			for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; ++interlayer_Index)
			{
				substrate.interlayer_Composition[interlayer_Index].my_Sigma.fit.is_Fitable = check_Box->isChecked();
			}
		}

		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

// for interlayers
void Table_Of_Structures::refresh_Check_Box_Label_Interlayer(bool b)
{
	// PARAMETER
	b;
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	int interlayer_Index = check_Box->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{check_Box->setChecked(layer.interlayer_Composition[interlayer_Index].enabled);	return;	}

		layer.interlayer_Composition[interlayer_Index].enabled = check_Box->isChecked();
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{check_Box->setChecked(substrate.interlayer_Composition[interlayer_Index].enabled);	return;	}

		substrate.interlayer_Composition[interlayer_Index].enabled = check_Box->isChecked();
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

void Table_Of_Structures::refresh_Weigts_Interlayer(QString temp)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();
	int interlayer_Index = line_Edit->property(interlayer_Index_Property).toInt();
	QString value_Type = line_Edit->property(value_Type_Property).toString();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;

	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(layer.interlayer_Composition[interlayer_Index].interlayer.value,	  line_edit_short_double_format, line_edit_interlayer_precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(layer.interlayer_Composition[interlayer_Index].interlayer.fit.min, line_edit_short_double_format, line_edit_interlayer_precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(layer.interlayer_Composition[interlayer_Index].interlayer.fit.max, line_edit_short_double_format, line_edit_interlayer_precision));

			return;
		}

		if(value_Type == VAL)	layer.interlayer_Composition[interlayer_Index].interlayer.value = line_Edit->text().toDouble();
		if(value_Type == MIN)	layer.interlayer_Composition[interlayer_Index].interlayer.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	layer.interlayer_Composition[interlayer_Index].interlayer.fit.max = line_Edit->text().toDouble();

		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)
		{
			if(value_Type == VAL)	line_Edit->setText(QString::number(substrate.interlayer_Composition[interlayer_Index].interlayer.value,	  line_edit_short_double_format, line_edit_interlayer_precision));
			if(value_Type == MIN)	line_Edit->setText(QString::number(substrate.interlayer_Composition[interlayer_Index].interlayer.fit.min, line_edit_short_double_format, line_edit_interlayer_precision));
			if(value_Type == MAX)	line_Edit->setText(QString::number(substrate.interlayer_Composition[interlayer_Index].interlayer.fit.max, line_edit_short_double_format, line_edit_interlayer_precision));

			return;
		}

		if(value_Type == VAL)	substrate.interlayer_Composition[interlayer_Index].interlayer.value = line_Edit->text().toDouble();
		if(value_Type == MIN)	substrate.interlayer_Composition[interlayer_Index].interlayer.fit.min = line_Edit->text().toDouble();
		if(value_Type == MAX)	substrate.interlayer_Composition[interlayer_Index].interlayer.fit.max = line_Edit->text().toDouble();

		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

void Table_Of_Structures::refresh_Weights_Check_Box_Fit_Interlayer(bool b)
{
	// PARAMETER
	b;
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = check_Boxes_Map.value(check_Box);
	QString item_Type_String = check_Box->property(item_Type_Property).toString();
	int interlayer_Index = check_Box->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = check_Box->property(reload_Property).toBool();

	QVariant var;
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{check_Box->setChecked(layer.interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable);	return;	}

		layer.interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable = check_Box->isChecked();
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{check_Box->setChecked(substrate.interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable);	return;	}

		substrate.interlayer_Composition[interlayer_Index].interlayer.fit.is_Fitable = check_Box->isChecked();
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

void Table_Of_Structures::refresh_MySigma_Interlayer(QString temp)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();
	int interlayer_Index = line_Edit->property(interlayer_Index_Property).toInt();

	// for reloading
	bool reload = line_Edit->property(reload_Property).toBool();

	QVariant var;

	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(reload)	{line_Edit->setText(QString::number(layer.interlayer_Composition[interlayer_Index].my_Sigma.value, line_edit_double_format, line_edit_sigma_precision)); return;}

		layer.interlayer_Composition[interlayer_Index].my_Sigma.value = line_Edit->text().toDouble();

		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(reload)	{line_Edit->setText(QString::number(substrate.interlayer_Composition[interlayer_Index].my_Sigma.value, line_edit_double_format, line_edit_sigma_precision)); return;}

		substrate.interlayer_Composition[interlayer_Index].my_Sigma.value = line_Edit->text().toDouble();

		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
	reload_All_Widgets(QObject::sender());
}

// general
void Table_Of_Structures::cells_On_Off(bool b)
{
	b;
	QCheckBox* check_Box = qobject_cast<QCheckBox*>(QObject::sender());

	int current_Row = check_Box->property(row_Property).toInt();
	int current_Column = check_Box->property(column_Property).toInt();

	int row_Start = check_Box->property(relative_Rows_To_Disable_Start_Property).toInt();
	int row_Finish = check_Box->property(relative_Rows_To_Disable_Finish_Property).toInt();
	int column_Start = check_Box->property(relative_Columns_To_Disable_Start_Property).toInt();
	int column_Finish = check_Box->property(relative_Columns_To_Disable_Finish_Property).toInt();

	for(int row=row_Start; row<=row_Finish; ++row)
	for(int col=column_Start; col<=column_Finish; ++col)
	{
		QWidget* widget = main_Table->cellWidget(current_Row+row,current_Column+col);
		if(widget)
		{
			widget->setDisabled(!check_Box->isChecked());

			if(!check_Box->isChecked())
				widget->setStyleSheet("border: 0px solid red");
			else
				widget->setStyleSheet("border: 1px solid grey");
		}
	}
}

void Table_Of_Structures::resize_Line_Edit(QString text, QLineEdit* line_Edit)
{
	if(line_Edit == NULL) line_Edit = qobject_cast<QLineEdit*>(QObject::sender());

	text = line_Edit->text();
	QFontMetrics fm = line_Edit->fontMetrics();
	int width = fm.width(text) + QLINEEDIT_RESIZE_MARGIN;
	if(width<line_Edit->property(min_Size_Property).toInt())	{width=line_Edit->property(min_Size_Property).toInt();}
	line_Edit->setFixedWidth(width);

	int current_Column = line_Edit->property(column_Property).toInt();
	fit_Column(0, current_Column);
}
