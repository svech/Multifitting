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

int Node::calculate_Intermediate_Points(tree<Node>::iterator this_Iter, tree<Node>::iterator active_Iter, QString active_Whats_This, QList<Node>& flat_List, QMap<int, tree<Node>::iterator>& flat_Tree_Map, QString& warning_Text)
{
	qInfo() << "\nnode : calculate_Intermediate_Points in " << whats_This << endl;

	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// if angle is changing
	if(active_Whats_This_List[1] == whats_This_Angle)
	{
		// we know that active data type == "Measurement"
		Measurement* measur = &(active_Iter.node->data.measurement);

		// for ambient
		if(whats_This_List[0] == whats_This_Ambient)
		{
			Node& corresponding_Flat_Node = flat_List[flat_Tree_Map.key(this_Iter)];

									plot_Points.resize(measur->probe_Angle.independent.num_Points);
			corresponding_Flat_Node.plot_Points.resize(measur->probe_Angle.independent.num_Points);

			///---------------------------------------------
			/// intermediate values
			///---------------------------------------------

			complex<double> delta_Epsilon;
			QVector<double> spectral_Points (1, measur->wavelength.value);

			// if known material
			if(ambient.composed_Material == false)
			{
				Material_Data temp_Material_Data = optical_Constants->material_Map.value(ambient.material + nk_Ext);

				QVector<complex<double>> n;
				int status = optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, warning_Text, ambient.material);
				if(status!=0)
				{
					return status;
				}
				delta_Epsilon = 1. - conj(n.first()*n.first());
			} else
			// compile from elements
			{
				QVector<complex<double>> temp_Delta_Epsilon;
				int status = optical_Constants->make_Epsilon_From_Factors(ambient.composition, ambient.absolute_Density.value, spectral_Points, temp_Delta_Epsilon, warning_Text);
				if(status!=0)
				{
					return status;
				}
				delta_Epsilon = 1. - temp_Delta_Epsilon.first();
			}

			// filling points
			for(int i=0; i<plot_Points.size(); ++i)
			{
				/// epsilon

				// if density
				if(ambient.separate_Optical_Constants != TRIL_TRUE)
				{
											plot_Points[i].delta_Epsilon = delta_Epsilon;	 // depend on absolute density if composed_Material
					corresponding_Flat_Node.plot_Points[i].delta_Epsilon = plot_Points[i].delta_Epsilon;

					// if absolute density
					if(ambient.composed_Material)
					{
												plot_Points[i].epsilon = 1. - plot_Points[i].delta_Epsilon;	// common epsilon, doesn't depend on "i"
						corresponding_Flat_Node.plot_Points[i].epsilon = plot_Points[i].epsilon;
					} else
					// if relative density
					{
												plot_Points[i].epsilon = 1. - ambient.relative_Density.value * plot_Points[i].delta_Epsilon;	// common epsilon, doesn't depend on "i"
						corresponding_Flat_Node.plot_Points[i].epsilon = plot_Points[i].epsilon;
					}

				} else
				// if separate optical constants
				{
											plot_Points[i].delta_Epsilon = complex<double>(real(delta_Epsilon) * ambient.permittivity.value / 100.,
																						   imag(delta_Epsilon) * ambient.absorption.value   / 100.);
					corresponding_Flat_Node.plot_Points[i].delta_Epsilon = plot_Points[i].delta_Epsilon;

											plot_Points[i].epsilon = 1. - plot_Points[i].delta_Epsilon;
					corresponding_Flat_Node.plot_Points[i].epsilon = plot_Points[i].epsilon;
				}

				/// hi
										plot_Points[i].hi = measur->k_Value*sqrt( plot_Points[i].epsilon - measur->cos2[i] );
				corresponding_Flat_Node.plot_Points[i].hi = plot_Points[i].hi;

				/// exponent
				/// no exponent for ambient

				/// weak factor
				/// no weak factor for ambient

				/// fresnel (reflections from the top border)
				/// no fresnel for ambient

//				cout << ambient.material.toStdString() << "              has Epsilon = " <<                         plot_Points[i].epsilon << " at " << measur->wavelength.value << endl;
//				cout << ambient.material.toStdString() << "         flat has Epsilon = " << corresponding_Flat_Node.plot_Points[i].epsilon << " at " << measur->wavelength.value << endl;
//				cout << endl;
//				cout << ambient.material.toStdString() << "              has Hi      = " <<                         plot_Points[i].hi << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
//				cout << ambient.material.toStdString() << "         flat has Hi      = " << corresponding_Flat_Node.plot_Points[i].hi << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
			}
		}

		// for layers
		if(whats_This_List[0] == whats_This_Layer)
		{
			Node& corresponding_Flat_Node = flat_List[flat_Tree_Map.key(this_Iter)-0];
			Node& higher_Flat_Node        = flat_List[flat_Tree_Map.key(this_Iter)-1];

									plot_Points.resize(measur->probe_Angle.independent.num_Points);
			corresponding_Flat_Node.plot_Points.resize(measur->probe_Angle.independent.num_Points);

			///---------------------------------------------
			/// intermediate values
			///---------------------------------------------

			complex<double> delta_Epsilon;
			QVector<double> spectral_Points (1, measur->wavelength.value);

			// if known material
			if(layer.composed_Material == false)
			{
				Material_Data temp_Material_Data = optical_Constants->material_Map.value(layer.material + nk_Ext);

				QVector<complex<double>> n;
				int status = optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, warning_Text, layer.material);
				if(status!=0)
				{
					return status;
				}
				delta_Epsilon = 1. - conj(n.first()*n.first());
			} else
			// compile from elements
			{
				QVector<complex<double>> temp_Delta_Epsilon;
				int status = optical_Constants->make_Epsilon_From_Factors(layer.composition, layer.absolute_Density.value, spectral_Points, temp_Delta_Epsilon, warning_Text);
				if(status!=0)
				{
					return status;
				}
				delta_Epsilon = 1. - temp_Delta_Epsilon.first();
			}

			// filling points
			for(int i=0; i<plot_Points.size(); ++i)
			{
				/// epsilon

				// if density
				if(layer.separate_Optical_Constants != TRIL_TRUE)
				{
											plot_Points[i].delta_Epsilon = delta_Epsilon;	 // depend on absolute density if composed_Material
					corresponding_Flat_Node.plot_Points[i].delta_Epsilon = plot_Points[i].delta_Epsilon;

					// if absolute density
					if(layer.composed_Material)
					{
												plot_Points[i].epsilon = 1. - plot_Points[i].delta_Epsilon;	// common epsilon, doesn't depend on "i"
						corresponding_Flat_Node.plot_Points[i].epsilon = plot_Points[i].epsilon;
					} else
					// if relative density
					{
												plot_Points[i].epsilon = 1. - layer.relative_Density.value * plot_Points[i].delta_Epsilon;	// common epsilon, doesn't depend on "i"
						corresponding_Flat_Node.plot_Points[i].epsilon = plot_Points[i].epsilon;
					}

				} else
				// if separate optical constants
				{
											plot_Points[i].delta_Epsilon = complex<double>(real(delta_Epsilon) * layer.permittivity.value / 100.,
																						   imag(delta_Epsilon) * layer.absorption.value   / 100.);
					corresponding_Flat_Node.plot_Points[i].delta_Epsilon = plot_Points[i].delta_Epsilon;

											plot_Points[i].epsilon = 1. - plot_Points[i].delta_Epsilon;
					corresponding_Flat_Node.plot_Points[i].epsilon = plot_Points[i].epsilon;
				}

				/// hi
										plot_Points[i].hi = measur->k_Value*sqrt( plot_Points[i].epsilon - measur->cos2[i] );
				corresponding_Flat_Node.plot_Points[i].hi = plot_Points[i].hi;

				/// exponent
										plot_Points[i].exponenta = exp( I * plot_Points[i].hi * layer.thickness.value );
				corresponding_Flat_Node.plot_Points[i].exponenta = plot_Points[i].exponenta;

				/// weak factor
										plot_Points[i].weak_Factor = 1; // TODO weak factor
				corresponding_Flat_Node.plot_Points[i].weak_Factor = plot_Points[i].weak_Factor;

				/// fresnel (reflections from the top border)

				// s - polarization
				if(measur->polarization.value > -1)
				{
					complex<double> temp_Fre_Denominator_s   = higher_Flat_Node.plot_Points[i].hi + plot_Points[i].hi;
					if(temp_Fre_Denominator_s!=0.)
					{
						complex<double> temp_Fre_Numerator_s = higher_Flat_Node.plot_Points[i].hi - plot_Points[i].hi;
						plot_Points[i].r_Fresnel_s = plot_Points[i].weak_Factor * temp_Fre_Numerator_s / temp_Fre_Denominator_s;
					}else
					{
						plot_Points[i].r_Fresnel_s = 0.;
					}
				}
				// p - polarization
				if(measur->polarization.value < 1)
				{
					complex<double> temp_Fre_Denominator_p   = higher_Flat_Node.plot_Points[i].hi / higher_Flat_Node.plot_Points[i].epsilon
																			  + plot_Points[i].hi / plot_Points[i].epsilon;
					if(temp_Fre_Denominator_p!=0.)
					{
						complex<double> temp_Fre_Numerator_p = higher_Flat_Node.plot_Points[i].hi / higher_Flat_Node.plot_Points[i].epsilon
																			  - plot_Points[i].hi / plot_Points[i].epsilon;
						plot_Points[i].r_Fresnel_p = plot_Points[i].weak_Factor * temp_Fre_Numerator_p / temp_Fre_Denominator_p;
					} else
					{
						plot_Points[i].r_Fresnel_p = 0.;
					}
				}
				cout << layer.material.toStdString() << "              has Epsilon = " <<                         plot_Points[i].epsilon << " at " << measur->wavelength.value << endl;
				cout << layer.material.toStdString() << "         flat has Epsilon = " << corresponding_Flat_Node.plot_Points[i].epsilon << " at " << measur->wavelength.value << endl;
				cout << layer.material.toStdString() << " higher  flat has Epsilon = " <<        higher_Flat_Node.plot_Points[i].epsilon << " at " << measur->wavelength.value << endl;
				cout << endl;
				cout << layer.material.toStdString() << "             has Hi       = " <<                         plot_Points[i].hi << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
				cout << layer.material.toStdString() << "        flat has Hi       = " << corresponding_Flat_Node.plot_Points[i].hi << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
				cout << layer.material.toStdString() << " higher flat has Hi       = " <<        higher_Flat_Node.plot_Points[i].hi << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
				cout << endl;
				cout << layer.material.toStdString() << " has r_Fresnel_s          = " << plot_Points[i].r_Fresnel_s << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
				cout << layer.material.toStdString() << " has r_Fresnel_p          = " << plot_Points[i].r_Fresnel_p << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
			}
		}

		// for substrate
		if(whats_This_List[0] == whats_This_Substrate)
		{
			Node& corresponding_Flat_Node = flat_List[flat_Tree_Map.key(this_Iter)-0];
			Node& higher_Flat_Node        = flat_List[flat_Tree_Map.key(this_Iter)-1];

									plot_Points.resize(measur->probe_Angle.independent.num_Points);
			corresponding_Flat_Node.plot_Points.resize(measur->probe_Angle.independent.num_Points);

			///---------------------------------------------
			/// intermediate values
			///---------------------------------------------

			complex<double> delta_Epsilon;
			QVector<double> spectral_Points (1, measur->wavelength.value);

			// if known material
			if(substrate.composed_Material == false)
			{
				Material_Data temp_Material_Data = optical_Constants->material_Map.value(substrate.material + nk_Ext);

				QVector<complex<double>> n;
				int status = optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, warning_Text, layer.material);
				if(status!=0)
				{
					return status;
				}
				delta_Epsilon = 1. - conj(n.first()*n.first());
			} else
			// compile from elements
			{
				QVector<complex<double>> temp_Delta_Epsilon;
				int status = optical_Constants->make_Epsilon_From_Factors(substrate.composition, substrate.absolute_Density.value, spectral_Points, temp_Delta_Epsilon, warning_Text);
				if(status!=0)
				{
					return status;
				}
				delta_Epsilon = 1. - temp_Delta_Epsilon.first();
			}

			// filling points
			for(int i=0; i<plot_Points.size(); ++i)
			{
				/// epsilon

				// if density
				if(substrate.separate_Optical_Constants != TRIL_TRUE)
				{
											plot_Points[i].delta_Epsilon = delta_Epsilon;	 // depend on absolute density if composed_Material
					corresponding_Flat_Node.plot_Points[i].delta_Epsilon = plot_Points[i].delta_Epsilon;

					// if absolute density
					if(substrate.composed_Material)
					{
												plot_Points[i].epsilon = 1. - plot_Points[i].delta_Epsilon;	// common epsilon, doesn't depend on "i"
						corresponding_Flat_Node.plot_Points[i].epsilon = plot_Points[i].epsilon;
					} else
					// if relative density
					{
												plot_Points[i].epsilon = 1. - substrate.relative_Density.value * plot_Points[i].delta_Epsilon;	// common epsilon, doesn't depend on "i"
						corresponding_Flat_Node.plot_Points[i].epsilon = plot_Points[i].epsilon;
					}

				} else
				// if separate optical constants
				{
											plot_Points[i].delta_Epsilon = complex<double>(real(delta_Epsilon) * substrate.permittivity.value / 100.,
																						   imag(delta_Epsilon) * substrate.absorption.value   / 100.);
					corresponding_Flat_Node.plot_Points[i].delta_Epsilon = plot_Points[i].delta_Epsilon;

											plot_Points[i].epsilon = 1. - plot_Points[i].delta_Epsilon;
					corresponding_Flat_Node.plot_Points[i].epsilon = plot_Points[i].epsilon;
				}

				/// hi
										plot_Points[i].hi = measur->k_Value*sqrt( plot_Points[i].epsilon - measur->cos2[i] );
				corresponding_Flat_Node.plot_Points[i].hi = plot_Points[i].hi;

				/// exponent
				/// no exponent for substrate

				/// weak factor
										plot_Points[i].weak_Factor = 1; // TODO weak factor
				corresponding_Flat_Node.plot_Points[i].weak_Factor = plot_Points[i].weak_Factor;

				/// fresnel (reflections from the top border)

				// s - polarization
				if(measur->polarization.value > -1)
				{
					complex<double> temp_Fre_Denominator_s   = higher_Flat_Node.plot_Points[i].hi + plot_Points[i].hi;
					if(temp_Fre_Denominator_s != 0.)
					{
						complex<double> temp_Fre_Numerator_s = higher_Flat_Node.plot_Points[i].hi - plot_Points[i].hi;
						plot_Points[i].r_Fresnel_s = plot_Points[i].weak_Factor * temp_Fre_Numerator_s / temp_Fre_Denominator_s;
					}else
					{
						plot_Points[i].r_Fresnel_s = 0.;
					}
				}
				// p - polarization
				if(measur->polarization.value < 1)
				{
					complex<double> temp_Fre_Denominator_p   = higher_Flat_Node.plot_Points[i].hi / higher_Flat_Node.plot_Points[i].epsilon
																			  + plot_Points[i].hi / plot_Points[i].epsilon;
					if(temp_Fre_Denominator_p != 0.)
					{
						complex<double> temp_Fre_Numerator_p = higher_Flat_Node.plot_Points[i].hi / higher_Flat_Node.plot_Points[i].epsilon
																			  - plot_Points[i].hi / plot_Points[i].epsilon;
						plot_Points[i].r_Fresnel_p = plot_Points[i].weak_Factor * temp_Fre_Numerator_p / temp_Fre_Denominator_p;
					} else
					{
						plot_Points[i].r_Fresnel_p = 0.;
					}
				}

//				cout << substrate.material.toStdString() << "              has Epsilon = " <<                         plot_Points[i].epsilon << " at " << measur->wavelength.value << endl;
//				cout << substrate.material.toStdString() << "         flat has Epsilon = " << corresponding_Flat_Node.plot_Points[i].epsilon << " at " << measur->wavelength.value << endl;
//				cout << substrate.material.toStdString() << " higher  flat has Epsilon = " <<        higher_Flat_Node.plot_Points[i].epsilon << " at " << measur->wavelength.value << endl;
//				cout << endl;
//				cout << substrate.material.toStdString() << "             has Hi       = " <<                         plot_Points[i].hi << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
//				cout << substrate.material.toStdString() << "        flat has Hi       = " << corresponding_Flat_Node.plot_Points[i].hi << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
//				cout << substrate.material.toStdString() << " higher flat has Hi       = " <<        higher_Flat_Node.plot_Points[i].hi << " at " << measur->wavelength.value << " and at " << measur->cos2[i] << endl;
			}
		}
	}

	return 0;
}
