#include "item_editing.h"

Item_Editing::Item_Editing(QTreeWidgetItem* item):
	item(item)
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
	norm_Interlayer_Composition();
	refresh_Data();
	refresh_Material();
	refresh_Stack_Data();
	emit is_Closed();
	emit is_Edited();
	event;
}

void Item_Editing::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);

	create_Menu();
		main_Layout->setMenuBar(menu_Bar);

	if(item->whatsThis(default_Column)==whats_This_Ambient)
	{
		item_Type = Item_Type::Ambient;
		make_Ambient_Editor();
	} else
	if(item->whatsThis(default_Column)==whats_This_Substrate)
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

	connect(done_Button, SIGNAL(clicked()), this, SLOT(close()));

	main_Layout->setContentsMargins(4,0,4,0);
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
	QVBoxLayout* material_Group_Box_Layout = new QVBoxLayout(material_Group_Box);

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
			browse_Material_Button->setFocusPolicy(Qt::NoFocus);

		QLabel* density_Label = new QLabel("Density [g/cm" + Cube_Sym + "]:");
			layout->addWidget(density_Label);

		density_Line_Edit = new QLineEdit;
		density_Line_Edit->setFixedWidth(50);
		density_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			show_Density();
			layout->addWidget(density_Line_Edit);

		material_Group_Box_Layout->addLayout(layout);

		connect(material_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Material(QString)));
		connect(density_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Data(QString)));
		connect(browse_Material_Button, SIGNAL(clicked(bool)), this, SLOT(browse_Material(bool)));
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
		if(item_Type==Item_Type::Ambient)
		{
			if(!item->data(default_Column, Qt::UserRole).value<Ambient>().composed_Material)	{filename_Radio->setChecked(true);    filename_Radio_Toggled(true);}
			if( item->data(default_Column, Qt::UserRole).value<Ambient>().composed_Material)	{composition_Radio->setChecked(true); composition_Radio_Toggled(true);}
		}
		if(item_Type==Item_Type::Layer)
		{
			if(!item->data(default_Column, Qt::UserRole).value<Layer>().composed_Material)		{filename_Radio->setChecked(true);    filename_Radio_Toggled(true);}
			if( item->data(default_Column, Qt::UserRole).value<Layer>().composed_Material)		{composition_Radio->setChecked(true); composition_Radio_Toggled(true);}
		}
		if(item_Type==Item_Type::Substrate)
		{
			if(!item->data(default_Column, Qt::UserRole).value<Substrate>().composed_Material)	{filename_Radio->setChecked(true);    filename_Radio_Toggled(true);}
			if( item->data(default_Column, Qt::UserRole).value<Substrate>().composed_Material)	{composition_Radio->setChecked(true); composition_Radio_Toggled(true);}
		}
	}
}

void Item_Editing::make_Layer_Editor()
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

void Item_Editing::make_Thickness_Group_Box()
{
	{
		thickness_Group_Box = new QGroupBox;
		thickness_Group_Box->setObjectName("thickness_Group_Box");
		thickness_Group_Box->setStyleSheet("QGroupBox#thickness_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
		QVBoxLayout* thickness_Group_Box_Layout = new QVBoxLayout(thickness_Group_Box);
		thickness_Group_Box_Layout->setAlignment(Qt::AlignTop);

		QHBoxLayout* layout = new QHBoxLayout;
		QLabel* thickness_Label = new QLabel("Thickness, z [" + Angstrom_Sym + "]:");
		layout->addWidget(thickness_Label);

		thickness_Line_Edit = new QLineEdit;
		thickness_Line_Edit->setFixedWidth(50);
		thickness_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
		show_Thickness();
		layout->addWidget(thickness_Line_Edit);
		thickness_Group_Box_Layout->addLayout(layout);

		depth_Grading_Button = new QPushButton(" Depth Grading");
			depth_Grading_Button->adjustSize();
			depth_Grading_Button->setFixedSize(depth_Grading_Button->size());
			depth_Grading_Button->setFocusPolicy(Qt::NoFocus);

		thickness_Group_Box_Layout->addWidget(depth_Grading_Button,0,Qt::AlignRight);

		connect(thickness_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Data(QString)));
	}
	{
		thickness_Done = true;
	}
}

