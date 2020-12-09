#include "curve_plot_1d.h"

Curve_Plot_1D::Curve_Plot_1D(Multilayer* multilayer, Target_Curve* target_Curve, Independent_Curve* independent_Curve, QString curve_Class, QWidget* parent) :
	multilayer(multilayer),
	curve_Class(curve_Class),
	target_Curve(target_Curve),
	independent_Curve(independent_Curve),

	measurement			(curve_Class == INDEPENDENT ? independent_Curve->measurement	   : target_Curve->measurement				),
	calculated_Values   (curve_Class == INDEPENDENT ? independent_Curve->calculated_Values : target_Curve->calculated_Values		),
	plot_Options_First  (curve_Class == INDEPENDENT ? independent_Curve->plot_Options	   : target_Curve->plot_Options_Experimental),
	plot_Options_Second (curve_Class == INDEPENDENT ? independent_Curve->plot_Options	   : target_Curve->plot_Options_Calculated	),
	spectral_Units		(curve_Class == INDEPENDENT ? independent_Curve->spectral_Units	   : target_Curve->spectral_Units			),
	angular_Units		(curve_Class == INDEPENDENT ? independent_Curve->angular_Units	   : target_Curve->angular_Units			),
	plot_Indicator		(curve_Class == INDEPENDENT ? independent_Curve->tab_Name		   : target_Curve->index					),
	additional_Curves	(curve_Class == INDEPENDENT ? independent_Curve->additional_Curves : target_Curve->additional_Curves		),

	QWidget(parent)
{
	create_Main_Layout();
	setAcceptDrops(true);
}

void Curve_Plot_1D::dragEnterEvent(QDragEnterEvent* event)
{
	if(event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void Curve_Plot_1D::dropEvent(QDropEvent* event)
{
	int counter = 0;
	foreach (const QUrl &url, event->mimeData()->urls())
	{
		if(counter==0)
		{
			QString fileName = url.toLocalFile();

			Simple_Curve new_Simple_Curve;
			new_Simple_Curve.read_Simple_Curve(fileName);

			additional_Curves.push_back(new_Simple_Curve);

			create_Plot_Frame_And_Scale();
			plot_All_Data();
		}
		++counter;
	}
}

void Curve_Plot_1D::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(4,4,4,0);

	custom_Plot = new QCustomPlot;
	main_Layout->addWidget(custom_Plot);

	if(multilayer->graph_Options_1D.show_Title)
	{
		plot_Title = new QCPTextElement(custom_Plot,"text_Data",QFont("Times", 10, QFont::DemiBold));
		custom_Plot->plotLayout()->insertRow(0);
		custom_Plot->plotLayout()->setRowSpacing(0);
		custom_Plot->plotLayout()->addElement(0,0,plot_Title);
	}

	create_Plot_Frame_And_Scale();

	if(curve_Class == TARGET) create_Subinterval_Rectangle();

	// for discretized structures plot vertical threshold line
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		infLine = new QCPItemStraightLine(custom_Plot);
		discretized_Threshold_Line();
	}

	create_Options();

	// events on selection
	connect(custom_Plot, &QCustomPlot::plottableDoubleClick,   this, &Curve_Plot_1D::choose_Graph_Color);
	connect(custom_Plot, &QCustomPlot::selectionChangedByUser, this, [=]
	{
		show_Thickness();
		show_Scatter_Size();
	});

	graph_Done = true;

	// rescale on opening
	bool rescale = plot_Options_First.rescale;
	plot_Options_First.rescale = true;

	plot_All_Data();

	plot_Options_First.rescale = rescale;

}

void Curve_Plot_1D::create_Subinterval_Rectangle()
{
	start_Rect = new QCPItemRect(custom_Plot);
			start_Rect->setPen  (subinterval_Plot_Pen);
			start_Rect->setBrush(subinterval_Plot_Brush);
	end_Rect = new QCPItemRect(custom_Plot);
			end_Rect->setPen  (subinterval_Plot_Pen);
			end_Rect->setBrush(subinterval_Plot_Brush);

	subinterval_Changed_Replot();

	connect(custom_Plot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Curve_Plot_1D::subinterval_Changed_Replot);
	connect(custom_Plot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Curve_Plot_1D::subinterval_Changed_Replot);
}

