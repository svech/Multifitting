#include "item_editing.h"

Item_Editing::Item_Editing(QTreeWidgetItem* item):
	item(item),
	gui_Settings(Gui_Settings_Path, QSettings::IniFormat),
	default_Values(Default_Values_Path, QSettings::IniFormat)
{
	setWindowTitle(item->whatsThis(default_Column));
	create_Main_Layout();
	set_Window_Geometry();
}

void Item_Editing::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);

	create_Menu();
		main_Layout->setMenuBar(menu_Bar);

	if(item->whatsThis(default_Column)==what_is_This_Ambient)
	{
		item_Type = Item_Type::Ambient;
		make_Ambient_Editor();
	} else
	if(item->whatsThis(default_Column)==what_is_This_Substrate)
	{
		item_Type = Item_Type::Substrate;
		make_Substrate_Editor();
	} else
	if(item->childCount()==0)
	{
		item_Type = Item_Type::Layer;
		make_Layer_Editor();
	} else
	{
		item_Type = Item_Type::Stack_Content;
		make_Multilayer_Editor();
	}

	done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button, SIGNAL(clicked()), this, SLOT(close()));

	main_Layout->setContentsMargins(4,0,4,0);
	setLayout(main_Layout);
}

void Item_Editing::create_Menu()
{
	// File
	file_Menu = new QMenu("File", this);

		QAction* act_Quit = new QAction("Done", this);
			connect(act_Quit, SIGNAL(triggered()), this, SLOT(close()));
			file_Menu->addAction(act_Quit);

	// add menu to menu bar
	menu_Bar = new QMenuBar(this);
	menu_Bar->addMenu(file_Menu);
}

void Item_Editing::make_Ambient_Editor()
{
	make_Materials_Group_Box();
		main_Layout->addWidget(material_Group_Box);
}

