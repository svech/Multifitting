#ifndef NODE_H
#define NODE_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/independent_variables/independent_variables.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"

class Node
{
public:
	Node();
	explicit Node(QTreeWidgetItem* item);

	void calculate_Intermediate_Points(const Data& measurement, Node* above_Node, QString active_Parameter_Whats_This, bool depth_Grading, bool sigma_Grading, bool enable_Discretization);

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

	vector<double> Fresnel_R_s_RE;
	vector<double> Fresnel_R_s_IM;
	vector<double> Fresnel_R_p_RE;
	vector<double> Fresnel_R_p_IM;

	vector<double> Fresnel_T_s_RE;
	vector<double> Fresnel_T_s_IM;
	vector<double> Fresnel_T_p_RE;
	vector<double> Fresnel_T_p_IM;

	vector<double> exponenta_RE;
	vector<double> exponenta_IM;
	vector<double> exponenta_2_RE;
	vector<double> exponenta_2_IM;
	vector<double> weak_Factor_R;
//	vector<double> weak_Factor_T;

	/// ------------------------------------------------
};

#endif // NODE_H
