#include "menu.h"

Menu::Menu(QString window_Type, QWidget *parent):
	window_Type(window_Type),
	QWidget(parent),
	my_Parent(parent)
{
	menu_Bar = new QMenuBar(this);
	add_Menu_Points();
}

void Menu::add_Menu_Points()
{
	{
		create_File_Menu();
			menu_Bar->addMenu(file_Menu);
	}
	if(window_Type == Window_Type::Independent_Variables_Editor())
	{
		independent_Variables_Editor = qobject_cast<Independent_Variables_Editor*>(my_Parent);

		create_Independent_Units_Menu();
			menu_Bar->addMenu(units_Menu);
		create_Independent_Precision_Menu();
			menu_Bar->addMenu(precision_Menu);
	}
	if(window_Type == Window_Type::Item_Editor())
	{
		item_Editor = qobject_cast<Item_Editor*>(my_Parent);

		create_Item_Units_Menu();
			menu_Bar->addMenu(units_Menu);
		create_Item_Precision_Menu();
			menu_Bar->addMenu(precision_Menu);
	}
	if(window_Type == Window_Type::Launcher() ||
	   window_Type == Window_Type::Multilayer_Approach())
	{
		create_Help_Menu();
			menu_Bar->addMenu(help_Menu);
	}
}

void Menu::create_File_Menu()
{
	file_Menu = new QMenu("File", this);
	{

		if(window_Type == Window_Type::Multilayer_Approach())
		{
			QAction* act_Open = new QAction("Open...", this);			
			act_Open->setShortcut(Qt::Key_O | Qt::CTRL);
			connect(act_Open, SIGNAL(triggered()), my_Parent, SLOT(open()));
			file_Menu->addAction(act_Open);

			QAction* act_Save = new QAction("Save...", this);
			act_Save->setShortcut(Qt::Key_S | Qt::CTRL);
			file_Menu->addAction(act_Save);
			connect(act_Save, SIGNAL(triggered()), my_Parent, SLOT(save()));
		}

		QAction* act_Quit = new QAction("Done", this);		
		act_Quit->setShortcut(Qt::Key_D | Qt::CTRL);
		connect(act_Quit, SIGNAL(triggered()), my_Parent, SLOT(close()));		
		file_Menu->addAction(act_Quit);

		if(window_Type == Window_Type::Launcher() ||
		   window_Type == Window_Type::Multilayer_Approach())
		{
			act_Quit->setText("Close all");
			act_Quit->setShortcut(Qt::Key_Q | Qt::CTRL);
			connect(act_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
		}
	}
}

void Menu::create_Independent_Units_Menu()
{
	// PARAMETER

	units_Menu = new QMenu("Units", this);

	QString whats_This = independent_Variables_Editor->item->whatsThis();
	QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// if density, permittivity, absorption, composition, num_repetitions, or gamma
	if((whats_This_List[1] == whats_This_Density)				 ||
	   (whats_This_List[1] == whats_This_Permittivity)			 ||
	   (whats_This_List[1] == whats_This_Absorption)			 ||
	   (whats_This_List[1] == whats_This_Composition)			 ||
	   (whats_This_List[1] == whats_This_Interlayer_Composition) ||
	   (whats_This_List[1] == whats_This_Num_Repetitions)		 ||
	   (whats_This_List[1] == whats_This_Gamma))
	{
		units_Menu->setDisabled(true);
	}
	// if thickness, sigma or period
	if((whats_This_List[1] == whats_This_Thickness) ||
	   (whats_This_List[1] == whats_This_Sigma)	    ||
	   (whats_This_List[1] == whats_This_Period))
	{
		QActionGroup* group_Act_Unit = new QActionGroup(this);
			group_Act_Unit->setExclusive(true);

		for(int index=0; index<length_Units_List.size(); index++)
		{
			QAction* act_Unit = new QAction(length_Units_List[index], this);
				act_Unit->setProperty(index_Property, index);
				act_Unit->setCheckable(true);
				act_Unit->setActionGroup(group_Act_Unit);

				if(length_Units_List[index] == length_units) act_Unit->setChecked(true);

			units_Menu->addAction(act_Unit);

			connect(act_Unit, SIGNAL(triggered()), this, SLOT(set_Length_Unit()));
		}
	}
	// if angle
	if(whats_This_List[1] == whats_This_Angle)
	{
		QActionGroup* group_Act_Angle = new QActionGroup(this);
			group_Act_Angle->setExclusive(true);

		// Grazing & Incidence
		for(int index=0; index<angle_Units_List.size(); index++)
		{
			QAction* act_Grazing   = new QAction(angle_Units_Legend_Map.value(angle_Units_List[index]) + ", " + Angle_Type::Grazing(),   this);
			QAction* act_Incidence = new QAction(angle_Units_Legend_Map.value(angle_Units_List[index]) + ", " + Angle_Type::Incidence(), this);
				act_Grazing->setProperty  (index_Property, index);
				act_Incidence->setProperty(index_Property, index);

				act_Grazing->setCheckable  (true);
				act_Incidence->setCheckable(true);

				act_Grazing->setActionGroup  (group_Act_Angle);
				act_Incidence->setActionGroup(group_Act_Angle);

				if(angle_Units_List[index] == angle_units)
				{
					Measurement measurement = independent_Variables_Editor->structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
					if(measurement.angle_Type == Angle_Type::Grazing())		act_Grazing->setChecked(true);
					if(measurement.angle_Type == Angle_Type::Incidence())	act_Incidence->setChecked(true);
				}

			units_Menu->addAction(act_Grazing);
			units_Menu->addAction(act_Incidence);

			connect(act_Grazing,   SIGNAL(triggered()), this, SLOT(set_Grazing_Unit()));
			connect(act_Incidence, SIGNAL(triggered()), this, SLOT(set_Incidence_Unit()));
		}
	}
	// if wavelength
	if(whats_This_List[1] == whats_This_Wavelength)
	{
		QActionGroup* group_Act_Unit = new QActionGroup(this);
			group_Act_Unit->setExclusive(true);

		for(int index=0; index<wavelength_Units_List.size(); index++)
		{
			QAction* act_Unit = new QAction(wavelength_Units_List[index], this);
				act_Unit->setProperty(index_Property, index);
				act_Unit->setCheckable(true);
				act_Unit->setActionGroup(group_Act_Unit);

				if(wavelength_Units_List[index] == wavelength_units) act_Unit->setChecked(true);

			units_Menu->addAction(act_Unit);

			connect(act_Unit, SIGNAL(triggered()), this, SLOT(set_Wavelength_Unit()));
		}
	}
}

void Menu::create_Independent_Precision_Menu()
{
	// PARAMETER

	QString whats_This = independent_Variables_Editor->item->whatsThis();
	QStringList whats_This_List = whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	precision_Menu = new QMenu("Precision", this);

	QMenu* menu_Thumb  = new QMenu("Thumbnail precision", this);
	QMenu* menu_Edit   = new QMenu("Operating precision", this);

	precision_Menu->addMenu(menu_Thumb);
	precision_Menu->addMenu(menu_Edit);

	QActionGroup* group_Precision_Thumb = new QActionGroup(this);
	QActionGroup* group_Precision_Edit  = new QActionGroup(this);
		group_Precision_Thumb->setExclusive(true);
		group_Precision_Edit->setExclusive(true);

	for(int index=0; index<MAX_PRECISION_USED; index++)
	{
		QAction* precision_Thumb  = new QAction(QString::number(index), this);
		QAction* precision_Edit   = new QAction(QString::number(index), this);
			precision_Thumb->setProperty(index_Property, index);
			precision_Edit->setProperty(index_Property, index);
			precision_Thumb->setCheckable(true);
			precision_Edit->setCheckable(true);
			precision_Thumb->setActionGroup(group_Precision_Thumb);
			precision_Edit->setActionGroup(group_Precision_Edit);

			menu_Thumb->addAction(precision_Thumb);
			menu_Edit->addAction(precision_Edit);

		// if density
		if(whats_This_List[1] == whats_This_Density)
		{
			if(index == thumbnail_density_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_density_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Density_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Density_Precision()));
		}
		// if permittivity
		if(whats_This_List[1] == whats_This_Permittivity)
		{
			if(index == thumbnail_permittivity_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_permittivity_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Permittivity_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Permittivity_Precision()));
		}
		// if absorption
		if(whats_This_List[1] == whats_This_Absorption)
		{
			if(index == thumbnail_absorption_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_absorption_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Absorption_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Absorption_Precision()));
		}
		// if composition
		if(whats_This_List[1] == whats_This_Composition)
		{
			if(index == thumbnail_composition_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_composition_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Composition_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Composition_Precision()));
		}
		// if thickness
		if(whats_This_List[1] == whats_This_Thickness)
		{
			if(index == thumbnail_thickness_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_thickness_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Thickness_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Thickness_Precision()));
		}
		// if sigma
		if(whats_This_List[1] == whats_This_Sigma)
		{
			if(index == thumbnail_sigma_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_sigma_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Sigma_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Sigma_Precision()));
		}
		// if interlayer composition
		if(whats_This_List[1] == whats_This_Interlayer_Composition)
		{
			if(index == thumbnail_interlayer_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_interlayer_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Interlayer_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Interlayer_Precision()));
		}
		// multilayer num_repetitions
		if(whats_This_List[1] == whats_This_Num_Repetitions)
		{
			precision_Menu->setDisabled(true);
		}
		// multilayer period
		if(whats_This_List[1] == whats_This_Period)
		{
			if(index == thumbnail_period_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_period_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Period_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Period_Precision()));
		}
		// multilayer gamma
		if(whats_This_List[1] == whats_This_Gamma)
		{
			if(index == thumbnail_gamma_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_gamma_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Gamma_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Gamma_Precision()));
		}
		// if angle
		if(whats_This_List[1] == whats_This_Angle)
		{
			if(index == thumbnail_angle_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_angle_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Angle_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Angle_Precision()));
		}
		// if wavelength
		if(whats_This_List[1] == whats_This_Wavelength)
		{
			if(index == thumbnail_wavelength_precision) precision_Thumb->setChecked(true);
			if(index == line_edit_wavelength_precision) precision_Edit->setChecked(true);

			connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(set_Thumbnail_Wavelength_Precision()));
			connect(precision_Edit, SIGNAL(triggered()), this, SLOT(set_Line_Edit_Wavelength_Precision()));
		}
	}
}

