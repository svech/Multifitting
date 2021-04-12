#include "node.h"
#include <gsl/gsl_sf_gamma.h>
#include <boost/math/quadrature/tanh_sinh.hpp>
#include <iostream>

double gamma_q05(double z)
{
	if(z>50) return 0;
	else	 return gsl_sf_gamma_inc(-0.5, z)/boost::math::tgamma(-0.5);
}

Node::Node()
{
//	qInfo() << "empty NODE" << endl;
}

Node::Node(QTreeWidgetItem* item):
	struct_Data(item->data(DEFAULT_COLUMN, Qt::UserRole).value<Data>())
{
	//	qInfo() << "NODE" << struct_Data.item_Type << endl;
}

void Node::calculate_Intermediate_Points(const Data& measurement, Node* above_Node, bool depth_Grading, bool inconvenient_Approximation, bool sigma_Grading, bool enable_Discretization, const Imperfections_Model& imperfections_Model, QString mode)
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
					// NaN checking
					for(size_t i=0; i<num_Points; ++i)
					{
						if(isnan(real(weak_Factor_R[i])) || isnan(imag(weak_Factor_R[i]))) weak_Factor_R[i] = 0;
						if(isnan(real(weak_Factor_T[i])) || isnan(imag(weak_Factor_T[i]))) weak_Factor_T[i] = 1;
					}

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

		double peak_Sigma = struct_Data.roughness_Model.peak_Sigma.value;
		double peak_Frequency = struct_Data.roughness_Model.peak_Frequency.value;
		double peak_Width = struct_Data.roughness_Model.peak_Frequency_Width.value;

		struct_Data.PSD_ABC_1D_Factor = 4*sqrt(M_PI) * tgamma(alpha+0.5)/tgamma(alpha) * sigma*sigma*xi;
		struct_Data.PSD_ABC_2D_Factor = 4*M_PI * sigma*sigma * xi*xi * alpha;
		struct_Data.PSD_Real_Gauss_1D_Factor = 2*sqrt(M_PI) * sigma*sigma*xi;
		struct_Data.PSD_Real_Gauss_2D_Factor = M_PI * sigma*sigma*xi*xi;

		// only for cases when we need factor
		// for splines we don't
		if(imperfections_Model.PSD_Model == ABC_Model)
		{
			struct_Data.main_PSD_1D_factor = struct_Data.PSD_ABC_1D_Factor;
			struct_Data.main_PSD_2D_factor = struct_Data.PSD_ABC_2D_Factor;
		}
		if(imperfections_Model.PSD_Model == fractal_Gauss_Model)
		{
			struct_Data.main_PSD_1D_factor = struct_Data.PSD_Real_Gauss_1D_Factor;
			struct_Data.main_PSD_2D_factor = struct_Data.PSD_Real_Gauss_2D_Factor;
		}

		if(peak_Frequency>DBL_EPSILON)
			struct_Data.PSD_Gauss_Peak_2D_Factor = peak_Sigma*peak_Sigma/(peak_Width*peak_Frequency*pow(M_PI,1.5)*(2-gamma_q05(peak_Frequency*peak_Frequency/(peak_Width*peak_Width))));
		else
			struct_Data.PSD_Gauss_Peak_2D_Factor = peak_Sigma*peak_Sigma/(M_PI*peak_Width*peak_Width);
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

	// if absolute density
	if(struct_Data.composed_Material)		epsilon_Ang = 1. - delta_Epsilon_Ang;
	// if relative density
	else
	{
		double density = struct_Data.relative_Density.value;
		if(specular_Case) density = struct_Data.average_Layer_density();
		epsilon_Ang = 1. - density * delta_Epsilon_Ang;
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

void Node::calc_Debye_Waller_Sigma(const Imperfections_Model& imperfections_Model, const Data& measurement)
{
	// angular width of detector
	double max_Delta_Theta_Detector = measurement.get_Max_Delta_Theta_Detector();

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
	specular_Debye_Waller_Weak_Factor_R.resize(num_Points,1);

	// case check
	if(!imperfections_Model.use_Roughness) return;
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(imperfections_Model.PSD_Model == ABC_Model || imperfections_Model.PSD_Model == fractal_Gauss_Model)
	{
		if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return; // if use_Common_Roughness_Function we calculate DW factor only for substrate
	}
	if(imperfections_Model.PSD_Model == measured_PSD)
	{
		if(struct_Data.item_Type != item_Type_Substrate) return;
		if(imperfections_Model.PSD_1D.argument.size()<2) return; // if PSD_1D not loaded
	}

	// max circular frequency inside detector
	vector<double> p0(num_Points);
	for(size_t i = 0; i<num_Points; ++i)
	{
		p0[i] = k[i]*abs(cos_Theta_0[i] - cos(qDegreesToRadians(angle_Theta_0[i] + max_Delta_Theta_Detector)));
	}

	// integration
	vector<double> sigma_2(num_Points);
	vector<double> peak_Sigma_2(num_Points,0);
	tanh_sinh<double> sigma_Integrator;
	double sigma_Integrator_Tolerance = 1E-4;

	auto psd_Peak = [&](double p){return gsl_spline_eval(spline_PSD_Peak, p, acc_PSD_Peak);};
	double peak_Sigma = 0;
	if(imperfections_Model.add_Gauss_Peak)
	{
		double peak_Range_Factor = 8;
			   peak_Sigma = struct_Data.roughness_Model.peak_Sigma.value;
		double peak_Frequency = struct_Data.roughness_Model.peak_Frequency.value;
		double peak_Frequency_Width = struct_Data.roughness_Model.peak_Frequency_Width.value;
		double p_Max = peak_Frequency + peak_Frequency_Width*peak_Range_Factor;


		for(size_t i = 0; i<num_Points; ++i)
		{
			if(p0[i]>DBL_MIN)
			{
				double p_Bound = min(p0[i]/(2*M_PI), p_Max); // real frequency
//				double integral = gauss_kronrod<double,11>::integrate(psd_Peak, 0, p_Bound, 1, 1e-7);
				double integral = sigma_Integrator.integrate(psd_Peak, 0, p_Bound, sigma_Integrator_Tolerance);
				if(isnan(integral)) integral = 0;
				peak_Sigma_2[i] = peak_Sigma*peak_Sigma - integral;
			} else
			{
				peak_Sigma_2[i] = peak_Sigma*peak_Sigma;
			}
		}
	}
	if(imperfections_Model.PSD_Model == ABC_Model)
	{
		double sigma = struct_Data.roughness_Model.sigma.value;
		double xi =    struct_Data.roughness_Model.cor_radius.value;
		double alpha = struct_Data.roughness_Model.fractal_alpha.value;

		if(imperfections_Model.vertical_Correlation == partial_Correlation &&
			(imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
			 imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model))
		{
			// for total sigma we integrate in whole range
			auto f_Full_Linear = [&](double nu)	{
				return 2*M_PI * gsl_spline_eval(spline_PSD_Linear_Growth_Top_2D, nu, acc_PSD_Linear_Growth_Top_2D) * nu;
			};
			auto f_Linear_1D = [&](double p)	{
				return gsl_spline_eval(spline_PSD_Linear_Growth_Top_1D, p, acc_PSD_Linear_Growth_Top_1D);
			};
			double total_Sigma_2 = sigma_Integrator.integrate(f_Linear_1D, 0, nu_Limit, sigma_Integrator_Tolerance); // f_Linear_1D is a bit better

			// for points
			for(size_t i = 0; i<num_Points; ++i)
			{
				if(p0[i]>DBL_MIN)
				{
					if(p0[i]/(2*M_PI)<nu_Limit) {
						sigma_2[i] = sigma_Integrator.integrate(f_Linear_1D, p0[i]/(2*M_PI), nu_Limit, sigma_Integrator_Tolerance);
					} else {
						sigma_2[i] = 0;
					}
				} else
				{
					sigma_2[i] = total_Sigma_2;
				}
			}
		} else
		{
			// full sigma up to nu_Limit
			double val = (2*M_PI*nu_Limit*xi);
			double total_Sigma_2 = sigma*sigma*(1.-pow(1 + val*val,-alpha));
			qInfo() << "ABC: true 2D sigma = "
					<< sqrt(total_Sigma_2 + peak_Sigma*peak_Sigma)
					<< endl;

			auto f_2 = [&](double p){return Global_Variables::PSD_ABC_1D_from_nu(struct_Data.PSD_ABC_1D_Factor, xi, alpha, p, nullptr, nullptr);};
			for(size_t i = 0; i<num_Points; ++i)
			{
				double z = -p0[i]*p0[i]*xi*xi; // z is non-negative
				double zz = z/(z-1);

				if(p0[i]>DBL_MIN)
				{
					if(abs(zz)<1)
					{
						double pFq = 1./sqrt(1-z) * gsl_sf_hyperg_2F1(0.5, 1.-alpha+1E-10, 1.5, zz);
//						double pFq = 1./sqrt(1-z) * boost::math::hypergeometric_pFq({0.5, 1.-alpha}, {1.5}, zz);
						sigma_2[i] = total_Sigma_2 - 2*p0[i]*xi*sigma*sigma*tgamma(alpha+0.5) * pFq / (sqrt(M_PI) * tgamma(alpha));
					} else
					{
						sigma_2[i] = sigma_Integrator.integrate(f_2, p0[i]/(2*M_PI), nu_Limit, sigma_Integrator_Tolerance);
					}
				} else
				{
					sigma_2[i] = total_Sigma_2;
				}
				if(imperfections_Model.add_Gauss_Peak)
				{
					sigma_2[i] += peak_Sigma_2[i];
				}
			}
		}
	}
	if(imperfections_Model.PSD_Model == fractal_Gauss_Model)
	{
		double sigma = struct_Data.roughness_Model.sigma.value;
		double xi =    struct_Data.roughness_Model.cor_radius.value;
		double alpha = struct_Data.roughness_Model.fractal_alpha.value;

		auto f_Cor_Sigma_1D = [&](double r) {return 1/r * sigma*sigma * exp(-pow(r/xi,2*alpha));};
		ooura_fourier_sin<double> integrator;

		// splining
		vector<double> sorted_p0 = p0;
		std::sort(sorted_p0.begin(), sorted_p0.end());
		double addition = 1E-10;
		double p_Min = max(sorted_p0.front() - addition,DBL_MIN);
		double p_Max =     sorted_p0.back()  + addition;

		int num_Sections = 4; // plus zero point
		vector<int> interpoints(num_Sections);
		int common_Size = 0;
		for(int i=0; i<num_Sections; i++)
		{
			interpoints[i] = 20;
			common_Size+=interpoints[i];
		}
		vector<double> interpoints_Sum_Argum_Vec(1+common_Size);
		vector<double> interpoints_Sum_Value_Vec(1+common_Size);

		vector<double> starts(num_Sections); // open start
		starts[0] = p_Min;
		starts[1] = p_Min+(p_Max-p_Min)*0.1;
		starts[2] = p_Min+(p_Max-p_Min)*0.30;
		starts[3] = p_Min+(p_Max-p_Min)*0.6;

		vector<double> dp(num_Sections);
		for(int i=0; i<num_Sections-1; i++)
		{
			dp[i] = (starts[i+1] - starts[i])/interpoints[i];
		}
		dp.back() = (p_Max - starts.back())/interpoints.back();

		double p = p_Min, result = 0;
		int counter = 1;

		for(int sec=0; sec<num_Sections; sec++)
		{
			for(int i=0; i<interpoints[sec]; i++)
			{
				p += dp[sec];

				// second part
				std::pair<double, double> result_Boost = integrator.integrate(f_Cor_Sigma_1D, p);
				result = result_Boost.first;

				interpoints_Sum_Argum_Vec[counter] = p;
				interpoints_Sum_Value_Vec[counter] = M_2_PI*result;
				counter++;
			}
		}
		gsl_interp_accel* acc_Delta_Sigma_2 = gsl_interp_accel_alloc();
		gsl_spline* spline_Delta_Sigma_2 = gsl_spline_alloc(gsl_interp_steffen, interpoints_Sum_Value_Vec.size());
		gsl_spline_init(spline_Delta_Sigma_2, interpoints_Sum_Argum_Vec.data(), interpoints_Sum_Value_Vec.data(), interpoints_Sum_Value_Vec.size());


		if(imperfections_Model.vertical_Correlation == partial_Correlation &&
			(imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
			 imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model))
		{
			// for total sigma we integrate in whole range
			auto f_Linear_1D = [&](double p)	{
				return gsl_spline_eval(spline_PSD_Linear_Growth_Top_1D, p, acc_PSD_Linear_Growth_Top_1D);
			};

			double nu_a = 1E-6;
			std::pair<double, double> result_Boost = integrator.integrate(f_Cor_Sigma_1D, 2*M_PI*nu_a);
			double sigma2_0_nu_a = M_2_PI*result_Boost.first;
			double total_Sigma_2 = sigma2_0_nu_a + sigma_Integrator.integrate(f_Linear_1D, nu_a, nu_Limit, sigma_Integrator_Tolerance);

			// peak from 0 to nu_a
			double peak_0_nu_a = 0;
			if(imperfections_Model.add_Gauss_Peak)
			{
				peak_0_nu_a = sigma_Integrator.integrate(psd_Peak, 0, nu_a, sigma_Integrator_Tolerance);
				total_Sigma_2 += peak_0_nu_a;
			}

			// for points
			for(size_t i = 0; i<num_Points; ++i)
			{
				if(p0[i]>DBL_MIN)
				{
					if(p0[i]/(2*M_PI)<nu_Limit) {
						if(p0[i]/(2*M_PI) < nu_a)
						{
							sigma_2[i] = total_Sigma_2 - gsl_spline_eval(spline_Delta_Sigma_2, p0[i], acc_Delta_Sigma_2) - (peak_Sigma*peak_Sigma - peak_Sigma_2[i]);
						} else
						{
							sigma_2[i] = total_Sigma_2 - sigma2_0_nu_a - peak_0_nu_a - sigma_Integrator.integrate(f_Linear_1D, nu_a, p0[i]/(2*M_PI), sigma_Integrator_Tolerance);
						}
					} else {
						sigma_2[i] = 0;
					}
				} else
				{
					sigma_2[i] = total_Sigma_2;
				}
			}
		} else
		{
			std::pair<double, double> result_Boost = integrator.integrate(f_Cor_Sigma_1D, 2*M_PI*nu_Limit);
			double total_Sigma_2 = M_2_PI*result_Boost.first;

			for(size_t i = 0; i<num_Points; ++i)
			{
				if(p0[i]>0)
				{
					sigma_2[i] = total_Sigma_2 - gsl_spline_eval(spline_Delta_Sigma_2, p0[i], acc_Delta_Sigma_2);
				} else
				{
					sigma_2[i] = total_Sigma_2;
				}
				if(imperfections_Model.add_Gauss_Peak)
				{
					sigma_2[i] += peak_Sigma_2[i];
				}
			}
		}
		gsl_spline_free(spline_Delta_Sigma_2);
		gsl_interp_accel_free(acc_Delta_Sigma_2);
	}
	if(imperfections_Model.PSD_Model == measured_PSD)
	{
		// PSD_2D should be loaded if partial correlation, otherwise PSD_1D
		PSD_Data psd_Data = imperfections_Model.vertical_Correlation == partial_Correlation ? imperfections_Model.PSD_2D : imperfections_Model.PSD_1D;
		double sigma_Factor = imperfections_Model.vertical_Correlation == partial_Correlation ? struct_Data.roughness_Model.sigma_Factor_PSD_2D.value : struct_Data.roughness_Model.sigma_Factor_PSD_1D.value;
		QVector<double> pi_nu(psd_Data.argument.size(),1);
		if(imperfections_Model.vertical_Correlation == partial_Correlation)
		{
			for(int i=0; i<psd_Data.argument.size(); i++)
			{
				pi_nu[i] = 2*M_PI*psd_Data.value[i];
			}
		}

		double min_Meas_p = psd_Data.argument.front();
		double max_Meas_p = psd_Data.argument.back();
		double full_Sigma2 = pow(psd_Data.calc_Sigma_Effective(),2);

		for(size_t i = 0; i<num_Points; ++i)
		{
			// real frequency
			p0[i] = p0[i]/(2*M_PI);

			if(p0[i]<=min_Meas_p)
			{
				sigma_2[i] = full_Sigma2;
			} else
			if(p0[i]>min_Meas_p && p0[i]<max_Meas_p)
			{
				// get psd_Index
				int psd_Index = 1;
				for(psd_Index = 1; psd_Index<psd_Data.argument.size(); psd_Index++)
				{
					if(p0[i]> psd_Data.argument[psd_Index-1] &&
					   p0[i]<=psd_Data.argument[psd_Index]) break;
				}

				// first point
				double dnu = psd_Data.argument[psd_Index] - p0[i] + (psd_Data.argument[psd_Index+1] - psd_Data.argument[psd_Index-1])/4;
				sigma_2[i] += psd_Data.value[psd_Index]*dnu*pi_nu[psd_Index];

				// intermediate points
				for(int integration_index = psd_Index+1; integration_index<psd_Data.argument.size()-1; integration_index++)
				{
					dnu = (psd_Data.argument[integration_index+1] - psd_Data.argument[integration_index-1])/2;
					sigma_2[i] += psd_Data.value[integration_index]*dnu*pi_nu[integration_index];
				}

				// last point
				dnu = psd_Data.argument[psd_Data.argument.size()-1] - psd_Data.argument[psd_Data.argument.size()-2];
				sigma_2[i] += psd_Data.value[psd_Data.argument.size()-1]*dnu*pi_nu[psd_Data.argument.size()-1];
			} else
			// p0[i]>max_Meas_p
			{
				sigma_2[i] = 0;
			}

			// multiply by factor
			sigma_2[i] *= sigma_Factor*sigma_Factor;
		}
	}


	for(size_t i = 0; i<num_Points; ++i)
	{
		double hi = k[i]*sin(qDegreesToRadians(angle_Theta_0[i]));
		specular_Debye_Waller_Weak_Factor_R[i] = exp( - 4. * hi*hi * sigma_2[i] );
	}
}

void Node::calc_Debye_Waller_Total_Sigma(const Imperfections_Model& imperfections_Model)
{
	// case check
	if(!imperfections_Model.use_Roughness) return;
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(imperfections_Model.PSD_Model == ABC_Model || imperfections_Model.PSD_Model == fractal_Gauss_Model)
	{
		if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return; // if use_Common_Roughness_Function we calculate DW factor only for substrate
	}
	if(imperfections_Model.PSD_Model == measured_PSD)
	{
		if(struct_Data.item_Type != item_Type_Substrate) return;
		if(imperfections_Model.PSD_1D.argument.size()<2) return; // if PSD_1D not loaded
	}

	specular_Debye_Waller_Total_Sigma = 0;

	tanh_sinh<double> sigma_Integrator;
	double sigma_Integrator_Tolerance = 1E-4;
	auto psd_Peak = [&](double p){return gsl_spline_eval(spline_PSD_Peak, p, acc_PSD_Peak);};

	double peak_Sigma = 0;
	if(imperfections_Model.add_Gauss_Peak)	{
		peak_Sigma = struct_Data.roughness_Model.peak_Sigma.value;
	}


	if(imperfections_Model.PSD_Model == ABC_Model)
	{
		double sigma = struct_Data.roughness_Model.sigma.value;
		double xi =    struct_Data.roughness_Model.cor_radius.value;
		double alpha = struct_Data.roughness_Model.fractal_alpha.value;

		if(imperfections_Model.vertical_Correlation == partial_Correlation &&
			(imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
			 imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model))
		{
			// for total sigma we integrate in whole range
			auto f_Linear_1D = [&](double p)	{
				return gsl_spline_eval(spline_PSD_Linear_Growth_Top_1D, p, acc_PSD_Linear_Growth_Top_1D);
			};
			specular_Debye_Waller_Total_Sigma = sigma_Integrator.integrate(f_Linear_1D, 0, nu_Limit, sigma_Integrator_Tolerance);
		} else
		{
			// full sigma up to nu_Limit
			double val = (2*M_PI*nu_Limit*xi);
			specular_Debye_Waller_Total_Sigma = sigma*sigma*(1.-pow(1 + val*val,-alpha)) + peak_Sigma*peak_Sigma;

		}
	}
	if(imperfections_Model.PSD_Model == fractal_Gauss_Model)
	{
		double sigma = struct_Data.roughness_Model.sigma.value;
		double xi =    struct_Data.roughness_Model.cor_radius.value;
		double alpha = struct_Data.roughness_Model.fractal_alpha.value;

		auto f_Cor_Sigma_1D = [&](double r) {return 1/r * sigma*sigma * exp(-pow(r/xi,2*alpha));};
		ooura_fourier_sin<double> integrator;

		if(imperfections_Model.vertical_Correlation == partial_Correlation &&
			(imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
			 imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model))
		{
			// for total sigma we integrate in whole range
			auto f_Linear_1D = [&](double p)	{
				return gsl_spline_eval(spline_PSD_Linear_Growth_Top_1D, p, acc_PSD_Linear_Growth_Top_1D);
			};

			double nu_a = 1E-6;
			std::pair<double, double> result_Boost = integrator.integrate(f_Cor_Sigma_1D, 2*M_PI*nu_a);
			double sigma2_0_nu_a = M_2_PI*result_Boost.first;
			specular_Debye_Waller_Total_Sigma = sigma2_0_nu_a + sigma_Integrator.integrate(f_Linear_1D, nu_a, nu_Limit, sigma_Integrator_Tolerance);

			// peak from 0 to nu_a
			double peak_0_nu_a = 0;
			if(imperfections_Model.add_Gauss_Peak)
			{
				peak_0_nu_a = sigma_Integrator.integrate(psd_Peak, 0, nu_a, sigma_Integrator_Tolerance);
				specular_Debye_Waller_Total_Sigma += peak_0_nu_a;
			}
		} else
		{
			std::pair<double, double> result_Boost = integrator.integrate(f_Cor_Sigma_1D, 2*M_PI*nu_Limit);
			specular_Debye_Waller_Total_Sigma = M_2_PI*result_Boost.first + peak_Sigma*peak_Sigma;
		}
	}
	if(imperfections_Model.PSD_Model == measured_PSD)
	{
		// PSD_2D should be loaded if partial correlation, otherwise PSD_1D
		PSD_Data psd_Data = imperfections_Model.vertical_Correlation == partial_Correlation ? imperfections_Model.PSD_2D : imperfections_Model.PSD_1D;
		double sigma_Factor = imperfections_Model.vertical_Correlation == partial_Correlation ? struct_Data.roughness_Model.sigma_Factor_PSD_2D.value : struct_Data.roughness_Model.sigma_Factor_PSD_1D.value;
		QVector<double> pi_nu(psd_Data.argument.size(),1);
		if(imperfections_Model.vertical_Correlation == partial_Correlation)
		{
			for(int i=0; i<psd_Data.argument.size(); i++)
			{
				pi_nu[i] = 2*M_PI*psd_Data.value[i];
			}
		}
		double full_Sigma2 = pow(psd_Data.calc_Sigma_Effective(),2);

		specular_Debye_Waller_Total_Sigma = full_Sigma2*sigma_Factor*sigma_Factor;
	}
	specular_Debye_Waller_Total_Sigma = sqrt(specular_Debye_Waller_Total_Sigma);
}

struct Params
{
	double lambda;
	double sin_Theta;
	Data* struct_Data;
	ooura_fourier_sin<double>* ooura_integrator;
	tanh_sinh<double>* tanh_sinh_integrator;
	gsl_spline* spline_PSD_Peak;
	gsl_interp_accel* acc_PSD_Peak;
	gsl_spline* spline_PSD;
	gsl_interp_accel* acc_PSD;
};

double integral_ABC(double p0, Params& params)
{
	const double& xi =    params.struct_Data->roughness_Model.cor_radius.value;
	const double& alpha = params.struct_Data->roughness_Model.fractal_alpha.value;

	double z = -4*M_PI*M_PI*p0*p0*xi*xi;	// z is non-negative
	double zz = z/(z-1);					// 0 <= zz < 1is non-negative

	if(abs(zz)<1)
	{
		double pFq = 1./sqrt(1-z) * gsl_sf_hyperg_2F1(0.5, 1.-alpha+1E-10, 1.5, zz);
		return params.struct_Data->PSD_ABC_1D_Factor * p0 * pFq * params.lambda / params.sin_Theta;
//		return 4*sqrt(M_PI)*p0*xi*sigma*sigma*tgamma(alpha+0.5) * pFq / params.lambda / params.sin_Theta;
	}

	qInfo() << "Node::integral_ABC  :  abs(zz)>=1, zz = " << zz << endl;
	return 0.;
}
double integral_Fractal_Gauss(double p0, Params& params)
{
	const double& sigma = params.struct_Data->roughness_Model.sigma.value;
	const double& xi =    params.struct_Data->roughness_Model.cor_radius.value;
	const double& alpha = params.struct_Data->roughness_Model.fractal_alpha.value;

	auto f = [&](double r) {return 1/r * sigma*sigma * exp(-pow(r/xi,2*alpha));};
	std::pair<double, double> result_Boost = params.ooura_integrator->integrate(f, 2*M_PI*p0);

	return M_2_PI*result_Boost.first * params.lambda / params.sin_Theta;
}
double integral_Real_Gauss(double p0, Params& params)
{
	const double& sigma = params.struct_Data->roughness_Model.sigma.value;
	const double& xi =    params.struct_Data->roughness_Model.cor_radius.value;

	return sigma*sigma*erf(p0*M_PI*xi) * params.lambda / params.sin_Theta;
}
double integral_Gauss_Peak(double p0, Params& params)
{
	double termination = 1E-5;
	double error, L1;
	size_t levels;
	auto f = [&](double p) {return gsl_spline_eval(params.spline_PSD_Peak, p, params.acc_PSD_Peak);};
	double result = params.tanh_sinh_integrator->integrate(f, 0.0, p0, termination, &error, &L1, &levels);

	return result * params.lambda / params.sin_Theta;
}
double zero_Func(double p0, Params& params)
{
	Q_UNUSED(p0)
	Q_UNUSED(params)
	return 0;
}

void Node::calc_Integral_Intensity_Near_Specular(const Imperfections_Model& imperfections_Model, Data& measurement, const Calc_Functions& calc_Functions)
{
	if(struct_Data.item_Type != item_Type_Substrate) return;

	// angular width of detector
	double max_Delta_Theta_Detector = measurement.get_Max_Delta_Theta_Detector();
	double area_Factor = 4.0;
	double ooura_Precision = 1E-4;
	// less impact of growth roughness
	if(imperfections_Model.vertical_Correlation == partial_Correlation &&
		(imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
		 imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model))
	{
		area_Factor = 3.0;
	}
	double limit_Distance_to_Specular = area_Factor*max_Delta_Theta_Detector;

	vector<double> theta_0_Vec,     theta_Vec;
	vector<double> theta_0_Cos_Vec, theta_Cos_Vec, theta_Sin_Vec;
	size_t num_Points;

	if(measurement.measurement_Type == measurement_Types[Detector_Scan])
	{
		theta_Vec = measurement.detector_Theta_Angle_Vec;
		theta_Cos_Vec = measurement.detector_Theta_Cos_Vec;
		theta_Sin_Vec = measurement.detector_Theta_Sin_Vec;

		num_Points = theta_Vec.size();

		theta_0_Vec.resize(num_Points, measurement.beam_Theta_0_Angle_Value);
		theta_0_Cos_Vec.resize(num_Points, measurement.beam_Theta_0_Cos_Value);
	}
	if(measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
	   measurement.measurement_Type == measurement_Types[Offset_Scan])
	{
		theta_0_Vec = measurement.beam_Theta_0_Angle_Vec;
		theta_0_Cos_Vec = measurement.beam_Theta_0_Cos_Vec;

		num_Points = theta_0_Vec.size();

		theta_Vec = measurement.detector_Theta_Angle_Vec;
		theta_Cos_Vec = measurement.detector_Theta_Cos_Vec;
		theta_Sin_Vec = measurement.detector_Theta_Sin_Vec;
	}

	measurement.detector_Factor_for_Intensity_Integration.resize(num_Points);
	for(double& point : measurement.detector_Factor_for_Intensity_Integration) point = qDegreesToRadians(measurement.theta_Resolution_FWHM);

	int& first_Point = measurement.first_Point_of_Intensity_Integration;
	int& second_Point = measurement.last_Point_of_Intensity_Integration;
	first_Point = -2021;
	second_Point = -2021;

	if(imperfections_Model.approximation != PT_approximation) return;
	if(!calc_Functions.instrumental_Smoothing || !calc_Functions.integrate_PSD_in_Detector) return;
	if(struct_Data.roughness_Model.sigma.value<=DBL_EPSILON) {
		if(!imperfections_Model.add_Gauss_Peak || struct_Data.roughness_Model.peak_Sigma.value<=DBL_EPSILON) {
			return;
		}
	}

	bool first_Point_Detected = false;
	for(int i = 0; i<num_Points; i++)
	{
		double detector_Theta_Position = theta_Vec[i];
		double specular_Theta_Position = theta_0_Vec[i];

		if(abs(detector_Theta_Position-specular_Theta_Position)<=limit_Distance_to_Specular) {
			second_Point = i;
			if(!first_Point_Detected) {
				first_Point = i;
				first_Point_Detected = true;
			}
		}
	}
	if(first_Point<0 || second_Point<0) return;

	vector<double> p1(num_Points), p2(num_Points);
	vector<double> p_min(num_Points), p_max_1(num_Points), p_max_2(num_Points);
	vector<double> two_Intervals(num_Points,false);

	// finding integration intervals for each point
	for(int i = first_Point; i<=second_Point; i++)
	{
		p1[i] = 1/measurement.lambda_Value * (theta_0_Cos_Vec[i] - cos(qDegreesToRadians(theta_Vec[i] - max_Delta_Theta_Detector)));
		p2[i] = 1/measurement.lambda_Value * (theta_0_Cos_Vec[i] - cos(qDegreesToRadians(theta_Vec[i] + max_Delta_Theta_Detector)));

		p_max_1[i] = max(abs(p1[i]),abs(p2[i]));
		if(p1[i]*p2[i]<0)
		{
			p_min[i] = 0;
			p_max_2[i] = min(abs(p1[i]),abs(p2[i]));
			two_Intervals[i] = true;
		} else
		{
			p_min[i] = min(abs(p1[i]),abs(p2[i]));
			p_max_2[i] = -2021;
		}
	}

	// choosing functions
	double (*integral_Func)(double, Params&);
	double (*PSD_Func)(double, double, double, double, double, double, gsl_spline*, gsl_interp_accel*);
	double (*integral_Peak_Func)(double, Params&);
	double (*PSD_Peak_Func)(double, double, double, double, double, double, gsl_spline*, gsl_interp_accel*);
	double factor = 1;

	if(imperfections_Model.PSD_Model == ABC_Model)	{
		integral_Func = &integral_ABC;
		PSD_Func = &Global_Variables::PSD_ABC_1D;
		factor = struct_Data.PSD_ABC_1D_Factor;
	}
	if(imperfections_Model.PSD_Model == fractal_Gauss_Model)	{
		const double& alpha = struct_Data.roughness_Model.fractal_alpha.value;

		if(abs(alpha-1)>DBL_EPSILON) {
			integral_Func = &integral_Fractal_Gauss;
			PSD_Func = &Global_Variables::PSD_Fractal_Gauss_1D;
		} else {
			integral_Func = &integral_Real_Gauss;
			PSD_Func = &Global_Variables::PSD_Real_Gauss_1D;
			factor = struct_Data.PSD_Real_Gauss_1D_Factor;
		}
	}
	if(imperfections_Model.PSD_Model == measured_PSD)	{
		// TODO remove!
	}

	// add near-specular gauss peak intensity
	if(imperfections_Model.add_Gauss_Peak && struct_Data.roughness_Model.peak_Sigma.value>DBL_EPSILON)	{
		integral_Peak_Func = &integral_Gauss_Peak;
		PSD_Peak_Func = &Global_Variables::PSD_Gauss_Peak_1D;
	} else	{
		integral_Peak_Func = &zero_Func;
		PSD_Peak_Func = &Global_Variables::zero_PSD_1D;
	}

	// add near-specular linear growth intensity
	{
		// TODO
//		double growth_Zero_Frequency_Addition = 0;
//		for(int media_Index = 0; media_Index<media_Data_Map_Vector.size(); media_Index++)
//		{
//			const Data& layer = *(media_Data_Map_Vector[media_Index]);
//			if(layer.item_Type == item_Type_Layer)
//			{
//				double omega = layer.roughness_Model.omega.value;
//				double thickness = layer.thickness.value; // without drift, but it should have low importance
//				growth_Zero_Frequency_Addition += omega*thickness;
//			}
//		}
	}

	/// with parallelization

	// detector-factor for multiplying calculated intensity
	Global_Variables::parallel_For(second_Point-first_Point+1, reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		// parameters for calculation
		ooura_fourier_sin<double> ooura_integrator(ooura_Precision);
		tanh_sinh<double> tanh_sinh_integrator;

		Params params;
		params.lambda = measurement.lambda_Value;
		//params.sin_Theta = measurement.detector_Theta_Sin_Vec;
		params.struct_Data = &struct_Data;
		params.ooura_integrator = &ooura_integrator;
		params.tanh_sinh_integrator = &tanh_sinh_integrator;
		params.spline_PSD_Peak = spline_PSD_Peak;
		params.acc_PSD_Peak = acc_PSD_Peak;
		params.spline_PSD = spline_PSD_FG_1D;
		params.acc_PSD = acc_PSD_FG_1D;


		Q_UNUSED(thread_Index)
		for(int i = first_Point + n_Min; i<first_Point+n_Max; i++)
		{
			params.sin_Theta = theta_Sin_Vec[i];

			// divide on old value, multiply on new
			const double& xi =    struct_Data.roughness_Model.cor_radius.value;
			const double& alpha = struct_Data.roughness_Model.fractal_alpha.value;
			const double& peak_Frequency = struct_Data.roughness_Model.peak_Frequency.value;
			const double& peak_Frequency_Width = struct_Data.roughness_Model.peak_Frequency_Width.value;
			const double& k = measurement.k_Value;
			const double& cos_Theta_0 = theta_0_Cos_Vec[i];
			const double& cos_Theta = theta_Cos_Vec[i];

			if(two_Intervals[i])
			{
				measurement.detector_Factor_for_Intensity_Integration[i]  =      integral_Func(p_max_1[i], params) +      integral_Func(p_max_2[i], params);
//				measurement.detector_Factor_for_Intensity_Integration[i] += integral_Peak_Func(p_max_1[i], params) + integral_Peak_Func(p_max_2[i], params);
				measurement.detector_Factor_for_Intensity_Integration[i] += PSD_Peak_Func(factor, peak_Frequency, peak_Frequency_Width, k, cos_Theta, cos_Theta_0, spline_PSD_Peak, acc_PSD_Peak)*qDegreesToRadians(measurement.theta_Resolution_FWHM);
			} else
			{
				measurement.detector_Factor_for_Intensity_Integration[i]  =      integral_Func(p_max_1[i], params) -      integral_Func(p_min[i], params);
//				measurement.detector_Factor_for_Intensity_Integration[i] += integral_Peak_Func(p_max_1[i], params) - integral_Peak_Func(p_min[i], params);
				measurement.detector_Factor_for_Intensity_Integration[i] += PSD_Peak_Func(factor, peak_Frequency, peak_Frequency_Width, k, cos_Theta, cos_Theta_0, spline_PSD_Peak, acc_PSD_Peak)*qDegreesToRadians(measurement.theta_Resolution_FWHM);
			}

			measurement.detector_Factor_for_Intensity_Integration[i] = max(measurement.detector_Factor_for_Intensity_Integration[i], 0.);

			measurement.detector_Factor_for_Intensity_Integration[i] /= (
														PSD_Func(factor, xi, alpha, k, cos_Theta, cos_Theta_0, spline_PSD_FG_1D, acc_PSD_FG_1D) +
														PSD_Peak_Func(factor, peak_Frequency, peak_Frequency_Width, k, cos_Theta, cos_Theta_0, spline_PSD_Peak, acc_PSD_Peak)
														);
			if( isinf(measurement.detector_Factor_for_Intensity_Integration[i]) ||
				isnan(measurement.detector_Factor_for_Intensity_Integration[i]))
			{
				measurement.detector_Factor_for_Intensity_Integration[i] = qDegreesToRadians(measurement.theta_Resolution_FWHM);
			}
		}
	});

	/// without parallelization

//	// parameters for calculation
//	ooura_fourier_sin<double> ooura_integrator(ooura_Precision);
//	tanh_sinh<double> tanh_sinh_integrator;

//	Params params;
//	params.lambda = measurement.lambda_Value;
//	//params.sin_Theta = measurement.detector_Theta_Sin_Vec;
//	params.struct_Data = &struct_Data;
//	params.ooura_integrator = &ooura_integrator;
//	params.tanh_sinh_integrator = &tanh_sinh_integrator;
//	params.spline_PSD_Peak = spline_PSD_Peak;
//	params.acc_PSD_Peak = acc_PSD_Peak;
//	params.spline_PSD = spline_PSD_FG_1D;
//	params.acc_PSD = acc_PSD_FG_1D;

//	for(int i = first_Point; i<=second_Point; i++)
//	{
//		params.sin_Theta = theta_Sin_Vec[i];

//		// divide on old value, multiply on new
//		const double& xi =    struct_Data.roughness_Model.cor_radius.value;
//		const double& alpha = struct_Data.roughness_Model.fractal_alpha.value;
//		const double& peak_Frequency = struct_Data.roughness_Model.peak_Frequency.value;
//		const double& peak_Frequency_Width = struct_Data.roughness_Model.peak_Frequency_Width.value;
//		const double& k = measurement.k_Value;
//		const double& cos_Theta_0 = theta_0_Cos_Vec[i];
//		const double& cos_Theta = theta_Cos_Vec[i];

//		if(two_Intervals[i])
//		{
//			measurement.detector_Factor_for_Intensity_Integration[i]  =      integral_Func(p_max_1[i], params) +      integral_Func(p_max_2[i], params);
////			measurement.detector_Factor_for_Intensity_Integration[i] += integral_Peak_Func(p_max_1[i], params) + integral_Peak_Func(p_max_2[i], params);
//			measurement.detector_Factor_for_Intensity_Integration[i] += PSD_Peak_Func(factor, peak_Frequency, peak_Frequency_Width, k, cos_Theta, cos_Theta_0, spline_PSD_Peak, acc_PSD_Peak)*qDegreesToRadians(measurement.theta_Resolution_FWHM);
//		} else
//		{
//			measurement.detector_Factor_for_Intensity_Integration[i]  =      integral_Func(p_max_1[i], params) -      integral_Func(p_min[i], params);
////			measurement.detector_Factor_for_Intensity_Integration[i] += integral_Peak_Func(p_max_1[i], params) - integral_Peak_Func(p_min[i], params);
//			measurement.detector_Factor_for_Intensity_Integration[i] += PSD_Peak_Func(factor, peak_Frequency, peak_Frequency_Width, k, cos_Theta, cos_Theta_0, spline_PSD_Peak, acc_PSD_Peak)*qDegreesToRadians(measurement.theta_Resolution_FWHM);
//		}

//		measurement.detector_Factor_for_Intensity_Integration[i] = max(measurement.detector_Factor_for_Intensity_Integration[i], 0.);

//		measurement.detector_Factor_for_Intensity_Integration[i] /= (
//												PSD_Func(factor, xi, alpha, k, cos_Theta, cos_Theta_0, spline_PSD_FG_1D, acc_PSD_FG_1D)
//											  + PSD_Peak_Func(factor, peak_Frequency, peak_Frequency_Width, k, cos_Theta, cos_Theta_0, spline_PSD_Peak, acc_PSD_Peak)
//											  );

//		if( isinf(measurement.detector_Factor_for_Intensity_Integration[i]) ||
//			isnan(measurement.detector_Factor_for_Intensity_Integration[i]))
//		{
//			measurement.detector_Factor_for_Intensity_Integration[i] = qDegreesToRadians(measurement.theta_Resolution_FWHM);
//		}


////		double p = abs(measurement.beam_Theta_0_Cos_Value - measurement.detector_Theta_Cos_Vec[i])/measurement.lambda_Value;
//		qInfo() << measurement.detector_Theta_Angle_Vec[i] << measurement.detector_Factor_for_Intensity_Integration[i] / qDegreesToRadians(measurement.theta_Resolution_FWHM)
////			<< qDegreesToRadians(measurement.theta_Resolution_FWHM) *
////			   4*sqrt(M_PI) * tgamma(alpha+0.5)/tgamma(alpha) * sigma*sigma*xi / pow(1+(2*M_PI*p*xi)*(2*M_PI*p*xi), alpha+0.5)
////				<< qDegreesToRadians(measurement.theta_Resolution_FWHM) *
////				   PSD_Func(factor, xi, alpha, k, cos_Theta, cos_Theta_0, spline_PSD, acc_PSD)
////				<< qDegreesToRadians(measurement.theta_Resolution_FWHM) *
////				   PSD_Peak_Func(factor, peak_Frequency, peak_Frequency_Width, k, cos_Theta, cos_Theta_0, spline_PSD_Peak, acc_PSD_Peak)
////				<< PSD_Func(factor, xi, alpha, k, cos_Theta, cos_Theta_0, spline_PSD_FG_1D, acc_PSD_FG_1D)
//				<< endl;
//	}
//	qInfo() << endl << endl;
}

