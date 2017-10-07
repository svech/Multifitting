#ifndef NODE_H
#define NODE_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/independent_variables.h"
#include "multilayer_approach/multilayer/target_curve.h"
#include "tree.hh"

class Node
{
public:
	Node();
	Node(QTreeWidgetItem* item);

	int calculate_Intermediate_Points(const Data& measurement, Node* above_Node, QString active_Parameter_Whats_This, QString& warning_Text, bool depth_Grading, bool sigma_Grading);

	Data struct_Data;
	bool stop_Calcuation = false;

	/// ------------------------------------------------

	vector<complex<double>> delta_Epsilon;
	vector<complex<double>> epsilon;

	/// ------------------------------------------------

	vector<double> epsilon_RE;
	vector<double> epsilon_IM;
	vector<double> epsilon_NORM;

	vector<double> hi_RE;
	vector<double> hi_IM;
	vector<double> Fresnel_s_RE;
	vector<double> Fresnel_s_IM;
	vector<double> Fresnel_p_RE;
	vector<double> Fresnel_p_IM;
	vector<double> exponenta_RE;
	vector<double> exponenta_IM;
	vector<double> weak_Factor;

	/// ------------------------------------------------

//	Data common_Data;
//	double sigma;
//	bool common_Sigma;	// not used yet
//	vector<Interlayer> boundary_Interlayer_Composition;
//	double thickness;

//	Drift thickness_Drift;
//	Drift sigma_Drift;
};

#endif // NODE_H
