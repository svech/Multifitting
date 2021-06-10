#include "roughness_plot.h"

Roughness_Plot::Roughness_Plot(Multilayer* multilayer, Roughness_Plots_Window* roughness_Plots_Window, QWidget* parent) :
	multilayer(multilayer),
	roughness_Plots_Window(roughness_Plots_Window),
	QWidget(parent)
{
	create_Main_Layout();
}

void Roughness_Plot::create_Main_Layout()
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

	if(multilayer->roughness_Plot_Options.show_Cursor_Position)
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

void Roughness_Plot::create_Left_Side()
{
	// left side
	///==============================================================================================
		QVBoxLayout* left_Layout = new QVBoxLayout;
			left_Layout->setSpacing(2);
			main_Layout->addLayout(left_Layout);

		///------------------------------------------------------------------------------------------
		// top part
		///------------------------------------------------------------------------------------------
		value_Type_GroupBox = new QGroupBox;
			left_Layout->addWidget(value_Type_GroupBox);
			left_Layout->setAlignment(Qt::AlignTop);
		QVBoxLayout* value_Type_Layout = new QVBoxLayout(value_Type_GroupBox);
			value_Type_Layout->setSpacing(2);
			value_Type_Layout->setContentsMargins(8,5,8,5);

		// ----------------------------------------------------
		// PSD 1D
		// ----------------------------------------------------
		PSD_1D_RadioButton = new QRadioButton("PSD 1D");
			PSD_1D_RadioButton->setChecked(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D);
		value_Type_Layout->addWidget(PSD_1D_RadioButton);
		connect(PSD_1D_RadioButton, &QRadioButton::clicked, this, [=]
		{
			bool checked = PSD_1D_RadioButton->isChecked();
			if(checked)
			{
				multilayer->roughness_Plot_Options.PSD_Type = PSD_Type_1D;
				plot_Data(true);
			}
		});

		// ----------------------------------------------------
		// PSD 2D
		// ----------------------------------------------------
		PSD_2D_RadioButton = new QRadioButton("PSD 2D");
			PSD_2D_RadioButton->setChecked(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_2D);
		value_Type_Layout->addWidget(PSD_2D_RadioButton);
		connect(PSD_2D_RadioButton, &QRadioButton::clicked, this, [=]
		{
			bool checked = PSD_2D_RadioButton->isChecked();
			if(checked)
			{
				multilayer->roughness_Plot_Options.PSD_Type = PSD_Type_2D;
				plot_Data(true);
			}
		});

		// ----------------------------------------------------
		QButtonGroup* radio_ButtonGroup = new QButtonGroup;
			radio_ButtonGroup->addButton(PSD_1D_RadioButton);
			radio_ButtonGroup->addButton(PSD_2D_RadioButton);
		// ----------------------------------------------------
		value_Type_GroupBox->adjustSize();
		value_Type_GroupBox->setFixedSize(value_Type_GroupBox->size());

		///------------------------------------------------------------------------------------------
		// middle part
		///------------------------------------------------------------------------------------------
		interface_GroupBox = new QGroupBox;
			left_Layout->addWidget(interface_GroupBox);
		QVBoxLayout* interface_Layout = new QVBoxLayout(interface_GroupBox);
			interface_Layout->setSpacing(2);
			interface_Layout->setContentsMargins(8,5,6,5);

		// ----------------------------------------------------

		show_Top_Surface_CheckBox = new QCheckBox("Show top surface");
			show_Top_Surface_CheckBox->setChecked(multilayer->roughness_Plot_Options.show_Top_Surface);
		interface_Layout->addWidget(show_Top_Surface_CheckBox);
		connect(show_Top_Surface_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->roughness_Plot_Options.show_Top_Surface = show_Top_Surface_CheckBox->isChecked();
			use_Top_Surface = multilayer->roughness_Plot_Options.show_Top_Surface;
			plot_Data(true);
		});

		QHBoxLayout* interface_Number_layout = new QHBoxLayout;
			interface_Number_layout->setAlignment(Qt::AlignLeft);
		interface_Layout->addLayout(interface_Number_layout);

		show_Interface_by_Number_CheckBox = new QCheckBox("Show interface");
			show_Interface_by_Number_CheckBox->setChecked(multilayer->roughness_Plot_Options.show_Interface_With_Number);
		interface_Number_layout->addWidget(show_Interface_by_Number_CheckBox);
		connect(show_Interface_by_Number_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->roughness_Plot_Options.show_Interface_With_Number = show_Interface_by_Number_CheckBox->isChecked();
			use_Current_Interface = multilayer->roughness_Plot_Options.show_Interface_With_Number;
			interface_by_Number_ComboBox->setEnabled(multilayer->roughness_Plot_Options.show_Interface_With_Number);
			plot_Data(true);
		});

		interface_by_Number_ComboBox = new QComboBox;
			interface_by_Number_ComboBox->setFixedWidth(55);
			interface_by_Number_ComboBox->setEnabled(multilayer->roughness_Plot_Options.show_Interface_With_Number);
		interface_Number_layout->addWidget(interface_by_Number_ComboBox,0,Qt::AlignLeft);
		connect(interface_by_Number_ComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]
		{
			multilayer->roughness_Plot_Options.interface_Number_To_Show = interface_by_Number_ComboBox->currentIndex()+1;
			plot_Data(true);
		});

		show_Substrate_Surface_CheckBox = new QCheckBox("Show substrate surface");
			show_Substrate_Surface_CheckBox->setChecked(multilayer->roughness_Plot_Options.show_Substrate_Surface);
		interface_Layout->addWidget(show_Substrate_Surface_CheckBox);
		connect(show_Substrate_Surface_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->roughness_Plot_Options.show_Substrate_Surface = show_Substrate_Surface_CheckBox->isChecked();
			use_Substrate_Surface = multilayer->roughness_Plot_Options.show_Substrate_Surface;
			plot_Data(true);
		});

		// ----------------------------------------------------
		interface_GroupBox->adjustSize();
		interface_GroupBox->setFixedSize(interface_GroupBox->size());

		///------------------------------------------------------------------------------------------
		// 3rd part
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

		double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_frequency_units);
		min_X_LineEdit = new QLineEdit(Locale.toString(multilayer->roughness_Plot_Options.x_Min/coeff));
			min_X_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			min_X_LineEdit->setFixedWidth(40);
		x_Range_layout->addWidget(min_X_LineEdit,0,Qt::AlignLeft);
		connect(min_X_LineEdit,  &QLineEdit::textEdited, this, [=]	{
			double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_frequency_units);
			multilayer->roughness_Plot_Options.x_Min = Locale.toDouble(min_X_LineEdit->text())*coeff;
			plot_Data(true);
		});

		QLabel* space_label = new QLabel(" - ");
		x_Range_layout->addWidget(space_label,0,Qt::AlignLeft);

		max_X_LineEdit = new QLineEdit(Locale.toString(multilayer->roughness_Plot_Options.x_Max/coeff));
			max_X_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			max_X_LineEdit->setFixedWidth(40);
		x_Range_layout->addWidget(max_X_LineEdit,0,Qt::AlignLeft);
		connect(max_X_LineEdit,  &QLineEdit::textEdited, this, [=]	{
			double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_frequency_units);
			multilayer->roughness_Plot_Options.x_Max = Locale.toDouble(max_X_LineEdit->text())*coeff;
			plot_Data(true);
		});

		QLabel* x_Units_label = new QLabel(" "+multilayer->roughness_Plot_Options.local_frequency_units);
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
			orders_Spinbox->setValue(multilayer->roughness_Plot_Options.orders_To_Show);
			orders_Spinbox->setAccelerated(true);
			orders_Spinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		z_Range_Layout->addWidget(orders_Spinbox);
		connect(orders_Spinbox, static_cast<void(MyDoubleSpinBox::*)(double)>(&MyDoubleSpinBox::valueChanged), this, [=]
		{
			multilayer->roughness_Plot_Options.orders_To_Show = orders_Spinbox->value();
			plot_Data();
		});

		// ----------------------------------------------------

		rescale_X_CheckBox = new QCheckBox("Rescale X");
			rescale_X_CheckBox->setChecked(multilayer->roughness_Plot_Options.rescale_X);
		scale_Layout->addWidget(rescale_X_CheckBox);
		connect(rescale_X_CheckBox, &QCheckBox::toggled, this, [=]		{
			multilayer->roughness_Plot_Options.rescale_X = rescale_X_CheckBox->isChecked();
		});

		// ----------------------------------------------------

		rescale_Y_CheckBox = new QCheckBox("Rescale Y");
			rescale_Y_CheckBox->setChecked(multilayer->roughness_Plot_Options.rescale_Y);
		scale_Layout->addWidget(rescale_Y_CheckBox);
		connect(rescale_Y_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->roughness_Plot_Options.rescale_Y = rescale_Y_CheckBox->isChecked();
		});

		// scaling
		// ----------------------------------------------------
