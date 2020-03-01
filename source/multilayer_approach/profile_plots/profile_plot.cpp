#include "profile_plot.h"

Profile_Plot::Profile_Plot(Multilayer* multilayer, Profile_Plots_Window* profile_Plots_Window, QWidget *parent) :
	multilayer(multilayer),
	profile_Plots_Window(profile_Plots_Window),
	plotting_Threads(reflectivity_Calc_Threads),
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
	custom_Plot->addGraph();

	horizontall_Scrollbar = new QScrollBar(Qt::Horizontal);
	connect(horizontall_Scrollbar, &QScrollBar::valueChanged, this, &Profile_Plot::horizontall_ScrollbarChanged);
	connect(custom_Plot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), this, &Profile_Plot::x_Axis_RangeChanged);
	horizontall_Scrollbar->blockSignals(true);

	// synchronize selection of graphs with selection of corresponding legend items:
	connect(custom_Plot, &QCustomPlot::selectionChangedByUser, this, [=]
	{
		for (int i=0; i<custom_Plot->graphCount(); ++i)
		{
			QCPGraph* graph = custom_Plot->graph(i);
			if(custom_Plot->legend->hasItemWithPlottable(graph))
			{
				QCPPlottableLegendItem* item = custom_Plot->legend->itemWithPlottable(graph);
				if (item->selected() || graph->selected())
				{
					item->setSelected(true);
					graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
				}
			}
		}
	});
	custom_Plot->legend->setSelectableParts(QCPLegend::spItems);
	custom_Plot->legend->setSelectedIconBorderPen(QPen(Qt::black, 1));
	connect(custom_Plot, &QCustomPlot::plottableDoubleClick, this, &Profile_Plot::hide_Show_Other_Plots);

	if(multilayer->profile_Plot_Options.show_Cursor_Position)
	{
		QCPItemText* text_Item = new QCPItemText(custom_Plot);
		text_Item->setText("");
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

void Profile_Plot::horizontall_ScrollbarChanged(int value)
{
	if (qAbs(custom_Plot->xAxis->range().center()-value/scrollbar_Factor) > 1./scrollbar_Factor) // if user is dragging plot, we don't want to replot twice
	{
		custom_Plot->xAxis->setRange(value/scrollbar_Factor, custom_Plot->xAxis->range().size(), Qt::AlignCenter);
		custom_Plot->replot();
	}
}

void Profile_Plot::x_Axis_RangeChanged(const QCPRange& range)
{
	horizontall_Scrollbar->setValue(qRound(range.center()*scrollbar_Factor)); // adjust position of scroll bar slider
	horizontall_Scrollbar->setPageStep(qRound(range.size()*scrollbar_Factor)); // adjust size of scroll bar slider
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
		at_Wavelength_LineEdit = new QLineEdit(Locale.toString(Global_Variables::wavelength_Energy(multilayer->profile_Plot_Options.local_wavelength_units, multilayer->profile_Plot_Options.local_Wavelength)/wavelength_Coefficients_Map.value(multilayer->profile_Plot_Options.local_wavelength_units),line_edit_double_format,line_edit_wavelength_precision));
			at_Wavelength_LineEdit->setFixedWidth(80);
			at_Wavelength_LineEdit->setProperty(min_Size_Property, 80);
			at_Wavelength_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			connect(at_Wavelength_LineEdit, &QLineEdit::textEdited, this, [=]{Global_Variables::resize_Line_Edit(at_Wavelength_LineEdit, false);});
			connect(at_Wavelength_LineEdit, &QLineEdit::editingFinished, this, [=]
			{
				multilayer->profile_Plot_Options.local_Wavelength = Global_Variables::wavelength_Energy(multilayer->profile_Plot_Options.local_wavelength_units, Locale.toDouble(at_Wavelength_LineEdit->text()))*wavelength_Coefficients_Map.value(multilayer->profile_Plot_Options.local_wavelength_units);
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
					plot_Data(true);
				}
			});

		// ----------------------------------------------------
		// elements
		// ----------------------------------------------------
		elements_RadioButton = new QRadioButton("Elements");
			value_Type_Layout->addWidget(elements_RadioButton);
			connect(elements_RadioButton, &QRadioButton::clicked, this, [=]
			{
				bool checked = elements_RadioButton->isChecked();
				if(checked)
				{
					multilayer->profile_Plot_Options.type = ELEMENTS;
					custom_Plot->yAxis->setLabel("Absolute concentration, cm"+Minus_Three_Sym);

					permittivity_RadioButton->clicked(permittivity_RadioButton->isChecked());
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
//		use_Roughness_CheckBox = new QCheckBox("Apply roughness");
//			// line_Type_Layout->addWidget(use_Roughness_CheckBox);
//			connect(use_Roughness_CheckBox, &QCheckBox::toggled, this, [=]
//			{
//				multilayer->profile_Plot_Options.apply_Roughness = use_Roughness_CheckBox->isChecked();
//			});

//		use_Diffusiness_CheckBox = new QCheckBox("Apply diffusiness");
//			// line_Type_Layout->addWidget(use_Diffusiness_CheckBox);
//			connect(use_Diffusiness_CheckBox, &QCheckBox::toggled, this, [=]
//			{
//				multilayer->profile_Plot_Options.apply_Diffusiness = use_Diffusiness_CheckBox->isChecked();
//			});

		show_Sharp_CheckBox = new QCheckBox("Show sharp profile");
			line_Type_Layout->addWidget(show_Sharp_CheckBox);
			connect(show_Sharp_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.show_Sharp_Profile = show_Sharp_CheckBox->isChecked();

				plot_Data(true);
			});

		discretization_CheckBox = new QCheckBox("Show discretization");
			line_Type_Layout->addWidget(discretization_CheckBox);
			connect(discretization_CheckBox, &QCheckBox::toggled, this, [=]
			{
				multilayer->profile_Plot_Options.show_Discretization = discretization_CheckBox->isChecked();

				plot_Data(true);
			});
		if(multilayer->discretization_Parameters.enable_Discretization) {discretization_CheckBox->setDisabled(false);}
		else															{discretization_CheckBox->setDisabled(true);}

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

//	use_Roughness_CheckBox->setChecked(multilayer->profile_Plot_Options.apply_Roughness);
//	use_Diffusiness_CheckBox->setChecked(multilayer->profile_Plot_Options.apply_Diffusiness);
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
		custom_Plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom /*| QCP::iSelectAxes*/ | QCP::iSelectLegend);

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

	// data
	if(recalculate_Profile)
	{
//		auto start = std::chrono::system_clock::now();
		calculate_Profile();
//		auto end = std::chrono::system_clock::now();
//		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		qInfo() << "	calculate_Profile:    "<< elapsed.count()/1000000. << " seconds" << endl << endl << endl;
	}

	// rescaling
	double minimum = -2020, maximum = 2020;
	if(multilayer->profile_Plot_Options.y_Scale == lin_Scale ||
	  (multilayer->profile_Plot_Options.type == PERMITTIVITY &&
	   multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS))
	{
		QVector<double> val_Final;
		QVector<double> val_Sharp_Final;

		if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
		{
			val_Final = val;
		}
		if(multilayer->profile_Plot_Options.type == MATERIAL)
		{
			val_Final.clear();
			for(int material_index = 0; material_index<different_Materials.size(); material_index++)
			{
				val_Final.append(val_Multiple[material_index]);
			}
		}
		if(multilayer->profile_Plot_Options.type == ELEMENTS)
		{
			val_Final.clear();
			for(int element_index = 0; element_index<different_Elements.size(); element_index++)
			{
				val_Final.append(val_Multiple[element_index]);
			}
		}

		double minimum_Raw = *std::min_element(val_Final.constBegin(), val_Final.constEnd());
		double maximum_Raw = *std::max_element(val_Final.constBegin(), val_Final.constEnd());

		// sharp profile
		if(multilayer->profile_Plot_Options.show_Sharp_Profile)
		{
			if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
			{
				minimum_Raw = min(minimum_Raw, *std::min_element(val_Sharp.constBegin(), val_Sharp.constEnd()));
				maximum_Raw = max(maximum_Raw, *std::max_element(val_Sharp.constBegin(), val_Sharp.constEnd()));
			}
			if(multilayer->profile_Plot_Options.type == MATERIAL)
			{
				val_Sharp_Final.clear();
				for(int material_index = 0; material_index<different_Materials.size(); material_index++)
				{
					val_Sharp_Final.append(val_Sharp_Multiple[material_index]);
				}

				minimum_Raw = min(minimum_Raw, *std::min_element(val_Sharp_Final.constBegin(), val_Sharp_Final.constEnd()));
				maximum_Raw = max(maximum_Raw, *std::max_element(val_Sharp_Final.constBegin(), val_Sharp_Final.constEnd()));
			}
			if(multilayer->profile_Plot_Options.type == ELEMENTS)
			{
				val_Sharp_Final.clear();
				for(int element_index = 0; element_index<different_Elements.size(); element_index++)
				{
					val_Sharp_Final.append(val_Sharp_Multiple[element_index]);
				}

				minimum_Raw = min(minimum_Raw, *std::min_element(val_Sharp_Final.constBegin(), val_Sharp_Final.constEnd()));
				maximum_Raw = max(maximum_Raw, *std::max_element(val_Sharp_Final.constBegin(), val_Sharp_Final.constEnd()));
			}
		}

		minimum = minimum_Raw - (maximum_Raw-minimum_Raw)*0.05;
		maximum = maximum_Raw + (maximum_Raw-minimum_Raw)*0.08;
	}
	if(multilayer->profile_Plot_Options.y_Scale == log_Scale &&
	   (multilayer->profile_Plot_Options.type != PERMITTIVITY ||
		multilayer->profile_Plot_Options.permittivity_Type != DELTA_EPS))
	{
		QVector<double> val_Non_Zero;
		QVector<double> val_Sharp_Non_Zero;

		// removing zeros
		if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
		{
			val_Non_Zero = val;
			for(int i=val_Non_Zero.size()-1; i>=0; i--)
			{
				if(arg[i]<0 || arg[i]>boundary_Vector.last() || val_Non_Zero[i]<default_logarithmic_threshold_beta) {val_Non_Zero.remove(i);}
			}
		}
		if(multilayer->profile_Plot_Options.type == MATERIAL)
		{
			val_Non_Zero.clear();
			for(int material_index = 0; material_index<different_Materials.size(); material_index++)
			{
				for(int i=val_Multiple[material_index].size()-1; i>=0; i--)
				{
					if(arg[i]<0/* || arg[i]>boundary_Vector.last() */|| val_Multiple[material_index][i]<default_logarithmic_threshold_material_density) {val_Multiple[material_index].remove(i);}
				}
				val_Non_Zero.append(val_Multiple[material_index]);
			}
		}
		if( multilayer->profile_Plot_Options.type == ELEMENTS)
		{
			val_Non_Zero.clear();
			for(int element_index = 0; element_index<different_Elements.size(); element_index++)
			{
				for(int i=val_Multiple[element_index].size()-1; i>=0; i--)
				{
					if(arg[i]<0/* || arg[i]>boundary_Vector.last() */|| val_Multiple[element_index][i]<default_logarithmic_threshold_element_concentration) {val_Multiple[element_index].remove(i);}
				}
				val_Non_Zero.append(val_Multiple[element_index]);
			}
		}

		double minimum_Raw_Non_Zero = *std::min_element(val_Non_Zero.constBegin(), val_Non_Zero.constEnd());
		double maximum_Raw_Non_Zero = *std::max_element(val_Non_Zero.constBegin(), val_Non_Zero.constEnd());

		// sharp profile
		if(multilayer->profile_Plot_Options.show_Sharp_Profile)
		{
			if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
			{
				val_Sharp_Non_Zero = val_Sharp;
				for(int i=val_Sharp.size()-1; i>=0; i--)
				{
					if(val_Sharp_Non_Zero[i]<default_logarithmic_threshold_beta) {val_Sharp_Non_Zero.remove(i);}
				}

				minimum_Raw_Non_Zero = min(minimum_Raw_Non_Zero, *std::min_element(val_Sharp_Non_Zero.constBegin(), val_Sharp_Non_Zero.constEnd()));
				maximum_Raw_Non_Zero = max(maximum_Raw_Non_Zero, *std::max_element(val_Sharp_Non_Zero.constBegin(), val_Sharp_Non_Zero.constEnd()));
			}
			if(multilayer->profile_Plot_Options.type == MATERIAL)
			{
				val_Sharp_Non_Zero.clear();
				for(int material_index = 0; material_index<different_Materials.size(); material_index++)
				{
					for(int i=val_Sharp_Multiple[material_index].size()-1; i>=0; i--)
					{
						if(val_Sharp_Multiple[material_index][i]<default_logarithmic_threshold_material_density) {val_Sharp_Multiple[material_index].remove(i);}
					}
					val_Sharp_Non_Zero.append(val_Sharp_Multiple[material_index]);
				}

				minimum_Raw_Non_Zero = min(minimum_Raw_Non_Zero, *std::min_element(val_Sharp_Non_Zero.constBegin(), val_Sharp_Non_Zero.constEnd()));
				maximum_Raw_Non_Zero = max(maximum_Raw_Non_Zero, *std::max_element(val_Sharp_Non_Zero.constBegin(), val_Sharp_Non_Zero.constEnd()));
			}
			if(multilayer->profile_Plot_Options.type == ELEMENTS)
			{
				val_Sharp_Non_Zero.clear();
				for(int element_index = 0; element_index<different_Elements.size(); element_index++)
				{
					for(int i=val_Sharp_Multiple[element_index].size()-1; i>=0; i--)
					{
						if(val_Sharp_Multiple[element_index][i]<default_logarithmic_threshold_element_concentration) {val_Sharp_Multiple[element_index].remove(i);}
					}
					val_Sharp_Non_Zero.append(val_Sharp_Multiple[element_index]);
				}

				minimum_Raw_Non_Zero = min(minimum_Raw_Non_Zero, *std::min_element(val_Sharp_Non_Zero.constBegin(), val_Sharp_Non_Zero.constEnd()));
				maximum_Raw_Non_Zero = max(maximum_Raw_Non_Zero, *std::max_element(val_Sharp_Non_Zero.constBegin(), val_Sharp_Non_Zero.constEnd()));
			}
		}

		minimum = minimum_Raw_Non_Zero/1.5;
		maximum = maximum_Raw_Non_Zero*1.5;
	}

	if(multilayer->profile_Plot_Options.rescale_X)	{ custom_Plot->xAxis->setRange(arg.first(), arg.last());	}
	else											{ custom_Plot->xAxis->setRange(multilayer->profile_Plot_Options.old_X_Begin, multilayer->profile_Plot_Options.old_X_End);}
	if(multilayer->profile_Plot_Options.rescale_Y)	{ custom_Plot->yAxis->setRange(minimum, maximum);			}
	else											{ custom_Plot->yAxis->setRange(multilayer->profile_Plot_Options.old_Y_Begin, multilayer->profile_Plot_Options.old_Y_End);}

	// scrollbar
	horizontall_Scrollbar->setRange(arg.first()*scrollbar_Factor, arg.last()*scrollbar_Factor);
	custom_Plot->xAxis->rangeChanged(custom_Plot->xAxis->range());
	horizontall_Scrollbar->blockSignals(false);

	custom_Plot->replot();
}

void Profile_Plot::calculate_Profile()
{
	struct_Data_Counter = 1; // ambient is the first
	max_Sigma = 0.1;
	get_Max_My_Sigma(multilayer->structure_Tree->tree->invisibleRootItem());

	different_Materials.clear();
	different_Elements.clear();
	discrete_Step_Vector.clear();
	struct_Data_Vector.resize(struct_Data_Counter);
	delta_Epsilon_Vector.resize(struct_Data_Vector.size());
	beta_Epsilon_Vector.resize(struct_Data_Vector.size());
	element_Concentration_Map_Vector.resize(struct_Data_Vector.size());
	struct_Data_Index = 0;

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

	// norm. Optimize (before unwrapping, check similar layers ....). Now more or less OK
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
				Different_Norm_Layer old_Norm = different_Norm_Layer[different_Norm_Layer.indexOf(temp_Dif_Norm)];
				layer_Norm_Vector[layer_Index] = old_Norm.norm;
			}
		} else
		{
			layer_Norm_Vector[layer_Index] = 1;
		}
	}
	gsl_integration_workspace_free(w);

	// profiling
	double step = 0.2;	// in angstroms
	double prefix = 15, suffix = 15;
	Global_Variables::get_Prefix_Suffix(prefix, suffix, max_Sigma);
	double length = prefix+boundary_Vector.last()+suffix;
	int data_Count = ceil(length/step)+1;
	int limit = 20000; // restriction
	data_Count = min(data_Count, limit);
	step = length/(data_Count-1);
