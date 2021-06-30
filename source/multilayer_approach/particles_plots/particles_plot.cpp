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


	////////////////////////////
	/// TODO
	/// //////////////////////

	/// preparing calc_Tree
	Calculation_Tree::fill_Tree_From_Scratch(calc_Tree, multilayer->structure_Tree->tree, multilayer);
	media_Counter = Calculation_Tree::get_Total_Num_Layers(calc_Tree.begin());

	// now we can fill interface_by_Number_ComboBox
	interface_by_Number_ComboBox->blockSignals(true);
	interface_by_Number_ComboBox->clear();
	for(int interface_Index = 1; interface_Index<media_Counter; interface_Index++)
	{
		interface_by_Number_ComboBox->addItem(QString::number(interface_Index));
	}
	interface_by_Number_ComboBox->setCurrentIndex(multilayer->roughness_Plot_Options.interface_Number_To_Show-1);
	interface_by_Number_ComboBox->blockSignals(false);

	// unstratified
	media_Data_Map_Vector.resize(media_Counter);
	media_Period_Index_Map_Vector.resize(media_Counter);
	Calculation_Tree::unwrap_Calc_Tree_Data(calc_Tree.begin(), media_Data_Map_Vector, media_Period_Index_Map_Vector);
	Calculation_Tree::stratify_Calc_Tree(calc_Tree);

	// stratified
	media_Node_Map_Vector.resize(media_Counter);
	flat_Calc_Tree.clear();
	Calculation_Tree::flatten_Tree(calc_Tree.begin(), calc_Tree, flat_Calc_Tree);
	Calculation_Tree::short_Tree(flat_Calc_Tree, short_Flat_Calc_Tree);
	Calculation_Tree::unwrap_Calc_Tree_Node(calc_Tree.begin(), media_Node_Map_Vector);

	///--------------------------------------------------------------------------------------------------------------

	// fill argument
	if(multilayer->roughness_Plot_Options.x_Scale == lin_Scale)
	{
		double step = (nu_Max - nu_Min) / (num_Plot_Points-1);

		arg[0] = nu_Min;
		for(int i=1; i<num_Plot_Points; i++)
		{
			arg[i] = arg[i-1] + step;
		}
	}
	if(multilayer->roughness_Plot_Options.x_Scale == log_Scale)
	{
		Node* substrate_Node = media_Node_Map_Vector[media_Counter-1];
		Data& substrate_Data = substrate_Node->struct_Data;

		double peak_Frequency		= substrate_Data.roughness_Model.peak_Frequency.value;
		double peak_Frequency_Width = substrate_Data.roughness_Model.peak_Frequency_Width.value;

		if(multilayer->imperfections_Model.add_Gauss_Peak)
		{
			Global_Variables::fill_Vector_With_Log_Step_With_Peak(arg, nu_Min, nu_Max, peak_Frequency, peak_Frequency_Width, num_Plot_Points, multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D);
		} else {
			Global_Variables::fill_Vector_With_Log_Step(arg, nu_Min, nu_Max, num_Plot_Points);
		}
	}

	custom_Plot->clearGraphs();
	QString material;
	double sigma_Eff;
	double length_Coeff = length_Coefficients_Map.value(length_units);


	if(use_Top_Surface)
	{
		bool go_Further = true;
		calc_PSD_For_Interface(media_Counter-1, top_Surface_Val, go_Further, material, sigma_Eff, nu_Min, nu_Max);

		if(go_Further)
		{
			for(int i=0; i<num_Plot_Points; i++)
			{
				top_Surface_Plot_Vector[i].key = arg[i] / arg_Coeff;
				top_Surface_Plot_Vector[i].value = top_Surface_Val[i] / val_Coeff;
			}

			custom_Plot->addGraph();
			custom_Plot->graph()->data()->set(top_Surface_Plot_Vector);
			custom_Plot->graph()->setPen(QPen(Qt::blue, 2.9));
			custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
			custom_Plot->graph()->setName("Top surface (" + material + "), "+Sigma_Sym+Subscript_e_Sym+"="+QString::number(sigma_Eff/length_Coeff,'f',2)+" "+length_units);

			// for testing
//			QCPScatterStyle scatter_Style;
//			scatter_Style.setShape(QCPScatterStyle::ScatterShape(QCPScatterStyle::ssCircle));
//			scatter_Style.setSize(5);
//			custom_Plot->graph()->setScatterStyle(scatter_Style);


			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setTextColor(Qt::blue);
			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setSelectedTextColor(Qt::blue);
		}
	}
	if(use_Current_Interface)
	{
		bool go_Further = true;
		calc_PSD_For_Interface(multilayer->roughness_Plot_Options.interface_Number_To_Show, current_Interface_Val, go_Further, material, sigma_Eff, nu_Min, nu_Max);
		if(go_Further)
		{
			for(int i=0; i<num_Plot_Points; i++)
			{
				current_Interface_Plot_Vector[i].key = arg[i] / arg_Coeff;
				current_Interface_Plot_Vector[i].value = current_Interface_Val[i] / val_Coeff;
			}

			custom_Plot->addGraph();
			custom_Plot->graph()->data()->set(current_Interface_Plot_Vector);
			custom_Plot->graph()->setPen(QPen(Qt::red, 2.3));
			custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
			custom_Plot->graph()->setName("Interface " + QString::number(multilayer->roughness_Plot_Options.interface_Number_To_Show) + " (" + material + "), "+Sigma_Sym+Subscript_e_Sym+"="+QString::number(sigma_Eff/length_Coeff,'f',2)+" "+length_units);

			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setTextColor(Qt::red);
			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setSelectedTextColor(Qt::red);
		}
	}
	if(use_Substrate_Surface)
	{
		bool go_Further = true;
		calc_PSD_For_Interface(1, substrate_Surface_Val, go_Further, material, sigma_Eff, nu_Min, nu_Max);
		if(go_Further)
		{
			for(int i=0; i<num_Plot_Points; i++)
			{
				substrate_Surface_Plot_Vector[i].key = arg[i] / arg_Coeff;
				substrate_Surface_Plot_Vector[i].value = substrate_Surface_Val[i] / val_Coeff;
			}

			custom_Plot->addGraph();
			custom_Plot->graph()->data()->set(substrate_Surface_Plot_Vector);
			custom_Plot->graph()->setPen(QPen(Qt::black, 1.4));
			custom_Plot->graph()->selectionDecorator()->setPen(QPen(custom_Plot->graph()->pen().color(),selected_Profile_Line_Thickness));
			custom_Plot->graph()->setName("Substrate (" + material + "), "+Sigma_Sym+Subscript_e_Sym+"="+QString::number(sigma_Eff/length_Coeff,'f',2)+" "+length_units);

			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setTextColor(Qt::black);
			custom_Plot->legend->itemWithPlottable(custom_Plot->graph())->setSelectedTextColor(Qt::black);
		}
	}
	custom_Plot->legend->setVisible(true);
	custom_Plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignBottom); // legend position
}

void Particles_Plot::calc_PSD_For_Interface(int interface_Index, QVector<double> &value_Vector, bool &use_Interface, QString &material, double &sigma_Eff, double nu_Min, double nu_Max)
{

}

void Particles_Plot::plot_Data(bool recalculate_Profile)
{

}

void Particles_Plot::lock_Interfaces()
{

}