void Item_Editing::make_Multilayer_Editor()
{
	make_Multilayer_Group_Box();
		main_Layout->addWidget(multilayer_Group_Box);
}

void Item_Editing::make_Multilayer_Group_Box()
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
		repetitions_Line_Edit->setValidator(new QIntValidator(0, MAX_INTEGER));
			layout->addWidget(repetitions_Line_Edit);

		multilayer_Group_Box_Layout->addLayout(layout);
	}
	{
		QHBoxLayout* layout = new QHBoxLayout;
		QLabel* period_Label = new QLabel("Period, d[" + Angstrom_Sym + "]:");
			layout->addWidget(period_Label);

		period_Line_Edit = new QLineEdit;
		period_Line_Edit->setFixedWidth(50);
		period_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			layout->addWidget(period_Line_Edit);

		multilayer_Group_Box_Layout->addLayout(layout);
	}
	{
		QHBoxLayout* layout = new QHBoxLayout;
		gamma_Label = new QLabel("Thickness Ratio, " + Gamma_Sym + ":");

		gamma_Line_Edit = new QLineEdit;
		gamma_Line_Edit->setFixedWidth(50);
		// TODO gamma check
		gamma_Line_Edit->setValidator(new QDoubleValidator(0, 1, MAX_PRECISION));

		if(item->childCount()==2)
		{
			layout->addWidget(gamma_Label);
			layout->addWidget(gamma_Line_Edit);

			multilayer_Group_Box_Layout->addLayout(layout);
		}
	}
	{
		stack_Done = true;
		show_Stack_Parameters();
	}

	connect(repetitions_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Stack(QString)));
	connect(period_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Stack(QString)));
	connect(gamma_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Stack(QString)));
}

void Item_Editing::make_Substrate_Editor()
{
	make_Materials_Group_Box();
		main_Layout->addWidget(material_Group_Box);
	make_Sigma_Group_Box();
		main_Layout->addWidget(sigma_Group_Box);
}

void Item_Editing::make_Sigma_Group_Box()
{
	sigma_Group_Box = new QGroupBox;
	sigma_Group_Box->setObjectName("sigma_Group_Box");
	sigma_Group_Box->setStyleSheet("QGroupBox#sigma_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
	QVBoxLayout* sigma_Group_Box_Layout = new QVBoxLayout(sigma_Group_Box);

	// sigma
	{
		QFrame* frame= new QFrame;
		frame->setContentsMargins(-6,-7,-6,-7);
		QHBoxLayout* layout = new QHBoxLayout(frame);
		layout->setAlignment(Qt::AlignLeft);
		QLabel* roughness_Label = new QLabel("Roughness/Diffuseness, " + Sigma_Sym + "["+ Angstrom_Sym + "]:");
			layout->addWidget(roughness_Label,0,Qt::AlignLeft);

		sigma_Line_Edit = new QLineEdit;
		sigma_Line_Edit->setFixedWidth(50);
		sigma_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
			show_Sigma();
			layout->addWidget(sigma_Line_Edit,0,Qt::AlignLeft);

		sigma_Group_Box_Layout->addWidget(frame);

		connect(sigma_Line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Data(QString)));
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
	}
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
		ambient.composed_Material = false;	var.setValue( ambient );
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		layer.composed_Material = false;	var.setValue( layer );
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		substrate.composed_Material = false;	var.setValue( substrate );
	}
	item->setData(default_Column, Qt::UserRole, var);

	composition_Frame->hide();
	read_Elements_From_Item();
	show_Material();	

	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
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
		ambient.composed_Material = true;	var.setValue( ambient );

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
		layer.composed_Material = true;	var.setValue( layer );

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
		substrate.composed_Material = true;	var.setValue( substrate );

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
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Item_Editing::more_Elements_Clicked(bool)
{
	if (element_Frame_Vec.size()==0)	{fewer_Elements->hide();}
	else								{fewer_Elements->show();}

	QVariant var;
	Stoichiometry stoich;

	QLineEdit* line_Edit = new QLineEdit;		line_Edit->setFixedWidth(50); line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));
	QLabel*    label = new QLabel("atom of");
	QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
	QLabel*	  at_Weight = new QLabel;

	connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(fast_Refresh_Data(QString)));
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
	elements->setCurrentIndex(elements->findText(stoich.type));
	at_Weight->setText("At. Wt.=" + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

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
		composition_Line_Edit_Vec.first()->setText(QString::number(1,line_edit_double_format,line_edit_composition_precision));
		fewer_Elements->hide();
	}
	if (element_Frame_Vec.size()>=2) {composition_Line_Edit_Vec.first()->setDisabled(false);fewer_Elements->show();}
}

