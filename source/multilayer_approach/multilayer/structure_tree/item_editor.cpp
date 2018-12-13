// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "item_editor.h"
#include "multilayer_approach/multilayer_approach.h"

Item_Editor::Item_Editor(QList<Item_Editor*>& list_Editors, QTreeWidgetItem* item, Structure_Tree* structure_Tree, QWidget *parent) :
	item(item),
	list_Editors(list_Editors),
	struct_Data(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>()),
	structure_Tree(structure_Tree),
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
				Global_Variables::resize_Line_Edit(material_Line_Edit);
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

	hor_Layout = new QHBoxLayout;
	main_Layout->addLayout(hor_Layout);
	create_Menu();
	Global_Variables::create_Shortcuts(this);

	if(struct_Data.item_Type == item_Type_Ambient)		make_Ambient_Editor();
	if(struct_Data.item_Type == item_Type_Layer)		make_Layer_Editor();
	if(struct_Data.item_Type == item_Type_Substrate)	make_Substrate_Editor();
	if(struct_Data.item_Type == item_Type_Multilayer)	make_Multilayer_Editor();
	if(struct_Data.item_Type == item_Type_Aperiodic)	make_Aperiodic_Editor();

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
}

void Item_Editor::make_Layer_Editor()
{	
	make_Materials_Group_Box();		
	make_Thickness_Group_Box();
	make_Sigma_Group_Box();
}

void Item_Editor::make_Substrate_Editor()
{
	make_Materials_Group_Box();
	make_Sigma_Group_Box();
}

void Item_Editor::make_Materials_Group_Box()
{
	material_Group_Box = new QGroupBox;
	material_Group_Box->setObjectName("material_Group_Box");
	material_Group_Box->setStyleSheet("QGroupBox#material_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
	QVBoxLayout* material_Group_Box_Layout = new QVBoxLayout(material_Group_Box);
		material_Group_Box_Layout->setSpacing(7);
		material_Group_Box_Layout->setContentsMargins(8,8,8,5);
	hor_Layout->addWidget(material_Group_Box);

	// materials
	{
		QHBoxLayout* layout = new QHBoxLayout;
		material_Group_Box_Layout->addLayout(layout);

		QLabel* material_Label = new QLabel("Material:");
		layout->addWidget(material_Label);

		material_Line_Edit = new QLineEdit;
			material_Line_Edit->setFixedWidth(70);
			material_Line_Edit->setProperty(min_Size_Property, material_Line_Edit->width());
		layout->addWidget(material_Line_Edit);

		browse_Material_Button = new QPushButton("Browse...");
		browse_Material_Button->setFixedWidth(60);
			browse_Material_Button->setFocusPolicy(Qt::NoFocus);
		layout->addWidget(browse_Material_Button);

		density_Label = new QLabel("<no label>");
		layout->addWidget(density_Label);

		density_Line_Edit = new QLineEdit;
			density_Line_Edit->setFixedWidth(50);
			density_Line_Edit->setProperty(min_Size_Property, density_Line_Edit->width());
			density_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
		layout->addWidget(density_Line_Edit);
			show_Density();

		connect(material_Line_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(material_Line_Edit);});
		connect(density_Line_Edit,  &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(density_Line_Edit);});

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
		connect(thickness_Line_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(thickness_Line_Edit);});
		connect(thickness_Line_Edit, &QLineEdit::textEdited, this, [=]{refresh_Data();});
	}
	hor_Layout->addWidget(thickness_Group_Box);
	{
		thickness_Done = true;
		show_Thickness();
	}
}

void Item_Editor::make_Multilayer_Editor()
{
	make_Multilayer_Group_Box();
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

		connect(repetitions_Line_Edit,  &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(repetitions_Line_Edit);});
		connect(repetitions_Line_Edit,	&QLineEdit::textEdited, this, &Item_Editor::fast_Refresh_Stack);
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

		connect(period_Line_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(period_Line_Edit);});
		connect(period_Line_Edit, &QLineEdit::textEdited, this, &Item_Editor::fast_Refresh_Stack);
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

			connect(gamma_Line_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(gamma_Line_Edit);});
			connect(gamma_Line_Edit, &QLineEdit::textEdited, this, &Item_Editor::fast_Refresh_Stack);
		}
	}
	main_Layout->addWidget(multilayer_Group_Box);

	{
		QGroupBox* make_Aperiodic_Group_Box = new QGroupBox;
			make_Aperiodic_Group_Box->setObjectName("make_Aperiodic_Group_Box");
			make_Aperiodic_Group_Box->setStyleSheet("QGroupBox#make_Aperiodic_Group_Box { border-radius: 2px;  border: 1px solid gray;}");

		QHBoxLayout* layout = new QHBoxLayout(make_Aperiodic_Group_Box);
		make_Aperiodic_CheckBox = new QCheckBox("Make aperiodic");
			make_Aperiodic_CheckBox->setChecked(false);
		layout->addWidget(make_Aperiodic_CheckBox);

		connect(make_Aperiodic_CheckBox, &QCheckBox::released, this, [=]
		{
			QMessageBox::StandardButton reply = QMessageBox::question(this,"Make aperiodic", "Are you sure?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
			if (reply == QMessageBox::Yes)
			{
				make_Aperiodic();
			} else
			{
				make_Aperiodic_CheckBox->setChecked(false);
			}
		});
		main_Layout->addWidget(make_Aperiodic_Group_Box);
	}

	{
		stack_Done = true;
		show_Stack_Parameters();
	}
}

