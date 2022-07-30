#include "calculation_settings_editor.h"

Calculation_Settings_Editor::Calculation_Settings_Editor(QWidget* parent) :
	total_Number_of_Targets(global_Multilayer_Approach->multilayer_Tabs->count(),0),
	total_Number_of_Independents(global_Multilayer_Approach->multilayer_Tabs->count(),0),
	multilayer_Tabs(global_Multilayer_Approach->multilayer_Tabs),
	QWidget(parent)
{
	setWindowTitle("Calculation settings");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Calculation_Settings_Editor::closeEvent(QCloseEvent* event)
{
	write_Window_Geometry();
	global_Multilayer_Approach->runned_Calculation_Settings_Editor.remove(calc_Settings_Key);
	global_Multilayer_Approach->unlock_Mainwindow_Interface();
	event->accept();
}

void Calculation_Settings_Editor::set_Window_Geometry()
{
	int width_add = 0, height_add = 0;
	if(make_all_windows_resizeable && !previous_all_windows_resizeable) {width_add+=2; height_add+=2;}

	setGeometry(calculation_settings_x_corner,calculation_settings_y_corner,calculation_settings_width+width_add,calculation_settings_height+height_add);
	if(!make_all_windows_resizeable) { adjustSize(); }
}

void Calculation_Settings_Editor::write_Window_Geometry()
{
	if(!isMaximized())
	{
		calculation_settings_x_corner = frameGeometry().x()-corner_x_shift;
		calculation_settings_y_corner = frameGeometry().y()-corner_y_shift;

		calculation_settings_width  = geometry().width() +  width_add;
		calculation_settings_height = geometry().height()+ height_add;
	}
}

void Calculation_Settings_Editor::create_Main_Layout()
{
	QWidget* main_Widget = this;
	if(make_all_windows_resizeable)
	{
		QVBoxLayout* top_Layout = new QVBoxLayout(this);
			top_Layout->setMargin(0);

		main_Widget = new QWidget;
		QScrollArea* scrollArea = new QScrollArea;
			scrollArea->setWidget(main_Widget);
			scrollArea->setWidgetResizable(true);
		top_Layout->addWidget(scrollArea);
	}

	main_Layout = new QVBoxLayout(main_Widget);
		main_Layout->setSizeConstraint(QLayout::SetFixedSize);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(0,0,0,0);

	global_Multilayer_Approach->lock_Mainwindow_Interface();
    Global_Variables::create_Shortcuts(this);
	create_Tabs();
		main_Layout->addWidget(main_Tabs);
	add_Tabs();

	QHBoxLayout* button_Layout = new QHBoxLayout;
		button_Layout->setAlignment(Qt::AlignCenter);
		button_Layout->setSpacing(54);
	{
		global_Norm_Button = new QPushButton("Calculate Weights");
			global_Norm_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			global_Norm_Button->setFocus();
			global_Norm_Button->setDefault(true);
//		button_Layout->addWidget(global_Norm_Button);
	}
//	main_Layout->addLayout(button_Layout);

	// TODO
	global_Norm_Button->setDisabled(true);
	connect(main_Tabs, &QTabWidget::currentChanged,	[=]
	{
		if(tab_synchronization)
		{
																										 {global_Multilayer_Approach->                  multilayer_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key)){global_Multilayer_Approach->table_Of_Structures   ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))	 {global_Multilayer_Approach->optical_Graphs_2D	    ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))	 {global_Multilayer_Approach->optical_Graphs_1D	    ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Profile_Plots_Window.contains(profile_Plots_Key))		 {global_Multilayer_Approach->profile_Plots_Window  ->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Roughness_Plots_Window.contains(roughness_Plots_Key))	 {global_Multilayer_Approach->roughness_Plots_Window->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
			if(global_Multilayer_Approach->runned_Particles_Plots_Window.contains(particles_Plots_Key))	 {global_Multilayer_Approach->particles_Plots_Window->main_Tabs->setCurrentIndex(main_Tabs->currentIndex());}
		}
	});

//	if(make_all_windows_resizeable)
//	{
//		main_Widget->adjustSize();
//		resize(main_Widget->width()+2,main_Widget->height()+2);
//	}
}

void Calculation_Settings_Editor::create_Tabs()
{
	main_Tabs = new QTabWidget(this);
	main_Tabs->setMovable(false);

	connect(main_Tabs, &QTabWidget::currentChanged,	[=](int index)
	{
		main_Tabs->tabBar()->setTabTextColor(index,Qt::black);
		for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
		{
			if(i!=index)main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
		}
	});
}

void Calculation_Settings_Editor::add_Tabs()
{
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		QWidget* new_Widget = new QWidget;

		create_Tab_Content(new_Widget, tab_Index);
		main_Tabs->addTab(new_Widget, multilayer_Tabs->tabText(tab_Index));
	}
	for(int i = 0; i<main_Tabs->tabBar()->count(); i++)
	{
		if(i!=main_Tabs->currentIndex()) main_Tabs->tabBar()->setTabTextColor(i,Qt::gray);
	}
	main_Tabs->setCurrentIndex(global_Multilayer_Approach->multilayer_Tabs->currentIndex());
}

void Calculation_Settings_Editor::create_Tab_Content(QWidget* new_Widget, int tab_Index)
{
	QVBoxLayout* tab_Layout = new QVBoxLayout(new_Widget);
		tab_Layout->setSpacing(5);
	tab_Layout->setContentsMargins(2,2,2,2);

	QBoxLayout* top_Layout = new QVBoxLayout;
		tab_Layout->setSpacing(10);
	tab_Layout->addLayout(top_Layout);
	{
		QGroupBox* discretization_Group_Box = new QGroupBox("Profile discretization", this);
			discretization_Group_Box->setCheckable(true);
			discretization_Group_Box->setObjectName("discretization_Group_Box");
			discretization_Group_Box->setStyleSheet("QGroupBox#discretization_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
																	"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		top_Layout->addWidget(discretization_Group_Box);

		discretization_Group_Box_Vec.append(discretization_Group_Box);
		load_Discretization_Parameters(tab_Index);
		discretization_Group_Box->adjustSize();
		discretization_Group_Box->setFixedHeight(discretization_Group_Box->height());
	}
	{
		QGroupBox* roughness_Group_Box = new QGroupBox("Roughness", this);
			roughness_Group_Box->setObjectName("roughness_Group_Box_Vec");
			roughness_Group_Box->setStyleSheet("QGroupBox#roughness_Group_Box_Vec { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
																"QGroupBox::title { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		top_Layout->addWidget(roughness_Group_Box);

		roughness_Group_Box_Vec.append(roughness_Group_Box);
		load_Roughness_Parameters(tab_Index);
		roughness_Group_Box->adjustSize();
		roughness_Group_Box->setFixedHeight(roughness_Group_Box->height());
	}
	{
		QGroupBox* target_Group_Box = new QGroupBox("Measured", this);
			target_Group_Box->setCheckable(true);
			target_Group_Box->setObjectName("target_Group_Box");
			target_Group_Box->setStyleSheet("QGroupBox#target_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
													"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		tab_Layout->addWidget(target_Group_Box);

		target_Group_Box_Vec.append(target_Group_Box);
		load_Target_Parameters(tab_Index);
		target_Group_Box->adjustSize();
		target_Group_Box->setFixedHeight(target_Group_Box->height());
	}
	{
		QGroupBox* independent_Group_Box = new QGroupBox("Independent", this);
			independent_Group_Box->setCheckable(true);
			independent_Group_Box->setObjectName("independent_Group_Box");
			independent_Group_Box->setStyleSheet("QGroupBox#independent_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
															  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		tab_Layout->addWidget(independent_Group_Box);

		independent_Group_Box_Vec.append(independent_Group_Box);
		load_Independent_Parameters(tab_Index);
	}

	int num_Curves_In_Row = 1;
	for(int t_i=0; t_i<multilayer_Tabs->count(); t_i++)
	{
		Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(t_i));

		// calculate total number of targets
		total_Number_of_Targets[t_i] = 0;
		for(Target_Curve* target_Curve: multilayer->target_Profiles_Vector)	{
			if(target_Curve->loaded_And_Ready)	{
				total_Number_of_Targets[t_i]++;
			}
		}
		total_Number_of_Independents[t_i] = 0;
		for(int independent_Index=0; independent_Index<multilayer->independent_Curve_Tabs->count(); ++independent_Index)
		{
			Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(independent_Index));
			if(independent_Curve->measurement.measurement_Type != no_Measurement_Type)
			{
				total_Number_of_Independents[t_i]++;
			}
		}

		int target_Curves	   = ceil(double(total_Number_of_Targets     [t_i])/double(multilayer->num_Target_Rows     ));
		int independent_Curves = ceil(double(total_Number_of_Independents[t_i])/double(multilayer->num_Independent_Rows));
		num_Curves_In_Row = max(num_Curves_In_Row, target_Curves);
		num_Curves_In_Row = max(num_Curves_In_Row, independent_Curves);
	}
	if(num_Curves_In_Row > 1)
		top_Layout->setDirection(QBoxLayout::LeftToRight);
	else
		top_Layout->setDirection(QBoxLayout::TopToBottom);
}

void Calculation_Settings_Editor::load_Discretization_Parameters(int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	// enable/disable discretization groupbox
	discretization_Group_Box_Vec[tab_Index]->setChecked(multilayer->discretization_Parameters.enable_Discretization);
	connect(discretization_Group_Box_Vec[tab_Index],  &QGroupBox::toggled, this, [=]
	{
		bool have_Scattering = false;
		for(Target_Curve* target_Curve : multilayer->target_Profiles_Vector)
		{
			if((target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] ) && target_Curve->fit_Params.calculate)
			{
				have_Scattering = true;
			}
		}
		for(int independent_Index=0; independent_Index<multilayer->independent_Curve_Tabs->count(); ++independent_Index)
		{
			Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(independent_Index));
			if((independent_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] ) && independent_Curve->calc_Functions.check_Scattering)
			{
				have_Scattering = true;
			}
		}

		if( have_Scattering &&
		   (multilayer->imperfections_Model.approximation == DWBA_approximation ||
			multilayer->imperfections_Model.approximation == SA_approximation   ||
			multilayer->imperfections_Model.approximation == CSA_approximation  ))
		{
			QString text;
			if( multilayer->imperfections_Model.approximation == DWBA_approximation ) text = "Discretization can't be used with DWBA approximation.\nChange the approximation in Structure Table or disable scattering calculation.";
			if( multilayer->imperfections_Model.approximation == SA_approximation   ) text = "Discretization can't be used with SA approximation.\nChange the approximation in Structure Table or disable scattering calculation.";
			if( multilayer->imperfections_Model.approximation == CSA_approximation  ) text = "Discretization can't be used with CSA approximation.\nChange the approximation in Structure Table or disable scattering calculation.";

			QMessageBox::information(this,"Discretization", text);

			multilayer->discretization_Parameters.enable_Discretization = false;
			discretization_Group_Box_Vec[tab_Index]->blockSignals(true);
			discretization_Group_Box_Vec[tab_Index]->setChecked(false);
			discretization_Group_Box_Vec[tab_Index]->blockSignals(false);
		} else
		{
			multilayer->discretization_Parameters.enable_Discretization = discretization_Group_Box_Vec[tab_Index]->isChecked();

			if(global_Multilayer_Approach->runned_Profile_Plots_Window.contains(profile_Plots_Key))
			{
				Profile_Plot* profile_Plot = global_Multilayer_Approach->profile_Plots_Window->profile_Plot_Vector[tab_Index];
				profile_Plot->discretization_CheckBox->setEnabled(multilayer->discretization_Parameters.enable_Discretization);
				profile_Plot->plot_Data(true);
			}
			// threshold line only for 1D
			if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
			{
				QVector<Curve_Plot_1D*>& tab_Plots = global_Multilayer_Approach->optical_Graphs_1D->plots_1D[tab_Index];
				for(Curve_Plot_1D* curve_Plot : tab_Plots)
				{
					if( curve_Plot->measurement.measurement_Type == measurement_Types[Specular_Scan] )
					{
						curve_Plot->discretized_Threshold_Line();
					}
				}
			}
			global_Multilayer_Approach->global_Recalculate();
		}
	});

	QVBoxLayout* discretization_Layout = new QVBoxLayout(discretization_Group_Box_Vec[tab_Index]);
		discretization_Layout->setSpacing(6);
	discretization_Layout->setContentsMargins(7,12,7,7);

	{
		QLabel* step_Label = new QLabel("Discretization step");

		MyDoubleSpinBox* step_SpinBox = new MyDoubleSpinBox;
			step_SpinBox->setRange(0.01, MAX_DOUBLE);
			step_SpinBox->setDecimals(2);
			step_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			step_SpinBox->setValue(multilayer->discretization_Parameters.discretization_Step);
			step_SpinBox->setAccelerated(true);
			step_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			step_SpinBox->installEventFilter(this);
			step_SpinBox->setProperty(min_Size_Property,TABLE_FIX_WIDTH_LINE_EDIT_SHORT+1);
		Global_Variables::resize_Line_Edit(step_SpinBox, false);
		connect(step_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			multilayer->discretization_Parameters.discretization_Step = step_SpinBox->value();
			Global_Variables::resize_Line_Edit(step_SpinBox, false);

			if(global_Multilayer_Approach->runned_Profile_Plots_Window.contains(profile_Plots_Key))
			{
				Profile_Plot* profile_Plot = global_Multilayer_Approach->profile_Plots_Window->profile_Plot_Vector[tab_Index];
				profile_Plot->plot_Data(true);
			}
			// threshold line only for 1D
			if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
			{
				QVector<Curve_Plot_1D*>& tab_Plots = global_Multilayer_Approach->optical_Graphs_1D->plots_1D[tab_Index];
				for(Curve_Plot_1D* curve_Plot : tab_Plots)
				{
					if( curve_Plot->measurement.measurement_Type == measurement_Types[Specular_Scan] )
					{
						curve_Plot->discretized_Threshold_Line();
					}
				}
			}
			global_Multilayer_Approach->global_Recalculate();
		});

		QLabel* step_Units_Label = new QLabel(" "+Angstrom_Sym);

		QHBoxLayout* discr_H_Layout = new QHBoxLayout;
			discr_H_Layout->setAlignment(Qt::AlignLeft);
			discr_H_Layout->addWidget(step_Label);
			discr_H_Layout->addWidget(step_SpinBox);
			discr_H_Layout->addWidget(step_Units_Label);
		discretization_Layout->addLayout(discr_H_Layout);
	}
}

