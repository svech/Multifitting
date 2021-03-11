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
		vector<double> k_Vec;		double k_Value;		// wihout saving
		vector<double> lambda_Vec;	double lambda_Value;// wihout saving

		// theta_0 angle
		Parameter beam_Theta_0_Angle;
		vector<double> beam_Theta_0_Cos2_Vec; double beam_Theta_0_Cos2_Value;	// wihout saving
		vector<double> beam_Theta_0_Cos_Vec;  double beam_Theta_0_Cos_Value;	// wihout saving
		vector<double> beam_Theta_0_Sin_Vec;  double beam_Theta_0_Sin_Value;	// wihout saving
		vector<double>beam_Theta_0_Angle_Vec; double beam_Theta_0_Angle_Value;	// wihout saving

		double beam_Theta_0_Specular_Position;

		// theta angle
		Parameter detector_Theta_Angle;
		vector<double> detector_Theta_Cos2_Vec;  double detector_Theta_Cos2_Value;	// wihout saving
		vector<double> detector_Theta_Cos_Vec;   double detector_Theta_Cos_Value;	// wihout saving
		vector<double> detector_Theta_Angle_Vec; double detector_Theta_Angle_Value; // wihout saving

		double detector_Theta_Offset;

		// phi angle
		Parameter detector_Phi_Angle;
		vector<double> detector_Phi_Cos_Vec;   double detector_Phi_Cos_Value;	// wihout saving
		vector<double> detector_Phi_Sin_Vec;   double detector_Phi_Sin_Value;	// wihout saving
		vector<double> detector_Phi_Angle_Vec; double detector_Phi_Angle_Value; // wihout saving
		size_t start_Phi_Index;
		size_t end_Phi_Number; // last index +1


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

		vector<double> theta_0_Resolution_Vec;					// without saving, arg = Beam_Grazing_Angle
		vector<double> theta_0_Resolution_Vec_Rocking_Offset;	// without saving, arg = Beam_Grazing_Angle
		vector<double> theta_0_Resolution_From_Spectral_Vec;	// without saving, arg = Beam_Grazing_Angle
		vector<double> spectral_Resolution_Vec;					// without saving, arg = Wavelength_Energy
		vector<double> spectral_Resolution_From_Theta_0_Vec;	// without saving, arg = Wavelength_Energy
		vector<double> theta_Resolution_Vec;					// without saving, arg = Detector_Polar_Angle
		vector<double> phi_Resolution_Vec;			// without saving, arg = Detector_Azimuthal_Angle
		double theta_Resolution_FWHM = 0;			// without saving, arg = Detector_Polar_Angle
		QString theta_Distribution;					// without saving, arg = Detector_Polar_Angle
		double phi_Resolution_FWHM = 0;				// without saving, arg = Detector_Azimuthal_Angle
		QString phi_Distribution;					// without saving, arg = Detector_Azimuthal_Angle

		vector<double>  footprint_Factor_Vec;		// without saving

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

		// material composition
		QList<Stoichiometry> composition;
		QString get_Composed_Material();
	///---------------------------------------------
	///---------------------------------------------
	// Layer, Substrate
	//---------------------------------------------
		bool common_Sigma_Diffuse = true;
		Parameter sigma_Diffuse;
		QVector<Interlayer> interlayer_Composition;

		Roughness_Model roughness_Model;
		Fluctuations_Model fluctuations_Model;

		double PSD_ABC_1D_Factor;			// without saving
		double PSD_ABC_2D_Factor;			// without saving
		double PSD_Real_Gauss_1D_Factor;	// without saving
		double PSD_Real_Gauss_2D_Factor;	// without saving
		double PSD_Gauss_Peak_2D_Factor;	// without saving

		double average_Layer_density() const;
	///---------------------------------------------
	///---------------------------------------------
	// Layer
	//---------------------------------------------
		int layer_Index = -1;
		bool has_Parent = false;
		Parameter thickness;

		Drift thickness_Drift;
		Drift sigma_Diffuse_Drift;
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
	vector<Parameter*> table_Showed_Parameters;
	void fill_Potentially_Fitable_Parameters_Vector(const Imperfections_Model& imperfections_Model);
	void fill_Table_Showed_Parameters_Vector(const Imperfections_Model& imperfections_Model);
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
									 min_Max_Values.sigma_Diffuse_Min = MAX_DOUBLE;
									 min_Max_Values.sigma_Diffuse_Max = 0;

									 for(Data& child : components)
									 {
										 min_Max_Values.thickness_Min = min(min_Max_Values.thickness_Min, child.thickness.value);
										 min_Max_Values.thickness_Max = max(min_Max_Values.thickness_Max, child.thickness.value);
										 min_Max_Values.sigma_Diffuse_Min = min(min_Max_Values.sigma_Diffuse_Min, child.sigma_Diffuse.value);
										 min_Max_Values.sigma_Diffuse_Max = max(min_Max_Values.sigma_Diffuse_Max, child.sigma_Diffuse.value);
									 }
								 }
								};

QDataStream& operator <<( QDataStream& stream, const Regular_Component& regular_Component );
QDataStream& operator >>( QDataStream& stream,		 Regular_Component& regular_Component );


QDataStream& operator <<( QDataStream& stream, const Data& data );
QDataStream& operator >>( QDataStream& stream,		 Data& data );

Q_DECLARE_METATYPE( Data )

#endif // LAYER_DATA_TYPES_H