void Node::create_Spline_PSD_Fractal_Gauss_1D(const Imperfections_Model& imperfections_Model, const Data& measurement)
{
	if(imperfections_Model.approximation != PT_approximation) return;
	if(imperfections_Model.PSD_Model != fractal_Gauss_Model) return;
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;

	// in other cases ( substrate or layer-with-individual-function ) go further

	double sigma = struct_Data.roughness_Model.sigma.value;
	double xi =    struct_Data.roughness_Model.cor_radius.value;
	double alpha = struct_Data.roughness_Model.fractal_alpha.value;

	vector<double> temp_Cos = measurement.detector_Theta_Cos_Vec;

	double addition = 1E-8;
	double p_Max = 2*M_PI*nu_Limit*(1+addition);

	if( measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
		for(size_t i=0; i<temp_Cos.size(); i++) temp_Cos[i] = measurement.k_Value*abs(temp_Cos[i] - measurement.beam_Theta_0_Cos_Value);
	}
	if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		for(size_t i=0; i<temp_Cos.size(); i++) temp_Cos[i] = measurement.k_Value*abs(temp_Cos[i] - measurement.beam_Theta_0_Cos_Vec[i]);
	}
	if(temp_Cos.size()>0)
	{
		std::sort(temp_Cos.begin(), temp_Cos.end());
		p_Max = temp_Cos.back()+addition;
	}

	if( imperfections_Model.vertical_Correlation == partial_Correlation &&
	   (imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
		imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model ))
	{
		p_Max = max(p_Max, 2*M_PI*nu_Limit*(1+addition));
	}

	int num_Sections = 8; // plus zero point
	int points_Per_Section = 50;
	vector<int> interpoints(num_Sections);
	int common_Size = 0;
	for(int i=0; i<num_Sections; i++)
	{
//		interpoints[i] = 40-1*i;
		interpoints[i] = points_Per_Section;
		common_Size+=interpoints[i];
	}
	vector<double> interpoints_Sum_Argum_Vec(1+common_Size);
	vector<double> interpoints_Sum_Value_Vec(1+common_Size);

	vector<double> starts(num_Sections); // open start
	starts[0] = 0;
	starts[1] = p_Max/5000;
	starts[2] = p_Max/1000;
	starts[3] = p_Max/200;
	starts[4] = p_Max/50;
	starts[5] = p_Max/10;
	starts[6] = p_Max/4;
	starts[7] = p_Max/2;

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

	// boost integrator
	auto f = [&](double r) {return sigma*sigma * exp(-pow(r/xi,2*alpha));};
	const double tol = 1E-7;
	int depth = 4;

	/// with parallelization
	int counter = 1;
	for(int sec=0; sec<num_Sections; sec++)
	{
		double p = starts[sec];
		for(int i=0; i<interpoints[sec]; i++)
		{
			p += dp[sec];
			interpoints_Sum_Argum_Vec[counter] = p;
			counter++;
		}
	}
	Global_Variables::parallel_For(common_Size, reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		Q_UNUSED(thread_Index)
		ooura_fourier_cos<double> integrator(tol,depth);

		double p = 0;
		double result = 0;
		for(int i = 1+n_Min; i<1+n_Max; i++)
		{
			p = interpoints_Sum_Argum_Vec[i];
			if(abs(1-alpha)>DBL_EPSILON/* && abs(0.5-alpha)>DBL_EPSILON*/) // integrate even for alpha == 0.5
			{
				std::pair<double, double> result_Boost = integrator.integrate(f, p);
				result = result_Boost.first;
			}
			interpoints_Sum_Value_Vec[i] = 4*result;
		}
	});

	/// without parallelization

