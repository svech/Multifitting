// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "item_editor.h"

Item_Editor::Item_Editor(QTreeWidgetItem* item, QWidget *parent) :
	QDialog(parent),
	item(item)
{
	setWindowTitle(item->whatsThis(DEFAULT_COLUMN));
	create_Main_Layout();
	set_Window_Geometry();
}

void Item_Editor::emit_Refresh()
{
	// TODO
	show_All();
	initial_Radio_Check(false);	
	emit refresh();
}

void Item_Editor::closeEvent(QCloseEvent* event)
{
	norm_Interlayer_Composition();
	refresh_Data();
	refresh_Material();
	refresh_Stack_Data();
	emit closed();
	emit edited();
	event;
}

void Item_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);

	create_Menu();

	if(item->whatsThis(DEFAULT_COLUMN)==whats_This_Ambient)
	{
		item_Type = Item_Type::Ambient;
		make_Ambient_Editor();
	} else
	if(item->whatsThis(DEFAULT_COLUMN)==whats_This_Substrate)
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
		done_Button->setFocus();
		done_Button->setDefault(true);

	connect(done_Button, SIGNAL(clicked()), this, SLOT(done_Slot()));

	main_Layout->setContentsMargins(4,0,4,0);
}

void Item_Editor::create_Menu()
{
	Menu* menu = new Menu(Window_Type::Item_Editor(),this);
	main_Layout->setMenuBar(menu->menu_Bar);
	connect(menu, SIGNAL(refresh()), this, SLOT(emit_Refresh()));
}

void Item_Editor::make_Ambient_Editor()
{
	make_Materials_Group_Box();
		main_Layout->addWidget(material_Group_Box);
}

