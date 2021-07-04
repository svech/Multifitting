#include "unwrapped_structure.h"
#include "multilayer_approach/multilayer/multilayer.h"

Unwrapped_Structure::Unwrapped_Structure(Multilayer* multilayer,
										 const Calc_Functions& calc_Functions,
										 const vector<Node*>& media_Node_Map_Vector,
										 const vector<Data>& media_Data_Map_Vector,
										 const vector<int>& media_Period_Index_Map_Vector,
										 const Data& measurement,
										 int num_Media_Sharp,
										 bool depth_Grading,
										 bool sigma_Grading,
										 gsl_rng* r) :
	r(r),
	num_Media_Sharp	(num_Media_Sharp),
	num_Boundaries	(num_Media_Sharp - 1),
	num_Layers		(num_Media_Sharp - 2),
	depth_Grading	(depth_Grading),
	sigma_Grading	(sigma_Grading),	
	multilayer			 (multilayer),
	enable_Discretization(multilayer->discretization_Parameters.enable_Discretization),
	imperfections_Model  (multilayer->imperfections_Model),
	calc_Functions		 (calc_Functions),
	media_Node_Map_Vector(media_Node_Map_Vector),
	media_Data_Map_Vector(media_Data_Map_Vector),
	media_Period_Index_Map_Vector(media_Period_Index_Map_Vector),
	measurement			 (measurement)
{
	if( enable_Discretization ||
		calc_Functions.check_Field || calc_Functions.check_Joule ||
		calc_Functions.check_Scattering	|| calc_Functions.check_GISAS )
	{
		fill_Epsilon_Sharp();
	}
	if( enable_Discretization ||
		calc_Functions.check_Scattering	|| calc_Functions.check_GISAS ||
		sigma_Grading )
	{
		fill_Sigma_Diffuse_And_Interlayers();
	}
	if( enable_Discretization ||
		calc_Functions.check_Field || calc_Functions.check_Joule ||
		calc_Functions.check_Scattering	|| calc_Functions.check_GISAS ||
		depth_Grading )
	{
		fill_Thickness_And_Boundaries_Position();
	}
	if( calc_Functions.check_Scattering	|| calc_Functions.check_GISAS )
	{
		fill_Roughness_Parameters();
		fill_PSD_Inheritance_Powers();
	}

	// discretized structure (if discretization is on or just field-epsilon integration for scattering with interlayer)
	if( enable_Discretization ||
		calc_Functions.check_Scattering || calc_Functions.check_GISAS )
	{
		// discretized_Thickness is constructed here
		layer_Normalizing();
		find_Discretization();

		// prolong discretization into ambient and substrate
		Global_Variables::get_Prefix_Suffix(prefix, suffix, max_Sigma);
		Global_Variables::discretize_Prefix_Suffix(prefix, suffix, num_Prefix_Slices, num_Suffix_Slices, discretized_Thickness, multilayer->discretization_Parameters.discretization_Step);
		num_Discretized_Media = discretized_Thickness.size()+2;

		find_Z_Positions();
		find_Subbounds_Limits();

		if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
		{
			int num_Lambda_Points = measurement.lambda_Vec.size();

			discretized_Epsilon_Dependent.resize(num_Lambda_Points, vector<complex<double>>(num_Discretized_Media));

			for(int lambda_Point=0; lambda_Point<num_Lambda_Points; lambda_Point++)
			{
				discretized_Epsilon_Dependent[lambda_Point].assign(num_Discretized_Media,complex<double>(1,0));
			}

			fill_Discretized_Epsilon_Dependent(num_Lambda_Points);
		} else
		// for all angular simulations, including scattering
		{
			discretized_Epsilon.resize(num_Discretized_Media);
			discretized_Epsilon.assign(num_Discretized_Media,complex<double>(1,0));

			fill_Discretized_Epsilon();
		}
	}

	// field functions
	if( calc_Functions.check_Field ||
		calc_Functions.check_Joule )
	{
		if(enable_Discretization)
		{
			discretized_Slices_Boundaries.resize(discretized_Thickness.size()+1);
			discretized_Slices_Boundaries.front() = -num_Prefix_Slices*discretized_Thickness.front();
			for(size_t i=0; i<discretized_Thickness.size(); i++)
			{
				discretized_Slices_Boundaries[i+1] = discretized_Slices_Boundaries[i]+discretized_Thickness[i];
			}
		}
		find_Field_Spacing();
	}
}

