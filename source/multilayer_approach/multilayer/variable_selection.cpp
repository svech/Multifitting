#include "variable_selection.h"

Variable_Selection::Variable_Selection(QTreeWidget* struct_Tree_Copy, QMap<QString, QListWidgetItem*>* variables_List_Map, QListWidget* variables_List, Variable_Type type, QWidget *parent) :
	QDialog(parent),
	struct_Tree_Copy(struct_Tree_Copy),
	variables_List(variables_List),
	variables_List_Map(variables_List_Map),
	type(type)
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
	emit closed();
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
		QString whats_This = item->whatsThis(DEFAULT_COLUMN);
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
	/// optical constants
	add_Density		(item, whats_This_Ambient);
	add_Permittivity(item, whats_This_Ambient);
	add_Absorption	(item, whats_This_Ambient);
	add_Composition	(item, whats_This_Ambient);
}

void Variable_Selection::fill_Layer_Variables(QTreeWidgetItem* item)
{
	/// optical constants
	add_Density					(item, whats_This_Layer);
	add_Permittivity			(item, whats_This_Layer);
	add_Absorption				(item, whats_This_Layer);
	add_Composition				(item, whats_This_Layer);
	/// thickness parameters
	add_Thickness				(item, whats_This_Layer);
	/// interface parameters
	add_Sigma					(item, whats_This_Layer);
	add_Interlayer_Composition	(item, whats_This_Layer);
}

void Variable_Selection::fill_Multilayer_Variables(QTreeWidgetItem* item)
{
	add_Num_repetitions	(item, whats_This_Multilayer);
	add_Period			(item, whats_This_Multilayer);
	add_Gamma			(item, whats_This_Multilayer);
}

void Variable_Selection::fill_Substrate_Variables(QTreeWidgetItem* item)
{
	/// optical constants
	add_Density					(item, whats_This_Substrate);
	add_Permittivity			(item, whats_This_Substrate);
	add_Absorption				(item, whats_This_Substrate);
	add_Composition				(item, whats_This_Substrate);
	/// interface parameters
	add_Sigma					(item, whats_This_Substrate);
	add_Interlayer_Composition	(item, whats_This_Substrate);
}

// adding parameters one by one

void Variable_Selection::add_Density(QTreeWidgetItem* item, QString whats_This_Type)
{
	QString material, brackets;
	QString separate_Optical_Constants;
	bool is_True_Condition;

	if(whats_This_Type == whats_This_Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		material = ambient.material;
		brackets = "(ambient)";
		separate_Optical_Constants = ambient.separate_Optical_Constants;

		if(type==Variable_Type::Independent)	is_True_Condition = ambient.density.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = ambient.density.coupled.	is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = ambient.density.fit.		is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = ambient.density.optimize.	is_Optimizable;
	} else
	if(whats_This_Type == whats_This_Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		material = layer.material;
		brackets = "(layer " + QString::number(layer.layer_Index) + ")";
		separate_Optical_Constants = layer.separate_Optical_Constants;

		if(type==Variable_Type::Independent)	is_True_Condition = layer.density.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = layer.density.coupled.	  is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = layer.density.fit.		  is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = layer.density.optimize.	  is_Optimizable;
	} else
	if(whats_This_Type == whats_This_Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		material = substrate.material;
		brackets = "(substrate)";
		separate_Optical_Constants = substrate.separate_Optical_Constants;

		if(type==Variable_Type::Independent)	is_True_Condition = substrate.density.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = substrate.density.coupled.	  is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = substrate.density.fit.		  is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = substrate.density.optimize.	  is_Optimizable;
	} else return;

	// item density (if not Vacuum and not arbitrary optical constants)
	if(material!="Vacuum" && separate_Optical_Constants != TRIL_TRUE)
	{
		QListWidgetItem* item_Density = new QListWidgetItem;
		QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Density;
		item_Density->setWhatsThis(whats_This);
		item_Density->setText(material + " " + brackets + " Density, " + Rho_Sym);

		if(!is_True_Condition)
		if(!variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(item_Density);
			optical_Constants->addItem(item_Density->clone());
		}
	}
}

