#ifndef UNWRAPPED_REFLECTION_H
#define UNWRAPPED_REFLECTION_H

#include "unwrapped_structure.h"
#include "gsl/gsl_integration.h"
#include "iostream"

class Multilayer;

class Unwrapped_Reflection
{
public:
	Unwrapped_Reflection(Multilayer* multilayer, Unwrapped_Structure* unwrapped_Structure, int num_Media,
						 const Data& measurement, bool depth_Grading, bool sigma_Grading,
						 const Calc_Functions& calc_Functions, Calculated_Values& calculated_Values, QString calc_Mode, QString spec_Scat_mode);

	~Unwrapped_Reflection();

	int num_Threads;
	int num_Layers;
	int num_Boundaries;
	int num_Media;
	int max_Depth;
	int depth_Threshold;
	size_t num_Points;
	size_t phi_Points;
	size_t short_Phi_Points;

	bool depth_Grading;
	bool sigma_Grading;
	const Calc_Functions& calc_Functions;
	Calculated_Values& calculated_Values;

	QString calc_Mode;
	QString spec_Scat_mode;
	Unwrapped_Structure* unwrapped_Structure;
	Multilayer* multilayer;
	const Data& measurement;
	Node* substrate_Node;
	Data substrate;

	double s_Weight;
	double p_Weight;

	vector<double(*)(double, double, double, double, double, double, gsl_spline*, gsl_interp_accel*)> PSD_1D_Func_Vec;
	vector<double(*)(double, double, double, double, double, double, double)> PSD_2D_Func_Vec;

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

	QMap<id_Type, int> id_Item_Map;
	vector<Data> appropriate_Item_Vec;					//	[item_Index]
	vector<Node> appropriate_Node_Vec;					//	[item_Index]
	vector<int> boundary_Item_Vec;						//	[boundary]

	vector<vector<double>> intensity_Term_Boundary_s;			//	[thread][boundary]
	vector<vector<double>> intensity_Term_Boundary_p;			//	[thread][boundary]
	vector<vector<complex<double>>> field_Term_Boundary_s;		//	[thread][boundary]
	vector<vector<complex<double>>> field_Term_Boundary_p;		//	[thread][boundary]
	vector<vector<vector<double>>> half_Sum_Field_Term_s;		//	[thread][boundary][layer]
	vector<vector<vector<double>>> half_Sum_Field_Term_p;		//	[thread][boundary][layer]

	vector<vector<double>> PSD_Factor_Item;						//	[thread][item_Index]
	vector<vector<double>> PSD_Factor_Boundary;					//	[thread][boundary]
	vector<vector<vector<double>>> cross_Exp_Factor_2D;			//	[thread][boundary][layer]

	int fill_s__Max_Depth_3(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_p__Max_Depth_3(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);
	int fill_sp_Max_Depth_3(const tree<Node>::iterator& parent, int thread_Index, int point_Index, int media_Index = 0);

	void fill_Item_Id_Map();
	void fill_Item_PSD_1D(int thread_Index, double cos_Theta, double cos_Theta_0);
	void fill_Item_PSD_2D(int thread_Index, int point_Index, int phi_Index);
	int fill_Boundary_Item_PSD(const tree<Node>::iterator &parent, int boundary_Index = 0);
	void fill_Epsilon_Ambient_Substrate	(int thread_Index,						  const vector<complex<double>>& epsilon_Vector);

	void calc_Hi						(int thread_Index, double k, double cos2, const vector<complex<double>>& epsilon_Vector);
	void calc_Weak_Factor				(int thread_Index);
	void calc_Fresnel					(int thread_Index,						  const vector<complex<double>>& epsilon_Vector);
	void calc_Exponenta					(int thread_Index, const vector<double>& thickness);
	void calc_Local						(int thread_Index);
	void calc_Amplitudes_Field			(int thread_Index, int point_Index);
	void calc_Sliced_Field				(int thread_Index, int point_Index,		  const vector<complex<double>>& epsilon_Vector);
	double calc_Field_Term_Sum			(QString polarization, int point_Index, int thread_Index);
	void calc_Environmental_Factor		(int thread_Index);
	void choose_PSD_1D_Function(const Data& struct_Data, int thread_Index);
	void choose_PSD_2D_Function(int point_Index, int thread_Index);
	double azimuthal_Integration (gsl_function* function, double delta);

	// for sigma grading
	void multifly_Fresnel_And_Weak_Factor(int thread_Index);

	// fields
	vector<double> boundaries_Enlarged;
	vector<vector<complex<double>>> U_i_s;		//	[thread][media]
	vector<vector<complex<double>>> U_r_s;		//	[thread][media]
	vector<vector<complex<double>>> U_i_p;		//	[thread][media]
	vector<vector<complex<double>>> U_r_p;		//	[thread][media]

	vector<gsl_spline*> spline_Vec;
	vector<gsl_interp_accel*> acc_Vec;
	vector<vector<double>> GISAS_Slice;
	vector<vector<double>> phi_Slice;


	void fill_Specular_Values            (int thread_Index, int point_Index);
	void calc_Specular_1_Point_1_Thread  (int thread_Index, int point_Index);
	void calc_Specular_nMin_nMax_1_Thread(int n_Min, int n_Max, int thread_Index);
	void calc_Specular();

	double find_Min_Mesh_Step(const QVector<double>& argument);
	void condense_Curve(const QVector<double>& sparse_Argument, const vector<double>* input_Sparse_Curve, double real_Delta, vector<double>& output_Dense_Curve, vector<double>& output_Dense_Argument);
	void wrap_Condensed_Curve(const QVector<double>& sparse_Argument, const vector<double>* sparse_Input_Curve, const vector<double>& dense_Argument, const vector<double>& dense_Curve, const vector<double>& resolution, vector<double>* output_Sparse_Curve);
	void interpolate_Curve(int res_Points, const QVector<double>& argument, const vector<double>& resolution, const vector<double>& input_Curve, vector<double>& output_Curve);
};

#endif // UNWRAPPED_REFLECTION_H
