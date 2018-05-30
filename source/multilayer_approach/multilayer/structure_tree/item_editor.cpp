// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "item_editor.h"

Item_Editor::Item_Editor(QList<Item_Editor*>& list_Editors, QTreeWidgetItem* item, QWidget *parent) :
	item(item),
	list_Editors(list_Editors),
	struct_Data(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>()),
	QDialog(parent)
{
	setWindowTitle(struct_Data.item_Type);
	create_Main_Layout();
	set_Window_Geometry();
}

void Item_Editor::emit_Item_Data_Edited()
{
	show_All();
	emit item_Data_Edited();
}

void Item_Editor::closeEvent(QCloseEvent* event)
{
	if(struct_Data.item_Type == item_Type_Ambient || struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		if(!struct_Data.composed_Material)
		{
			if(optical_Constants->material_Map.contains(struct_Data.material + nk_Ext))
			{
				struct_Data.approved_Material = struct_Data.material;
				close();
			} else
			{
				QMessageBox::information(this, "Wrong material", "File \"" + struct_Data.material + nk_Ext + "\" not found");
				struct_Data.material = struct_Data.approved_Material;
				material_Line_Edit->setText(struct_Data.material);
				resize_Line_Edit(material_Line_Edit->text(),material_Line_Edit);
			}
		}
	}

	norm_Interlayer_Composition();
	refresh_Material();
	refresh_Data();
	emit closed();
	event->accept();
}

void Item_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(3,0,3,0);

	create_Menu();

	if(struct_Data.item_Type == item_Type_Ambient)		make_Ambient_Editor();
	if(struct_Data.item_Type == item_Type_Layer)		make_Layer_Editor();
	if(struct_Data.item_Type == item_Type_Substrate)	make_Substrate_Editor();
	if(struct_Data.item_Type == item_Type_Multilayer)	make_Multilayer_Editor();

	done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		done_Button->setFocus();
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button, &QPushButton::clicked, this, &Item_Editor::close);
}

void Item_Editor::create_Menu()
{
	Menu* menu = new Menu(window_Type_Item_Editor,this);
	main_Layout->setMenuBar(menu->menu_Bar);
	connect(menu, &Menu::refresh, this, &Item_Editor::emit_Item_Data_Edited);
}