//	double p = 0, result = 0;
//	int counter = 1;
//	ooura_fourier_cos<double> integrator(tol,depth);
//	for(int sec=0; sec<num_Sections; sec++)
//	{
//		for(int i=0; i<interpoints[sec]; i++)
//		{
//			p += dp[sec];

//			interpoints_Sum_Argum_Vec[counter] = p;
//			if(abs(1-alpha)>DBL_EPSILON/* && abs(0.5-alpha)>DBL_EPSILON*/) // integrate even for alpha == 0.5
//			{
//				std::pair<double, double> result_Boost = integrator.integrate(f, p);
//				result = result_Boost.first;
//			}

//			interpoints_Sum_Value_Vec[counter] = 4*result;
//			counter++;
//		}
//	}
	interpoints_Sum_Argum_Vec.push_back(1.1*p_Max);
	interpoints_Sum_Value_Vec.push_back(0);
	interpoints_Sum_Argum_Vec.push_back(DBL_MAX);
	interpoints_Sum_Value_Vec.push_back(0);


	acc_PSD_FG_1D = gsl_interp_accel_alloc();
	if(p_Max<10*addition) 	spline_PSD_FG_1D = gsl_spline_alloc(gsl_interp_linear, interpoints_Sum_Value_Vec.size());
	else					spline_PSD_FG_1D = gsl_spline_alloc(gsl_interp_steffen,interpoints_Sum_Value_Vec.size());

	gsl_spline_init(spline_PSD_FG_1D, interpoints_Sum_Argum_Vec.data(), interpoints_Sum_Value_Vec.data(), interpoints_Sum_Value_Vec.size());
}
void Node::clear_Spline_PSD_Fractal_Gauss_1D(const Imperfections_Model& imperfections_Model)
{
	if(imperfections_Model.approximation != PT_approximation) return;
	if(imperfections_Model.PSD_Model != fractal_Gauss_Model) return;
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;

	gsl_spline_free(spline_PSD_FG_1D);
	gsl_interp_accel_free(acc_PSD_FG_1D);
}