void Item_Editor::make_Materials_Group_Box()
{
	material_Group_Box = new QGroupBox;
	material_Group_Box->setObjectName("material_Group_Box");
	material_Group_Box->setStyleSheet("QGroupBox#material_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
	QVBoxLayout* material_Group_Box_Layout = new QVBoxLayout(material_Group_Box);

	// materials
	{
		QHBoxLayout* layout = new QHBoxLayout;
		QLabel* material_Label = new QLabel("Material:");
			layout->addWidget(material_Label);

		material_Line_Edit = new QLineEdit;
			material_Line_Edit->setFixedWidth(70);
			material_Line_Edit->setProperty(min_Size_Property, material_Line_Edit->width());
			layout->addWidget(material_Line_Edit);
			show_Material();

		browse_Material_Button = new QPushButton("Browse...");
		browse_Material_Button->setFixedWidth(60);
			layout->addWidget(browse_Material_Button);
			browse_Material_Button->setFocusPolicy(Qt::NoFocus);

		density_Label = new QLabel("<no label>");
			layout->addWidget(density_Label);

		density_Line_Edit = new QLineEdit;
			density_Line_Edit->setFixedWidth(50);
			density_Line_Edit->setProperty(min_Size_Property, density_Line_Edit->width());
			density_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			layout->addWidget(density_Line_Edit);
			show_Density();

		material_Group_Box_Layout->addLayout(layout);

		connect(material_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(density_Line_Edit,  SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

		connect(material_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Material(QString)));
		connect(density_Line_Edit,  SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));
		connect(browse_Material_Button, SIGNAL(clicked(bool)),   this, SLOT(browse_Material(bool)));
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
		QVBoxLayout* composition_Layout = new QVBoxLayout(composition_Frame);
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
	{
		material_Done = true;
		initial_Radio_Check(true);
	}
}

void Item_Editor::initial_Radio_Check(bool temp_bool)
{
	if(item_Type==Item_Type::Ambient)
	{
		if(!item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().composed_Material)	{filename_Radio->setChecked(true);    filename_Radio_Toggled(temp_bool);}
		if( item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>().composed_Material)	{composition_Radio->setChecked(true); composition_Radio_Toggled(temp_bool);}
	}
	if(item_Type==Item_Type::Layer)
	{
		if(!item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().composed_Material)		{filename_Radio->setChecked(true);    filename_Radio_Toggled(temp_bool);}
		if( item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>().composed_Material)		{composition_Radio->setChecked(true); composition_Radio_Toggled(temp_bool);}
	}
	if(item_Type==Item_Type::Substrate)
	{
		if(!item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composed_Material)	{filename_Radio->setChecked(true);    filename_Radio_Toggled(temp_bool);}
		if( item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>().composed_Material)	{composition_Radio->setChecked(true); composition_Radio_Toggled(temp_bool);}
	}
}

void Item_Editor::make_Layer_Editor()
{
	QHBoxLayout* hor_Layout = new QHBoxLayout;
	make_Materials_Group_Box();
		hor_Layout->addWidget(material_Group_Box);
	make_Thickness_Group_Box();
		hor_Layout->addWidget(thickness_Group_Box);
		main_Layout->addLayout(hor_Layout);
	make_Sigma_Group_Box();
		main_Layout->addWidget(sigma_Group_Box);
}

void Item_Editor::make_Thickness_Group_Box()
{
	{
		thickness_Group_Box = new QGroupBox;
		thickness_Group_Box->setObjectName("thickness_Group_Box");
		thickness_Group_Box->setStyleSheet("QGroupBox#thickness_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
		QVBoxLayout* thickness_Group_Box_Layout = new QVBoxLayout(thickness_Group_Box);
		thickness_Group_Box_Layout->setAlignment(Qt::AlignTop);

		QHBoxLayout* layout = new QHBoxLayout;
		thickness_Label = new QLabel(thickness_Label_1 + length_units + thickness_Label_2);
		layout->addWidget(thickness_Label);

		thickness_Line_Edit = new QLineEdit;
			thickness_Line_Edit->setFixedWidth(50);
			thickness_Line_Edit->setProperty(min_Size_Property, thickness_Line_Edit->width());
			thickness_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			layout->addWidget(thickness_Line_Edit);
		thickness_Group_Box_Layout->addLayout(layout);

		depth_Grading_Button = new QPushButton(" Depth Grading");
			depth_Grading_Button->adjustSize();
			depth_Grading_Button->setFixedSize(depth_Grading_Button->size());
			depth_Grading_Button->setFocusPolicy(Qt::NoFocus);
		if(item->parent())
			thickness_Group_Box_Layout->addWidget(depth_Grading_Button,0,Qt::AlignRight);

        connect(depth_Grading_Button,SIGNAL(clicked(bool)),       this, SLOT(depth_Grading(bool)));
		connect(thickness_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(thickness_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));
	}
	{
		thickness_Done = true;
		show_Thickness();
	}
}

void Item_Editor::make_Multilayer_Editor()
{
	make_Multilayer_Group_Box();
		main_Layout->addWidget(multilayer_Group_Box);
}

void Item_Editor::make_Multilayer_Group_Box()
{
	multilayer_Group_Box = new QGroupBox;
	multilayer_Group_Box->setObjectName("multilayer_Group_Box");
	multilayer_Group_Box->setStyleSheet("QGroupBox#multilayer_Group_Box { border-radius: 2px;  border: 1px solid gray;}");

	QVBoxLayout* multilayer_Group_Box_Layout = new QVBoxLayout(multilayer_Group_Box);

	{
		QHBoxLayout* layout = new QHBoxLayout;
		QLabel* repetitions_Label = new QLabel("Number of repetitions, N:");
			layout->addWidget(repetitions_Label);

		repetitions_Line_Edit = new QLineEdit;
			repetitions_Line_Edit->setFixedWidth(50);
			repetitions_Line_Edit->setProperty(min_Size_Property, repetitions_Line_Edit->width());
			repetitions_Line_Edit->setValidator(new QIntValidator(0, MAX_INTEGER));
			layout->addWidget(repetitions_Line_Edit);

		multilayer_Group_Box_Layout->addLayout(layout);
	}
	{
		QHBoxLayout* layout = new QHBoxLayout;
		period_Label = new QLabel(period_Label_1 + length_units + period_Label_2);
			layout->addWidget(period_Label);

		period_Line_Edit = new QLineEdit;
			period_Line_Edit->setFixedWidth(50);
			period_Line_Edit->setProperty(min_Size_Property, period_Line_Edit->width());
			period_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			layout->addWidget(period_Line_Edit);

		multilayer_Group_Box_Layout->addLayout(layout);
	}
	{
		QHBoxLayout* layout = new QHBoxLayout;
		gamma_Label = new QLabel("Thickness Ratio, " + Gamma_Sym + ":");

		gamma_Line_Edit = new QLineEdit;
			gamma_Line_Edit->setFixedWidth(50);
			gamma_Line_Edit->setProperty(min_Size_Property, gamma_Line_Edit->width());
			gamma_Line_Edit->setValidator(new QDoubleValidator(0, 1, MAX_PRECISION));

		if(item->childCount()==2)
		{
			layout->addWidget(gamma_Label);
			layout->addWidget(gamma_Line_Edit);

			multilayer_Group_Box_Layout->addLayout(layout);
		}
	}
	connect(repetitions_Line_Edit,  SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
	connect(period_Line_Edit,		SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
	connect(gamma_Line_Edit,		SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));

	connect(repetitions_Line_Edit,	SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Stack(QString)));
	connect(period_Line_Edit,		SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Stack(QString)));
	connect(gamma_Line_Edit,		SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Stack(QString)));
	{
		stack_Done = true;
		show_Stack_Parameters();
	}
}

void Item_Editor::make_Substrate_Editor()
{
	make_Materials_Group_Box();
		main_Layout->addWidget(material_Group_Box);
	make_Sigma_Group_Box();
		main_Layout->addWidget(sigma_Group_Box);
}

void Item_Editor::make_Sigma_Group_Box()
{
	sigma_Group_Box = new QGroupBox;
	sigma_Group_Box->setObjectName("sigma_Group_Box");
	sigma_Group_Box->setStyleSheet("QGroupBox#sigma_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
	QGridLayout* sigma_Group_Box_Layout = new QGridLayout(sigma_Group_Box);

	// sigma
	{
		QFrame* frame= new QFrame;
		frame->setContentsMargins(-6,-7,-6,-7);
		QHBoxLayout* layout = new QHBoxLayout(frame);
		layout->setAlignment(Qt::AlignLeft);
		roughness_Label = new QLabel(sigma_Label_1+ length_units + sigma_Label_2);
			layout->addWidget(roughness_Label,0,Qt::AlignLeft);

		sigma_Line_Edit = new QLineEdit;
			sigma_Line_Edit->setFixedWidth(50);
			sigma_Line_Edit->setProperty(min_Size_Property, sigma_Line_Edit->width());
			sigma_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			layout->addWidget(sigma_Line_Edit,0,Qt::AlignLeft);
			show_Sigma_Start();

		sigma_Group_Box_Layout->addWidget(frame);

		sigma_Grading_Button = new QPushButton(" Sigma Grading");
			sigma_Grading_Button->adjustSize();
			sigma_Grading_Button->setFixedSize(sigma_Grading_Button->size());
			sigma_Grading_Button->setFocusPolicy(Qt::NoFocus);
		if(item->parent())
			sigma_Group_Box_Layout->addWidget(sigma_Grading_Button,0,Qt::AlignRight);

        connect(sigma_Grading_Button,SIGNAL(clicked(bool)),   this, SLOT(sigma_Grading(bool)));
		connect(sigma_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(resize_Line_Edit(QString)));
		connect(sigma_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));
	}
	// interlayer functions
	{
		interlayer_Composition_Frame = new QFrame;
		interlayer_Composition_Frame->setContentsMargins(-5,-5,-5,-9);
		QVBoxLayout* interlayer_Composition_Layout = new QVBoxLayout(interlayer_Composition_Frame);
			interlayer_Composition_Layout->setAlignment(Qt::AlignLeft);
			QHBoxLayout* sigma_PSD_Layout = new QHBoxLayout;
		interlayer_Composition_Group_Box = new QGroupBox("Interlayer Composition");
		interlayer_Composition_Group_Box->setObjectName("interlayer_Composition_Group_Box");
		interlayer_Composition_Group_Box->setStyleSheet("QGroupBox#interlayer_Composition_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;} "
														"QGroupBox::title { subcontrol-origin: margin;	 left: 4px; padding: 0 0px 0 1px;}");
//		TODO create here first and last sigmas
		interlayer_Composition_Layout_With_Elements_Vector = new QHBoxLayout(interlayer_Composition_Group_Box);
			sigma_PSD_Layout->addWidget(interlayer_Composition_Group_Box);
		psd_Check_Box = new QCheckBox("Use PSD");
			sigma_PSD_Layout->addWidget(psd_Check_Box);
		// TODO PSD
		interlayer_Composition_Layout->addLayout(sigma_PSD_Layout);
		sigma_Group_Box_Layout->addWidget(interlayer_Composition_Frame);
	}
	{
		sigma_Done = true;		
		read_Interlayers_From_Item();
		show_Sigma();
	}
}

void Item_Editor::set_Window_Geometry()
{
	// TODO geometry
	adjustSize();
	setFixedSize(size());
	move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

void Item_Editor::show_All()
{
	show_Material();
	show_Density();
	show_Thickness();
	show_Sigma();
	show_Interlayers();
	show_Stack_Parameters();
}

// slots

void Item_Editor::filename_Radio_Toggled(bool temp_bool)
{
	material_Line_Edit->setEnabled(true);
	browse_Material_Button->setEnabled(true);
	density_Label->setText(relative_Density_Label);

	// composition or filename
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		ambient.composed_Material = false;	var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		layer.composed_Material = false;	var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		substrate.composed_Material = false;	var.setValue( substrate );
	}
	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	composition_Frame->hide();
	read_Elements_From_Item(temp_bool);
	show_All();

	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Item_Editor::composition_Radio_Toggled(bool temp_bool)
{
	density_Label->setText(absolute_Density_Label);
	if (temp_bool) refresh_Material();
	material_Line_Edit->setEnabled(false);
	browse_Material_Button->setEnabled(false);

	// composition or filename
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		ambient.composed_Material = true;	var.setValue( ambient );

		// add element row if no data
		if(ambient.composition.size()==0)
		{
			QMetaObject::invokeMethod(this, "moreElementsClicked", Qt::QueuedConnection);
		}
		// if data already exists
		else
		{
			read_Elements_From_Item(temp_bool);
		}
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		layer.composed_Material = true;	var.setValue( layer );

		// add element row if no data
		if(layer.composition.size()==0)
		{
			QMetaObject::invokeMethod(this, "moreElementsClicked", Qt::QueuedConnection);
		}
		// if data already exists
		else
		{
			read_Elements_From_Item(temp_bool);
		}
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		substrate.composed_Material = true;	var.setValue( substrate );

		// add element row if no data
		if(substrate.composition.size()==0)
		{
			QMetaObject::invokeMethod(this, "moreElementsClicked", Qt::QueuedConnection);
		}
		// if data already exists
		else
		{
			read_Elements_From_Item(temp_bool);
		}
	}
	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	composition_Frame->show();

	if (temp_bool) refresh_Data();
	show_All();
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Item_Editor::more_Elements_Clicked(bool)
{
	if (element_Frame_Vec.size()==0)	{fewer_Elements->hide();}
	else								{fewer_Elements->show();}

	QVariant var;
	Stoichiometry stoich;

	QLineEdit* line_Edit = new QLineEdit;
		line_Edit->setFixedWidth(50);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	QLabel*    label = new QLabel(atoms);
	QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
	QLabel*	  at_Weight = new QLabel;

	connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(resize_Line_Edit(QString)));
	connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(refresh_Data(QString)));
	connect(elements,  SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Data(QString)));

	// loading settings
	if(item_Type==Item_Type::Ambient)
	{
		stoich.composition.value = ambient_default_stoichiometry_composition;
		stoich.type				 = ambient_default_stoichiometry_element;
	}
	if(item_Type==Item_Type::Layer)
	{
		stoich.composition.value = layer_default_stoichiometry_composition;
		stoich.type				 = layer_default_stoichiometry_element;
	}
	if(item_Type==Item_Type::Substrate)
	{
		stoich.composition.value = substrate_default_stoichiometry_composition;
		stoich.type				 = substrate_default_stoichiometry_element;
	}

	// creating ui elements
	line_Edit->setText(QString::number(stoich.composition.value,line_edit_double_format,line_edit_composition_precision));
		resize_Line_Edit("",line_Edit);
	elements->setCurrentIndex(elements->findText(stoich.type));
	at_Weight->setText(AtWt + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

	composition_Line_Edit_Vec.append(line_Edit);
	composition_Label_Vec.append(label);
	composition_Combo_Box_Vec.append(elements);
	composition_At_Weight_Vec.append(at_Weight);

	// adding data
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		ambient.composition.append(stoich);
		var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		layer.composition.append(stoich);
		var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		substrate.composition.append(stoich);
		var.setValue( substrate );
	}
	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

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
		composition_Line_Edit_Vec.first()->setText(QString::number(1,line_edit_double_format,line_edit_composition_precision));
			resize_Line_Edit("",composition_Line_Edit_Vec.first());
		fewer_Elements->hide();
	}
	if (element_Frame_Vec.size()>=2) {composition_Line_Edit_Vec.first()->setDisabled(false);fewer_Elements->show();}
}