void Unwrapped_Structure::fill_Epsilon_Sharp()
{
	if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
	{
		size_t num_Lambda_Points = measurement.lambda_Vec.size();
		epsilon_Dependent.resize(num_Lambda_Points, vector<complex<double>>(num_Media_Sharp));

		Global_Variables::parallel_For(num_Lambda_Points, reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
		{
			Q_UNUSED(thread_Index)
			for(int point_Index = n_Min; point_Index<n_Max; ++point_Index)
			{
				for(int media_Index=0; media_Index<num_Media_Sharp; media_Index++)
				{
					epsilon_Dependent[point_Index][media_Index] = media_Node_Map_Vector[media_Index]->epsilon[point_Index];
				}
			}
		});
	}
	else
	// for all angular simulations, including scattering // Beam_Grazing_Angle, Deviation_From_Specular_Angle, Detector_Polar_Angle, Detector_Azimuthal_Angle, Detector_Theta_Phi_Angles
	{
		epsilon.resize(num_Media_Sharp);
		for(int media_Index=0; media_Index<num_Media_Sharp; media_Index++)
		{
			epsilon[media_Index] = media_Node_Map_Vector[media_Index]->epsilon.front();
		}
	}
}

void Unwrapped_Structure::fill_Sigma_Diffuse_And_Interlayers()
{
	sigma_Diffuse.resize(num_Boundaries);
	common_Sigma_Diffuse.resize(num_Boundaries);
	enabled_Interlayer.resize(num_Boundaries);
	boundary_Interlayer_Composition.resize(num_Boundaries, QVector<Interlayer>(transition_Layer_Functions_Size));

	for(int boundary_Index=0; boundary_Index<num_Boundaries; boundary_Index++)
	{
		int media_Index = boundary_Index+1;
		sigma_Diffuse		[boundary_Index] = media_Data_Map_Vector[media_Index].sigma_Diffuse.value;
		common_Sigma_Diffuse[boundary_Index] = media_Data_Map_Vector[media_Index].common_Sigma_Diffuse;
		enabled_Interlayer  [boundary_Index] = false;


		// interlayers
		for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
		{
			boundary_Interlayer_Composition[boundary_Index][func_Index] = media_Data_Map_Vector[boundary_Index+1].interlayer_Composition[func_Index];

			if(common_Sigma_Diffuse[boundary_Index]) 	{
				boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma_Diffuse.value = sigma_Diffuse[boundary_Index];
			}
			// getting max_Sigma
			if(boundary_Interlayer_Composition[boundary_Index][func_Index].enabled)	{
				enabled_Interlayer[boundary_Index] = true;
				max_Sigma = max(max_Sigma, boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma_Diffuse.value);
			}
			// can drift
			Global_Variables::variable_Drift(boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma_Diffuse.value,
											 media_Data_Map_Vector[media_Index].sigma_Diffuse_Drift,
											 media_Period_Index_Map_Vector[media_Index],
											 media_Data_Map_Vector[media_Index].num_Repetition.value(),
											 r);
		}
		// interlayers drift
		Global_Variables::variable_Drift(sigma_Diffuse[boundary_Index],
										 media_Data_Map_Vector[media_Index].sigma_Diffuse_Drift,
										 media_Period_Index_Map_Vector[media_Index],
										 media_Data_Map_Vector[media_Index].num_Repetition.value(),
										 r);
	}

	// threaded copy
	boundary_Interlayer_Composition_Threaded.resize(reflectivity_calc_threads);

	for(int thread_Index=0; thread_Index<reflectivity_calc_threads; thread_Index++)
	{
		boundary_Interlayer_Composition_Threaded[thread_Index] = boundary_Interlayer_Composition;
	}
}

