#include "curve_plot_2d.h"

Curve_Plot_2D::Curve_Plot_2D(Multilayer* multilayer, Target_Curve* target_Curve, Independent_Curve* independent_Curve, QString curve_Class, QWidget* parent) :
	multilayer(multilayer),
	curve_Class(curve_Class),
	target_Curve(target_Curve),
	independent_Curve(independent_Curve),

	measurement			(curve_Class == INDEPENDENT ? independent_Curve->measurement	   : target_Curve->measurement				),
	calculated_Values   (curve_Class == INDEPENDENT ? independent_Curve->calculated_Values : target_Curve->calculated_Values		),
	plot_Options		(curve_Class == INDEPENDENT ? independent_Curve->plot_Options	   : target_Curve->plot_Options_Experimental),
	spectral_Units		(curve_Class == INDEPENDENT ? independent_Curve->spectral_Units	   : target_Curve->spectral_Units			),
	angular_Units		(curve_Class == INDEPENDENT ? independent_Curve->angular_Units	   : target_Curve->angular_Units			),
	plot_Indicator		(curve_Class == INDEPENDENT ? independent_Curve->tab_Name		   : target_Curve->index					),

	QWidget(parent)
{
	create_Main_Layout();	
}

void Curve_Plot_2D::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(4,4,4,0);

	//------------------------------------------------------
	/// create splitters
	//------------------------------------------------------
	bottom_Splitter = new MySplitter;
		bottom_Splitter->setOrientation(Qt::Vertical);
		bottom_Splitter->setChildrenCollapsible(false);
		bottom_Splitter->setHandleWidth(0);
		bottom_Splitter->setStyleSheet("QSplitter::handle{background-color: gray;}");
	main_Layout->addWidget(bottom_Splitter);
	//------------------------------------------------------
	left_Splitter = new MySplitter;
		left_Splitter->setOrientation(Qt::Horizontal);
		left_Splitter->setChildrenCollapsible(false);
		left_Splitter->setHandleWidth(1);
		left_Splitter->setStyleSheet("QSplitter::handle{background-color: gray; }");
	bottom_Splitter->addWidget(left_Splitter);
	//------------------------------------------------------
	bottom_Left_Splitter = new MySplitter;
		bottom_Left_Splitter->setOrientation(Qt::Horizontal);
		bottom_Left_Splitter->setChildrenCollapsible(false);
		bottom_Left_Splitter->setHandleWidth(0);
		bottom_Left_Splitter->setStyleSheet("QSplitter::handle{background-color: gray;}");
	bottom_Splitter->addWidget(bottom_Left_Splitter);

	connect(left_Splitter, &MySplitter::splitterMoved, this, [=](int pos, int index)
	{
		left_Vertical_Custom_Plot->setMaximumWidth(16777215);
		empty_Widget->setMaximumWidth(16777215);

		bottom_Left_Splitter->blockSignals(true);
		bottom_Left_Splitter->moveSplitter(pos,index);
		bottom_Left_Splitter->blockSignals(false);
	});
	connect(bottom_Left_Splitter, &MySplitter::splitterMoved, this, [=](int pos, int index)
	{
		left_Vertical_Custom_Plot->setMaximumWidth(16777215);
		empty_Widget->setMaximumWidth(16777215);

		left_Splitter->blockSignals(true);
		left_Splitter->moveSplitter(pos,index);
		left_Splitter->blockSignals(false);
	});
	connect(bottom_Splitter, &MySplitter::splitterMoved, this, [=](int pos, int index)
	{
		Q_UNUSED(pos) Q_UNUSED(index)
		bottom_Section_Tabs->setMaximumHeight(16777215);
	});
	//------------------------------------------------------
	/// fulill splitters
	//------------------------------------------------------
	left_Vertical_Custom_Plot = new QCustomPlot;
		left_Vertical_Custom_Plot->setHidden(!plot_Options.left_Section_Plot);
		left_Vertical_Custom_Plot->setMaximumWidth(100);
		left_Vertical_Custom_Plot->setMinimumWidth(100);
	left_Splitter->addWidget(left_Vertical_Custom_Plot);

	main_2D_Custom_Plot = new QCustomPlot;
	left_Splitter->addWidget(main_2D_Custom_Plot);

	empty_Widget = new QWidget;
		empty_Widget->setHidden(!plot_Options.left_Section_Plot || !plot_Options.bottom_Section_Plot);
		empty_Widget->setMinimumWidth(100);
		empty_Widget->setMaximumWidth(100);
	bottom_Left_Splitter->addWidget(empty_Widget);

	bottom_Section_Tabs = new QTabWidget;
		bottom_Section_Tabs->setTabsClosable(false);
		bottom_Section_Tabs->setTabPosition(QTabWidget::South);
		bottom_Section_Tabs->setMovable(false);
		bottom_Section_Tabs->setHidden(!plot_Options.bottom_Section_Plot);
		bottom_Section_Tabs->setMinimumHeight(120);
		bottom_Section_Tabs->setMaximumHeight(121);
	bottom_Left_Splitter->addWidget(bottom_Section_Tabs);

	//------------------------------------------------------

	color_Map = new QCPColorMap(main_2D_Custom_Plot->xAxis, main_2D_Custom_Plot->yAxis);
	color_Scale = new QCPColorScale(main_2D_Custom_Plot);
	margin_Group_Top_Bottom = new QCPMarginGroup(main_2D_Custom_Plot);
	margin_Group_Left_Right = new QCPMarginGroup(main_2D_Custom_Plot);
	main_2D_Custom_Plot->plotLayout()->addElement(0,1,color_Scale);

	bottom_Horizontal_Custom_Plot = new QCustomPlot(main_2D_Custom_Plot);
	QWidget* bottom_Hor_Widget = new QWidget;
	bottom_Hor_Layout = new QHBoxLayout(bottom_Hor_Widget);
		bottom_Hor_Layout->setContentsMargins(0,0,shift,0);
		bottom_Hor_Layout->addWidget(bottom_Horizontal_Custom_Plot);
	bottom_Section_Tabs->addTab(bottom_Hor_Widget, "Horizontal");

	bottom_Vertical_Custom_Plot = new QCustomPlot;
	QWidget* bottom_Ver_Widget = new QWidget;
	bottom_Ver_Layout = new QHBoxLayout(bottom_Ver_Widget);
		bottom_Ver_Layout->setContentsMargins(0,0,shift,0);
		bottom_Ver_Layout->addWidget(bottom_Vertical_Custom_Plot);
	bottom_Section_Tabs->addTab(bottom_Ver_Widget, "Vertical");


	if(curve_Class == TARGET) create_Subinterval_Rectangle();

	create_Plot_Frame_And_Scale();
	create_Left_Part();
	create_Bottom_Part();
	plot_All_Data();
	refresh_Labels();
	create_Plot_Options_GroupBox();

	// color scheme editor
	connect(main_2D_Custom_Plot, &QCustomPlot::axisDoubleClick, this, [=](QCPAxis* axis, QCPAxis::SelectablePart part, QMouseEvent* event)
	{
		Q_UNUSED(part)	Q_UNUSED(event)
		if(axis == color_Scale->axis()) { Global_Variables::color_Scheme_Change(color_Map, main_2D_Custom_Plot, &plot_Options.color_Scheme); }
	}, Qt::UniqueConnection);
}

