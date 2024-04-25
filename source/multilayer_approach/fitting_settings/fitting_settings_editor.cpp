#include "fitting_settings_editor.h"
#include "standard/spoiler.h"

Fitting_Settings_Editor::Fitting_Settings_Editor(QWidget* parent) :
	fitting_Settings(global_Multilayer_Approach->fitting_Settings),
	QWidget(parent)
{
	setWindowTitle("Fitting settings");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);

	setProperty(height_Property,height());
    global_Multilayer_Approach->windows_Stack.append(this);
}

void Fitting_Settings_Editor::changeEvent(QEvent *event)
{
    Global_Variables::common_Change_Event(event, this);
}

void Fitting_Settings_Editor::closeEvent(QCloseEvent *event)
{
	write_Window_Geometry();
	global_Multilayer_Approach->runned_Fitting_Settings_Editor.remove(fit_Settings_Key);
    global_Multilayer_Approach->windows_Stack.removeOne(this);
    event->accept();
}

void Fitting_Settings_Editor::create_Main_Layout()
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
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(4,4,4,0);
		main_Layout->setAlignment(Qt::AlignTop);

    Global_Variables::create_Shortcuts(this);
	create_Metods();
	create_Pages();
	create_GSL_Main_Params_Group_Box();
	create_GSL_AdditionalParams_Group_Box();
	create_SO_Main_Params_Group_Box();
	create_SO_AdditionalParams_Group_Box();
	{
		done_Button = new QPushButton("Close");
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
			done_Button->setDefault(true);
		connect(done_Button, &QPushButton::clicked, this, &Fitting_Settings_Editor::close);

		abort_Button = new QPushButton("Abort calculations");
			abort_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			abort_Button->setFocus();
			abort_Button->setDefault(true);
			abort_Button->setEnabled(fitting_Settings->in_Calculation);
		connect(abort_Button, &QPushButton::clicked, global_Multilayer_Approach, &Multilayer_Approach::abort_Calculations);

		QHBoxLayout* button_Layout = new QHBoxLayout;
//			button_Layout->setSpacing(0);
			button_Layout->addWidget(done_Button);
			button_Layout->addWidget(abort_Button);

		main_Layout->addLayout(button_Layout);
	}

	methods_Combo_Box->currentIndexChanged(methods_Combo_Box->currentIndex());
	interface_Created = true;
}

void Fitting_Settings_Editor::set_Window_Geometry()
{
	setGeometry(fitting_settings_x_corner,fitting_settings_y_corner,width(),height());
	adjustSize();
	if(!make_all_windows_resizeable) {
		setFixedSize(size());
	}
}

void Fitting_Settings_Editor::write_Window_Geometry()
{
	if(!isMaximized())
	{
		fitting_settings_x_corner = frameGeometry().x()-corner_x_shift;
		fitting_settings_y_corner = frameGeometry().y()-corner_y_shift;
	}
}

void Fitting_Settings_Editor::create_Metods()
{
	// add objects to window
	methods_Layout = new QHBoxLayout;
		methods_Layout->setAlignment(Qt::AlignLeft);
		methods_Layout->setSpacing(5);
	main_Layout->addLayout(methods_Layout);

	// label
	methods_Label = new QLabel("Fitting Algorithm");
	methods_Layout->addWidget(methods_Label);

	// combo
	methods_Combo_Box = new QComboBox;
	methods_Layout->addWidget(methods_Combo_Box);

	// create GSL group header
	{
		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());
		methods_Combo_Box->addItem(GSL_Group);
		indices_Vec.append(methods_Combo_Box->count()-1);

		QFont font(methods_Combo_Box->font());
//		font.setPointSize(8.25);
//		font.setFamily("MS Shell Dlg 2");
//		font.setItalic(true);
		font.setBold(true);

		QStandardItemModel* model =	qobject_cast<QStandardItemModel*>(methods_Combo_Box->model());
		QModelIndex firstIndex = model->index(methods_Combo_Box->count()-1, methods_Combo_Box->modelColumn(), methods_Combo_Box->rootModelIndex());
		QStandardItem* firstItem = model->itemFromIndex(firstIndex);
		firstItem->setSelectable(false);
		firstItem->setFont(font);

		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());
	}

	QString spacer = "      ";

	// fill GSL group
	{
		methods_Combo_Box->addItem(spacer + GSL_Methods[Levenberg_Marquardt]);
		methods_Combo_Box->addItem(spacer + GSL_Methods[Levenberg_Marquardt_with_Geodesic_Acceleration]);
		methods_Combo_Box->addItem(spacer + GSL_Methods[Dogleg]);
		methods_Combo_Box->addItem(spacer + GSL_Methods[Double_Dogleg]);
		methods_Combo_Box->addItem(spacer + GSL_Methods[Two_Dimensional_Subspace]);
//		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());

		// TODO
//		methods_Combo_Box->addItem(spacer + GSL_Methods[Fletcher_Reeves_Conjugate_Gradient]);
//		methods_Combo_Box->addItem(spacer + GSL_Methods[Polak_Ribiere_Conjugate_Gradient]);
//		methods_Combo_Box->addItem(spacer + GSL_Methods[Broyden_Fletcher_Goldfarb_Shanno_BFGS]);
//		methods_Combo_Box->addItem(spacer + GSL_Methods[Broyden_Fletcher_Goldfarb_Shanno_BFGS_2]);
//		methods_Combo_Box->addItem(spacer + GSL_Methods[Steepest_Descent]);
//		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());

//		methods_Combo_Box->addItem(spacer + GSL_Methods[Nelder_Mead_Simplex]);
//		methods_Combo_Box->addItem(spacer + GSL_Methods[Nelder_Mead_Simplex_2]);
//		methods_Combo_Box->addItem(spacer + GSL_Methods[Nelder_Mead_Simplex_2_Randomized]);
//		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());
	}

	// create SwarmOps group header
	{
		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());
		methods_Combo_Box->addItem(SO_Group);
		indices_Vec.append(methods_Combo_Box->count()-1);

		QFont font(methods_Combo_Box->font());
//		font.setPointSize(8.25);
//		font.setFamily("MS Shell Dlg 2");
//		font.setItalic(true);
		font.setBold(true);

		QStandardItemModel* model =	qobject_cast<QStandardItemModel*>(methods_Combo_Box->model());
		QModelIndex firstIndex = model->index(methods_Combo_Box->count()-1, methods_Combo_Box->modelColumn(), methods_Combo_Box->rootModelIndex());
		QStandardItem* firstItem = model->itemFromIndex(firstIndex);
		firstItem->setSelectable(false);
		firstItem->setFont(font);

		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());
	}
	// fill SwarmOps group
	{
		methods_Combo_Box->addItem(spacer + SO_Methods[Mesh_Iteration]						 );
		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());

		methods_Combo_Box->addItem(spacer + SO_Methods[Random_Sampling_Uniform]				 );
		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());

