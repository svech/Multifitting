#ifndef UNWRAPPED_REFLECTION_H
#define UNWRAPPED_REFLECTION_H

#include "unwrapped_structure.h"

class Unwrapped_Reflection
{
public:
	Unwrapped_Reflection();
	Unwrapped_Reflection(Unwrapped_Structure* unwrapped_Structure, int num_Media, QString active_Whats_This, Measurement& measurement);

	int num_Threads;
	int num_Layers;
	int num_Boundaries;
	int num_Media;
	int max_Depth;

	QString active_Whats_This;
	Unwrapped_Structure* unwrapped_Structure;
	Measurement measurement;

#ifndef REAL_VALUED    // complex-valued
	vector<vector<complex<double>>> r_Fresnel_s;	//	[thread][boundary]
	vector<vector<complex<double>>> r_Fresnel_p;	//	[thread][boundary]
	vector<vector<complex<double>>> r_Local_s;		//	[thread][boundary]
	vector<vector<complex<double>>> r_Local_p;		//	[thread][boundary]
	vector<vector<complex<double>>> hi;				//	[thread][media]
	vector<vector<complex<double>>> exponenta;		//	[thread][layer]

	int fill_s__Max_Depth_2	(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_p__Max_Depth_2	(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_sp_Max_Depth_2	(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);

	void calc_Hi			(double k, double cos2, int thread_Index);
	void calc_Exponenta		(int thread_Index);
	void calc_Fresnel		(double polarization, int thread_Index);
	void calc_Local			(double polarization, int thread_Index);
#else	               // real-valued
	vector<vector<double>> r_Fresnel_s_RE;		//	[thread][boundary]
	vector<vector<double>> r_Fresnel_s_IM;		//	[thread][boundary]
	vector<vector<double>> r_Fresnel_p_RE;		//	[thread][boundary]
	vector<vector<double>> r_Fresnel_p_IM;		//	[thread][boundary]
	vector<vector<double>> r_Local_s_RE;		//	[thread][boundary]
	vector<vector<double>> r_Local_s_IM;		//	[thread][boundary]
	vector<vector<double>> r_Local_p_RE;		//	[thread][boundary]
	vector<vector<double>> r_Local_p_IM;		//	[thread][boundary]
	vector<vector<double>> hi_RE;				//	[thread][media]
	vector<vector<double>> hi_IM;				//	[thread][media]
	vector<vector<double>> exponenta_RE;		//	[thread][layer]
	vector<vector<double>> exponenta_IM;		//	[thread][layer]

	int fill_s__Real_Val_Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_p__Real_Val_Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_sp_Real_Val_Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);

	void calc_Hi_Real_Val			(double k, double cos2, int thread_Index);
	void calc_Exponenta_Real_Val	(int thread_Index);
	void calc_Fresnel_Real_Val		(double polarization, int thread_Index);
	void calc_Local_Real_Val		(double polarization, int thread_Index);
#endif
	vector<vector<double>> weak_Factor;				//	[thread][boundary]
	void calc_Weak_Factor	(int thread_Index);

	vector<complex<double>> r_s;
	vector<complex<double>> r_p;
	vector<double> R_s;
	vector<double> R_p;
	vector<double> R;

	void calc_Reflectivity_1_Point_1_Thread(double k, double cos2, double polarization, int thread_Index, int point);
	void calc_Reflectivity_Angular_nMin_nMax_1_Thread(double k, QVector<double>& cos2_Vec, int n_Min, int n_Max, double polarization, int thread_Index);
	void calc_Reflectivity();
};

#endif // UNWRAPPED_REFLECTION_H