void Curve_Plot_2D::create_Plot_Frame_And_Scale()
{
	main_2D_Custom_Plot->setNoAntialiasingOnDrag(false);
	main_2D_Custom_Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	// frame
	main_2D_Custom_Plot->axisRect()->setupFullAxesBox(true);
//	main_2D_Custom_Plot->yAxis2->setTickLabels(true);
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		linTicker->setTickCount(12);
	main_2D_Custom_Plot->xAxis ->setTicker(linTicker);
	main_2D_Custom_Plot->xAxis2->setTicker(linTicker);
	main_2D_Custom_Plot->yAxis ->setTicker(linTicker);
	main_2D_Custom_Plot->yAxis2->setTicker(linTicker);

	// set grid style
	QPen pen = main_2D_Custom_Plot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
	main_2D_Custom_Plot->yAxis->grid()->setSubGridVisible(true);
	main_2D_Custom_Plot->xAxis->grid()->setSubGridVisible(true);
	main_2D_Custom_Plot->yAxis->grid()->setPen(pen);
	main_2D_Custom_Plot->xAxis->grid()->setPen(pen);

	// add a color scale:
	color_Scale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
	color_Map->setColorScale(color_Scale); // associate the color map with the color scale
	color_Map->setTightBoundary(true);
	color_Map->setGradient(plot_Options.color_Scheme); // set the color gradient of the color map to one of the presets

	// margin group
	main_2D_Custom_Plot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, margin_Group_Top_Bottom);
	color_Scale->setMarginGroup(QCP::msBottom | QCP::msTop, margin_Group_Top_Bottom);
	left_Vertical_Custom_Plot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, margin_Group_Top_Bottom);

	main_2D_Custom_Plot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, margin_Group_Left_Right);
	bottom_Horizontal_Custom_Plot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, margin_Group_Left_Right);
	bottom_Vertical_Custom_Plot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, margin_Group_Left_Right);

	// scale
	if(plot_Options.z_Scale == log_Scale) apply_Log_Scale();
	if(plot_Options.z_Scale == lin_Scale) apply_Lin_Scale();
}

