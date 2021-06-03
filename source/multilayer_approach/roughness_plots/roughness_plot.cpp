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

//	horizontall_Scrollbar = new QScrollBar(Qt::Horizontal);
//	connect(horizontall_Scrollbar, &QScrollBar::valueChanged, this, &Profile_Plot::horizontall_ScrollbarChanged);
//	connect(custom_Plot->xAxis, static_cast<void(QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), this, &Profile_Plot::x_Axis_RangeChanged);
//	horizontall_Scrollbar->blockSignals(true);

	// synchronize selection of graphs with selection of corresponding legend items:
//	connect(custom_Plot, &QCustomPlot::selectionChangedByUser, this, [=]
//	{
//		for (int i=0; i<custom_Plot->graphCount(); ++i)
//		{
//			QCPGraph* graph = custom_Plot->graph(i);
//			if(custom_Plot->legend->hasItemWithPlottable(graph))
//			{
//				QCPPlottableLegendItem* item = custom_Plot->legend->itemWithPlottable(graph);
//				if (item->selected() || graph->selected())
//				{
//					item->setSelected(true);
//					graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
//				}
//			}
//		}
//	});
//	custom_Plot->legend->setSelectableParts(QCPLegend::spItems);
//	custom_Plot->legend->setSelectedIconBorderPen(QPen(Qt::black, 1));
//	connect(custom_Plot, &QCustomPlot::plottableDoubleClick, this, &Profile_Plot::hide_Show_Other_Plots);

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

	main_Layout->addWidget(custom_Plot);
//	QVBoxLayout* plot_Bar_Layout = new QVBoxLayout;
//		plot_Bar_Layout->setContentsMargins(4,4,4,0);
//	main_Layout->addLayout(plot_Bar_Layout);
//	plot_Bar_Layout->addWidget(custom_Plot);
//	plot_Bar_Layout->addWidget(horizontall_Scrollbar);

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
			plot_Data(true);
		});

		show_Interface_by_Number_ComboBox = new QComboBox;
		QHBoxLayout* interface_by_Number_Layout = new QHBoxLayout;
			interface_by_Number_Layout->setAlignment(Qt::AlignLeft);
			interface_by_Number_Layout->addWidget(show_Interface_by_Number_ComboBox);
		interface_Layout->addLayout(interface_by_Number_Layout);
		connect(show_Interface_by_Number_ComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=]
		{
			multilayer->roughness_Plot_Options.num_Interface_To_Show = show_Interface_by_Number_ComboBox->currentIndex();
			plot_Data(true);
		});
		QLabel* interface_Label = new QLabel("  interface");
			interface_by_Number_Layout->addWidget(interface_Label);

		show_Substrate_Surface_CheckBox = new QCheckBox("Show substrate surface");
			show_Substrate_Surface_CheckBox->setChecked(multilayer->roughness_Plot_Options.show_Substrate_Surface);
		interface_Layout->addWidget(show_Substrate_Surface_CheckBox);
		connect(show_Substrate_Surface_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->roughness_Plot_Options.show_Substrate_Surface = show_Substrate_Surface_CheckBox->isChecked();
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
			min_X_LineEdit->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
			min_X_LineEdit->setFixedWidth(40);
		x_Range_layout->addWidget(min_X_LineEdit,0,Qt::AlignLeft);
		connect(min_X_LineEdit,  &QLineEdit::textEdited, this, [=]	{
			double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_frequency_units);
			multilayer->roughness_Plot_Options.x_Min = Locale.toDouble(min_X_LineEdit->text())*coeff;
		});

		QLabel* space_label = new QLabel(" - ");
			space_label->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
		x_Range_layout->addWidget(space_label,0,Qt::AlignLeft);

		max_X_LineEdit = new QLineEdit(Locale.toString(multilayer->roughness_Plot_Options.x_Max/coeff));
			max_X_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			max_X_LineEdit->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
			max_X_LineEdit->setFixedWidth(40);
		x_Range_layout->addWidget(max_X_LineEdit,0,Qt::AlignLeft);
		connect(max_X_LineEdit,  &QLineEdit::textEdited, this, [=]	{
			double coeff = spatial_Frequency_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_frequency_units);
			multilayer->roughness_Plot_Options.x_Max = Locale.toDouble(max_X_LineEdit->text())*coeff;
		});

		QLabel* x_Units_label = new QLabel(" "+multilayer->roughness_Plot_Options.local_frequency_units);
			x_Units_label->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
		x_Range_layout->addWidget(x_Units_label,0,Qt::AlignLeft);

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
	int num_Plot_Points = 1000; // by default, without measured PSD

	use_Top_Surface = false;
	use_Current_Interface = false;
	use_Substrate_Surface = false;

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

	double nu_Min = min(multilayer->roughness_Plot_Options.x_Min,multilayer->roughness_Plot_Options.x_Max);
	double nu_Max = max(multilayer->roughness_Plot_Options.x_Min,multilayer->roughness_Plot_Options.x_Max);

	/// preparing calc_Tree
	Calculation_Tree::fill_Tree_From_Scratch(calc_Tree, multilayer->structure_Tree->tree, multilayer);
	media_Counter = Calculation_Tree::get_Total_Num_Layers(calc_Tree.begin());

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

	Node* substrate_Node = flat_Calc_Tree.back();
	Data& substrate_Data = substrate_Node->struct_Data;
	substrate_Node->calculate_PSD_Factor(multilayer->imperfections_Model);

	/// creating FG splines
	Data fake_Measurement;
	fake_Measurement.measurement_Type = measurement_Types[Specular_Scan];
	if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
		substrate_Node->create_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model, fake_Measurement);
	} else {
		substrate_Node->create_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model, fake_Measurement);
	}

	/// choosing base PSD functions
	double (*PSD_Func_from_nu)(double, double, double, double, gsl_spline*, gsl_interp_accel*);
	double factor = 1;
	gsl_spline* spline_PSD_FG;
	gsl_interp_accel* acc_PSD_FG;
	double val_Coeff = 1;
	double arg_Coeff = spatial_Frequency_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_frequency_units);

	double xi    = substrate_Data.roughness_Model.cor_radius.value;
	double alpha = substrate_Data.roughness_Model.fractal_alpha.value;

	if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
		val_Coeff = PSD_1D_Value_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_PSD_1D_units);
	} else	{
		val_Coeff = PSD_2D_Value_Coefficients_Map.value(multilayer->roughness_Plot_Options.local_PSD_2D_units);
	}

	if(multilayer->imperfections_Model.PSD_Model == ABC_Model) {
		if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
			PSD_Func_from_nu = &Global_Variables::PSD_ABC_1D_from_nu;
			factor = substrate_Data.PSD_ABC_1D_Factor;
		} else {
			PSD_Func_from_nu = &Global_Variables::PSD_ABC_2D_from_nu;
			factor = substrate_Data.PSD_ABC_2D_Factor;
		}
	}
	if(multilayer->imperfections_Model.PSD_Model == fractal_Gauss_Model) {
		const double& alpha = substrate_Data.roughness_Model.fractal_alpha.value;

		if(abs(alpha-1)>DBL_EPSILON) {
			if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
				PSD_Func_from_nu = &Global_Variables::PSD_Fractal_Gauss_1D_from_nu;
				factor = 1;
				spline_PSD_FG = substrate_Node->spline_PSD_FG_1D;
				acc_PSD_FG = substrate_Node->acc_PSD_FG_1D;
			} else {
				PSD_Func_from_nu = &Global_Variables::PSD_Fractal_Gauss_2D_from_nu;
				factor = 1;
				spline_PSD_FG = substrate_Node->spline_PSD_FG_2D;
				acc_PSD_FG = substrate_Node->acc_PSD_FG_2D;
			}
		} else {
			if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
				PSD_Func_from_nu = &Global_Variables::PSD_Real_Gauss_1D_from_nu;
				factor = substrate_Data.PSD_Real_Gauss_1D_Factor;
			} else {
				PSD_Func_from_nu = &Global_Variables::PSD_Real_Gauss_2D_from_nu;
				factor = substrate_Data.PSD_Real_Gauss_2D_Factor;
			}
		}
	}

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
		double power = 1./(num_Plot_Points-1);
		double step_Factor = pow(nu_Max/nu_Min,power);

		arg[0] = nu_Min;
		for(int i=1; i<num_Plot_Points; i++)
		{
			arg[i] = arg[i-1]*step_Factor;
		}
	}


	// if common PSD for all interfaces
	if( multilayer->imperfections_Model.use_Common_Roughness_Function &&
	   (multilayer->imperfections_Model.vertical_Correlation == full_Correlation ||
		multilayer->imperfections_Model.vertical_Correlation == zero_Correlation) )
	{
		use_Substrate_Surface = true;

		for(int i=0; i<num_Plot_Points; i++)
		{
			substrate_Surface_Val[i] = PSD_Func_from_nu(factor, xi, alpha, arg[i], spline_PSD_FG, acc_PSD_FG) / val_Coeff;
			arg[i] = arg[i] / arg_Coeff;
		}

		for(int i=0; i<num_Plot_Points; i++)
		{
			substrate_Surface_Plot_Vector[i].key = arg[i];
			substrate_Surface_Plot_Vector[i].value = substrate_Surface_Val[i];
		}
		custom_Plot->graph()->data()->set(substrate_Surface_Plot_Vector);
		custom_Plot->graph()->setPen(QPen(Qt::black, 2.0));
	}



