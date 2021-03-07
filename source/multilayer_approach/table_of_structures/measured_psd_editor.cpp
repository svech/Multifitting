#include "measured_psd_editor.h"

Measured_PSD_Editor::Measured_PSD_Editor(Multilayer* multilayer,
										 QString PSD_Type,
										 QPushButton* PSD_Button,
										 MyDoubleSpinBox* PSD_Sigma_Lineedit,
										 QWidget* parent):
	multilayer(multilayer),
	PSD_Type(PSD_Type),
	psd_Data(PSD_Type == PSD_Type_1D ? multilayer->imperfections_Model.PSD_1D : multilayer->imperfections_Model.PSD_2D),
	PSD_Button(PSD_Button),
	PSD_Sigma_Lineedit(PSD_Sigma_Lineedit),
	QDialog(parent)
{
	psd_Data.PSD_Type = PSD_Type;

	setWindowTitle("Load "+PSD_Type);
	create_Main_Layout();
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
	setAcceptDrops(true);

	PSD_Sigma_Lineedit->valueChanged(PSD_Sigma_Lineedit->value());
}

Measured_PSD_Editor::~Measured_PSD_Editor()
{
}

void Measured_PSD_Editor::closeEvent(QCloseEvent *event)
{
	PSD_Button->setProperty(is_Opened_Property,false);
	event->accept();
}

void Measured_PSD_Editor::dragEnterEvent(QDragEnterEvent* event)
{
	if(event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void Measured_PSD_Editor::dropEvent(QDropEvent* event)
{
	foreach (const QUrl& url, event->mimeData()->urls())
	{
		QString fileName = url.toLocalFile();
		path_Line_Edit->setText(fileName);
		psd_Data.filepath = fileName;
		path_Line_Edit->returnPressed();
	}
}

void Measured_PSD_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
		main_Layout->setContentsMargins(4,4,4,0);
		main_Layout->setSpacing(0);

	create_Plot();

	bottom_Widget = new QWidget;
	main_Layout->addWidget(bottom_Widget);
	bottom_Widget_Layout = new QVBoxLayout(bottom_Widget);
		bottom_Widget_Layout->setContentsMargins(0,0,0,0);
		bottom_Widget_Layout->setSpacing(0);

	create_Plot_Options_Box();
	create_Filepath_Box();
	create_PSD_Options_Box();
	create_Buttons();

	bottom_Widget->adjustSize();
	bottom_Widget->setFixedHeight(bottom_Widget->height());

	// shortcuts
	Global_Variables::create_Shortcuts(this);
}

void Measured_PSD_Editor::create_Plot()
{
	custom_Plot = new QCustomPlot;
		custom_Plot->setMinimumHeight(300);
		custom_Plot->setMinimumWidth(450);
	main_Layout->addWidget(custom_Plot);

	custom_Plot->setNoAntialiasingOnDrag(false);
	custom_Plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom);

	custom_Plot->clearGraphs();
	custom_Plot->addGraph();

	// frame and axes
	custom_Plot->axisRect()->setupFullAxesBox(true);
	custom_Plot->xAxis2->setVisible(true);
	custom_Plot->xAxis2->setTickLabels(false);
	custom_Plot->yAxis2->setVisible(true);
	custom_Plot->yAxis2->setTickLabels(true);

	custom_Plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
	QSharedPointer<QCPAxisTickerLog> logTickerX(new QCPAxisTickerLog);
	custom_Plot->xAxis->setTicker(logTickerX);
	custom_Plot->xAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
	custom_Plot->xAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

	custom_Plot->xAxis2->setScaleType(QCPAxis::stLogarithmic);
	custom_Plot->xAxis2->setTicker(logTickerX);
	custom_Plot->xAxis2->setNumberFormat("eb");
	custom_Plot->xAxis2->setNumberPrecision(0);

	custom_Plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
	QSharedPointer<QCPAxisTickerLog> logTickerY(new QCPAxisTickerLog);
	custom_Plot->yAxis->setTicker(logTickerY);
	custom_Plot->yAxis->setNumberFormat("eb");
	custom_Plot->yAxis->setNumberPrecision(0);

	custom_Plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
	custom_Plot->yAxis2->setTicker(logTickerY);
	custom_Plot->yAxis2->setNumberFormat("eb");
	custom_Plot->yAxis2->setNumberPrecision(0);

	// set pen
	custom_Plot->graph()->setPen(QPen(QColor(255, 0, 0),1.5));

	// set grid style
	QPen pen = custom_Plot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
	custom_Plot->yAxis->grid()->setSubGridVisible(true);
	custom_Plot->xAxis->grid()->setSubGridVisible(true);
	custom_Plot->yAxis->grid()->setPen(pen);
	custom_Plot->xAxis->grid()->setPen(pen);

	// default ranges
	custom_Plot->xAxis->setRange(1e-2, 1e2);
	custom_Plot->yAxis->setRange(1e-3, 1e3);

	plot_Data();
	refresh_Labels();
}

