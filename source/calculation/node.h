#ifndef NODE_H
#define NODE_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/independent_curve/independent_curve.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"

class Node
{
public:
	Node();
	explicit Node(QTreeWidgetItem* item);

	void calculate_Intermediate_Points(const Data& measurement, Node* above_Node, bool depth_Grading, bool inconvenient_Approximation, bool sigma_Grading, bool enable_Discretization, QString mode);

	Data struct_Data;
	bool stop_Calcuation = false;

	/// ------------------------------------------------

	vector<complex<double>> delta_Epsilon;
	vector<complex<double>> epsilon;
	void fill_Epsilon_For_Angular_Measurements(vector<double>& spectral_Points);

	/// ------------------------------------------------

	vector<complex<double>> hi;
	vector<complex<double>> Fresnel_R_s;
	vector<complex<double>> Fresnel_R_p;
	vector<complex<double>> Fresnel_T_s;
	vector<complex<double>> Fresnel_T_p;
	vector<complex<double>> exponenta;
	vector<complex<double>> exponenta_2;

	vector<complex<double>> weak_Factor_R;
	vector<complex<double>> weak_Factor_T;

	/// ------------------------------------------------

	void create_Spline_PSD_Fractal_Gauss_1D(const Data& measurement, const Imperfections_Model& imperfections_Model);
	void create_Spline_PSD_Fractal_Gauss_2D(const Data& measurement, const Imperfections_Model& imperfections_Model);
	void clear_Spline_PSD_Fractal_Gauss(const Imperfections_Model& imperfections_Model);
	gsl_spline* spline;
	gsl_interp_accel* acc;
};

#endif // NODE_H