void Curve_Plot_1D::subinterval_Changed_Replot()
{
	start_Rect->setVisible(target_Curve->curve.use_Subinterval);
	end_Rect->setVisible(target_Curve->curve.use_Subinterval && !target_Curve->curve.outer_Area);

	if(target_Curve->curve.outer_Area)
	{
		start_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Left, custom_Plot->yAxis->range().upper);
		start_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Right,custom_Plot->yAxis->range().lower);
	} else
	{
		start_Rect->topLeft->setCoords(custom_Plot->xAxis->range().lower, custom_Plot->yAxis->range().upper);
		start_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Left,custom_Plot->yAxis->range().lower);

		end_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Right,custom_Plot->yAxis->range().upper);
		end_Rect->bottomRight->setCoords(custom_Plot->xAxis->range().upper, custom_Plot->yAxis->range().lower);
	}
	custom_Plot->replot();
}

void Curve_Plot_1D::discretized_Threshold_Line()
{
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		double safety_Factor = 0.3;

		if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
		{
			if(multilayer->discretization_Parameters.enable_Discretization)
			{
				double asin_Argument = measurement.wavelength.value/(2*multilayer->discretization_Parameters.discretization_Step);
				if(abs(asin_Argument)<1)
				{
					double angle = safety_Factor*qRadiansToDegrees(asin(asin_Argument));
					double angle_In_Plot_Units = angle/angle_Coefficients_Map.value(angular_Units);

					infLine->point1->setCoords(angle_In_Plot_Units, 0);
					infLine->point2->setCoords(angle_In_Plot_Units, 1);
				}
				else
				{
					infLine->point1->setCoords(-MAX_DOUBLE, 0);
					infLine->point2->setCoords(-MAX_DOUBLE, 1);
				}
			} else
			{
				infLine->point1->setCoords(-MAX_DOUBLE, 0);
				infLine->point2->setCoords(-MAX_DOUBLE, 1);
			}
		}
		if(measurement.argument_Type == argument_Types[Wavelength_Energy])
		{
			if(multilayer->discretization_Parameters.enable_Discretization)
			{
				double lambda_Argument = 2*multilayer->discretization_Parameters.discretization_Step*sin(qDegreesToRadians(measurement.beam_Theta_0_Angle.value))/safety_Factor;
				double wavelength_In_Plot_Units = Global_Variables::wavelength_Energy(spectral_Units, lambda_Argument)/ wavelength_Coefficients_Map.value(spectral_Units);

				infLine->point1->setCoords(wavelength_In_Plot_Units, 0);
				infLine->point2->setCoords(wavelength_In_Plot_Units, 1);
			}
			else
			{
				infLine->point1->setCoords(-MAX_DOUBLE, 0);
				infLine->point2->setCoords(-MAX_DOUBLE, 1);
			}
		}
		custom_Plot->replot();
	}
}

void Curve_Plot_1D::create_Plot_Frame_And_Scale()
{
	custom_Plot->setNoAntialiasingOnDrag(false);
	custom_Plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom);

	// frame
	custom_Plot->axisRect()->setupFullAxesBox(true);
	custom_Plot->yAxis2->setTickLabels(true);
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		linTicker->setTickCount(12);
	custom_Plot->xAxis ->setTicker(linTicker);
	custom_Plot->xAxis2->setTicker(linTicker);

	// set grid style
	QPen pen = custom_Plot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
	custom_Plot->yAxis->grid()->setSubGridVisible(true);
	custom_Plot->xAxis->grid()->setSubGridVisible(true);
	custom_Plot->yAxis->grid()->setPen(pen);
	custom_Plot->xAxis->grid()->setPen(pen);

	/// X axis
	if(plot_Options_First.x_Scale == lin_Scale)	apply_Lin_Scale("x");
	if(plot_Options_First.x_Scale == log_Scale) apply_Log_Scale("x");
	/// Y axis
	if(plot_Options_First.y_Scale == lin_Scale) apply_Lin_Scale("y");
	if(plot_Options_First.y_Scale == log_Scale) apply_Log_Scale("y");

	// create 2 graphs + additional
	if(custom_Plot->graphCount()!=2+additional_Curves.size())
	{
		custom_Plot->clearGraphs();
		custom_Plot->addGraph();
		custom_Plot->addGraph();

		graph_Options_Map.clear();
		graph_Options_Map.insert(custom_Plot->graph(0), &plot_Options_First);
		graph_Options_Map.insert(custom_Plot->graph(1), &plot_Options_Second);

		// additional curves
		for(size_t additional_Index=0; additional_Index<additional_Curves.size(); additional_Index++)
		{
			size_t absolute_Graph_Index = 2+additional_Index;
			custom_Plot->addGraph();

			// styling
			if(additional_Curves[additional_Index].plot_Options.color == QColor(0, 0, 0, 254))
			{
				if(additional_Index<color_Contrast_Adjoint_Sequence.size()) additional_Curves[additional_Index].plot_Options.color = color_Contrast_Adjoint_Sequence[additional_Index];
				else														additional_Curves[additional_Index].plot_Options.color = QColor(0, 0, 0);
			}
			custom_Plot->graph(absolute_Graph_Index)->setPen(QPen(additional_Curves[additional_Index].plot_Options.color,additional_Curves[additional_Index].plot_Options.thickness));
			graph_Options_Map.insert(custom_Plot->graph(absolute_Graph_Index), &additional_Curves[additional_Index].plot_Options);

			// additional data plotting
			QVector<QCPGraphData> data_To_Plot(additional_Curves[additional_Index].argument.size());
			for (size_t i=0; i<additional_Curves[additional_Index].argument.size(); ++i)
			{
				data_To_Plot[i].key = additional_Curves[additional_Index].argument[i];
				data_To_Plot[i].value = additional_Curves[additional_Index].values[i];
			}
			custom_Plot->graph(absolute_Graph_Index)->data()->set(data_To_Plot);

			// on selection
			custom_Plot->graph(absolute_Graph_Index)->selectionDecorator()->setPen(QPen(
							custom_Plot->graph(absolute_Graph_Index)->pen().color(),
						max(custom_Plot->graph(absolute_Graph_Index)->pen().widthF()*2,3.)));
		}
	}
	custom_Plot->replot();
	set_Title_Text();
}

