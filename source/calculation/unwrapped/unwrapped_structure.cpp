#include "unwrapped_structure.h"

Unwrapped_Structure::Unwrapped_Structure(const Calc_Functions& calc_Functions, const tree<Node>& calc_Tree, const Data& measurement, int num_Media, int max_Depth, int depth_Threshold, bool depth_Grading, bool sigma_Grading, Discretization_Parameters discretization_Parameters, gsl_rng* r):
	r(r),
	num_Threads		(epsilon_Partial_Fill_Threads),
	num_Media		(num_Media),
	num_Boundaries	(num_Media - 1),
	num_Layers		(num_Media - 2),
	max_Depth		(max_Depth),
	depth_Threshold	(depth_Threshold),
	depth_Grading	(depth_Grading),
	sigma_Grading	(sigma_Grading),	
	discretization_Parameters(discretization_Parameters),
	calc_Tree		(calc_Tree),
	calc_Functions  (calc_Functions)
{
	// recalculate all if depth is big
	if( (max_Depth > depth_Threshold) || discretization_Parameters.enable_Discretization
		|| calc_Functions.check_Field || calc_Functions.check_Joule)
	{		
		if( measurement.measurement_Type == measurement_Types[Specular_Scan] &&
			measurement.argument_Type == argument_Types[Wavelength_Energy] )
		{
			int num_Lambda_Points = measurement.lambda_Vec.size();

			epsilon_Dependent.resize(num_Lambda_Points, vector<complex<double>>(num_Media));
//			epsilon_Dependent_RE.resize(num_Lambda_Points, vector<double>(num_Media));
//			epsilon_Dependent_IM.resize(num_Lambda_Points, vector<double>(num_Media));
//			epsilon_Dependent_NORM.resize(num_Lambda_Points, vector<double>(num_Media));

			fill_Epsilon_Dependent(calc_Tree.begin(), num_Lambda_Points);
		} else
		{
			epsilon.resize(num_Media);
//			epsilon_RE.resize(num_Media);
//			epsilon_IM.resize(num_Media);
//			epsilon_NORM.resize(num_Media);
			fill_Epsilon(calc_Tree.begin());
		}
	}
	// recalculate sigmas if depth is big or sigma grading
	if( (max_Depth > depth_Threshold) || sigma_Grading || discretization_Parameters.enable_Discretization)
	{
		max_Sigma = 0.1;
		sigma.resize(num_Boundaries);
		common_Sigma.resize(num_Boundaries);
		boundary_Interlayer_Composition.resize(num_Boundaries, QVector<Interlayer>(transition_Layer_Functions_Size));
		fill_Sigma(calc_Tree.begin(), max_Sigma);

		// multithreading
		boundary_Interlayer_Composition_Threaded.resize(reflectivity_Calc_Threads);
		for(int thread_Index=0; thread_Index<reflectivity_Calc_Threads; thread_Index++)	{
			boundary_Interlayer_Composition_Threaded[thread_Index] = boundary_Interlayer_Composition;
		}
	}

	// recalculate thicknesses if depth is big or depth grading
	if( (max_Depth > depth_Threshold) || depth_Grading || discretization_Parameters.enable_Discretization
		|| calc_Functions.check_Field || calc_Functions.check_Joule)
	{
		thickness.resize(num_Layers);
		boundaries.resize(num_Boundaries);
		boundaries.front() = 0;
		fill_Thickness_And_Boundaries(calc_Tree.begin());

		// multithreading
		boundaries_Threaded.resize(reflectivity_Calc_Threads);
		for(int thread_Index=0; thread_Index<reflectivity_Calc_Threads; thread_Index++)	{
			boundaries_Threaded[thread_Index] = boundaries;
		}
	} /*else
	{
		epsilon.resize(num_Media);

		sigma.resize(num_Boundaries);
		boundary_Interlayer_Composition.resize(num_Boundaries, vector<Interlayer>(transition_Layer_Functions_Size));
		thickness.resize(num_Layers);

		fill_Epsilon_Max_Depth_2	(calc_Tree->begin());
		fill_Sigma_Max_Depth_2		(calc_Tree->begin());
		fill_Thickness_Max_Depth_2	(calc_Tree->begin());
	}*/

	if(discretization_Parameters.enable_Discretization)
	{
		// discretized_Thickness is constructed here
		find_Discretization();
		layer_Normalizing();

		// prolong discretization into ambient and substrate
		Global_Variables::get_Prefix_Suffix(prefix, suffix, max_Sigma);
		Global_Variables::discretize_Prefix_Suffix(prefix, suffix, num_Prefix_Slices, num_Suffix_Slices, discretized_Thickness, discretization_Parameters.discretization_Step);
		num_Discretized_Media = discretized_Thickness.size()+2;

		find_Z_Positions();

		// PARAMETER
		if( measurement.measurement_Type == measurement_Types[Specular_Scan] &&
			measurement.argument_Type == argument_Types[Wavelength_Energy] )
		{
			int num_Lambda_Points = measurement.lambda_Vec.size();

			discretized_Epsilon_Dependent.resize(num_Lambda_Points, vector<complex<double>>(num_Discretized_Media));

			for(int lambda_Point=0; lambda_Point<num_Lambda_Points; lambda_Point++)
			{
				discretized_Epsilon_Dependent[lambda_Point].assign(num_Discretized_Media,complex<double>(1,0));
			}

			fill_Discretized_Epsilon_Dependent(num_Lambda_Points);
		} else
		{
			discretized_Epsilon.resize(num_Discretized_Media);
			discretized_Epsilon.assign(num_Discretized_Media,complex<double>(1,0));

			fill_Discretized_Epsilon();
		}
	}

	// field functions
	if(calc_Functions.check_Field || calc_Functions.check_Joule)
	{
		if(discretization_Parameters.enable_Discretization)
		{
			discretized_Slices_Boundaries.resize(discretized_Thickness.size()+1);
			discretized_Slices_Boundaries.front() = -num_Prefix_Slices*discretized_Thickness.front();
			for(int i=0; i<discretized_Thickness.size(); i++)
			{
				discretized_Slices_Boundaries[i+1] = discretized_Slices_Boundaries[i]+discretized_Thickness[i];
			}
		}

		find_Field_Spacing();
	}
}

