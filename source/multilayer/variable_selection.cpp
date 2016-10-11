#include "variable_selection.h"

Variable_Selection::Variable_Selection(QTreeWidget* struct_Tree_Copy, QMap<QString, QListWidgetItem*>* independent_Variables_List_Map, QListWidget* independent_Variables_List, Variable_Type type):
	struct_Tree_Copy(struct_Tree_Copy),
	independent_Variables_List(independent_Variables_List),
	independent_Variables_List_Map(independent_Variables_List_Map),
	default_Values(Default_Values_Path, QSettings::IniFormat)
{
	create_Window();
	if(type==Variable_Type::Independent)
	{
		setWindowTitle("Choose independent variable");
		fill_Variables_List();
	}
	if(type==Variable_Type::Coupled)
	{
		setWindowTitle("Choose coupled parameter");
		// TODO
	}
	if(type==Variable_Type::Fitted)
	{
		setWindowTitle("Choose fitting parameter");
		// TODO
	}
	if(type==Variable_Type::Optimized)
	{
		setWindowTitle("Choose optimization parameter");
		// TODO
	}

	adjustSize();
	setFixedSize(size());

	setAttribute(Qt::WA_DeleteOnClose);
}

void Variable_Selection::closeEvent(QCloseEvent* event)
{
	emit is_Closed();
	event;
}

void Variable_Selection::create_Window()
{
	filtered_Parameters_Pages = new QStackedWidget;
	filters_Label = new QLabel("Filter :");
	filters_Combo_Box = new QComboBox;

	cancel_Button = new QPushButton("Cancel");
		cancel_Button->setFixedWidth(70);

	ok_Button = new QPushButton("Ok");
		ok_Button->setFixedWidth(40);
		ok_Button->setDefault(true);

	QFrame* hor_Frame = new QFrame;
		hor_Frame->setContentsMargins(0,-8,0,-10);
		hor_Frame->adjustSize();
	QHBoxLayout* hor_Layout = new QHBoxLayout(hor_Frame);
		hor_Layout->setSpacing(3);
		hor_Layout->addWidget(filters_Label);
		hor_Layout->addWidget(filters_Combo_Box);
		hor_Layout->setAlignment(Qt::AlignCenter);

	QFrame* but_Frame = new QFrame;
		but_Frame->setContentsMargins(0,-8,0,-10);
		but_Frame->adjustSize();
	QHBoxLayout* but_Layout = new QHBoxLayout(but_Frame);
		but_Layout->setSpacing(6);
		but_Layout->setAlignment(Qt::AlignCenter);
		but_Layout->addWidget(cancel_Button);
		but_Layout->addWidget(ok_Button);

	QVBoxLayout* ver_layout = new QVBoxLayout(this);
		ver_layout->setAlignment(Qt::AlignCenter);
		ver_layout->addWidget(filtered_Parameters_Pages);
		ver_layout->addWidget(hor_Frame);
		ver_layout->addWidget(but_Frame);

	connect(cancel_Button, SIGNAL(clicked()), this, SLOT(close()));
	connect(ok_Button    , SIGNAL(clicked()), this, SLOT(add_Variable()));
}

