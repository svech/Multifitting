// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "multilayer.h"

Multilayer::Multilayer(Multilayer_Approach* multilayer_Approach, QWidget *parent) :
	multilayer_Approach(multilayer_Approach),
	QWidget(parent)
{
	create_Main_Layout();
}

void Multilayer::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);

	create_Structure_Frame();
	create_Variables_Frame();
	create_Data_Frame();
}

void Multilayer::create_Structure_Frame()
{
	struct_Frame_Layout = new QVBoxLayout;
		struct_Frame_Layout->setSpacing(0);

	struct_Frame = new QFrame;
		struct_Frame->setContentsMargins(-7,-3,-7,-10);
		struct_Frame->setLayout(struct_Frame_Layout);

	structure_Tree = new Structure_Tree(this, this);
	main_Layout->addWidget(struct_Frame);
	connect(structure_Tree, SIGNAL(refresh()), this, SLOT(emit_Refresh()));
}

void Multilayer::create_Variables_Frame()
{
	variables_Frame = new QFrame;
		variables_Frame->setContentsMargins(-7,-8,-9,-10);
	main_Layout->addWidget(variables_Frame);

	variables_Frame_Layout = new QVBoxLayout(variables_Frame);
		variables_Frame_Layout->setSpacing(0);

	create_Variables_Tabs();
		variables_Frame_Layout->addWidget(variables_Tabs);
}

void Multilayer::create_Variables_Tabs()
{
	variables_Tabs = new QTabWidget;

	create_Dependent_Variables_Tabs();
	{
		QFrame* frame = new QFrame;
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(dependent_Variables_Tabs);
		frame->setLayout(layout);
		frame->setContentsMargins(0,-5,0,-8);
		variables_Tabs->addTab(frame, "Dependent Variables");
	}

	create_Independent_Variables_Tabs();
	{
		QFrame* frame = new QFrame;
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(independent_Variables_Plot_Tabs);
		layout->setSpacing(0);
		frame->setLayout(layout);
		frame->setContentsMargins(0,-5,0,-8);
		variables_Tabs->addTab(frame, "Independent Variables");
	}

	create_Coupled_Parameters_List();
	create_Coupled_Parameters_Toolbar();
	{
		QFrame* frame = new QFrame;
		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(coupled_Parameters_List);
		layout->addWidget(coupled_Parameters_Toolbar);
		layout->setSpacing(0);
		frame->setLayout(layout);
		frame->setContentsMargins(0,-5,0,-12);
		variables_Tabs->addTab(frame, "Coupled Parameters");
	}

	// TODO make other variables tabs
	variables_Tabs->addTab(new QWidget(), "Fitting");
	variables_Tabs->addTab(new QWidget(), "Optimization");
	variables_Tabs->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
}

void Multilayer::create_Dependent_Variables_Tabs()
{
	dependent_Variables_Tabs = new QTabWidget;

	specular_Functions_Layout = new QVBoxLayout;
		specular_Functions_Layout->setSpacing(0);
	create_Specular_Functions();
		specular_Functions_Layout->addWidget(standard_Functions_Group_Box);
		specular_Functions_Layout->addWidget(field_Functions_Group_Box);
		specular_Functions_Layout->addWidget(user_Functions_Group_Box);

	QFrame* specular_Frame = new QFrame;
		specular_Frame->setContentsMargins(-5,-5,-5,-9);
		specular_Frame->setLayout(specular_Functions_Layout);
	dependent_Variables_Tabs->addTab(specular_Frame, "Specular Optical Functions");

	// TODO non-specular functions
	nonspecular_Functions_Layout = new QVBoxLayout;
		nonspecular_Functions_Layout->setSpacing(0);
	create_Nonspecular_Functions();
//		nonspecular_Functions_Layout->addWidget(nonspecular_Group_Box);
	QFrame* nonspecular_Frame = new QFrame;
//		nonspecular_Frame->setContentsMargins(-5,-5,-5,-9);
		nonspecular_Frame->setLayout(nonspecular_Functions_Layout);
	dependent_Variables_Tabs->addTab(nonspecular_Frame, "Scattering Functions");
}