//		QHBoxLayout* scale_X_Layout = new QHBoxLayout;
//			scale_X_Layout->setAlignment(Qt::AlignLeft);
//			scale_Layout->addLayout(scale_X_Layout);

//		scale_X_Label = new QLabel("Scale X: ");
//			scale_X_Layout->addWidget(scale_X_Label);

//		// lin
//		lin_X_RadioButton = new QRadioButton("Lin");
//			lin_X_RadioButton->setChecked(multilayer->roughness_Plot_Options.x_Scale == lin_Scale);
//		scale_X_Layout->addWidget(lin_X_RadioButton);
//		connect(lin_X_RadioButton, &QRadioButton::clicked, this, [&]
//		{
//			if(lin_X_RadioButton->isChecked())
//			{
//				multilayer->roughness_Plot_Options.x_Scale = lin_Scale;
//			}
//			plot_Data();
//		});

//		// log
//		log_X_RadioButton = new QRadioButton("Log");
//			log_X_RadioButton->setChecked(multilayer->roughness_Plot_Options.x_Scale == log_Scale);
//		scale_X_Layout->addWidget(log_X_RadioButton);
//		connect(log_X_RadioButton, &QRadioButton::clicked, this, [&]
//		{
//			if(log_X_RadioButton->isChecked())
//			{
//				multilayer->roughness_Plot_Options.x_Scale = log_Scale;
//			}
//			plot_Data();
//		});

