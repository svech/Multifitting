#include "node.h"

Node::Node()
{

}

Node::Node(QTreeWidgetItem* item):
	whats_This(item->whatsThis(DEFAULT_COLUMN))
{
	whats_This_List = whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

	if(whats_This_List[0] == whats_This_Measurement)measurement	  = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
	if(whats_This_List[0] == whats_This_Ambient)	ambient		  = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
	if(whats_This_List[0] == whats_This_Layer)		layer		  = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
	if(whats_This_List[0] == whats_This_Multilayer)	stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
	if(whats_This_List[0] == whats_This_Substrate)	substrate	  = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
}

void Node::calculate_Intermediate_Points(tree<Node>::iterator active_Iter, QString active_Whats_This)
{
	qInfo() << "node : calculate_Intermediate_Points in " << whats_This << endl;

	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// for layers
	if(whats_This_List[0] == whats_This_Layer)
	{
		qInfo() << "active       : " << active_Whats_This;


		qInfo() << "active node  : " << active_Iter.node->data.whats_This;

		tree<Node>::pre_order_iterator top    = --this_Iter;
		qInfo() << "upper   node : " << top.node->data.whats_This;
		qInfo() << "current node : " << whats_This;
		tree<Node>::pre_order_iterator bottom = ++++this_Iter;
		qInfo() << "lower   node : " << bottom.node->data.whats_This;



		if(active_Whats_This_List[1] == whats_This_Angle)
		{
			// we know that data type == "Measurement"
			Measurement* measur = &(active_Iter.node->data.measurement);
			plot_Points.resize(measur->probe_Angle.independent.num_Points);

			///---------------------------------------------
			/// intermediate values
			///---------------------------------------------

			for(int i=0; i<plot_Points.size(); ++i)
			{
				/// epsilon
				// if density
				if(layer.separate_Optical_Constants != TRIL_TRUE)
				{
					plot_Points[i].delta_Epsilon = 0.123;
				} else
				// if separate optical constants
				{
					complex<double> nominal_Delta_Epsilon = 0.321;
					plot_Points[i].delta_Epsilon = complex<double>(real(nominal_Delta_Epsilon) * layer.permittivity.value / 100.,
																   imag(nominal_Delta_Epsilon) * layer.absorption.value   / 100.);
				}
				plot_Points[i].epsilon = 1. - layer.density.value * plot_Points[i].delta_Epsilon;	// common epsilon, doesn't depend on "i"

				/// hi
				plot_Points[i].hi = measur->k_Value*sqrt( plot_Points[i].epsilon - measur->cos2[i] );

				/// exponent
				plot_Points[i].exponenta = exp( I * plot_Points[i].hi * layer.thickness.value );

				/// weak factor
				plot_Points[i].weak_Factor = 1;

				/// fresnel
//				plot_Points[i].r_Fresnel_s =
			}

		}

	}
}