void Calculation_Settings_Editor::load_Roughness_Parameters(int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	QHBoxLayout* roughness_Layout = new QHBoxLayout(roughness_Group_Box_Vec[tab_Index]);
		roughness_Layout->setSpacing(6);
		roughness_Layout->setAlignment(Qt::AlignLeft);
	roughness_Layout->setContentsMargins(7,12,7,7);

	{
//		QVBoxLayout* radiobutton_Layout = new QVBoxLayout;
//			radiobutton_Layout->setSpacing(1);
//			radiobutton_Layout->setAlignment(Qt::AlignLeft);
//			radiobutton_Layout->setContentsMargins(0,0,0,0);
//		roughness_Layout->addLayout(radiobutton_Layout);

//		QRadioButton* DW_Radiobutton = new QRadioButton("Debye-Waller R");
//			DW_Radiobutton->setChecked(multilayer->imperfections_Model.reflectivity_With_Roughness == Debye_Waller_R);
//			DW_Radiobutton->setDisabled(true);
//		radiobutton_Layout->addWidget(DW_Radiobutton);
//		connect(DW_Radiobutton, &QRadioButton::toggled, this, [=]
//		{
//			if(DW_Radiobutton->isChecked())		{
//				multilayer->imperfections_Model.reflectivity_With_Roughness = Debye_Waller_R;
//				global_Multilayer_Approach->global_Recalculate();
//			}
//		});
//		QRadioButton* perturbative_Radiobutton = new QRadioButton("Perturbative R");
//			perturbative_Radiobutton->setChecked(multilayer->imperfections_Model.reflectivity_With_Roughness == perturbative_R);
//			perturbative_Radiobutton->setDisabled(true);
//		radiobutton_Layout->addWidget(perturbative_Radiobutton);
//		connect(perturbative_Radiobutton, &QRadioButton::toggled, this, [=]
//		{
//			if(perturbative_Radiobutton->isChecked())		{
//				multilayer->imperfections_Model.reflectivity_With_Roughness = perturbative_R;
//				global_Multilayer_Approach->global_Recalculate();
//			}
//		});
//		QButtonGroup* reflectivity_Roughness_Group = new QButtonGroup;
//			reflectivity_Roughness_Group->addButton(perturbative_Radiobutton);
//			reflectivity_Roughness_Group->addButton(DW_Radiobutton);
	}
	{
		QVBoxLayout* second_Layout = new QVBoxLayout;
			second_Layout->setSpacing(1);
			second_Layout->setAlignment(Qt::AlignLeft);
		roughness_Layout->addLayout(second_Layout);

		//-------------------------------------------------------------------------------------

		QHBoxLayout* nu_Limit_Layout = new QHBoxLayout;
		second_Layout->addLayout(nu_Limit_Layout);

		QLabel* nu_Limit_Label = new QLabel("Max spatial frequency ");
		nu_Limit_Layout->addWidget(nu_Limit_Label);

		QString units = spatial_Frequency_Units_List[inv_nm];
		double coeff = spatial_Frequency_Coefficients_Map.value(units);
		MyDoubleSpinBox* nu_Limit_Spinbox = new MyDoubleSpinBox(nullptr, false);
			nu_Limit_Spinbox->setAccelerated(true);
			nu_Limit_Spinbox->setRange(0.01, max_Frequency_For_2D_Spline/10/coeff);
			nu_Limit_Spinbox->setDecimals(2);
			nu_Limit_Spinbox->setValue(multilayer->imperfections_Model.nu_Limit/coeff);
			nu_Limit_Spinbox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
			nu_Limit_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			nu_Limit_Spinbox->setFixedWidth(40);
		nu_Limit_Layout->addWidget(nu_Limit_Spinbox);
		connect(nu_Limit_Spinbox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			multilayer->imperfections_Model.nu_Limit = nu_Limit_Spinbox->value()*coeff;
			global_Multilayer_Approach->global_Recalculate();
		});

		QLabel* nu_Limit_Units_Label = new QLabel(" "+units);
		nu_Limit_Layout->addWidget(nu_Limit_Units_Label);

		//-------------------------------------------------------------------------------------

		bool disable_DWBA_SA_CSA =	multilayer->imperfections_Model.approximation != DWBA_approximation &&
									multilayer->imperfections_Model.approximation != SA_approximation &&
									multilayer->imperfections_Model.approximation != CSA_approximation;
		QHBoxLayout* n_Max_Series_Layout = new QHBoxLayout;
		second_Layout->addLayout(n_Max_Series_Layout);

//		QLabel* n_Max_Series_Label = new QLabel("Number of terms for\nDWBA, SA, CSA scattering");
		QLabel* n_Max_Series_Label = new QLabel("Num terms for DWBA/SA/CSA ");
			n_Max_Series_Label->setDisabled(disable_DWBA_SA_CSA);
		n_Max_Series_Layout->addWidget(n_Max_Series_Label);

		QSpinBox* n_Max_Series_Spinbox = new QSpinBox;
			n_Max_Series_Spinbox->setRange(1, 150);
			n_Max_Series_Spinbox->setSingleStep(1);
			n_Max_Series_Spinbox->setValue(multilayer->imperfections_Model.DWBA_n_Max_Series);
			n_Max_Series_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			n_Max_Series_Spinbox->setFixedWidth(30);
			n_Max_Series_Spinbox->setDisabled(disable_DWBA_SA_CSA);
		n_Max_Series_Layout->addWidget(n_Max_Series_Spinbox);
		connect(n_Max_Series_Spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
		{
			multilayer->imperfections_Model.DWBA_n_Max_Series = n_Max_Series_Spinbox->value();
			global_Multilayer_Approach->global_Recalculate();
		});
	}
}