//		QButtonGroup* X_ButtonGroup = new QButtonGroup;
//			X_ButtonGroup->addButton(lin_X_RadioButton);
//			X_ButtonGroup->addButton(log_X_RadioButton);

//		// ----------------------------------------------------
//		QHBoxLayout* scale_Y_Layout = new QHBoxLayout;
//			scale_Y_Layout->setAlignment(Qt::AlignLeft);
//			scale_Layout->addLayout(scale_Y_Layout);

//		scale_Y_Label = new QLabel("Scale Y: ");
//			scale_Y_Layout->addWidget(scale_Y_Label);

//		// lin
//		lin_Y_RadioButton = new QRadioButton("Lin");
//			lin_Y_RadioButton->setChecked(multilayer->roughness_Plot_Options.y_Scale == lin_Scale);
//		scale_Y_Layout->addWidget(lin_Y_RadioButton);
//		connect(lin_Y_RadioButton, &QRadioButton::clicked, this, [&]
//		{
//			if(lin_Y_RadioButton->isChecked())
//			{
//				multilayer->roughness_Plot_Options.y_Scale = lin_Scale;
//			}
//			plot_Data();
//		});

//		// log
//		log_Y_RadioButton = new QRadioButton("Log");
//			log_Y_RadioButton->setChecked(multilayer->roughness_Plot_Options.y_Scale == log_Scale);
//		scale_Y_Layout->addWidget(log_Y_RadioButton);
//		connect(log_Y_RadioButton, &QRadioButton::clicked, this, [&]
//		{
//			if(log_Y_RadioButton->isChecked())
//			{
//				multilayer->roughness_Plot_Options.y_Scale = log_Scale;
//			}
//			plot_Data();
//		});