//	if(step >= 2)
//	{
//		QMessageBox::warning(nullptr,"Too many layers to show", "For correct representation of profile\nnumber of layers should be diminished");
//		qInfo() << "\n---------------------------------------------------------------------------------"
//				   "\n   Too many layers to show."
//				   "\n   For correct representation of profile"
//				м   "\n   number of layers should be diminished"
//				   "\n---------------------------------------------------------------------------------\n";
//	}

	// discretization
	int num_Prefix_Slices =1, num_Suffix_Slices = 1;
	if(multilayer->discretization_Parameters.enable_Discretization && multilayer->profile_Plot_Options.show_Discretization)
	{
		Global_Variables::discretize_Prefix_Suffix(prefix, suffix, num_Prefix_Slices, num_Suffix_Slices, discrete_Step_Vector, multilayer->discretization_Parameters.discretization_Step);
		num_Slices = int(discrete_Step_Vector.size());
	}

	map_Sharp_Smooth.clear();

	// multithreading
	{
		boundary_Vector_Std_Threaded.resize(plotting_Threads);
		layer_Norm_Vector_Threaded.resize(plotting_Threads);
		struct_Data_Vector_Threaded.resize(plotting_Threads);
		delta_Epsilon_Vector_Threaded.resize(plotting_Threads);
		beta_Epsilon_Vector_Threaded.resize(plotting_Threads);
		element_Concentration_Map_Vector_Threaded.resize(plotting_Threads);

		for(int thread_Index=0; thread_Index<plotting_Threads; thread_Index++)
		{
			boundary_Vector_Std_Threaded[thread_Index] = boundary_Vector.toStdVector();
			layer_Norm_Vector_Threaded[thread_Index] = layer_Norm_Vector.toStdVector();
			struct_Data_Vector_Threaded[thread_Index] = struct_Data_Vector.toStdVector();
			delta_Epsilon_Vector_Threaded[thread_Index] = delta_Epsilon_Vector.toStdVector();
			beta_Epsilon_Vector_Threaded[thread_Index] = beta_Epsilon_Vector.toStdVector();
			element_Concentration_Map_Vector_Threaded[thread_Index] = element_Concentration_Map_Vector.toStdVector();
		}
	}

	// delta
	if(multilayer->profile_Plot_Options.type == PERMITTIVITY && multilayer->profile_Plot_Options.permittivity_Type == DELTA_EPS	)
	{		
		QCPGraph* sharp_Graph;
		custom_Plot->clearGraphs();

		// sharp profile
		if(multilayer->profile_Plot_Options.show_Sharp_Profile)
		{
			custom_Plot->addGraph();
			sharp_Delta_To_Plot_Vector.resize(struct_Data_Vector.size()+1);
			val_Sharp.resize(struct_Data_Vector.size());

			// first point
			{
				sharp_Delta_To_Plot_Vector.first().key   = -prefix/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
				sharp_Delta_To_Plot_Vector.first().value = delta_Epsilon_Vector.first();

				val_Sharp.first() = delta_Epsilon_Vector.first();
			}
			// main part
			for(int media_Index=1; media_Index<struct_Data_Vector.size()-1; media_Index++)
			{
				sharp_Delta_To_Plot_Vector[media_Index].key   = boundary_Vector[media_Index-1]/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
				sharp_Delta_To_Plot_Vector[media_Index].value = delta_Epsilon_Vector[media_Index];

				val_Sharp[media_Index] = delta_Epsilon_Vector[media_Index];
			}
			// last point
			{
				sharp_Delta_To_Plot_Vector[sharp_Delta_To_Plot_Vector.size()-2].key   = boundary_Vector.last()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
				sharp_Delta_To_Plot_Vector[sharp_Delta_To_Plot_Vector.size()-2].value = delta_Epsilon_Vector.last();

				val_Sharp.last() = delta_Epsilon_Vector.last();

				// additional point (for stepplot)
				sharp_Delta_To_Plot_Vector.last().key   = (boundary_Vector.last()+suffix)/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
				sharp_Delta_To_Plot_Vector.last().value = delta_Epsilon_Vector.last();
			}

			custom_Plot->graph()->data()->set(sharp_Delta_To_Plot_Vector);
			custom_Plot->graph()->setPen(QPen(Qt::darkCyan, default_Profile_Line_Thickness));
			custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
			custom_Plot->graph()->setLineStyle(QCPGraph::lsStepLeft);
			sharp_Graph = custom_Plot->graph();
		}

		// smooth/discrete profile
		{
			custom_Plot->addGraph();
			if(multilayer->discretization_Parameters.enable_Discretization && multilayer->profile_Plot_Options.show_Discretization)
			{
				delta_To_Plot_Vector.resize(num_Slices);
				arg.resize(num_Slices);
				val.resize(num_Slices);

				double real_Z = -(num_Prefix_Slices-0.5)*discrete_Step_Vector.front();
				for(int i=0; i<num_Slices; ++i)
				{
					double visible_Z = real_Z-discrete_Step_Vector[i]/2.; // where we dispose point
					delta_To_Plot_Vector[i].key = visible_Z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					delta_To_Plot_Vector[i].value = real(delta_Beta_Epsilon_Func(real_Z));

					arg[i] = delta_To_Plot_Vector[i].key;
					val[i] = delta_To_Plot_Vector[i].value;

					if(i<(num_Slices-1)) {
						real_Z += (discrete_Step_Vector[i]+discrete_Step_Vector[i+1])/2.; // real z, where we calculate epsilon
					} else
					{
						real_Z += discrete_Step_Vector[i]; // real z, where we calculate epsilon
					}
				}

				// add ambient
//				QCPGraphData ambient;			
//					ambient.key = delta_To_Plot_Vector.first().key-discrete_Step_Vector.front()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
//					ambient.value = beta_Epsilon_Vector.first();
//				delta_To_Plot_Vector.prepend(ambient);
//				arg.prepend(delta_To_Plot_Vector.first().key);
//				val.prepend(delta_To_Plot_Vector.first().value);

				// add substrate
				QCPGraphData substrate;
					substrate.key = delta_To_Plot_Vector.last().key+discrete_Step_Vector.back()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					substrate.value = delta_Epsilon_Vector.last();
				delta_To_Plot_Vector.append(substrate);
				arg.append(delta_To_Plot_Vector.last().key);
				val.append(delta_To_Plot_Vector.last().value);

				custom_Plot->graph()->setLineStyle(QCPGraph::lsStepLeft);
			} else
			{
				delta_To_Plot_Vector.resize(data_Count);
				arg.resize(data_Count);
				val.resize(data_Count);

				Global_Variables::parallel_For(data_Count, reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
				{
					for(int i=n_Min; i<n_Max; ++i)
					{
						double z = -prefix + i*step;
						delta_To_Plot_Vector[i].key = z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
						delta_To_Plot_Vector[i].value = real(delta_Beta_Epsilon_Func(z, thread_Index));
					}
				});
				for(int i=0; i<data_Count; i++)
				{
					arg[i] = delta_To_Plot_Vector[i].key;
					val[i] = delta_To_Plot_Vector[i].value;
				}

			}
			custom_Plot->graph()->data()->set(delta_To_Plot_Vector);
			custom_Plot->graph()->setPen(QPen(Qt::blue, default_Profile_Line_Thickness));
			custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
		}

		// connection between sharp and smooth plots
		if(multilayer->profile_Plot_Options.show_Sharp_Profile)
		{
			map_Sharp_Smooth.insert(custom_Plot->graph(), sharp_Graph);
		}
		custom_Plot->legend->setVisible(false);
	}
	// beta
	if(multilayer->profile_Plot_Options.type == PERMITTIVITY && multilayer->profile_Plot_Options.permittivity_Type == BETA_EPS	)
	{
		QCPGraph* sharp_Graph;
		custom_Plot->clearGraphs();

		// sharp profile
		if(multilayer->profile_Plot_Options.show_Sharp_Profile)
		{
			custom_Plot->addGraph();
			sharp_Beta_To_Plot_Vector.resize(struct_Data_Vector.size()+1);
			val_Sharp.resize(struct_Data_Vector.size());

			// first point
			{
				sharp_Beta_To_Plot_Vector.first().key   = -prefix/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
				sharp_Beta_To_Plot_Vector.first().value = beta_Epsilon_Vector.first();

				val_Sharp.first() = beta_Epsilon_Vector.first();
			}
			// main part
			for(int media_Index=1; media_Index<struct_Data_Vector.size()-1; media_Index++)
			{
				sharp_Beta_To_Plot_Vector[media_Index].key   = boundary_Vector[media_Index-1]/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
				sharp_Beta_To_Plot_Vector[media_Index].value = beta_Epsilon_Vector[media_Index];

				val_Sharp[media_Index] = beta_Epsilon_Vector[media_Index];
			}
			// last point
			{
				sharp_Beta_To_Plot_Vector[sharp_Beta_To_Plot_Vector.size()-2].key   = boundary_Vector.last()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
				sharp_Beta_To_Plot_Vector[sharp_Beta_To_Plot_Vector.size()-2].value = beta_Epsilon_Vector.last();

				val_Sharp.last() = beta_Epsilon_Vector.last();

				// additional point (for stepplot)
				sharp_Beta_To_Plot_Vector.last().key   = (boundary_Vector.last()+suffix)/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
				sharp_Beta_To_Plot_Vector.last().value = beta_Epsilon_Vector.last();
			}

			custom_Plot->graph()->data()->set(sharp_Beta_To_Plot_Vector);
			custom_Plot->graph()->setPen(QPen(Qt::darkMagenta, default_Profile_Line_Thickness));
			custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
			custom_Plot->graph()->setLineStyle(QCPGraph::lsStepLeft);
			sharp_Graph = custom_Plot->graph();
		}

		// smooth/discrete profile
		{
			custom_Plot->addGraph();
			if(multilayer->discretization_Parameters.enable_Discretization && multilayer->profile_Plot_Options.show_Discretization)
			{
				beta_To_Plot_Vector.resize(num_Slices);
				arg.resize(num_Slices);
				val.resize(num_Slices);

				double real_Z = -(num_Prefix_Slices-0.5)*discrete_Step_Vector.front();
				for(int i=0; i<num_Slices; ++i)
				{
					double visible_Z = real_Z-discrete_Step_Vector[i]/2.; // where we dispose point
					beta_To_Plot_Vector[i].key = visible_Z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					beta_To_Plot_Vector[i].value = imag(delta_Beta_Epsilon_Func(real_Z));

					arg[i] = beta_To_Plot_Vector[i].key;
					val[i] = beta_To_Plot_Vector[i].value;

					if(i<(num_Slices-1)) {
						real_Z += (discrete_Step_Vector[i]+discrete_Step_Vector[i+1])/2.; // real z, where we calculate epsilon
					} else
					{
						real_Z += discrete_Step_Vector[i]; // real z, where we calculate epsilon
					}
				}

				// add ambient
//				QCPGraphData ambient;
//					ambient.key = beta_To_Plot_Vector.first().key-discrete_Step_Vector.front()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
//					ambient.value = beta_Epsilon_Vector.first();
//				beta_To_Plot_Vector.prepend(ambient);
//				arg.prepend(beta_To_Plot_Vector.first().key);
//				val.prepend(beta_To_Plot_Vector.first().value);

				// add substrate
				QCPGraphData substrate;
					substrate.key = beta_To_Plot_Vector.last().key+discrete_Step_Vector.back()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					substrate.value = beta_Epsilon_Vector.last();
				beta_To_Plot_Vector.append(substrate);
				arg.append(beta_To_Plot_Vector.last().key);
				val.append(beta_To_Plot_Vector.last().value);

				custom_Plot->graph()->setLineStyle(QCPGraph::lsStepLeft);
			} else
			{
				beta_To_Plot_Vector.resize(data_Count);
				arg.resize(data_Count);
				val.resize(data_Count);

				Global_Variables::parallel_For(data_Count, reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
				{
					for(int i=n_Min; i<n_Max; ++i)
					{
						double z = -prefix + i*step;
						beta_To_Plot_Vector[i].key = z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
						beta_To_Plot_Vector[i].value = imag(delta_Beta_Epsilon_Func(z, thread_Index));
					}
				});
				for(int i=0; i<data_Count; i++)
				{
					arg[i] = beta_To_Plot_Vector[i].key;
					val[i] = beta_To_Plot_Vector[i].value;
				}
			}
			custom_Plot->graph()->data()->set(beta_To_Plot_Vector);
			custom_Plot->graph()->setPen(QPen(Qt::red, default_Profile_Line_Thickness));
			custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
		}

		// connection between sharp and smooth plots
		if(multilayer->profile_Plot_Options.show_Sharp_Profile)
		{
			map_Sharp_Smooth.insert(custom_Plot->graph(), sharp_Graph);
		}
		custom_Plot->legend->setVisible(false);
	}
	// materials
	if(multilayer->profile_Plot_Options.type == MATERIAL)
	{
		materials_To_Plot_Vector_Vector      .resize(different_Materials.size());
		sharp_Materials_To_Plot_Vector_Vector.resize(different_Materials.size());

		val_Multiple      .resize(different_Materials.size());
		val_Sharp_Multiple.resize(different_Materials.size());

		custom_Plot->clearGraphs();

		for(int material_index = 0; material_index<different_Materials.size(); material_index++)
		{
			QCPGraph* sharp_Graph;

			// sharp profile
			if(multilayer->profile_Plot_Options.show_Sharp_Profile)
			{
				sharp_Materials_To_Plot_Vector_Vector[material_index].resize(struct_Data_Vector.size()+1);
				val_Sharp_Multiple[material_index].resize(struct_Data_Vector.size());

				custom_Plot->addGraph();

				// first point
				{
					sharp_Materials_To_Plot_Vector_Vector[material_index].first().key = -prefix/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					if(struct_Data_Vector.first().approved_Material == different_Materials[material_index])
					{
						sharp_Materials_To_Plot_Vector_Vector[material_index].first().value = struct_Data_Vector.first().relative_Density.value;
					} else
					{
						sharp_Materials_To_Plot_Vector_Vector[material_index].first().value = 0;
					}
					val_Sharp_Multiple[material_index].first() = sharp_Materials_To_Plot_Vector_Vector[material_index].first().value;
				}
				// main part
				for(int media_Index=1; media_Index<struct_Data_Vector.size()-1; media_Index++)
				{
					sharp_Materials_To_Plot_Vector_Vector[material_index][media_Index].key = boundary_Vector[media_Index-1]/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					if(struct_Data_Vector[media_Index].approved_Material == different_Materials[material_index])
					{
						sharp_Materials_To_Plot_Vector_Vector[material_index][media_Index].value = struct_Data_Vector[media_Index].relative_Density.value;
					} else
					{
						sharp_Materials_To_Plot_Vector_Vector[material_index][media_Index].value = 0;
					}
					val_Sharp_Multiple[material_index][media_Index] = sharp_Materials_To_Plot_Vector_Vector[material_index][media_Index].value;
				}
				// last point
				{
					sharp_Materials_To_Plot_Vector_Vector[material_index][sharp_Materials_To_Plot_Vector_Vector[material_index].size()-2].key = boundary_Vector.last()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					if(struct_Data_Vector.last().approved_Material == different_Materials[material_index])
					{
						sharp_Materials_To_Plot_Vector_Vector[material_index][sharp_Materials_To_Plot_Vector_Vector[material_index].size()-2].value = struct_Data_Vector.last().relative_Density.value;
					} else
					{
						sharp_Materials_To_Plot_Vector_Vector[material_index][sharp_Materials_To_Plot_Vector_Vector[material_index].size()-2].value = 0;
					}
					val_Sharp_Multiple[material_index].last() = sharp_Materials_To_Plot_Vector_Vector[material_index][sharp_Materials_To_Plot_Vector_Vector[material_index].size()-2].value;

					// additional point (for stepplot)
					sharp_Materials_To_Plot_Vector_Vector[material_index].last().key   = (boundary_Vector.last()+suffix)/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					sharp_Materials_To_Plot_Vector_Vector[material_index].last().value = sharp_Materials_To_Plot_Vector_Vector[material_index][sharp_Materials_To_Plot_Vector_Vector[material_index].size()-2].value;
				}

				custom_Plot->graph()->data()->set(sharp_Materials_To_Plot_Vector_Vector[material_index]);
				custom_Plot->graph()->setPen(QPen(color_Contrast_Adjoint_Sequence[material_index%color_Contrast_Adjoint_Sequence.size()], default_Profile_Line_Thickness));
				custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
				custom_Plot->graph()->setLineStyle(QCPGraph::lsStepLeft);
				custom_Plot->graph()->removeFromLegend();
				sharp_Graph = custom_Plot->graph();
			}

			// smooth/discrete profile
			{
				custom_Plot->addGraph();
				if(multilayer->discretization_Parameters.enable_Discretization && multilayer->profile_Plot_Options.show_Discretization)
				{
					val_Multiple[material_index].resize(num_Slices);
					arg.resize(num_Slices);
					materials_To_Plot_Vector_Vector[material_index].resize(num_Slices);

					double real_Z = -(num_Prefix_Slices-0.5)*discrete_Step_Vector.front();
					for(int i=0; i<num_Slices; ++i)
					{
						double visible_Z = real_Z-discrete_Step_Vector[i]/2.; // where we dispose point
						materials_To_Plot_Vector_Vector[material_index][i].key = visible_Z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
						materials_To_Plot_Vector_Vector[material_index][i].value = real(delta_Beta_Epsilon_Func(real_Z, 0, different_Materials[material_index]));

						arg[i] = materials_To_Plot_Vector_Vector[material_index][i].key;
						val_Multiple[material_index][i] = materials_To_Plot_Vector_Vector[material_index][i].value;

						if(i<(num_Slices-1)) {
							real_Z += (discrete_Step_Vector[i]+discrete_Step_Vector[i+1])/2.; // real z, where we calculate epsilon
						} else
						{
							real_Z += discrete_Step_Vector[i]; // real z, where we calculate epsilon
						}
					}

					// add ambient
//					QCPGraphData ambient;
//						ambient.key = materials_To_Plot_Vector_Vector[material_index].first().key-discrete_Step_Vector.front()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
//						ambient.value = struct_Data_Vector.first().relative_Density.value;
//					materials_To_Plot_Vector_Vector[material_index].prepend(ambient);
//					arg.prepend(materials_To_Plot_Vector_Vector[material_index].first().key);
//					val_Multiple[material_index].prepend(materials_To_Plot_Vector_Vector[material_index].first().value);

					// add substrate
					QCPGraphData substrate;
						substrate.key = materials_To_Plot_Vector_Vector[material_index].last().key+discrete_Step_Vector.back()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
						substrate.value = struct_Data_Vector.last().relative_Density.value;
					materials_To_Plot_Vector_Vector[material_index].append(substrate);
					arg.append(materials_To_Plot_Vector_Vector[material_index].last().key);
					val_Multiple[material_index].append(materials_To_Plot_Vector_Vector[material_index].last().value);

					custom_Plot->graph()->setLineStyle(QCPGraph::lsStepLeft);
				} else
				{
					val_Multiple[material_index].resize(data_Count);
					arg.resize(data_Count);
					materials_To_Plot_Vector_Vector[material_index].resize(data_Count);

					Global_Variables::parallel_For(data_Count, reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
					{
						for(int i=n_Min; i<n_Max; ++i)
						{
							double z = -prefix + i*step;
							materials_To_Plot_Vector_Vector[material_index][i].key = z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
							materials_To_Plot_Vector_Vector[material_index][i].value = real(delta_Beta_Epsilon_Func(z, thread_Index, different_Materials[material_index]));
						}
					});
					for(int i=0; i<data_Count; i++)
					{
						arg[i] = materials_To_Plot_Vector_Vector[material_index][i].key; // many times, but OK
						val_Multiple[material_index][i] = materials_To_Plot_Vector_Vector[material_index][i].value;
					}
				}
				custom_Plot->graph()->data()->set(materials_To_Plot_Vector_Vector[material_index]);
				custom_Plot->graph()->setPen(QPen(color_Contrast_Sequence[material_index%color_Contrast_Sequence.size()], default_Profile_Line_Thickness));
				custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
				custom_Plot->graph()->setName(different_Materials[material_index]);
				custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setTextColor(color_Contrast_Sequence[material_index%color_Contrast_Sequence.size()]);
				custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setSelectedTextColor(color_Contrast_Sequence[material_index%color_Contrast_Sequence.size()]);
			}

			// connection between sharp and smooth plots
			if(multilayer->profile_Plot_Options.show_Sharp_Profile)
			{
				map_Sharp_Smooth.insert(custom_Plot->graph(), sharp_Graph);
			}
		}
		custom_Plot->legend->setVisible(true);
		custom_Plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight/*|Qt::AlignTop*/);
	}
	// elements
	if(multilayer->profile_Plot_Options.type == ELEMENTS)
	{
		elements_To_Plot_Vector_Vector      .resize(different_Elements.size());
		sharp_Elements_To_Plot_Vector_Vector.resize(different_Elements.size());

		val_Multiple      .resize(different_Elements.size());
		val_Sharp_Multiple.resize(different_Elements.size());

		custom_Plot->clearGraphs();
		for(int element_Index = 0; element_Index<different_Elements.size(); element_Index++)
		{
			QCPGraph* sharp_Graph;

			// sharp profile
			if(multilayer->profile_Plot_Options.show_Sharp_Profile)
			{
				sharp_Elements_To_Plot_Vector_Vector[element_Index].resize(struct_Data_Vector.size()+1);
				val_Sharp_Multiple[element_Index].resize(struct_Data_Vector.size());

				custom_Plot->addGraph();

				// first point
				{
					sharp_Elements_To_Plot_Vector_Vector[element_Index].first().key = -prefix/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					if(element_Concentration_Map_Vector.first().contains(different_Elements[element_Index]))
					{
						sharp_Elements_To_Plot_Vector_Vector[element_Index].first().value = element_Concentration_Map_Vector.first().value(different_Elements[element_Index]);
					} else
					{
						sharp_Elements_To_Plot_Vector_Vector[element_Index].first().value = 0;
					}
					val_Sharp_Multiple[element_Index].first() = sharp_Elements_To_Plot_Vector_Vector[element_Index].first().value;
				}
				// main part
				for(int media_Index=1; media_Index<struct_Data_Vector.size()-1; media_Index++)
				{
					sharp_Elements_To_Plot_Vector_Vector[element_Index][media_Index].key = boundary_Vector[media_Index-1]/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					if(element_Concentration_Map_Vector[media_Index].contains(different_Elements[element_Index]))
					{
						sharp_Elements_To_Plot_Vector_Vector[element_Index][media_Index].value = element_Concentration_Map_Vector[media_Index].value(different_Elements[element_Index]);
					} else
					{
						sharp_Elements_To_Plot_Vector_Vector[element_Index][media_Index].value = 0;
					}
					val_Sharp_Multiple[element_Index][media_Index] = sharp_Elements_To_Plot_Vector_Vector[element_Index][media_Index].value;
				}
				// last point
				{
					sharp_Elements_To_Plot_Vector_Vector[element_Index][sharp_Elements_To_Plot_Vector_Vector[element_Index].size()-2].key = boundary_Vector.last()/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					if(element_Concentration_Map_Vector.last().contains(different_Elements[element_Index]))
					{
						sharp_Elements_To_Plot_Vector_Vector[element_Index][sharp_Elements_To_Plot_Vector_Vector[element_Index].size()-2].value = element_Concentration_Map_Vector.last().value(different_Elements[element_Index]);
					} else
					{
						sharp_Elements_To_Plot_Vector_Vector[element_Index][sharp_Elements_To_Plot_Vector_Vector[element_Index].size()-2].value = 0;
					}
					val_Sharp_Multiple[element_Index].last() = sharp_Elements_To_Plot_Vector_Vector[element_Index][sharp_Elements_To_Plot_Vector_Vector[element_Index].size()-2].value;

					// additional point (for stepplot)
					sharp_Elements_To_Plot_Vector_Vector[element_Index].last().key   = (boundary_Vector.last()+suffix)/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
					sharp_Elements_To_Plot_Vector_Vector[element_Index].last().value = sharp_Elements_To_Plot_Vector_Vector[element_Index][sharp_Elements_To_Plot_Vector_Vector[element_Index].size()-2].value;
				}

				custom_Plot->graph()->data()->set(sharp_Elements_To_Plot_Vector_Vector[element_Index]);
				custom_Plot->graph()->setPen(QPen(color_Contrast_Adjoint_Sequence[element_Index%color_Contrast_Adjoint_Sequence.size()], default_Profile_Line_Thickness));
				custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
				custom_Plot->graph()->setLineStyle(QCPGraph::lsStepLeft);
				custom_Plot->graph()->removeFromLegend();
				sharp_Graph = custom_Plot->graph();
			}

			// smooth/discrete profile
			{
				custom_Plot->addGraph();
				if(multilayer->discretization_Parameters.enable_Discretization && multilayer->profile_Plot_Options.show_Discretization)
				{
					val_Multiple[element_Index].resize(num_Slices);
					arg.resize(num_Slices);
					elements_To_Plot_Vector_Vector[element_Index].resize(num_Slices);

					double real_Z = -(num_Prefix_Slices-0.5)*discrete_Step_Vector.front();
					for(int i=0; i<num_Slices; ++i)
					{
						double visible_Z = real_Z-discrete_Step_Vector[i]/2.; // where we dispose point
						elements_To_Plot_Vector_Vector[element_Index][i].key = visible_Z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
						elements_To_Plot_Vector_Vector[element_Index][i].value = real(delta_Beta_Epsilon_Func(real_Z, 0, different_Elements[element_Index]));

						arg[i] = elements_To_Plot_Vector_Vector[element_Index][i].key;
						val_Multiple[element_Index][i] = elements_To_Plot_Vector_Vector[element_Index][i].value;

						if(i<(num_Slices-1)) {
							real_Z += (discrete_Step_Vector[i]+discrete_Step_Vector[i+1])/2.; // real z, where we calculate epsilon
						} else
						{
							real_Z += discrete_Step_Vector[i]; // real z, where we calculate epsilon
						}
					}

					// add ambient
//					QCPGraphData ambient;
//						ambient.key = elements_To_Plot_Vector_Vector[element_Index].first().key-discrete_Step_Vector.front();
//						ambient.value = element_Concentration_Map_Vector.first().value(different_Elements[element_Index]);
//					elements_To_Plot_Vector_Vector[element_Index].prepend(ambient);
//					arg.prepend(elements_To_Plot_Vector_Vector[element_Index].first().key);
//					val_Multiple[element_Index].prepend(elements_To_Plot_Vector_Vector[element_Index].first().value);

					// add substrate
					QCPGraphData substrate;
						substrate.key = elements_To_Plot_Vector_Vector[element_Index].last().key+discrete_Step_Vector.back();
						substrate.value = element_Concentration_Map_Vector.last().value(different_Elements[element_Index]);
					elements_To_Plot_Vector_Vector[element_Index].append(substrate);
					arg.append(elements_To_Plot_Vector_Vector[element_Index].last().key);
					val_Multiple[element_Index].append(elements_To_Plot_Vector_Vector[element_Index].last().value);

					custom_Plot->graph()->setLineStyle(QCPGraph::lsStepLeft);
				} else
				{
					val_Multiple[element_Index].resize(data_Count);
					arg.resize(data_Count);
					elements_To_Plot_Vector_Vector[element_Index].resize(data_Count);

					Global_Variables::parallel_For(data_Count, reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
					{
						for(int i=n_Min; i<n_Max; ++i)
						{
							double z = -prefix + i*step;
							elements_To_Plot_Vector_Vector[element_Index][i].key = z/length_Coefficients_Map.value(multilayer->profile_Plot_Options.local_length_units);
							elements_To_Plot_Vector_Vector[element_Index][i].value = real(delta_Beta_Epsilon_Func(z, thread_Index, different_Elements[element_Index]));
						}
					});
					for(int i=0; i<data_Count; i++)
					{
						arg[i] = elements_To_Plot_Vector_Vector[element_Index][i].key; // many times, but OK
						val_Multiple[element_Index][i] = elements_To_Plot_Vector_Vector[element_Index][i].value;
					}
				}
				custom_Plot->graph()->data()->set(elements_To_Plot_Vector_Vector[element_Index]);
				custom_Plot->graph()->setPen(QPen(color_Contrast_Sequence[element_Index%color_Contrast_Sequence.size()], default_Profile_Line_Thickness));
				custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
				custom_Plot->graph()->setName(different_Elements[element_Index]);
				custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setTextColor(color_Contrast_Sequence[element_Index%color_Contrast_Sequence.size()]);
				custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setSelectedTextColor(color_Contrast_Sequence[element_Index%color_Contrast_Sequence.size()]);
			}

			// connection between sharp and smooth plots
			if(multilayer->profile_Plot_Options.show_Sharp_Profile)
			{
				map_Sharp_Smooth.insert(custom_Plot->graph(), sharp_Graph);
			}
		}
		custom_Plot->legend->setVisible(true);
		custom_Plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight/*|Qt::AlignTop*/);
	}
}

