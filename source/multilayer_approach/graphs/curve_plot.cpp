#include "curve_plot.h"

Curve_Plot::Curve_Plot(Target_Curve* target_Curve, Independent_Variables* independent_Variables, QString curve_Class, QWidget* parent) :
	curve_Class(curve_Class),
	target_Curve(target_Curve),
	independent_Variables(independent_Variables),
	QWidget(parent)
{
	// if target
	if(curve_Class == TARGET)
	{
		if(target_Curve) // if passed pointer!=nullptr
		{
			measurement = &target_Curve->measurement;
			calculated_Values = &target_Curve->calculated_Values;
			plot_Options_First = &target_Curve->plot_Options_Experimental;
			plot_Options_Second = &target_Curve->plot_Options_Calculated;
			spectral_Units = &target_Curve->curve.spectral_Units;
			angle_Type = &target_Curve->curve.angle_Type;
			angular_Units = &target_Curve->curve.angular_Units;
			argument_Type = &target_Curve->curve.argument_Type;
		} else
		{
			QMessageBox::critical(nullptr, "Curve_Plot::Curve_Plot", "target_Curve is nullptr");
			exit(EXIT_FAILURE);
		}
	}

	// if independent
	if(curve_Class == INDEPENDENT)
	{
		if(independent_Variables) // if passed pointer!=nullptr
		{
			measurement = &independent_Variables->measurement;
			calculated_Values = &independent_Variables->calculated_Values;
			plot_Options_First = &independent_Variables->plot_Options;
			plot_Options_Second = plot_Options_First;
			spectral_Units = &wavelength_units;
			angle_Type = &measurement->angle_Type;
			angular_Units = &angle_units;
			argument_Type = &independent_Variables->argument_Type;
		} else
		{
			QMessageBox::critical(nullptr, "Curve_Plot::Curve_Plot", "independent_Variables is nullptr");
			exit(EXIT_FAILURE);
		}
	}

	create_Main_Layout();
}

void Curve_Plot::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	custom_Plot = new QCustomPlot(this);
	create_Plot_Frame_And_Scale();
		main_Layout->addWidget(custom_Plot);

	create_Options();
		main_Layout->addWidget(options_GroupBox);

	// events on selection
	connect(custom_Plot, &QCustomPlot::plottableDoubleClick,   this, &Curve_Plot::choose_Graph_Color);
	connect(custom_Plot, &QCustomPlot::selectionChangedByUser, this, [=]{
			colorize_Color_Button();
			show_Thickness();
			show_Scatter_Size();
	});

	colorize_Color_Button();
	graph_Done = true;
}

void Curve_Plot::create_Plot_Frame_And_Scale()
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

		if(plot_Options_First->scale == lin_Scale)
		{
			QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);

			custom_Plot->yAxis->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis->setTicker(linTicker);
			custom_Plot->yAxis->setNumberFormat("g");
			custom_Plot->yAxis->setNumberPrecision(4);
//			custom_Plot->xAxis->setRange(0, 9);
//			custom_Plot->yAxis->setRange(0, 1);

			custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis2->setTicker(linTicker);
			custom_Plot->yAxis2->setNumberFormat("g");
			custom_Plot->yAxis2->setNumberPrecision(4);
//			custom_Plot->yAxis2->setRange(0, 1);
		}
		if(plot_Options_First->scale == log_Scale)
		{
			QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);

			custom_Plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
			custom_Plot->yAxis->setTicker(logTicker);
			custom_Plot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
			custom_Plot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
//			custom_Plot->xAxis->setRange(0, 9);
//			custom_Plot->yAxis->setRange(1e-5, 1e0);

//			if(	target_Curve->curve.value_Mode == value_R_Mode[R] ||
//				target_Curve->curve.value_Mode == value_T_Mode[T] ||
//				target_Curve->curve.value_Mode == value_A_Mode[A] )
			{
				custom_Plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
				custom_Plot->yAxis2->setTicker(logTicker);
				custom_Plot->yAxis2->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
				custom_Plot->yAxis2->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
//				custom_Plot->yAxis2->setRange(1e-5, 1e0);
			}
