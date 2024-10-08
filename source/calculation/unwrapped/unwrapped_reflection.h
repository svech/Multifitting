#ifndef UNWRAPPED_REFLECTION_H
#define UNWRAPPED_REFLECTION_H

#include "unwrapped_structure.h"
#include "gsl/gsl_integration.h"
#include "iostream"
#include "Faddeeva.hh"

class Multilayer;

class Unwrapped_Reflection
{
public:
	Unwrapped_Reflection(const vector<Node*>& short_Flat_Calc_Tree, Calculated_Values& calculated_Values, Unwrapped_Structure* unwrapped_Structure, QString spec_Scat_mode);
	~Unwrapped_Reflection();

	int num_Threads;
	int num_Layers;
	int num_Layers_Sharp;
	int num_Boundaries;
	int num_Boundaries_Sharp;
	int num_Media;
	int num_Media_Sharp;

	bool if_Single_Beam_Value = false;

	size_t num_Points;
	size_t phi_Points;
	size_t short_Phi_Points;

	const Calc_Functions& calc_Functions;
	Calculated_Values& calculated_Values;

	QString spec_Scat_mode;
	Unwrapped_Structure* unwrapped_Structure;
	Multilayer* multilayer;
	int DWBA_n_Max_Series;
	const Data& measurement;

	const vector<Node*>& short_Flat_Calc_Tree;
	const vector<Node*>& media_Node_Map_Vector;
	const vector<Data>& media_Data_Map_Vector;
	Node* substrate_Node;
	Data substrate;

	double s_Weight;
	double p_Weight;

	bool has_S_Pol;
	bool has_P_Pol;

	bool pure_S_Pol;
	bool pure_P_Pol;
	bool mixed_Pol;

	vector<vector<complex<double>>> r_Fresnel_s;	//	[thread][boundary]
	vector<vector<complex<double>>> r_Fresnel_p;	//	[thread][boundary]
	vector<vector<complex<double>>> r_Local_s;		//	[thread][boundary]
	vector<vector<complex<double>>> r_Local_p;		//	[thread][boundary]
	vector<vector<complex<double>>> r_Exp_s;		//	[thread][boundary]
	vector<vector<complex<double>>> r_Exp_p;		//	[thread][boundary]

	vector<vector<complex<double>>> t_Fresnel_s;	//	[thread][boundary]
	vector<vector<complex<double>>> t_Fresnel_p;	//	[thread][boundary]
	vector<vector<complex<double>>> t_Local_s;		//	[thread][boundary]
	vector<vector<complex<double>>> t_Local_p;		//	[thread][boundary]

	vector<vector<complex<double>>>& hi;			//	[point][media]
	vector<vector<complex<double>>>& exponenta;		//	[point][boundary/layer]
	vector<vector<complex<double>>>  exponenta_2;	//	[thread][boundary/layer]

	vector<double> environment_Factor_s;	//	[thread]
	vector<double> environment_Factor_p;	//	[thread]

	vector<complex<double>> epsilon_Ambient;	//	[thread]
	vector<complex<double>> epsilon_Substrate;	//	[thread]

//	vector<vector<complex<double>>> weak_Factor_R;		//	[thread][boundary]
//	vector<vector<complex<double>>> weak_Factor_T;		//	[thread][boundary]
	vector<vector<double>> weak_Factor_R;		//	[thread][boundary]
	vector<vector<double>> weak_Factor_T;		//	[thread][boundary]
	vector<double> specular_Debye_Waller_Weak_Factor_R_Final; //	[thread]

	// fields
	vector<double> boundaries_Enlarged;
	vector<vector<complex<double>>>& U_i_s;		//	[point][media]
	vector<vector<complex<double>>>& U_r_s;		//	[point][media]
	vector<vector<complex<double>>>& U_i_p;		//	[point][media]
	vector<vector<complex<double>>>& U_r_p;		//	[point][media]