void Measured_PSD_Editor::plot_Data()
{
	int data_Count = psd_Data.argument.size();
	QVector<QCPGraphData> data_To_Plot(data_Count);
	double min_Y = DBL_MAX;
	double max_Y = -DBL_MAX;
	for (int i=0; i<data_Count; ++i)
	{
		data_To_Plot[i].key   = psd_Data.raw_Argument[i];
		data_To_Plot[i].value = psd_Data.raw_Value[i];

		if((max_Y<data_To_Plot[i].value) && (data_To_Plot[i].value > DBL_MIN)) {max_Y=data_To_Plot[i].value;}
		if((min_Y>data_To_Plot[i].value) && (data_To_Plot[i].value > DBL_MIN)) {min_Y=data_To_Plot[i].value;}
	}
	custom_Plot->graph()->data()->set(data_To_Plot);
	double range_Factor = 2;
	double min_X = min(psd_Data.raw_Argument.front(),psd_Data.raw_Argument.back());
	double max_X = max(psd_Data.raw_Argument.front(),psd_Data.raw_Argument.back());
	custom_Plot->xAxis->setRange(min_X/range_Factor, max_X*range_Factor);
	custom_Plot->yAxis->setRange(min_Y/range_Factor, max_Y*range_Factor);
	custom_Plot->replot();
}

void Measured_PSD_Editor::refresh_Labels()
{
	custom_Plot->xAxis->setLabel(Nu_Sym + ", " + psd_Data.argument_Units);
	custom_Plot->yAxis->setLabel(PSD_Type + ", " + psd_Data.value_Units);
	custom_Plot->replot();
}

void Measured_PSD_Editor::create_Plot_Options_Box()
{

}

