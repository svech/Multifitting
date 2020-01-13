#include "profile_plot.h"

Profile_Plot::Profile_Plot(Multilayer* multilayer, Profile_Plots_Window* profile_Plots_Window, QWidget *parent) :
	multilayer(multilayer),
	profile_Plots_Window(profile_Plots_Window),
	QWidget(parent)
{
	create_Main_Layout();
}

void Profile_Plot::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	custom_Plot = new QCustomPlot(this);
	horizontall_Scrollbar = new QScrollBar(Qt::Horizontal);

	if(multilayer->profile_Plot_Options.show_Cursor_Position)
	{
		QCPItemText* text_Item = new QCPItemText(custom_Plot);
		connect(custom_Plot, &QCustomPlot::mouseMove, this, [=](QMouseEvent *event)
		{
			double x       = custom_Plot->xAxis->pixelToCoord(event->pos().x());
			double y       = custom_Plot->yAxis->pixelToCoord(event->pos().y());
			double y_Text  = custom_Plot->yAxis->pixelToCoord(event->pos().y()-17);
			text_Item->setText(QString("(%1, %2)").arg(x).arg(y));
			text_Item->position->setCoords(QPointF(x, y_Text));
			text_Item->setFont(QFont(font().family(), 10));
			custom_Plot->replot();
		});
	}
	create_Left_Side();

	QVBoxLayout* plot_Bar_Layout = new QVBoxLayout;
		plot_Bar_Layout->setContentsMargins(4,4,4,0);
	main_Layout->addLayout(plot_Bar_Layout);
	plot_Bar_Layout->addWidget(custom_Plot);
	plot_Bar_Layout->addWidget(horizontall_Scrollbar);

	custom_Plot->xAxis->setLabel("Depth, "+multilayer->profile_Plot_Options.local_length_units);
}

