#include "item_editing.h"

Item_Editing::Item_Editing(QTreeWidgetItem* item):
	item(item),
	gui_Settings(Gui_Settings_Path, QSettings::IniFormat),
	default_Values(Default_Values_Path, QSettings::IniFormat)
{
	// map of sorted chemical elements
	for(int i=0; i<element_Name.size(); ++i)
		sorted_Elements.insert(element_Name[i],element_Mass[i]);

	setWindowTitle(item->whatsThis(default_Column));
	create_Main_Layout();
	set_Window_Geometry();
}

void Item_Editing::closeEvent(QCloseEvent* event)
{
	refresh_Data();
	refresh_Material();
	emit is_Closed();
	event;
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
	material_Group_Box = new QGroupBox;
	material_Group_Box->setObjectName("material_Group_Box");
	material_Group_Box->setStyleSheet("QGroupBox#material_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
	material_Group_Box_Layout = new QVBoxLayout(material_Group_Box);

	// materials
	{
		QHBoxLayout* layout = new QHBoxLayout;
		QLabel* material_Label = new QLabel("Material:");
			layout->addWidget(material_Label);

		material_Line_Edit = new QLineEdit;
		material_Line_Edit->setFixedWidth(70);
			show_Material();
			layout->addWidget(material_Line_Edit);

		browse_Material_Button = new QPushButton("Browse...");
		browse_Material_Button->setFixedWidth(60);
			layout->addWidget(browse_Material_Button);

		QLabel* density_Label = new QLabel("Density (g/cm" + Cube_Sym + "):");
			layout->addWidget(density_Label);

		density_Line_Edit = new QLineEdit;
		density_Line_Edit->setFixedWidth(45);
		density_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			show_Density();
			layout->addWidget(density_Line_Edit);

		material_Group_Box_Layout->addLayout(layout);

		connect(material_Line_Edit, SIGNAL(editingFinished()), this, SLOT(refresh_Material()));
		connect(density_Line_Edit, SIGNAL(editingFinished()), this, SLOT(refresh_Data()));
	}
	// radio buttons
	{
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
		composition_Frame = new QFrame;
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
		if( item->data(default_Column, Qt::UserRole).value<Ambient>().composed)	{composition_Radio->setChecked(true); composition_Radio_Toggled(true);}
	}
	if(item_Type==Item_Type::Layer)
	{
		if(!item->data(default_Column, Qt::UserRole).value<Layer>().composed)	{filename_Radio->setChecked(true);    filename_Radio_Toggled(true);}
		if( item->data(default_Column, Qt::UserRole).value<Layer>().composed)	{composition_Radio->setChecked(true); composition_Radio_Toggled(true);}
	}
	if(item_Type==Item_Type::Substrate)
	{
		if(!item->data(default_Column, Qt::UserRole).value<Substrate>().composed)	{filename_Radio->setChecked(true);    filename_Radio_Toggled(true);}
		if( item->data(default_Column, Qt::UserRole).value<Substrate>().composed)	{composition_Radio->setChecked(true); composition_Radio_Toggled(true);}
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
	make_Materials_Group_Box();
		main_Layout->addWidget(material_Group_Box);
}

void Item_Editing::set_Window_Geometry()
{
	// TODO geometry
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
	show_Material();
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Item_Editing::composition_Radio_Toggled(bool)
{
	refresh_Material();
	material_Line_Edit->setEnabled(false);
	browse_Material_Button->setEnabled(false);

	// composition or filename
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		ambient.composed = true;	var.setValue( ambient );

		// add element row if no data
		if(ambient.composition.size()==0)
		{
			QMetaObject::invokeMethod(this, "moreElementsClicked", Qt::QueuedConnection);
		}
		// if data already exists
		else
		{
			read_Elements_From_Item();
		}
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		layer.composed = true;	var.setValue( layer );

		// add element row if no data
		if(layer.composition.size()==0)
		{
			QMetaObject::invokeMethod(this, "moreElementsClicked", Qt::QueuedConnection);
		}
		// if data already exists
		else
		{
			read_Elements_From_Item();
		}
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		substrate.composed = true;	var.setValue( substrate );

		// add element row if no data
		if(substrate.composition.size()==0)
		{
			QMetaObject::invokeMethod(this, "moreElementsClicked", Qt::QueuedConnection);
		}
		// if data already exists
		else
		{
			read_Elements_From_Item();
		}
	}
	item->setData(default_Column, Qt::UserRole, var);

	composition_Frame->show();
	refresh_Data();
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Item_Editing::more_Elements_Clicked(bool)
{
	default_Values.beginGroup( Structure_Values_Representation );
		int precision = default_Values.value( "default_density_precision", 0 ).toInt();
		int at_wt_precision = default_Values.value( "default_atomic_weight_precision", 0 ).toInt();
	default_Values.endGroup();

	if (element_Frame_Vec.size()==0)	{fewer_Elements->hide();}
	else								{fewer_Elements->show();}

	QVariant var;
	Stoichiometry stoich;

	QLineEdit* line_Edit = new QLineEdit;		line_Edit->setFixedWidth(50); line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
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
			stoich.type  = default_Values.value( "substrate_default_stoichiometry_element", 0 ).toString();
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

	// adding data
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
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(QString::number(1,'f',precision));
		fewer_Elements->hide();
	}
	if (element_Frame_Vec.size()>=2) {composition_Line_Edit_Vec.first()->setDisabled(false);fewer_Elements->show();}
}

void Item_Editing::read_Elements_From_Item()
{
	default_Values.beginGroup( Structure_Values_Representation );
		int precision = default_Values.value( "default_density_precision", 0 ).toInt();
		int at_wt_precision = default_Values.value( "default_atomic_weight_precision", 0 ).toInt();
	default_Values.endGroup();

	// clear all previous widgets
	for(int i=0; i<element_Frame_Vec.size(); ++i)
		delete element_Frame_Vec[i];

	// reading data
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();

		composition_Line_Edit_Vec.resize(ambient.composition.size());
		composition_Label_Vec.resize(ambient.composition.size());
		composition_Combo_Box_Vec.resize(ambient.composition.size());
		composition_At_Weight_Vec.resize(ambient.composition.size());

		element_Frame_Vec.resize(ambient.composition.size());

		// renew ui
		for(int i=0; i<ambient.composition.size(); ++i)
		{
			// creating ui elements
			QLineEdit* line_Edit = new QLineEdit;		line_Edit->setFixedWidth(50); line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			QLabel*    label = new QLabel("atom of");
			QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
			QLabel*	  at_Weight = new QLabel;

			line_Edit->setText(QString::number(ambient.composition[i].composition,'f',precision));
			elements->setCurrentIndex(elements->findText(ambient.composition[i].type));
			at_Weight->setText("At. Wt.=" + QString::number(sorted_Elements.value(elements->currentText()),'f',at_wt_precision) + ")");

			composition_Line_Edit_Vec[i]=line_Edit;
			composition_Label_Vec[i]=label;
			composition_Combo_Box_Vec[i]=elements;
			composition_At_Weight_Vec[i]=at_Weight;

			connect(line_Edit, SIGNAL(editingFinished()), this, SLOT(refresh_Data()));
			connect(elements, SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Data(QString)));

			// placing ui elements
			QFrame* element_Frame = new QFrame;
			element_Frame_Vec[i] = element_Frame;
			element_Frame->setContentsMargins(-9,-9,-9,-9);
			QHBoxLayout* hor_Layout = new QHBoxLayout(element_Frame);
				hor_Layout->setSpacing(4);
				hor_Layout->addWidget(line_Edit);
				hor_Layout->addWidget(label);
				hor_Layout->addWidget(elements);
				hor_Layout->addWidget(at_Weight);
			composition_Layout_With_Elements_Vector->setSpacing(0);
			composition_Layout_With_Elements_Vector->addWidget(element_Frame,0,Qt::AlignLeft);
		}
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();

		composition_Line_Edit_Vec.resize(layer.composition.size());
		composition_Label_Vec.resize(layer.composition.size());
		composition_Combo_Box_Vec.resize(layer.composition.size());
		composition_At_Weight_Vec.resize(layer.composition.size());

		element_Frame_Vec.resize(layer.composition.size());

		// renew ui
		for(int i=0; i<layer.composition.size(); ++i)
		{
			// creating ui elements
			QLineEdit* line_Edit = new QLineEdit;		line_Edit->setFixedWidth(50); line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			QLabel*    label = new QLabel("atom of");
			QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
			QLabel*	  at_Weight = new QLabel;

			line_Edit->setText(QString::number(layer.composition[i].composition,'f',precision));
			elements->setCurrentIndex(elements->findText(layer.composition[i].type));
			at_Weight->setText("At. Wt.=" + QString::number(sorted_Elements.value(elements->currentText()),'f',at_wt_precision) + ")");

			composition_Line_Edit_Vec[i]=line_Edit;
			composition_Label_Vec[i]=label;
			composition_Combo_Box_Vec[i]=elements;
			composition_At_Weight_Vec[i]=at_Weight;

			connect(line_Edit, SIGNAL(editingFinished()), this, SLOT(refresh_Data()));
			connect(elements, SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Data(QString)));

			// placing ui elements
			QFrame* element_Frame = new QFrame;
			element_Frame_Vec[i] = element_Frame;
			element_Frame->setContentsMargins(-9,-9,-9,-9);
			QHBoxLayout* hor_Layout = new QHBoxLayout(element_Frame);
				hor_Layout->setSpacing(4);
				hor_Layout->addWidget(line_Edit);
				hor_Layout->addWidget(label);
				hor_Layout->addWidget(elements);
				hor_Layout->addWidget(at_Weight);
			composition_Layout_With_Elements_Vector->setSpacing(0);
			composition_Layout_With_Elements_Vector->addWidget(element_Frame,0,Qt::AlignLeft);
		}
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();

		composition_Line_Edit_Vec.resize(substrate.composition.size());
		composition_Label_Vec.resize(substrate.composition.size());
		composition_Combo_Box_Vec.resize(substrate.composition.size());
		composition_At_Weight_Vec.resize(substrate.composition.size());

		element_Frame_Vec.resize(substrate.composition.size());

		// renew ui
		for(int i=0; i<substrate.composition.size(); ++i)
		{
			// creating ui elements
			QLineEdit* line_Edit = new QLineEdit;		line_Edit->setFixedWidth(50); line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			QLabel*    label = new QLabel("atom of");
			QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
			QLabel*	  at_Weight = new QLabel;

			line_Edit->setText(QString::number(substrate.composition[i].composition,'f',precision));
			elements->setCurrentIndex(elements->findText(substrate.composition[i].type));
			at_Weight->setText("At. Wt.=" + QString::number(sorted_Elements.value(elements->currentText()),'f',at_wt_precision) + ")");

			composition_Line_Edit_Vec[i]=line_Edit;
			composition_Label_Vec[i]=label;
			composition_Combo_Box_Vec[i]=elements;
			composition_At_Weight_Vec[i]=at_Weight;

			connect(line_Edit, SIGNAL(editingFinished()), this, SLOT(refresh_Data()));
			connect(elements, SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Data(QString)));

			// placing ui elements
			QFrame* element_Frame = new QFrame;
			element_Frame_Vec[i] = element_Frame;
			element_Frame->setContentsMargins(-9,-9,-9,-9);
			QHBoxLayout* hor_Layout = new QHBoxLayout(element_Frame);
				hor_Layout->setSpacing(4);
				hor_Layout->addWidget(line_Edit);
				hor_Layout->addWidget(label);
				hor_Layout->addWidget(elements);
				hor_Layout->addWidget(at_Weight);
			composition_Layout_With_Elements_Vector->setSpacing(0);
			composition_Layout_With_Elements_Vector->addWidget(element_Frame,0,Qt::AlignLeft);
		}
	}

	refresh_Data();
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(QString::number(1,'f',precision));
		fewer_Elements->hide();
	}
	if (element_Frame_Vec.size()>=2) {composition_Line_Edit_Vec.first()->setDisabled(false);fewer_Elements->show();}
}