void Item_Editor::read_Elements_From_Item(bool temp_bool)
{
	// clear all previous widgets
	for(int i=0; i<element_Frame_Vec.size(); ++i)
		delete element_Frame_Vec[i];

	// reading data
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		composition_Line_Edit_Vec.resize(ambient.composition.size());
		composition_Label_Vec.	  resize(ambient.composition.size());
		composition_Combo_Box_Vec.resize(ambient.composition.size());
		composition_At_Weight_Vec.resize(ambient.composition.size());

		element_Frame_Vec.resize(ambient.composition.size());

		// renew ui
		for(int i=0; i<ambient.composition.size(); ++i)
		{
			// creating ui elements
			QLineEdit* line_Edit = new QLineEdit;
				line_Edit->setFixedWidth(50);
				line_Edit->setProperty(min_Size_Property, line_Edit->width());
				line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			QLabel*    label = new QLabel(atoms);
			QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
			QLabel*	  at_Weight = new QLabel;

			connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(resize_Line_Edit(QString)));

			line_Edit->setText(QString::number(ambient.composition[i].composition.value,line_edit_double_format,line_edit_composition_precision));
				resize_Line_Edit("",line_Edit);
			elements->setCurrentIndex(elements->findText(ambient.composition[i].type));
			at_Weight->setText(AtWt + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

			composition_Line_Edit_Vec[i]=line_Edit;
			composition_Label_Vec	 [i]=label;
			composition_Combo_Box_Vec[i]=elements;
			composition_At_Weight_Vec[i]=at_Weight;

			connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(refresh_Data(QString)));
			connect(elements,  SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Data(QString)));

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
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		composition_Line_Edit_Vec.resize(layer.composition.size());
		composition_Label_Vec.	  resize(layer.composition.size());
		composition_Combo_Box_Vec.resize(layer.composition.size());
		composition_At_Weight_Vec.resize(layer.composition.size());

		element_Frame_Vec.resize(layer.composition.size());

		// renew ui
		for(int i=0; i<layer.composition.size(); ++i)
		{
			// creating ui elements
			QLineEdit* line_Edit = new QLineEdit;
				line_Edit->setFixedWidth(50);
				line_Edit->setProperty(min_Size_Property, line_Edit->width());
				line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			QLabel*    label = new QLabel(atoms);
			QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
			QLabel*	  at_Weight = new QLabel;

			connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(resize_Line_Edit(QString)));

			line_Edit->setText(QString::number(layer.composition[i].composition.value,line_edit_double_format,line_edit_composition_precision));
				resize_Line_Edit("",line_Edit);
			elements->setCurrentIndex(elements->findText(layer.composition[i].type));
			at_Weight->setText(AtWt + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

			composition_Line_Edit_Vec[i]=line_Edit;
			composition_Label_Vec	 [i]=label;
			composition_Combo_Box_Vec[i]=elements;
			composition_At_Weight_Vec[i]=at_Weight;

			connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(refresh_Data(QString)));
			connect(elements,  SIGNAL(currentTextChanged(QString)), this, SLOT(refresh_Data(QString)));

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
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		composition_Line_Edit_Vec.resize(substrate.composition.size());
		composition_Label_Vec.resize(substrate.composition.size());
		composition_Combo_Box_Vec.resize(substrate.composition.size());
		composition_At_Weight_Vec.resize(substrate.composition.size());

		element_Frame_Vec.resize(substrate.composition.size());

		// renew ui
		for(int i=0; i<substrate.composition.size(); ++i)
		{
			// creating ui elements
			QLineEdit* line_Edit = new QLineEdit;
				line_Edit->setFixedWidth(50);
				line_Edit->setProperty(min_Size_Property, line_Edit->width());
				line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			QLabel*    label = new QLabel(atoms);
			QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
			QLabel*	  at_Weight = new QLabel;

			connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(resize_Line_Edit(QString)));

			line_Edit->setText(QString::number(substrate.composition[i].composition.value,line_edit_double_format,line_edit_composition_precision));
				resize_Line_Edit("",line_Edit);
			elements->setCurrentIndex(elements->findText(substrate.composition[i].type));
			at_Weight->setText(AtWt + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

			composition_Line_Edit_Vec[i] = line_Edit;
			composition_Label_Vec	 [i] = label;
			composition_Combo_Box_Vec[i] = elements;
			composition_At_Weight_Vec[i] = at_Weight;

			connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(refresh_Data(QString)));

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

	if (temp_bool) refresh_Data();
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(QString::number(1,line_edit_double_format,line_edit_composition_precision));
			resize_Line_Edit("",composition_Line_Edit_Vec.first());
		fewer_Elements->hide();
	}
	if (element_Frame_Vec.size()>=2) {composition_Line_Edit_Vec.first()->setDisabled(false);fewer_Elements->show();}
}

