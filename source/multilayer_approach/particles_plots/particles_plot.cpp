#include "particles_plot.h"

Particles_Plot::Particles_Plot(Multilayer* multilayer, Particles_Plots_Window* particles_Plots_Window, QWidget *parent) :
	multilayer(multilayer),
	particles_Plots_Window(particles_Plots_Window),
	QWidget(parent)
{
	create_Main_Layout();
}

void Particles_Plot::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	custom_Plot = new QCustomPlot(this);
	custom_Plot->addGraph();

	// synchronize selection of graphs with selection of corresponding legend items:
	connect(custom_Plot, &QCustomPlot::selectionChangedByUser, this, [=]
	{
		for (int i=0; i<custom_Plot->graphCount(); ++i)
		{
			QCPGraph* graph = custom_Plot->graph(i);
			if(custom_Plot->legend->hasItemWithPlottable(graph))
			{
				QCPPlottableLegendItem* item = custom_Plot->legend->itemWithPlottable(graph);
				if (item->selected() || graph->selected())
				{
					item->setSelected(true);
					graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
				}
			}
		}
	});
	custom_Plot->legend->setSelectableParts(QCPLegend::spItems);
	custom_Plot->legend->setSelectedIconBorderPen(QPen(Qt::black, 1));

	if(multilayer->particles_Plot_Options.show_Cursor_Position)
	{
		QCPItemText* text_Item = new QCPItemText(custom_Plot);
		text_Item->setText("");
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
	create_Left_Side();
	lock_Interfaces();

	main_Layout->addWidget(custom_Plot);
	plot_Data(true);
}