complex<double> Profile_Plot::delta_Beta_Epsilon_Func(double z, int thread_Index, QString given_Material_or_Element)
{
	// where we are
	int sigma_Factor = 6;
	std::vector<double>::iterator it_low = std::lower_bound(boundary_Vector_Std_Threaded[thread_Index].begin(), boundary_Vector_Std_Threaded[thread_Index].end(), z-sigma_Factor*max_Sigma);
	std::vector<double>::iterator it_up  = std::upper_bound(boundary_Vector_Std_Threaded[thread_Index].begin(), boundary_Vector_Std_Threaded[thread_Index].end(), z+sigma_Factor*max_Sigma);

	int min_Boundary_Index = max(min(int(it_low-boundary_Vector_Std_Threaded[thread_Index].begin())-1, thickness_Vector.size()-1), 0);
	int max_Boundary_Index = min(    int(it_up -boundary_Vector_Std_Threaded[thread_Index].begin())  , thickness_Vector.size()-1);

	double delta_Epsilon = 0;
	double beta_Epsilon = 0;
	double geometry_Factor = 1;

	// permittivity
	if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
	{
		for(int j=min_Boundary_Index; j<=max_Boundary_Index; j++) // instead of old slow for(int j=0; j<thickness_Vector.size(); j++)
		{
			geometry_Factor = layer_Norm_Vector_Threaded[thread_Index][j] *
					Global_Variables::interface_Profile_Function(z-boundary_Vector_Std_Threaded[thread_Index][j  ], struct_Data_Vector_Threaded[thread_Index][j+1].interlayer_Composition) *
					Global_Variables::interface_Profile_Function(boundary_Vector_Std_Threaded[thread_Index][j+1]-z, struct_Data_Vector_Threaded[thread_Index][j+2].interlayer_Composition);

			delta_Epsilon += delta_Epsilon_Vector_Threaded[thread_Index][j+1] * geometry_Factor;
			beta_Epsilon  += beta_Epsilon_Vector_Threaded [thread_Index][j+1] * geometry_Factor;
		}
		if(max_Boundary_Index>=thickness_Vector.size()-1)
		{
			geometry_Factor = Global_Variables::interface_Profile_Function(z-boundary_Vector_Std_Threaded[thread_Index].back(), struct_Data_Vector_Threaded[thread_Index].back().interlayer_Composition);

			delta_Epsilon += delta_Epsilon_Vector_Threaded[thread_Index].back() * geometry_Factor;
			beta_Epsilon  += beta_Epsilon_Vector_Threaded [thread_Index].back() * geometry_Factor;
		}
	}
	// material
	if(multilayer->profile_Plot_Options.type == MATERIAL)
	{
		for(int j=min_Boundary_Index; j<=max_Boundary_Index; j++) // instead of old slow for(int j=0; j<thickness_Vector.size(); j++)
		{
			Data& struct_Data = struct_Data_Vector_Threaded[thread_Index][j+1];
			if(!struct_Data.composed_Material && struct_Data.approved_Material == given_Material_or_Element)
			{
				geometry_Factor = layer_Norm_Vector_Threaded[thread_Index][j] *
						Global_Variables::interface_Profile_Function(z-boundary_Vector_Std_Threaded[thread_Index][j  ], struct_Data_Vector_Threaded[thread_Index][j+1].interlayer_Composition) *
						Global_Variables::interface_Profile_Function(boundary_Vector_Std_Threaded[thread_Index][j+1]-z, struct_Data_Vector_Threaded[thread_Index][j+2].interlayer_Composition);

				delta_Epsilon += struct_Data_Vector_Threaded[thread_Index][j+1].relative_Density.value * geometry_Factor;
			}
		}
		if(max_Boundary_Index>=thickness_Vector.size()-1)
		{
			if(!struct_Data_Vector_Threaded[thread_Index].back().composed_Material && struct_Data_Vector_Threaded[thread_Index].back().approved_Material == given_Material_or_Element)
			{
				geometry_Factor = Global_Variables::interface_Profile_Function(z-boundary_Vector_Std_Threaded[thread_Index].back(), struct_Data_Vector_Threaded[thread_Index].back().interlayer_Composition);

				delta_Epsilon += struct_Data_Vector_Threaded[thread_Index].back().relative_Density.value * geometry_Factor;
			}
		}
	}
	// element
	if(multilayer->profile_Plot_Options.type == ELEMENTS)
	{
		for(int j=min_Boundary_Index; j<=max_Boundary_Index; j++) // instead of old slow for(int j=0; j<thickness_Vector.size(); j++)
		{
			Data& struct_Data = struct_Data_Vector_Threaded[thread_Index][j+1];
			double concentration = element_Concentration_Map_Vector_Threaded[thread_Index][j+1].value(given_Material_or_Element);

			if(struct_Data.composed_Material && concentration>DBL_EPSILON)
			{
				geometry_Factor = layer_Norm_Vector_Threaded[thread_Index][j] *
						Global_Variables::interface_Profile_Function(z-boundary_Vector_Std_Threaded[thread_Index][j  ], struct_Data_Vector_Threaded[thread_Index][j+1].interlayer_Composition) *
						Global_Variables::interface_Profile_Function(boundary_Vector_Std_Threaded[thread_Index][j+1]-z, struct_Data_Vector_Threaded[thread_Index][j+2].interlayer_Composition);

				delta_Epsilon += concentration * geometry_Factor;
			}
		}
		if(max_Boundary_Index>=thickness_Vector.size()-1)
		{
			double concentration = element_Concentration_Map_Vector_Threaded[thread_Index].back().value(given_Material_or_Element);
			if(struct_Data_Vector_Threaded[thread_Index].back().composed_Material && concentration>DBL_EPSILON)
			{
				geometry_Factor = Global_Variables::interface_Profile_Function(z-boundary_Vector_Std_Threaded[thread_Index].back(), struct_Data_Vector_Threaded[thread_Index].back().interlayer_Composition);

				delta_Epsilon += concentration * geometry_Factor;
			}
		}
	}
	return complex<double>(delta_Epsilon,beta_Epsilon);
}