void Multilayer::create_Specular_Functions()
{
	standard_Functions_Group_Box = new QGroupBox("Standard Functions");
		standard_Functions_Group_Box->setContentsMargins(0,8,0,-4);

	QHBoxLayout* standard_Functions_Layout = new QHBoxLayout(standard_Functions_Group_Box);
		standard_Functions_Layout->setAlignment(Qt::AlignLeft);
		reflect_Functions = new QCheckBox("Reflectance");
			standard_Functions_Layout->addWidget(reflect_Functions);
		transmit_Functions = new QCheckBox("Transmittance");
			standard_Functions_Layout->addWidget(transmit_Functions);
		absorp_Functions = new QCheckBox("Absorptance");
			standard_Functions_Layout->addWidget(absorp_Functions);

	field_Functions_Group_Box = new QGroupBox("Field Functions");
		field_Functions_Group_Box->setContentsMargins(0,8,0,-4);

	QHBoxLayout* field_Functions_Layout = new QHBoxLayout(field_Functions_Group_Box);
		field_Functions_Layout->setAlignment(Qt::AlignLeft);
		field_Intensity = new QCheckBox("Intensity");
			field_Functions_Layout->addWidget(field_Intensity);
		joule_Absorption= new QCheckBox("Joule Absorption");
			field_Functions_Layout->addWidget(joule_Absorption);

	user_Functions_Group_Box = new QGroupBox("User-defined Functions");
		user_Functions_Group_Box->setContentsMargins(0,8,0,-4);

	QHBoxLayout* user_Functions_Layout = new QHBoxLayout(user_Functions_Group_Box);
		user_Functions_Layout->setAlignment(Qt::AlignLeft);
		user_Supplied_Functions_Check = new QCheckBox;
			user_Functions_Layout->addWidget(user_Supplied_Functions_Check);
		user_Supplied_Functions = new QLineEdit;
			user_Functions_Layout->addWidget(user_Supplied_Functions);
}

void Multilayer::create_Nonspecular_Functions()
{
	// TODO nonspecular functions
}

void Multilayer::create_Independent_Variables_Tabs()
{
	independent_Variables_Plot_Tabs = new QTabWidget;
		independent_Variables_Plot_Tabs->setMovable(true);
		independent_Variables_Plot_Tabs->setTabsClosable(true);

	independent_Variables_Corner_Button = new QToolButton;
		independent_Variables_Corner_Button->setText("+");
	QFont tmp_Qf; tmp_Qf.setBold(true);
		independent_Variables_Corner_Button->setFont(tmp_Qf);
		independent_Variables_Plot_Tabs->setCornerWidget(independent_Variables_Corner_Button);

	connect(independent_Variables_Corner_Button,SIGNAL(clicked()),				 this, SLOT(add_Independent_Variables_Tab()));
	connect(independent_Variables_Plot_Tabs,	SIGNAL(currentChanged(int)),	 this, SLOT(change_Tab_Independent_Variables_Tab_Color(int)));
	connect(independent_Variables_Plot_Tabs,	SIGNAL(tabCloseRequested(int)),  this, SLOT(remove_Independent_Variables_Tab(int)));
	connect(independent_Variables_Plot_Tabs,	SIGNAL(tabBarDoubleClicked(int)),this, SLOT(rename_Independent_Variables_Tab(int)));

	add_Independent_Variables_Tab();
	independent_Tabs_Exist = true;
}

void Multilayer::create_Coupled_Parameters_List()
{
	// TODO invisible if empty
	coupled_Parameters_List = new QListWidget;
	coupled_Parameters_List->setSizeAdjustPolicy(QListWidget::AdjustToContents);
	QSizePolicy sp_Retain = coupled_Parameters_List->sizePolicy();
				sp_Retain.setRetainSizeWhenHidden(true);
	coupled_Parameters_List->setSizePolicy(sp_Retain);

//	coupled_Parameters_List->hide();
}

void Multilayer::create_Coupled_Parameters_Toolbar()
{
	QPixmap new_Variable	(icon_path + "new.bmp");
	QPixmap edit			(icon_path + "roi.bmp");
	QPixmap remove			(icon_path + "delete.bmp");

	coupled_Parameters_Toolbar = new QToolBar;
	coupled_Parameters_Toolbar->addAction(QIcon(new_Variable),	"Add Coupled Parameter");	// 0
	coupled_Parameters_Toolbar->addAction(QIcon(edit),			"Edit");					// 1
	coupled_Parameters_Toolbar->addAction(QIcon(remove),		"Remove");					// 2

	coupled_Parameters_Toolbar->setIconSize(new_Variable.size());

	// TODO connect actions
}