void Curve_Plot_1D::apply_Log_Scale(QString xy)
{
	if(xy == "x")
	{
		QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);

		custom_Plot->xAxis ->setScaleType(QCPAxis::stLogarithmic);
		custom_Plot->xAxis->setTicker(logTicker);
		custom_Plot->xAxis ->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
		custom_Plot->xAxis ->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

		custom_Plot->xAxis2->setScaleType(QCPAxis::stLogarithmic);
		custom_Plot->xAxis2->setTicker(logTicker);
		custom_Plot->xAxis2->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
		custom_Plot->xAxis2->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
	}
	if(xy == "y")
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
}

void Curve_Plot_1D::apply_Lin_Scale(QString xy)
{
	if(xy == "x")
	{
		QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
			linTicker->setTickCount(12);

		custom_Plot->xAxis->setScaleType(QCPAxis::stLinear);
		custom_Plot->xAxis->setTicker(linTicker);
		custom_Plot->xAxis->setNumberFormat("g");
		custom_Plot->xAxis->setNumberPrecision(4);

		custom_Plot->xAxis2->setScaleType(QCPAxis::stLinear);
		custom_Plot->xAxis2->setTicker(linTicker);
		custom_Plot->xAxis2->setNumberFormat("g");
		custom_Plot->xAxis2->setNumberPrecision(4);
	}
	if(xy == "y")
	{
		QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
			linTicker->setTickCount(8);

		custom_Plot->yAxis->setScaleType(QCPAxis::stLinear);
		custom_Plot->yAxis->setTicker(linTicker);
		custom_Plot->yAxis->setNumberFormat("g");
		custom_Plot->yAxis->setNumberPrecision(4);

		custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
		custom_Plot->yAxis2->setTicker(linTicker);
		custom_Plot->yAxis2->setNumberFormat("g");
		custom_Plot->yAxis2->setNumberPrecision(4);
	}
}

