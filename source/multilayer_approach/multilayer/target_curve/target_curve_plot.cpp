#include "target_curve_plot.h"
#include "multilayer_approach/multilayer_approach.h"

Target_Curve_Plot::Target_Curve_Plot(Target_Curve* target_Curve, QWidget *parent) :
	target_Curve(target_Curve),
	QWidget(parent)
{
	create_Main_Layout();
}

void Target_Curve_Plot::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setContentsMargins(0,0,0,0);
		main_Layout->setSpacing(0);

	custom_Plot = new QCustomPlot;
		custom_Plot->setMinimumHeight(230);
	main_Layout->addWidget(custom_Plot);

	if( target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		create_Subinterval_Rectangle_1D();
		create_Plot_Frame_And_Scale_1D();
		plot_Data_1D();
		refresh_Labels_1D();
		create_Plot_Options_GroupBox_1D();
	}
	if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		color_Map = new QCPColorMap(custom_Plot->xAxis, custom_Plot->yAxis);
		color_Scale = new QCPColorScale(custom_Plot);
		margin_Group = new QCPMarginGroup(custom_Plot);
		custom_Plot->plotLayout()->addElement(0, 1, color_Scale); // add it to the right of the main axis rect

		create_Subinterval_Rectangle_2D();
		create_Plot_Frame_And_Scale_2D();
		plot_Data_2D();
		refresh_Labels_2D();
		create_Plot_Options_GroupBox_2D();

		// color scheme editor
		connect(custom_Plot, &QCustomPlot::axisDoubleClick, this, [=](QCPAxis* axis, QCPAxis::SelectablePart part, QMouseEvent* event)
		{
			Q_UNUSED(part)
			Q_UNUSED(event)
			if(axis == color_Scale->axis())	{Global_Variables::color_Scheme_Change(color_Map, custom_Plot, &target_Curve->plot_Options_Experimental.color_Scheme);}
		}, Qt::UniqueConnection);
	}
}

// 1D
void Target_Curve_Plot::create_Plot_Frame_And_Scale_1D()
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

	// create graph and set pen
	if(custom_Plot->graphCount()!=1)
	{
		custom_Plot->clearGraphs();
		custom_Plot->addGraph();
	}
	custom_Plot->graph()->setPen(QPen(target_Curve->plot_Options_Experimental.color,target_Curve->plot_Options_Experimental.thickness));

	// set scatter slyle
	QCPScatterStyle scatter_Style;
		scatter_Style.setShape(QCPScatterStyle::ScatterShape(target_Curve->plot_Options_Experimental.scatter_Shape));
		scatter_Style.setSize(target_Curve->plot_Options_Experimental.scatter_Size);
	custom_Plot->graph()->setScatterStyle(scatter_Style);

	// scale
	if(target_Curve->plot_Options_Experimental.y_Scale == log_Scale)  apply_Log_Scale_1D();
	if(target_Curve->plot_Options_Experimental.y_Scale == lin_Scale)  apply_Lin_Scale_1D();

	// default ranges
	custom_Plot->yAxis->setRange(1e-5, 1e0);
	custom_Plot->yAxis->setRange(0, 1);

	custom_Plot->replot();
}

void Target_Curve_Plot::apply_Log_Scale_1D()
{
	custom_Plot->yAxis ->setScaleType(QCPAxis::stLogarithmic);
	custom_Plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
	QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
	custom_Plot->yAxis->setTicker(logTicker);
	custom_Plot->yAxis2->setTicker(logTicker);
	custom_Plot->yAxis ->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
	custom_Plot->yAxis2->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
	custom_Plot->yAxis ->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
	custom_Plot->yAxis2->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
}

void Target_Curve_Plot::apply_Lin_Scale_1D()
{
	custom_Plot->yAxis ->setScaleType(QCPAxis::stLinear);
	custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		linTicker->setTickCount(8);
	custom_Plot->yAxis ->setTicker(linTicker);
	custom_Plot->yAxis2->setTicker(linTicker);
	custom_Plot->yAxis ->setNumberFormat("g");
	custom_Plot->yAxis2->setNumberFormat("g");
	custom_Plot->yAxis ->setNumberPrecision(4);
	custom_Plot->yAxis2->setNumberPrecision(4);
}

