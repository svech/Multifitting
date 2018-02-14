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
	create_Main_Params_Group_Box();
	create_AdditionalParams_Group_Box();

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
}

void Fitting_Settings_Editor::set_Window_Geometry()
{
	adjustSize();
	setGeometry(430,498,width(),height());
	setFixedSize(size());
}

void Fitting_Settings_Editor::create_Menu()
{
	Menu* menu = new Menu(window_Type_Fitting_Settings_Editor,this);
	main_Layout->setMenuBar(menu->menu_Bar);
}

void Fitting_Settings_Editor::create_Metods()
{
	methods_Combo_Box = new QComboBox;

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

	// fill GSL group
	QString spacer = "\t";
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
			if(previous>methods_Combo_Box->currentIndex() && methods_Combo_Box->currentIndex()-2 >= 0)
			{
				methods_Combo_Box->setCurrentIndex(methods_Combo_Box->currentIndex()-2);
			} else
			{
				methods_Combo_Box->setCurrentIndex(previous);
			}
		}
		methods_Combo_Box->setProperty("previous", methods_Combo_Box->currentIndex());
		fitting_Settings->current_Method = methods_Combo_Box->currentText().remove(QRegExp(spacer));
	});

	methods_Combo_Box->setMaxVisibleItems(methods_Combo_Box->count());
	methods_Combo_Box->setCurrentIndex(methods_Combo_Box->findText(spacer + fitting_Settings->current_Method));
	methods_Combo_Box->adjustSize();
	methods_Combo_Box->setFixedWidth(methods_Combo_Box->width()+90);

	methods_Label = new QLabel("Fitting Algorithm");

	// add objects to window
	methods_Layout = new QHBoxLayout;
		methods_Layout->setAlignment(Qt::AlignLeft);
		methods_Layout->setSpacing(5);

		methods_Layout->addWidget(methods_Label);
		methods_Layout->addWidget(methods_Combo_Box);
		main_Layout->addLayout(methods_Layout);
}

void Fitting_Settings_Editor::create_Pages()
{
	pages_Stack = new QStackedWidget;
	main_Layout->addWidget(pages_Stack);

	page = new QWidget;
		page->setContentsMargins(-5,-5,-5,-5);

	page_Layout = new QVBoxLayout(page);
	pages_Stack->addWidget(page);
}

void Fitting_Settings_Editor::create_Main_Params_Group_Box()
{
	{
		fit_Params_Group_Box = new QGroupBox(this);
			fit_Params_Group_Box->setObjectName("fit_Params_Group_Box");
			fit_Params_Group_Box->setStyleSheet("QGroupBox#fit_Params_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
															"QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		page_Layout->addWidget(fit_Params_Group_Box);
		fit_Params_Group_Box_Layout = new QGridLayout(fit_Params_Group_Box);
	}

	// for GSL TRS
	{
		max_Iter_Label = new QLabel("Max number of iterations");
		max_Iter_Line_Edit = new QLineEdit(QString::number(fitting_Settings->max_Iter));
			max_Iter_Line_Edit->setValidator(new QIntValidator(0, MAX_INTEGER));

		x_Tolerance_Label = new QLabel("Parameters tolerance");
		x_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->x_Tolerance));
			x_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		g_Tolerance_Label = new QLabel("Gradient tolerance");
		g_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->g_Tolerance));
			g_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		f_Tolerance_Label = new QLabel("Residual tolerance");
		f_Tolerance_Line_Edit = new QLineEdit(QString::number(fitting_Settings->f_Tolerance));
			f_Tolerance_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		fit_Params_Group_Box_Layout->addWidget(max_Iter_Label,		 0,0,1,1);
		fit_Params_Group_Box_Layout->addWidget(max_Iter_Line_Edit,	 0,1,1,1);
		fit_Params_Group_Box_Layout->addWidget(x_Tolerance_Label,	 1,0,1,1);
		fit_Params_Group_Box_Layout->addWidget(x_Tolerance_Line_Edit,1,1,1,1);
		fit_Params_Group_Box_Layout->addWidget(g_Tolerance_Label,	 2,0,1,1);
		fit_Params_Group_Box_Layout->addWidget(g_Tolerance_Line_Edit,2,1,1,1);
		fit_Params_Group_Box_Layout->addWidget(f_Tolerance_Label,	 3,0,1,1);
		fit_Params_Group_Box_Layout->addWidget(f_Tolerance_Line_Edit,3,1,1,1);

		connect(max_Iter_Line_Edit,	   &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->max_Iter    = max_Iter_Line_Edit->   text().toInt();	});
		connect(x_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->x_Tolerance = x_Tolerance_Line_Edit->text().toDouble();	});
		connect(g_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->g_Tolerance = g_Tolerance_Line_Edit->text().toDouble();	});
		connect(f_Tolerance_Line_Edit, &QLineEdit::textChanged, fitting_Settings, [=]{fitting_Settings->f_Tolerance = f_Tolerance_Line_Edit->text().toDouble();	});
	}
}