void Item_Editing::make_Materials_Group_Box()
{
	default_Values.beginGroup( Structure_Values_Representation );
		int precision = default_Values.value( "default_density_precision", 0 ).toInt();
	default_Values.endGroup();

	material_Group_Box = new QGroupBox;
	material_Group_Box->setObjectName("material_Group_Box");
	material_Group_Box->setStyleSheet("QGroupBox#material_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
	material_Group_Box_Layout = new QVBoxLayout(material_Group_Box);

	// materials
	{
		QHBoxLayout* layout = new QHBoxLayout;
		QLabel* material_Label = new QLabel("Material:");
			layout->addWidget(material_Label);
		material_Line_Edit = new QLineEdit(item->data(default_Column, Qt::UserRole).value<Ambient>().material);
		material_Line_Edit->setFixedWidth(70);
				layout->addWidget(material_Line_Edit);
		browse_Material_Button = new QPushButton("Browse...");
		browse_Material_Button->setFixedWidth(60);
			layout->addWidget(browse_Material_Button);
		QLabel* density_Label = new QLabel("Density (g/cm" + Cube_Sym + "):");
			layout->addWidget(density_Label);
		density_Line_Edit = new QLineEdit(QString::number(item->data(default_Column, Qt::UserRole).value<Ambient>().density,'f',precision));
		density_Line_Edit->setFixedWidth(45);
		density_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, precision));
			layout->addWidget(density_Line_Edit);
		material_Group_Box_Layout->addLayout(layout);

		connect(density_Line_Edit, SIGNAL(editingFinished()), this, SLOT(refresh_Data()));
	}
	// radio buttons
	{
		composition_Frame = new QFrame;

		QHBoxLayout* layout = new QHBoxLayout;
		filename_Radio = new QRadioButton("Optical constants file name");
			layout->addWidget(filename_Radio);
		composition_Radio = new QRadioButton("Composition of elements");
			layout->addWidget(composition_Radio);
		material_Group_Box_Layout->addLayout(layout);

		connect(filename_Radio,    SIGNAL(clicked(bool)), this, SLOT(filename_Radio_Toggled(bool)));
		connect(composition_Radio, SIGNAL(clicked(bool)), this, SLOT(composition_Radio_Toggled(bool)));
	}
	// more/fewer elements buttons
	{
		composition_Frame->setContentsMargins(-5,-5,-5,-10);
		composition_Layout = new QVBoxLayout(composition_Frame);
		composition_Group_Box = new QGroupBox("Composition");
		composition_Group_Box->setObjectName("composition_Group_Box");
		composition_Group_Box->setStyleSheet("QGroupBox#composition_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;} "
											 "QGroupBox::title { subcontrol-origin: margin;	 left: 4px; padding: 0 0px 0 1px;}");
		composition_Layout_With_Elements_Vector = new QVBoxLayout(composition_Group_Box);

		QFrame* buttons_Frame = new QFrame;
		buttons_Frame->setContentsMargins(-5,-10,-5,-9);
		QHBoxLayout* buttons_Layout = new QHBoxLayout(buttons_Frame);
			buttons_Layout->setSpacing(0); buttons_Layout->setAlignment(Qt::AlignLeft);
		more_Elements  = new QPushButton(" More elements ");
			more_Elements->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			buttons_Layout->addWidget(more_Elements);
		fewer_Elements = new QPushButton(" Fewer elements ");
			fewer_Elements->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			buttons_Layout->addWidget(fewer_Elements);

		composition_Layout->addWidget(composition_Group_Box);
		composition_Layout->addWidget(buttons_Frame);
		material_Group_Box_Layout->addWidget(composition_Frame);

		connect(more_Elements,  SIGNAL(clicked(bool)), this, SLOT(more_Elements_Clicked(bool)));
		connect(fewer_Elements, SIGNAL(clicked(bool)), this, SLOT(fewer_Elements_Clicked(bool)));
	}

	if(item_Type==Item_Type::Ambient)
	{
		if(!item->data(default_Column, Qt::UserRole).value<Ambient>().composed)	{filename_Radio->setChecked(true);    filename_Radio_Toggled(true);}
		if( item->data(default_Column, Qt::UserRole).value<Ambient>().composed)	{composition_Radio->setChecked(true); composition_Radio_Toggled(true); refresh_Data();}
	}
	if(item_Type==Item_Type::Layer)
	{
		if(!item->data(default_Column, Qt::UserRole).value<Layer>().composed)	{filename_Radio->setChecked(true);    filename_Radio_Toggled(true);}
		if( item->data(default_Column, Qt::UserRole).value<Layer>().composed)	{composition_Radio->setChecked(true); composition_Radio_Toggled(true);refresh_Data();}
	}
	if(item_Type==Item_Type::Substrate)
	{
		if(!item->data(default_Column, Qt::UserRole).value<Substrate>().composed)	{filename_Radio->setChecked(true);    filename_Radio_Toggled(true);}
		if( item->data(default_Column, Qt::UserRole).value<Substrate>().composed)	{composition_Radio->setChecked(true); composition_Radio_Toggled(true);refresh_Data();}
	}
}

void Item_Editing::make_Layer_Editor()
{
// TODO
}

void Item_Editing::make_Multilayer_Editor()
{
// TODO
}

void Item_Editing::make_Substrate_Editor()
{
// TODO
}

