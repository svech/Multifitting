// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "fitting_settings_editor.h"
#include "standard/spoiler.h"

Fitting_Settings_Editor::Fitting_Settings_Editor(QWidget* parent) :
	fitting_Settings(global_Multilayer_Approach->fitting_Settings),
	QWidget(parent) // nullptr!
{
	setWindowTitle("Fitting Settings");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Fitting_Settings_Editor::closeEvent(QCloseEvent *event)
{
	write_Window_Geometry();
	global_Multilayer_Approach->runned_Fitting_Settings_Editor.remove(fit_Settings_Key);
	event->accept();
}

void Fitting_Settings_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	create_Menu();
	create_Shortcuts();
	create_Metods();
	create_Pages();
	create_GSL_Main_Params_Group_Box();
	create_GSL_AdditionalParams_Group_Box();
	create_SO_Main_Params_Group_Box();
	create_SO_AdditionalParams_Group_Box();
	{
		done_Button = new QPushButton("Done");
			done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			done_Button->setFocus();
			done_Button->setDefault(true);
			connect(done_Button, &QPushButton::clicked, this, &Fitting_Settings_Editor::close);

		QHBoxLayout* button_Layout = new QHBoxLayout;
//			button_Layout->setSpacing(0);
			button_Layout->addWidget(done_Button);

		main_Layout->addLayout(button_Layout);
	}
	methods_Combo_Box->currentIndexChanged(methods_Combo_Box->currentIndex());
	interface_Created = true;
}

void Fitting_Settings_Editor::set_Window_Geometry()
{
	setGeometry(fitting_settings_x_corner,fitting_settings_y_corner,width(),height());
	adjustSize();
//#ifdef _WIN32
//#endif
//#ifdef __linux__
//	setGeometry(768,0,width(),height());
//#endif
	setFixedSize(size());
}

void Fitting_Settings_Editor::write_Window_Geometry()
{
	if(!isMaximized())
	{
		fitting_settings_x_corner = geometry().x()-WINDOW_BOUNDARY_SHIFT_X;
		fitting_settings_y_corner = geometry().y()-WINDOW_BOUNDARY_SHIFT_Y;
	}
}

void Fitting_Settings_Editor::create_Menu()
{
	Menu* menu = new Menu(window_Type_Fitting_Settings_Editor, this);
	main_Layout->setMenuBar(menu->menu_Bar);
}

void Fitting_Settings_Editor::create_Shortcuts()
{
	// shortcuts
	{
		QShortcut* save_Shortcut			= new QShortcut(QKeySequence(Qt::Key_S | Qt::CTRL), this);
		QShortcut* open_Shortcut			= new QShortcut(QKeySequence(Qt::Key_O | Qt::CTRL), this);
//		QShortcut* fit_Shortcut				= new QShortcut(QKeySequence(Qt::Key_F | Qt::CTRL | Qt::SHIFT), this);
//		QShortcut* calc_Specular_Shortcut	= new QShortcut(QKeySequence(Qt::Key_C | Qt::CTRL | Qt::SHIFT), this);

		connect(save_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->save(default_File);});
		connect(open_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->open(default_File);});
//		connect(fit_Shortcut,			&QShortcut::activated, this, [=]{ global_Multilayer_Approach->start_Fitting();	  });
//		connect(calc_Specular_Shortcut, &QShortcut::activated, this, [=]{ global_Multilayer_Approach->calc_Reflection(); });
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

	QString spacer = "\t";

	// fill GSL group
	{
		methods_Combo_Box->addItem(spacer + GSL_Methods[Levenberg_Marquardt]);
		methods_Combo_Box->addItem(spacer + GSL_Methods[Levenberg_Marquardt_with_Geodesic_Acceleration]);
		methods_Combo_Box->addItem(spacer + GSL_Methods[Dogleg]);
		methods_Combo_Box->addItem(spacer + GSL_Methods[Double_Dogleg]);
		methods_Combo_Box->addItem(spacer + GSL_Methods[Two_Dimensional_Subspace]);
		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());

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
		methods_Combo_Box->addItem(spacer + SO_Methods[Random_Sampling_Uniform]				 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Gradient_Descent]					 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Gradient_Emancipated_Descent]		 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Hill_Climber]						 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Simulated_Annealing]					 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Pattern_Search]						 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Local_Unimodal_Sampling]				 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Differential_Evolution]				 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Differential_Evolution_Suite]		 );
		methods_Combo_Box->addItem(spacer + SO_Methods[DE_with_Temporal_Parameters]			 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Jan_Differential_Evolution]			 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Evolution_by_Lingering_Global_Best]	 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[More_Yo_yos_Doing_Global_optimization]);