void Item_Editor::read_Interlayers_From_Item()
{
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		interlayer_Composition_Check_Box_Vec.resize(layer.interlayer_Composition.size());
		interlayer_Composition_Line_Edit_Vec.resize(layer.interlayer_Composition.size());

		// renew ui
		for(int i=0; i<layer.interlayer_Composition.size(); ++i)
		{
			// creating ui elements
			QCheckBox* check_Box = new QCheckBox(transition_Layer_Functions[i]);
			QLineEdit* line_Edit = new QLineEdit;
				line_Edit->setFixedWidth(40);
//				line_Edit->setProperty(min_Size_Property, line_Edit->width());
				line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

//			connect(line_Edit, SIGNAL(textEdited(QString)),	this, SLOT(resize_Line_Edit(QString)));

			check_Box->setChecked(layer.interlayer_Composition[i].enabled);
			line_Edit->setText(QString::number(layer.interlayer_Composition[i].interlayer.value,line_edit_double_format,line_edit_interlayer_precision));
//				resize_Line_Edit(line_Edit);
			interlayer_Composition_Check_Box_Vec[i]=check_Box;
			interlayer_Composition_Line_Edit_Vec[i]=line_Edit;

			connect(check_Box, SIGNAL(stateChanged(int)),	this, SLOT(interlayer_Check(int)));
			connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));

			// placing ui elements
			QVBoxLayout* vert_Layout = new QVBoxLayout;
				vert_Layout->setSpacing(3);
				vert_Layout->setAlignment(Qt::AlignCenter);
				vert_Layout->addWidget(check_Box);
				vert_Layout->addWidget(line_Edit);
			interlayer_Composition_Layout_With_Elements_Vector->addLayout(vert_Layout);
		}
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		interlayer_Composition_Check_Box_Vec.resize(substrate.interlayer_Composition.size());
		interlayer_Composition_Line_Edit_Vec.resize(substrate.interlayer_Composition.size());

		// renew ui
		for(int i=0; i<substrate.interlayer_Composition.size(); ++i)
		{
			// creating ui elements
			QCheckBox* check_Box = new QCheckBox(transition_Layer_Functions[i]);
			QLineEdit* line_Edit = new QLineEdit;
				line_Edit->setFixedWidth(40);
				line_Edit->setProperty(min_Size_Property, line_Edit->width());
				line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

			connect(line_Edit, SIGNAL(textEdited(QString)),	this, SLOT(resize_Line_Edit(QString)));

			check_Box->setChecked(substrate.interlayer_Composition[i].enabled);
			line_Edit->setText(QString::number(substrate.interlayer_Composition[i].interlayer.value,line_edit_double_format,line_edit_interlayer_precision));
				resize_Line_Edit("",line_Edit);
			interlayer_Composition_Check_Box_Vec[i]=check_Box;
			interlayer_Composition_Line_Edit_Vec[i]=line_Edit;

			connect(check_Box, SIGNAL(stateChanged(int)),	this, SLOT(interlayer_Check(int)));
			connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(refresh_Data(QString)));

			// placing ui elements
			QVBoxLayout* vert_Layout = new QVBoxLayout;
			vert_Layout->setSpacing(3);
			vert_Layout->setAlignment(Qt::AlignCenter);
				vert_Layout->addWidget(check_Box);
				vert_Layout->addWidget(line_Edit);
			interlayer_Composition_Layout_With_Elements_Vector->addLayout(vert_Layout);
		}
	}

	interlayer_Composition_Layout_With_Elements_Vector->setSpacing(10);
	interlayer_Composition_Group_Box->adjustSize();
	interlayer_Composition_Group_Box->setFixedWidth(interlayer_Composition_Group_Box->width());

	interlayer_Check(true); //-V601
}

