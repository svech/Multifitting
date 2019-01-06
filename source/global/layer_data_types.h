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
		// angle
		Parameter probe_Angle;
		QVector<double>  cos2;
		QVector<double>  angle;
		double			 cos2_Value;
		double			 angle_Value;
		Parameter angular_Resolution;
		QString	  angle_Type;
		vector<double>  instrumental_Factor_Vec;	// without saving


		// wavelength
		Parameter wavelength;
		QVector<double>    k;
		QVector<double>	   lambda;
		double			   k_Value;
		double			   lambda_Value;
		Parameter spectral_Resolution;
		Parameter polarization;
		Parameter polarization_Sensitivity;
		Parameter background;

		// geometry
		Parameter beam_Size;
		Parameter beam_Profile_Spreading;
		Parameter sample_Size;
		Parameter sample_Shift;

		void calc_Measured_cos2_k();
		void calc_Independent_cos2_k();
		void calc_Instrumental_Factor(QString active_Parameter_Whats_This);
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
		bool use_Soft_Restrictions = false;
		int threshold = 0;
		double Q_factor = 0;
	///---------------------------------------------
	///---------------------------------------------
	// Layer, Multilayer, Aperiodic
	//---------------------------------------------
		double step_Value_Change;
		int uniqueness;

	vector<Parameter*> potentially_Fitable_Parameters;
	void fill_Potentially_Fitable_Parameters_Vector();
};

struct Regular_Component		{id_Type top_Id;
								 QVector<Data> components;
								 Min_Max min_Max_Values;
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