void Menu::create_Item_Units_Menu()
{
	// PARAMETER

	QString whats_This = item_Editor->item->whatsThis(DEFAULT_COLUMN);
	QStringList whats_This_List = whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

	units_Menu = new QMenu("Units", this);	

	if(whats_This_List[0] == whats_This_Ambient)
		units_Menu->setDisabled(true);

	menu_Length_Units = new QMenu("Lengths", this);
	{
		QActionGroup* group_Act_Unit = new QActionGroup(this);
			group_Act_Unit->setExclusive(true);

		for(int index=0; index<length_Units_List.size(); index++)
		{
			QAction* act_Unit = new QAction(length_Units_List[index], this);
				act_Unit->setProperty(index_Property, index);
				act_Unit->setCheckable(true);
				act_Unit->setActionGroup(group_Act_Unit);

				if(length_Units_List[index] == length_units) act_Unit->setChecked(true);

			menu_Length_Units->addAction(act_Unit);
			units_Menu->addMenu(menu_Length_Units);

			connect(act_Unit, SIGNAL(triggered()), this, SLOT(set_Length_Unit()));
		}
	}
}

void Menu::create_Item_Precision_Menu()
{
	// PARAMETER

	QString whats_This = item_Editor->item->whatsThis(DEFAULT_COLUMN);
	QStringList whats_This_List = whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

	precision_Menu = new QMenu("Precision", this);

	menu_Density	 = new QMenu("Density", this);
	menu_Lengths	 = new QMenu("Lengths", this);
	menu_Composition = new QMenu("Atomic Composition", this);
	menu_Interlayer  = new QMenu("Interlayer Composition", this);

	menu_Thumb  = new QMenu("Thumbnail precision", this);
	menu_Edit   = new QMenu("Operating precision", this);

	if(whats_This_List[0] == whats_This_Ambient ||
	   whats_This_List[0] == whats_This_Layer   ||
	   whats_This_List[0] == whats_This_Substrate)
	{
		precision_Menu->addMenu(menu_Density);
			menu_Density->addMenu(menu_Thumb);
			menu_Density->addMenu(menu_Edit);
	}
	if(whats_This_List[0] == whats_This_Multilayer ||
	   whats_This_List[0] == whats_This_Layer	   ||
	   whats_This_List[0] == whats_This_Substrate)
	{
		precision_Menu->addMenu(menu_Lengths);
			menu_Lengths->addMenu(menu_Thumb);
			menu_Lengths->addMenu(menu_Edit);
	}
	if(whats_This_List[0] == whats_This_Ambient ||
	   whats_This_List[0] == whats_This_Layer   ||
	   whats_This_List[0] == whats_This_Substrate)
	{
		precision_Menu->addMenu(menu_Composition);
			//menu_Composition->addMenu(menu_Thumb);
			menu_Composition->addMenu(menu_Edit);
	}
	if(whats_This_List[0] == whats_This_Layer   ||
	   whats_This_List[0] == whats_This_Substrate)
	{
		precision_Menu->addMenu(menu_Interlayer);
			//menu_Interlayer->addMenu(menu_Thumb);
			menu_Interlayer->addMenu(menu_Edit);
	}

	connect(menu_Density,	 SIGNAL(aboutToShow()), this, SLOT(menu_Focus()));
	connect(menu_Lengths,  SIGNAL(aboutToShow()), this, SLOT(menu_Focus()));
	connect(menu_Composition,SIGNAL(aboutToShow()), this, SLOT(menu_Focus()));
	connect(menu_Interlayer, SIGNAL(aboutToShow()), this, SLOT(menu_Focus()));

	connect(menu_Thumb,		 SIGNAL(aboutToShow()), this, SLOT(thumb_Edit_Focus()));
	connect(menu_Edit,		 SIGNAL(aboutToShow()), this, SLOT(thumb_Edit_Focus()));

	QActionGroup* group_Precision_Thumb = new QActionGroup(this);
	QActionGroup* group_Precision_Edit  = new QActionGroup(this);
		group_Precision_Thumb->setExclusive(true);
		group_Precision_Edit->setExclusive(true);

	for(int index=0; index<MAX_PRECISION_USED; index++)
	{
		QAction* precision_Thumb  = new QAction(QString::number(index), menu_Thumb);
		QAction* precision_Edit   = new QAction(QString::number(index), menu_Edit);
			precision_Thumb->setProperty(index_Property, index);
			precision_Edit->setProperty(index_Property, index);

			precision_Thumb->setCheckable(true);
			precision_Edit->setCheckable(true);
			precision_Thumb->setActionGroup(group_Precision_Thumb);
			precision_Edit->setActionGroup(group_Precision_Edit);

			menu_Thumb->addAction(precision_Thumb);
			menu_Edit->addAction(precision_Edit);

		connect(precision_Thumb,SIGNAL(triggered()), this, SLOT(activate_Item_Thumbnail_Precision()));
		connect(precision_Edit, SIGNAL(triggered()), this, SLOT(activate_Item_Line_Edit_Precision()));
	}
}