void Item_Editor::fewer_Elements_Clicked(bool)
{
	if (element_Frame_Vec.size()<=2)	fewer_Elements->hide();

	QVariant var;

	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		ambient.composition.removeLast();
		var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		layer.composition.removeLast();
		var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		substrate.composition.removeLast();
		var.setValue( substrate );
	}
	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

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
		composition_Line_Edit_Vec.first()->setText(QString::number(1,line_edit_double_format,line_edit_composition_precision));
			resize_Line_Edit("",composition_Line_Edit_Vec.first());
		fewer_Elements->hide();
	}

	// crutch
	initial_Radio_Check(false);
}

void Item_Editor::refresh_Material(QString str)
{
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		ambient.material = material_Line_Edit->text();
		var.setValue( ambient );
		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		layer.material = material_Line_Edit->text();
		var.setValue( layer );
		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		substrate.material = material_Line_Edit->text();
		var.setValue( substrate );
		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}

	emit edited();
}

void Item_Editor::show_Material()
{
//	if(material_Done)
	{
		if(item_Type==Item_Type::Ambient)
		{
			Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

			if(ambient.composed_Material&&material_Done)
			{
				material_Line_Edit->clear();
				for(int i=0; i<ambient.composition.size(); ++i)
				{
					material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
					if( abs(composition_Line_Edit_Vec[i]->text().toDouble() - 1.) > DBL_EPSILON )
						material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_double_format,line_edit_composition_precision));
				}
			}
			else
				material_Line_Edit->setText(ambient.material);

			resize_Line_Edit("",material_Line_Edit);
		}
		if(item_Type==Item_Type::Layer)
		{
			Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

			if(layer.composed_Material&&material_Done)
			{
				material_Line_Edit->clear();
				for(int i=0; i<layer.composition.size(); ++i)
				{
					material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
					if( abs(composition_Line_Edit_Vec[i]->text().toDouble() - 1 ) > DBL_EPSILON )
						material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_double_format,line_edit_composition_precision));
				}
			}
			else
				material_Line_Edit->setText(layer.material);

			resize_Line_Edit("",material_Line_Edit);
		}
		if(item_Type==Item_Type::Substrate)
		{
			Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

			if(substrate.composed_Material&&material_Done)
			{
				material_Line_Edit->clear();
				for(int i=0; i<substrate.composition.size(); ++i)
				{
					material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
					if( abs(composition_Line_Edit_Vec[i]->text().toDouble() - 1 ) > DBL_EPSILON )
						material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_double_format,line_edit_composition_precision));
				}
			}
			else
				material_Line_Edit->setText(substrate.material);

			resize_Line_Edit("",material_Line_Edit);
		}
	}
}

