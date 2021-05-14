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

	void calculate_Intermediate_Points(const Data& measurement, Node* above_Node, bool depth_Grading, bool inconvenient_Approximation, bool sigma_Grading, bool enable_Discretization, const Imperfections_Model& imperfections_Model, QString mode);

	Data struct_Data;
	bool stop_Calcuation = false;

	/// ------------------------------------------------

	vector<complex<double>> delta_Epsilon;
	vector<complex<double>> epsilon;
	void fill_Epsilon_For_Angular_Measurements(vector<double>& spectral_Points, bool specular_Case = false);

	complex<double> delta_Epsilon_Contrast;
	void fill_Epsilon_Contrast_For_Density_Fluctuations(vector<double>& spectral_Points);

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

	vector<double> specular_Debye_Waller_Weak_Factor_R;
	double ABC_Combined_1D_Effective_Sigma_2(const Imperfections_Model& imperfections_Model, double sigma, double xi, double alpha, double nu_Max);
	double ABC_Combined_Total_Sigma_2(const Imperfections_Model& imperfections_Model, double sigma, double xi, double alpha);
	double FG_Combined_1D_Effective_Sigma_2(const Imperfections_Model& imperfections_Model, double sigma, double xi, double alpha, double nu_Max);
	double FG_Combined_Total_Sigma_2(const Imperfections_Model& imperfections_Model, double sigma, double xi, double alpha);
	double integrate_FG_2D_from_0_to_nu(double nu, double sigma, double xi, double alpha);

	void calc_Combined_Delta_Sigma_2_Spline(const vector<double>& p0, gsl_spline*& spline_Delta_Sigma_2);
	void calc_Debye_Waller_Sigma(const Imperfections_Model& imperfections_Model, const Data& measurement);

	double specular_Debye_Waller_Total_Sigma = 0;
	void calc_Debye_Waller_Total_Sigma(const Imperfections_Model& imperfections_Model);

	void calc_Integral_Intensity_Near_Specular(const Imperfections_Model& imperfections_Model, Data& measurement, const Calc_Functions& calc_Functions);
	/// ------------------------------------------------

	void create_Spline_PSD_Fractal_Gauss_1D(const Imperfections_Model& imperfections_Model, const Data& measurement);
	void clear_Spline_PSD_Fractal_Gauss_1D(const Imperfections_Model& imperfections_Model);

	gsl_spline* spline_PSD_FG_1D;
	gsl_interp_accel* acc_PSD_FG_1D;

	void create_Spline_PSD_Fractal_Gauss_2D(const Imperfections_Model& imperfections_Model, const Data& measurement);
	void clear_Spline_PSD_Fractal_Gauss_2D(const Imperfections_Model& imperfections_Model);

	gsl_spline* spline_PSD_FG_2D;
	gsl_interp_accel* acc_PSD_FG_2D;

	void create_Spline_PSD_Measured(const Imperfections_Model& imperfections_Model);
	void clear_Spline_PSD_Measured (const Imperfections_Model& imperfections_Model);

	gsl_spline* spline_PSD_Meas_1D;
	gsl_interp_accel* acc_PSD_Meas_1D;
	gsl_spline* spline_PSD_Meas_2D;
	gsl_interp_accel* acc_PSD_Meas_2D;

	bool spline_PSD_Combined_1D_Condition(const Imperfections_Model& imperfections_Model);
	void create_Spline_PSD_Combined_1D(const Imperfections_Model& imperfections_Model);
	void clear_Spline_PSD_Combined_1D (const Imperfections_Model& imperfections_Model);

	gsl_spline* spline_PSD_Combined_1D;
	gsl_interp_accel* acc_PSD_Combined_1D;

	void create_Spline_PSD_Peak(const Imperfections_Model& imperfections_Model);
	void clear_Spline_PSD_Peak (const Imperfections_Model& imperfections_Model);

	gsl_spline* spline_PSD_Peak;
	gsl_interp_accel* acc_PSD_Peak;

	void create_Spline_PSD_Linear_Growth_Top_2D(const Imperfections_Model& imperfections_Model, const Data& measurement, const vector<Data*>& media_Data_Map_Vector);
	void clear_Spline_PSD_Linear_Growth_Top_2D (const Imperfections_Model& imperfections_Model, const Data& measurement);

	gsl_spline* spline_PSD_Linear_Growth_Top_2D;
	gsl_interp_accel* acc_PSD_Linear_Growth_Top_2D;
	gsl_spline* spline_PSD_Linear_Pure_Growth_Top_2D;
	gsl_interp_accel* acc_PSD_Linear_Pure_Growth_Top_2D;
	gsl_spline* spline_PSD_Linear_Inheritance_Top_2D;
	gsl_interp_accel* acc_PSD_Linear_Inheritance_Top_2D;

	void create_Spline_PSD_Linear_Growth_Top_1D(const Imperfections_Model& imperfections_Model, const Data& measurement, const vector<Data*>& media_Data_Map_Vector);
	void clear_Spline_PSD_Linear_Growth_Top_1D (const Imperfections_Model& imperfections_Model, const Data& measurement);

	gsl_spline* spline_PSD_Linear_Growth_Top_1D;
	gsl_interp_accel* acc_PSD_Linear_Growth_Top_1D;

	/// ------------------------------------------------

	double G1_Type_Outer();
	double G2_Type_Outer(double q);
	double(*G1_Type)(double);
	vector<double(*)(double, double, double, double, double, double)> G2_Type;
	vector<double(*)(double, double, double, double, double)> G2_Type_q_Zero;
	vector<double(*)(long double, long double, long double, long double sigma, long double N, long double M)> G2_Type_long;
	void create_Spline_G2_2D(const Imperfections_Model& imperfections_Model, const Data& measurement);
	void clear_Spline_G2_2D (const Imperfections_Model& imperfections_Model, const Data& measurement);
	gsl_spline* spline_G2;
	gsl_interp_accel* acc_G2;
};

#endif // NODE_H