void Profile_Plot::create_Left_Side()
{
	// left side
	///==============================================================================================
		QVBoxLayout* left_Layout = new QVBoxLayout;
			left_Layout->setSpacing(2);
			main_Layout->addLayout(left_Layout);

		///------------------------------------------------------------------------------------------
		// top part
		///------------------------------------------------------------------------------------------
		value_Type_GroupBox = new QGroupBox;
			left_Layout->addWidget(value_Type_GroupBox);
			left_Layout->setAlignment(Qt::AlignTop);
		QVBoxLayout* value_Type_Layout = new QVBoxLayout(value_Type_GroupBox);
			value_Type_Layout->setSpacing(2);
			value_Type_Layout->setContentsMargins(8,5,8,5);

		// ----------------------------------------------------
		// permittivity
		// ----------------------------------------------------
		QGridLayout* permittivity_Layout = new QGridLayout;
			value_Type_Layout->addLayout(permittivity_Layout);

		permittivity_RadioButton = new QRadioButton("Permittivity");
			permittivity_Layout->addWidget(permittivity_RadioButton,0,0,1,6);

		at_Wavelength_Label = new QLabel("At fixed " + Global_Variables::wavelength_Energy_Name(multilayer->profile_Plot_Options.local_wavelength_units));
			permittivity_Layout->addWidget(at_Wavelength_Label,1,2,1,4);
		at_Wavelength_LineEdit = new QLineEdit(Locale.toString(Global_Variables::wavelength_Energy(multilayer->profile_Plot_Options.local_wavelength_units, multilayer->profile_Plot_Data.local_Wavelength)/wavelength_Coefficients_Map.value(multilayer->profile_Plot_Options.local_wavelength_units),line_edit_double_format,line_edit_wavelength_precision));
			at_Wavelength_LineEdit->setFixedWidth(80);
			at_Wavelength_LineEdit->setProperty(min_Size_Property, 80);
			at_Wavelength_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(at_Wavelength_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(at_Wavelength_LineEdit, false);});
			connect(at_Wavelength_LineEdit, &QLineEdit::editingFinished, this, [=]
			{
				multilayer->profile_Plot_Data.local_Wavelength = Global_Variables::wavelength_Energy(multilayer->profile_Plot_Options.local_wavelength_units, Locale.toDouble(at_Wavelength_LineEdit->text()))*wavelength_Coefficients_Map.value(multilayer->profile_Plot_Options.local_wavelength_units);
				plot_Data(true);
			});
		at_Wavelength_Unints_Label = new QLabel(" " + multilayer->profile_Plot_Options.local_wavelength_units);

		QHBoxLayout* wavelength_Layout = new QHBoxLayout;
			wavelength_Layout->setAlignment(Qt::AlignLeft);
			wavelength_Layout->addWidget(at_Wavelength_LineEdit,0,Qt::AlignLeft);
			wavelength_Layout->addWidget(at_Wavelength_Unints_Label,0,Qt::AlignLeft);
			permittivity_Layout->addLayout(wavelength_Layout,2,5);

		delta_RadioButton = new QRadioButton("Re(1-"+Epsilon_Sym+")");
			permittivity_Layout->addWidget(delta_RadioButton,3,2,1,4);
			connect(delta_RadioButton, &QRadioButton::clicked, this, [=]
			{
				bool checked = delta_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.permittivity_Type = DELTA_EPS;
					custom_Plot->yAxis->setLabel("Re(1-"+Epsilon_Sym+")");

					// disable log scale
					lin_Y_RadioButton->setDisabled(true);
					log_Y_RadioButton->setDisabled(true);

//					qInfo() << "delta_RadioButton" << endl;
					plot_Data(true);
				}
			});

		beta_RadioButton = new QRadioButton("Im("+Epsilon_Sym+")");
			permittivity_Layout->addWidget(beta_RadioButton,4,2,1,4);
			connect(beta_RadioButton, &QRadioButton::clicked, this, [=]
			{
				bool checked = beta_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.permittivity_Type = BETA_EPS;
					custom_Plot->yAxis->setLabel("Im("+Epsilon_Sym+")");

					// enable log scale
					lin_Y_RadioButton->setDisabled(false);
					log_Y_RadioButton->setDisabled(false);

//					qInfo() << "beta_RadioButton" << endl;
					plot_Data(true);
				}
			});

		QButtonGroup* delta_Beta_ButtonGroup = new QButtonGroup;
			delta_Beta_ButtonGroup->addButton(delta_RadioButton);
			delta_Beta_ButtonGroup->addButton(beta_RadioButton);

		connect(permittivity_RadioButton, &QRadioButton::clicked, this, [=]
		{
			bool checked = permittivity_RadioButton->isChecked();
			at_Wavelength_Label->setEnabled(checked);
			at_Wavelength_LineEdit->setEnabled(checked);
			at_Wavelength_Unints_Label->setEnabled(checked);

			if(checked)
			{
				multilayer->profile_Plot_Options.type = PERMITTIVITY;
				delta_RadioButton->clicked(delta_RadioButton->isChecked());
				beta_RadioButton ->clicked(beta_RadioButton ->isChecked());
			} else
			{
				// enable log scale
				lin_Y_RadioButton->setDisabled(false);
				log_Y_RadioButton->setDisabled(false);
			}
			delta_RadioButton->setEnabled(checked);
			beta_RadioButton->setEnabled(checked);

			plot_Data();
		});

		// ----------------------------------------------------
		// materials
		// ----------------------------------------------------
		materials_RadioButton = new QRadioButton("Materials");
			value_Type_Layout->addWidget(materials_RadioButton);
			connect(materials_RadioButton, &QRadioButton::clicked, this, [=]
			{
				bool checked = materials_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.type = MATERIAL;
					custom_Plot->yAxis->setLabel("Relative density");

					permittivity_RadioButton->clicked(permittivity_RadioButton->isChecked());

//					qInfo() << "materials_RadioButton" << endl;
					plot_Data(true);
				}
			});

		// ----------------------------------------------------
		// elements
		// ----------------------------------------------------
		elements_RadioButton = new QRadioButton("Elements");
			// TODO
			elements_RadioButton->setDisabled(true);
			value_Type_Layout->addWidget(elements_RadioButton);
			connect(elements_RadioButton, &QRadioButton::clicked, this, [=]
			{
				bool checked = elements_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.type = ELEMENTS;
					custom_Plot->yAxis->setLabel("Absolute concentration, "+Multiply_Sym+"10"+Power_23_Sym+" cm"+Minus_Three_Sym);

					permittivity_RadioButton->clicked(permittivity_RadioButton->isChecked());

//					qInfo() << "elements_RadioButton" << endl;
					plot_Data(true);
				}
			});

		// ----------------------------------------------------
		QButtonGroup* radio_ButtonGroup = new QButtonGroup;
			radio_ButtonGroup->addButton(permittivity_RadioButton);
			radio_ButtonGroup->addButton(materials_RadioButton);
			radio_ButtonGroup->addButton(elements_RadioButton);
		// ----------------------------------------------------
		value_Type_GroupBox->adjustSize();
		value_Type_GroupBox->setFixedSize(value_Type_GroupBox->size());

		///------------------------------------------------------------------------------------------
		// middle part
		///------------------------------------------------------------------------------------------
		line_Type_GroupBox = new QGroupBox;
			left_Layout->addWidget(line_Type_GroupBox);
		QVBoxLayout* line_Type_Layout = new QVBoxLayout(line_Type_GroupBox);
			line_Type_Layout->setSpacing(2);
			line_Type_Layout->setContentsMargins(8,5,6,5);

		// ----------------------------------------------------
		use_Roughness_CheckBox = new QCheckBox("Apply roughness");
			line_Type_Layout->addWidget(use_Roughness_CheckBox);
			connect(use_Roughness_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.apply_Roughness = use_Roughness_CheckBox->isChecked();
			});

		use_Diffusiness_CheckBox = new QCheckBox("Apply diffusiness");
			line_Type_Layout->addWidget(use_Diffusiness_CheckBox);
			connect(use_Diffusiness_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.apply_Diffusiness = use_Diffusiness_CheckBox->isChecked();
			});

		show_Sharp_CheckBox = new QCheckBox("Show sharp profile");
			line_Type_Layout->addWidget(show_Sharp_CheckBox);
			connect(show_Sharp_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.show_Sharp_Profile = show_Sharp_CheckBox->isChecked();
			});

		discretization_CheckBox = new QCheckBox("Show discretization");
			line_Type_Layout->addWidget(discretization_CheckBox);
			connect(discretization_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.show_Discretization = discretization_CheckBox->isChecked();
			});

		cursor_Cordinate_CheckBox = new QCheckBox("Show cursor position");
			line_Type_Layout->addWidget(cursor_Cordinate_CheckBox);
			connect(cursor_Cordinate_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.show_Cursor_Position = cursor_Cordinate_CheckBox->isChecked();

				// other profiles obtain the same option : synchronization
				for(Profile_Plot* profile_Plot : profile_Plots_Window->profile_Plot_Vector)
				{
					profile_Plot->multilayer->profile_Plot_Options.show_Cursor_Position = multilayer->profile_Plot_Options.show_Cursor_Position;
					profile_Plot->cursor_Cordinate_CheckBox->blockSignals(true);
					profile_Plot->cursor_Cordinate_CheckBox->setChecked(profile_Plot->multilayer->profile_Plot_Options.show_Cursor_Position);
					profile_Plot->cursor_Cordinate_CheckBox->blockSignals(false);
				}

				int active_Tab = profile_Plots_Window->main_Tabs->currentIndex();
				profile_Plots_Window->close();
				global_Multilayer_Approach->open_Profile_Plots();
				global_Multilayer_Approach->profile_Plots_Window->main_Tabs->setCurrentIndex(active_Tab);
			});

		// ----------------------------------------------------
		line_Type_GroupBox->adjustSize();
		line_Type_GroupBox->setFixedSize(line_Type_GroupBox->size());

		///------------------------------------------------------------------------------------------
		// bottom part
		///------------------------------------------------------------------------------------------
		scale_GroupBox = new QGroupBox;
			left_Layout->addWidget(scale_GroupBox);
		QVBoxLayout* scale_Layout = new QVBoxLayout(scale_GroupBox);
			scale_Layout->setSpacing(2);
			scale_Layout->setContentsMargins(8,5,2,5);

		// ----------------------------------------------------
		rescale_X_CheckBox = new QCheckBox("Rescale X");
			scale_Layout->addWidget(rescale_X_CheckBox);
			connect(rescale_X_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.rescale_X = rescale_X_CheckBox->isChecked();
			});

		rescale_Y_CheckBox = new QCheckBox("Rescale Y");
			scale_Layout->addWidget(rescale_Y_CheckBox);
			connect(rescale_Y_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.rescale_Y = rescale_Y_CheckBox->isChecked();
			});

		// scaling
		QGridLayout* scale_Y_Layout = new QGridLayout;
			scale_Layout->addLayout(scale_Y_Layout);

		scale_Y_Label = new QLabel("Scale Y: ");
			scale_Y_Layout->addWidget(scale_Y_Label,0,0);

		// lin
		lin_Y_RadioButton = new QRadioButton("Lin");
			scale_Y_Layout->addWidget(lin_Y_RadioButton,0,1);
			connect(lin_Y_RadioButton, &QRadioButton::clicked, this, [&]
			{
				if(lin_Y_RadioButton->isChecked())
				{
					multilayer->profile_Plot_Options.y_Scale  = lin_Scale;
				}
				plot_Data();
			});

		// log
		log_Y_RadioButton = new QRadioButton("Log");
			scale_Y_Layout->addWidget(log_Y_RadioButton,0,2);
			connect(log_Y_RadioButton, &QRadioButton::clicked, this, [&]
			{
				if(log_Y_RadioButton->isChecked())
				{
					multilayer->profile_Plot_Options.y_Scale = log_Scale;
				}
				plot_Data();
			});

		QButtonGroup* Y_ButtonGroup = new QButtonGroup;
			Y_ButtonGroup->addButton(lin_Y_RadioButton);
			Y_ButtonGroup->addButton(log_Y_RadioButton);

		// ----------------------------------------------------
		scale_GroupBox->adjustSize();
		scale_GroupBox->setFixedSize(scale_GroupBox->size());

		value_Type_GroupBox->setFixedWidth(scale_GroupBox->width());
		line_Type_GroupBox->setFixedWidth(scale_GroupBox->width());

	///==============================================================================================
	// ----------------------------------------------------
	// initialization
	// ----------------------------------------------------
	if(multilayer->profile_Plot_Options.type == PERMITTIVITY) {permittivity_RadioButton->setChecked(true);}
	if(multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS) {delta_RadioButton->setChecked(true);}
	if(multilayer->profile_Plot_Options.permittivity_Type == BETA_EPS)  {beta_RadioButton->setChecked(true);}
	if(multilayer->profile_Plot_Options.type == MATERIAL)	  {materials_RadioButton->setChecked(true);}
	if(multilayer->profile_Plot_Options.type == ELEMENTS)	  {elements_RadioButton->setChecked(true);}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - -

	use_Roughness_CheckBox->setChecked(multilayer->profile_Plot_Options.apply_Roughness);
	use_Diffusiness_CheckBox->setChecked(multilayer->profile_Plot_Options.apply_Diffusiness);
	show_Sharp_CheckBox->setChecked(multilayer->profile_Plot_Options.show_Sharp_Profile);
	discretization_CheckBox->setChecked(multilayer->profile_Plot_Options.show_Discretization);
	cursor_Cordinate_CheckBox->blockSignals(true);
	cursor_Cordinate_CheckBox->setChecked(multilayer->profile_Plot_Options.show_Cursor_Position);
	cursor_Cordinate_CheckBox->blockSignals(false);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - -

	rescale_X_CheckBox->setChecked(multilayer->profile_Plot_Options.rescale_X);
	rescale_Y_CheckBox->setChecked(multilayer->profile_Plot_Options.rescale_Y);
	if(multilayer->profile_Plot_Options.y_Scale == lin_Scale) {lin_Y_RadioButton->setChecked(true);}
	if(multilayer->profile_Plot_Options.y_Scale == log_Scale) {log_Y_RadioButton->setChecked(true);}