//			if(	target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )
//			{
//				QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
//				custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
//				custom_Plot->yAxis2->setTicker(linTicker);
//				custom_Plot->yAxis2->setNumberFormat("g");
//				custom_Plot->yAxis2->setNumberPrecision(4);
//				custom_Plot->yAxis2->setRange(0, 1);
//			}
		}

		// make range draggable and zoomable:
		custom_Plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend);

		// make top right axes clones of bottom left axes:
		custom_Plot->xAxis2->setVisible(true);
		custom_Plot->yAxis2->setVisible(true);
		custom_Plot->xAxis2->setTickLabels(false);

//		// connect signals so top and right axes move in sync with bottom and left axes:
//		if(	target_Curve->curve.value_Mode == value_R_Mode[R] ||
//			target_Curve->curve.value_Mode == value_T_Mode[T] ||
//			target_Curve->curve.value_Mode == value_A_Mode[A])
		{
			connect(custom_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->xAxis2, SLOT(setRange(QCPRange)));
			connect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));
		}
//		if(	target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )
//		{
//			connect   (custom_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->xAxis2, SLOT(setRange(QCPRange)));
//			disconnect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));
//		}
	}

//	custom_Plot->replot();
}

void Curve_Plot::create_Options()
{
	options_GroupBox = new QGroupBox;

	// scaling
	{
		scale_Label = new QLabel("Scale: ");

		lin_RadioButton = new QRadioButton("Lin");
		connect(lin_RadioButton, &QRadioButton::toggled, this, [=]
		{
			if(lin_RadioButton->isChecked())
			{
				plot_Options_First->scale = lin_Scale;
				plot_Options_Second->scale = lin_Scale;
			}
			plot_All_Data();
		});
		connect(lin_RadioButton, &QRadioButton::clicked, lin_RadioButton, &QRadioButton::toggled);
		if(plot_Options_First->scale == lin_Scale)
		{
			lin_RadioButton->setChecked(true);
		}

		log_RadioButton = new QRadioButton("Log");
		connect(log_RadioButton, &QRadioButton::toggled, this, [=]
		{
			if(log_RadioButton->isChecked())
			{
				plot_Options_First->scale = log_Scale;
				plot_Options_Second->scale = log_Scale;
			}
			plot_All_Data();
		});
		connect(log_RadioButton, &QRadioButton::clicked, log_RadioButton, &QRadioButton::toggled);
		if(plot_Options_First->scale == log_Scale)
		{
			log_RadioButton->setChecked(true);
		}
	}
	{
		colors_Button = new QPushButton;
		colors_Button->setFixedWidth(23);
		colorDialog = new QColorDialog(this);
		colorDialog->setModal(true);
		colorDialog->setOption(QColorDialog::ColorDialogOption::DontUseNativeDialog);
		default_Colors_Button_Palette = colors_Button->palette();
		connect(colors_Button, &QPushButton::clicked, this, &Curve_Plot::choose_Graph_Color);
	}
	{
		symbol_ComboBox = new QComboBox;
	}
	{
		scatter_Label = new QLabel("| Scatter:");

		scatter_Spin = new QDoubleSpinBox;
		scatter_Spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
		scatter_Spin->setAccelerated(true);
		scatter_Spin->setRange(0, MAX_DOUBLE);
		scatter_Spin->setDecimals(1);
		scatter_Spin->setValue(0);
		scatter_Spin->setSingleStep(0.1);
		scatter_Spin->setFixedWidth(35);
		connect(scatter_Spin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Curve_Plot::change_Scatter_Size);
	}
	{
		thickness_Label = new QLabel("Line:");

		thickness_Spin = new QDoubleSpinBox;
		thickness_Spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
		thickness_Spin->setAccelerated(true);
		thickness_Spin->setRange(0, MAX_DOUBLE);
		thickness_Spin->setDecimals(1);
		thickness_Spin->setValue(0);
		thickness_Spin->setSingleStep(0.1);
		thickness_Spin->setFixedWidth(35);
		connect(thickness_Spin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Curve_Plot::change_Thickness);
	}
	{
		rescale_Check_Box = new QCheckBox("Rescale");
		rescale_Check_Box->setChecked(plot_Options_First->rescale);
		connect(rescale_Check_Box, &QCheckBox::toggled, this, [=]{ plot_Options_First->rescale=rescale_Check_Box->isChecked(); });
	}
	// max value info
	{
		// target
		if(curve_Class == TARGET)
		{
			if(target_Curve->curve.value_Function == value_Function[Reflectance])
			{
				if(target_Curve->curve.value_Mode == value_R_Mode[R] || target_Curve->curve.value_Mode == value_R_Mode[R_Phi])
					max_Value_Title = "| Max R =";
			} else
			{
				if(target_Curve->curve.value_Function == value_Function[Transmittance])
				{
					if(target_Curve->curve.value_Mode == value_T_Mode[T])
						max_Value_Title = "| Max T =";
				} else
				{
					// none
					max_Value_Title = "";
				}
			}
		}
		// independent
		if(curve_Class == INDEPENDENT)
		{
			if( independent_Variables->calc_Functions.check_Reflectance )
			{
				max_Value_Title = "| Max R =";
			} else
			{
				if(!independent_Variables->calc_Functions.check_Reflectance &&
					independent_Variables->calc_Functions.check_Transmittance )
				{
					max_Value_Title = "| Max T =";
				} else
				{
					if(!independent_Variables->calc_Functions.check_Reflectance &&
					   !independent_Variables->calc_Functions.check_Transmittance &&
						independent_Variables->calc_Functions.check_Absorptance )
					{
						max_Value_Title = "| Max A =";
					} else
					{
						// none
						max_Value_Title = "";
					}
				}
			}
		}

		max_Value_Label = new QLabel;
	}

	// layout
	{
		options_Layout = new QHBoxLayout(options_GroupBox);
		options_Layout->setContentsMargins(5,1,5,1);
		options_Layout->setAlignment(Qt::AlignLeft);

		options_Layout->addWidget(scale_Label);
		options_Layout->addWidget(lin_RadioButton);
		options_Layout->addWidget(log_RadioButton);

//		options_Layout->addWidget(colors_Button);
//		options_Layout->addWidget(symbol_ComboBox);

		options_Layout->addWidget(scatter_Label);
		options_Layout->addWidget(scatter_Spin);

		options_Layout->addWidget(thickness_Label);
		options_Layout->addWidget(thickness_Spin);

		options_Layout->addWidget(rescale_Check_Box);

		options_Layout->addWidget(max_Value_Label);
	}

	options_GroupBox->adjustSize();
	options_GroupBox->setFixedHeight(options_GroupBox->height());
}