void Item_Editor::make_Ambient_Editor()
{
	make_Materials_Group_Box();
		main_Layout->addWidget(material_Group_Box);
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

void Item_Editor::make_Substrate_Editor()
{
	make_Materials_Group_Box();
		main_Layout->addWidget(material_Group_Box);
	make_Sigma_Group_Box();
		main_Layout->addWidget(sigma_Group_Box);
}

void Item_Editor::make_Multilayer_Editor()
{
	make_Multilayer_Group_Box();
		main_Layout->addWidget(multilayer_Group_Box);
}

void Item_Editor::make_Materials_Group_Box()
{
	material_Group_Box = new QGroupBox;
	material_Group_Box->setObjectName("material_Group_Box");
	material_Group_Box->setStyleSheet("QGroupBox#material_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
	QVBoxLayout* material_Group_Box_Layout = new QVBoxLayout(material_Group_Box);
		material_Group_Box_Layout->setSpacing(7);
		material_Group_Box_Layout->setContentsMargins(8,8,8,5);

	// materials
	{
		QHBoxLayout* layout = new QHBoxLayout;
		QLabel* material_Label = new QLabel("Material:");
			layout->addWidget(material_Label);

		material_Line_Edit = new QLineEdit;
			material_Line_Edit->setFixedWidth(70);
			material_Line_Edit->setProperty(min_Size_Property, material_Line_Edit->width());
			layout->addWidget(material_Line_Edit);

		browse_Material_Button = new QPushButton("Browse...");
		browse_Material_Button->setFixedWidth(60);
			layout->addWidget(browse_Material_Button);
			browse_Material_Button->setFocusPolicy(Qt::NoFocus);

		density_Label = new QLabel("<no label>");
			layout->addWidget(density_Label);

		density_Line_Edit = new QLineEdit;
			density_Line_Edit->setFixedWidth(50);
			density_Line_Edit->setProperty(min_Size_Property, density_Line_Edit->width());
			density_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			layout->addWidget(density_Line_Edit);
			show_Density();

		material_Group_Box_Layout->addLayout(layout);

		connect(material_Line_Edit, &QLineEdit::textEdited, this, [&](QString s){resize_Line_Edit(s);});
		connect(density_Line_Edit,  &QLineEdit::textEdited, this, [&](QString s){resize_Line_Edit(s);});

		connect(material_Line_Edit, &QLineEdit::textEdited, this, [=]{refresh_Material();});
		connect(density_Line_Edit,	&QLineEdit::textEdited, this, [=]{refresh_Data();});
		connect(browse_Material_Button,	&QPushButton::clicked, this, [=]{browse_Material();});
	}
	// radio buttons
	{
		QHBoxLayout* layout = new QHBoxLayout;
		filename_Radio = new QRadioButton("Optical constants file name");
			layout->addWidget(filename_Radio);
		composition_Radio = new QRadioButton("Composition of elements");
			layout->addWidget(composition_Radio);
		material_Group_Box_Layout->addLayout(layout);

		connect(filename_Radio,    &QRadioButton::clicked, this, &Item_Editor::filename_Radio_Toggled);
		connect(composition_Radio, &QRadioButton::clicked, this, &Item_Editor::composition_Radio_Toggled);
	}
	// more/fewer elements buttons
	{
		composition_Frame = new QFrame;
		QVBoxLayout* composition_Layout = new QVBoxLayout(composition_Frame);
			composition_Layout->setSpacing(5);
			composition_Layout->setContentsMargins(0,0,0,0);

		composition_Group_Box = new QGroupBox("Composition");
		composition_Group_Box->setObjectName("composition_Group_Box");
		composition_Group_Box->setStyleSheet("QGroupBox#composition_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;} "
											 "QGroupBox::title { subcontrol-origin: margin;	 left: 4px; padding: 0 0px 0 1px;}");

		composition_Layout_With_Elements_Vector = new QVBoxLayout(composition_Group_Box);
			composition_Layout_With_Elements_Vector->setSpacing(7);
			composition_Layout_With_Elements_Vector->setContentsMargins(8,10,8,8);

		QHBoxLayout* buttons_Layout = new QHBoxLayout;
			buttons_Layout->setSpacing(0);
			buttons_Layout->setAlignment(Qt::AlignLeft);

		more_Elements  = new QPushButton(" More elements ");
			more_Elements->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			buttons_Layout->addWidget(more_Elements);
		fewer_Elements = new QPushButton(" Fewer elements ");
			fewer_Elements->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			buttons_Layout->addWidget(fewer_Elements);

		composition_Layout->addWidget(composition_Group_Box);
		composition_Layout->addLayout(buttons_Layout);
		material_Group_Box_Layout->addWidget(composition_Frame);

		connect(more_Elements,  &QPushButton::clicked, this, [=]{more_Elements_Clicked();});
		connect(fewer_Elements, &QPushButton::clicked, this, [=]{fewer_Elements_Clicked();});
	}
	{
		material_Done = true;
		initial_Radio_Check();
	}
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
			thickness_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			layout->addWidget(thickness_Line_Edit);
		thickness_Group_Box_Layout->addLayout(layout);

		depth_Grading_Button = new QPushButton(" Depth Grading");
			depth_Grading_Button->adjustSize();
			depth_Grading_Button->setFixedSize(depth_Grading_Button->size());
			depth_Grading_Button->setFocusPolicy(Qt::NoFocus);
		if(item->parent())
			thickness_Group_Box_Layout->addWidget(depth_Grading_Button,0,Qt::AlignRight);

		connect(depth_Grading_Button,&QPushButton::clicked,  this, [=]{depth_Grading();});
		connect(thickness_Line_Edit, &QLineEdit::textEdited, this, [&](QString s){resize_Line_Edit(s);});
		connect(thickness_Line_Edit, &QLineEdit::textEdited, this, [=]{refresh_Data();});
	}
	{
		thickness_Done = true;
		show_Thickness();
	}
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
			repetitions_Line_Edit->setValidator(new QIntValidator(0, MAX_INTEGER, this));
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
			period_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			layout->addWidget(period_Line_Edit);

		multilayer_Group_Box_Layout->addLayout(layout);
	}
	{
		QHBoxLayout* layout = new QHBoxLayout;
		gamma_Label = new QLabel("Thickness Ratio, " + Gamma_Sym + ":");

		gamma_Line_Edit = new QLineEdit;
			gamma_Line_Edit->setFixedWidth(50);
			gamma_Line_Edit->setProperty(min_Size_Property, gamma_Line_Edit->width());
			gamma_Line_Edit->setValidator(new QDoubleValidator(0, 1, MAX_PRECISION, this));

		if(item->childCount()==2)
		{
			layout->addWidget(gamma_Label);
			layout->addWidget(gamma_Line_Edit);

			multilayer_Group_Box_Layout->addLayout(layout);
		}
	}
	connect(repetitions_Line_Edit,  &QLineEdit::textEdited, this, [&](QString s){resize_Line_Edit(s);});
	connect(period_Line_Edit,		&QLineEdit::textEdited, this, [&](QString s){resize_Line_Edit(s);});
	connect(gamma_Line_Edit,		&QLineEdit::textEdited, this, [&](QString s){resize_Line_Edit(s);});

	connect(repetitions_Line_Edit,	&QLineEdit::textEdited, this, &Item_Editor::fast_Refresh_Stack);
	connect(period_Line_Edit,		&QLineEdit::textEdited, this, &Item_Editor::fast_Refresh_Stack);
	connect(gamma_Line_Edit,		&QLineEdit::textEdited, this, &Item_Editor::fast_Refresh_Stack);
	{
		stack_Done = true;
		show_Stack_Parameters();
	}
}

