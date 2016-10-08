#include "independent_variables.h"

Independent_Variables::Independent_Variables(QTreeWidget* struct_Tree):
	gui_Settings(Gui_Settings_Path, QSettings::IniFormat),
	default_Values(Default_Values_Path, QSettings::IniFormat),
	struct_Tree(struct_Tree)
{
	create_Main_Layout();
	independent_Variables_List_Map = new QMap<QString, QListWidgetItem*>;
}

void Independent_Variables::create_Main_Layout()
{
	QVBoxLayout* main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);

	independent_Frame_Layout = new QVBoxLayout;
		independent_Frame_Layout->setSpacing(0);

	independent_Frame = new QFrame;
		independent_Frame->setContentsMargins(-7,-3,-7,-10);
		independent_Frame->setLayout(independent_Frame_Layout);
		main_Layout->addWidget(independent_Frame);

	create_Independent_Variables_List();
		independent_Frame_Layout->addWidget(independent_Variables_List);

	create_Independent_Variables_Toolbar();
		independent_Frame_Layout->addWidget(independent_Variables_Toolbar);

}

void Independent_Variables::create_Independent_Variables_List()
{
	independent_Variables_List = new QListWidget;
		independent_Variables_List->setSizeAdjustPolicy(QListWidget::AdjustToContents);

	QSizePolicy sp_Retain = independent_Variables_List->sizePolicy();
	sp_Retain.setRetainSizeWhenHidden(true);
	independent_Variables_List->setSizePolicy(sp_Retain);

	connect(independent_Variables_List, SIGNAL(itemSelectionChanged()), this, SLOT(if_Selected()));
}

void Independent_Variables::create_Independent_Variables_Toolbar()
{
	gui_Settings.beginGroup( Paths );
		QString icon_Path = gui_Settings.value( "icon_Path", 0 ).toString();
	gui_Settings.endGroup();

	QPixmap new_Variable	(icon_Path + "new.bmp");
	QPixmap edit			(icon_Path + "roi.bmp");
	QPixmap remove			(icon_Path + "delete.bmp");

	independent_Variables_Toolbar = new QToolBar;

	independent_Variables_Toolbar->addAction(QIcon(new_Variable),	"Add Independent Variable");// 0
	independent_Variables_Toolbar->addAction(QIcon(edit),			"Edit");					// 1
	independent_Variables_Toolbar->addAction(QIcon(remove),			"Remove");					// 2

	independent_Variables_Toolbar->setIconSize(new_Variable.size());

	connect(independent_Variables_Toolbar->actions()[0], SIGNAL(triggered(bool)), this, SLOT(add_Independent_Variable(bool)));
	connect(independent_Variables_Toolbar->actions()[1], SIGNAL(triggered(bool)), this, SLOT(edit_Independent_Variable(bool)));
	connect(independent_Variables_Toolbar->actions()[2], SIGNAL(triggered(bool)), this, SLOT(remove_Independent_Variable(bool)));

	if_Selected();
}

void Independent_Variables::add_Independent_Variable(bool)
{
	// TODO
	Variable_Selection* new_Variable = new Variable_Selection(struct_Tree, independent_Variables_List_Map, independent_Variables_List, Variable_Type::Independent);
		new_Variable->setParent(this);
		new_Variable->setModal(true);
		new_Variable->setWindowFlags(Qt::Window);
		new_Variable->show();
}

void Independent_Variables::edit_Independent_Variable(bool)
{
	// TODO
}

void Independent_Variables::remove_Independent_Variable(bool)
{
	QListWidgetItem* item = independent_Variables_List->currentItem();
	if(item)
	{
		independent_Variables_List_Map->remove(item->whatsThis());
		delete item;
	}
}

void Independent_Variables::if_Selected()
{
	if(independent_Variables_List->selectedItems().isEmpty())
	{
		independent_Variables_Toolbar->actions()[1]->setDisabled(true);		 // edit
		independent_Variables_Toolbar->actions()[2]->setDisabled(true);		 // remove
	} else
	{
		independent_Variables_Toolbar->actions()[1]->setDisabled(false);	 // edit
		independent_Variables_Toolbar->actions()[2]->setDisabled(false);	 // remove
	}
}