void Calculation_Settings_Editor::load_Target_Parameters(int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	// enable/disable target groupbox
	target_Group_Box_Vec[tab_Index]->setChecked(multilayer->enable_Calc_Target_Curves);

	connect(target_Group_Box_Vec[tab_Index],  &QGroupBox::toggled, this, [=]
	{
		multilayer->enable_Calc_Target_Curves = target_Group_Box_Vec[tab_Index]->isChecked();

		global_Multilayer_Approach->independent_Target_Added_1D = target_Group_Box_Vec[tab_Index]->isChecked();
		global_Multilayer_Approach->independent_Target_Added_2D = target_Group_Box_Vec[tab_Index]->isChecked();
		global_Multilayer_Approach->reopen_Optical_Graphs_1D(true, TARGET);
		global_Multilayer_Approach->reopen_Optical_Graphs_2D(true, TARGET);
		activateWindow();
	});

	QVBoxLayout* target_Vertical_Box_Layout = new QVBoxLayout(target_Group_Box_Vec[tab_Index]);
		target_Vertical_Box_Layout->setSpacing(20);
		target_Vertical_Box_Layout->setContentsMargins(7,14,7,7);

	// calculate total number of targets
	total_Number_of_Targets[tab_Index] = 0;
	QVector<Target_Curve*> targets;

	for(Target_Curve* target_Curve: multilayer->target_Profiles_Vector)
	{
		if(target_Curve->loaded_And_Ready)
		{
			targets.append(target_Curve);
			total_Number_of_Targets[tab_Index]++;
		}
	}

	int targets_in_Short_Row = total_Number_of_Targets[tab_Index]/multilayer->num_Target_Rows;
	int additional_Targets = total_Number_of_Targets[tab_Index]%multilayer->num_Target_Rows;
	int first_Long_Row_Index = multilayer->num_Target_Rows-additional_Targets;

	int current_Row = 0;
	int targets_in_Filled_Rows = 0;
	int length = -2018;

	QHBoxLayout* horizontal_Layout = new QHBoxLayout;
		horizontal_Layout->setSpacing(20);

	int target_Index = 0;
	for(Target_Curve* target_Curve: targets)
	{
		QGroupBox* box = new QGroupBox(target_Curve->label_Text);
			box->setCheckable(true);
			box->setObjectName("box");
			box->setStyleSheet("QGroupBox#box { border-radius: 2px; border: 1px solid gray; margin-top: 2ex;}"
							"QGroupBox::title { subcontrol-origin: margin;  left: 9px; padding: 0 0px 0 1px;}");

		horizontal_Layout->addWidget(box);

		// switching is here
		if(current_Row < first_Long_Row_Index) {length = targets_in_Short_Row;} else {length = targets_in_Short_Row + 1;}
		if(target_Index == targets_in_Filled_Rows+length-1)
		{
			targets_in_Filled_Rows += length;
			current_Row++;
			target_Vertical_Box_Layout->addLayout(horizontal_Layout);
			horizontal_Layout = new QHBoxLayout; // may be memory leak at the end
		}

		box->setChecked(target_Curve->fit_Params.calculate);
		connect(box, &QGroupBox::toggled, this, [=]
		{
			target_Curve->fit_Params.calculate = box->isChecked();
			if(target_Curve->measurement.measurement_Type != measurement_Types[GISAS_Map])
			{
				if((target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
					target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
					target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] ) &&
				   (multilayer->imperfections_Model.approximation == DWBA_approximation ||
					multilayer->imperfections_Model.approximation == SA_approximation   ||
					multilayer->imperfections_Model.approximation == CSA_approximation  ))
				{
					QString text;
					if( multilayer->imperfections_Model.approximation == DWBA_approximation ) text = "Discretization can't be used with DWBA approximation.\nChange the approximation in Structure Table or disable discretization.";
					if( multilayer->imperfections_Model.approximation == SA_approximation   ) text = "Discretization can't be used with SA approximation.\nChange the approximation in Structure Table or disable discretization.";
					if( multilayer->imperfections_Model.approximation == CSA_approximation  ) text = "Discretization can't be used with CSA approximation.\nChange the approximation in Structure Table or disable discretization.";

					QMessageBox::information(this,"Discretization", text);

					target_Curve->fit_Params.calculate = false;
					box->blockSignals(true);
					box->setChecked(false);
					box->blockSignals(false);
				} else
				{
					global_Multilayer_Approach->target_Added_1D = box->isChecked();
					global_Multilayer_Approach->reopen_Optical_Graphs_1D(true, TARGET);
				}
			}
			if(target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
			{
				if( //multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model ||
					multilayer->imperfections_Model.approximation == DWBA_approximation ||
					multilayer->imperfections_Model.approximation == SA_approximation ||
					multilayer->imperfections_Model.approximation == CSA_approximation )
				{
					QString text;
//					if( multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model ) text = "2D scattering can't be simulated with fractal Gauss model of roughness.\nChange the model in Structure Table.";
					if( multilayer->imperfections_Model.approximation == DWBA_approximation ) text = "2D scattering can't be simulated in DWBA approximation.\nChange the approximation in Structure Table.";
					if( multilayer->imperfections_Model.approximation == SA_approximation   ) text = "2D scattering can't be simulated in SA approximation.\nChange the approximation in Structure Table.";
					if( multilayer->imperfections_Model.approximation == CSA_approximation  ) text = "2D scattering can't be simulated in CSA approximation.\nChange the approximation in Structure Table.";

					QMessageBox::information(this,"GISAS simulation", text);
					target_Curve->fit_Params.calculate = false;
					box->blockSignals(true);
					box->setChecked(false);
					box->blockSignals(false);
				} else
				{
					global_Multilayer_Approach->target_Added_2D = box->isChecked();
					global_Multilayer_Approach->reopen_Optical_Graphs_2D(true, TARGET);
				}
			}
			activateWindow();
		});

		QVBoxLayout* box_Layout = new QVBoxLayout(box);
			box_Layout->setSpacing(5);
			box_Layout->setContentsMargins(5,10,5,5);
			box_Layout->setAlignment(Qt::AlignTop);

		// content
		{			
			QHBoxLayout* row_1_Layout = new QHBoxLayout;
			box_Layout->addLayout(row_1_Layout);

			// fit
			{
				QCheckBox* fit = new QCheckBox("Fit");
					fit->setChecked(target_Curve->fit_Params.fit);
				row_1_Layout->addWidget(fit,0,Qt::AlignLeft);
				connect(fit,  &QCheckBox::toggled, this, [=]
				{
					target_Curve->fit_Params.fit = fit->isChecked();
					if(fit->isChecked())
						fit->setStyleSheet("QCheckBox:enabled{ color: green; }QCheckBox:disabled{}");
					else
						fit->setStyleSheet("");
				});
				fit->toggled(fit->isChecked());
			}
			// mesh density & shift
			if(target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan])
			{
				QHBoxLayout* row_Right_Layout = new QHBoxLayout;
					row_Right_Layout->setAlignment(Qt::AlignRight);
				row_1_Layout->addLayout(row_Right_Layout);

				// in advance
				QLabel* shift_Label = new QLabel("Shift");
				MyDoubleSpinBox* shift_SpinBox = new MyDoubleSpinBox;

				//-------------------------------------

				QLabel* mesh_Density_Label = new QLabel("Mesh density factor");
				row_Right_Layout->addWidget(mesh_Density_Label,0,Qt::AlignRight);

				//-------------------------------------

				QSpinBox* mesh_Density_SpinBox = new QSpinBox;
					mesh_Density_SpinBox->setRange(1,9);
					mesh_Density_SpinBox->setSingleStep(1);
					mesh_Density_SpinBox->setValue(target_Curve->curve.mesh_Density_Factor);
					mesh_Density_SpinBox->setAccelerated(true);
					mesh_Density_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				Global_Variables::resize_Line_Edit(mesh_Density_SpinBox, false);
				mesh_Density_SpinBox->valueChanged(mesh_Density_SpinBox->value());
				connect(mesh_Density_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
				{
					target_Curve->curve.mesh_Density_Factor = mesh_Density_SpinBox->value();

					if(target_Curve->curve.mesh_Density_Factor == 1) {
						mesh_Density_SpinBox->setStyleSheet("QSpinBox { }");
						mesh_Density_Label  ->setStyleSheet("QLabel   { }");
						shift_SpinBox		->setStyleSheet("QSpinBox {  }");
						shift_Label			->setStyleSheet("QLabel   { }");
					} else	{
						mesh_Density_SpinBox->setStyleSheet("QSpinBox:enabled { color: blue } QCheckBox:disabled{}");
						mesh_Density_Label  ->setStyleSheet("QLabel:enabled   { color: blue } QCheckBox:disabled{}");
						shift_SpinBox		->setStyleSheet("QSpinBox:enabled { color: blue } QCheckBox:disabled{}");
						shift_Label			->setStyleSheet("QLabel:enabled   { color: blue } QCheckBox:disabled{}");
					}
				});
				mesh_Density_SpinBox->valueChanged(target_Curve->curve.mesh_Density_Factor);
				connect(mesh_Density_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
				{
					target_Curve->curve.mesh_Density_Factor = mesh_Density_SpinBox->value();

					global_Multilayer_Approach->global_Recalculate();
				});
				row_Right_Layout->addWidget(mesh_Density_SpinBox,0,Qt::AlignRight);

				// mesh shift

				row_Right_Layout->addWidget(shift_Label);

				//-------------------------------------

					shift_SpinBox->setRange(-0.9,0.9);
					shift_SpinBox->setDecimals(2);
					shift_SpinBox->setSingleStep(0.01);
					shift_SpinBox->setValue(target_Curve->curve.mesh_Density_Shift);
					shift_SpinBox->setAccelerated(true);
					shift_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				Global_Variables::resize_Line_Edit(shift_SpinBox, false);
				row_Right_Layout->addWidget(shift_SpinBox);
				connect(shift_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
				{
					target_Curve->curve.mesh_Density_Shift = shift_SpinBox->value();
					if(target_Curve->curve.mesh_Density_Factor > 1) {
						global_Multilayer_Approach->global_Recalculate();
					}
				});
			}

			QHBoxLayout* row_2_Layout = new QHBoxLayout;
			box_Layout->addLayout(row_2_Layout);

			// weght
			{
				QHBoxLayout* row_Left_Layout = new QHBoxLayout;
					row_Left_Layout->setAlignment(Qt::AlignLeft);
				row_2_Layout->addLayout(row_Left_Layout);

				QLabel* weight_Label = new QLabel("Weight");
				row_Left_Layout->addWidget(weight_Label,0,Qt::AlignLeft);

				QLineEdit* weight_Line_Edit = new QLineEdit(Locale.toString(target_Curve->fit_Params.weight));
					weight_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
					weight_Line_Edit->setProperty(min_Size_Property, 100);
				Global_Variables::resize_Line_Edit(weight_Line_Edit, false);
				row_Left_Layout->addWidget(weight_Line_Edit,0,Qt::AlignLeft);
				connect(weight_Line_Edit,  &QLineEdit::textEdited, this, [=]
				{
					target_Curve->fit_Params.weight      =      Locale.toDouble(weight_Line_Edit->text());
					target_Curve->fit_Params.weight_Sqrt = sqrt(Locale.toDouble(weight_Line_Edit->text()));
					Global_Variables::resize_Line_Edit(weight_Line_Edit, false);
				});
			}
			// adjust scale factor
			{
				QHBoxLayout* row_Right_Layout = new QHBoxLayout;
					row_Right_Layout->setAlignment(Qt::AlignRight);
				row_2_Layout->addLayout(row_Right_Layout);

				QCheckBox* adjust_Scale_Factor = new QCheckBox("Adjust scale factor");
					row_Right_Layout->addWidget(adjust_Scale_Factor,0,Qt::AlignRight);
					adjust_Scale_Factor->setChecked(target_Curve->fit_Params.adjust_Scale_Factor);
				connect(adjust_Scale_Factor,  &QCheckBox::toggled, this, [=]
				{
					target_Curve->fit_Params.adjust_Scale_Factor = adjust_Scale_Factor->isChecked();
					target_Curve->curve.val_Factor.fit.is_Fitable = target_Curve->fit_Params.adjust_Scale_Factor;
					if(target_Curve->fit_Params.adjust_Scale_Factor) {
						adjust_Scale_Factor->setStyleSheet("QCheckBox:enabled { color: red } QCheckBox:disabled{}");
					} else	{
						adjust_Scale_Factor->setStyleSheet("QCheckBox { }");
					}
				});
				adjust_Scale_Factor->toggled(adjust_Scale_Factor->isChecked());
			}

			QHBoxLayout* row_3_Layout = new QHBoxLayout;
			box_Layout->addLayout(row_3_Layout);

			// norm
			//-----------
				QCheckBox* norm_Checkbox = new QCheckBox("Divide by N");
					norm_Checkbox->setChecked(target_Curve->fit_Params.norm);
				row_3_Layout->addWidget(norm_Checkbox,0,Qt::AlignLeft);
				connect(norm_Checkbox,  &QCheckBox::toggled, this, [=]{ target_Curve->fit_Params.norm = norm_Checkbox->isChecked(); });

				QCheckBox* maximize_Integral_Checkbox = new QCheckBox("Maximize integral");
					maximize_Integral_Checkbox->setChecked(target_Curve->fit_Params.maximize_Integral);
				max_Integral_Map.insert(target_Curve, maximize_Integral_Checkbox);
				if(	target_Curve->measurement.measurement_Type != measurement_Types[Specular_Scan])
				{
					maximize_Integral_Checkbox->setDisabled(true);
				}

				row_3_Layout->addWidget(maximize_Integral_Checkbox,0,Qt::AlignRight);
			//-----------

			QHBoxLayout* row_4_Layout = new QHBoxLayout;
			box_Layout->addLayout(row_4_Layout);

			// function
			{
				QHBoxLayout* row_Left_Layout = new QHBoxLayout;
					row_Left_Layout->setAlignment(Qt::AlignLeft);
				row_4_Layout->addLayout(row_Left_Layout);

				QLabel* function_Label = new QLabel("Function:");
				row_Left_Layout->addWidget(function_Label,0,Qt::AlignLeft);

				// -------------------------------------------------------

				QLineEdit* fit_Function_Line_Edit = new QLineEdit(target_Curve->fit_Params.fit_Function);
				Global_Variables::resize_Line_Edit(fit_Function_Line_Edit, false);
				row_Left_Layout->addWidget(fit_Function_Line_Edit,0,Qt::AlignLeft);

				connect(fit_Function_Line_Edit,  &QLineEdit::textEdited, this, [=]
				{
					QStringList var_List = {fit_Function_Variable};
					if(Global_Variables::expression_Is_Valid(fit_Function_Line_Edit->text(), var_List) || fit_Function_Line_Edit->text().isEmpty())
					{
						target_Curve->fit_Params.fit_Function = fit_Function_Line_Edit->text();
					}
					Global_Variables::resize_Line_Edit(fit_Function_Line_Edit, false);
				});
				connect(fit_Function_Line_Edit,  &QLineEdit::editingFinished, this, [=]
				{
					QStringList var_List = {fit_Function_Variable};
					if(Global_Variables::expression_Is_Valid(fit_Function_Line_Edit->text(), var_List) || fit_Function_Line_Edit->text().isEmpty())
					{
						target_Curve->fit_Params.fit_Function = fit_Function_Line_Edit->text();
					} else
					{
						fit_Function_Line_Edit->setText(target_Curve->fit_Params.fit_Function);
						QMessageBox::information(this, "Wrong expression", "Expression has wrong syntax");
					}
					Global_Variables::resize_Line_Edit(fit_Function_Line_Edit, false);
				});

				// -------------------------------------------------------

				QHBoxLayout* row_Right_Layout = new QHBoxLayout;
					row_Right_Layout->setAlignment(Qt::AlignRight);
				row_4_Layout->addLayout(row_Right_Layout);

				QLabel* power_Label = new QLabel("Power:");
				row_Right_Layout->addWidget(power_Label,0,Qt::AlignRight);

				// -------------------------------------------------------

				MyDoubleSpinBox* power_SpinBox = new MyDoubleSpinBox;
					power_SpinBox->setRange(0,42);
					power_SpinBox->setDecimals(1);
					power_SpinBox->setSingleStep(1);
					power_SpinBox->setValue(target_Curve->fit_Params.power);
					power_SpinBox->setAccelerated(true);
					power_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				row_Right_Layout->addWidget(power_SpinBox,0,Qt::AlignRight);
				Global_Variables::resize_Line_Edit(power_SpinBox, false);
				connect(power_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
				{
					target_Curve->fit_Params.power = power_SpinBox->value();
				});

				// -------------------------------------------------------

				QCheckBox* chi2_CheckBox = new QCheckBox("Use "+Chi2_Sym);
					chi2_CheckBox->setChecked(target_Curve->fit_Params.use_Chi2);
				row_Right_Layout->addWidget(chi2_CheckBox,0,Qt::AlignRight);
				connect(chi2_CheckBox,  &QCheckBox::toggled, this, [=]
				{
					power_Label           ->setEnabled(!chi2_CheckBox->isChecked() && !maximize_Integral_Checkbox->isChecked());
					power_SpinBox         ->setEnabled(!chi2_CheckBox->isChecked() && !maximize_Integral_Checkbox->isChecked());
					norm_Checkbox         ->setEnabled(!chi2_CheckBox->isChecked() && !maximize_Integral_Checkbox->isChecked());

					function_Label        ->setDisabled(chi2_CheckBox->isChecked() && !target_Curve->fit_Params.maximize_Integral);
					fit_Function_Line_Edit->setDisabled(chi2_CheckBox->isChecked() && !target_Curve->fit_Params.maximize_Integral);

					target_Curve->fit_Params.use_Chi2 = chi2_CheckBox->isChecked();
				});

				// -------------------------------------------------------

				connect(maximize_Integral_Checkbox,  &QCheckBox::toggled, this, [=]
				{
					if(	target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan])
					{
						if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
						{
							target_Curve->fit_Params.maximize_Integral = maximize_Integral_Checkbox->isChecked();
						} else
						{
							maximize_Integral_Checkbox->blockSignals(true);
							maximize_Integral_Checkbox->setChecked(false);
							target_Curve->fit_Params.maximize_Integral = false;
							maximize_Integral_Checkbox->blockSignals(false);
							QMessageBox::information(this,"Wrong curve type", "Only spectral curve\nshoud be maximized");
						}
						chi2_CheckBox->setEnabled(!maximize_Integral_Checkbox->isChecked());

						power_Label  ->setEnabled(!maximize_Integral_Checkbox->isChecked() && !chi2_CheckBox->isChecked());
						power_SpinBox->setEnabled(!maximize_Integral_Checkbox->isChecked() && !chi2_CheckBox->isChecked());
						norm_Checkbox->setEnabled(!maximize_Integral_Checkbox->isChecked() && !chi2_CheckBox->isChecked());

						function_Label        ->setEnabled(maximize_Integral_Checkbox->isChecked() || !target_Curve->fit_Params.use_Chi2);
						fit_Function_Line_Edit->setEnabled(maximize_Integral_Checkbox->isChecked() || !target_Curve->fit_Params.use_Chi2);
					}
				});

				// -------------------------------------------------------
				// initialize
				chi2_CheckBox->toggled(chi2_CheckBox->isChecked());
				if(target_Curve->fit_Params.maximize_Integral)
				{maximize_Integral_Checkbox->toggled(maximize_Integral_Checkbox->isChecked());}
			}			

			// scattering: smoothing and specular peak
			if(	target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] ||
				target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
			{
				QHBoxLayout* row_5_Layout = new QHBoxLayout;
				box_Layout->addLayout(row_5_Layout);

				// -------------------------------------------------------

				QHBoxLayout* row_Left_Layout = new QHBoxLayout;
					row_Left_Layout->setAlignment(Qt::AlignLeft);
				row_5_Layout->addLayout(row_Left_Layout);

				QCheckBox* use_Instrumental_Smoothing = new QCheckBox("Instrumental smoothing");
					use_Instrumental_Smoothing->setChecked(target_Curve->calc_Functions.instrumental_Smoothing);
				row_Left_Layout->addWidget(use_Instrumental_Smoothing,0,Qt::AlignLeft);
				connect(use_Instrumental_Smoothing,&QCheckBox::toggled, this, [=]
				{
					target_Curve->calc_Functions.instrumental_Smoothing = use_Instrumental_Smoothing->isChecked();
					global_Multilayer_Approach->global_Recalculate();
				});

				// -------------------------------------------------------

				QHBoxLayout* row_Right_Layout = new QHBoxLayout;
					row_Right_Layout->setAlignment(Qt::AlignRight);
				row_5_Layout->addLayout(row_Right_Layout);

				QCheckBox* add_Specular_Peak = new QCheckBox("Add specular peak");
					add_Specular_Peak->setChecked(target_Curve->calc_Functions.add_Specular_Peak);
				row_Right_Layout->addWidget(add_Specular_Peak,0,Qt::AlignRight);
				connect(add_Specular_Peak,&QCheckBox::toggled, this, [=]
				{
					target_Curve->calc_Functions.add_Specular_Peak = add_Specular_Peak->isChecked();
					global_Multilayer_Approach->global_Recalculate();
				});

				// -------------------------------------------------------

				QHBoxLayout* row_6_Layout = new QHBoxLayout;
				box_Layout->addLayout(row_6_Layout);

				// -------------------------------------------------------

				QHBoxLayout* row_6_Left_Layout = new QHBoxLayout;
					row_6_Left_Layout->setAlignment(Qt::AlignLeft);
				row_6_Layout->addLayout(row_6_Left_Layout);

				QCheckBox* integrate_PSD_in_Detector_Checkbox = new QCheckBox("Integrate PSD in detector");
					integrate_PSD_in_Detector_Checkbox->setChecked(target_Curve->calc_Functions.integrate_PSD_in_Detector);
				row_6_Left_Layout->addWidget(integrate_PSD_in_Detector_Checkbox,0,Qt::AlignLeft);
				connect(integrate_PSD_in_Detector_Checkbox,&QCheckBox::toggled, this, [=]
				{
					target_Curve->calc_Functions.integrate_PSD_in_Detector = integrate_PSD_in_Detector_Checkbox->isChecked();
					global_Multilayer_Approach->global_Recalculate();
				});
			}
		}
		target_Index++;
	}
	if(!horizontal_Layout->parent()) delete horizontal_Layout;
}

