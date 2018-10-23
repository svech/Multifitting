// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "variable_selection.h"
#include "multilayer_approach/multilayer/independent_variables/independent_variables_editor.h"

Variable_Selection::Variable_Selection(QTreeWidget* struct_Tree_Copy, QMap<id_Type, QListWidgetItem*>* variables_List_Map, QListWidget* variables_List, QWidget *parent) :
	struct_Tree_Copy(struct_Tree_Copy),
	variables_List(variables_List),
	variables_List_Map(variables_List_Map),
	QDialog(parent)
{
	create_Window();
	setWindowTitle("Choose independent variable");
	fill_Variables_List();
	set_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Variable_Selection::closeEvent(QCloseEvent* event)
{
	emit closed();
	event->accept();
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
		hor_Frame->adjustSize();
	QHBoxLayout* hor_Layout = new QHBoxLayout(hor_Frame);
		hor_Layout->setContentsMargins(0,0,0,0);
		hor_Layout->setSpacing(3);
		hor_Layout->addWidget(filters_Label);
		hor_Layout->addWidget(filters_Combo_Box);
		hor_Layout->setAlignment(Qt::AlignCenter);

	QFrame* but_Frame = new QFrame;
		but_Frame->adjustSize();
	QHBoxLayout* but_Layout = new QHBoxLayout(but_Frame);
		but_Layout->setContentsMargins(0,0,0,0);
		but_Layout->setSpacing(6);
		but_Layout->setAlignment(Qt::AlignCenter);
		but_Layout->addWidget(cancel_Button);
		but_Layout->addWidget(ok_Button);

	QVBoxLayout* ver_layout = new QVBoxLayout(this);
		ver_layout->setSpacing(4);
		ver_layout->setContentsMargins(4,4,4,4);
		ver_layout->setAlignment(Qt::AlignCenter);
		ver_layout->addWidget(filtered_Parameters_Pages);
		ver_layout->addWidget(hor_Frame);
		ver_layout->addWidget(but_Frame);

	connect(cancel_Button, &QPushButton::clicked, this, &Variable_Selection::close);
	connect(ok_Button    , &QPushButton::clicked, this, &Variable_Selection::add_Variable);
}

void Variable_Selection::set_Geometry()
{
//	adjustSize();
	resize(nonfiltered_Parameters->sizeHintForColumn(0) + 40,400);
//	setFixedSize(size());
}

void Variable_Selection::fill_Variables_List()
{
	nonfiltered_Parameters = new QListWidget;
	optical_Constants_List = new QListWidget;
	interface_Parameters = new QListWidget;
	thickness_Parameters = new QListWidget;

	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		QTreeWidgetItem* struct_Item = *it;

		refresh_Independent_State_Of_Struct_Tree_Copy_Item(struct_Item);

		Data struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

		// PARAMETER

		/// optical constants
		add_Parameter				(struct_Item, whats_This_Absolute_Density);
		add_Parameter				(struct_Item, whats_This_Relative_Density);
		add_Parameter				(struct_Item, whats_This_Permittivity);
		add_Parameter				(struct_Item, whats_This_Absorption);
		add_Composition				(struct_Item);
		/// thickness parameters
		add_Parameter				(struct_Item, whats_This_Thickness);
		add_Parameter				(struct_Item, whats_This_Thickness_Drift_Line_Value);
		add_Parameter				(struct_Item, whats_This_Thickness_Drift_Rand_Rms);
		add_Parameter				(struct_Item, whats_This_Thickness_Drift_Sine_Amplitude);
		add_Parameter				(struct_Item, whats_This_Thickness_Drift_Sine_Frequency);
		add_Parameter				(struct_Item, whats_This_Thickness_Drift_Sine_Phase);
		/// interface parameters
		add_Parameter				(struct_Item, whats_This_Sigma);
		add_Interlayer_Composition	(struct_Item);
		add_Interlayer_My_Sigma		(struct_Item);
		add_Parameter				(struct_Item, whats_This_Sigma_Drift_Line_Value);
		add_Parameter				(struct_Item, whats_This_Sigma_Drift_Rand_Rms);
		add_Parameter				(struct_Item, whats_This_Sigma_Drift_Sine_Amplitude);
		add_Parameter				(struct_Item, whats_This_Sigma_Drift_Sine_Frequency);
		add_Parameter				(struct_Item, whats_This_Sigma_Drift_Sine_Phase);
		/// stack parameters
		add_Num_repetitions			(struct_Item);
		add_Parameter				(struct_Item, whats_This_Period);
		add_Parameter				(struct_Item, whats_This_Gamma);

		++it;
	}

	// add nonfiltered
	filters_Combo_Box->addItem("None");
	filtered_Parameters_Pages->addWidget(nonfiltered_Parameters);
	map_Of_Parameters_Lists.insert(filters_Combo_Box->itemText(filters_Combo_Box->count()-1), nonfiltered_Parameters);

	// add optical constants
	filters_Combo_Box->addItem("Optical Constants");
	filtered_Parameters_Pages->addWidget(optical_Constants_List);
	map_Of_Parameters_Lists.insert(filters_Combo_Box->itemText(filters_Combo_Box->count()-1), optical_Constants_List);

	// add interface parameters
	filters_Combo_Box->addItem("Interface Parameters");
	filtered_Parameters_Pages->addWidget(interface_Parameters);
	map_Of_Parameters_Lists.insert(filters_Combo_Box->itemText(filters_Combo_Box->count()-1), interface_Parameters);

	// add thickness parameters
	filters_Combo_Box->addItem("Thickness Parameters");
	filtered_Parameters_Pages->addWidget(thickness_Parameters);
	map_Of_Parameters_Lists.insert(filters_Combo_Box->itemText(filters_Combo_Box->count()-1), thickness_Parameters);

	connect(nonfiltered_Parameters, &QListWidget::itemDoubleClicked, this, [=]{add_Variable();});
	connect(optical_Constants_List,	&QListWidget::itemDoubleClicked, this, [=]{add_Variable();});
	connect(interface_Parameters,	&QListWidget::itemDoubleClicked, this, [=]{add_Variable();});
	connect(thickness_Parameters,	&QListWidget::itemDoubleClicked, this, [=]{add_Variable();});

	connect(filters_Combo_Box, SIGNAL(currentIndexChanged(int)), this, SLOT(filter_Change(int))); // don't change! it will be broken!
}