void Node::create_Spline_PSD_Fractal_Gauss_2D(const Imperfections_Model& imperfections_Model, const Data& measurement)
{
//	if(imperfections_Model.approximation != PT_approximation) return;
	if(imperfections_Model.PSD_Model != fractal_Gauss_Model) return;
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;

	// in other cases ( substrate or layer-with-individual-function ) go further

	double sigma = struct_Data.roughness_Model.sigma.value;
	double xi =    struct_Data.roughness_Model.cor_radius.value;
	double alpha = struct_Data.roughness_Model.fractal_alpha.value;

	double nu_Max = 0;
	double addition = 1E-8;
	if(measurement.measurement_Type == measurement_Types[GISAS_Map])
	{
		vector<double> temp_Nu2(measurement.detector_Theta_Cos_Vec.size());
		double min_Cos_Phi = min(measurement.detector_Phi_Cos_Vec.front(), measurement.detector_Phi_Cos_Vec.back());
		for(size_t i=0; i<temp_Nu2.size(); i++)
		{
			temp_Nu2[i] = measurement.k_Value*measurement.k_Value*( measurement.detector_Theta_Cos_Vec[i]*measurement.detector_Theta_Cos_Vec[i] +
																	measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value -
																  2*measurement.beam_Theta_0_Cos_Value*measurement.detector_Theta_Cos_Vec[i]*min_Cos_Phi);
		}
		std::sort(temp_Nu2.begin(), temp_Nu2.end());
		nu_Max = sqrt(temp_Nu2.back())*(1+addition);
	} else
	if(measurement.measurement_Type == measurement_Types[Specular_Scan])
	{
		nu_Max = 2*M_PI*nu_Limit*(1+addition)*10;
	} else
	{
		if(imperfections_Model.vertical_Correlation == partial_Correlation)
		{
			double min_Cos_Phi = cos(qDegreesToRadians(max_Phi_Azimuthal_Integration));
			vector<double> temp_Nu2 = measurement.detector_Theta_Cos_Vec;
			if( measurement.measurement_Type == measurement_Types[Detector_Scan] )
			{
				for(size_t i=0; i<temp_Nu2.size(); i++)
				{
					temp_Nu2[i] = measurement.k_Value*measurement.k_Value*(measurement.detector_Theta_Cos_Vec[i]*measurement.detector_Theta_Cos_Vec[i] +
																		   measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value -
																		   2*measurement.beam_Theta_0_Cos_Value*measurement.detector_Theta_Cos_Vec[i]*min_Cos_Phi);
				}
			}
			if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
				measurement.measurement_Type == measurement_Types[Offset_Scan] )
			{
				for(size_t i=0; i<temp_Nu2.size(); i++)
				{
					temp_Nu2[i] = measurement.k_Value*measurement.k_Value*(measurement.detector_Theta_Cos_Vec[i]*measurement.detector_Theta_Cos_Vec[i] +
																		   measurement.beam_Theta_0_Cos_Vec[i]*measurement.beam_Theta_0_Cos_Vec[i] -
																		   2*measurement.beam_Theta_0_Cos_Vec[i]*measurement.detector_Theta_Cos_Vec[i]*min_Cos_Phi);
				}
			}
			std::sort(temp_Nu2.begin(), temp_Nu2.end());
			nu_Max = sqrt(temp_Nu2.back())*(1+addition);

			if( imperfections_Model.vertical_Correlation == partial_Correlation &&
			   (imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model ||
				imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model ))
			{
				nu_Max = max(nu_Max, 2*M_PI*nu_Limit*(1+addition)*10);
			}
		}
	}

	int num_Sections = 8; // plus zero point
	int points_Per_Section = 60;
	vector<int> interpoints(num_Sections);
	int common_Size = 0;
	for(int i=0; i<num_Sections; i++)
	{
//		interpoints[i] = 50-3*i;
		interpoints[i] = points_Per_Section;
		common_Size+=interpoints[i];
	}
	vector<double> interpoints_Sum_Argum_Vec(1+common_Size);
	vector<double> interpoints_Sum_Value_Vec(1+common_Size);

	vector<double> starts(num_Sections); // open start
	starts[0] = 0;
	starts[1] = nu_Max/20000;
	starts[2] = nu_Max/5000;
	starts[3] = nu_Max/1000;
	starts[4] = nu_Max/100;
	starts[5] = nu_Max/10;
	starts[6] = nu_Max/5;
	starts[7] = nu_Max/2;

	vector<double> dnu(num_Sections);
	for(int i=0; i<num_Sections-1; i++)
	{
		dnu[i] = (starts[i+1] - starts[i])/interpoints[i];
	}
	dnu.back() = (nu_Max - starts.back())/interpoints.back();

	// zero point
	{
		interpoints_Sum_Argum_Vec[0] = 0;
		interpoints_Sum_Value_Vec[0] = M_PI*sigma*sigma*xi*xi*tgamma(1.+1/alpha);
	}

	const double tol = 1E-7;
	int depth = 4;
	int gauss_kronrod_Depth = 3;
	if(xi<100)
	{
		gauss_kronrod_Depth = 5;
	}
	double n = 2;
	double shift = M_PI*(2*n+0.25);

	/// with parallelization
	int counter = 1;
	for(int sec=0; sec<num_Sections; sec++)
	{
		double nu = starts[sec];
		for(int i=0; i<interpoints[sec]; i++)
		{
			nu += dnu[sec];
			interpoints_Sum_Argum_Vec[counter] = nu;
			counter++;
		}
	}
	Global_Variables::parallel_For(common_Size, reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		Q_UNUSED(thread_Index)
		ooura_fourier_cos<double> integrator_Cos(tol, depth);
		ooura_fourier_sin<double> integrator_Sin(tol, depth);

		double nu = 0;
		double error;
		for(int i = 1+n_Min; i<1+n_Max; i++)
		{
			nu = interpoints_Sum_Argum_Vec[i];
			if(abs(1-alpha)>DBL_EPSILON/* && abs(0.5-alpha)>DBL_EPSILON*/) // integrate even for alpha == 0.5
			{
				double integral = 0;
				double division_Point = shift/nu;
				// first part
				auto f_1 = [&](double r)
				{
					return exp(-pow(r/xi,2*alpha)) * cyl_bessel_j(0, nu*r) * r;
				};
				integral = 2*M_PI*gauss_kronrod<double, 61>::integrate(f_1, 0, division_Point, gauss_kronrod_Depth, 1e-7, &error);

				// second part
				auto f_2_cos = [&](double r)
				{
					double r_Sh = r + shift/nu;
					double r_Sh_W = nu*r + shift;
					double cos_Val = Global_Variables::val_Cos_Expansion(r_Sh_W);
					return exp(-pow(r_Sh/xi,2*alpha)) * cos_Val * sqrt(r_Sh/nu);
				};
				auto f_2_sin = [&](double r)
				{
					double r_Sh = r + shift/nu;
					double r_Sh_W = nu*r + shift;
					double sin_Val = Global_Variables::val_Sin_Expansion(r_Sh_W);
					return exp(-pow(r_Sh/xi,2*alpha)) * sin_Val * sqrt(r_Sh/nu);
				};
				std::pair<double, double> cos_Integral = integrator_Cos.integrate(f_2_cos, nu);
				integral += sqrt(8*M_PI)*cos_Integral.first;
				std::pair<double, double> sin_Integral = integrator_Sin.integrate(f_2_sin, nu);
				integral += sqrt(8*M_PI)*sin_Integral.first;

				interpoints_Sum_Value_Vec[i] = sigma*sigma*integral;
			}
		}
	});

	/// without parallelization

