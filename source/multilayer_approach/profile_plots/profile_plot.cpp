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

	create_Plot_Frame_And_Scale();
	plot_Data();

	create_Left_Side();
	main_Layout->addWidget(custom_Plot);

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
				plot_Data();
			});
		at_Wavelength_Unints_Label = new QLabel(" " + multilayer->profile_Plot_Options.local_wavelength_units);

		QHBoxLayout* wavelength_Layout = new QHBoxLayout;
			wavelength_Layout->setAlignment(Qt::AlignLeft);
			wavelength_Layout->addWidget(at_Wavelength_LineEdit,0,Qt::AlignLeft);
			wavelength_Layout->addWidget(at_Wavelength_Unints_Label,0,Qt::AlignLeft);
			permittivity_Layout->addLayout(wavelength_Layout,2,5);

		delta_RadioButton = new QRadioButton("Re(1-"+Epsilon_Sym+")");
			permittivity_Layout->addWidget(delta_RadioButton,3,2,1,4);
			connect(delta_RadioButton, &QRadioButton::toggled, this, [=]
			{
				bool checked = delta_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.permittivity_Type = DELTA_EPS;
					custom_Plot->yAxis->setLabel("Re(1-"+Epsilon_Sym+")");

					// disable log scale
					lin_Y_RadioButton->setDisabled(true);
					log_Y_RadioButton->setDisabled(true);
				} else
				{
					// enable log scale
					lin_Y_RadioButton->setDisabled(false);
					log_Y_RadioButton->setDisabled(false);
				}
				plot_Data();
			});
			connect(delta_RadioButton, &QRadioButton::clicked, delta_RadioButton, &QRadioButton::toggled);

		beta_RadioButton = new QRadioButton("Im("+Epsilon_Sym+")");
			permittivity_Layout->addWidget(beta_RadioButton,4,2,1,4);
			connect(beta_RadioButton, &QRadioButton::toggled, this, [=]
			{
				bool checked = beta_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.permittivity_Type = BETA_EPS;
					custom_Plot->yAxis->setLabel("Im("+Epsilon_Sym+")");
				}
				plot_Data();
			});
			connect(beta_RadioButton, &QRadioButton::clicked, beta_RadioButton, &QRadioButton::toggled);

		QButtonGroup* delta_Beta_ButtonGroup = new QButtonGroup;
			delta_Beta_ButtonGroup->addButton(delta_RadioButton);
			delta_Beta_ButtonGroup->addButton(beta_RadioButton);

		connect(permittivity_RadioButton, &QRadioButton::toggled, this, [=]
		{
			bool checked = permittivity_RadioButton->isChecked();
			at_Wavelength_Label->setEnabled(checked);
			at_Wavelength_LineEdit->setEnabled(checked);
			at_Wavelength_Unints_Label->setEnabled(checked);

			if(checked)
			{
				multilayer->profile_Plot_Options.type = PERMITTIVITY;
				delta_RadioButton->toggled(delta_RadioButton->isChecked());
				beta_RadioButton ->toggled(beta_RadioButton ->isChecked());
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
		connect(permittivity_RadioButton, &QRadioButton::clicked, permittivity_RadioButton, &QRadioButton::toggled);

		// ----------------------------------------------------
		// materials
		// ----------------------------------------------------
		materials_RadioButton = new QRadioButton("Materials");
			value_Type_Layout->addWidget(materials_RadioButton);
			connect(materials_RadioButton, &QRadioButton::toggled, this, [=]
			{
				bool checked = materials_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.type = MATERIAL;
					custom_Plot->yAxis->setLabel("Relative density");
				}
				permittivity_RadioButton->toggled(permittivity_RadioButton->isChecked());
			});
			connect(materials_RadioButton, &QRadioButton::clicked, materials_RadioButton, &QRadioButton::toggled);

		// ----------------------------------------------------
		// elements
		// ----------------------------------------------------
		elements_RadioButton = new QRadioButton("Elements");
			// TODO
			elements_RadioButton->setDisabled(true);
			value_Type_Layout->addWidget(elements_RadioButton);
			connect(elements_RadioButton, &QRadioButton::toggled, this, [=]
			{
				bool checked = elements_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.type = ELEMENTS;
					custom_Plot->yAxis->setLabel("Absolute concentration, "+Multiply_Sym+"10"+Power_23_Sym+" cm"+Minus_Three_Sym);
				}
				permittivity_RadioButton->toggled(permittivity_RadioButton->isChecked());
			});
			connect(elements_RadioButton, &QRadioButton::clicked, elements_RadioButton, &QRadioButton::toggled);

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
				qInfo() << "cursor_Cordinate_CheckBox\n\n";

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
			connect(lin_Y_RadioButton, &QRadioButton::toggled, this, [&]
			{
				if(lin_Y_RadioButton->isChecked())
				{
					multilayer->profile_Plot_Options.y_Scale  = lin_Scale;
				}
				plot_Data();
			});
			connect(lin_Y_RadioButton, &QRadioButton::clicked, lin_Y_RadioButton, &QRadioButton::toggled);

		// log
		log_Y_RadioButton = new QRadioButton("Log");
			scale_Y_Layout->addWidget(log_Y_RadioButton,0,2);
			connect(log_Y_RadioButton, &QRadioButton::toggled, this, [&]
			{
				if(log_Y_RadioButton->isChecked())
				{
					multilayer->profile_Plot_Options.y_Scale = log_Scale;
				}
				plot_Data();
			});
			connect(log_Y_RadioButton, &QRadioButton::clicked, log_Y_RadioButton, &QRadioButton::toggled);

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

//	delta_RadioButton->toggled(multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS);// already toggled when ->setChecked(true)
//	beta_RadioButton->toggled(multilayer->profile_Plot_Options.permittivity_Type == BETA_EPS);	// already toggled when ->setChecked(true)
//	materials_RadioButton->toggled(multilayer->profile_Plot_Options.type == MATERIAL);			// already toggled when ->setChecked(true)
//	elements_RadioButton->toggled(multilayer->profile_Plot_Options.type == ELEMENTS);			// already toggled when ->setChecked(true)

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
//			custom_Plot->xAxis->setRange(-5, 25);

			custom_Plot->xAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->xAxis2->setTicker(linTicker);
			custom_Plot->xAxis2->setNumberFormat("g");
			custom_Plot->xAxis2->setNumberPrecision(4);
//			custom_Plot->xAxis2->setRange(-5, 25);
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
//			custom_Plot->yAxis->setRange(-1, 1);

			custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis2->setTicker(linTicker);
			custom_Plot->yAxis2->setNumberFormat("g");
			custom_Plot->yAxis2->setNumberPrecision(4);
//			custom_Plot->yAxis2->setRange(-1, 1);
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

	}
