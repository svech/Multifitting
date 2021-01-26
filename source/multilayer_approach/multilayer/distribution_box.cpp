#include "distribution_box.h"
#include "multilayer_approach/multilayer_approach.h"
class Multilayer_Approach;

Distribution_Box::Distribution_Box(QString measurement_Type, Distribution& distribution, QString pre_Name, QString symbolic_Name, MyDoubleSpinBox* related_SpinBox, QString angular_Units, bool totally_Forbid_Sampling, QString phi, QWidget *parent) :
	distribution(distribution),
	pre_Name(pre_Name),
	symbolic_Name(symbolic_Name),
	related_SpinBox(related_SpinBox),
	angular_Units(angular_Units),
	totally_Forbid_Sampling(totally_Forbid_Sampling),
	measurement_Type(measurement_Type),
	phi(phi),
	QWidget(parent)
{
	create_Main_Layout();
}

void Distribution_Box::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);

	create_Plot();
	create_Box();
	connecting();
}

void Distribution_Box::create_Plot()
{
	distribution_Plot = new QCustomPlot;
		distribution_Plot->setMinimumHeight(270);
	main_Layout->addWidget(distribution_Plot);

	// title
	QCPTextElement* plot_Title = new QCPTextElement(distribution_Plot, symbolic_Name, QFont("Times", 12, QFont::DemiBold));
	distribution_Plot->plotLayout()->insertRow(0);
	distribution_Plot->plotLayout()->setRowSpacing(0);
	distribution_Plot->plotLayout()->addElement(0,0,plot_Title);

	distribution_Plot->setNoAntialiasingOnDrag(false);
	distribution_Plot->clearGraphs();
	distribution_Plot->addGraph();

	// frame, axes
	QPen pen = distribution_Plot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
	distribution_Plot->yAxis->grid()->setSubGridVisible(true);
	distribution_Plot->xAxis->grid()->setSubGridVisible(true);
	distribution_Plot->yAxis->grid()->setPen(pen);
	distribution_Plot->xAxis->grid()->setPen(pen);

	// curve
	distribution_Plot->graph()->setPen(QPen(QColor(0, 0, 255),1.5));

	// bars
	bars = new QCPBars(distribution_Plot->xAxis, distribution_Plot->yAxis);
		bars->setWidthType(QCPBars::wtAbsolute);
		bars->setWidth(8);
		bars->setPen(QPen(QColor(0, 0, 255),1));
		bars->setBrush(QBrush(QColor(0, 255, 0, 100)));

	// make top right axes clones of bottom left axes:
	distribution_Plot->xAxis2->setVisible(true);
	distribution_Plot->yAxis2->setVisible(true);
	distribution_Plot->xAxis2->setTickLabels(false);

	// ticks
	QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
		linTicker->setTickCount(4);
	distribution_Plot->xAxis->setTicker(linTicker);

	// make range draggable and zoomable:
	distribution_Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	// connect signals so top and right axes move in sync with bottom and left axes:
	connect(distribution_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), distribution_Plot->xAxis2, SLOT(setRange(QCPRange)));
	connect(distribution_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), distribution_Plot->yAxis2, SLOT(setRange(QCPRange)));

	replot();
}

void Distribution_Box::create_Box()
{
	groupbox = new QGroupBox;
	QGridLayout* groupbox_Layout = new QGridLayout(groupbox);
		groupbox_Layout->setSizeConstraint(QLayout::SetFixedSize);

	main_Layout->addWidget(groupbox,0,Qt::AlignHCenter);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	resolution_Label = new QLabel(pre_Name+symbolic_Name);
	groupbox_Layout->addWidget(resolution_Label,0,0,Qt::AlignLeft);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	resolution_SpinBox = new MyDoubleSpinBox(this,false);
		resolution_SpinBox->setAccelerated(true);				
		if(angle_Units_List.contains(angular_Units))
		{
			double coeff = angle_Coefficients_Map.value(angular_Units);
			resolution_SpinBox->setRange(0, 90/coeff);
		} else
		{
			resolution_SpinBox->setRange(0, MAX_DOUBLE);
		}
		resolution_SpinBox->setDecimals(7);
		resolution_SpinBox->setValue(set_From_Value(distribution.FWHM_distribution));
		resolution_SpinBox->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
		resolution_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		resolution_SpinBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
	groupbox_Layout->addWidget(resolution_SpinBox,0,1,Qt::AlignLeft);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	QLabel* units_Label = new QLabel;
	if(angle_Units_List.contains(angular_Units)) {units_Label->setText(angular_Units);}
	groupbox_Layout->addWidget(units_Label,0,2,Qt::AlignLeft);


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	distribution_Label = new QLabel("Distribution");
	groupbox_Layout->addWidget(distribution_Label,1,0,Qt::AlignLeft);

	distribution_ComboBox = new QComboBox;
		distribution_ComboBox->addItems(distributions);
		distribution_ComboBox->setCurrentText(distribution.distribution_Function);
		distribution_ComboBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
		if(phi == "phi") distribution_ComboBox->setDisabled(true);
	groupbox_Layout->addWidget(distribution_ComboBox,1,1,Qt::AlignLeft);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	sampling_Checkbox = new QCheckBox("Use sampling");
		sampling_Checkbox->setChecked(distribution.use_Sampling);
	groupbox_Layout->addWidget(sampling_Checkbox,2,0,Qt::AlignLeft);	
	sampling_Checkbox->setDisabled(totally_Forbid_Sampling);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	coverage_Label = new QLabel("Coverage (in units of FWHM)");
	groupbox_Layout->addWidget(coverage_Label,3,0,Qt::AlignLeft);
	coverage_Label->setDisabled(totally_Forbid_Sampling || !distribution.use_Sampling);

	coverage_SpinBox = new MyDoubleSpinBox(this,false);
		coverage_SpinBox->setAccelerated(true);
		coverage_SpinBox->setRange(0.1, 50);
		coverage_SpinBox->setDecimals(3);
		coverage_SpinBox->setValue(distribution.coverage);
		coverage_SpinBox->setSingleStep(0.1);
		coverage_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		coverage_SpinBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
	groupbox_Layout->addWidget(coverage_SpinBox,3,1,Qt::AlignLeft);
	coverage_SpinBox->setDisabled(totally_Forbid_Sampling || !distribution.use_Sampling);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	samples_Label = new QLabel("Number of samples");
	groupbox_Layout->addWidget(samples_Label,4,0,Qt::AlignLeft);
	samples_Label->setDisabled(totally_Forbid_Sampling || !distribution.use_Sampling);

	sample_SpinBox = new QSpinBox;
		sample_SpinBox->setAccelerated(true);
		sample_SpinBox->setRange(1, MAX_INTEGER);
		sample_SpinBox->setValue(distribution.number_of_Samples);
		sample_SpinBox->setSingleStep(2);
		sample_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		sample_SpinBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
	groupbox_Layout->addWidget(sample_SpinBox,4,1,Qt::AlignLeft);
	sample_SpinBox->setDisabled(totally_Forbid_Sampling || !distribution.use_Sampling);
}