void Item_Editing::fewer_Elements_Clicked(bool)
{
	default_Values.beginGroup( Structure_Values_Representation );
		int precision = default_Values.value( "default_density_precision", 0 ).toInt();
	default_Values.endGroup();

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
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(QString::number(1,'f',precision));
		fewer_Elements->hide();
	}

	// crutch
	filename_Radio_Toggled(true);
	composition_Radio_Toggled(true);
	filename_Radio_Toggled(true);
	composition_Radio_Toggled(true);
}

void Item_Editing::refresh_Material()
{
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		ambient.material = material_Line_Edit->text();
		var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		layer.material = material_Line_Edit->text();
		var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		substrate.material = material_Line_Edit->text();
		var.setValue( substrate );
	}
	item->setData(default_Column, Qt::UserRole, var);
}

void Item_Editing::show_Material()
{
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		material_Line_Edit->setText(ambient.material);
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		material_Line_Edit->setText(layer.material);
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		material_Line_Edit->setText(substrate.material);
	}
}

void Item_Editing::show_Density()
{
	default_Values.beginGroup( Structure_Values_Representation );
		int precision = default_Values.value( "default_density_precision", 0 ).toInt();
	default_Values.endGroup();

	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		density_Line_Edit->setText(QString::number(ambient.density,'f',precision));
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		density_Line_Edit->setText(QString::number(layer.density,'f',precision));
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		density_Line_Edit->setText(QString::number(substrate.density,'f',precision));
	}
}

