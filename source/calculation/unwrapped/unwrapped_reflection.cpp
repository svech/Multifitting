#include "unwrapped_reflection.h"
#include "iostream"

Unwrapped_Reflection::Unwrapped_Reflection(Unwrapped_Structure* unwrapped_Structure, int num_Media, QString active_Parameter_Whats_This, const Data& measurement, bool depth_Grading, bool sigma_Grading):
	num_Threads		(reflectivity_Calc_Threads),
	num_Layers		(num_Media-2),
	num_Boundaries	(num_Media-1),
	num_Media		(num_Media),
	max_Depth		(unwrapped_Structure->max_Depth),
	active_Parameter_Whats_This(active_Parameter_Whats_This),
	unwrapped_Structure(unwrapped_Structure),
	measurement(measurement),
	depth_Grading(depth_Grading),
	sigma_Grading(sigma_Grading),

	r_Fresnel_s_RE(num_Threads,vector<double>(num_Boundaries)),
	r_Fresnel_s_IM(num_Threads,vector<double>(num_Boundaries)),
	r_Fresnel_p_RE(num_Threads,vector<double>(num_Boundaries)),
	r_Fresnel_p_IM(num_Threads,vector<double>(num_Boundaries)),
	r_Local_s_RE  (num_Threads,vector<double>(num_Boundaries)),
	r_Local_s_IM  (num_Threads,vector<double>(num_Boundaries)),
	r_Local_p_RE  (num_Threads,vector<double>(num_Boundaries)),
	r_Local_p_IM  (num_Threads,vector<double>(num_Boundaries)),
	hi_RE		  (num_Threads,vector<double>(num_Media)),
	hi_IM		  (num_Threads,vector<double>(num_Media)),
	exponenta_RE  (num_Threads,vector<double>(num_Boundaries)),
	exponenta_IM  (num_Threads,vector<double>(num_Boundaries)),

	weak_Factor  (num_Threads,vector<double>(num_Boundaries))
{
	if(active_Parameter_Whats_This == whats_This_Angle)
	{
		num_Points = measurement.cos2.size();
	}
	if(active_Parameter_Whats_This == whats_This_Wavelength)
	{
		num_Points = measurement.lambda.size();
	}

	r_s.resize(num_Points);
	r_p.resize(num_Points);
	R_s.resize(num_Points);
	R_p.resize(num_Points);
	R  .resize(num_Points);
}