//		QButtonGroup* Y_ButtonGroup = new QButtonGroup;
//			Y_ButtonGroup->addButton(lin_Y_RadioButton);
//			Y_ButtonGroup->addButton(log_Y_RadioButton);

		// ----------------------------------------------------
		scale_GroupBox->adjustSize();
		scale_GroupBox->setFixedSize(scale_GroupBox->size());

		///------------------------------------------------------------------------------------------
		// bottom part
		///------------------------------------------------------------------------------------------
		units_GroupBox = new QGroupBox;
			left_Layout->addWidget(units_GroupBox);
		QGridLayout* units_Layout = new QGridLayout(units_GroupBox);
			units_Layout->setAlignment(Qt::AlignLeft);
			units_Layout->setSpacing(2);
			units_Layout->setContentsMargins(8,5,6,5);

		// ----------------------------------------------------

		cursor_Cordinate_CheckBox = new QCheckBox("Show cursor position");
			cursor_Cordinate_CheckBox->setChecked(multilayer->roughness_Plot_Options.show_Cursor_Position);
		units_Layout->addWidget(cursor_Cordinate_CheckBox,0,0,1,2);
		connect(cursor_Cordinate_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->roughness_Plot_Options.show_Cursor_Position = cursor_Cordinate_CheckBox->isChecked();

			// other profiles obtain the same option : synchronization
			for(Roughness_Plot* roughness_Plot : roughness_Plots_Window->roughness_Plot_Vector)
			{
				roughness_Plot->multilayer->roughness_Plot_Options.show_Cursor_Position = multilayer->roughness_Plot_Options.show_Cursor_Position;
				roughness_Plot->cursor_Cordinate_CheckBox->blockSignals(true);
				roughness_Plot->cursor_Cordinate_CheckBox->setChecked(roughness_Plot->multilayer->roughness_Plot_Options.show_Cursor_Position);
				roughness_Plot->cursor_Cordinate_CheckBox->blockSignals(false);
			}
			global_Multilayer_Approach->reopen_Roughness_Plots();
		});

		// ----------------------------------------------------

		QLabel* argument_Units_Label = new QLabel("Argument units ");
			units_Layout->addWidget(argument_Units_Label,1,0);

		argument_ComboBox = new QComboBox;
			argument_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			argument_ComboBox->addItems(spatial_Frequency_Units_List);
			argument_ComboBox->setCurrentText(multilayer->roughness_Plot_Options.local_frequency_units);
		units_Layout->addWidget(argument_ComboBox,1,1);
		connect(argument_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			multilayer->roughness_Plot_Options.local_frequency_units = argument_ComboBox->currentText();

			double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_frequency_units);
			min_X_LineEdit->setText(Locale.toString(multilayer->roughness_Plot_Options.x_Min/coeff));
			max_X_LineEdit->setText(Locale.toString(multilayer->roughness_Plot_Options.x_Max/coeff));
			x_Units_label->setText(" "+multilayer->roughness_Plot_Options.local_frequency_units);

			plot_Data(true);
		});

		// ----------------------------------------------------

		QLabel* PSD_1D_Units_Label = new QLabel("PDS 1D units ");
			units_Layout->addWidget(PSD_1D_Units_Label,2,0);

		PSD_1D_ComboBox = new QComboBox;
			PSD_1D_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			PSD_1D_ComboBox->addItems(PSD_1D_Value_Units_List);
			PSD_1D_ComboBox->setCurrentText(multilayer->roughness_Plot_Options.local_PSD_1D_units);
		units_Layout->addWidget(PSD_1D_ComboBox,2,1);
		connect(PSD_1D_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			multilayer->roughness_Plot_Options.local_PSD_1D_units = PSD_1D_ComboBox->currentText();
			if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
				plot_Data(true);
			}
		});

		// ----------------------------------------------------

		QLabel* PSD_2D_Units_Label = new QLabel("PDS 2D units ");
			units_Layout->addWidget(PSD_2D_Units_Label,3,0);

		PSD_2D_ComboBox = new QComboBox;
			PSD_2D_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
			PSD_2D_ComboBox->addItems(PSD_2D_Value_Units_List);
			PSD_2D_ComboBox->setCurrentText(multilayer->roughness_Plot_Options.local_PSD_2D_units);
		units_Layout->addWidget(PSD_2D_ComboBox,3,1);
		connect(PSD_2D_ComboBox, &QComboBox::currentTextChanged, this, [=]
		{
			multilayer->roughness_Plot_Options.local_PSD_2D_units = PSD_2D_ComboBox->currentText();
			if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_2D) {
				plot_Data(true);
			}
		});

		// ----------------------------------------------------

		units_GroupBox->adjustSize();
		units_GroupBox->setFixedSize(units_GroupBox->size());


		value_Type_GroupBox->setFixedWidth(interface_GroupBox->width());
		scale_GroupBox->setFixedWidth(interface_GroupBox->width());
		units_GroupBox->setFixedWidth(interface_GroupBox->width());
	// - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// bit of initialization
	PSD_1D_RadioButton->clicked(PSD_1D_RadioButton->isChecked());
	PSD_2D_RadioButton->clicked(PSD_2D_RadioButton->isChecked());

	custom_Plot->replot();
}