void Curve_Plot_1D::create_Options()
{
	QGroupBox* options_GroupBox = new QGroupBox;
	main_Layout->addWidget(options_GroupBox);

	QHBoxLayout* options_Layout = new QHBoxLayout(options_GroupBox);
		options_Layout->setContentsMargins(5,1,5,1);
		options_Layout->setAlignment(Qt::AlignLeft);

	//  scale radiobuttons
	{
		QLabel* scale_Label = new QLabel("Scale Y: ");
		options_Layout->addWidget(scale_Label);

		// -------------------------------------------------

		QRadioButton* lin_Y_RadioButton = new QRadioButton("Lin");
			lin_Y_RadioButton->setChecked(plot_Options_First.y_Scale == lin_Scale);
		options_Layout->addWidget(lin_Y_RadioButton);
		connect(lin_Y_RadioButton, &QRadioButton::toggled, this, [=]
		{
			if(lin_Y_RadioButton->isChecked())
			{
				plot_Options_First. y_Scale = lin_Scale;
				plot_Options_Second.y_Scale = lin_Scale;
			}
			apply_Lin_Scale("y");
			plot_All_Data();
			custom_Plot->replot();
		});
		connect(lin_Y_RadioButton, &QRadioButton::clicked, lin_Y_RadioButton, &QRadioButton::toggled);

		// -------------------------------------------------

		QRadioButton* log_Y_RadioButton = new QRadioButton("Log");
			log_Y_RadioButton->setChecked(plot_Options_First.y_Scale == log_Scale);
		options_Layout->addWidget(log_Y_RadioButton);
		connect(log_Y_RadioButton, &QRadioButton::toggled, this, [=]
		{
			if(log_Y_RadioButton->isChecked())
			{
				plot_Options_First. y_Scale = log_Scale;
				plot_Options_Second.y_Scale = log_Scale;
			}
			apply_Log_Scale("y");
			plot_All_Data();
			custom_Plot->replot();
		});
		connect(log_Y_RadioButton, &QRadioButton::clicked, log_Y_RadioButton, &QRadioButton::toggled);

		QButtonGroup* Y_ButtonGroup = new QButtonGroup;
			Y_ButtonGroup->addButton(lin_Y_RadioButton);
			Y_ButtonGroup->addButton(log_Y_RadioButton);
	}
	//  rescale
	{
		QCheckBox* rescale_Check_Box = new QCheckBox("Rescale");
			rescale_Check_Box->setChecked(plot_Options_First.rescale);
		connect(rescale_Check_Box, &QCheckBox::toggled, this, [=]{ plot_Options_First.rescale = rescale_Check_Box->isChecked(); });
		options_Layout->addWidget(rescale_Check_Box);
	}
	if(multilayer->graph_Options_1D.show_X_Scale)
	{
		QLabel* scale_X_Label = new QLabel("Scale X: ");
		options_Layout->addWidget(scale_X_Label);

		// -------------------------------------------------

		QRadioButton* lin_X_RadioButton = new QRadioButton("Lin");
			lin_X_RadioButton->setChecked(plot_Options_First.x_Scale == lin_Scale);
		options_Layout->addWidget(lin_X_RadioButton);
		connect(lin_X_RadioButton, &QRadioButton::toggled, this, [=]
		{
			if(lin_X_RadioButton->isChecked())
			{
				plot_Options_First. x_Scale = lin_Scale;
				plot_Options_Second.x_Scale = lin_Scale;
			}
			apply_Lin_Scale("x");
			plot_All_Data();
			custom_Plot->replot();
		});
		connect(lin_X_RadioButton, &QRadioButton::clicked, lin_X_RadioButton, &QRadioButton::toggled);

		// -------------------------------------------------

		QRadioButton* log_X_RadioButton = new QRadioButton("Log");
			log_X_RadioButton->setChecked(plot_Options_First.x_Scale == log_Scale);
		options_Layout->addWidget(log_X_RadioButton);
		connect(log_X_RadioButton, &QRadioButton::toggled, this, [=]
		{
			if(log_X_RadioButton->isChecked())
			{
				plot_Options_First. x_Scale = log_Scale;
				plot_Options_Second.x_Scale = log_Scale;
			}
			apply_Log_Scale("x");
			plot_All_Data();
			custom_Plot->replot();
		});
		connect(log_X_RadioButton, &QRadioButton::clicked, log_X_RadioButton, &QRadioButton::toggled);

		// -------------------------------------------------

		QButtonGroup* X_ButtonGroup = new QButtonGroup;
			X_ButtonGroup->addButton(lin_X_RadioButton);
			X_ButtonGroup->addButton(log_X_RadioButton);
	}
	if(multilayer->graph_Options_1D.show_Scatter)
	{
		QLabel* scatter_Label = new QLabel("| Scatter:");
		options_Layout->addWidget(scatter_Label);

		scatter_Spin = new QDoubleSpinBox;
			scatter_Spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
			scatter_Spin->setAccelerated(true);
			scatter_Spin->setRange(0, MAX_DOUBLE);
			scatter_Spin->setDecimals(1);
			scatter_Spin->setValue(0);
			scatter_Spin->setSingleStep(0.1);
			scatter_Spin->setFixedWidth(35);
		connect(scatter_Spin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Curve_Plot_1D::change_Scatter_Size);
		options_Layout->addWidget(scatter_Spin);
	}
	if(multilayer->graph_Options_1D.show_Thickness)
	{
		QLabel* thickness_Label = new QLabel("Line:");
		options_Layout->addWidget(thickness_Label);

		thickness_Spin = new QDoubleSpinBox;
			thickness_Spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
			thickness_Spin->setAccelerated(true);
			thickness_Spin->setRange(0, MAX_DOUBLE);
			thickness_Spin->setDecimals(1);
			thickness_Spin->setValue(0);
			thickness_Spin->setSingleStep(0.1);
			thickness_Spin->setFixedWidth(35);
		connect(thickness_Spin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Curve_Plot_1D::change_Thickness);
		options_Layout->addWidget(thickness_Spin);
	}
	if(multilayer->graph_Options_1D.show_Current_Coordinate)
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
	// max value info
	if(multilayer->graph_Options_1D.show_Max_Value)
	{
		max_Value_Label = new QLabel;
		if(curve_Class == TARGET)
		{
			if(target_Curve->curve.value_Type == value_Types[Reflectance])	{ max_Value_Title = "|  Max R ="; } else
			if(target_Curve->curve.value_Type == value_Types[Transmittance]){ max_Value_Title = "|  Max T ="; } else
			if(target_Curve->curve.value_Type == value_Types[Scattering])	{ max_Value_Title = "|  Max S ="; } else
																			{ max_Value_Title = "";	}
		}
		if(curve_Class == INDEPENDENT)
		{
			if( independent_Curve->calc_Functions.check_Absorptance )		{ max_Value_Title = "|  Max A ="; } else
			if( independent_Curve->calc_Functions.check_Transmittance )		{ max_Value_Title = "|  Max T ="; } else
			if( independent_Curve->calc_Functions.check_Reflectance )		{ max_Value_Title = "|  Max R ="; } else
			if( independent_Curve->calc_Functions.check_Scattering )		{ max_Value_Title = "|  Max S ="; } else
																			{ max_Value_Title = ""; }
		}

		options_Layout->addWidget(max_Value_Label);
	}

	options_GroupBox->adjustSize();
	options_GroupBox->setFixedHeight(options_GroupBox->height());
}

