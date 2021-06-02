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

	custom_Plot->xAxis->setLabel(Nu_Sym+", "+multilayer->roughness_Plot_Options.local_frequency_units);
	if(multilayer->roughness_Plot_Options.PSD_Type == PSD_Type_1D) {
		custom_Plot->yAxis->setLabel("PSD 1D, " + multilayer->roughness_Plot_Options.local_PSD_1D_units);
	} else {
		custom_Plot->yAxis->setLabel("PSD 2D, " + multilayer->roughness_Plot_Options.local_PSD_2D_units);
	}
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
				custom_Plot->yAxis->setLabel("PSD 1D, " + multilayer->roughness_Plot_Options.local_PSD_1D_units);
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
				custom_Plot->yAxis->setLabel("PSD 2D, " + multilayer->roughness_Plot_Options.local_PSD_2D_units);
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
		// bottom part
		///------------------------------------------------------------------------------------------

		scale_GroupBox = new QGroupBox;
			left_Layout->addWidget(scale_GroupBox);
		QVBoxLayout* scale_Layout = new QVBoxLayout(scale_GroupBox);
			scale_Layout->setSpacing(2);
		scale_Layout->setContentsMargins(8,5,2,5);

		// ----------------------------------------------------

		cursor_Cordinate_CheckBox = new QCheckBox("Show cursor position");
			cursor_Cordinate_CheckBox->setChecked(multilayer->roughness_Plot_Options.show_Cursor_Position);
		scale_Layout->addWidget(cursor_Cordinate_CheckBox);
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
		rescale_X_CheckBox = new QCheckBox("Rescale X");
			rescale_X_CheckBox->setChecked(multilayer->roughness_Plot_Options.rescale_X);
		scale_Layout->addWidget(rescale_X_CheckBox);

		// ----------------------------------------------------
		QHBoxLayout* x_Range_layout = new QHBoxLayout;
			x_Range_layout->setAlignment(Qt::AlignLeft);
		scale_Layout->addLayout(x_Range_layout);

		QLabel* shift_label = new QLabel("     ");
		x_Range_layout->addWidget(shift_label,0,Qt::AlignLeft);

		min_X_LineEdit = new QLineEdit(Locale.toString(multilayer->roughness_Plot_Options.x_Min));
			min_X_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			min_X_LineEdit->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
			min_X_LineEdit->setFixedWidth(40);
		x_Range_layout->addWidget(min_X_LineEdit,0,Qt::AlignLeft);
		connect(min_X_LineEdit,  &QLineEdit::textEdited, this, [=]	{
			multilayer->roughness_Plot_Options.x_Min = Locale.toDouble(min_X_LineEdit->text());
		});

		QLabel* space_label = new QLabel(" - ");
			space_label->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
		x_Range_layout->addWidget(space_label,0,Qt::AlignLeft);

		max_X_LineEdit = new QLineEdit(Locale.toString(multilayer->roughness_Plot_Options.x_Max));
			max_X_LineEdit->setValidator(new QDoubleValidator(0, MAX_DOUBLE, MAX_PRECISION, this));
			max_X_LineEdit->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
			max_X_LineEdit->setFixedWidth(40);
		x_Range_layout->addWidget(max_X_LineEdit,0,Qt::AlignLeft);
		connect(max_X_LineEdit,  &QLineEdit::textEdited, this, [=]	{
			multilayer->roughness_Plot_Options.x_Max = Locale.toDouble(max_X_LineEdit->text());
		});

		QLabel* x_Units_label = new QLabel(" "+multilayer->roughness_Plot_Options.local_frequency_units);
			x_Units_label->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
		x_Range_layout->addWidget(x_Units_label,0,Qt::AlignLeft);

		connect(rescale_X_CheckBox, &QCheckBox::toggled, this, [=]
		{
			multilayer->roughness_Plot_Options.rescale_X = rescale_X_CheckBox->isChecked();

			min_X_LineEdit->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
			space_label->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
			max_X_LineEdit->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
			x_Units_label->setEnabled(multilayer->roughness_Plot_Options.rescale_X);
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
		QHBoxLayout* scale_X_Layout = new QHBoxLayout;
			scale_X_Layout->setAlignment(Qt::AlignLeft);
			scale_Layout->addLayout(scale_X_Layout);

		scale_X_Label = new QLabel("Scale X: ");
			scale_X_Layout->addWidget(scale_X_Label);

		// lin
		lin_X_RadioButton = new QRadioButton("Lin");
			lin_X_RadioButton->setChecked(multilayer->roughness_Plot_Options.x_Scale == lin_Scale);
		scale_X_Layout->addWidget(lin_X_RadioButton);
		connect(lin_X_RadioButton, &QRadioButton::clicked, this, [&]
		{
			if(lin_X_RadioButton->isChecked())
			{
				multilayer->roughness_Plot_Options.x_Scale = lin_Scale;
			}
			plot_Data();
		});

		// log
		log_X_RadioButton = new QRadioButton("Log");
			log_X_RadioButton->setChecked(multilayer->roughness_Plot_Options.x_Scale == log_Scale);
		scale_X_Layout->addWidget(log_X_RadioButton);
		connect(log_X_RadioButton, &QRadioButton::clicked, this, [&]
		{
			if(log_X_RadioButton->isChecked())
			{
				multilayer->roughness_Plot_Options.x_Scale = log_Scale;
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
			lin_Y_RadioButton->setChecked(multilayer->roughness_Plot_Options.y_Scale == lin_Scale);
		scale_Y_Layout->addWidget(lin_Y_RadioButton);
		connect(lin_Y_RadioButton, &QRadioButton::clicked, this, [&]
		{
			if(lin_Y_RadioButton->isChecked())
			{
				multilayer->roughness_Plot_Options.y_Scale = lin_Scale;
			}
			plot_Data();
		});

		// log
		log_Y_RadioButton = new QRadioButton("Log");
			log_Y_RadioButton->setChecked(multilayer->roughness_Plot_Options.y_Scale == log_Scale);
		scale_Y_Layout->addWidget(log_Y_RadioButton);
		connect(log_Y_RadioButton, &QRadioButton::clicked, this, [&]
		{
			if(log_Y_RadioButton->isChecked())
			{
				multilayer->roughness_Plot_Options.y_Scale = log_Scale;
			}
			plot_Data();
		});

		QButtonGroup* Y_ButtonGroup = new QButtonGroup;
			Y_ButtonGroup->addButton(lin_Y_RadioButton);
			Y_ButtonGroup->addButton(log_Y_RadioButton);

		// ----------------------------------------------------
		scale_GroupBox->adjustSize();
		scale_GroupBox->setFixedSize(scale_GroupBox->size());


		value_Type_GroupBox->setFixedWidth(interface_GroupBox->width());
		scale_GroupBox->setFixedWidth(interface_GroupBox->width());
	// - - - - - - - - - - - - - - - - - - - - - - - - - - -

	custom_Plot->replot();
}

void Roughness_Plot::create_Plot_Frame_And_Scale()
{

}

void Roughness_Plot::plot_Data(bool recalculate_Profile)
{
	custom_Plot->replot();
}