void Variable_Selection::add_Permittivity(QTreeWidgetItem* item, QString whats_This_Type)
{
	QString material, brackets;
	QString separate_Optical_Constants;
	bool is_True_Condition, composed_Material;

	if(whats_This_Type == whats_This_Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		material = ambient.material;
		brackets = "(ambient)";
		separate_Optical_Constants = ambient.separate_Optical_Constants;
		composed_Material = ambient.composed_Material;

		if(type==Variable_Type::Independent)	is_True_Condition = ambient.permittivity.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = ambient.permittivity.coupled.	is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = ambient.permittivity.fit.		is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = ambient.permittivity.optimize.	is_Optimizable;
	} else
	if(whats_This_Type == whats_This_Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		material = layer.material;
		brackets = "(layer " + QString::number(layer.layer_Index) + ")";
		separate_Optical_Constants = layer.separate_Optical_Constants;
		composed_Material = layer.composed_Material;

		if(type==Variable_Type::Independent)	is_True_Condition = layer.permittivity.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = layer.permittivity.coupled.	   is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = layer.permittivity.fit.		   is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = layer.permittivity.optimize.   is_Optimizable;
	} else
	if(whats_This_Type == whats_This_Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		material = substrate.material;
		brackets = "(substrate)";
		separate_Optical_Constants = substrate.separate_Optical_Constants;
		composed_Material = substrate.composed_Material;

		if(type==Variable_Type::Independent)	is_True_Condition = substrate.permittivity.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = substrate.permittivity.coupled.	   is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = substrate.permittivity.fit.		   is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = substrate.permittivity.optimize.   is_Optimizable;
	} else return;

	// item permittivity (if not Vacuum, not specified and not composed material)
	if(material!="Vacuum" && separate_Optical_Constants != TRIL_FALSE && composed_Material == false)
	{
		QListWidgetItem* item_Permittivity = new QListWidgetItem;
		QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Permittivity;

		item_Permittivity->setWhatsThis(whats_This);
		item_Permittivity->setText(material + " " + brackets + " Permittivity, 1-" + Epsilon_Sym);

		if(!is_True_Condition)
		if(!variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(item_Permittivity);
			optical_Constants->addItem(item_Permittivity->clone());
		}
	}
}

void Variable_Selection::add_Absorption(QTreeWidgetItem* item, QString whats_This_Type)
{
	QString material, brackets;
	QString separate_Optical_Constants;
	bool is_True_Condition, composed_Material;

	if(whats_This_Type == whats_This_Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		material = ambient.material;
		brackets = "(ambient)";
		separate_Optical_Constants = ambient.separate_Optical_Constants;
		composed_Material = ambient.composed_Material;

		if(type==Variable_Type::Independent)	is_True_Condition = ambient.absorption.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = ambient.absorption.coupled.	is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = ambient.absorption.fit.		is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = ambient.absorption.optimize.	is_Optimizable;
	} else
	if(whats_This_Type == whats_This_Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		material = layer.material;
		brackets = "(layer " + QString::number(layer.layer_Index) + ")";
		separate_Optical_Constants = layer.separate_Optical_Constants;
		composed_Material = layer.composed_Material;

		if(type==Variable_Type::Independent)	is_True_Condition = layer.absorption.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = layer.absorption.coupled.	   is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = layer.absorption.fit.		   is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = layer.absorption.optimize.   is_Optimizable;
	} else
	if(whats_This_Type == whats_This_Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		material = substrate.material;
		brackets = "(substrate)";
		separate_Optical_Constants = substrate.separate_Optical_Constants;
		composed_Material = substrate.composed_Material;

		if(type==Variable_Type::Independent)	is_True_Condition = substrate.absorption.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = substrate.absorption.coupled.	   is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = substrate.absorption.fit.		   is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = substrate.absorption.optimize.   is_Optimizable;
	} else return;

	// item absorption (if not Vacuum, not specified and not composed material)
	if(material!="Vacuum" && separate_Optical_Constants != TRIL_FALSE && composed_Material == false)
	{
		QListWidgetItem* item_Absorption = new QListWidgetItem;
		QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Absorption;

		item_Absorption->setWhatsThis(whats_This);
		item_Absorption->setText(material + " " + brackets + " Absorption, " + Cappa_Sym);

		if(!is_True_Condition)
		if(!variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(item_Absorption);
			optical_Constants->addItem(item_Absorption->clone());
		}
	}
}

