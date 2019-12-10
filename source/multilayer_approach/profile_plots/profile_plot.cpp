#include "profile_plot.h"

Profile_Plot::Profile_Plot(Multilayer* multilayer, QWidget *parent) :
	multilayer(multilayer),
	QWidget(parent)
{
	create_Main_Layout();
}

void Profile_Plot::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setSpacing(0);
	main_Layout->setContentsMargins(4,4,4,0);

	custom_Plot = new QCustomPlot(this);

	create_Plot_Frame_And_Scale();
		main_Layout->addWidget(custom_Plot);
	plot_Data();
}

void Profile_Plot::create_Plot_Frame_And_Scale()
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
		{
			QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
			custom_Plot->xAxis->setScaleType(QCPAxis::stLinear);
			custom_Plot->xAxis->setTicker(linTicker);
			custom_Plot->xAxis->setNumberFormat("g");
			custom_Plot->xAxis->setNumberPrecision(4);
			custom_Plot->xAxis->setRange(-5, 25);

			custom_Plot->xAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->xAxis2->setTicker(linTicker);
			custom_Plot->xAxis2->setNumberFormat("g");
			custom_Plot->xAxis2->setNumberPrecision(4);
			custom_Plot->xAxis2->setRange(-5, 25);
		}
		/// Y axis
		{
			QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);

			custom_Plot->yAxis->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis->setTicker(linTicker);
			custom_Plot->yAxis->setNumberFormat("g");
			custom_Plot->yAxis->setNumberPrecision(4);
			custom_Plot->yAxis->setRange(-1, 1);

			custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis2->setTicker(linTicker);
			custom_Plot->yAxis2->setNumberFormat("g");
			custom_Plot->yAxis2->setNumberPrecision(4);
			custom_Plot->yAxis2->setRange(-1, 1);
		}
		// make range draggable and zoomable:
		custom_Plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom | /*QCP::iSelectAxes |*/ QCP::iSelectLegend);

		// make top right axes clones of bottom left axes:
		custom_Plot->xAxis2->setVisible(true);
		custom_Plot->yAxis2->setVisible(true);
		custom_Plot->xAxis2->setTickLabels(false);

		connect(custom_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->xAxis2, SLOT(setRange(QCPRange)));
		connect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));

	}
//	set_Title_Text();
	custom_Plot->replot();
}

void Profile_Plot::plot_Data(bool fast)
{
	if(!fast){create_Plot_Frame_And_Scale();}

	// TODO temporary
	int data_Count=1000;
	QVector<double> arg(data_Count);
	QVector<double> val(arg.size());
	for(int i=0; i<arg.size(); i++)
	{
		arg[i]=i/10.;
		val[i]=sin(arg[i]);
	}

	QVector<QCPGraphData> data_To_Plot(data_Count);
	double min = DBL_MAX;
	double max = 0;
	for (int i=0; i<data_Count; ++i)
	{
		data_To_Plot[i].key = arg[i];
		data_To_Plot[i].value = val[i];

		if(max<data_To_Plot[i].value) {max=data_To_Plot[i].value;}
		if(min>data_To_Plot[i].value) {min=data_To_Plot[i].value;}
	}

	if(!fast)
	{
		if(custom_Plot->graphCount()!=1)
		{
			custom_Plot->clearGraphs();
			custom_Plot->addGraph();
		}
//		custom_Plot->graph(0)->setPen(QPen(target_Curve->plot_Options_Experimental.color,target_Curve->plot_Options_Experimental.thickness));
		//custom_Plot->graph(0)->setBrush(QBrush(target_Curve->plot_Options.color));
		QCPScatterStyle scatter_Style;
//		scatter_Style.setShape(QCPScatterStyle::ScatterShape(target_Curve->plot_Options_Experimental.scatter_Shape));
//		scatter_Style.setSize(target_Curve->plot_Options_Experimental.scatter_Size);
		custom_Plot->graph(0)->setScatterStyle(scatter_Style);
	}

	custom_Plot->graph(0)->data()->set(data_To_Plot);
	custom_Plot->xAxis->setRange(arg[0], arg[data_Count-1]);
	custom_Plot->yAxis->setRange(min,max);
	//custom_Plot->yAxis2->setTickLabels(false);

//	if(!fast)
//		refresh_Labels();
//	else
		custom_Plot->replot();

}