void Curve_Plot::plot_All_Data()
{	
	create_Plot_Frame_And_Scale();
	custom_Plot->clearGraphs();
	refresh_Labels();

	if(curve_Class == TARGET)
	{
		argument = target_Curve->curve.shifted_Argument;
		values.resize(target_Curve->curve.shifted_Values.size());

		/// experimental data
		// first value (R,T,A...)
		{
			for(int i=0; i<target_Curve->curve.shifted_Values.size(); ++i)	{
				values[i] = target_Curve->curve.shifted_Values[i].val_1;
			}
			plot_Data(argument, values, plot_Options_First, left_Axis);
		}
		// second value (phase)
		if(target_Curve->curve.value_Mode == value_R_Mode[R_Phi])
		{
			for(int i=0; i<target_Curve->curve.shifted_Values.size(); ++i)	{
				values[i] = target_Curve->curve.shifted_Values[i].val_2;
			}
			plot_Data(argument, values, plot_Options_First, right_Axis);
		}

		/// calculated data
		// second value (phase) // placed first for showing max value, not phase
		if(target_Curve->curve.value_Mode == value_R_Mode[R_Phi])
		{
			values = calculated_Values->Phi_R;
			plot_Data(argument, values, plot_Options_Second, right_Axis);
		}
		// first value (R,T,A...)
		{
			if(	target_Curve->curve.value_Mode == value_R_Mode[R] ||
				target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )	{	values = calculated_Values->R; }
			if(	target_Curve->curve.value_Mode == value_T_Mode[T] )		{	values = calculated_Values->T; }

			plot_Data(argument, values, plot_Options_Second, left_Axis);
		}
	}

	if(curve_Class == INDEPENDENT)
	{
		if(	*argument_Type == whats_This_Angle )	 {	argument = measurement->angle;	}
		if(	*argument_Type == whats_This_Wavelength ){	argument = measurement->lambda;	}

		if(	independent_Variables->calc_Functions.check_Reflectance)   { values = calculated_Values->R; } else
		if(	independent_Variables->calc_Functions.check_Transmittance) { values = calculated_Values->T; } else
		if(	independent_Variables->calc_Functions.check_Absorptance)   { values = calculated_Values->A; }
		// TODO
//		else
//		if(	independent_Variables->calc_Functions.check_User) { values=calculated_Valufgh }

		/// calculated data
		// first value (R,T,A...)
		{
			double coeff=1;
			if(*argument_Type == whats_This_Angle)
			{
				coeff = angle_Coefficients_Map.value(angle_units);
				for(int i=0; i<values.size(); ++i)				{
					argument[i]=argument[i]/coeff;
				}
			}
			if(*argument_Type == whats_This_Wavelength)
			{
				coeff = wavelength_Coefficients_Map.value(wavelength_units);
				for(int i=0; i<values.size(); ++i)				{
					argument[i]=Global_Variables::wavelength_Energy(wavelength_units,argument[i])/coeff;
				}
			}

			plot_Data(argument, values, plot_Options_First, left_Axis);
		}
		// no second value up to now
	}	
	// show max value
	if(graph_Done)
	{
		double max_Value = *std::max_element(values.begin(), values.end());
		int max_Value_Position_Index = values.indexOf(max_Value);
		double max_Value_Position = argument[max_Value_Position_Index];
		max_Value_Label->setText(max_Value_Title + " " + Locale.toString(max_Value,'f',4) + " at " + Locale.toString(max_Value_Position,'f',4) + " " + argument_Units);
	}
	custom_Plot->replot();
}