void Item_Editor::make_Sigma_Group_Box()
{
	sigma_Group_Box = new QGroupBox;
	sigma_Group_Box->setObjectName("sigma_Group_Box");
	sigma_Group_Box->setStyleSheet("QGroupBox#sigma_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
	QGridLayout* sigma_Group_Box_Layout = new QGridLayout(sigma_Group_Box);
		sigma_Group_Box_Layout->setSpacing(13);
		sigma_Group_Box_Layout->setContentsMargins(13,10,10,10);

	// sigma
	{
		QFrame* frame= new QFrame;
		QHBoxLayout* layout = new QHBoxLayout(frame);
			layout->setContentsMargins(0,0,0,0);
			layout->setAlignment(Qt::AlignLeft);
		roughness_Label = new QLabel(sigma_Label_1 + length_units + sigma_Label_2);
			layout->addWidget(roughness_Label,0,Qt::AlignLeft);

		sigma_Line_Edit = new QLineEdit;
			sigma_Line_Edit->setFixedWidth(50);
			sigma_Line_Edit->setProperty(min_Size_Property, sigma_Line_Edit->width());
			sigma_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			layout->addWidget(sigma_Line_Edit,0,Qt::AlignLeft);
			show_Sigma_Start(true);

		sigma_Group_Box_Layout->addWidget(frame);

		sigma_Grading_Button = new QPushButton(" Sigma Grading");
			sigma_Grading_Button->adjustSize();
			sigma_Grading_Button->setFixedSize(sigma_Grading_Button->size());
			sigma_Grading_Button->setFocusPolicy(Qt::NoFocus);
		if(item->parent())
			sigma_Group_Box_Layout->addWidget(sigma_Grading_Button,0,Qt::AlignRight);

		connect(sigma_Grading_Button,	&QPushButton::clicked,  this, &Item_Editor::sigma_Grading);
		connect(sigma_Line_Edit,		&QLineEdit::textEdited, this, [&](QString s){resize_Line_Edit(s);});
		connect(sigma_Line_Edit,		&QLineEdit::textEdited, this, [=]{refresh_Data();});
	}
	// interlayer functions
	{
		interlayer_Composition_Frame = new QFrame;
		QVBoxLayout* interlayer_Composition_Layout = new QVBoxLayout(interlayer_Composition_Frame);
			interlayer_Composition_Layout->setSpacing(0);
			interlayer_Composition_Layout->setContentsMargins(0,0,0,0);
			interlayer_Composition_Layout->setAlignment(Qt::AlignLeft);
			QHBoxLayout* sigma_PSD_Layout = new QHBoxLayout;
		interlayer_Composition_Group_Box = new QGroupBox("Interlayer Composition");
		interlayer_Composition_Group_Box->setObjectName("interlayer_Composition_Group_Box");
		interlayer_Composition_Group_Box->setStyleSheet("QGroupBox#interlayer_Composition_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;} "
														"QGroupBox::title { subcontrol-origin: margin;	 left: 4px; padding: 0 0px 0 1px;}");

		interlayer_Composition_Layout_With_Elements_Vector = new QHBoxLayout(interlayer_Composition_Group_Box);
			sigma_PSD_Layout->addWidget(interlayer_Composition_Group_Box);

		QFrame* PSD_Frame = new QFrame;
		QVBoxLayout* PSD_Layout = new QVBoxLayout(PSD_Frame);
			PSD_Layout->setContentsMargins(10,0,0,0);
		PSD_Check_Box = new QCheckBox("Use PSD");
			PSD_Check_Box->setDisabled(true);
		individual_Sigma_Check_Box = new QCheckBox("Use many " + Sigma_Sym);
			PSD_Layout->addWidget(PSD_Check_Box);
			PSD_Layout->addWidget(individual_Sigma_Check_Box);
		sigma_PSD_Layout->addWidget(PSD_Frame);

		connect(individual_Sigma_Check_Box, &QCheckBox::stateChanged, this, &Item_Editor::interlayer_Check);

		// TODO PSD
		interlayer_Composition_Layout->addLayout(sigma_PSD_Layout);
		sigma_Group_Box_Layout->addWidget(interlayer_Composition_Frame);
	}
	{
		sigma_Done = true;
		read_Interlayers_From_Item();
		show_Sigma(true);
	}
}

void Item_Editor::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
	move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

void Item_Editor::reload_And_Show_All()
{
	struct_Data = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	show_All();
}

void Item_Editor::show_All()
{	
	show_Material();
	show_Density();
	show_Thickness();
	show_Sigma(true);
	show_Interlayers();
	show_Stack_Parameters();
}

void Item_Editor::initial_Radio_Check()
{
	if(!struct_Data.composed_Material)	{filename_Radio->setChecked(true);    filename_Radio_Toggled();}
	if( struct_Data.composed_Material)	{composition_Radio->setChecked(true); composition_Radio_Toggled();}
}

void Item_Editor::filename_Radio_Toggled()
{
	material_Line_Edit->setReadOnly(false);
	browse_Material_Button->setEnabled(true);
	density_Label->setText(relative_Density_Label);

	// composition or filename
	struct_Data.composed_Material = false;

	composition_Frame->hide();
	show_Material();

	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Item_Editor::composition_Radio_Toggled()
{
	density_Label->setText(absolute_Density_Label);
	material_Line_Edit->setReadOnly(true);
	browse_Material_Button->setEnabled(false);

	// composition or filename
	struct_Data.composed_Material = true;

	// add element row if no data
	if(struct_Data.composition.size()==0)
	{
		more_Elements_Clicked();
	}
	// if data already exists
	else
	{
		read_Elements_From_Item();
	}
	composition_Frame->show();

	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
	QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
}

void Item_Editor::more_Elements_Clicked()
{
	if (element_Frame_Vec.size()==0)	{fewer_Elements->hide();}
	else								{fewer_Elements->show();}

	Stoichiometry stoich;

	QLineEdit* line_Edit = new QLineEdit;
		line_Edit->setFixedWidth(50);
		line_Edit->setProperty(min_Size_Property, line_Edit->width());
		line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
	QLabel*    label = new QLabel(atoms);
	QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
	QLabel*	  at_Weight = new QLabel;

	connect(line_Edit, &QLineEdit::textEdited, this, [&](QString s){resize_Line_Edit(s);});

	// loading settings
	if(struct_Data.item_Type==item_Type_Ambient)
	{
		stoich.composition.value = ambient_default_stoichiometry_composition;
		stoich.type				 = ambient_default_stoichiometry_element;
	}
	if(struct_Data.item_Type==item_Type_Layer)
	{
		stoich.composition.value = layer_default_stoichiometry_composition;
		stoich.type				 = layer_default_stoichiometry_element;
	}
	if(struct_Data.item_Type==item_Type_Substrate)
	{
		stoich.composition.value = substrate_default_stoichiometry_composition;
		stoich.type				 = substrate_default_stoichiometry_element;
	}

	// set item_Id
	stoich.composition.indicator.item_Id = struct_Data.id;

	// creating ui elements
	line_Edit->setText(QString::number(stoich.composition.value,line_edit_short_double_format,line_edit_composition_precision));
	resize_Line_Edit("",line_Edit);
	elements->setCurrentIndex(elements->findText(stoich.type));
	at_Weight->setText(AtWt + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

	composition_Line_Edit_Vec.append(line_Edit);
	composition_Label_Vec.append(label);
	composition_Combo_Box_Vec.append(elements);
	composition_At_Weight_Vec.append(at_Weight);

	// adding stoich
	struct_Data.composition.append(stoich);

	// placing ui elements
	QFrame* element_Frame = new QFrame;
	QHBoxLayout* hor_Layout = new QHBoxLayout(element_Frame);
		hor_Layout->setSpacing(5);
		hor_Layout->setAlignment(Qt::AlignLeft);
		hor_Layout->setContentsMargins(0,0,0,0);
		hor_Layout->addWidget(line_Edit);
		hor_Layout->addWidget(label);
		hor_Layout->addWidget(elements);
		hor_Layout->addWidget(at_Weight);
	element_Frame_Vec.append(element_Frame);
	composition_Layout_With_Elements_Vector->addWidget(element_Frame,0,Qt::AlignLeft);

	show_Material();
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(QString::number(1,line_edit_short_double_format,line_edit_composition_precision));
			resize_Line_Edit("",composition_Line_Edit_Vec.first());
		fewer_Elements->hide();
	}
	if (element_Frame_Vec.size()>=2) {composition_Line_Edit_Vec.first()->setDisabled(false);fewer_Elements->show();}

	refresh_Data();
	connect(line_Edit, &QLineEdit::textEdited, this, [=]{refresh_Data();});
	connect(elements, &QComboBox::currentTextChanged, this, [=]{refresh_Data();});
}

void Item_Editor::read_Elements_From_Item()
{
	// clear all previous widgets
	for(auto element_Frame : element_Frame_Vec)
	{
		delete element_Frame;
	}
	element_Frame_Vec.clear();

	// reading data
	composition_Line_Edit_Vec.resize(struct_Data.composition.size());
	composition_Label_Vec.	  resize(struct_Data.composition.size());
	composition_Combo_Box_Vec.resize(struct_Data.composition.size());
	composition_At_Weight_Vec.resize(struct_Data.composition.size());

	element_Frame_Vec.resize(struct_Data.composition.size());

	// renew ui
	for(int i=0; i<struct_Data.composition.size(); ++i)
	{
		// creating ui elements
		QLineEdit* line_Edit = new QLineEdit;
				   line_Edit->setFixedWidth(50);
				   line_Edit->setProperty(min_Size_Property, line_Edit->width());
				   line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		QLabel*    label = new QLabel(atoms);
		QComboBox* elements = new QComboBox;		elements->addItems(sorted_Elements.keys());
		QLabel*	  at_Weight = new QLabel;

		connect(line_Edit, &QLineEdit::textEdited, this, [&](QString s){resize_Line_Edit(s);});

		line_Edit->setText(QString::number(struct_Data.composition[i].composition.value,line_edit_short_double_format,line_edit_composition_precision));
		resize_Line_Edit("",line_Edit);
		elements->setCurrentIndex(elements->findText(struct_Data.composition[i].type));
		at_Weight->setText(AtWt + QString::number(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

		composition_Line_Edit_Vec[i]=line_Edit;
		composition_Label_Vec	 [i]=label;
		composition_Combo_Box_Vec[i]=elements;
		composition_At_Weight_Vec[i]=at_Weight;

		connect(line_Edit, &QLineEdit::textEdited,		   this, [=]{refresh_Data();});
		connect(elements,  &QComboBox::currentTextChanged, this, [=]{refresh_Data();});

		// placing ui elements
		QFrame* element_Frame = new QFrame;
		QHBoxLayout*hor_Layout = new QHBoxLayout(element_Frame);
					hor_Layout->setSpacing(5);
					hor_Layout->setAlignment(Qt::AlignLeft);
					hor_Layout->setContentsMargins(0,0,0,0);
					hor_Layout->addWidget(line_Edit);
					hor_Layout->addWidget(label);
					hor_Layout->addWidget(elements);
					hor_Layout->addWidget(at_Weight);
		element_Frame_Vec[i] = element_Frame;
		composition_Layout_With_Elements_Vector->addWidget(element_Frame,0,Qt::AlignLeft);
	}

	show_Material();
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(QString::number(1,line_edit_short_double_format,line_edit_composition_precision));
			resize_Line_Edit("",composition_Line_Edit_Vec.first());
		fewer_Elements->hide();
	}
	if (element_Frame_Vec.size()>=2) {composition_Line_Edit_Vec.first()->setDisabled(false);fewer_Elements->show();}
}

void Item_Editor::read_Interlayers_From_Item()
{
	double coeff = length_Coefficients_Map.value(length_units);

	if(sigma_Done)
	if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		interlayer_Composition_Check_Box_Vec.		  resize(struct_Data.interlayer_Composition.size());
		interlayer_Composition_Comp_Line_Edit_Vec.	  resize(struct_Data.interlayer_Composition.size());
		interlayer_Composition_My_Sigma_Line_Edit_Vec.resize(struct_Data.interlayer_Composition.size());

		// add labels
		{
			QVBoxLayout* vert_Layout = new QVBoxLayout;
				vert_Layout->setSpacing(0);
			QLabel* empty_Label  = new QLabel;
			QLabel* weight_Label = new QLabel(sigma_Weight);
			my_Sigma_Label_Layer = new QLabel;
			vert_Layout->addWidget(empty_Label);
			vert_Layout->addWidget(weight_Label);
			vert_Layout->addWidget(my_Sigma_Label_Layer);

			interlayer_Composition_Layout_With_Elements_Vector->addLayout(vert_Layout);
		}

		// renew ui
		for(int i=0; i<struct_Data.interlayer_Composition.size(); ++i)
		{
			// creating ui elements
			QCheckBox* check_Box = new QCheckBox(transition_Layer_Functions[i]);
			QLineEdit* comp_Line_Edit = new QLineEdit;
				comp_Line_Edit->setFixedWidth(41);
//				comp_Line_Edit->setProperty(min_Size_Property, line_Edit->width());
				comp_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			QLineEdit* my_Sigma_Line_Edit = new QLineEdit;
				my_Sigma_Line_Edit->setFixedWidth(41);
//				my_Sigma_Line_Edit->setProperty(min_Size_Property, line_Edit->width());
				my_Sigma_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

//			connect(comp_Line_Edit,		&QLineEdit::textEdited,	this, [&](QString s){resize_Line_Edit(s);}); // better not use
//			connect(my_Sigma_Line_Edit, &QLineEdit::textEdited,	this, [&](QString s){resize_Line_Edit(s);}); // better not use

			check_Box->setChecked(struct_Data.interlayer_Composition[i].enabled);
			comp_Line_Edit->setText(QString::number(struct_Data.interlayer_Composition[i].interlayer.value,line_edit_short_double_format,line_edit_interlayer_precision));
			my_Sigma_Line_Edit->setText(QString::number(struct_Data.interlayer_Composition[i].my_Sigma.value/coeff,line_edit_short_double_format,line_edit_interlayer_precision));
//				resize_Line_Edit(comp_Line_Edit);     // better not use
//				resize_Line_Edit(my_Sigma_Line_Edit); // better not use

			interlayer_Composition_Check_Box_Vec		 [i]=check_Box;
			interlayer_Composition_Comp_Line_Edit_Vec	 [i]=comp_Line_Edit;
			interlayer_Composition_My_Sigma_Line_Edit_Vec[i]=my_Sigma_Line_Edit;

			connect(check_Box,			&QCheckBox::stateChanged, this, [=]{interlayer_Check();});
			connect(comp_Line_Edit,		&QLineEdit::textEdited,   this, &Item_Editor::refresh_Data);
			connect(my_Sigma_Line_Edit, &QLineEdit::textEdited,   this, &Item_Editor::refresh_Data);

			// placing ui elements
			QVBoxLayout* vert_Layout = new QVBoxLayout;
				vert_Layout->setSpacing(3);
				vert_Layout->setAlignment(Qt::AlignCenter);
				vert_Layout->addWidget(check_Box);
				vert_Layout->addWidget(comp_Line_Edit);
				vert_Layout->addWidget(my_Sigma_Line_Edit);
			interlayer_Composition_Layout_With_Elements_Vector->addLayout(vert_Layout);
		}

		individual_Sigma_Check_Box->setChecked(!struct_Data.common_Sigma);
		connect(individual_Sigma_Check_Box, &QCheckBox::stateChanged, this, [=]{interlayer_Check();});
	}

	interlayer_Composition_Layout_With_Elements_Vector->setSpacing(10);
	interlayer_Composition_Group_Box->adjustSize();
	interlayer_Composition_Group_Box->setFixedWidth(interlayer_Composition_Group_Box->width());

	interlayer_Check();
}

void Item_Editor::fewer_Elements_Clicked()
{
	if (element_Frame_Vec.size()<=2)	fewer_Elements->hide();

	struct_Data.composition.removeLast();

	composition_Line_Edit_Vec.removeLast();
	composition_Label_Vec.removeLast();
	composition_Combo_Box_Vec.removeLast();
	composition_At_Weight_Vec.removeLast();

	delete element_Frame_Vec.last();
	element_Frame_Vec.removeLast();

	refresh_Data();
	show_Material();
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(QString::number(1,line_edit_short_double_format,line_edit_composition_precision));
			resize_Line_Edit("",composition_Line_Edit_Vec.first());
		fewer_Elements->hide();
	}
}

void Item_Editor::show_Material()
{
	if(struct_Data.item_Type == item_Type_Ambient || struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		if(struct_Data.composed_Material)
		{
			material_Line_Edit->clear();
			for(int i=0; i<struct_Data.composition.size(); ++i)
			{
				material_Line_Edit->setText(material_Line_Edit->text() + composition_Combo_Box_Vec[i]->currentText());
				if( abs(composition_Line_Edit_Vec[i]->text().toDouble() - 1.) > DBL_EPSILON )
					material_Line_Edit->setText(material_Line_Edit->text() + QString::number(composition_Line_Edit_Vec[i]->text().toDouble(),line_edit_short_double_format,line_edit_composition_precision));
			}
		} else
		{
			material_Line_Edit->setText(struct_Data.material);
		}
		resize_Line_Edit("",material_Line_Edit);
	}
}

void Item_Editor::show_Density()
{
//	if(material_Done) // used before material_Done
	if(struct_Data.item_Type == item_Type_Ambient || struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		if(struct_Data.composed_Material)
		{
			density_Line_Edit->setText(QString::number(struct_Data.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
		} else
		{
			density_Line_Edit->setText(QString::number(struct_Data.relative_Density.value,line_edit_double_format,line_edit_density_precision));
		}
		resize_Line_Edit("",density_Line_Edit);
	}
}

void Item_Editor::show_Thickness()
{
	if(thickness_Done)
	if(struct_Data.item_Type == item_Type_Layer)
	{
		double coeff = length_Coefficients_Map.value(length_units);
		thickness_Label->setText(thickness_Label_1 + length_units + thickness_Label_2);

		thickness_Line_Edit->setText(QString::number(struct_Data.thickness.value/coeff,line_edit_double_format,line_edit_thickness_precision));
		resize_Line_Edit("",thickness_Line_Edit);
	}
}

void Item_Editor::show_Sigma_Start(bool at_Start)
{
	double coeff = length_Coefficients_Map.value(length_units);
	roughness_Label->setText(sigma_Label_1+ length_units + sigma_Label_2);

	if(!struct_Data.common_Sigma || at_Start)
	{
		sigma_Line_Edit->setText(QString::number(struct_Data.sigma.value/coeff,line_edit_double_format,line_edit_sigma_precision));
	}
}

void Item_Editor::show_Sigma(bool b)
{
	if(sigma_Done)
	if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		show_Sigma_Start(b);
		resize_Line_Edit("",sigma_Line_Edit);
	}
}

void Item_Editor::show_Stack_Parameters()
{
	if(stack_Done)
	if(struct_Data.item_Type == item_Type_Multilayer)
	{
		double coeff = length_Coefficients_Map.value(length_units);
		period_Label->setText(period_Label_1 + length_units + period_Label_2);

		repetitions_Line_Edit->setText(QString::number(struct_Data.num_Repetition.value));
		period_Line_Edit->setText(QString::number(struct_Data.period.value/coeff,line_edit_double_format,line_edit_period_precision));
		gamma_Line_Edit ->setText(QString::number(struct_Data.gamma.value,line_edit_double_format,line_edit_gamma_precision));

		resize_Line_Edit("",repetitions_Line_Edit);
		resize_Line_Edit("",period_Line_Edit);
		resize_Line_Edit("",gamma_Line_Edit);
	}
}

void Item_Editor::show_Interlayers(QObject* my_Sender)
{
	if(sigma_Done)
	if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		double coeff = length_Coefficients_Map.value(length_units);
		if(struct_Data.interlayer_Composition.size() == interlayer_Composition_Comp_Line_Edit_Vec.size())
		{
			for(int i=0; i<struct_Data.interlayer_Composition.size(); ++i)
			{
				if(interlayer_Composition_Comp_Line_Edit_Vec	 [i]!=my_Sender)
					interlayer_Composition_Comp_Line_Edit_Vec	 [i]->setText(QString::number(struct_Data.interlayer_Composition[i].interlayer.value,line_edit_short_double_format,line_edit_interlayer_precision));

				if(interlayer_Composition_My_Sigma_Line_Edit_Vec [i]!=my_Sender)
					interlayer_Composition_My_Sigma_Line_Edit_Vec[i]->setText(QString::number(struct_Data.interlayer_Composition[i].my_Sigma.value/coeff,line_edit_short_double_format,line_edit_sigma_precision));
			}
			my_Sigma_Label_Layer->setText(sigma_Label_3 + length_units + sigma_Label_2);
		}
		show_Sigma(false);
	}
}

void Item_Editor::resize_Line_Edit(QString text, QLineEdit* line_Edit)
{
	if(line_Edit == nullptr) line_Edit = qobject_cast<QLineEdit*>(QObject::sender());

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

void Item_Editor::browse_Material()
{
	QFileInfo filename = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Find Material", nk_Path, "Optical constants " + QString(nk_Filter) + ";;All files (*.*)"));
	if (!filename.completeBaseName().isEmpty())
	{
		material_Line_Edit->setText(filename.completeBaseName());
		material_Line_Edit->textEdited(material_Line_Edit->text());
	}
}

void Item_Editor::depth_Grading()
{
	Grading_Editor* depth_Grading = new Grading_Editor(item, whats_This_Thickness);
		depth_Grading->setParent(this);
		depth_Grading->setModal(true);
		depth_Grading->setWindowFlags(Qt::Window);
		depth_Grading->show();

	connect(depth_Grading, &Grading_Editor::grading_Edited, this, &Item_Editor::emit_Item_Data_Edited);
	connect(depth_Grading, &Grading_Editor::grading_Edited, this, &Item_Editor::reload_And_Show_All);
}

void Item_Editor::sigma_Grading()
{
	Grading_Editor* sigma_Grading = new Grading_Editor(item, whats_This_Sigma);
		sigma_Grading->setParent(this);
		sigma_Grading->setModal(true);
		sigma_Grading->setWindowFlags(Qt::Window);
		sigma_Grading->show();

	connect(sigma_Grading, &Grading_Editor::grading_Edited, this, &Item_Editor::emit_Item_Data_Edited);
	connect(sigma_Grading, &Grading_Editor::grading_Edited, this, &Item_Editor::reload_And_Show_All);
}

void Item_Editor::interlayer_Check()
{
	int counter=0;
	if(sigma_Done)
	if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		for(int i=0; i<interlayer_Composition_Check_Box_Vec.size(); ++i)
		{
			if(interlayer_Composition_Check_Box_Vec[i]->isChecked() == true)
			{
				counter++;
				interlayer_Composition_Comp_Line_Edit_Vec[i]->setDisabled(false);
				if(individual_Sigma_Check_Box->isChecked())
				{
					interlayer_Composition_My_Sigma_Line_Edit_Vec[i]->setDisabled(false);
				} else
				{
					interlayer_Composition_My_Sigma_Line_Edit_Vec[i]->setDisabled(true);
				}
			} else
			{
				interlayer_Composition_Comp_Line_Edit_Vec[i]->setDisabled(true);
				interlayer_Composition_My_Sigma_Line_Edit_Vec[i]->setDisabled(true);
			}
		}
		if(counter==0 || individual_Sigma_Check_Box->isChecked())
		{
			sigma_Line_Edit->setDisabled(true);
		} else
		{
			sigma_Line_Edit->setDisabled(false);
		}
	}
	refresh_Data();
}

void Item_Editor::norm_Interlayer_Composition()
{
	if(sigma_Done)
	if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		double sum=0;
		for(int i=0; i<transition_Layer_Functions_Size; ++i)
		{
			if(interlayer_Composition_Check_Box_Vec[i]->isChecked() == true)
			{
				sum+=interlayer_Composition_Comp_Line_Edit_Vec[i]->text().toDouble();
			}
		}
		for(int i=0; i<transition_Layer_Functions_Size; ++i)
		{
			if(interlayer_Composition_Check_Box_Vec[i]->isChecked() == true)
			{
				interlayer_Composition_Comp_Line_Edit_Vec[i]->setText(QString::number(interlayer_Composition_Comp_Line_Edit_Vec[i]->text().toDouble()/sum,line_edit_short_double_format,line_edit_interlayer_precision));
			}
		}
	}
}

