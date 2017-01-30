// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

int Node::calculate_Intermediate_Points(tree<Node>::iterator& active_Iter, Node* above_Node, QString active_Whats_This, QString& warning_Text)
{
	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// if angle is changing
	if(active_Whats_This_List[1] == whats_This_Angle)
	{
		// we know that active data type == "Measurement"
		Measurement* measur = &(active_Iter.node->data.measurement);
		int num_Points = measur->cos2.size();

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

				sigma = layer.sigma.value;
				boundary_Interlayer_Composition = layer.interlayer_Composition.toStdVector();

				thickness = layer.thickness.value;
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

				sigma = substrate.sigma.value;
				boundary_Interlayer_Composition = substrate.interlayer_Composition.toStdVector();;
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// delta_Epsilon_Ang
			/// ---------------------------------------------------------------------------------------------------------------

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
			/// epsilon_Ang
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

			#ifdef REAL_VALUED    // real-valued
				epsilon_Ang_RE = real(epsilon_Ang);
				epsilon_Ang_IM = imag(epsilon_Ang);
				epsilon_Ang_NORM = epsilon_Ang_RE*epsilon_Ang_RE + epsilon_Ang_IM*epsilon_Ang_IM;
			#endif

			/// ---------------------------------------------------------------------------------------------------------------
			/// hi_Ang
			/// ---------------------------------------------------------------------------------------------------------------

			{
				#ifndef REAL_VALUED    // complex-valued

					double re, im, phase, mod;
					complex<double> com;
					hi_Ang.resize(num_Points);

					for(int i=0; i<num_Points; ++i)
					{
						com = epsilon_Ang - measur->cos2[i];
						re = real(com);
						im = imag(com);

						phase = atan2(im, re)/2;
						mod = measur->k_Value*sqrt(sqrt(re*re + im*im));

						hi_Ang   [i] = mod*(cos(phase) + I*sin(phase));
					}

				#else	               // real-valued

					double re, im, phase, mod;
					complex<double> com;
					hi_Ang_RE.resize(num_Points);
					hi_Ang_IM.resize(num_Points);

					for(int i=0; i<num_Points; ++i)
					{
						com = epsilon_Ang - measur->cos2[i];
						re = real(com);
						im = imag(com);

						phase = atan2(im, re)/2;
						mod = measur->k_Value*sqrt(sqrt(re*re + im*im));

						hi_Ang_RE[i] = mod* cos(phase);
						hi_Ang_IM[i] = mod* sin(phase);
					}

				#endif
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// Fresnel_Ang
			/// ---------------------------------------------------------------------------------------------------------------

			if( whats_This_List[0] == whats_This_Layer   ||
				whats_This_List[0] == whats_This_Substrate )
			{
				#ifndef REAL_VALUED    // complex-valued

					complex<double> temp_Fre_Numer, temp_Fre_Denom, temp_1, temp_2;
					// s-polarization
					if (measur->polarization.value >-1)
					{
						Fresnel_s_Ang.resize(num_Points);
						for (int i=0; i<num_Points; ++i)
						{
							temp_Fre_Denom = hi_Ang[i] + above_Node->hi_Ang[i];
							if ( abs(temp_Fre_Denom) > DBL_EPSILON )
							{
								temp_Fre_Numer = hi_Ang[i] - above_Node->hi_Ang[i];
								Fresnel_s_Ang[i] = -temp_Fre_Numer / temp_Fre_Denom;
							} else
							{
								Fresnel_s_Ang[i] = 0;
							}
						}
					}
					// p-polarization
					if (measur->polarization.value < 1)
					{
						Fresnel_p_Ang.resize(num_Points);
						for (int i=0; i<num_Points; ++i)
						{
							temp_1 =			 hi_Ang[i]/			   epsilon_Ang;
							temp_2 = above_Node->hi_Ang[i]/above_Node->epsilon_Ang;
							temp_Fre_Denom = temp_1 + temp_2;

							if ( abs(temp_Fre_Denom) > DBL_EPSILON )
							{
								temp_Fre_Numer = temp_1 - temp_2;
								Fresnel_p_Ang[i] = -temp_Fre_Numer / temp_Fre_Denom;
							} else
							{
								Fresnel_p_Ang[i] = 0;
							}
						}
					}

				#else	               // real-valued

					double temp_Fre_Numer_RE, temp_Fre_Numer_IM, temp_Fre_Denom_SQARE, temp_Fre_Denom_RE, temp_Fre_Denom_IM, temp_1_RE, temp_1_IM, temp_2_RE, temp_2_IM;

					// s-polarization
					if (measur->polarization.value >-1)
					{
						Fresnel_s_Ang_RE.resize(num_Points);
						Fresnel_s_Ang_IM.resize(num_Points);
						for (int i=0; i<num_Points; ++i)
						{
							temp_Fre_Denom_RE = hi_Ang_RE[i] + above_Node->hi_Ang_RE[i];
							temp_Fre_Denom_IM = hi_Ang_IM[i] + above_Node->hi_Ang_IM[i];
							temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

							if ( abs(temp_Fre_Denom_SQARE) > DBL_EPSILON)
							{
								temp_Fre_Numer_RE = hi_Ang_RE[i] - above_Node->hi_Ang_RE[i];
								temp_Fre_Numer_IM = hi_Ang_IM[i] - above_Node->hi_Ang_IM[i];

								Fresnel_s_Ang_RE[i] = -(temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM) / temp_Fre_Denom_SQARE;
								Fresnel_s_Ang_IM[i] = -(temp_Fre_Numer_IM*temp_Fre_Denom_RE - temp_Fre_Numer_RE*temp_Fre_Denom_IM) / temp_Fre_Denom_SQARE;
							} else
							{
								Fresnel_s_Ang_RE[i] = 0;
								Fresnel_s_Ang_IM[i] = 0;
							}
						}
					}
					// p-polarization
					if (measur->polarization.value < 1)
					{
						Fresnel_p_Ang_RE.resize(num_Points);
						Fresnel_p_Ang_IM.resize(num_Points);
						for (int i=0; i<num_Points; ++i)
						{
							temp_1_RE = (hi_Ang_RE[i]*epsilon_Ang_RE + hi_Ang_IM[i]*epsilon_Ang_IM) / epsilon_Ang_NORM;
							temp_1_IM = (hi_Ang_IM[i]*epsilon_Ang_RE - hi_Ang_RE[i]*epsilon_Ang_IM) / epsilon_Ang_NORM;

							temp_2_RE = (above_Node->hi_Ang_RE[i]*above_Node->epsilon_Ang_RE + above_Node->hi_Ang_IM[i]*above_Node->epsilon_Ang_IM) / above_Node->epsilon_Ang_NORM;
							temp_2_IM = (above_Node->hi_Ang_IM[i]*above_Node->epsilon_Ang_RE - above_Node->hi_Ang_RE[i]*above_Node->epsilon_Ang_IM) / above_Node->epsilon_Ang_NORM;

							temp_Fre_Denom_RE = temp_1_RE + temp_2_RE;
							temp_Fre_Denom_IM = temp_1_IM + temp_2_IM;
							temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

							if ( abs(temp_Fre_Denom_SQARE) > DBL_EPSILON )
							{
								temp_Fre_Numer_RE = temp_1_RE - temp_2_RE;
								temp_Fre_Numer_IM = temp_1_IM - temp_2_IM;

								Fresnel_p_Ang_RE[i] = -(temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM) / temp_Fre_Denom_SQARE;
								Fresnel_p_Ang_IM[i] = -(temp_Fre_Numer_IM*temp_Fre_Denom_RE - temp_Fre_Numer_RE*temp_Fre_Denom_IM) / temp_Fre_Denom_SQARE ;

							} else
							{
								Fresnel_p_Ang_RE[i] = 0;
								Fresnel_p_Ang_IM[i] = 0;
							}
						}
					}

				#endif
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// weak_Factor_Ang	(if not sigma-graded)
			/// ---------------------------------------------------------------------------------------------------------------

			if( whats_This_List[0] == whats_This_Layer   ||
				whats_This_List[0] == whats_This_Substrate )
			{
				weak_Factor_Ang.resize(num_Points);

				// if >=1 interlayer is turned on
				bool is_Norm = false;
				for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
				{
					is_Norm = is_Norm || boundary_Interlayer_Composition[func_Index].enabled;
				}

				if(is_Norm && abs(sigma) > DBL_EPSILON)
				{
					// temp variables
					double a = M_PI/sqrt(M_PI*M_PI - 8.);
					double factor, x, y;

					double norm = 0;
					vector<double> s (num_Points);

					for(int i=0; i<num_Points; ++i)
					{
						#ifndef REAL_VALUED    // complex-valued

							s[i] = sqrt(real(above_Node->hi_Ang[i])*real(hi_Ang[i]));

						#else	               // real-valued

							s[i] = sqrt(above_Node->hi_Ang_RE[i]*hi_Ang_RE[i]);

						#endif
					}

					//-------------------------------------------------------------------------------
					// erf interlayer
					if(boundary_Interlayer_Composition[Erf].enabled)
					{
						norm += boundary_Interlayer_Composition[Erf].interlayer.value;
						for(int i=0; i<num_Points; ++i)
						{
							factor = exp( - 2 * s[i] * s[i] * sigma * sigma );
							weak_Factor_Ang[i] += boundary_Interlayer_Composition[Erf].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// lin interlayer
					if(boundary_Interlayer_Composition[Lin].enabled)
					{
						norm += boundary_Interlayer_Composition[Lin].interlayer.value;
						for(int i=0; i<num_Points; ++i)
						{
							x = sqrt(3.) * 2 * s[i] * sigma;
							factor = sin(x) / (x);
							weak_Factor_Ang[i] += boundary_Interlayer_Composition[Lin].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// exp interlayer
					if(boundary_Interlayer_Composition[Exp].enabled)
					{
						norm += boundary_Interlayer_Composition[Exp].interlayer.value;
						for(int i=0; i<num_Points; ++i)
						{
							x = 2 * pow(s[i] * sigma, 2);
							factor = 1. / (1. + x);
							weak_Factor_Ang[i] += boundary_Interlayer_Composition[Exp].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// tanh interlayer
					if(boundary_Interlayer_Composition[Tanh].enabled)
					{
						norm += boundary_Interlayer_Composition[Tanh].interlayer.value;
						for(int i=0; i<num_Points; ++i)
						{
							x = 2 * sqrt(3.) * s[i] * sigma;
							factor = x / sinh(x);
							weak_Factor_Ang[i] += boundary_Interlayer_Composition[Tanh].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// sin interlayer
					if(boundary_Interlayer_Composition[Sin].enabled)
					{
						norm += boundary_Interlayer_Composition[Sin].interlayer.value;
						for(int i=0; i<num_Points; ++i)
						{
							x = 2 * a * sigma * s[i] - M_PI_2;
							y = x + M_PI;
							factor = M_PI_4 * (sin(x)/x + sin(y)/y);
							weak_Factor_Ang[i] += boundary_Interlayer_Composition[Sin].interlayer.value * factor;
						}
					}
					//-------------------------------------------------------------------------------
					// normalization
					{
						if( norm > 0 )
						for(int i=0; i<num_Points; ++i)
						{
							weak_Factor_Ang[i] /= norm;
						}
						else
						for(int i=0; i<num_Points; ++i)
						{
							weak_Factor_Ang[i] = 1;
						}
					}

				} else
				{
					for(int i=0; i<num_Points; ++i)
					{
						weak_Factor_Ang[i] = 1;
					}
				}
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// exponenta_Ang (if not depth-graded)
			/// ---------------------------------------------------------------------------------------------------------------

			if( whats_This_List[0] == whats_This_Layer)
			{
				#ifndef REAL_VALUED    // complex-valued

					double re, im;
					complex<double> com;
					exponenta_Ang.resize(num_Points);

					for (int i = 0; i < num_Points; ++i)
					{
						com = 2.*(I*hi_Ang[i])*thickness;
						re = real(com);
						im = imag(com);

						exponenta_Ang[i] = exp(re)*(cos(im) + I*sin(im));
					}

				#else	               // real-valued

					double re, im, ere;
					exponenta_Ang_RE.resize(num_Points);
					exponenta_Ang_IM.resize(num_Points);

					for(int i=0; i<num_Points; ++i)
					{
						re = -2.*hi_Ang_IM[i]*thickness;
						im =  2.*hi_Ang_RE[i]*thickness;
						ere = exp(re);

						exponenta_Ang_RE[i] = ere*cos(im);
						exponenta_Ang_IM[i] = ere*sin(im);
					}

				#endif
			}

//			for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
//			{
//				qInfo() << layer.sigma.value << " with " << layer.interlayer_Composition[func_Index].enabled;
//			} qInfo() << " ";
//			cout << common_Data.material.toStdString() << " has Epsilon = " << epsilon << endl;
		}
	}
	return 0;
}