void Calculation_Settings_Editor::load_Independent_Parameters(int tab_Index)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));

	independent_Group_Box_Vec[tab_Index]->setChecked(multilayer->enable_Calc_Independent_Curves);

	connect(independent_Group_Box_Vec[tab_Index],  &QGroupBox::toggled, this, [=]
	{
		multilayer->enable_Calc_Independent_Curves = independent_Group_Box_Vec[tab_Index]->isChecked();
		global_Multilayer_Approach->independent_Target_Added_1D = independent_Group_Box_Vec[tab_Index]->isChecked();
		global_Multilayer_Approach->independent_Target_Added_2D = independent_Group_Box_Vec[tab_Index]->isChecked();
		global_Multilayer_Approach->reopen_Optical_Graphs_1D(true, INDEPENDENT);
		global_Multilayer_Approach->reopen_Optical_Graphs_2D(true, INDEPENDENT);
		activateWindow();
	});

	QVBoxLayout* independent_Vertical_Box_Layout = new QVBoxLayout(independent_Group_Box_Vec[tab_Index]);
		independent_Vertical_Box_Layout->setSpacing(20);
		independent_Vertical_Box_Layout->setContentsMargins(7,14,7,7);

	// calculate total number of independents
	QVector<Independent_Curve*> independents;
	total_Number_of_Independents[tab_Index] = 0;
	for(int independent_Index=0; independent_Index<multilayer->independent_Curve_Tabs->count(); ++independent_Index)
	{
		Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(independent_Index));
		if(independent_Curve->measurement.measurement_Type != no_Measurement_Type)
		{
			independents.append(independent_Curve);
			total_Number_of_Independents[tab_Index]++;
		}
	}

	int independents_in_Short_Row = total_Number_of_Independents[tab_Index]/multilayer->num_Independent_Rows;
	int additional_Independents = total_Number_of_Independents[tab_Index]%multilayer->num_Independent_Rows;
	int first_Long_Row_Index = multilayer->num_Independent_Rows-additional_Independents;

	int current_Row = 0;
	int independents_in_Filled_Rows = 0;
	int length = -2018;

	QHBoxLayout* horizontal_Layout = new QHBoxLayout;

	int independent_Index = 0;
	for(Independent_Curve* independent_Curve : independents)
	{
		QGroupBox* box = new QGroupBox(independent_Curve->name);
			box->setCheckable(true);
			box->setObjectName("box");
			box->setStyleSheet("QGroupBox#box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
						  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");

		horizontal_Layout->addWidget(box);
			horizontal_Layout->setSpacing(20);

		// switching is here
		if(current_Row < first_Long_Row_Index) length = independents_in_Short_Row; else length = independents_in_Short_Row + 1;

		if(independent_Index == independents_in_Filled_Rows+length-1)
		{
			independents_in_Filled_Rows += length;
			current_Row++;
			independent_Vertical_Box_Layout->addLayout(horizontal_Layout);
			horizontal_Layout = new QHBoxLayout; // may be memory leak at the end
		}

		box->setChecked(independent_Curve->calc_Functions.check_Enabled);
		connect(box,  &QGroupBox::toggled, this, [=]
		{
			refresh_Independent_Calc_Properties(tab_Index, independent_Index, box);

			if(independent_Curve->calc_Functions.if_Something_Enabled_1D())
			{
				global_Multilayer_Approach->independent_Added_1D = box->isChecked();
				global_Multilayer_Approach->reopen_Optical_Graphs_1D(true, INDEPENDENT);
			}
			if(independent_Curve->calc_Functions.if_Something_Enabled_2D())
			{
				global_Multilayer_Approach->independent_Added_2D = box->isChecked();
				global_Multilayer_Approach->reopen_Optical_Graphs_2D(true, INDEPENDENT);
			}
			activateWindow();
		});

		QVBoxLayout* box_Layout = new QVBoxLayout(box);
			box_Layout->setAlignment(Qt::AlignTop);
			box_Layout->setSpacing(2);
			box_Layout->setContentsMargins(5,10,5,5);
		// content
		{
			// standard functions
			{
				QHBoxLayout* standard_Functions_Layout = new QHBoxLayout;
					standard_Functions_Layout->setAlignment(Qt::AlignLeft);
				box_Layout->addLayout(standard_Functions_Layout);

				if(	independent_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] )
				{
					QCheckBox* reflect_Functions = new QCheckBox(reflectance_Function);
						reflect_Functions->setChecked(independent_Curve->calc_Functions.check_Reflectance);
					standard_Functions_Layout->addWidget(reflect_Functions);
					connect(reflect_Functions,  &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });

					QCheckBox* transmit_Functions = new QCheckBox(transmittance_Function);
						transmit_Functions->setChecked(independent_Curve->calc_Functions.check_Transmittance);
					standard_Functions_Layout->addWidget(transmit_Functions);
					connect(transmit_Functions, &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });

					QCheckBox* absorp_Functions = new QCheckBox(absorptance_Function);
						absorp_Functions->setChecked(independent_Curve->calc_Functions.check_Absorptance);
					standard_Functions_Layout->addWidget(absorp_Functions);
					connect(absorp_Functions,   &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
				}
				if(	independent_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
					independent_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
					independent_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan]   )
				{
					QLabel* scattering_Functions = new QLabel(QString(scattering_Function) + " (" + independent_Curve->measurement.measurement_Type + ")");
					standard_Functions_Layout->addWidget(scattering_Functions);

//					QCheckBox* scattering_Functions = new QCheckBox(QString(scattering_Function) + " (" + independent_Curve->measurement.measurement_Type + ")");
//						scattering_Functions->setChecked(independent_Curve->calc_Functions.check_Scattering);
//					standard_Functions_Layout->addWidget(scattering_Functions);
//					connect(scattering_Functions,&QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
				}
				if(	independent_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
				{
					QLabel* gisas_Functions = new QLabel(gisas_Function);
					standard_Functions_Layout->addWidget(gisas_Functions);

//					QCheckBox* gisas_Functions = new QCheckBox(gisas_Function);
//						gisas_Functions->setChecked(independent_Curve->calc_Functions.check_GISAS);
//					standard_Functions_Layout->addWidget(gisas_Functions);
//					connect(gisas_Functions, &QCheckBox::toggled, this, [=]{ refresh_Independent_Calc_Properties(tab_Index, independent_Index, box); });
				}
			}

			// field functions
			if(	independent_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] )
			{
				QGroupBox* field_Functions_Group_Box = new QGroupBox;
					field_Functions_Group_Box->setObjectName("field_Functions_Group_Box");
					field_Functions_Group_Box->setStyleSheet("QGroupBox#field_Functions_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 1ex;}"
															 "QGroupBox::title    { subcontrol-origin: margin;	 left: 9px; padding: 0 0px 0 1px;}");
				box_Layout->addWidget(field_Functions_Group_Box);

				QVBoxLayout* field_Functions_Layout = new QVBoxLayout(field_Functions_Group_Box);
					field_Functions_Layout->setSpacing(3);
				QHBoxLayout* field_Checkbox_Layout = new QHBoxLayout;
					field_Checkbox_Layout->setAlignment(Qt::AlignLeft);
				field_Functions_Layout->addLayout(field_Checkbox_Layout);

				QCheckBox* field_Intensity = new QCheckBox(intensity_Function);
					field_Intensity->setChecked(independent_Curve->calc_Functions.check_Field);
				field_Checkbox_Layout->addWidget(field_Intensity);

				QCheckBox* joule_Absorption = new QCheckBox(joule_Function);
					joule_Absorption->setChecked(independent_Curve->calc_Functions.check_Joule);
				field_Checkbox_Layout->addWidget(joule_Absorption);


				// in-depth step
				QList<QWidget*> step_Widgets_List;
				double depth_Coeff = length_Coefficients_Map.value(independent_Curve->calc_Functions.depth_Units);

				// ---------------------------------------------------------------------------------
				QWidget* field_Depth_Widget = new QWidget;
					field_Depth_Widget->setContentsMargins(0,0,0,0);
					field_Functions_Layout->addWidget(field_Depth_Widget);
				QGridLayout* field_Depth_Layout = new QGridLayout(field_Depth_Widget);
					field_Depth_Layout->setContentsMargins(0,0,0,0);
					field_Depth_Layout->setSpacing(3);
					field_Depth_Layout->setAlignment(Qt::AlignLeft);

//				QGridLayout* field_Depth_Layout = new QGridLayout;
//					field_Depth_Layout->setAlignment(Qt::AlignLeft);
//				field_Functions_Layout->addLayout(field_Depth_Layout);

				QLabel* field_Step_Label = new QLabel("Z-spacing");
					field_Depth_Layout->addWidget(field_Step_Label,0,0);
					step_Widgets_List.append(field_Step_Label);
				MyDoubleSpinBox* field_Step_SpinBox = new MyDoubleSpinBox;
					field_Step_SpinBox->setAccelerated(true);
					field_Step_SpinBox->setRange(0.1, MAX_DOUBLE);
					field_Step_SpinBox->setDecimals(2);
					field_Step_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
					field_Step_SpinBox->setValue(independent_Curve->calc_Functions.field_Step/depth_Coeff);
					field_Step_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				field_Depth_Layout->addWidget(field_Step_SpinBox,0,1);
				step_Widgets_List.append(field_Step_SpinBox);
				Global_Variables::resize_Line_Edit(field_Step_SpinBox, false);
				connect(field_Step_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
				{
					double depth_Coeff = length_Coefficients_Map.value(independent_Curve->calc_Functions.depth_Units);
					independent_Curve->calc_Functions.field_Step = field_Step_SpinBox->value()*depth_Coeff;
					global_Multilayer_Approach->global_Recalculate();
				});
				QComboBox* field_Step_Units_Combobox = new QComboBox;
					field_Step_Units_Combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
					field_Step_Units_Combobox->addItems(length_Units_List);
					field_Step_Units_Combobox->setCurrentText(independent_Curve->calc_Functions.depth_Units);
				field_Depth_Layout->addWidget(field_Step_Units_Combobox,0,2);
				step_Widgets_List.append(field_Step_Units_Combobox);


				QCheckBox* show_Surface_Checkbox = new QCheckBox("Show surface");
					show_Surface_Checkbox->setChecked(independent_Curve->calc_Functions.show_Surface);
					step_Widgets_List.append(show_Surface_Checkbox);
				field_Depth_Layout->addWidget(show_Surface_Checkbox,1,0,1,3);
				connect(show_Surface_Checkbox,  &QCheckBox::toggled, this, [=]
				{
					independent_Curve->calc_Functions.show_Surface = show_Surface_Checkbox->isChecked();
					// refresh graph
					if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
					{
						if(global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.contains(independent_Curve->measurement.id))
						{
							Curve_Plot_2D* curve_Plot_2D = global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.value(independent_Curve->measurement.id);
							curve_Plot_2D->create_Position_Lines();
							curve_Plot_2D->replot_All();
						}
					}
				});
				QCheckBox* show_Substrate_Checkbox = new QCheckBox("Show substrate");
					show_Substrate_Checkbox->setChecked(independent_Curve->calc_Functions.show_Substrate);
					step_Widgets_List.append(show_Substrate_Checkbox);
				field_Depth_Layout->addWidget(show_Substrate_Checkbox,2,0,1,3);
				connect(show_Substrate_Checkbox,  &QCheckBox::toggled, this, [=]
				{
					independent_Curve->calc_Functions.show_Substrate = show_Substrate_Checkbox->isChecked();
					// refresh graph
					if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
					{
						if(global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.contains(independent_Curve->measurement.id))
						{
							Curve_Plot_2D* curve_Plot_2D = global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.value(independent_Curve->measurement.id);
							curve_Plot_2D->create_Position_Lines();
							curve_Plot_2D->replot_All();
						}
					}
				});
				// ---------------------------------------------------------------------------------

				QGridLayout* field_Ambient_Substrate_Layout = new QGridLayout;
				field_Functions_Layout->addLayout(field_Ambient_Substrate_Layout);

				// ambient
				QLabel* field_Ambient_Label = new QLabel("      Calculation depth into ambient");
					field_Depth_Layout->addWidget(field_Ambient_Label,0,4);
					step_Widgets_List.append(field_Ambient_Label);
				MyDoubleSpinBox* field_Ambient_SpinBox = new MyDoubleSpinBox;
					field_Ambient_SpinBox->setAccelerated(true);
					field_Ambient_SpinBox->setRange(0, MAX_DOUBLE);
					field_Ambient_SpinBox->setDecimals(2);
					field_Ambient_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
					field_Ambient_SpinBox->setValue(independent_Curve->calc_Functions.field_Ambient_Distance/depth_Coeff);
					field_Ambient_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
				field_Depth_Layout->addWidget(field_Ambient_SpinBox,0,5);
				step_Widgets_List.append(field_Ambient_SpinBox);
				Global_Variables::resize_Line_Edit(field_Ambient_SpinBox, false);
				connect(field_Ambient_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
				{
					double depth_Coeff = length_Coefficients_Map.value(independent_Curve->calc_Functions.depth_Units);
					independent_Curve->calc_Functions.field_Ambient_Distance = field_Ambient_SpinBox->value()*depth_Coeff;
					global_Multilayer_Approach->global_Recalculate();
				});
				QLabel* field_Ambient_Units_Label = new QLabel(independent_Curve->calc_Functions.depth_Units);
					field_Depth_Layout->addWidget(field_Ambient_Units_Label,0,6);
					step_Widgets_List.append(field_Ambient_Units_Label);

				// surface
				QRadioButton* field_Surface_RadioButton = new QRadioButton("Calculation depth from surface");
					field_Depth_Layout->addWidget(field_Surface_RadioButton,1,4);
					field_Surface_RadioButton->setChecked(independent_Curve->calc_Functions.use_Surface_Distance);
				step_Widgets_List.append(field_Surface_RadioButton);
				MyDoubleSpinBox* field_Surface_SpinBox = new MyDoubleSpinBox;
					field_Surface_SpinBox->setAccelerated(true);
					field_Surface_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
					field_Surface_SpinBox->setDecimals(2);
					field_Surface_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
					field_Surface_SpinBox->setValue(independent_Curve->calc_Functions.field_Surface_Distance/depth_Coeff);
					field_Surface_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
					field_Surface_SpinBox->setEnabled(independent_Curve->calc_Functions.use_Surface_Distance);
				field_Depth_Layout->addWidget(field_Surface_SpinBox,1,5);
//				step_Widgets_List.append(field_Surface_SpinBox);
				Global_Variables::resize_Line_Edit(field_Surface_SpinBox, false);
				connect(field_Surface_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
				{
					double depth_Coeff = length_Coefficients_Map.value(independent_Curve->calc_Functions.depth_Units);
					independent_Curve->calc_Functions.field_Surface_Distance = field_Surface_SpinBox->value()*depth_Coeff;
					global_Multilayer_Approach->global_Recalculate();
				});
				QLabel* field_Surface_Units_Label = new QLabel(independent_Curve->calc_Functions.depth_Units);
					field_Surface_Units_Label->setEnabled(independent_Curve->calc_Functions.use_Surface_Distance);
					field_Depth_Layout->addWidget(field_Surface_Units_Label,1,6);
//					step_Widgets_List.append(field_Surface_Units_Label);

				// substrate
				QRadioButton* field_Substrate_RadioButton = new QRadioButton("Calculation depth into substrate");
					field_Depth_Layout->addWidget(field_Substrate_RadioButton,2,4);
					field_Substrate_RadioButton->setChecked(!independent_Curve->calc_Functions.use_Surface_Distance);
				step_Widgets_List.append(field_Substrate_RadioButton);
				MyDoubleSpinBox* field_Substrate_SpinBox = new MyDoubleSpinBox;
					field_Substrate_SpinBox->setAccelerated(true);
					field_Substrate_SpinBox->setRange(-MAX_DOUBLE, MAX_DOUBLE);
					field_Substrate_SpinBox->setDecimals(2);
					field_Substrate_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
					field_Substrate_SpinBox->setValue(independent_Curve->calc_Functions.field_Substrate_Distance/depth_Coeff);
					field_Substrate_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
					field_Substrate_SpinBox->setEnabled(!independent_Curve->calc_Functions.use_Surface_Distance);
				field_Depth_Layout->addWidget(field_Substrate_SpinBox,2,5);
//				step_Widgets_List.append(field_Substrate_SpinBox);
				Global_Variables::resize_Line_Edit(field_Substrate_SpinBox, false);
				connect(field_Substrate_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
				{
					double depth_Coeff = length_Coefficients_Map.value(independent_Curve->calc_Functions.depth_Units);
					independent_Curve->calc_Functions.field_Substrate_Distance = field_Substrate_SpinBox->value()*depth_Coeff;
					global_Multilayer_Approach->global_Recalculate();
				});
				QLabel* field_Substrate_Units_Label = new QLabel(independent_Curve->calc_Functions.depth_Units);
					field_Substrate_Units_Label->setEnabled(!independent_Curve->calc_Functions.use_Surface_Distance);
					field_Depth_Layout->addWidget(field_Substrate_Units_Label,2,6);
//					step_Widgets_List.append(field_Substrate_Units_Label);

				QButtonGroup* field_Group = new QButtonGroup;
					field_Group->addButton(field_Surface_RadioButton);
					field_Group->addButton(field_Substrate_RadioButton);

				connect(field_Step_Units_Combobox, &QComboBox::currentTextChanged, this, [=]
				{
					independent_Curve->calc_Functions.depth_Units = field_Step_Units_Combobox->currentText();

					double depth_Coeff = length_Coefficients_Map.value(independent_Curve->calc_Functions.depth_Units);

					field_Step_SpinBox->blockSignals(true);
					field_Step_SpinBox->setValue(independent_Curve->calc_Functions.field_Step/depth_Coeff);
					Global_Variables::resize_Line_Edit(field_Step_SpinBox, false);
					field_Step_SpinBox->blockSignals(false);


					field_Ambient_Units_Label->setText(independent_Curve->calc_Functions.depth_Units);
					field_Ambient_SpinBox->blockSignals(true);
					field_Ambient_SpinBox->setValue(independent_Curve->calc_Functions.field_Ambient_Distance/depth_Coeff);
					Global_Variables::resize_Line_Edit(field_Ambient_SpinBox, false);
					field_Ambient_SpinBox->blockSignals(false);

					field_Surface_Units_Label->setText(independent_Curve->calc_Functions.depth_Units);
					field_Surface_SpinBox->blockSignals(true);
					field_Surface_SpinBox->setValue(independent_Curve->calc_Functions.field_Surface_Distance/depth_Coeff);
					Global_Variables::resize_Line_Edit(field_Surface_SpinBox, false);
					field_Surface_SpinBox->blockSignals(false);

					field_Substrate_Units_Label->setText(independent_Curve->calc_Functions.depth_Units);
					field_Substrate_SpinBox->blockSignals(true);
					field_Substrate_SpinBox->setValue(independent_Curve->calc_Functions.field_Substrate_Distance/depth_Coeff);
					Global_Variables::resize_Line_Edit(field_Substrate_SpinBox, false);
					field_Substrate_SpinBox->blockSignals(false);

					// curve plots
					if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
					{
						if(global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.contains(independent_Curve->measurement.id))
						{
							Curve_Plot_2D* curve_Plot_2D = global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.value(independent_Curve->measurement.id);
							curve_Plot_2D->refresh_Axes_Labels();
							curve_Plot_2D->create_Position_Lines();
							curve_Plot_2D->plot_Data();
						}
					}
				});

				connect(field_Surface_RadioButton, &QRadioButton::toggled, this, [=]
				{
					independent_Curve->calc_Functions.use_Surface_Distance = field_Surface_RadioButton->isChecked();
					field_Surface_SpinBox->setEnabled(independent_Curve->calc_Functions.use_Surface_Distance);
					field_Surface_Units_Label->setEnabled(independent_Curve->calc_Functions.use_Surface_Distance);
					field_Substrate_SpinBox->setDisabled(independent_Curve->calc_Functions.use_Surface_Distance);
					field_Substrate_Units_Label->setDisabled(independent_Curve->calc_Functions.use_Surface_Distance);

					global_Multilayer_Approach->global_Recalculate();
				});
				connect(field_Substrate_RadioButton, &QRadioButton::toggled, this, [=]
				{
					independent_Curve->calc_Functions.use_Surface_Distance = !field_Substrate_RadioButton->isChecked();
					field_Surface_SpinBox->setEnabled(independent_Curve->calc_Functions.use_Surface_Distance);
					field_Surface_Units_Label->setEnabled(independent_Curve->calc_Functions.use_Surface_Distance);
					field_Substrate_SpinBox->setDisabled(independent_Curve->calc_Functions.use_Surface_Distance);
					field_Substrate_Units_Label->setDisabled(independent_Curve->calc_Functions.use_Surface_Distance);

					global_Multilayer_Approach->global_Recalculate();
				});
				// ---------------------------------------------------------------------------------

				connect(field_Intensity,  &QCheckBox::toggled, this, [=]
				{
					if(field_Intensity->isChecked() || joule_Absorption->isChecked())
						field_Depth_Widget->show();
					else
						field_Depth_Widget->hide();

//					for(QWidget* step_Widget : step_Widgets_List)
//					{
//						step_Widget->setEnabled(field_Intensity->isChecked() || joule_Absorption->isChecked());
//					}
//					bool yes1 =  independent_Curve->calc_Functions.use_Surface_Distance && (field_Intensity->isChecked() || joule_Absorption->isChecked());
//					bool yes2 = !independent_Curve->calc_Functions.use_Surface_Distance && (field_Intensity->isChecked() || joule_Absorption->isChecked());
//					field_Surface_SpinBox->setEnabled(yes1);
//					field_Surface_Units_Label->setEnabled(yes1);
//					field_Substrate_SpinBox->setEnabled(yes2);
//					field_Substrate_Units_Label->setEnabled(yes2);

					refresh_Independent_Calc_Properties(tab_Index, independent_Index, box);
				});
				connect(joule_Absorption,  &QCheckBox::toggled, this, [=]
				{
					if(field_Intensity->isChecked() || joule_Absorption->isChecked())
						field_Depth_Widget->show();
					else
						field_Depth_Widget->hide();

//					for(QWidget* step_Widget : step_Widgets_List)
//					{
//						step_Widget->setEnabled(field_Intensity->isChecked() || joule_Absorption->isChecked());
//					}
//					bool yes1 =  independent_Curve->calc_Functions.use_Surface_Distance && (field_Intensity->isChecked() || joule_Absorption->isChecked());
//					bool yes2 = !independent_Curve->calc_Functions.use_Surface_Distance && (field_Intensity->isChecked() || joule_Absorption->isChecked());
//					field_Surface_SpinBox->setEnabled(yes1);
//					field_Surface_Units_Label->setEnabled(yes1);
//					field_Substrate_SpinBox->setEnabled(yes2);
//					field_Substrate_Units_Label->setEnabled(yes2);

					refresh_Independent_Calc_Properties(tab_Index, independent_Index, box);
				});

				// initialization
				if(field_Intensity->isChecked() || joule_Absorption->isChecked())
					field_Depth_Widget->show();
				else
					field_Depth_Widget->hide();

//				for(QWidget* step_Widget : step_Widgets_List)
//				{
//					step_Widget->setEnabled(field_Intensity->isChecked() || joule_Absorption->isChecked());
//				}
//				bool yes1 =  independent_Curve->calc_Functions.use_Surface_Distance && (field_Intensity->isChecked() || joule_Absorption->isChecked());
//				bool yes2 = !independent_Curve->calc_Functions.use_Surface_Distance && (field_Intensity->isChecked() || joule_Absorption->isChecked());
//				field_Surface_SpinBox->setEnabled(yes1);
//				field_Surface_Units_Label->setEnabled(yes1);
//				field_Substrate_SpinBox->setEnabled(yes2);
//				field_Substrate_Units_Label->setEnabled(yes2);
			}

			// instrumental smoothing for scattering
			if(	independent_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan]   ||
				independent_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
			{
				QVBoxLayout* additional_Layout = new QVBoxLayout;
					additional_Layout->setAlignment(Qt::AlignLeft);
					additional_Layout->setContentsMargins(10,0,0,0);
				box_Layout->addLayout(additional_Layout);

				QCheckBox* use_Instrumental_Smoothing = new QCheckBox("Apply instrumental smoothing");
					use_Instrumental_Smoothing->setChecked(independent_Curve->calc_Functions.instrumental_Smoothing);
				additional_Layout->addWidget(use_Instrumental_Smoothing);
				connect(use_Instrumental_Smoothing,&QCheckBox::toggled, this, [=]
				{
					independent_Curve->calc_Functions.instrumental_Smoothing = use_Instrumental_Smoothing->isChecked();
					global_Multilayer_Approach->global_Recalculate();
				});


				QCheckBox* integrate_PSD_in_Detector_Checkbox = new QCheckBox("Integrate PSD in detector");
					integrate_PSD_in_Detector_Checkbox->setChecked(independent_Curve->calc_Functions.integrate_PSD_in_Detector);
				additional_Layout->addWidget(integrate_PSD_in_Detector_Checkbox);
				connect(integrate_PSD_in_Detector_Checkbox,&QCheckBox::toggled, this, [=]
				{
					independent_Curve->calc_Functions.integrate_PSD_in_Detector = integrate_PSD_in_Detector_Checkbox->isChecked();
					global_Multilayer_Approach->global_Recalculate();
				});
			}

			// specular peak
			if(	independent_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan]   ||
				independent_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
			{
				QHBoxLayout* additional_Layout = new QHBoxLayout;
					additional_Layout->setAlignment(Qt::AlignLeft);
					additional_Layout->setContentsMargins(10,0,0,0);
				box_Layout->addLayout(additional_Layout);

				QCheckBox* add_Specular_Peak = new QCheckBox("Add specular peak");
					add_Specular_Peak->setChecked(independent_Curve->calc_Functions.add_Specular_Peak);
				additional_Layout->addWidget(add_Specular_Peak);
				connect(add_Specular_Peak,&QCheckBox::toggled, this, [=]
				{
					independent_Curve->calc_Functions.add_Specular_Peak = add_Specular_Peak->isChecked();
					global_Multilayer_Approach->global_Recalculate();
				});
			}
		}
		independent_Index++;
	}
	if(!horizontal_Layout->parent()) delete horizontal_Layout;
}

