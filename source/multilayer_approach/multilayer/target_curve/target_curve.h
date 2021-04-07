#ifndef TARGET_CURVE_H
#define TARGET_CURVE_H

#include "global/settings.h"

// measured/target data types
struct Value {double val_1; double val_2;}; // legacy type

struct Curve
{
	Curve();

	// mesh density : 1D only
	int mesh_Density_Factor;
	double mesh_Density_Shift;

	// subinterval
	bool use_Subinterval;
	bool outer_Area;
	double subinterval_Left;
	double subinterval_Right;
	// 2D only
	double subinterval_Top;
	double subinterval_Bottom;

	// argument shift & factor 1D only
	double horizontal_Arg_Shift;
	double horizontal_Arg_Factor;

	// value shift & factor
	double val_Shift;
	Parameter val_Factor;

	// intensity
	bool divide_On_Beam_Intensity;
	double beam_Intensity_Initial;
	double beam_Time;
	// 1D only
	bool use_Final_Intensity;
	double beam_Intensity_Final;

	// type
	QString value_Type;

	// 1D
	QVector<double> argument;
	QVector<double> values;
	vector<double>  shifted_Argument;					  // without saving
	vector<double>  shifted_Values;						  // without saving
	vector<double>  shifted_Values_No_Scaling_And_Offset; // without saving, for fitting scale purposes

	// 2D
	QVector<QVector<double>> value_2D;
	vector<vector<double>>   value_2D_Shifted;					// without saving
	vector<vector<double>>   value_2D_No_Scaling_And_Offset;	// without saving

	// for old files support
	QString argument_Type;	// without saving
	QString angular_Units;	// without saving
	QString spectral_Units;	// without saving
};

struct Fit_Params
{
	Fit_Params();

	void create_Expressions_for_Residual();

	bool calculate;
	bool fit;
	bool norm;
	bool adjust_Scale_Factor;
	bool maximize_Integral;
	double weight;
	double weight_Sqrt;
	QString fit_Function;
	double power;
	bool use_Chi2;

	double expression_Argument;							// not to store

#ifdef EXPRTK
	QVector<exprtk::expression<double>> expression_Vec;	// not to store
#else
	QVector<QString> expression_Vec;					// not to store
#endif
};

class Target_Curve : public QObject
{
	Q_OBJECT
public:
	explicit Target_Curve(QLabel* description_Label, QWidget *parent = 0);

	QLabel* description_Label;

	void import_Data_From_File(QString bare_Filename);
		void parse_Data_From_List();
			void parse_1D_Data();
			void parse_2D_Data();
		void fill_Measurement_And_Curve_With_Shifted_Data();
			void fill_Measurement_And_Curve_With_Shifted_1D_Data();
			void fill_Measurement_And_Curve_With_Shifted_2D_Data();
			void rotate_Data_From_Previous_State(QString left_Right);
	void refresh_Description_Label();

	void increase_Mesh_Density(vector<double>& dense_Vector);
	void calc_Measured_cos2_k(double angle_Shift = 0, double lambda_Shift = 0, bool decrease_Mesh = false);
	void calc_cos2_k(double angle_Shift = 0, double lambda_Shift = 0);

	Curve curve;
	Fit_Params fit_Params;
	Data measurement;
	Calculated_Values calculated_Values;
	vector<Simple_Curve> additional_Curves; // without saving

	QString filename;	// should be empty
	QString filepath;	// should be empty

	bool loaded_And_Ready = false;
	Calc_Functions calc_Functions;
	Plot_Options plot_Options_Experimental;
	Plot_Options plot_Options_Calculated;

	QStringList lines_List;
	QStringList header;

	QString label_Text = "<no description>";
	QString index;
	QString name = "";
	QString index_Name;

	// units
	QString angular_Units;
	QString spectral_Units;

	// 2D positions
	Graph_2D_Positions graph_2D_Positions;

	Target_Curve& operator =(const Target_Curve& referent_Target_Curve);
};

QDataStream& operator <<( QDataStream& stream, const Value& value );
QDataStream& operator >>( QDataStream& stream,		 Value& value );

QDataStream& operator <<( QDataStream& stream, const Curve& curve );
QDataStream& operator >>( QDataStream& stream,		 Curve& curve );

QDataStream& operator <<( QDataStream& stream, const Fit_Params& fit_Params );
QDataStream& operator >>( QDataStream& stream,		 Fit_Params& fit_Params );

QDataStream& operator <<( QDataStream& stream, const Target_Curve* target_Curve );
QDataStream& operator >>( QDataStream& stream,	     Target_Curve* target_Curve );

#endif // TARGET_CURVE_H