//		methods_Combo_Box->addItem(spacer + SO_Methods[Gradient_Descent]					 );//
//		methods_Combo_Box->addItem(spacer + SO_Methods[Gradient_Emancipated_Descent]		 );//
		methods_Combo_Box->addItem(spacer + SO_Methods[Hill_Climber]						 );//
		methods_Combo_Box->addItem(spacer + SO_Methods[Simulated_Annealing]					 );//
		methods_Combo_Box->addItem(spacer + SO_Methods[Pattern_Search]						 );//
		methods_Combo_Box->addItem(spacer + SO_Methods[Local_Unimodal_Sampling]				 );//
		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());

		methods_Combo_Box->addItem(spacer + SO_Methods[Differential_Evolution]				 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Differential_Evolution_Suite]		 );
		methods_Combo_Box->addItem(spacer + SO_Methods[DE_with_Temporal_Parameters]			 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Jan_Differential_Evolution]			 );//
		methods_Combo_Box->addItem(spacer + SO_Methods[Evolution_by_Lingering_Global_Best]	 );//
		methods_Combo_Box->addItem(spacer + SO_Methods[More_Yo_yos_Doing_Global_optimization]);//
		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());

		methods_Combo_Box->addItem(spacer + SO_Methods[Particle_Swarm_Optimization]			 );//
		methods_Combo_Box->addItem(spacer + SO_Methods[Forever_Accumulating_Evolution]		 );//
		methods_Combo_Box->addItem(spacer + SO_Methods[Many_Optimizing_Liaisons]			 );//
		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());

		methods_Combo_Box->addItem(spacer + SO_Methods[Layered_and_Interleaved_CoEvolution]	 );//
		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());
	}

	methods_Combo_Box->setMaxVisibleItems(methods_Combo_Box->count());
	methods_Combo_Box->setCurrentIndex(methods_Combo_Box->findText(spacer + fitting_Settings->current_Method));
	methods_Combo_Box->adjustSize();
	methods_Combo_Box->setFixedWidth(methods_Combo_Box->width()+90);

	// skip group headers
	connect(methods_Combo_Box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]
	{
		if(indices_Vec.contains(methods_Combo_Box->currentIndex()))
		{
			int previous = methods_Combo_Box->property("previous").toInt();
			if(previous<methods_Combo_Box->currentIndex() && methods_Combo_Box->currentIndex()+2 < methods_Combo_Box->count())
			{
				methods_Combo_Box->setCurrentIndex(methods_Combo_Box->currentIndex()+2);
			} else
			if(previous>methods_Combo_Box->currentIndex() && methods_Combo_Box->currentIndex()-3 >= 0)
			{
				methods_Combo_Box->setCurrentIndex(methods_Combo_Box->currentIndex()-3);
			} else
			{
				methods_Combo_Box->setCurrentIndex(previous);
			}
		}
		methods_Combo_Box->setProperty("previous", methods_Combo_Box->currentIndex());
		fitting_Settings->current_Method = methods_Combo_Box->currentText().remove(QRegExp(spacer));

		// TODO temporary
		/// GSL
		if(	GSL_Methods.contains(fitting_Settings->current_Method) && GSL_Page)
		{
			pages_Stack->setCurrentWidget(GSL_Page);
		}
		/// SO
		if(	SO_Methods. contains(fitting_Settings->current_Method) && SO_Page )
		{
			pages_Stack->setCurrentWidget(SO_Page);

			if(SO_Additional_Pages_Stack)
			{
				SO_Additional_Pages_Stack->setCurrentIndex(SO_Methods.indexOf(fitting_Settings->current_Method));
			}
		}

		// reload interface state
		if(interface_Created)
		{
			GSL_randomized_Start_Check_Box->setChecked(fitting_Settings->randomized_Start);
			SO_randomized_Start_Check_Box->setChecked(fitting_Settings->randomized_Start);
			SO_initialize_By_Current_Check_Box->setChecked(fitting_Settings->initialize_By_Current_State);
		}		
	});
}

void Fitting_Settings_Editor::create_Pages()
{
	pages_Stack = new QStackedWidget;
	main_Layout->addWidget(pages_Stack);

	// GSL
	{
		GSL_Page = new QWidget(this);

		GSL_Page_Layout = new QVBoxLayout(GSL_Page);
			GSL_Page_Layout->setAlignment(Qt::AlignTop);
			GSL_Page_Layout->setContentsMargins(0,0,0,0);
		pages_Stack->addWidget(GSL_Page);
	}

	// SO
	{
		SO_Page = new QWidget(this);

		SO_Page_Layout = new QVBoxLayout(SO_Page);
			SO_Page_Layout->setAlignment(Qt::AlignTop);
			SO_Page_Layout->setContentsMargins(0,0,0,0);
		pages_Stack->addWidget(SO_Page);
	}
}

void Fitting_Settings_Editor::create_GSL_Main_Params_Group_Box()
{
	{
		GSL_Fit_Params_Group_Box = new QGroupBox(this);
			GSL_Fit_Params_Group_Box->setObjectName("GSL_Fit_Params_Group_Box");
			GSL_Fit_Params_Group_Box->setStyleSheet("QGroupBox#GSL_Fit_Params_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
																"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		GSL_Page_Layout->addWidget(GSL_Fit_Params_Group_Box);
		GSL_Fit_Params_Group_Box_Layout = new QGridLayout(GSL_Fit_Params_Group_Box);
	}

	// for GSL TRS
	{
		GSL_randomized_Start_Check_Box = new QCheckBox("Randomized start",this);
			GSL_randomized_Start_Check_Box->setChecked(fitting_Settings->randomized_Start);

		GSL_num_Runs_Label = new QLabel("Number of runs");
			GSL_num_Runs_Label->setEnabled(fitting_Settings->randomized_Start);
		GSL_num_Runs_SpinBox = new QSpinBox;
			GSL_num_Runs_SpinBox->setRange(1, MAX_INTEGER);
                        GSL_num_Runs_SpinBox->setValue(fitting_Settings->num_Runs_GSL);
			GSL_num_Runs_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			GSL_num_Runs_SpinBox->setAccelerated(true);
			GSL_num_Runs_SpinBox->setEnabled(fitting_Settings->randomized_Start);

        GSL_max_Iter_Label = new QLabel("Number of iterations");
		GSL_max_Iter_SpinBox = new QSpinBox;
			GSL_max_Iter_SpinBox->setRange(1,MAX_INTEGER);
			GSL_max_Iter_SpinBox->setValue(fitting_Settings->max_Iter);
			GSL_max_Iter_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			GSL_max_Iter_SpinBox->setAccelerated(true);

		GSL_common_Tolerance_Label = new QLabel("General tolerance");
		GSL_common_Tolerance_Line_Edit = new QLineEdit(Locale.toString(fitting_Settings->x_Tolerance));
			GSL_common_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_randomized_Start_Check_Box,	0,0,1,2);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_num_Runs_Label,				1,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_num_Runs_SpinBox,			1,1,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_max_Iter_Label,				2,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_max_Iter_SpinBox,			2,1,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_common_Tolerance_Label,		3,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_common_Tolerance_Line_Edit,	3,1,1,1);

		connect(GSL_randomized_Start_Check_Box,	&QCheckBox::toggled, this, [=]
		{
			fitting_Settings->randomized_Start = GSL_randomized_Start_Check_Box->isChecked();
			GSL_num_Runs_Label->setEnabled(fitting_Settings->randomized_Start);
			GSL_num_Runs_SpinBox->setEnabled(fitting_Settings->randomized_Start);
		});
                connect(GSL_num_Runs_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{fitting_Settings->num_Runs_GSL = GSL_num_Runs_SpinBox->value();});
		connect(GSL_max_Iter_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{fitting_Settings->max_Iter = GSL_max_Iter_SpinBox->value();});
		connect(GSL_common_Tolerance_Line_Edit, &QLineEdit::textChanged, this, [=]
		{
			fitting_Settings->x_Tolerance = Locale.toDouble(GSL_common_Tolerance_Line_Edit->text());
			fitting_Settings->f_Tolerance = fitting_Settings->x_Tolerance;
			fitting_Settings->g_Tolerance = fitting_Settings->x_Tolerance;
		});
	}
}