void Variable_Selection::fill_Variables_List()
{
	nonfiltered_Parameters = new QListWidget;
	optical_Constants = new QListWidget;
	interface_Parameters = new QListWidget;
	thickness_Parameters = new QListWidget;

	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		QTreeWidgetItem* item = *it;
		QString whats_This = item->whatsThis(default_Column);
		QStringList whats_This_List_Type = whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

		if(whats_This_List_Type[0] == whats_This_Ambient)	 fill_Ambient_Variables(item);		else
		if(whats_This_List_Type[0] == whats_This_Layer)		 fill_Layer_Variables(item);		else
		if(whats_This_List_Type[0] == whats_This_Multilayer) fill_Multilayer_Variables(item);	else
		if(whats_This_List_Type[0] == whats_This_Substrate)	 fill_Substrate_Variables(item);
		++it;
	}

	// add nonfiltered
	filters_Combo_Box->addItem("None");
	filtered_Parameters_Pages->addWidget(nonfiltered_Parameters);
	map_Of_Parameters_Lists.insert(filters_Combo_Box->itemText(filters_Combo_Box->count()-1), nonfiltered_Parameters);

	// add optical constants
	filters_Combo_Box->addItem("Optical Constants");
	filtered_Parameters_Pages->addWidget(optical_Constants);
	map_Of_Parameters_Lists.insert(filters_Combo_Box->itemText(filters_Combo_Box->count()-1), optical_Constants);

	// add interface parameters
	filters_Combo_Box->addItem("Interface Parameters");
	filtered_Parameters_Pages->addWidget(interface_Parameters);
	map_Of_Parameters_Lists.insert(filters_Combo_Box->itemText(filters_Combo_Box->count()-1), interface_Parameters);

	// add thickness parameters
	filters_Combo_Box->addItem("Thickness Parameters");
	filtered_Parameters_Pages->addWidget(thickness_Parameters);
	map_Of_Parameters_Lists.insert(filters_Combo_Box->itemText(filters_Combo_Box->count()-1), thickness_Parameters);

	connect(nonfiltered_Parameters, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(add_Variable()));
	connect(optical_Constants,		SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(add_Variable()));
	connect(interface_Parameters,	SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(add_Variable()));
	connect(thickness_Parameters,	SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(add_Variable()));

	connect(filters_Combo_Box, SIGNAL(currentIndexChanged(int)), this, SLOT(filter_Change(int)));
}

void Variable_Selection::fill_Ambient_Variables(QTreeWidgetItem* item)
{
	Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
	QString whats_This;

	// ambient density (if not Vacuum and not arbitrary optical constants)
	if(ambient.material!="Vacuum" && ambient.separate_Optical_Constants != Tril::True)
	{
		QListWidgetItem* ambient_Density = new QListWidgetItem;		
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Density;

		ambient_Density->setWhatsThis(whats_This);
		ambient_Density->setText(ambient.material + " (ambient) Density, " + Rho_Sym);

		if(!ambient.density.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(ambient_Density);
			optical_Constants->addItem(ambient_Density->clone());
		}
	}
	// ambient permittivity and absorption (if not Vacuum, not specified and not composed material)
	if(ambient.material!="Vacuum" && ambient.separate_Optical_Constants != Tril::False && ambient.composed_Material == false)
	{
		QListWidgetItem* ambient_Permittivity = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Permittivity;

		ambient_Permittivity->setWhatsThis(whats_This);
		ambient_Permittivity->setText(ambient.material + " (ambient) Permittivity, 1-" + Epsilon_Sym);

		if(!ambient.permittivity.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(ambient_Permittivity);
			optical_Constants->addItem(ambient_Permittivity->clone());
		}

		QListWidgetItem* ambient_Absorption = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Absorption;
		ambient_Absorption->setWhatsThis(whats_This);
		ambient_Absorption->setText(ambient.material + " (ambient) Absorption, " + Cappa_Sym);

		if(!ambient.absorption.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(ambient_Absorption);
			optical_Constants->addItem(ambient_Absorption->clone());
		}
	}
	// ambient composition (if not Vacuum, composed and >=2 elements)
	if(ambient.material!="Vacuum" && ambient.composed_Material == true && ambient.composition.size()>=2)
	{
		for(int i=0; i<ambient.composition.size(); i++)
		{
			QListWidgetItem* ambient_Composition = new QListWidgetItem;
			whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Composition + whats_This_Delimiter + QString::number(i);
			ambient_Composition->setWhatsThis(whats_This);
			ambient_Composition->setText(ambient.material + " (ambient) " + ambient.composition[i].type + " Composition, " + Zeta_Sym + "_" + ambient.composition[i].type);

			if(!ambient.composition[i].composition.independent.is_Independent)
			if(!independent_Variables_List_Map->contains(whats_This))
			{
				nonfiltered_Parameters->addItem(ambient_Composition);
				optical_Constants->addItem(ambient_Composition->clone());
			}
		}
	}
}