int Unwrapped_Reflection::fill_s__Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);

		if(child.node->data.struct_Data.item_Type == item_Type_Ambient)
		{
			hi_RE[thread_Index][media_Index] = child.node->data.hi_RE[point_Index];
			hi_IM[thread_Index][media_Index] = child.node->data.hi_IM[point_Index];
			++media_Index;
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Layer)
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_s_RE[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_s_IM[point_Index] * child.node->data.weak_Factor[point_Index];
			exponenta_RE  [thread_Index][media_Index-1] = child.node->data.exponenta_RE[point_Index];
			exponenta_IM  [thread_Index][media_Index-1] = child.node->data.exponenta_IM[point_Index];
			++media_Index;
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Substrate )
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_s_RE[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_s_IM[point_Index] * child.node->data.weak_Factor[point_Index];
			++media_Index;
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi_RE         [thread_Index][media_Index  ] = grandchild.node->data.hi_RE[point_Index];
					hi_IM         [thread_Index][media_Index  ] = grandchild.node->data.hi_IM[point_Index];
					r_Fresnel_s_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_RE[point_Index] * grandchild.node->data.weak_Factor[point_Index];
					r_Fresnel_s_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_IM[point_Index] * grandchild.node->data.weak_Factor[point_Index];
					exponenta_RE  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_RE[point_Index];
					exponenta_IM  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_IM[point_Index];
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}
int Unwrapped_Reflection::fill_p__Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);

		if(child.node->data.struct_Data.item_Type == item_Type_Ambient)
		{
			hi_RE[thread_Index][media_Index] = child.node->data.hi_RE[point_Index];
			hi_IM[thread_Index][media_Index] = child.node->data.hi_IM[point_Index];
			++media_Index;
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Layer)
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_p_RE[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_p_IM[point_Index] * child.node->data.weak_Factor[point_Index];
			exponenta_RE  [thread_Index][media_Index-1] = child.node->data.exponenta_RE[point_Index];
			exponenta_IM  [thread_Index][media_Index-1] = child.node->data.exponenta_IM[point_Index];
			++media_Index;
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Substrate )
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_p_RE[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_p_IM[point_Index] * child.node->data.weak_Factor[point_Index];
			++media_Index;
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi_RE         [thread_Index][media_Index  ] = grandchild.node->data.hi_RE[point_Index];
					hi_IM         [thread_Index][media_Index  ] = grandchild.node->data.hi_IM[point_Index];
					r_Fresnel_p_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_RE[point_Index] * grandchild.node->data.weak_Factor[point_Index];
					r_Fresnel_p_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_IM[point_Index] * grandchild.node->data.weak_Factor[point_Index];
					exponenta_RE  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_RE[point_Index];
					exponenta_IM  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_IM[point_Index];
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}
int Unwrapped_Reflection::fill_sp_Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);

		if(child.node->data.struct_Data.item_Type == item_Type_Ambient)
		{
			hi_RE[thread_Index][media_Index] = child.node->data.hi_RE[point_Index];
			hi_IM[thread_Index][media_Index] = child.node->data.hi_IM[point_Index];
			++media_Index;
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Layer)
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_s_RE[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_s_IM[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_p_RE[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_p_IM[point_Index] * child.node->data.weak_Factor[point_Index];
			exponenta_RE  [thread_Index][media_Index-1] = child.node->data.exponenta_RE[point_Index];
			exponenta_IM  [thread_Index][media_Index-1] = child.node->data.exponenta_IM[point_Index];
			++media_Index;
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Substrate )
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_s_RE[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_s_IM[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_p_RE[point_Index] * child.node->data.weak_Factor[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_p_IM[point_Index] * child.node->data.weak_Factor[point_Index];
			++media_Index;
		} else
		if(child.node->data.struct_Data.item_Type == item_Type_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.struct_Data.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi_RE         [thread_Index][media_Index  ] = grandchild.node->data.hi_RE[point_Index];
					hi_IM         [thread_Index][media_Index  ] = grandchild.node->data.hi_IM[point_Index];
					r_Fresnel_s_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_RE[point_Index] * grandchild.node->data.weak_Factor[point_Index];
					r_Fresnel_s_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_IM[point_Index] * grandchild.node->data.weak_Factor[point_Index];
					r_Fresnel_p_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_RE[point_Index] * grandchild.node->data.weak_Factor[point_Index];
					r_Fresnel_p_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_IM[point_Index] * grandchild.node->data.weak_Factor[point_Index];
					exponenta_RE  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_RE[point_Index];
					exponenta_IM  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_IM[point_Index];
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}

void Unwrapped_Reflection::calc_Hi(double k, double cos2, const vector<double>& eps_RE, const vector<double>& eps_IM, int thread_Index)
{
	double re, im, phase, mod;
	for (int i = 0; i < num_Media; ++i)
	{
		re = eps_RE[i] - cos2;
		im = eps_IM[i];

		phase = atan2(im, re)/2.;
		mod = k*sqrt(sqrt(re*re + im*im));

		hi_RE[thread_Index][i] = mod*cos(phase);
		hi_IM[thread_Index][i] = mod*sin(phase);
	}
}

void Unwrapped_Reflection::calc_Weak_Factor(int thread_Index)
{
	bool is_Norm = false;
	double norm, s, factor, x, y, a = M_PI/sqrt(M_PI*M_PI - 8.);
	double my_Sigma=0;

	for (int i = 0; i < num_Boundaries; ++i)
	{
		// if >=1 interlayer is turned on
		is_Norm = false;
		for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
		{
			is_Norm = is_Norm || unwrapped_Structure->boundary_Interlayer_Composition[i][func_Index].enabled;
		}

		weak_Factor[thread_Index][i] = 0;
		if(is_Norm && (abs(unwrapped_Structure->sigma[i]) > DBL_MIN)) //-V674
		{
			norm = 0;
			s = sqrt(hi_RE[thread_Index][i+1]*hi_RE[thread_Index][i]);

			//-------------------------------------------------------------------------------
			// erf interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].interlayer.value;
				my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].my_Sigma.value;
				factor = exp( - 2 * s * s * my_Sigma * my_Sigma);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// lin interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].interlayer.value;
				my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].my_Sigma.value;
				x = sqrt(3.) * 2 * s * my_Sigma;
				factor = sin(x) / (x);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// exp interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].interlayer.value;
				my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].my_Sigma.value;
				x = 2 * pow(s * my_Sigma, 2);
				factor = 1. / (1. + x);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// tanh interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].interlayer.value;
				my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].my_Sigma.value;
				x = 2 * sqrt(3.) * s * my_Sigma;
				factor = x / sinh(x);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// sin interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].interlayer.value;
				my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].my_Sigma.value;
				x = 2 * a * s * my_Sigma - M_PI_2;
				y = x + M_PI;
				factor = M_PI_4 * (sin(x)/x + sin(y)/y);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// step interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Step].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Step].interlayer.value;
				my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Step].my_Sigma.value;
				factor = cos(2 * s * my_Sigma);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Step].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// normalization
			{
				if( norm > 0 )
				{
					weak_Factor[thread_Index][i] /= norm;
				}
				else
				{
					weak_Factor[thread_Index][i] = 1;
				}
			}
		} else
		{
			weak_Factor[thread_Index][i] = 1;
		}
	}
}

