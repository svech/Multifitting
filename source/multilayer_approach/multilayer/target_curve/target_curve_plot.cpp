// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "target_curve_plot.h"

Target_Curve_Plot::Target_Curve_Plot(Target_Curve* target_Curve, QWidget *parent) :
	target_Curve(target_Curve),
	QWidget(parent)
{
	custom_Plot = new QCustomPlot(parent);
	custom_Plot->setMinimumHeight(300);
	custom_Plot->replot();
}

void Target_Curve_Plot::create_Plot_Frame_And_Scale()
{
	custom_Plot->setNoAntialiasingOnDrag(false); // more performance/responsiveness during dragging

	// frame, axes
	{
		QPen pen = custom_Plot->yAxis->grid()->pen();
		pen.setStyle(Qt::DashLine);
		custom_Plot->yAxis->grid()->setSubGridVisible(true);
		custom_Plot->xAxis->grid()->setSubGridVisible(true);
		custom_Plot->yAxis->grid()->setPen(pen);
		custom_Plot->xAxis->grid()->setPen(pen);

		if(target_Curve->plot_Options_Experimental.scale == log_Scale)
		{
			custom_Plot->yAxis ->setScaleType(QCPAxis::stLogarithmic);

			QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
			custom_Plot->yAxis ->setTicker(logTicker);
			custom_Plot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
			custom_Plot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
			custom_Plot->xAxis->setRange(0, 5);
			custom_Plot->yAxis->setRange(1e-5, 1e0);

			if(	target_Curve->curve.value_Mode == value_R_Mode[R] ||
				target_Curve->curve.value_Mode == value_T_Mode[T] )
			{
				custom_Plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
				custom_Plot->yAxis2->setTicker(logTicker);
				custom_Plot->yAxis2->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
				custom_Plot->yAxis2->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
				custom_Plot->yAxis2->setRange(1e-5, 1e0);
			}
			if(	target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )
			{
				QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
				custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);
				custom_Plot->yAxis2->setTicker(linTicker);
				custom_Plot->yAxis2->setNumberFormat("g");
				custom_Plot->yAxis2->setNumberPrecision(4);
				custom_Plot->yAxis2->setRange(0, 1);
			}
		}
		if(target_Curve->plot_Options_Experimental.scale == lin_Scale)
		{
			custom_Plot->yAxis ->setScaleType(QCPAxis::stLinear);
			custom_Plot->yAxis2->setScaleType(QCPAxis::stLinear);

			QSharedPointer<QCPAxisTicker> linTicker(new QCPAxisTicker);
			custom_Plot->yAxis ->setTicker(linTicker);
			custom_Plot->yAxis2->setTicker(linTicker);
			custom_Plot->yAxis ->setNumberFormat("g");
			custom_Plot->yAxis2->setNumberFormat("g");
			custom_Plot->yAxis ->setNumberPrecision(4);
			custom_Plot->yAxis2->setNumberPrecision(4);
			custom_Plot->xAxis->setRange(0, 5);
			custom_Plot->yAxis->setRange(0, 1);
			custom_Plot->yAxis2->setRange(0, 1);
		}

		// make range draggable and zoomable:
		custom_Plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom);

		// make top right axes clones of bottom left axes:
		custom_Plot->xAxis2->setVisible(true);
		custom_Plot->yAxis2->setVisible(true);
		custom_Plot->xAxis2->setTickLabels(false);

		// connect signals so top and right axes move in sync with bottom and left axes:
		if(	target_Curve->curve.value_Mode == value_R_Mode[R] ||
			target_Curve->curve.value_Mode == value_T_Mode[T] )
		{
			connect(custom_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->xAxis2, SLOT(setRange(QCPRange)));
			connect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));
		}
		if(	target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )
		{
			connect   (custom_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->xAxis2, SLOT(setRange(QCPRange)));
			disconnect(custom_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), custom_Plot->yAxis2, SLOT(setRange(QCPRange)));
		}
	}

	custom_Plot->replot();
}