void Item_Editor::refresh_Material()
{
	if(material_Done)
	if(struct_Data.item_Type == item_Type_Ambient || struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		QVariant var;
		struct_Data.material = material_Line_Edit->text();
		var.setValue( struct_Data );
		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

		if(sender() == material_Line_Edit)
		{
			emit item_Data_Edited();
		}
	}
}

void Item_Editor::refresh_Data()
{
	double coeff = length_Coefficients_Map.value(length_units);
	QVariant var;

	// material
	if(material_Done)
	if(struct_Data.item_Type == item_Type_Ambient || struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		if(!struct_Data.composed_Material)
		{
			struct_Data.relative_Density.value = density_Line_Edit->text().toDouble();
		} else
		{
			struct_Data.absolute_Density.value = density_Line_Edit->text().toDouble();

			for(int i=0; i<struct_Data.composition.size(); ++i)
			{
				struct_Data.composition[i].composition.value = composition_Line_Edit_Vec[i]->text().toDouble();
				struct_Data.composition[i].composition.fit.min = struct_Data.composition[i].composition.value*(1-dispersion);
				struct_Data.composition[i].composition.fit.max = struct_Data.composition[i].composition.value*(1+dispersion);

				struct_Data.composition[i].type = composition_Combo_Box_Vec[i]->currentText();
				composition_At_Weight_Vec[i]->setText(AtWt + QString::number(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),thumbnail_double_format,at_weight_precision) + ")");
			}
		}
	}

	// sigma
	if(sigma_Done)
	if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		struct_Data.common_Sigma = !individual_Sigma_Check_Box->isChecked();

		double sum=0;
		for(int i=0; i<struct_Data.interlayer_Composition.size(); ++i)
		{
			struct_Data.interlayer_Composition[i].enabled = interlayer_Composition_Check_Box_Vec[i]->isChecked();
			if(interlayer_Composition_Check_Box_Vec[i]->isChecked())
			{
				struct_Data.interlayer_Composition[i].interlayer.value = interlayer_Composition_Comp_Line_Edit_Vec[i]->text().toDouble();

				// sum for normalizing
				sum+=struct_Data.interlayer_Composition[i].interlayer.value;
			}
		}
		if(abs(sum)<DBL_EPSILON) sum = DBL_EPSILON;

		if(struct_Data.common_Sigma)
		{
			struct_Data.sigma.value = sigma_Line_Edit->text().toDouble()*coeff;
			for(int i=0; i<struct_Data.interlayer_Composition.size(); ++i)
			{
				struct_Data.interlayer_Composition[i].my_Sigma.value = struct_Data.sigma.value;
			}
		} else
		{
			// normalizing and saving
			double temp_Sigma_Square=0;
			for(int i=0; i<struct_Data.interlayer_Composition.size(); ++i)
			{
				struct_Data.interlayer_Composition[i].my_Sigma.value = interlayer_Composition_My_Sigma_Line_Edit_Vec[i]->text().toDouble()*coeff;
				if(interlayer_Composition_Check_Box_Vec[i]->isChecked())
				{
					temp_Sigma_Square += pow(struct_Data.interlayer_Composition[i].my_Sigma.value,2) * struct_Data.interlayer_Composition[i].interlayer.value/sum;
				}
			}
			struct_Data.sigma.value = sqrt(temp_Sigma_Square);
		}
	}

	// thickness
	if(thickness_Done)
	if(struct_Data.item_Type == item_Type_Layer)
	{
		struct_Data.thickness.value = thickness_Line_Edit->text().toDouble()*coeff;
	}

	// multilayer
	if(stack_Done)
	if(struct_Data.item_Type == item_Type_Multilayer)
	{
		double coeff = length_Coefficients_Map.value(length_units);
		double init_Period = struct_Data.period.value;

		struct_Data.num_Repetition.value = repetitions_Line_Edit->text().toInt();
		struct_Data.period.value = period_Line_Edit->text().toDouble()*coeff;
		if(item->childCount()==2)
		{
			double temp_Gamma = gamma_Line_Edit->text().toDouble();
			if(temp_Gamma>1)
			{
				gamma_Line_Edit->setText(QString::number(struct_Data.gamma.value));
				resize_Line_Edit("",gamma_Line_Edit);
			} else
			{
				struct_Data.gamma.value = gamma_Line_Edit->text().toDouble();
			}
		}

		// children of stack
		if( abs(init_Period) > DBL_MIN )
		{
			if(item->childCount()==2)
			{
				change_Stack_Gamma();
			} else
			{
				if( abs(init_Period) > DBL_MIN )
				{
					double factor = struct_Data.period.value / init_Period;
					change_Child_Layers_Thickness(item, factor);
//					init_Period = struct_Data.period.value; // never used
				}
			}
		}
	}

	var.setValue( struct_Data );
	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	show_Material();
	show_Interlayers(sender());
	emit item_Data_Edited();
}