void Item_Editing::refresh_Data()
{
	default_Values.beginGroup( Structure_Values_Representation );
		int at_wt_precision = default_Values.value( "default_atomic_weight_precision", 0 ).toInt();
	default_Values.endGroup();

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
			composition_At_Weight_Vec[i]->setText("At. Wt.=" + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),'f',at_wt_precision) + ")");
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
			composition_At_Weight_Vec[i]->setText("At. Wt.=" + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),'f',at_wt_precision) + ")");

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
			composition_At_Weight_Vec[i]->setText("At. Wt.=" + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),'f',at_wt_precision) + ")");

			//TODO other fields
		}
		var.setValue( substrate );
	}
	item->setData(default_Column, Qt::UserRole, var);

	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		if(ambient.composed)
		{
			material_Line_Edit->clear();
			for(int i=0; i<ambient.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if(composition_Line_Edit_Vec[i]->text().toDouble()!=1)
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble()));
			}
		}
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		if(layer.composed)
		{
			material_Line_Edit->clear();
			for(int i=0; i<layer.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if(composition_Line_Edit_Vec[i]->text().toDouble()!=1)
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble()));
			}
		}
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		if(substrate.composed)
		{
			material_Line_Edit->clear();
			for(int i=0; i<substrate.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if(composition_Line_Edit_Vec[i]->text().toDouble()!=1)
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble()));
			}
		}
	}
}

void Item_Editing::refresh_Data(QString)
{
	refresh_Data();
}