void Unwrapped_Structure::fill_Thickness_And_Boundaries_Position()
{
	thickness.resize(num_Layers);
	boundaries_Position.resize(num_Boundaries);
	boundaries_Position.front() = 0;

	for(int layer_Index=0; layer_Index<num_Layers; layer_Index++)
	{
		thickness[layer_Index] = media_Data_Map_Vector[layer_Index+1].thickness.value;
		// can drift
		Global_Variables::variable_Drift(thickness[layer_Index],
										 media_Data_Map_Vector[layer_Index+1].thickness_Drift,
										 media_Period_Index_Map_Vector[layer_Index+1],
										 media_Data_Map_Vector[layer_Index+1].num_Repetition.value(),
										 r);
		boundaries_Position[layer_Index+1] = boundaries_Position[layer_Index] + thickness[layer_Index];
	}

	// threaded copy
	boundaries_Position_Threaded.resize(reflectivity_calc_threads);
	thickness_Threaded.resize(reflectivity_calc_threads);

	for(int thread_Index=0; thread_Index<reflectivity_calc_threads; thread_Index++)	{
		boundaries_Position_Threaded[thread_Index] = boundaries_Position;
		thickness_Threaded[thread_Index] = thickness;
	}
}

void Unwrapped_Structure::fill_Roughness_Parameters()
{
	sigma_Roughness.resize(num_Boundaries);
	alpha.resize(num_Boundaries);
	omega.resize(num_Layers);
	mu.resize(num_Layers);
	omega_pow23.resize(num_Layers);
	beta.resize(num_Layers);
	a1.resize(num_Layers);
	a2.resize(num_Layers);
	a3.resize(num_Layers);
	a4.resize(num_Layers);

	for(int layer_Index=0; layer_Index<num_Layers; layer_Index++)
	{
		int media_Index = layer_Index+1;
		if(imperfections_Model.PSD_Model == ABC_Model || imperfections_Model.PSD_Model == fractal_Gauss_Model)
		{
//			double s = media_Data_Map_Vector[media_Index]->roughness_Model.sigma.value;
			double s =  media_Node_Map_Vector[media_Index]->specular_Debye_Waller_Total_Sigma;

//			qInfo() << "lay s" << s << endl;
			sigma_Roughness [layer_Index] = s;
//			if(imperfections_Model.add_Gauss_Peak)
//			{
//				double p = media_Data_Map_Vector[media_Index]->roughness_Model.peak_Sigma.value;
//				sigma_Roughness [layer_Index] = sqrt(s*s + p*p);
//			}
		}
		mu			[layer_Index] = media_Data_Map_Vector[media_Index].roughness_Model.mu.value;
		omega		[layer_Index] = media_Data_Map_Vector[media_Index].roughness_Model.omega.value;
		alpha		[layer_Index] = media_Data_Map_Vector[media_Index].roughness_Model.fractal_alpha.value;
		beta		[layer_Index] = media_Data_Map_Vector[media_Index].roughness_Model.fractal_beta.value;
		omega_pow23	[layer_Index] = pow(omega[layer_Index], (2*alpha[layer_Index]+2)/3); // for linear growth

		a1[layer_Index] = media_Data_Map_Vector[media_Index].roughness_Model.a1.value;
		a2[layer_Index] = media_Data_Map_Vector[media_Index].roughness_Model.a2.value;
		a3[layer_Index] = media_Data_Map_Vector[media_Index].roughness_Model.a3.value;
		a4[layer_Index] = media_Data_Map_Vector[media_Index].roughness_Model.a4.value;
	}
	// substrate
	if(imperfections_Model.PSD_Model == ABC_Model || imperfections_Model.PSD_Model == fractal_Gauss_Model)
	{
		double s =  media_Node_Map_Vector[num_Layers+1]->specular_Debye_Waller_Total_Sigma;
		sigma_Roughness[num_Layers] = s;
	}
	alpha[num_Layers] = media_Data_Map_Vector[num_Layers+1].roughness_Model.fractal_alpha.value;

	// common sigma for partial correlation. for full correlation we use only substrate
	if(imperfections_Model.use_Common_Roughness_Function || imperfections_Model.vertical_Correlation == partial_Correlation)
	{
		for(int boundary_Index=0; boundary_Index<num_Boundaries; boundary_Index++)
		{
			sigma_Roughness[boundary_Index] = sigma_Roughness.back();
		}
	}
	// common partial roughness for use_Common_Roughness_Function
	if(imperfections_Model.use_Common_Roughness_Function)
	{
		for(int layer_Index=0; layer_Index<num_Layers; layer_Index++)
		{
			mu			[layer_Index] = mu.back();
			omega		[layer_Index] = omega.back();
			alpha		[layer_Index] = alpha[num_Layers-1];
			beta		[layer_Index] = beta.back();
			omega_pow23	[layer_Index] = omega_pow23.back();

			a1[layer_Index] = a1.back();
			a2[layer_Index] = a2.back();
			a3[layer_Index] = a3.back();
			a4[layer_Index] = a4.back();
		}
	}

	// threaded copy
	sigma_Roughness_Threaded.resize(reflectivity_calc_threads);
	omega_Threaded.			 resize(reflectivity_calc_threads);
	mu_Threaded.			 resize(reflectivity_calc_threads);
	omega_pow23_Threaded.	 resize(reflectivity_calc_threads);
	alpha_Threaded.			 resize(reflectivity_calc_threads);
	beta_Threaded.			 resize(reflectivity_calc_threads);
	a1_Threaded.			 resize(reflectivity_calc_threads);
	a2_Threaded.			 resize(reflectivity_calc_threads);
	a3_Threaded.			 resize(reflectivity_calc_threads);
	a4_Threaded.			 resize(reflectivity_calc_threads);
	for(int thread_Index=0; thread_Index<reflectivity_calc_threads; thread_Index++)
	{
		sigma_Roughness_Threaded[thread_Index] = sigma_Roughness;
		omega_Threaded			[thread_Index] = omega;
		mu_Threaded				[thread_Index] = mu;
		omega_pow23_Threaded	[thread_Index] = omega_pow23;
		alpha_Threaded			[thread_Index] = alpha;
		beta_Threaded			[thread_Index] = beta;

		a1_Threaded[thread_Index] = a1;
		a2_Threaded[thread_Index] = a2;
		a3_Threaded[thread_Index] = a3;
		a4_Threaded[thread_Index] = a4;
	}
}

