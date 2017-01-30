// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "unwrapped_reflection.h"
#include "iostream"

Unwrapped_Reflection::Unwrapped_Reflection() //-V730
{

}

Unwrapped_Reflection::Unwrapped_Reflection(Unwrapped_Structure* unwrapped_Structure, int num_Media, QString active_Whats_This, Measurement& measurement) :
	num_Threads		(reflectivity_Calc_Threads),
	num_Layers		(num_Media-2),
	num_Boundaries	(num_Media-1),
	num_Media		(num_Media),
	max_Depth		(unwrapped_Structure->max_Depth),
	active_Whats_This(active_Whats_This),
	unwrapped_Structure(unwrapped_Structure),
	measurement(measurement),

#ifndef REAL_VALUED    // complex-valued
	r_Fresnel_s	(num_Threads,vector<complex<double>>(num_Boundaries)),
	r_Fresnel_p	(num_Threads,vector<complex<double>>(num_Boundaries)),
	r_Local_s	(num_Threads,vector<complex<double>>(num_Boundaries)),
	r_Local_p	(num_Threads,vector<complex<double>>(num_Boundaries)),
	hi			(num_Threads,vector<complex<double>>(num_Media)),
	exponenta	(num_Threads,vector<complex<double>>(num_Layers)),
#else	               // real-valued
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
#endif
	weak_Factor  (num_Threads,vector<double>(num_Boundaries))
{
	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// if angle is changing
	if(active_Whats_This_List[1] == whats_This_Angle)
	{
		r_s.resize(measurement.cos2.size());
		r_p.resize(measurement.cos2.size());
		R_s.resize(measurement.cos2.size());
		R_p.resize(measurement.cos2.size());
		R  .resize(measurement.cos2.size());
	}
	// TODO spectral
}

#ifndef REAL_VALUED    // complex-valued