void Particles_Plot::create_Left_Side()
{
	// left side
	///==============================================================================================
		QVBoxLayout* left_Layout = new QVBoxLayout;
			left_Layout->setSpacing(2);
			left_Layout->setAlignment(Qt::AlignTop);
			main_Layout->addLayout(left_Layout);

		///------------------------------------------------------------------------------------------
		// top part
		///------------------------------------------------------------------------------------------

		layer_GroupBox = new QGroupBox;
			left_Layout->addWidget(layer_GroupBox);
		QVBoxLayout* layer_Layout = new QVBoxLayout(layer_GroupBox);
			layer_Layout->setSpacing(2);
			layer_Layout->setContentsMargins(8,5,6,5);

		// ----------------------------------------------------

		QHBoxLayout* A_layer_Number_layout = new QHBoxLayout;
			A_layer_Number_layout->setAlignment(Qt::AlignLeft);
		layer_Layout->addLayout(A_layer_Number_layout);

		A_show_Layer_by_Number_CheckBox = new QCheckBox("Show layer");
			A_show_Layer_by_Number_CheckBox->setChecked(multilayer->particles_Plot_Options.A_show_Layer_With_Number);
		A_layer_Number_layout->addWidget(A_show_Layer_by_Number_CheckBox);
		connect(A_show_Layer_by_Number_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->particles_Plot_Options.A_show_Layer_With_Number = A_show_Layer_by_Number_CheckBox->isChecked();
			use_A_layer = multilayer->particles_Plot_Options.A_show_Layer_With_Number;
			plot_Data(true);
		});
		A_layer_by_Number_ComboBox = new QComboBox;
			A_layer_by_Number_ComboBox->setFixedWidth(55);
			A_layer_by_Number_ComboBox->setEnabled(multilayer->particles_Plot_Options.A_show_Layer_With_Number);
		A_layer_Number_layout->addWidget(A_layer_by_Number_ComboBox,0,Qt::AlignLeft);
		connect(A_layer_by_Number_ComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]
		{
			multilayer->particles_Plot_Options.A_layer_Number_To_Show = A_layer_by_Number_ComboBox->currentIndex()+1;
			plot_Data(true);
		});

		// -------------------------------------

		QHBoxLayout* B_layer_Number_layout = new QHBoxLayout;
			B_layer_Number_layout->setAlignment(Qt::AlignLeft);
		layer_Layout->addLayout(B_layer_Number_layout);

		B_show_Layer_by_Number_CheckBox = new QCheckBox("Show layer");
			B_show_Layer_by_Number_CheckBox->setChecked(multilayer->particles_Plot_Options.B_show_Layer_With_Number);
		B_layer_Number_layout->addWidget(B_show_Layer_by_Number_CheckBox);
		connect(B_show_Layer_by_Number_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->particles_Plot_Options.B_show_Layer_With_Number = B_show_Layer_by_Number_CheckBox->isChecked();
			use_B_layer = multilayer->particles_Plot_Options.B_show_Layer_With_Number;
			plot_Data(true);
		});
		B_layer_by_Number_ComboBox = new QComboBox;
			B_layer_by_Number_ComboBox->setFixedWidth(55);
			B_layer_by_Number_ComboBox->setEnabled(multilayer->particles_Plot_Options.B_show_Layer_With_Number);
		B_layer_Number_layout->addWidget(B_layer_by_Number_ComboBox,0,Qt::AlignLeft);
		connect(B_layer_by_Number_ComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]
		{
			multilayer->particles_Plot_Options.B_layer_Number_To_Show = B_layer_by_Number_ComboBox->currentIndex()+1;
			plot_Data(true);
		});

		// ----------------------------------------------------

		layer_GroupBox->adjustSize();
		layer_GroupBox->setFixedSize(layer_GroupBox->size());

		///------------------------------------------------------------------------------------------
		// 2nd part
		///------------------------------------------------------------------------------------------

		scale_GroupBox = new QGroupBox;
			left_Layout->addWidget(scale_GroupBox);
		QVBoxLayout* scale_Layout = new QVBoxLayout(scale_GroupBox);
			scale_Layout->setSpacing(2);
		scale_Layout->setContentsMargins(8,5,2,5);

		// ----------------------------------------------------
		QHBoxLayout* x_Range_layout = new QHBoxLayout;
			x_Range_layout->setAlignment(Qt::AlignLeft);
		scale_Layout->addLayout(x_Range_layout);

		QLabel* range_Label = new QLabel(Nu_Sym + Element_Sym);
		x_Range_layout->addWidget(range_Label,0,Qt::AlignLeft);

		double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->particles_Plot_Options.local_frequency_units);
		min_X_LineEdit = new QLineEdit(Locale.toString(multilayer->particles_Plot_Options.x_Min/coeff));
			min_X_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			min_X_LineEdit->setFixedWidth(40);
		x_Range_layout->addWidget(min_X_LineEdit,0,Qt::AlignLeft);
		connect(min_X_LineEdit,  &QLineEdit::textEdited, this, [=]	{
			double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->particles_Plot_Options.local_frequency_units);
			multilayer->particles_Plot_Options.x_Min = Locale.toDouble(min_X_LineEdit->text())*coeff;
			multilayer->particles_Plot_Options.x_Min+=1E-50;
			plot_Data(true);
		});

		QLabel* space_label = new QLabel(" - ");
		x_Range_layout->addWidget(space_label,0,Qt::AlignLeft);

		max_X_LineEdit = new QLineEdit(Locale.toString(multilayer->particles_Plot_Options.x_Max/coeff));
			max_X_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			max_X_LineEdit->setFixedWidth(40);
		x_Range_layout->addWidget(max_X_LineEdit,0,Qt::AlignLeft);
		connect(max_X_LineEdit,  &QLineEdit::textEdited, this, [=]	{
			double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->particles_Plot_Options.local_frequency_units);
			multilayer->particles_Plot_Options.x_Max = Locale.toDouble(max_X_LineEdit->text())*coeff;
			multilayer->particles_Plot_Options.x_Max+=1E-50;
			plot_Data(true);
		});

		QLabel* x_Units_label = new QLabel(" "+multilayer->particles_Plot_Options.local_frequency_units);
		x_Range_layout->addWidget(x_Units_label,0,Qt::AlignLeft);

		// ----------------------------------------------------

		QHBoxLayout* z_Range_Layout = new QHBoxLayout;
			z_Range_Layout->setAlignment(Qt::AlignLeft);
		scale_Layout->addLayout(z_Range_Layout);

		QLabel* orders_Label = new QLabel("Y range, orders: ");
		z_Range_Layout->addWidget(orders_Label);

		orders_Spinbox = new MyDoubleSpinBox(nullptr, false);
			orders_Spinbox->setRange(1,99);
			orders_Spinbox->setDecimals(1);
			orders_Spinbox->setSingleStep(0.1);
			orders_Spinbox->setValue(multilayer->particles_Plot_Options.orders_To_Show);
			orders_Spinbox->setAccelerated(true);
			orders_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		z_Range_Layout->addWidget(orders_Spinbox);
		connect(orders_Spinbox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			multilayer->particles_Plot_Options.orders_To_Show = orders_Spinbox->value();
			plot_Data();
		});

		// ----------------------------------------------------

		rescale_X_CheckBox = new QCheckBox("Rescale X");
			rescale_X_CheckBox->setChecked(multilayer->particles_Plot_Options.rescale_X);
		scale_Layout->addWidget(rescale_X_CheckBox);
		connect(rescale_X_CheckBox, &QCheckBox::toggled, this, [=]		{
			multilayer->particles_Plot_Options.rescale_X = rescale_X_CheckBox->isChecked();
		});

		// ----------------------------------------------------

		rescale_Y_CheckBox = new QCheckBox("Rescale Y");
			rescale_Y_CheckBox->setChecked(multilayer->particles_Plot_Options.rescale_Y);
		scale_Layout->addWidget(rescale_Y_CheckBox);
		connect(rescale_Y_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->particles_Plot_Options.rescale_Y = rescale_Y_CheckBox->isChecked();
		});

		// scaling
		// ----------------------------------------------------
		QHBoxLayout* scale_X_Layout = new QHBoxLayout;
			scale_X_Layout->setAlignment(Qt::AlignLeft);
			scale_Layout->addLayout(scale_X_Layout);

		scale_X_Label = new QLabel("Scale X: ");
			scale_X_Layout->addWidget(scale_X_Label);

		// lin
		lin_X_RadioButton = new QRadioButton("Lin");
			lin_X_RadioButton->setChecked(multilayer->particles_Plot_Options.x_Scale == lin_Scale);
		scale_X_Layout->addWidget(lin_X_RadioButton);
		connect(lin_X_RadioButton, &QRadioButton::clicked, this, [&]
		{
			if(lin_X_RadioButton->isChecked())
			{
				multilayer->particles_Plot_Options.x_Scale = lin_Scale;
			}
			plot_Data();
		});

		// log
		log_X_RadioButton = new QRadioButton("Log");
			log_X_RadioButton->setChecked(multilayer->particles_Plot_Options.x_Scale == log_Scale);
		scale_X_Layout->addWidget(log_X_RadioButton);
		connect(log_X_RadioButton, &QRadioButton::clicked, this, [&]
		{
			if(log_X_RadioButton->isChecked())
			{
				multilayer->particles_Plot_Options.x_Scale = log_Scale;
			}
			plot_Data();
		});

		QButtonGroup* X_ButtonGroup = new QButtonGroup;
			X_ButtonGroup->addButton(lin_X_RadioButton);
			X_ButtonGroup->addButton(log_X_RadioButton);

		// ----------------------------------------------------
		QHBoxLayout* scale_Y_Layout = new QHBoxLayout;
			scale_Y_Layout->setAlignment(Qt::AlignLeft);
			scale_Layout->addLayout(scale_Y_Layout);

		scale_Y_Label = new QLabel("Scale Y: ");
			scale_Y_Layout->addWidget(scale_Y_Label);

		// lin
		lin_Y_RadioButton = new QRadioButton("Lin");
			lin_Y_RadioButton->setChecked(multilayer->particles_Plot_Options.y_Scale == lin_Scale);
		scale_Y_Layout->addWidget(lin_Y_RadioButton);
		connect(lin_Y_RadioButton, &QRadioButton::clicked, this, [&]
		{
			if(lin_Y_RadioButton->isChecked())
			{
				multilayer->particles_Plot_Options.y_Scale = lin_Scale;
			}
			plot_Data();
		});

		// log
		log_Y_RadioButton = new QRadioButton("Log");
			log_Y_RadioButton->setChecked(multilayer->particles_Plot_Options.y_Scale == log_Scale);
		scale_Y_Layout->addWidget(log_Y_RadioButton);
		connect(log_Y_RadioButton, &QRadioButton::clicked, this, [&]
		{
			if(log_Y_RadioButton->isChecked())
			{
				multilayer->particles_Plot_Options.y_Scale = log_Scale;
			}
			plot_Data();
		});

		QButtonGroup* Y_ButtonGroup = new QButtonGroup;
			Y_ButtonGroup->addButton(lin_Y_RadioButton);
			Y_ButtonGroup->addButton(log_Y_RadioButton);

		// ----------------------------------------------------
		scale_GroupBox->adjustSize();
		scale_GroupBox->setFixedSize(scale_GroupBox->size());

		///------------------------------------------------------------------------------------------
		// 3rd part
		///------------------------------------------------------------------------------------------
		units_GroupBox = new QGroupBox;
			left_Layout->addWidget(units_GroupBox);
		QGridLayout* units_Layout = new QGridLayout(units_GroupBox);
			units_Layout->setAlignment(Qt::AlignLeft);
			units_Layout->setSpacing(2);
			units_Layout->setContentsMargins(8,5,6,5);

		// ----------------------------------------------------

		cursor_Cordinate_CheckBox = new QCheckBox("Show cursor position");
			cursor_Cordinate_CheckBox->setChecked(multilayer->particles_Plot_Options.show_Cursor_Position);
		units_Layout->addWidget(cursor_Cordinate_CheckBox,0,0,1,2);
		connect(cursor_Cordinate_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->particles_Plot_Options.show_Cursor_Position = cursor_Cordinate_CheckBox->isChecked();

			// other profiles obtain the same option : synchronization
			for(Particles_Plot* particles_Plot : particles_Plots_Window->particles_Plot_Vector)
			{
				particles_Plot->multilayer->particles_Plot_Options.show_Cursor_Position = multilayer->particles_Plot_Options.show_Cursor_Position;
				particles_Plot->cursor_Cordinate_CheckBox->blockSignals(true);
				particles_Plot->cursor_Cordinate_CheckBox->setChecked(particles_Plot->multilayer->particles_Plot_Options.show_Cursor_Position);
				particles_Plot->cursor_Cordinate_CheckBox->blockSignals(false);
			}
			global_Multilayer_Approach->reopen_Particles_Plots();
		});

		// ----------------------------------------------------

		QLabel* argument_Units_Label = new QLabel("Argument units ");
			units_Layout->addWidget(argument_Units_Label,1,0);

		argument_ComboBox = new QComboBox;
			argument_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			argument_ComboBox->addItems(spatial_Frequency_Units_List);
			argument_ComboBox->setCurrentText(multilayer->particles_Plot_Options.local_frequency_units);
		units_Layout->addWidget(argument_ComboBox,1,1);
		connect(argument_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			multilayer->particles_Plot_Options.local_frequency_units = argument_ComboBox->currentText();

			double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->particles_Plot_Options.local_frequency_units);
			min_X_LineEdit->setText(Locale.toString(multilayer->particles_Plot_Options.x_Min/coeff));
			max_X_LineEdit->setText(Locale.toString(multilayer->particles_Plot_Options.x_Max/coeff));
			x_Units_label->setText(" "+multilayer->particles_Plot_Options.local_frequency_units);

			plot_Data(true);
		});

		// ----------------------------------------------------

		QLabel* value_Units_Label = new QLabel("Value units ");
			units_Layout->addWidget(value_Units_Label,3,0);

		value_ComboBox = new QComboBox;
			value_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			value_ComboBox->addItems(PSD_2D_Value_Units_List);
			value_ComboBox->setCurrentText(multilayer->particles_Plot_Options.local_value_units);
		units_Layout->addWidget(value_ComboBox,3,1);
		connect(value_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			multilayer->particles_Plot_Options.local_value_units = value_ComboBox->currentText();
			plot_Data(true);
		});

		// ----------------------------------------------------

		units_GroupBox->adjustSize();
		units_GroupBox->setFixedSize(units_GroupBox->size());

		scale_GroupBox->setFixedWidth(layer_GroupBox->width());
		units_GroupBox->setFixedWidth(layer_GroupBox->width());
	// - - - - - - - - - - - - - - - - - - - - - - - - - - -

	custom_Plot->replot();
}