void Curve_Plot_1D::set_Title_Text()
{
	if(multilayer->graph_Options_1D.show_Title)
	if(plot_Title)
	{
		QString lambda_Energy;
		if(	spectral_Units == wavelength_Units_List[angstrom] ||
			spectral_Units == wavelength_Units_List[nm]		  )
		{
			lambda_Energy = Lambda_Sym;
		} else
		{
			lambda_Energy = "E";
		}

		double spectral_Coeff = wavelength_Coefficients_Map.value(spectral_Units);
		double angular_Coeff = angle_Coefficients_Map.value(angular_Units);

		QString plot_Title_Text;
		if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				plot_Title_Text = plot_Indicator + ": " +
								  lambda_Energy + "=" + Locale.toString(Global_Variables::wavelength_Energy(spectral_Units,measurement.wavelength.value)/spectral_Coeff, line_edit_double_format, line_edit_wavelength_precision) + " " + spectral_Units +
								  ", pol=" + QString::number(measurement.polarization,'f',3);
			}
			if(measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
				plot_Title_Text = plot_Indicator + ": " +
								  Theta_Sym + Zero_Subscript_Sym + "=" + Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, line_edit_double_format, line_edit_angle_precision) + " " + angular_Units +
								  ", pol=" + QString::number(measurement.polarization,'f',3);
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Detector_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Detector_Polar_Angle])
			{
				plot_Title_Text = plot_Indicator + ": " +
								  Theta_Sym + Zero_Subscript_Sym + "=" + Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, line_edit_double_format, line_edit_angle_precision) + " " + angular_Units +
								  ", " + lambda_Energy + "=" + Locale.toString(Global_Variables::wavelength_Energy(spectral_Units,measurement.wavelength.value)/spectral_Coeff, line_edit_double_format, line_edit_wavelength_precision) + " " + spectral_Units +
								  ", pol=" + QString::number(measurement.polarization,'f',3);
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Rocking_Curve] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				plot_Title_Text = plot_Indicator + ": " +
								  "specular " + Theta_Sym + Zero_Subscript_Sym + "=" + Locale.toString(measurement.beam_Theta_0_Specular_Position/angular_Coeff, line_edit_double_format, line_edit_angle_precision) + " " + angular_Units +
								  ", " + lambda_Energy + "=" + Locale.toString(Global_Variables::wavelength_Energy(spectral_Units,measurement.wavelength.value)/spectral_Coeff, line_edit_double_format, line_edit_wavelength_precision) + " " + spectral_Units +
								  ", pol=" + QString::number(measurement.polarization,'f',3);
			}
			if(measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle])
			{
				plot_Title_Text = plot_Indicator + ": " +
								  "specular " + Theta_Sym + Zero_Subscript_Sym + "=" + Locale.toString(measurement.beam_Theta_0_Specular_Position/angular_Coeff, line_edit_double_format, line_edit_angle_precision) + " " + angular_Units +
								  ", " + lambda_Energy + "=" + Locale.toString(Global_Variables::wavelength_Energy(spectral_Units,measurement.wavelength.value)/spectral_Coeff, line_edit_double_format, line_edit_wavelength_precision) + " " + spectral_Units +
								  ", pol=" + QString::number(measurement.polarization,'f',3);
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Offset_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				plot_Title_Text = plot_Indicator + ": " +
								  "offset " + Delta_Big_Sym + Theta_Sym + "=" + Locale.toString(measurement.detector_Theta_Offset/angular_Coeff, line_edit_double_format, line_edit_angle_precision) + " " + angular_Units +
								  ", " + lambda_Energy + "=" + Locale.toString(Global_Variables::wavelength_Energy(spectral_Units,measurement.wavelength.value)/spectral_Coeff, line_edit_double_format, line_edit_wavelength_precision) + " " + spectral_Units +
								  ", pol=" + QString::number(measurement.polarization,'f',3);
			}
		}
		plot_Title->setText(plot_Title_Text);
		custom_Plot->replot();
	}
}

