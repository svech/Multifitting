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

	void decrease_Mesh_Density(Target_Curve* target_Curve, bool fit_Mode = false);
	void preliminary_Calculation();
	template <typename Type>
	void calculation_With_Sampling(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element);
	template <typename Type>
	void calculation_With_Sampling_Spectral_Single(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element, size_t num_Points, vector<double>* calculated_Curve, QString single_Vector_Theta_0, bool R_and_T = false);
	template <typename Type>
	void calculation_With_Sampling_Theta_0_Single(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element, size_t num_Points, vector<double>* calculated_Curve, double lambda_Shift, bool R_and_T = false);
	template <typename Type>
	void calculation_With_Sampling_Theta_0_Vector(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element, size_t num_Points, vector<double>* calculated_Curve, double lambda_Shift);
	void single_Calculation(bool print_And_Verbose = true);

	void wrap_Curve(const Data& measurement,
					const vector<double>& sparse_Argument,
					const vector<double>* sparse_Input_Curve,
					const vector<double>& resolution_FWHM,
					vector<double>* output_Sparse_Curve,
					QString distribution_Function,
					bool theta_0_Beam_Profile = false, bool detector_Wrap = false);
	void wrap_With_Specular		  (Calculated_Values& calculated_Values, const Data& measurement);
//	void wrap_With_Specular_Single(Calculated_Values& calculated_Values, const Data& measurement);
//	void wrap_With_Specular_Vector(Calculated_Values& calculated_Values, const Data& measurement);
	void wrap_With_Specular_2D(Calculated_Values& calculated_Values, const Data& measurement);
	void wrap_2D_Curve(const Data& measurement,
					   Calculated_Values& calculated_Values,
					   const vector<double>& resolution_FWHM,
					   QString distribution_Function,
					   QString theta_Phi,
					   bool theta_0_Beam_Profile = false);
	template <typename Type>
	void postprocessing(Data_Element<Type>& data_Element, bool fit_Mode = false);

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
	void print_PSD_1D_To_File(Data_Element<Target_Curve>& data_Element, QString struct_Name, int index);
	void print_PSD_Data(QTextStream& out, vector<double>& arg, vector<double>& val,
									 double incident_Polarization,
									 QString data_Type,
									 QString argument_Heading,
									 QString value_Heading,
									 QString at_Fixed_Heading,
									 QString instrumental_Heading,
									 QString geometry_Heading);

	void print_Calculated_To_File();
	void print_Data(QTextStream& out, vector<double>& arg,
									 Unwrapped_Reflection* unwrapped_Reflection,
									 Calc_Functions& calc_Functions,
									 double incident_Polarization,
									 QString measurement_Type,
									 QString argument_Heading,
									 QString at_Fixed_Heading,
									 QString instrumental_Heading,
									 QString geometry_Heading);
	void print_Matrix(QString function, const Calc_Functions& calc_Functions, QTextStream& out, const vector<vector<double>>& matrix, const Data& measurement, QString angular_Units, QString spectral_Units);

	void add_Fit(QString name_Modificator, int run=0);
};

#endif // MAIN_CALCULATION_MODULE_H