void Item_Editing::read_Elements_From_Item()
{
	// clear all previous widgets
	for(int i=0; i<element_Frame_Vec.size(); ++i)
		delete element_Frame_Vec[i];

	// reading data
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();

		composition_Line_Edit_Vec.resize(ambient.composition.size());
		composition_Label_Vec.	  resize(ambient.composition.size());
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

			line_Edit->setText(QString::number(ambient.composition[i].composition.value,line_edit_double_format,line_edit_composition_precision));
			elements->setCurrentIndex(elements->findText(ambient.composition[i].type));
			at_Weight->setText("At. Wt.=" + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

			composition_Line_Edit_Vec[i]=line_Edit;
			composition_Label_Vec	 [i]=label;
			composition_Combo_Box_Vec[i]=elements;
			composition_At_Weight_Vec[i]=at_Weight;

			connect(line_Edit, SIGNAL(textEdited(QString)),			this, SLOT(fast_Refresh_Data(QString)));
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
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();

		composition_Line_Edit_Vec.resize(layer.composition.size());
		composition_Label_Vec.	  resize(layer.composition.size());
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

			line_Edit->setText(QString::number(layer.composition[i].composition.value,line_edit_double_format,line_edit_composition_precision));
			elements->setCurrentIndex(elements->findText(layer.composition[i].type));
			at_Weight->setText("At. Wt.=" + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

			composition_Line_Edit_Vec[i]=line_Edit;
			composition_Label_Vec	 [i]=label;
			composition_Combo_Box_Vec[i]=elements;
			composition_At_Weight_Vec[i]=at_Weight;

			connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Data(QString)));
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

			line_Edit->setText(QString::number(substrate.composition[i].composition.value,line_edit_double_format,line_edit_composition_precision));
			elements->setCurrentIndex(elements->findText(substrate.composition[i].type));
			at_Weight->setText("At. Wt.=" + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

			composition_Line_Edit_Vec[i] = line_Edit;
			composition_Label_Vec	 [i] = label;
			composition_Combo_Box_Vec[i] = elements;
			composition_At_Weight_Vec[i] = at_Weight;

			connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Data(QString)));
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
		composition_Line_Edit_Vec.first()->setText(QString::number(1,line_edit_double_format,line_edit_composition_precision));
		fewer_Elements->hide();
	}
	if (element_Frame_Vec.size()>=2) {composition_Line_Edit_Vec.first()->setDisabled(false);fewer_Elements->show();}
}

