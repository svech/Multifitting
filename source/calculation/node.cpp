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

int Node::calculate_Intermediate_Points(tree<Node>::iterator active_Iter, QString active_Whats_This, QString& warning_Text)
{
	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// if angle is changing
	if(active_Whats_This_List[1] == whats_This_Angle)
	{
		// we know that active data type == "Measurement"
		Measurement* measur = &(active_Iter.node->data.measurement);

		// for any media
		if(whats_This_List[0] == whats_This_Ambient ||
		   whats_This_List[0] == whats_This_Layer   ||
		   whats_This_List[0] == whats_This_Substrate )
		{

			if(whats_This_List[0] == whats_This_Ambient)
			{
				common_Data = ambient;
			} else
			if(whats_This_List[0] == whats_This_Layer)
			{
				common_Data.composed_Material			= layer.composed_Material;
				common_Data.material					= layer.material;
				common_Data.approved_Material			= layer.approved_Material;
				common_Data.relative_Density			= layer.relative_Density;
				common_Data.absolute_Density			= layer.absolute_Density;
				common_Data.separate_Optical_Constants	= layer.separate_Optical_Constants;
				common_Data.permittivity				= layer.permittivity;
				common_Data.absorption					= layer.absorption;
				common_Data.composition					= layer.composition;
			} else
			if(whats_This_List[0] == whats_This_Substrate)
			{
				common_Data.composed_Material			= substrate.composed_Material;
				common_Data.material					= substrate.material;
				common_Data.approved_Material			= substrate.approved_Material;
				common_Data.relative_Density			= substrate.relative_Density;
				common_Data.absolute_Density			= substrate.absolute_Density;
				common_Data.separate_Optical_Constants	= substrate.separate_Optical_Constants;
				common_Data.permittivity				= substrate.permittivity;
				common_Data.absorption					= substrate.absorption;
				common_Data.composition					= substrate.composition;
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// delta_Epsilon
			/// ---------------------------------------------------------------------------------------------------------------

			QVector<double> spectral_Points (1, measur->wavelength.value);

			// if known material
			if(common_Data.composed_Material == false)
			{
				Material_Data temp_Material_Data = optical_Constants->material_Map.value(common_Data.material + nk_Ext);
				QVector<complex<double>> n;
				int status = optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, warning_Text, common_Data.material);
				if(status!=0){return status;}
				delta_Epsilon = 1. - n.first()*n.first();
			} else
			// compile from elements
			{
				QVector<complex<double>> temp_Epsilon;
				int status = optical_Constants->make_Epsilon_From_Factors(common_Data.composition, common_Data.absolute_Density.value, spectral_Points, temp_Epsilon, warning_Text);
				if(status!=0){return status;}
				delta_Epsilon = 1. - temp_Epsilon.first();
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// epsilon
			/// ---------------------------------------------------------------------------------------------------------------

			// if density
			if(common_Data.separate_Optical_Constants != TRIL_TRUE)
			{
				// if absolute density
				if(common_Data.composed_Material)		epsilon = 1. - delta_Epsilon;
				// if relative density
				else									epsilon = 1. - common_Data.relative_Density.value * delta_Epsilon;

			} else
				// if separate optical constants
			{
				delta_Epsilon = complex<double>(real(delta_Epsilon) * common_Data.permittivity.value / 100.,
												imag(delta_Epsilon) * common_Data.absorption  .value / 100.);
				epsilon = 1. - delta_Epsilon;
			}

//			for(int func_Index=0; func_Index<transition_Layer_Functions.size(); ++func_Index)
//			{
//				qInfo() << layer.sigma.value << " with " << layer.interlayer_Composition[func_Index].enabled;
//			} qInfo() << " ";
//			cout << common_Data.material.toStdString() << " has Epsilon = " << epsilon << endl;
		}
	}
	return 0;
}
