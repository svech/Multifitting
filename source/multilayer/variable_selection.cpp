#include "variable_selection.h"

Variable_Selection::Variable_Selection(QTreeWidget* struct_Tree, QMap<QString, QListWidgetItem*>* independent_Variables_List_Map, QListWidget* independent_Variables_List, Variable_Type type):
	struct_Tree(struct_Tree),
	independent_Variables_List(independent_Variables_List),
	independent_Variables_List_Map(independent_Variables_List_Map)
{
	create_Window();
	if(type==Variable_Type::Independent)
	{
		setWindowTitle("Choose independent variable");
		fill_None_Filtered_Variables_List();
	}
	if(type==Variable_Type::Coupled)
	{
		setWindowTitle("Choose coupled parameter");
		// TODO
	}
	if(type==Variable_Type::Fitted)
	{
		setWindowTitle("Choose fit parameter");
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

void Variable_Selection::fill_None_Filtered_Variables_List()
{
	nonfiltered_Parameters = new QListWidget;

	QTreeWidgetItemIterator it(struct_Tree);
	while (*it)
	{
		QTreeWidgetItem* item = *it;
		if(item->whatsThis(default_Column)   ==whats_This_Ambient)			fill_None_Filtered_Ambient_Variables(item);
		if(item->whatsThis(default_Column)[0]==whats_This_Layer[0])			fill_None_Filtered_Layer_Variables(item);
		if(item->whatsThis(default_Column)[0]==whats_This_Multilayer[0])	fill_None_Filtered_Multilayer_Variables(item);
		if(item->whatsThis(default_Column)   ==whats_This_Substrate)		fill_None_Filtered_Substrate_Variables(item);
		++it;
	}

	filters_Combo_Box->addItem("None");
	filtered_Parameters_Pages->addWidget(nonfiltered_Parameters);
	map_Of_Parameters_Lists.insert(filters_Combo_Box->itemText(filters_Combo_Box->count()-1), nonfiltered_Parameters);

	connect(nonfiltered_Parameters, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(add_Variable()));
}

void Variable_Selection::fill_None_Filtered_Ambient_Variables(QTreeWidgetItem* item)
{
	Ambient ambient = item->data(default_Column, Qt::UserRole).value<Ambient>();
	QString whats_This;

//	QMap<QString, QListWidgetItem*>::iterator it;
//	qInfo() << "\nmap content";
//	for (it = independent_Variables_List_Map->begin(); it != independent_Variables_List_Map->end(); ++it)
//		qInfo() << QString("%1 :").arg(it.key());
//	qInfo() << "";

	// ambient density (if not Vacuum and not arbitrary optical constants)
	if(ambient.material!="Vacuum" && ambient.separate_Optical_Constants != Tril::True)
	{
		QListWidgetItem* ambient_Density = new QListWidgetItem;		
		whats_This = item->whatsThis(default_Column)+"_Density";

		ambient_Density->setWhatsThis(whats_This);
		ambient_Density->setText(ambient.material+" (ambient) Density, " + Rho_Sym);

		if(!independent_Variables_List_Map->contains(whats_This))
			nonfiltered_Parameters->addItem(ambient_Density);
	}
	// ambient permittivity and absorption (if not specified and not composed material)
	if(ambient.separate_Optical_Constants != Tril::False && ambient.composed_Material == false)
	{
		QListWidgetItem* ambient_Permittivity = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column)+"_Permittivity";

		ambient_Permittivity->setWhatsThis(whats_This);
		ambient_Permittivity->setText(ambient.material+" (ambient) Permittivity, 1-" + Epsilon_Sym);
		if(!independent_Variables_List_Map->contains(whats_This))
			nonfiltered_Parameters->addItem(ambient_Permittivity);

		QListWidgetItem* ambient_Absorption = new QListWidgetItem;
		whats_This = item->whatsThis(default_Column)+"_Absorption";
		ambient_Absorption->setWhatsThis(whats_This);
		ambient_Absorption->setText(ambient.material+" (ambient) Absorption, " + Cappa_Sym);

		if(!independent_Variables_List_Map->contains(whats_This))
			nonfiltered_Parameters->addItem(ambient_Absorption);
	}
	// ambient composition (if composed and >=2 elements)
	if(ambient.composed_Material == true && ambient.composition.size()>=2)
	{
		for(int i=0; i<ambient.composition.size(); i++)
		{
			QListWidgetItem* ambient_Composition = new QListWidgetItem;
			whats_This = item->whatsThis(default_Column)+"_Composition_"+QString::number(i);
			ambient_Composition->setWhatsThis(whats_This);
			ambient_Composition->setText(ambient.material+" (ambient) " + ambient.composition[i].type + " Composition, " + Zeta_Sym + "_" + ambient.composition[i].type);
			if(!independent_Variables_List_Map->contains(whats_This))
				nonfiltered_Parameters->addItem(ambient_Composition);
		}
	}
}

void Variable_Selection::fill_None_Filtered_Layer_Variables(QTreeWidgetItem* item)
{
	// TODO
}

void Variable_Selection::fill_None_Filtered_Multilayer_Variables(QTreeWidgetItem* item)
{
	// TODO
}

void Variable_Selection::fill_None_Filtered_Substrate_Variables(QTreeWidgetItem* item)
{
	// TODO
}

void Variable_Selection::add_Variable()
{
	if(map_Of_Parameters_Lists.value(filters_Combo_Box->currentText())->currentItem())
	{
		QListWidgetItem* new_Item = map_Of_Parameters_Lists.value(filters_Combo_Box->currentText())->currentItem()->clone();

		independent_Variables_List_Map->insert(new_Item->whatsThis(), new_Item);
		independent_Variables_List->addItem(new_Item);
	}
	close();
}