void Curve_Plot::plot_Data(const QVector<double>& argument, const QVector<double>& values, Plot_Options* plot_Options, QString left_Right)
{
	if(left_Right==left_Axis)
	{
		custom_Plot->addGraph();
	}
	if(left_Right==right_Axis)
	{
		custom_Plot->addGraph(custom_Plot->xAxis2, custom_Plot->yAxis2);
	}
	int graph_Index = custom_Plot->graphCount()-1;
	graph_Options_Map.insert(custom_Plot->graph(graph_Index), plot_Options);
	graph_Left_Right_Map.insert(custom_Plot->graph(graph_Index), left_Right);

	QVector<QCPGraphData> data_To_Plot(argument.size());
	double local_Min = DBL_MAX;
	double local_Max = -DBL_MAX;

	for (int i=0; i<argument.size(); ++i)
	{
		data_To_Plot[i].key = argument[i];
		data_To_Plot[i].value = values[i];

		if(left_Right==left_Axis)
		{
			if(local_Max<data_To_Plot[i].value && (plot_Options->scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {local_Max=data_To_Plot[i].value;}
			if(local_Min>data_To_Plot[i].value && (plot_Options->scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {local_Min=data_To_Plot[i].value;}
		}
		if(left_Right==right_Axis)
		{
			if(local_Max<data_To_Plot[i].value) local_Max=data_To_Plot[i].value;
			if(local_Min>data_To_Plot[i].value) local_Min=data_To_Plot[i].value;
		}
	}
	custom_Plot->graph(graph_Index)->data()->set(data_To_Plot);

	if(left_Right==left_Axis)
	{
		max_Value_Left = max(max_Value_Left, local_Max);
		if(plot_Options->scale == log_Scale)
		{
			if((min_Value_Left>DBL_MIN) && (local_Min<DBL_MIN)) {min_Value_Left = min_Value_Left;} else
			if((min_Value_Left<DBL_MIN) && (local_Min>DBL_MIN)) {min_Value_Left = local_Min;} else
			min_Value_Left = min(min_Value_Left, local_Min) ;
		}
		if(plot_Options->scale == lin_Scale)
		{
			min_Value_Left = min(min_Value_Left, local_Min);
		}
	}
	if(left_Right==right_Axis)
	{
		max_Value_Right = max(max_Value_Right, local_Max);
		min_Value_Right = min(min_Value_Right, local_Min);
	}

	// styling
	QCPScatterStyle scatter_Style;
	if(left_Right==left_Axis)
	{
		custom_Plot->graph(graph_Index)->setPen(QPen(plot_Options->color, plot_Options->thickness));
		scatter_Style.setShape(QCPScatterStyle::ScatterShape(plot_Options->scatter_Shape));
		scatter_Style.setSize(plot_Options->scatter_Size);
	}
	if(left_Right==right_Axis)
	{
		custom_Plot->graph(graph_Index)->setPen(QPen(plot_Options->color_Second, plot_Options->thickness_Second));
		scatter_Style.setShape(QCPScatterStyle::ScatterShape(plot_Options->scatter_Shape_Second));
		scatter_Style.setSize(plot_Options->scatter_Size_Second);
	}
	custom_Plot->graph(graph_Index)->setScatterStyle(scatter_Style);

	// on selection
	custom_Plot->graph(graph_Index)->selectionDecorator()->setPen(QPen(
					custom_Plot->graph(graph_Index)->pen().color(),
				max(custom_Plot->graph(graph_Index)->pen().widthF()*2,3.)));

	if(left_Right==left_Axis)
	{
		custom_Plot->yAxis2->setLabel("");
		if(plot_Options_First->rescale)
		{
			custom_Plot->yAxis->setRange(min_Value_Left,max_Value_Left);
			custom_Plot->xAxis->setRange(argument.first(), argument.last());
		}
	}
	if(left_Right==right_Axis)
	{
		QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
		custom_Plot->yAxis2->setTicker(linTicker);
		custom_Plot->yAxis2->setNumberFormat("g");
		custom_Plot->yAxis2->setNumberPrecision(4);

		disconnect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));

		custom_Plot->yAxis2->setLabel(val_Mode_Label_2);

		if(plot_Options_First->rescale)
		{
			custom_Plot->xAxis2->setRange(argument.first(), argument.last());
			custom_Plot->yAxis2->setRange(min_Value_Right, max_Value_Right);
		}
	}
//	custom_Plot->replot();

	// reset
	min_Value_Left = DBL_MAX;
	max_Value_Left = -DBL_MAX;
	min_Value_Right = DBL_MAX;
	max_Value_Right = -DBL_MAX;
}

void Curve_Plot::refresh_Labels()
{
//	bool show_Second_Label = false;

	// value
	if(curve_Class == TARGET)
	{
		val_Type_Label = target_Curve->curve.value_Function;

		if( target_Curve->curve.value_Mode == value_R_Mode[R] )	{val_Mode_Label_1 = value_R_Mode_Label_1[R];}
		if( target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )	{
			val_Mode_Label_1 = value_R_Mode_Label_1[R_Phi];
			val_Mode_Label_2 = value_R_Mode_Label_2[R_Phi];
//			show_Second_Label = true;
		}
		if( target_Curve->curve.value_Mode == value_T_Mode[T] )	{val_Mode_Label_1 = value_T_Mode[T];}
	}
	if(curve_Class == INDEPENDENT)
	{
		if(	independent_Variables->calc_Functions.check_Reflectance)
		{
			val_Type_Label = value_Function[Reflectance];
			val_Mode_Label_1 = value_R_Mode_Label_1[R];
		}
		if( !independent_Variables->calc_Functions.check_Reflectance &&
			 independent_Variables->calc_Functions.check_Transmittance)
		{
			val_Type_Label = value_Function[Transmittance];
			val_Mode_Label_1 = value_T_Mode[T];
		}
		if( !independent_Variables->calc_Functions.check_Reflectance &&
			!independent_Variables->calc_Functions.check_Transmittance &&
			 independent_Variables->calc_Functions.check_Absorptance )
		{
			val_Type_Label = value_Function[Absorptance];
			val_Mode_Label_1 = value_A_Mode[A];
		}
	}

	// argument
	{
		if(*argument_Type == whats_This_Angle)
		{
			if(*angle_Type == angle_Type_Grazing)	argument_Type_Label = argument_Types[Grazing_angle];
			if(*angle_Type == angle_Type_Incidence)	argument_Type_Label = argument_Types[Incident_angle];

			argument_Label = argument_Type_Label + " " + Theta_Sym + ", " + *angular_Units;

			if(graph_Done)
			{
				argument_Units = *angular_Units;
			}
		}
		if(*argument_Type == whats_This_Wavelength)
		{
			if(	*spectral_Units == wavelength_Units_List[angstrom] ||
				*spectral_Units == wavelength_Units_List[nm]	   ||
				*spectral_Units == wavelength_Units_List[mcm]	   )
			{
				argument_Type_Label = QString(argument_Types[Wavelength_Energy]).split("/").first();
				argument_Label = argument_Type_Label + " " + Lambda_Sym + ", " + spectral_Units;
			} else
			{
				argument_Type_Label = QString(argument_Types[Wavelength_Energy]).split("/").last();
				argument_Label = argument_Type_Label + " E, " + *spectral_Units;
			}

			if(graph_Done)
			{
				argument_Units = *spectral_Units;
			}
		}
	}
	custom_Plot->yAxis->setLabel(val_Type_Label + ", " + val_Mode_Label_1);
	custom_Plot->xAxis->setLabel(argument_Label);

//	custom_Plot->replot();
}

QCPGraph* Curve_Plot::get_Selected_Graph()
{
	if(custom_Plot->selectedGraphs().size()==1)
	{
		return custom_Plot->selectedGraphs()[0];
	}
	return nullptr;
}

void Curve_Plot::choose_Graph_Color()
{
	QCPGraph* graph = get_Selected_Graph();
	if(graph!=nullptr)
	{
		custom_Plot->deselectAll();
//		custom_Plot->replot();

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

void Curve_Plot::set_Graph_Color(QCPGraph* graph, QColor color)
{
	graph->setPen(QPen(color, graph->pen().widthF()));
	graph->selectionDecorator()->setPen(QPen(color,graph->selectionDecorator()->pen().widthF()));
	colors_Button->setPalette(color);
//	custom_Plot->replot();

	// renew data in plot_Options
	Plot_Options* plot_Options = graph_Options_Map.value(graph);
	QString left_Right = graph_Left_Right_Map.value(graph);
	if(left_Right==left_Axis)
	{
		plot_Options->color = color;
	}
	if(left_Right==right_Axis)
	{
		plot_Options->color_Second = color;
	}
}

void Curve_Plot::colorize_Color_Button()
{
	QPalette pal = colors_Button->palette();
	QCPGraph* graph = get_Selected_Graph();
	if(graph!=nullptr)
	{
		pal.setColor(QPalette::Button, graph->pen().color());
		colors_Button->setPalette(pal);
	} else
	{
		colors_Button->setPalette(default_Colors_Button_Palette);
	}
}

void Curve_Plot::show_Thickness()
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

void Curve_Plot::show_Scatter_Size()
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

void Curve_Plot::change_Thickness()
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
		QString left_Right = graph_Left_Right_Map.value(graph);
		if(left_Right==left_Axis)
		{
			plot_Options->thickness = new_Width;
		}
		if(left_Right==right_Axis)
		{
			plot_Options->thickness_Second = new_Width;
		}
	}
}

void Curve_Plot::change_Scatter_Size()
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
		QString left_Right = graph_Left_Right_Map.value(graph);
		if(left_Right==left_Axis)
		{
			plot_Options->scatter_Size = new_Scatter_Size;
		}
		if(left_Right==right_Axis)
		{
			plot_Options->scatter_Size_Second = new_Scatter_Size;
		}
	}
}
