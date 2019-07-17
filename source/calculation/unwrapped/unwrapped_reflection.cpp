#include "unwrapped_reflection.h"

Unwrapped_Reflection::Unwrapped_Reflection(Unwrapped_Structure* unwrapped_Structure, int num_Media, QString active_Parameter_Whats_This, const Data& measurement, bool depth_Grading, bool sigma_Grading, Calc_Functions calc_Functions, QString calc_Mode):
	num_Threads		(reflectivity_Calc_Threads),
	num_Layers		(num_Media-2),
	num_Boundaries	(num_Media-1),
	num_Media		(num_Media),
	max_Depth		(unwrapped_Structure->max_Depth),
	depth_Grading	(depth_Grading),
	sigma_Grading	(sigma_Grading),
	calc_Functions  (calc_Functions),
	calc_Mode		(calc_Mode),
	active_Parameter_Whats_This(active_Parameter_Whats_This),
	unwrapped_Structure(unwrapped_Structure),
	measurement(measurement),

	r_Fresnel_s_RE(num_Threads,vector<double>(num_Boundaries)),
	r_Fresnel_s_IM(num_Threads,vector<double>(num_Boundaries)),
	r_Fresnel_p_RE(num_Threads,vector<double>(num_Boundaries)),
	r_Fresnel_p_IM(num_Threads,vector<double>(num_Boundaries)),
	r_Local_s_RE  (num_Threads,vector<double>(num_Boundaries)),
	r_Local_s_IM  (num_Threads,vector<double>(num_Boundaries)),
	r_Local_p_RE  (num_Threads,vector<double>(num_Boundaries)),
	r_Local_p_IM  (num_Threads,vector<double>(num_Boundaries)),

	t_Fresnel_s_RE(num_Threads,vector<double>(num_Boundaries)),
	t_Fresnel_s_IM(num_Threads,vector<double>(num_Boundaries)),
	t_Fresnel_p_RE(num_Threads,vector<double>(num_Boundaries)),
	t_Fresnel_p_IM(num_Threads,vector<double>(num_Boundaries)),
	t_Local_s_RE  (num_Threads,vector<double>(num_Boundaries)),
	t_Local_s_IM  (num_Threads,vector<double>(num_Boundaries)),
	t_Local_p_RE  (num_Threads,vector<double>(num_Boundaries)),
	t_Local_p_IM  (num_Threads,vector<double>(num_Boundaries)),

	epsilon_Ambient(num_Threads),
	epsilon_Substrate(num_Threads),

	hi_RE		  (num_Threads,vector<double>(num_Media)),
	hi_IM		  (num_Threads,vector<double>(num_Media)),
	exponenta_RE  (num_Threads,vector<double>(num_Boundaries)),
	exponenta_IM  (num_Threads,vector<double>(num_Boundaries)),
	exponenta_2_RE(num_Threads,vector<double>(num_Boundaries)),
	exponenta_2_IM(num_Threads,vector<double>(num_Boundaries)),

	weak_Factor_R (num_Threads,vector<double>(num_Boundaries)),
	weak_Factor_T (num_Threads,vector<double>(num_Boundaries))
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
	Phi_R_s.resize(num_Points);
	Phi_R_p.resize(num_Points);
	R_s.resize(num_Points);
	R_p.resize(num_Points);
	R  .resize(num_Points);
	R_Instrumental.resize(num_Points);

	t_s.resize(num_Points);
	t_p.resize(num_Points);
	Phi_T_s.resize(num_Points);
	Phi_T_p.resize(num_Points);
	T_s.resize(num_Points);
	T_p.resize(num_Points);
	T  .resize(num_Points);
	T_Instrumental.resize(num_Points);

	A_s.resize(num_Points);
	A_p.resize(num_Points);
	A  .resize(num_Points);
	A_Instrumental.resize(num_Points);
}

Unwrapped_Reflection::~Unwrapped_Reflection()
{
}