void Unwrapped_Structure::fill_PSD_Inheritance_Powers()
{
	PSD_h_mu.resize(num_Layers);
	for(int layer_Index=0; layer_Index<num_Layers; layer_Index++)
	{
		PSD_h_mu[layer_Index] = thickness[layer_Index]/mu[layer_Index];
	}

	// threaded copy
	PSD_h_mu_Threaded.resize(reflectivity_calc_threads);
	for(int thread_Index=0; thread_Index<reflectivity_calc_threads; thread_Index++)
	{
		PSD_h_mu_Threaded[thread_Index] = PSD_h_mu;
	}
}

///-------------------------------------------------------------
// DISCRETIZATION
///-------------------------------------------------------------

void Unwrapped_Structure::layer_Normalizing()
{
	// norm. Optimize (before unwrapping, check similar layers ....). Now more or less OK
	layer_Norm_Vector.resize(thickness.size());
	QList<Different_Norm_Layer> different_Norm_Layer;
	Different_Norm_Layer temp_Dif_Norm;
//	gsl_integration_workspace* w = gsl_integration_workspace_alloc(1000);
	for(size_t layer_Index=0; layer_Index<thickness.size(); layer_Index++)
	{
		// thickness
		if(thickness[layer_Index]>0)
		{
			temp_Dif_Norm.thickness = thickness[layer_Index];
			temp_Dif_Norm.sigma_Diffuse_Left = sigma_Diffuse[layer_Index];
			temp_Dif_Norm.sigma_Diffuse_Right = sigma_Diffuse[layer_Index+1];

			if(!different_Norm_Layer.contains(temp_Dif_Norm))
			{
				layer_Norm_Vector[layer_Index] = thickness[layer_Index] /
//						Global_Variables::layer_Normalization_GSL(	thickness[layer_Index],
//																boundary_Interlayer_Composition[layer_Index],
//																boundary_Interlayer_Composition[layer_Index+1], w);
						Global_Variables::layer_Normalization(thickness[layer_Index],
																	boundary_Interlayer_Composition[layer_Index],
																	boundary_Interlayer_Composition[layer_Index+1]);

				temp_Dif_Norm.norm = layer_Norm_Vector[layer_Index];
				different_Norm_Layer.append(temp_Dif_Norm);
			} else
			{
				Different_Norm_Layer old_Norm = different_Norm_Layer[different_Norm_Layer.indexOf(temp_Dif_Norm)];
				layer_Norm_Vector[layer_Index] = old_Norm.norm;
			}
		} else
		{
			layer_Norm_Vector[layer_Index] = 1;
		}
	}
//	gsl_integration_workspace_free(w);

	// threaded copy
	layer_Norm_Vector_Threaded.resize(reflectivity_calc_threads);
	for(int thread_Index=0; thread_Index<reflectivity_calc_threads; thread_Index++) {
		layer_Norm_Vector_Threaded[thread_Index] = layer_Norm_Vector;
	}
}