void Item_Editor::make_Aperiodic_Editor()
{
	make_Aperiodic_Group_Box();
}

void Item_Editor::make_Aperiodic_Group_Box()
{
	calc_Uniqueness();

	aperiodic_Group_Box = new QGroupBox;
		aperiodic_Group_Box->setObjectName("aperiodic_Group_Box");
		aperiodic_Group_Box->setStyleSheet("QGroupBox#aperiodic_Group_Box { border-radius: 2px;  border: 1px solid gray;}");
	main_Layout->addWidget(aperiodic_Group_Box);

	QHBoxLayout* aperiodic_Group_Box_Layout = new QHBoxLayout(aperiodic_Group_Box);

	// unique items in aperiodic
	{
		QVector<int> used_Uniqueness;
		int unique_Index = 1;

		// header

		QVBoxLayout* layer_Layout = new QVBoxLayout;
		aperiodic_Group_Box_Layout->addLayout(layer_Layout);
		QLabel* layer_Label = new QLabel("Item");
		layer_Layout->addWidget(layer_Label,0,Qt::AlignCenter);

		QVBoxLayout* relate_Z_Layout = new QVBoxLayout;
		aperiodic_Group_Box_Layout->addLayout(relate_Z_Layout);
		QLabel* relate_Z_Label = new QLabel(" Relate \"z\" ");
		relate_Z_Layout->addWidget(relate_Z_Label);

		QVBoxLayout* fit_Z_Layout = new QVBoxLayout;
		aperiodic_Group_Box_Layout->addLayout(fit_Z_Layout);
		QLabel* fit_Z_Label = new QLabel(" Fit \"z\" ");
		fit_Z_Layout->addWidget(fit_Z_Label,0,Qt::AlignCenter);

		QVBoxLayout* relate_Sigma_Layout = new QVBoxLayout;
		aperiodic_Group_Box_Layout->addLayout(relate_Sigma_Layout);
		QLabel* relate_Sigma_Label = new QLabel(" Relate \""+Sigma_Sym+"\" ");
		relate_Sigma_Layout->addWidget(relate_Sigma_Label,0,Qt::AlignCenter);

		QVBoxLayout* fit_Sigma_Layout = new QVBoxLayout;
		aperiodic_Group_Box_Layout->addLayout(fit_Sigma_Layout);
		QLabel* fit_Sigma_Label = new QLabel(" Fit \""+Sigma_Sym+"\" ");
		fit_Sigma_Layout->addWidget(fit_Sigma_Label,0,Qt::AlignCenter);

		QVBoxLayout* soft_Restriction_Layout = new QVBoxLayout;
		aperiodic_Group_Box_Layout->addLayout(soft_Restriction_Layout);
		QLabel* soft_Restriction_Label = new QLabel("       Smooth \"z\": {"+Plus_Minus_Sym+Delta_Big_Sym+", Q}");
		soft_Restriction_Layout->addWidget(soft_Restriction_Label,0,Qt::AlignLeft);

		for(int i=0; i<item->childCount(); i++)
		{
			QTreeWidgetItem* current_Child_Item = item->child(i);
			int current_Child_Uniqueness = item_Uniqueness_Map.value(current_Child_Item);

			if(!used_Uniqueness.contains(current_Child_Uniqueness)) // uniqueness of list
			{
				used_Uniqueness.append(current_Child_Uniqueness);
				Data current_Child_Data = current_Child_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

				// initial state
				bool is_Relate_Thickness = false;
				bool is_Fit_Thickness = true;
				bool is_Relate_Sigma = false;
				bool is_Fit_Sigma = true;
				for(int k=0; k<item->childCount(); k++)
				{
					Data temp_Data = item->child(k)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
					if(temp_Data.uniqueness == current_Child_Uniqueness)
					{
						// relate thickness
						if(item->child(k) != current_Child_Item)
						{
							for(Parameter_Indicator& slave : current_Child_Data.thickness.coupled.slaves)
							{
								if(slave.id == temp_Data.thickness.indicator.id) is_Relate_Thickness = true;
								else											 is_Relate_Thickness = false;
							}

							// relate sigma
							for(Parameter_Indicator& slaves : current_Child_Data.sigma.coupled.slaves)
							{
								if(slaves == temp_Data.sigma.indicator) is_Relate_Sigma = true;
								else									is_Relate_Sigma = false;
							}
						}

						// fit thickness
						is_Fit_Thickness = is_Fit_Thickness && temp_Data.thickness.fit.is_Fitable;

						// fit sigma
						is_Fit_Sigma = is_Fit_Sigma && temp_Data.sigma.fit.is_Fitable;
					}
				}

				// text
				QString composed;
				if(current_Child_Data.item_Type == item_Type_Layer)
				{
					if(current_Child_Data.composed_Material) composed = "(composed)";
					else									 composed = "(tabular)";
				}
				QString text =	Locale.toString(unique_Index++) + ")  " +
								current_Child_Data.item_Type + "  " +
								current_Child_Data.material + "  " +
								composed;

				QLabel* unique_Item_Label = new QLabel(text);
				layer_Layout->addWidget(unique_Item_Label);

				QSizePolicy sp_retain;
					sp_retain.setRetainSizeWhenHidden(true);

				QCheckBox* unique_Item_Connect_Thickness = new QCheckBox;
					unique_Item_Connect_Thickness->setSizePolicy(sp_retain);
					unique_Item_Connect_Thickness->setChecked(is_Relate_Thickness);
				relate_Z_Layout->addWidget(unique_Item_Connect_Thickness,0,Qt::AlignCenter);
				QCheckBox* unique_Item_Fit_Thickness = new QCheckBox;
					unique_Item_Fit_Thickness->setSizePolicy(sp_retain);
					unique_Item_Fit_Thickness->setChecked(is_Fit_Thickness);
				fit_Z_Layout->addWidget(unique_Item_Fit_Thickness,0,Qt::AlignCenter);
				QCheckBox* unique_Item_Connect_Sigma = new QCheckBox;
					unique_Item_Connect_Sigma->setSizePolicy(sp_retain);
					unique_Item_Connect_Sigma->setChecked(is_Relate_Sigma);
				relate_Sigma_Layout->addWidget(unique_Item_Connect_Sigma,0,Qt::AlignCenter);
				QCheckBox* unique_Item_Fit_Sigma = new QCheckBox;
					unique_Item_Fit_Sigma->setSizePolicy(sp_retain);
					unique_Item_Fit_Sigma->setChecked(is_Fit_Sigma);
				fit_Sigma_Layout->addWidget(unique_Item_Fit_Sigma,0,Qt::AlignCenter);

				if(current_Child_Data.item_Type == item_Type_Layer)
				{
					connect(unique_Item_Connect_Thickness, &QCheckBox::toggled, this, [=]{unique_Item_Do(relate_Thickness,	current_Child_Uniqueness);});
					connect(unique_Item_Fit_Thickness,	   &QCheckBox::toggled, this, [=]{unique_Item_Do(fit_Thickness,		current_Child_Uniqueness);});
					connect(unique_Item_Connect_Sigma,	   &QCheckBox::toggled, this, [=]{unique_Item_Do(relate_Sigma,		current_Child_Uniqueness);});
					connect(unique_Item_Fit_Sigma,		   &QCheckBox::toggled, this, [=]{unique_Item_Do(fit_Sigma,			current_Child_Uniqueness);});
				} else
				{
					unique_Item_Connect_Thickness->hide();
					unique_Item_Fit_Thickness->hide();
					unique_Item_Connect_Sigma->hide();
					unique_Item_Fit_Sigma->hide();
				}

				// create additional restrictions
				{
					QHBoxLayout* soft_Restriction_Row_Layout = new QHBoxLayout;
						soft_Restriction_Row_Layout->setAlignment(Qt::AlignLeft);
					soft_Restriction_Layout->addLayout(soft_Restriction_Row_Layout);

					QCheckBox* soft_Restriction_CheckBox = new QCheckBox;
						soft_Restriction_CheckBox->setSizePolicy(sp_retain);
						soft_Restriction_CheckBox->setChecked(current_Child_Data.use_Soft_Restrictions);
					soft_Restriction_Row_Layout->addWidget(soft_Restriction_CheckBox);

					QLabel* plus_Minus_Label = new QLabel(Plus_Minus_Sym);
						plus_Minus_Label->setEnabled(current_Child_Data.use_Soft_Restrictions);
					soft_Restriction_Row_Layout->addWidget(plus_Minus_Label);

					QSpinBox* soft_Restriction_Threshold_SpinBox = new QSpinBox;
						soft_Restriction_Threshold_SpinBox->setRange(0, 100);
						soft_Restriction_Threshold_SpinBox->setEnabled(current_Child_Data.use_Soft_Restrictions);
						soft_Restriction_Threshold_SpinBox->setSuffix("%");
						soft_Restriction_Threshold_SpinBox->setValue(current_Child_Data.threshold);
						soft_Restriction_Threshold_SpinBox->setAccelerated(true);
						soft_Restriction_Threshold_SpinBox->setFixedWidth(45);
						soft_Restriction_Threshold_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
					soft_Restriction_Row_Layout->addWidget(soft_Restriction_Threshold_SpinBox);

					QLabel* Q_Label = new QLabel(", Q=");
						Q_Label->setEnabled(current_Child_Data.use_Soft_Restrictions);
					soft_Restriction_Row_Layout->addWidget(Q_Label);

					MyDoubleSpinBox* soft_Restriction_Q_SpinBox = new MyDoubleSpinBox;
						soft_Restriction_Q_SpinBox->setEnabled(current_Child_Data.use_Soft_Restrictions);
						soft_Restriction_Q_SpinBox->setSuffix("  ");
						soft_Restriction_Q_SpinBox->setRange(0, MAX_DOUBLE);
						soft_Restriction_Q_SpinBox->setSingleStep(1);
						soft_Restriction_Q_SpinBox->setValue(current_Child_Data.Q_factor);
						soft_Restriction_Q_SpinBox->setAccelerated(true);
						soft_Restriction_Q_SpinBox->setDecimals(3);
						soft_Restriction_Q_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
					soft_Restriction_Row_Layout->addWidget(soft_Restriction_Q_SpinBox);
					Global_Variables::resize_Line_Edit(soft_Restriction_Q_SpinBox);

					if(current_Child_Data.item_Type == item_Type_Layer)
					{
						connect(soft_Restriction_CheckBox, &QCheckBox::toggled, this, [=]
						{
							soft_Restriction_Save(current_Child_Uniqueness,
												  soft_Restriction_CheckBox->isChecked(),
												  soft_Restriction_Threshold_SpinBox->value(),
												  soft_Restriction_Q_SpinBox->value());

							plus_Minus_Label->setEnabled(soft_Restriction_CheckBox->isChecked());
							soft_Restriction_Threshold_SpinBox->setEnabled(soft_Restriction_CheckBox->isChecked());
							Q_Label->setEnabled(soft_Restriction_CheckBox->isChecked());
							soft_Restriction_Q_SpinBox->setEnabled(soft_Restriction_CheckBox->isChecked());
						});
						connect(soft_Restriction_Threshold_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
						{
							soft_Restriction_Save(current_Child_Uniqueness,
												  soft_Restriction_CheckBox->isChecked(),
												  soft_Restriction_Threshold_SpinBox->value(),
												  soft_Restriction_Q_SpinBox->value());
						});
						connect(soft_Restriction_Q_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
						{
							soft_Restriction_Save(current_Child_Uniqueness,
												  soft_Restriction_CheckBox->isChecked(),
												  soft_Restriction_Threshold_SpinBox->value(),
												  soft_Restriction_Q_SpinBox->value());

							Global_Variables::resize_Line_Edit(soft_Restriction_Q_SpinBox);
						});

					} else
					{
						plus_Minus_Label->hide();
						soft_Restriction_CheckBox->hide();
						soft_Restriction_Threshold_SpinBox->hide();
						Q_Label->hide();
						soft_Restriction_Q_SpinBox->hide();
					}
				}
			}
		}
	}
	// make periodic and invert
	{
		QGroupBox* make_Multilayer_Group_Box = new QGroupBox;
			make_Multilayer_Group_Box->setObjectName("make_Multilayer_Group_Box");
			make_Multilayer_Group_Box->setStyleSheet("QGroupBox#make_Multilayer_Group_Box { border-radius: 2px;  border: 1px solid gray;}");

		QHBoxLayout* layout = new QHBoxLayout(make_Multilayer_Group_Box);
		make_Multilayer_CheckBox = new QCheckBox("Make periodic");
			make_Multilayer_CheckBox->setChecked(false);
		layout->addWidget(make_Multilayer_CheckBox);
		invert_CheckBox = new QCheckBox("Invert order of layers");
			invert_CheckBox->setChecked(false);
		layout->addWidget(invert_CheckBox);

		connect(make_Multilayer_CheckBox, &QCheckBox::released, this, [=]
		{
			QMessageBox::StandardButton reply = QMessageBox::question(this,"Make periodic", "Are you sure?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
			if (reply == QMessageBox::Yes)
			{
				make_Multilayer();
			} else
			{
				make_Multilayer_CheckBox->setChecked(false);
			}
		});
		connect(invert_CheckBox, &QCheckBox::toggled, this, &Item_Editor::invert_Aperiodic);

		main_Layout->addWidget(make_Multilayer_Group_Box);
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
		connect(sigma_Line_Edit,		&QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(sigma_Line_Edit);});
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
	main_Layout->addWidget(sigma_Group_Box);
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
	density_Line_Edit->setText(Locale.toString(struct_Data.relative_Density.value,line_edit_double_format,line_edit_density_precision));
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
	density_Line_Edit->setText(Locale.toString(struct_Data.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
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

	connect(line_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(line_Edit);});

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
	stoich.composition.confidence.num_Points = default_num_confidence_points;
	stoich.composition.confidence.calc_Conf_Interval = false;

	// set item_Id
	stoich.composition.indicator.item_Id = struct_Data.id;

	// creating ui elements
	line_Edit->setText(Locale.toString(stoich.composition.value,line_edit_short_double_format,line_edit_composition_precision));
	elements->setCurrentIndex(elements->findText(stoich.type));
	at_Weight->setText(AtWt + Locale.toString(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

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

	Global_Variables::resize_Line_Edit(line_Edit);

	show_Material();
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(Locale.toString(1.,line_edit_short_double_format,line_edit_composition_precision));
			Global_Variables::resize_Line_Edit(composition_Line_Edit_Vec.first());
		fewer_Elements->hide();
	}
	if (element_Frame_Vec.size()>=2) {composition_Line_Edit_Vec.first()->setDisabled(false);fewer_Elements->show();}

	auto_Density_On = true; refresh_Data(); auto_Density_On = false;
	connect(line_Edit, &QLineEdit::textEdited,        this, [=]{auto_Density_On = true; refresh_Data(); auto_Density_On = false;});
	connect(elements, &QComboBox::currentTextChanged, this, [=]{auto_Density_On = true; refresh_Data(); auto_Density_On = false; });
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

		connect(line_Edit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(line_Edit);});

		line_Edit->setText(Locale.toString(struct_Data.composition[i].composition.value,line_edit_short_double_format,line_edit_composition_precision));
		elements->setCurrentIndex(elements->findText(struct_Data.composition[i].type));
		at_Weight->setText(AtWt + Locale.toString(sorted_Elements.value(elements->currentText()),thumbnail_double_format,at_weight_precision) + ")");

		composition_Line_Edit_Vec[i]=line_Edit;
		composition_Label_Vec	 [i]=label;
		composition_Combo_Box_Vec[i]=elements;
		composition_At_Weight_Vec[i]=at_Weight;

		connect(line_Edit, &QLineEdit::textEdited,		   this, [=]{auto_Density_On = true; refresh_Data(); auto_Density_On = false;});
		connect(elements,  &QComboBox::currentTextChanged, this, [=]{auto_Density_On = true; refresh_Data(); auto_Density_On = false;});

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

		Global_Variables::resize_Line_Edit(line_Edit);
	}

	show_Material();
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(Locale.toString(1.,line_edit_short_double_format,line_edit_composition_precision));
			Global_Variables::resize_Line_Edit(composition_Line_Edit_Vec.first());
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

//			connect(comp_Line_Edit,		&QLineEdit::textEdited,	this, [=]{Global_Variables::resize_Line_Edit(comp_Line_Edit);}); // better not use
//			connect(my_Sigma_Line_Edit, &QLineEdit::textEdited,	this, [=]{Global_Variables::resize_Line_Edit(my_Sigma_Line_Edit);}); // better not use

			check_Box->setChecked(struct_Data.interlayer_Composition[i].enabled);
			comp_Line_Edit->setText(Locale.toString(struct_Data.interlayer_Composition[i].interlayer.value,line_edit_short_double_format,line_edit_interlayer_precision));
			my_Sigma_Line_Edit->setText(Locale.toString(struct_Data.interlayer_Composition[i].my_Sigma.value/coeff,line_edit_short_double_format,line_edit_interlayer_precision));
//				Global_Variables::resize_Line_Edit(comp_Line_Edit);     // better not use
//				Global_Variables::resize_Line_Edit(my_Sigma_Line_Edit); // better not use

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

	auto_Density_On = true; refresh_Data(); auto_Density_On = false;
	show_Material();
	if (element_Frame_Vec.size()==1)
	{
		composition_Line_Edit_Vec.first()->setDisabled(true);
		composition_Line_Edit_Vec.first()->setText(Locale.toString(1.,line_edit_short_double_format,line_edit_composition_precision));
			Global_Variables::resize_Line_Edit(composition_Line_Edit_Vec.first());
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
				if( abs(Locale.toDouble(composition_Line_Edit_Vec[i]->text()) - 1.) > DBL_EPSILON )
					material_Line_Edit->setText(material_Line_Edit->text() + Locale.toString(Locale.toDouble(composition_Line_Edit_Vec[i]->text()),line_edit_short_double_format,line_edit_composition_precision));
			}
		} else
		{
			material_Line_Edit->setText(struct_Data.material);
		}
		Global_Variables::resize_Line_Edit(material_Line_Edit);
	}
}