	///---------------------------------------------------------------------
	// PT
	///---------------------------------------------------------------------
	vector<double(*)(double, double, double, double, double, double, gsl_spline*, gsl_interp_accel*)> PSD_1D_Func_Vec;
	vector<double(*)(double, double, double, double, double, double, gsl_spline*, gsl_interp_accel*)> PSD_Gauss_Peak_1D_Func_Vec;
	vector<double(*)(double, double, double, double, double, double, double, gsl_spline*, gsl_interp_accel*)> PSD_2D_Func_Vec;
	vector<double(*)(double, double, double, double, double, double, double, gsl_spline*, gsl_interp_accel*)> PSD_Gauss_Peak_2D_Func_Vec;
	vector<double(*)(double, double, double)> Cor_Func_Vec;

	QMap<id_Type, int> id_Item_Map;
	vector<Data> appropriate_Item_Vec;					//	[item_Index]
	vector<int> used_Appropriate_Item_Index_Vec;		//  (indices of layers with used particles)
	vector<int> boundary_Item_Vec;						//	[boundary]
	vector<vector<int>> boundaries_Of_Item_Vec;			//	[item_Index][boundary]

	vector<vector<double>> intensity_Term_Boundary_s;			//	[thread][boundary]
	vector<vector<double>> intensity_Term_Boundary_p;			//	[thread][boundary]
	vector<vector<complex<double>>> field_Term_Boundary_s;		//	[thread][boundary]
	vector<vector<complex<double>>> field_Term_Boundary_p;		//	[thread][boundary]
	vector<vector<vector<double>>> half_Sum_Field_Term_s;		//	[thread][boundary][layer]
	vector<vector<vector<double>>> half_Sum_Field_Term_p;		//	[thread][boundary][layer]

	vector<vector<double>> PSD_Factor_Item;						//	[thread][item_Index]
	vector<vector<double>> PSD_Factor_Boundary;					//	[thread][boundary]
	vector<vector<vector<double>>> cross_Exp_Factor_2D;			//	[thread][boundary][layer]

	vector<gsl_spline*> spline_Vec;
	vector<gsl_interp_accel*> acc_Vec;
	vector<vector<double>> GISAS_Slice;
	vector<vector<double>> phi_Slice;
	bool delete_GISAXS_Spline = false;
	bool delete_Particles_F_Spline = false;

	///---------------------------------------------------------------------
	// DWBA SA CSA
	///---------------------------------------------------------------------

	// up
	vector<vector<complex<double>>> b1_Up_Boundary_s;	//	[thread][boundary]
	vector<vector<complex<double>>> b2_Up_Boundary_s;	//	[thread][boundary]
	vector<vector<complex<double>>> b3_Up_Boundary_s;	//	[thread][boundary]
	vector<vector<complex<double>>> b4_Up_Boundary_s;	//	[thread][boundary]

	vector<vector<complex<double>>> b1_Up_Boundary_p;	//	[thread][boundary]
	vector<vector<complex<double>>> b2_Up_Boundary_p;	//	[thread][boundary]
	vector<vector<complex<double>>> b3_Up_Boundary_p;	//	[thread][boundary]
	vector<vector<complex<double>>> b4_Up_Boundary_p;	//	[thread][boundary]

	vector<vector<complex<double>>> k1_Up_Boundary;		//	[thread][boundary]
	vector<vector<complex<double>>> k2_Up_Boundary;		//	[thread][boundary]
	vector<vector<complex<double>>> k3_Up_Boundary;		//	[thread][boundary]
	vector<vector<complex<double>>> k4_Up_Boundary;		//	[thread][boundary]

	vector<vector<complex<double>>> D1_Up;	//	[thread][n term]
	vector<vector<complex<double>>> D2_Up;	//	[thread][n term]
	vector<vector<complex<double>>> D3_Up;	//	[thread][n term]
	vector<vector<complex<double>>> D4_Up;	//	[thread][n term]

	// low
	vector<vector<complex<double>>> b1_Low_Boundary_s;	//	[thread][boundary]
	vector<vector<complex<double>>> b2_Low_Boundary_s;	//	[thread][boundary]
	vector<vector<complex<double>>> b3_Low_Boundary_s;	//	[thread][boundary]
	vector<vector<complex<double>>> b4_Low_Boundary_s;	//	[thread][boundary]

	vector<vector<complex<double>>> b1_Low_Boundary_p;	//	[thread][boundary]
	vector<vector<complex<double>>> b2_Low_Boundary_p;	//	[thread][boundary]
	vector<vector<complex<double>>> b3_Low_Boundary_p;	//	[thread][boundary]
	vector<vector<complex<double>>> b4_Low_Boundary_p;	//	[thread][boundary]