void Variable_Selection::fill_Layer_Variables(QTreeWidgetItem* item)
{
	Layer layer = item->data(default_Column, Qt::UserRole).value<Layer>();

	QString whats_This;
	QStringList whats_This_List = item->whatsThis(default_Column).split(item_Type_Delimiter,QString::SkipEmptyParts);
	QString layer_Number = whats_This_List[1].mid(1,1);

	/// optical constants

	// layer density (if not Vacuum and not arbitrary optical constants)
	if(layer.material!="Vacuum" && layer.separate_Optical_Constants != Tril::True)
	{
		QListWidgetItem* layer_Density = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Density;

		layer_Density->setWhatsThis(whats_This);
		layer_Density->setText(layer.material + " (layer " + layer_Number + ") Density, " + Rho_Sym);

		if(!layer.density.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(layer_Density);
			optical_Constants->addItem(layer_Density->clone());
		}
	}
	// layer permittivity and absorption (if not Vacuum, not specified and not composed material)
	if(layer.material!="Vacuum" && layer.separate_Optical_Constants != Tril::False && layer.composed_Material == false)
	{
		QListWidgetItem* layer_Permittivity = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Permittivity;

		layer_Permittivity->setWhatsThis(whats_This);
		layer_Permittivity->setText(layer.material + " (layer " + layer_Number + ") Permittivity, 1-" + Epsilon_Sym);

		if(!layer.permittivity.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(layer_Permittivity);
			optical_Constants->addItem(layer_Permittivity->clone());
		}

		QListWidgetItem* layer_Absorption = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Absorption;
		layer_Absorption->setWhatsThis(whats_This);
		layer_Absorption->setText(layer.material + " (layer " + layer_Number + ") Absorption, " + Cappa_Sym);

		if(!layer.absorption.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(layer_Absorption);
			optical_Constants->addItem(layer_Absorption->clone());
		}
	}
	// layer composition (if not Vacuum, composed and >=2 elements)
	if(layer.material!="Vacuum" && layer.composed_Material == true && layer.composition.size()>=2)
	{
		for(int i=0; i<layer.composition.size(); i++)
		{
			QListWidgetItem* layer_Composition = new QListWidgetItem;
			whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Composition + whats_This_Delimiter + QString::number(i);
			layer_Composition->setWhatsThis(whats_This);
			layer_Composition->setText(layer.material + " (layer " + layer_Number + ") " + layer.composition[i].type + " Composition, " + Zeta_Sym + "_" + layer.composition[i].type);

			if(!layer.composition[i].composition.independent.is_Independent)
			if(!independent_Variables_List_Map->contains(whats_This))
			{
				nonfiltered_Parameters->addItem(layer_Composition);
				optical_Constants->addItem(layer_Composition->clone());
			}
		}
	}

	/// thickness parameters

	// layer thickness (if parent's stack parameters are not independent)
	bool thickness_Condition = true;
	iterate_Over_Parents(item, thickness_Condition);
	if(thickness_Condition)
	{
		QListWidgetItem* layer_Thickness = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Thickness;

		layer_Thickness->setWhatsThis(whats_This);
		layer_Thickness->setText(layer.material + " (layer " + layer_Number + ") Thickness, z");

		if(!layer.thickness.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(layer_Thickness);
			thickness_Parameters->addItem(layer_Thickness->clone());
		}
	}

	/// interface parameters

	// layer sigma
	{
		QListWidgetItem* layer_Sigma = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Sigma;

		layer_Sigma->setWhatsThis(whats_This);
		layer_Sigma->setText(layer.material + " (layer " + layer_Number + ") Roughness/Diffuseness, " + Sigma_Sym);

		if(!layer.sigma.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(layer_Sigma);
			interface_Parameters->addItem(layer_Sigma->clone());
		}
	}
	// layer interlayer composition (if enabled and >=2 elements)
	{
		int interlayers_Counter=0;
		for(int i=0; i<layer.interlayer_Composition.size(); i++)
		{
			if(layer.interlayer_Composition[i].enabled) ++interlayers_Counter;
		}

		for(int i=0; i<layer.interlayer_Composition.size(); i++)
		{
			QListWidgetItem* layer_Composition = new QListWidgetItem;
			whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Interlayer_Composition + whats_This_Delimiter + QString::number(i);
			layer_Composition->setWhatsThis(whats_This);
			layer_Composition->setText(layer.material + " (layer " + layer_Number + ") Interlayer Composition, " + transition_Layer_Functions[i]);

			if(layer.interlayer_Composition[i].enabled && interlayers_Counter>=2)
			if(!layer.interlayer_Composition[i].interlayer.independent.is_Independent)
			if(!independent_Variables_List_Map->contains(whats_This))
			{
				nonfiltered_Parameters->addItem(layer_Composition);
				interface_Parameters->addItem(layer_Composition->clone());
			}
		}
	}
}