void Fitting_Settings_Editor::create_GSL_AdditionalParams_Group_Box()
{
	{
		GSL_Additional_Params_Group_Box = new QGroupBox(this);
			GSL_Additional_Params_Group_Box->setObjectName("GSL_Additional_Params_Group_Box");
			GSL_Additional_Params_Group_Box->setStyleSheet("QGroupBox#GSL_Additional_Params_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
																				  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		GSL_Additional_Params_Group_Box_Layout = new QGridLayout(GSL_Additional_Params_Group_Box);
	}
	{
		GSL_x_Tolerance_Label = new QLabel("Parameters tolerance");
		GSL_x_Tolerance_Line_Edit = new QLineEdit(Locale.toString(fitting_Settings->x_Tolerance));
			GSL_x_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_g_Tolerance_Label = new QLabel("Gradient tolerance");
		GSL_g_Tolerance_Line_Edit = new QLineEdit(Locale.toString(fitting_Settings->g_Tolerance));
			GSL_g_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_f_Tolerance_Label = new QLabel("Residual tolerance");
		GSL_f_Tolerance_Line_Edit = new QLineEdit(Locale.toString(fitting_Settings->f_Tolerance));
			GSL_f_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));



		GSL_scale_Label = new QLabel("Scaling method (default Mor"+More_Sym+")");
		GSL_scale_Combo_Box = new QComboBox;
			GSL_scale_Combo_Box->addItem(GSL_Scales[More]);
			GSL_scale_Combo_Box->addItem(GSL_Scales[Levenberg]);
			GSL_scale_Combo_Box->addItem(GSL_Scales[Marquardt]);
			GSL_scale_Combo_Box->setCurrentIndex(GSL_scale_Combo_Box->findText(fitting_Settings->current_Scale));

		GSL_solver_Label = new QLabel("Solver method (default QR decomposition)");
		GSL_solver_Combo_Box = new QComboBox;
			GSL_solver_Combo_Box->addItem(GSL_Solvers[QR_decomposition]);
			GSL_solver_Combo_Box->addItem(GSL_Solvers[Cholesky_decomposition]);
			GSL_solver_Combo_Box->addItem(GSL_Solvers[Singular_value_decomposition]);
			GSL_solver_Combo_Box->setCurrentIndex(GSL_solver_Combo_Box->findText(fitting_Settings->current_Solver));

		GSL_fdtype_Label = new QLabel("Finite difference method (default Forward)");
		GSL_fdtype_Combo_Box = new QComboBox;
			GSL_fdtype_Combo_Box->addItem(GSL_Fdtype[Forward]);
			GSL_fdtype_Combo_Box->addItem(GSL_Fdtype[Central]);
			GSL_fdtype_Combo_Box->setCurrentIndex(GSL_fdtype_Combo_Box->findText(fitting_Settings->current_Fdtype));

		GSL_factor_up_Label = new QLabel("Factor for increasing trust radius (default 3)");
		GSL_factor_up_Line_Edit = new QLineEdit(Locale.toString(fitting_Settings->factor_Up));
			GSL_factor_up_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_factor_down_Label = new QLabel("Factor for decreasing trust radius (default 2)");
		GSL_factor_down_Line_Edit = new QLineEdit(Locale.toString(fitting_Settings->factor_Down));
			GSL_factor_down_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_avmax_Label = new QLabel("Max allowed |a|/|v| (default 0.75)");
		GSL_avmax_Line_Edit = new QLineEdit(Locale.toString(fitting_Settings->avmax));
			GSL_avmax_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_h_df_Label = new QLabel("Step size for finite difference J (default ~1.5e-8)");
		GSL_h_df_Line_Edit = new QLineEdit(Locale.toString(fitting_Settings->h_df));
			GSL_h_df_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_h_fvv_Label = new QLabel("Step size for finite difference fvv (default 0.02)");
		GSL_h_fvv_Line_Edit = new QLineEdit(Locale.toString(fitting_Settings->h_fvv));
			GSL_h_fvv_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		int p=0;
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_x_Tolerance_Label,	p+0,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_x_Tolerance_Line_Edit,p+0,1,1,2);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_g_Tolerance_Label,	p+1,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_g_Tolerance_Line_Edit,p+1,1,1,2);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_f_Tolerance_Label,	p+2,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_f_Tolerance_Line_Edit,p+2,1,1,2);
		p+=3;
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_scale_Label,			p+0,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_scale_Combo_Box,		p+0,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_solver_Label,			p+1,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_solver_Combo_Box,		p+1,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_fdtype_Label,			p+2,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_fdtype_Combo_Box,		p+2,1,1,1);
		p+=3;
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_factor_up_Label,		p+0,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_factor_up_Line_Edit,	p+0,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_factor_down_Label,	p+1,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_factor_down_Line_Edit,p+1,1,1,1);
		p+=2;
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_avmax_Label,			p+0,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_avmax_Line_Edit,		p+0,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_h_df_Label,			p+1,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_h_df_Line_Edit,		p+1,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_h_fvv_Label,			p+2,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(GSL_h_fvv_Line_Edit,		p+2,1,1,1);

		connect(GSL_x_Tolerance_Line_Edit, &QLineEdit::textChanged, this, [=]{fitting_Settings->x_Tolerance = Locale.toDouble(GSL_x_Tolerance_Line_Edit->text());	});
		connect(GSL_g_Tolerance_Line_Edit, &QLineEdit::textChanged, this, [=]{fitting_Settings->g_Tolerance = Locale.toDouble(GSL_g_Tolerance_Line_Edit->text());	});
		connect(GSL_f_Tolerance_Line_Edit, &QLineEdit::textChanged, this, [=]{fitting_Settings->f_Tolerance = Locale.toDouble(GSL_f_Tolerance_Line_Edit->text());	});

		connect(GSL_scale_Combo_Box,  static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]{fitting_Settings->current_Scale  = GSL_scale_Combo_Box->currentText(); });
		connect(GSL_solver_Combo_Box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]{fitting_Settings->current_Solver = GSL_solver_Combo_Box->currentText();});
		connect(GSL_fdtype_Combo_Box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]{fitting_Settings->current_Fdtype = GSL_fdtype_Combo_Box->currentText();});

		connect(GSL_factor_up_Line_Edit,   &QLineEdit::textChanged, this, [=]{fitting_Settings->factor_Up	= Locale.toDouble(GSL_factor_up_Line_Edit->text());		});
		connect(GSL_factor_down_Line_Edit, &QLineEdit::textChanged, this, [=]{fitting_Settings->factor_Down	= Locale.toDouble(GSL_factor_down_Line_Edit->text());	});
		connect(GSL_avmax_Line_Edit,	   &QLineEdit::textChanged, this, [=]{fitting_Settings->avmax		= Locale.toDouble(GSL_avmax_Line_Edit->text());			});
		connect(GSL_h_df_Line_Edit,		   &QLineEdit::textChanged, this, [=]{fitting_Settings->h_df		= Locale.toDouble(GSL_h_df_Line_Edit->text());			});
		connect(GSL_h_fvv_Line_Edit,	   &QLineEdit::textChanged, this, [=]{fitting_Settings->h_fvv		= Locale.toDouble(GSL_h_fvv_Line_Edit->text());			});
	}
	{
		// create spoiler
		QVBoxLayout* frame_Layout = new QVBoxLayout;
			frame_Layout->setContentsMargins(0,0,0,0);
			frame_Layout->addWidget(GSL_Additional_Params_Group_Box);
		Spoiler* spoiler = new Spoiler("Additional parameters", 5, this);
			spoiler->setContentLayout(*frame_Layout);
		GSL_Page_Layout->addWidget(spoiler);
	}
}

void Fitting_Settings_Editor::create_SO_Main_Params_Group_Box()
{
	{
		SO_Fit_Params_Group_Box = new QGroupBox(this);
			SO_Fit_Params_Group_Box->setObjectName("SO_Fit_Params_Group_Box");
			SO_Fit_Params_Group_Box->setStyleSheet("QGroupBox#SO_Fit_Params_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
																  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		SO_Page_Layout->addWidget(SO_Fit_Params_Group_Box);
		SO_Fit_Params_Group_Box_Layout = new QGridLayout(SO_Fit_Params_Group_Box);
	}
	{
		SO_randomized_Start_Check_Box = new QCheckBox("Randomized start",this);
			SO_randomized_Start_Check_Box->setChecked(fitting_Settings->randomized_Start);

		SO_initialize_By_Current_Check_Box = new QCheckBox("Initialize by current state",this);
			SO_initialize_By_Current_Check_Box->setChecked(fitting_Settings->initialize_By_Current_State);

		SO_num_Runs_Label = new QLabel("Number of runs");
			SO_num_Runs_Label->setEnabled(fitting_Settings->randomized_Start);
		SO_num_Runs_SpinBox = new QSpinBox;
			SO_num_Runs_SpinBox->setRange(1, MAX_INTEGER);
			SO_num_Runs_SpinBox->setEnabled(fitting_Settings->randomized_Start);
			SO_num_Runs_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			SO_num_Runs_SpinBox->setAccelerated(true);
                        SO_num_Runs_SpinBox->setValue(fitting_Settings->num_Runs_SO);

		SO_max_Evaluations_Label = new QLabel("Max number of evaluations");
			SO_max_Evaluations_Label->setDisabled(fitting_Settings->max_Eval_Check);
		SO_max_Evaluations_SpinBox = new QSpinBox;
			SO_max_Evaluations_SpinBox->setRange(1, MAX_INTEGER);
			SO_max_Evaluations_SpinBox->setDisabled(fitting_Settings->max_Eval_Check);
			SO_max_Evaluations_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			SO_max_Evaluations_SpinBox->setAccelerated(true);
			SO_max_Evaluations_SpinBox->setValue(fitting_Settings->max_Evaluations);

		SO_max_Eval_Check_Box = new QCheckBox("Num. evals "+Proportional_Sym+" num. params",this);
			SO_max_Eval_Check_Box->setChecked(fitting_Settings->max_Eval_Check);
		SO_max_Eval_Factor_SpinBox = new QSpinBox;
			SO_max_Eval_Factor_SpinBox->setRange(1, MAX_INTEGER);
			SO_max_Eval_Factor_SpinBox->setEnabled(fitting_Settings->max_Eval_Check);
			SO_max_Eval_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			SO_max_Eval_Factor_SpinBox->setAccelerated(true);
			SO_max_Eval_Factor_SpinBox->setValue(fitting_Settings->max_Eval_Factor);

		SO_Fit_Params_Group_Box_Layout->addWidget(SO_randomized_Start_Check_Box,		0,0,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_initialize_By_Current_Check_Box,	0,1,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_num_Runs_Label,					1,0,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_num_Runs_SpinBox,					1,1,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_max_Evaluations_Label,				2,0,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_max_Evaluations_SpinBox,			2,1,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_max_Eval_Check_Box,				3,0,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_max_Eval_Factor_SpinBox,			3,1,1,1);

		connect(SO_randomized_Start_Check_Box,&QCheckBox::toggled,	   this, [=]
		{
			fitting_Settings->randomized_Start = SO_randomized_Start_Check_Box->isChecked();
			SO_num_Runs_Label->setEnabled(fitting_Settings->randomized_Start);
			SO_num_Runs_SpinBox->setEnabled(fitting_Settings->randomized_Start);
		});
		connect(SO_initialize_By_Current_Check_Box,&QCheckBox::toggled,	   this, [=]
		{
			fitting_Settings->initialize_By_Current_State = SO_initialize_By_Current_Check_Box->isChecked();
		});
                connect(SO_num_Runs_SpinBox,		static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{fitting_Settings->num_Runs_SO = SO_num_Runs_SpinBox->value();});
		connect(SO_max_Evaluations_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{fitting_Settings->max_Evaluations = SO_max_Evaluations_SpinBox->value();});
		connect(SO_max_Eval_Factor_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]{fitting_Settings->max_Eval_Factor = SO_max_Eval_Factor_SpinBox->value();});
		connect(SO_max_Eval_Check_Box,		&QCheckBox::toggled,	 this, [=]
		{
			fitting_Settings->max_Eval_Check = SO_max_Eval_Check_Box->isChecked();
			SO_max_Evaluations_Label  ->setDisabled(fitting_Settings->max_Eval_Check);
			SO_max_Evaluations_SpinBox->setDisabled(fitting_Settings->max_Eval_Check);
			SO_max_Eval_Factor_SpinBox->setEnabled (fitting_Settings->max_Eval_Check);
		});
	}
}

