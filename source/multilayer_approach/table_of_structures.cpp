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
	resize(800,500);
}

void Table_Of_Structures::create_Table()
{
	// PARAMETER

	read_Trees();
	main_Table = new QTableWidget(basic_Row_Number, basic_Column_Number, this);
	rows_List_To_Span.clear();

//	main_Table->setShowGrid(false);

	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		// header for each structure
		main_Table->insertRow(main_Table->rowCount());
		int current_Row = main_Table->rowCount()-1;
		rows_List_To_Span.append(current_Row);
		main_Table->setItem(current_Row,0, new QTableWidgetItem(multilayer_Tabs->tabText(tab_Index)));
		main_Table->item   (current_Row,0)->setTextAlignment(Qt::AlignCenter);
		main_Table->item   (current_Row,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

		main_Table->insertRow(main_Table->rowCount());

		// structure display
		int max_Depth = Global_Variables::get_Tree_Depth(list_Of_Trees[tab_Index]->tree->invisibleRootItem());
		int depth;
		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(list_Of_Trees[tab_Index]->tree);
		while (*it)
		{
			structure_Item = *it;
			depth = Global_Variables::get_Item_Depth(structure_Item);
			main_Table->insertRow(main_Table->rowCount());
//			main_Table->insertRow(main_Table->rowCount());
			current_Row = main_Table->rowCount()-1;
			main_Table->insertRow(main_Table->rowCount());

			add_Columns(depth);

			// print whatsThis
			main_Table->setItem(current_Row,depth, new QTableWidgetItem(structure_Item->whatsThis(DEFAULT_COLUMN)));
			{
				main_Table->item(current_Row,depth)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			}

			// common variables
			bool composed_Material;

			// load data
			QStringList whats_This_List = structure_Item->whatsThis(DEFAULT_COLUMN).split(item_Type_Delimiter,QString::SkipEmptyParts);
			item_Type_String = whats_This_List[0];
			if(item_Type_String == whats_This_Ambient)
			{
				Ambient ambient			 = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
				composed_Material = ambient.composed_Material;
			}
			if(item_Type_String == whats_This_Layer)
			{
				Layer layer				 = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
				composed_Material = layer.composed_Material;
			}
			if(item_Type_String == whats_This_Multilayer)
			{
				Stack_Content stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
			}
			if(item_Type_String == whats_This_Substrate)
			{
				Substrate substrate		 = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
				composed_Material = substrate.composed_Material;
			}

			int current_Column = max_Depth+3;

			// creation
			if(item_Type_String == whats_This_Ambient || item_Type_String == whats_This_Layer || item_Type_String == whats_This_Substrate)
			if(composed_Material)
			{
				create_Combo_Elements(current_Row,   current_Column,   structure_Item, item_Type_String);
				create_Val_Stoich	 (current_Row+1, current_Column,   structure_Item, item_Type_String);
				create_Min_Stoich	 (current_Row+1, current_Column-1, structure_Item, item_Type_String);
				create_Max_Stoich	 (current_Row+1, current_Column+1, structure_Item, item_Type_String);
			}
			else
			{
				create_Material      (current_Row,   current_Column, structure_Item, item_Type_String);
				create_Browse_Button (current_Row+1, current_Column);
			}

			++it;
		}
		main_Table->insertRow(main_Table->rowCount());
//		main_Table->insertRow(main_Table->rowCount());
	}
	span_Structure_Headers();
	span_Structure_Items();
	main_Table->resizeColumnsToContents();
	main_Table->resizeRowsToContents();

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

void Table_Of_Structures::create_Combo_Elements(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QList<Stoichiometry> composition;

	if(item_Type_String == whats_This_Ambient)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().	 composition;}
	if(item_Type_String == whats_This_Layer)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().	 composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composition;}

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

		// create item
		main_Table->setCellWidget(current_Row, current_Column, elements);
		elements_Map.insert(elements, structure_Item);
		connect(elements,  SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Element(QString)));

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_Val_Stoich(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QList<Stoichiometry> composition;

	if(item_Type_String == whats_This_Ambient)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().	 composition;}
	if(item_Type_String == whats_This_Layer)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().	 composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composition;}

	int current_Column = start_Column;
	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		double value = composition[composition_Index].composition.value;
		QString text_Value = QString::number(value, line_edit_short_double_format, line_edit_composition_precision);

		// create lineedit
		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

		// number of element is here
		line_Edit->setProperty(num_Chemic_Element_Property, composition_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(item_Type_Property, item_Type_String);
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		// create item
		main_Table->setCellWidget(current_Row, current_Column, line_Edit);
		line_Edits_Map.insert(line_Edit, structure_Item);
		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Stoich_Val(QString)));
		line_Edit->textEdited(line_Edit->text());

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_Min_Stoich(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QList<Stoichiometry> composition;

	if(item_Type_String == whats_This_Ambient)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().	 composition;}
	if(item_Type_String == whats_This_Layer)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().	 composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composition;}

	int current_Column = start_Column;
	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		double value = composition[composition_Index].composition.fit.min;
		QString text_Value = QString::number(value, line_edit_short_double_format, line_edit_composition_precision);

		// create lineedit
		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

		// number of element is here
		line_Edit->setProperty(num_Chemic_Element_Property, composition_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(item_Type_Property, item_Type_String);
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		// create item
		main_Table->setCellWidget(current_Row, current_Column, line_Edit);
		line_Edits_Map.insert(line_Edit, structure_Item);
		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Stoich_Min(QString)));
		line_Edit->textEdited(line_Edit->text());

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_Max_Stoich(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QList<Stoichiometry> composition;

	if(item_Type_String == whats_This_Ambient)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().	 composition;}
	if(item_Type_String == whats_This_Layer)		{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().	 composition;}
	if(item_Type_String == whats_This_Substrate)	{composition = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composition;}

	int current_Column = start_Column;
	for(int composition_Index=0; composition_Index<composition.size(); ++composition_Index)
	{
		double value = composition[composition_Index].composition.fit.max;
		QString text_Value = QString::number(value, line_edit_short_double_format, line_edit_composition_precision);

		// create lineedit
		QLineEdit* line_Edit = new QLineEdit(text_Value);
		line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);

		// number of element is here
		line_Edit->setProperty(num_Chemic_Element_Property, composition_Index);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setProperty(item_Type_Property, item_Type_String);
		line_Edit->setProperty(column_Property, current_Column);
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		// create item
		main_Table->setCellWidget(current_Row, current_Column, line_Edit);
		line_Edits_Map.insert(line_Edit, structure_Item);
		connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Stoich_Max(QString)));
		line_Edit->textEdited(line_Edit->text());

		current_Column+=TABLE_COLUMN_ELEMENTS_SHIFT;
	}
}