void Item_Editor::fast_Refresh_Stack()
{
	if(stack_Done)
	if(struct_Data.item_Type == item_Type_Multilayer)
	if( abs(period_Line_Edit->text().toDouble()) > DBL_MIN || abs(struct_Data.period.value) < DBL_MIN )
	{
		if(repetitions_Line_Edit->text().toInt()!=0 || struct_Data.num_Repetition.value == 0)
		{
			if(item->childCount()==2)
			{
				if( abs(gamma_Line_Edit->text().toDouble()) > DBL_MIN || abs(struct_Data.gamma.value) < DBL_MIN )
				{
					if( abs(gamma_Line_Edit->text().toDouble() - 1) > DBL_EPSILON || abs(struct_Data.gamma.value - 1) < DBL_EPSILON )
					{
						refresh_Data();
					}
				}
			} else
			{
				refresh_Data();
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
			Data layer = multilayer_Item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
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

void Item_Editor::change_Stack_Gamma()
{
	int i=0;
	{
		double new_Thickness = struct_Data.period.value*struct_Data.gamma.value;
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
		double new_Thickness = struct_Data.period.value*(1-struct_Data.gamma.value);
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
	Data layer = layer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	layer.thickness.value = new_Thickness;
	var.setValue( layer );
	layer_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
}

void Item_Editor::reset_Multilayer_Thickness(QTreeWidgetItem* multilayer_Item, double new_Thickness)
{
	Data stack_Content = multilayer_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	double factor=1;
	if( abs(stack_Content.period.value) > DBL_MIN && stack_Content.num_Repetition.value!=0)
	{
		factor = new_Thickness/(  stack_Content.period.value*stack_Content.num_Repetition.value  );
		change_Child_Layers_Thickness(multilayer_Item, factor);
	}
}