void Unwrapped_Reflection::calc_Fresnel(double polarization, const vector<double>& eps_RE, const vector<double>& eps_IM, const vector<double>& eps_NORM, int thread_Index)
{
	double temp_Fre_Numer_RE, temp_Fre_Numer_IM, temp_Fre_Denom_SQARE, temp_Fre_Denom_RE, temp_Fre_Denom_IM, temp_1_RE, temp_1_IM, temp_2_RE, temp_2_IM;
	// s-polarization
	if (polarization >-1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		temp_Fre_Denom_RE = hi_RE[thread_Index][i] + hi_RE[thread_Index][i+1];
		temp_Fre_Denom_IM = hi_IM[thread_Index][i] + hi_IM[thread_Index][i+1];
		temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

		if ( abs(temp_Fre_Denom_SQARE) > DBL_MIN )
		{
			temp_Fre_Numer_RE = hi_RE[thread_Index][i] - hi_RE[thread_Index][i+1];
			temp_Fre_Numer_IM = hi_IM[thread_Index][i] - hi_IM[thread_Index][i+1];

			r_Fresnel_s_RE[thread_Index][i] = weak_Factor[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM);
			r_Fresnel_s_IM[thread_Index][i] = weak_Factor[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_IM*temp_Fre_Denom_RE - temp_Fre_Numer_RE*temp_Fre_Denom_IM);
		} else
		{
			r_Fresnel_s_RE[thread_Index][i] = 0;
			r_Fresnel_s_IM[thread_Index][i] = 0;
		}
	}
	// p-polarization
	if (polarization < 1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		temp_1_RE = (hi_RE[thread_Index][i]*eps_RE[i] + hi_IM[thread_Index][i]*eps_IM[i]) / eps_NORM[i];
		temp_1_IM = (hi_IM[thread_Index][i]*eps_RE[i] - hi_RE[thread_Index][i]*eps_IM[i]) / eps_NORM[i];

		temp_2_RE = (hi_RE[thread_Index][i+1]*eps_RE[i+1] + hi_IM[thread_Index][i+1]*eps_IM[i+1]) / eps_NORM[i+1];
		temp_2_IM = (hi_IM[thread_Index][i+1]*eps_RE[i+1] - hi_RE[thread_Index][i+1]*eps_IM[i+1]) / eps_NORM[i+1];

		temp_Fre_Denom_RE = temp_1_RE + temp_2_RE;
		temp_Fre_Denom_IM = temp_1_IM + temp_2_IM;
		temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

		if ( abs(temp_Fre_Denom_SQARE) > DBL_MIN )
		{
			temp_Fre_Numer_RE = temp_1_RE - temp_2_RE;
			temp_Fre_Numer_IM = temp_1_IM - temp_2_IM;

			r_Fresnel_p_RE[thread_Index][i] = weak_Factor[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM);
			r_Fresnel_p_IM[thread_Index][i] = weak_Factor[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_IM*temp_Fre_Denom_RE - temp_Fre_Numer_RE*temp_Fre_Denom_IM);
		} else
		{
			r_Fresnel_p_RE[thread_Index][i] = 0;
			r_Fresnel_p_IM[thread_Index][i] = 0;
		}
	}
}

