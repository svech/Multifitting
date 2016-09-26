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
	material_Group_Box->setStyleSheet("QGroupBox { border-radius: 2px;  border: 1px solid gray;}");
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
			layout->addWidget(density_Line_Edit);
		material_Group_Box_Layout->addLayout(layout);
	}
	// radio buttons
	{
		composition_Frame = new QFrame;

		QHBoxLayout* layout = new QHBoxLayout;
		filename_Radio = new QRadioButton("Optical constants file name");
			layout->addWidget(filename_Radio);
		composition_Radio = new QRadioButton("Composition and Density");
			layout->addWidget(composition_Radio);
		material_Group_Box_Layout->addLayout(layout);

		connect(filename_Radio,    SIGNAL(clicked(bool)), this, SLOT(filename_Radio_Toggled(bool)));
		connect(composition_Radio, SIGNAL(clicked(bool)), this, SLOT(composition_Radio_Toggled(bool)));
	}
	// more/fewer elements buttons
	{
		composition_Frame->setContentsMargins(-5,-5,-5,-10);
		composition_Layout = new QVBoxLayout(composition_Frame);
		composition_Group_Box = new QGroupBox;
		composition_Layout_With_Elements_Vector = new QVBoxLayout(composition_Group_Box);

		QHBoxLayout* buttons_Layout = new QHBoxLayout;
			buttons_Layout->setSpacing(0);
		more_Elements  = new QPushButton(" More Elements ");
			more_Elements->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			buttons_Layout->addWidget(more_Elements);
		fewer_Elements = new QPushButton(" Fewer Elements ");
			fewer_Elements->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			buttons_Layout->addWidget(fewer_Elements,0,Qt::AlignLeft);

		composition_Layout->addWidget(composition_Group_Box);
		composition_Layout->addLayout(buttons_Layout);
		material_Group_Box_Layout->addWidget(composition_Frame);

		connect(more_Elements,  SIGNAL(clicked(bool)), this, SLOT(more_Elements_Clicked(bool)));
		connect(fewer_Elements, SIGNAL(clicked(bool)), this, SLOT(fewer_Elements_Clicked(bool)));

		more_Elements_Clicked(true);
	}

	if(item_Type==Item_Type::Ambient)
	{
		if(!item->data(default_Column, Qt::UserRole).value<Ambient>().composed)	{filename_Radio->setChecked(true);    composition_Frame->hide();}
		if( item->data(default_Column, Qt::UserRole).value<Ambient>().composed)	{composition_Radio->setChecked(true); composition_Frame->show();}
	}
	if(item_Type==Item_Type::Layer)
	{
		if(!item->data(default_Column, Qt::UserRole).value<Layer>().composed)	{filename_Radio->setChecked(true);    composition_Frame->hide();}
		if( item->data(default_Column, Qt::UserRole).value<Layer>().composed)	{composition_Radio->setChecked(true); composition_Frame->show();}
	}
	if(item_Type==Item_Type::Substrate)
	{
		if(!item->data(default_Column, Qt::UserRole).value<Substrate>().composed)	{filename_Radio->setChecked(true);    composition_Frame->hide();}
		if( item->data(default_Column, Qt::UserRole).value<Substrate>().composed)	{composition_Radio->setChecked(true); composition_Frame->show();}
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
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Item_Editing::more_Elements_Clicked(bool)
{
	QVariant var;
	Stoichiometry stoich;

	QLineEdit line_Edit;
	QLabel    label;
	QComboBox elements;
	QLabel	  at_Weight;

	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		default_Values.beginGroup( Ambient_Values );
			stoich.composition = default_Values.value( "ambient_default_stoichiometry_composition", 0 ).toDouble();
			stoich.type        = default_Values.value( "ambient_default_stoichiometry_element", 0 ).toString();
		default_Values.endGroup();
		ambient.composition.append(stoich);
		var.setValue( ambient );

		QList<QLineEdit*> composition_Line_Edit_Vec;
		QList<QLabel*>    composition_Label_Vec;
		QList<QComboBox*> composition_Combo_Box_Vec;
		QList<QLabel*>    composition_At_Weight_Vec;
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		default_Values.beginGroup( Layer_Values );
			stoich.composition = default_Values.value( "layer_default_stoichiometry_composition", 0 ).toDouble();
			stoich.type        = default_Values.value( "layer_default_stoichiometry_element", 0 ).toString();
		default_Values.endGroup();
		layer.composition.append(stoich);
		var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		default_Values.beginGroup( Substrate_Values );
			stoich.composition = default_Values.value( "substrate_default_stoichiometry_composition", 0 ).toDouble();
			stoich.type        = default_Values.value( "substrate_default_stoichiometry_element", 0 ).toString();
		default_Values.endGroup();
		substrate.composition.append(stoich);
		var.setValue( substrate );
	}
	item->setData(default_Column, Qt::UserRole, var);
}

void Item_Editing::fewer_Elements_Clicked(bool)
{

}
