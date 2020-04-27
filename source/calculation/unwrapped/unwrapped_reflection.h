#ifndef UNWRAPPED_REFLECTION_H
#define UNWRAPPED_REFLECTION_H

#include "unwrapped_structure.h"
#include "gsl/gsl_integration.h"
#include "iostream"

class Unwrapped_Reflection
{
public:
	Unwrapped_Reflection(Unwrapped_Structure* unwrapped_Structure, int num_Media,
						 const Data& measurement, bool depth_Grading, bool sigma_Grading,
						 const Calc_Functions& calc_Functions, Calculated_Values& calculated_Values, QString calc_Mode);
	~Unwrapped_Reflection();

	int num_Threads;
	int num_Layers;
	int num_Boundaries;
	int num_Media;
	int max_Depth;
	int num_Points = 0;

	bool depth_Grading;
	bool sigma_Grading;
	const Calc_Functions& calc_Functions;
	Calculated_Values& calculated_Values;

	QString calc_Mode;
	Unwrapped_Structure* unwrapped_Structure;
	const Data& measurement;

	vector<vector<complex<double>>> r_Fresnel_s;	//	[thread][boundary]
	vector<vector<complex<double>>> r_Fresnel_p;	//	[thread][boundary]
	vector<vector<complex<double>>> r_Local_s;		//	[thread][boundary]
	vector<vector<complex<double>>> r_Local_p;		//	[thread][boundary]

	vector<vector<complex<double>>> t_Fresnel_s;	//	[thread][boundary]
	vector<vector<complex<double>>> t_Fresnel_p;	//	[thread][boundary]
	vector<vector<complex<double>>> t_Local_s;		//	[thread][boundary]
	vector<vector<complex<double>>> t_Local_p;		//	[thread][boundary]

	vector<vector<complex<double>>> hi;				//	[thread][media]
	vector<vector<complex<double>>> exponenta;		//	[thread][layer]
	vector<vector<complex<double>>> exponenta_2;	//	[thread][layer]

	vector<double> environment_Factor_s;	//	[thread]
	vector<double> environment_Factor_p;	//	[thread]

	vector<complex<double>> epsilon_Ambient;	//	[thread]
	vector<complex<double>> epsilon_Substrate;	//	[thread]

	vector<vector<complex<double>>> weak_Factor_R;		//	[thread][boundary]
	vector<vector<complex<double>>> weak_Factor_T;		//	[thread][boundary]

	int fill_s__Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_p__Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_sp_Max_Depth_2(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	void fill_Epsilon_Ambient_Substrate(int thread_Index, vector<complex<double>>& epsilon_In_Depth);

	void calc_Hi		 (double k, double cos2,
						  const vector<complex<double>>& eps,
						  int thread_Index);
	void calc_Weak_Factor(int thread_Index);
	void calc_Fresnel	 (double polarization,
						  const vector<complex<double>>& eps,
						  int thread_Index);
	void calc_Exponenta	 (int thread_Index, const vector<double>& thickness);
	void calc_Local		 (double polarization, int thread_Index);
	void calc_Field		 (double polarization, int thread_Index, int point_Index, const vector<complex<double>>& epsilon_Vector);
	void calc_Environmental_Factor(int thread_Index);

	// for sigma grading
	void multifly_Fresnel_And_Weak_Factor(double polarization, int thread_Index);

	// fields
	vector<double> boundaries_Enlarged;
	vector<vector<complex<double>>> U_i_s;		//	[thread][media]
	vector<vector<complex<double>>> U_r_s;		//	[thread][media]
	vector<vector<complex<double>>> U_i_p;		//	[thread][media]
	vector<vector<complex<double>>> U_r_p;		//	[thread][media]

	void fill_Specular_Values            (const Data& measurement, int thread_Index, int point_Index);
	void calc_Specular_1_Point_1_Thread  (const Data& measurement, int thread_Index, int point_Index);
	void calc_Specular_nMin_nMax_1_Thread(const Data& measurement, int n_Min, int n_Max, int thread_Index);
	void calc_Specular();

	double find_Min_Mesh_Step(const QVector<double>& argument);
	void condense_Curve(const QVector<double>& sparse_Argument, const vector<double>* input_Sparse_Curve, double real_Delta, vector<double>& output_Dense_Curve, vector<double>& output_Dense_Argument);
	void wrap_Condensed_Curve(const QVector<double>& sparse_Argument, const vector<double>* sparse_Input_Curve, const vector<double>& dense_Argument, const vector<double>& dense_Curve, const vector<double>& resolution, vector<double>* output_Sparse_Curve);
	void interpolate_Curve(int res_Points, const QVector<double>& argument, const vector<double>& resolution, const vector<double>& input_Curve, vector<double>& output_Curve);
};

#endif // UNWRAPPED_REFLECTION_H