void Unwrapped_Reflection::calc_Exponenta(int thread_Index)
{
	double re, im, ere;
	for (int i = 0; i < num_Layers; ++i)
	{
		re = -2.*hi_IM[thread_Index][i+1]*unwrapped_Structure->thickness[i];
		im =  2.*hi_RE[thread_Index][i+1]*unwrapped_Structure->thickness[i];
		ere = exp(re);

		exponenta_RE[thread_Index][i] = ere*cos(im);
		exponenta_IM[thread_Index][i] = ere*sin(im);
	}
}

void Unwrapped_Reflection::calc_Local(double polarization, int thread_Index)
{
	double loc_Denom_RE, loc_Denom_IM, loc_Denom_SQUARE, temp_RE, temp_IM, loc_Numer_RE, loc_Numer_IM;
	// s-polarization
	if (polarization >-1)
	{
		r_Local_s_RE[thread_Index].back() = r_Fresnel_s_RE[thread_Index].back();	// last boundary
		r_Local_s_IM[thread_Index].back() = r_Fresnel_s_IM[thread_Index].back();	// last boundary

		for (int i = num_Layers-1; i >= 0; --i)
		{
			temp_RE = r_Local_s_RE[thread_Index][i+1]*exponenta_RE[thread_Index][i] - r_Local_s_IM[thread_Index][i+1]*exponenta_IM[thread_Index][i];
			temp_IM = r_Local_s_IM[thread_Index][i+1]*exponenta_RE[thread_Index][i] + r_Local_s_RE[thread_Index][i+1]*exponenta_IM[thread_Index][i];
			loc_Denom_RE = 1. + (temp_RE*r_Fresnel_s_RE[thread_Index][i] - temp_IM*r_Fresnel_s_IM[thread_Index][i]);
			loc_Denom_IM =       temp_IM*r_Fresnel_s_RE[thread_Index][i] + temp_RE*r_Fresnel_s_IM[thread_Index][i];
			loc_Denom_SQUARE = loc_Denom_RE*loc_Denom_RE + loc_Denom_IM*loc_Denom_IM;

			loc_Numer_RE = r_Fresnel_s_RE[thread_Index][i] + temp_RE;
			loc_Numer_IM = r_Fresnel_s_IM[thread_Index][i] + temp_IM;

			r_Local_s_RE[thread_Index][i] = (loc_Numer_RE*loc_Denom_RE + loc_Numer_IM*loc_Denom_IM) / loc_Denom_SQUARE;
			r_Local_s_IM[thread_Index][i] = (loc_Numer_IM*loc_Denom_RE - loc_Numer_RE*loc_Denom_IM) / loc_Denom_SQUARE;
		}
	}
	// p-polarization
	if (polarization <1)
	{
		r_Local_p_RE[thread_Index].back() = r_Fresnel_p_RE[thread_Index].back();	// last boundary
		r_Local_p_IM[thread_Index].back() = r_Fresnel_p_IM[thread_Index].back();	// last boundary

		for (int i = num_Layers-1; i >= 0; --i)
		{
			temp_RE = r_Local_p_RE[thread_Index][i+1]*exponenta_RE[thread_Index][i] - r_Local_p_IM[thread_Index][i+1]*exponenta_IM[thread_Index][i];
			temp_IM = r_Local_p_IM[thread_Index][i+1]*exponenta_RE[thread_Index][i] + r_Local_p_RE[thread_Index][i+1]*exponenta_IM[thread_Index][i];
			loc_Denom_RE = 1. + (temp_RE*r_Fresnel_p_RE[thread_Index][i] - temp_IM*r_Fresnel_p_IM[thread_Index][i]);
			loc_Denom_IM =       temp_IM*r_Fresnel_p_RE[thread_Index][i] + temp_RE*r_Fresnel_p_IM[thread_Index][i];
			loc_Denom_SQUARE = loc_Denom_RE*loc_Denom_RE + loc_Denom_IM*loc_Denom_IM;

			loc_Numer_RE = r_Fresnel_p_RE[thread_Index][i] + temp_RE;
			loc_Numer_IM = r_Fresnel_p_IM[thread_Index][i] + temp_IM;

			r_Local_p_RE[thread_Index][i] = (loc_Numer_RE*loc_Denom_RE + loc_Numer_IM*loc_Denom_IM) / loc_Denom_SQUARE;
			r_Local_p_IM[thread_Index][i] = (loc_Numer_IM*loc_Denom_RE - loc_Numer_RE*loc_Denom_IM) / loc_Denom_SQUARE;
		}
	}
}