void Particles_Plot::create_Plot_Frame_And_Scale()
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

		/// X axis
		if(multilayer->particles_Plot_Options.x_Scale == lin_Scale)
		{
			QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
			custom_Plot->xAxis->setScaleType(QCPAxis::stLinear);
			custom_Plot->xAxis->setTicker(linTicker);
			custom_Plot->xAxis->setNumberFormat("g");
			custom_Plot->xAxis->setNumberPrecision(4);

			custom_Plot->xAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->xAxis2->setTicker(linTicker);
			custom_Plot->xAxis2->setNumberFormat("g");
			custom_Plot->xAxis2->setNumberPrecision(4);
		}
		if(multilayer->particles_Plot_Options.x_Scale == log_Scale)
		{
			QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);

			custom_Plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
			custom_Plot->xAxis->setTicker(logTicker);
			custom_Plot->xAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
			custom_Plot->xAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

			custom_Plot->xAxis2->setScaleType(QCPAxis::stLogarithmic);
			custom_Plot->xAxis2->setTicker(logTicker);
			custom_Plot->xAxis2->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
			custom_Plot->xAxis2->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
		}
		/// Y axis
		if(multilayer->particles_Plot_Options.y_Scale == lin_Scale)
		{
			QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);

			custom_Plot->yAxis->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis->setTicker(linTicker);
			custom_Plot->yAxis->setNumberFormat("g");
			custom_Plot->yAxis->setNumberPrecision(4);

			custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis2->setTicker(linTicker);
			custom_Plot->yAxis2->setNumberFormat("g");
			custom_Plot->yAxis2->setNumberPrecision(4);
		}
		if(multilayer->particles_Plot_Options.y_Scale == log_Scale)
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

		// make range draggable and zoomable:
		custom_Plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom /*| QCP::iSelectAxes*/ | QCP::iSelectLegend);

		// make top right axes clones of bottom left axes:
		custom_Plot->xAxis2->setVisible(true);
		custom_Plot->yAxis2->setVisible(true);
		custom_Plot->xAxis2->setTickLabels(false);

		connect(custom_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->xAxis2, SLOT(setRange(QCPRange)));
		connect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));

		connect(custom_Plot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), this, [=](const QCPRange&)
		{
			multilayer->particles_Plot_Options.old_X_Begin = custom_Plot->xAxis->range().lower;
			multilayer->particles_Plot_Options.old_X_End   = custom_Plot->xAxis->range().upper;
		});
		connect(custom_Plot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), this, [=](const QCPRange&)
		{
			multilayer->particles_Plot_Options.old_Y_Begin = custom_Plot->yAxis->range().lower;
			multilayer->particles_Plot_Options.old_Y_End   = custom_Plot->yAxis->range().upper;
		});
	}
}