void Target_Curve_Plot::plot_Data_1D()
{
	if(target_Curve->loaded_And_Ready)
	{
		int data_Count = target_Curve->curve.shifted_Argument.size();
		QVector<QCPGraphData> data_To_Plot(data_Count);
		double min = DBL_MAX;
		double max = -DBL_MAX;
		for (int i=0; i<data_Count; ++i)
		{
			data_To_Plot[i].key   = target_Curve->curve.shifted_Argument[i];
			data_To_Plot[i].value = target_Curve->curve.shifted_Values  [i];

			if(max<data_To_Plot[i].value && (target_Curve->plot_Options_Experimental.y_Scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {max=data_To_Plot[i].value;}
			if(min>data_To_Plot[i].value && (target_Curve->plot_Options_Experimental.y_Scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {min=data_To_Plot[i].value;}
		}
		custom_Plot->graph()->data()->set(data_To_Plot);
		custom_Plot->xAxis->setRange(target_Curve->curve.shifted_Argument.front(), target_Curve->curve.shifted_Argument.back());
		custom_Plot->yAxis->setRange(min,max);
		custom_Plot->replot();
	}
}

void Target_Curve_Plot::refresh_Labels_1D()
{
	custom_Plot->xAxis->setLabel(argument_Type_Text + argument_Sym_Text + ", " + argument_Units_Text);
	custom_Plot->yAxis->setLabel(value_Type_Text);
	custom_Plot->replot();
}

void Target_Curve_Plot::create_Plot_Options_GroupBox_1D()
{
	QGroupBox* plot_Options_GroupBox = new QGroupBox("Plot options");
	main_Layout->addWidget(plot_Options_GroupBox);

	QHBoxLayout* plot_Options_GroupBox_Layout = new QHBoxLayout(plot_Options_GroupBox);
		plot_Options_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	//  scale radiobuttons
	{
		QHBoxLayout* radio_Button_Layout = new QHBoxLayout;
			radio_Button_Layout->setAlignment(Qt::AlignLeft);
		plot_Options_GroupBox_Layout->addLayout(radio_Button_Layout);

		QLabel* scale_Label = new QLabel("Scale: ");
		radio_Button_Layout->addWidget(scale_Label);

		QRadioButton* lin_Radio_Button = new QRadioButton("Lin");
			lin_Radio_Button->setChecked(target_Curve->plot_Options_Experimental.y_Scale == lin_Scale);
		radio_Button_Layout->addWidget(lin_Radio_Button);
		connect(lin_Radio_Button, &QRadioButton::toggled, this, [=]
		{
			if(lin_Radio_Button->isChecked())
			{
				target_Curve->plot_Options_Experimental.y_Scale = lin_Scale;
//				target_Curve->plot_Options_Calculated.y_Scale = lin_Scale;
			}
			create_Plot_Frame_And_Scale_1D();
			plot_Data_1D();
		});
		connect(lin_Radio_Button, &QRadioButton::clicked, lin_Radio_Button, &QRadioButton::toggled);

		// -----------------------------------------------------------

		QRadioButton* log_Radio_Button = new QRadioButton("Log");
			log_Radio_Button->setChecked(target_Curve->plot_Options_Experimental.y_Scale == log_Scale);
		radio_Button_Layout->addWidget(log_Radio_Button);
		connect(log_Radio_Button, &QRadioButton::toggled, this, [=]
		{
			if(log_Radio_Button->isChecked())
			{
				target_Curve->plot_Options_Experimental.y_Scale = log_Scale;
//				target_Curve->plot_Options_Calculated.y_Scale = log_Scale;
			}
			create_Plot_Frame_And_Scale_1D();
			plot_Data_1D();
		});
		connect(log_Radio_Button, &QRadioButton::clicked, log_Radio_Button, &QRadioButton::toggled);

		// -----------------------------------------------------------

		QButtonGroup* buttonGroup = new QButtonGroup;
			buttonGroup->addButton(lin_Radio_Button);
			buttonGroup->addButton(log_Radio_Button);
	}

	plot_Options_GroupBox->adjustSize();
	plot_Options_GroupBox->setFixedHeight(plot_Options_GroupBox->height());
}

// 2D
void Target_Curve_Plot::create_Plot_Frame_And_Scale_2D()
{
	custom_Plot->setNoAntialiasingOnDrag(false);
	custom_Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	// frame
	custom_Plot->axisRect()->setupFullAxesBox(true);
	custom_Plot->yAxis2->setTickLabels(true);
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		linTicker->setTickCount(12);
	custom_Plot->xAxis ->setTicker(linTicker);
	custom_Plot->xAxis2->setTicker(linTicker);
	custom_Plot->yAxis ->setTicker(linTicker);
	custom_Plot->yAxis2->setTicker(linTicker);

	// set grid style
	QPen pen = custom_Plot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
	custom_Plot->yAxis->grid()->setSubGridVisible(true);
	custom_Plot->xAxis->grid()->setSubGridVisible(true);
	custom_Plot->yAxis->grid()->setPen(pen);
	custom_Plot->xAxis->grid()->setPen(pen);

	// add a color scale:
	color_Scale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
	color_Map->setColorScale(color_Scale); // associate the color map with the color scale
	color_Map->setTightBoundary(true);

	// make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
	custom_Plot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, margin_Group);
	color_Scale->setMarginGroup(QCP::msBottom | QCP::msTop, margin_Group);
	color_Map->setGradient(target_Curve->plot_Options_Experimental.color_Scheme); // set the color gradient of the color map to one of the presets

	// scale
	if(target_Curve->plot_Options_Experimental.z_Scale == log_Scale)  apply_Log_Scale_2D();
	if(target_Curve->plot_Options_Experimental.z_Scale == lin_Scale)  apply_Lin_Scale_2D();

	// default ranges
	refresh_Axes_Range_2D();
}

void Target_Curve_Plot::refresh_Axes_Range_2D()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);
	color_Map->data()->setRange(QCPRange(target_Curve->measurement.detector_Theta_Angle.independent.min/coeff, target_Curve->measurement.detector_Theta_Angle.independent.max/coeff),
								QCPRange(target_Curve->measurement.detector_Phi_Angle.independent.min/coeff,   target_Curve->measurement.detector_Phi_Angle.independent.max/coeff));
	custom_Plot->rescaleAxes(); // rescale the key (x) and value (y) axes so the whole color map is visible:
}

void Target_Curve_Plot::apply_Log_Scale_2D()
{
	color_Scale->axis()->setScaleType(QCPAxis::stLogarithmic);
	QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
	color_Scale->axis()->setTicker(logTicker);
	color_Scale->axis()->setNumberFormat("eb");
	color_Scale->axis()->setNumberPrecision(0);
}

void Target_Curve_Plot::apply_Lin_Scale_2D()
{
	color_Scale->axis()->setScaleType(QCPAxis::stLinear);
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
	color_Scale->axis()->setTicker(linTicker);
	color_Scale->axis()->setNumberFormat("gbc");
	color_Scale->axis()->setNumberPrecision(2);
}

void Target_Curve_Plot::plot_Data_2D()
{
	if(target_Curve->loaded_And_Ready)
	{
		int nx = target_Curve->curve.value_2D_Shifted.front().size();
		int ny = target_Curve->curve.value_2D_Shifted.size();
		color_Map->data()->setSize(nx, ny);

		double min_Val = DBL_MAX;
		double max_Val = -DBL_MAX;
		for (int x_Index=0; x_Index<nx; ++x_Index)
		{
			for (int y_Index=0; y_Index<ny; ++y_Index)
			{
				double val = target_Curve->curve.value_2D_Shifted[y_Index][x_Index];
				color_Map->data()->setCell(x_Index, y_Index, val);

				if(max_Val<val && (target_Curve->plot_Options_Experimental.z_Scale == lin_Scale || val > DBL_MIN)) {max_Val = val;}
				if(min_Val>val && (target_Curve->plot_Options_Experimental.z_Scale == lin_Scale || val > DBL_MIN)) {min_Val = val;}
			}
		}
		// x,y ranges
		refresh_Axes_Range_2D();

		// z range
                // color_Map->rescaleDataRange(); // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient
                if(target_Curve->plot_Options_Experimental.z_Scale == log_Scale) {
                        min_Val = max(min_Val,max_Val/pow(10,target_Curve->plot_Options_Experimental.orders_To_Show));
                }
		color_Map->setDataRange(QCPRange(min_Val,max_Val));

		color_Map->setInterpolate(target_Curve->plot_Options_Experimental.use_Interpolation);
		custom_Plot->replot();
	}
}

void Target_Curve_Plot::refresh_Labels_2D()
{
	custom_Plot->xAxis->setLabel(argument_Type_Text + ", " + argument_Units_Text);
	custom_Plot->yAxis->setLabel(value_Type_Text + ", " + argument_Units_Text);
	custom_Plot->replot();
}

void Target_Curve_Plot::create_Plot_Options_GroupBox_2D()
{
	QGroupBox* plot_Options_GroupBox = new QGroupBox("Plot options");
	main_Layout->addWidget(plot_Options_GroupBox);

	QHBoxLayout* plot_Options_GroupBox_Layout = new QHBoxLayout(plot_Options_GroupBox);
		plot_Options_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	//  scale radiobuttons
	{
		QHBoxLayout* radio_Button_Layout = new QHBoxLayout;
			radio_Button_Layout->setAlignment(Qt::AlignLeft);
		plot_Options_GroupBox_Layout->addLayout(radio_Button_Layout);

		QLabel* scale_Label = new QLabel("Scale: ");
		radio_Button_Layout->addWidget(scale_Label);

		QRadioButton* lin_Radio_Button = new QRadioButton("Lin");
			lin_Radio_Button->setChecked(target_Curve->plot_Options_Experimental.z_Scale == lin_Scale);
		radio_Button_Layout->addWidget(lin_Radio_Button);
		connect(lin_Radio_Button, &QRadioButton::toggled, this, [=]
		{
			if(lin_Radio_Button->isChecked())
			{
				target_Curve->plot_Options_Experimental.z_Scale = lin_Scale;
				target_Curve->plot_Options_Calculated.z_Scale = lin_Scale;
			}
			create_Plot_Frame_And_Scale_2D();
			plot_Data_2D();
		});
		connect(lin_Radio_Button, &QRadioButton::clicked, lin_Radio_Button, &QRadioButton::toggled);

		QRadioButton* log_Radio_Button = new QRadioButton("Log");
			log_Radio_Button->setChecked(target_Curve->plot_Options_Experimental.z_Scale == log_Scale);
		radio_Button_Layout->addWidget(log_Radio_Button);
		connect(log_Radio_Button, &QRadioButton::toggled, this, [=]
		{
			if(log_Radio_Button->isChecked())
			{
				target_Curve->plot_Options_Experimental.z_Scale = log_Scale;
				target_Curve->plot_Options_Calculated.z_Scale = log_Scale;
			}
			create_Plot_Frame_And_Scale_2D();
			plot_Data_2D();
		});
		connect(log_Radio_Button, &QRadioButton::clicked, log_Radio_Button, &QRadioButton::toggled);
	}
	//  rotations
	{
		QHBoxLayout* rotate_Button_Layout = new QHBoxLayout;
			rotate_Button_Layout->setAlignment(Qt::AlignLeft);
		plot_Options_GroupBox_Layout->addLayout(rotate_Button_Layout);

		QLabel* rotate_Label = new QLabel("Rotate: ");
		rotate_Button_Layout->addWidget(rotate_Label);

		QPixmap rotate_Left_Pic	(Paths_Icon_Path + "rotate-left-24.png");
		QPixmap rotate_Right_Pic(Paths_Icon_Path + "rotate-right-24.png");

		QToolButton* rotate_Left_Button = new QToolButton;
			rotate_Left_Button->setIcon(QIcon(rotate_Left_Pic));
			rotate_Left_Button->setToolTip("Rotate 90"+Degree_Sym+" left");
		rotate_Button_Layout->addWidget(rotate_Left_Button);
		connect(rotate_Left_Button, &QToolButton::clicked, this, [=]
		{
			if(target_Curve->loaded_And_Ready)
			{
				target_Curve->plot_Options_Experimental.rotation_Angle += 90;
				target_Curve->plot_Options_Calculated.rotation_Angle += 90;
				target_Curve->plot_Options_Experimental.rotation_Angle = (target_Curve->plot_Options_Experimental.rotation_Angle+2*360)%360;
				target_Curve->plot_Options_Calculated.rotation_Angle   = (target_Curve->plot_Options_Calculated.rotation_Angle  +2*360)%360;

				target_Curve->rotate_Data_From_Previous_State(left);
				target_Curve->fill_Measurement_And_Curve_With_Shifted_2D_Data();
				plot_Data_2D();
//				global_Multilayer_Approach->calculate(true);
			}
		});

		QToolButton* rotate_Right_Button = new QToolButton;
			rotate_Right_Button->setIcon(QIcon(rotate_Right_Pic));
			rotate_Right_Button->setToolTip("Rotate 90"+Degree_Sym+" right");
		rotate_Button_Layout->addWidget(rotate_Right_Button);
		connect(rotate_Right_Button, &QToolButton::clicked, this, [=]
		{
			if(target_Curve->loaded_And_Ready)
			{
				target_Curve->plot_Options_Experimental.rotation_Angle -= 90;
				target_Curve->plot_Options_Calculated.rotation_Angle -= 90;
				target_Curve->plot_Options_Experimental.rotation_Angle = (target_Curve->plot_Options_Experimental.rotation_Angle+2*360)%360;
				target_Curve->plot_Options_Calculated.rotation_Angle   = (target_Curve->plot_Options_Calculated.rotation_Angle  +2*360)%360;

				target_Curve->rotate_Data_From_Previous_State(right);
				target_Curve->fill_Measurement_And_Curve_With_Shifted_2D_Data();
				plot_Data_2D();
//				global_Multilayer_Approach->calculate(true);
			}
		});
	}
	//  interpolation
	{
		QCheckBox* use_Interpolation_CheckBox = new QCheckBox("Interpolate");
			use_Interpolation_CheckBox->setChecked(target_Curve->plot_Options_Experimental.use_Interpolation);
		plot_Options_GroupBox_Layout->addWidget(use_Interpolation_CheckBox);
		connect(use_Interpolation_CheckBox, &QCheckBox::clicked, this, [=]
		{
			target_Curve->plot_Options_Experimental.use_Interpolation = use_Interpolation_CheckBox->isChecked();
			target_Curve->plot_Options_Calculated.use_Interpolation = use_Interpolation_CheckBox->isChecked();
			plot_Data_2D();
		});
        }
        // z range to show
        {
                QHBoxLayout* z_Range_Layout = new QHBoxLayout;
                    z_Range_Layout->setAlignment(Qt::AlignLeft);
                plot_Options_GroupBox_Layout->addLayout(z_Range_Layout);

                QLabel* orders_Label = new QLabel("Range to show, orders: ");
                z_Range_Layout->addWidget(orders_Label);

                MyDoubleSpinBox* orders_Spinbox = new MyDoubleSpinBox(nullptr, false);
                    orders_Spinbox->setRange(1,99);
                    orders_Spinbox->setDecimals(1);
                    orders_Spinbox->setSingleStep(0.1);
                    orders_Spinbox->setValue(target_Curve->plot_Options_Experimental.orders_To_Show);
                    orders_Spinbox->setAccelerated(true);
                    orders_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
                z_Range_Layout->addWidget(orders_Spinbox);
                connect(orders_Spinbox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
                {
                    target_Curve->plot_Options_Experimental.orders_To_Show = orders_Spinbox->value();
                    target_Curve->plot_Options_Calculated.orders_To_Show = orders_Spinbox->value();
                    plot_Data_2D();
                });
        }
	plot_Options_GroupBox->adjustSize();
	plot_Options_GroupBox->setFixedHeight(plot_Options_GroupBox->height());
}

// ---------------------------------------------------

void Target_Curve_Plot::create_Subinterval_Rectangle_1D()
{
	// put it on top
	custom_Plot->setCurrentLayer("overlay");

	start_Rect = new QCPItemRect(custom_Plot);
			start_Rect->setPen  (subinterval_Plot_Pen);
			start_Rect->setBrush(subinterval_Plot_Brush);
	end_Rect = new QCPItemRect(custom_Plot);
			end_Rect->setPen  (subinterval_Plot_Pen);
			end_Rect->setBrush(subinterval_Plot_Brush);

	custom_Plot->setCurrentLayer("main");

	subinterval_Changed_Replot_1D();

	connect(custom_Plot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Target_Curve_Plot::subinterval_Changed_Replot_1D);
	connect(custom_Plot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Target_Curve_Plot::subinterval_Changed_Replot_1D);
}

void Target_Curve_Plot::create_Subinterval_Rectangle_2D()
{
	// put it on top
	custom_Plot->setCurrentLayer("overlay");

	start_Rect = new QCPItemRect(custom_Plot);
			start_Rect->setPen  (subinterval_Plot_Pen);
			start_Rect->setBrush(subinterval_Plot_Brush_2D);
	end_Rect = new QCPItemRect(custom_Plot);
			end_Rect->setPen  (subinterval_Plot_Pen);
			end_Rect->setBrush(subinterval_Plot_Brush_2D);
	top_Rect = new QCPItemRect(custom_Plot);
			top_Rect->setPen  (subinterval_Plot_Pen);
			top_Rect->setBrush(subinterval_Plot_Brush_2D);
	bottom_Rect = new QCPItemRect(custom_Plot);
			bottom_Rect->setPen  (subinterval_Plot_Pen);
			bottom_Rect->setBrush(subinterval_Plot_Brush_2D);

	custom_Plot->setCurrentLayer("main");

	subinterval_Changed_Replot_2D();

	connect(custom_Plot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Target_Curve_Plot::subinterval_Changed_Replot_2D);
	connect(custom_Plot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Target_Curve_Plot::subinterval_Changed_Replot_2D);
}

void Target_Curve_Plot::subinterval_Changed_Replot_1D()
{
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

	// show/hide
	start_Rect->setVisible(target_Curve->curve.use_Subinterval);
	end_Rect->setVisible(target_Curve->curve.use_Subinterval && !target_Curve->curve.outer_Area);

	custom_Plot->replot();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_1D.contains(optical_Graphs_1D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.contains(target_Curve->measurement.id))
		{
			Curve_Plot_1D* curve_Plot_1D = global_Multilayer_Approach->optical_Graphs_1D->meas_Id_Curve_1D_Map.value(target_Curve->measurement.id);
			curve_Plot_1D->subinterval_Changed_Replot();
		}
	}
}