void Multilayer::create_Fitting_Parameters_List()
{
// TODO
}

void Multilayer::create_Fitting_Parameters_Toolbar()
{
// TODO
}

void Multilayer::create_Optimization_Parameters_List()
{
// TODO
}

void Multilayer::create_Optimization_Parameters_Toolbar()
{
// TODO
}

void Multilayer::create_Data_Frame()
{
	data_Frame = new QFrame;
		data_Frame->setContentsMargins(-8,-8,-8,-7);
	main_Layout->addWidget(data_Frame);

	data_Frame_Layout = new QVBoxLayout(data_Frame);
		data_Frame_Layout->setSpacing(0);


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

// --------------------------------------------------------------------------------------

void Multilayer::add_Independent_Variables_Tab()
{
	// hidden copy of main structure
	QTreeWidget* new_Struct_Tree_Copy = new QTreeWidget;
	for(int i=0; i<structure_Tree->tree->topLevelItemCount(); i++)
	{
		new_Struct_Tree_Copy->addTopLevelItem(structure_Tree->tree->topLevelItem(i)->clone());
	}

	// add "measurement" item to structure_Tree->tree_Copy
	QTreeWidgetItem* new_Measurement_Item = new QTreeWidgetItem;
		// set unique id to measurenent
		int id = rand()*RAND_SHIFT+rand();
		new_Measurement_Item->setStatusTip(DEFAULT_COLUMN, QString::number(id));

		Measurement measurement;
		QVariant var; var.setValue(measurement);
		new_Measurement_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
		new_Measurement_Item->setWhatsThis(DEFAULT_COLUMN, whats_This_Measurement);
	new_Struct_Tree_Copy->insertTopLevelItem(0, new_Measurement_Item);

	// create new Independent_Variables instance
	Independent_Variables* new_Independent = new Independent_Variables(new_Struct_Tree_Copy, structure_Tree->tree, this);
		new_Independent->setContentsMargins(-8,-10,-8,-10);

		connect(new_Independent, SIGNAL(refresh()), this, SLOT(emit_Refresh()));

	// add new tab
	independent_Variables_Plot_Tabs->addTab(new_Independent, default_independent_variable_tab_name);
	independent_Variables_Plot_Tabs->setTabText(independent_Variables_Plot_Tabs->count()-1, default_independent_variable_tab_name+QString::number(independent_Variables_Plot_Tabs->count()));

	if(independent_Variables_Plot_Tabs->count()>1)
	{
		independent_Variables_Plot_Tabs->tabBar()->setTabTextColor(independent_Variables_Plot_Tabs->count()-1,Qt::gray);
		independent_Variables_Plot_Tabs->tabBar()->tabButton(independent_Variables_Plot_Tabs->count()-1, QTabBar::RightSide)->hide();
	}
	independent_Tabs_Exist = true;
}

void Multilayer::change_Tab_Independent_Variables_Tab_Color(int index)
{
	independent_Variables_Plot_Tabs->tabBar()->setTabTextColor(index,Qt::black);
	if(independent_Variables_Plot_Tabs->tabBar()->tabButton(index, QTabBar::RightSide))
	   independent_Variables_Plot_Tabs->tabBar()->tabButton(index, QTabBar::RightSide)->show();

	for(int i = 0; i<independent_Variables_Plot_Tabs->tabBar()->count(); i++)
	{
		if(i!=index)
		{
			independent_Variables_Plot_Tabs->tabBar()->tabButton(i, QTabBar::RightSide)->hide();
			independent_Variables_Plot_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	}
}

void Multilayer::remove_Independent_Variables_Tab(int index)
{
	QMessageBox::StandardButton reply = QMessageBox::question(this,"Removal", "Variables tab \"" + independent_Variables_Plot_Tabs->tabBar()->tabText(index) + "\" will be removed.\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		delete independent_Variables_Plot_Tabs->widget(index);
		if(independent_Variables_Plot_Tabs->count()==0) add_Independent_Variables_Tab();
	}
}

void Multilayer::rename_Independent_Variables_Tab(int tab_Index)
{
	bool ok;
	QString text = QInputDialog::getText(this, "Rename plot", "New name", QLineEdit::Normal, independent_Variables_Plot_Tabs->tabText(tab_Index), &ok);
	if (ok && !text.isEmpty())
		independent_Variables_Plot_Tabs->setTabText(tab_Index, text);

	// TODO temporary
//	print_Hidden_Copy(tab_Index);
}

void Multilayer::reset_Independent_Variables_Structure()
{
	// PARAMETER
	if(independent_Tabs_Exist)
	for(int tab=0; tab<independent_Variables_Plot_Tabs->count(); tab++)
	{
		Independent_Variables* independent_Widget = dynamic_cast<Independent_Variables*>(independent_Variables_Plot_Tabs->widget(tab));

		/// -----------------------------------------------------

		// temporary copy
		QTreeWidget* temp_Tree = new QTreeWidget;
		for(int i=0; i<independent_Widget->struct_Tree_Copy->topLevelItemCount(); i++)
		{
			temp_Tree->addTopLevelItem(independent_Widget->struct_Tree_Copy->topLevelItem(i)->clone());
		};

		// clear
		independent_Widget->clear_Structure();

		// reset hidden copy of main structure
		for(int i=0; i<structure_Tree->tree->topLevelItemCount(); i++)
		{
			independent_Widget->struct_Tree_Copy->addTopLevelItem(structure_Tree->tree->topLevelItem(i)->clone());
		}

		// search for previously existing items
		QTreeWidgetItem* structure_Item;
		QTreeWidgetItemIterator it(independent_Widget->struct_Tree_Copy);
		while (*it)
		{
			structure_Item = *it;

			QString whats_This = structure_Item->whatsThis(DEFAULT_COLUMN);
			QStringList whats_This_List = whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);
			QVariant var;

			// iterate over copy of old tree
			QTreeWidgetItem* old_Item;
			QTreeWidgetItemIterator old_It(temp_Tree);
			while (*old_It)
			{
				old_Item = *old_It;

				if(structure_Item->statusTip(DEFAULT_COLUMN) == old_Item->statusTip(DEFAULT_COLUMN))
				{
					// if measurement
					if(whats_This_List[0] == whats_This_Measurement)
					{
						Measurement measurement     = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
						Measurement old_Measurement =       old_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();

						measurement = old_Measurement; //-V519

						var.setValue(measurement);
						structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
					}
					// if ambient
					if(whats_This_List[0] == whats_This_Ambient)
					{
						Ambient ambient     = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
						Ambient old_Ambient =       old_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();

						/// optical
						ambient.absolute_Density.independent = old_Ambient.absolute_Density.independent;
						ambient.relative_Density.independent = old_Ambient.relative_Density.independent;
						ambient.permittivity.independent	 = old_Ambient.permittivity.independent;
						ambient.absorption.independent		 = old_Ambient.absorption.independent;

						for(int index=0; index<ambient.composition.size(); ++index)
						{
							ambient.composition[index].composition.independent.min = 0;
							ambient.composition[index].composition.independent.max = 1;
							ambient.composition[index].composition.independent.num_Points = 1;
						}

						var.setValue(ambient);
						structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
					}
					// if layer
					if(whats_This_List[0] == whats_This_Layer)
					{
						Layer layer     = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
						Layer old_Layer =       old_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();

						/// optical
						layer.absolute_Density.independent	= old_Layer.absolute_Density.independent;
						layer.relative_Density.independent	= old_Layer.relative_Density.independent;
						layer.permittivity.independent		= old_Layer.permittivity.independent;
						layer.absorption.independent		= old_Layer.absorption.independent;

						for(int index=0; index<layer.composition.size(); ++index)
						{
							layer.composition[index].composition.independent.min = 0;
							layer.composition[index].composition.independent.max = 1;
							layer.composition[index].composition.independent.num_Points = 1;
						}

						/// thickness
						layer.thickness.independent = old_Layer.thickness.independent;

						/// interface parameters
						layer.sigma.independent = old_Layer.sigma.independent;

						for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
						{
							layer.interlayer_Composition[func_Index].interlayer.independent = old_Layer.interlayer_Composition[func_Index].interlayer.independent;
						}

						var.setValue(layer);
						structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
					}
					// if multilayer
					if(whats_This_List[0] == whats_This_Multilayer)
					{
						Stack_Content stack_Content     = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
						Stack_Content old_Stack_Content =       old_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();

						stack_Content.num_Repetition     = old_Stack_Content.num_Repetition;
						stack_Content.period.independent = old_Stack_Content.period.independent;
						stack_Content.gamma.independent  = old_Stack_Content.gamma.independent;

						var.setValue(stack_Content);
						structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
					}
					// if substrate
					if(whats_This_List[0] == whats_This_Substrate)
					{
						Substrate substrate     = structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
						Substrate old_Substrate =       old_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();

						/// optical
						substrate.absolute_Density.independent	= old_Substrate.absolute_Density.independent;
						substrate.relative_Density.independent	= old_Substrate.relative_Density.independent;
						substrate.permittivity.independent		= old_Substrate.permittivity.independent;
						substrate.absorption.independent		= old_Substrate.absorption.independent;

						for(int index=0; index<substrate.composition.size(); ++index)
						{
							substrate.composition[index].composition.independent.min = 0;
							substrate.composition[index].composition.independent.max = 1;
							substrate.composition[index].composition.independent.num_Points = 1;
						}

						/// interface parameters
						substrate.sigma.independent = old_Substrate.sigma.independent;

						for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
						{
							substrate.interlayer_Composition[func_Index].interlayer.independent = old_Substrate.interlayer_Composition[func_Index].interlayer.independent;
						}

						var.setValue(substrate);
						structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
					}
				}
				++old_It;
			}
			++it;

//			qInfo() << "reset_Text";
//			QTreeWidgetItem* temp_structure_Item;
//			QTreeWidgetItemIterator temp_it(independent_Widget->struct_Tree_Copy);
//			while (*temp_it)
//			{
//				temp_structure_Item = *temp_it;

//				QString whats_This = temp_structure_Item->whatsThis(DEFAULT_COLUMN);
//				QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);
//				QStringList whats_This_List_Type = whats_This_List[0].split(item_Type_Delimiter,QString::SkipEmptyParts);
//				if(whats_This_List_Type[0] == whats_This_Ambient)
//				{
//					Ambient ambient = temp_structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
//					qInfo() <<whats_This_List[0]<<" : "<< ambient.permittivity.independent.num_Points;
//				}
//				if(whats_This_List_Type[0] == whats_This_Layer)
//				{
//					Layer layer = temp_structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
//					qInfo() <<whats_This_List[0]<<" : "<< layer.sigma.independent.num_Points;
//				}
//				if(whats_This_List_Type[0] == whats_This_Substrate)
//				{
//					Substrate substrate = temp_structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
//					qInfo() <<whats_This_List[0]<<" : "<< substrate.sigma.independent.num_Points;
//				}
//				++temp_it;
//			}
//			qInfo() <<"\n";
		}

		delete temp_Tree;

		/// -----------------------------------------------------

		// clear irrelevant variables
		independent_Widget->clear_Independent_List();
		independent_Widget->refresh_Text();
	}
}

void Multilayer::refresh_Text()
{
//	if(struct_Exist)
	{
		structure_Tree->refresh_Text();
		structure_Tree->editors_Refresh();
		structure_Tree->tree->expandAll();
	}
	if(independent_Tabs_Exist)
	for(int tab=0; tab<independent_Variables_Plot_Tabs->count(); tab++)
	{
		dynamic_cast<Independent_Variables*>(independent_Variables_Plot_Tabs->widget(tab))->refresh_Text();
	}
}

void Multilayer::refresh_State()
{
	// TODO
	refresh_Text();
}

void Multilayer::add_Measured_Data()
{
	setUpdatesEnabled(false);

	// window resizing
	if(!data_Measured_Data_Frame_Vector.isEmpty())
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() + multilayer_height_additive);

	QFrame* new_Frame = new QFrame;	data_Measured_Data_Frame_Vector.append(new_Frame);
	QHBoxLayout* new_Frame_Layout = new QHBoxLayout(new_Frame);
	new_Frame_Layout->setMargin(0);
	QHBoxLayout* left_Layout  = new QHBoxLayout;
	QHBoxLayout* right_Layout = new QHBoxLayout;
	left_Layout->setSpacing(10);
	right_Layout->setSpacing(0);

	QPushButton* new_Import_Button = new QPushButton("Import");
	QLabel* new_Description_Label  = new QLabel("<no description>");
	QPushButton* new_Add_Button    = new QPushButton("Add Row");
	QPushButton* new_Remove_Button = new QPushButton("Remove");

	new_Import_Button->		setObjectName("Import");
	new_Description_Label->	setObjectName("Description");
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

	setUpdatesEnabled(true);
}