void Profile_Plot::hide_Show_Other_Plots()
{
	QCPGraph* graph = custom_Plot->selectedGraphs()[0];

	if(visibility_State)
	{
		// hide
		for(int i=0; i<custom_Plot->graphCount(); i++)
		{
			if(custom_Plot->graph(i) != graph)
			{
				custom_Plot->graph(i)->setVisible(false);
			}
			if(multilayer->profile_Plot_Options.show_Sharp_Profile)
			{
				QCPGraph* sharp_Graph = map_Sharp_Smooth.value(graph);
				if(sharp_Graph) {sharp_Graph->setVisible(true);	}

				QCPGraph* smooth_Graph = map_Sharp_Smooth.key(graph);
				if(smooth_Graph) {smooth_Graph->setVisible(true);	}
			}
		}
		visibility_State = false;
	} else
	{
		// show
		for(int i=0; i<custom_Plot->graphCount(); i++)
		{
			if(custom_Plot->graph(i) != graph) {custom_Plot->graph(i)->setVisible(true);}
		}
		visibility_State = true;
	}
	custom_Plot->replot();
}

void Profile_Plot::get_Delta_Epsilon(const Data& struct_Data, double& delta, double& beta)
{
	QVector<double> spectral_Points (1, multilayer->profile_Plot_Options.local_Wavelength);
	QVector<complex<double>> temp_Epsilon; temp_Epsilon.resize(1);
	QVector<complex<double>> n; n.resize(1);
	complex<double> delta_Epsilon;

	if(struct_Data.composed_Material)
	{
		optical_Constants->make_Epsilon_From_Factors(struct_Data.composition, struct_Data.absolute_Density.value, spectral_Points, temp_Epsilon);
		delta_Epsilon = 1. - temp_Epsilon.first();
	} else
	{
		Material_Data temp_Material_Data = optical_Constants->material_Map.value(struct_Data.approved_Material + nk_Ext);
		optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, struct_Data.approved_Material);
		delta_Epsilon = struct_Data.relative_Density.value * (1. - n.first()*n.first());
	}
	delta =  real(delta_Epsilon);
	beta  = -imag(delta_Epsilon);
}