void Unwrapped_Structure::find_Discretization()
{
	discretized_Thickness.clear();
	discretized_Thickness.reserve(100000);
	num_Slices_Vec.resize(num_Layers);

	for(int layer_Index=0; layer_Index<num_Layers; layer_Index++)
	{
		int num_Slices = ceil(thickness[layer_Index]/ multilayer->discretization_Parameters.discretization_Step);
		double adapted_Step = thickness[layer_Index]/num_Slices;

		discretized_Thickness.resize(discretized_Thickness.size()+num_Slices);
		size_t last_Index = discretized_Thickness.size()-1;
		for(int i=0; i<num_Slices; i++)
		{
			discretized_Thickness[last_Index-i] = adapted_Step;
		}
		num_Slices_Vec[layer_Index] = num_Slices;
	}	
}

void Unwrapped_Structure::find_Z_Positions()
{
	z_Positions.resize(discretized_Thickness.size());
	double z = -(num_Prefix_Slices-0.5)*discretized_Thickness.front();
	for(size_t i=0; i<discretized_Thickness.size(); ++i)
	{
		z_Positions[i] = z;
		if(i<(discretized_Thickness.size()-1))
		{
			z += (discretized_Thickness[i]+discretized_Thickness[i+1])/2.; // real z, where we calculate epsilon
		} else
		{
			z += discretized_Thickness[i]; // real z, where we calculate epsilon
		}
	}
}