void Variable_Selection::add_Composition(QTreeWidgetItem* item, QString whats_This_Type)
{
	QString material, brackets;
	bool composed_Material;
	QVector<bool> is_True_Condition;
	QStringList composition_type;
	int composition_Size;

	if(whats_This_Type == whats_This_Ambient)
	{
		Ambient ambient = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
		material = ambient.material;
		brackets = "(ambient)";
		composed_Material = ambient.composed_Material;
		composition_Size = ambient.composition.size();
		for(int i=0; i<composition_Size; i++)	{composition_type.append(ambient.composition[i].type);}

		if(type==Variable_Type::Independent)	for(int i=0; i<composition_Size; i++) {is_True_Condition.append(ambient.composition[i].composition.independent.is_Independent);}
		if(type==Variable_Type::Coupled)		for(int i=0; i<composition_Size; i++) {is_True_Condition.append(ambient.composition[i].composition.coupled.	   is_Coupled	 );}
		if(type==Variable_Type::Fitted)			for(int i=0; i<composition_Size; i++) {is_True_Condition.append(ambient.composition[i].composition.fit.		   is_Fitable	 );}
		if(type==Variable_Type::Optimized)		for(int i=0; i<composition_Size; i++) {is_True_Condition.append(ambient.composition[i].composition.optimize.   is_Optimizable);}
	} else
	if(whats_This_Type == whats_This_Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		material = layer.material;
		brackets = "(layer " + QString::number(layer.layer_Index) + ")";
		composed_Material = layer.composed_Material;
		composition_Size = layer.composition.size();
		for(int i=0; i<composition_Size; i++)	{composition_type.append(layer.composition[i].type);}

		if(type==Variable_Type::Independent)	for(int i=0; i<composition_Size; i++) {is_True_Condition.append(layer.composition[i].composition.independent.is_Independent);}
		if(type==Variable_Type::Coupled)		for(int i=0; i<composition_Size; i++) {is_True_Condition.append(layer.composition[i].composition.coupled.	 is_Coupled	   );}
		if(type==Variable_Type::Fitted)			for(int i=0; i<composition_Size; i++) {is_True_Condition.append(layer.composition[i].composition.fit.		 is_Fitable	   );}
		if(type==Variable_Type::Optimized)		for(int i=0; i<composition_Size; i++) {is_True_Condition.append(layer.composition[i].composition.optimize.	 is_Optimizable);}
	} else
	if(whats_This_Type == whats_This_Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		material = substrate.material;
		brackets = "(substrate)";
		composed_Material = substrate.composed_Material;
		composition_Size = substrate.composition.size();
		for(int i=0; i<composition_Size; i++)	{composition_type.append(substrate.composition[i].type);}

		if(type==Variable_Type::Independent)	for(int i=0; i<composition_Size; i++) {is_True_Condition.append(substrate.composition[i].composition.independent.is_Independent);}
		if(type==Variable_Type::Coupled)		for(int i=0; i<composition_Size; i++) {is_True_Condition.append(substrate.composition[i].composition.coupled.	 is_Coupled	   );}
		if(type==Variable_Type::Fitted)			for(int i=0; i<composition_Size; i++) {is_True_Condition.append(substrate.composition[i].composition.fit.		 is_Fitable	   );}
		if(type==Variable_Type::Optimized)		for(int i=0; i<composition_Size; i++) {is_True_Condition.append(substrate.composition[i].composition.optimize.	 is_Optimizable);}
	} else return;

	// item composition (if composed and >=2 elements)
	if(composed_Material == true && composition_Size>=2)
	{
		for(int i=0; i<composition_Size; i++)
		{
			QListWidgetItem* item_Composition = new QListWidgetItem;
			QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Composition + whats_This_Delimiter + QString::number(i);
			item_Composition->setWhatsThis(whats_This);
			item_Composition->setText(material + " " + brackets + " " + composition_type[i] + " Composition, " + Zeta_Sym + "_" + composition_type[i]);

			if(!is_True_Condition[i])
			if(!variables_List_Map->contains(whats_This))
			{
				nonfiltered_Parameters->addItem(item_Composition);
				optical_Constants->addItem(item_Composition->clone());
			}
		}
	}
}