void Measured_PSD_Editor::create_PSD_Options_Box()
{
	QGroupBox* PSD_Units_GroupBox = new QGroupBox;
	bottom_Widget_Layout->addWidget(PSD_Units_GroupBox);

	QGridLayout* PSD_Units_GroupBox_Layout = new QGridLayout(PSD_Units_GroupBox);
		PSD_Units_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	QLabel* argument_Units_Label = new QLabel("Spatial frequency units");
	PSD_Units_GroupBox_Layout->addWidget(argument_Units_Label,0,0);

	QComboBox* argument_Units_Combobox = new QComboBox;
		argument_Units_Combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		argument_Units_Combobox->addItems(PSD_Argument_Units_List);
		argument_Units_Combobox->setCurrentText(psd_Data.argument_Units);
	PSD_Units_GroupBox_Layout->addWidget(argument_Units_Combobox,0,1);
	connect(argument_Units_Combobox,	&QComboBox::currentTextChanged, this, [=]
	{
		psd_Data.argument_Units = argument_Units_Combobox->currentText();

		double arg_Coeff = PSD_Argument_Coefficients_Map.value(psd_Data.argument_Units);
		for(int i=0; i<psd_Data.argument.size(); i++)
		{
			psd_Data.argument[i] = psd_Data.raw_Argument[i]*arg_Coeff;
		}
		PSD_Sigma_Lineedit->valueChanged(PSD_Sigma_Lineedit->value());
		refresh_Labels();
	});

	QLabel* value_Units_Label = new QLabel("PSD value units");
	PSD_Units_GroupBox_Layout->addWidget(value_Units_Label,1,0);

	QComboBox* value_Units_Combobox = new QComboBox;
		value_Units_Combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		if(PSD_Type == PSD_Type_1D)	value_Units_Combobox->addItems(PSD_1D_Value_Units_List);
		if(PSD_Type == PSD_Type_2D)	value_Units_Combobox->addItems(PSD_2D_Value_Units_List);
		value_Units_Combobox->setCurrentText(psd_Data.value_Units);
	PSD_Units_GroupBox_Layout->addWidget(value_Units_Combobox,1,1);
	connect(value_Units_Combobox,	&QComboBox::currentTextChanged, this, [=]
	{
		psd_Data.value_Units = value_Units_Combobox->currentText();

		double val_Coeff = PSD_Type == PSD_Type_1D ? PSD_1D_Value_Coefficients_Map.value(psd_Data.value_Units) : PSD_2D_Value_Coefficients_Map.value(psd_Data.value_Units);
		for(int i=0; i<psd_Data.argument.size(); i++)
		{
			psd_Data.value[i] = psd_Data.raw_Value[i]*val_Coeff;
		}
		PSD_Sigma_Lineedit->valueChanged(PSD_Sigma_Lineedit->value());
		refresh_Labels();
	});
}

void Measured_PSD_Editor::create_Filepath_Box()
{
	QGroupBox* filepath_GroupBox = new QGroupBox;
	bottom_Widget_Layout->addWidget(filepath_GroupBox);

	QHBoxLayout* filepath_GroupBox_Layout = new QHBoxLayout(filepath_GroupBox);
		filepath_GroupBox_Layout->setAlignment(Qt::AlignLeft);

	QLabel* filepath_Label = new QLabel("File path");
	filepath_GroupBox_Layout->addWidget(filepath_Label,0);

	path_Line_Edit = new QLineEdit;
		path_Line_Edit->setText(psd_Data.filepath);
	filepath_GroupBox_Layout->addWidget(path_Line_Edit,0);
	connect(path_Line_Edit, &QLineEdit::returnPressed, this, [=]
	{
		read_PSD_File();
		plot_Data();
	});

	QPushButton* browse_Button = new QPushButton("Browse...");
	filepath_GroupBox_Layout->addWidget(browse_Button,0);
	connect(browse_Button, &QPushButton::clicked, this, [=]
	{
		QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "Find PSD", path_Line_Edit->text(), "Text data (*.txt *.dat *.xy);;All files (*.*)"));
		if (!directory.isEmpty())
		{
			path_Line_Edit->setText(directory);
			psd_Data.filepath = directory;
			path_Line_Edit->returnPressed();
		}
	});
}

