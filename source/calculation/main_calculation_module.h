#ifndef MAIN_CALCULATION_MODULE_H
#define MAIN_CALCULATION_MODULE_H

#include "global/layer_data_types.h"
#include "calculation/calculation_tree.h"
#include "multilayer_approach/multilayer/multilayer.h"
#include "multilayer_approach/multilayer_approach.h"
#include "calculation/fitting/fitting.h"

class Multilayer;
class Multilayer_Approach;

class Main_Calculation_Module: public QWidget
{
	Q_OBJECT
public:
	explicit Main_Calculation_Module(QString calc_Mode);
	~Main_Calculation_Module();

	QTabWidget* multilayer_Tabs;
	QVector<Multilayer*> multilayers;
	QVector<Calculation_Tree*> calculation_Trees;
	QVector<QTreeWidget*> copy_Real_Struct_Trees;
	QString calc_Mode;

	Fitables confidentials;
	Fitables confidentials_Rejected_Thicknesses_and_Periods;
	Fitables confidentials_Rejected_Periods;

	Fitables fitables;
	Fitables fit_Rejected_Min_Max;
	Fitables fit_Rejected_Sigmas;
	Fitables fit_Rejected_Thicknesses_and_Periods;
	Fitables fit_Rejected_Periods;

	QString parametrization_Type = triangle;
	size_t number_Of_Restricted_Regular_Components = 0;

	void increase_Mesh_density(Data_Element<Target_Curve>& target);
	void decrease_Mesh_density(Data_Element<Target_Curve>& target, bool fit_Mode = false);
	void preliminary_Calculation();
	template <typename Type>
	void calculation_With_Sampling(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element);
	template <typename Type>
	void calculation_With_Sampling_1_Case(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element, size_t num_Points, vector<double>* calculated_Curve, QString spectral_Angular, bool R_and_T = false);
	void single_Calculation(bool print_And_Verbose = true);

	void wrap_Curve(const vector<double>& sparse_Argument, const vector<double>* sparse_Input_Curve, const vector<double>& resolution_FWHM, vector<double>* output_Sparse_Curve, QString distribution_Function);
	void wrap_2D_Curve(const Data& measurement, Calculated_Values& calculated_Values, const vector<double>& resolution_FWHM, QString distribution_Function, QString theta_Phi);
	template <typename Type>
	void postprocessing(Data_Element<Type>& data_Element);

	void fitting_and_Confidence();
	void save_Init_State_Trees();
	void load_Init_State_Trees();
	void renew_Item_Trees();
	bool reject();
	void calc_Tree_Iteration(const tree<Node>::iterator& parent, int tab_Index, bool fitables_Period_Gamma = false, bool confidentials_Period_Gamma = false);
	void find_Fittable_Confidence_Parameters(Data& struct_Data, const Data& parent_Data, const tree<Node>::iterator& current, int child_Index, int tab_Index, bool fitables_Period_Gamma, bool confidentials_Period_Gamma);
	void slaves_Pointer_Iteration(Parameter* master);
	void slaves_Expression_Iteration(Parameter* master);

#ifdef EXPRTK
	QMap<id_Type, int> slaves_Expression_Map;  // <parameter ID ; vector index>
	QVector<exprtk::expression<double>> expression_Vec;
	QVector<double*> argument_Vec;
#endif

	Parameter* find_Slave_Pointer_by_Id(const Parameter_Indicator& slave_Parameter_Indicator);
	void find_Slave_Pointer_Calc_Tree_Iteration(const tree<Node>::iterator& parent, const Parameter_Indicator& slave_Parameter_Indicator, Parameter* &pointer);

	double triangle_Wave(double x);
	double parametrize  (double value,                      double min, double max);
	double unparametrize(double parametrized_Shifted_Value, double min, double max);

	template <typename Type>
	void print_Reflect_To_File(Data_Element<Type>& data_Element, QString struct_Name, int index);
	void print_Calculated_To_File();
	void print_Data(QTextStream& out, vector<double>& arg,
									 Unwrapped_Reflection* unwrapped_Reflection,
									 Calc_Functions& calc_Functions,
									 double incident_Polarization
									 );
	void print_Matrix(QString function, const Calc_Functions& calc_Functions, QTextStream& out, const vector<vector<double>>& matrix, const Data& measurement);

	void add_Fit(QString name_Modificator, int run=0);
};

#endif // MAIN_CALCULATION_MODULE_H