void Item_Editor::show_Density()
{
//	if(material_Done) // used before material_Done
	if(struct_Data.item_Type == item_Type_Ambient || struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		if(struct_Data.composed_Material)
		{
			density_Line_Edit->setText(Locale.toString(struct_Data.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
		} else
		{
			density_Line_Edit->setText(Locale.toString(struct_Data.relative_Density.value,line_edit_double_format,line_edit_density_precision));
		}
		Global_Variables::resize_Line_Edit(density_Line_Edit);
	}
}

void Item_Editor::show_Thickness()
{
	if(thickness_Done)
	if(struct_Data.item_Type == item_Type_Layer)
	{
		double coeff = length_Coefficients_Map.value(length_units);
		thickness_Label->setText(thickness_Label_1 + length_units + thickness_Label_2);

		thickness_Line_Edit->setText(Locale.toString(struct_Data.thickness.value/coeff,line_edit_double_format,line_edit_thickness_precision));
		Global_Variables::resize_Line_Edit(thickness_Line_Edit);
	}
}

void Item_Editor::show_Sigma_Start(bool at_Start)
{
	double coeff = length_Coefficients_Map.value(length_units);
	roughness_Label->setText(sigma_Label_1+ length_units + sigma_Label_2);

	if(!struct_Data.common_Sigma || at_Start)
	{
		sigma_Line_Edit->setText(Locale.toString(struct_Data.sigma.value/coeff,line_edit_double_format,line_edit_sigma_precision));
	}
}

void Item_Editor::show_Sigma(bool b)
{
	if(sigma_Done)
	if(struct_Data.item_Type == item_Type_Layer || struct_Data.item_Type == item_Type_Substrate)
	{
		show_Sigma_Start(b);
		Global_Variables::resize_Line_Edit(sigma_Line_Edit);
	}
}

void Item_Editor::show_Stack_Parameters()
{
	if(stack_Done)
	if(struct_Data.item_Type == item_Type_Multilayer)
	{
		double coeff = length_Coefficients_Map.value(length_units);
		period_Label->setText(period_Label_1 + length_units + period_Label_2);

		repetitions_Line_Edit->setText(Locale.toString(struct_Data.num_Repetition.value()));
		period_Line_Edit->setText(Locale.toString(struct_Data.period.value/coeff,line_edit_double_format,line_edit_period_precision));
		gamma_Line_Edit ->setText(Locale.toString(struct_Data.gamma.value,line_edit_double_format,line_edit_gamma_precision));

		repetitions_Line_Edit->textEdited(repetitions_Line_Edit->text());
		period_Line_Edit->textEdited(period_Line_Edit->text());
		gamma_Line_Edit->textEdited(gamma_Line_Edit->text());
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
					interlayer_Composition_Comp_Line_Edit_Vec	 [i]->setText(Locale.toString(struct_Data.interlayer_Composition[i].interlayer.value,line_edit_short_double_format,line_edit_interlayer_precision));

				if(interlayer_Composition_My_Sigma_Line_Edit_Vec [i]!=my_Sender)
					interlayer_Composition_My_Sigma_Line_Edit_Vec[i]->setText(Locale.toString(struct_Data.interlayer_Composition[i].my_Sigma.value/coeff,line_edit_short_double_format,line_edit_sigma_precision));
			}
			my_Sigma_Label_Layer->setText(sigma_Label_3 + length_units + sigma_Label_2);
		}
		show_Sigma(false);
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
			if(interlayer_Composition_Check_Box_Vec[i]->isChecked())
			{
				sum+=Locale.toDouble(interlayer_Composition_Comp_Line_Edit_Vec[i]->text());
			}
		}
		for(int i=0; i<transition_Layer_Functions_Size; ++i)
		{
			if(interlayer_Composition_Check_Box_Vec[i]->isChecked())
			{
				interlayer_Composition_Comp_Line_Edit_Vec[i]->setText(Locale.toString(Locale.toDouble(interlayer_Composition_Comp_Line_Edit_Vec[i]->text())/sum,line_edit_short_double_format,line_edit_interlayer_precision));
				struct_Data.interlayer_Composition[i].interlayer.value = Locale.toDouble(interlayer_Composition_Comp_Line_Edit_Vec[i]->text());
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
			struct_Data.relative_Density.value = Locale.toDouble(density_Line_Edit->text());
		} else
		{	
			double auto_Density = 0;
			double stoich_Weight = DBL_MIN;
			for(int i=0; i<struct_Data.composition.size(); ++i)
			{
				struct_Data.composition[i].composition.value = Locale.toDouble(composition_Line_Edit_Vec[i]->text());
				struct_Data.composition[i].composition.fit.min = struct_Data.composition[i].composition.value*(1-dispersion);
				struct_Data.composition[i].composition.fit.max = struct_Data.composition[i].composition.value*(1+dispersion);
				struct_Data.composition[i].composition.confidence.min = struct_Data.composition[i].composition.fit.min;
				struct_Data.composition[i].composition.confidence.max = struct_Data.composition[i].composition.fit.max;

				struct_Data.composition[i].type = composition_Combo_Box_Vec[i]->currentText();
				composition_At_Weight_Vec[i]->setText(AtWt + Locale.toString(sorted_Elements.value(composition_Combo_Box_Vec[i]->currentText()),thumbnail_double_format,at_weight_precision) + ")");

				auto_Density += sorted_Density.value(struct_Data.composition[i].type)*struct_Data.composition[i].composition.value;
				stoich_Weight += struct_Data.composition[i].composition.value;
			}

			if(auto_Density_On && auto_density_from_elements)
			{
				// automatical density
				struct_Data.absolute_Density.value = auto_Density/stoich_Weight;
				density_Line_Edit->setText(Locale.toString(struct_Data.absolute_Density.value,line_edit_double_format,line_edit_density_precision));
				Global_Variables::resize_Line_Edit(density_Line_Edit);
			} else
			{
				// previous density
				struct_Data.absolute_Density.value = Locale.toDouble(density_Line_Edit->text());
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
				struct_Data.interlayer_Composition[i].interlayer.value = Locale.toDouble(interlayer_Composition_Comp_Line_Edit_Vec[i]->text());

				// sum for normalizing
				sum+=struct_Data.interlayer_Composition[i].interlayer.value;
			}
		}
		if(abs(sum)<DBL_EPSILON) {sum = DBL_EPSILON; qInfo() << "Item_Editor::refresh_Data :: abs(sum)<DBL_EPSILON";}

		if(struct_Data.common_Sigma)
		{
			struct_Data.sigma.value = Locale.toDouble(sigma_Line_Edit->text())*coeff;
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
				struct_Data.interlayer_Composition[i].my_Sigma.value = Locale.toDouble(interlayer_Composition_My_Sigma_Line_Edit_Vec[i]->text())*coeff;
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
		struct_Data.thickness.value = Locale.toDouble(thickness_Line_Edit->text())*coeff;
	}

	// multilayer
	if(stack_Done)
	if(struct_Data.item_Type == item_Type_Multilayer)
	{
		double coeff = length_Coefficients_Map.value(length_units);
		double init_Period = struct_Data.period.value;

		struct_Data.num_Repetition.parameter.value = Locale.toDouble(repetitions_Line_Edit->text());
		struct_Data.period.value = Locale.toDouble(period_Line_Edit->text())*coeff;
		if(item->childCount()==2)
		{
			double temp_Gamma = Locale.toDouble(gamma_Line_Edit->text());
			if(temp_Gamma>1)
			{
				gamma_Line_Edit->setText(Locale.toString(struct_Data.gamma.value));
				Global_Variables::resize_Line_Edit(gamma_Line_Edit);
			} else
			{
				struct_Data.gamma.value = Locale.toDouble(gamma_Line_Edit->text());
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
	if( abs(Locale.toDouble(period_Line_Edit->text())) > DBL_MIN || abs(struct_Data.period.value) < DBL_MIN )
	{
		if(Locale.toInt(repetitions_Line_Edit->text())!=0 || struct_Data.num_Repetition.value() == 0)
		{
			if(item->childCount()==2)
			{
				if( abs(Locale.toDouble(gamma_Line_Edit->text())) > DBL_MIN || abs(struct_Data.gamma.value) < DBL_MIN )
				{
					if( abs(Locale.toDouble(gamma_Line_Edit->text()) - 1) > DBL_EPSILON || abs(struct_Data.gamma.value - 1) < DBL_EPSILON )
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
	if( abs(stack_Content.period.value) > DBL_MIN && stack_Content.num_Repetition.value()!=0)
	{
		factor = new_Thickness/(  stack_Content.period.value*stack_Content.num_Repetition.value()  );
		change_Child_Layers_Thickness(multilayer_Item, factor);
	}
}

void Item_Editor::make_Aperiodic()
{
	// set up aperiodic
	struct_Data.item_Type = item_Type_Aperiodic;
	QTreeWidgetItem* new_Layer;
	int old_Child_Count = item->childCount();
	for(int n=0; n<struct_Data.num_Repetition.value()-1; n++) // if N==0, still 1 period exists
	{
		for(int i=0; i<old_Child_Count; i++)
		{
			new_Layer = item->child(i)->clone();
			structure_Tree->structure_Toolbar->change_IDs_Of_Subtree(new_Layer);
			item->insertChild(item->childCount(), new_Layer);
		}
	}
	struct_Data.period.fit.is_Fitable = false;
	struct_Data.gamma.fit.is_Fitable = false;
	struct_Data.num_Repetition.parameter.fit.is_Fitable = false;
	struct_Data.num_Repetition.parameter.value = 1; // important!

	// save
	QVariant var;
	var.setValue( struct_Data );
	item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	if(item->childCount() <= 20)
	{	item->setExpanded(true);}
	else
	{	item->setExpanded(false);}
	structure_Tree->refresh__StructureTree__Data_and_Text();

	// close old editor
	close();

	// open new editor
	structure_Tree->if_DoubleClicked(item);
}

void Item_Editor::make_Multilayer()
{
//	calc_Uniqueness(); // no need

	// find period
	QVector<int> tested_Uniqueness_Vector(uniqueness_Vector.size());
	QVector<int> possible_Periods;
	int N=1;
	int true_Period = uniqueness_Vector.size();
	for(int period = 2; period<=uniqueness_Vector.size()/2.; period++)
	{
		if(uniqueness_Vector.size()%period==0) possible_Periods.append(period);
	}
	for(int period : possible_Periods)
	{
		N = uniqueness_Vector.size()/period;
		for(int n = 0; n<N; n++)
		{
			for(int i=0; i<period; i++)
			{
				tested_Uniqueness_Vector[i+n*period] = uniqueness_Vector[i];
			}
		}
		if(tested_Uniqueness_Vector == uniqueness_Vector)
		{
			true_Period = period;
			goto endlabel;
		}
	}
	endlabel:

	QMessageBox::StandardButton reply = QMessageBox::Yes;
	if(true_Period == uniqueness_Vector.size())
	{
		N=1;
		reply = QMessageBox::question(this,"Make periodic", "Aperiodic can't be reduced to\nperiodic multilayer with N>1.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	}
	if (reply == QMessageBox::Yes)
	{
		// remove excess children
		while(item->childCount()>true_Period)
		{
			item->removeChild(item->child(item->childCount()-1));
		}

		// set up multilayer
		struct_Data.item_Type = item_Type_Multilayer;
		struct_Data.num_Repetition.parameter.value = N;

		// save
		QVariant var;
		var.setValue( struct_Data );
		item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		if(item->childCount() <= 30)
		{	item->setExpanded(true);}
		else
		{	item->setExpanded(false);}
		structure_Tree->refresh__StructureTree__Data_and_Text();

		// close old editor
		close();

		// open new editor
		structure_Tree->if_DoubleClicked(item);	} else
	{
		make_Multilayer_CheckBox->setChecked(false);
		return;
	}
}

void Item_Editor::invert_Aperiodic()
{
	for(int i=0; i<item->childCount(); i++)
	{
		item->insertChild(item->childCount()-i, item->child(0)->clone());
		delete item->child(0);
	}
	structure_Tree->structure_Toolbar->refresh_Toolbar();
}

void Item_Editor::calc_Uniqueness()
{
	uniqueness_Vector.clear();
	int last_Uniqueness = 0;
	QMap<QString, int> uniqueness_Map;
	QTreeWidgetItem* current_Child_Item;

	for(int i=0; i<item->childCount(); i++)
	{
		current_Child_Item = item->child(i);
		Data current_Child_Data = current_Child_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		QString current_Characretistic; // we don't distinguish multilayers and aperiodics between themselves
		if(current_Child_Data.item_Type == item_Type_Layer)      current_Characretistic = current_Child_Data.item_Type + Locale.toString(current_Child_Data.composed_Material) + current_Child_Data.material;
		if(current_Child_Data.item_Type == item_Type_Multilayer) current_Characretistic = current_Child_Data.item_Type;
		if(current_Child_Data.item_Type == item_Type_Aperiodic)  current_Characretistic = current_Child_Data.item_Type;

		if(uniqueness_Map.contains(current_Characretistic))
		{
			current_Child_Data.uniqueness = uniqueness_Map.value(current_Characretistic);
		} else
		{
			current_Child_Data.uniqueness = last_Uniqueness;
			uniqueness_Map.insert(current_Characretistic, last_Uniqueness);
			last_Uniqueness++;
		}
		uniqueness_Vector.append(current_Child_Data.uniqueness);
		item_Uniqueness_Map.insert(current_Child_Item, current_Child_Data.uniqueness);

		// save
		QVariant var;
		var.setValue( current_Child_Data );
		current_Child_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
}

void Item_Editor::unique_Item_Do(QString action, int uniqueness)
{
//	calc_Uniqueness(); // no need

	QCheckBox* sender = qobject_cast<QCheckBox*>(QObject::sender());
	bool checked = sender->isChecked();

	bool first = true;
	QTreeWidgetItem* current_Child_Item = nullptr;
	QTreeWidgetItem* first_Child_Item = nullptr;
	Data first_Child_Data;

	for(int i=0; i<item->childCount(); i++)
	{
		current_Child_Item = item->child(i);
		Data current_Child_Data = current_Child_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(uniqueness == uniqueness_Vector[i])
		{
			if(first)
			{
				first_Child_Item = current_Child_Item;
				first_Child_Data = first_Child_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			}

			Parameter* current_Child_Parameter = nullptr;
			Parameter* first_Child_Parameter = nullptr;

			if(action == relate_Thickness || action == fit_Thickness)
			{
				current_Child_Parameter = &current_Child_Data.thickness;
				first_Child_Parameter   = &first_Child_Data.thickness;
			}
			if(action == relate_Sigma || action == fit_Sigma)
			{
				current_Child_Parameter = &current_Child_Data.sigma;
				first_Child_Parameter   = &first_Child_Data.sigma;
			}
			if(action == relate_Thickness || action == relate_Sigma)
			if(!first)
			{
				if(checked)
				{
					// add master to slave
//					current_Child_Parameter->coupled.is_Coupled = true;
					current_Child_Parameter->coupled.master = first_Child_Parameter->indicator;
					current_Child_Parameter->coupled.master.exist = true;

					// add slave to master
//					first_Child_Parameter->coupled.is_Coupled = true;
					first_Child_Parameter->coupled.slaves.append(current_Child_Parameter->indicator);
				} else
				{
					// remove master from slave
//					current_Child_Parameter->coupled.is_Coupled = false;
					current_Child_Parameter->coupled.master.id = 0;
					current_Child_Parameter->coupled.master.exist = false;
					current_Child_Parameter->coupled.slaves.clear();

					// remove slaves from master (many times, but OK)
//					first_Child_Parameter->coupled.is_Coupled = false;
					first_Child_Parameter->coupled.master.id = 0;
					first_Child_Parameter->coupled.master.exist = false;
					first_Child_Parameter->coupled.slaves.clear();
				}
			}
			if(action == fit_Thickness || action == fit_Sigma)
			{
				if(checked)
				{
					current_Child_Parameter->fit.is_Fitable = true;
				} else
				{
					current_Child_Parameter->fit.is_Fitable = false;
				}
			}

			// save
			QVariant var;
			var.setValue( current_Child_Data );
			current_Child_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

			first = false;
		}
	}
	if(!first)
	{
		if(action == relate_Thickness || action == relate_Sigma)
		{
			QVariant var;
			var.setValue( first_Child_Data );
			first_Child_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
	} else
	{
		qInfo() << "Item_Editor::unique_Item_Do  :  first == true";
	}
}

void Item_Editor::soft_Restriction_Save(int current_Uniqueness, bool use_Soft_Restrictions, int threshold, double Q_factor)
{
	for(int k=0; k<item->childCount(); k++)
	{
		Data child_Data = item->child(k)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(child_Data.uniqueness == current_Uniqueness)
		{
			child_Data.use_Soft_Restrictions = use_Soft_Restrictions;
			child_Data.threshold = threshold;
			child_Data.Q_factor = Q_factor;

			QVariant var;
			var.setValue( child_Data );
			item->child(k)->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}
	}
}
