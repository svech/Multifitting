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
	calculate_Profile();

	QVector<QCPGraphData> data_To_Plot(arg.size());
	for (int i=0; i<data_To_Plot.size(); ++i)
	{
		data_To_Plot[i].key = arg[i];
		data_To_Plot[i].value = val[i];

//		if(max<data_To_Plot[i].value) {max=data_To_Plot[i].value;}
//		if(min>data_To_Plot[i].value) {min=data_To_Plot[i].value;}
	}
	double minimum = *std::min_element(val.constBegin(), val.constEnd());
	double maximum = *std::max_element(val.constBegin(), val.constEnd());

	maximum += maximum*0.1;
	minimum -= maximum*0.1;

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
	custom_Plot->xAxis->setRange(arg.first(), arg.last());
	custom_Plot->yAxis->setRange(minimum,maximum);
	//custom_Plot->yAxis2->setTickLabels(false);

//	if(!fast)
//		refresh_Labels();
//	else
		custom_Plot->replot();		
}

void Profile_Plot::calculate_Profile()
{
	struct_Data_Vector.clear();
	unwrap_Subtree(struct_Data_Vector, multilayer->structure_Tree->tree->invisibleRootItem());
	// if has no substrate, add it
	if(struct_Data_Vector.last().item_Type != item_Type_Substrate)
	{
		Data substrate = struct_Data_Vector.first();

		// change id
		substrate.reset_All_IDs();
		substrate.item_Type = item_Type_Substrate;

		// add to the end
		struct_Data_Vector.append(substrate);
	}

	// thicknesses and boundaries position
	boundary_Vector.resize(struct_Data_Vector.size()-1);	boundary_Vector.first() = 0;
	sigma_Vector.resize(struct_Data_Vector.size()-1);		sigma_Vector.last() = struct_Data_Vector.last().sigma.value;
	thickness_Vector.resize(struct_Data_Vector.size()-2);
	for(int i=0; i<thickness_Vector.size(); i++)
	{
		thickness_Vector[i] = struct_Data_Vector[i+1].thickness.value;
		boundary_Vector[i+1] = boundary_Vector[i]+thickness_Vector[i];
		sigma_Vector[i] = struct_Data_Vector[i+1].sigma.value;
	}

	// materials and relative density
	material_Vector.resize(struct_Data_Vector.size());
	density_Vector.resize(struct_Data_Vector.size());
	delta_Epsilon_Vector.resize(struct_Data_Vector.size());
	beta_Epsilon_Vector.resize(struct_Data_Vector.size());

	complex<double> delta_Epsilon_Ang;
	QVector<double> spectral_Points (1, multilayer->profile_Plot_Data.local_Wavelength);
	for(int i=0; i<struct_Data_Vector.size(); i++)
	{
		Data& struct_Data = struct_Data_Vector[i];
		if(struct_Data.composed_Material)
		{
			material_Vector[i] = struct_Data.get_Composed_Material();

			double stoich_Weight = DBL_MIN;
			double auto_Density = 0;
			for(int comp_Index=0; comp_Index<struct_Data.composition.size(); ++comp_Index)
			{
				auto_Density += sorted_Density.value(struct_Data.composition[comp_Index].type)*struct_Data.composition[comp_Index].composition.value;
				stoich_Weight += struct_Data.composition[comp_Index].composition.value;
			}
			density_Vector[i] = struct_Data.absolute_Density.value/(auto_Density/stoich_Weight);

			// get epsilon
			QVector<complex<double>> temp_Epsilon;
			optical_Constants->make_Epsilon_From_Factors(struct_Data.composition, struct_Data.absolute_Density.value, spectral_Points, temp_Epsilon);
			delta_Epsilon_Ang = 1. - temp_Epsilon.first();

			delta_Epsilon_Vector[i] = real(delta_Epsilon_Ang);
			beta_Epsilon_Vector [i] = imag(delta_Epsilon_Ang);
		} else
		{
			material_Vector[i] = struct_Data.approved_Material;
			density_Vector[i] = struct_Data.relative_Density.value;

			// get epsilon
			Material_Data temp_Material_Data = optical_Constants->material_Map.value(struct_Data.approved_Material + nk_Ext);
			QVector<complex<double>> n;
			optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, struct_Data.approved_Material);
			delta_Epsilon_Ang = 1. - n.first()*n.first();

			delta_Epsilon_Vector[i] = real(delta_Epsilon_Ang);
			beta_Epsilon_Vector [i] = imag(delta_Epsilon_Ang);
		}
	}
	// find different materials
	different_Materials.clear();
	for(int i=0; i<material_Vector.size(); i++)
	{
		if(!different_Materials.contains(material_Vector[i])) different_Materials.append(material_Vector[i]);
	}

	qInfo() << material_Vector << endl;
//	qInfo() << different_Materials << endl;
	qInfo() << delta_Epsilon_Vector << endl;
	qInfo() << beta_Epsilon_Vector << endl;
//	qInfo() << density_Vector << endl;
//	qInfo() << thickness_Vector << endl;
	qInfo() << boundary_Vector << endl;

	// profiling
	int data_Count=20000;
	arg.resize(data_Count);
	val.resize(arg.size());
	for(int i=0; i<data_Count; i++)
	{
		double z = i/5. -250;
		arg[i]=z;
		val[i]=delta_Epsilon_Func(z);
	}
}

double Profile_Plot::delta_Epsilon_Func(double z)
{
	// TODO norm to save substance
	double delta_Epsilon = 0;
	for(int j=0; j<thickness_Vector.size(); j++)
	{
		delta_Epsilon += delta_Epsilon_Vector[j+1] *
				erfc((boundary_Vector[j  ]-z)/sigma_Vector[j  ]) *
				erfc((z-boundary_Vector[j+1])/sigma_Vector[j+1]) * 0.25;
	}
	delta_Epsilon += delta_Epsilon_Vector.last() *
				erfc((boundary_Vector.last()-z)/sigma_Vector.last()) * 0.5;
	return delta_Epsilon;
}

void Profile_Plot::unwrap_Subtree(QVector<Data>& struct_Data_Vector,  QTreeWidgetItem* item)
{
	for(int i=0; i<item->childCount(); ++i)
	{
		Data struct_Data = item->child(i)->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>();
		if(struct_Data.item_Enabled)
		{
			if(struct_Data.item_Type == item_Type_Ambient ||
			   struct_Data.item_Type == item_Type_Layer   ||
			   struct_Data.item_Type == item_Type_Substrate)
			{
				struct_Data_Vector.append(struct_Data);
			}

			if(item->child(i)->childCount()>0)
			{
				if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
				{
					for(int layer_Index=0; layer_Index<struct_Data.regular_Components.first().components.size(); layer_Index++)
					{
						for(int component_Index=0; component_Index<struct_Data.regular_Components.size(); component_Index++)
						{
							struct_Data_Vector.append(struct_Data.regular_Components[component_Index].components[layer_Index]);
						}
					}
				}

				if(struct_Data.item_Type == item_Type_Multilayer)
				{
					for(int pariod_Index=0; pariod_Index<struct_Data.num_Repetition.value(); pariod_Index++)
					{
						unwrap_Subtree(struct_Data_Vector, item->child(i));
					}
				}
			}
		}
	}
}