void Variable_Selection::fill_Multilayer_Variables(QTreeWidgetItem* item)
{
	Stack_Content stack_Content = item->data(default_Column, Qt::UserRole).value<Stack_Content>();
	QString whats_This;

	// stack_Content num_Repetition (if parent's stack parameters are not independent)
	bool thickness_Condition = true;
	iterate_Over_Parents(item, thickness_Condition);
	if(thickness_Condition)
	{
		QListWidgetItem* stack_Content_Num_Repetition = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Num_Repetitions;

		stack_Content_Num_Repetition->setWhatsThis(whats_This);
		stack_Content_Num_Repetition->setText(item->whatsThis(default_Column) + " Number of repetitions, N");

		if(!stack_Content.num_Repetition.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(stack_Content_Num_Repetition);
			thickness_Parameters->addItem(stack_Content_Num_Repetition->clone());
		}
	}
	// stack_Content period (if parent stacks are not independent and children don't have independent thicknesses or periods or num_Repetitions)
	thickness_Condition = true;
	iterate_Over_Parents (item, thickness_Condition);
	iterate_Over_Children(item, thickness_Condition);
	if(thickness_Condition)
	{
		QListWidgetItem* stack_Content_Period = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Period;

		stack_Content_Period->setWhatsThis(whats_This);
		stack_Content_Period->setText(item->whatsThis(default_Column) + " Period, d");

		if(!stack_Content.period.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(stack_Content_Period);
			thickness_Parameters->addItem(stack_Content_Period->clone());
		}
	}
	// stack_Content gamma (if have 2 children and children don't have independent thicknesses or periods or num_Repetitions)
	thickness_Condition = true;
	iterate_Over_Children(item, thickness_Condition);
	if(item->childCount()==2 && thickness_Condition)
	{
		QListWidgetItem* stack_Content_Gamma = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Gamma;

		stack_Content_Gamma->setWhatsThis(whats_This);
		stack_Content_Gamma->setText(item->whatsThis(default_Column) + " Thickness Ratio, " + Gamma_Sym);

		if(!stack_Content.gamma.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(stack_Content_Gamma);
			thickness_Parameters->addItem(stack_Content_Gamma->clone());
		}
	}
}

