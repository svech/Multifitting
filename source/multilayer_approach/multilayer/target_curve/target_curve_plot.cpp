#include "target_curve_plot.h"

Target_Curve_Plot::Target_Curve_Plot(Target_Curve* target_Curve, QWidget *parent) :
	target_Curve(target_Curve),
	QWidget(parent)
{
	create_Main_Layout();
}

void Target_Curve_Plot::create_Main_Layout()
{
	main_layout = new QVBoxLayout(this);
		main_layout->setContentsMargins(0,0,0,0);
		main_layout->setSpacing(0);

	create_Plot();
}

void Target_Curve_Plot::create_Plot()
{
	custom_Plot = new QCustomPlot;
		custom_Plot->setMinimumHeight(300);
	main_layout->addWidget(custom_Plot);

	if( target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		create_Subinterval_Rectangle();
		create_Plot_Options_GroupBox();
	}
	if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		// TODO GISAS
//		color_Map = new QCPColorMap()
		create_Subinterval_Rectangle();
		create_Plot_Options_GroupBox();
	}
}

void Target_Curve_Plot::create_Plot_Frame_And_Scale_1D()
{
	custom_Plot->setNoAntialiasingOnDrag(false); // more performance/responsiveness during dragging

	// frame, axes

	QPen pen = custom_Plot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
	custom_Plot->yAxis->grid()->setSubGridVisible(true);
	custom_Plot->xAxis->grid()->setSubGridVisible(true);
	custom_Plot->yAxis->grid()->setPen(pen);
	custom_Plot->xAxis->grid()->setPen(pen);

	custom_Plot->xAxis->setRange(0, 5);

	if(target_Curve->plot_Options_Experimental.y_Scale == log_Scale)
	{
		custom_Plot->yAxis ->setScaleType(QCPAxis::stLogarithmic);
		custom_Plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);

		QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
		custom_Plot->yAxis->setTicker(logTicker);
		custom_Plot->yAxis2->setTicker(logTicker);
		custom_Plot->yAxis ->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
		custom_Plot->yAxis2->setNumberFormat("eb");
		custom_Plot->yAxis ->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
		custom_Plot->yAxis2->setNumberPrecision(0);
		custom_Plot->yAxis ->setRange(1e-5, 1e0);
		custom_Plot->yAxis2->setRange(1e-5, 1e0);

	}
	if(target_Curve->plot_Options_Experimental.y_Scale == lin_Scale)
	{
		custom_Plot->yAxis ->setScaleType(QCPAxis::stLinear);
		custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);

		QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		custom_Plot->yAxis ->setTicker(linTicker);
		custom_Plot->yAxis2->setTicker(linTicker);
		custom_Plot->yAxis ->setNumberFormat("g");
		custom_Plot->yAxis2->setNumberFormat("g");
		custom_Plot->yAxis ->setNumberPrecision(4);
		custom_Plot->yAxis2->setNumberPrecision(4);
		custom_Plot->yAxis ->setRange(0, 1);
		custom_Plot->yAxis2->setRange(0, 1);
	}

	// make range draggable and zoomable:
	custom_Plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom);

	// make top right axes clones of bottom left axes:
	custom_Plot->xAxis2->setVisible(true);
	custom_Plot->yAxis2->setVisible(true);
	custom_Plot->xAxis2->setTickLabels(false);

	// connect signals so top and right axes move in sync with bottom and left axes:
	connect(custom_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->xAxis2, SLOT(setRange(QCPRange)));
	connect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));

	custom_Plot->replot();
}

void Target_Curve_Plot::create_Subinterval_Rectangle()
{
	start_Rect = new QCPItemRect(custom_Plot);
			start_Rect->setPen  (subinterval_Plot_Pen);
			start_Rect->setBrush(subinterval_Plot_Brush);
	end_Rect = new QCPItemRect(custom_Plot);
			end_Rect->setPen  (subinterval_Plot_Pen);
			end_Rect->setBrush(subinterval_Plot_Brush);

	if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		top_Rect = new QCPItemRect(custom_Plot);
				top_Rect->setPen  (subinterval_Plot_Pen);
				top_Rect->setBrush(subinterval_Plot_Brush);
		bottom_Rect = new QCPItemRect(custom_Plot);
				bottom_Rect->setPen  (subinterval_Plot_Pen);
				bottom_Rect->setBrush(subinterval_Plot_Brush);
	}

	subinterval_Changed_Replot();

	connect(custom_Plot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Target_Curve_Plot::subinterval_Changed_Replot);
	connect(custom_Plot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Target_Curve_Plot::subinterval_Changed_Replot);
}

void Target_Curve_Plot::subinterval_Changed_Replot()
{
	start_Rect->topLeft->setCoords(custom_Plot->xAxis->range().lower, custom_Plot->yAxis->range().upper);
	start_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Left,custom_Plot->yAxis->range().lower);

	end_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Right,custom_Plot->yAxis->range().upper);
	end_Rect->bottomRight->setCoords(custom_Plot->xAxis->range().upper, custom_Plot->yAxis->range().lower);

	// show/hide
	start_Rect->setVisible(target_Curve->curve.use_Subinterval);
	end_Rect->setVisible(target_Curve->curve.use_Subinterval);

	if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		top_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Left, custom_Plot->yAxis->range().upper);
		top_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Right,target_Curve->curve.subinterval_Top);

		bottom_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Left, target_Curve->curve.subinterval_Bottom);
		bottom_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Right, custom_Plot->yAxis->range().lower);

		// show/hide
		top_Rect->setVisible(target_Curve->curve.use_Subinterval);
		bottom_Rect->setVisible(target_Curve->curve.use_Subinterval);
	}

	custom_Plot->replot();
}