//	use_Roughness_CheckBox->toggled(multilayer->profile_Plot_Options.apply_Roughness);		// already toggled when ->setChecked(true)
//	use_Diffusiness_CheckBox->toggled(multilayer->profile_Plot_Options.apply_Diffusiness);	// already toggled when ->setChecked(true)
//	show_Sharp_CheckBox->toggled(multilayer->profile_Plot_Options.show_Sharp_Profile);		// already toggled when ->setChecked(true)
//	discretization_CheckBox->toggled(multilayer->profile_Plot_Options.show_Discretization);	// already toggled when ->setChecked(true)

	delta_RadioButton->clicked(multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS);
	beta_RadioButton->clicked(multilayer->profile_Plot_Options.permittivity_Type == BETA_EPS);
	materials_RadioButton->clicked(multilayer->profile_Plot_Options.type == MATERIAL);
	elements_RadioButton->clicked(multilayer->profile_Plot_Options.type == ELEMENTS);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - -

	custom_Plot->replot();
}

void Profile_Plot::create_Plot_Frame_And_Scale()
{
	// runned many times!
	custom_Plot->setNoAntialiasingOnDrag(false); // more performance/responsiveness during dragging

	// frame, axes
	{
		QPen pen = custom_Plot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
		custom_Plot->yAxis->grid()->setSubGridVisible(true);
		custom_Plot->xAxis->grid()->setSubGridVisible(true);
		custom_Plot->yAxis->grid()->setPen(pen);
		custom_Plot->xAxis->grid()->setPen(pen);

		/// X axis
		{
			QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
			custom_Plot->xAxis->setScaleType(QCPAxis::stLinear);
			custom_Plot->xAxis->setTicker(linTicker);
			custom_Plot->xAxis->setNumberFormat("g");
			custom_Plot->xAxis->setNumberPrecision(4);

			custom_Plot->xAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->xAxis2->setTicker(linTicker);
			custom_Plot->xAxis2->setNumberFormat("g");
			custom_Plot->xAxis2->setNumberPrecision(4);
		}
		/// Y axis
		if(multilayer->profile_Plot_Options.y_Scale == lin_Scale ||
		  (multilayer->profile_Plot_Options.type == PERMITTIVITY &&
		   multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS))
		{
			QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);

			custom_Plot->yAxis->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis->setTicker(linTicker);
			custom_Plot->yAxis->setNumberFormat("g");
			custom_Plot->yAxis->setNumberPrecision(4);

			custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis2->setTicker(linTicker);
			custom_Plot->yAxis2->setNumberFormat("g");
			custom_Plot->yAxis2->setNumberPrecision(4);
		}
		if(multilayer->profile_Plot_Options.y_Scale == log_Scale &&
		   (multilayer->profile_Plot_Options.type != PERMITTIVITY ||
			multilayer->profile_Plot_Options.permittivity_Type != DELTA_EPS))
		{
			QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);

			custom_Plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
			custom_Plot->yAxis->setTicker(logTicker);
			custom_Plot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
			custom_Plot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

			custom_Plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
			custom_Plot->yAxis2->setTicker(logTicker);
			custom_Plot->yAxis2->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
			custom_Plot->yAxis2->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
		}

		// make range draggable and zoomable:
		custom_Plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom | /*QCP::iSelectAxes |*/ QCP::iSelectLegend);

		// make top right axes clones of bottom left axes:
		custom_Plot->xAxis2->setVisible(true);
		custom_Plot->yAxis2->setVisible(true);
		custom_Plot->xAxis2->setTickLabels(false);

		connect(custom_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->xAxis2, SLOT(setRange(QCPRange)));
		connect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));


		connect(custom_Plot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), this, [=](const QCPRange&)
		{
			multilayer->profile_Plot_Options.old_X_Begin = custom_Plot->xAxis->range().lower;
			multilayer->profile_Plot_Options.old_X_End   = custom_Plot->xAxis->range().upper;
		});
		connect(custom_Plot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), this, [=](const QCPRange&)
		{
			multilayer->profile_Plot_Options.old_Y_Begin = custom_Plot->yAxis->range().lower;
			multilayer->profile_Plot_Options.old_Y_End   = custom_Plot->yAxis->range().upper;
		});
	}
