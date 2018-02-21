#include "fitting_settings_editor.h"
#include "standard/spoiler.h"

Fitting_Settings_Editor::Fitting_Settings_Editor(Multilayer_Approach* multilayer_Approach, QWidget* parent) :
	multilayer_Approach(multilayer_Approach),
	fitting_Settings(multilayer_Approach->fitting_Settings),
	QWidget(parent)
{
	setWindowTitle("Fitting Settings");
	create_Main_Layout();
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Fitting_Settings_Editor::closeEvent(QCloseEvent *event)
{
	multilayer_Approach->runned_Fitting_Settings_Editor.remove(fit_Settings_Key);
	event->accept();
}

void Fitting_Settings_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	create_Menu();
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
}

void Fitting_Settings_Editor::set_Window_Geometry()
{
	adjustSize();
	setGeometry(430,498,width(),height());
	setFixedSize(size());
}

void Fitting_Settings_Editor::create_Menu()
{
	Menu* menu = new Menu(window_Type_Fitting_Settings_Editor, this);
	main_Layout->setMenuBar(menu->menu_Bar);
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
		methods_Combo_Box->addItem(spacer + SO_Methods[Gradient_Descent]					 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Gradient_Emancipated_Descent]		 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Hill_Climber]						 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Simulated_Annealing]					 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Pattern_Search]						 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Local_Unimodal_Sampling]				 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Differential_Evolution]				 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Differential_Evolution_Suite]		 );
		methods_Combo_Box->addItem(spacer + SO_Methods[DE_with_Temporal_Parameters]			 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Jan_Differential_Evolution]			 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Evolution_by_Lingering_Global_Best]	 );
		methods_Combo_Box->addItem(spacer + SO_Methods[More_Yo_yos_Doing_Global_optimization]);
		methods_Combo_Box->addItem(spacer + SO_Methods[Particle_Swarm_Optimization]			 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Forever_Accumulating_Evolution]		 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Many_Optimizing_Liaisons]			 );
		methods_Combo_Box->addItem(spacer + SO_Methods[Layered_and_Interleaved_CoEvolution]	 );

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
	});
}