void Unwrapped_Structure::layer_Normalizing()
{
	// norm. Optimize (before unwrapping, check similar layers ....). Now more or less OK
	layer_Norm_Vector.resize(thickness.size());
	QList<Different_Norm_Layer> different_Norm_Layer;
	Different_Norm_Layer temp_Dif_Norm;
	gsl_integration_workspace* w = gsl_integration_workspace_alloc(1000);
	for(size_t layer_Index=0; layer_Index<thickness.size(); layer_Index++)
	{
		// thickness
		if(thickness[layer_Index]>0)
		{
			temp_Dif_Norm.thickness = thickness[layer_Index];
			temp_Dif_Norm.sigma_Left = sigma[layer_Index];
			temp_Dif_Norm.sigma_Right = sigma[layer_Index+1];

			if(!different_Norm_Layer.contains(temp_Dif_Norm))
			{
				layer_Norm_Vector[layer_Index] = thickness[layer_Index] /
						Global_Variables::layer_Normalization(	thickness[layer_Index],
																boundary_Interlayer_Composition[layer_Index],
																boundary_Interlayer_Composition[layer_Index+1], w);
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
	gsl_integration_workspace_free(w);

	// multithreading
	layer_Norm_Vector_Threaded.resize(reflectivity_Calc_Threads);
	for(int thread_Index=0; thread_Index<reflectivity_Calc_Threads; thread_Index++) {
		layer_Norm_Vector_Threaded[thread_Index] = layer_Norm_Vector;
	}
}

void Unwrapped_Structure::find_Discretization()
{
	discretized_Thickness.clear();
	discretized_Thickness.reserve(100000);

	for(int layer_Index=0; layer_Index<num_Layers; layer_Index++)
	{
		int num_Slices = ceil(thickness[layer_Index]/discretization_Parameters.discretization_Step);
		double adapted_Step = thickness[layer_Index]/num_Slices;

		discretized_Thickness.resize(discretized_Thickness.size()+num_Slices);
		size_t last_Index = discretized_Thickness.size()-1;
		for(int i=0; i<num_Slices; i++)
		{
			discretized_Thickness[last_Index-i] = adapted_Step;
		}
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

void Unwrapped_Structure::fill_Discretized_Epsilon()
{
	// ambient
	discretized_Epsilon.front() = epsilon.front();

	// main part
	Global_Variables::parallel_For(discretized_Thickness.size(), reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
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
	Global_Variables::parallel_For(discretized_Thickness.size(), reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
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
	std::vector<double>::iterator it_low = std::lower_bound(boundaries_Threaded[thread_Index].begin(), boundaries_Threaded[thread_Index].end(), z-sigma_Factor*max_Sigma);
	std::vector<double>::iterator it_up  = std::upper_bound(boundaries_Threaded[thread_Index].begin(), boundaries_Threaded[thread_Index].end(), z+sigma_Factor*max_Sigma);

	int min_Boundary_Index = max(min(int(it_low-boundaries_Threaded[thread_Index].begin())-1, int(thickness.size()-1)),0);
	int max_Boundary_Index = min(    int(it_up -boundaries_Threaded[thread_Index].begin()),   int(thickness.size()-1));

	double geometry_Factor = 1;

	for(int j=min_Boundary_Index; j<=max_Boundary_Index; j++) // instead of old slow for(int j=0; j<thickness_Vector.size(); j++)
	{
		geometry_Factor = layer_Norm_Vector_Threaded[thread_Index][j] *
				Global_Variables::interface_Profile_Function(z-boundaries_Threaded[thread_Index][j  ], boundary_Interlayer_Composition_Threaded[thread_Index][j]) *
				Global_Variables::interface_Profile_Function(boundaries_Threaded[thread_Index][j+1]-z, boundary_Interlayer_Composition_Threaded[thread_Index][j+1]);

		if(!is_Dependent)
		{			
			discretized_Epsilon[media_Index] += complex<double>(real(epsilon_Vector[j+1])-1,imag(epsilon_Vector[j+1])) * geometry_Factor;
		} else
		{
			for(int lambda_Point=0; lambda_Point<discretized_Epsilon_Dependent.size(); lambda_Point++)
			{
				discretized_Epsilon_Dependent[lambda_Point][media_Index] += complex<double>(real(epsilon_Dependent_Vector[lambda_Point][j+1])-1,imag(epsilon_Dependent_Vector[lambda_Point][j+1])) * geometry_Factor;
			}
		}
	}
	if(max_Boundary_Index>=thickness.size()-1)
	{
		geometry_Factor = Global_Variables::interface_Profile_Function(z-boundaries_Threaded[thread_Index].back(), boundary_Interlayer_Composition_Threaded[thread_Index].back());

		if(!is_Dependent)
		{
			discretized_Epsilon[media_Index] += complex<double>(real(epsilon_Vector.back())-1,imag(epsilon_Vector.back())) * geometry_Factor;
		} else
		{
			for(int lambda_Point=0; lambda_Point<discretized_Epsilon_Dependent.size(); lambda_Point++)
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
	num_Field_Slices = ceil(structure_Thickness/calc_Functions.field_Step)+1;

	field_Z_Positions.resize(num_Field_Slices);
	for(int i=0; i<num_Field_Slices; ++i)
	{
		field_Z_Positions[i] = i*calc_Functions.field_Step;
	}

	// ambient
	int num_Ambient_Slices = floor(calc_Functions.field_Ambient_Distance/calc_Functions.field_Step);
	num_Field_Slices += num_Ambient_Slices;
	for(int i=0; i<num_Ambient_Slices; ++i)
	{
		double z = -(i+1)*calc_Functions.field_Step;
		field_Z_Positions.insert(field_Z_Positions.begin(), z);
	}
}

int Unwrapped_Structure::get_Layer_or_Slice_Index(double z)
{
	if(discretization_Parameters.enable_Discretization)
	{
		std::vector<double>::iterator it_low = std::lower_bound(discretized_Slices_Boundaries.begin(), discretized_Slices_Boundaries.end(), z);
		return int(it_low-discretized_Slices_Boundaries.begin())-1;
	} else
	{
		std::vector<double>::iterator it_low = std::lower_bound(boundaries.begin(), boundaries.end(), z);
		return int(it_low-boundaries.begin())-1;
	}
}

/*
int Unwrapped_Structure::fill_Epsilon_Max_Depth_2(const tree<Node>::iterator& parent, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient ||
		   whats_This_List[0] == whats_This_Layer   ||
		   whats_This_List[0] == whats_This_Substrate )
		{
			// TODO extreme layers
				epsilon[media_Index] = child.node->data.epsilon;
				epsilon_Norm[media_Index] = real(epsilon[media_Index])*real(epsilon[media_Index]) + imag(epsilon[media_Index])*imag(epsilon[media_Index]);
				epsilon_RE  [media_Index] = real(epsilon[media_Index]);
				epsilon_IM  [media_Index] = imag(epsilon[media_Index]);
			++media_Index;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					// TODO extreme layers
						epsilon[media_Index] = grandchild.node->data.epsilon;
						epsilon_Norm[media_Index] = real(epsilon[media_Index])*real(epsilon[media_Index]) + imag(epsilon[media_Index])*imag(epsilon[media_Index]);
						epsilon_RE  [media_Index] = real(epsilon[media_Index]);
						epsilon_IM  [media_Index] = imag(epsilon[media_Index]);
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}

int Unwrapped_Structure::fill_Sigma_Max_Depth_2(const tree<Node>::iterator& parent, int boundary_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Layer)
		{
			// TODO extreme layers
			sigma[boundary_Index] = child.node->data.layer.sigma.value;

			for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
			{
				if(child.node->data.layer.interlayer_Composition[func_Index].enabled)
					boundary_Interlayer_Composition[boundary_Index][func_Index] = child.node->data.layer.interlayer_Composition[func_Index];
			}
			++boundary_Index;
		}
		if(whats_This_List[0] == whats_This_Substrate)
		{
			// TODO extreme layers
			sigma[boundary_Index] = child.node->data.substrate.sigma.value;

			for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
			{
				if(child.node->data.substrate.interlayer_Composition[func_Index].enabled)
					boundary_Interlayer_Composition[boundary_Index][func_Index] = child.node->data.substrate.interlayer_Composition[func_Index];
			}
			++boundary_Index;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					// TODO extreme layers
					sigma[boundary_Index] = grandchild.node->data.layer.sigma.value;

					for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
					{
						if(grandchild.node->data.layer.interlayer_Composition[func_Index].enabled)
							boundary_Interlayer_Composition[boundary_Index][func_Index] = grandchild.node->data.layer.interlayer_Composition[func_Index];
					}
					++boundary_Index;
				}
			}
		}
	}
	return boundary_Index;
}

int Unwrapped_Structure::fill_Thickness_Max_Depth_2(const tree<Node>::iterator& parent, int layer_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Layer)
		{
			// TODO extreme layers
			thickness[layer_Index] = child.node->data.layer.thickness.value;
			++layer_Index;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					// TODO extreme layers
					thickness[layer_Index] = grandchild.node->data.layer.thickness.value;
					++layer_Index;
				}
			}
		}
	}
	return layer_Index;
}
*/

int Unwrapped_Structure::fill_Epsilon(const tree<Node>::iterator& parent, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);

		// layers that are not from regular aperiodic
		if(child.node->data.struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		{
			if(child.node->data.struct_Data.item_Type == item_Type_Ambient ||
			   child.node->data.struct_Data.item_Type == item_Type_Layer   ||
			   child.node->data.struct_Data.item_Type == item_Type_Substrate )
			{
				// TODO extreme layers
				epsilon     [media_Index] = child.node->data.epsilon     .front();
				++media_Index;
			}
		}

		if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value(); ++period_Index)
			{
				// TODO optimize for depth 2
				media_Index = fill_Epsilon(child, media_Index);
			}
		}

		if( child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value(); ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					epsilon     [media_Index] = grandchild.node->data.epsilon     .front();
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}

int Unwrapped_Structure::fill_Epsilon_Dependent(const tree<Node>::iterator& parent, int num_Lambda_Points, int media_Index)
{
	// epsilon depends on variable
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);

		// layers that are not from regular aperiodic
		if(child.node->data.struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic)
		{
			if(child.node->data.struct_Data.item_Type == item_Type_Ambient ||
			   child.node->data.struct_Data.item_Type == item_Type_Layer   ||
			   child.node->data.struct_Data.item_Type == item_Type_Substrate )
			{
				for(int point_Index = 0; point_Index<num_Lambda_Points; ++point_Index)
				{
				// TODO extreme layers
					epsilon_Dependent     [point_Index][media_Index] = child.node->data.epsilon     [point_Index];
				}
				++media_Index;
			}
		}

		if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value(); ++period_Index)
			{
				// TODO optimize for depth 2
				media_Index = fill_Epsilon_Dependent(child, num_Lambda_Points, media_Index);
			}
		}

		if( child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value(); ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					for(int point_Index = 0; point_Index<num_Lambda_Points; ++point_Index)
					{
						epsilon_Dependent     [point_Index][media_Index] = grandchild.node->data.epsilon     [point_Index];
					}
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}

int Unwrapped_Structure::fill_Sigma(const tree<Node>::iterator& parent, double& max_Sigma, int boundary_Index, int per_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);

		// layers that are not from regular aperiodic
		if( child.node->data.struct_Data.item_Type == item_Type_Layer &&
			child.node->data.struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic )
		{
			// TODO extreme layers			
			sigma       [boundary_Index] = child.node->data.struct_Data.sigma.value;
			common_Sigma[boundary_Index] = child.node->data.struct_Data.common_Sigma;

			for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
			{
				boundary_Interlayer_Composition[boundary_Index][func_Index] = child.node->data.struct_Data.interlayer_Composition[func_Index];			

				if(boundary_Interlayer_Composition[boundary_Index][func_Index].enabled)
				{
					if(max_Sigma<boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma.value)
					{
						max_Sigma=boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma.value;
					}
				}

				// can drift
				Global_Variables::variable_Drift(boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma.value, child.node->data.struct_Data.sigma_Drift, per_Index, child.node->data.struct_Data.num_Repetition.value(), r);
			}
			// can drift
			Global_Variables::variable_Drift(sigma[boundary_Index], child.node->data.struct_Data.sigma_Drift, per_Index, child.node->data.struct_Data.num_Repetition.value(), r);
			++boundary_Index;
		}
		if( child.node->data.struct_Data.item_Type == item_Type_Substrate )
		{
			// TODO extreme layers
			sigma       [boundary_Index] = child.node->data.struct_Data.sigma.value;
			common_Sigma[boundary_Index] = child.node->data.struct_Data.common_Sigma;

			for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
			{
				boundary_Interlayer_Composition[boundary_Index][func_Index] = child.node->data.struct_Data.interlayer_Composition[func_Index];
				if(boundary_Interlayer_Composition[boundary_Index][func_Index].enabled)
				{
					if(max_Sigma<boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma.value)
					{
						max_Sigma=boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma.value;
					}
				}
			}
			++boundary_Index;
		}

		if( child.node->data.struct_Data.item_Type == item_Type_Multilayer )
		{
			int per_Index_Amendment = 0;
			if(child.node->data.struct_Data.num_Repetition.value()>=1) per_Index_Amendment=1;
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value(); ++period_Index)
			{
				// TODO optimize for depth 2
				boundary_Index = fill_Sigma(child, max_Sigma, boundary_Index, period_Index+per_Index_Amendment);
			}
		}

		if( child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			Data& regular_Aperiodic = child.node->data.struct_Data;
			for(int period_Index=0; period_Index<regular_Aperiodic.num_Repetition.value(); ++period_Index)
			{
				for(int cell_Index=0; cell_Index<regular_Aperiodic.regular_Components.size(); ++cell_Index)
				{
					sigma       [boundary_Index] = regular_Aperiodic.regular_Components[cell_Index].components[period_Index].sigma.value;
					common_Sigma[boundary_Index] = true;

					for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
					{
						boundary_Interlayer_Composition[boundary_Index][func_Index] = regular_Aperiodic.regular_Components[cell_Index].components[period_Index].interlayer_Composition[func_Index];
						if(boundary_Interlayer_Composition[boundary_Index][func_Index].enabled)
						{
							if(max_Sigma<boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma.value)
							{
								max_Sigma=boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma.value;
							}
						}
					}
					++boundary_Index;
				}
			}
		}
	}
	return boundary_Index;
}

int Unwrapped_Structure::fill_Thickness_And_Boundaries(const tree<Node>::iterator& parent, int layer_Index, int per_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);

		// layers that are not from regular aperiodic
		if( child.node->data.struct_Data.item_Type == item_Type_Layer &&
			child.node->data.struct_Data.parent_Item_Type != item_Type_Regular_Aperiodic )
		{
			// TODO extreme layers
			thickness[layer_Index] = child.node->data.struct_Data.thickness.value;

			// can drift
			Global_Variables::variable_Drift(thickness[layer_Index], child.node->data.struct_Data.thickness_Drift, per_Index, child.node->data.struct_Data.num_Repetition.value(), r);
			boundaries[layer_Index+1] = boundaries[layer_Index] + thickness[layer_Index];

			++layer_Index;
		}

		if( child.node->data.struct_Data.item_Type == item_Type_Multilayer )
		{
			int per_Index_Amendment = 0;
			if(child.node->data.struct_Data.num_Repetition.value()>=1) per_Index_Amendment=1;
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value(); ++period_Index)
			{
				// TODO optimize for depth 2
				layer_Index = fill_Thickness_And_Boundaries(child, layer_Index, period_Index+per_Index_Amendment);
			}
		}

		if( child.node->data.struct_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			Data& regular_Aperiodic = child.node->data.struct_Data;
			for(int period_Index=0; period_Index<regular_Aperiodic.num_Repetition.value(); ++period_Index)
			{
				for(int cell_Index=0; cell_Index<regular_Aperiodic.regular_Components.size(); ++cell_Index)
				{
					thickness[layer_Index] = regular_Aperiodic.regular_Components[cell_Index].components[period_Index].thickness.value;
					boundaries[layer_Index+1] = boundaries[layer_Index] + thickness[layer_Index];
					++layer_Index;
				}
			}
		}
	}
	return layer_Index;
}