void Particles_Plot::calculate_Profile()
{
	double nu_Min = min(multilayer->particles_Plot_Options.x_Min,multilayer->particles_Plot_Options.x_Max);
	double nu_Max = max(multilayer->particles_Plot_Options.x_Min,multilayer->particles_Plot_Options.x_Max);

	num_Plot_Points = num_roughness_plot_points;

	arg.clear();
	A_layer_Val.clear();
	B_layer_Val.clear();

	arg.resize(num_Plot_Points);
	A_layer_Val.resize(num_Plot_Points);
	B_layer_Val.resize(num_Plot_Points);

	A_Layer_Plot_Vector.resize(num_Plot_Points);
	B_Layer_Plot_Vector.resize(num_Plot_Points);

	double val_Coeff = PSD_2D_Value_Coefficients_Map.value(multilayer->particles_Plot_Options.local_value_units);
	double arg_Coeff = spatial_Frequency_Coefficients_Map.value(multilayer->particles_Plot_Options.local_frequency_units);

	/// preparing calc_Tree
	Calculation_Tree::fill_Tree_From_Scratch(calc_Tree, multilayer->structure_Tree->tree, multilayer);

	// unstratified
	Calculation_Tree::stratify_Calc_Tree(calc_Tree);

	// stratified
	flat_Calc_Tree.clear();
	Calculation_Tree::flatten_Tree(calc_Tree.begin(), calc_Tree, flat_Calc_Tree);
	Calculation_Tree::short_Tree(flat_Calc_Tree, short_Flat_Calc_Tree);

	num_Layers = short_Flat_Calc_Tree.size()-1;
	qInfo() << "num_Layers" << num_Layers << endl;

	// now we can fill interface_by_Number_ComboBox
	A_layer_by_Number_ComboBox->blockSignals(true);
	A_layer_by_Number_ComboBox->clear();
	B_layer_by_Number_ComboBox->blockSignals(true);
	B_layer_by_Number_ComboBox->clear();
	for(int layer_Index=0; layer_Index<num_Layers; layer_Index++)
	{
		A_layer_by_Number_ComboBox->addItem(QString::number(layer_Index+1));
		B_layer_by_Number_ComboBox->addItem(QString::number(layer_Index+1));
	}
	A_layer_by_Number_ComboBox->setCurrentIndex(multilayer->particles_Plot_Options.A_layer_Number_To_Show-1);
	A_layer_by_Number_ComboBox->blockSignals(false);
	B_layer_by_Number_ComboBox->setCurrentIndex(multilayer->particles_Plot_Options.B_layer_Number_To_Show-1);
	B_layer_by_Number_ComboBox->blockSignals(false);


	///--------------------------------------------------------------------------------------------------------------

	// fill argument
	if(multilayer->particles_Plot_Options.x_Scale == lin_Scale)
	{
		double step = (nu_Max - nu_Min) / (num_Plot_Points-1);

		arg[0] = nu_Min;
		for(int i=1; i<num_Plot_Points; i++)
		{
			arg[i] = arg[i-1] + step;
		}
	}
	if(multilayer->particles_Plot_Options.x_Scale == log_Scale)
	{
		nu_Min = max(nu_Min, 1E-16);
		Global_Variables::fill_Vector_With_Log_Step(arg, nu_Min, nu_Max, num_Plot_Points);
	}

	custom_Plot->clearGraphs();
	QString material;
	double sigma_Eff;
	double length_Coeff = length_Coefficients_Map.value(length_units);

	if(use_A_layer)
	{
		bool go_Further = true;
//		calc_PSD_For_Interface(media_Counter-1, top_Surface_Val, go_Further, material, sigma_Eff, nu_Min, nu_Max);

		if(go_Further)
		{
			for(int i=0; i<num_Plot_Points; i++)
			{
				A_Layer_Plot_Vector[i].key = arg[i] / arg_Coeff;
				A_Layer_Plot_Vector[i].value = A_layer_Val[i] / val_Coeff;
			}

			custom_Plot->addGraph();
			custom_Plot->graph()->data()->set(A_Layer_Plot_Vector);
			custom_Plot->graph()->setPen(QPen(Qt::red, 2.9));
			custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
			custom_Plot->graph()->setName("Layer " + QString::number(multilayer->particles_Plot_Options.A_layer_Number_To_Show) + " (" + material + ")");

			// for testing
//			QCPScatterStyle scatter_Style;
//			scatter_Style.setShape(QCPScatterStyle::ScatterShape(QCPScatterStyle::ssCircle));
//			scatter_Style.setSize(5);
//			custom_Plot->graph()->setScatterStyle(scatter_Style);

			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setTextColor(Qt::red);
			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setSelectedTextColor(Qt::red);
		}
	}
	if(use_B_layer)
	{
		bool go_Further = true;
//		calc_PSD_For_Interface(media_Counter-1, top_Surface_Val, go_Further, material, sigma_Eff, nu_Min, nu_Max);

		if(go_Further)
		{
			for(int i=0; i<num_Plot_Points; i++)
			{
				B_Layer_Plot_Vector[i].key = arg[i] / arg_Coeff;
				B_Layer_Plot_Vector[i].value = B_layer_Val[i] / val_Coeff;
			}

			custom_Plot->addGraph();
			custom_Plot->graph()->data()->set(B_Layer_Plot_Vector);
			custom_Plot->graph()->setPen(QPen(Qt::black, 1.4));
			custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
			custom_Plot->graph()->setName("Layer " + QString::number(multilayer->particles_Plot_Options.B_layer_Number_To_Show) + " (" + material + ")");

			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setTextColor(Qt::black);
			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setSelectedTextColor(Qt::black);
		}
	}
	custom_Plot->legend->setVisible(true);
	custom_Plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop); // legend position
}