void Item_Editing::read_Interlayers_From_Item()
{
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();

		interlayer_Composition_Check_Box_Vec.resize(layer.interlayer_Composition.size());
		interlayer_Composition_Line_Edit_Vec.resize(layer.interlayer_Composition.size());

		// renew ui
		for(int i=0; i<layer.interlayer_Composition.size(); ++i)
		{
			// creating ui elements
			QCheckBox* check_Box = new QCheckBox(transition_Layer_Functions[i]);
			QLineEdit* line_Edit = new QLineEdit;		line_Edit->setFixedWidth(40); line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

			check_Box->setChecked(layer.interlayer_Composition[i].enabled);
			line_Edit->setText(QString::number(layer.interlayer_Composition[i].interlayer.value,line_edit_double_format,line_edit_interlayer_precision));

			interlayer_Composition_Check_Box_Vec[i]=check_Box;
			interlayer_Composition_Line_Edit_Vec[i]=line_Edit;

			connect(check_Box, SIGNAL(stateChanged(int)),	this, SLOT(interlayer_Check(int)));
			connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Data(QString)));

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
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();

		interlayer_Composition_Check_Box_Vec.resize(substrate.interlayer_Composition.size());
		interlayer_Composition_Line_Edit_Vec.resize(substrate.interlayer_Composition.size());

		// renew ui
		for(int i=0; i<substrate.interlayer_Composition.size(); ++i)
		{
			// creating ui elements
			QCheckBox* check_Box = new QCheckBox(transition_Layer_Functions[i]);
			QLineEdit* line_Edit = new QLineEdit;		line_Edit->setFixedWidth(40); line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

			check_Box->setChecked(substrate.interlayer_Composition[i].enabled);
			line_Edit->setText(QString::number(substrate.interlayer_Composition[i].interlayer.value,line_edit_double_format,line_edit_interlayer_precision));

			interlayer_Composition_Check_Box_Vec[i]=check_Box;
			interlayer_Composition_Line_Edit_Vec[i]=line_Edit;

			connect(check_Box, SIGNAL(stateChanged(int)),	this, SLOT(interlayer_Check(int)));
			connect(line_Edit, SIGNAL(textEdited(QString)), this, SLOT(fast_Refresh_Data(QString)));

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

	interlayer_Check(true);
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
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(QString::number(1,line_edit_double_format,line_edit_composition_precision));
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
		item->setData(default_Column, Qt::UserRole, var);
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		layer.material = material_Line_Edit->text();
		var.setValue( layer );
		item->setData(default_Column, Qt::UserRole, var);
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		substrate.material = material_Line_Edit->text();
		var.setValue( substrate );
		item->setData(default_Column, Qt::UserRole, var);
	}	

	emit is_Edited();
}

void Item_Editing::fast_Refresh_Material(QString)
{
	refresh_Material();
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
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		density_Line_Edit->setText(QString::number(ambient.density.value,line_edit_double_format,line_edit_density_precision));
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		density_Line_Edit->setText(QString::number(layer.density.value,line_edit_double_format,line_edit_density_precision));
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		density_Line_Edit->setText(QString::number(substrate.density.value,line_edit_double_format,line_edit_density_precision));
	}
}

void Item_Editing::show_Thickness()
{
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		thickness_Line_Edit->setText(QString::number(layer.thickness.value,line_edit_double_format,line_edit_thickness_precision));
	}
}

void Item_Editing::show_Sigma()
{
	// TODO extreme layers
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		sigma_Line_Edit->setText(QString::number(layer.sigma.value,line_edit_double_format,line_edit_sigma_precision));
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		sigma_Line_Edit->setText(QString::number(substrate.sigma.value,line_edit_double_format,line_edit_sigma_precision));
	}
}

void Item_Editing::show_Stack_Parameters()
{
	if(item_Type==Item_Type::Stack_Content)
	{
		if(stack_Done)
		{
			Stack_Content stack_Content = item->data(default_Column, Qt::UserRole).value<Stack_Content>();
			repetitions_Line_Edit->setText(QString::number(stack_Content.num_Repetition.value));
			period_Line_Edit->setText(QString::number(stack_Content.period.value,line_edit_double_format,line_edit_period_precision));
			gamma_Line_Edit ->setText(QString::number(stack_Content.gamma. value,line_edit_double_format,line_edit_gamma_precision));
		}
	}
}

void Item_Editing::browse_Material(bool)
{
	// TODO
	qInfo() << "browse...";
}