//	struct_Data_Vector.resize(struct_Data_Counter);
//	media_Period_Index_Map_Vector.resize(struct_Data_Counter);

//	/// unwrap tree to vector of pointers
//	struct_Data_Pointer_Vector.resize(struct_Data_Counter);
//	Calculation_Tree::unwrap_Calc_Tree_Data(calc_Tree.begin(), struct_Data_Pointer_Vector, media_Period_Index_Map_Vector);

//	// copy
//	for(int struct_Index=0; struct_Index<struct_Data_Counter; struct_Index++)
//	{
//		struct_Data_Vector[struct_Index] = (*struct_Data_Pointer_Vector[struct_Index]);
//	}

//	fill_All_Data_From_Struct_Vector();

//	// thicknesses and boundaries position
//	boundary_Vector.resize(struct_Data_Vector.size()-1);	boundary_Vector.first() = 0;
//	thickness_Vector.resize(struct_Data_Vector.size()-2);
//	for(int i=0; i<thickness_Vector.size(); i++)
//	{
//		thickness_Vector[i] = struct_Data_Vector[i+1].thickness.value;
//		boundary_Vector[i+1] = boundary_Vector[i]+thickness_Vector[i];
//	}

	if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
		substrate_Node->clear_Spline_PSD_Fractal_Gauss_1D(multilayer->imperfections_Model);
	} else {
		substrate_Node->clear_Spline_PSD_Fractal_Gauss_2D(multilayer->imperfections_Model);
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
	}

	if(multilayer->roughness_Plot_Options.rescale_X){ custom_Plot->xAxis->setRange(arg.first(), arg.last());	}
	else											{ custom_Plot->xAxis->setRange(multilayer->roughness_Plot_Options.old_X_Begin, multilayer->roughness_Plot_Options.old_X_End);}
	if(multilayer->roughness_Plot_Options.rescale_Y){ custom_Plot->yAxis->setRange(minimum, maximum);			}
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