void Curve_Plot_1D::plot_All_Data()
{	
	refresh_Labels();

	min_Value_Left = DBL_MAX;
	max_Value_Left = -DBL_MAX;

	if(curve_Class == TARGET)
	{
		argument = target_Curve->curve.shifted_Argument;
		/// experimental data
		{
			values = target_Curve->curve.shifted_Values;

			// if changing val_Factor
			if(target_Curve->fit_Params.adjust_Scale_Factor)
			{
				for(size_t i=0; i<target_Curve->curve.shifted_Values.size(); ++i)
				{
					values[i] = target_Curve->curve.shifted_Values_No_Scaling_And_Offset[i]*
								target_Curve->curve.val_Factor.value +
								target_Curve->curve.val_Shift;
				}
			}
			plot_Data(argument, values, plot_Options_First, 0);
			get_Min_Max_For_Graph(plot_Options_First, values, min_Value_Left, max_Value_Left);
		}
		/// calculated data
		{
			if(	target_Curve->curve.value_Type == value_Types[Reflectance])		{ values = calculated_Values.R_Instrumental; }
			if(	target_Curve->curve.value_Type == value_Types[Transmittance])	{ values = calculated_Values.T_Instrumental; }
			if(	target_Curve->curve.value_Type == value_Types[Scattering])		{ values = calculated_Values.S_Instrumental; }

			// TODO
			if(values.size() == 0)
			{
				qInfo() << "Target curve " << plot_Indicator << " : calculation of " << target_Curve->curve.value_Type << "is not done. Fake data are shown." << endl;
				values = target_Curve->curve.shifted_Values;
				for(size_t i=0; i<values.size(); i++) {values[i] = 0;}
			}
			plot_Data(argument, values, plot_Options_Second, 1);
			get_Min_Max_For_Graph(plot_Options_Second, values, min_Value_Left, max_Value_Left);
		}
	}

	if(curve_Class == INDEPENDENT)
	{
		// value
		if(	independent_Curve->calc_Functions.check_Reflectance )	{ values = calculated_Values.R_Instrumental; }
		if(	independent_Curve->calc_Functions.check_Transmittance )	{ values = calculated_Values.T_Instrumental; }
		if(	independent_Curve->calc_Functions.check_Absorptance )	{ values = calculated_Values.A; }
		if( independent_Curve->calc_Functions.check_Scattering )	{ values = calculated_Values.S_Instrumental; }

		// argument
		double coeff_Angular = angle_Coefficients_Map.value(angular_Units);
		double coeff_Spectral = wavelength_Coefficients_Map.value(spectral_Units);


		if(	measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
		{
			argument.resize(measurement.beam_Theta_0_Angle_Vec.size());
			for(size_t i=0; i<measurement.beam_Theta_0_Angle_Vec.size(); i++)
			{
				argument[i] = measurement.beam_Theta_0_Angle_Vec[i]/coeff_Angular;
			}
		}
		if(	measurement.argument_Type == argument_Types[Wavelength_Energy])
		{
			argument.resize(measurement.lambda_Vec.size());
			for(size_t i=0; i<measurement.lambda_Vec.size(); i++)
			{
				argument[i] = Global_Variables::wavelength_Energy(spectral_Units, measurement.lambda_Vec[i])/coeff_Spectral;
			}
		}
		if(	measurement.argument_Type == argument_Types[Detector_Polar_Angle])
		{
			argument.resize(measurement.detector_Theta_Angle_Vec.size());
			for(size_t i=0; i<measurement.detector_Theta_Angle_Vec.size(); i++)
			{
				argument[i] = measurement.detector_Theta_Angle_Vec[i]/coeff_Angular;
			}
		}
		if(	measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle])
		{
			argument.resize(measurement.beam_Theta_0_Angle_Vec.size());
			for(size_t i=0; i<measurement.beam_Theta_0_Angle_Vec.size(); i++)
			{
				argument[i] = (measurement.beam_Theta_0_Angle_Vec[i]-measurement.beam_Theta_0_Specular_Position)/coeff_Angular;
			}
		}

		// TODO
		if(values.size() == 0 )
		{
			qInfo() << "Independent curve " << plot_Indicator << " : calculation is not done. Fake data are shown." << endl;
			if(argument.size() == 0)
			{
				argument.resize(1000);
				for(size_t i=0; i<values.size(); i++)
				{
					argument[i] = i/100.;
				}
			}
			values.resize(argument.size());
			for(size_t i=0; i<values.size(); i++)
			{
				values[i] = 0;//sin(argument[i])*sin(argument[i])+DBL_EPSILON;
			}
		}
		plot_Data(argument, values, plot_Options_First, 0);
		get_Min_Max_For_Graph(plot_Options_First, values, min_Value_Left, max_Value_Left);
	}

	if(plot_Options_First.rescale)
	{
		custom_Plot->yAxis->setRange(min_Value_Left,max_Value_Left);
		custom_Plot->xAxis->setRange(argument.front(), argument.back());
	}

	// show max value
	if(multilayer->graph_Options_1D.show_Max_Value)
	if(graph_Done && multilayer->graph_Options_1D.show_Max_Value)
	{
		if(argument.size()>3)
		{
			double max_Value, max_Value_Position, width;
			Global_Variables::get_Peak_Parameters(argument, values, max_Value_Position, max_Value, width);

			QString argument_Units;
			if(	measurement.argument_Type == argument_Types[Beam_Grazing_Angle])			argument_Units = angular_Units;
			if(	measurement.argument_Type == argument_Types[Wavelength_Energy])				argument_Units = spectral_Units;
			if(	measurement.argument_Type == argument_Types[Detector_Polar_Angle])			argument_Units = angular_Units;
			if(	measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle])	argument_Units = angular_Units;

			max_Value_Label->setText(max_Value_Title + " " + Locale.toString(max_Value,'f',4) + " at " + Locale.toString(max_Value_Position,'f',4) + " " + argument_Units);
			if(measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
				max_Value_Label->setText(max_Value_Label->text() + ", FWHM " + Locale.toString(width,'f',4) + " " + argument_Units);
			}
		} else
		{
			max_Value_Label->clear();
		}
	}
	custom_Plot->replot();
}