void Variable_Selection::fill_Substrate_Variables(QTreeWidgetItem* item)
{
	Substrate substrate = item->data(default_Column, Qt::UserRole).value<Substrate>();

	QString whats_This;

	/// optical constants

	// substrate density (if not Vacuum and not arbitrary optical constants)
	if(substrate.material!="Vacuum" && substrate.separate_Optical_Constants != Tril::True)
	{
		QListWidgetItem* substrate_Density = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Density;

		substrate_Density->setWhatsThis(whats_This);
		substrate_Density->setText(substrate.material + " (substrate) Density, " + Rho_Sym);

		if(!substrate.density.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(substrate_Density);
			optical_Constants->addItem(substrate_Density->clone());
		}
	}
	// substrate permittivity and absorption (if not Vacuum, not specified and not composed material)
	if(substrate.material!="Vacuum" && substrate.separate_Optical_Constants != Tril::False && substrate.composed_Material == false)
	{
		QListWidgetItem* substrate_Permittivity = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Permittivity;

		substrate_Permittivity->setWhatsThis(whats_This);
		substrate_Permittivity->setText(substrate.material + " (substrate) Permittivity, 1-" + Epsilon_Sym);

		if(!substrate.permittivity.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(substrate_Permittivity);
			optical_Constants->addItem(substrate_Permittivity->clone());
		}

		QListWidgetItem* substrate_Absorption = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Absorption;
		substrate_Absorption->setWhatsThis(whats_This);
		substrate_Absorption->setText(substrate.material + " (substrate) Absorption, " + Cappa_Sym);

		if(!substrate.absorption.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(substrate_Absorption);
			optical_Constants->addItem(substrate_Absorption->clone());
		}
	}
	// substrate composition (if not Vacuum, composed and >=2 elements)
	if(substrate.material!="Vacuum" && substrate.composed_Material == true && substrate.composition.size()>=2)
	{
		for(int i=0; i<substrate.composition.size(); i++)
		{
			QListWidgetItem* substrate_Composition = new QListWidgetItem;
			whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Composition + whats_This_Delimiter + QString::number(i);
			substrate_Composition->setWhatsThis(whats_This);
			substrate_Composition->setText(substrate.material + " (substrate) " + substrate.composition[i].type + " Composition, " + Zeta_Sym + "_" + substrate.composition[i].type);

			if(!substrate.composition[i].composition.independent.is_Independent)
			if(!independent_Variables_List_Map->contains(whats_This))
			{
				nonfiltered_Parameters->addItem(substrate_Composition);
				optical_Constants->addItem(substrate_Composition->clone());
			}
		}
	}

	/// interface parameters

	// substrate sigma
	{
		QListWidgetItem* substrate_Sigma = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Sigma;

		substrate_Sigma->setWhatsThis(whats_This);
		substrate_Sigma->setText(substrate.material + " (substrate) Roughness/Diffuseness, " + Sigma_Sym);

		if(!substrate.sigma.independent.is_Independent)
		if(!independent_Variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(substrate_Sigma);
			interface_Parameters->addItem(substrate_Sigma->clone());
		}
	}
	// substrate interlayer composition (if enabled and >=2 elements)
	{
		int interlayers_Counter=0;
		for(int i=0; i<substrate.interlayer_Composition.size(); i++)
		{
			if(substrate.interlayer_Composition[i].enabled) ++interlayers_Counter;
		}

		for(int i=0; i<substrate.interlayer_Composition.size(); i++)
		{
			QListWidgetItem* substrate_Composition = new QListWidgetItem;
			whats_This = item->whatsThis(default_Column) + whats_This_Delimiter + whats_This_Interlayer_Composition + whats_This_Delimiter + QString::number(i);
			substrate_Composition->setWhatsThis(whats_This);
			substrate_Composition->setText(substrate.material + " (substrate) Interlayer Composition, " + transition_Layer_Functions[i]);

			if(substrate.interlayer_Composition[i].enabled && interlayers_Counter>=2)
			if(!substrate.interlayer_Composition[i].interlayer.independent.is_Independent)
			if(!independent_Variables_List_Map->contains(whats_This))
			{
				nonfiltered_Parameters->addItem(substrate_Composition);
				interface_Parameters->addItem(substrate_Composition->clone());
			}
		}
	}
}