//	set_Title_Text();
}

void Profile_Plot::plot_Data(bool fast)
{
	qInfo() << "plot_Data\n\n";
	if(!fast)
	{
		create_Plot_Frame_And_Scale();
		calculate_Profile();
	}

	QVector<QCPGraphData> data_To_Plot(arg.size());
	for (int i=0; i<data_To_Plot.size(); ++i)
	{
		data_To_Plot[i].key = arg[i];
		data_To_Plot[i].value = val[i];

//		if(max<data_To_Plot[i].value) {max=data_To_Plot[i].value;}
//		if(min>data_To_Plot[i].value) {min=data_To_Plot[i].value;}
	}

	double minimum_Raw = *std::min_element(val.constBegin(), val.constEnd());
	double maximum_Raw = *std::max_element(val.constBegin(), val.constEnd());
	val.removeAll(0);
	double maximum_Raw_Non_Zero = *std::max_element(val.constBegin(), val.constEnd()); val.removeAll(0);
	double minimum_Raw_Non_Zero = *std::min_element(val.constBegin(), val.constEnd()); val.removeAll(0);
	double minimum = -2020;
	double maximum = -2020;
	if(multilayer->profile_Plot_Options.y_Scale == lin_Scale ||
	  (multilayer->profile_Plot_Options.type == PERMITTIVITY &&
	   multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS))
	{
		maximum = maximum_Raw + (maximum_Raw-minimum_Raw)*0.08;
		minimum = minimum_Raw - (maximum_Raw-minimum_Raw)*0.08;
	}
	if(multilayer->profile_Plot_Options.y_Scale == log_Scale &&
	   (multilayer->profile_Plot_Options.type != PERMITTIVITY ||
		multilayer->profile_Plot_Options.permittivity_Type != DELTA_EPS))
	{
		maximum = maximum_Raw_Non_Zero*1.5;
		minimum = minimum_Raw_Non_Zero/1.5;
	}

	if(!fast)
	{
		if(custom_Plot->graphCount()!=1)
		{
			custom_Plot->clearGraphs();
			custom_Plot->addGraph();
		}
//		custom_Plot->graph(0)->setPen(QPen(target_Curve->plot_Options_Experimental.color,target_Curve->plot_Options_Experimental.thickness));
		//custom_Plot->graph(0)->setBrush(QBrush(target_Curve->plot_Options.color));
		QCPScatterStyle scatter_Style;
//		scatter_Style.setShape(QCPScatterStyle::ScatterShape(target_Curve->plot_Options_Experimental.scatter_Shape));
//		scatter_Style.setSize(target_Curve->plot_Options_Experimental.scatter_Size);
		custom_Plot->graph(0)->setScatterStyle(scatter_Style);
		custom_Plot->graph(0)->setPen(QPen(custom_Plot->graph(0)->pen().color(), 1.3));
	}

	custom_Plot->graph(0)->data()->set(data_To_Plot);

	if(!fast)
	{
		if(multilayer->profile_Plot_Options.rescale_X)
		{
			custom_Plot->xAxis->setRange(arg.first(), arg.last());
		}
		if(multilayer->profile_Plot_Options.rescale_Y)
		{
			custom_Plot->yAxis->setRange(minimum,maximum);
		}
	}

	//custom_Plot->yAxis2->setTickLabels(false);

//	if(!fast)
//		refresh_Labels();
//	else
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
	qInfo() << "calculate_Profile\n\n";

	get_Max_My_Sigma(multilayer->structure_Tree->tree->invisibleRootItem());

	struct_Data_Vector.clear();
	unwrap_Subtree(struct_Data_Vector, multilayer->structure_Tree->tree->invisibleRootItem());

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

	auto start = std::chrono::system_clock::now();
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
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	qInfo() << "	norm:    "<< elapsed.count()/1000000. << " seconds" << endl << endl;

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
	int data_Count=9000;
	arg.resize(data_Count);
	val.resize(arg.size());

	auto start1 = std::chrono::system_clock::now();

	if(multilayer->profile_Plot_Options.type == PERMITTIVITY && multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS	)
	{
		for(int i=0; i<data_Count; i++)
		{
			double z = i/30. -50;
			arg[i]=z;
			val[i]=real(delta_Beta_Epsilon_Func(z));
		}
	}
	if(multilayer->profile_Plot_Options.type == PERMITTIVITY && multilayer->profile_Plot_Options.permittivity_Type == BETA_EPS	)
	{
		for(int i=0; i<data_Count; i++)
		{
			double z = i/30. -50;
			arg[i]=z;
			val[i]=imag(delta_Beta_Epsilon_Func(z));
		}
	}

	auto end1 = std::chrono::system_clock::now();
	auto elapsed1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
	qInfo() << "	profile plot:    "<< elapsed1.count()/1000000. << " seconds" << endl << endl << endl;
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

void Profile_Plot::unwrap_Subtree(QVector<Data>& struct_Data_Vector, QTreeWidgetItem* item)
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
						unwrap_Subtree(struct_Data_Vector, item->child(i));
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







