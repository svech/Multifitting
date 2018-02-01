#include "fits_selector.h"

Fits_Selector::Fits_Selector(Multilayer_Approach* multilayer_Approach, QWidget* parent) :
	multilayer_Approach(multilayer_Approach),
	fitted_Structures(multilayer_Approach->fitted_Structures),
	runned_Fits_Selectors(multilayer_Approach->runned_Fits_Selectors),
	QWidget(parent)
{
	setWindowTitle("Fits Selector");
	create_Main_Layout();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Fits_Selector::closeEvent(QCloseEvent* event)
{
	runned_Fits_Selectors.remove(fits_Selector_Key);
	event->accept();
}

void Fits_Selector::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	create_List();
	load_Data();
	{
		QHBoxLayout* button_Layout = new QHBoxLayout;
			button_Layout->setAlignment(Qt::AlignCenter);

		done_Button = new QPushButton("Done");
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
			done_Button->setDefault(true);
			done_Button->setShortcut(Qt::Key_D | Qt::CTRL);
		button_Layout->addWidget(done_Button);
		main_Layout->addLayout(button_Layout);

		connect(done_Button, &QPushButton::clicked, this, &Fits_Selector::close);
	}
}

void Fits_Selector::create_List()
{
	fits_List = new QListWidget(this);
	main_Layout->addWidget(fits_List);
}

void Fits_Selector::load_Data()
{
	fits_List->clear();
	items_To_Structures_Map.clear();
	for(Fitted_Structure& fitted_Structure : fitted_Structures)
	{
		add_Item(fitted_Structure);
	}
}

void Fits_Selector::add_Item(Fitted_Structure& fitted_Structure)
{
	QListWidgetItem* new_List_Item = new QListWidgetItem(fitted_Structure.name);
	fits_List->addItem(new_List_Item);

	items_To_Structures_Map.insert(new_List_Item, fitted_Structure);
}
