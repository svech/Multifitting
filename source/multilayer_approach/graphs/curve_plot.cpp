#include "curve_plot.h"

Curve_Plot::Curve_Plot(Target_Curve* target_Curve, Independent_Variables* independent_Variables, QString curve_Class, QWidget* parent) :
	curve_Class(curve_Class),
	target_Curve(target_Curve),
	independent_Variables(independent_Variables),
	QWidget(parent)
{
	create_Main_Layout();
}

void Curve_Plot::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	// plot
	{
		custom_Plot = new QCustomPlot(this);
		main_Layout->addWidget(custom_Plot);

		// generate some data:
		QVector<double> x(101), y(101); // initialize with entries 0..100
		for (int i=0; i<101; ++i)
		{
		  x[i] = i/50.0 - 1; // x goes from -1 to 1
		  y[i] = x[i]*x[i];  // let's plot a quadratic function
		}
		// create graph and assign data to it:
		custom_Plot->addGraph();
		custom_Plot->graph(0)->setData(x, y);
		// give the axes some labels:
		custom_Plot->xAxis->setLabel("x");
		custom_Plot->yAxis->setLabel("y");
		// set axes ranges, so we see all data:
		custom_Plot->xAxis->setRange(-1, 1);
		custom_Plot->yAxis->setRange(0, 1);
	}

	create_Options();
		main_Layout->addWidget(options_GroupBox);
}

void Curve_Plot::create_Options()
{
	options_GroupBox = new QGroupBox;

	scale_Label = new QLabel("Scale: ");

	lin_RadioButton = new QRadioButton("Lin");
	connect(lin_RadioButton, &QRadioButton::toggled, this, [=]
	{
//		if(lin_Radio_Button->isChecked())	{target_Curve->plot_Scale = lin_Scale;}
//		target_Curve_Plot->plot_Data();
	});
	connect(lin_RadioButton, &QRadioButton::clicked, lin_RadioButton, &QRadioButton::toggled);

	log_RadioButton = new QRadioButton("Log");
	connect(log_RadioButton, &QRadioButton::toggled, this, [=]
	{
//		if(log_Radio_Button->isChecked())	{target_Curve->plot_Scale = log_Scale;}
//		target_Curve_Plot->plot_Data();
	});
	connect(log_RadioButton, &QRadioButton::clicked, log_RadioButton, &QRadioButton::toggled);

	colors_Button = new QPushButton;
	symbol_ComboBox = new QComboBox;
	thickness_Spin = new QDoubleSpinBox;
	thickness_Spin->setButtonSymbols(QAbstractSpinBox::NoButtons);

	{
		options_Layout = new QHBoxLayout(options_GroupBox);
		options_Layout->setAlignment(Qt::AlignLeft);

		options_Layout->addWidget(scale_Label);
		options_Layout->addWidget(lin_RadioButton);
		options_Layout->addWidget(log_RadioButton);

		options_Layout->addWidget(colors_Button);
		options_Layout->addWidget(symbol_ComboBox);
		options_Layout->addWidget(thickness_Spin);
	}

	options_GroupBox->adjustSize();
	options_GroupBox->setFixedHeight(options_GroupBox->height());
}
