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

	Fitables fitables;
	Fitables rejected_Min_Max;
	Fitables rejected_Sigmas;
	Fitables rejected_Thicknesses_and_Periods;
	Fitables rejected_Periods;

	QString parametrization_Type = triangle;

	void single_Calculation(bool print = true);
	void fitting();
	void save_Init_State_Trees();
	void load_Init_State_Trees();
	void renew_Item_Trees();
	bool reject();
	void calc_Tree_Iteration(const tree<Node>::iterator& parent, bool fitables_Period_Gamma = false);
	void find_Fittable_Parameters(Data& struct_Data, const tree<Node>::iterator& parent, bool fitables_Period_Gamma);
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

	void add_Fit(QString name_Modificator, int run=0);
};

#endif // MAIN_CALCULATION_MODULE_H
