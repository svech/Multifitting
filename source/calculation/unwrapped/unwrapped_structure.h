#ifndef UNWRAPPED_STRUCTURE_H
#define UNWRAPPED_STRUCTURE_H

#include "calculation/node.h"
#include <chrono>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

class Multilayer;

class Unwrapped_Structure
{
public:
	Unwrapped_Structure(Multilayer* multilayer,
						const Calc_Functions& calc_Functions,
						const vector<Node*>& media_Node_Map_Vector,
						const vector<Data*>& media_Data_Map_Vector,
						const vector<int>& media_Period_Index_Map_Vector,
						const Data& measurement,
						int num_Media_Sharp,
						bool depth_Grading,
						bool sigma_Grading,
						gsl_rng* r);
	~Unwrapped_Structure();

	gsl_rng* r;

	int num_Threads;
	int num_Media_Sharp;
	int num_Boundaries;
	int num_Layers;

	bool depth_Grading;
	bool sigma_Grading;

	Multilayer* multilayer;
	const Calc_Functions& calc_Functions;
	const vector<Node*>& media_Node_Map_Vector;
	const vector<Data*>& media_Data_Map_Vector;
	const vector<int>& media_Period_Index_Map_Vector;
	const Data& measurement;

	///-------------------------------------------------
	// SHARP STRUCTURE
	///-------------------------------------------------
	vector<complex<double>> epsilon;								//	[media]
	vector<vector<complex<double>>> epsilon_Dependent;				//	[wavelength][media]

	double max_Sigma = 0.1;
	double min_Sigma = 1000;
	vector<double> sigma_Diffuse;									//	[boundary]
	vector<bool> common_Sigma_Diffuse;								//	[boundary]

	vector<QVector<Interlayer>> boundary_Interlayer_Composition;	//  [boundary][function] not use in multithreaded mode
	vector<vector<QVector<Interlayer>>> boundary_Interlayer_Composition_Threaded;	//  [thread][boundary][function]

	vector<double> thickness;										//	[layer]
	vector<vector<double>> thickness_Threaded;						//	[thread][layer]

	vector<double> boundaries_Position;								//	[boundary]
	vector<vector<double>> boundaries_Position_Threaded;			//	[thread][boundary]

	//--------------------------------------------------------------
	/// roughness
	//--------------------------------------------------------------
	vector<double> sigma_Roughness;									//	[boundary]
	vector<vector<double>> sigma_Roughness_Threaded;				//	[thread][boundary]

	vector<double> omega;											//	[layer]
	vector<vector<double>> omega_Threaded;							//	[thread][layer]

	vector<double> mu;												//	[layer]
	vector<vector<double>> mu_Threaded;								//	[thread][layer]

	vector<double> omega_pow23;										//	[layer]
	vector<vector<double>> omega_pow23_Threaded;					//	[thread][layer]

	vector<double> alpha;											//	[boundary]
	vector<vector<double>> alpha_Threaded;							//	[thread][boundary]

	vector<double> PSD_mu_alpha;									//	[layer]
	vector<vector<double>> PSD_mu_alpha_Threaded;					//	[thread][layer]

	vector<double> PSD_mu_alpha_h;									//	[layer]
	vector<vector<double>> PSD_mu_alpha_h_Threaded;					//	[thread][layer]

	//--------------------------------------------------------------

	void fill_Epsilon_Sharp();
	void fill_Sigma_Diffuse_And_Interlayers();
	void fill_Thickness_And_Boundaries_Position();
	void fill_Roughness_Parameters();
	void fill_PSD_Inheritance_Powers();

	///-------------------------------------------------------------
	// DISCRETIZATION
	///-------------------------------------------------------------

	double discretization_Step_We_Use;
	gsl_spline* discretized_Epsilon_Spline_Re;
	gsl_spline* discretized_Epsilon_Spline_Im;
	gsl_interp_accel* discretized_Epsilon_Acc_Re;
	gsl_interp_accel* discretized_Epsilon_Acc_Im;

	vector<double> layer_Norm_Vector;								//	[layer]
	vector<vector<double>> layer_Norm_Vector_Threaded;				//	[thread][layer]

	vector<complex<double>> discretized_Epsilon;
	vector<vector<complex<double>>> discretized_Epsilon_Dependent;

	vector<double> discretized_Thickness;
	vector<double> discretized_Slices_Boundaries;
	vector<double> z_Positions;
	int num_Discretized_Media;
	double prefix = 15, suffix = 15;
	int num_Prefix_Slices =1, num_Suffix_Slices = 1;

	void layer_Normalizing();
	void find_Discretization();
	void find_Z_Positions();
	void fill_Discretized_Epsilon_Dependent(int num_Lambda_Points);
	void fill_Discretized_Epsilon();
	void epsilon_Func(double z, int media_Index, bool is_Dependent,

								const vector<complex<double>>& epsilon_Vector,
								const vector<vector<complex<double>>>& epsilon_Dependent_Vector,

								vector<complex<double>>& discretized_Epsilon,
								vector<vector<complex<double>>>& discretized_Epsilon_Dependent,
								int thread_Index);

	// field spacing
	int num_Field_Slices = 0;
	vector<double> field_Z_Positions;
	void find_Field_Spacing();
	int get_Layer_or_Slice_Index(double z);
};

#endif // UNWRAPPED_STRUCTURE_H
