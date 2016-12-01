#include "node.h"
#include <iostream>

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

void Node::calculate_Intermediate_Points(tree<Node>& calc_Tree, tree<Node>::iterator this_Iter, tree<Node>::iterator active_Iter, QString active_Whats_This, QList<Node>& flat_List, QMap<int, tree<Node>::iterator>& flat_Tree_Map, Optical_Constants* optical_Constants)
{
	qInfo() << "\nnode : calculate_Intermediate_Points in " << whats_This << endl;

	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// for layers
	if(whats_This_List[0] == whats_This_Layer)
	{
//		qInfo() << "active       : " << active_Whats_This;
//		qInfo() << "active node  : " << active_Iter.node->data.whats_This;

//		qInfo() << "current node : " << this_Iter.node->data.whats_This;
//		qInfo() << "prev flat    : " << flat_List[flat_Tree_Map.key(this_Iter)-1].whats_This;
//		qInfo() << "current flat : " << flat_List[flat_Tree_Map.key(this_Iter)].whats_This;
//		qInfo() << "next flat    : " << flat_List[flat_Tree_Map.key(this_Iter)+1].whats_This;

		if(active_Whats_This_List[1] == whats_This_Angle)
		{
			// we know that data type == "Measurement"
			Measurement* measur = &(active_Iter.node->data.measurement);
			plot_Points.resize(measur->probe_Angle.independent.num_Points);

			///---------------------------------------------
			/// intermediate values
			///---------------------------------------------

			complex<double> delta_Epsilon;
			QVector<double> spectral_Points (1, measur->wavelength.value);

			// if known material
			if(layer.composed_Material == false)
			{
				Material_Data temp_Material_Data = optical_Constants->material_Map.value(layer.material + nk_Ext);

				// TODO if material not found
				if(temp_Material_Data.material_Data.isEmpty())
				{
					qInfo() << "\n--------------------------------\n" << layer.material << " is wrong material!!!!\n--------------------------------\n";
					exit(EXIT_FAILURE);
				};

				// range check
				if((measur->wavelength.value <= temp_Material_Data.material_Data.first().lambda) ||
				   (measur->wavelength.value >= temp_Material_Data.material_Data.last(). lambda))
				{
					// TODO correct warning
					qInfo() << "Node::calculate_Intermediate_Points  :  Wavelength is out of range for " << layer.material;
					qInfo() << "Range is " << temp_Material_Data.material_Data.first().lambda << " - " << temp_Material_Data.material_Data.last().lambda;
					exit(EXIT_FAILURE);
				}
				complex<double> n = optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points).first();
				delta_Epsilon = 1. - conj(n*n);
			} else
			// compile from elements
			{
				delta_Epsilon = 1. - optical_Constants->make_Epsilon_From_Factors(layer.composition, layer.absolute_Density.value, spectral_Points).first();
			}

			std::cout << layer.material.toStdString() << " has dE = " << delta_Epsilon << " at " << measur->wavelength.value << endl;

			// filling points
			for(int i=0; i<plot_Points.size(); ++i)
			{
				/// epsilon

				// if density
				if(layer.separate_Optical_Constants != TRIL_TRUE)
				{
					plot_Points[i].delta_Epsilon = delta_Epsilon;	 // depend on absolute density if composed_Material

					// if absolute density
					if(layer.composed_Material)
					{
						plot_Points[i].epsilon = 1. - plot_Points[i].delta_Epsilon;	// common epsilon, doesn't depend on "i"
					} else
					// if relative density
					{
						plot_Points[i].epsilon = 1. - layer.relative_Density.value * plot_Points[i].delta_Epsilon;	// common epsilon, doesn't depend on "i"
					}

				} else
				// if separate optical constants
				{
					plot_Points[i].delta_Epsilon = complex<double>(real(delta_Epsilon) * layer.permittivity.value / 100.,
																   imag(delta_Epsilon) * layer.absorption.value   / 100.);
					plot_Points[i].epsilon = 1. - plot_Points[i].delta_Epsilon;
				}

				/// hi
				plot_Points[i].hi = measur->k_Value*sqrt( plot_Points[i].epsilon - measur->cos2[i] );

				/// exponent
				plot_Points[i].exponenta = exp( I * plot_Points[i].hi * layer.thickness.value );

				/// weak factor
				plot_Points[i].weak_Factor = 1;

				/// fresnel (reflections from the top border)
				// DANGER!!! flat list has no epsilon data!!!!!
				Node higher_Node = flat_List[flat_Tree_Map.key(this_Iter)-1];

				// s - polarization
				if(measur->polarization.value > -1)
				{
					complex<double> temp_Fre_Denominator_s   = higher_Node.plot_Points[i].hi + plot_Points[i].hi;
					if(temp_Fre_Denominator_s!=0.)
					{
						complex<double> temp_Fre_Numerator_s = higher_Node.plot_Points[i].hi - plot_Points[i].hi;
						plot_Points[i].r_Fresnel_s = plot_Points[i].weak_Factor * temp_Fre_Numerator_s / temp_Fre_Denominator_s;
					}else
					{
						plot_Points[i].r_Fresnel_s = 0.;
					}
				}
				// p - polarization
				if(measur->polarization.value < 1)
				{
					complex<double> temp_Fre_Denominator_p   = higher_Node.plot_Points[i].hi / higher_Node.plot_Points[i].epsilon
																		 + plot_Points[i].hi / plot_Points[i].epsilon;
					if(temp_Fre_Denominator_p!=0.)
					{
						complex<double> temp_Fre_Numerator_p = higher_Node.plot_Points[i].hi / higher_Node.plot_Points[i].epsilon
																		 - plot_Points[i].hi / plot_Points[i].epsilon;
						plot_Points[i].r_Fresnel_p = plot_Points[i].weak_Factor * temp_Fre_Numerator_p / temp_Fre_Denominator_p;
					} else
					{
						plot_Points[i].r_Fresnel_p = 0.;
					}
				}
			}
		}
	}
}