void Item_Editing::set_Window_Geometry()
{
	// TODO geometry
//	gui_Settings.beginGroup( Multilayer_Window_Geometry );
//		int item_Editing_X_Corner = gui_Settings.value( "multilayer_X_Corner", 0 ).toInt();
//		int multilayer_Y_Corner   = gui_Settings.value( "multilayer_Y_Corner", 0 ).toInt();
//		int multilayer_Min_Width  = gui_Settings.value( "multilayer_Min_Width", 0 ).toInt();
//		int multilayer_Width	  = gui_Settings.value( "multilayer_Width", 0 ).toInt();
//		int multilayer_Min_Height = gui_Settings.value( "multilayer_Min_Height", 0 ).toInt();
//		int multilayer_Height	  = gui_Settings.value( "multilayer_Height", 0 ).toInt();
//	gui_Settings.endGroup();

	adjustSize();
	setFixedSize(size());
	move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

// slots

void Item_Editing::filename_Radio_Toggled(bool)
{
	material_Line_Edit->setEnabled(true);
	browse_Material_Button->setEnabled(true);

	// composition or filename
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		ambient.composed = false;	var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		layer.composed = false;	var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		substrate.composed = false;	var.setValue( substrate );
	}
	item->setData(default_Column, Qt::UserRole, var);

	composition_Frame->hide();	
	material_Line_Edit->setText(item->data(default_Column, Qt::UserRole).value<Ambient>().material);

	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Item_Editing::composition_Radio_Toggled(bool)
{
	material_Line_Edit->setEnabled(false);
	browse_Material_Button->setEnabled(false);

	// composition or filename
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		ambient.composed = true;	var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		layer.composed = true;	var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		substrate.composed = true;	var.setValue( substrate );
	}
	item->setData(default_Column, Qt::UserRole, var);

	composition_Frame->show();	
	refresh_Data();

	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);

	// add element row first time
	if(element_Frame_Vec.size()==0) {more_Elements_Clicked(true);}
}

void Item_Editing::more_Elements_Clicked(bool)
{
	default_Values.beginGroup( Structure_Values_Representation );
		int precision = default_Values.value( "default_density_precision", 0 ).toInt();
		int at_wt_precision = default_Values.value( "default_atomic_weight_precision", 0 ).toInt();
	default_Values.endGroup();

	if (element_Frame_Vec.size()==0)	fewer_Elements->hide();
	else								fewer_Elements->show();

	// create map of chemical elements
	QMap<QString,double> sorted_Elements;
	for(int i=0; i<element_Name.size(); ++i)
		sorted_Elements.insert(element_Name[i],element_Mass[i]);

	QVariant var;
	Stoichiometry stoich;

	QLineEdit* line_Edit = new QLineEdit;		line_Edit->setFixedWidth(50); line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, precision));
	QLabel*    label = new QLabel("atom of");
	QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
	QLabel*	  at_Weight = new QLabel;

	connect(line_Edit, SIGNAL(editingFinished()), this, SLOT(refresh_Data()));
	connect(elements, SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Data(QString)));

	// loading settings
	default_Values.beginGroup( Structure_Init_Values );
	if(item_Type==Item_Type::Ambient)
	{
		default_Values.beginGroup( Ambient_Values );
			stoich.composition = default_Values.value( "ambient_default_stoichiometry_composition", 0 ).toDouble();
			stoich.type  = default_Values.value( "ambient_default_stoichiometry_element", 0 ).toString();
		default_Values.endGroup();
	}
	if(item_Type==Item_Type::Layer)
	{
		default_Values.beginGroup( Layer_Values );
			stoich.composition = default_Values.value( "layer_default_stoichiometry_composition", 0 ).toDouble();
			stoich.type  = default_Values.value( "layer_default_stoichiometry_element", 0 ).toString();
		default_Values.endGroup();
	}
	if(item_Type==Item_Type::Substrate)
	{
		default_Values.beginGroup( Substrate_Values );
			stoich.composition = default_Values.value( "substrate_default_stoichiometry_composition", 0 ).toDouble();
			stoich.type  = default_Values.value( "substrate_default_stoichiometry_composition", 0 ).toString();
		default_Values.endGroup();
	}
	default_Values.endGroup();

	// creating ui elements
	line_Edit->setText(QString::number(stoich.composition,'f',precision));
	elements->setCurrentIndex(elements->findText(stoich.type));
	at_Weight->setText("At. Wt.=" + QString::number(sorted_Elements.value(elements->currentText()),'f',at_wt_precision) + ")");

	composition_Line_Edit_Vec.append(line_Edit);
	composition_Label_Vec.append(label);
	composition_Combo_Box_Vec.append(elements);
	composition_At_Weight_Vec.append(at_Weight);

	// setting data
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		ambient.composition.append(stoich);
		var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		layer.composition.append(stoich);
		var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		substrate.composition.append(stoich);
		var.setValue( substrate );
	}
	item->setData(default_Column, Qt::UserRole, var);


	// placing ui elements
	QFrame* element_Frame = new QFrame;
	element_Frame_Vec.append(element_Frame);
	element_Frame->setContentsMargins(-9,-9,-9,-9);
	QHBoxLayout* hor_Layout = new QHBoxLayout(element_Frame);
		hor_Layout->setSpacing(4);
		hor_Layout->addWidget(line_Edit);
		hor_Layout->addWidget(label);
		hor_Layout->addWidget(elements);
		hor_Layout->addWidget(at_Weight);
	composition_Layout_With_Elements_Vector->setSpacing(0);
	composition_Layout_With_Elements_Vector->addWidget(element_Frame,0,Qt::AlignLeft);

	refresh_Data();

	if (element_Frame_Vec.size()==1) composition_Line_Edit_Vec.first()->setDisabled(true);
	else							 composition_Line_Edit_Vec.first()->setDisabled(false);
}