void Curve_Plot_1D::plot_Data(const vector<double>& argument, const vector<double>& values, Plot_Options plot_Options, int graph_Index)
{
	QVector<QCPGraphData> data_To_Plot(argument.size());

	for (size_t i=0; i<argument.size(); ++i)
	{
		data_To_Plot[i].key = argument[i];
		data_To_Plot[i].value = values[i];
	}
	custom_Plot->graph(graph_Index)->data()->set(data_To_Plot);

	// styling
	QCPScatterStyle scatter_Style;
	custom_Plot->graph(graph_Index)->setPen(QPen(plot_Options.color, plot_Options.thickness));
	scatter_Style.setShape(QCPScatterStyle::ScatterShape(plot_Options.scatter_Shape));
	scatter_Style.setSize(plot_Options.scatter_Size);

	custom_Plot->graph(graph_Index)->setScatterStyle(scatter_Style);

	// on selection
	custom_Plot->graph(graph_Index)->selectionDecorator()->setPen(QPen(
					custom_Plot->graph(graph_Index)->pen().color(),
				max(custom_Plot->graph(graph_Index)->pen().widthF()*2,3.)));
}

void Curve_Plot_1D::refresh_Labels()
{
	// value
	QString value_Label;
	if(curve_Class == TARGET)
	{
		value_Label = target_Curve->curve.value_Type;
	}
	if(curve_Class == INDEPENDENT)
	{
		if(	independent_Curve->calc_Functions.check_Reflectance )	value_Label = value_Types[Reflectance];
		if( independent_Curve->calc_Functions.check_Transmittance)	value_Label = value_Types[Transmittance];
		if( independent_Curve->calc_Functions.check_Absorptance )	value_Label = value_Types[Absorptance];
		if( independent_Curve->calc_Functions.check_Scattering )	value_Label = value_Types[Scattering];
	}
	custom_Plot->yAxis->setLabel(value_Label);

	// argument
	QString argument_Label;
	if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])				argument_Label = measurement.argument_Type + ", " + angular_Units;
	if(measurement.argument_Type == argument_Types[Wavelength_Energy])
		argument_Label = Global_Variables::wavelength_Energy_Name(spectral_Units) + " " + Global_Variables::wavelength_Energy_Symbol(spectral_Units) + ", " + spectral_Units;
	if(measurement.argument_Type == argument_Types[Detector_Polar_Angle])			argument_Label = measurement.argument_Type + ", " + angular_Units;
	if(measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle])	argument_Label = measurement.argument_Type + ", " + angular_Units;

	custom_Plot->xAxis->setLabel(argument_Label);
	custom_Plot->replot();
}