void Measured_PSD_Editor::create_Buttons()
{
	QHBoxLayout* button_Layout = new QHBoxLayout();
		button_Layout->setSpacing(10);
		button_Layout->setAlignment(Qt::AlignCenter);
	bottom_Widget_Layout->addLayout(button_Layout);

	// close
	{
		QPushButton* close_Button = new QPushButton("Close", this);
			close_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			close_Button->setDefault(true);
			button_Layout->addWidget(close_Button,0,Qt::AlignCenter);
		connect(close_Button, &QPushButton::clicked, this, [=]{ close(); });
	}
	// clear
	{
		QPushButton* clear_Button = new QPushButton("Delete PSD");
			clear_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		button_Layout->addWidget(clear_Button,0,Qt::AlignCenter);
		connect(clear_Button, &QPushButton::clicked, this, [=]
		{
			QMessageBox::StandardButton reply = QMessageBox::question(this, "Clear", "Delete this PSD?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
			if (reply == QMessageBox::Yes)
			{
				psd_Data.raw_Argument.clear();
				psd_Data.raw_Value.clear();
				psd_Data.argument.clear();
				psd_Data.value.clear();
				custom_Plot->clearGraphs();
				custom_Plot->replot();
				psd_Data.filepath.clear();
				path_Line_Edit->clear();
				PSD_Button->setStyleSheet("background-color: white");
				PSD_Sigma_Lineedit->valueChanged(PSD_Sigma_Lineedit->value());
			}
		});
	}
}

void Measured_PSD_Editor::read_PSD_File()
{
	QFileInfo filename = psd_Data.filepath;

	// reading whole file to lines_list
	QFile input_File(filename.absoluteFilePath());
	QString temp_Line = "not empty now";

	QStringList lines_List;
	if(input_File.open(QIODevice::ReadOnly))
	{
		QTextStream input_Stream(&input_File);
		lines_List.clear();
		while ( !input_Stream.atEnd() )
		{
			temp_Line=input_Stream.readLine();
			lines_List.append(temp_Line);
		}
		input_File.close();
	}

	// parsing lines
	psd_Data.raw_Argument.clear();
	psd_Data.raw_Value.clear();
	psd_Data.argument.clear();
	psd_Data.value.clear();	

	double arg_Coeff = PSD_Argument_Coefficients_Map.value(psd_Data.argument_Units);
	double val_Coeff = PSD_Type == PSD_Type_1D ? PSD_1D_Value_Coefficients_Map.value(psd_Data.value_Units) : PSD_2D_Value_Coefficients_Map.value(psd_Data.value_Units);

	for(int line_Index=0; line_Index<lines_List.size(); ++line_Index)
	{
		QString temp_Line = lines_List[line_Index];
		temp_Line = temp_Line.simplified();

		// check if not header line (starts from digit)
		bool is_Decimal = false;
		QString(temp_Line[0]).toInt(&is_Decimal);

		// split line into "numbers"
		QStringList potentional_Numbers = temp_Line.split(delimiters,QString::SkipEmptyParts);

		if(is_Decimal && potentional_Numbers.size()>=2)	// temp_Line[0]!=';' && temp_Line[0]!='#' && numbers.size()>0    less common
		{
			bool ok_To_Double = false;

			double temp_Argument = QString(potentional_Numbers[0]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas are considered as dots
			double temp_Value    = QString(potentional_Numbers[1]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas are considered as dots

			if(!ok_To_Double) goto skip_line_label;

			// argument should be monotonic
			if(psd_Data.raw_Argument.size()>=2)
			{
				if(psd_Data.raw_Argument[psd_Data.raw_Argument.size()-1]>psd_Data.raw_Argument[psd_Data.raw_Argument.size()-2]) // increasing argument is allowed
				{
					if(temp_Argument <= psd_Data.raw_Argument.back()) goto skip_line_label; // read only monotonical arguments
				}
				if(psd_Data.raw_Argument[psd_Data.raw_Argument.size()-1]<psd_Data.raw_Argument[psd_Data.raw_Argument.size()-2]) // decreasing argument is allowed
				{
					if(temp_Argument >= psd_Data.raw_Argument.back()) goto skip_line_label; // read only monotonical arguments
				}
			}

			psd_Data.raw_Argument.push_back(temp_Argument);
			psd_Data.raw_Value.push_back(temp_Value);

			psd_Data.argument.push_back(temp_Argument*arg_Coeff);
			psd_Data.value.push_back(temp_Value*val_Coeff);

			// this line may be skipped
			skip_line_label: ok_To_Double = false;
		}
	}

	if(psd_Data.raw_Argument.size()>2)
		PSD_Button->setStyleSheet("QWidget { background: rgb(100, 255, 220); }");
	else
		PSD_Button->setStyleSheet("background-color: white");

	PSD_Sigma_Lineedit->valueChanged(PSD_Sigma_Lineedit->value());
}