void Item_Editor::show_Density()
{
	//if(material_Done)
	{
		if(item_Type==Item_Type::Ambient)
		{
			Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
			if(ambient.composed_Material)
			{
				density_Line_Edit->setText(QString::number(ambient.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
			} else
			{
				density_Line_Edit->setText(QString::number(ambient.relative_Density.value,line_edit_double_format,line_edit_density_precision));
			}
			resize_Line_Edit("",density_Line_Edit);
		}
		if(item_Type==Item_Type::Layer)
		{
			Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
			if(layer.composed_Material)
			{
				density_Line_Edit->setText(QString::number(layer.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
			} else
			{
				density_Line_Edit->setText(QString::number(layer.relative_Density.value,line_edit_double_format,line_edit_density_precision));
			}
			resize_Line_Edit("",density_Line_Edit);
		}
		if(item_Type==Item_Type::Substrate)
		{
			Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
			if(substrate.composed_Material)
			{
				density_Line_Edit->setText(QString::number(substrate.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
			} else
			{
				density_Line_Edit->setText(QString::number(substrate.relative_Density.value,line_edit_double_format,line_edit_density_precision));
			}
			resize_Line_Edit("",density_Line_Edit);
		}
	}
}

void Item_Editor::show_Thickness()
{
	if(thickness_Done)
	{
		double coeff = length_Coefficients_Map.value(length_units);

		if(item_Type==Item_Type::Layer)
		{
			thickness_Label->setText(thickness_Label_1 + length_units + thickness_Label_2);

			Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
			thickness_Line_Edit->setText(QString::number(layer.thickness.value/coeff,line_edit_double_format,line_edit_thickness_precision));
			resize_Line_Edit("",thickness_Line_Edit);
		}
	}
}

void Item_Editor::show_Sigma_Start()
{
	double coeff = length_Coefficients_Map.value(length_units);

	if(item_Type==Item_Type::Layer)
	{
		roughness_Label->setText(sigma_Label_1+ length_units + sigma_Label_2);

		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		sigma_Line_Edit->setText(QString::number(layer.sigma.value/coeff,line_edit_double_format,line_edit_sigma_precision));
	}
	if(item_Type==Item_Type::Substrate)
	{
		roughness_Label->setText(sigma_Label_1+ length_units + sigma_Label_2);

		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		sigma_Line_Edit->setText(QString::number(substrate.sigma.value/coeff,line_edit_double_format,line_edit_sigma_precision));
	}
}

void Item_Editor::show_Sigma()
{
	// TODO extreme layers
	if(sigma_Done)
	{
		show_Sigma_Start();
		resize_Line_Edit("",sigma_Line_Edit);
	}
}

void Item_Editor::show_Stack_Parameters()
{
	if(item_Type==Item_Type::Stack_Content)
	{
		double coeff = length_Coefficients_Map.value(length_units);
		//if(stack_Done)
		{
			period_Label->setText(period_Label_1 + length_units + period_Label_2);

			Stack_Content stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();			
			repetitions_Line_Edit->setText(QString::number(stack_Content.num_Repetition.value));
			period_Line_Edit->setText(QString::number(stack_Content.period.value/coeff,line_edit_double_format,line_edit_period_precision));
			gamma_Line_Edit ->setText(QString::number(stack_Content.gamma. value,line_edit_double_format,line_edit_gamma_precision));

			resize_Line_Edit("",repetitions_Line_Edit);
			resize_Line_Edit("",period_Line_Edit);
			resize_Line_Edit("",gamma_Line_Edit);
		}
	}
}

void Item_Editor::show_Interlayers()
{
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(layer.interlayer_Composition.size() == interlayer_Composition_Line_Edit_Vec.size())
		for(int i=0; i<layer.interlayer_Composition.size(); ++i)
		{
			interlayer_Composition_Line_Edit_Vec[i]->setText(QString::number(layer.interlayer_Composition[i].interlayer.value,line_edit_double_format,line_edit_interlayer_precision));
		}

	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		if(substrate.interlayer_Composition.size() == interlayer_Composition_Line_Edit_Vec.size())
		for(int i=0; i<substrate.interlayer_Composition.size(); ++i)
		{
			interlayer_Composition_Line_Edit_Vec[i]->setText(QString::number(substrate.interlayer_Composition[i].interlayer.value,line_edit_double_format,line_edit_interlayer_precision));
		}
	}
}

void Item_Editor::done_Slot()
{
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

		if(optical_Constants->material_Map.contains(ambient.material + nk_Ext) || ambient.composed_Material)
		{
			ambient.approved_Material = ambient.material;
			var.setValue( ambient );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			close();
		} else
		{
			material_Line_Edit->setText(ambient.approved_Material);
			material_Line_Edit->textEdited(material_Line_Edit->text());
			QMessageBox::information(this, "Wrong material", "File \"" + ambient.material + nk_Ext + "\" not found");
		}
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(optical_Constants->material_Map.contains(layer.material + nk_Ext) || layer.composed_Material)
		{
			layer.approved_Material = layer.material;
			var.setValue( layer );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			close();
		} else
		{
			material_Line_Edit->setText(layer.approved_Material);
			material_Line_Edit->textEdited(material_Line_Edit->text());
			QMessageBox::information(this, "Wrong material", "File \"" + layer.material + nk_Ext + "\" not found");
		}
	}
	if(item_Type==Item_Type::Stack_Content)
	{
		close();
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

		if(optical_Constants->material_Map.contains(substrate.material + nk_Ext) || substrate.composed_Material)
		{
			substrate.approved_Material = substrate.material;
			var.setValue( substrate );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
			close();
		} else
		{
			material_Line_Edit->setText(substrate.approved_Material);
			material_Line_Edit->textEdited(material_Line_Edit->text());
			QMessageBox::information(this, "Wrong material", "File \"" + substrate.material + nk_Ext + "\" not found");
		}
	}
}

void Item_Editor::resize_Line_Edit(QString text, QLineEdit* line_Edit)
{
	if(line_Edit == NULL) line_Edit = qobject_cast<QLineEdit*>(QObject::sender());

	text = line_Edit->text();
	QFontMetrics fm = line_Edit->fontMetrics();
	int width = fm.width(text) + QLINEEDIT_RESIZE_MARGIN;
	if(width>line_Edit->property(min_Size_Property).toInt())
	{
		line_Edit->setFixedWidth(width);
		QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
	} else
	{
		line_Edit->setFixedWidth(line_Edit->property(min_Size_Property).toInt());
		QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
	}
}

void Item_Editor::browse_Material(bool)
{
	// TODO
    qInfo() << "browse...";
}

void Item_Editor::depth_Grading(bool)
{
	Grading_Editor* depth_Grading = new Grading_Editor(item, whats_This_Thickness);
		depth_Grading->setParent(this);
		depth_Grading->setModal(true);
		depth_Grading->setWindowFlags(Qt::Window);
		depth_Grading->show();

	connect(depth_Grading, SIGNAL(refresh()), this, SLOT(emit_Refresh()));
}

void Item_Editor::sigma_Grading(bool)
{
	Grading_Editor* sigma_Grading = new Grading_Editor(item, whats_This_Sigma);
		sigma_Grading->setParent(this);
		sigma_Grading->setModal(true);
		sigma_Grading->setWindowFlags(Qt::Window);
		sigma_Grading->show();

	connect(sigma_Grading, SIGNAL(refresh()), this, SLOT(emit_Refresh()));
}

void Item_Editor::norm_Interlayer_Composition()
{
	if(sigma_Done)
	{
		double sum=0;
		for(int i=0; i<interlayer_Composition_Line_Edit_Vec.size(); ++i)
		{
			if(interlayer_Composition_Check_Box_Vec[i]->isChecked() == true)
				sum+=interlayer_Composition_Line_Edit_Vec[i]->text().toDouble();
		}
		for(int i=0; i<interlayer_Composition_Line_Edit_Vec.size(); ++i)
		{
			if(interlayer_Composition_Check_Box_Vec[i]->isChecked() == true)
				interlayer_Composition_Line_Edit_Vec[i]->setText(QString::number(interlayer_Composition_Line_Edit_Vec[i]->text().toDouble()/sum,line_edit_double_format,line_edit_interlayer_precision));
			resize_Line_Edit("",interlayer_Composition_Line_Edit_Vec[i]);
		}
	}
}

void Item_Editor::interlayer_Check(int)
{
	int counter=0;
	if(sigma_Done)
	{
		for(int i=0; i<interlayer_Composition_Check_Box_Vec.size(); ++i)
		{
			if(interlayer_Composition_Check_Box_Vec[i]->isChecked() == true)
			{
				counter++;
				interlayer_Composition_Line_Edit_Vec[i]->setDisabled(false);
			} else
			{
				interlayer_Composition_Line_Edit_Vec[i]->setDisabled(true);
			}
		}
		if(counter==0)
		{
			sigma_Line_Edit->setDisabled(true);
		} else
		{
			sigma_Line_Edit->setDisabled(false);
		}
	}
	refresh_Data();
}

void Item_Editor::refresh_Data(QString str)
{
	double coeff = length_Coefficients_Map.value(length_units);
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		if(material_Done)
		{
			if(ambient.composed_Material)
			{
				ambient.absolute_Density.value = density_Line_Edit->text().toDouble();
			} else
			{
				ambient.relative_Density.value = density_Line_Edit->text().toDouble();
			}
			for(int i=0; i<ambient.composition.size(); ++i)
			{
				ambient.composition[i].composition.value = composition_Line_Edit_Vec[i]->text().toDouble();
				ambient.composition[i].type = composition_Combo_Box_Vec[i]->currentText();
				composition_At_Weight_Vec[i]->setText(AtWt + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),thumbnail_double_format,at_weight_precision) + ")");
			}
		}
		var.setValue( ambient );
		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

		if(material_Done)
		{
			if(layer.composed_Material)
			{
				layer.absolute_Density.value = density_Line_Edit->text().toDouble();
			} else
			{
				layer.relative_Density.value = density_Line_Edit->text().toDouble();
			}
			for(int i=0; i<layer.composition.size(); ++i)
			{
				layer.composition[i].composition.value = composition_Line_Edit_Vec[i]->text().toDouble();
				layer.composition[i].type = composition_Combo_Box_Vec[i]->currentText();
				composition_At_Weight_Vec[i]->setText(AtWt + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),thumbnail_double_format,at_weight_precision) + ")");
			}
		}
		if(thickness_Done)
		{
			layer.thickness.value = thickness_Line_Edit->text().toDouble()*coeff;
		}
		if(sigma_Done)
		{
			layer.sigma.value = sigma_Line_Edit->text().toDouble()*coeff;
			for(int i=0; i<layer.interlayer_Composition.size(); ++i)
			{
				layer.interlayer_Composition[i].enabled = interlayer_Composition_Check_Box_Vec[i]->isChecked();
				layer.interlayer_Composition[i].interlayer.value = interlayer_Composition_Line_Edit_Vec[i]->text().toDouble();
			}
		}
		//TODO other fields
		var.setValue( layer );
		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		if(material_Done)
		{
			if(substrate.composed_Material)
			{
				substrate.absolute_Density.value = density_Line_Edit->text().toDouble();
			} else
			{
				substrate.relative_Density.value = density_Line_Edit->text().toDouble();
			}
			for(int i=0; i<substrate.composition.size(); ++i)
			{
				substrate.composition[i].composition.value = composition_Line_Edit_Vec[i]->text().toDouble();
				substrate.composition[i].type = composition_Combo_Box_Vec[i]->currentText();
				composition_At_Weight_Vec[i]->setText(AtWt + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),thumbnail_double_format,at_weight_precision) + ")");
			}
		}
		if(sigma_Done)
		{
			substrate.sigma.value = sigma_Line_Edit->text().toDouble()*coeff;
			for(int i=0; i<substrate.interlayer_Composition.size(); ++i)
			{
				substrate.interlayer_Composition[i].enabled = interlayer_Composition_Check_Box_Vec[i]->isChecked();
				substrate.interlayer_Composition[i].interlayer.value = interlayer_Composition_Line_Edit_Vec[i]->text().toDouble();
			}
		}
		var.setValue( substrate );
		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}

	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		if(ambient.composed_Material && material_Done)
		{
			material_Line_Edit->clear();
			for(int i=0; i<ambient.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if( abs(composition_Line_Edit_Vec[i]->text().toDouble() - 1 ) > DBL_EPSILON )
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_double_format,line_edit_composition_precision));
				resize_Line_Edit("",material_Line_Edit);
			}
		}
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		if(layer.composed_Material && material_Done)
		{
			material_Line_Edit->clear();
			for(int i=0; i<layer.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if( abs(composition_Line_Edit_Vec[i]->text().toDouble() - 1.) > DBL_EPSILON)
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_double_format,line_edit_composition_precision));
				resize_Line_Edit("",material_Line_Edit);
			}
		}
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		if(substrate.composed_Material && material_Done)
		{
			material_Line_Edit->clear();
			for(int i=0; i<substrate.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if( abs(composition_Line_Edit_Vec[i]->text().toDouble()) > DBL_EPSILON )
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_double_format,line_edit_composition_precision));
				resize_Line_Edit("",material_Line_Edit);
			}
		}
	}

	emit edited();
}

