// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "unwrapped_structure.h"

Unwrapped_Structure::Unwrapped_Structure() //-V730
{

}

Unwrapped_Structure::Unwrapped_Structure(tree<Node>* calc_Tree, const tree<Node>::iterator& active_Iter, QString active_Whats_This, int num_Media, int max_Depth, bool depth_Grading, bool sigma_Grading, gsl_rng* r):
	calc_Tree		(calc_Tree),
	num_Threads		(epsilon_Partial_Fill_Threads),
	num_Media		(num_Media),
	num_Boundaries	(num_Media - 1),
	num_Layers		(num_Media - 2),
	max_Depth		(max_Depth),
	depth_Grading	(depth_Grading),
	sigma_Grading	(sigma_Grading),
	r(r)
{	
	// recalculate all if depth is big
	if( max_Depth > 2 )
	{		
		QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

		// PARAMETER
		if(active_Whats_This_List[1] == whats_This_Wavelength )
		{
			Measurement* measur = &active_Iter.node->data.measurement;
			int num_Lambda_Points = measur->lambda.size();

			epsilon_Dependent.resize(num_Lambda_Points, vector<complex<double>>(num_Media));
			epsilon_Dependent_RE.resize(num_Lambda_Points, vector<double>(num_Media));
			epsilon_Dependent_IM.resize(num_Lambda_Points, vector<double>(num_Media));
			epsilon_Dependent_NORM.resize(num_Lambda_Points, vector<double>(num_Media));

			fill_Epsilon_Dependent(calc_Tree->begin(), num_Lambda_Points);
		} else
		{
			epsilon.resize(num_Media);
			epsilon_RE.resize(num_Media);
			epsilon_IM.resize(num_Media);
			epsilon_NORM.resize(num_Media);
			fill_Epsilon(calc_Tree->begin());
		}
	}
	// recalculate sigmas if depth is big or sigma grading
	if( (max_Depth > 2) || sigma_Grading )
	{
		sigma.resize(num_Boundaries);
		boundary_Interlayer_Composition.resize(num_Boundaries, vector<Interlayer>(transition_Layer_Functions_Size));
		fill_Sigma		(calc_Tree->begin());
	}

	// recalculate thicknesses if depth is big or depth grading
	if( (max_Depth > 2) || depth_Grading )
	{
		thickness.resize(num_Layers);
		fill_Thickness	(calc_Tree->begin());
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

void Unwrapped_Structure::variable_Drift(double& value, Drift& drift, int period_Index)
{
	double drift_Factor = 1;
	if(drift.is_Drift_Line)
	{
		drift_Factor += drift.drift_Line_Value.value*period_Index/100.;
	}
	if(drift.is_Drift_Sine)
	{
		drift_Factor += drift.drift_Sine_Amplitude.value*sin(2*M_PI*(period_Index*drift.drift_Sine_Frequency.value + drift.drift_Sine_Phase.value))/100.;
	}
	if(drift.is_Drift_Rand)
	{
		drift_Factor += gsl_ran_gaussian(r, drift.drift_Rand_Rms.value)/100.;
	}
	if(drift_Factor>=0)
	{
		value *= drift_Factor;
	} else
	{
		// TODO stop calculation and show error!
		qInfo() << "==========\nUnwrapped_Structure::variable_Drift  :  negative drifted variable!\n==========";
		value *= abs(drift_Factor);
	}
}

int Unwrapped_Structure::fill_Epsilon(const tree<Node>::iterator& parent, int media_Index)
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
				epsilon     [media_Index] = child.node->data.epsilon     .front();
				epsilon_RE  [media_Index] = child.node->data.epsilon_RE  .front();
				epsilon_IM  [media_Index] = child.node->data.epsilon_IM  .front();
				epsilon_NORM[media_Index] = child.node->data.epsilon_NORM.front();
			++media_Index;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				media_Index = fill_Epsilon(child, media_Index);
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
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient ||
		   whats_This_List[0] == whats_This_Layer   ||
		   whats_This_List[0] == whats_This_Substrate )
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

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				media_Index = fill_Epsilon_Dependent(child, num_Lambda_Points, media_Index);
			}
		}
	}
	return media_Index;
}

int Unwrapped_Structure::fill_Sigma(const tree<Node>::iterator& parent, int boundary_Index, int per_Index)
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
//				if(child.node->data.layer.interlayer_Composition[func_Index].enabled)
				{
					boundary_Interlayer_Composition[boundary_Index][func_Index] = child.node->data.layer.interlayer_Composition[func_Index];

					// can drift
					variable_Drift(boundary_Interlayer_Composition[boundary_Index][func_Index].my_Sigma.value, child.node->data.layer.sigma_Drift,	per_Index);
				}
			}
			++boundary_Index;
		}
		if(whats_This_List[0] == whats_This_Substrate)
		{
			// TODO extreme layers
			sigma[boundary_Index] = child.node->data.substrate.sigma.value;

			for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
			{
//				if(child.node->data.substrate.interlayer_Composition[func_Index].enabled)
				{
					boundary_Interlayer_Composition[boundary_Index][func_Index] = child.node->data.substrate.interlayer_Composition[func_Index];
				}
			}
			++boundary_Index;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				boundary_Index = fill_Sigma(child, boundary_Index, period_Index);
			}
		}
	}
	return boundary_Index;
}

int Unwrapped_Structure::fill_Thickness(const tree<Node>::iterator& parent, int layer_Index, int per_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Layer)
		{
			// TODO extreme layers
			thickness[layer_Index] = child.node->data.layer.thickness.value;

			// can drift
			variable_Drift(thickness[layer_Index], child.node->data.layer.thickness_Drift, per_Index);

			++layer_Index;
		}

		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				layer_Index = fill_Thickness(child, layer_Index, period_Index);
			}
		}
	}
	return layer_Index;
}
