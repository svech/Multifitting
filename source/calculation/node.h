#ifndef NODE_H
#define NODE_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/independent_variables.h"
#include "tree.hh"

class Node
{
public:
	Node();
	Node(QTreeWidgetItem* item);

	int calculate_Intermediate_Points(tree<Node>::iterator& active_Iter, Node* above_Node, QString active_Whats_This, QString& warning_Text);

	QString whats_This;
	QStringList whats_This_List;

	Measurement measurement;
	Ambient ambient;
	Layer layer;
	Stack_Content stack_Content;
	Substrate substrate;

	/// ------------------------------------------------
	/// if angle
	/// ------------------------------------------------

	// for any type
	Ambient common_Data;
	complex<double> delta_Epsilon_Ang;
	complex<double> epsilon_Ang;

#ifndef REAL_VALUED    // complex-valued
	vector<complex<double>> hi_Ang;
	vector<complex<double>> Fresnel_s_Ang;
	vector<complex<double>> Fresnel_p_Ang;
	vector<complex<double>> exponenta_Ang;
#else	               // real-valued
	double epsilon_Ang_RE;
	double epsilon_Ang_IM;
	double epsilon_Ang_NORM;
	vector<double> hi_Ang_RE;
	vector<double> hi_Ang_IM;
	vector<double> Fresnel_s_Ang_RE;
	vector<double> Fresnel_s_Ang_IM;
	vector<double> Fresnel_p_Ang_RE;
	vector<double> Fresnel_p_Ang_IM;
	vector<double> exponenta_Ang_RE;
	vector<double> exponenta_Ang_IM;
#endif
	vector<double> weak_Factor_Ang;

	// for layers and substrate
	double sigma;
	vector<Interlayer> boundary_Interlayer_Composition;

	// for layers
	double thickness;
};

#endif // NODE_H