void Item_Editing::norm_Interlayer_Composition()
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
		}
	}
}

void Item_Editing::interlayer_Check(int)
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

void Item_Editing::refresh_Data()
{
	QVariant var;
	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		if(material_Done)
		{
			ambient.density.value = density_Line_Edit->text().toDouble();
			for(int i=0; i<ambient.composition.size(); ++i)
			{
				ambient.composition[i].composition.value = composition_Line_Edit_Vec[i]->text().toDouble();
				ambient.composition[i].type = composition_Combo_Box_Vec[i]->currentText();
				composition_At_Weight_Vec[i]->setText("At. Wt.=" + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),thumbnail_double_format,at_weight_precision) + ")");
			}
		}
		var.setValue( ambient );
		item->setData(default_Column, Qt::UserRole, var);
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();

		if(material_Done)
		{
			layer.density.value = density_Line_Edit->text().toDouble();
			for(int i=0; i<layer.composition.size(); ++i)
			{
				layer.composition[i].composition.value = composition_Line_Edit_Vec[i]->text().toDouble();
				layer.composition[i].type = composition_Combo_Box_Vec[i]->currentText();
				composition_At_Weight_Vec[i]->setText("At. Wt.=" + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),thumbnail_double_format,at_weight_precision) + ")");
			}
		}
		if(thickness_Done)
		{
			layer.thickness.value = thickness_Line_Edit->text().toDouble();
		}
		if(sigma_Done)
		{
			layer.sigma.value = sigma_Line_Edit->text().toDouble();
			for(int i=0; i<layer.interlayer_Composition.size(); ++i)
			{
				layer.interlayer_Composition[i].enabled = interlayer_Composition_Check_Box_Vec[i]->isChecked();
				layer.interlayer_Composition[i].interlayer.value = interlayer_Composition_Line_Edit_Vec[i]->text().toDouble();
			}
		}
		//TODO other fields
		var.setValue( layer );
		item->setData(default_Column, Qt::UserRole, var);
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		if(material_Done)
		{
			substrate.density.value = density_Line_Edit->text().toDouble();
			for(int i=0; i<substrate.composition.size(); ++i)
			{
				substrate.composition[i].composition.value = composition_Line_Edit_Vec[i]->text().toDouble();
				substrate.composition[i].type = composition_Combo_Box_Vec[i]->currentText();
				composition_At_Weight_Vec[i]->setText("At. Wt.=" + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),thumbnail_double_format,at_weight_precision) + ")");
			}
		}
		if(sigma_Done)
		{
			substrate.sigma.value = sigma_Line_Edit->text().toDouble();
			for(int i=0; i<substrate.interlayer_Composition.size(); ++i)
			{
				substrate.interlayer_Composition[i].enabled = interlayer_Composition_Check_Box_Vec[i]->isChecked();
				substrate.interlayer_Composition[i].interlayer.value = interlayer_Composition_Line_Edit_Vec[i]->text().toDouble();
			}
		}
		var.setValue( substrate );
		item->setData(default_Column, Qt::UserRole, var);
	}

	if(item_Type==Item_Type::Ambient)
	{
		Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
		if(ambient.composed_Material && material_Done)
		{
			material_Line_Edit->clear();
			for(int i=0; i<ambient.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if(composition_Line_Edit_Vec[i]->text().toDouble()!=1)
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_double_format,line_edit_composition_precision));
			}
		}
	}
	if(item_Type==Item_Type::Layer)
	{
		Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();
		if(layer.composed_Material && material_Done)
		{
			material_Line_Edit->clear();
			for(int i=0; i<layer.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if(composition_Line_Edit_Vec[i]->text().toDouble()!=1)
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_double_format,line_edit_composition_precision));
			}
		}
	}
	if(item_Type==Item_Type::Substrate)
	{
		Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();
		if(substrate.composed_Material && material_Done)
		{
			material_Line_Edit->clear();
			for(int i=0; i<substrate.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if(composition_Line_Edit_Vec[i]->text().toDouble()!=1)
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_double_format,line_edit_composition_precision));
			}
		}
	}

	emit is_Edited();
}