QCPGraph* Curve_Plot_1D::get_Selected_Graph()
{
	if(custom_Plot->selectedGraphs().size()==1)
	{
		return custom_Plot->selectedGraphs()[0];
	}
	return nullptr;
}

void Curve_Plot_1D::choose_Graph_Color()
{
	QCPGraph* graph = get_Selected_Graph();
	if(graph!=nullptr)
	{
		custom_Plot->deselectAll();

		QColorDialog* colorDialog = new QColorDialog(this);
			colorDialog->setModal(true);
			colorDialog->setOption(QColorDialog::ColorDialogOption::DontUseNativeDialog);
			colorDialog->show();

		QColor old_Color = graph->pen().color();
		connect(colorDialog, &QColorDialog::currentColorChanged, this, [=]{set_Graph_Color(graph, colorDialog->currentColor());});
		connect(colorDialog, &QColorDialog::rejected, this, [=]{set_Graph_Color(graph, old_Color);});
		connect(colorDialog, &QColorDialog::finished, this, [=]{
			disconnect(colorDialog, &QColorDialog::currentColorChanged, this, nullptr);
			disconnect(colorDialog, &QColorDialog::rejected, this, nullptr);
			disconnect(colorDialog, &QColorDialog::finished, this, nullptr);
		});
	}
}

void Curve_Plot_1D::set_Graph_Color(QCPGraph* graph, QColor color)
{
	graph->setPen(QPen(color, graph->pen().widthF()));
	graph->selectionDecorator()->setPen(QPen(color,graph->selectionDecorator()->pen().widthF()));

	// renew data in plot_Options
	Plot_Options* plot_Options = graph_Options_Map.value(graph);
	if(plot_Options != nullptr)
	{
		plot_Options->color = color;
	}
}

void Curve_Plot_1D::show_Thickness()
{
	if(multilayer->graph_Options_1D.show_Thickness)
	{
		QCPGraph* graph = get_Selected_Graph();
		if(graph!=nullptr)
		{
			thickness_Spin->setValue(graph->pen().widthF());
		} else
		{
			thickness_Spin->setValue(0);
		}
	}
}

void Curve_Plot_1D::show_Scatter_Size()
{
	if(multilayer->graph_Options_1D.show_Scatter)
	{
		QCPGraph* graph = get_Selected_Graph();
		if(graph!=nullptr)
		{
			scatter_Spin->setValue(graph->scatterStyle().size());
		} else
		{
			scatter_Spin->setValue(0);
		}
	}
}

void Curve_Plot_1D::change_Thickness()
{
	QCPGraph* graph = get_Selected_Graph();
	if(graph!=nullptr)
	{
		double new_Width = thickness_Spin->value();
		graph->setPen(QPen(graph->pen().color(), new_Width));

		graph->selectionDecorator()->setPen(QPen(graph->pen().color(),new_Width));
		custom_Plot->replot();
		graph->selectionDecorator()->setPen(QPen(graph->pen().color(),max(new_Width*2,3.)));

		// renew data in plot_Options
		Plot_Options* plot_Options = graph_Options_Map.value(graph);
		if(plot_Options != nullptr)
		{
			plot_Options->thickness = new_Width;
		}
	}
}

void Curve_Plot_1D::change_Scatter_Size()
{
	QCPGraph* graph = get_Selected_Graph();
	if(graph!=nullptr)
	{
		double new_Scatter_Size = scatter_Spin->value();
		graph->setScatterStyle(QCPScatterStyle(graph->scatterStyle().shape(),new_Scatter_Size));

		double line_Width = graph->pen().widthF();
		graph->selectionDecorator()->setPen(QPen(graph->pen().color(),line_Width));
		custom_Plot->replot();
		graph->selectionDecorator()->setPen(QPen(graph->pen().color(),max(line_Width*2,3.)));

		// renew data in plot_Options
		Plot_Options* plot_Options = graph_Options_Map.value(graph);
		if(plot_Options != nullptr)
		{
			plot_Options->scatter_Size = new_Scatter_Size;
		}
	}
}

void Curve_Plot_1D::get_Min_Max_For_Graph(Plot_Options plot_Options, const vector<double>& values, double& minimum, double& maximum)
{
	double local_Min = DBL_MAX;
	double local_Max = -DBL_MAX;

	for (size_t i=0; i<values.size(); ++i)
	{
		if(local_Max<values[i] && (plot_Options.y_Scale == lin_Scale || values[i] > DBL_MIN)) {local_Max=values[i];}
		if(local_Min>values[i] && (plot_Options.y_Scale == lin_Scale || values[i] > DBL_MIN)) {local_Min=values[i];}
	}

	minimum = min(minimum, local_Min);
	maximum = max(maximum, local_Max);
}