//	ooura_fourier_cos<double> integrator_Cos(tol, depth);
//	ooura_fourier_sin<double> integrator_Sin(tol, depth);

//	double nu = 0, error;
//	int counter = 1;
//	for(int sec=0; sec<num_Sections; sec++)
//	{
//		for(int i=0; i<interpoints[sec]; i++)
//		{
//			nu += dnu[sec];

//			interpoints_Sum_Argum_Vec[counter] = nu;

//			if(abs(1-alpha)>DBL_EPSILON/* && abs(0.5-alpha)>DBL_EPSILON*/) // integrate even for alpha == 0.5
//			{
//				double integral = 0;
//				double division_Point = shift/nu;
//				// first part
//				auto f_1 = [&](double r)
//				{
//					return exp(-pow(r/xi,2*alpha)) * cyl_bessel_j(0, nu*r) * r;
//				};
//				integral = 2*M_PI*gauss_kronrod<double, 61>::integrate(f_1, 0, division_Point, 5, 1e-7, &error);

//				// second part
//				auto f_2_cos = [&](double r)
//				{
//					double r_Sh = r + shift/nu;
//					double r_Sh_W = nu*r + shift;
//					double cos_Val = Global_Variables::val_Cos_Expansion(r_Sh_W);
//					return exp(-pow(r_Sh/xi,2*alpha)) * cos_Val * sqrt(r_Sh/nu);
//				};
//				auto f_2_sin = [&](double r)
//				{
//					double r_Sh = r + shift/nu;
//					double r_Sh_W = nu*r + shift;
//					double sin_Val = Global_Variables::val_Sin_Expansion(r_Sh_W);
//					return exp(-pow(r_Sh/xi,2*alpha)) * sin_Val * sqrt(r_Sh/nu);
//				};
//				std::pair<double, double> cos_Integral = integrator_Cos.integrate(f_2_cos, nu);
//				integral += sqrt(8*M_PI)*cos_Integral.first;
//				std::pair<double, double> sin_Integral = integrator_Sin.integrate(f_2_sin, nu);
//				integral += sqrt(8*M_PI)*sin_Integral.first;

//				interpoints_Sum_Value_Vec[counter] = sigma*sigma*integral;
//			}
//			counter++;
//		}
//	}
	interpoints_Sum_Argum_Vec.push_back(1.1*nu_Max);
	interpoints_Sum_Value_Vec.push_back(0);
	interpoints_Sum_Argum_Vec.push_back(DBL_MAX);
	interpoints_Sum_Value_Vec.push_back(0);

	acc_PSD_FG_2D = gsl_interp_accel_alloc();
	if(nu_Max<10*addition) 	spline_PSD_FG_2D = gsl_spline_alloc(gsl_interp_linear, interpoints_Sum_Value_Vec.size());
	else					spline_PSD_FG_2D = gsl_spline_alloc(gsl_interp_steffen,interpoints_Sum_Value_Vec.size());

	gsl_spline_init(spline_PSD_FG_2D, interpoints_Sum_Argum_Vec.data(), interpoints_Sum_Value_Vec.data(), interpoints_Sum_Value_Vec.size());
}
void Node::clear_Spline_PSD_Fractal_Gauss_2D(const Imperfections_Model& imperfections_Model)
{
//	if(imperfections_Model.approximation != PT_approximation) return;
	if(imperfections_Model.PSD_Model != fractal_Gauss_Model) return;
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;

	gsl_spline_free(spline_PSD_FG_2D);
	gsl_interp_accel_free(acc_PSD_FG_2D);
}

void Node::create_Spline_PSD_Measured(const Imperfections_Model& imperfections_Model, QString PSD_Type)
{
//	if(imperfections_Model.approximation != PT_approximation) return;
	if(imperfections_Model.PSD_Model != measured_PSD) return;
	if(struct_Data.item_Type != item_Type_Substrate ) return;

	// in other cases ( substrate ) go further

	PSD_Data psd_Data   = PSD_Type == PSD_Type_1D ? imperfections_Model.PSD_1D : imperfections_Model.PSD_2D;
	double sigma_Factor = PSD_Type == PSD_Type_1D ? struct_Data.roughness_Model.sigma_Factor_PSD_1D.value : struct_Data.roughness_Model.sigma_Factor_PSD_2D.value;

	QVector<double> argument_Vec = psd_Data.argument;
	QVector<double> value_Vec = psd_Data.value;


	// if has data to spline
	if(psd_Data.argument.size()>=2)
	{
		// sigma-scaling is here
		for(int i=0; i<argument_Vec.size(); i++)
		{
			value_Vec[i] = psd_Data.value[i] * sigma_Factor*sigma_Factor;
		}

		// additional zeros at low frequencies
		if(psd_Data.argument.front()>0)
		{
			argument_Vec.prepend(psd_Data.argument.front()*(1.-DBL_EPSILON));
			argument_Vec.prepend(0);

			value_Vec.prepend(0);
			value_Vec.prepend(0);
		}
		// additional zeros at high frequencies
		if(psd_Data.argument.front()<DBL_MAX)
		{
			argument_Vec.append(psd_Data.argument.back()*(1.+DBL_EPSILON));
			argument_Vec.append(DBL_MAX);

			value_Vec.append(0);
			value_Vec.append(0);
		}
	} else
	// zero spline
	{
		argument_Vec.clear();
		argument_Vec = {0,DBL_MAX};
		value_Vec.clear();
		value_Vec = {0,0};
	}

	// linear interpolation
	acc_PSD_Meas = gsl_interp_accel_alloc();
	spline_PSD_Meas = gsl_spline_alloc(gsl_interp_linear, argument_Vec.size());
	gsl_spline_init(spline_PSD_Meas, argument_Vec.data(), value_Vec.data(), argument_Vec.size());
}
void Node::clear_Spline_PSD_Measured(const Imperfections_Model& imperfections_Model)
{
//	if(imperfections_Model.approximation != PT_approximation) return;
	if(imperfections_Model.PSD_Model != measured_PSD) return;
	if(struct_Data.item_Type != item_Type_Substrate ) return;

	gsl_spline_free(spline_PSD_Meas);
	gsl_interp_accel_free(acc_PSD_Meas);
}

struct Frequency_Params
{
	double* pa;
	double peak_Frequency;
	double peak_Width;
};
double func(double nu, void* par)
{
	Frequency_Params* params = reinterpret_cast<Frequency_Params*>(par);
	double& p = *(params->pa);
	double& peak_Frequency = params->peak_Frequency;
	double& peak_Width = params->peak_Width;

	return exp(-pow((nu-peak_Frequency)/peak_Width,2)) * nu / sqrt(nu*nu - p*p);
}