void Roughness_Plot::create_Plot_Frame_And_Scale()
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
		if(multilayer->roughness_Plot_Options.x_Scale == lin_Scale)
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
		if(multilayer->roughness_Plot_Options.x_Scale == log_Scale)
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
		if(multilayer->roughness_Plot_Options.y_Scale == lin_Scale)
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
		if(multilayer->roughness_Plot_Options.y_Scale == log_Scale)
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
			multilayer->roughness_Plot_Options.old_X_Begin = custom_Plot->xAxis->range().lower;
			multilayer->roughness_Plot_Options.old_X_End   = custom_Plot->xAxis->range().upper;
		});
		connect(custom_Plot->yAxis, static_cast<void(QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), this, [=](const QCPRange&)
		{
			multilayer->roughness_Plot_Options.old_Y_Begin = custom_Plot->yAxis->range().lower;
			multilayer->roughness_Plot_Options.old_Y_End   = custom_Plot->yAxis->range().upper;
		});
	}
}

void Roughness_Plot::calculate_Profile()
{
	double nu_Min = min(multilayer->roughness_Plot_Options.x_Min,multilayer->roughness_Plot_Options.x_Max);
	double nu_Max = max(multilayer->roughness_Plot_Options.x_Min,multilayer->roughness_Plot_Options.x_Max);

	num_Plot_Points = num_roughness_plot_points;

	arg.clear();
	top_Surface_Val.clear();
	current_Interface_Val.clear();
	substrate_Surface_Val.clear();

	arg.resize(num_Plot_Points);
	top_Surface_Val.resize(num_Plot_Points);
	current_Interface_Val.resize(num_Plot_Points);
	substrate_Surface_Val.resize(num_Plot_Points);

	top_Surface_Plot_Vector.resize(num_Plot_Points);
	current_Interface_Plot_Vector.resize(num_Plot_Points);
	substrate_Surface_Plot_Vector.resize(num_Plot_Points);


	double val_Coeff = 1;
	double arg_Coeff = spatial_Frequency_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_frequency_units);

	if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
		val_Coeff = PSD_1D_Value_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_PSD_1D_units);
	} else	{
		val_Coeff = PSD_2D_Value_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_PSD_2D_units);
	}

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
		Global_Variables::fill_Vector_With_Log_Step(arg, nu_Min, nu_Max, num_Plot_Points);
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

