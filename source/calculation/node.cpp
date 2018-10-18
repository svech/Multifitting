#include "node.h"
#include <iostream>


Node::Node()
{
//	qInfo() << "empty NODE";
}

Node::Node(QTreeWidgetItem* item):
	struct_Data(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>())
{
//	qInfo() << "NODE" << struct_Data.item_Type;
}

void Node::calculate_Intermediate_Points(const Data& measurement, Node* above_Node, QString active_Parameter_Whats_This, bool depth_Grading, bool sigma_Grading)
{
	// PARAMETER
	if(active_Parameter_Whats_This == whats_This_Angle ||
	   active_Parameter_Whats_This == whats_This_Wavelength )
	{
		// here we know that active item type == "Measurement"

		int num_Points = 0;
		vector<double> cos2;
		vector<double> k;

		// for any media
		if( struct_Data.item_Type == item_Type_Ambient ||
			struct_Data.item_Type == item_Type_Layer   ||
			struct_Data.item_Type == item_Type_Substrate )
		{
			// if angle is changing
			if(active_Parameter_Whats_This == whats_This_Angle)
			{
				// measured points
				num_Points = measurement.cos2.size();
				cos2 = measurement.cos2.toStdVector();
				k.resize(num_Points);
				for(int point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					k[point_Index] = measurement.k_Value;
				}

				epsilon.resize(num_Points);
				epsilon_RE.resize(num_Points);
				epsilon_IM.resize(num_Points);
				epsilon_NORM.resize(num_Points);

				/// ---------------------------------------------------------------------------------------------------------------
				/// delta_Epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				complex<double> delta_Epsilon_Ang, epsilon_Ang;

				QVector<double> spectral_Points (1, measurement.wavelength.value);

				// if known material
				if(struct_Data.composed_Material == false)
				{
					Material_Data temp_Material_Data = optical_Constants->material_Map.value(struct_Data.material + nk_Ext);
					QVector<complex<double>> n;
					optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, struct_Data.material);
					delta_Epsilon_Ang = 1. - n.first()*n.first();
				} else
				// compile from elements
				{
					QVector<complex<double>> temp_Epsilon;
					optical_Constants->make_Epsilon_From_Factors(struct_Data.composition, struct_Data.absolute_Density.value, spectral_Points, temp_Epsilon);
					delta_Epsilon_Ang = 1. - temp_Epsilon.first();
				}

				/// ---------------------------------------------------------------------------------------------------------------
				/// epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				// if density
				if(struct_Data.separate_Optical_Constants != TRIL_TRUE)
				{
					// if absolute density
					if(struct_Data.composed_Material)		epsilon_Ang = 1. - delta_Epsilon_Ang;
					// if relative density
					else									epsilon_Ang = 1. - struct_Data.relative_Density.value * delta_Epsilon_Ang;

				} else
				// if separate optical constants
				{
					delta_Epsilon_Ang = complex<double>(real(delta_Epsilon_Ang) * struct_Data.permittivity.value / 100.,
														imag(delta_Epsilon_Ang) * struct_Data.absorption  .value / 100.);
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
			if(active_Parameter_Whats_This == whats_This_Wavelength)
			{
				// measured points
				num_Points = measurement.lambda.size();
				k = measurement.k.toStdVector();
				cos2.resize(num_Points);
				for(int point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					cos2[point_Index] = measurement.cos2_Value;
				}

				delta_Epsilon.resize(num_Points);
				epsilon.resize(num_Points);
				epsilon_RE.resize(num_Points);
				epsilon_IM.resize(num_Points);
				epsilon_NORM.resize(num_Points);

				/// ---------------------------------------------------------------------------------------------------------------
				/// delta_Epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				QVector<double> spectral_Points = measurement.lambda;

				// if known material
				if(struct_Data.composed_Material == false)
				{
					Material_Data temp_Material_Data = optical_Constants->material_Map.value(struct_Data.material + nk_Ext);
					QVector<complex<double>> n;
					optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, struct_Data.material);

					for(int point_Index = 0; point_Index<num_Points; ++point_Index)
					{
						delta_Epsilon[point_Index] = 1. - n[point_Index]*n[point_Index];
					}
				} else
				// compile from elements
				{
					QVector<complex<double>> temp_Epsilon;
					optical_Constants->make_Epsilon_From_Factors(struct_Data.composition, struct_Data.absolute_Density.value, spectral_Points, temp_Epsilon);

					for(int point_Index = 0; point_Index<num_Points; ++point_Index)
					{
						delta_Epsilon[point_Index] = 1. - temp_Epsilon[point_Index];
					}
				}

				/// ---------------------------------------------------------------------------------------------------------------
				/// epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				// if density
				if(struct_Data.separate_Optical_Constants != TRIL_TRUE)
				{
					// if absolute density
					if(struct_Data.composed_Material)
															for(int point_Index = 0; point_Index<num_Points; ++point_Index)
															{
																epsilon[point_Index] = 1. - delta_Epsilon[point_Index];
															}
					// if relative density
					else									for(int point_Index = 0; point_Index<num_Points; ++point_Index)
															{
																epsilon[point_Index] = 1. - struct_Data.relative_Density.value * delta_Epsilon[point_Index];
															}
				} else
				// if separate optical constants
				{
					for(int point_Index = 0; point_Index<num_Points; ++point_Index)
					{
						delta_Epsilon[point_Index] = complex<double>(real(delta_Epsilon[point_Index]) * struct_Data.permittivity.value / 100.,
																	 imag(delta_Epsilon[point_Index]) * struct_Data.absorption  .value / 100.);
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

			if( struct_Data.item_Type == item_Type_Layer   ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				double temp_Fre_Numer_RE, temp_Fre_Numer_IM, temp_Fre_Denom_SQARE, temp_Fre_Denom_RE, temp_Fre_Denom_IM, temp_1_RE, temp_1_IM, temp_2_RE, temp_2_IM;
				// s-polarization
				if (measurement.polarization.value >-1)
				{
					Fresnel_s_RE.resize(num_Points);
					Fresnel_s_IM.resize(num_Points);
					for (int i=0; i<num_Points; ++i)
					{
						temp_Fre_Denom_RE = hi_RE[i] + above_Node->hi_RE[i];
						temp_Fre_Denom_IM = hi_IM[i] + above_Node->hi_IM[i];
						temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

						if ( abs(temp_Fre_Denom_SQARE) > DBL_MIN)
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
				if (measurement.polarization.value < 1)
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

						if ( abs(temp_Fre_Denom_SQARE) > DBL_MIN )
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
			/// weak_Factor_Ang
			/// ---------------------------------------------------------------------------------------------------------------

			// if sigma graded, set WF=1 here and recalculate later
			if( struct_Data.item_Type == item_Type_Layer   ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				weak_Factor_R.resize(num_Points);
				weak_Factor_T.resize(num_Points);

				// if >=1 interlayer is turned on
				bool is_Norm = false;
				for(const Interlayer& inter : struct_Data.interlayer_Composition)
				{
					is_Norm = is_Norm || (inter.enabled && (inter.interlayer.value > DBL_MIN) && (inter.my_Sigma.value > DBL_MIN));
				}

				if( is_Norm && (abs(struct_Data.sigma.value) > DBL_MIN) && (!sigma_Grading) )
				{
					// temp variables
					double a = M_PI/sqrt(M_PI*M_PI - 8.);
					double factor_r, x_r, y_r, six_r, siy_r;
					double factor_t, x_t, y_t, six_t, siy_t;

					double norm = 0;
					double my_Sigma = struct_Data.sigma.value;	// by default, otherwise we change it
					vector<double> s_r (num_Points);
					vector<double> s_t (num_Points);

					for(int i=0; i<num_Points; ++i)
					{
						weak_Factor_R[i] = 0;
						weak_Factor_T[i] = 0;
						s_r[i] = sqrt(above_Node->hi_RE[i]*hi_RE[i]);
						s_t[i] =      above_Node->hi_RE[i]-hi_RE[i];
					}

					//-------------------------------------------------------------------------------
					// erf interlayer
					if(struct_Data.interlayer_Composition[Erf].enabled)
					if(struct_Data.interlayer_Composition[Erf].interlayer.value > DBL_MIN)
					{
						norm += struct_Data.interlayer_Composition[Erf].interlayer.value;
						if(!struct_Data.common_Sigma)
							my_Sigma = struct_Data.interlayer_Composition[Erf].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							// reftectance
							factor_r = exp( - s_r[i] * s_r[i] * my_Sigma * my_Sigma * 2 );
							weak_Factor_R[i] += struct_Data.interlayer_Composition[Erf].interlayer.value * factor_r;

							// transmittance
							factor_t = exp(   s_t[i] * s_t[i] * my_Sigma * my_Sigma / 2 );
							weak_Factor_T[i] += struct_Data.interlayer_Composition[Erf].interlayer.value * factor_t;
						}
					}
					//-------------------------------------------------------------------------------
					// lin interlayer
					if(struct_Data.interlayer_Composition[Lin].enabled)
					if(struct_Data.interlayer_Composition[Lin].interlayer.value > DBL_MIN)
					{
						norm += struct_Data.interlayer_Composition[Lin].interlayer.value;
						if(!struct_Data.common_Sigma)
							my_Sigma = struct_Data.interlayer_Composition[Lin].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							// reftectance
							x_r = sqrt(3.) * s_r[i] * my_Sigma * 2;
							if(abs(x_r)>DBL_MIN) {
								factor_r = sin(x_r) / (x_r);
							}
							else {
								factor_r = 1;
							}
							weak_Factor_R[i] += struct_Data.interlayer_Composition[Lin].interlayer.value * factor_r;

							// transmittance
							x_t = sqrt(3.) * s_t[i] * my_Sigma * 1;
							if(abs(x_t)>DBL_MIN) {
								factor_t = sin(x_t) / (x_t);
							} else {
								factor_t = 1;
							}
							weak_Factor_T[i] += struct_Data.interlayer_Composition[Lin].interlayer.value * factor_t;
						}
					}
					//-------------------------------------------------------------------------------
					// exp interlayer
					if(struct_Data.interlayer_Composition[Exp].enabled)
					if(struct_Data.interlayer_Composition[Exp].interlayer.value > DBL_MIN)
					{
						norm += struct_Data.interlayer_Composition[Exp].interlayer.value;
						if(!struct_Data.common_Sigma)
							my_Sigma = struct_Data.interlayer_Composition[Exp].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							// reftectance
							x_r = pow(s_r[i] * my_Sigma, 2) * 2;
							factor_r = 1. / (1. + x_r);
							weak_Factor_R[i] += struct_Data.interlayer_Composition[Exp].interlayer.value * factor_r;

							// transmittance
							x_t = pow(s_t[i] * my_Sigma, 2) / 2;
							factor_t = 1. / (1. + x_t);
							weak_Factor_T[i] += struct_Data.interlayer_Composition[Exp].interlayer.value * factor_t;
						}
					}
					//-------------------------------------------------------------------------------
					// tanh interlayer
					if(struct_Data.interlayer_Composition[Tanh].enabled)
					if(struct_Data.interlayer_Composition[Tanh].interlayer.value > DBL_MIN)
					{
						norm += struct_Data.interlayer_Composition[Tanh].interlayer.value;
						if(!struct_Data.common_Sigma)
							my_Sigma = struct_Data.interlayer_Composition[Tanh].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							// reftectance
							x_r = sqrt(3.) * s_r[i] * my_Sigma * 2;
							if(abs(x_r)>DBL_MIN)
							{
								factor_r = x_r / sinh(x_r);
							}
							else
							{
								factor_r = 1;
							}
							weak_Factor_R[i] += struct_Data.interlayer_Composition[Tanh].interlayer.value * factor_r;

							// transmittance
							x_t = sqrt(3.) * s_t[i] * my_Sigma * 1;
							if(abs(x_t)>DBL_MIN)
							{
								factor_t = x_t / sinh(x_t);
							}
							else
							{
								factor_t = 1;
							}
							weak_Factor_T[i] += struct_Data.interlayer_Composition[Tanh].interlayer.value * factor_t;
						}
					}
					//-------------------------------------------------------------------------------
					// sin interlayer
					if(struct_Data.interlayer_Composition[Sin].enabled)
					if(struct_Data.interlayer_Composition[Sin].interlayer.value > DBL_MIN)
					{
						norm += struct_Data.interlayer_Composition[Sin].interlayer.value;
						if(!struct_Data.common_Sigma)
							my_Sigma = struct_Data.interlayer_Composition[Sin].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							// reftectance
							x_r = a * my_Sigma * s_r[i] * 2 - M_PI_2;
							y_r = x_r + M_PI;

							if(abs(x_r)>DBL_MIN) six_r = sin(x_r)/x_r; else six_r = 1;
							if(abs(y_r)>DBL_MIN) siy_r = sin(y_r)/y_r; else siy_r = 1;

							factor_r = M_PI_4 * (six_r + siy_r);
							weak_Factor_R[i] += struct_Data.interlayer_Composition[Sin].interlayer.value * factor_r;

							// transmittance
							x_t = a * my_Sigma * s_t[i] * 1 - M_PI_2;
							y_t = x_t + M_PI;

							if(abs(x_t)>DBL_MIN) six_t = sin(x_t)/x_t; else six_t = 1;
							if(abs(y_t)>DBL_MIN) siy_t = sin(y_t)/y_t; else siy_t = 1;

							factor_t = M_PI_4 * (six_t + siy_t);
							weak_Factor_T[i] += struct_Data.interlayer_Composition[Sin].interlayer.value * factor_t;
						}
					}
					//-------------------------------------------------------------------------------
					// step interlayer
					if(struct_Data.interlayer_Composition[Step].enabled)
					if(struct_Data.interlayer_Composition[Step].interlayer.value > DBL_MIN)
					{
						norm += struct_Data.interlayer_Composition[Step].interlayer.value;
						if(!struct_Data.common_Sigma)
							my_Sigma = struct_Data.interlayer_Composition[Step].my_Sigma.value;
						for(int i=0; i<num_Points; ++i)
						{
							// reftectance
							factor_r = cos(s_r[i] * my_Sigma * 2);
							weak_Factor_R[i] += struct_Data.interlayer_Composition[Step].interlayer.value * factor_r;

							// transmittance
							factor_t = cos(s_t[i] * my_Sigma * 1);
							weak_Factor_R[i] += struct_Data.interlayer_Composition[Step].interlayer.value * factor_t;
						}
					}
					//-------------------------------------------------------------------------------
					// normalization
					{
						if( abs(norm) > DBL_MIN )
						for(int i=0; i<num_Points; ++i)
						{
							weak_Factor_R[i] /= norm;
							weak_Factor_T[i] /= norm;
						} else
						for(int i=0; i<num_Points; ++i)
						{
							weak_Factor_R[i] = 1;
							weak_Factor_T[i] = 1;
						}
					}
				} else
				{
					for(int i=0; i<num_Points; ++i)
					{
						weak_Factor_R[i] = 1;
						weak_Factor_T[i] = 1;
					}
				}
			}

			/// ---------------------------------------------------------------------------------------------------------------
			/// exponenta
			/// ---------------------------------------------------------------------------------------------------------------

			// if depth graded, set exp=1 here and recalculate later
			if( struct_Data.item_Type == item_Type_Layer)
			{
				double re, im, ere;
				exponenta_RE.resize(num_Points);
				exponenta_IM.resize(num_Points);

				if( depth_Grading )
				{
					for(int i=0; i<num_Points; ++i)
					{
						exponenta_RE[i] = 1;
						exponenta_IM[i] = 1;
					}
				} else
				{
					for(int i=0; i<num_Points; ++i)
					{
						re = -2.*hi_IM[i]*struct_Data.thickness.value;
						im =  2.*hi_RE[i]*struct_Data.thickness.value;
						ere = exp(re);

						exponenta_RE[i] = ere*cos(im);
						exponenta_IM[i] = ere*sin(im);
					}
				}
			}
		}
	}
}
