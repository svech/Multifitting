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

	QString active_Whats_This;
	Unwrapped_Structure* unwrapped_Structure;
	Measurement measurement;

	MyVector<MyVector<complex<double>>> r_Fresnel_s;	//	[thread][boundary]
	MyVector<MyVector<complex<double>>> r_Fresnel_p;	//	[thread][boundary]
	MyVector<MyVector<complex<double>>> r_Local_s;		//	[thread][boundary]
	MyVector<MyVector<complex<double>>> r_Local_p;		//	[thread][boundary]
	MyVector<MyVector<complex<double>>> hi;				//	[thread][media]
	MyVector<MyVector<complex<double>>> exponenta;		//	[thread][layer]
	MyVector<MyVector<double>> weak_Factor;				//	[thread][boundary]

	MyVector<complex<double>> r_s;
	MyVector<complex<double>> r_p;
	MyVector<double> R_s;
	MyVector<double> R_p;
	MyVector<double> R;

	//---------------------------------------------------
	// test real-valued calculation
	MyVector<MyVector<double>> r_Fresnel_s_RE;		//	[thread][boundary]
	MyVector<MyVector<double>> r_Fresnel_s_IM;		//	[thread][boundary]
	MyVector<MyVector<double>> r_Fresnel_p_RE;		//	[thread][boundary]
	MyVector<MyVector<double>> r_Fresnel_p_IM;		//	[thread][boundary]
	MyVector<MyVector<double>> r_Local_s_RE;		//	[thread][boundary]
	MyVector<MyVector<double>> r_Local_s_IM;		//	[thread][boundary]
	MyVector<MyVector<double>> r_Local_p_RE;		//	[thread][boundary]
	MyVector<MyVector<double>> r_Local_p_IM;		//	[thread][boundary]
	MyVector<MyVector<double>> hi_RE;				//	[thread][media]
	MyVector<MyVector<double>> hi_IM;				//	[thread][media]
	MyVector<MyVector<double>> exponenta_RE;		//	[thread][layer]
	MyVector<MyVector<double>> exponenta_IM;		//	[thread][layer]

	MyVector<complex<double>> r_s_RV;
	MyVector<complex<double>> r_p_RV;
	MyVector<double> R_s_RV;
	MyVector<double> R_p_RV;
	MyVector<double> R_RV;
	//---------------------------------------------------

	void calc_Hi		 (double k, double cos2, int thread_Index);
	void calc_Hi_Real_Val(double k, double cos2, int thread_Index);
	void calc_Weak_Factor			(int thread_Index);
	void calc_Weak_Factor_Real_Val	(int thread_Index);
	void calc_Exponenta				(int thread_Index);
	void calc_Exponenta_Real_Val	(int thread_Index);
	void calc_Fresnel			(double polarization, int thread_Index);
	void calc_Fresnel_Real_Val	(double polarization, int thread_Index);
	void calc_Local				(double polarization, int thread_Index);
	void calc_Local_Real_Val	(double polarization, int thread_Index);

	void calc_Reflectivity_1_Point_1_Thread(double k, double cos2, double polarization, int thread_Index);
	void calc_Reflectivity_Angular_nMin_nMax_1_Thread(double k, QVector<double>& cos2_Vec, int n_Min, int n_Max, double polarization, int thread_Index);
	void calc_Reflectivity();
};

#endif // UNWRAPPED_REFLECTION_H