void Variable_Selection::add_Thickness(QTreeWidgetItem* item, QString whats_This_Type)
{
	QString material, brackets;
	bool is_True_Condition;

	if(whats_This_Type == whats_This_Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		material = layer.material;
		brackets = "(layer " + QString::number(layer.layer_Index) + ")";

		if(type==Variable_Type::Independent)	is_True_Condition = layer.thickness.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = layer.thickness.coupled.	  is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = layer.thickness.fit.		  is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = layer.thickness.optimize.	  is_Optimizable;
	} else return;

	// layer thickness (if parent's stack parameters are not independent)
	bool thickness_Condition = true;
	iterate_Over_Parents(item, thickness_Condition);
	if(thickness_Condition)
	{
		QListWidgetItem* item_Thickness = new QListWidgetItem;
		QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Thickness;
		item_Thickness->setWhatsThis(whats_This);
		item_Thickness->setText(material + " " + brackets + " Thickness, z");

		if(!is_True_Condition)
		if(!variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(item_Thickness);
			thickness_Parameters->addItem(item_Thickness->clone());
		}
	}
}

void Variable_Selection::add_Sigma(QTreeWidgetItem* item, QString whats_This_Type)
{
	QString material, brackets;
	QVector<bool> interlayer_Composition_Enabled;
	bool is_True_Condition;
	int interlayer_Composition_Size;

	if(whats_This_Type == whats_This_Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		material = layer.material;
		brackets = "(layer " + QString::number(layer.layer_Index) + ")";
		interlayer_Composition_Size = layer.interlayer_Composition.size();
		for(int i=0; i<interlayer_Composition_Size; i++)	{interlayer_Composition_Enabled.append(layer.interlayer_Composition[i].enabled);}

		if(type==Variable_Type::Independent)	is_True_Condition = layer.sigma.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = layer.sigma.coupled.	is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = layer.sigma.fit.		is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = layer.sigma.optimize.	is_Optimizable;
	} else
	if(whats_This_Type == whats_This_Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		material = substrate.material;
		brackets = "(substrate)";
		interlayer_Composition_Size = substrate.interlayer_Composition.size();
		for(int i=0; i<interlayer_Composition_Size; i++)	{interlayer_Composition_Enabled.append(substrate.interlayer_Composition[i].enabled);}

		if(type==Variable_Type::Independent)	is_True_Condition = substrate.sigma.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = substrate.sigma.coupled.	is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = substrate.sigma.fit.		is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = substrate.sigma.optimize.	is_Optimizable;
	} else return;

	// item sigma (if >=1 interlayers)
	int interlayers_Counter=0;
	for(int i=0; i<interlayer_Composition_Size; i++)
	{
		if(interlayer_Composition_Enabled[i]) ++interlayers_Counter;
	}
	if(interlayers_Counter>=1)
	{
		QListWidgetItem* item_Sigma = new QListWidgetItem;
		QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Sigma;

		item_Sigma->setWhatsThis(whats_This);
		item_Sigma->setText(material + " " + brackets + " Roughness/Diffuseness, " + Sigma_Sym);

		if(!is_True_Condition)
		if(!variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(item_Sigma);
			interface_Parameters->addItem(item_Sigma->clone());
		}
	}
}