void Curve_Plot_2D::create_Left_Part()
{

}

void Curve_Plot_2D::create_Bottom_Part()
{

}

void Curve_Plot_2D::refresh_Axes_Range()
{
	// TODO
	if(plot_Options.rescale)
	{
		double coeff = angle_Coefficients_Map.value(angular_Units); // ?????????? field joule
		color_Map->data()->setRange(QCPRange(measurement.detector_Theta_Angle.independent.min/coeff, measurement.detector_Theta_Angle.independent.max/coeff),
									QCPRange(measurement.detector_Phi_Angle.independent.min/coeff,   measurement.detector_Phi_Angle.independent.max/coeff));
		main_2D_Custom_Plot->rescaleAxes();
	}
}

void Curve_Plot_2D::apply_Log_Scale()
{
	QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
	color_Scale->axis()->setScaleType(QCPAxis::stLogarithmic);
	color_Scale->axis()->setTicker(logTicker);
	color_Scale->axis()->setNumberFormat("eb");
	color_Scale->axis()->setNumberPrecision(0);

	// crutch for aligning sections with map
	shift = 55;
	bottom_Hor_Layout->setContentsMargins(0,0,shift,0);
	bottom_Ver_Layout->setContentsMargins(0,0,shift,0);
}

void Curve_Plot_2D::apply_Lin_Scale()
{
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
	color_Scale->axis()->setScaleType(QCPAxis::stLinear);
	color_Scale->axis()->setTicker(linTicker);
	color_Scale->axis()->setNumberFormat("gbc");
	color_Scale->axis()->setNumberPrecision(2);

	// crutch for aligning sections with map
	shift = 55;
	bottom_Hor_Layout->setContentsMargins(0,0,shift,0);
	bottom_Ver_Layout->setContentsMargins(0,0,shift,0);
}