void Table_Of_Structures::create_Material(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String)
{
	QString material;
	QString approved_Material;

	if(item_Type_String == whats_This_Ambient)		{		  material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().material;
													 approved_Material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().approved_Material;}
	if(item_Type_String == whats_This_Layer)		{		  material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().material;
													 approved_Material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().approved_Material;}
	if(item_Type_String == whats_This_Substrate)	{		  material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().material;
													 approved_Material = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().approved_Material;}

	add_Columns(start_Column+1);

	QLineEdit* material_Line_Edit = new QLineEdit(material);
	material_Line_Edit->setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT);
	material_Line_Edit->setProperty(min_Size_Property, material_Line_Edit->width());
	material_Line_Edit->setProperty(item_Type_Property, item_Type_String);
	material_Line_Edit->setProperty(column_Property, start_Column);

	connect(material_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

	main_Table->setCellWidget(current_Row, start_Column, material_Line_Edit);
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

void Table_Of_Structures::refresh_Element(QString temp)
{
	QComboBox* combo_Box = qobject_cast<QComboBox*>(QObject::sender());
	QTreeWidgetItem* structure_Item = elements_Map.value(combo_Box);
	QString item_Type_String = combo_Box->property(item_Type_Property).toString();
	int composition_Index = combo_Box->property(num_Chemic_Element_Property).toInt();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		ambient.composition[composition_Index].type = combo_Box->currentText();
		ambient.material = material_From_Composition(ambient.composition);
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		layer.composition[composition_Index].type = combo_Box->currentText();
		layer.material = material_From_Composition(layer.composition);
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		substrate.composition[composition_Index].type = combo_Box->currentText();
		substrate.material = material_From_Composition(substrate.composition);
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
}

void Table_Of_Structures::refresh_Stoich_Val(QString temp)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();
	int composition_Index = line_Edit->property(num_Chemic_Element_Property).toInt();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		ambient.composition[composition_Index].composition.value = line_Edit->text().toDouble();
		ambient.material = material_From_Composition(ambient.composition);
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		layer.composition[composition_Index].composition.value = line_Edit->text().toDouble();
		layer.material = material_From_Composition(layer.composition);
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		substrate.composition[composition_Index].composition.value = line_Edit->text().toDouble();
		substrate.material = material_From_Composition(substrate.composition);
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
}

