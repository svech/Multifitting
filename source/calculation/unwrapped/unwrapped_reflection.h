#ifndef UNWRAPPED_REFLECTION_H
#define UNWRAPPED_REFLECTION_H

#include "unwrapped_structure.h"
#include "gsl/gsl_integration.h"

class Unwrapped_Reflection
{
public:
	Unwrapped_Reflection(Unwrapped_Structure* unwrapped_Structure, int num_Media, QString active_Parameter_Whats_This, const Data& measurement, bool depth_Grading, bool sigma_Grading, bool calc_Transmission);

	int num_Threads;
	int num_Layers;
	int num_Boundaries;
	int num_Media;
	int max_Depth;
	int num_Points = 0;

	bool depth_Grading;
	bool sigma_Grading;
	bool calc_Transmission = false;

	QString active_Parameter_Whats_This;
	Unwrapped_Structure* unwrapped_Structure;
	const Data& measurement;

	vector<vector<double>> r_Fresnel_s_RE;		//	[thread][boundary]
	vector<vector<double>> r_Fresnel_s_IM;		//	[thread][boundary]
	vector<vector<double>> r_Fresnel_p_RE;		//	[thread][boundary]
	vector<vector<double>> r_Fresnel_p_IM;		//	[thread][boundary]
	vector<vector<double>> r_Local_s_RE;		//	[thread][boundary]
	vector<vector<double>> r_Local_s_IM;		//	[thread][boundary]
	vector<vector<double>> r_Local_p_RE;		//	[thread][boundary]
	vector<vector<double>> r_Local_p_IM;		//	[thread][boundary]

	vector<vector<double>> t_Fresnel_s_RE;		//	[thread][boundary]
	vector<vector<double>> t_Fresnel_s_IM;		//	[thread][boundary]
	vector<vector<double>> t_Fresnel_p_RE;		//	[thread][boundary]
	vector<vector<double>> t_Fresnel_p_IM;		//	[thread][boundary]
	vector<vector<double>> t_Local_s_RE;		//	[thread][boundary]
	vector<vector<double>> t_Local_s_IM;		//	[thread][boundary]
	vector<vector<double>> t_Local_p_RE;		//	[thread][boundary]
	vector<vector<double>> t_Local_p_IM;		//	[thread][boundary]

	vector<vector<double>> hi_RE;				//	[thread][media]
	vector<vector<double>> hi_IM;				//	[thread][media]
	vector<vector<double>> exponenta_RE;		//	[thread][layer]
	vector<vector<double>> exponenta_IM;		//	[thread][layer]
	vector<vector<double>> exponenta_2_RE;		//	[thread][layer]
	vector<vector<double>> exponenta_2_IM;		//	[thread][layer]
	vector<vector<double>> weak_Factor_R;		//	[thread][boundary]
	vector<vector<double>> weak_Factor_T;		//	[thread][boundary]

	int fill_s__Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_p__Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_sp_Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);

	void calc_Hi		 (double k, double cos2, const vector<double>& eps_RE, const vector<double>& eps_IM, int thread_Index);
	void calc_Weak_Factor(int thread_Index);
	void calc_Fresnel	 (double polarization,   const vector<double>& eps_RE, const vector<double>& eps_IM, const vector<double>& eps_NORM, int thread_Index);
	void calc_Exponenta	 (int thread_Index);
	void calc_Local		 (double polarization, int thread_Index);

	// for sigma grading
	void multifly_Fresnel_And_Weak_Factor(double polarization, int thread_Index);

	vector<complex<double>> r_s;
	vector<complex<double>> r_p;
	vector<double> Phi_R_s;
	vector<double> Phi_R_p;
	vector<double> R_s;
	vector<double> R_p;
	vector<double> R;
	vector<double> R_Instrumental;

	void fill_Specular_Values            (const Data& measurement, int thread_Index, int point_Index);
	void calc_Specular_1_Point_1_Thread  (const Data& measurement, int thread_Index, int point_Index);
	void calc_Specular_nMin_nMax_1_Thread(const Data& measurement, int n_Min, int n_Max, int thread_Index);
	void calc_Specular();

	void interpolate_R(int res_Points, const QVector<double> &argument, const QVector<double>& resolution);
	void size_Effect(double angle, double& denominator, double& instrumental_Factor, int key, const double epsabs,
					 const double epsrel, size_t limit, gsl_integration_workspace* w, gsl_function* F);
};

#endif // UNWRAPPED_REFLECTION_H