//	set_Title_Text();
}

void Profile_Plot::plot_Data(bool recalculate_Profile)
{
	create_Plot_Frame_And_Scale();

	if(custom_Plot->graphCount()!=1)
	{
		custom_Plot->clearGraphs();
		custom_Plot->addGraph();
	}
	custom_Plot->graph(0)->setPen(QPen(custom_Plot->graph(0)->pen().color(), 1.3));

	// data
	if(recalculate_Profile)
	{
		auto start = std::chrono::system_clock::now();

		calculate_Profile();

		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		qInfo() << "	profile plot:    "<< elapsed.count()/1000000. << " seconds" << endl << endl << endl;
	}

	// rescaling
	double minimum = -2020, maximum = 2020;
	if(multilayer->profile_Plot_Options.y_Scale == lin_Scale ||
	  (multilayer->profile_Plot_Options.type == PERMITTIVITY &&
	   multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS))
	{
		double minimum_Raw = *std::min_element(val.constBegin(), val.constEnd());
		double maximum_Raw = *std::max_element(val.constBegin(), val.constEnd());

		maximum = maximum_Raw + (maximum_Raw-minimum_Raw)*0.08;
		minimum = minimum_Raw - (maximum_Raw-minimum_Raw)*0.08;
	}
	if(multilayer->profile_Plot_Options.y_Scale == log_Scale &&
	   (multilayer->profile_Plot_Options.type != PERMITTIVITY ||
		multilayer->profile_Plot_Options.permittivity_Type != DELTA_EPS))
	{
		// val is changing here (removing zeros)
		val.removeAll(0);
		double maximum_Raw_Non_Zero = *std::max_element(val.constBegin(), val.constEnd()); val.removeAll(0);
		double minimum_Raw_Non_Zero = *std::min_element(val.constBegin(), val.constEnd()); val.removeAll(0);

		maximum = maximum_Raw_Non_Zero*1.5;
		minimum = minimum_Raw_Non_Zero/1.5;
	}

	if(multilayer->profile_Plot_Options.rescale_X)	{ custom_Plot->xAxis->setRange(arg.first(), arg.last());	}
	else											{ custom_Plot->xAxis->setRange(multilayer->profile_Plot_Options.old_X_Begin, multilayer->profile_Plot_Options.old_X_End);}
	if(multilayer->profile_Plot_Options.rescale_Y)	{ custom_Plot->yAxis->setRange(minimum,maximum);			}
	else											{ custom_Plot->yAxis->setRange(multilayer->profile_Plot_Options.old_Y_Begin, multilayer->profile_Plot_Options.old_Y_End);}

	custom_Plot->replot();
}