int Unwrapped_Reflection::fill_s__Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index)
{
	for(unsigned child_Index=0; child_Index<parent.number_of_children(); ++child_Index)
	{
		tree<Node>::post_order_iterator child = tree<Node>::child(parent,child_Index);
		const Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s_RE[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s_IM[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s_RE[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s_IM[point_Index] * child.node->data.weak_Factor_T[point_Index];
			exponenta_RE  [thread_Index][media_Index-1] = child.node->data.exponenta_RE[point_Index];
			exponenta_IM  [thread_Index][media_Index-1] = child.node->data.exponenta_IM[point_Index];
			exponenta_2_RE[thread_Index][media_Index-1] = child.node->data.exponenta_2_RE[point_Index];
			exponenta_2_IM[thread_Index][media_Index-1] = child.node->data.exponenta_2_IM[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Substrate )
		{
			epsilon_Substrate[thread_Index] = child.node->data.epsilon[point_Index];
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s_RE[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s_IM[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s_RE[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s_IM[point_Index] * child.node->data.weak_Factor_T[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Ambient)
		{
			epsilon_Ambient[thread_Index] = child.node->data.epsilon[point_Index];
			hi_RE[thread_Index][media_Index] = child.node->data.hi_RE[point_Index];
			hi_IM[thread_Index][media_Index] = child.node->data.hi_IM[point_Index];
			++media_Index;
		} else
		if( child_Data.item_Type == item_Type_Multilayer ||
			child_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			int start_Period = 0;
			if(child_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
			{	start_Period = 1;	}

			for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi_RE         [thread_Index][media_Index  ] = grandchild.node->data.hi_RE[point_Index];
					hi_IM         [thread_Index][media_Index  ] = grandchild.node->data.hi_IM[point_Index];
					r_Fresnel_s_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_s_RE[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
					r_Fresnel_s_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_s_IM[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
					t_Fresnel_s_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_s_RE[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
					t_Fresnel_s_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_s_IM[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
					exponenta_RE  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_RE[point_Index];
					exponenta_IM  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_IM[point_Index];
					exponenta_2_RE[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2_RE[point_Index];
					exponenta_2_IM[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2_IM[point_Index];
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
		const Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p_RE[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p_IM[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p_RE[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p_IM[point_Index] * child.node->data.weak_Factor_T[point_Index];
			exponenta_RE  [thread_Index][media_Index-1] = child.node->data.exponenta_RE[point_Index];
			exponenta_IM  [thread_Index][media_Index-1] = child.node->data.exponenta_IM[point_Index];
			exponenta_2_RE[thread_Index][media_Index-1] = child.node->data.exponenta_2_RE[point_Index];
			exponenta_2_IM[thread_Index][media_Index-1] = child.node->data.exponenta_2_IM[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Substrate )
		{
			epsilon_Substrate[thread_Index] = child.node->data.epsilon[point_Index];
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p_RE[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p_IM[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p_RE[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p_IM[point_Index] * child.node->data.weak_Factor_T[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Ambient)
		{
			epsilon_Ambient[thread_Index] = child.node->data.epsilon[point_Index];
			hi_RE[thread_Index][media_Index] = child.node->data.hi_RE[point_Index];
			hi_IM[thread_Index][media_Index] = child.node->data.hi_IM[point_Index];
			++media_Index;
		} else
		if( child_Data.item_Type == item_Type_Multilayer ||
			child_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			int start_Period = 0;
			if(child_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
			{	start_Period = 1;	}

			for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi_RE         [thread_Index][media_Index  ] = grandchild.node->data.hi_RE[point_Index];
					hi_IM         [thread_Index][media_Index  ] = grandchild.node->data.hi_IM[point_Index];
					r_Fresnel_p_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_p_RE[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
					r_Fresnel_p_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_p_IM[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
					t_Fresnel_p_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_p_RE[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
					t_Fresnel_p_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_p_IM[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
					exponenta_RE  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_RE[point_Index];
					exponenta_IM  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_IM[point_Index];
					exponenta_2_RE[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2_RE[point_Index];
					exponenta_2_IM[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2_IM[point_Index];
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
		const Data& child_Data = child.node->data.struct_Data;

		if(child_Data.item_Type == item_Type_Layer)
		{
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s_RE[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s_IM[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p_RE[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p_IM[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s_RE[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s_IM[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p_RE[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p_IM[point_Index] * child.node->data.weak_Factor_T[point_Index];
			exponenta_RE  [thread_Index][media_Index-1] = child.node->data.exponenta_RE[point_Index];
			exponenta_IM  [thread_Index][media_Index-1] = child.node->data.exponenta_IM[point_Index];
			exponenta_2_RE[thread_Index][media_Index-1] = child.node->data.exponenta_2_RE[point_Index];
			exponenta_2_IM[thread_Index][media_Index-1] = child.node->data.exponenta_2_IM[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Substrate )
		{
			epsilon_Substrate[thread_Index] = child.node->data.epsilon[point_Index];
			hi_RE         [thread_Index][media_Index  ] = child.node->data.hi_RE[point_Index];
			hi_IM         [thread_Index][media_Index  ] = child.node->data.hi_IM[point_Index];
			r_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s_RE[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_R_s_IM[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p_RE[point_Index] * child.node->data.weak_Factor_R[point_Index];
			r_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_R_p_IM[point_Index] * child.node->data.weak_Factor_R[point_Index];
			t_Fresnel_s_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s_RE[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_s_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_T_s_IM[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_p_RE[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p_RE[point_Index] * child.node->data.weak_Factor_T[point_Index];
			t_Fresnel_p_IM[thread_Index][media_Index-1] = child.node->data.Fresnel_T_p_IM[point_Index] * child.node->data.weak_Factor_T[point_Index];
			++media_Index;
		} else
		if(child_Data.item_Type == item_Type_Ambient)
		{
			epsilon_Ambient[thread_Index] = child.node->data.epsilon[point_Index];
			hi_RE[thread_Index][media_Index] = child.node->data.hi_RE[point_Index];
			hi_IM[thread_Index][media_Index] = child.node->data.hi_IM[point_Index];
			++media_Index;
		} else
		if( child_Data.item_Type == item_Type_Multilayer ||
			child_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			int start_Period = 0;
			if(child_Data.item_Type == item_Type_Regular_Aperiodic) // only slight difference here
			{	start_Period = 1;	}

			for(int period_Index=start_Period; period_Index<child_Data.num_Repetition.value(); ++period_Index)
			{
				for(unsigned grandchild_Index=0; grandchild_Index<child.number_of_children(); ++grandchild_Index)
				{
					tree<Node>::post_order_iterator grandchild = tree<Node>::child(child,grandchild_Index);
					hi_RE         [thread_Index][media_Index  ] = grandchild.node->data.hi_RE[point_Index];
					hi_IM         [thread_Index][media_Index  ] = grandchild.node->data.hi_IM[point_Index];
					r_Fresnel_s_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_s_RE[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
					r_Fresnel_s_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_s_IM[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
					r_Fresnel_p_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_p_RE[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
					r_Fresnel_p_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_R_p_IM[point_Index] * grandchild.node->data.weak_Factor_R[point_Index];
					t_Fresnel_s_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_s_RE[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
					t_Fresnel_s_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_s_IM[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
					t_Fresnel_p_RE[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_p_RE[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
					t_Fresnel_p_IM[thread_Index][media_Index-1] = grandchild.node->data.Fresnel_T_p_IM[point_Index] * grandchild.node->data.weak_Factor_T[point_Index];
					exponenta_RE  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_RE[point_Index];
					exponenta_IM  [thread_Index][media_Index-1] = grandchild.node->data.exponenta_IM[point_Index];
					exponenta_2_RE[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2_RE[point_Index];
					exponenta_2_IM[thread_Index][media_Index-1] = grandchild.node->data.exponenta_2_IM[point_Index];
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
	double norm, a = M_PI/sqrt(M_PI*M_PI - 8.);
	double s_r, factor_r, x_r, y_r, six_r, siy_r;
	double s_t, factor_t, x_t, y_t, six_t, siy_t;
	double my_Sigma = 0;
	for (int i = 0; i < num_Boundaries; ++i)
	{
		// if >=1 interlayer is turned on
		is_Norm = false;
		for(int func_Index=0; func_Index<transition_Layer_Functions_Size; ++func_Index)
		{
			is_Norm = is_Norm || unwrapped_Structure->boundary_Interlayer_Composition[i][func_Index].enabled;
		}

		weak_Factor_R[thread_Index][i] = 0;
		weak_Factor_T[thread_Index][i] = 0;

		if(is_Norm && (abs(unwrapped_Structure->sigma[i]) > DBL_MIN)) //-V674
		{
			my_Sigma = unwrapped_Structure->sigma[i];	// by default, otherwise we change it
			norm = 0;
			s_r = sqrt(hi_RE[thread_Index][i]*hi_RE[thread_Index][i+1]);
			s_t =     (hi_RE[thread_Index][i]-hi_RE[thread_Index][i+1])/2;

			//-------------------------------------------------------------------------------
			// erf interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].interlayer.value;
				if(!unwrapped_Structure->common_Sigma[i]) {
					my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].my_Sigma.value; }

				// reflectance
				factor_r = exp( - s_r * s_r * my_Sigma * my_Sigma * 2);
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].interlayer.value * factor_r;

				// transmittance
				factor_t = exp( - s_t * s_t * my_Sigma * my_Sigma * 2);
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Erf].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// lin interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].interlayer.value;
				if(!unwrapped_Structure->common_Sigma[i]) {
					my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].my_Sigma.value; }

				// reflectance
				x_r = sqrt(3.) * s_r * my_Sigma * 2;
				if(abs(x_r)>DBL_MIN)	{
					factor_r = sin(x_r) / (x_r);
				} else {
					factor_r = 1;
				}
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].interlayer.value * factor_r;

				// transmittance
				x_t = sqrt(3.) * s_t * my_Sigma * 2;
				if(abs(x_t)>DBL_MIN)	{
					factor_t = sin(x_t) / (x_t);
				} else {
					factor_t = 1;
				}
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Lin].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// exp interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].interlayer.value;
				if(!unwrapped_Structure->common_Sigma[i]) {
					my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].my_Sigma.value; }

				// reflectance
				x_r = 2 * pow(s_r * my_Sigma, 2);
				factor_r = 1. / (1. + x_r);
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].interlayer.value * factor_r;

				// transmittance
				x_t = 2 * pow(s_t * my_Sigma, 2);
				factor_t = 1. / (1. + x_t);
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Exp].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// tanh interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].interlayer.value;
				if(!unwrapped_Structure->common_Sigma[i]) {
					my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].my_Sigma.value; }

				// reflectance
				x_r = 2 * sqrt(3.) * s_r * my_Sigma;
				if(abs(x_r)>DBL_MIN)				{
					factor_r = x_r / sinh(x_r);
				} else {
					factor_r = 1;
				}
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].interlayer.value * factor_r;

				// transmittance
				x_t = 2 * sqrt(3.) * s_t * my_Sigma;
				if(abs(x_t)>DBL_MIN)				{
					factor_t = x_t / sinh(x_t);
				} else {
					factor_t = 1;
				}
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Tanh].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// sin interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].interlayer.value;
				if(!unwrapped_Structure->common_Sigma[i]) {
					my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].my_Sigma.value; }

				// reflectance
				x_r = 2 * a * s_r * my_Sigma - M_PI_2;
				y_r = x_r + M_PI;
				if(abs(x_r)>DBL_MIN) six_r = sin(x_r)/x_r; else six_r = 1;
				if(abs(y_r)>DBL_MIN) siy_r = sin(y_r)/y_r; else siy_r = 1;
				factor_r = M_PI_4 * (six_r + siy_r);
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].interlayer.value * factor_r;

				// transmittance
				x_t = 2 * a * s_t * my_Sigma - M_PI_2;
				y_t = x_t + M_PI;
				if(abs(x_t)>DBL_MIN) six_t = sin(x_t)/x_t; else six_t = 1;
				if(abs(y_t)>DBL_MIN) siy_t = sin(y_t)/y_t; else siy_t = 1;
				factor_t = M_PI_4 * (six_t + siy_t);
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Sin].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// step interlayer
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Step].enabled)
			if(unwrapped_Structure->boundary_Interlayer_Composition[i][Step].interlayer.value > DBL_MIN)
			{
				norm += unwrapped_Structure->boundary_Interlayer_Composition[i][Step].interlayer.value;
				if(!unwrapped_Structure->common_Sigma[i]) {
					my_Sigma = unwrapped_Structure->boundary_Interlayer_Composition[i][Step].my_Sigma.value; }

				// reflectance
				factor_r = cos(2 * s_r * my_Sigma);
				weak_Factor_R[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Step].interlayer.value * factor_r;

				// transmittance
				factor_t = cos(2 * s_t * my_Sigma);
				weak_Factor_T[thread_Index][i] += unwrapped_Structure->boundary_Interlayer_Composition[i][Step].interlayer.value * factor_t;
			}
			//-------------------------------------------------------------------------------
			// normalization
			{
				if( abs(norm) > DBL_MIN )
				{
					weak_Factor_R[thread_Index][i] /= norm;
					weak_Factor_T[thread_Index][i] /= norm;
				} else
				{
					weak_Factor_R[thread_Index][i] = 1;
					weak_Factor_T[thread_Index][i] = 1;
				}
			}
		} else
		{
			weak_Factor_R[thread_Index][i] = 1;
			weak_Factor_T[thread_Index][i] = 1;
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

			r_Fresnel_s_RE[thread_Index][i] = weak_Factor_R[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM);
			r_Fresnel_s_IM[thread_Index][i] = weak_Factor_R[thread_Index][i] / temp_Fre_Denom_SQARE *
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

			r_Fresnel_p_RE[thread_Index][i] = weak_Factor_R[thread_Index][i] / temp_Fre_Denom_SQARE *
											  (temp_Fre_Numer_RE*temp_Fre_Denom_RE + temp_Fre_Numer_IM*temp_Fre_Denom_IM);
			r_Fresnel_p_IM[thread_Index][i] = weak_Factor_R[thread_Index][i] / temp_Fre_Denom_SQARE *
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
	double re, im, ere, ere2;
	for (int i = 0; i < num_Layers; ++i)
	{
		re = -hi_IM[thread_Index][i+1]*unwrapped_Structure->thickness[i];
		im =  hi_RE[thread_Index][i+1]*unwrapped_Structure->thickness[i];
		ere = exp(re);
		ere2 = exp(2.*re);

		exponenta_RE[thread_Index][i] = ere*cos(im);
		exponenta_IM[thread_Index][i] = ere*sin(im);
		exponenta_2_RE[thread_Index][i] = ere2*cos(2.*im);
		exponenta_2_IM[thread_Index][i] = ere2*sin(2.*im);
	}
}

void Unwrapped_Reflection::calc_Local(double polarization, int thread_Index)
{
	double loc_Denom_RE, loc_Denom_IM, loc_Denom_SQUARE, temp_R_RE, temp_R_IM, temp_T_RE, temp_T_IM, loc_Numer_R_RE, loc_Numer_R_IM, loc_Numer_T_RE, loc_Numer_T_IM;
	// s-polarization
	if (polarization >-1)
	{
		// reflectance only
		if( !calc_Functions.check_Transmittance &&
			!calc_Functions.check_Absorptance	&&
			!calc_Functions.check_User			&&
			!calc_Functions.check_Joule			&&
			!calc_Functions.check_Field )
		{
			r_Local_s_RE[thread_Index].back() = r_Fresnel_s_RE[thread_Index].back();	// last boundary
			r_Local_s_IM[thread_Index].back() = r_Fresnel_s_IM[thread_Index].back();	// last boundary

			for (int i = num_Layers-1; i >= 0; --i)
			{
				temp_R_RE = r_Local_s_RE[thread_Index][i+1]*exponenta_2_RE[thread_Index][i] - r_Local_s_IM[thread_Index][i+1]*exponenta_2_IM[thread_Index][i];
				temp_R_IM = r_Local_s_IM[thread_Index][i+1]*exponenta_2_RE[thread_Index][i] + r_Local_s_RE[thread_Index][i+1]*exponenta_2_IM[thread_Index][i];
				loc_Denom_RE = 1. + (temp_R_RE*r_Fresnel_s_RE[thread_Index][i] - temp_R_IM*r_Fresnel_s_IM[thread_Index][i]);
				loc_Denom_IM =       temp_R_IM*r_Fresnel_s_RE[thread_Index][i] + temp_R_RE*r_Fresnel_s_IM[thread_Index][i];
				loc_Denom_SQUARE = loc_Denom_RE*loc_Denom_RE + loc_Denom_IM*loc_Denom_IM;

				loc_Numer_R_RE = r_Fresnel_s_RE[thread_Index][i] + temp_R_RE;
				loc_Numer_R_IM = r_Fresnel_s_IM[thread_Index][i] + temp_R_IM;

				r_Local_s_RE[thread_Index][i] = (loc_Numer_R_RE*loc_Denom_RE + loc_Numer_R_IM*loc_Denom_IM) / loc_Denom_SQUARE;
				r_Local_s_IM[thread_Index][i] = (loc_Numer_R_IM*loc_Denom_RE - loc_Numer_R_RE*loc_Denom_IM) / loc_Denom_SQUARE;
			}
		}
		// reflectance and transmittance
		if( calc_Functions.check_Transmittance ||
			calc_Functions.check_Absorptance   ||
			calc_Functions.check_User		   ||
			calc_Functions.check_Joule		   ||
			calc_Functions.check_Field )
		{
			r_Local_s_RE[thread_Index].back() = r_Fresnel_s_RE[thread_Index].back();	// last boundary
			r_Local_s_IM[thread_Index].back() = r_Fresnel_s_IM[thread_Index].back();	// last boundary
			t_Local_s_RE[thread_Index].back() = t_Fresnel_s_RE[thread_Index].back();	// last boundary
			t_Local_s_IM[thread_Index].back() = t_Fresnel_s_IM[thread_Index].back();	// last boundary

			for (int i = num_Layers-1; i >= 0; --i)
			{
				temp_R_RE = r_Local_s_RE[thread_Index][i+1]*exponenta_2_RE[thread_Index][i] - r_Local_s_IM[thread_Index][i+1]*exponenta_2_IM[thread_Index][i];
				temp_R_IM = r_Local_s_IM[thread_Index][i+1]*exponenta_2_RE[thread_Index][i] + r_Local_s_RE[thread_Index][i+1]*exponenta_2_IM[thread_Index][i];

				temp_T_RE = t_Local_s_RE[thread_Index][i+1]*exponenta_RE[thread_Index][i] - t_Local_s_IM[thread_Index][i+1]*exponenta_IM[thread_Index][i];
				temp_T_IM = t_Local_s_IM[thread_Index][i+1]*exponenta_RE[thread_Index][i] + t_Local_s_RE[thread_Index][i+1]*exponenta_IM[thread_Index][i];

				loc_Denom_RE = 1. + (temp_R_RE*r_Fresnel_s_RE[thread_Index][i] - temp_R_IM*r_Fresnel_s_IM[thread_Index][i]);
				loc_Denom_IM =       temp_R_IM*r_Fresnel_s_RE[thread_Index][i] + temp_R_RE*r_Fresnel_s_IM[thread_Index][i];
				loc_Denom_SQUARE = loc_Denom_RE*loc_Denom_RE + loc_Denom_IM*loc_Denom_IM;

				loc_Numer_R_RE = r_Fresnel_s_RE[thread_Index][i] + temp_R_RE;
				loc_Numer_R_IM = r_Fresnel_s_IM[thread_Index][i] + temp_R_IM;

				loc_Numer_T_RE = t_Fresnel_s_RE[thread_Index][i]*temp_T_RE - t_Fresnel_s_IM[thread_Index][i]*temp_T_IM;
				loc_Numer_T_IM = t_Fresnel_s_RE[thread_Index][i]*temp_T_IM + t_Fresnel_s_IM[thread_Index][i]*temp_T_RE;

				r_Local_s_RE[thread_Index][i] = (loc_Numer_R_RE*loc_Denom_RE + loc_Numer_R_IM*loc_Denom_IM) / loc_Denom_SQUARE;
				r_Local_s_IM[thread_Index][i] = (loc_Numer_R_IM*loc_Denom_RE - loc_Numer_R_RE*loc_Denom_IM) / loc_Denom_SQUARE;

				t_Local_s_RE[thread_Index][i] = (loc_Numer_T_RE*loc_Denom_RE + loc_Numer_T_IM*loc_Denom_IM) / loc_Denom_SQUARE;
				t_Local_s_IM[thread_Index][i] = (loc_Numer_T_IM*loc_Denom_RE - loc_Numer_T_RE*loc_Denom_IM) / loc_Denom_SQUARE;
			}
		}
	}
	// p-polarization
	if (polarization <1)
	{
		// reflectance only
		if( !calc_Functions.check_Transmittance &&
			!calc_Functions.check_Absorptance	&&
			!calc_Functions.check_User			&&
			!calc_Functions.check_Joule			&&
			!calc_Functions.check_Field )
		{
			r_Local_p_RE[thread_Index].back() = r_Fresnel_p_RE[thread_Index].back();	// last boundary
			r_Local_p_IM[thread_Index].back() = r_Fresnel_p_IM[thread_Index].back();	// last boundary

			for (int i = num_Layers-1; i >= 0; --i)
			{
				temp_R_RE = r_Local_p_RE[thread_Index][i+1]*exponenta_2_RE[thread_Index][i] - r_Local_p_IM[thread_Index][i+1]*exponenta_2_IM[thread_Index][i];
				temp_R_IM = r_Local_p_IM[thread_Index][i+1]*exponenta_2_RE[thread_Index][i] + r_Local_p_RE[thread_Index][i+1]*exponenta_2_IM[thread_Index][i];
				loc_Denom_RE = 1. + (temp_R_RE*r_Fresnel_p_RE[thread_Index][i] - temp_R_IM*r_Fresnel_p_IM[thread_Index][i]);
				loc_Denom_IM =       temp_R_IM*r_Fresnel_p_RE[thread_Index][i] + temp_R_RE*r_Fresnel_p_IM[thread_Index][i];
				loc_Denom_SQUARE = loc_Denom_RE*loc_Denom_RE + loc_Denom_IM*loc_Denom_IM;

				loc_Numer_R_RE = r_Fresnel_p_RE[thread_Index][i] + temp_R_RE;
				loc_Numer_R_IM = r_Fresnel_p_IM[thread_Index][i] + temp_R_IM;

				r_Local_p_RE[thread_Index][i] = (loc_Numer_R_RE*loc_Denom_RE + loc_Numer_R_IM*loc_Denom_IM) / loc_Denom_SQUARE;
				r_Local_p_IM[thread_Index][i] = (loc_Numer_R_IM*loc_Denom_RE - loc_Numer_R_RE*loc_Denom_IM) / loc_Denom_SQUARE;
			}
		}
		// reflectance and transmittance
		if( calc_Functions.check_Transmittance ||
			calc_Functions.check_Absorptance   ||
			calc_Functions.check_User		   ||
			calc_Functions.check_Joule		   ||
			calc_Functions.check_Field )
		{
			r_Local_p_RE[thread_Index].back() = r_Fresnel_p_RE[thread_Index].back();	// last boundary
			r_Local_p_IM[thread_Index].back() = r_Fresnel_p_IM[thread_Index].back();	// last boundary
			t_Local_p_RE[thread_Index].back() = t_Fresnel_p_RE[thread_Index].back();	// last boundary
			t_Local_p_IM[thread_Index].back() = t_Fresnel_p_IM[thread_Index].back();	// last boundary

			for (int i = num_Layers-1; i >= 0; --i)
			{
				temp_R_RE = r_Local_p_RE[thread_Index][i+1]*exponenta_2_RE[thread_Index][i] - r_Local_p_IM[thread_Index][i+1]*exponenta_2_IM[thread_Index][i];
				temp_R_IM = r_Local_p_IM[thread_Index][i+1]*exponenta_2_RE[thread_Index][i] + r_Local_p_RE[thread_Index][i+1]*exponenta_2_IM[thread_Index][i];

				temp_T_RE = t_Local_p_RE[thread_Index][i+1]*exponenta_RE[thread_Index][i] - t_Local_p_IM[thread_Index][i+1]*exponenta_IM[thread_Index][i];
				temp_T_IM = t_Local_p_IM[thread_Index][i+1]*exponenta_RE[thread_Index][i] + t_Local_p_RE[thread_Index][i+1]*exponenta_IM[thread_Index][i];

				loc_Denom_RE = 1. + (temp_R_RE*r_Fresnel_p_RE[thread_Index][i] - temp_R_IM*r_Fresnel_p_IM[thread_Index][i]);
				loc_Denom_IM =       temp_R_IM*r_Fresnel_p_RE[thread_Index][i] + temp_R_RE*r_Fresnel_p_IM[thread_Index][i];
				loc_Denom_SQUARE = loc_Denom_RE*loc_Denom_RE + loc_Denom_IM*loc_Denom_IM;

				loc_Numer_R_RE = r_Fresnel_p_RE[thread_Index][i] + temp_R_RE;
				loc_Numer_R_IM = r_Fresnel_p_IM[thread_Index][i] + temp_R_IM;

				loc_Numer_T_RE = t_Fresnel_p_RE[thread_Index][i]*temp_T_RE - t_Fresnel_p_IM[thread_Index][i]*temp_T_IM;
				loc_Numer_T_IM = t_Fresnel_p_RE[thread_Index][i]*temp_T_IM + t_Fresnel_p_IM[thread_Index][i]*temp_T_RE;

				r_Local_p_RE[thread_Index][i] = (loc_Numer_R_RE*loc_Denom_RE + loc_Numer_R_IM*loc_Denom_IM) / loc_Denom_SQUARE;
				r_Local_p_IM[thread_Index][i] = (loc_Numer_R_IM*loc_Denom_RE - loc_Numer_R_RE*loc_Denom_IM) / loc_Denom_SQUARE;

				t_Local_p_RE[thread_Index][i] = (loc_Numer_T_RE*loc_Denom_RE + loc_Numer_T_IM*loc_Denom_IM) / loc_Denom_SQUARE;
				t_Local_p_IM[thread_Index][i] = (loc_Numer_T_IM*loc_Denom_RE - loc_Numer_T_RE*loc_Denom_IM) / loc_Denom_SQUARE;
			}
		}
	}
}

void Unwrapped_Reflection::multifly_Fresnel_And_Weak_Factor(double polarization, int thread_Index)
{
	// s-polarization
	if (polarization >-1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		// reflectance
		r_Fresnel_s_RE[thread_Index][i] *= weak_Factor_R[thread_Index][i];
		r_Fresnel_s_IM[thread_Index][i] *= weak_Factor_R[thread_Index][i];

		// transmittance
		t_Fresnel_s_RE[thread_Index][i] *= weak_Factor_T[thread_Index][i];
		t_Fresnel_s_IM[thread_Index][i] *= weak_Factor_T[thread_Index][i];
	}

	// p-polarization
	if (polarization < 1)
	for (int i = 0; i < num_Boundaries; ++i)
	{
		// reflectance
		r_Fresnel_p_RE[thread_Index][i] *= weak_Factor_R[thread_Index][i];
		r_Fresnel_p_IM[thread_Index][i] *= weak_Factor_R[thread_Index][i];

		// transmittance
		t_Fresnel_p_RE[thread_Index][i] *= weak_Factor_T[thread_Index][i];
		t_Fresnel_p_IM[thread_Index][i] *= weak_Factor_T[thread_Index][i];
	}
}

void Unwrapped_Reflection::calc_Specular_1_Point_1_Thread(const Data& measurement, int thread_Index, int point_Index)
{
//	auto start = std::chrono::system_clock::now();
	// PARAMETER

	if( max_Depth <= 2 )
	{
		// in case of grading, some of these values are temporary and will be recalculated
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

	calc_Local(measurement.polarization.value, thread_Index);

//	auto enD = std::chrono::system_clock::now();
//	auto elapseD = std::chrono::duration_cast<std::chrono::nanoseconds>(enD - end);

//	auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
//	qInfo() << "Pre    : "<< elapsed.count()/1000000000. << " seconds";
//	qInfo() << "Local  : "<< elapseD.count()/1000000000. << " seconds";
}

void Unwrapped_Reflection::fill_Specular_Values(const Data& measurement, int thread_Index, int point_Index)
{
	double s_Weight = (1. + measurement.polarization.value) / 2.;
	double p_Weight = (1. - measurement.polarization.value) / 2.;

	// reflectance
	r_s[point_Index] = complex<double>(r_Local_s_RE[thread_Index][0], r_Local_s_IM[thread_Index][0]);
	r_p[point_Index] = complex<double>(r_Local_p_RE[thread_Index][0], r_Local_p_IM[thread_Index][0]);

	Phi_R_s[point_Index] = arg(r_s[point_Index])/M_PI*180.;
	Phi_R_p[point_Index] = arg(r_p[point_Index])/M_PI*180.;

	R_s[point_Index] = pow(abs(r_s[point_Index]),2);
	R_p[point_Index] = pow(abs(r_p[point_Index]),2);
	R  [point_Index] = s_Weight * R_s[point_Index] + p_Weight * R_p[point_Index];

	// transmittance
	t_s[point_Index] = complex<double>(t_Local_s_RE[thread_Index][0], t_Local_s_IM[thread_Index][0]);
	t_p[point_Index] = complex<double>(t_Local_p_RE[thread_Index][0], t_Local_p_IM[thread_Index][0]);

	Phi_T_s[point_Index] = arg(t_s[point_Index])/M_PI*180.;
	Phi_T_p[point_Index] = arg(t_p[point_Index])/M_PI*180.;

	complex<double> hi_Ambient   = complex<double>(hi_RE[thread_Index][0],           hi_IM[thread_Index][0]);
	complex<double> hi_Substrate = complex<double>(hi_RE[thread_Index][num_Media-1], hi_IM[thread_Index][num_Media-1]);
	double environment_Factor_s = real(hi_Substrate/hi_Ambient);
	double environment_Factor_p = real(
										(hi_Substrate/epsilon_Substrate[thread_Index])/
										(hi_Ambient  /epsilon_Ambient[thread_Index]  ));
	if(isinf(environment_Factor_s)) environment_Factor_s = 0;
	if(isinf(environment_Factor_p)) environment_Factor_p = 0;
	if(isnan(environment_Factor_s)) environment_Factor_s = 0;
	if(isnan(environment_Factor_p)) environment_Factor_p = 0;

	T_s[point_Index] = pow(abs(t_s[point_Index]),2)*environment_Factor_s;
	T_p[point_Index] = pow(abs(t_p[point_Index]),2)*environment_Factor_p;
	T  [point_Index] = s_Weight * T_s[point_Index] + p_Weight * T_p[point_Index];

	// absorptance (without scattering!)
	A_s[point_Index] = 1.-T_s[point_Index]-R_s[point_Index];
	A_p[point_Index] = 1.-T_p[point_Index]-R_p[point_Index];		
	A  [point_Index] = s_Weight * A_s[point_Index] + p_Weight * A_p[point_Index];	

	if(A_s[point_Index] < DBL_MIN) A_s[point_Index] = DBL_MIN;
	if(A_p[point_Index] < DBL_MIN) A_p[point_Index] = DBL_MIN;
	if(A  [point_Index] < DBL_MIN) A  [point_Index] = DBL_MIN;

	// NaN
	if(isnan(R[point_Index]) || isnan(T[point_Index]))
	{
		if(isnan(R[point_Index])) {R[point_Index]=10000; qInfo() << "Unwrapped_Reflection::fill_Specular_Values  :  R = NaN at point" << point_Index; QMessageBox::warning(nullptr, "Unwrapped_Reflection::fill_Specular_Values", "R = NaN");}		// NaN to 10000. Be careful!
		if(isnan(T[point_Index])) {T[point_Index]=10000; qInfo() << "Unwrapped_Reflection::fill_Specular_Values  :  T = NaN at point" << point_Index; QMessageBox::warning(nullptr, "Unwrapped_Reflection::fill_Specular_Values", "T = NaN");}		// NaN to 10000. Be careful!

		qInfo() << "r_Local_s_RE" << r_Local_s_RE[thread_Index][0] << "r_Local_s_IM" << r_Local_s_IM[thread_Index][0];
		qInfo() << "r_Local_p_RE" << r_Local_p_RE[thread_Index][0] << "r_Local_p_IM" << r_Local_p_IM[thread_Index][0];
		qInfo() << "t_Local_s_RE" << t_Local_s_RE[thread_Index][0] << "t_Local_s_IM" << t_Local_s_IM[thread_Index][0];
		qInfo() << "t_Local_p_RE" << t_Local_p_RE[thread_Index][0] << "t_Local_p_IM" << t_Local_p_IM[thread_Index][0];
		qInfo() << "environment_Factor_s" << environment_Factor_s << "environment_Factor_p" << environment_Factor_p;
		qInfo() << "";
	}
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
	auto start = std::chrono::system_clock::now();

	/// ----------------------------------------------------------------------------------------------------------------------
	/// parallelization
	/// ----------------------------------------------------------------------------------------------------------------------
	int delta_N = num_Points / num_Threads;	// number of points per thread

	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		int n_Min = thread_Index*delta_N;
		int n_Max = (thread_Index + 1)*delta_N;
		if(thread_Index == (num_Threads-1))
		{
			n_Max = num_Points;
		}
		global_Workers[thread_Index] = thread(&Unwrapped_Reflection::calc_Specular_nMin_nMax_1_Thread, this, measurement, n_Min, n_Max, thread_Index);
	}
	// join threads
	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		if (global_Workers[thread_Index].joinable()) global_Workers[thread_Index].join();
	}
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	qInfo() << "	parallelization:    "<< elapsed.count()/1000000. << " seconds" << endl;
	/// ----------------------------------------------------------------------------------------------------------------------
	// postprocessing
	{
		auto start = std::chrono::system_clock::now();
		// interpolation
		if(active_Parameter_Whats_This == whats_This_Angle)
		{
			if(measurement.angular_Resolution.value>0 && measurement.angle.size()>=MIN_ANGULAR_RESOLUTION_POINTS)
			{
				int ang_Res_Points = 20; //max(2.,3*pow(measurement.angular_Resolution.value*100,2));
					qInfo() << "ang_Res_Points =" << ang_Res_Points;
//					ang_Res_Points = min(30,ang_Res_Points);
//					qInfo() << "ang_Res_Points =" << ang_Res_Points;
				QVector<double> ang_Resolution(measurement.angle.size());
				for(int i=0; i<measurement.angle.size(); ++i)
				{
					ang_Resolution[i] = measurement.angular_Resolution.value;
				}

				vector<double>& calculated_Curve = R;
				vector<double>& working_Curve = R_Instrumental;

				if( calc_Functions.check_Reflectance)
				{	calculated_Curve = R; working_Curve = R_Instrumental;}
				if( calc_Functions.check_Transmittance)
				{	calculated_Curve = T; working_Curve = T_Instrumental;}
				if( calc_Functions.check_Absorptance)
				{	calculated_Curve = A; working_Curve = A_Instrumental;}

				interpolate_Curve(ang_Res_Points, measurement.angle, ang_Resolution, calculated_Curve, working_Curve);

				// interpolation on dense mesh
				auto start_1 = std::chrono::system_clock::now();
				{
					double min_Step_As_Is = find_Min_Mesh_Step(measurement.angle);
					double min_Resolution = measurement.angular_Resolution.value;  // minimal resolution over all curve points
					int resolution_Points = 30; // 30 is tunable
					double delta_From_Resolution = min_Resolution/resolution_Points;
					double preliminary_Delta = min(min_Step_As_Is, delta_From_Resolution); // interpolation mesh is more dense than initial
						   preliminary_Delta = max(min_Step_As_Is/20, preliminary_Delta);  // but not too dense (10-20 is tunable)

					double first_Argument = measurement.angle.first();
					double last_Argument = measurement.angle.last();
					unsigned long long num_Points_in_Dense_Mesh = ceil( (last_Argument - first_Argument)/preliminary_Delta );
					double real_Delta = (last_Argument - first_Argument)/(num_Points_in_Dense_Mesh-1);  // interpolated curve is equidistant

					vector<double> dense_Mesh(num_Points_in_Dense_Mesh);
					vector<double> dense_Mesh_Interpolated_Curve(num_Points_in_Dense_Mesh);
					condense_Curve(measurement.angle, calculated_Curve, real_Delta, dense_Mesh_Interpolated_Curve, dense_Mesh);

					qInfo() << "num_Points_in_Dense_Mesh =" << num_Points_in_Dense_Mesh;
				}
				auto end_1 = std::chrono::system_clock::now();
				auto elapsed_1 = std::chrono::duration_cast<std::chrono::microseconds>(end_1 - start_1);
				qInfo() << "	dense mesh:    "<< elapsed_1.count()/1000000. << " seconds" << endl;
			} else
			{
				R_Instrumental = R;
				T_Instrumental = T;
				A_Instrumental = A;
			}
		}
		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		qInfo() << "	interpolation:    "<< elapsed.count()/1000000. << " seconds" << endl;

		if(active_Parameter_Whats_This == whats_This_Wavelength)
		{
			if(measurement.spectral_Resolution.value>0 && measurement.lambda.size()>=MIN_SPECTRAL_RESOLUTION_POINTS)
			{
				int spec_Res_Points = max(2.,3*pow(measurement.spectral_Resolution.value*1000,2));
					spec_Res_Points = min(30,spec_Res_Points);
				QVector<double> spec_Resolution(measurement.lambda.size());
				for(int i=0; i<measurement.lambda.size(); ++i)
				{
					spec_Resolution[i] = measurement.spectral_Resolution.value*measurement.lambda[i];
				}
				if( calc_Functions.check_Reflectance)
				{	interpolate_Curve(spec_Res_Points, measurement.lambda, spec_Resolution, R, R_Instrumental);}
				if( calc_Functions.check_Transmittance)
				{	interpolate_Curve(spec_Res_Points, measurement.lambda, spec_Resolution, T, T_Instrumental);}
				if( calc_Functions.check_Absorptance)
				{	interpolate_Curve(spec_Res_Points, measurement.lambda, spec_Resolution, A, A_Instrumental);}
			} else
			{
				R_Instrumental = R;
				T_Instrumental = T;
				A_Instrumental = A;
			}
		}

		// instrumental function
		if(measurement.beam_Size.value>DBL_EPSILON)
		for(size_t point_Index=0; point_Index<R.size(); ++point_Index)
		{
			R_Instrumental[point_Index] *= measurement.instrumental_Factor_Vec[point_Index];
		}

		// any way
		for(size_t point_Index=0; point_Index<R.size(); ++point_Index)
		{
			R_Instrumental[point_Index] += measurement.background.value;
			T_Instrumental[point_Index] += measurement.background.value;
			A_Instrumental[point_Index] += measurement.background.value;
		}
	}
}

double Unwrapped_Reflection::find_Min_Mesh_Step(const QVector<double>& argument)
{
	double min = DBL_MAX;
	double val = 0;
	for(int i=0; i<argument.size()-1; i++)
	{
		val = abs(argument[i+1]-argument[i]);
		if( val < min ) {min = val;}
	}
	return min;
}

void Unwrapped_Reflection::condense_Curve(const QVector<double>& sparse_Argument, const vector<double>& input_Sparse_Curve, double real_Delta, vector<double>& output_Dense_Curve, vector<double>& output_Dense_Argument)
{
	// OPTIMIZE
	const gsl_interp_type* interp_type = gsl_interp_steffen;
	gsl_interp_accel* Acc = gsl_interp_accel_alloc();
	gsl_spline* Spline = gsl_spline_alloc(interp_type, input_Sparse_Curve.size());

	gsl_spline_init(Spline, sparse_Argument.data(), input_Sparse_Curve.data(), input_Sparse_Curve.size());

	double new_Arg;
//	Global_Variables::parallel_For(output_Curve.size(), reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
//	{
		for(uint point_Index=0; point_Index<output_Dense_Curve.size(); ++point_Index)
//		for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
		{
			new_Arg = sparse_Argument.first()+point_Index*real_Delta;
			if(new_Arg>sparse_Argument.last()) new_Arg = sparse_Argument.last(); // control over machine precision
			output_Dense_Argument[point_Index] = new_Arg;
			output_Dense_Curve   [point_Index] = gsl_spline_eval(Spline, new_Arg, Acc);
		}
//	});

	gsl_spline_free(Spline);
	gsl_interp_accel_free(Acc);
}

void Unwrapped_Reflection::wrap_Condensed_Curve(const QVector<double>& sparse_Argument, const QVector<double>& dense_Argument, const vector<double>& input_Dense_Curve, const QVector<double>& resolution, vector<double>& output_Sparse_Curve)
{

}

void Unwrapped_Reflection::interpolate_Curve(int res_Points, const QVector<double> &argument, const QVector<double>& resolution, vector<double>& input_Curve, vector<double>& output_Curve)
{
	// OPTIMIZE
	const gsl_interp_type* interp_type = gsl_interp_steffen;
	gsl_interp_accel* Acc = gsl_interp_accel_alloc();
	gsl_spline* Spline = gsl_spline_alloc(interp_type, input_Curve.size());

	gsl_spline_init(Spline, argument.data(), input_Curve.data(), input_Curve.size());
	int counter=0;

//	Global_Variables::parallel_For(input_Curve.size(), reflectivity_Calc_Threads, [&](int n_Min, int n_Max)
//	{
		for(uint point_Index=0; point_Index<input_Curve.size(); ++point_Index)
//		for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
		{
			double delta = 2*resolution[point_Index]/res_Points; // spectral resolution is not constant in absolute values

			output_Curve[point_Index] = 0;
			double weight, x;
			double weight_Accumulator = 0;
			for(int i=-res_Points; i<res_Points; ++i)
			{
				x = i*delta;
				if((argument[point_Index]+x>argument.first())&&
				   (argument[point_Index]+x<argument.last()))
				{
					weight = exp(-pow(x/(resolution[point_Index]/2),2));
					weight_Accumulator += weight;
					output_Curve[point_Index] += weight*gsl_spline_eval(Spline, argument[point_Index]+i*delta, Acc);
				}
			}
			output_Curve[point_Index] /= weight_Accumulator;
		}

//		{
//			double delta = resolution[point_Index]/res_Points; // spectral resolution is not constant in absolute values

//			output_Curve[point_Index] = 0;
//			double weight, x;
//			double weight_Accumulator = 0;
//			for(int i=-3*res_Points; i<3*res_Points; ++i)
//			{
//				if((argument[point_Index]+i*delta>argument.first())&&
//				   (argument[point_Index]+i*delta<argument.last()))
//				{
//					x = i/double(res_Points);
//					weight = exp(-pow(x,2));
//					weight_Accumulator += weight;
//					counter++;
//					output_Curve[point_Index] += weight*gsl_spline_eval(Spline, argument[point_Index]+i*delta, Acc);
//				}
//			}
//			output_Curve[point_Index] /= weight_Accumulator;
//		}
		qInfo() << "counter =" << counter;
//	});

	gsl_spline_free(Spline);
	gsl_interp_accel_free(Acc);
}
