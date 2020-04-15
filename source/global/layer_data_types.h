#ifndef LAYER_DATA_TYPES_H
#define LAYER_DATA_TYPES_H

#include "global/settings.h"

struct Regular_Component;

class Data
{
public:
	explicit Data(QString item_Type_Passed = "empty");

	void reset_All_IDs();
	QString	parent_Item_Type;
	QString	item_Type;
	id_Type id = 0;
	bool item_Enabled = true;

	///---------------------------------------------
	// Measurement
	//---------------------------------------------

		QString measurement_Type;
		QString argument_Type;

		// wavelength
		Parameter wavelength;
		QVector<double> k_Vec;		double k_Value;
		QVector<double>	lambda_Vec;	double lambda_Value;

		// theta_0 angle
		Parameter beam_Theta_0_Angle;
		QVector<double> beam_Theta_0_Cos2_Vec;  double beam_Theta_0_Cos2_Value;
		QVector<double>	beam_Theta_0_Angle_Vec; double beam_Theta_0_Angle_Value;

		// theta angle
		Parameter detector_Theta_Angle;
		QVector<double> detector_Theta_Cos2_Vec;  double detector_Theta_Cos2_Value;
		QVector<double>	detector_Theta_Angle_Vec; double detector_Theta_Angle_Value;

		double detector_Theta_Offset;

		// phi angle
		Parameter detector_Phi_Angle;
		QVector<double> detector_Phi_Cos2_Vec;  double detector_Phi_Cos2_Value;
		QVector<double>	detector_Phi_Angle_Vec; double detector_Phi_Angle_Value;

		// deam distribution
		Distribution spectral_Distribution;		
		Distribution beam_Theta_0_Distribution;
		Distribution beam_Phi_0_Distribution;		

		// detector
		Detector_1D	detector_1D;
		Detector_2D	detector_2D;

		// footprint
		Beam_Geometry beam_Geometry;
		Sample_Geometry sample_Geometry;

		// other
		double polarization;
		double background;

		// TODO
		vector<double>  angular_Resolution_Mixed;	// without saving
		vector<double>  spectral_Resolution_Mixed;	// without saving
		vector<double>  footprint_Factor_Vec;		// without saving

		void calc_Measured_cos2_k();
		void calc_Independent_cos2_k();

		void calc_Instrumental_Factor();
		void calc_Mixed_Resolution();
	///---------------------------------------------
	///---------------------------------------------
	// Ambient, Layer, Substrate
	//---------------------------------------------
		bool composed_Material;

		QString material;
		QString approved_Material;
		Parameter relative_Density;
		Parameter absolute_Density;

		// tabular material
		QString separate_Optical_Constants = TRIL_NOT_DEFINED;
		Parameter permittivity;
		Parameter absorption;

		// material composition
		QList<Stoichiometry> composition;
		QString get_Composed_Material();
	///---------------------------------------------
	///---------------------------------------------
	// Layer, Substrate
	//---------------------------------------------
		bool use_PSD = false;
		bool common_Sigma = true;
		Parameter sigma;
		QVector<Interlayer> interlayer_Composition;
	///---------------------------------------------
	///---------------------------------------------
	// Layer
	//---------------------------------------------
		int layer_Index = -1;
		bool has_Parent = false;
		Parameter thickness;

		Drift thickness_Drift;
		Drift sigma_Drift;
	///---------------------------------------------
	///---------------------------------------------
	// Multilayer, Aperiodic
	//---------------------------------------------
		int first_Layer_Index = -1;
		int last_Layer_Index = -1;

		Int_Independent num_Repetition;
		Parameter period;
		Parameter gamma;

		// regular aperiodic
		QList<Regular_Component> regular_Components;
	///---------------------------------------------
	///---------------------------------------------
	// Layer, Multilayer, Aperiodic
	//---------------------------------------------
		double step_Value_Change;
		int uniqueness;

	vector<Parameter*> potentially_Fitable_Parameters;
	void fill_Potentially_Fitable_Parameters_Vector();
	void prepare_Layer_For_Regular_Component();
	void make_Free(Parameter& parameter);
};

struct Regular_Component		{id_Type top_Id;
								 QVector<Data> components;
															  Min_Max min_Max_Values;

								 bool is_Common_Thickness = false;
								 bool is_Common_Sigma = false;

								 bool use_Soft_Restrictions = false;
								 int threshold = 0;
								 int power = 2;
								 double Q_factor = 0;

								 double get_Mean_Thickness()
								 {
									 double mean = 0;
									 for(int n=0; n<components.size(); n++)
									 {
										 mean += components[n].thickness.value;
									 }
									 mean = mean/components.size();
									 return mean;
								 }

								 void find_Min_Max_Values()
								 {
									 min_Max_Values.thickness_Min = MAX_DOUBLE;
									 min_Max_Values.thickness_Max = 0;
									 min_Max_Values.sigma_Min = MAX_DOUBLE;
									 min_Max_Values.sigma_Max = 0;

									 for(Data& child : components)
									 {
										 min_Max_Values.thickness_Min = min(min_Max_Values.thickness_Min, child.thickness.value);
										 min_Max_Values.thickness_Max = max(min_Max_Values.thickness_Max, child.thickness.value);
										 min_Max_Values.sigma_Min = min(min_Max_Values.sigma_Min, child.sigma.value);
										 min_Max_Values.sigma_Max = max(min_Max_Values.sigma_Max, child.sigma.value);
									 }
								 }
								};

QDataStream& operator <<( QDataStream& stream, const Regular_Component& regular_Component );
QDataStream& operator >>( QDataStream& stream,		 Regular_Component& regular_Component );


QDataStream& operator <<( QDataStream& stream, const Data& data );
QDataStream& operator >>( QDataStream& stream,		 Data& data );

Q_DECLARE_METATYPE( Data )

#endif // LAYER_DATA_TYPES_H