void Roughness_Plot::calc_PSD_For_Interface(int interface_Index, QVector<double>& value_Vector, bool& use_Interface, QString& material, double& sigma_Eff, double nu_Min, double nu_Max)
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

	/// independent PSD without measured
	if( interface_Index == media_Counter-1 ||								// if substrate or
		!multilayer->imperfections_Model.use_Common_Roughness_Function)		// if any independent interface
	{
		current_Node->calculate_PSD_Factor(multilayer->imperfections_Model);

		/// creating FG splines. additional checks inside
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
			current_Node->create_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model);
		} else {
			current_Node->create_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
		}

		/// choosing base PSD functions
		double (*PSD_Func_from_nu)(double, double, double, double, gsl_spline*, gsl_interp_accel*);
		double factor = 1;
		gsl_spline* spline_PSD_FG;
		gsl_interp_accel* acc_PSD_FG;

		if(multilayer->imperfections_Model.PSD_Model == ABC_Model) {
			if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
				PSD_Func_from_nu = &Global_Variables::PSD_ABC_1D_from_nu;
				factor = current_Data.PSD_ABC_1D_Factor;
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
					PSD_Func_from_nu = &Global_Variables::PSD_Real_Gauss_1D_from_nu;
					factor = current_Data.PSD_Real_Gauss_1D_Factor;
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
				value_Vector[i] = PSD_Func_from_nu(factor, xi, alpha, arg[i], spline_PSD_FG, acc_PSD_FG);
			} else {
				value_Vector[i] = 0;
			}
		}

		/// sigma_effective calculation
		double sigma_2 = current_Node->combined_Effective_Sigma_2(multilayer->imperfections_Model, sigma, xi, alpha, calc_Nu_Min, calc_Nu_Max, multilayer->roughness_Plot_Options.PSD_Type);
		sigma_Eff = sqrt(sigma_2);

		/// clear FG splines
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
			current_Node->clear_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model);
		} else {
			current_Node->clear_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
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
				current_Node->create_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model);
				current_Node->create_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
				current_Node->create_Spline_PSD_Combined_1D(multilayer->imperfections_Model);

				for(int i=0; i<num_Plot_Points; i++)
				{
					if(arg[i] <= multilayer->imperfections_Model.nu_Limit) {
						value_Vector[i] = gsl_spline_eval(current_Node->spline_PSD_Combined_1D, arg[i], current_Node->acc_PSD_Combined_1D);
					} else {
						value_Vector[i] = 0;
					}
				}

				/// sigma_effective calculation
				double sigma_2 = current_Node->combined_Effective_Sigma_2_From_Spline(multilayer->imperfections_Model, calc_Nu_Min, calc_Nu_Max, current_Node->spline_PSD_Combined_1D, current_Node->acc_PSD_Combined_1D, multilayer->roughness_Plot_Options.PSD_Type);
				sigma_Eff = sqrt(sigma_2);

				current_Node->clear_Spline_PSD_Combined_1D(multilayer->imperfections_Model);
				current_Node->clear_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
				current_Node->clear_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model);
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
			double sigma_2 = current_Node->combined_Effective_Sigma_2(multilayer->imperfections_Model, sigma, xi, alpha, calc_Nu_Min, calc_Nu_Max, multilayer->roughness_Plot_Options.PSD_Type);
			sigma_Eff = sqrt(sigma_2);
		}
		/// clear measured spline
		current_Node->clear_Spline_PSD_Measured(multilayer->imperfections_Model);
	}

	/// gaussian peak
	if( multilayer->imperfections_Model.add_Gauss_Peak &&					// if have peak
	   (interface_Index == media_Counter-1 ||								// if substrate or
		!multilayer->imperfections_Model.use_Common_Roughness_Function))	// if any independent interface
	{
		current_Node->calculate_PSD_Factor(multilayer->imperfections_Model);

		/// creating peak spline
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
			current_Node->create_Spline_PSD_Peak(multilayer->imperfections_Model);
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
		double sigma_2 = current_Node->combined_Effective_Sigma_2_Peak(calc_Nu_Min, calc_Nu_Max, multilayer->roughness_Plot_Options.PSD_Type);
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

		substrate_Node->calculate_PSD_Factor(multilayer->imperfections_Model);
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D)
		{
			/// 1D
			substrate_Node->create_Spline_PSD_Fractal_Gauss_1D	(multilayer->imperfections_Model);
			substrate_Node->create_Spline_PSD_Fractal_Gauss_2D	(multilayer->imperfections_Model);
			substrate_Node->create_Spline_PSD_Combined_1D		(multilayer->imperfections_Model);
			substrate_Node->create_Spline_PSD_Peak				(multilayer->imperfections_Model);
			substrate_Node->create_Spline_PSD_Measured			(multilayer->imperfections_Model);
			substrate_Node->create_Spline_PSD_Linear_Growth_2D	(multilayer->imperfections_Model, media_Data_Map_Vector, interface_Index-1);
			substrate_Node->create_Spline_PSD_Linear_Growth_1D	(multilayer->imperfections_Model);

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
			double sigma_2 = substrate_Node->combined_Effective_Sigma_2_From_Spline(multilayer->imperfections_Model, calc_Nu_Min, calc_Nu_Max, substrate_Node->spline_PSD_Linear_Growth_1D, substrate_Node->acc_PSD_Linear_Growth_1D, multilayer->roughness_Plot_Options.PSD_Type);
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
			substrate_Node->create_Spline_PSD_Linear_Growth_2D	(multilayer->imperfections_Model, media_Data_Map_Vector, interface_Index-1);

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
			double sigma_2 = substrate_Node->combined_Effective_Sigma_2_From_Spline(multilayer->imperfections_Model, calc_Nu_Min, calc_Nu_Max, substrate_Node->spline_PSD_Linear_Growth_2D, substrate_Node->acc_PSD_Linear_Growth_2D, multilayer->roughness_Plot_Options.PSD_Type);
			sigma_Eff = sqrt(sigma_2);
			///--------------------------------------------------------------------------------------------------------

			substrate_Node->clear_Spline_PSD_Fractal_Gauss_2D	(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Measured			(multilayer->imperfections_Model);
			substrate_Node->clear_Spline_PSD_Linear_Growth_2D	(multilayer->imperfections_Model);
		}
	}
}

