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
	{
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
			left_Ver_Widget->setMaximumWidth(16777215);
			corner_Widget->setMaximumWidth(16777215);

			bottom_Left_Splitter->blockSignals(true);
			bottom_Left_Splitter->moveSplitter(pos,index);
			bottom_Left_Splitter->blockSignals(false);
		});
		connect(bottom_Left_Splitter, &MySplitter::splitterMoved, this, [=](int pos, int index)
		{
			left_Ver_Widget->setMaximumWidth(16777215);
			corner_Widget->setMaximumWidth(16777215);

			left_Splitter->blockSignals(true);
			left_Splitter->moveSplitter(pos,index);
			left_Splitter->blockSignals(false);
		});
		connect(bottom_Splitter, &MySplitter::splitterMoved, this, [=](int pos, int index)
		{
			Q_UNUSED(pos) Q_UNUSED(index)
			bottom_Section_Tabs->setMaximumHeight(16777215);
		});
	}
	//------------------------------------------------------
	/// fulill splitters
	//------------------------------------------------------
	{
		left_Vertical_Custom_Plot = new QCustomPlot;
		left_Ver_Widget = new QWidget;
			left_Ver_Widget->setHidden(!plot_Options.left_Section_Plot);
			left_Ver_Widget->setMaximumWidth(130);
			left_Ver_Widget->setMinimumWidth(130);
		left_Ver_Layout = new QHBoxLayout(left_Ver_Widget);
			left_Ver_Layout->setContentsMargins(0,shift_Left,0,0);
			left_Ver_Layout->addWidget(left_Vertical_Custom_Plot);
		left_Splitter->addWidget(left_Ver_Widget);

		main_2D_Custom_Plot = new QCustomPlot;
		left_Splitter->addWidget(main_2D_Custom_Plot);

		corner_Widget = new QWidget;
			corner_Widget->setHidden(!plot_Options.left_Section_Plot || !plot_Options.bottom_Section_Plot);
			corner_Widget->setMinimumWidth(130);
			corner_Widget->setMaximumWidth(130);
		bottom_Left_Splitter->addWidget(corner_Widget);

		bottom_Section_Tabs = new QTabWidget;
			bottom_Section_Tabs->setTabsClosable(false);
			bottom_Section_Tabs->setTabPosition(QTabWidget::South);
			bottom_Section_Tabs->setMovable(false);
			bottom_Section_Tabs->setHidden(!plot_Options.bottom_Section_Plot);
			bottom_Section_Tabs->setMinimumHeight(150);
			bottom_Section_Tabs->setMaximumHeight(150);
		bottom_Left_Splitter->addWidget(bottom_Section_Tabs);
	}
	//------------------------------------------------------
	/// create plots
	//------------------------------------------------------
	{
		color_Map = new QCPColorMap(main_2D_Custom_Plot->xAxis, main_2D_Custom_Plot->yAxis);
		color_Scale = new QCPColorScale(main_2D_Custom_Plot);
		margin_Group_Top_Bottom = new QCPMarginGroup(main_2D_Custom_Plot);
		margin_Group_Left_Right = new QCPMarginGroup(main_2D_Custom_Plot);
		main_2D_Custom_Plot->plotLayout()->addElement(0,1,color_Scale);
		main_2D_Custom_Plot->plotLayout()->setColumnSpacing(-5);

		bottom_Horizontal_Custom_Plot = new QCustomPlot(main_2D_Custom_Plot);
		QWidget* bottom_Hor_Widget = new QWidget;
		bottom_Hor_Layout = new QHBoxLayout(bottom_Hor_Widget);
			bottom_Hor_Layout->setContentsMargins(0,0,shift_Bottom,0);
			bottom_Hor_Layout->addWidget(bottom_Horizontal_Custom_Plot);
		bottom_Section_Tabs->addTab(bottom_Hor_Widget, "Horizontal");

		bottom_Vertical_Custom_Plot = new QCustomPlot;
		QWidget* bottom_Ver_Widget = new QWidget;
		bottom_Ver_Layout = new QHBoxLayout(bottom_Ver_Widget);
			bottom_Ver_Layout->setContentsMargins(0,0,shift_Bottom,0);
			bottom_Ver_Layout->addWidget(bottom_Vertical_Custom_Plot);
		bottom_Section_Tabs->addTab(bottom_Ver_Widget, "Vertical");

		connect(main_2D_Custom_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), bottom_Horizontal_Custom_Plot->xAxis, SLOT(setRange(QCPRange)));
		connect(main_2D_Custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), bottom_Vertical_Custom_Plot->xAxis, SLOT(setRange(QCPRange)));
		connect(main_2D_Custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), left_Vertical_Custom_Plot->yAxis, SLOT(setRange(QCPRange)));

		connect(color_Scale->axis(), SIGNAL(rangeChanged(QCPRange)), bottom_Horizontal_Custom_Plot->yAxis, SLOT(setRange(QCPRange)));
		connect(color_Scale->axis(), SIGNAL(rangeChanged(QCPRange)), bottom_Vertical_Custom_Plot->yAxis, SLOT(setRange(QCPRange)));
		connect(color_Scale->axis(), SIGNAL(rangeChanged(QCPRange)), left_Vertical_Custom_Plot->xAxis, SLOT(setRange(QCPRange)));
	}
	//------------------------------------------------------
	/// create lines
	//------------------------------------------------------
	{
		main_2D_Custom_Plot->setCurrentLayer("overlay");
		hor_Line_Current = new QCPItemStraightLine(main_2D_Custom_Plot);
		ver_Line_Current = new QCPItemStraightLine(main_2D_Custom_Plot);
		hor_Line_Fixed = new QCPItemStraightLine(main_2D_Custom_Plot);
		ver_Line_Fixed = new QCPItemStraightLine(main_2D_Custom_Plot);
		main_2D_Custom_Plot->setCurrentLayer("main");
		create_Position_Lines();
	}
	//------------------------------------------------------
	/// create current position labels
	//------------------------------------------------------
	{
		cur_Arg_X = new QLabel("cur_Arg_X");
		cur_Arg_Y = new QLabel("cur_Arg_Y");
		cur_Bin_X = new QLabel("cur_Bin_X");
		cur_Bin_Y = new QLabel("cur_Bin_Y");
		cur_Val = new QLabel("cur_Val");

		QVBoxLayout* corner_Layout = new QVBoxLayout(corner_Widget);
			corner_Layout->addWidget(cur_Arg_X);
			corner_Layout->addWidget(cur_Arg_Y);
			corner_Layout->addWidget(cur_Bin_X);
			corner_Layout->addWidget(cur_Bin_Y);
			corner_Layout->addWidget(cur_Val);
	}

	if(curve_Class == TARGET) create_Subinterval_Rectangle();

	if(multilayer->graph_Options_2D.show_Title)
	{
		plot_Title = new QCPTextElement(main_2D_Custom_Plot,"text_Data",QFont("Times", 10, QFont::DemiBold));
		main_2D_Custom_Plot->plotLayout()->insertRow(0);
		main_2D_Custom_Plot->plotLayout()->setRowSpacing(0);
		main_2D_Custom_Plot->plotLayout()->addElement(0,0,plot_Title);
		main_2D_Custom_Plot->plotLayout()->setRowSpacing(-12);

		// crutch for aligning
		shift_Left = 10;
		left_Ver_Layout->setContentsMargins(0,shift_Left,0,0);
	}

	create_Section_Parts();
	create_Plot_Frame_And_Scale();
	plot_All_Data();
	refresh_Axes_Labels();
	refresh_Corner_Labels();
	create_Plot_Options_GroupBox();
	touch_It();

	// color scheme editor
	connect(main_2D_Custom_Plot, &QCustomPlot::axisDoubleClick, this, [=](QCPAxis* axis, QCPAxis::SelectablePart part, QMouseEvent* event)
	{
		Q_UNUSED(part)	Q_UNUSED(event)
		if(axis == color_Scale->axis()){Global_Variables::color_Scheme_Change(color_Map, main_2D_Custom_Plot, &plot_Options.color_Scheme);}
	}, Qt::UniqueConnection);
}

