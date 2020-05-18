#include "node.h"
#include <iostream>


Node::Node()
{
//	qInfo() << "empty NODE" << endl;
}

Node::Node(QTreeWidgetItem* item):
	struct_Data(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>())
{
//	qInfo() << "NODE" << struct_Data.item_Type << endl;
}

void Node::calculate_Intermediate_Points(const Data& measurement, Node* above_Node, bool depth_Grading, bool sigma_Grading, bool enable_Discretization)
{
	// PARAMETER
	{
		// crutch
		struct_Data.relative_Density.value = max(struct_Data.relative_Density.value,DBL_EPSILON);

		int num_Points = 0;
		vector<double> cos2;
		vector<double> k;

		// for any media
		if( struct_Data.item_Type == item_Type_Ambient ||
			struct_Data.item_Type == item_Type_Layer   ||
			struct_Data.item_Type == item_Type_Substrate )
		{
			// if angle is changing
			if( measurement.measurement_Type == measurement_Types[Specular_Scan] &&
				measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
			{
				// measured points
				num_Points = measurement.beam_Theta_0_Cos2_Vec.size();
				cos2 = measurement.beam_Theta_0_Cos2_Vec;
				k.resize(num_Points);
				for(int point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					k[point_Index] = measurement.k_Value;
				}

				epsilon.resize(num_Points);

				/// ---------------------------------------------------------------------------------------------------------------
				/// delta_Epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				complex<double> delta_Epsilon_Ang, epsilon_Ang;

				vector<double> spectral_Points (1, measurement.wavelength.value);

				// if known material
				if(struct_Data.composed_Material == false)
				{
					Material_Data temp_Material_Data = optical_Constants->material_Map.value(struct_Data.approved_Material + nk_Ext);
					vector<complex<double>> n;
					optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, struct_Data.approved_Material);
					delta_Epsilon_Ang = 1. - n.front()*n.front();
				} else
				// compile from elements
				{
					vector<complex<double>> temp_Epsilon;
					optical_Constants->make_Epsilon_From_Factors(struct_Data.composition, struct_Data.absolute_Density.value, spectral_Points, temp_Epsilon);
					delta_Epsilon_Ang = 1. - temp_Epsilon.front();
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
				}
			}

			// if wavelength is changing
			if( measurement.measurement_Type == measurement_Types[Specular_Scan] &&
				measurement.argument_Type == argument_Types[Wavelength_Energy] )
			{
				// measured points
				num_Points = measurement.lambda_Vec.size();
				k = measurement.k_Vec;
				cos2.resize(num_Points);
				for(int point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					cos2[point_Index] = measurement.beam_Theta_0_Cos2_Value;
				}

				delta_Epsilon.resize(num_Points);
				epsilon.resize(num_Points);

				/// ---------------------------------------------------------------------------------------------------------------
				/// delta_Epsilon
				/// ---------------------------------------------------------------------------------------------------------------

				vector<double> spectral_Points = measurement.lambda_Vec;

				// if known material
				if(struct_Data.composed_Material == false)
				{
					Material_Data temp_Material_Data = optical_Constants->material_Map.value(struct_Data.approved_Material + nk_Ext);
					vector<complex<double>> n;
					optical_Constants->interpolation_Epsilon(temp_Material_Data.material_Data, spectral_Points, n, struct_Data.approved_Material);

					for(int point_Index = 0; point_Index<num_Points; ++point_Index)
					{
						delta_Epsilon[point_Index] = 1. - n[point_Index]*n[point_Index];
					}
				} else
				// compile from elements
				{
					vector<complex<double>> temp_Epsilon;
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
			}

			// anyway
			if( struct_Data.item_Type == item_Type_Layer   ||
				struct_Data.item_Type == item_Type_Substrate )
			{
				if(struct_Data.common_Sigma)
				{
					for(Interlayer& inter : struct_Data.interlayer_Composition)
					{
						inter.my_Sigma.value = struct_Data.sigma.value;
					}
				}
			}

			// only without discretization
			if(!enable_Discretization)
			{
				/// ---------------------------------------------------------------------------------------------------------------
				/// hi
				/// ---------------------------------------------------------------------------------------------------------------

				if(true)
				{
					hi.resize(num_Points);
//					Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
//					{
//						for(int i=n_Min; i<n_Max; ++i)
						for(int i=0; i<num_Points; ++i)
						{
							hi[i] = k[i]*sqrt(epsilon[i] - cos2[i]);
						}
//					});
				}

				/// ---------------------------------------------------------------------------------------------------------------
				/// Fresnel
				/// ---------------------------------------------------------------------------------------------------------------

				if( struct_Data.item_Type == item_Type_Layer   ||
					struct_Data.item_Type == item_Type_Substrate )
				{
					// s-polarization
					if( measurement.polarization > -1+POLARIZATION_TOLERANCE )			// s-polarization only
					{
						// reflectance
						Fresnel_R_s.resize(num_Points);
						// transmittance
						Fresnel_T_s.resize(num_Points);

//						Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
//						{
							for(int i=0; i<num_Points; ++i)
//							for (int i=n_Min; i<n_Max; ++i)
							{
								if ( abs(above_Node->hi[i] + hi[i]) > DBL_MIN)
								{
									// reflectance
									Fresnel_R_s[i] = (above_Node->hi[i] - hi[i]) / (above_Node->hi[i] + hi[i]);
									// transmittance
									Fresnel_T_s[i] = 2.*above_Node->hi[i] / (above_Node->hi[i] + hi[i]);
								} else
								{
									// reflectance
									Fresnel_R_s[i] = 0;
									// transmittance
									Fresnel_T_s[i] = 1;
								}
							}
//						});
					}

					// p-polarization
					if (measurement.polarization < 1-POLARIZATION_TOLERANCE)
					{
						// reflectance
						Fresnel_R_p.resize(num_Points);
						// transmittance
						Fresnel_T_p.resize(num_Points);

		//				Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
		//				{
							complex<double> hi_je, hi_j1e;
							for(int i=0; i<num_Points; ++i)
		//					for (int i=n_Min; i<n_Max; ++i)
							{
								hi_je = above_Node->hi[i]/above_Node->epsilon[i];
								hi_j1e = hi[i]/epsilon[i];

								if ( abs(hi_je+hi_j1e) > DBL_MIN )
								{
									// reflectance
									Fresnel_R_p[i] = (hi_je-hi_j1e) / (hi_je+hi_j1e);
									// transmittance
									Fresnel_T_p[i] = 2.*hi_je / (hi_je+hi_j1e);
								} else
								{
									// reflectance
									Fresnel_R_p[i] = 0;
									// transmittance
									Fresnel_T_p[i] = 1;
								}
							}
		//				});
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

						double norm = 0;
						double my_Sigma = struct_Data.sigma.value;	// by default, otherwise we change it
						vector<complex<double>> s_r (num_Points);
						vector<complex<double>> s_t (num_Points);

						// reftectance
	//					Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//					{
							for(int i=0; i<num_Points; ++i)
	//						for(int i=n_Min; i<n_Max; ++i)
							{
								weak_Factor_R[i] = 0;
								s_r[i] = sqrt(above_Node->hi[i]*hi[i]);
								// transmittance
								weak_Factor_T[i] = 0;
								s_t[i] = (above_Node->hi[i]-hi[i])/2.;
							}
	//					});

						//-------------------------------------------------------------------------------
						// erf interlayer
						if(struct_Data.interlayer_Composition[Erf].enabled)
						if(struct_Data.interlayer_Composition[Erf].interlayer.value > DBL_MIN)
						{
							norm += struct_Data.interlayer_Composition[Erf].interlayer.value;
							if(!struct_Data.common_Sigma) {
								my_Sigma = struct_Data.interlayer_Composition[Erf].my_Sigma.value;}

	//						Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//						{
								complex<double> factor_r;
								complex<double> factor_t;
								for(int i=0; i<num_Points; ++i)
	//							for(int i=n_Min; i<n_Max; ++i)
								{
									// reflectance
									factor_r = exp( - s_r[i] * s_r[i] * my_Sigma * my_Sigma * 2. );
									weak_Factor_R[i] += struct_Data.interlayer_Composition[Erf].interlayer.value * factor_r;

									// transmittance
									factor_t = exp(   s_t[i] * s_t[i] * my_Sigma * my_Sigma * 2. );
									weak_Factor_T[i] += struct_Data.interlayer_Composition[Erf].interlayer.value * factor_t;
								}
	//						});
						}
						//-------------------------------------------------------------------------------
						// lin interlayer
						if(struct_Data.interlayer_Composition[Lin].enabled)
						if(struct_Data.interlayer_Composition[Lin].interlayer.value > DBL_MIN)
						{
							norm += struct_Data.interlayer_Composition[Lin].interlayer.value;
							if(!struct_Data.common_Sigma) {
								my_Sigma = struct_Data.interlayer_Composition[Lin].my_Sigma.value;}

	//						Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//						{
								complex<double> factor_r, x_r;
								complex<double> factor_t, x_t;
	//							for(int i=n_Min; i<n_Max; ++i)
								for(int i=0; i<num_Points; ++i)
								{
									// reflectance
									x_r = sqrt(3.) * s_r[i] * my_Sigma * 2.;
									if(abs(x_r)>DBL_MIN) {
										factor_r = sin(x_r) / (x_r);
									}
									else {
										factor_r = 1;
									}
									weak_Factor_R[i] += struct_Data.interlayer_Composition[Lin].interlayer.value * factor_r;

									// transmittance
									x_t = sqrt(3.) * s_t[i] * my_Sigma * 2.;
									if(abs(x_t)>DBL_MIN) {
										factor_t = sin(x_t) / (x_t);
									} else {
										factor_t = 1;
									}
									weak_Factor_T[i] += struct_Data.interlayer_Composition[Lin].interlayer.value * factor_t;
								}
	//						});
						}
						//-------------------------------------------------------------------------------
						// exp interlayer
						if(struct_Data.interlayer_Composition[Exp].enabled)
						if(struct_Data.interlayer_Composition[Exp].interlayer.value > DBL_MIN)
						{
							norm += struct_Data.interlayer_Composition[Exp].interlayer.value;
							if(!struct_Data.common_Sigma) {
								my_Sigma = struct_Data.interlayer_Composition[Exp].my_Sigma.value;}

	//						Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//						{
								complex<double> factor_r, x_r;
								complex<double> factor_t, x_t;
								for(int i=0; i<num_Points; ++i)
	//							for(int i=n_Min; i<n_Max; ++i)
								{
									// reflectance
									x_r = pow(s_r[i] * my_Sigma, 2) * 2.;
									factor_r = 1. / (1. + x_r);
									weak_Factor_R[i] += struct_Data.interlayer_Composition[Exp].interlayer.value * factor_r;

									// transmittance
									x_t = pow(s_t[i] * my_Sigma, 2) * 2.;
									factor_t = 1. / (1. + x_t);
									weak_Factor_T[i] += struct_Data.interlayer_Composition[Exp].interlayer.value * factor_t;
								}
	//						});
						}
						//-------------------------------------------------------------------------------
						// tanh interlayer
						if(struct_Data.interlayer_Composition[Tanh].enabled)
						if(struct_Data.interlayer_Composition[Tanh].interlayer.value > DBL_MIN)
						{
							norm += struct_Data.interlayer_Composition[Tanh].interlayer.value;
							if(!struct_Data.common_Sigma) {
								my_Sigma = struct_Data.interlayer_Composition[Tanh].my_Sigma.value;}

	//						Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//						{
								complex<double> factor_r, x_r;
								complex<double> factor_t, x_t;
								for(int i=0; i<num_Points; ++i)
	//							for(int i=n_Min; i<n_Max; ++i)
								{
									// reflectance
									x_r = sqrt(3.) * s_r[i] * my_Sigma * 2.;
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
									x_t = sqrt(3.) * s_t[i] * my_Sigma * 2.;
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
	//						});
						}
						//-------------------------------------------------------------------------------
						// sin interlayer
						if(struct_Data.interlayer_Composition[Sin].enabled)
						if(struct_Data.interlayer_Composition[Sin].interlayer.value > DBL_MIN)
						{
							norm += struct_Data.interlayer_Composition[Sin].interlayer.value;
							if(!struct_Data.common_Sigma) {
								my_Sigma = struct_Data.interlayer_Composition[Sin].my_Sigma.value;}

	//						Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//						{
								complex<double> factor_r, x_r, y_r, six_r, siy_r;
								complex<double> factor_t, x_t, y_t, six_t, siy_t;
	//							for(int i=n_Min; i<n_Max; ++i)
								for(int i=0; i<num_Points; ++i)
								{
									// reflectance
									x_r = a * my_Sigma * s_r[i] * 2. - M_PI_2;
									y_r = x_r + M_PI;

									if(abs(x_r)>DBL_MIN) six_r = sin(x_r)/x_r; else six_r = 1;
									if(abs(y_r)>DBL_MIN) siy_r = sin(y_r)/y_r; else siy_r = 1;

									factor_r = M_PI_4 * (six_r + siy_r);
									weak_Factor_R[i] += struct_Data.interlayer_Composition[Sin].interlayer.value * factor_r;

									// transmittance
									x_t = a * my_Sigma * s_t[i] * 2. - M_PI_2;
									y_t = x_t + M_PI;

									if(abs(x_t)>DBL_MIN) six_t = sin(x_t)/x_t; else six_t = 1;
									if(abs(y_t)>DBL_MIN) siy_t = sin(y_t)/y_t; else siy_t = 1;

									factor_t = M_PI_4 * (six_t + siy_t);
									weak_Factor_T[i] += struct_Data.interlayer_Composition[Sin].interlayer.value * factor_t;
								}
	//						});
						}
						//-------------------------------------------------------------------------------
						// step interlayer
						if(struct_Data.interlayer_Composition[Step].enabled)
						if(struct_Data.interlayer_Composition[Step].interlayer.value > DBL_MIN)
						{
							norm += struct_Data.interlayer_Composition[Step].interlayer.value;
							if(!struct_Data.common_Sigma) {
								my_Sigma = struct_Data.interlayer_Composition[Step].my_Sigma.value;}

	//						Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//						{
								complex<double> factor_r;
								complex<double> factor_t;
								for(int i=0; i<num_Points; ++i)
	//							for(int i=n_Min; i<n_Max; ++i)
								{
									// reflectance
									factor_r = cos(s_r[i] * my_Sigma * 2.);
									weak_Factor_R[i] += struct_Data.interlayer_Composition[Step].interlayer.value * factor_r;

									// transmittance
									factor_t = cos(s_t[i] * my_Sigma * 2.);
									weak_Factor_T[i] += struct_Data.interlayer_Composition[Step].interlayer.value * factor_t;
								}
	//						});
						}
						//-------------------------------------------------------------------------------
						// normalization
						{
							if( abs(norm) > DBL_MIN )
							{
	//							Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//							{
									for(int i=0; i<num_Points; ++i)
	//								for(int i=n_Min; i<n_Max; ++i)
									{
										weak_Factor_R[i] /= norm;
										weak_Factor_T[i] /= norm;
									}
	//							});
							} else
							{
	//							Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//							{
									for(int i=0; i<num_Points; ++i)
	//								for(int i=n_Min; i<n_Max; ++i)
									{
										weak_Factor_R[i] = 1;
										weak_Factor_T[i] = 1;
									}
	//							});
							}
						}
					} else
					{
	//					Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//					{
							for(int i=0; i<num_Points; ++i)
	//						for(int i=n_Min; i<n_Max; ++i)
							{
								weak_Factor_R[i] = 1;
								weak_Factor_T[i] = 1;
							}
	//					});
					}
				}

				/// ---------------------------------------------------------------------------------------------------------------
				/// exponenta
				/// ---------------------------------------------------------------------------------------------------------------

				// if depth graded, set exp=1 here and recalculate later
				if( struct_Data.item_Type == item_Type_Layer)
				{
					exponenta.  resize(num_Points);
					exponenta_2.resize(num_Points);

					if( depth_Grading )
					{
	//					Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//					{
							for(int i=0; i<num_Points; ++i)
	//						for(int i=n_Min; i<n_Max; ++i)
							{
								exponenta[i]   = 1.;
								exponenta_2[i] = 1.;
							}
	//					});
					} else
					{
	//					Global_Variables::parallel_For(num_Points, reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
	//					{
							for(int i=0; i<num_Points; ++i)
	//						for(int i=n_Min; i<n_Max; ++i)
							{
								exponenta[i] = exp(I*hi[i]*struct_Data.thickness.value);
								exponenta_2[i] = pow(exponenta[i],2);
							}
	//					});
					}
				}
			}
		}
	}
}