	vector<vector<complex<double>>> k1_Low_Boundary;	//	[thread][boundary]
	vector<vector<complex<double>>> k2_Low_Boundary;	//	[thread][boundary]
	vector<vector<complex<double>>> k3_Low_Boundary;	//	[thread][boundary]
	vector<vector<complex<double>>> k4_Low_Boundary;	//	[thread][boundary]

	vector<vector<complex<double>>> D1_Low;	//	[thread][n]
	vector<vector<complex<double>>> D2_Low;	//	[thread][n]
	vector<vector<complex<double>>> D3_Low;	//	[thread][n]
	vector<vector<complex<double>>> D4_Low;	//	[thread][n]

	// K
	vector<vector<vector<complex<double>>>> K_Factor_Boundary_s;	//	[thread][boundary][n]
	vector<vector<vector<complex<double>>>> K_Factor_Boundary_p;	//	[thread][boundary][n]
	vector<double> hermites;							// [n]
	vector<double> factorial;							// [n]
	vector<ooura_fourier_cos<double>> integrator_Vec;	//	[thread]

	vector<vector<double>> pre_Fourier_Factor;			//	[thread][n]
	vector<vector<vector<double>>> incoherent_Diagonal_Term;	//	[thread][item_Index][n]
	vector<vector<double>> cor_Func_Item;						//	[thread][item_Index]

	///---------------------------------------------------------------------
	// particles
	///---------------------------------------------------------------------

	vector<vector<vector<complex<double>>>> k_03;	//	[thread][item_Index][0-3]
	vector<vector<vector<complex<double>>>> exp_03;	//	[thread][layer][0-3]

	vector<vector<vector<complex<double>>>>			C_03_s;		//	[thread][layer][0-3]
	vector<vector<vector<vector<complex<double>>>>> C_03_03_s;	//	[thread][layer][0-3][0-3]
	vector<vector<vector<double>>>					C_03_s_norm;//	[thread][layer][0-3]

	vector<vector<vector<complex<double>>>>			C_03_p;		//	[thread][layer][0-3]
	vector<vector<vector<vector<complex<double>>>>> C_03_03_p;	//	[thread][layer][0-3][0-3]
	vector<vector<vector<double>>>					C_03_p_norm;//	[thread][layer][0-3]

	// for any polarization
	vector<vector<vector<complex<double>>>>			C_03;		//	[thread][layer][0-3]
	vector<vector<vector<vector<complex<double>>>>> C_03_03;	//	[thread][layer][0-3][0-3]
	vector<vector<vector<double>>>					C_03_norm;  //	[thread][layer][0-3]

	vector<vector<complex<double>(*)(        const complex<double>&, double, double, double)>> prefactor_2D_Func_Vec; // [thread][item_Index]
	vector<vector<complex<double>(*)(double, const complex<double>&, double, double, double)>> q_Factor_2D_Func_Vec; // [thread][item_Index]
	vector<vector<complex<double>(*)(double, const complex<double>&, double, double, double)>> form_Factor_2D_Func_Vec; // [thread][item_Index]
	vector<vector<vector<complex<double>>>> F_03;		//	[thread][item_Index][0-3]
	vector<vector<vector<complex<double>>>> F_pre_03;	//	[thread][item_Index][0-3]
	vector<vector<vector<complex<double>>>> w_03;		//	[thread][item_Index][0-3]
	vector<vector<vector<vector<complex<double>>>>> g_03_03;		//	[thread][item_Index][0-3][0-3]
	vector<vector<vector<vector<complex<double>>>>> alfa_03_03;		//	[thread][item_Index][0-3][0-3]
	vector<vector<vector<double>>>                  alfa_nn_03;		//	[thread][item_Index][0-3]
	vector<vector<complex<double>>> coherent_Field_Factor;			//	[thread][phi_Index]
	vector<vector<complex<double>>> complex_Coef;					//	[thread][0-3]
	vector<vector<vector<complex<double>>>> layer_Field_Factor;		//	[thread][phi_Index][layer]
	vector<vector<complex<double>>> layer_Field_Factor_Short;		//	[thread][layer]