void Roughness_Plot::plot_Data(bool recalculate_Profile)
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
	if(multilayer->roughness_Plot_Options.y_Scale == lin_Scale )
	{
		QVector<double> val_Final;

		if(use_Top_Surface)		  val_Final.append(top_Surface_Val);
		if(use_Current_Interface) val_Final.append(current_Interface_Val);
		if(use_Substrate_Surface) val_Final.append(substrate_Surface_Val);

		if(val_Final.size()==0) val_Final = {0,1};

		double minimum_Raw = *std::min_element(val_Final.constBegin(), val_Final.constEnd());
		double maximum_Raw = *std::max_element(val_Final.constBegin(), val_Final.constEnd());

		minimum = minimum_Raw - (maximum_Raw-minimum_Raw)*0.05;
		maximum = maximum_Raw + (maximum_Raw-minimum_Raw)*0.08;
	}
	if(multilayer->roughness_Plot_Options.y_Scale == log_Scale )
	{
		QVector<double> val_Non_Zero;

		if(use_Top_Surface)		  val_Non_Zero.append(top_Surface_Val);
		if(use_Current_Interface) val_Non_Zero.append(current_Interface_Val);
		if(use_Substrate_Surface) val_Non_Zero.append(substrate_Surface_Val);

		for(int i=val_Non_Zero.size()-1; i>=0; i--)
		{
			if(val_Non_Zero[i]<default_logarithmic_threshold_PSD) {val_Non_Zero.remove(i);}
		}

		if(val_Non_Zero.size()==0) val_Non_Zero = {0.1,10};

		double minimum_Raw_Non_Zero = *std::min_element(val_Non_Zero.constBegin(), val_Non_Zero.constEnd());
		double maximum_Raw_Non_Zero = *std::max_element(val_Non_Zero.constBegin(), val_Non_Zero.constEnd());

		minimum = minimum_Raw_Non_Zero/1;
		maximum = maximum_Raw_Non_Zero*3;

		minimum = max(minimum,maximum/pow(10,multilayer->roughness_Plot_Options.orders_To_Show));
	}

	// units coefficients
	double arg_Coeff = spatial_Frequency_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_frequency_units);
	double val_Coeff = 1;
	if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
		val_Coeff = PSD_1D_Value_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_PSD_1D_units);
	} else	{
		val_Coeff = PSD_2D_Value_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_PSD_2D_units);
	}

	if(multilayer->roughness_Plot_Options.rescale_X){ custom_Plot->xAxis->setRange(arg.front()/arg_Coeff, arg.back()/arg_Coeff);	}
	else											{ custom_Plot->xAxis->setRange(multilayer->roughness_Plot_Options.old_X_Begin, multilayer->roughness_Plot_Options.old_X_End);}
	if(multilayer->roughness_Plot_Options.rescale_Y){ custom_Plot->yAxis->setRange(minimum/val_Coeff, maximum/val_Coeff);			}
	else											{ custom_Plot->yAxis->setRange(multilayer->roughness_Plot_Options.old_Y_Begin, multilayer->roughness_Plot_Options.old_Y_End);}

	// labels
	custom_Plot->xAxis->setLabel(Nu_Sym + ", "+multilayer->roughness_Plot_Options.local_frequency_units);
	if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
		custom_Plot->yAxis->setLabel("PSD 1D, " + multilayer->roughness_Plot_Options.local_PSD_1D_units);
	} else {
		custom_Plot->yAxis->setLabel("PSD 2D, " + multilayer->roughness_Plot_Options.local_PSD_2D_units);
	}

	custom_Plot->replot();
}