void Fitting_Settings_Editor::create_Pages()
{
	pages_Stack = new QStackedWidget;
	main_Layout->addWidget(pages_Stack);

	// GSL
	{
		GSL_Page = new QWidget(this);
			GSL_Page->setContentsMargins(-5,-5,-5,-5);

		GSL_Page_Layout = new QVBoxLayout(GSL_Page);
			GSL_Page_Layout->setAlignment(Qt::AlignTop);
		pages_Stack->addWidget(GSL_Page);
	}

	// SO
	{
		SO_Page = new QWidget(this);
			SO_Page->setContentsMargins(-5,-5,-5,-5);

		SO_Page_Layout = new QVBoxLayout(SO_Page);
			SO_Page_Layout->setAlignment(Qt::AlignTop);
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
		max_Iter_Label = new QLabel("Max number of iterations");
		max_Iter_Line_Edit = new QLineEdit(QString::number(fitting_Settings->max_Iter));
			max_Iter_Line_Edit->setValidator(new QIntValidator(0, MAX_INTEGER, this));

		x_Tolerance_Label = new QLabel("Parameters tolerance");
		x_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->x_Tolerance));
			x_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		g_Tolerance_Label = new QLabel("Gradient tolerance");
		g_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->g_Tolerance));
			g_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		f_Tolerance_Label = new QLabel("Residual tolerance");
		f_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->f_Tolerance));
			f_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		GSL_Fit_Params_Group_Box_Layout->addWidget(max_Iter_Label,		 0,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(max_Iter_Line_Edit,	 0,1,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(x_Tolerance_Label,	 1,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(x_Tolerance_Line_Edit,1,1,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(g_Tolerance_Label,	 2,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(g_Tolerance_Line_Edit,2,1,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(f_Tolerance_Label,	 3,0,1,1);
		GSL_Fit_Params_Group_Box_Layout->addWidget(f_Tolerance_Line_Edit,3,1,1,1);

		connect(max_Iter_Line_Edit,	   &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->max_Iter    = max_Iter_Line_Edit->   text().toInt();	});
		connect(x_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->x_Tolerance = x_Tolerance_Line_Edit->text().toDouble();	});
		connect(g_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->g_Tolerance = g_Tolerance_Line_Edit->text().toDouble();	});
		connect(f_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->f_Tolerance = f_Tolerance_Line_Edit->text().toDouble();	});
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
		scale_Label = new QLabel("Scaling method (default Mor"+More_Sym+")");
		scale_Combo_Box = new QComboBox;
			scale_Combo_Box->addItem(GSL_Scales[More]);
			scale_Combo_Box->addItem(GSL_Scales[Levenberg]);
			scale_Combo_Box->addItem(GSL_Scales[Marquardt]);
			scale_Combo_Box->setCurrentIndex(scale_Combo_Box->findText(fitting_Settings->current_Scale));

		solver_Label = new QLabel("Solver method (default QR decomposition)");
		solver_Combo_Box = new QComboBox;
			solver_Combo_Box->addItem(GSL_Solvers[QR_decomposition]);
			solver_Combo_Box->addItem(GSL_Solvers[Cholesky_decomposition]);
			solver_Combo_Box->addItem(GSL_Solvers[Singular_value_decomposition]);
			solver_Combo_Box->setCurrentIndex(solver_Combo_Box->findText(fitting_Settings->current_Solver));

		fdtype_Label = new QLabel("Finite difference method (default Forward)");
		fdtype_Combo_Box = new QComboBox;
			fdtype_Combo_Box->addItem(GSL_Fdtype[Forward]);
			fdtype_Combo_Box->addItem(GSL_Fdtype[Central]);
			fdtype_Combo_Box->setCurrentIndex(fdtype_Combo_Box->findText(fitting_Settings->current_Fdtype));

		factor_up_Label = new QLabel("Factor for increasing trust radius (default 3)");
		factor_up_Line_Edit = new QLineEdit(QString::number(fitting_Settings->factor_Up));
			factor_up_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		factor_down_Label = new QLabel("Factor for decreasing trust radius (default 2)");
		factor_down_Line_Edit = new QLineEdit(QString::number(fitting_Settings->factor_Down));
			factor_down_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		avmax_Label = new QLabel("Max allowed |a|/|v| (default 0.75)");
		avmax_Line_Edit = new QLineEdit(QString::number(fitting_Settings->avmax));
			avmax_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		h_df_Label = new QLabel("Step size for finite difference J (default ~1.5e-8)");
		h_df_Line_Edit = new QLineEdit(QString::number(fitting_Settings->h_df));
			h_df_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));

		h_fvv_Label = new QLabel("Step size for finite difference fvv (default 0.02)");
		h_fvv_Line_Edit = new QLineEdit(QString::number(fitting_Settings->h_fvv));
			h_fvv_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));


		GSL_Additional_Params_Group_Box_Layout->addWidget(scale_Label,		0,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(scale_Combo_Box,	0,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(solver_Label,		1,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(solver_Combo_Box,	1,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(fdtype_Label,		2,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(fdtype_Combo_Box,	2,1,1,1);

		GSL_Additional_Params_Group_Box_Layout->addWidget(factor_up_Label,		3,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(factor_up_Line_Edit,	3,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(factor_down_Label,	4,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(factor_down_Line_Edit,4,1,1,1);

		GSL_Additional_Params_Group_Box_Layout->addWidget(avmax_Label,		5,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(avmax_Line_Edit,	5,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(h_df_Label,		6,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(h_df_Line_Edit,	6,1,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(h_fvv_Label,		7,0,1,1);
		GSL_Additional_Params_Group_Box_Layout->addWidget(h_fvv_Line_Edit,	7,1,1,1);

		connect(scale_Combo_Box,  static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]{fitting_Settings->current_Scale  = scale_Combo_Box->currentText(); });
		connect(solver_Combo_Box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]{fitting_Settings->current_Solver = solver_Combo_Box->currentText();});
		connect(fdtype_Combo_Box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=]{fitting_Settings->current_Fdtype = fdtype_Combo_Box->currentText();});

		connect(factor_up_Line_Edit,   &QLineEdit::textChanged, [=]{fitting_Settings->factor_Up		= factor_up_Line_Edit->text().toDouble();	});
		connect(factor_down_Line_Edit, &QLineEdit::textChanged, [=]{fitting_Settings->factor_Down	= factor_down_Line_Edit->text().toDouble();	});
		connect(avmax_Line_Edit,	   &QLineEdit::textChanged, [=]{fitting_Settings->avmax			= avmax_Line_Edit->text().toDouble();		});
		connect(h_df_Line_Edit,		   &QLineEdit::textChanged, [=]{fitting_Settings->h_df			= h_df_Line_Edit->text().toDouble();		});
		connect(h_fvv_Line_Edit,	   &QLineEdit::textChanged, [=]{fitting_Settings->h_fvv			= h_fvv_Line_Edit->text().toDouble();		});
	}
	{
		// create spoiler
		QVBoxLayout* frame_Layout = new QVBoxLayout;
		frame_Layout->addWidget(GSL_Additional_Params_Group_Box);
		Spoiler* spoiler = new Spoiler("Additional parameters", 5, this);
			spoiler->setContentsMargins(-9,0,-9,-0);
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
		num_Runs_Label = new QLabel("Number of runs");
		num_Runs_Line_Edit = new QLineEdit(QString::number(fitting_Settings->num_Runs));
			num_Runs_Line_Edit->setValidator(new QIntValidator(0, MAX_INTEGER, this));

		max_Evaluations_Label = new QLabel("Max number of evaluations");
		max_Evaluations_Line_Edit = new QLineEdit(QString::number(fitting_Settings->max_Evaluations));
			max_Evaluations_Line_Edit->setValidator(new QIntValidator(0, MAX_INTEGER, this));
			max_Evaluations_Line_Edit->setDisabled(fitting_Settings->max_Eval_Check);


		max_Eval_Check_Box = new QCheckBox(this);
			max_Eval_Check_Box->setChecked(fitting_Settings->max_Eval_Check);
		max_Eval_Factor_Label = new QLabel("~ number of parameters");
		max_Eval_Factor_Line_Edit = new QLineEdit(QString::number(fitting_Settings->max_Eval_Factor));
			max_Eval_Factor_Line_Edit->setValidator(new QIntValidator(0, MAX_INTEGER, this));
			max_Eval_Factor_Line_Edit->setEnabled(fitting_Settings->max_Eval_Check);



		SO_Fit_Params_Group_Box_Layout->addWidget(num_Runs_Label,				0,0,1,2);
		SO_Fit_Params_Group_Box_Layout->addWidget(num_Runs_Line_Edit,			0,2,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(max_Evaluations_Label,		1,0,1,2);
		SO_Fit_Params_Group_Box_Layout->addWidget(max_Evaluations_Line_Edit,	1,2,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(max_Eval_Check_Box,			2,0,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(max_Eval_Factor_Label,		2,1,1,1);
		SO_Fit_Params_Group_Box_Layout->addWidget(max_Eval_Factor_Line_Edit,	2,2,1,1);

		connect(num_Runs_Line_Edit,		   &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->num_Runs		= num_Runs_Line_Edit->		 text().toInt();});
		connect(max_Evaluations_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->max_Evaluations = max_Evaluations_Line_Edit->text().toInt();});
		connect(max_Eval_Factor_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->max_Eval_Factor = max_Eval_Factor_Line_Edit->text().toInt();});
		connect(max_Eval_Check_Box,		   &QCheckBox::toggled,		fitting_Settings, [=]
		{
			fitting_Settings->max_Eval_Check = max_Eval_Check_Box->isChecked();
			max_Evaluations_Line_Edit->setDisabled(fitting_Settings->max_Eval_Check);
			max_Eval_Factor_Line_Edit->setEnabled(fitting_Settings->max_Eval_Check);
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