void Calculation_Settings_Editor::refresh_Independent_Calc_Properties(int tab_Index, int independent_Index, QGroupBox* box)
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
	Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(multilayer->independent_Curve_Tabs->widget(independent_Index));

	if(independent_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map])
	{
		if( //multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model ||
			multilayer->imperfections_Model.approximation == DWBA_approximation ||
			multilayer->imperfections_Model.approximation == SA_approximation ||
			multilayer->imperfections_Model.approximation == CSA_approximation )
		{
			QString text;
//			if( multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model ) text = "2D scattering can't be simulated with fractal Gauss model of roughness.\nChange the model in Structure Table.";
			if( multilayer->imperfections_Model.approximation == DWBA_approximation ) text = "2D scattering can't be simulated in DWBA approximation.\nChange the approximation in Structure Table.";
			if( multilayer->imperfections_Model.approximation == SA_approximation   ) text = "2D scattering can't be simulated in SA approximation.\nChange the approximation in Structure Table.";
			if( multilayer->imperfections_Model.approximation == CSA_approximation  ) text = "2D scattering can't be simulated in CSA approximation.\nChange the approximation in Structure Table.";

			QMessageBox::information(this,"GISAS simulation", text);
			box->blockSignals(true);
			box->setChecked(false);
			box->blockSignals(false);
		}
	}
	independent_Curve->calc_Functions.check_Enabled = box->isChecked();

	if(qobject_cast<QCheckBox*>(sender()))
	{
		bool recalc = true;
		QCheckBox* check_Box = qobject_cast<QCheckBox*>(sender());
		// 1D
		if(check_Box->text() == reflectance_Function)	{independent_Curve->calc_Functions.check_Reflectance = check_Box->isChecked();	global_Multilayer_Approach->reopen_Optical_Graphs_1D(true);}
		if(check_Box->text() == transmittance_Function) {independent_Curve->calc_Functions.check_Transmittance = check_Box->isChecked();global_Multilayer_Approach->reopen_Optical_Graphs_1D(true);}
		if(check_Box->text() == absorptance_Function)	{independent_Curve->calc_Functions.check_Absorptance = check_Box->isChecked();	global_Multilayer_Approach->reopen_Optical_Graphs_1D(true);}
		if(check_Box->text() == QString(scattering_Function) + " (" + independent_Curve->measurement.measurement_Type + ")")
		{
			if( multilayer->discretization_Parameters.enable_Discretization &&
			   (independent_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				independent_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] ) &&
			   (multilayer->imperfections_Model.approximation == DWBA_approximation ||
				multilayer->imperfections_Model.approximation == SA_approximation   ||
				multilayer->imperfections_Model.approximation == CSA_approximation  ))
			{
				QString text;
				if( multilayer->imperfections_Model.approximation == DWBA_approximation ) text = "Discretization can't be used with DWBA approximation.\nChange the approximation in Structure Table or disable discretization.";
				if( multilayer->imperfections_Model.approximation == SA_approximation   ) text = "Discretization can't be used with SA approximation.\nChange the approximation in Structure Table or disable discretization.";
				if( multilayer->imperfections_Model.approximation == CSA_approximation  ) text = "Discretization can't be used with CSA approximation.\nChange the approximation in Structure Table or disable discretization.";

				QMessageBox::information(this,"Discretization", text);
				independent_Curve->calc_Functions.check_Scattering = false;
				check_Box->blockSignals(true);
				check_Box->setChecked(false);
				check_Box->blockSignals(false);
				recalc = false;
			} else
			{
				independent_Curve->calc_Functions.check_Scattering = check_Box->isChecked();
				global_Multilayer_Approach->reopen_Optical_Graphs_1D(true);
			}
		}

		// 2D
		if(check_Box->text() == intensity_Function)		{independent_Curve->calc_Functions.check_Field = check_Box->isChecked(); global_Multilayer_Approach->reopen_Optical_Graphs_2D(true);}
		if(check_Box->text() == joule_Function)			{independent_Curve->calc_Functions.check_Joule = check_Box->isChecked(); global_Multilayer_Approach->reopen_Optical_Graphs_2D(true);}
		if(check_Box->text() == gisas_Function)
		{
			if( //multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model ||
				multilayer->imperfections_Model.approximation == DWBA_approximation ||
				multilayer->imperfections_Model.approximation == SA_approximation ||
				multilayer->imperfections_Model.approximation == CSA_approximation )
			{
				QString text;
//				if( multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model ) text = "2D scattering can't be simulated with fractal Gauss model of roughness.\nChange the model in Structure Table.";
				if( multilayer->imperfections_Model.approximation == DWBA_approximation ) text = "2D scattering can't be simulated in DWBA approximation.\nChange the approximation in Structure Table.";
				if( multilayer->imperfections_Model.approximation == SA_approximation   ) text = "2D scattering can't be simulated in SA approximation.\nChange the approximation in Structure Table.";
				if( multilayer->imperfections_Model.approximation == CSA_approximation  ) text = "2D scattering can't be simulated in CSA approximation.\nChange the approximation in Structure Table.";

				QMessageBox::information(this,"GISAS simulation", text);
				independent_Curve->calc_Functions.check_GISAS = false;
				check_Box->blockSignals(true);
				check_Box->setChecked(false);
				check_Box->blockSignals(false);
				recalc = false;
			} else
			{
				independent_Curve->calc_Functions.check_GISAS = check_Box->isChecked();
				global_Multilayer_Approach->reopen_Optical_Graphs_2D(true);
			}
		}

		if(recalc) global_Multilayer_Approach->global_Recalculate();
		activateWindow();
	}
	multilayer->independent_Curve_Tabs->setTabText(independent_Index, independent_Curve->name + independent_Curve->enlarge_Tab_Name());
	box->setTitle(multilayer->independent_Curve_Tabs->tabText(independent_Index));
}