void Curve_Plot_2D::plot_All_Data()
{
	refresh_Labels();

	// GISAS
	if(curve_Class == TARGET)
	{
		values_2D_Meas = &target_Curve->curve.value_2D_Shifted;
		values_2D_Calc = &calculated_Values.GISAS_Instrumental;

		if(plot_Options.data_To_Show == meas) values_2D = values_2D_Meas;
		if(plot_Options.data_To_Show == calc) values_2D = values_2D_Calc;

		// TODO
		if(values_2D_Calc->size() == 0)
		{
			qInfo() << "Curve_Plot_2D   Target curve " << plot_Indicator << " : calculation of " << target_Curve->curve.value_Type << "is not done. Fake data are shown." << endl;
			values_2D_Calc->resize(values_2D_Meas->size(), vector<double>(values_2D_Meas->front().size()));
			for(size_t i=0; i<values_2D_Calc->size(); i++)
			{
				for(size_t j=0; j<values_2D_Calc->front().size(); j++)
				{
					(*values_2D_Calc)[i][j] = cos((i-500.)/100.)*cos((j-500.)/100.);
				}
			}
		}
	}
	// GISAS or field or joule
	if(curve_Class == INDEPENDENT)
	{
		if(	independent_Curve->calc_Functions.check_Field )	values_2D = &calculated_Values.field_Intensity;
		if(	independent_Curve->calc_Functions.check_Joule )	values_2D = &calculated_Values.absorption_Map;
		if(	independent_Curve->calc_Functions.check_GISAS )	values_2D = &calculated_Values.GISAS_Instrumental;

		// TODO
		if(values_2D->size() == 0)
		{
			values_2D->resize(1000, vector<double>(1000));
			qInfo() << "Curve_Plot_2D   Independent curve " << plot_Indicator << " : calculation is not done. Fake data are shown." << endl;
			for(size_t i=0; i<values_2D->size(); i++)
			{
				for(size_t j=0; j<values_2D->front().size(); j++)
				{
					(*values_2D)[i][j] = cos((i-500.)/100.)*cos((j-500.)/100.);
				}
			}
		}
	}

	plot_Data();
}

void Curve_Plot_2D::plot_Data()
{
	int nx = values_2D->front().size();
	int ny = values_2D->size();
	color_Map->data()->setSize(nx, ny);

	double min_Val = DBL_MAX;
	double max_Val = -DBL_MAX;
	for (int x_Index=0; x_Index<nx; ++x_Index)
	{
		for (int y_Index=0; y_Index<ny; ++y_Index)
		{
			double val = (*values_2D)[y_Index][x_Index];
			color_Map->data()->setCell(x_Index, y_Index, val);

			if(max_Val<val && (plot_Options.z_Scale == lin_Scale || val > DBL_MIN)) {max_Val = val;}
			if(min_Val>val && (plot_Options.z_Scale == lin_Scale || val > DBL_MIN)) {min_Val = val;}
		}
	}
	// x,y ranges
	refresh_Axes_Range();
	// z range
//	color_Map->rescaleDataRange(); // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient
	if(plot_Options.z_Scale == log_Scale) { min_Val = max(min_Val,max_Val/1e5); } // no more than 5 orders
	if(plot_Options.rescale) { color_Map->setDataRange(QCPRange(min_Val,max_Val)); }

	color_Map->setInterpolate(plot_Options.use_Interpolation);
	main_2D_Custom_Plot->replot();
}

void Curve_Plot_2D::refresh_Labels()
{
	// TODO
	main_2D_Custom_Plot->xAxis->setLabel(argument_Type_Text + ", " + argument_Units_Text);
	main_2D_Custom_Plot->yAxis->setLabel(value_Type_Text + ", " + argument_Units_Text);
	main_2D_Custom_Plot->replot();
}