void Variable_Selection::add_Variable()
{
	default_Values.beginGroup( Structure_Values_Representation );
		int default_density_precision = default_Values.value( "default_density_precision", 0 ).toInt();
	default_Values.endGroup();

	if(map_Of_Parameters_Lists.value(filters_Combo_Box->currentText())->currentItem())
	{
		QListWidgetItem* new_Item = map_Of_Parameters_Lists.value(filters_Combo_Box->currentText())->currentItem()->clone();

		independent_Variables_List_Map->insert(new_Item->whatsThis(), new_Item);
		independent_Variables_List->addItem(new_Item);

		// sorting
		QString whats_This = new_Item->whatsThis();
		QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
		QStringList whats_This_List_Type = whats_This_List[0].split(item_Type_Delimiter,QString::SkipEmptyParts);
		QVariant var;

		// item search
		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(struct_Tree_Copy);
		while (*it)
		{
			structure_Item = *it;
			if(structure_Item->whatsThis(default_Column) == whats_This_List[0])
				break;
			++it;
		}

		// if ambient
		if(whats_This_List_Type[0] == whats_This_Ambient)
		{
			Ambient ambient = structure_Item->data(default_Column, Qt::UserRole).value<Ambient>();

			/// optical constants

			// ambient density
			if(whats_This_List[1] == whats_This_Density)
			{
				ambient.density.independent.is_Independent = true;
				ambient.density.independent.num_Points = 1;
				ambient.density.independent.min = ambient.density.value;
				ambient.density.independent.max = ambient.density.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(ambient.density.value,'f',default_density_precision) + " g/cm" + Cube_Sym + "]");
			}
			// ambient permittivity
			if(whats_This_List[1] == whats_This_Permittivity)
			{
				ambient.permittivity.independent.is_Independent = true;
				ambient.permittivity.independent.num_Points = 1;
				ambient.permittivity.independent.min = ambient.permittivity.value;
				ambient.permittivity.independent.max = ambient.permittivity.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(ambient.permittivity.value,'f',default_density_precision) + " % of nominal]");
			}
			// ambient absorption
			if(whats_This_List[1] == whats_This_Absorption)
			{
				ambient.absorption.independent.is_Independent = true;
				ambient.absorption.independent.num_Points = 1;
				ambient.absorption.independent.min = ambient.absorption.value;
				ambient.absorption.independent.max = ambient.absorption.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(ambient.absorption.value,'f',default_density_precision) + " % of nominal]");
			}
			//ambient composition
			if(whats_This_List[1] == whats_This_Composition)
			{
				int index = QString(whats_This_List[2]).toInt();
				ambient.composition[index].composition.independent.is_Independent = true;
				ambient.composition[index].composition.independent.num_Points = 1;
				ambient.composition[index].composition.independent.min = ambient.composition[index].composition.value;
				ambient.composition[index].composition.independent.max = ambient.composition[index].composition.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(ambient.composition[index].composition.value,'f',default_density_precision) + "]");
			}

			var.setValue(ambient);
			structure_Item->setData(default_Column, Qt::UserRole, var);
		}

		// if layer
		if(whats_This_List_Type[0] == whats_This_Layer)
		{
			Layer layer = structure_Item->data(default_Column, Qt::UserRole).value<Layer>();

			/// optical constants

			// layer density
			if(whats_This_List[1] == whats_This_Density)
			{
				layer.density.independent.is_Independent = true;
				layer.density.independent.num_Points = 1;
				layer.density.independent.min = layer.density.value;
				layer.density.independent.max = layer.density.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.density.value,'f',default_density_precision) + " g/cm" + Cube_Sym + "]");
			}
			// layer permittivity
			if(whats_This_List[1] == whats_This_Permittivity)
			{
				layer.permittivity.independent.is_Independent = true;
				layer.permittivity.independent.num_Points = 1;
				layer.permittivity.independent.min = layer.permittivity.value;
				layer.permittivity.independent.max = layer.permittivity.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.permittivity.value,'f',default_density_precision) + " % of nominal]");
			}
			// layer absorption
			if(whats_This_List[1] == whats_This_Absorption)
			{
				layer.absorption.independent.is_Independent = true;
				layer.absorption.independent.num_Points = 1;
				layer.absorption.independent.min = layer.absorption.value;
				layer.absorption.independent.max = layer.absorption.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.absorption.value,'f',default_density_precision) + " % of nominal]");
			}
			// layer composition
			if(whats_This_List[1] == whats_This_Composition)
			{
				int index = QString(whats_This_List[2]).toInt();
				layer.composition[index].composition.independent.is_Independent = true;
				layer.composition[index].composition.independent.num_Points = 1;
				layer.composition[index].composition.independent.min = layer.composition[index].composition.value;
				layer.composition[index].composition.independent.max = layer.composition[index].composition.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.composition[index].composition.value,'f',default_density_precision) + "]");
			}

			/// thickness parameters

			// layer thickness
			if(whats_This_List[1] == whats_This_Thickness)
			{
				layer.thickness.independent.is_Independent = true;
				layer.thickness.independent.num_Points = 1;
				layer.thickness.independent.min = layer.thickness.value;
				layer.thickness.independent.max = layer.thickness.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.thickness.value,'f',default_density_precision) + " " + Angstrom_Sym + "]");
			}

			/// interface parameters

			// layer sigma
			if(whats_This_List[1] == whats_This_Sigma)
			{
				layer.sigma.independent.is_Independent = true;
				layer.sigma.independent.num_Points = 1;
				layer.sigma.independent.min = layer.sigma.value;
				layer.sigma.independent.max = layer.sigma.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.sigma.value,'f',default_density_precision) + " " + Angstrom_Sym + "]");
			}
			// layer interlayer composition (if enabled and >=2 elements)
			if(whats_This_List[1] == whats_This_Interlayer_Composition)
			{
				int index = QString(whats_This_List.last()).toInt();
				layer.interlayer_Composition[index].interlayer.independent.is_Independent = true;
				layer.interlayer_Composition[index].interlayer.independent.num_Points = 1;
				layer.interlayer_Composition[index].interlayer.independent.min = layer.interlayer_Composition[index].interlayer.value;
				layer.interlayer_Composition[index].interlayer.independent.max = layer.interlayer_Composition[index].interlayer.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.interlayer_Composition[index].interlayer.value,'f',default_density_precision) + "]");
			}

			var.setValue(layer);
			structure_Item->setData(default_Column, Qt::UserRole, var);
		}

		// if multilayer
		if(whats_This_List_Type[0] == whats_This_Multilayer)
		{
			Stack_Content stack_Content = structure_Item->data(default_Column, Qt::UserRole).value<Stack_Content>();

			// multilayer num_repetitions
			if(whats_This_List[1] == whats_This_Num_Repetitions)
			{
				stack_Content.num_Repetition.is_Independent = true;
				stack_Content.num_Repetition.num_steps = 0;
				stack_Content.num_Repetition.start = stack_Content.num_Repetition.value;
				stack_Content.num_Repetition.step = 1;

				new_Item->setText(new_Item->text() + " [" + QString::number(stack_Content.num_Repetition.value) + "]");
			}
			// multilayer period
			if(whats_This_List[1] == whats_This_Period)
			{
				stack_Content.period.independent.is_Independent = true;
				stack_Content.period.independent.num_Points = 1;
				stack_Content.period.independent.min = stack_Content.period.value;
				stack_Content.period.independent.max = stack_Content.period.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(stack_Content.period.value,'f',default_density_precision) + " " + Angstrom_Sym + "]");
			}
			// multilayer gamma
			if(whats_This_List[1] == whats_This_Gamma)
			{
				stack_Content.gamma.independent.is_Independent = true;
				stack_Content.gamma.independent.num_Points = 1;
				stack_Content.gamma.independent.min = stack_Content.gamma.value;
				stack_Content.gamma.independent.max = stack_Content.gamma.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(stack_Content.gamma.value,'f',default_density_precision) + "]");
			}

			var.setValue(stack_Content);
			structure_Item->setData(default_Column, Qt::UserRole, var);
		}

		// if substrate
		if(whats_This_List_Type[0] == whats_This_Substrate)
		{
			Substrate substrate = structure_Item->data(default_Column, Qt::UserRole).value<Substrate>();

			/// optical constants

			// substrate density
			if(whats_This_List[1] == whats_This_Density)
			{
				substrate.density.independent.is_Independent = true;
				substrate.density.independent.num_Points = 1;
				substrate.density.independent.min = substrate.density.value;
				substrate.density.independent.max = substrate.density.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.density.value,'f',default_density_precision) + " g/cm" + Cube_Sym + "]");
			}
			// substrate permittivity
			if(whats_This_List[1] == whats_This_Permittivity)
			{
				substrate.permittivity.independent.is_Independent = true;
				substrate.permittivity.independent.num_Points = 1;
				substrate.permittivity.independent.min = substrate.permittivity.value;
				substrate.permittivity.independent.max = substrate.permittivity.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.permittivity.value,'f',default_density_precision) + " % of nominal]");
			}
			// substrate absorption
			if(whats_This_List[1] == whats_This_Absorption)
			{
				substrate.absorption.independent.is_Independent = true;
				substrate.absorption.independent.num_Points = 1;
				substrate.absorption.independent.min = substrate.absorption.value;
				substrate.absorption.independent.max = substrate.absorption.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.absorption.value,'f',default_density_precision) + " % of nominal]");
			}
			// substrate composition
			if(whats_This_List[1] == whats_This_Composition)
			{
				int index = QString(whats_This_List[2]).toInt();
				substrate.composition[index].composition.independent.is_Independent = true;
				substrate.composition[index].composition.independent.num_Points = 1;
				substrate.composition[index].composition.independent.min = substrate.composition[index].composition.value;
				substrate.composition[index].composition.independent.max = substrate.composition[index].composition.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.composition[index].composition.value,'f',default_density_precision) + "]");
			}

			/// interface parameters

			// substrate sigma
			if(whats_This_List[1] == whats_This_Sigma)
			{
				substrate.sigma.independent.is_Independent = true;
				substrate.sigma.independent.num_Points = 1;
				substrate.sigma.independent.min = substrate.sigma.value;
				substrate.sigma.independent.max = substrate.sigma.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.sigma.value,'f',default_density_precision) + " " + Angstrom_Sym + "]");
			}
			// substrate interlayer composition (if enabled and >=2 elements)
			if(whats_This_List[1] == whats_This_Interlayer_Composition)
			{
				int index = QString(whats_This_List.last()).toInt();
				substrate.interlayer_Composition[index].interlayer.independent.is_Independent = true;
				substrate.interlayer_Composition[index].interlayer.independent.num_Points = 1;
				substrate.interlayer_Composition[index].interlayer.independent.min = substrate.interlayer_Composition[index].interlayer.value;
				substrate.interlayer_Composition[index].interlayer.independent.max = substrate.interlayer_Composition[index].interlayer.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.interlayer_Composition[index].interlayer.value,'f',default_density_precision) + "]");
			}

			var.setValue(substrate);
			structure_Item->setData(default_Column, Qt::UserRole, var);
		}
	}
	close();
}

