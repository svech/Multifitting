#ifndef UNWRAPPED_STRUCTURE_H
#define UNWRAPPED_STRUCTURE_H

#include "calculation/node.h"

class Unwrapped_Structure
{
public:
	Unwrapped_Structure();
	Unwrapped_Structure(tree<Node>* calc_Tree, const tree<Node>::iterator& active_Iter, QString active_Whats_This, int num_Media, int max_Depth);

	int num_Threads;
	int num_Media;
	int num_Layers;
	int num_Boundaries;
	int max_Depth;
	tree<Node>* calc_Tree;

	vector<complex<double>> epsilon;								//	[media]
	vector<double> epsilon_RE;										//	[media]
	vector<double> epsilon_IM;										//	[media]
	vector<double> epsilon_NORM;									//	[media]

	// if epsilon is dependent on variable
	vector<vector<complex<double>>> epsilon_Dependent;				//	[wavelength][media]
	vector<vector<double>> epsilon_Dependent_RE;					//	[wavelength][media]
	vector<vector<double>> epsilon_Dependent_IM;					//	[wavelength][media]
	vector<vector<double>> epsilon_Dependent_NORM;					//	[wavelength][media]

	vector<double> sigma;											//	[boundary]
	vector<vector<Interlayer>> boundary_Interlayer_Composition;		//  [boundary][function]
	vector<double> thickness;										//	[layer]

//	int fill_Epsilon_Angular_Max_Depth_2  (const tree<Node>::iterator& parent, int media_Index = 0);
//	int fill_Epsilon_Spectra_Max_Depth_2  (const tree<Node>::iterator& parent, int media_Index = 0);
//	int fill_Sigma_Max_Depth_2    (const tree<Node>::iterator& parent, int boundary_Index = 0);
//	int fill_Thickness_Max_Depth_2(const tree<Node>::iterator& parent, int layer_Index = 0);

	int fill_Epsilon		  (const tree<Node>::iterator& parent, int media_Index = 0);
	int fill_Epsilon_Dependent(const tree<Node>::iterator& parent, int num_Lambda_Points, int media_Index = 0);
	int fill_Sigma    (const tree<Node>::iterator& parent, int boundary_Index = 0);
	int fill_Thickness(const tree<Node>::iterator& parent, int layer_Index = 0);
};

#endif // UNWRAPPED_STRUCTURE_H
