#include "multilayer.h"

Multilayer::Multilayer(Settings* settings):
	settings(settings)
{
	create_Main_Layout();
}

void Multilayer::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);

	create_Struct_Frame();
		main_Layout->addWidget(struct_Frame);
	create_Variables_Frame();
		main_Layout->addWidget(variables_Frame);
	create_Data_Frame();
		main_Layout->addWidget(data_Frame);

	main_Layout->setSpacing(0);
	setLayout(main_Layout);
}

void Multilayer::create_Struct_Frame()
{
	struct_Frame = new QFrame;
	struct_Frame_Layout = new QVBoxLayout(struct_Frame);

	create_Struct_Tree();
		struct_Frame_Layout->addWidget(struct_Tree);
	create_Struct_Toolbar();
		struct_Frame_Layout->addWidget(struct_Toolbar);

	struct_Frame_Layout->setSpacing(0);
	struct_Frame->setContentsMargins(-7,-3,-7,-10);
	struct_Frame->setLayout(struct_Frame_Layout);
}

void Multilayer::create_Struct_Tree()
{
	struct_Tree = new QTreeWidget;
	struct_Tree->setColumnCount(1);
	struct_Tree->header()->close();
	QTreeWidgetItem *parent = new QTreeWidgetItem(struct_Tree,QStringList(QObject::tr("A")));
	parent->addChild(new QTreeWidgetItem(QStringList(QObject::tr("A"))));
	parent->child(0)->addChild(new QTreeWidgetItem(QStringList(QObject::tr("AA"))));
	parent->child(0)->addChild(new QTreeWidgetItem(QStringList(QObject::tr("AB"))));
	parent->addChild(new QTreeWidgetItem(QStringList(QObject::tr("B"))));
	parent->child(1)->addChild(new QTreeWidgetItem(QStringList(QObject::tr("BA"))));
	//connect(parent, SIGNAL(itemDoubleClicked()), parent, SLOT(clear()));
	parent->addChild(new QTreeWidgetItem(QStringList(QObject::tr("C"))));
	struct_Tree->expandAll();
}

void Multilayer::create_Struct_Toolbar()
{
	QPixmap add_Layer		(settings->gui.icon_Path + "add_layer.bmp");
	QPixmap add_Multilayer  (settings->gui.icon_Path + "add_multilayer.bmp");
	QPixmap add_Substrate	(settings->gui.icon_Path + "add_substrate.bmp");
	QPixmap edit			(settings->gui.icon_Path + "roi.bmp");
	QPixmap remove			(settings->gui.icon_Path + "delete.bmp");
	QPixmap cut				(settings->gui.icon_Path + "cut.bmp");
	QPixmap copy			(settings->gui.icon_Path + "copy.bmp");
	QPixmap paste			(settings->gui.icon_Path + "paste.bmp");
	QPixmap move_Up			(settings->gui.icon_Path + "shift_up.bmp");
	QPixmap move_Down		(settings->gui.icon_Path + "shift_down.bmp");
	QPixmap group			(settings->gui.icon_Path + "group.bmp");
	QPixmap ungroup			(settings->gui.icon_Path + "ungroup.bmp");
	QPixmap thickness_Plot	(settings->gui.icon_Path + "zplot.bmp");
	QPixmap sigma_Plot		(settings->gui.icon_Path + "sigmaplot.bmp");
	QPixmap destroy			(settings->gui.icon_Path + "bomb.bmp");

	struct_Toolbar = new QToolBar;
	struct_Toolbar->addAction(QIcon(add_Layer),		"Add Layer");
	struct_Toolbar->addAction(QIcon(add_Multilayer),"Add Multilayer");
	struct_Toolbar->addAction(QIcon(add_Substrate), "Add Substrate");
	struct_Toolbar->addAction(QIcon(edit),			"Edit");
	struct_Toolbar->addAction(QIcon(remove),		"Remove");
	struct_Toolbar->addAction(QIcon(cut),			"Cut");
	struct_Toolbar->addAction(QIcon(copy),			"Copy");
	struct_Toolbar->addAction(QIcon(paste),			"Paste");
	struct_Toolbar->addAction(QIcon(move_Up),		"Move Up");
	struct_Toolbar->addAction(QIcon(move_Down),		"Move Down");
	struct_Toolbar->addAction(QIcon(group),			"Group");
	struct_Toolbar->addAction(QIcon(ungroup),		"Ungroup");
	struct_Toolbar->addAction(QIcon(thickness_Plot),"Plot Layer Thickness Profile");
	struct_Toolbar->addAction(QIcon(sigma_Plot),	"Plot Interface Width Profile");
	struct_Toolbar->addAction(QIcon(destroy),		"Remove Substrate and All Layers");

	struct_Toolbar->setIconSize(add_Layer.size());
}