void Unwrapped_Reflection::multifly_Fresnel_And_Weak_Factor(double polarization, int thread_Index)
{
	// s-polarization
	if (polarization >-1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		r_Fresnel_s_RE[thread_Index][i] *= weak_Factor[thread_Index][i];
		r_Fresnel_s_IM[thread_Index][i] *= weak_Factor[thread_Index][i];
	}

	// p-polarization
	if (polarization < 1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		r_Fresnel_p_RE[thread_Index][i] *= weak_Factor[thread_Index][i];
		r_Fresnel_p_IM[thread_Index][i] *= weak_Factor[thread_Index][i];
	}
}

void Unwrapped_Reflection::calc_Specular_1_Point_1_Thread(const Data& measurement, int thread_Index, int point_Index)
{
//	auto start = std::chrono::system_clock::now();
	// PARAMETER

	if( max_Depth <= 2 )
	{
		if( abs(measurement.polarization.value - 1) < DBL_EPSILON )			// s-polarization only
		{
			fill_s__Max_Depth_2(unwrapped_Structure->calc_Tree.begin(), thread_Index, point_Index);
		} else
		if( abs(measurement.polarization.value + 1) < DBL_EPSILON )			// p-polarization only
		{
			fill_p__Max_Depth_2(unwrapped_Structure->calc_Tree.begin(), thread_Index, point_Index);
		} else																// both polarizations
		{
			fill_sp_Max_Depth_2(unwrapped_Structure->calc_Tree.begin(), thread_Index, point_Index);
		}

		// if we have some grading
		if( depth_Grading )
		{
			calc_Exponenta  (thread_Index);
		}
		if( sigma_Grading )
		{
			calc_Weak_Factor(thread_Index);
			multifly_Fresnel_And_Weak_Factor(measurement.polarization.value, thread_Index);
		}
	} else
	{
		if(active_Parameter_Whats_This == whats_This_Angle)
		{
			calc_Hi(measurement.k_Value, measurement.cos2[point_Index], unwrapped_Structure->epsilon_RE, unwrapped_Structure->epsilon_IM, thread_Index);
			calc_Weak_Factor(thread_Index);
			calc_Exponenta  (thread_Index);
			calc_Fresnel(measurement.polarization.value, unwrapped_Structure->epsilon_RE,
														 unwrapped_Structure->epsilon_IM,
														 unwrapped_Structure->epsilon_NORM,
																							thread_Index);
		}
		if(active_Parameter_Whats_This == whats_This_Wavelength)
		{
			calc_Hi(measurement.k[point_Index], measurement.cos2_Value, unwrapped_Structure->epsilon_Dependent_RE[point_Index], unwrapped_Structure->epsilon_Dependent_IM[point_Index], thread_Index);
			calc_Weak_Factor(thread_Index);
			calc_Exponenta  (thread_Index);
			calc_Fresnel(measurement.polarization.value, unwrapped_Structure->epsilon_Dependent_RE  [point_Index],
														 unwrapped_Structure->epsilon_Dependent_IM  [point_Index],
														 unwrapped_Structure->epsilon_Dependent_NORM[point_Index],
																												   thread_Index);
		}
	}
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Pre    : "<< elapsed.count()/1000. << " seconds";

//	start = std::chrono::system_clock::now();
	calc_Local(measurement.polarization.value, thread_Index);

//	auto enD = std::chrono::system_clock::now();
//	auto elapseD = std::chrono::duration_cast<std::chrono::milliseconds>(enD - start);
//	qInfo() << "Local  : "<< elapseD.count()/1000. << " seconds";
}

