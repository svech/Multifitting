// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "curve_plot.h"

Curve_Plot::Curve_Plot(Target_Curve* target_Curve, Independent_Variables* independent_Variables, QString curve_Class, QWidget* parent) :
	curve_Class(curve_Class),
	target_Curve(target_Curve),
	independent_Variables(independent_Variables),
	QWidget(parent)
{
	// if target
	if(curve_Class == TARGET)
	if(target_Curve) // if passed pointer!=nullptr
	{		
		measurement = &target_Curve->measurement;
		calculated_Values = &target_Curve->calculated_Values;
		plot_Options_First = &target_Curve->plot_Options_Experimental;
		plot_Options_Second = &target_Curve->plot_Options_Calculated;
		argument_Type = target_Curve->curve.argument_Type;
	} else
	{
		QMessageBox::critical(nullptr, "Curve_Plot::Curve_Plot", "target_Curve is nullptr");
		exit(EXIT_FAILURE);
	}

	// if independent
	if(curve_Class == INDEPENDENT)
	if(independent_Variables) // if passed pointer!=nullptr
	{
		measurement = &independent_Variables->measurement;
		calculated_Values = &independent_Variables->calculated_Values;
		plot_Options_First = &independent_Variables->plot_Options;
		plot_Options_Second = plot_Options_First;

		// find whats_This of active item
		for(int item_Index=0; item_Index<independent_Variables->independent_Variables_List->count(); ++item_Index)
		{
			QListWidgetItem* list_Item = independent_Variables->independent_Variables_List->item(item_Index);
			Independent_Indicator item_Indicator = list_Item->data(Qt::UserRole).value<Independent_Indicator>();
			// if active
			if(item_Indicator.is_Active)
			{
				argument_Type = item_Indicator.parameter_Whats_This;
			}
		}
	} else
	{
		QMessageBox::critical(nullptr, "Curve_Plot::Curve_Plot", "independent_Variables is nullptr");
		exit(EXIT_FAILURE);
	}

	create_Main_Layout();
//	plot_All_Data();
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
}

void Curve_Plot::create_Plot_Frame_And_Scale()
{
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
			custom_Plot->xAxis->setRange(0, 9);
			custom_Plot->yAxis->setRange(0, 1);

			custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis2->setTicker(linTicker);
			custom_Plot->yAxis2->setNumberFormat("g");
			custom_Plot->yAxis2->setNumberPrecision(4);
			custom_Plot->yAxis2->setRange(0, 1);
		}
		if(plot_Options_First->scale == log_Scale)
		{
			QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);

			custom_Plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
			custom_Plot->yAxis->setTicker(logTicker);
			custom_Plot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
			custom_Plot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
			custom_Plot->xAxis->setRange(0, 9);
			custom_Plot->yAxis->setRange(1e-5, 1e0);

//			if(	target_Curve->curve.value_Mode == value_R_Mode[R] ||
//				target_Curve->curve.value_Mode == value_T_Mode[T] ||
//				target_Curve->curve.value_Mode == value_A_Mode[A] )
			{
				custom_Plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
				custom_Plot->yAxis2->setTicker(logTicker);
				custom_Plot->yAxis2->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
				custom_Plot->yAxis2->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
				custom_Plot->yAxis2->setRange(1e-5, 1e0);
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

	custom_Plot->replot();
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
			lin_RadioButton->toggled(true);
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
			log_RadioButton->toggled(true);
		}
	}

	colors_Button = new QPushButton;
	symbol_ComboBox = new QComboBox;

	{
		thickness_Label = new QLabel("  Thickness:");

		thickness_Spin = new QDoubleSpinBox;
//		thickness_Spin->setValue(plot_Options_First->thickness);
		thickness_Spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
		thickness_Spin->setAccelerated(true);
		thickness_Spin->setRange(0, MAX_DOUBLE);
		thickness_Spin->setDecimals(1);
		thickness_Spin->setSingleStep(0.1);
		thickness_Spin->setFixedWidth(30);
	}

	// layout
	{
		options_Layout = new QHBoxLayout(options_GroupBox);
		options_Layout->setAlignment(Qt::AlignLeft);

		options_Layout->addWidget(scale_Label);
		options_Layout->addWidget(lin_RadioButton);
		options_Layout->addWidget(log_RadioButton);

		options_Layout->addWidget(colors_Button);
		options_Layout->addWidget(symbol_ComboBox);

		options_Layout->addWidget(thickness_Label);
		options_Layout->addWidget(thickness_Spin);
	}

	options_GroupBox->adjustSize();
	options_GroupBox->setFixedHeight(options_GroupBox->height());
}

