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

void Node::calculate_Intermediate_Points(const Data& measurement, Node* above_Node, bool depth_Grading, bool sigma_Grading, bool enable_Discretization, QString mode)
{			
	// crutch
	struct_Data.relative_Density.value = max(struct_Data.relative_Density.value,DBL_EPSILON); // TODO vacuum crashes?

	int num_Points = 0;
	vector<double> cos2;
	vector<double> k;

	// for any media
	if( struct_Data.item_Type == item_Type_Ambient ||
		struct_Data.item_Type == item_Type_Layer   ||
		struct_Data.item_Type == item_Type_Substrate )
	{
		if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
		{
			if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
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
				vector<double> spectral_Points (1, measurement.wavelength.value);
				fill_Epsilon_For_Angular_Measurements(spectral_Points);
			}
			if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
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

					// just in case
					if(measurement.beam_Theta_0_Cos2_Vec.size() != measurement.detector_Theta_Cos2_Vec.size())
					{
						qInfo() << "Rocking_Curve, Offset_Scan  :  measurement.beam_Theta_0_Cos2_Vec.size() != measurement.detector_Theta_Cos2_Vec.size()" << endl;
					}
				}
			}
			if(mode == SCATTERED_MODE)
			{
				num_Points = measurement.detector_Theta_Cos2_Vec.size();
				cos2 = measurement.detector_Theta_Cos2_Vec;
			}

			// common
			k.resize(num_Points);
			for(int point_Index = 0; point_Index<num_Points; ++point_Index)
			{
				k[point_Index] = measurement.k_Value;
			}

			epsilon.resize(num_Points);
			vector<double> spectral_Points (1, measurement.wavelength.value);
			fill_Epsilon_For_Angular_Measurements(spectral_Points);
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
				for(int i=0; i<num_Points; ++i)
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
					for(int i=0; i<num_Points; ++i)
					{
						if ( abs(above_Node->hi[i] + hi[i]) > DBL_MIN)
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
					for(int i=0; i<num_Points; ++i)
					{
						hi_je = above_Node->hi[i]/above_Node->epsilon[i];
						hi_j1e = hi[i]/epsilon[i];

						if ( abs(hi_je+hi_j1e) > DBL_MIN )
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

				if( is_Norm && (abs(struct_Data.sigma_Diffuse.value) > DBL_MIN) && !sigma_Grading && measurement.measurement_Type == measurement_Types[Specular_Scan] )
				{
					// temp variables
					double a = M_PI/sqrt(M_PI*M_PI - 8.);

					double norm = 0;
					double my_Sigma = struct_Data.sigma_Diffuse.value;	// by default, otherwise we change it
					vector<complex<double>> s_r (num_Points);
					vector<complex<double>> s_t (num_Points);

					for(int i=0; i<num_Points; ++i)
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
						for(int i=0; i<num_Points; ++i)
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
						for(int i=0; i<num_Points; ++i)
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
						for(int i=0; i<num_Points; ++i)
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
						for(int i=0; i<num_Points; ++i)
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
							for(int i=0; i<num_Points; ++i)
							{
								weak_Factor_R[i] /= norm;
								weak_Factor_T[i] /= norm;
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
			if( struct_Data.item_Type == item_Type_Layer )
			{
				exponenta.  resize(num_Points);
				exponenta_2.resize(num_Points);
				if( depth_Grading )
				{
					for(int i=0; i<num_Points; ++i)
					{
						exponenta[i]   = 1.;
						exponenta_2[i] = 1.;
					}
				} else
				{
					for(int i=0; i<num_Points; ++i)
					{
						exponenta[i] = exp(I*hi[i]*struct_Data.thickness.value);
						exponenta_2[i] = pow(exponenta[i],2);
					}
				}
			}

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
	}
}

void Node::fill_Epsilon_For_Angular_Measurements(vector<double>& spectral_Points)
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
	for(size_t point_Index = 0; point_Index<epsilon.size(); ++point_Index)
	{
		epsilon[point_Index] = epsilon_Ang;
	}
}

// fractal gauss integration
struct Fractal_Gauss_Params
{
	double sigma;
	double xi;
	double alpha;
};
double Cor_Fractal_Gauss(double r, void* params)
{
	Fractal_Gauss_Params* p = reinterpret_cast<Fractal_Gauss_Params*>(params);

	if(p->xi > 0)	return p->sigma*p->sigma * exp(-pow(r/p->xi,2*p->alpha));
	else			return 0;
}
void Node::create_Spline_PSD_Fractal_Gauss_1D(const Data& measurement)
{
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
	double p_Max = temp_Cos.back()+1E-7;


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

	gsl_integration_workspace* w = gsl_integration_workspace_alloc(1000);
	gsl_integration_workspace* wc = gsl_integration_workspace_alloc(1000);

	Fractal_Gauss_Params params = {sigma, xi, alpha};
	gsl_function F = { &Cor_Fractal_Gauss, &params };

	double abs_Err = 1e-7;
	double rel_Err = 1e-7;

	double p = 0, result = 0, error;
	double interval = 2*xi/sqrt(alpha);
	int counter = 1;
	for(int sec=0; sec<num_Sections; sec++)
	{
		for(int i=0; i<interpoints[sec]; i++)
		{
			p += dp[sec];
			gsl_integration_qawo_table* wf = gsl_integration_qawo_table_alloc(p, interval, GSL_INTEG_COSINE, 25);
			double current_Point = 0, sum_Result = 0;
			gsl_integration_qawo(&F, current_Point, abs_Err, rel_Err, w->limit, w, wf, &result, &error); sum_Result += result; current_Point += interval;
			gsl_integration_qawo(&F, current_Point, abs_Err, rel_Err, w->limit, w, wf, &result, &error); sum_Result += result; current_Point += interval;
			gsl_integration_qawf(&F, current_Point,         1e-4, w->limit, w, wc, wf, &result, &error); sum_Result += result; current_Point += interval;
			gsl_integration_qawo_table_free(wf);
			interpoints_Sum_Argum_Vec[counter] = p;
			interpoints_Sum_Value_Vec[counter] = 4*sum_Result;
			counter++;
		}
	}
	// chech for artifacts
	for(int i=interpoints_Sum_Argum_Vec.size()-1; i>=0; i--)
	{
		if(interpoints_Sum_Value_Vec[i]<0.5*interpoints_Sum_Value_Vec[i+1])
		{
			interpoints_Sum_Value_Vec.erase (interpoints_Sum_Value_Vec.begin()+i);
			interpoints_Sum_Argum_Vec.erase (interpoints_Sum_Argum_Vec.begin()+i);
		}
	}
	gsl_integration_workspace_free(wc);
	gsl_integration_workspace_free(w);

	const gsl_interp_type* interp_type = gsl_interp_steffen;
	acc = gsl_interp_accel_alloc();
	spline = gsl_spline_alloc(interp_type, interpoints_Sum_Value_Vec.size());
	gsl_spline_init(spline, interpoints_Sum_Argum_Vec.data(), interpoints_Sum_Value_Vec.data(), interpoints_Sum_Value_Vec.size());
}

// fractal gauss integration
struct Cor_Func_Series_Params
{
	double xi;
	double alpha;
	int n;
	double factor;
};
double Cor_Fractal_Gauss_Pow_n(double r, void* params)
{
	Cor_Func_Series_Params* p = reinterpret_cast<Cor_Func_Series_Params*>(params);

	if(p->xi > 0)	return exp(-(p->n)*pow(r/p->xi,2*p->alpha));
	else			return 0;
}
double Cor_ABC_Pow_n(double r, void* params)
{
	Cor_Func_Series_Params* p = reinterpret_cast<Cor_Func_Series_Params*>(params);

	if(p->xi > 0)	return p->factor*pow(r/p->xi, p->n*p->alpha) * pow(cyl_bessel_k(p->alpha, r/p->xi),p->n);
	else			return 0;
}
void Node::create_Spline_DWBA_SA_CSA_1D(const Data& measurement, QString model)
{
	double xi =    struct_Data.roughness_Model.cor_radius.value;
	double alpha = struct_Data.roughness_Model.fractal_alpha.value;

	// taking p_Max
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
	double p_Max = temp_Cos.back()+1E-7;

	// subintervals
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

	gsl_integration_workspace* w = gsl_integration_workspace_alloc(1000);
	gsl_integration_workspace* wc = gsl_integration_workspace_alloc(1000);

	spline_n_Vec.clear();
	acc_n_Vec.clear();

	// n=35 is still ok
	for(int n=1; n<2; n++)
	{
		Cor_Func_Series_Params params = {xi, alpha, n, pow(pow(2,1-alpha)/tgamma(alpha),n)};
		gsl_function F;
		if(model == fractal_Gauss_Model) F = { &Cor_Fractal_Gauss_Pow_n, &params };
		if(model == ABC_model)			 F = { &Cor_ABC_Pow_n,           &params };

		double p = 0, result = 0, error;
		double abs_Err = 1e-7;
		double rel_Err = 1e-7;

		// zero point
		{
			interpoints_Sum_Argum_Vec[0] = 0;
			if(model == fractal_Gauss_Model) interpoints_Sum_Value_Vec[0] = xi*tgamma(1.+1/(2*alpha))/pow(n,1/(2*alpha));
			if(model == ABC_model)
			{
				double interval = 20*xi/sqrt(alpha);
				double current_Point = 0, sum_Result = 0;
				gsl_integration_qag  (&F, current_Point, current_Point+interval, abs_Err, rel_Err, w->limit, GSL_INTEG_GAUSS61, w, &result, &error); sum_Result += result; current_Point += interval;
				gsl_integration_qag  (&F, current_Point, current_Point+interval, abs_Err, rel_Err, w->limit, GSL_INTEG_GAUSS61, w, &result, &error); sum_Result += result; current_Point += interval;
				gsl_integration_qag  (&F, current_Point, current_Point+interval, abs_Err, rel_Err, w->limit, GSL_INTEG_GAUSS61, w, &result, &error); sum_Result += result; current_Point += interval;
				gsl_integration_qag  (&F, current_Point, current_Point+interval, abs_Err, rel_Err, w->limit, GSL_INTEG_GAUSS61, w, &result, &error); sum_Result += result; current_Point += interval;
				gsl_integration_qag  (&F, current_Point, current_Point+interval, abs_Err, rel_Err, w->limit, GSL_INTEG_GAUSS61, w, &result, &error); sum_Result += result; current_Point += interval;
				//gsl_integration_qagiu(&F, current_Point,                         abs_Err, rel_Err, w->limit,                    w, &result, &error); sum_Result += result; current_Point += interval;
				interpoints_Sum_Value_Vec[0] = sum_Result;
			}
		}

		// other points
		double interval = 2*xi/sqrt(alpha);
		int counter = 1;
		for(int sec=0; sec<num_Sections; sec++)
		{
			for(int i=0; i<interpoints[sec]; i++)
			{
				p += dp[sec];
				gsl_integration_qawo_table* wf = gsl_integration_qawo_table_alloc(p, interval, GSL_INTEG_COSINE, 45);
				double current_Point = 0, sum_Result = 0;
				if(model == fractal_Gauss_Model)
				{
				gsl_integration_qawo(&F, current_Point, abs_Err, rel_Err, w->limit, w, wf, &result, &error); sum_Result += result; current_Point += interval;
				gsl_integration_qawo(&F, current_Point, abs_Err, rel_Err, w->limit, w, wf, &result, &error); sum_Result += result; current_Point += interval;
				}
				gsl_integration_qawf(&F, current_Point,         1e-4, w->limit, w, wc, wf, &result, &error); sum_Result += result; current_Point += interval;
				gsl_integration_qawo_table_free(wf);
				interpoints_Sum_Argum_Vec[counter] = p;
				interpoints_Sum_Value_Vec[counter] = sum_Result;
				counter++;
			}
		}

		// chech for artifacts
		for(int i=interpoints_Sum_Argum_Vec.size()-1; i>=0; i--)
		{
			if(interpoints_Sum_Value_Vec[i]<0.5*interpoints_Sum_Value_Vec[i+1])
			{
				interpoints_Sum_Value_Vec.erase (interpoints_Sum_Value_Vec.begin()+i);
				interpoints_Sum_Argum_Vec.erase (interpoints_Sum_Argum_Vec.begin()+i);
			}
		}

		// splines
		acc_n_Vec.resize(n);
		spline_n_Vec.resize(n);
		acc_n_Vec[n-1] = gsl_interp_accel_alloc();
		spline_n_Vec[n-1] = gsl_spline_alloc(gsl_interp_steffen, interpoints_Sum_Value_Vec.size());
		gsl_spline_init(spline_n_Vec[n-1], interpoints_Sum_Argum_Vec.data(), interpoints_Sum_Value_Vec.data(), interpoints_Sum_Value_Vec.size());
	}
	gsl_integration_workspace_free(wc);
	gsl_integration_workspace_free(w);
}

void Node::clear_Spline_DWBA_SA_CSA_1D()
{
	for(size_t i=0; i<acc_n_Vec.size(); i++)
	{
		gsl_spline_free(spline_n_Vec[i]);
		gsl_interp_accel_free(acc_n_Vec[i]);
	}
}

void Node::clear_Spline_PSD_Fractal_Gauss_1D()
{
	gsl_spline_free(spline);
	gsl_interp_accel_free(acc);
}