void Multilayer::create_Variables_Frame()
{
	variables_Frame = new QFrame;
	variables_Frame_Layout = new QVBoxLayout(variables_Frame);

	create_Variable_Tabs();
		variables_Frame_Layout->addWidget(variables_Tabs);

	variables_Frame_Layout->setSpacing(0);
	variables_Frame->setContentsMargins(-7,-8,-9,-10);
	variables_Frame->setLayout(variables_Frame_Layout);
}

void Multilayer::create_Variable_Tabs()
{
	variables_Tabs = new QTabWidget;
	variables_Tabs->addTab(new QWidget(), "Dependent Variables");
	variables_Tabs->addTab(new QWidget(), "Independent Variables");
	variables_Tabs->addTab(new QWidget(), "Fitting");
	variables_Tabs->addTab(new QWidget(), "Optimization");
}

void Multilayer::create_Data_Frame()
{
	data_Frame = new QFrame;
	data_Frame_Layout = new QVBoxLayout(data_Frame);
	data_Frame_Layout->setSpacing(0);
	data_Frame->setContentsMargins(-8,-8,-8,-7);

	data_Measured_Data_Group_Box = new QGroupBox("Measured Data");
	data_Target_Profile_Group_Box= new QGroupBox("Target Profile");

	data_Frame_Layout->addWidget(data_Measured_Data_Group_Box);
	data_Frame_Layout->addWidget(data_Target_Profile_Group_Box);

	layout_Measured_Data_With_Frame_Vector = new QVBoxLayout(data_Measured_Data_Group_Box);
	layout_Target_Profile_With_Frame_Vector= new QVBoxLayout(data_Target_Profile_Group_Box);

	layout_Measured_Data_With_Frame_Vector->setSpacing(0);
	layout_Target_Profile_With_Frame_Vector->setSpacing(0);

	layout_Measured_Data_With_Frame_Vector->setMargin(0);
	layout_Target_Profile_With_Frame_Vector->setMargin(0);

	add_Measured_Data();
	add_Target_Profile();
}

void Multilayer::add_Measured_Data()
{
	// window resizing
	if(!data_Measured_Data_Frame_Vector.isEmpty())
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height()+settings->gui.multilayer_Height_Additive);

	QFrame* new_Frame = new QFrame;	data_Measured_Data_Frame_Vector.append(new_Frame);
	QHBoxLayout* new_Frame_Layout = new QHBoxLayout(new_Frame);
	new_Frame_Layout->setMargin(0);
	QHBoxLayout* left_Layout  = new QHBoxLayout;
	QHBoxLayout* right_Layout = new QHBoxLayout;
	left_Layout->setSpacing(10);
	right_Layout->setSpacing(0);

	QPushButton* new_Import_Button = new QPushButton("Import " + QString::number(data_Measured_Data_Frame_Vector.count()));
	QLabel* new_Description_Label  = new QLabel("<description>");
	QPushButton* new_Add_Button    = new QPushButton("Add "    + QString::number(data_Measured_Data_Frame_Vector.count()));
	QPushButton* new_Remove_Button = new QPushButton("Remove " + QString::number(data_Measured_Data_Frame_Vector.count()));

	new_Import_Button->		setObjectName("Import Data");
	new_Description_Label->	setObjectName("Data Description");
	new_Add_Button->		setObjectName("Add Row");
	new_Remove_Button->		setObjectName("Remove Row");

	new_Import_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Description_Label->	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Add_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Remove_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	left_Layout->addWidget(new_Import_Button);
	left_Layout->addWidget(new_Description_Label, 0, Qt::AlignLeft);
	right_Layout->addWidget(new_Add_Button);
	right_Layout->addWidget(new_Remove_Button);
	new_Frame_Layout->addLayout(left_Layout, Qt::AlignLeft);
	new_Frame_Layout->addLayout(right_Layout, Qt::AlignRight);

	new_Frame->setLayout(new_Frame_Layout);
	layout_Measured_Data_With_Frame_Vector->addWidget(new_Frame);

	//connect(new_Import_Button, SIGNAL(clicked()), this, SLOT(     ));
	connect(new_Add_Button,    SIGNAL(clicked()), this, SLOT(add_Measured_Data()));
	connect(new_Remove_Button, SIGNAL(clicked()), this, SLOT(remove_Measured_Data()));

	// hiding add button
	QPushButton* add_Button;
	if(data_Measured_Data_Frame_Vector.size()>1)
	{
		add_Button = data_Measured_Data_Frame_Vector[data_Measured_Data_Frame_Vector.size()-2]->findChild<QPushButton*>(new_Add_Button->objectName());
		add_Button->hide();
	}
}