void Variable_Selection::add_Interlayer_Composition(QTreeWidgetItem* item, QString whats_This_Type)
{
	QString material, brackets;
	QVector<bool> interlayer_Composition_Enabled, interlayer_Composition_Independent;
	bool is_True_Condition;
	int interlayer_Composition_Size;

	if(whats_This_Type == whats_This_Layer)
	{
		Layer layer = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
		material = layer.material;
		brackets = "(layer " + QString::number(layer.layer_Index) + ")";
		interlayer_Composition_Size = layer.interlayer_Composition.size();
		for(int i=0; i<interlayer_Composition_Size; i++)	{interlayer_Composition_Enabled.append(layer.interlayer_Composition[i].enabled);}
		for(int i=0; i<interlayer_Composition_Size; i++)	{interlayer_Composition_Independent.append(layer.interlayer_Composition[i].interlayer.independent.is_Independent);}

		if(type==Variable_Type::Independent)	is_True_Condition = layer.sigma.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = layer.sigma.coupled.	is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = layer.sigma.fit.		is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = layer.sigma.optimize.	is_Optimizable;
	} else
	if(whats_This_Type == whats_This_Substrate)
	{
		Substrate substrate = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
		material = substrate.material;
		brackets = "(substrate)";
		interlayer_Composition_Size = substrate.interlayer_Composition.size();
		for(int i=0; i<interlayer_Composition_Size; i++)	{interlayer_Composition_Enabled.append(substrate.interlayer_Composition[i].enabled);}
		for(int i=0; i<interlayer_Composition_Size; i++)	{interlayer_Composition_Independent.append(substrate.interlayer_Composition[i].interlayer.independent.is_Independent);}

		if(type==Variable_Type::Independent)	is_True_Condition = substrate.sigma.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = substrate.sigma.coupled.	is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = substrate.sigma.fit.		is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = substrate.sigma.optimize.	is_Optimizable;
	} else return;

	// item interlayer composition (if enabled and >=2 interlayers)
	int interlayers_Counter=0;
	for(int i=0; i<interlayer_Composition_Size; i++)
	{
		if(interlayer_Composition_Enabled[i]) ++interlayers_Counter;
	}
	if(interlayers_Counter>=2)
	{
		for(int i=0; i<interlayer_Composition_Size; i++)
		{
			QListWidgetItem* item_Composition = new QListWidgetItem;
			QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Interlayer_Composition + whats_This_Delimiter + QString::number(i);
			item_Composition->setWhatsThis(whats_This);
			item_Composition->setText(material + " " + brackets + " Interlayer Composition, " + transition_Layer_Functions[i]);

			if(interlayer_Composition_Enabled[i])
			if(!interlayer_Composition_Independent[i])
			if(!variables_List_Map->contains(whats_This))
			{
				nonfiltered_Parameters->addItem(item_Composition);
				interface_Parameters->addItem(item_Composition->clone());
			}
		}
	}
}

void Variable_Selection::add_Num_repetitions(QTreeWidgetItem* item, QString whats_This_Type)
{
	bool is_True_Condition;
	if(whats_This_Type == whats_This_Multilayer)
	{
		Stack_Content stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
		if(type==Variable_Type::Independent)	is_True_Condition = stack_Content.num_Repetition.is_Independent;
		else return;
	} else return;

	// stack_Content num_Repetition (if parent's stack parameters are not independent)
	bool thickness_Condition = true;
	iterate_Over_Parents(item, thickness_Condition);
	if(thickness_Condition)
	{
		QListWidgetItem* item_Num_Repetition = new QListWidgetItem;
		QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Num_Repetitions;
		item_Num_Repetition->setWhatsThis(whats_This);
		item_Num_Repetition->setText(item->whatsThis(DEFAULT_COLUMN) + " Number of repetitions, N");

		if(!is_True_Condition)
		if(!variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(item_Num_Repetition);
			thickness_Parameters->addItem(item_Num_Repetition->clone());
		}
	}
}

void Variable_Selection::add_Period(QTreeWidgetItem* item, QString whats_This_Type)
{
	bool is_True_Condition;
	if(whats_This_Type == whats_This_Multilayer)
	{
		Stack_Content stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
		if(type==Variable_Type::Independent)	is_True_Condition = stack_Content.period.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = stack_Content.period.coupled.	 is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = stack_Content.period.fit.		 is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = stack_Content.period.optimize.	 is_Optimizable;
	} else return;

	// stack_Content period (if parent stacks are not independent and children don't have independent thicknesses or periods or num_Repetitions)
	bool thickness_Condition = true;
	iterate_Over_Parents (item, thickness_Condition);
	iterate_Over_Children(item, thickness_Condition);
	if(thickness_Condition)
	{
		QListWidgetItem* item_Period = new QListWidgetItem;
		QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Period;
		item_Period->setWhatsThis(whats_This);
		item_Period->setText(item->whatsThis(DEFAULT_COLUMN) + " Period, d");

		if(!is_True_Condition)
		if(!variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(item_Period);
			thickness_Parameters->addItem(item_Period->clone());
		}
	}
}

