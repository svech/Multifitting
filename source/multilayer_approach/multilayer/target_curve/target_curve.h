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
	double subinterval_Left;
	double subinterval_Right;
	// 2D only
	double subinterval_Top;
	double subinterval_Bottom;

	// argument shift & factor
	double horizontal_Arg_Shift;
	double horizontal_Arg_Factor;
	// 2D only
	double vertical_Arg_Shift;
	double vertical_Arg_Factor;

	// value shift & factor
	double val_Shift;
	Parameter val_Factor;

	// intensity
	bool divide_On_Beam_Intensity;
	double beam_Intensity_Initial;
	// 1D only
	bool use_Final_Intensity;
	double beam_Intensity_Final;

	// units
	QString angular_Units;
	QString spectral_Units;

	// type
	QString value_Type;

	/// data without saving! reimport from lines_List!
	// 1D
	QVector<double> argument;		  // without saving
	QVector<double> shifted_Argument; // without saving
	QVector<double> values;			  // without saving
	QVector<double> shifted_Values;   // without saving
	 vector<double> shifted_Values_No_Scaling_And_Offset; // without saving, for fitting scale purposes

	// 2D
	QVector<double> horizontal_Argument;						// without saving
	QVector<double> horizontal_Argument_Shifted;				// without saving
	QVector<double> vertical_Argument;							// without saving
	QVector<double> vertical_Argument_Shifted;					// without saving
	QVector<QVector<double>> value_2D;							// without saving
	 vector<QVector<double>> value_2D_Shifted;					// without saving
	 vector<QVector<double>> value_2D_No_Scaling_And_Offset;	// without saving
};

struct Fit_Params
{
	Fit_Params();

	void create_Expressions_for_Residual();

	bool calc;
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

class Target_Curve : public QWidget
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
	void refresh_Description_Label();

	Curve curve;
	Fit_Params fit_Params;
	Data measurement;
	Calculated_Values calculated_Values;

	QString filename;	// should be empty
	QString filepath;	// should be empty

	bool loaded_And_Ready = false;
	Plot_Options plot_Options_Experimental;
	Plot_Options plot_Options_Calculated;

	QStringList lines_List;
	QString arg_Units;
	QString at_Fixed;
	QString arg_Type_For_Label;

	QString label_Text = "<no description>";
	QString index;

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