void Item_Editor::refresh_Stack_Data()
{
	QVariant var;
	if(stack_Done)
	{
		double coeff = length_Coefficients_Map.value(length_units);
		if(item_Type==Item_Type::Stack_Content)
		{
			Stack_Content stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
			double init_Period = stack_Content.period.value;

			stack_Content.num_Repetition.value = repetitions_Line_Edit->text().toInt();
			stack_Content.period.value = period_Line_Edit->text().toDouble()*coeff;
			if(item->childCount()==2)
			{
				double temp_Gamma = gamma_Line_Edit->text().toDouble();
				if(temp_Gamma>1)
				{
					gamma_Line_Edit->setText(QString::number(stack_Content.gamma.value));
					resize_Line_Edit("",gamma_Line_Edit);
				} else
				{
					stack_Content.gamma.value = gamma_Line_Edit->text().toDouble();
				}
			}
			var.setValue( stack_Content );
			item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			// children of stack
			if( abs(init_Period) > DBL_EPSILON )
			{
				if(item->childCount()==2)
				{
					change_Stack_Gamma(stack_Content);
				} else
				{
					if( abs(init_Period) > DBL_EPSILON )
					{
						double factor = stack_Content.period.value / init_Period;
						change_Child_Layers_Thickness(item, factor);
						init_Period = stack_Content.period.value;
					}
				}
			}
		}
	}

	emit edited();
}