void Curve_Plot_2D::create_Plot_Options_GroupBox()
{
	QGroupBox* plot_Options_GroupBox = new QGroupBox;
	main_Layout->addWidget(plot_Options_GroupBox);

	QHBoxLayout* plot_Options_GroupBox_Layout = new QHBoxLayout(plot_Options_GroupBox);
	plot_Options_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	// scale radiobuttons
	{
		QHBoxLayout* radio_Button_Layout = new QHBoxLayout;
			radio_Button_Layout->setAlignment(Qt::AlignLeft);
		plot_Options_GroupBox_Layout->addLayout(radio_Button_Layout);

		QLabel* scale_Label = new QLabel("Scale: ");
		radio_Button_Layout->addWidget(scale_Label);

		QRadioButton* lin_Radio_Button = new QRadioButton("Lin");
			lin_Radio_Button->setChecked(plot_Options.z_Scale == lin_Scale);
		radio_Button_Layout->addWidget(lin_Radio_Button);
		connect(lin_Radio_Button, &QRadioButton::clicked, this, [=]
		{
			if(lin_Radio_Button->isChecked())	plot_Options.z_Scale = lin_Scale;

			apply_Lin_Scale();
			plot_Data();
			apply_Lin_Scale();
			main_2D_Custom_Plot->replot();
		});

		QRadioButton* log_Radio_Button = new QRadioButton("Log");
			log_Radio_Button->setChecked(plot_Options.z_Scale == log_Scale);
		radio_Button_Layout->addWidget(log_Radio_Button);
		connect(log_Radio_Button, &QRadioButton::clicked, this, [=]
		{
			if(log_Radio_Button->isChecked())	plot_Options.z_Scale = log_Scale;

			apply_Log_Scale();
			plot_Data();
			apply_Log_Scale();
			main_2D_Custom_Plot->replot();
		});

		QButtonGroup* Z_ButtonGroup = new QButtonGroup;
			Z_ButtonGroup->addButton(lin_Radio_Button);
			Z_ButtonGroup->addButton(log_Radio_Button);
	}
	// rescale
	{
		QCheckBox* rescale_Check_Box = new QCheckBox("Rescale  |");
			rescale_Check_Box->setChecked(plot_Options.rescale);
		plot_Options_GroupBox_Layout->addWidget(rescale_Check_Box);
		connect(rescale_Check_Box, &QCheckBox::toggled, this, [=]{ plot_Options.rescale = rescale_Check_Box->isChecked(); });
	}
	// left panel
	{
		QCheckBox* show_Left_Panel_CheckBox = new QCheckBox("Left panel");
			show_Left_Panel_CheckBox->setChecked(plot_Options.left_Section_Plot);
		plot_Options_GroupBox_Layout->addWidget(show_Left_Panel_CheckBox);
		connect(show_Left_Panel_CheckBox, &QCheckBox::clicked, this, [=]
		{
			plot_Options.left_Section_Plot = show_Left_Panel_CheckBox->isChecked();

			left_Vertical_Custom_Plot->setHidden(!plot_Options.left_Section_Plot);
			empty_Widget->setHidden(!plot_Options.left_Section_Plot || !plot_Options.bottom_Section_Plot);
		});
	}
	// bottom panel
	{
		QCheckBox* show_Bottom_Panel_CheckBox = new QCheckBox("Bottom panel  |");
			show_Bottom_Panel_CheckBox->setChecked(plot_Options.bottom_Section_Plot);
		plot_Options_GroupBox_Layout->addWidget(show_Bottom_Panel_CheckBox);
		connect(show_Bottom_Panel_CheckBox, &QCheckBox::clicked, this, [=]
		{
			plot_Options.bottom_Section_Plot = show_Bottom_Panel_CheckBox->isChecked();

			bottom_Section_Tabs->setHidden(!plot_Options.bottom_Section_Plot);
			empty_Widget->setHidden(!plot_Options.left_Section_Plot || !plot_Options.bottom_Section_Plot);
		});
	}
	// data to show
	if(curve_Class == TARGET)
	{
		QHBoxLayout* radio_Button_Layout = new QHBoxLayout;
			radio_Button_Layout->setAlignment(Qt::AlignLeft);
		plot_Options_GroupBox_Layout->addLayout(radio_Button_Layout);

		QRadioButton* meas_Button = new QRadioButton("meas");
		QRadioButton* calc_Button = new QRadioButton("calc");
			meas_Button->setChecked(plot_Options.data_To_Show == meas);
			calc_Button->setChecked(plot_Options.data_To_Show == calc);
		radio_Button_Layout->addWidget(meas_Button);
		radio_Button_Layout->addWidget(calc_Button);

		QButtonGroup* data_ButtonGroup = new QButtonGroup;
			data_ButtonGroup->addButton(meas_Button);
			data_ButtonGroup->addButton(calc_Button);

		connect(meas_Button, &QCheckBox::clicked, this, [=]
		{
			if(meas_Button->isChecked()) plot_Options.data_To_Show = meas;
			// TODO
			qInfo() << "plot_Options.data_To_Show = meas" << endl;
//			someting plot_Data();
		});
		connect(calc_Button, &QCheckBox::clicked, this, [=]
		{
			if(calc_Button->isChecked()) plot_Options.data_To_Show = calc;
			// TODO
			qInfo() << "plot_Options.data_To_Show = calc" << endl;
//			someting plot_Data();
		});
	}
	// interpolation
	if(multilayer->graph_Options_2D.show_Interpolation)
	{
		QCheckBox* use_Interpolation_CheckBox = new QCheckBox("Interpolate");
			use_Interpolation_CheckBox->setChecked(plot_Options.use_Interpolation);
		plot_Options_GroupBox_Layout->addWidget(use_Interpolation_CheckBox);
		connect(use_Interpolation_CheckBox, &QCheckBox::clicked, this, [=]
		{
			plot_Options.use_Interpolation = use_Interpolation_CheckBox->isChecked();
			// TODO
			qInfo() << "plot_Options.use_Interpolation" << endl;
//			plot_Data();
		});
	}
	plot_Options_GroupBox->adjustSize();
	plot_Options_GroupBox->setFixedHeight(plot_Options_GroupBox->height());
}