void Menu::menu_Focus()
{
	precision_Menu_Title = qobject_cast<QMenu*>(sender())->title();
}

void Menu::thumb_Edit_Focus()
{
	precision_Menu_Thumb_Edit = qobject_cast<QMenu*>(sender())->title();;

	// density
	if(menu_Density->title() == precision_Menu_Title)
	{
		if(precision_Menu_Thumb_Edit == menu_Thumb->title()) menu_Thumb->actions()[thumbnail_density_precision]->setChecked(true);
		if(precision_Menu_Thumb_Edit == menu_Edit ->title()) menu_Edit ->actions()[line_edit_density_precision]->setChecked(true);
	}

	// thickness
	if(menu_Lengths->title() == precision_Menu_Title)
	{
		if(precision_Menu_Thumb_Edit == menu_Thumb->title()) menu_Thumb->actions()[thumbnail_thickness_precision]->setChecked(true);
		if(precision_Menu_Thumb_Edit == menu_Edit ->title()) menu_Edit ->actions()[line_edit_thickness_precision]->setChecked(true);
	}

	// composition
	if(menu_Composition->title() == precision_Menu_Title)
	{
		if(precision_Menu_Thumb_Edit == menu_Thumb->title()) menu_Thumb->actions()[thumbnail_composition_precision]->setChecked(true);
		if(precision_Menu_Thumb_Edit == menu_Edit ->title()) menu_Edit ->actions()[line_edit_composition_precision]->setChecked(true);
	}

	// interlayer
	if(menu_Interlayer->title() == precision_Menu_Title)
	{
		if(precision_Menu_Thumb_Edit == menu_Thumb->title()) menu_Thumb->actions()[thumbnail_interlayer_precision]->setChecked(true);
		if(precision_Menu_Thumb_Edit == menu_Edit ->title()) menu_Edit ->actions()[line_edit_interlayer_precision]->setChecked(true);
	}
}