void Curve_Plot_2D::create_Position_Lines()
{
	// current
	{
		hor_Line_Current->point1->setCoords(-MAX_DOUBLE, 0);
		hor_Line_Current->point2->setCoords(-MAX_DOUBLE, 1);
		ver_Line_Current->point1->setCoords(-MAX_DOUBLE, 0);
		ver_Line_Current->point2->setCoords(-MAX_DOUBLE, 1);

		QPen pen(Qt::black, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
		hor_Line_Current->setPen(pen);
		ver_Line_Current->setPen(pen);
	}

	// fixed
	{
		hor_Line_Fixed->point1->setCoords(-MAX_DOUBLE, 0);
		hor_Line_Fixed->point2->setCoords(-MAX_DOUBLE, 1);
		ver_Line_Fixed->point1->setCoords(-MAX_DOUBLE, 0);
		ver_Line_Fixed->point2->setCoords(-MAX_DOUBLE, 1);

		QPen pen(Qt::darkRed, 2, Qt::DashDotLine);
		hor_Line_Fixed->setPen(pen);
		ver_Line_Fixed->setPen(pen);
	}
}

void Curve_Plot_2D::create_Plot_Frame_And_Scale()
{
	main_2D_Custom_Plot->setNoAntialiasingOnDrag(false);
	main_2D_Custom_Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	// frame
	main_2D_Custom_Plot->axisRect()->setupFullAxesBox(true);
	main_2D_Custom_Plot->yAxis2->setTickLabels(true);
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

	set_Title_Text();
}

void Curve_Plot_2D::create_Section_Parts()
{
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		linTicker->setTickCount(12);

	// add graphs
	for(int i=0; i<4; i++)
	{
		left_Vertical_Custom_Plot->addGraph(left_Vertical_Custom_Plot->yAxis,left_Vertical_Custom_Plot->xAxis);
		bottom_Horizontal_Custom_Plot->addGraph();
		bottom_Vertical_Custom_Plot->addGraph();
	}
	// left part
	{
		left_Vertical_Custom_Plot->axisRect()->setupFullAxesBox(true);
		left_Vertical_Custom_Plot->xAxis->setRangeReversed(true);
		left_Vertical_Custom_Plot->xAxis->setRange(color_Scale->axis()->range());

		left_Vertical_Custom_Plot->xAxis->setTickLabels(false);
		left_Vertical_Custom_Plot->xAxis2->setTickLabels(false);

		left_Vertical_Custom_Plot->yAxis ->setTicker(linTicker);
		left_Vertical_Custom_Plot->yAxis2->setTicker(linTicker);

		QPen pen = left_Vertical_Custom_Plot->yAxis->grid()->pen();
			pen.setStyle(Qt::DashLine);
		left_Vertical_Custom_Plot->yAxis->grid()->setSubGridVisible(true);
		left_Vertical_Custom_Plot->xAxis->grid()->setSubGridVisible(true);
		left_Vertical_Custom_Plot->yAxis->grid()->setPen(pen);
		left_Vertical_Custom_Plot->xAxis->grid()->setPen(pen);

		if(curve_Class == TARGET)
		{
			// current
			left_Vertical_Custom_Plot->graph(0)->setPen(QPen(color_Meas, thickness_Current, Qt::SolidLine));
			left_Vertical_Custom_Plot->graph(1)->setPen(QPen(color_Calc, thickness_Current, Qt::SolidLine));

			// fixed
			left_Vertical_Custom_Plot->graph(2)->setPen(QPen(color_Meas, thickness_Fixed, Qt::DotLine));
			left_Vertical_Custom_Plot->graph(3)->setPen(QPen(color_Calc, thickness_Fixed, Qt::DotLine));
		}
		if(curve_Class == INDEPENDENT)
		{
			// current
			left_Vertical_Custom_Plot->graph(0)->setPen(QPen(color_Calc, thickness_Current, Qt::SolidLine));

			// fixed
			left_Vertical_Custom_Plot->graph(1)->setPen(QPen(color_Calc, thickness_Fixed, Qt::DotLine));
		}
	}
	// bottom part
	{
		bottom_Horizontal_Custom_Plot->axisRect()->setupFullAxesBox(true);
		bottom_Horizontal_Custom_Plot->yAxis->setRange(color_Scale->axis()->range());
		bottom_Horizontal_Custom_Plot->yAxis->setTickLabels(true);
		bottom_Horizontal_Custom_Plot->yAxis2->setTickLabels(true);

		bottom_Horizontal_Custom_Plot->xAxis ->setTicker(linTicker);
		bottom_Horizontal_Custom_Plot->xAxis2->setTicker(linTicker);

		QPen pen = bottom_Horizontal_Custom_Plot->yAxis->grid()->pen();
			pen.setStyle(Qt::DashLine);
		bottom_Horizontal_Custom_Plot->yAxis->grid()->setSubGridVisible(true);
		bottom_Horizontal_Custom_Plot->xAxis->grid()->setSubGridVisible(true);
		bottom_Horizontal_Custom_Plot->yAxis->grid()->setPen(pen);
		bottom_Horizontal_Custom_Plot->xAxis->grid()->setPen(pen);

		bottom_Vertical_Custom_Plot->axisRect()->setupFullAxesBox(true);
		bottom_Vertical_Custom_Plot->yAxis->setRange(color_Scale->axis()->range());
		bottom_Vertical_Custom_Plot->yAxis->setTickLabels(true);
		bottom_Vertical_Custom_Plot->yAxis2->setTickLabels(true);

		bottom_Vertical_Custom_Plot->xAxis ->setTicker(linTicker);
		bottom_Vertical_Custom_Plot->xAxis2->setTicker(linTicker);

		pen = bottom_Vertical_Custom_Plot->yAxis->grid()->pen();
			pen.setStyle(Qt::DashLine);
		bottom_Vertical_Custom_Plot->yAxis->grid()->setSubGridVisible(true);
		bottom_Vertical_Custom_Plot->xAxis->grid()->setSubGridVisible(true);
		bottom_Vertical_Custom_Plot->yAxis->grid()->setPen(pen);
		bottom_Vertical_Custom_Plot->xAxis->grid()->setPen(pen);

		if(curve_Class == TARGET)
		{
			// current
			bottom_Horizontal_Custom_Plot->graph(0)->setPen(QPen(color_Meas, thickness_Current, Qt::SolidLine));
			bottom_Horizontal_Custom_Plot->graph(1)->setPen(QPen(color_Calc, thickness_Current, Qt::SolidLine));
			// fixed
			bottom_Horizontal_Custom_Plot->graph(2)->setPen(QPen(color_Meas, thickness_Fixed, Qt::DotLine));
			bottom_Horizontal_Custom_Plot->graph(3)->setPen(QPen(color_Calc, thickness_Fixed, Qt::DotLine));

			// current
			bottom_Vertical_Custom_Plot->graph(0)->setPen(QPen(color_Meas, thickness_Current, Qt::SolidLine));
			bottom_Vertical_Custom_Plot->graph(1)->setPen(QPen(color_Calc, thickness_Current, Qt::SolidLine));
			// fixed
			bottom_Vertical_Custom_Plot->graph(2)->setPen(QPen(color_Meas, thickness_Fixed, Qt::DotLine));
			bottom_Vertical_Custom_Plot->graph(3)->setPen(QPen(color_Calc, thickness_Fixed, Qt::DotLine));
		}
		if(curve_Class == INDEPENDENT)
		{
			// current
			bottom_Horizontal_Custom_Plot->graph(0)->setPen(QPen(color_Calc, thickness_Current, Qt::SolidLine));
			// fixed
			bottom_Horizontal_Custom_Plot->graph(1)->setPen(QPen(color_Calc, thickness_Fixed, Qt::DotLine));
			// current
			bottom_Vertical_Custom_Plot->graph(0)->setPen(QPen(color_Calc, thickness_Current, Qt::SolidLine));
			// fixed
			bottom_Vertical_Custom_Plot->graph(1)->setPen(QPen(color_Calc, thickness_Fixed, Qt::DotLine));
		}
	}
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

	// left
	left_Vertical_Custom_Plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
	left_Vertical_Custom_Plot->xAxis->setTicker(logTicker);
	left_Vertical_Custom_Plot->xAxis->setNumberFormat("eb");
	left_Vertical_Custom_Plot->xAxis->setNumberPrecision(0);

	left_Vertical_Custom_Plot->xAxis2->setScaleType(QCPAxis::stLogarithmic);
	left_Vertical_Custom_Plot->xAxis2->setTicker(logTicker);
	left_Vertical_Custom_Plot->xAxis2->setNumberFormat("eb");
	left_Vertical_Custom_Plot->xAxis2->setNumberPrecision(0);

	// bottom hor
	bottom_Horizontal_Custom_Plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
	bottom_Horizontal_Custom_Plot->yAxis->setTicker(logTicker);
	bottom_Horizontal_Custom_Plot->yAxis->setNumberFormat("eb");
	bottom_Horizontal_Custom_Plot->yAxis->setNumberPrecision(0);

	bottom_Horizontal_Custom_Plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
	bottom_Horizontal_Custom_Plot->yAxis2->setTicker(logTicker);
	bottom_Horizontal_Custom_Plot->yAxis2->setNumberFormat("eb");
	bottom_Horizontal_Custom_Plot->yAxis2->setNumberPrecision(0);

	// bottom ver
	bottom_Vertical_Custom_Plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
	bottom_Vertical_Custom_Plot->yAxis->setTicker(logTicker);
	bottom_Vertical_Custom_Plot->yAxis->setNumberFormat("eb");
	bottom_Vertical_Custom_Plot->yAxis->setNumberPrecision(0);

	bottom_Vertical_Custom_Plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
	bottom_Vertical_Custom_Plot->yAxis2->setTicker(logTicker);
	bottom_Vertical_Custom_Plot->yAxis2->setNumberFormat("eb");
	bottom_Vertical_Custom_Plot->yAxis2->setNumberPrecision(0);

	replot_All();
	// crutch for aligning sections with map
	shift_Bottom = 45;
	bottom_Hor_Layout->setContentsMargins(0,0,shift_Bottom,0);
	bottom_Ver_Layout->setContentsMargins(0,0,shift_Bottom,0);
}

void Curve_Plot_2D::apply_Lin_Scale()
{
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
	color_Scale->axis()->setScaleType(QCPAxis::stLinear);
	color_Scale->axis()->setTicker(linTicker);
	color_Scale->axis()->setNumberFormat("gbc");
	color_Scale->axis()->setNumberPrecision(2);

	// left
	left_Vertical_Custom_Plot->xAxis->setScaleType(QCPAxis::stLinear);
	left_Vertical_Custom_Plot->xAxis->setTicker(linTicker);
	left_Vertical_Custom_Plot->xAxis->setNumberFormat("gbc");
	left_Vertical_Custom_Plot->xAxis->setNumberPrecision(2);

	left_Vertical_Custom_Plot->xAxis2->setScaleType(QCPAxis::stLinear);
	left_Vertical_Custom_Plot->xAxis2->setTicker(linTicker);
	left_Vertical_Custom_Plot->xAxis2->setNumberFormat("gbc");
	left_Vertical_Custom_Plot->xAxis2->setNumberPrecision(2);

	// bottom hor
	bottom_Horizontal_Custom_Plot->yAxis->setScaleType(QCPAxis::stLinear);
	bottom_Horizontal_Custom_Plot->yAxis->setTicker(linTicker);
	bottom_Horizontal_Custom_Plot->yAxis->setNumberFormat("gbc");
	bottom_Horizontal_Custom_Plot->yAxis->setNumberPrecision(2);

	bottom_Horizontal_Custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
	bottom_Horizontal_Custom_Plot->yAxis2->setTicker(linTicker);
	bottom_Horizontal_Custom_Plot->yAxis2->setNumberFormat("gbc");
	bottom_Horizontal_Custom_Plot->yAxis2->setNumberPrecision(2);

	// bottom ver
	bottom_Vertical_Custom_Plot->yAxis->setScaleType(QCPAxis::stLinear);
	bottom_Vertical_Custom_Plot->yAxis->setTicker(linTicker);
	bottom_Vertical_Custom_Plot->yAxis->setNumberFormat("gbc");
	bottom_Vertical_Custom_Plot->yAxis->setNumberPrecision(2);

	bottom_Vertical_Custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
	bottom_Vertical_Custom_Plot->yAxis2->setTicker(linTicker);
	bottom_Vertical_Custom_Plot->yAxis2->setNumberFormat("gbc");
	bottom_Vertical_Custom_Plot->yAxis2->setNumberPrecision(2);

	replot_All();
	// crutch for aligning sections with map
	shift_Bottom = 45;
	bottom_Hor_Layout->setContentsMargins(0,0,shift_Bottom,0);
	bottom_Ver_Layout->setContentsMargins(0,0,shift_Bottom,0);
}

void Curve_Plot_2D::plot_All_Data()
{
	refresh_Axes_Labels();

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

		values_Hor_Meas.resize(values_2D_Meas->front().size());
		values_Hor_Calc.resize(values_2D_Calc->front().size());
		values_Ver_Meas.resize(values_2D_Meas->size());
		values_Ver_Calc.resize(values_2D_Calc->size());
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

		values_Hor_Calc.resize(values_2D->front().size());
		values_Ver_Calc.resize(values_2D->size());
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
	replot_All();
}

void Curve_Plot_2D::refresh_Axes_Labels()
{
	if(	measurement.measurement_Type == measurement_Types[GISAS_Map] ) // both independent and target
	{
		value_Type_Text = argument_Types[Detector_Azimuthal_Angle];
		argument_Type_Text = argument_Types[Detector_Polar_Angle];
		argument_Sym_Text = "";
		argument_Units_Text = angular_Units;

		main_2D_Custom_Plot->xAxis->setLabel(argument_Type_Text + ", " + argument_Units_Text);
		main_2D_Custom_Plot->yAxis->setLabel(value_Type_Text + ", " + argument_Units_Text);
	}

	// fields
	if(curve_Class == INDEPENDENT)
	{
		if(independent_Curve->calc_Functions.check_Field || independent_Curve->calc_Functions.check_Joule)
		{
			value_Type_Text = "Depth, " + Angstrom_Sym;
			if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
			{
				if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
				{
					argument_Type_Text = argument_Types[Beam_Grazing_Angle];
					argument_Sym_Text = "";
					argument_Units_Text = angular_Units;
				}
				if(measurement.argument_Type == argument_Types[Wavelength_Energy])
				{
					if(	spectral_Units == wavelength_Units_List[angstrom] ||
						spectral_Units == wavelength_Units_List[nm]		  )
					{
						argument_Type_Text = QString(argument_Types[Wavelength_Energy]).split("/").first();
						argument_Sym_Text = " "+Lambda_Sym;
						argument_Units_Text = spectral_Units;
					} else
					{
						argument_Type_Text = QString(argument_Types[Wavelength_Energy]).split("/").last();
						argument_Sym_Text = " E";
						argument_Units_Text = spectral_Units;
					}
				}
			}
			main_2D_Custom_Plot->xAxis->setLabel(argument_Type_Text + ", " + argument_Units_Text);
			main_2D_Custom_Plot->yAxis->setLabel(value_Type_Text );
		}
	}
	replot_All();
}

void Curve_Plot_2D::refresh_Corner_Labels(double x, double y, int x_Cell, int y_Cell)
{
	QString base_Arg_X; QString add_Arg_X;
	QString base_Arg_Y;	QString add_Arg_Y;
	QString base_Bin_X;	QString add_Bin_X;
	QString base_Bin_Y;	QString add_Bin_Y;
	QString base_Val;	QString add_Val;

	base_Bin_X = "bin_x:    ";
	base_Bin_Y = "bin_y:    ";
	base_Val   = "Value:    ";

	if(x_Cell>=0 && y_Cell>=0)
	{
		add_Bin_X = QString::number(x_Cell+1);
		add_Bin_Y = QString::number(y_Cell+1);
		add_Val = QString::number(color_Map->data()->data(x,y), 'g', 3);
	}

	if(	measurement.measurement_Type == measurement_Types[GISAS_Map] ) // both independent and target
	{
		base_Arg_X = Theta_Sym + ":          ";
		base_Arg_Y = Phi_Sym   + ":          ";

		if(x_Cell>=0 && y_Cell>=0)
		{
			add_Arg_X = QString::number(x, 'g', 3) + " " + angular_Units;
			add_Arg_Y = QString::number(y, 'g', 3) + " " + angular_Units;
		}
	}

	// fields
	if(curve_Class == INDEPENDENT)
	{
		if(independent_Curve->calc_Functions.check_Field || independent_Curve->calc_Functions.check_Joule)
		{
			if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
			{
				base_Arg_Y = "Depth:  ";
				if(x_Cell>=0 && y_Cell>=0)
				{
					add_Arg_Y = QString::number(y, 'g', 3) + " " + Angstrom_Sym;
				}

				if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
				{
					base_Arg_X = Theta_Sym + Zero_Subscript_Sym + ":        ";
					if(x_Cell>=0 && y_Cell>=0)
					{
						add_Arg_X = QString::number(x, 'g', 3) + " " + angular_Units;
					}
				}
				if(measurement.argument_Type == argument_Types[Wavelength_Energy])
				{
					base_Arg_X = Global_Variables::wavelength_Energy_Symbol(spectral_Units) + ":          ";
					if(x_Cell>=0 && y_Cell>=0)
					{
						add_Arg_X = QString::number(x, 'g', 3) + " " + spectral_Units;
					}
				}
			}
		}
	}

	if(x_Cell>=0 && y_Cell>=0)
	{
		cur_Arg_X->setText(base_Arg_X+add_Arg_X);
		cur_Arg_Y->setText(base_Arg_Y+add_Arg_Y);
		cur_Bin_X->setText(base_Bin_X+add_Bin_X);
		cur_Bin_Y->setText(base_Bin_Y+add_Bin_Y);
		cur_Val  ->setText(base_Val  +add_Val);
	} else
	{
		cur_Arg_X->setText(base_Arg_X);
		cur_Arg_Y->setText(base_Arg_Y);
		cur_Bin_X->setText(base_Bin_X);
		cur_Bin_Y->setText(base_Bin_Y);
		cur_Val  ->setText(base_Val);
	}
}

void Curve_Plot_2D::set_Title_Text()
{
	if(multilayer->graph_Options_2D.show_Title)
	if(plot_Title)
	{
		QString lambda_Energy = Global_Variables::wavelength_Energy_Symbol(spectral_Units);
		double spectral_Coeff = wavelength_Coefficients_Map.value(spectral_Units);
		double angular_Coeff = angle_Coefficients_Map.value(angular_Units);

		QString plot_Title_Text;
		if(	measurement.measurement_Type == measurement_Types[GISAS_Map] ) // both independent and target
		{
			plot_Title_Text = plot_Indicator + ": GISAS at " +
							  Theta_Sym + Zero_Subscript_Sym + "=" + Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, line_edit_double_format, line_edit_angle_precision) + " " + angular_Units +
							  ", " + lambda_Energy + "=" + Locale.toString(Global_Variables::wavelength_Energy(spectral_Units,measurement.wavelength.value)/spectral_Coeff, line_edit_double_format, line_edit_wavelength_precision) + " " + spectral_Units +
							  ", pol=" + QString::number(measurement.polarization,'f',3);

		}
		if(curve_Class == INDEPENDENT)
		{
			if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
			{
				QString add_Text;
				if(	independent_Curve->calc_Functions.check_Field )	add_Text = ": field intensity at ";
				if(	independent_Curve->calc_Functions.check_Joule ) add_Text = ": photoabsorption at ";

				if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
				{
					plot_Title_Text = plot_Indicator + add_Text +
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
		}
		plot_Title->setText(plot_Title_Text);
	}
}

void Curve_Plot_2D::touch_It()
{
	main_2D_Custom_Plot->mouseMove(new QMouseEvent(QEvent::MouseMove, QPointF(-MAX_DOUBLE, -MAX_DOUBLE), Qt::RightButton, Qt::RightButton, Qt::NoModifier));
}

void Curve_Plot_2D::replot_All()
{
	main_2D_Custom_Plot->replot();
	left_Vertical_Custom_Plot->replot();
	bottom_Horizontal_Custom_Plot->replot();
	bottom_Vertical_Custom_Plot->replot();
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

			plot_Data();
			apply_Lin_Scale();
		});
		connect(lin_Radio_Button, &QRadioButton::toggled, lin_Radio_Button, &QRadioButton::clicked);
		connect(lin_Radio_Button, &QRadioButton::released, this, [=]{lin_Radio_Button->clicked();});

		QRadioButton* log_Radio_Button = new QRadioButton("Log");
			log_Radio_Button->setChecked(plot_Options.z_Scale == log_Scale);
		radio_Button_Layout->addWidget(log_Radio_Button);
		connect(log_Radio_Button, &QRadioButton::clicked, this, [=]
		{
			if(log_Radio_Button->isChecked())	plot_Options.z_Scale = log_Scale;

			plot_Data();
			apply_Log_Scale();
		});
		connect(log_Radio_Button, &QRadioButton::toggled, log_Radio_Button, &QRadioButton::clicked);
		connect(log_Radio_Button, &QRadioButton::released, this, [=]{log_Radio_Button->clicked();});

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

			left_Ver_Widget->setHidden(!plot_Options.left_Section_Plot);
			corner_Widget->setHidden(!plot_Options.left_Section_Plot || !plot_Options.bottom_Section_Plot);
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
			corner_Widget->setHidden(!plot_Options.left_Section_Plot || !plot_Options.bottom_Section_Plot);
		});
	}
	// data to show
	if(curve_Class == TARGET)
	{
		QHBoxLayout* radio_Button_Layout = new QHBoxLayout;
			radio_Button_Layout->setAlignment(Qt::AlignLeft);
		plot_Options_GroupBox_Layout->addLayout(radio_Button_Layout);

		QRadioButton* meas_Button = new QRadioButton("meas");
		QRadioButton* calc_Button = new QRadioButton("calc  |");
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
			plot_All_Data();
		});
		connect(calc_Button, &QCheckBox::clicked, this, [=]
		{
			if(calc_Button->isChecked()) plot_Options.data_To_Show = calc;
			// TODO
			qInfo() << "plot_Options.data_To_Show = calc" << endl;
			plot_All_Data();
		});
	}
	// interpolation
	{
		QCheckBox* use_Interpolation_CheckBox = new QCheckBox("Interpolate");
			use_Interpolation_CheckBox->setChecked(plot_Options.use_Interpolation);
		plot_Options_GroupBox_Layout->addWidget(use_Interpolation_CheckBox);
		connect(use_Interpolation_CheckBox, &QCheckBox::clicked, this, [=]
		{
			plot_Options.use_Interpolation = use_Interpolation_CheckBox->isChecked();
			plot_Data();
		});
	}
	// mark on click
	{
		connect(main_2D_Custom_Plot, &QCustomPlot::itemClick, this, [=](QCPAbstractItem* item, QMouseEvent* event)
		{
			Q_UNUSED(item)

			double x = main_2D_Custom_Plot->xAxis->pixelToCoord(event->pos().x());
			double y = main_2D_Custom_Plot->yAxis->pixelToCoord(event->pos().y());

			color_Map->data()->coordToCell(x,y,&x_Cell_Fix,&y_Cell_Fix);

			bool mark = true;
			mark = mark && x_Cell_Fix>=0;
			mark = mark && y_Cell_Fix>=0;
			mark = mark && x_Cell_Fix<=int(values_2D->front().size())-1;
			mark = mark && y_Cell_Fix<=int(values_2D->size())-1;

			if(mark)
			{
				if(event->button() & Qt::RightButton)
				{
					// remove lines
					hor_Line_Fixed->point1->setCoords(-MAX_DOUBLE, 0);
					hor_Line_Fixed->point2->setCoords(-MAX_DOUBLE, 1);
					ver_Line_Fixed->point1->setCoords(-MAX_DOUBLE, 0);
					ver_Line_Fixed->point2->setCoords(-MAX_DOUBLE, 1);

					// remove sections
					if(curve_Class == TARGET)
					{
						bottom_Horizontal_Custom_Plot->graph(2)->data()->clear();
						bottom_Horizontal_Custom_Plot->graph(3)->data()->clear();
						bottom_Vertical_Custom_Plot->graph(2)->data()->clear();
						bottom_Vertical_Custom_Plot->graph(3)->data()->clear();
						left_Vertical_Custom_Plot->graph(2)->data()->clear();
						left_Vertical_Custom_Plot->graph(3)->data()->clear();
					}
					if(curve_Class == INDEPENDENT)
					{
						bottom_Horizontal_Custom_Plot->graph(1)->data()->clear();
						bottom_Vertical_Custom_Plot->graph(1)->data()->clear();
						left_Vertical_Custom_Plot->graph(1)->data()->clear();
					}
					replot_All();
				}
				if(event->button() & Qt::LeftButton)
				{
					// mark position
					hor_Line_Fixed->point1->setCoords(0, y);
					hor_Line_Fixed->point2->setCoords(1, y);

					ver_Line_Fixed->point1->setCoords(x, 0);
					ver_Line_Fixed->point2->setCoords(x, 1);

					double temp;
					if(curve_Class == TARGET)
					{
						// horizontal
						for (int j=0; j<values_Hor_Meas.size(); ++j)
						{
							color_Map->data()->cellToCoord(j,y_Cell_Fix,&(values_Hor_Meas[j].key),&temp);
							values_Hor_Meas[j].value = (*values_2D_Meas)[y_Cell_Fix][j];

							color_Map->data()->cellToCoord(j,y_Cell_Fix,&(values_Hor_Calc[j].key),&temp);
							values_Hor_Calc[j].value = (*values_2D_Calc)[y_Cell_Fix][j];
						}
						bottom_Horizontal_Custom_Plot->graph(2)->data()->set(values_Hor_Meas);
						bottom_Horizontal_Custom_Plot->graph(3)->data()->set(values_Hor_Calc);

						// vertical
						for (int i=0; i<values_Ver_Meas.size(); ++i)
						{
							color_Map->data()->cellToCoord(x_Cell_Fix,i,&temp,&(values_Ver_Meas[i].key));
							values_Ver_Meas[i].value = (*values_2D_Meas)[i][x_Cell_Fix];

							color_Map->data()->cellToCoord(x_Cell_Fix,i,&temp,&(values_Ver_Calc[i].key));
							values_Ver_Calc[i].value = (*values_2D_Calc)[i][x_Cell_Fix];
						}
						bottom_Vertical_Custom_Plot->graph(2)->data()->set(values_Ver_Meas);
						bottom_Vertical_Custom_Plot->graph(3)->data()->set(values_Ver_Calc);

						left_Vertical_Custom_Plot->graph(2)->data()->set(values_Ver_Meas);
						left_Vertical_Custom_Plot->graph(3)->data()->set(values_Ver_Calc);
					}
					if(curve_Class == INDEPENDENT)
					{
						// horizontal
						for (int j=0; j<values_Hor_Calc.size(); ++j)
						{
							color_Map->data()->cellToCoord(j,y_Cell_Fix,&(values_Hor_Calc[j].key),&temp);
							values_Hor_Calc[j].value = (*values_2D)[y_Cell_Fix][j];
						}
						bottom_Horizontal_Custom_Plot->graph(1)->data()->set(values_Hor_Calc);

						// vertical
						for (int i=0; i<values_Ver_Calc.size(); ++i)
						{
							color_Map->data()->cellToCoord(x_Cell_Fix,i,&temp,&(values_Ver_Calc[i].key));
							values_Ver_Calc[i].value = (*values_2D)[i][x_Cell_Fix];
						}
						bottom_Vertical_Custom_Plot->graph(1)->data()->set(values_Ver_Calc);

						left_Vertical_Custom_Plot->graph(1)->data()->set(values_Ver_Calc);
					}
					replot_All();
				}
			}
		});
	}
	// show current position with sections
	{
		QCPItemText* text_Item = new QCPItemText(main_2D_Custom_Plot);
			text_Item->position->setCoords(QPointF(-MAX_DOUBLE, -MAX_DOUBLE));
		connect(main_2D_Custom_Plot, &QCustomPlot::mouseMove, this, [=](QMouseEvent* event)
		{
			double x = main_2D_Custom_Plot->xAxis->pixelToCoord(event->pos().x());
			double y = main_2D_Custom_Plot->yAxis->pixelToCoord(event->pos().y());

			color_Map->data()->coordToCell(x,y,&x_Cell_Cur,&y_Cell_Cur);

			bool show_Lines = true;
			show_Lines = show_Lines && x<main_2D_Custom_Plot->xAxis->range().upper;
			show_Lines = show_Lines && x>main_2D_Custom_Plot->xAxis->range().lower;
			show_Lines = show_Lines && y<main_2D_Custom_Plot->yAxis->range().upper;
			show_Lines = show_Lines && y>main_2D_Custom_Plot->yAxis->range().lower;

			// mark position
			if(show_Lines)
			{
				hor_Line_Current->point1->setCoords(0, y);
				hor_Line_Current->point2->setCoords(1, y);
				ver_Line_Current->point1->setCoords(x, 0);
				ver_Line_Current->point2->setCoords(x, 1);
			} else
			{
				hor_Line_Current->point1->setCoords(-MAX_DOUBLE, 0);
				hor_Line_Current->point2->setCoords(-MAX_DOUBLE, 1);
				ver_Line_Current->point1->setCoords(-MAX_DOUBLE, 0);
				ver_Line_Current->point2->setCoords(-MAX_DOUBLE, 1);
			}

			bool show = true;
			show = show && x_Cell_Cur>=0;
			show = show && y_Cell_Cur>=0;
			show = show && x_Cell_Cur<=int(values_2D->front().size())-1;
			show = show && y_Cell_Cur<=int(values_2D->size())-1;

			if(show)
			{
				double temp;
				if(curve_Class == TARGET)
				{
					// horizontal
					for (int j=0; j<values_Hor_Meas.size(); ++j)
					{
						color_Map->data()->cellToCoord(j,y_Cell_Cur,&(values_Hor_Meas[j].key),&temp);
						values_Hor_Meas[j].value = (*values_2D_Meas)[y_Cell_Cur][j];

						color_Map->data()->cellToCoord(j,y_Cell_Cur,&(values_Hor_Calc[j].key),&temp);
						values_Hor_Calc[j].value = (*values_2D_Calc)[y_Cell_Cur][j];
					}
					bottom_Horizontal_Custom_Plot->graph(0)->data()->set(values_Hor_Meas);
					bottom_Horizontal_Custom_Plot->graph(1)->data()->set(values_Hor_Calc);

					// vertical
					for (int i=0; i<values_Ver_Meas.size(); ++i)
					{
						color_Map->data()->cellToCoord(x_Cell_Cur,i,&temp,&(values_Ver_Meas[i].key));
						values_Ver_Meas[i].value = (*values_2D_Meas)[i][x_Cell_Cur];

						color_Map->data()->cellToCoord(x_Cell_Cur,i,&temp,&(values_Ver_Calc[i].key));
						values_Ver_Calc[i].value = (*values_2D_Calc)[i][x_Cell_Cur];
					}
					bottom_Vertical_Custom_Plot->graph(0)->data()->set(values_Ver_Meas);
					bottom_Vertical_Custom_Plot->graph(1)->data()->set(values_Ver_Calc);

					left_Vertical_Custom_Plot->graph(0)->data()->set(values_Ver_Meas);
					left_Vertical_Custom_Plot->graph(1)->data()->set(values_Ver_Calc);
				}
				if(curve_Class == INDEPENDENT)
				{
					// horizontal
					for (int j=0; j<values_Hor_Calc.size(); ++j)
					{
						color_Map->data()->cellToCoord(j,y_Cell_Cur,&(values_Hor_Calc[j].key),&temp);
						values_Hor_Calc[j].value = (*values_2D)[y_Cell_Cur][j];
					}
					bottom_Horizontal_Custom_Plot->graph(0)->data()->set(values_Hor_Calc);

					// vertical
					for (int i=0; i<values_Ver_Calc.size(); ++i)
					{
						color_Map->data()->cellToCoord(x_Cell_Cur,i,&temp,&(values_Ver_Calc[i].key));
						values_Ver_Calc[i].value = (*values_2D)[i][x_Cell_Cur];
					}
					bottom_Vertical_Custom_Plot->graph(0)->data()->set(values_Ver_Calc);

					left_Vertical_Custom_Plot->graph(0)->data()->set(values_Ver_Calc);
				}
				if(multilayer->graph_Options_2D.show_Current_Coordinate)
				{
					double x_Text  = main_2D_Custom_Plot->xAxis->pixelToCoord(event->pos().x()+50);
					double y_Text  = main_2D_Custom_Plot->yAxis->pixelToCoord(event->pos().y()-17);
					double z	   = color_Map->data()->data(x,y);
//					text_Item->setText(QString("(%1, %2, %3)").arg(x).arg(y).arg(z));
					text_Item->setText(QString("(%1)").arg(z));
					text_Item->position->setCoords(QPointF(x_Text, y_Text));
					text_Item->setFont(QFont(font().family(), 10, QFont::Bold));
//					text_Item->setColor(Qt::white);
				}
				refresh_Corner_Labels(x, y, x_Cell_Cur, y_Cell_Cur);
				replot_All();
			} else
			{
				// remove text item
				text_Item->position->setCoords(QPointF(-MAX_DOUBLE, -MAX_DOUBLE));

				// remove sections
				bottom_Horizontal_Custom_Plot->graph(0)->data()->clear();
				bottom_Vertical_Custom_Plot->graph(0)->data()->clear();
				left_Vertical_Custom_Plot->graph(0)->data()->clear();

				if(curve_Class == TARGET)
				{
					bottom_Horizontal_Custom_Plot->graph(1)->data()->clear();
					bottom_Vertical_Custom_Plot->graph(1)->data()->clear();
					left_Vertical_Custom_Plot->graph(1)->data()->clear();
				}
				refresh_Corner_Labels();
				replot_All();
			}
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