int Unwrapped_Reflection::fill_s__Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient)
		{
			hi[thread_Index][media_Index] = child.node->data.hi_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Layer)
		{
			hi		   [thread_Index][media_Index  ] = child.node->data.hi_Ang[point_Index];
			r_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			exponenta  [thread_Index][media_Index-1] = child.node->data.exponenta_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Substrate )
		{
			hi		   [thread_Index][media_Index  ] = child.node->data.hi_Ang[point_Index];
			r_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi	       [thread_Index][media_Index  ] = grandchild.node->data.hi_Ang       [point_Index];
					r_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_Ang[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					exponenta  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_Ang[point_Index];
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
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient)
		{
			hi[thread_Index][media_Index] = child.node->data.hi_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Layer)
		{
			hi		   [thread_Index][media_Index  ] = child.node->data.hi_Ang[point_Index];
			r_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			exponenta  [thread_Index][media_Index-1] = child.node->data.exponenta_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Substrate )
		{
			hi		   [thread_Index][media_Index  ] = child.node->data.hi_Ang[point_Index];
			r_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi         [thread_Index][media_Index  ] = grandchild.node->data.hi_Ang       [point_Index];
					r_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_Ang[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					exponenta  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_Ang[point_Index];
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
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient)
		{
			hi[thread_Index][media_Index] = child.node->data.hi_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Layer)
		{
			hi         [thread_Index][media_Index  ] = child.node->data.hi_Ang[point_Index];
			r_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			exponenta  [thread_Index][media_Index-1] = child.node->data.exponenta_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Substrate )
		{
			hi		   [thread_Index][media_Index  ] = child.node->data.hi_Ang[point_Index];
			r_Fresnel_s[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_p[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi	       [thread_Index][media_Index  ] = grandchild.node->data.hi_Ang       [point_Index];
					r_Fresnel_s[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_Ang[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					r_Fresnel_p[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_Ang[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					exponenta  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_Ang[point_Index];
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}

void Unwrapped_Reflection::calc_Hi(double k, double cos2, int thread_Index)
{
	double re, im, phase, mod;
	complex<double> com;
	for (int i = 0; i < num_Media; ++i)
	{
		com = unwrapped_Structure->epsilon[i] - cos2;
		re = real(com);
		im = imag(com);

		phase = atan2(im, re)/2;
		mod = k*sqrt(sqrt(re*re + im*im));

		hi[thread_Index][i] = mod*(cos(phase) + I*sin(phase));
	}
}

void Unwrapped_Reflection::calc_Exponenta(int thread_Index)
{
	double re, im;
	complex<double> com;
	for (int i = 0; i < num_Layers; ++i)
	{
		com = 2.*(I*hi[thread_Index][i+1])*unwrapped_Structure->thickness[i];
		re = real(com);
		im = imag(com);

		exponenta[thread_Index][i] = exp(re)*(cos(im) + I*sin(im));
	}
}

void Unwrapped_Reflection::calc_Fresnel(double polarization, int thread_Index)
{
	complex<double> temp_Fre_Numer, temp_Fre_Denom, temp_1, temp_2;
	// s-polarization
	if (polarization >-1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		temp_Fre_Denom = hi[thread_Index][i] + hi[thread_Index][i+1];
		if ( abs(temp_Fre_Denom) > DBL_EPSILON )
		{
			temp_Fre_Numer = hi[thread_Index][i] - hi[thread_Index][i+1];
			r_Fresnel_s[thread_Index][i] = weak_Factor[thread_Index][i] * temp_Fre_Numer / temp_Fre_Denom;
		} else
		{
			r_Fresnel_s[thread_Index][i] = 0;
		}
	}
	// p-polarization
	if (polarization < 1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		temp_1 = hi[thread_Index][i  ]/unwrapped_Structure->epsilon[i  ];
		temp_2 = hi[thread_Index][i+1]/unwrapped_Structure->epsilon[i+1];
		temp_Fre_Denom = temp_1 + temp_2;
		if ( abs(temp_Fre_Denom) > DBL_EPSILON )
		{
			temp_Fre_Numer = temp_1 - temp_2;
			r_Fresnel_p[thread_Index][i] = weak_Factor[thread_Index][i] * temp_Fre_Numer / temp_Fre_Denom;
		} else
		{
			r_Fresnel_p[thread_Index][i] = 0;
		}
	}
}

void Unwrapped_Reflection::calc_Local(double polarization, int thread_Index)
{
	complex<double> loc_Denom;
	// s-polarization
	if (polarization >-1)
	{
		r_Local_s[thread_Index].back() = r_Fresnel_s[thread_Index].back();	// last boundary
		for (int i = num_Layers-1; i >= 0; --i)
		{
			loc_Denom = 1. + r_Fresnel_s[thread_Index][i]*r_Local_s[thread_Index][i+1]*exponenta[thread_Index][i];
			r_Local_s[thread_Index][i] = (r_Fresnel_s[thread_Index][i] + r_Local_s[thread_Index][i+1]*exponenta[thread_Index][i]) / loc_Denom;
		}
	}
	// p-polarization
	if (polarization <1)
	{
		r_Local_p[thread_Index].back() = r_Fresnel_p[thread_Index].back();	// last boundary
		for (int i = num_Layers-1; i >= 0; --i)
		{
			loc_Denom = 1. + r_Fresnel_p[thread_Index][i]*r_Local_p[thread_Index][i+1]*exponenta[thread_Index][i];
			r_Local_p[thread_Index][i] = (r_Fresnel_p[thread_Index][i] + r_Local_p[thread_Index][i+1]*exponenta[thread_Index][i]) / loc_Denom;
		}
	}
}

#else	               // real-valued

int Unwrapped_Reflection::fill_s__Real_Val_Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient)
		{
			hi_RE[thread_Index][media_Index] = child.node->data.hi_Ang_RE[point_Index];
			hi_IM[thread_Index][media_Index] = child.node->data.hi_Ang_IM[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Layer)
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_Ang_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_Ang_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang_RE[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang_IM[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			exponenta_RE  [thread_Index][media_Index-1] = child.node->data.exponenta_Ang_RE[point_Index];
			exponenta_IM  [thread_Index][media_Index-1] = child.node->data.exponenta_Ang_IM[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Substrate )
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_Ang_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_Ang_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang_RE[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang_IM[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi_RE         [thread_Index][media_Index  ] = grandchild.node->data.hi_Ang_RE[point_Index];
					hi_IM         [thread_Index][media_Index  ] = grandchild.node->data.hi_Ang_IM[point_Index];
					r_Fresnel_s_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_Ang_RE[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					r_Fresnel_s_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_Ang_IM[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					exponenta_RE  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_Ang_RE[point_Index];
					exponenta_IM  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_Ang_IM[point_Index];
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}
int Unwrapped_Reflection::fill_p__Real_Val_Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient)
		{
			hi_RE[thread_Index][media_Index] = child.node->data.hi_Ang_RE[point_Index];
			hi_IM[thread_Index][media_Index] = child.node->data.hi_Ang_IM[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Layer)
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_Ang_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_Ang_IM[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang_RE[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang_IM[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			exponenta_RE  [thread_Index][media_Index-1] = child.node->data.exponenta_Ang_RE[point_Index];
			exponenta_IM  [thread_Index][media_Index-1] = child.node->data.exponenta_Ang_IM[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Substrate )
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_Ang_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_Ang_IM[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang_RE[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang_IM[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi_RE         [thread_Index][media_Index  ] = grandchild.node->data.hi_Ang_RE[point_Index];
					hi_IM         [thread_Index][media_Index  ] = grandchild.node->data.hi_Ang_IM[point_Index];
					r_Fresnel_p_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_Ang_RE[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					r_Fresnel_p_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_Ang_IM[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					exponenta_RE  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_Ang_RE[point_Index];
					exponenta_IM  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_Ang_IM[point_Index];
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}
int Unwrapped_Reflection::fill_sp_Real_Val_Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		QStringList whats_This_List = child.node->data.whats_This_List;

		if(whats_This_List[0] == whats_This_Ambient)
		{
			hi_RE[thread_Index][media_Index] = child.node->data.hi_Ang_RE[point_Index];
			hi_IM[thread_Index][media_Index] = child.node->data.hi_Ang_IM[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Layer)
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_Ang_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_Ang_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang_RE[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang_IM[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang_RE[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang_IM[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			exponenta_RE  [thread_Index][media_Index-1] = child.node->data.exponenta_Ang_RE[point_Index];
			exponenta_IM  [thread_Index][media_Index-1] = child.node->data.exponenta_Ang_IM[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Substrate )
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_Ang_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_Ang_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang_RE[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_s_Ang_IM[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang_RE[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_p_Ang_IM[point_Index] * child.node->data.weak_Factor_Ang[point_Index];
			++media_Index;
		} else
		if(whats_This_List[0] == whats_This_Multilayer)
		{
			for(int period_Index=0; period_Index<child.node->data.stack_Content.num_Repetition.value; ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi_RE         [thread_Index][media_Index  ] = grandchild.node->data.hi_Ang_RE[point_Index];
					hi_IM         [thread_Index][media_Index  ] = grandchild.node->data.hi_Ang_IM[point_Index];
					r_Fresnel_s_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_Ang_RE[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					r_Fresnel_s_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_s_Ang_IM[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					r_Fresnel_p_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_Ang_RE[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					r_Fresnel_p_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_p_Ang_IM[point_Index] * grandchild.node->data.weak_Factor_Ang[point_Index];
					exponenta_RE  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_Ang_RE[point_Index];
					exponenta_IM  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_Ang_IM[point_Index];
					++media_Index;
				}
			}
		}
	}
	return media_Index;
}

void Unwrapped_Reflection::calc_Hi_Real_Val(double k, double cos2, int thread_Index)
{
	double re, im, phase, mod;
	for (int i = 0; i < num_Media; ++i)
	{
		re = unwrapped_Structure->epsilon_RE[i] - cos2;
		im = unwrapped_Structure->epsilon_IM[i];

		phase = atan2(im, re)/2;
		mod = k*sqrt(sqrt(re*re + im*im));

		hi_RE[thread_Index][i] = mod*cos(phase);
		hi_IM[thread_Index][i] = mod*sin(phase);
	}
}

void Unwrapped_Reflection::calc_Exponenta_Real_Val(int thread_Index)
{
	double re, im, recom, imcom, ere;
	for (int i = 0; i < num_Layers; ++i)
	{
		recom = -2.*hi_IM[thread_Index][i+1];
		imcom =  2.*hi_RE[thread_Index][i+1];
		re = recom*unwrapped_Structure->thickness[i];
		im = imcom*unwrapped_Structure->thickness[i];
		ere = exp(re);

		exponenta_RE[thread_Index][i] = ere*cos(im);
		exponenta_IM[thread_Index][i] = ere*sin(im);
	}
}

void Unwrapped_Reflection::calc_Fresnel_Real_Val(double polarization, int thread_Index)
{
	double temp_Fre_Numer_RE, temp_Fre_Numer_IM, temp_Fre_Denom_SQARE, temp_Fre_Denom_RE, temp_Fre_Denom_IM, temp_1_RE, temp_1_IM, temp_2_RE, temp_2_IM;
	// s-polarization
	if (polarization >-1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		temp_Fre_Denom_RE = hi_RE[thread_Index][i] + hi_RE[thread_Index][i+1];
		temp_Fre_Denom_IM = hi_IM[thread_Index][i] + hi_IM[thread_Index][i+1];
		temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

		if ( abs(temp_Fre_Denom_SQARE) > DBL_EPSILON )
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
		temp_1_RE = (hi_RE[thread_Index][i]*unwrapped_Structure->epsilon_RE[i] + hi_IM[thread_Index][i]*unwrapped_Structure->epsilon_IM[i]) / unwrapped_Structure->epsilon_Norm[i];
		temp_1_IM = (hi_IM[thread_Index][i]*unwrapped_Structure->epsilon_RE[i] - hi_RE[thread_Index][i]*unwrapped_Structure->epsilon_IM[i]) / unwrapped_Structure->epsilon_Norm[i];

		temp_2_RE = (hi_RE[thread_Index][i+1]*unwrapped_Structure->epsilon_RE[i+1] + hi_IM[thread_Index][i+1]*unwrapped_Structure->epsilon_IM[i+1]) / unwrapped_Structure->epsilon_Norm[i+1];
		temp_2_IM = (hi_IM[thread_Index][i+1]*unwrapped_Structure->epsilon_RE[i+1] - hi_RE[thread_Index][i+1]*unwrapped_Structure->epsilon_IM[i+1]) / unwrapped_Structure->epsilon_Norm[i+1];

		temp_Fre_Denom_RE = temp_1_RE + temp_2_RE;
		temp_Fre_Denom_IM = temp_1_IM + temp_2_IM;
		temp_Fre_Denom_SQARE = temp_Fre_Denom_RE*temp_Fre_Denom_RE + temp_Fre_Denom_IM*temp_Fre_Denom_IM;

		if ( abs(temp_Fre_Denom_SQARE) > DBL_EPSILON )
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

void Unwrapped_Reflection::calc_Local_Real_Val(double polarization, int thread_Index)
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

#endif

void Unwrapped_Reflection::calc_Weak_Factor(int thread_Index)
{
	bool is_Norm = false;
	double norm, s, factor, x, y, a = M_PI/sqrt(M_PI*M_PI - 8.);

	for (int i = 0; i < num_Boundaries; ++i)
	{
		// if >=1 interlayer is turned on
		is_Norm = false;
		for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
		{
			is_Norm = is_Norm || unwrapped_Structure->boundary_Interlayer_Composition[i][func_Index].enabled;
		}

		if(is_Norm && abs(unwrapped_Structure->sigma[i]) > DBL_EPSILON)
		{
			norm = 0;

			#ifndef REAL_VALUED    // complex-valued
				s = unwrapped_Structure->sigma[i] * sqrt(real(hi[thread_Index][i+1])*real(hi[thread_Index][i]));
			#else	               // real-valued
				s = unwrapped_Structure->sigma[i] * sqrt(hi_RE[thread_Index][i+1]*hi_RE[thread_Index][i]);
			#endif

			//-------------------------------------------------------------------------------
			// erf interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].interlayer.value;
				factor = exp( - 2 * s * s);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// lin interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].interlayer.value;
				x = sqrt(3.) * 2 * s;
				factor = sin(x) / (x);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// exp interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].interlayer.value;
				x = 2 * pow(s, 2);
				factor = 1. / (1. + x);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// tanh interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].interlayer.value;
				x = 2 * sqrt(3.) * s;
				factor = x / sinh(x);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].interlayer.value * factor;
			}
			//-------------------------------------------------------------------------------
			// sin interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].enabled)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].interlayer.value;
				x = 2 * a * s - M_PI_2;
				y = x + M_PI;
				factor = M_PI_4 * (sin(x)/x + sin(y)/y);
				weak_Factor[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].interlayer.value * factor;
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

void Unwrapped_Reflection::calc_Reflectivity_1_Point_1_Thread(double k, double cos2, double polarization, int thread_Index, int point_Index)
{
//	auto start = std::chrono::system_clock::now();

#ifndef REAL_VALUED    // complex-valued
	if(max_Depth <= 2)
	{
		if( abs(polarization-1) < DBL_EPSILON)
			fill_s__Max_Depth_2(unwrapped_Structure->calc_Tree->begin(), thread_Index, point_Index);
		else
		if( abs(polarization+1) < DBL_EPSILON)
			fill_p__Max_Depth_2(unwrapped_Structure->calc_Tree->begin(), thread_Index, point_Index);
		else
			fill_sp_Max_Depth_2(unwrapped_Structure->calc_Tree->begin(), thread_Index, point_Index);
	} else
	{
		calc_Hi(k, cos2, thread_Index);
		calc_Weak_Factor(thread_Index);
		calc_Exponenta	(thread_Index);
		calc_Fresnel(polarization, thread_Index);
	}
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Pre    : "<< elapsed.count()/1000. << " seconds";

//	start = std::chrono::system_clock::now();
		calc_Local(polarization, thread_Index);
#else	               // real-valued
	if(max_Depth <= 2)
	{
		if( abs(polarization-1) < DBL_EPSILON)
			fill_s__Real_Val_Max_Depth_2(unwrapped_Structure->calc_Tree->begin(), thread_Index, point_Index);
		else
		if( abs(polarization+1) < DBL_EPSILON)
			fill_p__Real_Val_Max_Depth_2(unwrapped_Structure->calc_Tree->begin(), thread_Index, point_Index);
		else
			fill_sp_Real_Val_Max_Depth_2(unwrapped_Structure->calc_Tree->begin(), thread_Index, point_Index);
	} else
	{
		calc_Hi_Real_Val(k, cos2, thread_Index);
		calc_Weak_Factor	     (thread_Index);
		calc_Exponenta_Real_Val  (thread_Index);
		calc_Fresnel_Real_Val(polarization, thread_Index);
	}
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	qInfo() << "Pre    : "<< elapsed.count()/1000. << " seconds";

//	start = std::chrono::system_clock::now();
		calc_Local_Real_Val(polarization, thread_Index);
#endif

//	auto enD = std::chrono::system_clock::now();
//	auto elapseD = std::chrono::duration_cast<std::chrono::milliseconds>(enD - start);
//	qInfo() << "Local  : "<< elapseD.count()/1000. << " seconds";

}

void Unwrapped_Reflection::calc_Reflectivity_Angular_nMin_nMax_1_Thread(double k, QVector<double>& cos2_Vec, int n_Min, int n_Max, double polarization, int thread_Index)
{
	// polarization
	double s_Weight = (1. + polarization) / 2.;
	double p_Weight = (1. - polarization) / 2.;

	for(int point_Index = n_Min; point_Index<n_Max; ++point_Index)
	{
		if(cos2_Vec[point_Index]!=1)
		{
			calc_Reflectivity_1_Point_1_Thread(k, cos2_Vec[point_Index], polarization, thread_Index, point_Index);

		#ifndef REAL_VALUED    // complex-valued
			r_s[point_Index] = r_Local_s[thread_Index][0];
			r_p[point_Index] = r_Local_p[thread_Index][0];
		#else	               // real-valued
			r_s[point_Index] = complex<double>(r_Local_s_RE[thread_Index][0], r_Local_s_IM[thread_Index][0]);
			r_p[point_Index] = complex<double>(r_Local_p_RE[thread_Index][0], r_Local_p_IM[thread_Index][0]);
		#endif

			R_s[point_Index] = pow(abs(r_s[point_Index]),2);
			R_p[point_Index] = pow(abs(r_p[point_Index]),2);
			R  [point_Index] = s_Weight * R_s[point_Index] + p_Weight * R_p[point_Index];
		}
		else
		{
			r_s[point_Index] = 1;
			r_p[point_Index] = 1;
			R_s[point_Index] = 1;
			R_p[point_Index] = 1;
			R  [point_Index] = 1;
		}
	}
}	// TODO spectral

void Unwrapped_Reflection::calc_Reflectivity()
{
	QStringList active_Whats_This_List = active_Whats_This.split(whats_This_Delimiter,QString::SkipEmptyParts);

	// if angle is changing
	if(active_Whats_This_List[1] == whats_This_Angle)
	{
		// ----------------------------------------------------------------------------------------------------------------------
		// parallelization
		vector<thread> workers(num_Threads);
		auto delta_N = measurement.cos2.size() / num_Threads;	// number of points per thread

		for (auto thread_Index = 0; thread_Index < num_Threads - 1; ++thread_Index)
		{
			auto n_Min = thread_Index*delta_N;
			auto n_Max = (thread_Index + 1)*delta_N;
			workers[thread_Index] = thread(&Unwrapped_Reflection::calc_Reflectivity_Angular_nMin_nMax_1_Thread, this, measurement.k_Value, measurement.cos2, n_Min, n_Max, measurement.polarization.value, thread_Index);
		}
		auto thread_Index = num_Threads - 1;
		auto n_Min = thread_Index*delta_N;
		auto n_Max = measurement.cos2.size();
		workers[thread_Index] = thread(&Unwrapped_Reflection::calc_Reflectivity_Angular_nMin_nMax_1_Thread, this, measurement.k_Value, measurement.cos2, n_Min, n_Max, measurement.polarization.value, thread_Index);

		for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
		{
			if (workers[thread_Index].joinable()) workers[thread_Index].join();	// присоединение потоков
		}
		// ----------------------------------------------------------------------------------------------------------------------
	}
	// TODO spectral
}
