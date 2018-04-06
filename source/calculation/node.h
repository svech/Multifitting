#ifndef NODE_H
#define NODE_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/independent_variables/independent_variables.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"

class Node
{
public:
	Node();
	Node(QTreeWidgetItem* item);

	void calculate_Intermediate_Points(const Data& measurement, Node* above_Node, QString active_Parameter_Whats_This, bool depth_Grading, bool sigma_Grading);

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
};

#endif // NODE_H