struct Different_Norm_Layer
{
	Different_Norm_Layer()
	{
//		interlayer_Composition_Weight_Left.resize(transition_Layer_Functions_Size);
//		interlayer_My_Sigma_Left.resize(transition_Layer_Functions_Size);
//		interlayer_Composition_Weight_Right.resize(transition_Layer_Functions_Size);
//		interlayer_My_Sigma_Right.resize(transition_Layer_Functions_Size);
	}

//	void set_Interlayer(QVector<Interlayer>& interlayer_Composition, QString side)
//	{
//		for(int i=0; i<transition_Layer_Functions_Size; i++)
//		{
//			if(interlayer_Composition[i].enabled)
//			{
//				if(side == "left")
//				{
//					interlayer_Composition_Weight_Left[i] = interlayer_Composition[i].interlayer.value;
//					interlayer_My_Sigma_Left[i] = interlayer_Composition[i].my_Sigma.value;
//				}
//				if(side == "right")
//				{
//					interlayer_Composition_Weight_Right[i] = interlayer_Composition[i].interlayer.value;
//					interlayer_My_Sigma_Right[i] = interlayer_Composition[i].my_Sigma.value;
//				}
//			}
//		}
//	}

//	QVector<double> interlayer_Composition_Weight_Left;
//	QVector<double> interlayer_My_Sigma_Left;
//	QVector<double> interlayer_Composition_Weight_Right;
//	QVector<double> interlayer_My_Sigma_Right;