void Unwrapped_Structure::find_Subbounds_Limits()
{
	boundary_Subboundary_Map_Vector.resize(num_Boundaries);
	boundary_Subboundary_Map_Vector.front().top_Boundary = 0;

	int current_Boundary = num_Prefix_Slices;
	for(int layer_Index=0; layer_Index<num_Layers; layer_Index++)
	{
		if(num_Slices_Vec[layer_Index]%2==0) // 2, 4, 6...
		{
			boundary_Subboundary_Map_Vector[layer_Index].bottom_Boundary = current_Boundary + num_Slices_Vec[layer_Index]/2-1;
			boundary_Subboundary_Map_Vector[layer_Index+1].top_Boundary  = current_Boundary + num_Slices_Vec[layer_Index]/2+1;

			boundary_Subboundary_Map_Vector[layer_Index].bottom_Half_Boundary = true;
			boundary_Subboundary_Map_Vector[layer_Index+1].top_Half_Boundary = true;
		} else
		{
			boundary_Subboundary_Map_Vector[layer_Index].bottom_Boundary = current_Boundary + floor(double(num_Slices_Vec[layer_Index])/2);
			boundary_Subboundary_Map_Vector[layer_Index+1].top_Boundary  = current_Boundary + ceil (double(num_Slices_Vec[layer_Index])/2);
		}
		current_Boundary += num_Slices_Vec[layer_Index];
	}
	boundary_Subboundary_Map_Vector.back().bottom_Boundary = discretized_Thickness.size();
}

void Unwrapped_Structure::fill_Discretized_Epsilon()
{
	// ambient
	discretized_Epsilon.front() = epsilon.front();

	// main part
	Global_Variables::parallel_For(discretized_Thickness.size(), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		for(int i=n_Min; i<n_Max; ++i)
//		for(int i=0; i<discretized_Thickness.size(); ++i)
		{
			epsilon_Func(z_Positions[i], i+1, false, epsilon, epsilon_Dependent, discretized_Epsilon, discretized_Epsilon_Dependent, thread_Index);
		}
	});

	// substrate
	discretized_Epsilon.back() = epsilon.back();
}

void Unwrapped_Structure::fill_Discretized_Epsilon_Dependent(int num_Lambda_Points)
{
	// ambient
	for(int lambda_Point=0; lambda_Point<num_Lambda_Points; lambda_Point++)
	{
		discretized_Epsilon_Dependent[lambda_Point].front() = epsilon_Dependent[lambda_Point].front();
	}

	// main part
	Global_Variables::parallel_For(discretized_Thickness.size(), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		for(int i=n_Min; i<n_Max; ++i)
//		for(int i=0; i<discretized_Thickness.size(); ++i)
		{
			epsilon_Func(z_Positions[i], i+1, true, epsilon, epsilon_Dependent, discretized_Epsilon, discretized_Epsilon_Dependent, thread_Index);
		}
	});

	// substrate
	for(int lambda_Point=0; lambda_Point<num_Lambda_Points; lambda_Point++)
	{
		discretized_Epsilon_Dependent[lambda_Point].back() = epsilon_Dependent[lambda_Point].back();
	}
}

void Unwrapped_Structure::epsilon_Func(double z, int media_Index, bool is_Dependent,
								const vector<complex<double>>& epsilon_Vector,
								const vector<vector<complex<double>>>& epsilon_Dependent_Vector,
								vector<complex<double>>& discretized_Epsilon,
								vector<vector<complex<double>>>& discretized_Epsilon_Dependent,
								int thread_Index)