void Item_Editing::refresh_Data(QString)
{
	refresh_Data();
}

void Item_Editing::fast_Refresh_Data(QString)
{
	refresh_Data();
}

void Item_Editing::refresh_Stack_Data()
{
	QVariant var;
	if(stack_Done)
	{
		if(item_Type==Item_Type::Stack_Content)
		{
			Stack_Content stack_Content = item->data(default_Column, Qt::UserRole).value<Stack_Content>();
			double init_Period = stack_Content.period.value;

			stack_Content.num_Repetition.value = repetitions_Line_Edit->text().toInt();
			stack_Content.period.value = period_Line_Edit->text().toDouble();
			if(item->childCount()==2)
			{
				double temp_Gamma = gamma_Line_Edit->text().toDouble();
				if(temp_Gamma>1)
				{
					gamma_Line_Edit->setText(QString::number(stack_Content.gamma.value));
				} else
				{
					stack_Content.gamma.value = gamma_Line_Edit->text().toDouble();
				}
			}
			var.setValue( stack_Content );
			item->setData(default_Column, Qt::UserRole, var);

			// children of stack
			if(init_Period!=0)
			{
				if(item->childCount()==2)
				{
					change_Stack_Gamma(stack_Content);
				} else
				{
					if(init_Period!=0)
					{
						double factor = stack_Content.period.value / init_Period;
						change_Child_Layers_Thickness(item, factor);
						init_Period = stack_Content.period.value;
					}
				}
			}
		}
	}

	emit is_Edited();
}

void Item_Editing::fast_Refresh_Stack(QString)
{
	Stack_Content sc = item->data(default_Column, Qt::UserRole).value<Stack_Content>();
	if(period_Line_Edit->text().toDouble()!=0 || sc.period.value==0)
	{
		if(repetitions_Line_Edit->text().toInt()!=0 || sc.num_Repetition.value==0)
		{
			if(item->childCount()==2)
			{
				if(gamma_Line_Edit->text().toDouble()!=0 || sc.gamma.value==0)
				{
					if(gamma_Line_Edit->text().toDouble()!=1 || sc.gamma.value==1)
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

void Item_Editing::change_Child_Layers_Thickness(QTreeWidgetItem* multilayer_Item, double factor)
{
	QVariant var;
	for(int i=0; i<multilayer_Item->childCount(); ++i)
	{
		// if layer
		if(multilayer_Item->child(i)->childCount()==0)
		{
			Layer layer = multilayer_Item->child(i)->data(default_Column, Qt::UserRole).value<Layer>();
			layer.thickness.value = layer.thickness.value*factor;
			var.setValue( layer );
			multilayer_Item->child(i)->setData(default_Column, Qt::UserRole, var);
		} else
		// if multilayer
		{
			change_Child_Layers_Thickness(multilayer_Item->child(i), factor);
		}
	}
}

void Item_Editing::change_Stack_Gamma(Stack_Content stack_Content)
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

void Item_Editing::reset_Layer_Thickness(QTreeWidgetItem* layer_Item, double new_Thickness)
{
	QVariant var;
	Layer layer = layer_Item->data(default_Column, Qt::UserRole).value<Layer>();
	layer.thickness.value = new_Thickness;
	var.setValue( layer );
	layer_Item->setData(default_Column, Qt::UserRole, var);
}

void Item_Editing::reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness)
{
	Stack_Content stack_Content = multilayer_Item->data(default_Column, Qt::UserRole).value<Stack_Content>();
	double factor=1;
	if(stack_Content.period.value!=0 && stack_Content.num_Repetition.value!=0)
	{
		factor = new_Thickness/(  stack_Content.period.value*stack_Content.num_Repetition.value  );
		change_Child_Layers_Thickness(multilayer_Item, factor);
	}
}