void Variable_Selection::add_Gamma(QTreeWidgetItem* item, QString whats_This_Type)
{
	bool is_True_Condition;
	if(whats_This_Type == whats_This_Multilayer)
	{
		Stack_Content stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
		if(type==Variable_Type::Independent)	is_True_Condition = stack_Content.gamma.independent.is_Independent;
		if(type==Variable_Type::Coupled)		is_True_Condition = stack_Content.gamma.coupled.	is_Coupled;
		if(type==Variable_Type::Fitted)			is_True_Condition = stack_Content.gamma.fit.		is_Fitable;
		if(type==Variable_Type::Optimized)		is_True_Condition = stack_Content.gamma.optimize.	is_Optimizable;
	} else return;

	// stack_Content gamma (if have 2 children and children don't have independent thicknesses or periods or num_Repetitions)
	bool thickness_Condition = true;
	iterate_Over_Children(item, thickness_Condition);
	if(item->childCount()==2 && thickness_Condition)
	{
		QListWidgetItem* item_Gamma = new QListWidgetItem;
		QString whats_This = item->whatsThis(DEFAULT_COLUMN) + whats_This_Delimiter + whats_This_Gamma;
		item_Gamma->setWhatsThis(whats_This);
		item_Gamma->setText(item->whatsThis(DEFAULT_COLUMN) + " Thickness Ratio, " + Gamma_Sym);

		if(!is_True_Condition)
		if(!variables_List_Map->contains(whats_This))
		{
			nonfiltered_Parameters->addItem(item_Gamma);
			thickness_Parameters->addItem(item_Gamma->clone());
		}
	}
}

// -------------------------------------------