	int q_Spline_Points;
	vector<vector<vector<gsl_spline*>>> spline_F_03_Real;	//	[thread][item_Index][0-3]
	vector<vector<vector<gsl_spline*>>> spline_F_03_Imag;	//	[thread][item_Index][0-3]
	vector<vector<vector<gsl_interp_accel*>>> acc_F_03_Real;//	[thread][item_Index][0-3]
	vector<vector<vector<gsl_interp_accel*>>> acc_F_03_Imag;//	[thread][item_Index][0-3]
	bool delete_F_03_Spline = false;

	///---------------------------------------------------------------------

	void fill_Components_From_Node_Vector(int thread_Index, int point_Index);
	void fill_DW_Factors_From_Node_Vector(int thread_Index, int point_Index);

	void fill_Item_Id_Map();
	void fill_Boundary_Item();

	// specular
	void calc_Hi						(int point_Index, double k, double cos2, const vector<complex<double>>& epsilon_Vector);
	void calc_Weak_Factor				(int thread_Index, int point_Index);
	void calc_Fresnel					(int thread_Index, int point_Index, const vector<complex<double>>& epsilon_Vector);
	void calc_Exponenta					(int thread_Index, int point_Index, const vector<double>& thickness);
	void calc_Local						(int thread_Index, int point_Index);
	void calc_Environmental_Factor		(int thread_Index, int point_Index);

	// fields
	void calc_Amplitudes_Field			(int thread_Index, int point_Index, QString polarization);
	void calc_Sliced_Field				(int thread_Index, int point_Index, const vector<complex<double>>& epsilon_Vector);

	// PT
	void fill_Item_PSD_1D				(int thread_Index, int point_Index, double cos_Theta_0);
	void fill_Item_PSD_2D				(int thread_Index, int point_Index, int phi_Index);
	complex<double> calc_Field_Term_j_Discrete	(int point_Index, int j,
														const vector<complex<double>>& q0_U_i,
														const vector<complex<double>>& q0_U_r,
														const vector<complex<double>>& q_U_i,
														const vector<complex<double>>& q_U_r );
	complex<double> calc_Field_Term_j	(int point_Index, int j, int thread_Index,
														const vector<complex<double>>& q0_Hi,
														const vector<complex<double>>& q_Hi,

														const vector<complex<double>>& b1_Up_Boundary,
														const vector<complex<double>>& b2_Up_Boundary,
														const vector<complex<double>>& b3_Up_Boundary,
														const vector<complex<double>>& b4_Up_Boundary,

														const vector<complex<double>>& b1_Low_Boundary,
														const vector<complex<double>>& b2_Low_Boundary,
														const vector<complex<double>>& b3_Low_Boundary,
														const vector<complex<double>>& b4_Low_Boundary);
	complex<double> calc_Field_Term_j_Simplified(int point_Index, int j, int thread_Index,
														const vector<complex<double>>& q0_Hi,
														const vector<complex<double>>& q_Hi,

														 const vector<complex<double>>& b1_Up_Boundary,
														 const vector<complex<double>>& b2_Up_Boundary,
														 const vector<complex<double>>& b3_Up_Boundary,
														 const vector<complex<double>>& b4_Up_Boundary,

														 const vector<complex<double>>& b1_Low_Boundary,
														 const vector<complex<double>>& b2_Low_Boundary,
														 const vector<complex<double>>& b3_Low_Boundary,
														 const vector<complex<double>>& b4_Low_Boundary);

	double calc_Field_Term_Sum				(QString polarization, int point_Index, int thread_Index);
	void choose_PSD_1D_Function				(const Data& struct_Data, int thread_Index);
	void choose_PSD_2D_Function				(const Data& struct_Data, int thread_Index);
	double roughness_Azimuthal_Integration			(gsl_function* function, double cos_Theta_0, int point_Index);