void Node::create_Spline_PSD_Peak(const Imperfections_Model& imperfections_Model)
{
	if(!imperfections_Model.add_Gauss_Peak) return;
	if(struct_Data.item_Type == item_Type_Ambient) return;
	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;

	double peak_Range_Factor = 5;
	double peak_Frequency = struct_Data.roughness_Model.peak_Frequency.value;
	double peak_Width = struct_Data.roughness_Model.peak_Frequency_Width.value;

	double p_Max = min(peak_Frequency + peak_Width*peak_Range_Factor,nu_Limit);
	double p_Min_Integration = max(peak_Frequency - peak_Width*peak_Range_Factor, 0.);
		   p_Min_Integration = min(p_Min_Integration, nu_Limit);
	int common_Size = 200;

	QVector<double> interpoints_Argum_Vec(common_Size);
	QVector<double> interpoints_Value_Vec(common_Size);
	double dp = p_Max/(common_Size-1);


	// integrator
	double p = 0;
	if(peak_Frequency>DBL_EPSILON)
	{	
		/// with parallelization (use if many points)

		// intermediate points
		for(int i=0; i<common_Size; i++)
		{
			interpoints_Argum_Vec[i] = p;
			p += dp;
		}
		Global_Variables::parallel_For(common_Size, max(3,QThread::idealThreadCount())/*reflectivity_calc_threads*/, [&](int n_Min, int n_Max, int thread_Index)
		{
			Q_UNUSED(thread_Index)

			double result, abserr;
			double epsabs = 0;
			double epsrel = 1E-6;
			size_t limit = 1000;
			gsl_integration_workspace* w = gsl_integration_workspace_alloc(limit);

			double p_Thread = 0;
			Frequency_Params frequency_Params;
			frequency_Params.pa = &p_Thread;
			frequency_Params.peak_Frequency = peak_Frequency;
			frequency_Params.peak_Width = peak_Width;

			gsl_function F;
			F.function = &func;
			F.params = &frequency_Params;

			size_t npts = 2;
			vector<double> pts = {p_Thread, p_Max*(1.+1E-5)};

			for(int i=n_Min; i<n_Max; ++i)
			{
				p_Thread = interpoints_Argum_Vec[i];
				if(p_Min_Integration < interpoints_Argum_Vec[i])
				{
					pts[0] = interpoints_Argum_Vec[i];
				} else
				{
					pts[0] = p_Min_Integration;
				}
				gsl_integration_qagp(&F, pts.data(), npts, epsabs, epsrel, limit, w, &result, &abserr);
				interpoints_Value_Vec[i] = 4*struct_Data.PSD_Gauss_Peak_2D_Factor * result;
			}
			gsl_integration_workspace_free (w);
		});
		// first point
		interpoints_Value_Vec[0] = 4*struct_Data.PSD_Gauss_Peak_2D_Factor * 0.5*peak_Width*SQRT_PI * (1+erf(peak_Frequency/peak_Width));

		/// without parallelization

//		double result, abserr;
//		double epsabs = 0;
//		double epsrel = 1E-7;
//		size_t limit = 1000;
//		gsl_integration_workspace* w = gsl_integration_workspace_alloc(limit);

//		Frequency_Params frequency_Params;
//		frequency_Params.pa = &p;
//		frequency_Params.peak_Frequency = peak_Frequency;
//		frequency_Params.peak_Width = peak_Width;

//		gsl_function F;
//		F.function = &func;
//		F.params = &frequency_Params;

//		size_t npts = 2;
//		vector<double> pts = {p, p_Max*(1.+1E-5)};

//		// first point
//		interpoints_Argum_Vec[0] = 0;
//		interpoints_Value_Vec[0] = 4*struct_Data.PSD_Gauss_Peak_2D_Factor * 0.5*peak_Width*SQRT_PI * (1+erf(peak_Frequency/peak_Width));

//		// intermediate points
//		for(int i=1; i<common_Size; i++)
//		{
//			p += dp;
//			interpoints_Argum_Vec[i] = p;

//			pts[0] = p;
//			gsl_integration_qagp(&F, pts.data(), npts, epsabs, epsrel, limit, w, &result, &abserr);
//			interpoints_Value_Vec[i] = 4*struct_Data.PSD_Gauss_Peak_2D_Factor * result;
//		}
//		gsl_integration_workspace_free (w);
	} else
	// usual gauss
	{
		// first and intermediate points
		for(int i=0; i<common_Size; i++)
		{
			interpoints_Argum_Vec[i] = p;
			interpoints_Value_Vec[i] = 4*struct_Data.PSD_Gauss_Peak_2D_Factor * 0.5*peak_Width*SQRT_PI * exp(-pow(p/peak_Width,2));
			p += dp;
		}
	}
	// last 2 points
	interpoints_Argum_Vec.append(1.1*p_Max);
	interpoints_Value_Vec.append(0);
	interpoints_Argum_Vec.append(MAX_DOUBLE);
	interpoints_Value_Vec.append(0);

	acc_PSD_Peak = gsl_interp_accel_alloc();
	spline_PSD_Peak = gsl_spline_alloc(gsl_interp_steffen, interpoints_Value_Vec.size());
	gsl_spline_init(spline_PSD_Peak, interpoints_Argum_Vec.data(), interpoints_Value_Vec.data(), interpoints_Value_Vec.size());
}

void Node::clear_Spline_PSD_Peak(const Imperfections_Model& imperfections_Model)
{
	if(!imperfections_Model.add_Gauss_Peak) return;
	if(struct_Data.item_Type == item_Type_Ambient ) return;
	if(struct_Data.item_Type == item_Type_Layer && imperfections_Model.use_Common_Roughness_Function) return;

	gsl_spline_free(spline_PSD_Peak);
	gsl_interp_accel_free(acc_PSD_Peak);
}

void Node::create_Spline_PSD_Linear_Growth_Top_2D(const Imperfections_Model& imperfections_Model, const Data& measurement, const vector<Data*>& media_Data_Map_Vector)
{
	// used for DW sigmas
	if(imperfections_Model.vertical_Correlation != partial_Correlation) return;
	if(imperfections_Model.inheritance_Model != linear_Growth_Alpha_Inheritance_Model &&
	   imperfections_Model.inheritance_Model != linear_Growth_n_1_4_Inheritance_Model )	return;
	if(struct_Data.item_Type != item_Type_Substrate ) return;

	// choosing base PSD functions
	double (*PSD_1D_Func_from_nu)(double, double, double, double, gsl_spline*, gsl_interp_accel*);
	double (*PSD_2D_Func_from_nu)(double, double, double, double, gsl_spline*, gsl_interp_accel*);
	double factor_1D = 1;
	double factor_2D = 1;

	if(imperfections_Model.PSD_Model == ABC_Model)	{
		PSD_1D_Func_from_nu = &Global_Variables::PSD_ABC_1D_from_nu;
		PSD_2D_Func_from_nu = &Global_Variables::PSD_ABC_2D_from_nu;
		factor_1D = struct_Data.PSD_ABC_1D_Factor;
		factor_2D = struct_Data.PSD_ABC_2D_Factor;
	}
	if(imperfections_Model.PSD_Model == fractal_Gauss_Model)	{
		const double& alpha = struct_Data.roughness_Model.fractal_alpha.value;

		if(abs(alpha-1)>DBL_EPSILON) {
			PSD_1D_Func_from_nu = &Global_Variables::PSD_Fractal_Gauss_1D_from_nu;
			PSD_2D_Func_from_nu = &Global_Variables::PSD_Fractal_Gauss_2D_from_nu;
			factor_1D = 1;
			factor_2D = 1;
		} else {
			PSD_1D_Func_from_nu = &Global_Variables::PSD_Real_Gauss_1D_from_nu;
			PSD_2D_Func_from_nu = &Global_Variables::PSD_Real_Gauss_2D_from_nu;
			factor_1D = struct_Data.PSD_Real_Gauss_1D_Factor;
			factor_2D = struct_Data.PSD_Real_Gauss_2D_Factor;
		}
	}
	if(imperfections_Model.PSD_Model == measured_PSD)	{
		// TODO remove!
	}

	// choosing PSD gauss peak function
	double (*PSD_2D_Peak_Func_from_nu)(double, double, double, double);
	double peak_Factor_2D = struct_Data.PSD_Gauss_Peak_2D_Factor;

	if(imperfections_Model.add_Gauss_Peak && struct_Data.roughness_Model.peak_Sigma.value>DBL_EPSILON)	{
		PSD_2D_Peak_Func_from_nu = &Global_Variables::PSD_Gauss_Peak_2D_from_nu;
	} else	{
		PSD_2D_Peak_Func_from_nu = &Global_Variables::zero_PSD_2D_from_nu;
	}

	// fill nu points for splining
	int num_Sections = 9; // plus zero point, plus max point, plus infinite point
	int points_Per_Section = 60;
	vector<int> interpoints(num_Sections);
	int common_Size = 3;
	{
		interpoints[0] = 100;
		common_Size+=interpoints[0];
	}
	for(int i=1; i<num_Sections; i++)
	{
		interpoints[i] = points_Per_Section;
		common_Size+=interpoints[i];
	}
	double nu_Max = nu_Limit*10;

	vector<double> starts(num_Sections); // open start
	starts[0] = 0;
	starts[1] = nu_Max/1000000;
	starts[2] = nu_Max/200000;
	starts[3] = nu_Max/50000;
	starts[4] = nu_Max/10000;
	starts[5] = nu_Max/1000;
	starts[6] = nu_Max/100;
	starts[7] = nu_Max/10;
	starts[8] = nu_Max/2;

	vector<double> dnu(num_Sections);
	for(int i=0; i<num_Sections-1; i++)
	{
		dnu[i] = (starts[i+1] - starts[i])/interpoints[i];
	}
	dnu.back() = (nu_Max - starts.back())/interpoints.back();

	// filling
	int counter = 1;
	vector<double> nu_Vec(common_Size);
	nu_Vec[0] = 0;
	for(int sec=0; sec<num_Sections; sec++)
	{
		double nu = starts[sec];
		for(int i=0; i<interpoints[sec]; i++)
		{
			nu += dnu[sec];
			nu_Vec[counter] = nu;
			counter++;
		}
	}
	nu_Vec[common_Size-3] = nu_Max;
	nu_Vec[common_Size-2] = 1.1*nu_Max;
	nu_Vec[common_Size-1] = DBL_MAX;

	// calculating values
	const double& xi =					struct_Data.roughness_Model.cor_radius.value;
	const double& alpha =				struct_Data.roughness_Model.fractal_alpha.value;
	const double& peak_Frequency =		struct_Data.roughness_Model.peak_Frequency.value;
	const double& peak_Frequency_Width =struct_Data.roughness_Model.peak_Frequency_Width.value;

	/// 2D spline
	vector<double> PSD_2D_Values_Vec(common_Size);
	vector<double> growth_PSD_Vec(common_Size);
	vector<double> inheritance_Vec(common_Size);

	Global_Variables::parallel_For(common_Size-2, reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		Q_UNUSED(thread_Index)
		for(int i=n_Min; i<n_Max; ++i)
		{
			double nu = nu_Vec[i];
			double nu2 = nu*nu;
			double nu3 = nu2*nu;
			double nu4 = nu2*nu2;

			double nu_nu_0 = nu/imperfections_Model.vertical_Inheritance_Frequency;

			growth_PSD_Vec[i] = 0;
			inheritance_Vec[i] = 1;
			PSD_2D_Values_Vec[i] = PSD_2D_Func_from_nu(factor_2D, xi, alpha, nu, spline_PSD_FG_2D, acc_PSD_FG_2D)/* +
								   PSD_2D_Peak_Func_from_nu(peak_Factor_2D, peak_Frequency, peak_Frequency_Width, nu)*/;

			for(int bound_Index = media_Data_Map_Vector.size()-2; bound_Index>=1; bound_Index--)
			{
				Data* struct_Data = media_Data_Map_Vector[bound_Index];

				double inheritance_Exp = 1;
				double growth_PSD = 0;

				double thickness = struct_Data->thickness.value;
				double omega = struct_Data->roughness_Model.omega.value;
				double mu = struct_Data->roughness_Model.mu.value;
				double alpha = struct_Data->roughness_Model.fractal_alpha.value;

				double a1 = struct_Data->roughness_Model.a1.value;
				double a2 = struct_Data->roughness_Model.a2.value;
				double a3 = struct_Data->roughness_Model.a3.value;
				double a4 = struct_Data->roughness_Model.a4.value;

				double b_Function = 0;
				if(imperfections_Model.inheritance_Model == linear_Growth_Alpha_Inheritance_Model)	{
					b_Function = pow(nu_nu_0,2*alpha+2)/mu;
				}
				if(imperfections_Model.inheritance_Model == linear_Growth_n_1_4_Inheritance_Model)	{
					b_Function = nu*a1 + nu2*a2 + nu3*a3 + nu4*a4;
				}

				if(b_Function*thickness>DBL_EPSILON) {
					inheritance_Exp = exp(-b_Function*thickness);
					growth_PSD = omega*(1.-inheritance_Exp)/b_Function;
				} else
				{
					inheritance_Exp = 1;
					growth_PSD = omega*thickness;
				}

				// PSD evolution
				inheritance_Vec[i]   = inheritance_Vec[i]*inheritance_Exp;
				growth_PSD_Vec[i]    = growth_PSD_Vec   [i]*inheritance_Exp + growth_PSD; // pure growth
				PSD_2D_Values_Vec[i] = PSD_2D_Values_Vec[i]*inheritance_Exp + growth_PSD; // summary growth
			}
		}
	});
	growth_PSD_Vec[common_Size-2] = 0;
	growth_PSD_Vec[common_Size-1] = 0;
	PSD_2D_Values_Vec[common_Size-2] = 0;
	PSD_2D_Values_Vec[common_Size-1] = 0;

	// splining
	acc_PSD_Linear_Growth_Top_2D = gsl_interp_accel_alloc();
	spline_PSD_Linear_Growth_Top_2D = gsl_spline_alloc(gsl_interp_steffen, PSD_2D_Values_Vec.size());
	gsl_spline_init(spline_PSD_Linear_Growth_Top_2D, nu_Vec.data(), PSD_2D_Values_Vec.data(), PSD_2D_Values_Vec.size());

	// pure growth
	spline_PSD_Linear_Pure_Growth_Top_2D = gsl_spline_alloc(gsl_interp_steffen, growth_PSD_Vec.size());
	acc_PSD_Linear_Pure_Growth_Top_2D = gsl_interp_accel_alloc();
	gsl_spline_init(spline_PSD_Linear_Pure_Growth_Top_2D, nu_Vec.data(), growth_PSD_Vec.data(), growth_PSD_Vec.size());

	// inheritance
	spline_PSD_Linear_Inheritance_Top_2D = gsl_spline_alloc(gsl_interp_steffen, inheritance_Vec.size());
	acc_PSD_Linear_Inheritance_Top_2D = gsl_interp_accel_alloc();
	gsl_spline_init(spline_PSD_Linear_Inheritance_Top_2D, nu_Vec.data(), inheritance_Vec.data(), inheritance_Vec.size());

}
void Node::clear_Spline_PSD_Linear_Growth_Top_2D(const Imperfections_Model& imperfections_Model, const Data& measurement)
{
	if(imperfections_Model.vertical_Correlation != partial_Correlation) return;
	if(imperfections_Model.inheritance_Model != linear_Growth_Alpha_Inheritance_Model &&
	   imperfections_Model.inheritance_Model != linear_Growth_n_1_4_Inheritance_Model )	return;
	if(struct_Data.item_Type != item_Type_Substrate ) return;

	gsl_spline_free(spline_PSD_Linear_Growth_Top_2D);
	gsl_interp_accel_free(acc_PSD_Linear_Growth_Top_2D);

	gsl_spline_free(spline_PSD_Linear_Pure_Growth_Top_2D);
	gsl_interp_accel_free(acc_PSD_Linear_Pure_Growth_Top_2D);

	gsl_spline_free(spline_PSD_Linear_Inheritance_Top_2D);
	gsl_interp_accel_free(acc_PSD_Linear_Inheritance_Top_2D);
}