void Variable_Selection::add_Variable()
{
	// PARAMETER
	if(map_Of_Parameters_Lists.value(filters_Combo_Box->currentText())->currentItem())
	{
		QListWidgetItem* new_Item = map_Of_Parameters_Lists.value(filters_Combo_Box->currentText())->currentItem()->clone();
		new_Item->setData(Qt::UserRole, false);	// adding "passive" status

		variables_List_Map->insert(new_Item->whatsThis(), new_Item);
		variables_List->addItem(new_Item);

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
			if(structure_Item->whatsThis(DEFAULT_COLUMN) == whats_This_List[0])
				break;
			++it;
		}

		// if ambient
		if(whats_This_List_Type[0] == whats_This_Ambient)
		{
			Ambient ambient = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

			/// optical constants

			// ambient density
			if(whats_This_List[1] == whats_This_Density)
			{
				ambient.density.independent.is_Independent = true;
				ambient.density.independent.num_Points = 1;
				ambient.density.independent.min = ambient.density.value;
				ambient.density.independent.max = ambient.density.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(ambient.density.value,thumbnail_double_format,thumbnail_density_precision) + " " + density_units + "]");
			}
			// ambient permittivity
			if(whats_This_List[1] == whats_This_Permittivity)
			{
				ambient.permittivity.independent.is_Independent = true;
				ambient.permittivity.independent.num_Points = 1;
				ambient.permittivity.independent.min = ambient.permittivity.value;
				ambient.permittivity.independent.max = ambient.permittivity.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(ambient.permittivity.value,thumbnail_double_format,thumbnail_permittivity_precision) + " " + opt_const_units + "]");
			}
			// ambient absorption
			if(whats_This_List[1] == whats_This_Absorption)
			{
				ambient.absorption.independent.is_Independent = true;
				ambient.absorption.independent.num_Points = 1;
				ambient.absorption.independent.min = ambient.absorption.value;
				ambient.absorption.independent.max = ambient.absorption.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(ambient.absorption.value,thumbnail_double_format,thumbnail_absorption_precision) + " " + opt_const_units + "]");
			}
			//ambient composition
			if(whats_This_List[1] == whats_This_Composition)
			{
				int index = QString(whats_This_List[2]).toInt();
				ambient.composition[index].composition.independent.is_Independent = true;
				ambient.composition[index].composition.independent.num_Points = 1;
				ambient.composition[index].composition.independent.min = ambient.composition[index].composition.value;
				ambient.composition[index].composition.independent.max = ambient.composition[index].composition.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(ambient.composition[index].composition.value,thumbnail_double_format,thumbnail_composition_precision) + "]");
			}

			var.setValue(ambient);
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if layer
		if(whats_This_List_Type[0] == whats_This_Layer)
		{
			Layer layer = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

			/// optical constants

			// layer density
			if(whats_This_List[1] == whats_This_Density)
			{
				layer.density.independent.is_Independent = true;
				layer.density.independent.num_Points = 1;
				layer.density.independent.min = layer.density.value;
				layer.density.independent.max = layer.density.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.density.value,thumbnail_double_format,thumbnail_density_precision) + " " + density_units + "]");
			}
			// layer permittivity
			if(whats_This_List[1] == whats_This_Permittivity)
			{
				layer.permittivity.independent.is_Independent = true;
				layer.permittivity.independent.num_Points = 1;
				layer.permittivity.independent.min = layer.permittivity.value;
				layer.permittivity.independent.max = layer.permittivity.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.permittivity.value,thumbnail_double_format,thumbnail_permittivity_precision) + " " + opt_const_units + "]");
			}
			// layer absorption
			if(whats_This_List[1] == whats_This_Absorption)
			{
				layer.absorption.independent.is_Independent = true;
				layer.absorption.independent.num_Points = 1;
				layer.absorption.independent.min = layer.absorption.value;
				layer.absorption.independent.max = layer.absorption.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.absorption.value,thumbnail_double_format,thumbnail_absorption_precision) + " " + opt_const_units + "]");
			}
			// layer composition
			if(whats_This_List[1] == whats_This_Composition)
			{
				int index = QString(whats_This_List[2]).toInt();
				layer.composition[index].composition.independent.is_Independent = true;
				layer.composition[index].composition.independent.num_Points = 1;
				layer.composition[index].composition.independent.min = layer.composition[index].composition.value;
				layer.composition[index].composition.independent.max = layer.composition[index].composition.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.composition[index].composition.value,thumbnail_double_format,thumbnail_composition_precision) + "]");
			}

			/// thickness parameters

			// layer thickness
			if(whats_This_List[1] == whats_This_Thickness)
			{
				double coeff = length_Coefficients_Map.value(length_units);

				layer.thickness.independent.is_Independent = true;
				layer.thickness.independent.num_Points = 1;
				layer.thickness.independent.min = layer.thickness.value;
				layer.thickness.independent.max = layer.thickness.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.thickness.value/coeff,thumbnail_double_format,thumbnail_thickness_precision) + " " + length_units + "]");
			}

			/// interface parameters

			// layer sigma
			if(whats_This_List[1] == whats_This_Sigma)
			{
				double coeff = length_Coefficients_Map.value(length_units);

				layer.sigma.independent.is_Independent = true;
				layer.sigma.independent.num_Points = 1;
				layer.sigma.independent.min = layer.sigma.value;
				layer.sigma.independent.max = layer.sigma.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.sigma.value/coeff,thumbnail_double_format,thumbnail_sigma_precision) + " " + length_units + "]");
			}
			// layer interlayer composition (if enabled and >=2 elements)
			if(whats_This_List[1] == whats_This_Interlayer_Composition)
			{
				int index = QString(whats_This_List.last()).toInt();
				layer.interlayer_Composition[index].interlayer.independent.is_Independent = true;
				layer.interlayer_Composition[index].interlayer.independent.num_Points = 1;
				layer.interlayer_Composition[index].interlayer.independent.min = layer.interlayer_Composition[index].interlayer.value;
				layer.interlayer_Composition[index].interlayer.independent.max = layer.interlayer_Composition[index].interlayer.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(layer.interlayer_Composition[index].interlayer.value,thumbnail_double_format,thumbnail_interlayer_precision) + "]");
			}

			var.setValue(layer);
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if multilayer
		if(whats_This_List_Type[0] == whats_This_Multilayer)
		{
			Stack_Content stack_Content = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();

			// multilayer num_repetitions
			if(whats_This_List[1] == whats_This_Num_Repetitions)
			{
				stack_Content.num_Repetition.is_Independent = true;
				stack_Content.num_Repetition.num_steps = 1;
				stack_Content.num_Repetition.start = stack_Content.num_Repetition.value;
				stack_Content.num_Repetition.step = 1;

				new_Item->setText(new_Item->text() + " [" + QString::number(stack_Content.num_Repetition.value) + "]");
			}
			// multilayer period
			if(whats_This_List[1] == whats_This_Period)
			{
				double coeff = length_Coefficients_Map.value(length_units);

				stack_Content.period.independent.is_Independent = true;
				stack_Content.period.independent.num_Points = 1;
				stack_Content.period.independent.min = stack_Content.period.value;
				stack_Content.period.independent.max = stack_Content.period.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(stack_Content.period.value/coeff,thumbnail_double_format,thumbnail_period_precision) + " " + length_units + "]");
			}
			// multilayer gamma
			if(whats_This_List[1] == whats_This_Gamma)
			{
				stack_Content.gamma.independent.is_Independent = true;
				stack_Content.gamma.independent.num_Points = 1;
				stack_Content.gamma.independent.min = stack_Content.gamma.value;
				stack_Content.gamma.independent.max = stack_Content.gamma.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(stack_Content.gamma.value,thumbnail_double_format,thumbnail_gamma_precision) + "]");
			}

			var.setValue(stack_Content);
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		}

		// if substrate
		if(whats_This_List_Type[0] == whats_This_Substrate)
		{
			Substrate substrate = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

			/// optical constants

			// substrate density
			if(whats_This_List[1] == whats_This_Density)
			{
				substrate.density.independent.is_Independent = true;
				substrate.density.independent.num_Points = 1;
				substrate.density.independent.min = substrate.density.value;
				substrate.density.independent.max = substrate.density.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.density.value,thumbnail_double_format,thumbnail_density_precision) + " " + density_units + "]");
			}
			// substrate permittivity
			if(whats_This_List[1] == whats_This_Permittivity)
			{
				substrate.permittivity.independent.is_Independent = true;
				substrate.permittivity.independent.num_Points = 1;
				substrate.permittivity.independent.min = substrate.permittivity.value;
				substrate.permittivity.independent.max = substrate.permittivity.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.permittivity.value,thumbnail_double_format,thumbnail_permittivity_precision) + " " + opt_const_units + "]");
			}
			// substrate absorption
			if(whats_This_List[1] == whats_This_Absorption)
			{
				substrate.absorption.independent.is_Independent = true;
				substrate.absorption.independent.num_Points = 1;
				substrate.absorption.independent.min = substrate.absorption.value;
				substrate.absorption.independent.max = substrate.absorption.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.absorption.value,thumbnail_double_format,thumbnail_absorption_precision) + " " + opt_const_units + "]");
			}
			// substrate composition
			if(whats_This_List[1] == whats_This_Composition)
			{
				int index = QString(whats_This_List[2]).toInt();
				substrate.composition[index].composition.independent.is_Independent = true;
				substrate.composition[index].composition.independent.num_Points = 1;
				substrate.composition[index].composition.independent.min = substrate.composition[index].composition.value;
				substrate.composition[index].composition.independent.max = substrate.composition[index].composition.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.composition[index].composition.value,thumbnail_double_format,thumbnail_composition_precision) + "]");
			}

			/// interface parameters

			// substrate sigma
			if(whats_This_List[1] == whats_This_Sigma)
			{
				double coeff = length_Coefficients_Map.value(length_units);

				substrate.sigma.independent.is_Independent = true;
				substrate.sigma.independent.num_Points = 1;
				substrate.sigma.independent.min = substrate.sigma.value;
				substrate.sigma.independent.max = substrate.sigma.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.sigma.value/coeff,thumbnail_double_format,thumbnail_sigma_precision) + " " + length_units + "]");
			}
			// substrate interlayer composition (if enabled and >=2 elements)
			if(whats_This_List[1] == whats_This_Interlayer_Composition)
			{
				int index = QString(whats_This_List.last()).toInt();
				substrate.interlayer_Composition[index].interlayer.independent.is_Independent = true;
				substrate.interlayer_Composition[index].interlayer.independent.num_Points = 1;
				substrate.interlayer_Composition[index].interlayer.independent.min = substrate.interlayer_Composition[index].interlayer.value;
				substrate.interlayer_Composition[index].interlayer.independent.max = substrate.interlayer_Composition[index].interlayer.value;

				new_Item->setText(new_Item->text() + " [" + QString::number(substrate.interlayer_Composition[index].interlayer.value,thumbnail_double_format,thumbnail_interlayer_precision) + "]");
			}

			var.setValue(substrate);
			structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
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
			layer = baby->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
			thickness_Condition = thickness_Condition && !layer.thickness.independent.is_Independent;
		}

		// if multilayer
		if(baby->childCount()>0)
		{
			stack_Content = baby->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
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
			stack_Content = parent->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
			//											  if                 false						  and		                false
			thickness_Condition = thickness_Condition && (!stack_Content.gamma.independent.is_Independent && !stack_Content.period.independent.is_Independent);
			parent = parent->parent();
		}
	}
}
