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
		plot_Options = &target_Curve->plot_Options;
	} else
	{
		QMessageBox::critical(nullptr, "Curve_Plot::Curve_Plot", "target_Curve is nullptr");
		exit(EXIT_FAILURE);
	}

	// if inndependent
	if(curve_Class == INDEPENDENT)
	if(independent_Variables) // if passed pointer!=nullptr
	{
		measurement = &independent_Variables->measurement;
		calculated_Values = &independent_Variables->calculated_Values;
		plot_Options = &independent_Variables->plot_Options;
	} else
	{
		QMessageBox::critical(nullptr, "Curve_Plot::Curve_Plot", "independent_Variables is nullptr");
		exit(EXIT_FAILURE);
	}

	create_Main_Layout();
}

void Curve_Plot::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	create_Plot_Frame_And_Scale();
		main_Layout->addWidget(custom_Plot);

	create_Options();
		main_Layout->addWidget(options_GroupBox);
}

void Curve_Plot::create_Plot_Frame_And_Scale()
{
	custom_Plot = new QCustomPlot(this);
	custom_Plot->setNoAntialiasingOnDrag(false); // more performance/responsiveness during dragging

	// frame, axes
	{
		QPen pen = custom_Plot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
		custom_Plot->yAxis->grid()->setSubGridVisible(true);
		custom_Plot->xAxis->grid()->setSubGridVisible(true);
		custom_Plot->yAxis->grid()->setPen(pen);
		custom_Plot->xAxis->grid()->setPen(pen);

		if(plot_Options->scale == lin_Scale)
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
		if(plot_Options->scale == log_Scale)
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
			if(lin_RadioButton->isChecked())	{plot_Options->scale = lin_Scale;}
//			custom_Plot->replot();
		});
		connect(lin_RadioButton, &QRadioButton::clicked, lin_RadioButton, &QRadioButton::toggled);
		if(plot_Options->scale == lin_Scale)
		{
			lin_RadioButton->setChecked(true);
			lin_RadioButton->toggled(true);
		}

		log_RadioButton = new QRadioButton("Log");
		connect(log_RadioButton, &QRadioButton::toggled, this, [=]
		{
			if(log_RadioButton->isChecked())	{plot_Options->scale = log_Scale;}
//			custom_Plot->replot();
		});
		connect(log_RadioButton, &QRadioButton::clicked, log_RadioButton, &QRadioButton::toggled);
		if(plot_Options->scale == log_Scale)
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
		thickness_Spin->setValue(plot_Options->thickness);
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