	// only checking
	double thickness = -2019;
	double sigma_Left = -2019;
	double sigma_Right = -2019;

	double norm = -2019;
};

bool operator ==( const Different_Norm_Layer& different_Norm_Layer_Left, const Different_Norm_Layer& different_Norm_Layer_Right )
{
	return	/*(different_Norm_Layer_Left.interlayer_Composition_Weight_Left		 == different_Norm_Layer_Right.interlayer_Composition_Weight_Left		) &&
			(different_Norm_Layer_Left.interlayer_My_Sigma_Left	 == different_Norm_Layer_Right.interlayer_My_Sigma_Left	) &&
			(different_Norm_Layer_Left.interlayer_Composition_Weight_Right == different_Norm_Layer_Right.interlayer_Composition_Weight_Right) &&
			(different_Norm_Layer_Left.interlayer_My_Sigma_Right == different_Norm_Layer_Right.interlayer_My_Sigma_Right) &&*/
			(different_Norm_Layer_Left.sigma_Left == different_Norm_Layer_Right.sigma_Left) &&
			(different_Norm_Layer_Left.sigma_Right == different_Norm_Layer_Right.sigma_Right) &&
			(different_Norm_Layer_Left.thickness == different_Norm_Layer_Right.thickness);
}

void Profile_Plot::calculate_Profile()
{
	get_Max_My_Sigma(multilayer->structure_Tree->tree->invisibleRootItem());

	struct_Data_Vector.clear();
	unwrap_Subtree(struct_Data_Vector, multilayer->structure_Tree->tree->invisibleRootItem(), 0, 0);

	// if has no substrate, add it
	if(struct_Data_Vector.last().item_Type != item_Type_Substrate)
	{
		Data substrate = struct_Data_Vector.first();

		// change id
		substrate.reset_All_IDs();
		substrate.item_Type = item_Type_Substrate;

		// add to the end
		struct_Data_Vector.append(substrate);
	}

	// thicknesses and boundaries position
	boundary_Vector.resize(struct_Data_Vector.size()-1);	boundary_Vector.first() = 0;
	thickness_Vector.resize(struct_Data_Vector.size()-2);
	for(int i=0; i<thickness_Vector.size(); i++)
	{
		thickness_Vector[i] = struct_Data_Vector[i+1].thickness.value;
		boundary_Vector[i+1] = boundary_Vector[i]+thickness_Vector[i];
	}

	// norm
	// TODO optimize (before unwrapping, check similar layers ....). Now more or less OK
	layer_Norm_Vector.resize(thickness_Vector.size());
	QList<Different_Norm_Layer> different_Norm_Layer;
	Different_Norm_Layer temp_Dif_Norm;
	gsl_integration_workspace* w = gsl_integration_workspace_alloc(1000);
	for(int layer_Index=0; layer_Index<thickness_Vector.size(); layer_Index++)
	{
		// thickness
		if(thickness_Vector[layer_Index]>0)
		{
			temp_Dif_Norm.thickness = thickness_Vector[layer_Index];
			temp_Dif_Norm.sigma_Left = struct_Data_Vector[layer_Index].sigma.value;
			temp_Dif_Norm.sigma_Right = struct_Data_Vector[layer_Index+1].sigma.value;

			if(!different_Norm_Layer.contains(temp_Dif_Norm))
			{
				layer_Norm_Vector[layer_Index] = thickness_Vector[layer_Index] /
						Global_Variables::layer_Normalization(	thickness_Vector[layer_Index],
																struct_Data_Vector[layer_Index+1].interlayer_Composition,
																struct_Data_Vector[layer_Index+2].interlayer_Composition, w);
				temp_Dif_Norm.norm = layer_Norm_Vector[layer_Index];
				different_Norm_Layer.append(temp_Dif_Norm);
			} else
			{
				layer_Norm_Vector[layer_Index] = temp_Dif_Norm.norm;
			}
		} else
		{
			layer_Norm_Vector[layer_Index] = 1;
		}
	}
	gsl_integration_workspace_free(w);

	// materials and relative density
	material_Vector.resize(struct_Data_Vector.size());
	density_Vector.resize(struct_Data_Vector.size());
	delta_Epsilon_Vector.resize(struct_Data_Vector.size());
	beta_Epsilon_Vector.resize(struct_Data_Vector.size());

	complex<double> delta_Epsilon_Ang;
	QVector<double> spectral_Points (1, multilayer->profile_Plot_Data.local_Wavelength);
	for(int i=0; i<struct_Data_Vector.size(); i++)
	{
		Data& struct_Data = struct_Data_Vector[i];
		if(struct_Data.composed_Material)
		{
			material_Vector[i] = struct_Data.get_Composed_Material();

			double stoich_Weight = DBL_MIN;
			double auto_Density = 0;
			for(int comp_Index=0; comp_Index<struct_Data.composition.size(); ++comp_Index)
			{
				auto_Density += sorted_Density.value(struct_Data.composition[comp_Index].type)*struct_Data.composition[comp_Index].composition.value;
				stoich_Weight += struct_Data.composition[comp_Index].composition.value;
			}
			density_Vector[i] = struct_Data.absolute_Density.value/(auto_Density/stoich_Weight);

			// get epsilon
			QVector<complex<double>> temp_Epsilon;
			optical_Constants->make_Epsilon_From_Factors(struct_Data.composition, struct_Data.absolute_Density.value, spectral_Points, temp_Epsilon);
			delta_Epsilon_Ang = 1. - temp_Epsilon.first();

			delta_Epsilon_Vector[i] = real(delta_Epsilon_Ang);
			beta_Epsilon_Vector [i] = imag(delta_Epsilon_Ang);
		} else
		{
			material_Vector[i] = struct_Data.approved_Material;
			density_Vector[i] = struct_Data.relative_Density.value;

			// get epsilon
			Material_Data temp_Material_Data = optical_Constants->material_Map.value(struct_Data.approved_Material + nk_Ext);
			QVector<complex<double>> n;
			optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, struct_Data.approved_Material);
			delta_Epsilon_Ang = 1. - n.first()*n.first();

			delta_Epsilon_Vector[i] = real(delta_Epsilon_Ang);
			beta_Epsilon_Vector [i] = -imag(delta_Epsilon_Ang); // make it positive
		}
	}
	// find different materials
	different_Materials.clear();
	for(int i=0; i<material_Vector.size(); i++)
	{
		if(!different_Materials.contains(material_Vector[i])) different_Materials.append(material_Vector[i]);
	}	

	// profiling
	double step = 0.1;	// in angstroms
	double prefix = 15;	// in angstroms
	double suffix = 15;	// in angstroms
	double length = prefix+boundary_Vector.last()+suffix;
	int data_Count = ceil(length/step)+1;
	data_Count = min(data_Count, 10000); // restriction
	step = length/(data_Count-1);

	arg.resize(data_Count);
	val.resize(data_Count);

	// delta
	if(multilayer->profile_Plot_Options.type == PERMITTIVITY && multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS	)
	{
		delta_To_Plot_Vector.resize(data_Count);
		for(int i=0; i<data_Count; i++)
		{
			double z = -prefix + i*step;
			delta_To_Plot_Vector[i].key = z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
			delta_To_Plot_Vector[i].value = real(delta_Beta_Epsilon_Func(z));

			arg[i] = delta_To_Plot_Vector[i].key;
			val[i] = delta_To_Plot_Vector[i].value;
		}
		custom_Plot->graph(0)->data()->set(delta_To_Plot_Vector);
	}
	// beta
	if(multilayer->profile_Plot_Options.type == PERMITTIVITY && multilayer->profile_Plot_Options.permittivity_Type == BETA_EPS	)
	{
		beta_To_Plot_Vector.resize(data_Count);
		for(int i=0; i<data_Count; i++)
		{
			double z = -prefix + i*step;
			beta_To_Plot_Vector[i].key = z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
			beta_To_Plot_Vector[i].value = imag(delta_Beta_Epsilon_Func(z));

			arg[i] = beta_To_Plot_Vector[i].key;
			val[i] = beta_To_Plot_Vector[i].value;
		}
		custom_Plot->graph(0)->data()->set(beta_To_Plot_Vector);
	}
}

