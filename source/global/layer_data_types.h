#ifndef LAYER_DATA_TYPES_H
#define LAYER_DATA_TYPES_H

#include "global/settings.h"

class Data
{
public:
	Data(QString item_Type_Passed = "empty");

	void reset_All_IDs();
	QString	parent_Item_Type;
	QString	item_Type;
	id_Type id = 0;
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
	///---------------------------------------------

	vector<Parameter*> potentially_Fitable_Parameters;
	void fill_Potentially_Fitable_Parameters_Vector();
};

QDataStream& operator <<( QDataStream& stream, const Data& data );
QDataStream& operator >>( QDataStream& stream,		 Data& data );

Q_DECLARE_METATYPE( Data )

#endif // LAYER_DATA_TYPES_H