void Fitting_Settings_Editor::create_SO_AdditionalParams_Group_Box()
{
	{
		SO_Additional_Params_Group_Box = new QGroupBox(this);
			SO_Additional_Params_Group_Box->setObjectName("SO_Additional_Params_Group_Box");
			SO_Additional_Params_Group_Box->setStyleSheet("QGroupBox#SO_Additional_Params_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
																				  "QGroupBox::title { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		SO_Additional_Params_Group_Box_Layout = new QVBoxLayout(SO_Additional_Params_Group_Box);
	}
	{
		SO_Additional_Pages_Stack = new QStackedWidget;
		SO_Additional_Params_Group_Box_Layout->addWidget(SO_Additional_Pages_Stack);

		// pages for each method
		Mesh_Iteration;
		{
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Mesh_Iteration,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);

			// specific widgets
			QLabel* label = new QLabel("No options for Mesh Iteration method");
			page_Layout->addWidget(label,0,0,Qt::AlignCenter);
		}
		Random_Sampling_Uniform;
		{
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Random_Sampling_Uniform,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);

			// specific widgets
			QLabel* label = new QLabel("No options for Random Sampling (Uniform) method");
			page_Layout->addWidget(label,0,0,Qt::AlignCenter);
		}
		Hill_Climber;
		{
			/// look at Methods/HC.c in SO source code and at page 23 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Hill_Climber,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// r factor
			QLabel* r_HC_Label = new QLabel("Range factor for divergence r"+Element_Sym+"(0,1]"); // official: "Sampling range"
			MyDoubleSpinBox* r_HC_SpinBox = new MyDoubleSpinBox(this, false);
				r_HC_SpinBox->setRange(1E-5,1);
				r_HC_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				r_HC_SpinBox->setDecimals(8);
				r_HC_SpinBox->setSingleStep(0.01);
				r_HC_SpinBox->setValue(fitting_Settings->r_Factor_HC);
				r_HC_SpinBox->setAccelerated(true);
				r_HC_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(r_HC_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->r_Factor_HC = r_HC_SpinBox->value();
			});

			// D weight
			QLabel* D_HC_Label = new QLabel("\"Temperature\" of transition probability D>0"); // official: "Probability weight"
			MyDoubleSpinBox* D_HC_SpinBox = new MyDoubleSpinBox(this, false);
				D_HC_SpinBox->setRange(1E-3,100000);
				D_HC_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				D_HC_SpinBox->setDecimals(8);
				D_HC_SpinBox->setSingleStep(0.01);
				D_HC_SpinBox->setValue(fitting_Settings->D_HC);
				D_HC_SpinBox->setAccelerated(true);
				D_HC_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(D_HC_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->D_HC = D_HC_SpinBox->value();
			});

			page_Layout->addWidget(r_HC_Label,0,0);
			page_Layout->addWidget(r_HC_SpinBox,0,1);

			page_Layout->addWidget(D_HC_Label,1,0);
			page_Layout->addWidget(D_HC_SpinBox,1,1);
		}
		Simulated_Annealing;
		{
			/// look at Methods/SA.c in SO source code and at page 24 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Simulated_Annealing,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// r factor
			QLabel* r_SA_Label = new QLabel("Range factor for divergence r"+Element_Sym+"(0,1]"); // official: "r, sampling range factor"
			MyDoubleSpinBox* r_SA_SpinBox = new MyDoubleSpinBox(this, false);
				r_SA_SpinBox->setRange(1E-5,1);
				r_SA_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				r_SA_SpinBox->setDecimals(8);
				r_SA_SpinBox->setSingleStep(0.01);
				r_SA_SpinBox->setValue(fitting_Settings->r_Factor_SA);
				r_SA_SpinBox->setAccelerated(true);
				r_SA_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(r_SA_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->r_Factor_SA = r_SA_SpinBox->value();
			});

			// alpha
			QLabel* alpha_SA_Label = new QLabel("Start temperature "+Alpha_Sym+Element_Sym+"(0,1]"); // official: "alpha, start-value"
			MyDoubleSpinBox* alpha_SA_SpinBox = new MyDoubleSpinBox(this, false);
				alpha_SA_SpinBox->setRange(1E-5,1);
				alpha_SA_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				alpha_SA_SpinBox->setDecimals(8);
				alpha_SA_SpinBox->setSingleStep(0.01);
				alpha_SA_SpinBox->setValue(fitting_Settings->alpha_SA);
				alpha_SA_SpinBox->setAccelerated(true);
				alpha_SA_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(alpha_SA_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->alpha_SA = alpha_SA_SpinBox->value();
			});

			// beta
			QLabel* beta_SA_Label = new QLabel("End temperature "+Beta_Sym+Element_Sym+"(0,1]"); // official: "beta, end-value"
			MyDoubleSpinBox* beta_SA_SpinBox = new MyDoubleSpinBox(this, false);
				beta_SA_SpinBox->setRange(1E-5,1);
				beta_SA_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				beta_SA_SpinBox->setDecimals(8);
				beta_SA_SpinBox->setSingleStep(0.01);
				beta_SA_SpinBox->setValue(fitting_Settings->beta_SA);
				beta_SA_SpinBox->setAccelerated(true);
				beta_SA_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(beta_SA_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->beta_SA = beta_SA_SpinBox->value();
			});

			// T, iterations
			QLabel* T_SA_Label = new QLabel("Iterations between resets"); // official: "T, iterations between resets"
			QSpinBox* T_SA_SpinBox = new QSpinBox;
				T_SA_SpinBox->setRange(100,100000);
				T_SA_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				T_SA_SpinBox->setSingleStep(10);
				T_SA_SpinBox->setValue(fitting_Settings->T_SA);
				T_SA_SpinBox->setAccelerated(true);
				T_SA_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(T_SA_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->T_SA = T_SA_SpinBox->value();
			});
			int p=0;
			page_Layout->addWidget(r_SA_Label,p,0);
			page_Layout->addWidget(r_SA_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(alpha_SA_Label,p,0);
			page_Layout->addWidget(alpha_SA_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(beta_SA_Label,p,0);
			page_Layout->addWidget(beta_SA_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(T_SA_Label,p,0);
			page_Layout->addWidget(T_SA_SpinBox,p,1);
		}
		Pattern_Search;
		{
			/// look at Methods/PS.c in SO source code and at page 27 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Pattern_Search,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);

			// specific widgets
			QLabel* label = new QLabel("No options for Pattern Search method");
			page_Layout->addWidget(label,0,0,Qt::AlignCenter);
		}
		Local_Unimodal_Sampling;
		{
			/// look at Methods/LUS.c in SO source code and at page 25 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Local_Unimodal_Sampling,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// gamma
			QLabel* gamma_LUS_Label = new QLabel("Power of decreasing range "+Gamma_Sym+Element_Sym+"[0.5,20]"); // official: "gamma"
			MyDoubleSpinBox* gamma_LUS_SpinBox = new MyDoubleSpinBox(this, false);
				gamma_LUS_SpinBox->setRange(0.5,20);
				gamma_LUS_SpinBox->setDecimals(8);
				gamma_LUS_SpinBox->setSingleStep(0.1);
				gamma_LUS_SpinBox->setValue(fitting_Settings->gamma_LUS);
				gamma_LUS_SpinBox->setAccelerated(true);
				gamma_LUS_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(gamma_LUS_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->gamma_LUS = gamma_LUS_SpinBox->value();
			});

			page_Layout->addWidget(gamma_LUS_Label,0,0);
			page_Layout->addWidget(gamma_LUS_SpinBox,0,1);
		}
		Differential_Evolution;
		{
			/// look at Methods/DE.c in SO source code and at page 28 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Differential_Evolution,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// NP
			QLabel* NP_DE_Label = new QLabel("Swarm size NP"+Element_Sym+"[3,500]"); // official: "Number of agents NP"
			MyDoubleSpinBox* NP_DE_SpinBox = new MyDoubleSpinBox(this, false);
				NP_DE_SpinBox->setRange(3,500);
				NP_DE_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				NP_DE_SpinBox->setDecimals(0);
				NP_DE_SpinBox->setSingleStep(1);
				NP_DE_SpinBox->setValue(fitting_Settings->NP_DE);
				NP_DE_SpinBox->setAccelerated(true);
				NP_DE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(NP_DE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->NP_DE = NP_DE_SpinBox->value();
			});

			// CR
			QLabel* CR_DE_Label = new QLabel("Crossover probability CR"+Element_Sym+"[0,1]"); // official: "Crossover Probability (CR)"
			MyDoubleSpinBox* CR_DE_SpinBox = new MyDoubleSpinBox(this, false);
				CR_DE_SpinBox->setRange(0,1);
				CR_DE_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				CR_DE_SpinBox->setDecimals(8);
				CR_DE_SpinBox->setSingleStep(0.01);
				CR_DE_SpinBox->setValue(fitting_Settings->CR_DE);
				CR_DE_SpinBox->setAccelerated(true);
				CR_DE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(CR_DE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->CR_DE = CR_DE_SpinBox->value();
			});

			// F
			QLabel* F_DE_Label = new QLabel("Differential weight F"+Element_Sym+"[0,2]"); // official: "Differential weight (F)"
			MyDoubleSpinBox* F_DE_SpinBox = new MyDoubleSpinBox(this, false);
				F_DE_SpinBox->setRange(0,2);
				F_DE_SpinBox->setDecimals(8);
				F_DE_SpinBox->setSingleStep(0.1);
				F_DE_SpinBox->setValue(fitting_Settings->F_DE);
				F_DE_SpinBox->setAccelerated(true);
				F_DE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(F_DE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->F_DE = F_DE_SpinBox->value();
			});

			page_Layout->addWidget(NP_DE_Label,0,0);
			page_Layout->addWidget(NP_DE_SpinBox,0,1);

			page_Layout->addWidget(CR_DE_Label,1,0);
			page_Layout->addWidget(CR_DE_SpinBox,1,1);

			page_Layout->addWidget(F_DE_Label,2,0);
			page_Layout->addWidget(F_DE_SpinBox,2,1);
		}
		Differential_Evolution_Suite;
		{
			/// look at Methods/DESuite.c in SO source code and at page 29 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Differential_Evolution_Suite,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// NP
			QLabel* NP_DES_Label = new QLabel("Swarm size NP"+Element_Sym+"[4,500]"); // official: "Number of agents NP"
			MyDoubleSpinBox* NP_DES_SpinBox = new MyDoubleSpinBox(this, false);
				NP_DES_SpinBox->setRange(3,500);
				NP_DES_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				NP_DES_SpinBox->setDecimals(0);
				NP_DES_SpinBox->setSingleStep(1);
				NP_DES_SpinBox->setValue(fitting_Settings->NP_DES);
				NP_DES_SpinBox->setAccelerated(true);
				NP_DES_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(NP_DES_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->NP_DES = NP_DES_SpinBox->value();
			});

			// CR
			QLabel* CR_DES_Label = new QLabel("Crossover probability CR"+Element_Sym+"[0,1]"); // official: "Crossover Probability (CR)"
			MyDoubleSpinBox* CR_DES_SpinBox = new MyDoubleSpinBox(this, false);
				CR_DES_SpinBox->setRange(0,1);
				CR_DES_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				CR_DES_SpinBox->setDecimals(8);
				CR_DES_SpinBox->setSingleStep(0.01);
				CR_DES_SpinBox->setValue(fitting_Settings->CR_DES);
				CR_DES_SpinBox->setAccelerated(true);
				CR_DES_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(CR_DES_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->CR_DES = CR_DES_SpinBox->value();
			});

			// F
			QLabel* F_DES_Label = new QLabel("Differential weight F"+Element_Sym+"[0,2]"); // official: "Differential weight (F) (note different order from DE (Basic))"
			MyDoubleSpinBox* F_DES_SpinBox = new MyDoubleSpinBox(this, false);
				F_DES_SpinBox->setRange(0,2);
				F_DES_SpinBox->setDecimals(8);
				F_DES_SpinBox->setSingleStep(0.01);
				F_DES_SpinBox->setValue(fitting_Settings->F_DES);
				F_DES_SpinBox->setAccelerated(true);
				F_DES_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(F_DES_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->F_DES = F_DES_SpinBox->value();
			});

			page_Layout->addWidget(NP_DES_Label,0,0);
			page_Layout->addWidget(NP_DES_SpinBox,0,1);

			page_Layout->addWidget(CR_DES_Label,1,0);
			page_Layout->addWidget(CR_DES_SpinBox,1,1);

			page_Layout->addWidget(F_DES_Label,2,0);
			page_Layout->addWidget(F_DES_SpinBox,2,1);
		}
		DE_with_Temporal_Parameters;
		{
			/// look at Methods/DETP.c in SO source code
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(DE_with_Temporal_Parameters,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// NP
			QLabel* NP_DETP_Label = new QLabel("Swarm size NP"+Element_Sym+"[4,500]"); // official: "Number of agents NP"
			MyDoubleSpinBox* NP_DETP_SpinBox = new MyDoubleSpinBox(this, false);
				NP_DETP_SpinBox->setRange(4,500);
				NP_DETP_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				NP_DETP_SpinBox->setDecimals(0);
				NP_DETP_SpinBox->setSingleStep(1);
				NP_DETP_SpinBox->setValue(fitting_Settings->NP_DETP);
				NP_DETP_SpinBox->setAccelerated(true);
				NP_DETP_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(NP_DETP_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->NP_DETP = NP_DETP_SpinBox->value();
			});

			// CR_1
			QLabel* CR_1_DETP_Label = new QLabel("Crossover probability CR1"+Element_Sym+"[0,1]"); // official: "Crossover probability (CR1)"
			MyDoubleSpinBox* CR_1_DETP_SpinBox = new MyDoubleSpinBox(this, false);
				CR_1_DETP_SpinBox->setRange(0,1);
				CR_1_DETP_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				CR_1_DETP_SpinBox->setDecimals(8);
				CR_1_DETP_SpinBox->setSingleStep(0.01);
				CR_1_DETP_SpinBox->setValue(fitting_Settings->CR_1_DETP);
				CR_1_DETP_SpinBox->setAccelerated(true);
				CR_1_DETP_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(CR_1_DETP_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->CR_1_DETP = CR_1_DETP_SpinBox->value();
			});

			// CR_2
			QLabel* CR_2_DETP_Label = new QLabel("Crossover probability CR2"+Element_Sym+"[0,1]"); // official: "Crossover probability (CR2)"
			MyDoubleSpinBox* CR_2_DETP_SpinBox = new MyDoubleSpinBox(this, false);
				CR_2_DETP_SpinBox->setRange(0,1);
				CR_2_DETP_SpinBox->setDecimals(8);
				CR_2_DETP_SpinBox->setSingleStep(0.01);
				CR_2_DETP_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				CR_2_DETP_SpinBox->setValue(fitting_Settings->CR_2_DETP);
				CR_2_DETP_SpinBox->setAccelerated(true);
				CR_2_DETP_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(CR_2_DETP_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->CR_2_DETP = CR_2_DETP_SpinBox->value();
			});

			// F_1
			QLabel* F_1_DETP_Label = new QLabel("Differential weight F1"+Element_Sym+"[0,2]"); // official: "Differential weight (F1)"
			MyDoubleSpinBox* F_1_DETP_SpinBox = new MyDoubleSpinBox(this, false);
				F_1_DETP_SpinBox->setRange(0,2);
				F_1_DETP_SpinBox->setDecimals(8);
				F_1_DETP_SpinBox->setSingleStep(0.01);
				F_1_DETP_SpinBox->setValue(fitting_Settings->F_1_DETP);
				F_1_DETP_SpinBox->setAccelerated(true);
				F_1_DETP_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(F_1_DETP_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->F_1_DETP = F_1_DETP_SpinBox->value();
			});

			// F_2
			QLabel* F_2_DETP_Label = new QLabel("Differential weight F2"+Element_Sym+"[0,2]"); // official: "Differential weight (F2)"
			MyDoubleSpinBox* F_2_DETP_SpinBox = new MyDoubleSpinBox(this, false);
				F_2_DETP_SpinBox->setRange(0,2);
				F_2_DETP_SpinBox->setDecimals(8);
				F_2_DETP_SpinBox->setSingleStep(0.01);
				F_2_DETP_SpinBox->setValue(fitting_Settings->F_2_DETP);
				F_2_DETP_SpinBox->setAccelerated(true);
				F_2_DETP_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(F_2_DETP_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->F_2_DETP = F_2_DETP_SpinBox->value();
			});

			page_Layout->addWidget(NP_DETP_Label,0,0);
			page_Layout->addWidget(NP_DETP_SpinBox,0,1);

			page_Layout->addWidget(CR_1_DETP_Label,1,0);
			page_Layout->addWidget(CR_1_DETP_SpinBox,1,1);

			page_Layout->addWidget(CR_2_DETP_Label,2,0);
			page_Layout->addWidget(CR_2_DETP_SpinBox,2,1);

			page_Layout->addWidget(F_1_DETP_Label,3,0);
			page_Layout->addWidget(F_1_DETP_SpinBox,3,1);

			page_Layout->addWidget(F_2_DETP_Label,4,0);
			page_Layout->addWidget(F_2_DETP_SpinBox,4,1);
		}
		Jan_Differential_Evolution;
		{
			/// look at Methods/JDE.c in SO source code
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Jan_Differential_Evolution,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// NP
			QLabel* NP_JDE_Label = new QLabel("Swarm size NP"+Element_Sym+"[4,500]"); // official: "Number of agents NP"
			MyDoubleSpinBox* NP_JDE_SpinBox = new MyDoubleSpinBox(this, false);
				NP_JDE_SpinBox->setRange(4,500);
				NP_JDE_SpinBox->setDecimals(0);
				NP_JDE_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				NP_JDE_SpinBox->setSingleStep(1);
				NP_JDE_SpinBox->setValue(fitting_Settings->NP_JDE);
				NP_JDE_SpinBox->setAccelerated(true);
				NP_JDE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(NP_JDE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->NP_JDE = NP_JDE_SpinBox->value();
			});

			// F_Init
			QLabel* F_Init_JDE_Label = new QLabel("Differential weight, initial F_Init"+Element_Sym+"[0,2]"); // official: "FInit (Differential weight, initial value)"
			MyDoubleSpinBox* F_Init_JDE_SpinBox = new MyDoubleSpinBox(this, false);
				F_Init_JDE_SpinBox->setRange(0,2);
				F_Init_JDE_SpinBox->setDecimals(8);
				F_Init_JDE_SpinBox->setSingleStep(0.01);
				F_Init_JDE_SpinBox->setValue(fitting_Settings->F_Init_JDE);
				F_Init_JDE_SpinBox->setAccelerated(true);
				F_Init_JDE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(F_Init_JDE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->F_Init_JDE = F_Init_JDE_SpinBox->value();
			});

			// F_l
			QLabel* F_l_JDE_Label = new QLabel("Differential weight, min F_l"+Element_Sym+"[0,2]"); // official: "Fl"
			MyDoubleSpinBox* F_l_JDE_SpinBox = new MyDoubleSpinBox(this, false);
				F_l_JDE_SpinBox->setRange(0,2);
				F_l_JDE_SpinBox->setDecimals(8);
				F_l_JDE_SpinBox->setSingleStep(0.01);
				F_l_JDE_SpinBox->setValue(fitting_Settings->F_l_JDE);
				F_l_JDE_SpinBox->setAccelerated(true);
				F_l_JDE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(F_l_JDE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->F_l_JDE = F_l_JDE_SpinBox->value();
			});

			// F_u
			QLabel* F_u_JDE_Label = new QLabel("Differential weight, max F_u"+Element_Sym+"[0,2]"); // official: "Fu"
			MyDoubleSpinBox* F_u_JDE_SpinBox = new MyDoubleSpinBox(this, false);
				F_u_JDE_SpinBox->setRange(0,1);
				F_u_JDE_SpinBox->setDecimals(8);
				F_u_JDE_SpinBox->setSingleStep(0.01);
				F_u_JDE_SpinBox->setValue(fitting_Settings->F_u_JDE);
				F_u_JDE_SpinBox->setAccelerated(true);
				F_u_JDE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(F_u_JDE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->F_u_JDE = F_u_JDE_SpinBox->value();
			});

			// Tau_F
			QLabel* Tau_F_JDE_Label = new QLabel("tau_F"+Element_Sym+"[0,1]"); // official: "TauF (aka. Tau1)"
			MyDoubleSpinBox* Tau_F_JDE_SpinBox = new MyDoubleSpinBox(this, false);
				Tau_F_JDE_SpinBox->setRange(0,1);
				Tau_F_JDE_SpinBox->setDecimals(8);
				Tau_F_JDE_SpinBox->setSingleStep(0.01);
				Tau_F_JDE_SpinBox->setValue(fitting_Settings->Tau_F_JDE);
				Tau_F_JDE_SpinBox->setAccelerated(true);
				Tau_F_JDE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(Tau_F_JDE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->Tau_F_JDE = Tau_F_JDE_SpinBox->value();
			});

			// CR_Init
			QLabel* CR_Init_JDE_Label = new QLabel("Crossover probability, init CR_Init"+Element_Sym+"[0,1]"); // official: "CRInit (Crossover probability, initial value)"
			MyDoubleSpinBox* CR_Init_JDE_SpinBox = new MyDoubleSpinBox(this, false);
				CR_Init_JDE_SpinBox->setRange(0,1);
				CR_Init_JDE_SpinBox->setDecimals(8);
				CR_Init_JDE_SpinBox->setSingleStep(0.01);
				CR_Init_JDE_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				CR_Init_JDE_SpinBox->setValue(fitting_Settings->CR_Init_JDE);
				CR_Init_JDE_SpinBox->setAccelerated(true);
				CR_Init_JDE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(CR_Init_JDE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->CR_Init_JDE = CR_Init_JDE_SpinBox->value();
			});

			// CR_l
			QLabel* CR_l_JDE_Label = new QLabel("Crossover probability, min CR_l"+Element_Sym+"[0,1]"); // official: "CRl"
			MyDoubleSpinBox* CR_l_JDE_SpinBox = new MyDoubleSpinBox(this, false);
				CR_l_JDE_SpinBox->setRange(0,1);
				CR_l_JDE_SpinBox->setDecimals(8);
				CR_l_JDE_SpinBox->setSingleStep(0.01);
				CR_l_JDE_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				CR_l_JDE_SpinBox->setValue(fitting_Settings->CR_l_JDE);
				CR_l_JDE_SpinBox->setAccelerated(true);
				CR_l_JDE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(CR_l_JDE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->CR_l_JDE = CR_l_JDE_SpinBox->value();
			});

			// CR_u
			QLabel* CR_u_JDE_Label = new QLabel("Crossover probability, max CR_u"+Element_Sym+"[0,1]"); // official: "CRu"
			MyDoubleSpinBox* CR_u_JDE_SpinBox = new MyDoubleSpinBox(this, false);
				CR_u_JDE_SpinBox->setRange(0,1);
				CR_u_JDE_SpinBox->setDecimals(8);
				CR_u_JDE_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				CR_u_JDE_SpinBox->setSingleStep(0.01);
				CR_u_JDE_SpinBox->setValue(fitting_Settings->CR_u_JDE);
				CR_u_JDE_SpinBox->setAccelerated(true);
				CR_u_JDE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(CR_u_JDE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->CR_u_JDE = CR_u_JDE_SpinBox->value();
			});

			// Tau_CR
			QLabel* Tau_CR_JDE_Label = new QLabel("tau_CR"+Element_Sym+"[0,1]"); // official: "TauCR (aka. Tau2)"
			MyDoubleSpinBox* Tau_CR_JDE_SpinBox = new MyDoubleSpinBox(this, false);
				Tau_CR_JDE_SpinBox->setRange(0,1);
				Tau_CR_JDE_SpinBox->setDecimals(8);
				Tau_CR_JDE_SpinBox->setSingleStep(0.01);
				Tau_CR_JDE_SpinBox->setValue(fitting_Settings->Tau_CR_JDE);
				Tau_CR_JDE_SpinBox->setAccelerated(true);
				Tau_CR_JDE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(Tau_CR_JDE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->Tau_CR_JDE = Tau_CR_JDE_SpinBox->value();
			});
			int p=0;
			page_Layout->addWidget(NP_JDE_Label,p,0);
			page_Layout->addWidget(NP_JDE_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(F_Init_JDE_Label,p,0);
			page_Layout->addWidget(F_Init_JDE_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(F_l_JDE_Label,p,0);
			page_Layout->addWidget(F_l_JDE_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(F_u_JDE_Label,p,0);
			page_Layout->addWidget(F_u_JDE_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(Tau_F_JDE_Label,p,0);
			page_Layout->addWidget(Tau_F_JDE_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(CR_Init_JDE_Label,p,0);
			page_Layout->addWidget(CR_Init_JDE_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(CR_l_JDE_Label,p,0);
			page_Layout->addWidget(CR_l_JDE_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(CR_u_JDE_Label,p,0);
			page_Layout->addWidget(CR_u_JDE_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(Tau_CR_JDE_Label,p,0);
			page_Layout->addWidget(Tau_CR_JDE_SpinBox,p,1);
		}
		Evolution_by_Lingering_Global_Best;
		{
			/// look at Methods/ELG.c in SO source code and at page 29 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Evolution_by_Lingering_Global_Best,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// NP
			QLabel* NP_ELG_Label = new QLabel("Swarm size NP"+Element_Sym+"[2,500]"); // official: "Number of agents NP"
			MyDoubleSpinBox* NP_ELG_SpinBox = new MyDoubleSpinBox(this, false);
				NP_ELG_SpinBox->setRange(2,500);
				NP_ELG_SpinBox->setDecimals(0);
				NP_ELG_SpinBox->setSingleStep(1);
				NP_ELG_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				NP_ELG_SpinBox->setValue(fitting_Settings->NP_ELG);
				NP_ELG_SpinBox->setAccelerated(true);
				NP_ELG_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(NP_ELG_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->NP_ELG = NP_ELG_SpinBox->value();
			});

			page_Layout->addWidget(NP_ELG_Label,0,0);
			page_Layout->addWidget(NP_ELG_SpinBox,0,1);
		}
		More_Yo_yos_Doing_Global_optimization;
		{
			/// look at Methods/MYG.c in SO source code and at page 30 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(More_Yo_yos_Doing_Global_optimization,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// NP
			QLabel* NP_MYG_Label = new QLabel("Swarm size NP"+Element_Sym+"[3,500]"); // official: "Number of agents NP"
			MyDoubleSpinBox* NP_MYG_SpinBox = new MyDoubleSpinBox(this, false);
				NP_MYG_SpinBox->setRange(3,500);
				NP_MYG_SpinBox->setDecimals(0);
				NP_MYG_SpinBox->setSingleStep(1);
				NP_MYG_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				NP_MYG_SpinBox->setValue(fitting_Settings->NP_MYG);
				NP_MYG_SpinBox->setAccelerated(true);
				NP_MYG_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(NP_MYG_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->NP_MYG = NP_MYG_SpinBox->value();
			});

			// F
			QLabel* F_MYG_Label = new QLabel("Differential weight F"+Element_Sym+"[0.5,2]"); // official: "Differential weight F, aka. alpha."
			MyDoubleSpinBox* F_MYG_SpinBox = new MyDoubleSpinBox(this, false);
				F_MYG_SpinBox->setRange(0.5,2);
				F_MYG_SpinBox->setDecimals(8);
				F_MYG_SpinBox->setSingleStep(0.01);
				F_MYG_SpinBox->setValue(fitting_Settings->F_MYG);
				F_MYG_SpinBox->setAccelerated(true);
				F_MYG_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(F_MYG_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->F_MYG = F_MYG_SpinBox->value();
			});

			page_Layout->addWidget(NP_MYG_Label,0,0);
			page_Layout->addWidget(NP_MYG_SpinBox,0,1);

			page_Layout->addWidget(F_MYG_Label,1,0);
			page_Layout->addWidget(F_MYG_SpinBox,1,1);
		}
		Particle_Swarm_Optimization;
		{
			/// look at Methods/PSO.c in SO source code and at page 31 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Particle_Swarm_Optimization,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// S
			QLabel* S_PSO_Label = new QLabel("Swarm size S"+Element_Sym+"[1,500]"); // official: "Number of agents"
			MyDoubleSpinBox* S_PSO_SpinBox = new MyDoubleSpinBox(this, false);
				S_PSO_SpinBox->setRange(1,500);
				S_PSO_SpinBox->setDecimals(0);
				S_PSO_SpinBox->setSingleStep(1);
				S_PSO_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				S_PSO_SpinBox->setValue(fitting_Settings->S_PSO);
				S_PSO_SpinBox->setAccelerated(true);
				S_PSO_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(S_PSO_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->S_PSO = S_PSO_SpinBox->value();
			});

			// omega
			QLabel* omega_PSO_Label = new QLabel("Inertia weight "+Omega_Sym+Element_Sym+"[-2,2]"); // official: "Inertia weight"
			MyDoubleSpinBox* omega_PSO_SpinBox = new MyDoubleSpinBox(this, false);
				omega_PSO_SpinBox->setRange(-2,2);
				omega_PSO_SpinBox->setDecimals(8);
				omega_PSO_SpinBox->setSingleStep(0.1);
				omega_PSO_SpinBox->setValue(fitting_Settings->omega_PSO);
				omega_PSO_SpinBox->setAccelerated(true);
				omega_PSO_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(omega_PSO_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->omega_PSO = omega_PSO_SpinBox->value();
			});

			// phi_p
			QLabel* phi_p_PSO_Label = new QLabel("Weight on particle-best attraction "+Phi_Sym+"_p"+Element_Sym+"[-4,4]"); // official: "Weight on particle-best attraction"
			MyDoubleSpinBox* phi_p_PSO_SpinBox = new MyDoubleSpinBox(this, false);
				phi_p_PSO_SpinBox->setRange(-4,4);
				phi_p_PSO_SpinBox->setDecimals(8);
				phi_p_PSO_SpinBox->setSingleStep(0.1);
				phi_p_PSO_SpinBox->setValue(fitting_Settings->phi_p_PSO);
				phi_p_PSO_SpinBox->setAccelerated(true);
				phi_p_PSO_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(phi_p_PSO_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->phi_p_PSO = phi_p_PSO_SpinBox->value();
			});

			// phi_g
			QLabel* phi_g_PSO_Label = new QLabel("Weight on swarm-best attraction "+Phi_Sym+"_g"+Element_Sym+"[-4,4]"); // official: "Weight on swarm-best attraction"
			MyDoubleSpinBox* phi_g_PSO_SpinBox = new MyDoubleSpinBox(this, false);
				phi_g_PSO_SpinBox->setRange(-4,4);
				phi_g_PSO_SpinBox->setDecimals(8);
				phi_g_PSO_SpinBox->setSingleStep(0.1);
				phi_g_PSO_SpinBox->setValue(fitting_Settings->phi_g_PSO);
				phi_g_PSO_SpinBox->setAccelerated(true);
				phi_g_PSO_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(phi_g_PSO_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->phi_g_PSO = phi_g_PSO_SpinBox->value();
			});

			int p=0;
			page_Layout->addWidget(S_PSO_Label,p,0);
			page_Layout->addWidget(S_PSO_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(omega_PSO_Label,p,0);
			page_Layout->addWidget(omega_PSO_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(phi_p_PSO_Label,p,0);
			page_Layout->addWidget(phi_p_PSO_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(phi_g_PSO_Label,p,0);
			page_Layout->addWidget(phi_g_PSO_SpinBox,p,1);
		}
		Forever_Accumulating_Evolution;
		{
			/// look at Methods/FAE.c in SO source code and at page 33 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Forever_Accumulating_Evolution,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// S
			QLabel* S_FAE_Label = new QLabel("Swarm size S"+Element_Sym+"[1,200]"); // official: "Number of agents"
			MyDoubleSpinBox* S_FAE_SpinBox = new MyDoubleSpinBox(this, false);
				S_FAE_SpinBox->setRange(1,200);
				S_FAE_SpinBox->setDecimals(0);
				S_FAE_SpinBox->setSingleStep(1);
				S_FAE_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				S_FAE_SpinBox->setValue(fitting_Settings->S_FAE);
				S_FAE_SpinBox->setAccelerated(true);
				S_FAE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(S_FAE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->S_FAE = S_FAE_SpinBox->value();
			});

			// lambda_g
			QLabel* lambda_g_FAE_Label = new QLabel("Factor "+Lambda_Sym+"-g"+Element_Sym+"[-2,2]"); // official: "Lambda-g"
			MyDoubleSpinBox* lambda_g_FAE_SpinBox = new MyDoubleSpinBox(this, false);
				lambda_g_FAE_SpinBox->setRange(-2,2);
				lambda_g_FAE_SpinBox->setDecimals(8);
				lambda_g_FAE_SpinBox->setSingleStep(0.1);
				lambda_g_FAE_SpinBox->setValue(fitting_Settings->lambda_g_FAE);
				lambda_g_FAE_SpinBox->setAccelerated(true);
				lambda_g_FAE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(lambda_g_FAE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->lambda_g_FAE = lambda_g_FAE_SpinBox->value();
			});

			// lambda_x
			QLabel* lambda_x_FAE_Label = new QLabel("Factor "+Lambda_Sym+"-x"+Element_Sym+"[-8,-1]"); // official: "Lambda-x"
			MyDoubleSpinBox* lambda_x_FAE_SpinBox = new MyDoubleSpinBox(this, false);
				lambda_x_FAE_SpinBox->setRange(-8,-1);
				lambda_x_FAE_SpinBox->setDecimals(8);
				lambda_x_FAE_SpinBox->setSingleStep(0.01);
				lambda_x_FAE_SpinBox->setValue(fitting_Settings->lambda_x_FAE);
				lambda_x_FAE_SpinBox->setAccelerated(true);
				lambda_x_FAE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(lambda_x_FAE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->lambda_x_FAE = lambda_x_FAE_SpinBox->value();
			});

			page_Layout->addWidget(S_FAE_Label,0,0);
			page_Layout->addWidget(S_FAE_SpinBox,0,1);

			page_Layout->addWidget(lambda_g_FAE_Label,1,0);
			page_Layout->addWidget(lambda_g_FAE_SpinBox,1,1);

			page_Layout->addWidget(lambda_x_FAE_Label,2,0);
			page_Layout->addWidget(lambda_x_FAE_SpinBox,2,1);
		}
		Many_Optimizing_Liaisons;
		{
			/// look at Methods/MOL.c in SO source code and at page 32 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Many_Optimizing_Liaisons,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// S
			QLabel* S_MOL_Label = new QLabel("Swarm size S"+Element_Sym+"[1,500]"); // official: "Number of agents"
			MyDoubleSpinBox* S_MOL_SpinBox = new MyDoubleSpinBox(this, false);
				S_MOL_SpinBox->setRange(1,500);
				S_MOL_SpinBox->setDecimals(0);
				S_MOL_SpinBox->setSingleStep(1);
				S_MOL_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
				S_MOL_SpinBox->setValue(fitting_Settings->S_MOL);
				S_MOL_SpinBox->setAccelerated(true);
				S_MOL_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(S_MOL_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->S_MOL = S_MOL_SpinBox->value();
			});

			// omega
			QLabel* omega_MOL_Label = new QLabel("Inertia weight "+Omega_Sym+Element_Sym+"[-2,2]"); // official: "Inertia weight"
			MyDoubleSpinBox* omega_MOL_SpinBox = new MyDoubleSpinBox(this, false);
				omega_MOL_SpinBox->setRange(-2,2);
				omega_MOL_SpinBox->setDecimals(8);
				omega_MOL_SpinBox->setSingleStep(0.1);
				omega_MOL_SpinBox->setValue(fitting_Settings->omega_MOL);
				omega_MOL_SpinBox->setAccelerated(true);
				omega_MOL_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(omega_MOL_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->omega_MOL = omega_MOL_SpinBox->value();
			});

			// phi_g
			QLabel* phi_g_MOL_Label = new QLabel("Weight on swarm-best attraction "+Phi_Sym+"_g"+Element_Sym+"[-4,4]"); // official: "Weight on swarm-best attraction"
			MyDoubleSpinBox* phi_g_MOL_SpinBox = new MyDoubleSpinBox(this, false);
				phi_g_MOL_SpinBox->setRange(-4,4);
				phi_g_MOL_SpinBox->setDecimals(8);
				phi_g_MOL_SpinBox->setSingleStep(0.1);
				phi_g_MOL_SpinBox->setValue(fitting_Settings->phi_g_MOL);
				phi_g_MOL_SpinBox->setAccelerated(true);
				phi_g_MOL_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(phi_g_MOL_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->phi_g_MOL = phi_g_MOL_SpinBox->value();
			});

			int p=0;
			page_Layout->addWidget(S_MOL_Label,p,0);
			page_Layout->addWidget(S_MOL_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(omega_MOL_Label,p,0);
			page_Layout->addWidget(omega_MOL_SpinBox,p,1);
			p+=1;
			page_Layout->addWidget(phi_g_MOL_Label,p,0);
			page_Layout->addWidget(phi_g_MOL_SpinBox,p,1);
		}
		Layered_and_Interleaved_CoEvolution;
		{
			/// look at Methods/LICE.c in SO source code and at page 34 of SO manual
			QWidget* page_Widget = new QWidget;
			SO_Additional_Pages_Stack->addWidget(page_Widget);
			SO_Additional_Pages_Widgets.insert(Layered_and_Interleaved_CoEvolution,page_Widget);
			QGridLayout* page_Layout = new QGridLayout(page_Widget);
				page_Layout->setAlignment(Qt::AlignTop);

			// gamma_2
			QLabel* gamma_2_LICE_Label = new QLabel("Meta-power of decreasing range "+Gamma_Sym+"2"+Element_Sym+"[0.5,4]"); // official: "gamma2"
			MyDoubleSpinBox* gamma_2_LICE_SpinBox = new MyDoubleSpinBox(this, false);
				gamma_2_LICE_SpinBox->setRange(0.5,4);
				gamma_2_LICE_SpinBox->setDecimals(8);
				gamma_2_LICE_SpinBox->setSingleStep(0.1);
				gamma_2_LICE_SpinBox->setValue(fitting_Settings->gamma_2_LICE);
				gamma_2_LICE_SpinBox->setAccelerated(true);
				gamma_2_LICE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(gamma_2_LICE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->gamma_2_LICE = gamma_2_LICE_SpinBox->value();
			});

			// N
			QLabel* N_LICE_Label = new QLabel("Base layer iteration factor N"+Element_Sym+"[10,40]"); // official: "N"
			MyDoubleSpinBox* N_LICE_SpinBox = new MyDoubleSpinBox(this, false);
				N_LICE_SpinBox->setRange(10,40);
				N_LICE_SpinBox->setDecimals(0);
				N_LICE_SpinBox->setSingleStep(1);
				N_LICE_SpinBox->setValue(fitting_Settings->N_LICE);
				N_LICE_SpinBox->setAccelerated(true);
				N_LICE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(N_LICE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->N_LICE = N_LICE_SpinBox->value();
			});

			// gamma
			QLabel* gamma_LICE_Label = new QLabel("Power of decreasing range "+Gamma_Sym+Element_Sym+"[0.5,6]"); // official: "gamma"
			MyDoubleSpinBox* gamma_LICE_SpinBox = new MyDoubleSpinBox(this, false);
				gamma_LICE_SpinBox->setRange(0.5,6);
				gamma_LICE_SpinBox->setDecimals(8);
				gamma_LICE_SpinBox->setSingleStep(0.1);
				gamma_LICE_SpinBox->setValue(fitting_Settings->gamma_LICE);
				gamma_LICE_SpinBox->setAccelerated(true);
				gamma_LICE_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			connect(gamma_LICE_SpinBox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
			{
				fitting_Settings->gamma_LICE = gamma_LICE_SpinBox->value();
			});

			page_Layout->addWidget(gamma_2_LICE_Label,0,0);
			page_Layout->addWidget(gamma_2_LICE_SpinBox,0,1);

			page_Layout->addWidget(N_LICE_Label,1,0);
			page_Layout->addWidget(N_LICE_SpinBox,1,1);

			page_Layout->addWidget(gamma_LICE_Label,2,0);
			page_Layout->addWidget(gamma_LICE_SpinBox,2,1);
		}
	}
	{
		// create spoiler
		QVBoxLayout* frame_Layout = new QVBoxLayout;
			frame_Layout->addWidget(SO_Additional_Params_Group_Box);
		Spoiler* spoiler = new Spoiler("Additional parameters", 5, this);
			spoiler->setContentsMargins(-9,0,-9,-0);
			spoiler->setContentLayout(*frame_Layout);
		SO_Page_Layout->addWidget(spoiler);
	}
}