void Curve_Plot_2D::create_Subinterval_Rectangle()
{
	if(curve_Class == TARGET)
	{
		// put main_2D_Custom_Plot on top
		main_2D_Custom_Plot->setCurrentLayer("overlay");

		start_Rect = new QCPItemRect(main_2D_Custom_Plot);
				start_Rect->setPen  (subinterval_Plot_Pen);
				start_Rect->setBrush(subinterval_Plot_Brush_2D);
		end_Rect = new QCPItemRect(main_2D_Custom_Plot);
				end_Rect->setPen  (subinterval_Plot_Pen);
				end_Rect->setBrush(subinterval_Plot_Brush_2D);
		top_Rect = new QCPItemRect(main_2D_Custom_Plot);
				top_Rect->setPen  (subinterval_Plot_Pen);
				top_Rect->setBrush(subinterval_Plot_Brush_2D);
		bottom_Rect = new QCPItemRect(main_2D_Custom_Plot);
				bottom_Rect->setPen  (subinterval_Plot_Pen);
				bottom_Rect->setBrush(subinterval_Plot_Brush_2D);

		main_2D_Custom_Plot->setCurrentLayer("main");

		subinterval_Changed_Replot();

		connect(main_2D_Custom_Plot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Curve_Plot_2D::subinterval_Changed_Replot);
		connect(main_2D_Custom_Plot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), this, &Curve_Plot_2D::subinterval_Changed_Replot);
	}
}

void Curve_Plot_2D::subinterval_Changed_Replot()
{
	if(curve_Class == TARGET)
	{
		double coeff = angle_Coefficients_Map.value(angular_Units);

		start_Rect->topLeft->setCoords(main_2D_Custom_Plot->xAxis->range().lower, main_2D_Custom_Plot->yAxis->range().upper);
		start_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Left/coeff,main_2D_Custom_Plot->yAxis->range().lower);

		end_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Right/coeff,main_2D_Custom_Plot->yAxis->range().upper);
		end_Rect->bottomRight->setCoords(main_2D_Custom_Plot->xAxis->range().upper, main_2D_Custom_Plot->yAxis->range().lower);

		top_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Left/coeff, main_2D_Custom_Plot->yAxis->range().upper);
		top_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Right/coeff,target_Curve->curve.subinterval_Top/coeff);

		bottom_Rect->topLeft->setCoords(target_Curve->curve.subinterval_Left/coeff, target_Curve->curve.subinterval_Bottom/coeff);
		bottom_Rect->bottomRight->setCoords(target_Curve->curve.subinterval_Right/coeff, main_2D_Custom_Plot->yAxis->range().lower);

		// show/hide
		start_Rect->setVisible(target_Curve->curve.use_Subinterval);
		end_Rect->setVisible(target_Curve->curve.use_Subinterval);
		top_Rect->setVisible(target_Curve->curve.use_Subinterval);
		bottom_Rect->setVisible(target_Curve->curve.use_Subinterval);

		main_2D_Custom_Plot->replot();
	}
}