complex<double> Profile_Plot::delta_Beta_Epsilon_Func(double z)
{
	// where we are
	int min_Boundary_Index = -2019;
	int max_Boundary_Index = -2019;
	int sigma_Factor = 6;

	for(int j=0; j<boundary_Vector.size(); j++)
	{
		min_Boundary_Index = j-1;
		if(z-sigma_Factor*max_Sigma < boundary_Vector[j]) {break;}
	}
	for(int j=0; j<boundary_Vector.size(); j++)
	{
		max_Boundary_Index = j;
		if(z+sigma_Factor*max_Sigma < boundary_Vector[j]) {break;}
	}
	min_Boundary_Index = max(min_Boundary_Index,0);
	max_Boundary_Index = min(max_Boundary_Index,thickness_Vector.size()-1);

	double delta_Epsilon = 0;
	double beta_Epsilon = 0;
	double geometry_Factor = 1;
	for(int j=min_Boundary_Index; j<=max_Boundary_Index; j++) // instead of old slow for(int j=0; j<thickness_Vector.size(); j++)
	{
		geometry_Factor = layer_Norm_Vector[j] *
				Global_Variables::interface_Profile_Function(z-boundary_Vector[j  ], struct_Data_Vector[j+1].interlayer_Composition) *
				Global_Variables::interface_Profile_Function(boundary_Vector[j+1]-z, struct_Data_Vector[j+2].interlayer_Composition);

		delta_Epsilon += delta_Epsilon_Vector[j+1] * geometry_Factor;
		beta_Epsilon  += beta_Epsilon_Vector [j+1] * geometry_Factor;
	}
	if(max_Boundary_Index>=thickness_Vector.size()-1)
	{
		geometry_Factor = Global_Variables::interface_Profile_Function(z-boundary_Vector.last(), struct_Data_Vector.last().interlayer_Composition);

		delta_Epsilon += delta_Epsilon_Vector.last() * geometry_Factor;
		beta_Epsilon  += beta_Epsilon_Vector. last() * geometry_Factor;
	}
	return complex<double>(delta_Epsilon,beta_Epsilon);
}