//		methods_Combo_Box->addItem(spacer + SO_Methods[Particle_Swarm_Optimization]			 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Forever_Accumulating_Evolution]		 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Many_Optimizing_Liaisons]			 );
//		methods_Combo_Box->addItem(spacer + SO_Methods[Layered_and_Interleaved_CoEvolution]	 );

		methods_Combo_Box->insertSeparator(methods_Combo_Box->count());
	}

	methods_Combo_Box->setMaxVisibleItems(methods_Combo_Box->count());
	methods_Combo_Box->setCurrentIndex(methods_Combo_Box->findText(spacer + fitting_Settings->current_Method));
	methods_Combo_Box->adjustSize();
	methods_Combo_Box->setFixedWidth(methods_Combo_Box->width()+90);

	// skip group headers
	connect(methods_Combo_Box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]
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
		if(	GSL_Methods.contains(fitting_Settings->current_Method) && GSL_Page)	pages_Stack->setCurrentWidget(GSL_Page);
		if(	SO_Methods. contains(fitting_Settings->current_Method) && SO_Page )	pages_Stack->setCurrentWidget(SO_Page);

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
			GSL_num_Runs_SpinBox->setValue(fitting_Settings->num_Runs);
			GSL_num_Runs_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			GSL_num_Runs_SpinBox->setAccelerated(true);
			GSL_num_Runs_SpinBox->setRange(1, MAX_INTEGER);
			GSL_num_Runs_SpinBox->setEnabled(fitting_Settings->randomized_Start);

		GSL_max_Iter_Label = new QLabel("Max number of iterations");
		GSL_max_Iter_SpinBox = new QSpinBox;
			GSL_max_Iter_SpinBox->setValue(fitting_Settings->max_Iter);
			GSL_max_Iter_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			GSL_max_Iter_SpinBox->setAccelerated(true);
			GSL_max_Iter_SpinBox->setRange(1,MAX_INTEGER);

		GSL_common_Tolerance_Label = new QLabel("General tolerance");
		GSL_common_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->x_Tolerance));
			GSL_common_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));


		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_randomized_Start_Check_Box,	0,0,1,2);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_num_Runs_Label,				1,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_num_Runs_SpinBox,			1,1,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_max_Iter_Label,				2,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_max_Iter_SpinBox,			2,1,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_common_Tolerance_Label,		3,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(GSL_common_Tolerance_Line_Edit,	3,1,1,1);

		connect(GSL_randomized_Start_Check_Box,	&QCheckBox::toggled,	 fitting_Settings, [=]
		{
			fitting_Settings->randomized_Start = GSL_randomized_Start_Check_Box->isChecked();
			GSL_num_Runs_Label->setEnabled(fitting_Settings->randomized_Start);
			GSL_num_Runs_SpinBox->setEnabled(fitting_Settings->randomized_Start);
		});
		connect(GSL_num_Runs_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), fitting_Settings, [=]{fitting_Settings->num_Runs = GSL_num_Runs_SpinBox->value();});
		connect(GSL_max_Iter_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), fitting_Settings, [=]{fitting_Settings->max_Iter = GSL_max_Iter_SpinBox->value();	});
		connect(GSL_common_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]
		{
			fitting_Settings->x_Tolerance = GSL_common_Tolerance_Line_Edit->text().toDouble();
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
		GSL_x_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->x_Tolerance));
			GSL_x_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_g_Tolerance_Label = new QLabel("Gradient tolerance");
		GSL_g_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->g_Tolerance));
			GSL_g_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_f_Tolerance_Label = new QLabel("Residual tolerance");
		GSL_f_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->f_Tolerance));
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
		GSL_factor_up_Line_Edit = new QLineEdit(QString::number(fitting_Settings->factor_Up));
			GSL_factor_up_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_factor_down_Label = new QLabel("Factor for decreasing trust radius (default 2)");
		GSL_factor_down_Line_Edit = new QLineEdit(QString::number(fitting_Settings->factor_Down));
			GSL_factor_down_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_avmax_Label = new QLabel("Max allowed |a|/|v| (default 0.75)");
		GSL_avmax_Line_Edit = new QLineEdit(QString::number(fitting_Settings->avmax));
			GSL_avmax_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_h_df_Label = new QLabel("Step size for finite difference J (default ~1.5e-8)");
		GSL_h_df_Line_Edit = new QLineEdit(QString::number(fitting_Settings->h_df));
			GSL_h_df_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_h_fvv_Label = new QLabel("Step size for finite difference fvv (default 0.02)");
		GSL_h_fvv_Line_Edit = new QLineEdit(QString::number(fitting_Settings->h_fvv));
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

		connect(GSL_x_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->x_Tolerance = GSL_x_Tolerance_Line_Edit->text().toDouble();	});
		connect(GSL_g_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->g_Tolerance = GSL_g_Tolerance_Line_Edit->text().toDouble();	});
		connect(GSL_f_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->f_Tolerance = GSL_f_Tolerance_Line_Edit->text().toDouble();	});

		connect(GSL_scale_Combo_Box,  static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]{fitting_Settings->current_Scale  = GSL_scale_Combo_Box->currentText(); });
		connect(GSL_solver_Combo_Box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]{fitting_Settings->current_Solver = GSL_solver_Combo_Box->currentText();});
		connect(GSL_fdtype_Combo_Box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]{fitting_Settings->current_Fdtype = GSL_fdtype_Combo_Box->currentText();});

		connect(GSL_factor_up_Line_Edit,   &QLineEdit::textChanged, [=]{fitting_Settings->factor_Up		= GSL_factor_up_Line_Edit->text().toDouble();	});
		connect(GSL_factor_down_Line_Edit, &QLineEdit::textChanged, [=]{fitting_Settings->factor_Down	= GSL_factor_down_Line_Edit->text().toDouble();	});
		connect(GSL_avmax_Line_Edit,	   &QLineEdit::textChanged, [=]{fitting_Settings->avmax			= GSL_avmax_Line_Edit->text().toDouble();		});
		connect(GSL_h_df_Line_Edit,		   &QLineEdit::textChanged, [=]{fitting_Settings->h_df			= GSL_h_df_Line_Edit->text().toDouble();		});
		connect(GSL_h_fvv_Line_Edit,	   &QLineEdit::textChanged, [=]{fitting_Settings->h_fvv			= GSL_h_fvv_Line_Edit->text().toDouble();		});
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
			SO_num_Runs_SpinBox->setEnabled(fitting_Settings->randomized_Start);
			SO_num_Runs_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			SO_num_Runs_SpinBox->setAccelerated(true);
			SO_num_Runs_SpinBox->setValue(fitting_Settings->num_Runs);
			SO_num_Runs_SpinBox->setRange(1, MAX_INTEGER);

		SO_max_Evaluations_Label = new QLabel("Max number of evaluations");
			SO_max_Evaluations_Label->setDisabled(fitting_Settings->max_Eval_Check);
		SO_max_Evaluations_SpinBox = new QSpinBox;
			SO_max_Evaluations_SpinBox->setDisabled(fitting_Settings->max_Eval_Check);
			SO_max_Evaluations_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			SO_max_Evaluations_SpinBox->setAccelerated(true);
			SO_max_Evaluations_SpinBox->setValue(fitting_Settings->max_Evaluations);
			SO_max_Evaluations_SpinBox->setRange(1, MAX_INTEGER);

		SO_max_Eval_Check_Box = new QCheckBox("Num. evals "+Proportional_Sym+" num. params",this);
			SO_max_Eval_Check_Box->setChecked(fitting_Settings->max_Eval_Check);
		SO_max_Eval_Factor_SpinBox = new QSpinBox;
			SO_max_Eval_Factor_SpinBox->setEnabled(fitting_Settings->max_Eval_Check);
			SO_max_Eval_Factor_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			SO_max_Eval_Factor_SpinBox->setAccelerated(true);
			SO_max_Eval_Factor_SpinBox->setValue(fitting_Settings->max_Eval_Factor);
			SO_max_Eval_Factor_SpinBox->setRange(1, MAX_INTEGER);

		SO_Fit_Params_Group_Box_Layout->addWidget(SO_randomized_Start_Check_Box,		0,0,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_initialize_By_Current_Check_Box,	0,1,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_num_Runs_Label,					1,0,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_num_Runs_SpinBox,					1,1,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_max_Evaluations_Label,				2,0,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_max_Evaluations_SpinBox,			2,1,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_max_Eval_Check_Box,				3,0,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(SO_max_Eval_Factor_SpinBox,			3,1,1,1);

		connect(SO_randomized_Start_Check_Box,&QCheckBox::toggled,	   fitting_Settings, [=]
		{
			fitting_Settings->randomized_Start = SO_randomized_Start_Check_Box->isChecked();
			SO_num_Runs_Label->setEnabled(fitting_Settings->randomized_Start);
			SO_num_Runs_SpinBox->setEnabled(fitting_Settings->randomized_Start);
		});
		connect(SO_initialize_By_Current_Check_Box,&QCheckBox::toggled,	   fitting_Settings, [=]
		{
			fitting_Settings->initialize_By_Current_State = SO_initialize_By_Current_Check_Box->isChecked();
		});
		connect(SO_num_Runs_SpinBox,		static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), fitting_Settings, [=]{fitting_Settings->num_Runs		  = SO_num_Runs_SpinBox->	    value();});
		connect(SO_max_Evaluations_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), fitting_Settings, [=]{fitting_Settings->max_Evaluations = SO_max_Evaluations_SpinBox->value();});
		connect(SO_max_Eval_Factor_SpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), fitting_Settings, [=]{fitting_Settings->max_Eval_Factor = SO_max_Eval_Factor_SpinBox->value();});
		connect(SO_max_Eval_Check_Box,		&QCheckBox::toggled,	 fitting_Settings, [=]
		{
			fitting_Settings->max_Eval_Check = SO_max_Eval_Check_Box->isChecked();
			SO_max_Evaluations_Label->setDisabled(fitting_Settings->max_Eval_Check);
			SO_max_Evaluations_SpinBox->setDisabled(fitting_Settings->max_Eval_Check);
			SO_max_Eval_Factor_SpinBox->setEnabled(fitting_Settings->max_Eval_Check);
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
		SO_Additional_Params_Group_Box_Layout = new QGridLayout(SO_Additional_Params_Group_Box);
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