void Fitting_Settings_Editor::create_AdditionalParams_Group_Box()
{
	{
		additional_Params_Group_Box = new QGroupBox(this);
			additional_Params_Group_Box->setObjectName("additional_Params_Group_Box");
			additional_Params_Group_Box->setStyleSheet("QGroupBox#additional_Params_Group_Box { border-radius: 2px;  border: 1px solid gray; margin-top: 2ex;}"
																		  "QGroupBox::title   { subcontrol-origin: margin;   left: 9px; padding: 0 0px 0 1px;}");
		additional_Params_Group_Box_Layout = new QGridLayout(additional_Params_Group_Box);
	}
	// for GSL TRS
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
			factor_up_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		factor_down_Label = new QLabel("Factor for decreasing trust radius (default 2)");
		factor_down_Line_Edit = new QLineEdit(QString::number(fitting_Settings->factor_Down));
			factor_down_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		avmax_Label = new QLabel("Max allowed |a|/|v| (default 0.75)");
		avmax_Line_Edit = new QLineEdit(QString::number(fitting_Settings->avmax));
			avmax_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		h_df_Label = new QLabel("Step size for finite difference J (default ~1.5e-8)");
		h_df_Line_Edit = new QLineEdit(QString::number(fitting_Settings->h_df));
			h_df_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));

		h_fvv_Label = new QLabel("Step size for finite difference fvv (default 0.02)");
		h_fvv_Line_Edit = new QLineEdit(QString::number(fitting_Settings->h_fvv));
			h_fvv_Line_Edit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION));


		additional_Params_Group_Box_Layout->addWidget(scale_Label,		0,0,1,1);
		additional_Params_Group_Box_Layout->addWidget(scale_Combo_Box,	0,1,1,1);
		additional_Params_Group_Box_Layout->addWidget(solver_Label,		1,0,1,1);
		additional_Params_Group_Box_Layout->addWidget(solver_Combo_Box,	1,1,1,1);
		additional_Params_Group_Box_Layout->addWidget(fdtype_Label,		2,0,1,1);
		additional_Params_Group_Box_Layout->addWidget(fdtype_Combo_Box,	2,1,1,1);

		additional_Params_Group_Box_Layout->addWidget(factor_up_Label,		3,0,1,1);
		additional_Params_Group_Box_Layout->addWidget(factor_up_Line_Edit,	3,1,1,1);
		additional_Params_Group_Box_Layout->addWidget(factor_down_Label,	4,0,1,1);
		additional_Params_Group_Box_Layout->addWidget(factor_down_Line_Edit,4,1,1,1);

		additional_Params_Group_Box_Layout->addWidget(avmax_Label,		5,0,1,1);
		additional_Params_Group_Box_Layout->addWidget(avmax_Line_Edit,	5,1,1,1);
		additional_Params_Group_Box_Layout->addWidget(h_df_Label,		6,0,1,1);
		additional_Params_Group_Box_Layout->addWidget(h_df_Line_Edit,	6,1,1,1);
		additional_Params_Group_Box_Layout->addWidget(h_fvv_Label,		7,0,1,1);
		additional_Params_Group_Box_Layout->addWidget(h_fvv_Line_Edit,	7,1,1,1);

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
		frame_Layout->addWidget(additional_Params_Group_Box);
		Spoiler* spoiler = new Spoiler("Additional parameters", 5, this);
			spoiler->setContentsMargins(-9,0,-9,-0);
			spoiler->setContentLayout(*frame_Layout);
		page_Layout->addWidget(spoiler);
	}
}