void Table_Of_Structures::refresh_Stoich_Min(QString temp)
{

}

void Table_Of_Structures::refresh_Stoich_Max(QString temp)
{

}

void Table_Of_Structures::refresh_Material(QString temp)
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		ambient.material = line_Edit->text();
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		layer.material = line_Edit->text();
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		substrate.material = line_Edit->text();
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
}

void Table_Of_Structures::check_Material()
{
	QLineEdit* line_Edit = qobject_cast<QLineEdit*>(QObject::sender());
	QTreeWidgetItem* structure_Item = line_Edits_Map.value(line_Edit);
	QString item_Type_String = line_Edit->property(item_Type_Property).toString();

	QVariant var;
	if(item_Type_String == whats_This_Ambient)
	{
		Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		if(optical_Constants->material_Map.contains(ambient.material + nk_Ext))
		{
			ambient.approved_Material = ambient.material;
		} else
		{
			line_Edit->setText(ambient.approved_Material);
			ambient.material = ambient.approved_Material;
			line_Edit->textEdited(line_Edit->text());
			QMessageBox::information(this, "Wrong material", "File \"" + ambient.material + nk_Ext + "\" not found");
		}
		var.setValue( ambient );
	}
	if(item_Type_String == whats_This_Layer)
	{
		Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		if(optical_Constants->material_Map.contains(layer.material + nk_Ext))
		{
			layer.approved_Material = layer.material;
		} else
		{
			line_Edit->setText(layer.approved_Material);
			layer.material = layer.approved_Material;
			line_Edit->textEdited(line_Edit->text());
			QMessageBox::information(this, "Wrong material", "File \"" + layer.material + nk_Ext + "\" not found");
		}
		var.setValue( layer );
	}
	if(item_Type_String == whats_This_Substrate)
	{
		Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		if(optical_Constants->material_Map.contains(substrate.material + nk_Ext))
		{
			substrate.approved_Material = substrate.material;
		} else
		{
			line_Edit->setText(substrate.approved_Material);
			substrate.material = substrate.approved_Material;
			line_Edit->textEdited(line_Edit->text());
			QMessageBox::information(this, "Wrong material", "File \"" + substrate.material + nk_Ext + "\" not found");
		}
		var.setValue( substrate );
	}
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	emit_Data_Edited();
}

void Table_Of_Structures::browse_Material(bool b)
{
	// TODO
	b;
	qInfo() << "browse...";
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
					main_Table->setSpan(row_Index,col_Index,2,1);
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
		if(max_Width<current_Line_Edit->width())
		{
			max_Width=current_Line_Edit->width()+1;
		}
	}
	main_Table->setColumnWidth(current_Column,max_Width);
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