void Node::create_Spline_PSD_Linear_Growth_Top_1D(const Imperfections_Model &imperfections_Model, const Data &measurement, const vector<Data *> &media_Data_Map_Vector)
{
	// used for DW sigmas
	if(imperfections_Model.vertical_Correlation != partial_Correlation) return;
	if(imperfections_Model.inheritance_Model != linear_Growth_Alpha_Inheritance_Model &&
	   imperfections_Model.inheritance_Model != linear_Growth_n_1_4_Inheritance_Model )	return;
	if(struct_Data.item_Type != item_Type_Substrate ) return;

	// choosing base PSD functions
	double (*PSD_1D_Func_from_nu)(double, double, double, double, gsl_spline*, gsl_interp_accel*);
	double factor_1D = 1;

	if(imperfections_Model.PSD_Model == ABC_Model)	{
		PSD_1D_Func_from_nu = &Global_Variables::PSD_ABC_1D_from_nu;
		factor_1D = struct_Data.PSD_ABC_1D_Factor;
	}
	if(imperfections_Model.PSD_Model == fractal_Gauss_Model)	{
		const double& alpha = struct_Data.roughness_Model.fractal_alpha.value;

		if(abs(alpha-1)>DBL_EPSILON) {
			PSD_1D_Func_from_nu = &Global_Variables::PSD_Fractal_Gauss_1D_from_nu;
			factor_1D = 1;
		} else {
			PSD_1D_Func_from_nu = &Global_Variables::PSD_Real_Gauss_1D_from_nu;
			factor_1D = struct_Data.PSD_Real_Gauss_1D_Factor;
		}
	}
	if(imperfections_Model.PSD_Model == measured_PSD)	{
		// TODO remove!
	}

	// choosing PSD gauss peak function
	double (*PSD_1D_Peak_Func_from_nu)(double, double, double, double, gsl_spline*, gsl_interp_accel*);
	double (*PSD_2D_Peak_Func_from_nu)(double, double, double, double);
	double peak_Factor_1D = 1;
	double peak_Factor_2D = struct_Data.PSD_Gauss_Peak_2D_Factor;

	if(imperfections_Model.add_Gauss_Peak && struct_Data.roughness_Model.peak_Sigma.value>DBL_EPSILON)	{
		PSD_1D_Peak_Func_from_nu = &Global_Variables::PSD_Gauss_Peak_1D_from_nu;
		PSD_2D_Peak_Func_from_nu = &Global_Variables::PSD_Gauss_Peak_2D_from_nu;
	} else	{
		PSD_1D_Peak_Func_from_nu = &Global_Variables::zero_PSD_1D_from_nu;
		PSD_2D_Peak_Func_from_nu = &Global_Variables::zero_PSD_2D_from_nu;
	}

	// fill nu points for splining
	int num_Sections = 9; // plus zero point, plus max point, plus infinite point
	int points_Per_Section = 30;
	vector<int> interpoints(num_Sections);
	int common_Size = 3;
	{
		interpoints[0] = 50;
		common_Size+=interpoints[0];
	}
	for(int i=1; i<num_Sections; i++)
	{
		interpoints[i] = points_Per_Section;
		common_Size+=interpoints[i];
	}
	double nu_Max = nu_Limit;

	vector<double> starts(num_Sections); // open start
	starts[0] = 0;
	starts[1] = nu_Max/500000;
	starts[2] = nu_Max/20000;
	starts[3] = nu_Max/5000;
	starts[4] = nu_Max/2000;
	starts[5] = nu_Max/500;
	starts[6] = nu_Max/20;
	starts[7] = nu_Max/5;
	starts[8] = nu_Max/2;

	vector<double> dnu(num_Sections);
	for(int i=0; i<num_Sections-1; i++)
	{
		dnu[i] = (starts[i+1] - starts[i])/interpoints[i];
	}
	dnu.back() = (nu_Max - starts.back())/interpoints.back();

	// filling
	int counter = 1;
	vector<double> nu_Vec(common_Size);
	nu_Vec[0] = 0;
	for(int sec=0; sec<num_Sections; sec++)
	{
		double nu = starts[sec];
		for(int i=0; i<interpoints[sec]; i++)
		{
			nu += dnu[sec];
			nu_Vec[counter] = nu;
			counter++;
		}
	}
	nu_Vec[common_Size-3] = nu_Max;
	nu_Vec[common_Size-2] = 1.1*nu_Max;
	nu_Vec[common_Size-1] = DBL_MAX;

	// calculating values
	const double& xi =					struct_Data.roughness_Model.cor_radius.value;
	const double& alpha =				struct_Data.roughness_Model.fractal_alpha.value;
	const double& peak_Frequency =		struct_Data.roughness_Model.peak_Frequency.value;
	const double& peak_Frequency_Width =struct_Data.roughness_Model.peak_Frequency_Width.value;

	/// 1D spline

	double nu_a = 1E-6; // empirical

	vector<double> PSD_1D_Values_Vec(common_Size);
//	auto start = std::chrono::system_clock::now();
	Global_Variables::parallel_For(common_Size-2, reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		Q_UNUSED(thread_Index)
		double p = 0;
		double dp = 0;
		tanh_sinh<double> integrator;
		double integrator_tolerance = 1E-4;
		auto f = [&](double nu)		{
			return 4 * gsl_spline_eval(spline_PSD_Linear_Growth_Top_2D, nu, acc_PSD_Linear_Growth_Top_2D) * nu / sqrt(nu*nu - p*p);
		};
		auto integral_p_nu = [&](double nu)		{
			return 4 * gsl_spline_eval(spline_PSD_Linear_Growth_Top_2D, p, acc_PSD_Linear_Growth_Top_2D) * sqrt(2*p*(nu-p));
		};
		auto integral_p_dp = [&]()		{
			return 4 * gsl_spline_eval(spline_PSD_Linear_Growth_Top_2D, p, acc_PSD_Linear_Growth_Top_2D) * sqrt(2*p*dp);
		};
		auto f_Pure = [&](double nu)		{
			return 4 * gsl_spline_eval(spline_PSD_Linear_Pure_Growth_Top_2D, nu, acc_PSD_Linear_Pure_Growth_Top_2D) * nu / sqrt(nu*nu - p*p);
		};

//		auto f_Peak_2D = [&](double nu)		{
//			return 4 * PSD_2D_Peak_Func_from_nu(peak_Factor_2D, peak_Frequency, peak_Frequency_Width, nu)
//					 * gsl_spline_eval(spline_PSD_Linear_Inheritance_Top_2D, nu, acc_PSD_Linear_Inheritance_Top_2D) * nu / sqrt(nu*nu - p*p);
//		};

		for(int i=n_Min; i<n_Max; ++i)
		{
			double result = 0;
			p = nu_Vec[i];
//			p = media_Data_Map_Vector[media_Data_Map_Vector.size()-1]->relative_Density.value;
			dp = p*1E-6;

			double pdp = p+dp;
			if(p<nu_a)
			{
				result =
						 PSD_1D_Func_from_nu(factor_1D, xi, alpha, p, spline_PSD_FG_1D, acc_PSD_FG_1D) +
						 PSD_1D_Peak_Func_from_nu(1, peak_Frequency, peak_Frequency_Width, p, spline_PSD_Peak, acc_PSD_Peak) +
						 integrator.integrate(f_Pure, nu_a, nu_Max, integrator_tolerance);
			} else
			if(p<nu_Max)
			{
				if(pdp<nu_Max)
				{
					result  = integral_p_dp();
					result += integrator.integrate(f, pdp, nu_Max, integrator_tolerance);
				} else
				{
					result  = integral_p_nu(nu_Max);
				}
			} else
			{
				result = 0;
			}

			PSD_1D_Values_Vec[i] = result;

//			qInfo()
//					<< result
//					<< PSD_1D_Func_from_nu(factor_1D, xi, alpha, p, spline_PSD_FG_1D, acc_PSD_FG_1D)
//					<< endl;
		}
	});
	PSD_1D_Values_Vec[common_Size-2] = 0;
	PSD_1D_Values_Vec[common_Size-1] = 0;

	acc_PSD_Linear_Growth_Top_1D = gsl_interp_accel_alloc();
	spline_PSD_Linear_Growth_Top_1D = gsl_spline_alloc(gsl_interp_steffen, PSD_1D_Values_Vec.size());
	gsl_spline_init(spline_PSD_Linear_Growth_Top_1D, nu_Vec.data(), PSD_1D_Values_Vec.data(), PSD_1D_Values_Vec.size());

//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//	qInfo() << "\nintegrate :   "<< elapsed.count()/1000000. << " seconds" << endl;
}
void Node::clear_Spline_PSD_Linear_Growth_Top_1D(const Imperfections_Model &imperfections_Model, const Data &measurement)
{
	if(imperfections_Model.vertical_Correlation != partial_Correlation) return;
	if(imperfections_Model.inheritance_Model != linear_Growth_Alpha_Inheritance_Model &&
	   imperfections_Model.inheritance_Model != linear_Growth_n_1_4_Inheritance_Model )	return;
	if(struct_Data.item_Type != item_Type_Substrate ) return;

	if(measurement.measurement_Type != measurement_Types[Specular_Scan]) return;

	gsl_spline_free(spline_PSD_Linear_Growth_Top_1D);
	gsl_interp_accel_free(acc_PSD_Linear_Growth_Top_1D);
}