{
	// where we are
	int sigma_Factor = 6;
	std::vector<double>::iterator it_low = std::lower_bound(boundaries_Position_Threaded[thread_Index].begin(), boundaries_Position_Threaded[thread_Index].end(), z-sigma_Factor*max_Sigma);
	std::vector<double>::iterator it_up  = std::upper_bound(boundaries_Position_Threaded[thread_Index].begin(), boundaries_Position_Threaded[thread_Index].end(), z+sigma_Factor*max_Sigma);

	int min_Boundary_Index = max(min(int(it_low-boundaries_Position_Threaded[thread_Index].begin())-1, int(thickness.size()-1)),0);
	int max_Boundary_Index = min(    int(it_up -boundaries_Position_Threaded[thread_Index].begin()),   int(thickness.size()-1));

	double geometry_Factor = 1;

	for(int j=min_Boundary_Index; j<=max_Boundary_Index; j++) // instead of old slow for(int j=0; j<thickness_Vector.size(); j++)
	{
		geometry_Factor = layer_Norm_Vector_Threaded[thread_Index][j] *
				Global_Variables::interface_Profile_Function(z-boundaries_Position_Threaded[thread_Index][j  ], boundary_Interlayer_Composition_Threaded[thread_Index][j]) *
				Global_Variables::interface_Profile_Function(boundaries_Position_Threaded[thread_Index][j+1]-z, boundary_Interlayer_Composition_Threaded[thread_Index][j+1]);

		if(!is_Dependent)
		{			
			discretized_Epsilon[media_Index] += complex<double>(real(epsilon_Vector[j+1])-1,imag(epsilon_Vector[j+1])) * geometry_Factor;
		} else
		{
			for(size_t lambda_Point=0; lambda_Point<discretized_Epsilon_Dependent.size(); lambda_Point++)
			{
				discretized_Epsilon_Dependent[lambda_Point][media_Index] += complex<double>(real(epsilon_Dependent_Vector[lambda_Point][j+1])-1,imag(epsilon_Dependent_Vector[lambda_Point][j+1])) * geometry_Factor;
			}
		}
	}
	if(max_Boundary_Index>=thickness.size()-1)
	{
		geometry_Factor = Global_Variables::interface_Profile_Function(z-boundaries_Position_Threaded[thread_Index].back(), boundary_Interlayer_Composition_Threaded[thread_Index].back());

		if(!is_Dependent)
		{
			discretized_Epsilon[media_Index] += complex<double>(real(epsilon_Vector.back())-1,imag(epsilon_Vector.back())) * geometry_Factor;
		} else
		{
			for(size_t lambda_Point=0; lambda_Point<discretized_Epsilon_Dependent.size(); lambda_Point++)
			{
				discretized_Epsilon_Dependent[lambda_Point][media_Index] += complex<double>(real(epsilon_Dependent_Vector[lambda_Point].back())-1,imag(epsilon_Dependent_Vector[lambda_Point].back())) * geometry_Factor;
			}
		}
	}
}

void Unwrapped_Structure::find_Field_Spacing()
{
	// structure and substrate
	double structure_Thickness = 0;
	for(int layer_Index=0; layer_Index<num_Layers; layer_Index++)
	{
		structure_Thickness += thickness[layer_Index];
	}
	structure_Thickness += calc_Functions.field_Substrate_Distance;
	structure_Thickness = max(structure_Thickness, 0.);
	num_Field_Slices = ceil(structure_Thickness/calc_Functions.field_Step)+1;

	field_Z_Positions.resize(num_Field_Slices);
	for(int i=0; i<num_Field_Slices; ++i)
	{
		field_Z_Positions[i] = i*calc_Functions.field_Step;
	}

	// ambient
	int num_Ambient_Slices = ceil(calc_Functions.field_Ambient_Distance/calc_Functions.field_Step);
	num_Field_Slices += num_Ambient_Slices;
	for(int i=0; i<num_Ambient_Slices; ++i)
	{
		double z = -(i+1)*calc_Functions.field_Step;
		field_Z_Positions.insert(field_Z_Positions.begin(), z);
	}
}

int Unwrapped_Structure::get_Layer_or_Slice_Index(double z)
{
	if(enable_Discretization)
	{
		std::vector<double>::iterator it_low = std::lower_bound(discretized_Slices_Boundaries.begin(), discretized_Slices_Boundaries.end(), z);
		return int(it_low-discretized_Slices_Boundaries.begin())-1;
	} else
	{
		std::vector<double>::iterator it_low = std::lower_bound(boundaries_Position.begin(), boundaries_Position.end(), z);
		return int(it_low-boundaries_Position.begin())-1;
	}
}