void Curve_Plot::plot_All_Data()
{
	create_Plot_Frame_And_Scale();
	custom_Plot->clearGraphs();

	if(curve_Class == TARGET)
	{
		argument = target_Curve->curve.shifted_Argument;
		values.resize(target_Curve->curve.shifted_Values.size());

		/// experimental data
		// first value (R,T,A...)
		{
			for(int i=0; i<target_Curve->curve.shifted_Values.size(); ++i)		{
				values[i] = target_Curve->curve.shifted_Values[i].val_1;
			}
			plot_Data(argument, values, *plot_Options_First, left);
		}
		// second value (phase)
		if(target_Curve->curve.value_Mode == value_R_Mode[R_Phi])
		{
			for(int i=0; i<target_Curve->curve.shifted_Values.size(); ++i)	{
				values[i] = target_Curve->curve.shifted_Values[i].val_2;
			}
			plot_Data(argument, values, *plot_Options_First, right);
		}

		/// calculated data
		// first value (R,T,A...)
		{
			if(	target_Curve->curve.value_Mode == value_R_Mode[R] ||
				target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )	{	values = calculated_Values->R; }
//			if(	target_Curve->curve.value_Mode == value_T_Mode[T] )		{	values = calculated_Values->T; }
//			if(	target_Curve->curve.value_Mode == value_A_Mode[A] )		{	values = calculated_Values->A; }

			plot_Data(argument, values, *plot_Options_Second, left);
		}
		// second value (phase)
		if(target_Curve->curve.value_Mode == value_R_Mode[R_Phi])
		{
			values = calculated_Values->Phi;
			plot_Data(argument, values, *plot_Options_Second, right);
		}
	}

	if(curve_Class == INDEPENDENT)
	{
		if(	argument_Type == whats_This_Angle )		{	argument = measurement->angle;	}
		if(	argument_Type == whats_This_Wavelength ){	argument = measurement->lambda;	}

		if(	independent_Variables->calc_Functions.check_Reflectance) { values=calculated_Values->R; }

		/// calculated data
		// first value (R,T,A...)
		{
			plot_Data(argument, values, *plot_Options_First, left);
		}
		// no second value up to now
	}
}

void Curve_Plot::plot_Data(const QVector<double>& argument, const QVector<double>& values, Plot_Options plot_Options, QString left_Right)
{
	if(left_Right==left)
	{
		custom_Plot->addGraph();
	}
	if(left_Right==right)
	{
		custom_Plot->addGraph(custom_Plot->xAxis2, custom_Plot->yAxis2);
	}
	int graph_Index = custom_Plot->graphCount()-1;

	QVector<QCPGraphData> data_To_Plot(argument.size());
	double local_Min = DBL_MAX;
	double local_Max = -DBL_MAX;

	for (int i=0; i<argument.size(); ++i)
	{
		data_To_Plot[i].key = argument[i];
		data_To_Plot[i].value = values[i];

		if(left_Right==left)
		{
			if(local_Max<data_To_Plot[i].value && (plot_Options.scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {local_Max=data_To_Plot[i].value;}
			if(local_Min>data_To_Plot[i].value && (plot_Options.scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {local_Min=data_To_Plot[i].value;}
		}
		if(left_Right==right)
		{
			if(local_Max<data_To_Plot[i].value) local_Max=data_To_Plot[i].value;
			if(local_Min>data_To_Plot[i].value) local_Min=data_To_Plot[i].value;
		}
	}

	if(left_Right==left)
	{
		max_Value_Left = max(max_Value_Left, local_Max);
		if(plot_Options.scale == log_Scale)
		{
			if((min_Value_Left>DBL_MIN) && (local_Min<DBL_MIN)) {min_Value_Left = min_Value_Left;} else
			if((min_Value_Left<DBL_MIN) && (local_Min>DBL_MIN)) {min_Value_Left = local_Min;} else
			min_Value_Left = min(min_Value_Left, local_Min) ;
		}
		if(plot_Options.scale == lin_Scale)
		{
			min_Value_Left = min(min_Value_Left, local_Min);
		}
	}
	if(left_Right==right)
	{
		max_Value_Right = max(max_Value_Right, local_Max);
		min_Value_Right = min(min_Value_Right, local_Min);
	}

	// styling
	QCPScatterStyle scatter_Style;
	if(left_Right==left)
	{
		custom_Plot->graph(graph_Index)->setPen(QPen(plot_Options.color, plot_Options.thickness));		
		scatter_Style.setShape(QCPScatterStyle::ScatterShape(plot_Options.scatter_Shape));
		scatter_Style.setSize(plot_Options.scatter_Size);
	}
	if(left_Right==right)
	{
		custom_Plot->graph(graph_Index)->setPen(QPen(plot_Options.color_Second, plot_Options.thickness_Second));
		scatter_Style.setShape(QCPScatterStyle::ScatterShape(plot_Options.scatter_Shape_Second));
		scatter_Style.setSize(plot_Options.scatter_Size_Second);
	}
	custom_Plot->graph(graph_Index)->setScatterStyle(scatter_Style);

	custom_Plot->graph(graph_Index)->data()->set(data_To_Plot);
	if(left_Right==left)
	{
		custom_Plot->xAxis->setRange(argument.first(), argument.last());
		custom_Plot->yAxis->setRange(min_Value_Left,max_Value_Left);
	}
	if(left_Right==right)
	{
		QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
		custom_Plot->yAxis2->setTicker(linTicker);
		custom_Plot->yAxis2->setNumberFormat("g");
		custom_Plot->yAxis2->setNumberPrecision(4);

		disconnect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));

		custom_Plot->xAxis2->setRange(argument.first(), argument.last());
		custom_Plot->yAxis2->setRange(min_Value_Right, max_Value_Right);
	}
	custom_Plot->replot();
}