	// DWBA SA CSA
	void calc_k_Wavenumber_Up_Low		(int thread_Index, int point_Index);
	void calc_Field_Up_Low						(int thread_Index, int point_Index, QString polarization);
	void calc_K_Factor_DWBA_SA_CSA				(int point_Index, int thread_Index,                  QString polarization);
	double calc_K_Factor_Term_Sum_DWBA_SA_CSA	(int thread_Index, QString polarization, int n_Power);
	void choose_Cor_Function					(int thread_Index);
	double function_DWBA_SA_CSA_Batch_Common_Integrand	  (double r, int thread_Index);
	double function_DWBA_SA_CSA_Batch_Individual_Integrand(double r, int thread_Index);
	double cor_Function_Integration		(int point_Index, int thread_Index, double cos_Theta_0);

	// particles
	void calc_k_Wavenumber_Layer	(						int thread_Index, int point_Index);
	void calc_Half_Layer_Exponents	(						int thread_Index);
	void calc_C_Factor				(QString polarization,	int thread_Index, int point_Index);
	void fill_C_Factor_Mixed		(QString polarization,	int thread_Index);
	void calc_C_Factor_Mixed		(						int thread_Index);
	void calc_Omega_Factor			(						int thread_Index);
	void calc_Gamma_Factor			(						int thread_Index);

	void choose_Form_Factor_2D_Function		(int thread_Index);
	void calc_Item_Pre_Form_Factor			(int thread_Index, int item_Index, Data& item);
	void calc_Item_Form_Factor				(int thread_Index, int item_Index, double q, Data& item);
	void calc_Item_Form_Factor_From_Spline	(int thread_Index, int item_Index, double q);
	void calc_Item_Form_Factor_Splines		(int thread_Index, int item_Index, int order_Item_Index, Data& order_Item, double cos_Theta_0, double cos_Theta);
	void choose_disorder_Paracrystal		(int thread_Index);
	void calc_Item_Alfa_Factor_With_G2		(int thread_Index, int item_Index, double G2_Type_Value, double G1_Type_Value);
	void calc_Item_Alfa_Factor_No_G2		(int thread_Index, int item_Index,						 double G1_Type_Value);

	void calc_Item_Alfa_Factor_G1			(int thread_Index, int item_Index,           double G1_Type_Value);
	void calc_Coherent_Coef_G2				(int thread_Index, int item_Index,           double G2_Type_Value_Sqrt, complex<double> d_Eps);

	double body_Scattering_Particles_Partial_Correlation_Z_Deviation(vector<complex<double>>& layer_Field_Factor,   int thread_Index, int item_Index, QString particle_Interference_Function, Node* last_Node, double G1_Type_Value, double G2_Type_Value,      double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps);
	double body_Scattering_Particles_Partial_Correlation		    (vector<complex<double>>& layer_Field_Factor,   int thread_Index, int item_Index, QString particle_Interference_Function, Node* last_Node,						 double G2_Type_Value_Sqrt, double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps);
	double body_Scattering_Particles_Full_Correlation_Z_Deviation   (		complex<double>& coherent_Field_Factor, int thread_Index, int item_Index, QString particle_Interference_Function, Node* last_Node, double G1_Type_Value, double G2_Type_Value_Sqrt, double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps);
	void   body_Scattering_Particles_Full_Correlation			    (		complex<double>& coherent_Field_Factor, int thread_Index, int item_Index, QString particle_Interference_Function, Node* last_Node,                       double G2_Type_Value_Sqrt, double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps);
	double body_Scattering_Particles_Zero_Correlation_Z_Deviation   (												int thread_Index, int item_Index, QString particle_Interference_Function, Node* G2_node,   double G1_Type_Value,							double cos_Theta, double cos_Theta_0, double cos_Phi						);
	double body_Scattering_Particles_Zero_Correlation			    (												int thread_Index, int item_Index, QString particle_Interference_Function, Node* G2_node,						 double G2_Type_Value_Sqrt, double cos_Theta, double cos_Theta_0, double cos_Phi, complex<double>& d_Eps);
	double particles_Azimuthal_Integration	(gsl_function* function, double cos_Theta_0, int point_Index);

	// for sigma grading
	void multifly_Fresnel_And_Weak_Factor(int thread_Index);

	void fill_Specular_Values            (int thread_Index, int point_Index);
	void calc_Specular_1_Point_1_Thread  (int thread_Index, int point_Index);
	void calc_Specular_nMin_nMax_1_Thread(int n_Min, int n_Max, int thread_Index);
	void calc_Specular();
};

#endif // UNWRAPPED_REFLECTION_H
