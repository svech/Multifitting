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

void Node::calculate_Intermediate_Points(const Data& measurement, Node* above_Node, bool depth_Grading, bool inconvenient_Approximation, bool sigma_Grading, bool enable_Discretization, QString mode)
{				
//	struct_Data.relative_Density.value = max(struct_Data.relative_Density.value, DBL_EPSILON); // crutch

	size_t num_Points = 0;
	vector<double> cos2;
	vector<double> k;

	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// measured points
			num_Points = measurement.beam_Theta_0_Cos2_Vec.size();
			cos2 = measurement.beam_Theta_0_Cos2_Vec;
			k.resize(num_Points);
			for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
			{
				k[point_Index] = measurement.k_Value;
			}

			epsilon.resize(num_Points);
			vector<double> spectral_Points (1, measurement.lambda_Value);
			fill_Epsilon_For_Angular_Measurements(spectral_Points, true);
		}
		if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
		{
			// measured points
			num_Points = measurement.lambda_Vec.size();
			k = measurement.k_Vec;
			cos2.resize(num_Points);
			for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
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

				for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					delta_Epsilon[point_Index] = 1. - n[point_Index]*n[point_Index];
//					if(abs(imag(delta_Epsilon[point_Index]))<DBL_EPSILON) delta_Epsilon[point_Index] -= complex<double>(0,DBL_EPSILON);
				}
			} else
			// compile from elements
			{
				vector<complex<double>> temp_Epsilon;
				optical_Constants->make_Epsilon_From_Factors(struct_Data.composition, struct_Data.average_Layer_density(), spectral_Points, temp_Epsilon);

				for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					delta_Epsilon[point_Index] = 1. - temp_Epsilon[point_Index];
//					if(abs(imag(delta_Epsilon[point_Index]))<DBL_EPSILON) delta_Epsilon[point_Index] -= complex<double>(0,DBL_EPSILON);
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
														for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
														{
															epsilon[point_Index] = 1. - delta_Epsilon[point_Index];
														}
				// if relative density
				else									for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
														{
															epsilon[point_Index] = 1. - struct_Data.average_Layer_density() * delta_Epsilon[point_Index];
														}
			} else
			// if separate optical constants
			{
				for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
				{
					delta_Epsilon[point_Index] = complex<double>(real(delta_Epsilon[point_Index]) * struct_Data.permittivity.value / 100.,
																 imag(delta_Epsilon[point_Index]) * struct_Data.absorption  .value / 100.);
					epsilon[point_Index] = 1. - delta_Epsilon[point_Index];
				}
			}
		}
	}
	if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan] 	 ||
		measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		if(mode == SPECULAR_MODE)
		{
			if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
				measurement.measurement_Type == measurement_Types[GISAS_Map] )
			{
				num_Points = 1;
				cos2.resize(num_Points);
				cos2.front() = measurement.beam_Theta_0_Cos2_Value;
			}
			if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				measurement.measurement_Type == measurement_Types[Offset_Scan] )
			{
				num_Points = measurement.beam_Theta_0_Cos2_Vec.size();
				cos2 = measurement.beam_Theta_0_Cos2_Vec;
			}
		}
		if(mode == SCATTERED_MODE)
		{
			num_Points = measurement.detector_Theta_Cos2_Vec.size();
			cos2 = measurement.detector_Theta_Cos2_Vec;
		}

		// common
		k.resize(num_Points);
		for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
		{
			k[point_Index] = measurement.k_Value;
		}

		epsilon.resize(num_Points);
		vector<double> spectral_Points(1, measurement.lambda_Value);
		fill_Epsilon_For_Angular_Measurements(spectral_Points);

		// for scattering on density fluctuation
		if(mode == SCATTERED_MODE)
		{
			fill_Epsilon_Contrast_For_Density_Fluctuations(spectral_Points);
		}
	}

	// anyway
	if( struct_Data.item_Type == item_Type_Layer   ||
		struct_Data.item_Type == item_Type_Substrate )
	{
		if(struct_Data.common_Sigma_Diffuse)
		{
			for(Interlayer& inter : struct_Data.interlayer_Composition)
			{
				inter.my_Sigma_Diffuse.value = struct_Data.sigma_Diffuse.value;
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
			for(size_t i=0; i<num_Points; ++i)
			{
				hi[i] = k[i]*sqrt(epsilon[i] - cos2[i]);
			}
		}

		/// ---------------------------------------------------------------------------------------------------------------
		/// Fresnel
		/// ---------------------------------------------------------------------------------------------------------------

		if( struct_Data.item_Type == item_Type_Layer   ||
			struct_Data.item_Type == item_Type_Substrate )
		{
			// s-polarization
			if( measurement.polarization > -1+POLARIZATION_TOLERANCE )
			{
				Fresnel_R_s.resize(num_Points);
				Fresnel_T_s.resize(num_Points);
				for(size_t i=0; i<num_Points; ++i)
				{
					if ( norm(above_Node->hi[i] + hi[i]) > DBL_MIN)
					{
						Fresnel_R_s[i] = (above_Node->hi[i] - hi[i]) / (above_Node->hi[i] + hi[i]);
						Fresnel_T_s[i] = 2.*above_Node->hi[i] / (above_Node->hi[i] + hi[i]);
					} else
					{
						Fresnel_R_s[i] = 0;
						Fresnel_T_s[i] = 1;
					}
				}
			}

			// p-polarization
			if (measurement.polarization < 1-POLARIZATION_TOLERANCE)
			{
				complex<double> hi_je, hi_j1e;
				Fresnel_R_p.resize(num_Points);
				Fresnel_T_p.resize(num_Points);
				for(size_t i=0; i<num_Points; ++i)
				{
					hi_je = above_Node->hi[i]/above_Node->epsilon[i];
					hi_j1e = hi[i]/epsilon[i];

					if ( norm(hi_je+hi_j1e) > DBL_MIN )
					{
						Fresnel_R_p[i] = (hi_je-hi_j1e) / (hi_je+hi_j1e);
						Fresnel_T_p[i] = 2.*hi_je / (hi_je+hi_j1e);
					} else
					{
						Fresnel_R_p[i] = 0;
						Fresnel_T_p[i] = 1;
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
				is_Norm = is_Norm || (inter.enabled && (inter.interlayer.value > DBL_MIN) && (inter.my_Sigma_Diffuse.value > DBL_MIN));
			}

			if( is_Norm && (abs(struct_Data.sigma_Diffuse.value) > DBL_MIN) && !sigma_Grading && !inconvenient_Approximation
				&& (measurement.measurement_Type == measurement_Types[Specular_Scan] || mode == SPECULAR_MODE)
				)
			{
				// temp variables
				double a = M_PI/sqrt(M_PI*M_PI - 8.);

				double norm = 0;
				double my_Sigma = struct_Data.sigma_Diffuse.value;	// by default, otherwise we change it
				vector<complex<double>> s_r (num_Points);
				vector<complex<double>> s_t (num_Points);

				for(size_t i=0; i<num_Points; ++i)
				{
					// reflectance
					weak_Factor_R[i] = 0;
					s_r[i] = sqrt(above_Node->hi[i]*hi[i]);

					// transmittance
					weak_Factor_T[i] = 0;
					s_t[i] = (above_Node->hi[i]-hi[i])/2.;
				}

				//-------------------------------------------------------------------------------
				// erf interlayer
				if(struct_Data.interlayer_Composition[Erf].enabled)
				if(struct_Data.interlayer_Composition[Erf].interlayer.value > DBL_MIN)
				{
					norm += struct_Data.interlayer_Composition[Erf].interlayer.value;
					if(!struct_Data.common_Sigma_Diffuse) {
						my_Sigma = struct_Data.interlayer_Composition[Erf].my_Sigma_Diffuse.value;}

					complex<double> factor_r, factor_t;
					for(size_t i=0; i<num_Points; ++i)
					{
						// reflectance
						factor_r = exp( - s_r[i] * s_r[i] * my_Sigma * my_Sigma * 2. );
						weak_Factor_R[i] += struct_Data.interlayer_Composition[Erf].interlayer.value * factor_r;

						// transmittance
						factor_t = exp(   s_t[i] * s_t[i] * my_Sigma * my_Sigma * 2. );
						weak_Factor_T[i] += struct_Data.interlayer_Composition[Erf].interlayer.value * factor_t;
					}
				}
				//-------------------------------------------------------------------------------
				// lin interlayer
				if(struct_Data.interlayer_Composition[Lin].enabled)
				if(struct_Data.interlayer_Composition[Lin].interlayer.value > DBL_MIN)
				{
					norm += struct_Data.interlayer_Composition[Lin].interlayer.value;
					if(!struct_Data.common_Sigma_Diffuse) {
						my_Sigma = struct_Data.interlayer_Composition[Lin].my_Sigma_Diffuse.value;}

					complex<double> factor_r, x_r, factor_t, x_t;
					for(size_t i=0; i<num_Points; ++i)
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
					if(!struct_Data.common_Sigma_Diffuse) {
						my_Sigma = struct_Data.interlayer_Composition[Exp].my_Sigma_Diffuse.value;}

					complex<double> factor_r, x_r, factor_t, x_t;
					for(size_t i=0; i<num_Points; ++i)
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
				}
				//-------------------------------------------------------------------------------
				// tanh interlayer
				if(struct_Data.interlayer_Composition[Tanh].enabled)
				if(struct_Data.interlayer_Composition[Tanh].interlayer.value > DBL_MIN)
				{
					norm += struct_Data.interlayer_Composition[Tanh].interlayer.value;
					if(!struct_Data.common_Sigma_Diffuse) {
						my_Sigma = struct_Data.interlayer_Composition[Tanh].my_Sigma_Diffuse.value;}

					complex<double> factor_r, x_r, factor_t, x_t;
					for(size_t i=0; i<num_Points; ++i)
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
				}
				//-------------------------------------------------------------------------------
				// sin interlayer
				if(struct_Data.interlayer_Composition[Sin].enabled)
				if(struct_Data.interlayer_Composition[Sin].interlayer.value > DBL_MIN)
				{
					norm += struct_Data.interlayer_Composition[Sin].interlayer.value;
					if(!struct_Data.common_Sigma_Diffuse) {
						my_Sigma = struct_Data.interlayer_Composition[Sin].my_Sigma_Diffuse.value;}

					complex<double> factor_r, x_r, y_r, six_r, siy_r, factor_t, x_t, y_t, six_t, siy_t;
					for(size_t i=0; i<num_Points; ++i)
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
				}
				//-------------------------------------------------------------------------------
				// step interlayer
				if(struct_Data.interlayer_Composition[Step].enabled)
				if(struct_Data.interlayer_Composition[Step].interlayer.value > DBL_MIN)
				{
					norm += struct_Data.interlayer_Composition[Step].interlayer.value;
					if(!struct_Data.common_Sigma_Diffuse) {
						my_Sigma = struct_Data.interlayer_Composition[Step].my_Sigma_Diffuse.value;}

					complex<double> factor_r;
					complex<double> factor_t;
					for(size_t i=0; i<num_Points; ++i)
					{
						// reflectance
						factor_r = cos(s_r[i] * my_Sigma * 2.);
						weak_Factor_R[i] += struct_Data.interlayer_Composition[Step].interlayer.value * factor_r;

						// transmittance
						factor_t = cos(s_t[i] * my_Sigma * 2.);
						weak_Factor_T[i] += struct_Data.interlayer_Composition[Step].interlayer.value * factor_t;
					}
				}
				//-------------------------------------------------------------------------------
				// normalization
				{
					if( abs(norm) > DBL_MIN )
					{
						for(size_t i=0; i<num_Points; ++i)
						{
							weak_Factor_R[i] /= norm;
							weak_Factor_T[i] /= norm;
						}
					} else
					{
						for(size_t i=0; i<num_Points; ++i)
						{
							weak_Factor_R[i] = 1;
							weak_Factor_T[i] = 1;
						}
					}
				}
			} else
			{
				for(size_t i=0; i<num_Points; ++i)
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
		if( struct_Data.item_Type == item_Type_Layer )
		{
			exponenta.  resize(num_Points);
			exponenta_2.resize(num_Points);
			if( depth_Grading )
			{
				for(size_t i=0; i<num_Points; ++i)
				{
					exponenta[i]   = 1.;
					exponenta_2[i] = 1.;
				}
			} else
			{
				for(size_t i=0; i<num_Points; ++i)
				{
					exponenta[i] = exp(I*hi[i]*struct_Data.thickness.value);
					exponenta_2[i] = pow(exponenta[i],2);
				}
			}
		}

	}

	// with and without discretization
	if( struct_Data.item_Type == item_Type_Layer ||
		struct_Data.item_Type == item_Type_Substrate )
	{
		double sigma = struct_Data.roughness_Model.sigma.value;
		double xi = struct_Data.roughness_Model.cor_radius.value;
		double alpha = struct_Data.roughness_Model.fractal_alpha.value;

		struct_Data.PSD_ABC_1D_Factor = 4*sqrt(M_PI) * tgamma(alpha+0.5)/tgamma(alpha) * sigma*sigma*xi;
		struct_Data.PSD_ABC_2D_Factor = 4*M_PI * sigma*sigma * xi*xi * alpha;
		struct_Data.PSD_Real_Gauss_1D_Factor = 2*sqrt(M_PI) * sigma*sigma*xi;
		struct_Data.PSD_Real_Gauss_2D_Factor = M_PI * sigma*sigma*xi*xi;
	}
}

void Node::fill_Epsilon_For_Angular_Measurements(vector<double>& spectral_Points, bool specular_Case)
{
	complex<double> delta_Epsilon_Ang, epsilon_Ang;

	/// ---------------------------------------------------------------------------------------------------------------
	/// delta_Epsilon
	/// ---------------------------------------------------------------------------------------------------------------

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
		double density = struct_Data.absolute_Density.value;
		if(specular_Case) density = struct_Data.average_Layer_density();
		vector<complex<double>> temp_Epsilon;
		optical_Constants->make_Epsilon_From_Factors(struct_Data.composition, density, spectral_Points, temp_Epsilon);
		delta_Epsilon_Ang = 1. - temp_Epsilon.front();
	}
//	if(abs(imag(delta_Epsilon_Ang))<DBL_EPSILON) delta_Epsilon_Ang -= complex<double>(0,DBL_EPSILON);

	/// ---------------------------------------------------------------------------------------------------------------
	/// epsilon
	/// ---------------------------------------------------------------------------------------------------------------

	// if density
	if(struct_Data.separate_Optical_Constants != TRIL_TRUE)
	{
		// if absolute density
		if(struct_Data.composed_Material)		epsilon_Ang = 1. - delta_Epsilon_Ang;
		// if relative density
		else
		{
			double density = struct_Data.relative_Density.value;
			if(specular_Case) density = struct_Data.average_Layer_density();
			epsilon_Ang = 1. - density * delta_Epsilon_Ang;
		}

	} else
	// if separate optical constants
	{
		delta_Epsilon_Ang = complex<double>(real(delta_Epsilon_Ang) * struct_Data.permittivity.value / 100.,
											imag(delta_Epsilon_Ang) * struct_Data.absorption  .value / 100.);
		epsilon_Ang = 1. - delta_Epsilon_Ang;
	}

	// fill epsilon with equal epsilons
	for(size_t point_Index = 0; point_Index<epsilon.size(); ++point_Index)
	{
		epsilon[point_Index] = epsilon_Ang;
	}
}

void Node::fill_Epsilon_Contrast_For_Density_Fluctuations(vector<double>& spectral_Points)
{
	if(struct_Data.fluctuations_Model.is_Used)
	{
		complex<double> delta_Epsilon_Ang, epsilon_Ang;

		/// ---------------------------------------------------------------------------------------------------------------
		/// delta_Epsilon
		/// ---------------------------------------------------------------------------------------------------------------

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
			optical_Constants->make_Epsilon_From_Factors(struct_Data.composition, struct_Data.fluctuations_Model.particle_Absolute_Density.value, spectral_Points, temp_Epsilon);
			delta_Epsilon_Ang = 1. - temp_Epsilon.front();
		}

		/// ---------------------------------------------------------------------------------------------------------------
		/// epsilon
		/// ---------------------------------------------------------------------------------------------------------------

		// if absolute density
		if(struct_Data.composed_Material)	epsilon_Ang = 1. - delta_Epsilon_Ang;
		// if relative density
		else								epsilon_Ang = 1. - struct_Data.fluctuations_Model.particle_Relative_Density.value * delta_Epsilon_Ang;


		delta_Epsilon_Contrast = epsilon.front() - epsilon_Ang;
	}
}

void Node::calc_Debye_Waller_Sigma(const Data& measurement, const Imperfections_Model& imperfections_Model)
{
	// angular width of detector
	double max_Delta_Theta_Detector = 0;
	if(measurement.detector_1D.detector_Type == detectors[Slit])
	{
		double w_2 = (measurement.detector_1D.slit_Width/2);
		double d = (measurement.detector_1D.distance_To_Sample);
		max_Delta_Theta_Detector = atan(w_2/d) * 180./M_PI;  // in degrees
	}
	if(measurement.detector_1D.detector_Type == detectors[Crystal])
	{
		max_Delta_Theta_Detector = measurement.detector_1D.detector_Theta_Resolution.FWHM_distribution/2 * 180./M_PI;
	}

	// measurement points
	size_t num_Points = 0;
	vector<double> cos_Theta_0;
	vector<double> angle_Theta_0;
	vector<double> k;

	if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
	{
		num_Points = measurement.beam_Theta_0_Cos2_Vec.size();
		cos_Theta_0 = measurement.beam_Theta_0_Cos_Vec;
		angle_Theta_0 = measurement.beam_Theta_0_Angle_Vec;
		k.resize(num_Points);
		for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
		{
			k[point_Index] = measurement.k_Value;
		}
	}
	if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
	{
		num_Points = measurement.lambda_Vec.size();
		k = measurement.k_Vec;
		cos_Theta_0.resize(num_Points);
		angle_Theta_0.resize(num_Points);
		for(size_t point_Index = 0; point_Index<num_Points; ++point_Index)
		{
			cos_Theta_0[point_Index] = measurement.beam_Theta_0_Cos2_Value;
			angle_Theta_0[point_Index] = measurement.beam_Theta_0_Angle_Value;
		}
	}

	double sigma = struct_Data.roughness_Model.sigma.value;
	double xi =    struct_Data.roughness_Model.cor_radius.value;
	double alpha = struct_Data.roughness_Model.fractal_alpha.value;

	specular_Debye_Waller_Weak_Factor_R.resize(num_Points,1);
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;
	if(sigma<DBL_EPSILON) return;

	// max frequency to detector
	vector<double> p0(num_Points);
	for(size_t i = 0; i<num_Points; ++i)
	{
		p0[i] = k[i]*abs(cos_Theta_0[i] - cos((angle_Theta_0[i] + max_Delta_Theta_Detector) * M_PI/180.));
	}

	// integration
	vector<double> delta_Sigma_2(num_Points);
	if(imperfections_Model.common_Model == ABC_model)
	{
		auto f_2 = [&](double p){return 2./sqrt(M_PI) * tgamma(alpha+0.5)/tgamma(alpha) * sigma*sigma*xi / pow(1+p*p*xi*xi, alpha+0.5);};
		for(size_t i = 0; i<num_Points; ++i)
		{
			double z = -p0[i]*p0[i]*xi*xi;
			if(abs(z)<1)
			{
				delta_Sigma_2[i] = 2*p0[i]*xi*sigma*sigma*tgamma(alpha+0.5) * boost::math::hypergeometric_pFq({0.5, 0.5+alpha}, {1.5}, z) / (sqrt(M_PI) * tgamma(alpha));
			} else
			{
				if(p0[i]>DBL_MIN)
				{
					if(p0[i]*xi<1e2) delta_Sigma_2[i] = min(gauss_kronrod<double, 15>::integrate(f_2, 0, p0[i], 0, 1e-7), sigma*sigma);
					else			 delta_Sigma_2[i] = sigma*sigma;
				} else
				{
					delta_Sigma_2[i] = 0;
				}
			}
		}
	}
	if(imperfections_Model.common_Model == fractal_Gauss_Model)
	{
		vector<double> sorted_p0 = p0;
		std::sort(sorted_p0.begin(), sorted_p0.end());
		double addition = 1E-10;
		double p_Max = sorted_p0.back() + addition;

		int num_Sections = 4; // plus zero point
		vector<int> interpoints(num_Sections);
		int common_Size = 0;
		for(int i=0; i<num_Sections; i++)
		{
			interpoints[i] = 10-2*i;
			common_Size+=interpoints[i];
		}
		vector<double> interpoints_Sum_Argum_Vec(1+common_Size);
		vector<double> interpoints_Sum_Value_Vec(1+common_Size);

		vector<double> starts(num_Sections); // open start
		starts[0] = 0;
		starts[1] = p_Max/50;
		starts[2] = p_Max/10;
		starts[3] = p_Max/2;

		vector<double> dp(num_Sections);
		for(int i=0; i<num_Sections-1; i++)
		{
			dp[i] = (starts[i+1] - starts[i])/interpoints[i];
		}
		dp.back() = (p_Max - starts.back())/interpoints.back();

		// zero point
		{
			interpoints_Sum_Argum_Vec[0] = 0;
			interpoints_Sum_Value_Vec[0] = M_2_PI*sigma*sigma*xi*tgamma(1.+1/(2*alpha));
		}

		double p = 0, result = 0;
		int counter = 1;
		// boost integrator
		auto f = [&](double r) {return sigma*sigma * exp(-pow(r/xi,2*alpha));};

		ooura_fourier_cos<double> integrator;
		for(int sec=0; sec<num_Sections; sec++)
		{
			for(int i=0; i<interpoints[sec]; i++)
			{
				p += dp[sec];

				// second part
				std::pair<double, double> result_Boost = integrator.integrate(f, p);
				result = result_Boost.first;

				interpoints_Sum_Argum_Vec[counter] = p;
				interpoints_Sum_Value_Vec[counter] = M_2_PI*result;
				counter++;
			}
		}

		acc_PSD = gsl_interp_accel_alloc();
		if(p_Max<10*addition) 	spline_PSD = gsl_spline_alloc(gsl_interp_linear, interpoints_Sum_Value_Vec.size());
		else					spline_PSD = gsl_spline_alloc(gsl_interp_steffen,interpoints_Sum_Value_Vec.size());
		gsl_spline_init(spline_PSD, interpoints_Sum_Argum_Vec.data(), interpoints_Sum_Value_Vec.data(), interpoints_Sum_Value_Vec.size());

		// integration
		auto f_2 = [&](double p){return gsl_spline_eval(spline_PSD, p, acc_PSD);};
		for(size_t i = 0; i<num_Points; ++i)
		{
			if(p0[i]>DBL_MIN)
			{
				if(p0[i]*xi<1e2) delta_Sigma_2[i] = min(gauss_kronrod<double, 15>::integrate(f_2, 0, p0[i], 0, 1e-7), sigma*sigma);
				else			 delta_Sigma_2[i] = sigma*sigma;
			} else
			{
				delta_Sigma_2[i] = 0;
			}
		}
		gsl_spline_free(spline_PSD);
		gsl_interp_accel_free(acc_PSD);
	}

	for(size_t i = 0; i<num_Points; ++i)
	{
		double s2 = sigma*sigma - delta_Sigma_2[i];
		double hi = k[i]*sin(angle_Theta_0[i] * M_PI/180.);
		specular_Debye_Waller_Weak_Factor_R[i] = exp( - 4. * hi*hi * s2 );
	}
}

void Node::create_Spline_PSD_Fractal_Gauss_1D(const Data& measurement, const Imperfections_Model& imperfections_Model)
{
	if(imperfections_Model.approximation != PT_approximation) return;
	if(imperfections_Model.common_Model != fractal_Gauss_Model) return;
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;

	// in other cases ( substrate or layer-with-individual-function ) go further

	double sigma = struct_Data.roughness_Model.sigma.value;
	double xi =    struct_Data.roughness_Model.cor_radius.value;
	double alpha = struct_Data.roughness_Model.fractal_alpha.value;

	vector<double> temp_Cos = measurement.detector_Theta_Cos_Vec;

	if( measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
		for(size_t i=0; i<temp_Cos.size(); i++) temp_Cos[i] = measurement.k_Value*abs(temp_Cos[i] - measurement.beam_Theta_0_Cos_Value);
	}
	if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		for(size_t i=0; i<temp_Cos.size(); i++) temp_Cos[i] = measurement.k_Value*abs(temp_Cos[i] - measurement.beam_Theta_0_Cos_Vec[i]);
	}
	std::sort(temp_Cos.begin(), temp_Cos.end());
	double addition = 1E-8;
	double p_Max = temp_Cos.back()+addition;


	int num_Sections = 6; // plus zero point
	vector<int> interpoints(num_Sections);
	int common_Size = 0;
	for(int i=0; i<num_Sections; i++)
	{
		interpoints[i] = 20-2*i;
		common_Size+=interpoints[i];
	}
	vector<double> interpoints_Sum_Argum_Vec(1+common_Size);
	vector<double> interpoints_Sum_Value_Vec(1+common_Size);

	vector<double> starts(num_Sections); // open start
	starts[0] = 0;
	starts[1] = p_Max/300;
	starts[2] = p_Max/40;
	starts[3] = p_Max/10;
	starts[4] = p_Max/5;
	starts[5] = p_Max/2;

	vector<double> dp(num_Sections);
	for(int i=0; i<num_Sections-1; i++)
	{
		dp[i] = (starts[i+1] - starts[i])/interpoints[i];
	}
	dp.back() = (p_Max - starts.back())/interpoints.back();

	// zero point
	{
		interpoints_Sum_Argum_Vec[0] = 0;
		interpoints_Sum_Value_Vec[0] = 4*sigma*sigma*xi*tgamma(1.+1/(2*alpha));
	}

	double p = 0, result = 0;
	int counter = 1;
	// boost integrator
	auto f = [&](double r) {return sigma*sigma * exp(-pow(r/xi,2*alpha));};

	ooura_fourier_cos<double> integrator;
	for(int sec=0; sec<num_Sections; sec++)
	{
		for(int i=0; i<interpoints[sec]; i++)
		{
			p += dp[sec];

			// second part
			std::pair<double, double> result_Boost = integrator.integrate(f, p);
			result = result_Boost.first;

			interpoints_Sum_Argum_Vec[counter] = p;
			interpoints_Sum_Value_Vec[counter] = 4*result;
			counter++;
		}
	}

	acc_PSD = gsl_interp_accel_alloc();
	if(p_Max<10*addition) 	spline_PSD = gsl_spline_alloc(gsl_interp_linear, interpoints_Sum_Value_Vec.size());
	else					spline_PSD = gsl_spline_alloc(gsl_interp_steffen,interpoints_Sum_Value_Vec.size());

	gsl_spline_init(spline_PSD, interpoints_Sum_Argum_Vec.data(), interpoints_Sum_Value_Vec.data(), interpoints_Sum_Value_Vec.size());
}

void Node::create_Spline_PSD_Fractal_Gauss_2D(const Data& measurement, const Imperfections_Model& imperfections_Model)
{
	Q_UNUSED(measurement)
	Q_UNUSED(imperfections_Model)
	// TODO integration of 2D fractal gauss
//	if(imperfections_Model.approximation != PT_approximation) return;
//	if(imperfections_Model.common_Model != fractal_Gauss_Model) return;
//	if(struct_Data.item_Type == item_Type_Ambient ) return;
//	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;

//	// in other cases ( substrate or layer-with-individual-function ) go further

//	double sigma = struct_Data.roughness_Model.sigma.value;
//	double xi =    struct_Data.roughness_Model.cor_radius.value;
//	double alpha = struct_Data.roughness_Model.fractal_alpha.value;

//	vector<double> temp_Nu2(measurement.detector_Theta_Cos_Vec.size());

//	double min_Cos_Phi = min(measurement.detector_Phi_Cos_Vec.front(), measurement.detector_Phi_Cos_Vec.back());
//	for(size_t i=0; i<temp_Nu2.size(); i++)
//	{
//		temp_Nu2[i] = measurement.k_Value*measurement.k_Value*( measurement.detector_Theta_Cos_Vec[i]*measurement.detector_Theta_Cos_Vec[i] +
//																measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value -
//															  2*measurement.beam_Theta_0_Cos_Value*measurement.detector_Theta_Cos_Vec[i]*min_Cos_Phi);
//	}
//	std::sort(temp_Nu2.begin(), temp_Nu2.end());
//	double nu_Max = sqrt(temp_Nu2.back())*(1+1E-8);

//	int num_Sections = 6; // plus zero point
//	vector<int> interpoints(num_Sections);
//	int common_Size = 0;
//	for(int i=0; i<num_Sections; i++)
//	{
//		interpoints[i] = 20-2*i;
//		common_Size+=interpoints[i];
//	}
//	vector<double> interpoints_Sum_Argum_Vec(1+common_Size);
//	vector<double> interpoints_Sum_Value_Vec(1+common_Size);

//	vector<double> starts(num_Sections); // open start
//	starts[0] = 0;
//	starts[1] = nu_Max/300;
//	starts[2] = nu_Max/40;
//	starts[3] = nu_Max/10;
//	starts[4] = nu_Max/5;
//	starts[5] = nu_Max/2;

//	vector<double> dnu(num_Sections);
//	for(int i=0; i<num_Sections-1; i++)
//	{
//		dnu[i] = (starts[i+1] - starts[i])/interpoints[i];
//	}
//	dnu.back() = (nu_Max - starts.back())/interpoints.back();

//	// zero point
//	{
//		interpoints_Sum_Argum_Vec[0] = 0;
//		interpoints_Sum_Value_Vec[0] = M_PI*sigma*sigma*xi*xi*tgamma(1.+1/alpha);
//	}

//	ooura_fourier_cos<double> integrator;

//	double nu = 0, error;
//	int counter = 1;
//	for(int sec=0; sec<num_Sections; sec++)
//	{
//		for(int i=0; i<interpoints[sec]; i++)
//		{
//			nu += dnu[sec];

//			double integral = 0;
//			double start_Point = 2*M_PI*(10+0.125)/nu;
//			// first part
//			auto f_1 = [&](double r) {return 2*M_PI * sigma*sigma*exp(-pow(r/xi,2*alpha)) * cyl_bessel_j(0, nu*r) * r;};
//			integral = gauss_kronrod<double, 31>::integrate(f_1, 0, start_Point, 2, 1e-7, &error);
//			// second part
//			double factor = 2*sqrt(2*M_PI/nu);
//			auto f_2 = [&](double r) {return factor * sigma*sigma * exp(-pow((r+start_Point)/xi,2*alpha)) * sqrt(r+start_Point);};
//			std::pair<double, double> result_Boost = integrator.integrate(f_2, nu);
//			integral += result_Boost.first;

//			interpoints_Sum_Argum_Vec[counter] = nu;
//			interpoints_Sum_Value_Vec[counter] = integral;
//			counter++;
//		}
//	}
//	// chech for artifacts
////	for(int i=interpoints_Sum_Argum_Vec.size()-2; i>=0; i--)
////	{
////		if(interpoints_Sum_Value_Vec[i]<0.5*interpoints_Sum_Value_Vec[i+1])
////		{
////			interpoints_Sum_Value_Vec.erase (interpoints_Sum_Value_Vec.begin()+i);
////			interpoints_Sum_Argum_Vec.erase (interpoints_Sum_Argum_Vec.begin()+i);
////		}
////	}
//	for(int i=0; i<interpoints_Sum_Argum_Vec.size(); i+=1)
//	{
//		qInfo() << interpoints_Sum_Argum_Vec[i] << interpoints_Sum_Value_Vec[i] << endl;
//	}

//	const gsl_interp_type* interp_type = gsl_interp_steffen;
//	acc = gsl_interp_accel_alloc();
//	spline = gsl_spline_alloc(interp_type, interpoints_Sum_Value_Vec.size());
//	gsl_spline_init(spline, interpoints_Sum_Argum_Vec.data(), interpoints_Sum_Value_Vec.data(), interpoints_Sum_Value_Vec.size());
}
void Node::clear_Spline_PSD_Fractal_Gauss(const Imperfections_Model& imperfections_Model)
{
	if(imperfections_Model.approximation != PT_approximation) return;
	if(imperfections_Model.common_Model != fractal_Gauss_Model) return;
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;

	gsl_spline_free(spline_PSD);
	gsl_interp_accel_free(acc_PSD);
}

double Node::G1_Type_Outer()
{
	if(struct_Data.fluctuations_Model.geometric_Model == square_Model)
	{
		return Global_Variables::G1_Square(struct_Data.fluctuations_Model.particle_Radial_Distance.value);
	}
	if(struct_Data.fluctuations_Model.geometric_Model == hexagonal_Model)
	{
		return Global_Variables::G1_Hexagone(struct_Data.fluctuations_Model.particle_Radial_Distance.value);
	}
	qInfo() << endl << "Node::G1_Type_Outer  :  wrong fluctuations_Model.geometric_Model" << endl << endl;
	return -2020;
}

double Node::G2_Type_Outer(double k, double cos_Theta, double cos_Theta_0, double cos_Phi)
{
	double q = k*sqrt(cos_Theta*cos_Theta + cos_Theta_0*cos_Theta_0 - 2*cos_Theta_0*cos_Theta*cos_Phi);
	return gsl_spline_eval(spline_G2, q, acc_G2);
}

void Node::create_Spline_G2_2D(const Data& measurement, const Imperfections_Model& imperfections_Model)
{
	if(!imperfections_Model.use_Fluctuations) return;
	if(!struct_Data.fluctuations_Model.is_Used) return;
	if(struct_Data.fluctuations_Model.particle_Interference_Function != radial_Paracrystal) return;
	if(struct_Data.item_Type != item_Type_Layer ) return;

	// in other cases ( layer with radial paracrystal ) go further

	vector<double> temp_q2_Max(measurement.detector_Theta_Cos_Vec.size());
	vector<double> temp_q2_Min(measurement.detector_Theta_Cos_Vec.size());

	double min_Cos_Phi_0 = min(measurement.detector_Phi_Cos_Vec.front(), measurement.detector_Phi_Cos_Vec.back());
	double max_Cos_Phi_0 = max(measurement.detector_Phi_Cos_Vec.front(), measurement.detector_Phi_Cos_Vec.back());
	double min_Cos_Phi = min(min_Cos_Phi_0,max_Cos_Phi_0);
	double max_Cos_Phi = max(min_Cos_Phi_0,max_Cos_Phi_0);
	if(measurement.detector_Phi_Angle_Vec.front()*measurement.detector_Phi_Angle_Vec.back() < DBL_EPSILON ) max_Cos_Phi = 1;

	vector<double> detector_Theta_Cos_Sorted_Vec = measurement.detector_Theta_Cos_Vec;
	std::sort(detector_Theta_Cos_Sorted_Vec.begin(), detector_Theta_Cos_Sorted_Vec.end());
	for(size_t i=0; i<detector_Theta_Cos_Sorted_Vec.size(); i++)
	{
		temp_q2_Max[i] = measurement.k_Value*measurement.k_Value*( detector_Theta_Cos_Sorted_Vec[i]*detector_Theta_Cos_Sorted_Vec[i] +
																   measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value -
																 2*measurement.beam_Theta_0_Cos_Value*detector_Theta_Cos_Sorted_Vec[i]*min_Cos_Phi);
		temp_q2_Min[i] = measurement.k_Value*measurement.k_Value*( detector_Theta_Cos_Sorted_Vec[i]*detector_Theta_Cos_Sorted_Vec[i] +
																   measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value -
																 2*measurement.beam_Theta_0_Cos_Value*detector_Theta_Cos_Sorted_Vec[i]*max_Cos_Phi);
	}
	std::sort(temp_q2_Max.begin(), temp_q2_Max.end());
	std::sort(temp_q2_Min.begin(), temp_q2_Min.end());
	double q_Max = sqrt(max(temp_q2_Max.back(),DBL_EPSILON))*(1+1E-8);
	double q_Min = sqrt(max(temp_q2_Min.front(),0. ))*(1-1E-8);
	double q_Range = q_Max-q_Min;
	vector<double> q_Peak;

	// choose pattern
	double phi_Max;
	double a = struct_Data.fluctuations_Model.particle_Radial_Distance.value;
	double b = a;
	double sigma = struct_Data.fluctuations_Model.particle_Radial_Distance_Deviation.value;
	G2_Type.resize(reflectivity_Calc_Threads);
	G2_Type_long.resize(reflectivity_Calc_Threads);
	if(struct_Data.fluctuations_Model.geometric_Model == square_Model)
	{
		G1_Type = Global_Variables::G1_Square;
		for(int thread_Index = 0; thread_Index<reflectivity_Calc_Threads; thread_Index++)
		{
			G2_Type     [thread_Index] = Global_Variables::G2_Square;
			G2_Type_long[thread_Index] = Global_Variables::G2_Square_long;
		}
		phi_Max = M_PI_4;
		b = a;

		// peak positions
		int n_Max = ceil(q_Max*a/(2*M_PI));
		for(int n = 0; n<=n_Max; n++)
		{
			for(int m = n; m<=n_Max; m++)
			{
				double q = 2*M_PI/a*sqrt(n*n+m*m);
				if(	(q < q_Max) &&
					(q > q_Min)
				  )
				{
					q_Peak.push_back(q);
				}
			}
		}
		std::sort(q_Peak.begin(), q_Peak.end());
	}
	if(struct_Data.fluctuations_Model.geometric_Model == hexagonal_Model)
	{
		G1_Type = Global_Variables::G1_Hexagone;
		for(int thread_Index = 0; thread_Index<reflectivity_Calc_Threads; thread_Index++)
		{
			G2_Type     [thread_Index] = Global_Variables::G2_Hexagone;
			G2_Type_long[thread_Index] = Global_Variables::G2_Hexagone_long;
		}
		phi_Max = M_PI/6;
		b = a*M_SQRT3/2;

		// peak positions
		int n_Max = ceil(q_Max*a/(2*M_PI));
		int m_Max = ceil(q_Max*b/(  M_PI));
		for(int n = 1; n<=n_Max; n++)
		{
			for(int m = 1; m<=m_Max; m++)
			{
				double q = 2*M_PI/a*sqrt(n*n+m*m/3.);
				if(	(q < q_Max) &&
					(q > q_Min) &&
					((n+m)%2 == 0)
				  )
				{
					bool contains = false;
					for(double& qp : q_Peak)	{
						if( norm(qp-q) < 1E-6 ) contains = true;
					}
					if(!contains) q_Peak.push_back(q);
				}
			}
		}
		std::sort(q_Peak.begin(), q_Peak.end());
	}

	double N = 1e7*min(measurement.sample_Geometry.size, measurement.beam_Geometry.size/max(measurement.beam_Theta_0_Sin_Value, DBL_EPSILON))/a;
	double M = 1e7*measurement.beam_Geometry.lateral_Width/b;
	N = max(N,1.);
	M = max(M,1.);

	// peak half-widths
	vector<double> hw_Peak(q_Peak.size());
	for(int i=0; i<q_Peak.size(); i++)
	{
		double damp = exp(-0.5*q_Peak[i]*q_Peak[i]*sigma*sigma);
		hw_Peak[i] = acos( 2*damp / (1.+damp*damp) ) / a;
	}
	double damp_Max = exp(-0.5*q_Max*q_Max/4*sigma*sigma);
	double hw_Peak_Max = acos( 2*damp_Max / (1.+damp_Max*damp_Max) ) / a;
	if(hw_Peak.size()>=1) hw_Peak_Max = hw_Peak.front();

	// points
	vector<double> q_Vec; q_Vec.reserve(100000);
	q_Vec.push_back(q_Min);

	// num points if no peaks in range
	int num_Bare_Points = 300;
	int num_Peak_Points = 51; // points inside FWHM * hw_Factor/2
	int hw_Factor = 5;

	// no peaks in range
	if(q_Peak.size() == 0)
	{
		q_Vec.resize(num_Bare_Points);
		double dq = q_Range/(num_Bare_Points-1);
		for(int i=0; i<num_Bare_Points; i++)
		{
			q_Vec[i] = q_Min+dq*i;
		}
	} else
	{
		double dq_Bare = q_Range/(num_Bare_Points-1);

		// except last peak
		for(size_t peak_Index = 0; peak_Index<q_Peak.size()-1; peak_Index++)
		{
			// before each peak
			while(q_Vec.back()+dq_Bare < q_Peak[peak_Index]-hw_Factor*hw_Peak[peak_Index])
			{
				q_Vec.push_back(q_Vec.back()+dq_Bare);
			}

			// inside each peak
			q_Vec.push_back(q_Peak[peak_Index]-hw_Factor*hw_Peak[peak_Index]); // may be < 0

			double dq_Peak = 2*hw_Factor*hw_Peak[peak_Index]/(num_Peak_Points-1);

			while( (q_Vec.back()+dq_Peak < q_Peak[peak_Index  ]+hw_Factor*hw_Peak[peak_Index  ]) &&
				   (q_Vec.back()+dq_Peak < q_Peak[peak_Index+1]-hw_Factor*hw_Peak[peak_Index+1]) // before next peak starts
				   )
			{
				q_Vec.push_back(q_Vec.back()+dq_Peak);
			}
		}

		// last peak
		size_t peak_Index = q_Peak.size()-1;
		{
			// before last peak
			while(q_Vec.back()+dq_Bare < q_Peak[peak_Index]-hw_Factor*hw_Peak[peak_Index])
			{
				q_Vec.push_back(q_Vec.back()+dq_Bare);
			}

			// inside last peak
			q_Vec.push_back(q_Peak[peak_Index]-hw_Factor*hw_Peak[peak_Index]);

			double dq_Peak = 2*hw_Factor*hw_Peak[peak_Index]/(num_Peak_Points-1);

			while( (q_Vec.back()+dq_Peak < q_Peak[peak_Index]+hw_Factor*hw_Peak[peak_Index]) &&
				   (q_Vec.back()+dq_Peak < q_Max)
				   )
			{
				q_Vec.push_back(q_Vec.back()+dq_Peak);
			}

			// after last peak
			while(q_Vec.back()+dq_Bare < q_Max)
			{
				q_Vec.push_back(q_Vec.back()+dq_Bare);
			}
			q_Vec.push_back(q_Max);
		}
	}

	// sorting
	std::sort(q_Vec.begin(), q_Vec.end());
	for(int i=q_Vec.size()-1; i>=1; i--)
	{
		if(q_Vec[i]-q_Vec[i-1] < DBL_EPSILON) q_Vec.erase(q_Vec.begin()+i);
	}
	for(int i=q_Vec.size()-1; i>=0; i--)
	{
		if(q_Vec[i] < q_Min) q_Vec.erase(q_Vec.begin()+i);
	}

	// calculation
	int num_Phi_Points_Per_hw = 1;
	int max_Phi_Division = 20;
	vector<double> G2_Vec(q_Vec.size());
	vector<bool> too_Narrow(reflectivity_Calc_Threads);

	Global_Variables::parallel_For(int(q_Vec.size()), reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		too_Narrow[thread_Index] = false;
		vector<double> phi_Vec; phi_Vec.reserve(max_Phi_Division+1);
		for(size_t q_Index = n_Min; q_Index<n_Max; q_Index++)
		{
			double q = q_Vec[q_Index];

			// phi division
			double damp = exp(-0.5*q*q*sigma*sigma);
			double hw_q = max(acos( 2*damp / (1.+damp*damp) ) / a, 4*hw_Peak_Max);
			double arc_q = phi_Max*q;

			const int gk_points = 61;
			int phi_Division = ceil( arc_q/gk_points * num_Phi_Points_Per_hw/hw_q);
			phi_Division = max(phi_Division, 1);
			phi_Division = min(phi_Division, max_Phi_Division); // reasonable limit
			if(phi_Division == max_Phi_Division) too_Narrow[thread_Index] = true;
			phi_Vec.resize(phi_Division+1);
			for(int i=0; i<=phi_Division; i++)
			{
				phi_Vec[i] = phi_Max/phi_Division*i;
			}

			// integration
			auto func = [&](double phi)	{return G2_Type_long[thread_Index](q, phi, a, sigma, N, M)/phi_Max;};
			double integral = 0;

			// threshold
			for(int phi_Index=0; phi_Index<phi_Division; phi_Index++)
			{
				integral += gauss_kronrod<double,gk_points>::integrate(func, phi_Vec[phi_Index], phi_Vec[phi_Index+1], 0, 1e-7);
			}
			G2_Vec[q_Index] = integral;
		}
	});
	// thresholds
	for(int q_Index = q_Vec.size()-1; q_Index>=0; q_Index--)
	{
		// TODO
		if(q_Vec[q_Index]<2e-6)
		{
//			if(q_Vec[q_Index]<DBL_EPSILON)
//			{
//				G2_Vec[q_Index] = -G1_Type(a); // TODO why not working?
//			} else
			{
				if(q_Index<q_Vec.size()-1)
				{
					G2_Vec[q_Index] = G2_Vec[q_Index+1];
				}
			}
		}
	}

	bool narrow = false;
	for(size_t i=0; i<too_Narrow.size(); i++)
	{
		if(too_Narrow[i]) narrow = true;
	}
	if(narrow)
	{
		qInfo() << endl << "WARNING: in " << Global_Variables::structure_Item_Name(struct_Data) << " in density fluctuations r/dr is too high" << endl << endl;
	}

//	QFile file("G2.txt");
//	file.open(QIODevice::WriteOnly);
//	QTextStream out(&file);
//	out.setFieldAlignment(QTextStream::AlignLeft);
//	for(size_t i=0; i<q_Vec.size(); ++i)
//	{
//		out << q_Vec[i] << "\t" << G2_Vec[i] << endl;
//	}
//	file.close();

	const gsl_interp_type* interp_type = gsl_interp_steffen;
	acc_G2 = gsl_interp_accel_alloc();
	spline_G2 = gsl_spline_alloc(interp_type, q_Vec.size());
	gsl_spline_init(spline_G2, q_Vec.data(), G2_Vec.data(), q_Vec.size());
}

void Node::clear_Spline_G2_2D(const Imperfections_Model& imperfections_Model)
{
	if(!imperfections_Model.use_Fluctuations) return;
	if(!struct_Data.fluctuations_Model.is_Used) return;
	if(struct_Data.fluctuations_Model.particle_Interference_Function != radial_Paracrystal) return;
	if(struct_Data.item_Type != item_Type_Layer ) return;

	gsl_spline_free(spline_G2);
	gsl_interp_accel_free(acc_G2);
}