void Multilayer::remove_Measured_Data()
{
	QString add_Name = data_Measured_Data_Frame_Vector.first()->findChildren<QPushButton*>().end()[-2]->objectName();	// add button is the second from the end

	int i0=-1;
	for(int i=0; i<data_Measured_Data_Frame_Vector.size(); i++)
		if(QObject::sender()->parent() == data_Measured_Data_Frame_Vector[i]) i0=i;
	if(i0!=-1) data_Measured_Data_Frame_Vector.remove(i0);
	delete QObject::sender()->parent();

	// window resizing
	if(!data_Measured_Data_Frame_Vector.isEmpty())
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height()-settings->gui.multilayer_Height_Additive);

	if(data_Measured_Data_Frame_Vector.isEmpty())
		add_Measured_Data();

	// showing add button
	QPushButton* add_Button;
	add_Button = data_Measured_Data_Frame_Vector.last()->findChild<QPushButton*>(add_Name);
	add_Button->show();
}

void Multilayer::add_Target_Profile()
{
	// window resizing
	if(!data_Target_Profile_Frame_Vector.isEmpty())
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height()+settings->gui.multilayer_Height_Additive);

	QFrame* new_Frame = new QFrame;	data_Target_Profile_Frame_Vector.append(new_Frame);
	QHBoxLayout* new_Frame_Layout = new QHBoxLayout(new_Frame);
	new_Frame_Layout->setMargin(0);
	QHBoxLayout* left_Layout  = new QHBoxLayout;
	QHBoxLayout* right_Layout = new QHBoxLayout;
	left_Layout->setSpacing(10);
	right_Layout->setSpacing(0);

	QPushButton* new_Import_Button = new QPushButton("Import " + QString::number(data_Target_Profile_Frame_Vector.count()));
	QLabel* new_Description_Label  = new QLabel("<description>");
	QPushButton* new_Add_Button    = new QPushButton("Add "    + QString::number(data_Target_Profile_Frame_Vector.count()));
	QPushButton* new_Remove_Button = new QPushButton("Remove " + QString::number(data_Target_Profile_Frame_Vector.count()));

	new_Import_Button->		setObjectName("Import Data");
	new_Description_Label->	setObjectName("Data Description");
	new_Add_Button->		setObjectName("Add Row");
	new_Remove_Button->		setObjectName("Remove Row");

	new_Import_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Description_Label->	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Add_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	new_Remove_Button->		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	left_Layout->addWidget(new_Import_Button);
	left_Layout->addWidget(new_Description_Label, 0, Qt::AlignLeft);
	right_Layout->addWidget(new_Add_Button);
	right_Layout->addWidget(new_Remove_Button);
	new_Frame_Layout->addLayout(left_Layout, Qt::AlignLeft);
	new_Frame_Layout->addLayout(right_Layout, Qt::AlignRight);

	new_Frame->setLayout(new_Frame_Layout);
	layout_Target_Profile_With_Frame_Vector->addWidget(new_Frame);

	//connect(new_Import_Button, SIGNAL(clicked()), this, SLOT(     ));
	connect(new_Add_Button,    SIGNAL(clicked()), this, SLOT(add_Target_Profile()));
	connect(new_Remove_Button, SIGNAL(clicked()), this, SLOT(remove_Target_Profile()));

	// hiding add button
	QPushButton* add_Button;
	if(data_Target_Profile_Frame_Vector.size()>1)
	{
		add_Button = data_Target_Profile_Frame_Vector[data_Target_Profile_Frame_Vector.size()-2]->findChild<QPushButton*>(new_Add_Button->objectName());
		add_Button->hide();
	}
}

void Multilayer::remove_Target_Profile()
{
	QString add_Name = data_Target_Profile_Frame_Vector.first()->findChildren<QPushButton*>().end()[-2]->objectName();	// add button is the second from the end

	int i0=-1;
	for(int i=0; i<data_Target_Profile_Frame_Vector.size(); i++)
		if(QObject::sender()->parent() == data_Target_Profile_Frame_Vector[i]) i0=i;
	if(i0!=-1) data_Target_Profile_Frame_Vector.remove(i0);
	delete QObject::sender()->parent();

	// window resizing
	if(!data_Target_Profile_Frame_Vector.isEmpty())
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height()-settings->gui.multilayer_Height_Additive);

	if(data_Target_Profile_Frame_Vector.isEmpty())
		add_Target_Profile();

	// showing add button
	QPushButton* add_Button;
	add_Button = data_Target_Profile_Frame_Vector.last()->findChild<QPushButton*>(add_Name);
	add_Button->show();
}