void Particles_Plot::calc_Interference_Function_For_Layer(int layer_Index, QVector<double> &value_Vector, bool &use_Interface, QString &material, double &sigma_Eff, double nu_Min, double nu_Max)
{
	double calc_Nu_Min = min(nu_Min, multilayer->imperfections_Model.nu_Limit);
	double calc_Nu_Max = min(nu_Max, multilayer->imperfections_Model.nu_Limit);

	if(interface_Index >= media_Counter)
	{
		use_Interface = false;
		return;
	}
	interface_Index = media_Counter - interface_Index; // new interface index, 1 on top surface, media_Counter-1 on substrate

	Node* current_Node = media_Node_Map_Vector[interface_Index];
	Data& current_Data = current_Node->struct_Data;

	double sigma = current_Data.roughness_Model.sigma.value;
	double xi    = current_Data.roughness_Model.cor_radius.value;
	double alpha = current_Data.roughness_Model.fractal_alpha.value;

	material = current_Data.material;

	Data fake_Measurement;
	fake_Measurement.measurement_Type = measurement_Types[Detector_Scan];
//	fake_Measurement.detector_1D.finite_Slit = refill_dependent_structure_table;
//	fake_Measurement.detector_1D.slit_Length = current_Data.relative_Density.value;

	double tolerance = 1E-3;
	double depth = 2;
	ooura_fourier_sin<double> integrator(tolerance,depth);
	tanh_sinh<double> integrator_Tanh_Sinh;

	/// independent PSD without measured
	if( interface_Index == media_Counter-1 ||								// if substrate or
		!multilayer->imperfections_Model.use_Common_Roughness_Function)		// if any independent interface
	{
		current_Node->calculate_PSD_Factor(multilayer->imperfections_Model);

		/// creating FG splines. additional checks inside
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
			if(fake_Measurement.detector_1D.finite_Slit) {
				current_Node->create_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
			}
			current_Node->create_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model, fake_Measurement);
		} else {
			current_Node->create_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
		}

		/// choosing base PSD functions
		double (*PSD_Func_from_nu)(double, double, double, double, double, gsl_spline*, gsl_interp_accel*);
		double factor = 1;
		gsl_spline* spline_PSD_FG;
		gsl_interp_accel* acc_PSD_FG;

		if(multilayer->imperfections_Model.PSD_Model == ABC_Model) {
			if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
				if(fake_Measurement.detector_1D.finite_Slit) {
					PSD_Func_from_nu = &Global_Variables::PSD_ABC_1D_Finite_from_nu;
					factor = current_Data.roughness_Model.sigma.value;
				} else {
					PSD_Func_from_nu = &Global_Variables::PSD_ABC_1D_from_nu;
					factor = current_Data.PSD_ABC_1D_Factor;
				}
			} else {
				PSD_Func_from_nu = &Global_Variables::PSD_ABC_2D_from_nu;
				factor = current_Data.PSD_ABC_2D_Factor;
			}
		}
		if(multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model) {
			const double& alpha = current_Data.roughness_Model.fractal_alpha.value;

			if(abs(alpha-1)>DBL_EPSILON) {
				if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
					PSD_Func_from_nu = &Global_Variables::PSD_Fractal_Gauss_1D_from_nu;
					factor = 1;
					spline_PSD_FG = current_Node->spline_PSD_FG_1D;
					acc_PSD_FG = current_Node->acc_PSD_FG_1D;
				} else {
					PSD_Func_from_nu = &Global_Variables::PSD_Fractal_Gauss_2D_from_nu;
					factor = 1;
					spline_PSD_FG = current_Node->spline_PSD_FG_2D;
					acc_PSD_FG = current_Node->acc_PSD_FG_2D;
				}
			} else {
				if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
					if(fake_Measurement.detector_1D.finite_Slit) {
						PSD_Func_from_nu = &Global_Variables::PSD_Real_Gauss_1D_Finite_from_nu;
						factor = current_Data.roughness_Model.sigma.value;
					} else {
						PSD_Func_from_nu = &Global_Variables::PSD_Real_Gauss_1D_from_nu;
						factor = current_Data.PSD_Real_Gauss_1D_Factor;
					}
				} else {
					PSD_Func_from_nu = &Global_Variables::PSD_Real_Gauss_2D_from_nu;
					factor = current_Data.PSD_Real_Gauss_2D_Factor;
				}
			}
		}

		/// PSD calculation
		for(int i=0; i<num_Plot_Points; i++)
		{
			if(arg[i] <= multilayer->imperfections_Model.nu_Limit) {

				double nu_Max_Integration_2D = max_Frequency_For_2D_Spline;
				if(fake_Measurement.detector_1D.finite_Slit)
				{
					nu_Max_Integration_2D = min(max_Frequency_For_2D_Spline, Global_Variables::get_Nu_Max_From_Finite_Slit(arg[i], fake_Measurement));
				}
				value_Vector[i] = PSD_Func_from_nu(factor, xi, alpha, arg[i], nu_Max_Integration_2D, spline_PSD_FG, acc_PSD_FG);
			} else {
				value_Vector[i] = 0;
			}
		}

		/// sigma_effective calculation
		double sigma_2 = current_Node->combined_Effective_Sigma_2(fake_Measurement, multilayer->imperfections_Model, sigma, xi, alpha, calc_Nu_Min, calc_Nu_Max, multilayer->roughness_Plot_Options.PSD_Type, integrator);
		sigma_Eff = sqrt(sigma_2);

		/// clear FG splines, but not for substrate
		if(interface_Index != media_Counter-1) // not clear here for substrate
		{
			if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
				current_Node->clear_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model);
				if(fake_Measurement.detector_1D.finite_Slit) {
					current_Node->clear_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
				}
			} else {
				current_Node->clear_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
			}
		}
	}

	/// combined PSD
	if( interface_Index == media_Counter-1 )	// if substrate
	{
		PSD_Data psd_Data;
		gsl_spline* spline_PSD_Meas;
		gsl_interp_accel* acc_PSD_Meas;
		bool use_Measured = false;

		current_Node->calculate_PSD_Factor(multilayer->imperfections_Model);

		/// create measured spline
		current_Node->create_Spline_PSD_Measured(multilayer->imperfections_Model);

		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D)
		{
			psd_Data = multilayer->imperfections_Model.PSD_1D;
			spline_PSD_Meas = current_Node->spline_PSD_Meas_1D;
			acc_PSD_Meas = current_Node->acc_PSD_Meas_1D;
			use_Measured = multilayer->imperfections_Model.add_Measured_PSD_1D;

			if(current_Node->spline_PSD_Combined_1D_Condition(multilayer->imperfections_Model))
			{
				if(!fake_Measurement.detector_1D.finite_Slit) {	// if finite slit, already created and not deleted for substrate
					current_Node->create_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
//					current_Node->create_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model, fake_Measurement);
				}
				current_Node->create_Spline_PSD_Combined_1D(multilayer->imperfections_Model, fake_Measurement);

				for(int i=0; i<num_Plot_Points; i++)
				{
					if(arg[i] <= multilayer->imperfections_Model.nu_Limit) {
						value_Vector[i] = gsl_spline_eval(current_Node->spline_PSD_Combined_1D, arg[i], current_Node->acc_PSD_Combined_1D);
					} else {
						value_Vector[i] = 0;
					}
				}

				/// sigma_effective calculation
				double sigma_2 = current_Node->combined_Effective_Sigma_2_From_Spline(multilayer->imperfections_Model, calc_Nu_Min, calc_Nu_Max, current_Node->spline_PSD_Combined_1D, current_Node->acc_PSD_Combined_1D, multilayer->roughness_Plot_Options.PSD_Type, integrator_Tanh_Sinh);
				sigma_Eff = sqrt(sigma_2);

				current_Node->clear_Spline_PSD_Combined_1D(multilayer->imperfections_Model);
				if(!fake_Measurement.detector_1D.finite_Slit) {	// if finite slit, clear later for substrate
					current_Node->clear_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
//					current_Node->clear_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model);
				}
			}
		} else
		{
			psd_Data = multilayer->imperfections_Model.PSD_2D;
			spline_PSD_Meas = current_Node->spline_PSD_Meas_2D;
			acc_PSD_Meas = current_Node->acc_PSD_Meas_2D;
			use_Measured = multilayer->imperfections_Model.add_Measured_PSD_2D;
		}

		if(psd_Data.is_Loaded() && use_Measured)
		{
			for(int i=0; i<num_Plot_Points; i++)
			{
				if( arg[i]>psd_Data.argument.front() &&
					arg[i]<psd_Data.argument.back())
				{
					if(arg[i] <= multilayer->imperfections_Model.nu_Limit) {
						value_Vector[i] = gsl_spline_eval(spline_PSD_Meas, arg[i], acc_PSD_Meas);
					} else {
						value_Vector[i] = 0;
					}
				}
			}

			/// sigma_effective calculation
			double sigma_2 = current_Node->combined_Effective_Sigma_2(fake_Measurement, multilayer->imperfections_Model, sigma, xi, alpha, calc_Nu_Min, calc_Nu_Max, multilayer->roughness_Plot_Options.PSD_Type, integrator);
			sigma_Eff = sqrt(sigma_2);
		}
		/// clear measured spline
		current_Node->clear_Spline_PSD_Measured(multilayer->imperfections_Model);

		/// clear FG splines
		// clear here for substrate
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
			current_Node->clear_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model);
			if(fake_Measurement.detector_1D.finite_Slit) {
				current_Node->clear_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
			}
		} else {
			current_Node->clear_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
		}
	}

	/// gaussian peak
	if( multilayer->imperfections_Model.add_Gauss_Peak &&					// if have peak
	   (interface_Index == media_Counter-1 ||								// if substrate or
		!multilayer->imperfections_Model.use_Common_Roughness_Function))	// if any independent interface
	{
		current_Node->calculate_PSD_Factor(multilayer->imperfections_Model);

		/// creating peak spline
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
			current_Node->create_Spline_PSD_Peak(multilayer->imperfections_Model, fake_Measurement);
		} else {
			/* do nothing */
		}

		/// choosing base PSD functions
		double (*PSD_Peak_from_nu)(double, double, double, double, gsl_spline*, gsl_interp_accel*);
		double factor = 1;
		gsl_spline* spline_PSD_Peak;
		gsl_interp_accel* acc_PSD_Peak;

		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
			PSD_Peak_from_nu = &Global_Variables::PSD_Gauss_Peak_1D_from_nu;
			factor = 1;
			spline_PSD_Peak = current_Node->spline_PSD_Peak;
			acc_PSD_Peak = current_Node->acc_PSD_Peak;
		} else {
			PSD_Peak_from_nu = &Global_Variables::PSD_Gauss_Peak_2D_from_nu;
			factor = current_Data.PSD_Gauss_Peak_2D_Factor;
			spline_PSD_Peak = nullptr;
			acc_PSD_Peak = nullptr;
		}

		/// PSD calculation
		double peak_Frequency =		current_Data.roughness_Model.peak_Frequency.value;
		double peak_Frequency_Width =current_Data.roughness_Model.peak_Frequency_Width.value;
		for(int i=0; i<num_Plot_Points; i++)
		{
			if(arg[i] <= multilayer->imperfections_Model.nu_Limit) {
				value_Vector[i] += PSD_Peak_from_nu(factor, peak_Frequency, peak_Frequency_Width, arg[i], spline_PSD_Peak, acc_PSD_Peak);
			} else {
				value_Vector[i] += 0;
			}
		}

		/// sigma_effective peak addition calculation
		double sigma_2 = current_Node->combined_Effective_Sigma_2_Peak(calc_Nu_Min, calc_Nu_Max, multilayer->roughness_Plot_Options.PSD_Type, integrator_Tanh_Sinh);
		sigma_Eff = sqrt(sigma_Eff*sigma_Eff + sigma_2);

		/// clear peak spline
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
			current_Node->clear_Spline_PSD_Peak(multilayer->imperfections_Model);
		} else {
			/* do nothing */
		}
	}

	/// ------------------------------------------------------------------------------------------------------------------
	/// for linear growth of layers
	/// ------------------------------------------------------------------------------------------------------------------

	if( interface_Index < media_Counter-1 &&	// not for substrate
		multilayer->imperfections_Model.vertical_Correlation == partial_Correlation &&
	   (multilayer->imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
		multilayer->imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model))
	{
		// for legacy reasons we should calculate splines in substrate node
		Node* substrate_Node = media_Node_Map_Vector[media_Counter-1];
//		Data& substrate_Data = substrate_Node->struct_Data;

		substrate_Node->calculate_PSD_Factor(multilayer->imperfections_Model);
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D)
		{
			/// 1D
			substrate_Node->create_Spline_PSD_Fractal_Gauss_2D	(multilayer->imperfections_Model);
			substrate_Node->create_Spline_PSD_Fractal_Gauss_1D	(multilayer->imperfections_Model, fake_Measurement);
			substrate_Node->create_Spline_PSD_Combined_1D		(multilayer->imperfections_Model, fake_Measurement);
			substrate_Node->create_Spline_PSD_Peak				(multilayer->imperfections_Model, fake_Measurement);
			substrate_Node->create_Spline_PSD_Measured			(multilayer->imperfections_Model);
			substrate_Node->create_Spline_PSD_Linear_Growth_2D	(multilayer->imperfections_Model, fake_Measurement, media_Data_Map_Vector, interface_Index-1);
			substrate_Node->create_Spline_PSD_Linear_Growth_1D	(multilayer->imperfections_Model, fake_Measurement);

			///--------------------------------------------------------------------------------------------------------
			for(int i=0; i<num_Plot_Points; i++)
			{
				if(arg[i] <= multilayer->imperfections_Model.nu_Limit) {
					value_Vector[i] = gsl_spline_eval(substrate_Node->spline_PSD_Linear_Growth_1D, arg[i], substrate_Node->acc_PSD_Linear_Growth_1D);
				} else {
					value_Vector[i] = 0;
				}
			}

			/// sigma_effective calculation
			double sigma_2 = substrate_Node->combined_Effective_Sigma_2_From_Spline(multilayer->imperfections_Model, calc_Nu_Min, calc_Nu_Max, substrate_Node->spline_PSD_Linear_Growth_1D, substrate_Node->acc_PSD_Linear_Growth_1D, multilayer->roughness_Plot_Options.PSD_Type, integrator_Tanh_Sinh);
			sigma_Eff = sqrt(sigma_2);
			///--------------------------------------------------------------------------------------------------------

			substrate_Node->clear_Spline_PSD_Linear_Growth_1D	(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Linear_Growth_2D	(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Measured			(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Peak				(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Combined_1D		(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Fractal_Gauss_2D	(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Fractal_Gauss_1D	(multilayer->imperfections_Model);
		} else
		{
			/// 2D
			substrate_Node->create_Spline_PSD_Fractal_Gauss_2D	(multilayer->imperfections_Model);
			substrate_Node->create_Spline_PSD_Measured			(multilayer->imperfections_Model);
			substrate_Node->create_Spline_PSD_Linear_Growth_2D	(multilayer->imperfections_Model, fake_Measurement, media_Data_Map_Vector, interface_Index-1);

			///--------------------------------------------------------------------------------------------------------
			for(int i=0; i<num_Plot_Points; i++)
			{
				if(arg[i] <= multilayer->imperfections_Model.nu_Limit) {
					value_Vector[i] = gsl_spline_eval(substrate_Node->spline_PSD_Linear_Growth_2D, arg[i], substrate_Node->acc_PSD_Linear_Growth_2D);
				} else {
					value_Vector[i] = 0;
				}
			}

			/// sigma_effective calculation
			double sigma_2 = substrate_Node->combined_Effective_Sigma_2_From_Spline(multilayer->imperfections_Model, calc_Nu_Min, calc_Nu_Max, substrate_Node->spline_PSD_Linear_Growth_2D, substrate_Node->acc_PSD_Linear_Growth_2D, multilayer->roughness_Plot_Options.PSD_Type, integrator_Tanh_Sinh);
			sigma_Eff = sqrt(sigma_2);
			///--------------------------------------------------------------------------------------------------------

			substrate_Node->clear_Spline_PSD_Fractal_Gauss_2D	(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Measured			(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Linear_Growth_2D	(multilayer->imperfections_Model);
		}
	}
}

void Particles_Plot::plot_Data(bool recalculate_Profile)
{
	create_Plot_Frame_And_Scale();

	// data
	if(recalculate_Profile)
	{
//		auto start = std::chrono::system_clock::now();
		calculate_Profile();
//		auto end = std::chrono::system_clock::now();
//		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//		qInfo() << "	calculate_Profile:    "<< elapsed.count()/1000000. << " seconds" << endl << endl << endl;
	}

	// rescaling
	double minimum = -2020, maximum = 2020;
	if(multilayer->particles_Plot_Options.y_Scale == lin_Scale )
	{
		QVector<double> val_Final;

		if(use_A_layer) val_Final.append(A_layer_Val);
		if(use_B_layer) val_Final.append(B_layer_Val);

		if(val_Final.size()==0) val_Final = {0,1};

		double minimum_Raw = *std::min_element(val_Final.constBegin(), val_Final.constEnd());
		double maximum_Raw = *std::max_element(val_Final.constBegin(), val_Final.constEnd());

		minimum = minimum_Raw - (maximum_Raw-minimum_Raw)*0.05;
		maximum = maximum_Raw + (maximum_Raw-minimum_Raw)*0.08;
	}
	if(multilayer->particles_Plot_Options.y_Scale == log_Scale )
	{
		QVector<double> val_Non_Zero;

		if(use_A_layer) val_Non_Zero.append(A_layer_Val);
		if(use_B_layer) val_Non_Zero.append(B_layer_Val);

		for(int i=val_Non_Zero.size()-1; i>=0; i--)
		{
			if(val_Non_Zero[i]<default_logarithmic_threshold_PSD) {val_Non_Zero.remove(i);}
		}

		if(val_Non_Zero.size()==0) val_Non_Zero = {0.1,10};

		double minimum_Raw_Non_Zero = *std::min_element(val_Non_Zero.constBegin(), val_Non_Zero.constEnd());
		double maximum_Raw_Non_Zero = *std::max_element(val_Non_Zero.constBegin(), val_Non_Zero.constEnd());

		minimum = minimum_Raw_Non_Zero/1;
		maximum = maximum_Raw_Non_Zero*3;

		minimum = max(minimum,maximum/pow(10,multilayer->particles_Plot_Options.orders_To_Show));
	}

	// units coefficients
	double arg_Coeff = spatial_Frequency_Coefficients_Map.value(multilayer->particles_Plot_Options.local_frequency_units);
	double val_Coeff = PSD_2D_Value_Coefficients_Map.     value(multilayer->particles_Plot_Options.local_value_units);

	if(multilayer->particles_Plot_Options.rescale_X){ custom_Plot->xAxis->setRange(arg.front()/arg_Coeff, arg.back()/arg_Coeff);	}
	else											{ custom_Plot->xAxis->setRange(multilayer->particles_Plot_Options.old_X_Begin, multilayer->particles_Plot_Options.old_X_End);}
	if(multilayer->particles_Plot_Options.rescale_Y){ custom_Plot->yAxis->setRange(minimum/val_Coeff, maximum/val_Coeff);			}
	else											{ custom_Plot->yAxis->setRange(multilayer->particles_Plot_Options.old_Y_Begin, multilayer->particles_Plot_Options.old_Y_End);}

	// labels
	custom_Plot->xAxis->setLabel(Nu_Sym + ", "+multilayer->particles_Plot_Options.local_frequency_units);
	custom_Plot->yAxis->setLabel("Interference function, " + multilayer->particles_Plot_Options.local_value_units);

	custom_Plot->replot();
}

void Particles_Plot::lock_Interfaces()
{
	if(!multilayer->imperfections_Model.use_Common_Particle_Function &&
		multilayer->imperfections_Model.particle_Vertical_Correlation == zero_Correlation)
	{
		use_A_layer = multilayer->particles_Plot_Options.A_show_Layer_With_Number;
		use_B_layer = multilayer->particles_Plot_Options.B_show_Layer_With_Number;

		A_show_Layer_by_Number_CheckBox->setDisabled(false);
		A_layer_by_Number_ComboBox->setDisabled(!multilayer->particles_Plot_Options.A_show_Layer_With_Number);

		B_show_Layer_by_Number_CheckBox->setDisabled(false);
		B_layer_by_Number_ComboBox->setDisabled(!multilayer->particles_Plot_Options.B_show_Layer_With_Number);

	} else {
		use_A_layer = false;
		use_B_layer = true;

		A_show_Layer_by_Number_CheckBox->setDisabled(true);
		A_layer_by_Number_ComboBox->setDisabled(true);

		B_show_Layer_by_Number_CheckBox->setDisabled(true);
		B_layer_by_Number_ComboBox->setDisabled(true);
	}
}