void Menu::create_Help_Menu()
{
	help_Menu = new QMenu("Help",this);
	{
		QAction* act_Documentation = new QAction("Multifitting.pdf", this);
		help_Menu->addAction(act_Documentation);
		connect(act_Documentation, SIGNAL(triggered()), this, SLOT(open_Documentation()));
	}
	{
		QAction* act_About = new QAction("About Multifitting",this);
		help_Menu->addAction(act_About);
		connect(act_About, SIGNAL(triggered()), this, SLOT(open_About()));
	}
}

// menu actions

void Menu::open_Documentation()
{
	// TODO
}

void Menu::open_About()
{
	// TODO
	QMessageBox::about(this,"About","-Say something\n-Something");
}

void Menu::set_Grazing_Unit()
{
	Measurement measurement = independent_Variables_Editor->structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
	int index = sender()->property(index_Property).toInt();
	//-------------------------------------------------
	angle_units				= angle_Units_List[index];
	measurement.angle_Type	= Angle_Type::Grazing();
	//-------------------------------------------------
	QVariant var; var.setValue(measurement);
	independent_Variables_Editor->structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	emit refresh();
}

void Menu::set_Incidence_Unit()
{
	Measurement measurement = independent_Variables_Editor->structure_Item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
	int index = sender()->property(index_Property).toInt();
	//-------------------------------------------------
	angle_units				= angle_Units_List[index];
	measurement.angle_Type	= Angle_Type::Incidence();
	//-------------------------------------------------
	QVariant var; var.setValue(measurement);
	independent_Variables_Editor->structure_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	emit refresh();
}