void Item_Editor::fast_Refresh_Stack(QString)
{
	Stack_Content sc = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
	if( abs(period_Line_Edit->text().toDouble()) > DBL_EPSILON || abs(sc.period.value) < DBL_EPSILON )
	{
		if(repetitions_Line_Edit->text().toInt()!=0 || sc.num_Repetition.value == 0)
		{
			if(item->childCount()==2)
			{
				if( abs(gamma_Line_Edit->text().toDouble()) > DBL_EPSILON || abs(sc.gamma.value) < DBL_EPSILON )
				{
					if( abs(gamma_Line_Edit->text().toDouble() - 1.) > DBL_EPSILON || abs(sc.gamma.value - 1) < DBL_EPSILON )
					{
						refresh_Stack_Data();
					}
				}
			} else
			{
				refresh_Stack_Data();
			}
		}
	}
}

void Item_Editor::change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor)
{
	QVariant var;
	for(int i=0; i<multilayer_Item->childCount(); ++i)
	{
		// if layer
		if(multilayer_Item->child(i)->childCount()==0)
		{
			Layer layer = multilayer_Item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
			layer.thickness.value = layer.thickness.value*factor;
			var.setValue( layer );
			multilayer_Item->child(i)->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		} else
		// if multilayer
		{
			change_Child_Layers_Thickness(multilayer_Item->child(i), factor);
		}
	}
}

void Item_Editor::change_Stack_Gamma(const Stack_Content& stack_Content)
{
	int i=0;
	{
		double new_Thickness = stack_Content.period.value*stack_Content.gamma.value;
		if(item->child(i)->childCount()==0)
		{
			reset_Layer_Thickness(item->child(i), new_Thickness);
		}
		if(item->child(i)->childCount()>0)
		{
			reset_Multilayer_Thickness(item->child(i), new_Thickness);
		}
	}
	i=1;
	{
		double new_Thickness = stack_Content.period.value*(1-stack_Content.gamma.value);
		if(item->child(i)->childCount()==0)
		{
			reset_Layer_Thickness(item->child(i), new_Thickness);
		}
		if(item->child(i)->childCount()>0)
		{
			reset_Multilayer_Thickness(item->child(i), new_Thickness);
		}
	}
}

void Item_Editor::reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness)
{
	QVariant var;
	Layer layer = layer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
	layer.thickness.value = new_Thickness;
	var.setValue( layer );
	layer_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

void Item_Editor::reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness)
{
	Stack_Content stack_Content = multilayer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
	double factor=1;
	if( abs(stack_Content.period.value) > DBL_EPSILON && stack_Content.num_Repetition.value!=0)
	{
		factor = new_Thickness/(  stack_Content.period.value*stack_Content.num_Repetition.value  );
		change_Child_Layers_Thickness(multilayer_Item, factor);
	}
}
