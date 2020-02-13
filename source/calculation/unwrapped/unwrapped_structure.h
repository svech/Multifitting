#ifndef UNWRAPPED_STRUCTURE_H
#define UNWRAPPED_STRUCTURE_H

#include "calculation/node.h"
#include <chrono>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

class Unwrapped_Structure
{
public:
	Unwrapped_Structure(const tree<Node>& calc_Tree, const Data& measurement, QString active_Parameter_Whats_This, int num_Media, int max_Depth, bool depth_Grading, bool sigma_Grading, Discretization_Parameters discretization_Parameters, gsl_rng* r);

	gsl_rng * r;

	int num_Threads;
	int num_Media;
	int num_Boundaries;
	int num_Layers;
	int max_Depth;

	bool depth_Grading;
	bool sigma_Grading;
	Discretization_Parameters discretization_Parameters;

	const tree<Node>& calc_Tree;

	vector<complex<double>> epsilon;								//	[media]
	vector<double> epsilon_RE;										//	[media]
	vector<double> epsilon_IM;										//	[media]
	vector<double> epsilon_NORM;									//	[media]

	// if epsilon is dependent on variable
	vector<vector<complex<double>>> epsilon_Dependent;				//	[wavelength][media]
	vector<vector<double>> epsilon_Dependent_RE;					//	[wavelength][media]
	vector<vector<double>> epsilon_Dependent_IM;					//	[wavelength][media]
	vector<vector<double>> epsilon_Dependent_NORM;					//	[wavelength][media]

	double max_Sigma;
	vector<double> sigma;											//	[boundary]
	vector<bool> common_Sigma;										//	[boundary]
	vector<vector<Interlayer>> boundary_Interlayer_Composition;		//  [boundary][function]
	vector<double> thickness;										//	[layer]
	vector<double> boundaries;										//	[boundary]

	// discretized structure
	vector<complex<double>> discretized_Epsilon;
	vector<double> discretized_Epsilon_RE;
	vector<double> discretized_Epsilon_IM;
	vector<double> discretized_Epsilon_NORM;

	vector<vector<complex<double>>> discretized_Epsilon_Dependent;
	vector<vector<double>> discretized_Epsilon_Dependent_RE;
	vector<vector<double>> discretized_Epsilon_Dependent_IM;
	vector<vector<double>> discretized_Epsilon_Dependent_NORM;

	vector<double> discretized_Thickness;
	double prefix = 15, suffix = 15;
	int num_Prefix_Slices =1, num_Suffix_Slices = 1;

	void find_Discretization();
	void fill_Discretized_Epsilon_Dependent(int num_Lambda_Points);
	void fill_Discretized_Epsilon();
	complex<double> epsilon_Func(double z);

//	int fill_Epsilon_Angular_Max_Depth_2  (const tree<Node>::iterator& parent, int media_Index = 0);
//	int fill_Epsilon_Spectra_Max_Depth_2  (const tree<Node>::iterator& parent, int media_Index = 0);
//	int fill_Sigma_Max_Depth_2    (const tree<Node>::iterator& parent, int boundary_Index = 0);
//	int fill_Thickness_Max_Depth_2(const tree<Node>::iterator& parent, int layer_Index = 0);

	int fill_Epsilon		  (const tree<Node>::iterator& parent, int media_Index = 0);
	int fill_Epsilon_Dependent(const tree<Node>::iterator& parent, int num_Lambda_Points, int media_Index = 0);
	int fill_Sigma    (const tree<Node>::iterator& parent, double& max_Sigma, int boundary_Index = 0, int per_Index = 0);
	int fill_Thickness_And_Boundaries(const tree<Node>::iterator& parent, int layer_Index = 0,    int per_Index = 0);
};

#endif // UNWRAPPED_STRUCTURE_H