void Menu::set_Wavelength_Unit()
{
	int index = sender()->property(index_Property).toInt();
	//-------------------------------------------------
	wavelength_units = wavelength_Units_List[index];
	//-------------------------------------------------
	emit refresh();	
}

void Menu::set_Length_Unit()
{
	int index = sender()->property(index_Property).toInt();
	//-------------------------------------------------
	length_units = length_Units_List[index];
	//-------------------------------------------------
	emit refresh();
}

void Menu::activate_Item_Thumbnail_Precision()
{
	// density
	if(menu_Density->title() == precision_Menu_Title)		set_Thumbnail_Density_Precision();

	// thickness
	if(menu_Lengths->title() == precision_Menu_Title)	{	set_Thumbnail_Thickness_Precision();
															set_Thumbnail_Sigma_Precision();
															set_Thumbnail_Period_Precision();
															set_Thumbnail_Gamma_Precision();}

	// composition
	if(menu_Composition->title() == precision_Menu_Title)	set_Thumbnail_Composition_Precision();

	// interlayer
	if(menu_Interlayer->title() == precision_Menu_Title)	set_Thumbnail_Interlayer_Precision();
}

void Menu::activate_Item_Line_Edit_Precision()
{
	// density
	if(menu_Density->title() == precision_Menu_Title)		set_Line_Edit_Density_Precision();

	// thickness
	if(menu_Lengths->title() == precision_Menu_Title)	{	set_Line_Edit_Thickness_Precision();
															set_Line_Edit_Sigma_Precision();
															set_Line_Edit_Period_Precision();
															set_Line_Edit_Gamma_Precision();	}

	// composition
	if(menu_Composition->title() == precision_Menu_Title)	set_Line_Edit_Composition_Precision();

	// interlayer
	if(menu_Interlayer->title() == precision_Menu_Title)	set_Line_Edit_Interlayer_Precision();
}

// precisions

void Menu::set_Thumbnail_Density_Precision()
{
	thumbnail_density_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Density_Precision()
{
	line_edit_density_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Permittivity_Precision()
{
	thumbnail_permittivity_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Permittivity_Precision()
{
	line_edit_permittivity_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Absorption_Precision()
{
	thumbnail_absorption_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Absorption_Precision()
{
	line_edit_absorption_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Composition_Precision()
{
	thumbnail_composition_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Composition_Precision()
{
	line_edit_composition_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Thickness_Precision()
{
	thumbnail_thickness_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Thickness_Precision()
{
	line_edit_thickness_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Sigma_Precision()
{
	thumbnail_sigma_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Sigma_Precision()
{
	line_edit_sigma_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Interlayer_Precision()
{
	thumbnail_interlayer_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Interlayer_Precision()
{
	line_edit_interlayer_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Period_Precision()
{
	thumbnail_period_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Period_Precision()
{
	line_edit_period_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Gamma_Precision()
{
	thumbnail_gamma_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Gamma_Precision()
{
	line_edit_gamma_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Angle_Precision()
{
	thumbnail_angle_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Angle_Precision()
{
	line_edit_angle_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Thumbnail_Wavelength_Precision()
{
	thumbnail_wavelength_precision = sender()->property(index_Property).toInt();
	emit refresh();
}

void Menu::set_Line_Edit_Wavelength_Precision()
{
	line_edit_wavelength_precision = sender()->property(index_Property).toInt();
	emit refresh();
}