void Unwrapped_Reflection::fill_Specular_Values(const Data& measurement, int thread_Index, int point_Index)
{
	double s_Weight = (1. + measurement.polarization.value) / 2.;
	double p_Weight = (1. - measurement.polarization.value) / 2.;

	r_s[point_Index] = complex<double>(r_Local_s_RE[thread_Index][0], r_Local_s_IM[thread_Index][0]);
	r_p[point_Index] = complex<double>(r_Local_p_RE[thread_Index][0], r_Local_p_IM[thread_Index][0]);

	R_s[point_Index] = pow(abs(r_s[point_Index]),2);
	R_p[point_Index] = pow(abs(r_p[point_Index]),2);
	R  [point_Index] = s_Weight * R_s[point_Index] + p_Weight * R_p[point_Index];
	if(R[point_Index]!=R[point_Index]) {R[point_Index]=1;} // NaN to 1. Be careful!
}

void Unwrapped_Reflection::calc_Specular_nMin_nMax_1_Thread(const Data& measurement, int n_Min, int n_Max, int thread_Index)
{
	for(int point_Index = n_Min; point_Index<n_Max; ++point_Index)
	{
		calc_Specular_1_Point_1_Thread(measurement, thread_Index, point_Index);
		fill_Specular_Values		  (measurement, thread_Index, point_Index);
	}
}

void Unwrapped_Reflection::calc_Specular()
{
	/// ----------------------------------------------------------------------------------------------------------------------
	/// parallelization
	/// ----------------------------------------------------------------------------------------------------------------------
	vector<thread> workers(num_Threads);
	int delta_N = num_Points / num_Threads;	// number of points per thread

	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		int n_Min = thread_Index*delta_N;
		int n_Max = (thread_Index + 1)*delta_N;
		if(thread_Index == (num_Threads-1))
		{
			n_Max = num_Points;
		}
		workers[thread_Index] = thread(&Unwrapped_Reflection::calc_Specular_nMin_nMax_1_Thread, this, measurement, n_Min, n_Max, thread_Index);
	}
	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		if (workers[thread_Index].joinable()) workers[thread_Index].join();	// присоединение потоков
	}
	/// ----------------------------------------------------------------------------------------------------------------------
}