void Variable_Selection::filter_Change(int index)
{
	filtered_Parameters_Pages->setCurrentIndex(index);
}

void Variable_Selection::iterate_Over_Children(QTreeWidgetItem* this_Stack, bool& thickness_Condition)
{
	Stack_Content stack_Content;
	Layer layer;
	QTreeWidgetItem* baby;

	for(int i=0; i<this_Stack->childCount(); ++i)
	{
		baby = this_Stack->child(i);

		// if layer
		if(baby->childCount()==0)
		{
			layer = baby->data(default_Column, Qt::UserRole).value<Layer>();
			thickness_Condition = thickness_Condition && !layer.thickness.independent.is_Independent;
		}

		// if multilayer
		if(baby->childCount()>0)
		{
			stack_Content = baby->data(default_Column, Qt::UserRole).value<Stack_Content>();
			thickness_Condition = thickness_Condition && (!stack_Content.period.independent.is_Independent && !stack_Content.num_Repetition.is_Independent);
			iterate_Over_Children(baby, thickness_Condition);
		}
	}
}

void Variable_Selection::iterate_Over_Parents(QTreeWidgetItem* this_Item, bool& thickness_Condition)
{
	if(this_Item->parent())
	{
		Stack_Content stack_Content;
		QTreeWidgetItem* parent = this_Item->parent();
		while(parent)
		{
			stack_Content = parent->data(default_Column, Qt::UserRole).value<Stack_Content>();
			//											  if                 false						  and		                false
			thickness_Condition = thickness_Condition && (!stack_Content.gamma.independent.is_Independent && !stack_Content.period.independent.is_Independent);
			parent = parent->parent();
		}
	}
}