double Distribution_Box::set_From_Value(double real_input)
{
	if(angle_Units_List.contains(angular_Units))
	{
		double coeff = angle_Coefficients_Map.value(angular_Units);
		return real_input/coeff;
	} else
	{
		return real_input;
	}
}

double Distribution_Box::save_To_Value(double nominal_input)
{
	if(angle_Units_List.contains(angular_Units))
	{
		double coeff = angle_Coefficients_Map.value(angular_Units);
		return nominal_input*coeff;
	} else
	{
		return nominal_input;
	}
}

void Distribution_Box::replot()
{
	int data_Count = 301;
	double FWHM = distribution.FWHM_distribution;
	double limit = distribution.coverage*FWHM;
	double delta = (2*limit)/(data_Count-1);

	// shape
	QVector<QCPGraphData> data_To_Plot(data_Count);
	for (int i=0; i<data_Count; ++i)
	{
		double x = -limit + delta*i;
		data_To_Plot[i].key   = x;
		data_To_Plot[i].value = Global_Variables::distribution_Function(distribution.distribution_Function, FWHM, x);
	}
	distribution_Plot->graph()->data()->set(data_To_Plot);

	if(!totally_Forbid_Sampling && distribution.use_Sampling)
	{
		// bars
		QVector<double> x_Bars(distribution.number_of_Samples);
		QVector<double> y_Bars(distribution.number_of_Samples);
		if(distribution.number_of_Samples>1 && FWHM>DBL_EPSILON)
		{
			Global_Variables::distribution_Sampling(distribution, x_Bars, y_Bars);
		}
		else
		{
			x_Bars[0] = 0;
			y_Bars[0] = 1;
		}
		bars->setData(x_Bars, y_Bars);
		bars->setVisible(true);
	} else
	{
		bars->setVisible(false);
	}

	if(FWHM > DBL_EPSILON)	{distribution_Plot->xAxis->setRange(-limit,limit);}
	else					{distribution_Plot->xAxis->setRange(-1E-7,1E-7);}
	distribution_Plot->yAxis->setRange(0,1.05);
	distribution_Plot->replot();
}

void Distribution_Box::connecting()
{
	// distribution FWHM
	connect(resolution_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		distribution.FWHM_distribution = save_To_Value(resolution_SpinBox->value());

		related_SpinBox->blockSignals(true);
		related_SpinBox->setValue(resolution_SpinBox->value());
		related_SpinBox->blockSignals(false);

		replot();
		global_Multilayer_Approach->global_Recalculate();
	});
	// distribution function
	connect(distribution_ComboBox, &QComboBox::currentTextChanged, this, [=]
	{
		distribution.distribution_Function = distribution_ComboBox->currentText();

		replot();
		global_Multilayer_Approach->global_Recalculate();
	});

	// use sampling
	connect(sampling_Checkbox, &QCheckBox::toggled, this, [=]
	{
		distribution.use_Sampling = sampling_Checkbox->isChecked();

		coverage_Label  ->setDisabled(totally_Forbid_Sampling || !distribution.use_Sampling);
		coverage_SpinBox->setDisabled(totally_Forbid_Sampling || !distribution.use_Sampling);
		samples_Label   ->setDisabled(totally_Forbid_Sampling || !distribution.use_Sampling);
		sample_SpinBox  ->setDisabled(totally_Forbid_Sampling || !distribution.use_Sampling);

		replot();
		global_Multilayer_Approach->global_Recalculate();
	});

	// sampling
	connect(sample_SpinBox,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=]
	{
		distribution.number_of_Samples = sample_SpinBox->value();

		replot();
		global_Multilayer_Approach->global_Recalculate();
	});
	// coverage
	connect(coverage_SpinBox,  static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
	{
		distribution.coverage = coverage_SpinBox->value();

		replot();
		global_Multilayer_Approach->global_Recalculate();
	});
}