void Profile_Plot::get_Material(const Data& struct_Data)
{
	if(!different_Materials.contains(struct_Data.approved_Material) && struct_Data.approved_Material!="Vacuum")
	{
		different_Materials.append(struct_Data.approved_Material);
	}
}

void Profile_Plot::get_Element_Map(const Data& struct_Data, QMap<QString, double>& element_Map)
{
	element_Map.clear();
	double denominator = 0;
	for(int comp_Index=0; comp_Index<struct_Data.composition.size(); ++comp_Index)
	{
		denominator += struct_Data.composition[comp_Index].composition.value * sorted_Elements.value(struct_Data.composition[comp_Index].type);
	}
	if(abs(denominator)<DBL_EPSILON) {denominator = DBL_EPSILON;}
	double compound_Concentration = Na * struct_Data.absolute_Density.value / denominator;

	for(int comp_Index=0; comp_Index<struct_Data.composition.size(); ++comp_Index)
	{
		QString element = struct_Data.composition[comp_Index].type;
		double element_Concentration = compound_Concentration * struct_Data.composition[comp_Index].composition.value;

		if(!element_Map.contains(element))
		{
			// if doesn't have such element in this layer
			element_Map.insert(element, element_Concentration);
		} else
		{
			// adds concentration to existing element
			element_Map.insert(element, element_Map.value(element)+element_Concentration);
		}

		if(!different_Elements.contains(element))
		{
			different_Elements.append(element);
		}
	}
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
				   struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
				{
					// thickness drift
					Global_Variables::variable_Drift(struct_Data.thickness.value, struct_Data.thickness_Drift, period_Index, num_Repetition, nullptr);

					// sigma drift
					for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)	{
						Global_Variables::variable_Drift(struct_Data.interlayer_Composition[func_Index].my_Sigma.value, struct_Data.sigma_Drift, period_Index, num_Repetition, nullptr);
					}
					Global_Variables::variable_Drift(struct_Data.sigma.value, struct_Data.sigma_Drift, period_Index, num_Repetition, nullptr);
				}
				struct_Data_Vector[struct_Data_Index] = struct_Data;
				const Data& my_Little_Data = struct_Data_Vector[struct_Data_Index];

				// discretization
				if(struct_Data.item_Type == item_Type_Layer &&
				   struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic &&
				   multilayer->discretization_Parameters.enable_Discretization && multilayer->profile_Plot_Options.show_Discretization)
				{
					int num_Slices_Local = ceil(struct_Data.thickness.value/multilayer->discretization_Parameters.discretization_Step);
					double adapted_Step = struct_Data.thickness.value/num_Slices_Local;

					discrete_Step_Vector.resize(discrete_Step_Vector.size()+num_Slices_Local);
					size_t last_Index = discrete_Step_Vector.size()-1;
					for(int i=0; i<num_Slices_Local; i++)
					{
						discrete_Step_Vector[last_Index-i] = adapted_Step;
					}
				}

				/// -----------------------------------------------------------------------------------------------------------
				// get target data
				/// -----------------------------------------------------------------------------------------------------------
				if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
				{
					get_Delta_Epsilon(my_Little_Data, delta_Epsilon_Vector[struct_Data_Index], beta_Epsilon_Vector[struct_Data_Index]);
				}
				if(multilayer->profile_Plot_Options.type == MATERIAL && !my_Little_Data.composed_Material)
				{
					get_Material(my_Little_Data);
				}
				if(multilayer->profile_Plot_Options.type == ELEMENTS && my_Little_Data.composed_Material)
				{
					get_Element_Map(my_Little_Data, element_Concentration_Map_Vector[struct_Data_Index]);
				}
				/// -----------------------------------------------------------------------------------------------------------

				struct_Data_Index++;
			}

			if(item->child(i)->childCount()>0)
			{
				if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
				{
					/// -----------------------------------------------------------------------------------------------------------
					// get target data for one unit cell
					/// -----------------------------------------------------------------------------------------------------------
					QVector<double> delta_Vector; delta_Vector.resize(struct_Data.regular_Components.size());
					QVector<double> beta_Vector;  beta_Vector .resize(struct_Data.regular_Components.size());
					QVector<QMap<QString,double>> concentration_Map_Vector;  concentration_Map_Vector.resize(struct_Data.regular_Components.size());

					for(int component_Index=0; component_Index<struct_Data.regular_Components.size(); component_Index++)
					{
						const Data& my_Little_Data = struct_Data.regular_Components[component_Index].components.first();
						if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
						{
							get_Delta_Epsilon(my_Little_Data, delta_Vector[component_Index], beta_Vector[component_Index]);
						}
						if(multilayer->profile_Plot_Options.type == MATERIAL && !my_Little_Data.composed_Material)
						{
							get_Material(my_Little_Data);
						}
						if(multilayer->profile_Plot_Options.type == ELEMENTS && my_Little_Data.composed_Material)
						{
							get_Element_Map(my_Little_Data, concentration_Map_Vector[component_Index]);
						}
					}
					/// -----------------------------------------------------------------------------------------------------------

					for(int layer_Index=0; layer_Index<struct_Data.regular_Components.first().components.size(); layer_Index++)
					{
						for(int component_Index=0; component_Index<struct_Data.regular_Components.size(); component_Index++)
						{
							struct_Data_Vector[struct_Data_Index] = struct_Data.regular_Components[component_Index].components[layer_Index];

							/// -----------------------------------------------------------------------------------------------------------
							// copy already obtained epsilon
							/// -----------------------------------------------------------------------------------------------------------
							if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
							{
								delta_Epsilon_Vector[struct_Data_Index] = delta_Vector[component_Index];
								beta_Epsilon_Vector [struct_Data_Index] = beta_Vector [component_Index];
							}
							if(multilayer->profile_Plot_Options.type == ELEMENTS && struct_Data_Vector[struct_Data_Index].composed_Material)
							{
								element_Concentration_Map_Vector[struct_Data_Index] = concentration_Map_Vector[component_Index];
							}
							/// -----------------------------------------------------------------------------------------------------------

							// discretization
							if(multilayer->discretization_Parameters.enable_Discretization && multilayer->profile_Plot_Options.show_Discretization)
							{
								int num_Slices_Local = ceil(struct_Data_Vector[struct_Data_Index].thickness.value/multilayer->discretization_Parameters.discretization_Step);
								double adapted_Step = struct_Data_Vector[struct_Data_Index].thickness.value/num_Slices_Local;

								discrete_Step_Vector.resize(discrete_Step_Vector.size()+num_Slices_Local);
								size_t last_Index = discrete_Step_Vector.size()-1;
								for(int i=0; i<num_Slices_Local; i++)
								{
									discrete_Step_Vector[last_Index-i] = adapted_Step;
								}
							}
							struct_Data_Index++;
						}
					}
				}

				if(struct_Data.item_Type == item_Type_Multilayer)
				{
					// prepare list
					bool all_Layers = true;
					QList<Data> child_Data_List;
					for(int child_Index=0; child_Index<item->child(i)->childCount(); child_Index++)
					{
						Data child_Data = item->child(i)->child(child_Index)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
						all_Layers = all_Layers && (child_Data.item_Type == item_Type_Layer);
						child_Data_List.append(child_Data);
					}

					if(all_Layers)
					{
						/// -----------------------------------------------------------------------------------------------------------
						// get target data for one period
						/// -----------------------------------------------------------------------------------------------------------
						QVector<double> delta_Vector; delta_Vector.resize(item->child(i)->childCount());
						QVector<double> beta_Vector;  beta_Vector .resize(item->child(i)->childCount());
						QVector<QMap<QString,double>> concentration_Map_Vector;  concentration_Map_Vector.resize(item->child(i)->childCount());

						for(int child_Index=0; child_Index<item->child(i)->childCount(); child_Index++)
						{
							const Data& my_Little_Data = child_Data_List[child_Index];
							if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
							{
								get_Delta_Epsilon(my_Little_Data, delta_Vector[child_Index], beta_Vector[child_Index]);
							}
							if(multilayer->profile_Plot_Options.type == MATERIAL && !my_Little_Data.composed_Material)
							{
								get_Material(my_Little_Data);
							}
							if(multilayer->profile_Plot_Options.type == ELEMENTS && my_Little_Data.composed_Material)
							{
								get_Element_Map(my_Little_Data, concentration_Map_Vector[child_Index]);
							}
						}
						/// -----------------------------------------------------------------------------------------------------------

						for(int period_Index=0; period_Index<struct_Data.num_Repetition.value(); period_Index++)
						{
							for(int child_Index=0; child_Index<item->child(i)->childCount(); child_Index++)
							{
								// here we know that all child_Data are layers
								struct_Data_Vector[struct_Data_Index] = child_Data_List[child_Index];
								Data& child_Data = struct_Data_Vector[struct_Data_Index];

								if(child_Data.item_Enabled)
								{
									/// -----------------------------------------------------------------------------------------------------------
									// copy already obtained epsilon
									/// -----------------------------------------------------------------------------------------------------------
									if(multilayer->profile_Plot_Options.type == PERMITTIVITY)
									{
										delta_Epsilon_Vector[struct_Data_Index] = delta_Vector[child_Index];
										beta_Epsilon_Vector [struct_Data_Index] = beta_Vector [child_Index];
									}
									if(multilayer->profile_Plot_Options.type == ELEMENTS && child_Data.composed_Material)
									{
										element_Concentration_Map_Vector[struct_Data_Index] = concentration_Map_Vector[child_Index];
									}
									/// -----------------------------------------------------------------------------------------------------------
									struct_Data_Index++;

									// thickness drift
									Global_Variables::variable_Drift(child_Data.thickness.value, child_Data.thickness_Drift, period_Index, struct_Data.num_Repetition.value(), nullptr);

									// sigma drift
									for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)	{
										Global_Variables::variable_Drift(child_Data.interlayer_Composition[func_Index].my_Sigma.value, child_Data.sigma_Drift, period_Index, struct_Data.num_Repetition.value(), nullptr);
									}
									Global_Variables::variable_Drift(child_Data.sigma.value, child_Data.sigma_Drift, period_Index, struct_Data.num_Repetition.value(), nullptr);

									// discretization
									if(multilayer->discretization_Parameters.enable_Discretization && multilayer->profile_Plot_Options.show_Discretization)
									{
										int num_Slices_Local = ceil(child_Data.thickness.value/multilayer->discretization_Parameters.discretization_Step);
										double adapted_Step = child_Data.thickness.value/num_Slices_Local;

										discrete_Step_Vector.resize(discrete_Step_Vector.size()+num_Slices_Local);
										size_t last_Index = discrete_Step_Vector.size()-1;
										for(int i=0; i<num_Slices_Local; i++)
										{
											discrete_Step_Vector[last_Index-i] = adapted_Step;
										}
									}
								}
							}
						}
					} else
					{
						for(int period_Index=0; period_Index<struct_Data.num_Repetition.value(); period_Index++)
						{
							unwrap_Subtree(struct_Data_Vector, item->child(i), struct_Data.num_Repetition.value(), period_Index);
						}
					}
				}

				if(struct_Data.item_Type == item_Type_General_Aperiodic)
				{
					unwrap_Subtree(struct_Data_Vector, item->child(i), 1, 0);
				}
			}
		}
	}
}

void Profile_Plot::get_Max_My_Sigma(QTreeWidgetItem* item, int periods_Factor)
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
				struct_Data_Counter+=1*periods_Factor;
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
							struct_Data_Counter++;
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
					get_Max_My_Sigma(item->child(i), periods_Factor*struct_Data.num_Repetition.value());
				}
			}
		}
	}
}