// if common PSD for all interfaces: calculate for substrate only
void Roughness_Plot::lock_Interfaces()
{
	if( multilayer->imperfections_Model.use_Common_Roughness_Function &&(
		(multilayer->imperfections_Model.vertical_Correlation == full_Correlation    || multilayer->imperfections_Model.vertical_Correlation == zero_Correlation                 ) ||
		(multilayer->imperfections_Model.vertical_Correlation == partial_Correlation && multilayer->imperfections_Model.inheritance_Model == replication_Factor_Inheritance_Model)
		))
	{
		use_Top_Surface = false;
		use_Current_Interface = false;
		use_Substrate_Surface = true;

		show_Top_Surface_CheckBox->setDisabled(true);
		show_Interface_by_Number_CheckBox->setDisabled(true);
		interface_by_Number_ComboBox->setDisabled(true);
		show_Substrate_Surface_CheckBox->setDisabled(true);
	} else {
		use_Top_Surface = multilayer->roughness_Plot_Options.show_Top_Surface;
		use_Current_Interface = multilayer->roughness_Plot_Options.show_Interface_With_Number;
		use_Substrate_Surface = multilayer->roughness_Plot_Options.show_Substrate_Surface;

		show_Top_Surface_CheckBox->setDisabled(false);
		show_Interface_by_Number_CheckBox->setDisabled(false);
		interface_by_Number_ComboBox->setDisabled(!multilayer->roughness_Plot_Options.show_Interface_With_Number);
		show_Substrate_Surface_CheckBox->setDisabled(false);
	}
}