void Variable_Selection::add_Parameter(QTreeWidgetItem* struct_Item, QString whats_This)
{
	// PARAMETER

	// no vectors here (interlayer composition, my_Sigma, composition etc) and no Num_Repetitons here
	Data struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	QListWidget* filtered_Parameters = nullptr;
	Parameter parameter;
	bool additional_Condition = false;

	/// optical constants
	if(	struct_Data.item_Type == item_Type_Ambient	||
		struct_Data.item_Type == item_Type_Layer	||
		struct_Data.item_Type == item_Type_Substrate )
	{
		if(whats_This == whats_This_Absolute_Density)
		{
			parameter = struct_Data.absolute_Density;
			filtered_Parameters = optical_Constants_List;

			// if not Vacuum and not arbitrary optical constants and if composed_Material
			additional_Condition =	struct_Data.material!="Vacuum" &&
									struct_Data.separate_Optical_Constants != TRIL_TRUE &&
									struct_Data.composed_Material;
		}
		if(whats_This == whats_This_Relative_Density)
		{
			parameter = struct_Data.relative_Density;
			filtered_Parameters = optical_Constants_List;

			// if not Vacuum and not arbitrary optical constants and if NOT composed_Material
			additional_Condition =	struct_Data.material!="Vacuum" &&
									struct_Data.separate_Optical_Constants != TRIL_TRUE &&
									!struct_Data.composed_Material;
		}
		if(whats_This == whats_This_Permittivity)
		{
			parameter = struct_Data.permittivity;
			filtered_Parameters = optical_Constants_List;

			// if not Vacuum and not specified material
			additional_Condition =	struct_Data.material!="Vacuum" &&
									struct_Data.separate_Optical_Constants != TRIL_FALSE;
		}
		if(whats_This == whats_This_Absorption)
		{
			parameter = struct_Data.absorption;
			filtered_Parameters = optical_Constants_List;

			// if not Vacuum and not specified material
			additional_Condition =	struct_Data.material!="Vacuum" &&
									struct_Data.separate_Optical_Constants != TRIL_FALSE;
		}
	}

	/// thickness parameters
	if(	struct_Data.item_Type == item_Type_Layer )
	{
		if(whats_This == whats_This_Thickness)
		{
			parameter = struct_Data.thickness;
			filtered_Parameters = thickness_Parameters;

			// if parent's stack parameters are not independent
			additional_Condition = true;
			iterate_Over_Parents(struct_Item, additional_Condition);
		}
		if(whats_This == whats_This_Thickness_Drift_Line_Value)
		{
			parameter = struct_Data.thickness_Drift.drift_Line_Value;
			filtered_Parameters = thickness_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
		if(whats_This == whats_This_Thickness_Drift_Rand_Rms)
		{
			parameter = struct_Data.thickness_Drift.drift_Rand_Rms;
			filtered_Parameters = thickness_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Amplitude)
		{
			parameter = struct_Data.thickness_Drift.drift_Sine_Amplitude;
			filtered_Parameters = thickness_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Frequency)
		{
			parameter = struct_Data.thickness_Drift.drift_Sine_Frequency;
			filtered_Parameters = thickness_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
		if(whats_This == whats_This_Thickness_Drift_Sine_Phase)
		{
			parameter = struct_Data.thickness_Drift.drift_Sine_Phase;
			filtered_Parameters = thickness_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
	}

	/// interface parameters
	if(	struct_Data.item_Type == item_Type_Layer ||
		struct_Data.item_Type == item_Type_Substrate  )
	{
		if(whats_This == whats_This_Sigma)
		{
			parameter = struct_Data.sigma;
			filtered_Parameters = interface_Parameters;

			// if we have some interlayers and sigma is common
			for(const Interlayer& inter : struct_Data.interlayer_Composition)
			{
				if(inter.enabled) additional_Condition = true;
			}
			additional_Condition = additional_Condition && struct_Data.common_Sigma;
		}
		if(whats_This == whats_This_Sigma_Drift_Line_Value)
		{
			parameter = struct_Data.sigma_Drift.drift_Line_Value;
			filtered_Parameters = interface_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
		if(whats_This == whats_This_Sigma_Drift_Rand_Rms)
		{
			parameter = struct_Data.sigma_Drift.drift_Rand_Rms;
			filtered_Parameters = interface_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Amplitude)
		{
			parameter = struct_Data.sigma_Drift.drift_Sine_Amplitude;
			filtered_Parameters = interface_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Frequency)
		{
			parameter = struct_Data.sigma_Drift.drift_Sine_Frequency;
			filtered_Parameters = interface_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
		if(whats_This == whats_This_Sigma_Drift_Sine_Phase)
		{
			parameter = struct_Data.sigma_Drift.drift_Sine_Phase;
			filtered_Parameters = interface_Parameters;

			// if has parent
			additional_Condition = bool(struct_Item->parent());
		}
	}

	/// stack parameters
	if(	struct_Data.item_Type == item_Type_Multilayer )
	{
		if(whats_This == whats_This_Period)
		{
			parameter = struct_Data.period;
			filtered_Parameters = thickness_Parameters;

			// if parent stacks are not independent and children don't have independent thicknesses or periods or num_Repetitions
			additional_Condition = true;
			iterate_Over_Parents (struct_Item, additional_Condition);
			iterate_Over_Children(struct_Item, additional_Condition);
		}
		if(whats_This == whats_This_Gamma)
		{
			parameter = struct_Data.gamma;
			filtered_Parameters = thickness_Parameters;

			// if have 2 children and children don't have independent thicknesses or periods or num_Repetitions
			additional_Condition = true;
			iterate_Over_Children(struct_Item, additional_Condition);
			additional_Condition = additional_Condition && (struct_Item->childCount()==2);
		}
	}

	if(filtered_Parameters)
	{
		QListWidgetItem* list_Item = new QListWidgetItem;

		Independent_Indicator item_Indicator;
		item_Indicator.item_Id			   = struct_Data.id;
		item_Indicator.item_Type		   = struct_Data.item_Type;
		item_Indicator.id				   = parameter.indicator.id;
		item_Indicator.parameter_Whats_This= parameter.indicator.whats_This;
		item_Indicator.is_Active		   = false;

		QVariant var; var.setValue(item_Indicator);
		list_Item->setData(Qt::UserRole,var);
		list_Item->setText(Global_Variables::parameter_Name(struct_Data, parameter.indicator.whats_This));

		if(!parameter.independent.is_Independent && additional_Condition)
		if(!variables_List_Map->contains(parameter.indicator.id))
		{
			nonfiltered_Parameters->addItem(list_Item);
			filtered_Parameters->addItem(list_Item->clone());
		}
	}
}

//// adding special parameters

void Variable_Selection::add_Composition(QTreeWidgetItem* struct_Item)
{
	// PARAMETER

	Data struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(	struct_Data.item_Type != item_Type_Ambient &&
		struct_Data.item_Type != item_Type_Layer &&
		struct_Data.item_Type != item_Type_Substrate)
	{
		return;
	}

	// item composition (if composed and >=2 elements)
	if(struct_Data.composed_Material == true && struct_Data.composition.size()>=2)
	{
		int index = 0;
		for(const Stoichiometry& stoich : struct_Data.composition)
		{
			Parameter parameter = stoich.composition;
			bool is_Independent = parameter.independent.is_Independent;

			QListWidgetItem* item_Composition = new QListWidgetItem;

			Independent_Indicator composition_Indicator;
			composition_Indicator.item_Id			   = struct_Data.id;
			composition_Indicator.item_Type		       = struct_Data.item_Type;
			composition_Indicator.id				   = parameter.indicator.id;
			composition_Indicator.parameter_Whats_This = parameter.indicator.whats_This;
			composition_Indicator.is_Active			   = false;
			composition_Indicator.index				   = index;

			QVariant var; var.setValue(composition_Indicator);
			item_Composition->setData(Qt::UserRole,var);
			item_Composition->setText(Global_Variables::parameter_Name(struct_Data, parameter.indicator.whats_This, index));

			if(!is_Independent)
			if(!variables_List_Map->contains(parameter.indicator.id))
			{
				nonfiltered_Parameters->addItem(item_Composition);
				optical_Constants_List->addItem(item_Composition->clone());
			}
			index++;
		}
	}
}

void Variable_Selection::add_Interlayer_Composition(QTreeWidgetItem* struct_Item)
{
	// PARAMETER

	Data struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(	struct_Data.item_Type != item_Type_Layer &&
		struct_Data.item_Type != item_Type_Substrate )
	{
		return;
	}

	int interlayers_Counter=0;
	for(const Interlayer& inter : struct_Data.interlayer_Composition)
	{
		if(inter.enabled) interlayers_Counter++;
	}

	int transition_Layer_Index=0;

	// if >=2 interlayers
	if(interlayers_Counter>=2)
	for(const Interlayer& inter : struct_Data.interlayer_Composition)
	{
		Parameter parameter = inter.interlayer;
		bool is_Independent = parameter.independent.is_Independent;
		bool interlayer_Enabled = inter.enabled;

		// if enabled
		QListWidgetItem* item_Composition = new QListWidgetItem;

		Independent_Indicator composition_Indicator;
		composition_Indicator.item_Id			  = struct_Data.id;
		composition_Indicator.item_Type		      = struct_Data.item_Type;
		composition_Indicator.id		  = parameter.indicator.id;
		composition_Indicator.parameter_Whats_This= parameter.indicator.whats_This;
		composition_Indicator.is_Active			  = false;
		composition_Indicator.index				  = transition_Layer_Index;

		QVariant var; var.setValue(composition_Indicator);
		item_Composition->setData(Qt::UserRole,var);
		item_Composition->setText(Global_Variables::parameter_Name(struct_Data, parameter.indicator.whats_This, transition_Layer_Index));

		if(!is_Independent && interlayer_Enabled)
		if(!variables_List_Map->contains(parameter.indicator.id))
		{
			nonfiltered_Parameters->addItem(item_Composition);
			interface_Parameters->addItem(item_Composition->clone());
		}
		transition_Layer_Index++;
	}
}

void Variable_Selection::add_Interlayer_My_Sigma(QTreeWidgetItem* struct_Item)
{
	// PARAMETER

	Data struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(	struct_Data.item_Type != item_Type_Layer &&
		struct_Data.item_Type != item_Type_Substrate )
	{
		return;
	}

	int interlayers_Counter=0;
	for(const Interlayer& inter : struct_Data.interlayer_Composition)
	{
		if(inter.enabled) interlayers_Counter++;
	}

	int transition_Layer_Index=0;

	// if >=2 interlayers and sigma is NOT common
	if(interlayers_Counter>=2 && !struct_Data.common_Sigma)
	for(const Interlayer& inter : struct_Data.interlayer_Composition)
	{
		Parameter parameter = inter.my_Sigma;
		bool is_Independent = parameter.independent.is_Independent;
		bool interlayer_Enabled = inter.enabled;

		// if enabled
		QListWidgetItem* item_Composition = new QListWidgetItem;

		Independent_Indicator composition_Indicator;
		composition_Indicator.item_Id			   = struct_Data.id;
		composition_Indicator.item_Type		       = struct_Data.item_Type;
		composition_Indicator.id				   = parameter.indicator.id;
		composition_Indicator.parameter_Whats_This = parameter.indicator.whats_This;
		composition_Indicator.is_Active			   = false;
		composition_Indicator.index				   = transition_Layer_Index;

		QVariant var; var.setValue(composition_Indicator);
		item_Composition->setData(Qt::UserRole,var);
		item_Composition->setText(Global_Variables::parameter_Name(struct_Data, parameter.indicator.whats_This, transition_Layer_Index));

		if(!is_Independent && interlayer_Enabled)
		if(!variables_List_Map->contains(parameter.indicator.id))
		{
			nonfiltered_Parameters->addItem(item_Composition);
			interface_Parameters->addItem(item_Composition->clone());
		}
		transition_Layer_Index++;
	}
}

void Variable_Selection::add_Num_repetitions(QTreeWidgetItem* struct_Item)
{
	// PARAMETER

	Data struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
	if(	struct_Data.item_Type != item_Type_Multilayer )
	{
		return;
	}

	Int_Independent int_Ind = struct_Data.num_Repetition;
	bool is_Independent = int_Ind.is_Independent;

	// if parent's stack parameters are not independent
	bool parent_Condition = true;
	iterate_Over_Parents(struct_Item, parent_Condition);
	if(parent_Condition)
	{
		QListWidgetItem* item_Num_Repetition = new QListWidgetItem;

		Independent_Indicator num_Repetition_Indicator;
		num_Repetition_Indicator.item_Type			 = item_Type_Multilayer;
		num_Repetition_Indicator.item_Id			 = struct_Data.id;
		num_Repetition_Indicator.id					 = int_Ind.parameter.indicator.id;
		num_Repetition_Indicator.parameter_Whats_This= int_Ind.parameter.indicator.whats_This;
		num_Repetition_Indicator.is_Active			 = false;

		QVariant var; var.setValue(num_Repetition_Indicator);
		item_Num_Repetition->setData(Qt::UserRole,var);
		item_Num_Repetition->setText(Global_Variables::parameter_Name(struct_Data, int_Ind.parameter.indicator.whats_This));

		if(!is_Independent)
		if(!variables_List_Map->contains(int_Ind.parameter.indicator.id))
		{
			nonfiltered_Parameters->addItem(item_Num_Repetition);
			thickness_Parameters->addItem(item_Num_Repetition->clone());
		}
	}
}

void Variable_Selection::refresh_Independent_State_Of_Struct_Tree_Copy_Item(QTreeWidgetItem* struct_Item)
{
	// PARAMETER

	Data struct_Data = struct_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	if(	struct_Data.item_Type == item_Type_Ambient ||
		struct_Data.item_Type == item_Type_Layer ||
		struct_Data.item_Type == item_Type_Substrate)
	{
		if( struct_Data.absolute_Density.independent.is_Independent == true ) {struct_Data.separate_Optical_Constants = TRIL_FALSE;} else
		if( struct_Data.relative_Density.independent.is_Independent == true ) {struct_Data.separate_Optical_Constants = TRIL_FALSE;} else
		if( struct_Data.permittivity.	 independent.is_Independent == true ) {struct_Data.separate_Optical_Constants = TRIL_TRUE;}  else
		if( struct_Data.absorption.		 independent.is_Independent == true ) {struct_Data.separate_Optical_Constants = TRIL_TRUE;}  else
																			  {struct_Data.separate_Optical_Constants = TRIL_NOT_DEFINED;}

		QVariant var;
		var.setValue(struct_Data);
		struct_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	}
}

//// -------------------------------------------

void Variable_Selection::add_Variable_Item(QListWidgetItem* new_Item)
{
	// PARAMETER

	Independent_Indicator new_Data = new_Item->data(Qt::UserRole).value<Independent_Indicator>();

	variables_List_Map->insert(new_Data.id, new_Item);
	variables_List->addItem(new_Item);

	QVariant var;

	// item search
	QTreeWidgetItem* structure_Item = nullptr;
	QTreeWidgetItemIterator it(struct_Tree_Copy);
	while (*it)
	{
		structure_Item = *it;
		if(structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>().id == new_Data.item_Id)
			break;
		++it;
	}

	Data struct_Data = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();

	// change
	if(new_Data.parameter_Whats_This == whats_This_Num_Repetitions)
	{
		struct_Data.num_Repetition.is_Independent = true;
		struct_Data.num_Repetition.num_Steps = 1;
		struct_Data.num_Repetition.start = struct_Data.num_Repetition.value;
		struct_Data.num_Repetition.step = 1;
	} else
	{
		Parameter* parameter = Global_Variables::get_Parameter_From_Struct_Item_by_Whats_This(struct_Data, new_Data.parameter_Whats_This);
		(*parameter).independent.is_Independent = true;
		(*parameter).independent.num_Points = 1;
		(*parameter).independent.min = (*parameter).value;
		(*parameter).independent.max = (*parameter).value;
	}

	var.setValue(struct_Data);
	structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);

	QString temp_Argument_Type;
	Independent_Variables_Editor* editor = new Independent_Variables_Editor(structure_Item, new_Item, variables_List, temp_Argument_Type);
		editor->close();
}

void Variable_Selection::add_Variable()
{
	if(map_Of_Parameters_Lists.value(filters_Combo_Box->currentText())->currentItem())
	{
		QListWidgetItem* new_Item = map_Of_Parameters_Lists.value(filters_Combo_Box->currentText())->currentItem()->clone();
		add_Variable_Item(new_Item);
	}
	close();
}

void Variable_Selection::filter_Change(int index)
{
	filtered_Parameters_Pages->setCurrentIndex(index);
}

void Variable_Selection::iterate_Over_Children(QTreeWidgetItem* this_Stack, bool& thickness_Condition)
{
	QTreeWidgetItem* baby;

	for(int i=0; i<this_Stack->childCount(); ++i)
	{
		baby = this_Stack->child(i);

		// if layer
		if(baby->childCount()==0)
		{
			Data layer = baby->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			thickness_Condition = thickness_Condition && !layer.thickness.independent.is_Independent;
		}

		// if multilayer
		if(baby->childCount()>0)
		{
			Data stack_Content = baby->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			thickness_Condition = thickness_Condition && (!stack_Content.period.independent.is_Independent && !stack_Content.num_Repetition.is_Independent);
			iterate_Over_Children(baby, thickness_Condition);
		}
	}
}

void Variable_Selection::iterate_Over_Parents(QTreeWidgetItem* this_Item, bool& parent_Condition)
{
	QTreeWidgetItem* parent = this_Item->parent();
	if(parent)
	{
		while(parent)
		{
			Data stack_Content = parent->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
			//											  if                 false						  and		                false
			parent_Condition = parent_Condition && (!stack_Content.gamma.independent.is_Independent && !stack_Content.period.independent.is_Independent);
			parent = parent->parent();
		}
	}
}