void Target_Curve_Plot::plot_Data(bool fast)
{
	if(!fast)
	{
		if( target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
		{
			create_Plot_Frame_And_Scale_1D();
		}
		if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
		{
			// TODO GISAS
	//		color_Map = new QCPColorMap()
//			create_Plot_Frame_And_Scale_2D();
		}
	}

	if(target_Curve->loaded_And_Ready)
	{
		if( target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
			target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
		{
			int data_Count = target_Curve->curve.argument.size();

			// first graph
			{
				QVector<QCPGraphData> data_To_Plot(data_Count);
				double min = DBL_MAX;
				double max = 0;
				for (int i=0; i<data_Count; ++i)
				{
					data_To_Plot[i].key   = target_Curve->curve.shifted_Argument[i];
					data_To_Plot[i].value = target_Curve->curve.shifted_Values  [i];

					if(max<data_To_Plot[i].value && (target_Curve->plot_Options_Experimental.y_Scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {max=data_To_Plot[i].value;}
					if(min>data_To_Plot[i].value && (target_Curve->plot_Options_Experimental.y_Scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {min=data_To_Plot[i].value;}
				}

				if(!fast)
				{
					if(custom_Plot->graphCount()!=1)
					{
						custom_Plot->clearGraphs();
						custom_Plot->addGraph();
					}
					custom_Plot->graph()->setPen(QPen(target_Curve->plot_Options_Experimental.color,target_Curve->plot_Options_Experimental.thickness));
					QCPScatterStyle scatter_Style;
					scatter_Style.setShape(QCPScatterStyle::ScatterShape(target_Curve->plot_Options_Experimental.scatter_Shape));
					scatter_Style.setSize(target_Curve->plot_Options_Experimental.scatter_Size);
					custom_Plot->graph()->setScatterStyle(scatter_Style);
				}

				custom_Plot->graph()->data()->set(data_To_Plot);
				custom_Plot->xAxis->setRange(target_Curve->curve.shifted_Argument[0], target_Curve->curve.shifted_Argument[data_Count-1]);
				custom_Plot->yAxis->setRange(min,max);
			}
		}
		if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
		{
			// TODO GISAS
		}
	}

	if(!fast)
	{
		refresh_Labels();
	} else
	{
		custom_Plot->replot();
	}
}

void Target_Curve_Plot::create_Plot_Options_GroupBox()
{
	QGroupBox* plot_Options_GroupBox = new QGroupBox("Plot options");
		main_layout->addWidget(plot_Options_GroupBox);
	QVBoxLayout* plot_Options_GroupBox_Layout = new QVBoxLayout(plot_Options_GroupBox);
		plot_Options_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	{
		QHBoxLayout* radio_Button_Layout = new QHBoxLayout;
			radio_Button_Layout->setAlignment(Qt::AlignLeft);
		plot_Options_GroupBox_Layout->addLayout(radio_Button_Layout);

		QLabel* scale_Label = new QLabel("Scale: ");
		radio_Button_Layout->addWidget(scale_Label);

		QRadioButton* lin_Radio_Button = new QRadioButton("Lin");
		radio_Button_Layout->addWidget(lin_Radio_Button);
		connect(lin_Radio_Button, &QRadioButton::toggled, this, [=]
		{
			if(lin_Radio_Button->isChecked())
			{
				target_Curve->plot_Options_Experimental.y_Scale = lin_Scale;
				target_Curve->plot_Options_Calculated.y_Scale = lin_Scale;
			}
			plot_Data();
		});
		connect(lin_Radio_Button, &QRadioButton::clicked, lin_Radio_Button, &QRadioButton::toggled);

		if(target_Curve->plot_Options_Experimental.y_Scale == lin_Scale)
		{
			lin_Radio_Button->setChecked(true);
			lin_Radio_Button->toggled(true);
		}

		QRadioButton* log_Radio_Button = new QRadioButton("Log");
		radio_Button_Layout->addWidget(log_Radio_Button);
		connect(log_Radio_Button, &QRadioButton::toggled, this, [=]
		{
			if(log_Radio_Button->isChecked())
			{
				target_Curve->plot_Options_Experimental.y_Scale = log_Scale;
				target_Curve->plot_Options_Calculated.y_Scale = log_Scale;
			}
			plot_Data();
		});
		connect(log_Radio_Button, &QRadioButton::clicked, log_Radio_Button, &QRadioButton::toggled);

		if(target_Curve->plot_Options_Experimental.y_Scale == log_Scale)
		{
			log_Radio_Button->setChecked(true);
			log_Radio_Button->toggled(true);
		}
	}
	plot_Options_GroupBox->adjustSize();
	plot_Options_GroupBox->setFixedHeight(plot_Options_GroupBox->height());
}

void Target_Curve_Plot::refresh_Labels()
{
	// argument
	custom_Plot->xAxis->setLabel(argument_Type_Text + argument_Sym_Text + ", " + argument_Units_Text);
	// value
	custom_Plot->yAxis->setLabel(value_Type_Text);

	custom_Plot->yAxis2->setLabel("");
	custom_Plot->yAxis2->setLabelColor(Qt::black);
	custom_Plot->yAxis2->setTickLabelColor(Qt::black);
	custom_Plot->yAxis->setLabelColor(Qt::black);
	custom_Plot->yAxis->setTickLabelColor(Qt::black);

	custom_Plot->replot();
}
