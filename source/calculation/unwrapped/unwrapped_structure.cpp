#include "unwrapped_structure.h"

Unwrapped_Structure::Unwrapped_Structure(const tree<Node>& calc_Tree, const Data& measurement, QString active_Parameter_Whats_This, int num_Media, int max_Depth, bool depth_Grading, bool sigma_Grading, Discretization_Parameters discretization_Parameters, gsl_rng* r):
	r(r),
	num_Threads		(epsilon_Partial_Fill_Threads),
	num_Media		(num_Media),
	num_Boundaries	(num_Media - 1),
	num_Layers		(num_Media - 2),
	max_Depth		(max_Depth),
	depth_Grading	(depth_Grading),
	sigma_Grading	(sigma_Grading),	
	discretization_Parameters(discretization_Parameters),
	calc_Tree		(calc_Tree)
{
	int depth_Threshold = 2;

	// recalculate all if depth is big
	if( max_Depth > depth_Threshold || discretization_Parameters.enable_Discretization)
	{		
		// PARAMETER
		if(active_Parameter_Whats_This == whats_This_Wavelength)
		{
			int num_Lambda_Points = measurement.lambda.size();

			epsilon_Dependent.resize(num_Lambda_Points, vector<complex<double>>(num_Media));
			epsilon_Dependent_RE.resize(num_Lambda_Points, vector<double>(num_Media));
			epsilon_Dependent_IM.resize(num_Lambda_Points, vector<double>(num_Media));
			epsilon_Dependent_NORM.resize(num_Lambda_Points, vector<double>(num_Media));

			fill_Epsilon_Dependent(calc_Tree.begin(), num_Lambda_Points);
		} else
		{
			epsilon.resize(num_Media);
			epsilon_RE.resize(num_Media);
			epsilon_IM.resize(num_Media);
			epsilon_NORM.resize(num_Media);
			fill_Epsilon(calc_Tree.begin());
		}
	}
	// recalculate sigmas if depth is big or sigma grading
	if( (max_Depth > depth_Threshold) || sigma_Grading || discretization_Parameters.enable_Discretization)
	{
		max_Sigma = 0.1;
		sigma.resize(num_Boundaries);
		common_Sigma.resize(num_Boundaries);
		boundary_Interlayer_Composition.resize(num_Boundaries, vector<Interlayer>(transition_Layer_Functions_Size));
		fill_Sigma(calc_Tree.begin(), max_Sigma);
	}

	// recalculate thicknesses if depth is big or depth grading
	if( (max_Depth > depth_Threshold) || depth_Grading || discretization_Parameters.enable_Discretization)
	{
		thickness.resize(num_Layers);
		boundaries.resize(num_Boundaries);
		boundaries.front() = 0;
		fill_Thickness_And_Boundaries(calc_Tree.begin());
	} /*else
	{
		epsilon.resize(num_Media);
		epsilon_Norm.resize(num_Media);
		epsilon_RE.resize(num_Media);
		epsilon_IM.resize(num_Media);

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

		// prolong discretization into ambient and substrate
		Global_Variables::get_Prefix_Suffix(prefix, suffix, max_Sigma);
		Global_Variables::discretize_Prefix_Suffix(prefix, suffix, num_Prefix_Slices, num_Suffix_Slices, discretized_Thickness);
		size_t num_Slices_Media = discretized_Thickness.size()+2;

		// PARAMETER
		if(active_Parameter_Whats_This == whats_This_Wavelength)
		{
			int num_Lambda_Points = measurement.lambda.size();

			discretized_Epsilon_Dependent.resize(num_Lambda_Points, vector<complex<double>>(num_Slices_Media));
			discretized_Epsilon_Dependent_RE.resize(num_Lambda_Points, vector<double>(num_Slices_Media));
			discretized_Epsilon_Dependent_IM.resize(num_Lambda_Points, vector<double>(num_Slices_Media));
			discretized_Epsilon_Dependent_NORM.resize(num_Lambda_Points, vector<double>(num_Slices_Media));

			fill_Discretized_Epsilon_Dependent(num_Lambda_Points);
		} else
		{
			discretized_Epsilon.resize(num_Slices_Media);
			discretized_Epsilon_RE.resize(num_Slices_Media);
			discretized_Epsilon_IM.resize(num_Slices_Media);
			discretized_Epsilon_NORM.resize(num_Slices_Media);

			fill_Discretized_Epsilon();
		}
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

void Unwrapped_Structure::fill_Discretized_Epsilon()
{
	// ambient
	discretized_Epsilon.front() = epsilon.front();
	discretized_Epsilon_RE.front() = epsilon_RE.front();
	discretized_Epsilon_IM.front() = epsilon_IM.front();
	discretized_Epsilon_NORM.front() = epsilon_NORM.front();

	// main part
	double real_Z = -(num_Prefix_Slices-0.5)*discretized_Thickness.front();
	for(int i=1; i<discretized_Thickness.size(); ++i)
	{
//		double visible_Z = real_Z-discretized_Thickness[i]/2.; // where we dispose point
		discretized_Epsilon[i] = real(epsilon_Func(real_Z));

		if(i!=(discretized_Thickness.size()-1)) {
			real_Z += (discretized_Thickness[i]+discretized_Thickness[i+1])/2.; // real z, where we calculate epsilon
		}
	}

	// substrate
	discretized_Epsilon.back() = epsilon.back();
	discretized_Epsilon_RE.back() = epsilon_RE.back();
	discretized_Epsilon_IM.back() = epsilon_IM.back();
	discretized_Epsilon_NORM.back() = epsilon_NORM.back();
}

void Unwrapped_Structure::fill_Discretized_Epsilon_Dependent(int num_Lambda_Points)
{

}

complex<double> Unwrapped_Structure::epsilon_Func(double z)
{
	// where we are
	int sigma_Factor = 6;
	std::vector<double>::iterator it_low = std::lower_bound(boundaries.begin(), boundaries.end(), z-sigma_Factor*max_Sigma);
	std::vector<double>::iterator it_up  = std::upper_bound(boundaries.begin(), boundaries.end(), z+sigma_Factor*max_Sigma);

	int min_Boundary_Index = min(max(int(it_low-boundaries.begin())-1, 0), thickness.size()-1);
	int max_Boundary_Index = min(    int(it_up -boundaries.begin()),       thickness.size()-1);

	double delta_Epsilon = 0;
	double beta_Epsilon = 0;
	double geometry_Factor = 1;

	for(int j=min_Boundary_Index; j<=max_Boundary_Index; j++) // instead of old slow for(int j=0; j<thickness_Vector.size(); j++)
	{
		geometry_Factor = layer_Norm_Vector[j] *
				Global_Variables::interface_Profile_Function(z-boundaries[j  ], struct_Data_Vector[j+1].interlayer_Composition) *
				Global_Variables::interface_Profile_Function(boundaries[j+1]-z, struct_Data_Vector[j+2].interlayer_Composition);

		delta_Epsilon += delta_Epsilon_Vector[j+1] * geometry_Factor;
		beta_Epsilon  += beta_Epsilon_Vector [j+1] * geometry_Factor;
	}
	if(max_Boundary_Index>=thickness.size()-1)
	{
		geometry_Factor = Global_Variables::interface_Profile_Function(z-boundaries.last(), struct_Data_Vector.last().interlayer_Composition);

		delta_Epsilon += delta_Epsilon_Vector.last() * geometry_Factor;
		beta_Epsilon  += beta_Epsilon_Vector. last() * geometry_Factor;
	}

	return complex<double>(delta_Epsilon,beta_Epsilon);
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
					epsilon_RE  [media_Index] = child.node->data.epsilon_RE  .front();
					epsilon_IM  [media_Index] = child.node->data.epsilon_IM  .front();
					epsilon_NORM[media_Index] = child.node->data.epsilon_NORM.front();
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
					epsilon_RE  [media_Index] = grandchild.node->data.epsilon_RE  .front();
					epsilon_IM  [media_Index] = grandchild.node->data.epsilon_IM  .front();
					epsilon_NORM[media_Index] = grandchild.node->data.epsilon_NORM.front();
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
					epsilon_Dependent_RE  [point_Index][media_Index] = child.node->data.epsilon_RE  [point_Index];
					epsilon_Dependent_IM  [point_Index][media_Index] = child.node->data.epsilon_IM  [point_Index];
					epsilon_Dependent_NORM[point_Index][media_Index] = child.node->data.epsilon_NORM[point_Index];
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
						epsilon_Dependent_RE  [point_Index][media_Index] = grandchild.node->data.epsilon_RE  [point_Index];
						epsilon_Dependent_IM  [point_Index][media_Index] = grandchild.node->data.epsilon_IM  [point_Index];
						epsilon_Dependent_NORM[point_Index][media_Index] = grandchild.node->data.epsilon_NORM[point_Index];
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
			boundaries[layer_Index+1] = boundaries[layer_Index] + thickness[layer_Index];

			// can drift
			Global_Variables::variable_Drift(thickness[layer_Index], child.node->data.struct_Data.thickness_Drift, per_Index, child.node->data.struct_Data.num_Repetition.value(), r);

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
