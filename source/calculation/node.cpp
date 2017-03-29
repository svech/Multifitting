// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "node.h"
#include <iostream>

Node::Node() //-V730
{

}

Node::Node(QTreeWidgetItem* item): //-V730
	whats_This(item->whatsThis(DEFAULT_COLUMN))
{
	whats_This_List = whats_This.split(item_Type_Delimiter,QString::SkipEmptyParts);

	if(whats_This_List[0] == whats_This_Measurement)measurement	  = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Measurement>();
	if(whats_This_List[0] == whats_This_Ambient)	ambient		  = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Ambient>();
	if(whats_This_List[0] == whats_This_Layer)		layer		  = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Layer>();
	if(whats_This_List[0] == whats_This_Multilayer)	stack_Content = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Stack_Content>();
	if(whats_This_List[0] == whats_This_Substrate)	substrate	  = item->data(DEFAULT_COLUMN, Qt::UserRole).value<Substrate>();
}

int Node::calculate_Intermediate_Points(const tree<Node>::iterator& active_Iter, Node* above_Node, QString active_Whats_This, QString& warning_Text, bool depth_Grading, bool sigma_Grading)
{
	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	if(active_Whats_This_List[1] == whats_This_Angle ||
	   active_Whats_This_List[1] == whats_This_Wavelength )
	{
		// we know that active data type == "Measurement"
		Measurement* measur = &(active_Iter.node->data.measurement);
		int num_Points = 0;
		vector<double> cos2;
		vector<double> k;

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

				common_Sigma					= layer.common_Sigma;
				sigma							= layer.sigma.value;
				boundary_Interlayer_Composition = layer.interlayer_Composition.toStdVector();

				thickness = layer.thickness.value;

				thickness_Drift = layer.thickness_Drift;
				sigma_Drift		= layer.sigma_Drift;
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

				common_Sigma					= substrate.common_Sigma;
				sigma							= substrate.sigma.value;
				boundary_Interlayer_Composition = substrate.interlayer_Composition.toStdVector();;
			}

			// if angle is changing
			if(active_Whats_This_List[1] == whats_This_Angle)
			{
				// measured points
				num_Points = measur->cos2.size();
				cos2 = measur->cos2.toStdVector();
				k.resize(num_Points);
				for(int point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					k[point_Index] = measur->k_Value;
				}

				epsilon.resize(num_Points);
				epsilon_RE.resize(num_Points);
				epsilon_IM.resize(num_Points);
				epsilon_NORM.resize(num_Points);

				/// ---------------------------------------------------------------------------------------------------------------
				/// delta_Epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				complex<double> delta_Epsilon_Ang, epsilon_Ang;

				QVector<double> spectral_Points (1, measur->wavelength.value);

				// if known material
				if(common_Data.composed_Material == false)
				{
					Material_Data temp_Material_Data = optical_Constants->material_Map.value(common_Data.material + nk_Ext);
					QVector<complex<double>> n;
					int status = optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, warning_Text, common_Data.material);
					if(status!=0){return status;}
					delta_Epsilon_Ang = 1. - n.first()*n.first();
				} else
				// compile from elements
				{
					QVector<complex<double>> temp_Epsilon;
					int status = optical_Constants->make_Epsilon_From_Factors(common_Data.composition, common_Data.absolute_Density.value, spectral_Points, temp_Epsilon, warning_Text);
					if(status!=0){return status;}
					delta_Epsilon_Ang = 1. - temp_Epsilon.first();
				}

				/// ---------------------------------------------------------------------------------------------------------------
				/// epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				// if density
				if(common_Data.separate_Optical_Constants != TRIL_TRUE)
				{
					// if absolute density
					if(common_Data.composed_Material)		epsilon_Ang = 1. - delta_Epsilon_Ang;
					// if relative density
					else									epsilon_Ang = 1. - common_Data.relative_Density.value * delta_Epsilon_Ang;

				} else
				// if separate optical constants
				{
					delta_Epsilon_Ang = complex<double>(real(delta_Epsilon_Ang) * common_Data.permittivity.value / 100.,
														imag(delta_Epsilon_Ang) * common_Data.absorption  .value / 100.);
					epsilon_Ang = 1. - delta_Epsilon_Ang;
				}

				// fill epsilon with equal epsilons
				for(int point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					epsilon     [point_Index] =      epsilon_Ang;
					epsilon_RE  [point_Index] = real(epsilon_Ang);
					epsilon_IM  [point_Index] = imag(epsilon_Ang);
					epsilon_NORM[point_Index] = epsilon_RE[point_Index]*epsilon_RE[point_Index] + epsilon_IM[point_Index]*epsilon_IM[point_Index];
				}
			}

			// if wavelength is changing
			if(active_Whats_This_List[1] == whats_This_Wavelength)
			{
				// measured points
				num_Points = measur->lambda.size();
				k = measur->k.toStdVector();
				cos2.resize(num_Points);
				for(int point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					cos2[point_Index] = measur->cos2_Value;
				}

				delta_Epsilon.resize(num_Points);
				epsilon.resize(num_Points);
				epsilon_RE.resize(num_Points);
				epsilon_IM.resize(num_Points);
				epsilon_NORM.resize(num_Points);

				/// ---------------------------------------------------------------------------------------------------------------
				/// delta_Epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				QVector<double> spectral_Points = measur->lambda;

				// if known material
				if(common_Data.composed_Material == false)
				{
					Material_Data temp_Material_Data = optical_Constants->material_Map.value(common_Data.material + nk_Ext);
					QVector<complex<double>> n;
					int status = optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, warning_Text, common_Data.material);
					if(status!=0){return status;}

					for(int point_Index = 0; point_Index<num_Points; ++point_Index)
					{
						delta_Epsilon[point_Index] = 1. - n[point_Index]*n[point_Index];
					}
				} else
				// compile from elements
				{
					QVector<complex<double>> temp_Epsilon;
					int status = optical_Constants->make_Epsilon_From_Factors(common_Data.composition, common_Data.absolute_Density.value, spectral_Points, temp_Epsilon, warning_Text);
					if(status!=0){return status;}

					for(int point_Index = 0; point_Index<num_Points; ++point_Index)
					{
						delta_Epsilon[point_Index] = 1. - temp_Epsilon[point_Index];
					}
				}

				/// ---------------------------------------------------------------------------------------------------------------
				/// epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				// if density
				if(common_Data.separate_Optical_Constants != TRIL_TRUE)
				{
					// if absolute density
					if(common_Data.composed_Material)
															for(int point_Index = 0; point_Index<num_Points; ++point_Index)
															{
																epsilon[point_Index] = 1. - delta_Epsilon[point_Index];
															}
					// if relative density
					else									for(int point_Index = 0; point_Index<num_Points; ++point_Index)
															{
																epsilon[point_Index] = 1. - common_Data.relative_Density.value * delta_Epsilon[point_Index];
															}
				} else
				// if separate optical constants
				{
					for(int point_Index = 0; point_Index<num_Points; ++point_Index)
					{
						delta_Epsilon[point_Index] = complex<double>(real(delta_Epsilon[point_Index]) * common_Data.permittivity.value / 100.,
																	 imag(delta_Epsilon[point_Index]) * common_Data.absorption  .value / 100.);
						epsilon[point_Index] = 1. - delta_Epsilon[point_Index];
					}
				}

				for(int point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					epsilon_RE  [point_Index] = real(epsilon[point_Index]);
					epsilon_IM  [point_Index] = imag(epsilon[point_Index]);
					epsilon_NORM[point_Index] = epsilon_RE[point_Index]*epsilon_RE[point_Index] + epsilon_IM[point_Index]*epsilon_IM[point_Index];
				}
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// hi
			/// ---------------------------------------------------------------------------------------------------------------

			if(true)
			{
				double re, im, phase, mod;
				hi_RE.resize(num_Points);
				hi_IM.resize(num_Points);

				for(int i=0; i<num_Points; ++i)
				{
					re = epsilon_RE[i] - cos2[i];
					im = epsilon_IM[i];

					phase = atan2(im, re)/2;
					mod = k[i]*sqrt(sqrt(re*re + im*im));

					hi_RE[i] = mod* cos(phase);
					hi_IM[i] = mod* sin(phase);
				}
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// Fresnel
			/// ---------------------------------------------------------------------------------------------------------------

			if( whats_This_List[0] == whats_This_Layer   ||
				whats_This_List[0] == whats_This_Substrate )
			{
				double temp_Fre_Numer_RE, temp_Fre_Numer_IM, temp_Fre_Denom_SQARE, temp_Fre_Denom_RE, temp_Fre_Denom_IM, temp_1_RE, temp_1_IM, temp_2_RE, temp_2_IM;
				// s-polarization
				if (measur->polarization.value >-1)
				{
					Fresnel_s_RE.resize(num_Points);
					Fresnel_s_IM.resize(num_Points);
					for (int i=0; i<num_Points; ++i)
					{
						temp_Fre_Denom_RE = hi_RE[i] + above_Node->hi_RE[i];
						temp_Fre_Denom_IM = hi_IM[i] + above_Node->hi_IM[i];
						temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

						if ( abs(temp_Fre_Denom_SQARE) > DBL_EPSILON)
						{
							temp_Fre_Numer_RE = hi_RE[i] - above_Node->hi_RE[i];
							temp_Fre_Numer_IM = hi_IM[i] - above_Node->hi_IM[i];

							Fresnel_s_RE[i] = -(temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM) / temp_Fre_Denom_SQARE;
							Fresnel_s_IM[i] = -(temp_Fre_Numer_IM*temp_Fre_Denom_RE - temp_Fre_Numer_RE*temp_Fre_Denom_IM) / temp_Fre_Denom_SQARE;
						} else
						{
							Fresnel_s_RE[i] = 0;
							Fresnel_s_IM[i] = 0;
						}
					}
				}
				// p-polarization
				if (measur->polarization.value < 1)
				{
					Fresnel_p_RE.resize(num_Points);
					Fresnel_p_IM.resize(num_Points);
					for (int i=0; i<num_Points; ++i)
					{
						temp_1_RE = (hi_RE[i]*epsilon_RE[i] + hi_IM[i]*epsilon_IM[i]) / epsilon_NORM[i];
						temp_1_IM = (hi_IM[i]*epsilon_RE[i] - hi_RE[i]*epsilon_IM[i]) / epsilon_NORM[i];

						temp_2_RE = (above_Node->hi_RE[i]*above_Node->epsilon_RE[i] + above_Node->hi_IM[i]*above_Node->epsilon_IM[i]) / above_Node->epsilon_NORM[i];
						temp_2_IM = (above_Node->hi_IM[i]*above_Node->epsilon_RE[i] - above_Node->hi_RE[i]*above_Node->epsilon_IM[i]) / above_Node->epsilon_NORM[i];

						temp_Fre_Denom_RE = temp_1_RE + temp_2_RE;
						temp_Fre_Denom_IM = temp_1_IM + temp_2_IM;
						temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

						if ( abs(temp_Fre_Denom_SQARE) > DBL_EPSILON )
						{
							temp_Fre_Numer_RE = temp_1_RE - temp_2_RE;
							temp_Fre_Numer_IM = temp_1_IM - temp_2_IM;

							Fresnel_p_RE[i] = -(temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM) / temp_Fre_Denom_SQARE;
							Fresnel_p_IM[i] = -(temp_Fre_Numer_IM*temp_Fre_Denom_RE - temp_Fre_Numer_RE*temp_Fre_Denom_IM) / temp_Fre_Denom_SQARE ;

						} else
						{
							Fresnel_p_RE[i] = 0;
							Fresnel_p_IM[i] = 0;
						}
					}
				}
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// weak_Factor_Ang	(if not sigma-graded)
			/// ---------------------------------------------------------------------------------------------------------------

//			if( !sigma_Grading )
			if( whats_This_List[0] == whats_This_Layer   ||
				whats_This_List[0] == whats_This_Substrate )
			{
				weak_Factor.resize(num_Points);

				// if >=1 interlayer is turned on
				bool is_Norm = false;
				for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
				{
					is_Norm = is_Norm || boundary_Interlayer_Composition[func_Index].enabled;
				}

				if(is_Norm && (abs(sigma) > DBL_EPSILON) )
				{
					// temp variables
					double a = M_PI/sqrt(M_PI*M_PI - 8.);
					double factor, x, y;

					double norm = 0;
					double my_Sigma = 0;
					vector<double> s (num_Points);

					for(int i=0; i<num_Points; ++i)
					{
						s[i] = sqrt(above_Node->hi_RE[i]*hi_RE[i]);
					}

					//-------------------------------------------------------------------------------
					// erf interlayer
					if(boundary_Interlayer_Composition[Erf].enabled)
					{
						norm += boundary_Interlayer_Composition[Erf].interlayer.value;
						my_Sigma = boundary_Interlayer_Composition[Erf].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							factor = exp( - 2 * s[i] * s[i] * my_Sigma * my_Sigma );
							weak_Factor[i] += boundary_Interlayer_Composition[Erf].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// lin interlayer
					if(boundary_Interlayer_Composition[Lin].enabled)
					{
						norm += boundary_Interlayer_Composition[Lin].interlayer.value;
						my_Sigma = boundary_Interlayer_Composition[Lin].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							x = sqrt(3.) * 2 * s[i] * my_Sigma;
							factor = sin(x) / (x);
							weak_Factor[i] += boundary_Interlayer_Composition[Lin].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// exp interlayer
					if(boundary_Interlayer_Composition[Exp].enabled)
					{
						norm += boundary_Interlayer_Composition[Exp].interlayer.value;
						my_Sigma = boundary_Interlayer_Composition[Exp].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							x = 2 * pow(s[i] * my_Sigma, 2);
							factor = 1. / (1. + x);
							weak_Factor[i] += boundary_Interlayer_Composition[Exp].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// tanh interlayer
					if(boundary_Interlayer_Composition[Tanh].enabled)
					{
						norm += boundary_Interlayer_Composition[Tanh].interlayer.value;
						my_Sigma = boundary_Interlayer_Composition[Tanh].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							x = 2 * sqrt(3.) * s[i] * my_Sigma;
							factor = x / sinh(x);
							weak_Factor[i] += boundary_Interlayer_Composition[Tanh].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// sin interlayer
					if(boundary_Interlayer_Composition[Sin].enabled)
					{
						norm += boundary_Interlayer_Composition[Sin].interlayer.value;
						my_Sigma = boundary_Interlayer_Composition[Sin].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							x = 2 * a * my_Sigma * s[i] - M_PI_2;
							y = x + M_PI;
							factor = M_PI_4 * (sin(x)/x + sin(y)/y);
							weak_Factor[i] += boundary_Interlayer_Composition[Sin].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// normalization
					{
						if( norm > 0 )
						for(int i=0; i<num_Points; ++i)
						{
							weak_Factor[i] /= norm;
						}
						else
						for(int i=0; i<num_Points; ++i)
						{
							weak_Factor[i] = 1;
						}
					}

				} else
				{
					for(int i=0; i<num_Points; ++i)
					{
						weak_Factor[i] = 1;
					}
				}
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// exponenta (if not depth-graded)
			/// ---------------------------------------------------------------------------------------------------------------

//			if( !depth_Grading )
			if( whats_This_List[0] == whats_This_Layer)
			{
				double re, im, ere;
				exponenta_RE.resize(num_Points);
				exponenta_IM.resize(num_Points);

				for(int i=0; i<num_Points; ++i)
				{
					re = -2.*hi_IM[i]*thickness;
					im =  2.*hi_RE[i]*thickness;
					ere = exp(re);

					exponenta_RE[i] = ere*cos(im);
					exponenta_IM[i] = ere*sin(im);
				}
			}
		}
	}
	return 0;
}
