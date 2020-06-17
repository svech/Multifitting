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
						const tree<Node>& calc_Tree,
						const Data& measurement,
						const vector<Node*>& media_Node_Map_Vector,
						int num_Media_Sharp,
						bool depth_Grading,
						bool sigma_Grading,
						Discretization_Parameters discretization_Parameters,
						gsl_rng* r);

	gsl_rng * r;

	int num_Threads;
	int num_Media_Sharp;
	int num_Boundaries;
	int num_Layers;

	bool depth_Grading;
	bool sigma_Grading;
	Discretization_Parameters discretization_Parameters;

	const tree<Node>& calc_Tree;
	const vector<Node*>& media_Node_Map_Vector;
	const Calc_Functions& calc_Functions;
	Multilayer* multilayer;

	vector<complex<double>> epsilon;								//	[media]
	vector<vector<complex<double>>> epsilon_Dependent;				//	[wavelength][media]

	double max_Sigma;
	vector<double> sigma_Diffuse;									//	[boundary]
	vector<bool> common_Sigma_Diffuse;								//	[boundary]
	vector<QVector<Interlayer>> boundary_Interlayer_Composition;	//  [boundary][function] not use in multithreaded mode
	vector<vector<QVector<Interlayer>>> boundary_Interlayer_Composition_Threaded;	//  [thread][boundary][function]
	vector<double> thickness;										//	[layer]
	vector<vector<double>> thickness_Threaded;						//	[thread][layer]
	vector<vector<double>> sigma_Roughness_Threaded;				//	[thread][boundary]
	vector<vector<double>> mu;										//	[thread][layer]
	vector<vector<double>> omega;									//	[thread][layer]
	vector<vector<double>> omega_pow23;								//	[thread][layer]
	vector<vector<double>> alpha;									//	[thread][layer]
	vector<double> boundaries;										//	[boundary]
	vector<vector<double>> PSD_mu_alpha;							//	[thread][layer]
	vector<vector<double>> PSD_mu_alpha_h;							//	[thread][layer]
	vector<vector<double>> boundaries_Threaded;						//	[thread][boundary]
	vector<double> layer_Norm_Vector;								//	[layer]
	vector<vector<double>> layer_Norm_Vector_Threaded;				//	[thread][layer]

	// discretized structure
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



	void fill_Epsilon		  ();
//	void fill_Epsilon_Dependent();

//	int fill_Epsilon		  (const tree<Node>::iterator& parent, int media_Index = 0);
	int fill_Epsilon_Dependent(const tree<Node>::iterator& parent, int num_Lambda_Points, size_t media_Index = 0);
	int fill_Sigma    (const tree<Node>::iterator& parent, double& max_Sigma, int boundary_Index = 0, int per_Index = 0);
	int fill_Thickness_And_Boundaries(const tree<Node>::iterator& parent, int layer_Index = 0,    int per_Index = 0);
	int fill_Thickness_Sigma_Mu_And_Alpha(const tree<Node>::iterator& parent, int layer_Index = 0,    int per_Index = 0);
	void fill_PSD_Inheritance_Powers();
};

#endif // UNWRAPPED_STRUCTURE_H