void Multilayer::remove_Measured_Data()
{
	setUpdatesEnabled(false);

	QString add_Name = data_Measured_Data_Frame_Vector.first()->findChildren<QPushButton*>().end()[-2]->objectName();	// add button is the second from the end

	int i0=-1;
	for(int i=0; i<data_Measured_Data_Frame_Vector.size(); i++)
		if(QObject::sender()->parent() == data_Measured_Data_Frame_Vector[i]) i0=i;
	if(i0!=-1) data_Measured_Data_Frame_Vector.remove(i0);
	delete QObject::sender()->parent();

	// window resizing
	if(!data_Measured_Data_Frame_Vector.isEmpty())
	{
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() - multilayer_height_additive);
		adjustSize();
	}

	if(data_Measured_Data_Frame_Vector.isEmpty())
		add_Measured_Data();

	setUpdatesEnabled(false);

	// showing add button
	QPushButton* add_Button;
	add_Button = data_Measured_Data_Frame_Vector.last()->findChild<QPushButton*>(add_Name);
	add_Button->show();

	setUpdatesEnabled(true);
}

void Multilayer::add_Target_Profile()
{
	setUpdatesEnabled(false);

	// window resizing
	if(!data_Target_Profile_Frame_Vector.isEmpty())
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() + multilayer_height_additive);

	QFrame* new_Frame = new QFrame;	data_Target_Profile_Frame_Vector.append(new_Frame);

	QHBoxLayout* new_Frame_Layout = new QHBoxLayout(new_Frame);
	new_Frame_Layout->setMargin(0);
	QHBoxLayout* left_Layout  = new QHBoxLayout;
	QHBoxLayout* right_Layout = new QHBoxLayout;
	left_Layout->setSpacing(10);
	right_Layout->setSpacing(0);

	QPushButton* new_Import_Button = new QPushButton("Import");
	QLabel* new_Description_Label  = new QLabel("<no description>");
	QPushButton* new_Add_Button    = new QPushButton("Add Row");
	QPushButton* new_Remove_Button = new QPushButton("Remove");

	new_Import_Button->		setObjectName("Import");
	new_Description_Label->	setObjectName("Description");
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

	setUpdatesEnabled(true);
}

void Multilayer::remove_Target_Profile()
{
	setUpdatesEnabled(false);

	QString add_Name = data_Target_Profile_Frame_Vector.first()->findChildren<QPushButton*>().end()[-2]->objectName();	// add_button is the second from the end

	int i0=-1;
	for(int i=0; i<data_Target_Profile_Frame_Vector.size(); i++)
		if(QObject::sender()->parent() == data_Target_Profile_Frame_Vector[i]) i0=i;
	if(i0!=-1) data_Target_Profile_Frame_Vector.remove(i0);
	delete QObject::sender()->parent();

	// window resizing
	if(!data_Target_Profile_Frame_Vector.isEmpty())
	{
		QWidget::window()->resize(QWidget::window()->width(),QWidget::window()->height() - multilayer_height_additive);
		adjustSize();
	}

	if(data_Target_Profile_Frame_Vector.isEmpty())
		add_Target_Profile();

	setUpdatesEnabled(false);

	// showing add button
	QPushButton* add_Button;
	add_Button = data_Target_Profile_Frame_Vector.last()->findChild<QPushButton*>(add_Name);
	add_Button->show();

	setUpdatesEnabled(true);
}

void Multilayer::emit_Refresh()
{	
	emit refresh();
}