void Calculation_Settings_Editor::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu;
	QAction settings_Action("Settings");
	menu.addAction(&settings_Action);

	connect(&settings_Action, &QAction::triggered, this, [=]{ settings();});

	menu.exec(event->globalPos());
}

void Calculation_Settings_Editor::settings()
{
	Multilayer* multilayer = qobject_cast<Multilayer*>(global_Multilayer_Approach->multilayer_Tabs->widget(main_Tabs->currentIndex()));

	QWidget* settings_Window = new QWidget(this);
		settings_Window->setWindowTitle("Settings");
		settings_Window->setWindowModality(Qt::ApplicationModal);
		settings_Window->setAttribute(Qt::WA_DeleteOnClose);
		settings_Window->setWindowFlags(Qt::Window);
		settings_Window->show();

	QVBoxLayout* settings_Main_Layout = new QVBoxLayout(settings_Window);
		settings_Main_Layout->setSizeConstraint(QLayout::SetFixedSize);
		settings_Main_Layout->setSpacing(5);
		settings_Main_Layout->setContentsMargins(5,5,5,5);

	// settings group box
	QGroupBox* settings_Group_Box = new QGroupBox;
		settings_Group_Box->setObjectName("settings_Group_Box");
		settings_Group_Box->setStyleSheet("QGroupBox#settings_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 0ex;}"
													"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
	settings_Main_Layout->addWidget(settings_Group_Box);

	QGridLayout* settings_Group_Box_Layout = new QGridLayout(settings_Group_Box);
		settings_Group_Box_Layout->setContentsMargins(5,5,5,5);
	settings_Main_Layout->addWidget(settings_Group_Box);

	// num target rows
	QLabel* target_Rows_Label = new QLabel("Number of \"Measured\" rows");
	QSpinBox* target_Rows_SpinBox = new QSpinBox;
		target_Rows_SpinBox->setRange(1, total_Number_of_Targets[main_Tabs->currentIndex()]);
		target_Rows_SpinBox->setValue(multilayer->num_Target_Rows);
		target_Rows_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		target_Rows_SpinBox->setAccelerated(true);
		target_Rows_SpinBox->setFixedWidth(25);

	settings_Group_Box_Layout->addWidget(target_Rows_Label,		0,0,1,1);
	settings_Group_Box_Layout->addWidget(target_Rows_SpinBox,	0,1,1,1);

	// num independent rows
	QLabel* independent_Rows_Label = new QLabel("Number of \"Independent\" rows");
	QSpinBox* independent_Rows_SpinBox = new QSpinBox;
		independent_Rows_SpinBox->setRange(1, total_Number_of_Independents[main_Tabs->currentIndex()]);
		independent_Rows_SpinBox->setValue(multilayer->num_Independent_Rows);
		independent_Rows_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		independent_Rows_SpinBox->setAccelerated(true);
		independent_Rows_SpinBox->setFixedWidth(25);

	settings_Group_Box_Layout->addWidget(independent_Rows_Label,	1,0,1,1);
	settings_Group_Box_Layout->addWidget(independent_Rows_SpinBox,	1,1,1,1);

	// buttons
	QHBoxLayout* buttons_Layout = new QHBoxLayout;
		buttons_Layout->setSpacing(10);
		buttons_Layout->setContentsMargins(0,0,0,0);

	QPushButton* ok_Button = new QPushButton ("OK");
	QPushButton* cancel_Button = new QPushButton("Cancel");
		buttons_Layout->addWidget(ok_Button);
		buttons_Layout->addWidget(cancel_Button);
	settings_Main_Layout->addLayout(buttons_Layout);

	connect(cancel_Button,  &QPushButton::clicked, this, [=]{settings_Window->close();});
	connect(ok_Button,      &QPushButton::clicked, this, [=]
	{
		multilayer->num_Target_Rows = target_Rows_SpinBox->value();
		multilayer->num_Independent_Rows = independent_Rows_SpinBox->value();

		global_Multilayer_Approach->reopen_Calculation_Settings(true);
		settings_Window->close();
	});
}