void Profile_Plot::unwrap_Subtree(QVector<Data>& struct_Data_Vector, QTreeWidgetItem* item, int num_Repetition, int period_Index)
{
	for(int i=0; i<item->childCount(); ++i)
	{
		Data struct_Data = item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(struct_Data.item_Enabled)
		{
			if(struct_Data.item_Type == item_Type_Ambient ||
			   struct_Data.item_Type == item_Type_Layer   ||
			   struct_Data.item_Type == item_Type_Substrate)
			{
				// drift
				if(struct_Data.item_Type == item_Type_Layer &&
				   struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic )
				{
					// thickness drift
					Global_Variables::variable_Drift(struct_Data.thickness.value, struct_Data.thickness_Drift, period_Index, num_Repetition, nullptr);

					// sigma drift
					for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)	{
						Global_Variables::variable_Drift(struct_Data.interlayer_Composition[func_Index].my_Sigma.value, struct_Data.sigma_Drift, period_Index, num_Repetition, nullptr);
					}
					Global_Variables::variable_Drift(struct_Data.sigma.value, struct_Data.sigma_Drift, period_Index, num_Repetition, nullptr);
				}
				struct_Data_Vector.append(struct_Data);
			}

			if(item->child(i)->childCount()>0)
			{
				if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
				{
					for(int layer_Index=0; layer_Index<struct_Data.regular_Components.first().components.size(); layer_Index++)
					{
						for(int component_Index=0; component_Index<struct_Data.regular_Components.size(); component_Index++)
						{
							struct_Data_Vector.append(struct_Data.regular_Components[component_Index].components[layer_Index]);
						}
					}
				}

				if(struct_Data.item_Type == item_Type_Multilayer)
				{
					for(int period_Index=0; period_Index<struct_Data.num_Repetition.value(); period_Index++)
					{
						unwrap_Subtree(struct_Data_Vector, item->child(i), struct_Data.num_Repetition.value(), period_Index);
					}
				}
			}
		}
	}
}

void Profile_Plot::get_Max_My_Sigma(QTreeWidgetItem *item)
{
	// doesn't look for sigma if ambient only
	for(int i=0; i<item->childCount(); ++i)
	{
		Data struct_Data = item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(struct_Data.item_Enabled)
		{
			if(struct_Data.item_Type == item_Type_Layer   ||
			   struct_Data.item_Type == item_Type_Substrate)
			{
				for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; interlayer_Index++)
				{
					if(struct_Data.interlayer_Composition[interlayer_Index].enabled)
					{
						if(max_Sigma<struct_Data.interlayer_Composition[interlayer_Index].my_Sigma.value)
						{
							max_Sigma=struct_Data.interlayer_Composition[interlayer_Index].my_Sigma.value;
						}
					}
				}
			}

			if(item->child(i)->childCount()>0)
			{
				if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
				{
					for(int layer_Index=0; layer_Index<struct_Data.regular_Components.first().components.size(); layer_Index++)
					{
						for(int component_Index=0; component_Index<struct_Data.regular_Components.size(); component_Index++)
						{
							Data& component_Data = struct_Data.regular_Components[component_Index].components[layer_Index];
							for(int interlayer_Index=0; interlayer_Index<transition_Layer_Functions_Size; interlayer_Index++)
							{
								if(component_Data.interlayer_Composition[interlayer_Index].enabled)
								{
									if(max_Sigma<component_Data.interlayer_Composition[interlayer_Index].my_Sigma.value)
									{
										max_Sigma=component_Data.interlayer_Composition[interlayer_Index].my_Sigma.value;
									}
								}
							}

						}
					}
				}

				if(struct_Data.item_Type == item_Type_Multilayer ||
				   struct_Data.item_Type == item_Type_General_Aperiodic)
				{
					get_Max_My_Sigma(item->child(i));
				}
			}
		}
	}
}






