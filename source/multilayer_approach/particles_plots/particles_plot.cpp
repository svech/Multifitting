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

}

void Particles_Plot::create_Plot_Frame_And_Scale()
{

}

void Particles_Plot::calculate_Profile()
{

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