void Target_Curve_Plot::plot_Data(bool fast)
{
	if(!fast)
		create_Plot_Frame_And_Scale();

	if(target_Curve->loaded_And_Ready)
	{
		int data_Count = target_Curve->curve.argument.size();

		// first graph
		{
			QVector<QCPGraphData> data_To_Plot(data_Count);
			double min = DBL_MAX;
			double max = 0;
			for (int i=0; i<data_Count; ++i)
			{
				data_To_Plot[i].key = target_Curve->curve.shifted_Argument[i];
				data_To_Plot[i].value = target_Curve->curve.shifted_Values[i].val_1;

				if(max<data_To_Plot[i].value && (target_Curve->plot_Options_Experimental.scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {max=data_To_Plot[i].value;}
				if(min>data_To_Plot[i].value && (target_Curve->plot_Options_Experimental.scale == lin_Scale || data_To_Plot[i].value > DBL_MIN)) {min=data_To_Plot[i].value;}
			}

			if(!fast)
			{
				if(custom_Plot->graphCount()!=1)
				{
					custom_Plot->clearGraphs();
					custom_Plot->addGraph();
				}
				custom_Plot->graph(0)->setPen(QPen(target_Curve->plot_Options_Experimental.color,target_Curve->plot_Options_Experimental.thickness));
//				custom_Plot->graph(0)->setBrush(QBrush(target_Curve->plot_Options.color));
				QCPScatterStyle scatter_Style;
				scatter_Style.setShape(QCPScatterStyle::ScatterShape(target_Curve->plot_Options_Experimental.scatter_Shape));
				scatter_Style.setSize(target_Curve->plot_Options_Experimental.scatter_Size);
				custom_Plot->graph(0)->setScatterStyle(scatter_Style);
			}

			custom_Plot->graph(0)->data()->set(data_To_Plot);
			custom_Plot->xAxis->setRange(target_Curve->curve.shifted_Argument[0], target_Curve->curve.shifted_Argument[data_Count-1]);
			custom_Plot->yAxis->setRange(min,max);
//			custom_Plot->yAxis2->setTickLabels(false);
		}

		// second graph
		if(	target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )
		{
			QVector<QCPGraphData> data_To_Plot(data_Count);
			double min = DBL_MAX;
			double max = 0;
			for (int i=0; i<data_Count; ++i)
			{
				data_To_Plot[i].key = target_Curve->curve.shifted_Argument[i];
				data_To_Plot[i].value = target_Curve->curve.shifted_Values[i].val_2;

				if(max<data_To_Plot[i].value) max=data_To_Plot[i].value;
				if(min>data_To_Plot[i].value) min=data_To_Plot[i].value;
			}

			if(!fast)
			{
				if(custom_Plot->graphCount()==1)
				{
					custom_Plot->addGraph(custom_Plot->xAxis2, custom_Plot->yAxis2);
				}
				custom_Plot->graph(1)->setPen(QPen(target_Curve->plot_Options_Experimental.color_Second,target_Curve->plot_Options_Experimental.thickness_Second));
//				custom_Plot->graph(1)->setBrush(QBrush(target_Curve->plot_Options.colorsdfd));
				QCPScatterStyle scatter_Style_Second;
				scatter_Style_Second.setShape(QCPScatterStyle::ScatterShape(target_Curve->plot_Options_Experimental.scatter_Shape_Second));
				scatter_Style_Second.setSize(target_Curve->plot_Options_Experimental.scatter_Size_Second);
				custom_Plot->graph(1)->setScatterStyle(scatter_Style_Second);
			}

			custom_Plot->graph(1)->data()->set(data_To_Plot);
			custom_Plot->xAxis2->setRange(target_Curve->curve.shifted_Argument[0], target_Curve->curve.shifted_Argument[data_Count-1]);
			custom_Plot->yAxis2->setRange(min,max);
//			custom_Plot->yAxis2->setTickLabels(true);
		}
	}

	if(!fast)
		refresh_Labels();
	else
		custom_Plot->replot();
}

void Target_Curve_Plot::refresh_Labels()
{
	// value
	bool show_Second_Label = false;
	{
		val_Type_Label = target_Curve->curve.value_Function;

		if( target_Curve->curve.value_Mode == value_R_Mode[R] )	{val_Mode_Label_1 = value_R_Mode_Label_1[R];}
		if( target_Curve->curve.value_Mode == value_R_Mode[R_Phi] )	{
			val_Mode_Label_1 = value_R_Mode_Label_1[R_Phi];
			val_Mode_Label_2 = value_R_Mode_Label_2[R_Phi];
			show_Second_Label = true;
		}
		if( target_Curve->curve.value_Mode == value_T_Mode[T] )	{val_Mode_Label_1 = value_T_Mode[T];}
	}

	// argument
	{
		if(target_Curve->curve.argument_Type == whats_This_Angle)
		{
			if(target_Curve->curve.angle_Type == angle_Type_Grazing)	argument_Type_Label = argument_Types[Grazing_angle];
			if(target_Curve->curve.angle_Type == angle_Type_Incidence)	argument_Type_Label = argument_Types[Incident_angle];

			argument_Label = argument_Type_Label + " " + Theta_Sym + ", " + target_Curve->curve.angular_Units;
		}
		if(target_Curve->curve.argument_Type == whats_This_Wavelength)
		{
			if(	target_Curve->curve.spectral_Units == wavelength_Units_List[angstrom] ||
				target_Curve->curve.spectral_Units == wavelength_Units_List[nm]		  ||
				target_Curve->curve.spectral_Units == wavelength_Units_List[mcm]	   )
			{
				argument_Type_Label = QString(argument_Types[Wavelength_Energy]).split("/").first();
				argument_Label = argument_Type_Label + " " + Lambda_Sym + ", " + target_Curve->curve.spectral_Units;
			} else
			{
				argument_Type_Label = QString(argument_Types[Wavelength_Energy]).split("/").last();
				argument_Label = argument_Type_Label + " E, " + target_Curve->curve.spectral_Units;
			}
		}
	}

	// plot
	{
		// argument
		custom_Plot->xAxis->setLabel(argument_Label);

		// value
		QString full_Label_1 = val_Type_Label + ", " + val_Mode_Label_1;
		custom_Plot->yAxis->setLabel(full_Label_1);

		if(show_Second_Label)
		{
			custom_Plot->yAxis2->setLabel(val_Mode_Label_2);
			custom_Plot->yAxis2->setLabelColor(target_Curve->plot_Options_Experimental.color_Second);
			custom_Plot->yAxis2->setTickLabelColor(target_Curve->plot_Options_Experimental.color_Second);
			custom_Plot->yAxis->setLabelColor(target_Curve->plot_Options_Experimental.color);
			custom_Plot->yAxis->setTickLabelColor(target_Curve->plot_Options_Experimental.color);
		}
		else
		{
			custom_Plot->yAxis2->setLabel("");
			custom_Plot->yAxis2->setLabelColor(Qt::black);
			custom_Plot->yAxis2->setTickLabelColor(Qt::black);
			custom_Plot->yAxis->setLabelColor(Qt::black);
			custom_Plot->yAxis->setTickLabelColor(Qt::black);
		}
	}

	custom_Plot->replot();
}