void Item_Editing::fewer_Elements_Clicked(bool)
{
	if (element_Frame_Vec.size()<=2)	fewer_Elements->hide();

	QVariant var;

	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		ambient.composition.removeLast();
		var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		layer.composition.removeLast();
		var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		substrate.composition.removeLast();
		var.setValue( substrate );
	}
	item->setData(default_Column, Qt::UserRole, var);

	composition_Line_Edit_Vec.removeLast();
	composition_Label_Vec.removeLast();
	composition_Combo_Box_Vec.removeLast();
	composition_At_Weight_Vec.removeLast();

	delete element_Frame_Vec.last();
	element_Frame_Vec.removeLast();

	refresh_Data();
	if (element_Frame_Vec.size()==1) composition_Line_Edit_Vec.first()->setDisabled(true);

	// crutch
	filename_Radio_Toggled(true);
	composition_Radio_Toggled(true);
	filename_Radio_Toggled(true);
	composition_Radio_Toggled(true);
}


void Item_Editing::refresh_Data()
{
	// refresh material name
	material_Line_Edit->clear();
	for(int i=0; i<element_Frame_Vec.size(); ++i)
	{
		material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
		if(composition_Line_Edit_Vec[i]->text().toDouble()!=1)
			material_Line_Edit->setText(material_Line_Edit->text() + composition_Line_Edit_Vec[i]->text());
	}

	// refresh data
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();

		// material is not changing
		ambient.density = density_Line_Edit->text().toDouble();
		for(int i=0; i<ambient.composition.size(); ++i)
		{
			ambient.composition[i].composition = composition_Line_Edit_Vec[i]->text().toDouble();
			ambient.composition[i].type = composition_Combo_Box_Vec[i]->currentText();
		}
		var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();

		// material is not changing
		layer.density = density_Line_Edit->text().toDouble();
		for(int i=0; i<layer.composition.size(); ++i)
		{
			layer.composition[i].composition = composition_Line_Edit_Vec[i]->text().toDouble();
			layer.composition[i].type = composition_Combo_Box_Vec[i]->currentText();

			//TODO other fields
		}
		var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();

		// material is not changing
		substrate.density = density_Line_Edit->text().toDouble();
		for(int i=0; i<substrate.composition.size(); ++i)
		{
			substrate.composition[i].composition = composition_Line_Edit_Vec[i]->text().toDouble();
			substrate.composition[i].type = composition_Combo_Box_Vec[i]->currentText();

			//TODO other fields
		}
		var.setValue( substrate );
	}
	item->setData(default_Column, Qt::UserRole, var);
}

void Item_Editing::refresh_Data(QString)
{
	refresh_Data();
}