double Node::G1_Type_Outer()
{
	double a = 1;
	if(struct_Data.fluctuations_Model.particle_Interference_Function == radial_Paracrystal)
	{
		a = struct_Data.fluctuations_Model.particle_Radial_Distance.value;
	}
	if(struct_Data.fluctuations_Model.particle_Interference_Function == disorder)
	{
		a = struct_Data.fluctuations_Model.particle_Average_Distance.value;
	}

	if(struct_Data.fluctuations_Model.geometric_Model == square_Model)
	{
		return Global_Variables::G1_Square(a);
	}
	if(struct_Data.fluctuations_Model.geometric_Model == hexagonal_Model )
	{
		return Global_Variables::G1_Hexagone(a);
	}
	if(struct_Data.fluctuations_Model.geometric_Model == pure_Radial_Model )
	{
		return Global_Variables::G1_Pure_Radial(a);
	}
	qInfo() << endl << "Node::G1_Type_Outer  :  wrong fluctuations_Model.geometric_Model" << endl << endl;
	return -2020;
}

double Node::G2_Type_Outer(double q)
{
	return gsl_spline_eval(spline_G2, q, acc_G2);
}

void Node::create_Spline_G2_2D(const Imperfections_Model& imperfections_Model, const Data& measurement)
{
	if(!imperfections_Model.use_Fluctuations) return;
	if(!struct_Data.fluctuations_Model.is_Used) return;
	if(struct_Data.fluctuations_Model.particle_Interference_Function != radial_Paracrystal) return;
	if(struct_Data.item_Type != item_Type_Layer ) return;
	if(measurement.measurement_Type != measurement_Types[GISAS]) return;


	// in other cases ( layer with radial paracrystal ) go further

	bool negative = false;
	double q_Max;
	for(size_t i=0; i<measurement.detector_Theta_Cos_Vec.size(); i++)
	{
		if(measurement.detector_Theta_Cos_Vec[i]<0)
		{
			negative = true;
			break;
		}
	}
	if(negative)
	{
		vector<double> temp_q2(measurement.detector_Theta_Cos_Vec.size()*measurement.detector_Phi_Cos_Vec.size());

		for(size_t theta_Index=0; theta_Index<measurement.detector_Theta_Cos_Vec.size(); theta_Index++)
		{
			for(size_t phi_Index=0; phi_Index<measurement.detector_Phi_Cos_Vec.size(); phi_Index++)
			{
				temp_q2[theta_Index*measurement.detector_Phi_Cos_Vec.size()+phi_Index] =
									measurement.k_Value*measurement.k_Value*( measurement.detector_Theta_Cos_Vec[theta_Index]*measurement.detector_Theta_Cos_Vec[theta_Index] +
									measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value -
								  2*measurement.beam_Theta_0_Cos_Value*measurement.detector_Theta_Cos_Vec[theta_Index]*measurement.detector_Phi_Cos_Vec[phi_Index]);
			}
		}
		std::sort(temp_q2.begin(), temp_q2.end());
		q_Max = sqrt(max(temp_q2.back(),DBL_EPSILON))*(1+1E-5);
	} else
	{
		vector<double> temp_q2_Max(measurement.detector_Theta_Cos_Vec.size());
		double min_Cos_Phi_0 = min(measurement.detector_Phi_Cos_Vec.front(), measurement.detector_Phi_Cos_Vec.back());
		double max_Cos_Phi_0 = max(measurement.detector_Phi_Cos_Vec.front(), measurement.detector_Phi_Cos_Vec.back());
		double min_Cos_Phi = min(min_Cos_Phi_0,max_Cos_Phi_0);
		vector<double> detector_Theta_Cos_Sorted_Vec = measurement.detector_Theta_Cos_Vec;
		std::sort(detector_Theta_Cos_Sorted_Vec.begin(), detector_Theta_Cos_Sorted_Vec.end());
		for(size_t i=0; i<detector_Theta_Cos_Sorted_Vec.size(); i++)
		{
			temp_q2_Max[i] = measurement.k_Value*measurement.k_Value*( detector_Theta_Cos_Sorted_Vec[i]*detector_Theta_Cos_Sorted_Vec[i] +
																	   measurement.beam_Theta_0_Cos_Value*measurement.beam_Theta_0_Cos_Value -
																	 2*measurement.beam_Theta_0_Cos_Value*detector_Theta_Cos_Sorted_Vec[i]*min_Cos_Phi);
		}
		std::sort(temp_q2_Max.begin(), temp_q2_Max.end());
		q_Max = sqrt(max(temp_q2_Max.back(),DBL_EPSILON))*(1+1E-5);
	}
	double q_Min = 0;
	double q_Range = q_Max-q_Min;
	vector<double> q_Peak;

	// choose pattern
	double phi_Max;
	double a = struct_Data.fluctuations_Model.particle_Radial_Distance.value;
	double b = a;
	double sigma = struct_Data.fluctuations_Model.particle_Radial_Distance_Deviation.value;
	G2_Type.resize(reflectivity_calc_threads);
	G2_Type_q_Zero.resize(reflectivity_calc_threads);
	G2_Type_long.resize(reflectivity_calc_threads);
	if(struct_Data.fluctuations_Model.geometric_Model == square_Model)
	{
		G1_Type = Global_Variables::G1_Square;
		for(int thread_Index = 0; thread_Index<reflectivity_calc_threads; thread_Index++)
		{
			G2_Type			[thread_Index] = Global_Variables::G2_Square;
			G2_Type_q_Zero	[thread_Index] = Global_Variables::G2_Square_q_Zero;
			G2_Type_long	[thread_Index] = Global_Variables::G2_Square_long;
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
		for(int thread_Index = 0; thread_Index<reflectivity_calc_threads; thread_Index++)
		{
			G2_Type			[thread_Index] = Global_Variables::G2_Hexagone;
			G2_Type_q_Zero	[thread_Index] = Global_Variables::G2_Hexagone_q_Zero;
			G2_Type_long	[thread_Index] = Global_Variables::G2_Hexagone_long;
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
	if(struct_Data.fluctuations_Model.geometric_Model == pure_Radial_Model)
	{
		G1_Type = Global_Variables::G1_Square;
		for(int thread_Index = 0; thread_Index<reflectivity_calc_threads; thread_Index++)
		{
			G2_Type			[thread_Index] = Global_Variables::G2_Pure_Radial;
			G2_Type_q_Zero	[thread_Index] = Global_Variables::G2_Pure_Radial_q_Zero;
		}
		b = a*M_SQRT3/2;

		// peak positions
		int n_Max = ceil(q_Max*a/(2*M_PI));
		for(int n = 0; n<=n_Max; n++)
		{
			double q = 2*M_PI/a*n;
			if(	(q < q_Max) &&
				(q > q_Min)
			  )
			{
				q_Peak.push_back(q);
			}
		}
		std::sort(q_Peak.begin(), q_Peak.end());
	}
	double N_size = 1e7*min(measurement.sample_Geometry.size, measurement.beam_Geometry.size/max(measurement.beam_Theta_0_Sin_Value, DBL_EPSILON))/a;
	double N_domain = struct_Data.fluctuations_Model.domain_Size.value/a;
	double N = min(N_size, N_domain);
	N = max(N,3.);
	double M_size = 1e7*measurement.beam_Geometry.lateral_Width/b;
	double M_domain = struct_Data.fluctuations_Model.domain_Size.value/b;
	double M = min(M_size, M_domain);
	M = max(M,3.);
	// we will use M dependent of N
	if(struct_Data.fluctuations_Model.geometric_Model == square_Model)		M = N;
	if(struct_Data.fluctuations_Model.geometric_Model == hexagonal_Model)	M = N*2/M_SQRT3;
	if(struct_Data.fluctuations_Model.geometric_Model == pure_Radial_Model) M = N*2/M_SQRT3;

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
	int num_Bare_Dense_Points = 300;
	int num_Bare_Points = 300;
	int num_Peak_Points = 201; // points inside FWHM * hw_Factor/2
	int hw_Factor = 6;
	double dq_Bare = q_Range/(num_Bare_Points-1);

	// no peaks in range
	if(q_Peak.size() == 0)
	{
		q_Vec.resize(num_Bare_Points);
		for(int i=0; i<num_Bare_Points; i++)
		{
			q_Vec[i] = q_Min+dq_Bare*i;
		}
	} else
	{
		double dq_Bare = q_Range/(num_Bare_Points-1);

		// first peak
		for(size_t peak_Index = 0; peak_Index<1; peak_Index++)
		{
			double dq_Bare_Dense = (q_Peak[peak_Index]-hw_Factor*hw_Peak[peak_Index])/(num_Bare_Dense_Points-1);

			// before each peak
			while(q_Vec.back()+dq_Bare_Dense < q_Peak[peak_Index]-hw_Factor*hw_Peak[peak_Index])
			{
				q_Vec.push_back(q_Vec.back()+dq_Bare_Dense);
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

		// except last and first peak
		for(size_t peak_Index = 1; peak_Index<q_Peak.size()-1; peak_Index++)
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
		if( q_Vec[i] < q_Min) q_Vec.erase(q_Vec.begin()+i);
	}

	// calculation
	int num_Phi_Points_Per_hw = 1;//1
	int max_Phi_Division = 20;
	vector<double> G2_Vec(q_Vec.size());
	vector<bool> too_Narrow(reflectivity_calc_threads);


	if(struct_Data.fluctuations_Model.geometric_Model == pure_Radial_Model)
	{
		Global_Variables::parallel_For(int(q_Vec.size()), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
		{
			for(size_t q_Index = n_Min; q_Index<n_Max; q_Index++)
			{
				double q = q_Vec[q_Index];

//				bool zero_Condition = q>0.5/(a*N) || q>0.5/(sigma*sqrt(N)); // q>DBL_MIN
				if(q>DBL_MIN)
				{
					G2_Vec[q_Index] = Global_Variables::G2_Pure_Radial(q, 0, a, sigma, N, M);
				} else
				{
					G2_Vec[q_Index] = G2_Type_q_Zero[thread_Index](q, a, sigma, N, M);
//					G2_Vec[q_Index] = G1_Type(a);
//					G2_Vec[q_Index] = 0;
				}
			}
		});
	} else
	{
		Global_Variables::parallel_For(int(q_Vec.size()), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
		{
			too_Narrow[thread_Index] = false;
			vector<double> phi_Vec; phi_Vec.reserve(max_Phi_Division+1);
			for(size_t q_Index = n_Min; q_Index<n_Max; q_Index++)
			{
				double q = q_Vec[q_Index];

//				bool zero_Condition = q>0.5/(a*N) || q>0.5/(sigma*sqrt(N)); // q>DBL_MIN
				if(q>DBL_MIN)
				{
					// phi division
					double damp = exp(-0.5*q*q*sigma*sigma);
					double hw_q = max(acos( 2*damp / (1.+damp*damp) ) / a, 4*hw_Peak_Max);
					double arc_q = phi_Max*q;

					int factor = q<1E-3 ? 2 : 1;
					const int gk_points = 61;
					int phi_Division = ceil( arc_q/gk_points * num_Phi_Points_Per_hw/hw_q * factor);
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
				} else
				{
					G2_Vec[q_Index] = G2_Type_q_Zero[thread_Index](q, a, sigma, N, M);
//					G2_Vec[q_Index] = G1_Type(a);
//					G2_Vec[q_Index] = 0;
				}
			}
		});
	}
	for(int i=q_Vec.size()-1; i>=0; i--)
	{
		if(isnan(G2_Vec[i]) || isinf(G2_Vec[i]))
		{
			q_Vec.erase(q_Vec.begin()+i);
			G2_Vec.erase(G2_Vec.begin()+i);
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

void Node::clear_Spline_G2_2D(const Imperfections_Model& imperfections_Model, const Data& measurement)
{
	if(!imperfections_Model.use_Fluctuations) return;
	if(!struct_Data.fluctuations_Model.is_Used) return;
	if(struct_Data.fluctuations_Model.particle_Interference_Function != radial_Paracrystal) return;
	if(struct_Data.item_Type != item_Type_Layer ) return;
	if(measurement.measurement_Type != measurement_Types[GISAS]) return;

	gsl_spline_free(spline_G2);
	gsl_interp_accel_free(acc_G2);
}