void Target_Curve_Plot::subinterval_Changed_Replot_2D()
{
	double coeff = angle_Coefficients_Map.value(target_Curve->angular_Units);

	if(target_Curve->curve.outer_Area)
	{
		start_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Left/coeff, target_Curve->curve.subinterval_Top/coeff);
		start_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Right/coeff,target_Curve->curve.subinterval_Bottom/coeff);
	} else
	{
		start_Rect->topLeft->setCoords(custom_Plot->xAxis->range().lower, custom_Plot->yAxis->range().upper);
		start_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Left/coeff,custom_Plot->yAxis->range().lower);

		end_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Right/coeff,custom_Plot->yAxis->range().upper);
		end_Rect->bottomRight->setCoords(custom_Plot->xAxis->range().upper, custom_Plot->yAxis->range().lower);

		top_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Left/coeff, custom_Plot->yAxis->range().upper);
		top_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Right/coeff,target_Curve->curve.subinterval_Top/coeff);

		bottom_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Left/coeff, target_Curve->curve.subinterval_Bottom/coeff);
		bottom_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Right/coeff, custom_Plot->yAxis->range().lower);
	}

	// show/hide
	start_Rect->setVisible(target_Curve->curve.use_Subinterval);
	end_Rect->setVisible(target_Curve->curve.use_Subinterval && !target_Curve->curve.outer_Area);
	top_Rect->setVisible(target_Curve->curve.use_Subinterval && !target_Curve->curve.outer_Area);
	bottom_Rect->setVisible(target_Curve->curve.use_Subinterval && !target_Curve->curve.outer_Area);

	custom_Plot->replot();

	// curve plots
	if(global_Multilayer_Approach->runned_Optical_Graphs_2D.contains(optical_Graphs_2D_Key))
	{
		if(global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.contains(target_Curve->measurement.id))
		{
			Curve_Plot_2D* curve_Plot_2D = global_Multilayer_Approach->optical_Graphs_2D->meas_Id_Curve_2D_Map.value(target_Curve->measurement.id);
			curve_Plot_2D->subinterval_Changed_Replot();
		}
	}
}

void Target_Curve_Plot::plot_Data()
{
	if( target_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		target_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		plot_Data_1D();
	}
	if( target_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		plot_Data_2D();
	}
}
