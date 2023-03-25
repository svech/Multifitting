#include "main_calculation_module.h"

#define single_Theta_0 "single_Theta_0"
#define vector_Theta_0 "vector_Theta_0"

Main_Calculation_Module::Main_Calculation_Module(QString calc_Mode):
	multilayer_Tabs			(global_Multilayer_Approach->multilayer_Tabs),
	multilayers				(multilayer_Tabs->count()),
	calculation_Trees		(multilayer_Tabs->count()),
	copy_Real_Struct_Trees	(multilayer_Tabs->count()),
	calc_Mode				(calc_Mode)
{
	// create calculation_Trees
	lambda_Out_Of_Range = false;
	for(int tab_Index=0; tab_Index<multilayer_Tabs->count(); ++tab_Index)
	{
		multilayers[tab_Index] = qobject_cast<Multilayer*>(multilayer_Tabs->widget(tab_Index));
		calculation_Trees[tab_Index] = new Calculation_Tree(multilayers[tab_Index], calc_Mode);
	}
	global_Multilayer_Approach->calculation_Started();
}

Main_Calculation_Module::~Main_Calculation_Module()
{
	global_Multilayer_Approach->calculation_Finished();

	for(Calculation_Tree* calculation_Tree : calculation_Trees)
	{
		delete calculation_Tree;
	}
//	for(QTreeWidget* copy_Real_Struct_Tree : copy_Real_Struct_Trees)
//	{
//		copy_Real_Struct_Tree->clear();
//	}
}

void Main_Calculation_Module::decrease_Mesh_Density(Target_Curve* target_Curve, bool fit_Mode)
{
	size_t size = target_Curve->curve.shifted_Argument.size();
	vector<double> sparse_Val  (size);

	if(!fit_Mode)
	{
		// decrease argument
		target_Curve->calc_Measured_cos2_k(0,0, true);

		// value vectors
		vector<double> sparse_Val_s(size);
		vector<double> sparse_Val_p(size);
		vector<double> sparse_Phi_s(size);
		vector<double> sparse_Phi_p(size);

		/// R
		// reflectance
		if( target_Curve->calc_Functions.check_Reflectance ||
			target_Curve->calc_Functions.check_Absorptance )
		{
			for(size_t i=0; i<size; i++)
			{
				sparse_Val  [i] = target_Curve->calculated_Values.R_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
				sparse_Val_s[i] = target_Curve->calculated_Values.R_s           [i*target_Curve->curve.mesh_Density_Factor];
				sparse_Val_p[i] = target_Curve->calculated_Values.R_p           [i*target_Curve->curve.mesh_Density_Factor];
				sparse_Phi_s[i] = target_Curve->calculated_Values.Phi_R_s       [i*target_Curve->curve.mesh_Density_Factor];
				sparse_Phi_p[i] = target_Curve->calculated_Values.Phi_R_p       [i*target_Curve->curve.mesh_Density_Factor];
			}
			sparse_Val  .back() = target_Curve->calculated_Values.R_Instrumental.back();
			sparse_Val_s.back() = target_Curve->calculated_Values.R_s           .back();
			sparse_Val_p.back() = target_Curve->calculated_Values.R_p           .back();
			sparse_Phi_s.back() = target_Curve->calculated_Values.Phi_R_s       .back();
			sparse_Phi_p.back() = target_Curve->calculated_Values.Phi_R_p       .back();

			// copying
			target_Curve->calculated_Values.R				= sparse_Val;
			target_Curve->calculated_Values.R_Instrumental	= sparse_Val;
			target_Curve->calculated_Values.R_s				= sparse_Val_s;
			target_Curve->calculated_Values.R_p				= sparse_Val_p;
			target_Curve->calculated_Values.Phi_R_s			= sparse_Phi_s;
			target_Curve->calculated_Values.Phi_R_p			= sparse_Phi_p;
		}
		/// T
		// transmittance
		if( target_Curve->calc_Functions.check_Transmittance ||
			target_Curve->calc_Functions.check_Absorptance )
		{
			for(size_t i=0; i<size; i++)
			{
				sparse_Val  [i] = target_Curve->calculated_Values.T_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
				sparse_Val_s[i] = target_Curve->calculated_Values.T_s           [i*target_Curve->curve.mesh_Density_Factor];
				sparse_Val_p[i] = target_Curve->calculated_Values.T_p           [i*target_Curve->curve.mesh_Density_Factor];
				sparse_Phi_s[i] = target_Curve->calculated_Values.Phi_T_s       [i*target_Curve->curve.mesh_Density_Factor];
				sparse_Phi_p[i] = target_Curve->calculated_Values.Phi_T_p       [i*target_Curve->curve.mesh_Density_Factor];
			}
			sparse_Val  .back() = target_Curve->calculated_Values.T_Instrumental.back();
			sparse_Val_s.back() = target_Curve->calculated_Values.T_s           .back();
			sparse_Val_p.back() = target_Curve->calculated_Values.T_p           .back();
			sparse_Phi_s.back() = target_Curve->calculated_Values.Phi_T_s       .back();
			sparse_Phi_p.back() = target_Curve->calculated_Values.Phi_T_p       .back();

			// copying
			target_Curve->calculated_Values.T			  = sparse_Val;
			target_Curve->calculated_Values.T_Instrumental = sparse_Val;
			target_Curve->calculated_Values.T_s			  = sparse_Val_s;
			target_Curve->calculated_Values.T_p			  = sparse_Val_p;
			target_Curve->calculated_Values.Phi_T_s		  = sparse_Phi_s;
			target_Curve->calculated_Values.Phi_T_p		  = sparse_Phi_p;
		}
		/// A
		// absorptance
		if( target_Curve->calc_Functions.check_Absorptance )
		{
			for(size_t i=0; i<size; i++)
			{
				sparse_Val  [i] = target_Curve->calculated_Values.A	 [i*target_Curve->curve.mesh_Density_Factor];
				sparse_Val_s[i] = target_Curve->calculated_Values.A_s[i*target_Curve->curve.mesh_Density_Factor];
				sparse_Val_p[i] = target_Curve->calculated_Values.A_p[i*target_Curve->curve.mesh_Density_Factor];
			}
			sparse_Val  .back() = target_Curve->calculated_Values.A  .back();
			sparse_Val_s.back() = target_Curve->calculated_Values.A_s.back();
			sparse_Val_p.back() = target_Curve->calculated_Values.A_p.back();

			// copying
			target_Curve->calculated_Values.A   = sparse_Val;
			target_Curve->calculated_Values.A_s = sparse_Val_s;
			target_Curve->calculated_Values.A_p = sparse_Val_p;
		}
		/// S
		// scattering
		if( target_Curve->calc_Functions.check_Scattering )
		{
			for(size_t i=0; i<size; i++)
			{
				sparse_Val  [i] = target_Curve->calculated_Values.S_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
				sparse_Val_s[i] = target_Curve->calculated_Values.S_s           [i*target_Curve->curve.mesh_Density_Factor];
				sparse_Val_p[i] = target_Curve->calculated_Values.S_p           [i*target_Curve->curve.mesh_Density_Factor];
			}
			sparse_Val  .back() = target_Curve->calculated_Values.S_Instrumental.back();
			sparse_Val_s.back() = target_Curve->calculated_Values.S_s           .back();
			sparse_Val_p.back() = target_Curve->calculated_Values.S_p           .back();

			// copying
			target_Curve->calculated_Values.S			   = sparse_Val;
			target_Curve->calculated_Values.S_Instrumental = sparse_Val;
			target_Curve->calculated_Values.S_s			   = sparse_Val_s;
			target_Curve->calculated_Values.S_p			   = sparse_Val_p;
		}
	} else
	// if fitting
	{
		/// R
		if( target_Curve->calc_Functions.check_Reflectance )
		{
			for(size_t i=0; i<size; i++)
			{
				sparse_Val[i] = target_Curve->calculated_Values.R_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
			}
			sparse_Val.back() = target_Curve->calculated_Values.R_Instrumental.back();

			// copying
			target_Curve->calculated_Values.R_Instrumental = sparse_Val;
		}
		/// T
		if( target_Curve->calc_Functions.check_Transmittance )
		{
			for(size_t i=0; i<size; i++)
			{
				sparse_Val[i] = target_Curve->calculated_Values.T_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
			}
			sparse_Val.back() = target_Curve->calculated_Values.T_Instrumental.back();

			// copying
			target_Curve->calculated_Values.T_Instrumental = sparse_Val;
		}
		/// S
		if( target_Curve->calc_Functions.check_Scattering )
		{
			for(size_t i=0; i<size; i++)
			{
				sparse_Val[i] = target_Curve->calculated_Values.S_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
			}
			sparse_Val.back() = target_Curve->calculated_Values.S_Instrumental.back();

			// copying
			target_Curve->calculated_Values.S_Instrumental = sparse_Val;
		}
	}
}

void Main_Calculation_Module::preliminary_Calculation()
{
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		calculation_Trees[tab_Index]->fill_Calc_Trees();

		for(Data_Element<Independent_Curve>& independent_Data_Element : calculation_Trees[tab_Index]->independent)
		{
			calculation_Trees[tab_Index]->calculate_1_Kind_Preliminary(independent_Data_Element);
		}
		for(Data_Element<Target_Curve>& target_Data_Element : calculation_Trees[tab_Index]->target)
		{
			calculation_Trees[tab_Index]->calculate_1_Kind_Preliminary(target_Data_Element);
		}
	}
}

template <typename Type>
void Main_Calculation_Module::calculation_With_Sampling(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element)
{
	Data& measurement = data_Element.the_Class->measurement;
	Calculated_Values& calculated_Values = data_Element.the_Class->calculated_Values;
	vector<double>* calculated_Curve = nullptr;

	/// Specular_Scan
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		// single calculation without any distribution
		bool state_Absorptance = data_Element.calc_Functions.check_Absorptance;
		bool state_Field = data_Element.calc_Functions.check_Field;
		bool state_Joule = data_Element.calc_Functions.check_Joule;
		if( state_Absorptance || state_Field || state_Joule )
		{
			calculation_Tree->calculate_1_Curve(data_Element);
		}
		data_Element.calc_Functions.check_Absorptance = false;
		data_Element.calc_Functions.check_Field = false;
		data_Element.calc_Functions.check_Joule = false;

		// distribution for R and T only
		if(data_Element.calc_Functions.check_Reflectance || data_Element.calc_Functions.check_Transmittance)
		{
			bool R_and_T = false;
			if(data_Element.calc_Functions.check_Reflectance && data_Element.calc_Functions.check_Transmittance)
			{
				R_and_T = true;
			} else
			{
				if( data_Element.calc_Functions.check_Reflectance)	 calculated_Curve = &calculated_Values.R;
				if( data_Element.calc_Functions.check_Transmittance) calculated_Curve = &calculated_Values.T;
			}

			if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
			{
				// spectral distribution
				calculation_With_Sampling_Spectral_Single(calculation_Tree, data_Element, measurement.beam_Theta_0_Angle_Vec.size(), calculated_Curve, "nothing", R_and_T);
			}
			if( measurement.argument_Type  == argument_Types[Wavelength_Energy] )
			{
				// theta_0 distribution
				calculation_With_Sampling_Theta_0_Single(calculation_Tree, data_Element, measurement.lambda_Vec.size(), calculated_Curve, 0, R_and_T);
			}
		}
		data_Element.calc_Functions.check_Absorptance = state_Absorptance;
		data_Element.calc_Functions.check_Field = state_Field;
		data_Element.calc_Functions.check_Joule = state_Joule;
	}
	/// Rocking_Curve
	/// Offset_Scan
	if( measurement.measurement_Type == measurement_Types[Offset_Scan]   ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		calculation_With_Sampling_Spectral_Single(calculation_Tree, data_Element, measurement.beam_Theta_0_Angle_Vec.size(), &calculated_Values.S, vector_Theta_0);
	}
	/// Detector_Scan
	if( measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
		calculation_With_Sampling_Spectral_Single(calculation_Tree, data_Element, measurement.detector_Theta_Angle_Vec.size(), &calculated_Values.S, single_Theta_0);
	}
	/// GISAS_Map
	if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		// no sampling here
		calculation_Tree->calculate_1_Curve(data_Element);
	}
}
template void Main_Calculation_Module::calculation_With_Sampling<Independent_Curve>(Calculation_Tree*, Data_Element<Independent_Curve>&);
template void Main_Calculation_Module::calculation_With_Sampling<Target_Curve>	   (Calculation_Tree*, Data_Element<Target_Curve>&);

template<typename Type>
void Main_Calculation_Module::calculation_With_Sampling_Spectral_Single(Calculation_Tree* calculation_Tree,
																		Data_Element<Type>& data_Element,
																		size_t num_Points,
																		vector<double>* calculated_Curve,
																		QString single_Vector_Theta_0,
																		bool R_and_T)
{
	Calculated_Values& calculated_Values = data_Element.the_Class->calculated_Values;
	Data& measurement = data_Element.the_Class->measurement;
	const double& lambda_Value = measurement.wavelength.value;
	Distribution& distribution = measurement.spectral_Distribution;

	// in each spectral sampling point we will first do angular sampling
	auto f_Sampling_Theta_0 = [&](double lambda_Shift = 0)
	{
		if(single_Vector_Theta_0 == single_Theta_0)
		{
			calculation_With_Sampling_Theta_0_Single(calculation_Tree, data_Element, num_Points, calculated_Curve, lambda_Shift, R_and_T);
		}
		else
		if(single_Vector_Theta_0 == vector_Theta_0)
		{
			calculation_With_Sampling_Theta_0_Vector(calculation_Tree, data_Element, num_Points, calculated_Curve, lambda_Shift);
		}
		else
		{
			data_Element.the_Class->calc_cos2_k(0, lambda_Shift);
			calculation_Tree->calculate_1_Curve(data_Element);
		}
	};

	// anyway
	QVector<double> sampled_Position_Vec(1, 0);
	QVector<double> sampled_Weight_Vec(1, 1.);

	// if sampled
	if( distribution.use_Sampling &&
		distribution.FWHM_distribution>DBL_EPSILON &&
		distribution.number_of_Samples>1)
	{
		sampled_Position_Vec.resize(distribution.number_of_Samples);
		sampled_Weight_Vec  .resize(distribution.number_of_Samples);
		Global_Variables::distribution_Sampling(distribution, sampled_Position_Vec, sampled_Weight_Vec);

		// remove out of range points
		for(int sampling_Point = 0; sampling_Point<distribution.number_of_Samples; sampling_Point++)
		{
			sampled_Position_Vec[sampling_Point] = lambda_Value*sampled_Position_Vec[sampling_Point];
			if((lambda_Value + sampled_Position_Vec[sampling_Point]) <= DBL_EPSILON)
			{
				sampled_Weight_Vec[sampling_Point] = 0;
			}
		}

		// initialization of accumulator
		vector<double> temporary_State(num_Points);
		vector<double> temporary_State_T;// special case
		if(R_and_T) temporary_State_T.resize(num_Points);

		double weight = 0;
		for(int sampling_Point = 0; sampling_Point<sampled_Position_Vec.size(); sampling_Point++)
		{
			///---------------------------------------------------------------
			f_Sampling_Theta_0(sampled_Position_Vec[sampling_Point]);
			///---------------------------------------------------------------
			weight += sampled_Weight_Vec[sampling_Point];
			if(R_and_T)
			{
				for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
				{
					temporary_State  [point_Index] += sampled_Weight_Vec[sampling_Point]*calculated_Values.R[point_Index];
					temporary_State_T[point_Index] += sampled_Weight_Vec[sampling_Point]*calculated_Values.T[point_Index];
				}
			} else
			{
				for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
				{
					temporary_State[point_Index] += sampled_Weight_Vec[sampling_Point]*(*calculated_Curve)[point_Index];
				}
			}
		}
		data_Element.the_Class->calc_cos2_k();
		if(R_and_T)
		{
			for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
			{
				temporary_State  [point_Index] /= weight;
				temporary_State_T[point_Index] /= weight;
			}
			calculated_Values.R = temporary_State;
			calculated_Values.T = temporary_State;
		} else
		{
			for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
			{
				temporary_State[point_Index] /= weight;
			}
			(*calculated_Curve) = temporary_State;
		}
	} else
	{
		///---------------------------------------------------------------
		f_Sampling_Theta_0();
		///---------------------------------------------------------------
	}
}
template void Main_Calculation_Module::calculation_With_Sampling_Spectral_Single<Independent_Curve>(Calculation_Tree*, Data_Element<Independent_Curve>&, size_t, vector<double>*, QString, bool);
template void Main_Calculation_Module::calculation_With_Sampling_Spectral_Single<Target_Curve>	   (Calculation_Tree*, Data_Element<Target_Curve>&,      size_t, vector<double>*, QString, bool);

template<typename Type>
void Main_Calculation_Module::calculation_With_Sampling_Theta_0_Single(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element, size_t num_Points, vector<double>* calculated_Curve, double lambda_Shift, bool R_and_T)
{
	Calculated_Values& calculated_Values = data_Element.the_Class->calculated_Values;
	Data& measurement = data_Element.the_Class->measurement;
	const double& theta_0_Value = measurement.beam_Theta_0_Angle.value;
	Distribution& distribution = measurement.beam_Theta_0_Distribution; // copy

	// anyway
	QVector<double> sampled_Position_Vec(1, 0);
	QVector<double> sampled_Weight_Vec(1, 1.);

	// if sampled
	if( distribution.use_Sampling &&
		distribution.FWHM_distribution>DBL_EPSILON &&
		distribution.number_of_Samples>1)
	{
		sampled_Position_Vec.resize(distribution.number_of_Samples);
		sampled_Weight_Vec  .resize(distribution.number_of_Samples);
		Global_Variables::distribution_Sampling(distribution, sampled_Position_Vec, sampled_Weight_Vec);

		// remove out of range points
		for(int sampling_Point = 0; sampling_Point<distribution.number_of_Samples; sampling_Point++)
		{
			if((theta_0_Value+sampled_Position_Vec[sampling_Point]) < DBL_EPSILON)
			{
				sampled_Weight_Vec[sampling_Point] = 0;
			}
			if((theta_0_Value+sampled_Position_Vec[sampling_Point]) >= 180)
			{
				sampled_Weight_Vec[sampling_Point] = 0;
			}
		}

		// initialization of accumulator
		vector<double> temporary_State(num_Points);
		vector<double> temporary_State_T;// special case
		if(R_and_T) temporary_State_T.resize(num_Points);

		double weight = 0;
		for(int sampling_Point = 0; sampling_Point<sampled_Position_Vec.size(); sampling_Point++)
		{
			data_Element.the_Class->calc_cos2_k(sampled_Position_Vec[sampling_Point], lambda_Shift);
			///---------------------------------------------------------------
			calculation_Tree->calculate_1_Curve(data_Element);
			///---------------------------------------------------------------
			weight += sampled_Weight_Vec[sampling_Point];
			if(R_and_T)
			{
				for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
				{
					temporary_State  [point_Index] += sampled_Weight_Vec[sampling_Point]*calculated_Values.R[point_Index];
					temporary_State_T[point_Index] += sampled_Weight_Vec[sampling_Point]*calculated_Values.T[point_Index];
				}
			} else
			{
				for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
				{
					temporary_State[point_Index] += sampled_Weight_Vec[sampling_Point]*(*calculated_Curve)[point_Index];
				}
			}
		}
		data_Element.the_Class->calc_cos2_k(0, lambda_Shift);
		if(R_and_T)
		{
			for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
			{
				temporary_State  [point_Index] /= weight;
				temporary_State_T[point_Index] /= weight;
			}
			calculated_Values.R = temporary_State;
			calculated_Values.T = temporary_State;
		} else
		{
			for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
			{
				temporary_State[point_Index] /= weight;
			}
			(*calculated_Curve) = temporary_State;
		}
	} else
	{
		///---------------------------------------------------------------
		data_Element.the_Class->calc_cos2_k(0, lambda_Shift);
		calculation_Tree->calculate_1_Curve(data_Element);
		///---------------------------------------------------------------
	}
}
template void Main_Calculation_Module::calculation_With_Sampling_Theta_0_Single<Independent_Curve>(Calculation_Tree*, Data_Element<Independent_Curve>&, size_t, vector<double>*, double, bool);
template void Main_Calculation_Module::calculation_With_Sampling_Theta_0_Single<Target_Curve>	  (Calculation_Tree*, Data_Element<Target_Curve>&,      size_t, vector<double>*, double, bool);

template<typename Type>
void Main_Calculation_Module::calculation_With_Sampling_Theta_0_Vector(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element, size_t num_Points, vector<double>* calculated_Curve, double lambda_Shift)
{
	Data& measurement = data_Element.the_Class->measurement;
	Distribution& distribution = measurement.beam_Theta_0_Distribution;

	// anyway
	QVector<double> sampled_Position_Vec(1, 0);
	QVector<double> sampled_Weight_Vec(1, 1);

	// if sampled
	if( distribution.use_Sampling &&
		distribution.FWHM_distribution>DBL_EPSILON &&
		distribution.number_of_Samples>1)
	{
		sampled_Position_Vec.resize(distribution.number_of_Samples);
		sampled_Weight_Vec  .resize(distribution.number_of_Samples);
		Global_Variables::distribution_Sampling(distribution, sampled_Position_Vec, sampled_Weight_Vec);

		// initialization of accumulator
		vector<double> temporary_State(num_Points);
		double weight = 0;
		for(int sampling_Point = 0; sampling_Point<sampled_Position_Vec.size(); sampling_Point++)
		{
			data_Element.the_Class->calc_cos2_k(sampled_Position_Vec[sampling_Point], lambda_Shift);
			///---------------------------------------------------------------
			calculation_Tree->calculate_1_Curve(data_Element);
			///---------------------------------------------------------------
			weight += sampled_Weight_Vec[sampling_Point];
			for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
			{
				temporary_State[point_Index] += sampled_Weight_Vec[sampling_Point]*(*calculated_Curve)[point_Index];
			}
		}
		data_Element.the_Class->calc_cos2_k(0, lambda_Shift);
		for(size_t point_Index = 0; point_Index<num_Points; point_Index++)
		{
			temporary_State[point_Index] /= weight;
		}
		(*calculated_Curve) = temporary_State;
	} else
	{
		///---------------------------------------------------------------
		data_Element.the_Class->calc_cos2_k(0, lambda_Shift);
		calculation_Tree->calculate_1_Curve(data_Element);
		///---------------------------------------------------------------
	}
}
template void Main_Calculation_Module::calculation_With_Sampling_Theta_0_Vector<Independent_Curve>(Calculation_Tree*, Data_Element<Independent_Curve>&, size_t, vector<double>*, double);
template void Main_Calculation_Module::calculation_With_Sampling_Theta_0_Vector<Target_Curve>	  (Calculation_Tree*, Data_Element<Target_Curve>&,      size_t, vector<double>*, double);

void Main_Calculation_Module::single_Calculation(bool print_And_Verbose)
{
	if(calc_Mode!=CALCULATION)
	{
		QMessageBox::critical(nullptr, "Main_Calculation_Module::single_Calculation", "wrong calc_Mode");
		return;
	}

	// prepare k and cos2 vectors; vector of Nodes is also filled for each calc_Tree
	preliminary_Calculation();

	// timer
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	vector<vector<double>> individual_Residuals_Vec_Vec(multilayers.size());

	/// common residual
	// look for restricted components
	number_Of_Restricted_Regular_Components = 0;
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		calc_Tree_Iteration(calculation_Trees[tab_Index]->real_Calc_Tree.begin(), tab_Index);
	}
	int residual_Shift = 0;
	double common_Residual = 0;
	Fitting_Params common_Params = {this,
									calculation_Trees,
									fitables,
									confidentials,
									calc_Mode,
									0,
									gsl_vector_calloc(Fitting::num_Residual_Points(calculation_Trees) + number_Of_Restricted_Regular_Components),
									  Fitting::num_Residual_Points_Full_With_GISAS(calculation_Trees) + number_Of_Restricted_Regular_Components,
													  Fitting::num_Residual_Points(calculation_Trees) + number_Of_Restricted_Regular_Components,
									gsl_vector_calloc(0),
									-2018,
									-2018,
									0
								   };

	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		for(Data_Element<Independent_Curve>& independent_Data_Element : calculation_Trees[tab_Index]->independent)
		{
			calculation_With_Sampling(calculation_Trees[tab_Index], independent_Data_Element);
			postprocessing(independent_Data_Element, multilayers[tab_Index]);
			if(lambda_Out_Of_Range) return;
		}
		for(Data_Element<Target_Curve>& target_Data_Element : calculation_Trees[tab_Index]->target)
		{
			// extract PSD
			if(print_1D_PSD_From_Scattering_On_Recalculation)
			{
				if( target_Data_Element.the_Class->measurement.measurement_Type == measurement_Types[Offset_Scan]   ||
					target_Data_Element.the_Class->measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
					target_Data_Element.the_Class->measurement.measurement_Type == measurement_Types[Detector_Scan] )
				{
					set_PSD_to_1 = true;
						target_Data_Element.the_Class->calc_cos2_k(0, 0);
						calculation_Trees[tab_Index]->calculate_1_Curve(target_Data_Element);
					set_PSD_to_1 = false;

					const Data& measurement = target_Data_Element.the_Class->measurement;
					Calculated_Values& calculated_Values = target_Data_Element.the_Class->calculated_Values;

					// special postprocessing
					for(size_t point_Index=0; point_Index<calculated_Values.S.size(); ++point_Index)
					{
						calculated_Values.S[point_Index] *= qDegreesToRadians(measurement.theta_Resolution_FWHM);
						calculated_Values.S_Instrumental[point_Index] = 0; // for storing specular beam
					}

					// finding specular beam curve and storing it to calculated_Values.S_Instrumental
					if(measurement.theta_Resolution_FWHM>DBL_EPSILON)
					{
						if(target_Data_Element.calc_Functions.add_Specular_Peak)
						{
							wrap_With_Specular(calculated_Values, measurement);
						}
					}

					// getting PSD
					vector<double> temp_PSD(calculated_Values.S.size());

					calculated_Values.PSD_Left_Wing_Value .clear();
					calculated_Values.PSD_Right_Wing_Value.clear();
					calculated_Values.PSD_Left_Wing_Argument .clear();
					calculated_Values.PSD_Right_Wing_Argument.clear();

					calculated_Values.PSD_Left_Wing_Value .reserve(calculated_Values.S.size());
					calculated_Values.PSD_Right_Wing_Value.reserve(calculated_Values.S.size());
					calculated_Values.PSD_Left_Wing_Argument .reserve(calculated_Values.S.size());
					calculated_Values.PSD_Right_Wing_Argument.reserve(calculated_Values.S.size());

					for(size_t point_Index=0; point_Index<calculated_Values.S.size(); ++point_Index)
					{
						if(calculated_Values.S[point_Index]>0)
						{
							temp_PSD[point_Index] = (target_Data_Element.the_Class->curve.shifted_Values[point_Index] - calculated_Values.S_Instrumental[point_Index]) / calculated_Values.S[point_Index];
						}
						double p;
						if(	measurement.measurement_Type == measurement_Types[Detector_Scan] )
						{
							p = abs(measurement.detector_Theta_Cos_Vec[point_Index]-measurement.beam_Theta_0_Cos_Value)/measurement.lambda_Value;

							if(target_Data_Element.the_Class->curve.shifted_Argument[point_Index] < measurement.beam_Theta_0_Angle_Value)
							{
								// left
								calculated_Values.PSD_Left_Wing_Argument.push_back(p);
								calculated_Values.PSD_Left_Wing_Value.push_back(temp_PSD[point_Index]);
							} else
							{
								// right
								calculated_Values.PSD_Right_Wing_Argument.push_back(p);
								calculated_Values.PSD_Right_Wing_Value.push_back(temp_PSD[point_Index]);
							}
						}
						if(	measurement.measurement_Type == measurement_Types[Rocking_Curve] )
						{
							p = abs(measurement.detector_Theta_Cos_Vec[point_Index]-measurement.beam_Theta_0_Cos_Vec[point_Index])/measurement.lambda_Value;

							if(target_Data_Element.the_Class->curve.shifted_Argument[point_Index] > measurement.beam_Theta_0_Specular_Position)
							{
								// left
								calculated_Values.PSD_Left_Wing_Argument.push_back(p);
								calculated_Values.PSD_Left_Wing_Value.push_back(temp_PSD[point_Index]);
							} else
							{
								// right
								calculated_Values.PSD_Right_Wing_Argument.push_back(p);
								calculated_Values.PSD_Right_Wing_Value.push_back(temp_PSD[point_Index]);
							}
						}
						if(	measurement.measurement_Type == measurement_Types[Offset_Scan] )
						{
							p = abs(measurement.detector_Theta_Cos_Vec[point_Index]-measurement.beam_Theta_0_Cos_Vec[point_Index])/measurement.lambda_Value;

							if(measurement.detector_Theta_Offset < 0)
							{
								// left
								calculated_Values.PSD_Left_Wing_Argument.push_back(p);
								calculated_Values.PSD_Left_Wing_Value.push_back(temp_PSD[point_Index]);
							} else
							{
								// right
								calculated_Values.PSD_Right_Wing_Argument.push_back(p);
								calculated_Values.PSD_Right_Wing_Value.push_back(temp_PSD[point_Index]);
							}
						}
					}
				}
			}

			// usual calculation
			calculation_With_Sampling(calculation_Trees[tab_Index], target_Data_Element);
			postprocessing(target_Data_Element, multilayers[tab_Index]);
			if(lambda_Out_Of_Range) return;

			if(show_residuals)
			{
				// individual residual
				{
					// enlarge
					individual_Residuals_Vec_Vec[tab_Index].resize(individual_Residuals_Vec_Vec[tab_Index].size()+1);
					double& individual_Residual = individual_Residuals_Vec_Vec[tab_Index].back();

					size_t n;
					if(target_Data_Element.the_Class->measurement.measurement_Type != measurement_Types[GISAS_Map] )	{
						n = target_Data_Element.the_Class->curve.shifted_Values.size();
					} else {
						n = target_Data_Element.the_Class->curve.value_2D_Shifted.front().size();
					}

					size_t n_Full_With_GISAS;
					if(target_Data_Element.the_Class->measurement.measurement_Type != measurement_Types[GISAS_Map] )	{
						n_Full_With_GISAS = target_Data_Element.the_Class->curve.shifted_Values.size();
					} else	{
						n_Full_With_GISAS = target_Data_Element.the_Class->curve.value_2D_Shifted.front().size()*target_Data_Element.the_Class->curve.value_2D_Shifted.size();
					}

					Fitting_Params params = {this,
											 calculation_Trees,
											 fitables,
											 confidentials,
											 calc_Mode,
											 0,
											 gsl_vector_calloc(n + number_Of_Restricted_Regular_Components),
											   n_Full_With_GISAS + number_Of_Restricted_Regular_Components,
															   n + number_Of_Restricted_Regular_Components,
											 gsl_vector_calloc(0),
											 -2018,
											 -2018,
											 0
											 };
					int temp = 0;
					Fitting::fill_Residual(&params, temp, target_Data_Element, params.f);
//					/// addition to residual from restrictions of regular aperiodics
//					size_t counter = 0;
//					for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
//					{
//						Fitting::regular_Restriction_Tree_Iteration(params.calculation_Trees[tab_Index]->real_Calc_Tree.begin(), &params, params.f, counter);
//					}
					gsl_blas_ddot(params.f, params.f, &individual_Residual);
					gsl_vector_free(params.f);
				}
				// common residual
				{
					Fitting::fill_Residual(&common_Params, residual_Shift, target_Data_Element, common_Params.f);
					/// addition to residual from restrictions of regular aperiodics
					size_t counter = 0;
					for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
					{
						Fitting::regular_Restriction_Tree_Iteration(common_Params.calculation_Trees[tab_Index]->real_Calc_Tree.begin(), &common_Params, common_Params.f, counter);
					}
					gsl_blas_ddot(common_Params.f, common_Params.f, &common_Residual);
				}
			}
		}
	}
	gsl_vector_free(common_Params.f);
	if(print_And_Verbose && calculation_time)
	{
		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		qInfo() <<  "\n-------------------------------------------------------"
					"\n  Calculation...      " << elapsed.count()/1000000. << " seconds" <<
					"\n-------------------------------------------------------\n\n";
	}
	if(print_And_Verbose && show_residuals)
	{
		bool show = false;
		for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
		{
			for(size_t target_Index = 0; target_Index<calculation_Trees[tab_Index]->target.size(); target_Index++)
			{
				show = true;
				qInfo() << "  struct" << tab_Index << "    curve" << target_Index << "    residual   " << individual_Residuals_Vec_Vec[tab_Index][target_Index] << endl;
			}
		}
		if(show) qInfo() << "  common residual ...................." << common_Residual << "\n-------------------------------------------------------\n\n\n";
	}
	// replot graphs
	Global_Variables::plot_All_Data_in_Graphs();
	Global_Variables::plot_All_Data_in_Profiles();
	Global_Variables::plot_All_Data_in_Roughness();
	Global_Variables::plot_All_Data_in_Particles();

	if(print_And_Verbose)
	{
		print_Calculated_To_File();
	}
}

void Main_Calculation_Module::wrap_Curve(const Data& measurement,
										 const vector<double>& sparse_Argument,
										 const vector<double>* sparse_Input_Curve,
										 const vector<double>& resolution_FWHM,
										 vector<double>* output_Sparse_Curve,
										 QString distribution_Function,
										 bool theta_0_Beam_Profile,
										 bool detector_Wrap)
{
	int first_Point = measurement.first_Point_of_Intensity_Integration;
	int second_Point = measurement.last_Point_of_Intensity_Integration;

	double range_Limit = 3;
	double(*distribution)(double, double);
	if(distribution_Function == distributions[Gate])	{distribution = Global_Variables::distribution_Gate;	range_Limit = 0.5; } else
	if(distribution_Function == distributions[Gaussian]){distribution = Global_Variables::distribution_Gaussian;range_Limit = 2;   } else return;

	// splining beam profile with wings for faster calculations
	gsl_spline* spline_Beam_Profile_With_Wings;
	gsl_interp_accel* acc_Beam_Profile_With_Wings;
	double max_Width;
	if(theta_0_Beam_Profile)
	{
		int profile_Points = 100;
		QVector<double> arg(profile_Points);
		QVector<double> val(profile_Points);

		max_Width = Global_Variables::beam_Profile_Bounds(measurement.beam_Geometry.size,measurement.beam_Geometry.size*measurement.beam_Geometry.smoothing);
		if(measurement.beam_Geometry.wings_Full_Width>DBL_EPSILON && measurement.beam_Geometry.wings_Intensity>DBL_EPSILON)	{
			max_Width = max(max_Width, measurement.beam_Geometry.wings_Full_Width/2);
		}
		max_Width = min(max_Width, measurement.sample_Geometry.size*measurement.beam_Theta_0_Sin_Value/2);
		max_Width = max(max_Width, DBL_EPSILON);
		double step = max_Width/(profile_Points-1);

		double x = 0;
		for(int i=0; i<profile_Points; i++)
		{
			arg[i] = x;
			val[i] = Global_Variables::beam_Profile_With_Wings(x, measurement.beam_Geometry.size, measurement.beam_Geometry.smoothing, measurement.beam_Geometry.wings_Full_Width, measurement.beam_Geometry.wings_Intensity);
			x+=step;
		}
		arg.append(max_Width+2*step);
		val.append(0);
		arg.append(MAX_DOUBLE);
		val.append(0);

		acc_Beam_Profile_With_Wings = gsl_interp_accel_alloc();
		spline_Beam_Profile_With_Wings = gsl_spline_alloc(gsl_interp_steffen, arg.size());
		gsl_spline_init(spline_Beam_Profile_With_Wings, arg.data(), val.data(), arg.size());
	}
	auto f_Beam_Profile_With_Wings = [&](double x_Distance)
	{
		return gsl_spline_eval(spline_Beam_Profile_With_Wings, x_Distance, acc_Beam_Profile_With_Wings);
	};

	auto f_Beam_Spot_Profile = [&](double point_Index)
	{
		(*output_Sparse_Curve)[point_Index] = (*sparse_Input_Curve)[point_Index];
		double weight_Accumulator = 1;
		double weight = DBL_MIN;

		/*  asymmetrical condition  */
		/*  positive direction  */
		double distance = 0;
		double x_Distance = 0;
		for(int i=point_Index+1; (x_Distance<max_Width && i<sparse_Argument.size()); i++)
		{
			distance = abs(sparse_Argument[i] - sparse_Argument[point_Index]);
			x_Distance = qDegreesToRadians(distance)*measurement.detector_1D.distance_To_Sample;
			weight = f_Beam_Profile_With_Wings(x_Distance);

			(*output_Sparse_Curve)[point_Index] += weight*(*sparse_Input_Curve)[i];
			weight_Accumulator += weight;
		}
		/*  negative direction  */
		distance = 0;
		x_Distance = 0;
		for(int i=point_Index-1; (x_Distance<max_Width && i>=0); i--)
		{
			distance = abs(sparse_Argument[i] - sparse_Argument[point_Index]);
			x_Distance = qDegreesToRadians(distance)*measurement.detector_1D.distance_To_Sample;
			weight = f_Beam_Profile_With_Wings(x_Distance);

			(*output_Sparse_Curve)[point_Index] += weight*(*sparse_Input_Curve)[i];
			weight_Accumulator += weight;
		}
		(*output_Sparse_Curve)[point_Index] /= weight_Accumulator;
	};
	auto f = [&](double point_Index)
	{
		if(detector_Wrap && first_Point<=point_Index && point_Index<=second_Point) return;

		(*output_Sparse_Curve)[point_Index] = (*sparse_Input_Curve)[point_Index];
		double weight_Accumulator = 1;
		double weight = DBL_MIN;

//		/* condition is symmetrical in both directions */
//		double distance = 0;
//		for(int diff_Index=1; (distance<range_Limit*resolution_FWHM[point_Index] && (point_Index+diff_Index)<sparse_Argument.size() && (point_Index-diff_Index)>=0); diff_Index++)
//		{
//			/* positive */
//			int i = point_Index+diff_Index;
//			distance = abs(sparse_Argument[i] - sparse_Argument[point_Index]);
//			weight = distribution(resolution_FWHM[point_Index], distance);

//			(*output_Sparse_Curve)[point_Index] += weight*(*sparse_Input_Curve)[i];
//			weight_Accumulator += weight;

//			/* negative */
//			i = point_Index-diff_Index;
//			distance = abs(sparse_Argument[i] - sparse_Argument[point_Index]);
//			weight = distribution(resolution_FWHM[point_Index], distance);

//			(*output_Sparse_Curve)[point_Index] += weight*(*sparse_Input_Curve)[i];
//			weight_Accumulator += weight;
//		}

		/*  asymmetrical condition  */
		/*  positive direction  */
		double distance = 0;
		for(int i=point_Index+1; (distance<range_Limit*resolution_FWHM[point_Index] && i<sparse_Argument.size()); i++)
		{
			distance = abs(sparse_Argument[i] - sparse_Argument[point_Index]);
			weight = distribution(resolution_FWHM[point_Index], distance);

			(*output_Sparse_Curve)[point_Index] += weight*(*sparse_Input_Curve)[i];
			weight_Accumulator += weight;
		}
		/*  negative direction  */
		distance = 0;
		for(int i=point_Index-1; (distance<range_Limit*resolution_FWHM[point_Index] && i>=0); i--)
		{
			distance = abs(sparse_Argument[i] - sparse_Argument[point_Index]);
			weight = distribution(resolution_FWHM[point_Index], distance);

			(*output_Sparse_Curve)[point_Index] += weight*(*sparse_Input_Curve)[i];
			weight_Accumulator += weight;
		}
		(*output_Sparse_Curve)[point_Index] /= weight_Accumulator;
	};

	if(sparse_Argument.size()*resolution_FWHM[0]>50) // tunable
	{
		Global_Variables::parallel_For(sparse_Argument.size(), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
		{
			Q_UNUSED(thread_Index)
			if(theta_0_Beam_Profile)
			{
				for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
				{
					f_Beam_Spot_Profile(point_Index);
				}
			} else
			{
				for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
				{
					f(point_Index);
				}
			}
		});
	} else
	{
		if(theta_0_Beam_Profile)
		{
			for(size_t point_Index=0; point_Index<sparse_Argument.size(); ++point_Index)
			{
				f_Beam_Spot_Profile(point_Index);
			}
		} else
		{
			for(size_t point_Index=0; point_Index<sparse_Argument.size(); ++point_Index)
			{
				f(point_Index);
			}
		}
	}
	if(theta_0_Beam_Profile)
	{
		gsl_spline_free(spline_Beam_Profile_With_Wings);
		gsl_interp_accel_free(acc_Beam_Profile_With_Wings);
	}
}

void Main_Calculation_Module::wrap_With_Specular(Calculated_Values& calculated_Values, const Data& measurement)
{
	vector<double> theta_0_Vec, theta_Vec, beam_Angular_Divergence_FWHM_Vec, R, max_Beam_Size_Angular_Vec;
	if(measurement.measurement_Type == measurement_Types[Detector_Scan])	{
		theta_Vec = measurement.detector_Theta_Angle_Vec;
		theta_0_Vec.resize(theta_Vec.size(), measurement.beam_Theta_0_Angle_Value);
		beam_Angular_Divergence_FWHM_Vec.resize(theta_Vec.size(),measurement.beam_Theta_0_Divergence_With_Curvature/*measurement.beam_Theta_0_Distribution.FWHM_distribution*/);
		R.resize(theta_Vec.size(), calculated_Values.R.front());
		max_Beam_Size_Angular_Vec.resize(theta_Vec.size(), qRadiansToDegrees(measurement.sample_Geometry.size*measurement.beam_Theta_0_Sin_Value/measurement.detector_1D.distance_To_Sample));
	} else
	if(measurement.measurement_Type == measurement_Types[Rocking_Curve])	{
		theta_0_Vec = measurement.beam_Theta_0_Angle_Vec;
		theta_Vec = measurement.detector_Theta_Angle_Vec;
//		theta_Vec.resize(theta_0_Vec.size());
//		for(int i=0; i<theta_0_Vec.size(); i++)
//		{theta_Vec[i] = 2*measurement.beam_Theta_0_Specular_Position - theta_0_Vec[i];}
		beam_Angular_Divergence_FWHM_Vec = measurement.theta_0_Resolution_Vec;
		R = calculated_Values.R;
		max_Beam_Size_Angular_Vec.resize(theta_Vec.size());
		for(int i=0; i<max_Beam_Size_Angular_Vec.size(); i++)
		{max_Beam_Size_Angular_Vec[i] = qRadiansToDegrees(measurement.sample_Geometry.size*measurement.beam_Theta_0_Sin_Vec[i]/measurement.detector_1D.distance_To_Sample);}
	} else
	if(measurement.measurement_Type == measurement_Types[Offset_Scan])		{
		theta_0_Vec = measurement.beam_Theta_0_Angle_Vec;
		theta_Vec = measurement.detector_Theta_Angle_Vec;
//		theta_Vec.resize(theta_0_Vec.size());
//		for(int i=0; i<theta_0_Vec.size(); i++)
//		{theta_Vec[i] = theta_0_Vec[i] + measurement.detector_Theta_Offset;}
		beam_Angular_Divergence_FWHM_Vec = measurement.theta_0_Resolution_Vec;
		R = calculated_Values.R;
		max_Beam_Size_Angular_Vec.resize(theta_0_Vec.size());
		for(int i=0; i<max_Beam_Size_Angular_Vec.size(); i++)
		{max_Beam_Size_Angular_Vec[i] = qRadiansToDegrees(measurement.sample_Geometry.size*measurement.beam_Theta_0_Sin_Vec[i]/measurement.detector_1D.distance_To_Sample);}
	} else return;

	// detector
	double detector_Angular_Width_FWHM		= measurement.theta_Resolution_FWHM;
	QString detector_Sensitivity_Function	= measurement.theta_Distribution;
	double detector_Range_Limit = 3;
	double(*detector_Distribution)(double, double);
	if(detector_Sensitivity_Function == distributions[Gate])	{detector_Distribution = Global_Variables::distribution_Gate;	 detector_Range_Limit = 0.5;  } else
	if(detector_Sensitivity_Function == distributions[Gaussian]){detector_Distribution = Global_Variables::distribution_Gaussian;detector_Range_Limit = 3;    } else return;

	// beam angular divergence
	QString beam_Angular_Divergence_Function= measurement.beam_Theta_0_Distribution.distribution_Function;
	double beam_Angular_Divergence_Range_Limit = 2;
	vector<double> beam_Angular_Divergence_Norm_Factor_Vec(beam_Angular_Divergence_FWHM_Vec.size(),1);
	double(*beam_Angular_Distribution)(double, double);
	if(beam_Angular_Divergence_Function == distributions[Gate])		{beam_Angular_Distribution = Global_Variables::distribution_Gate;	  for(size_t i=0;i<beam_Angular_Divergence_Norm_Factor_Vec.size();i++) {beam_Angular_Divergence_Norm_Factor_Vec[i] =                  1./beam_Angular_Divergence_FWHM_Vec[i];}beam_Angular_Divergence_Range_Limit = 0.5;  } else
	if(beam_Angular_Divergence_Function == distributions[Gaussian])	{beam_Angular_Distribution = Global_Variables::distribution_Gaussian; for(size_t i=0;i<beam_Angular_Divergence_Norm_Factor_Vec.size();i++) {beam_Angular_Divergence_Norm_Factor_Vec[i] = sqrt(log(16.)/M_PI)/beam_Angular_Divergence_FWHM_Vec[i];}beam_Angular_Divergence_Range_Limit = 2;    } else return;

	// beam spot
	double beam_Spot_FWHM_Angular = qRadiansToDegrees(measurement.beam_Geometry.size			/measurement.detector_1D.distance_To_Sample);
	double beam_Spot_Wing_Angular = qRadiansToDegrees(measurement.beam_Geometry.wings_Full_Width/measurement.detector_1D.distance_To_Sample);
	double beam_Spot_Wings_Intensity = measurement.beam_Geometry.wings_Intensity;
	double beam_Spot_Smoothing = measurement.beam_Geometry.smoothing;
	double spot_Range_Main_Limit = 3;
	double spot_Range_Wing_Limit = 0.5;

	double arg_Limit = max(10*beam_Spot_FWHM_Angular,10*beam_Spot_Wing_Angular);
	double spot_Norm_Factor = Global_Variables::beam_Profile_With_Wings_Integral( arg_Limit, beam_Spot_FWHM_Angular, beam_Spot_Smoothing, beam_Spot_Wing_Angular, beam_Spot_Wings_Intensity)-
							  Global_Variables::beam_Profile_With_Wings_Integral(-arg_Limit, beam_Spot_FWHM_Angular, beam_Spot_Smoothing, beam_Spot_Wing_Angular, beam_Spot_Wings_Intensity);
	double spot_Norm_Factor_Gate = Global_Variables::beam_Profile_With_Wings_Integral( arg_Limit, beam_Spot_FWHM_Angular, 0, beam_Spot_Wing_Angular, beam_Spot_Wings_Intensity)-
								   Global_Variables::beam_Profile_With_Wings_Integral(-arg_Limit, beam_Spot_FWHM_Angular, 0, beam_Spot_Wing_Angular, beam_Spot_Wings_Intensity);


	/// detector only (no spot, no divergence)
	auto f = [&](double point_Index)
	{
		const double& theta   = theta_Vec  [point_Index];
		const double& theta_0 = theta_0_Vec[point_Index];
		return detector_Distribution(detector_Angular_Width_FWHM, theta-theta_0);
	};
	/// detector and angular divergence (no spot)
	auto f_D = [&](double point_Index)
	{
		const double& theta   = theta_Vec  [point_Index];
		const double& theta_0 = theta_0_Vec[point_Index];
		double distance_t_t0 = theta - theta_0;
		const double& beam_Angular_Divergence_FWHM = beam_Angular_Divergence_FWHM_Vec[point_Index];
		const double& beam_Angular_Divergence_Norm_Factor = beam_Angular_Divergence_Norm_Factor_Vec[point_Index];

		double left_Bound_a  = max(theta - detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 - beam_Angular_Divergence_Range_Limit*beam_Angular_Divergence_FWHM);
		double right_Bound_a = min(theta + detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 + beam_Angular_Divergence_Range_Limit*beam_Angular_Divergence_FWHM);

		if(left_Bound_a>=right_Bound_a) return 0.;

		// specific cases
		double integral;

		if(beam_Angular_Divergence_Function == distributions[Gaussian] && detector_Sensitivity_Function == distributions[Gaussian])
		{
			integral = Global_Variables::gauss_Gauss_Integral(detector_Angular_Width_FWHM, beam_Angular_Divergence_FWHM, distance_t_t0);
		} else
		if(beam_Angular_Divergence_Function == distributions[Gaussian] && detector_Sensitivity_Function == distributions[Gate])
		{
			integral = Global_Variables::gate_Gauss_Integral(detector_Angular_Width_FWHM, beam_Angular_Divergence_FWHM, distance_t_t0);
		} else
		if(beam_Angular_Divergence_Function == distributions[Gate] && detector_Sensitivity_Function == distributions[Gaussian])
		{
			integral = Global_Variables::gate_Gauss_Integral(beam_Angular_Divergence_FWHM, detector_Angular_Width_FWHM, distance_t_t0);
		} else
		if(beam_Angular_Divergence_Function == distributions[Gate] && detector_Sensitivity_Function == distributions[Gate])
		{
			integral = Global_Variables::gate_Gate_Integral(detector_Angular_Width_FWHM, beam_Angular_Divergence_FWHM, theta, theta_0);
		}
		return beam_Angular_Divergence_Norm_Factor * integral;
	};
	/// detector and spot (no divergence)
	auto f_S = [&](double point_Index, double my_Theta_0)
	{
		const double& theta   = theta_Vec  [point_Index];
		const double& theta_0 = my_Theta_0;
		double distance_t_t0 = theta - theta_0;
		const double& max_Beam_Size_Angular = max_Beam_Size_Angular_Vec[point_Index];

		double left_Bound_Wings  = max(theta - detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 - spot_Range_Wing_Limit*beam_Spot_Wing_Angular);
		double right_Bound_Wings = min(theta + detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 + spot_Range_Wing_Limit*beam_Spot_Wing_Angular);

		double left_Bound_i  = max(theta - detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 - spot_Range_Main_Limit*beam_Spot_FWHM_Angular);
		double right_Bound_i = min(theta + detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 + spot_Range_Main_Limit*beam_Spot_FWHM_Angular);

		// specific cases
		double integral = 0;

		// wings
		if(beam_Spot_Wing_Angular > DBL_EPSILON && beam_Spot_Wings_Intensity > DBL_EPSILON && right_Bound_Wings>left_Bound_Wings)
		{
			double wings_Width_Angular = min(beam_Spot_Wing_Angular, max_Beam_Size_Angular);

			if(detector_Sensitivity_Function == distributions[Gate])
			{
				integral += beam_Spot_Wings_Intensity/spot_Norm_Factor*Global_Variables::gate_Gate_Integral(detector_Angular_Width_FWHM, wings_Width_Angular, theta, theta_0);
			} else
			if(detector_Sensitivity_Function == distributions[Gaussian])
			{
				integral += beam_Spot_Wings_Intensity/spot_Norm_Factor*Global_Variables::gate_Gauss_Integral(wings_Width_Angular, detector_Angular_Width_FWHM, distance_t_t0);
			}
		}
		if(beam_Spot_Wing_Angular <= DBL_EPSILON || beam_Spot_Wings_Intensity <= DBL_EPSILON)
		{
			beam_Spot_Wings_Intensity = 0; // for using in beam weight
		}

		// main beam
		if(right_Bound_i>left_Bound_i)
		{
			if(beam_Spot_FWHM_Angular > DBL_EPSILON)
			{
				if(detector_Sensitivity_Function == distributions[Gate])
				{
					integral += (1.-beam_Spot_Wings_Intensity)/spot_Norm_Factor*(
							Global_Variables::beam_Profile_Integral_Bounded(distance_t_t0+detector_Angular_Width_FWHM/2, beam_Spot_FWHM_Angular, beam_Spot_Smoothing, -max_Beam_Size_Angular/2, max_Beam_Size_Angular/2)
						   -Global_Variables::beam_Profile_Integral_Bounded(distance_t_t0-detector_Angular_Width_FWHM/2, beam_Spot_FWHM_Angular, beam_Spot_Smoothing, -max_Beam_Size_Angular/2, max_Beam_Size_Angular/2)
								);
				} else
				if(detector_Sensitivity_Function == distributions[Gaussian])
				{
//					double weight_Gate = 1-beam_Spot_Smoothing;
//					double weight_Gauss =  beam_Spot_Smoothing;

					/// use trimmed gate beam profile for Gauss detector sensitivity
					double beam_Width_Angular = min(beam_Spot_FWHM_Angular, max_Beam_Size_Angular);
					integral += /*weight_Gate **/(1.-beam_Spot_Wings_Intensity)/spot_Norm_Factor_Gate*Global_Variables::gate_Gauss_Integral (beam_Width_Angular, detector_Angular_Width_FWHM, distance_t_t0);
//					integral += /*weight_Gauss**/(1.-beam_Spot_Wings_Intensity)/spot_Norm_Factor*Global_Variables::gauss_Gauss_Integral(beam_Spot_FWHM_Angular, detector_Angular_Width_FWHM, distance_t_t0);
				}
			}
		}
		return integral;
	};
	/// detector and spot and divergence
	auto f_SD = [&](double point_Index)
	{
		const double& theta_0 = theta_0_Vec[point_Index];
		const double& beam_Angular_Divergence_FWHM = beam_Angular_Divergence_FWHM_Vec[point_Index];
		const double& beam_Angular_Divergence_Norm_Factor = beam_Angular_Divergence_Norm_Factor_Vec[point_Index];
		auto f_Divergence_and_Spot_and_Detector = [&](double theta_0_i)
		{
			return beam_Angular_Divergence_Norm_Factor * beam_Angular_Distribution(beam_Angular_Divergence_FWHM, theta_0_i-theta_0) * f_S(point_Index, theta_0_i);
		};

		double left_Bound_a  = theta_0 - beam_Angular_Divergence_Range_Limit*beam_Angular_Divergence_FWHM;
		double right_Bound_a = theta_0 + beam_Angular_Divergence_Range_Limit*beam_Angular_Divergence_FWHM;
		if(left_Bound_a>=right_Bound_a) return 0.;

		double integral = 0;
		vector<double> interpoints(4+1);
		interpoints[0] = left_Bound_a;
		for(int i=1; i<interpoints.size(); i++)
		{
			interpoints[i] = left_Bound_a + i*(right_Bound_a-left_Bound_a)/(interpoints.size()-1);
			integral += gauss_kronrod<double,31>::integrate(f_Divergence_and_Spot_and_Detector, interpoints[i-1], interpoints[i], 3, 1e-7);
		}
		return integral;
	};

	double nominal_Divergence = max(measurement.beam_Theta_0_Distribution.FWHM_distribution, measurement.beam_Theta_0_Divergence_With_Curvature);
	Global_Variables::parallel_For(theta_Vec.size(), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		Q_UNUSED(thread_Index)
		if(nominal_Divergence>DBL_EPSILON && (beam_Spot_FWHM_Angular>DBL_EPSILON || (beam_Spot_Wing_Angular>DBL_EPSILON && beam_Spot_Wings_Intensity>DBL_EPSILON)))
		{
			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
			{
				double integral = f_SD(point_Index);
				calculated_Values.S_Instrumental[point_Index] += integral*R[point_Index]/measurement.footprint_Factor_Vec[point_Index];
			}
		}
		if(nominal_Divergence<=DBL_EPSILON && (beam_Spot_FWHM_Angular>DBL_EPSILON || (beam_Spot_Wing_Angular>DBL_EPSILON && beam_Spot_Wings_Intensity>DBL_EPSILON)))
		{
			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
			{
				double integral = f_S(point_Index, theta_0_Vec[point_Index]);
				calculated_Values.S_Instrumental[point_Index] += integral*R[point_Index]/measurement.footprint_Factor_Vec[point_Index];
			}
		}
		if(nominal_Divergence>DBL_EPSILON && (beam_Spot_FWHM_Angular<=DBL_EPSILON && (beam_Spot_Wing_Angular<=DBL_EPSILON || beam_Spot_Wings_Intensity<=DBL_EPSILON)))
		{
			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
			{
				double integral = f_D(point_Index);
				calculated_Values.S_Instrumental[point_Index] += integral*R[point_Index]/measurement.footprint_Factor_Vec[point_Index];
			}
		}
		if(nominal_Divergence<=DBL_EPSILON && (beam_Spot_FWHM_Angular<=DBL_EPSILON && (beam_Spot_Wing_Angular<=DBL_EPSILON || beam_Spot_Wings_Intensity<=DBL_EPSILON)))
		{
			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
			{
				double integral = f(point_Index);
				calculated_Values.S_Instrumental[point_Index] += integral*R[point_Index]/measurement.footprint_Factor_Vec[point_Index];
			}
		}
	});
}
// deprecated
//void Main_Calculation_Module::wrap_With_Specular_Single(Calculated_Values& calculated_Values, const Data& measurement)
//{
//	// detector
//	const vector<double>& detector_Position	= measurement.detector_Theta_Angle_Vec;
//	double detector_Angular_Width_FWHM		= measurement.theta_Resolution_FWHM;
//	QString detector_Sensitivity_Function	= measurement.theta_Distribution;

//	double detector_Range_Limit = 3;
//	double(*detector_Distribution)(double, double);
//	if(detector_Sensitivity_Function == distributions[Gate])	{detector_Distribution = Global_Variables::distribution_Gate;	 detector_Range_Limit = 0.5;  } else
//	if(detector_Sensitivity_Function == distributions[Gaussian]){detector_Distribution = Global_Variables::distribution_Gaussian;detector_Range_Limit = 3;    } else return;

//	// beam angular divergence
//	double beam_Theta_0_Position			= measurement.beam_Theta_0_Angle_Value;
//	double beam_Angular_Divergence_FWHM		= measurement.beam_Theta_0_Distribution.FWHM_distribution;
//	QString beam_Angular_Divergence_Function= measurement.beam_Theta_0_Distribution.distribution_Function;

//	double beam_Angular_Divergence_Range_Limit = 2;
//	double beam_Angular_Divergence_Norm_Factor = 1;
//	double(*beam_Angular_Distribution)(double, double);
//	if(beam_Angular_Divergence_Function == distributions[Gate])		{beam_Angular_Distribution = Global_Variables::distribution_Gate;	  beam_Angular_Divergence_Norm_Factor = 1./beam_Angular_Divergence_FWHM;					beam_Angular_Divergence_Range_Limit = 0.5;  } else
//	if(beam_Angular_Divergence_Function == distributions[Gaussian])	{beam_Angular_Distribution = Global_Variables::distribution_Gaussian; beam_Angular_Divergence_Norm_Factor = sqrt(log(16.)/M_PI)/beam_Angular_Divergence_FWHM;	beam_Angular_Divergence_Range_Limit = 2;    } else return;

//	// beam spot
//	double beam_Spot_FWHM_Angular = qRadiansToDegrees(measurement.beam_Geometry.size			/measurement.detector_1D.distance_To_Sample);
//	double beam_Spot_Wing_Angular = qRadiansToDegrees(measurement.beam_Geometry.wings_Full_Width/measurement.detector_1D.distance_To_Sample);
//	double max_Beam_Size_Angular  = qRadiansToDegrees(measurement.sample_Geometry.size*measurement.beam_Theta_0_Sin_Value/measurement.detector_1D.distance_To_Sample);

//	double beam_Spot_Wings_Intensity = measurement.beam_Geometry.wings_Intensity;
//	double beam_Spot_Smoothing = measurement.beam_Geometry.smoothing;
//	double spot_Range_Main_Limit = 3;
//	double spot_Range_Wing_Limit = 0.5;

//	double arg_Limit = max(10*beam_Spot_FWHM_Angular,10*beam_Spot_Wing_Angular);
//	double spot_Norm_Factor = Global_Variables::beam_Profile_With_Wings_Integral( arg_Limit, beam_Spot_FWHM_Angular, beam_Spot_Smoothing, beam_Spot_Wing_Angular, beam_Spot_Wings_Intensity)-
//							  Global_Variables::beam_Profile_With_Wings_Integral(-arg_Limit, beam_Spot_FWHM_Angular, beam_Spot_Smoothing, beam_Spot_Wing_Angular, beam_Spot_Wings_Intensity);
//	double spot_Norm_Factor_Gate = Global_Variables::beam_Profile_With_Wings_Integral( arg_Limit, beam_Spot_FWHM_Angular, 0, beam_Spot_Wing_Angular, beam_Spot_Wings_Intensity)-
//								   Global_Variables::beam_Profile_With_Wings_Integral(-arg_Limit, beam_Spot_FWHM_Angular, 0, beam_Spot_Wing_Angular, beam_Spot_Wings_Intensity);


//	/// detector only (no spot, no divergence)
//	auto f = [&](double point_Index)
//	{
//		double theta = detector_Position[point_Index];
//		double theta_0 = beam_Theta_0_Position;
//		return detector_Distribution(detector_Angular_Width_FWHM, theta-theta_0);
//	};
//	/// detector and angular divergence (no spot)
//	auto f_D = [&](double point_Index)
//	{
//		double theta = detector_Position[point_Index];
//		double theta_0 = beam_Theta_0_Position;
//		double distance_t_t0 = theta - theta_0;

//		double left_Bound_a  = max(theta - detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 - beam_Angular_Divergence_Range_Limit*beam_Angular_Divergence_FWHM);
//		double right_Bound_a = min(theta + detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 + beam_Angular_Divergence_Range_Limit*beam_Angular_Divergence_FWHM);

//		if(left_Bound_a>=right_Bound_a) return 0.;

//		// specific cases
//		double integral;

//		if(beam_Angular_Divergence_Function == distributions[Gaussian] && detector_Sensitivity_Function == distributions[Gaussian])
//		{
//			integral = Global_Variables::gauss_Gauss_Integral(detector_Angular_Width_FWHM, beam_Angular_Divergence_FWHM, distance_t_t0);
//		} else
//		if(beam_Angular_Divergence_Function == distributions[Gaussian] && detector_Sensitivity_Function == distributions[Gate])
//		{
//			integral = Global_Variables::gate_Gauss_Integral(detector_Angular_Width_FWHM, beam_Angular_Divergence_FWHM, distance_t_t0);
//		} else
//		if(beam_Angular_Divergence_Function == distributions[Gate] && detector_Sensitivity_Function == distributions[Gaussian])
//		{
//			integral = Global_Variables::gate_Gauss_Integral(beam_Angular_Divergence_FWHM, detector_Angular_Width_FWHM, distance_t_t0);
//		} else
//		if(beam_Angular_Divergence_Function == distributions[Gate] && detector_Sensitivity_Function == distributions[Gate])
//		{
//			integral = Global_Variables::gate_Gate_Integral(detector_Angular_Width_FWHM, beam_Angular_Divergence_FWHM, theta, theta_0);
//		}
//		return beam_Angular_Divergence_Norm_Factor * integral;
//	};
//	/// detector and spot (no divergence)
//	auto f_S = [&](double point_Index, double my_Theta_0)
//	{
//		double theta = detector_Position[point_Index];
//		double theta_0 = my_Theta_0;
//		double distance_t_t0 = theta - theta_0;

//		double left_Bound_Wings  = max(theta - detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 - spot_Range_Wing_Limit*beam_Spot_Wing_Angular);
//		double right_Bound_Wings = min(theta + detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 + spot_Range_Wing_Limit*beam_Spot_Wing_Angular);

//		double left_Bound_i  = max(theta - detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 - spot_Range_Main_Limit*beam_Spot_FWHM_Angular);
//		double right_Bound_i = min(theta + detector_Range_Limit*detector_Angular_Width_FWHM, theta_0 + spot_Range_Main_Limit*beam_Spot_FWHM_Angular);

//		// specific cases
//		double integral = 0;

//		// wings
//		if(beam_Spot_Wing_Angular > DBL_EPSILON && beam_Spot_Wings_Intensity > DBL_EPSILON && right_Bound_Wings>left_Bound_Wings)
//		{
//			double wings_Width_Angular = min(beam_Spot_Wing_Angular, max_Beam_Size_Angular);

//			if(detector_Sensitivity_Function == distributions[Gate])
//			{
//				integral += beam_Spot_Wings_Intensity/spot_Norm_Factor*Global_Variables::gate_Gate_Integral(detector_Angular_Width_FWHM, wings_Width_Angular, theta, theta_0);
//			} else
//			if(detector_Sensitivity_Function == distributions[Gaussian])
//			{
//				integral += beam_Spot_Wings_Intensity/spot_Norm_Factor*Global_Variables::gate_Gauss_Integral(wings_Width_Angular, detector_Angular_Width_FWHM, distance_t_t0);
//			}
//		}
//		if(beam_Spot_Wing_Angular <= DBL_EPSILON || beam_Spot_Wings_Intensity <= DBL_EPSILON)
//		{
//			beam_Spot_Wings_Intensity = 0; // for using in beam weight
//		}

//		// main beam
//		if(right_Bound_i>left_Bound_i)
//		{
//			if(beam_Spot_FWHM_Angular > DBL_EPSILON)
//			{
//				if(detector_Sensitivity_Function == distributions[Gate])
//				{
//					integral += (1.-beam_Spot_Wings_Intensity)/spot_Norm_Factor*(
//							Global_Variables::beam_Profile_Integral_Bounded(distance_t_t0+detector_Angular_Width_FWHM/2, beam_Spot_FWHM_Angular, beam_Spot_Smoothing, -max_Beam_Size_Angular/2, max_Beam_Size_Angular/2)
//						   -Global_Variables::beam_Profile_Integral_Bounded(distance_t_t0-detector_Angular_Width_FWHM/2, beam_Spot_FWHM_Angular, beam_Spot_Smoothing, -max_Beam_Size_Angular/2, max_Beam_Size_Angular/2)
//								);
//				} else
//				if(detector_Sensitivity_Function == distributions[Gaussian])
//				{
////					double weight_Gate = 1-beam_Spot_Smoothing;
////					double weight_Gauss =  beam_Spot_Smoothing;

//					/// use trimmed gate beam profile for Gauss detector sensitivity
//					double beam_Width_Angular = min(beam_Spot_FWHM_Angular, max_Beam_Size_Angular);
//					integral += /*weight_Gate **/(1.-beam_Spot_Wings_Intensity)/spot_Norm_Factor_Gate*Global_Variables::gate_Gauss_Integral (beam_Width_Angular, detector_Angular_Width_FWHM, distance_t_t0);
////					integral += /*weight_Gauss**/(1.-beam_Spot_Wings_Intensity)/spot_Norm_Factor*Global_Variables::gauss_Gauss_Integral(beam_Spot_FWHM_Angular, detector_Angular_Width_FWHM, distance_t_t0);
//				}
//			}
//		}
//		return integral;
//	};
//	/// detector and spot and divergence
//	auto f_SD = [&](double point_Index)
//	{
//		double theta_0 = beam_Theta_0_Position;
//		auto f_Divergence_and_Spot_and_Detector = [&](double theta_0_i)
//		{
//			return beam_Angular_Divergence_Norm_Factor * beam_Angular_Distribution(beam_Angular_Divergence_FWHM, theta_0_i-theta_0) * f_S(point_Index, theta_0_i);
//		};

//		double left_Bound_a  = theta_0 - beam_Angular_Divergence_Range_Limit*beam_Angular_Divergence_FWHM;
//		double right_Bound_a = theta_0 + beam_Angular_Divergence_Range_Limit*beam_Angular_Divergence_FWHM;
//		if(left_Bound_a>=right_Bound_a) return 0.;

//		double integral = 0;
//		vector<double> interpoints(4+1);
//		interpoints[0] = left_Bound_a;
//		for(int i=1; i<interpoints.size(); i++)
//		{
//			interpoints[i] = left_Bound_a + i*(right_Bound_a-left_Bound_a)/(interpoints.size()-1);
//			integral += gauss_kronrod<double,31>::integrate(f_Divergence_and_Spot_and_Detector, interpoints[i-1], interpoints[i], 3, 1e-7);
//		}
//		return integral;
//	};

//	Global_Variables::parallel_For(detector_Position.size(), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
//	{
//		Q_UNUSED(thread_Index)
//		if(beam_Angular_Divergence_FWHM>DBL_EPSILON && (beam_Spot_FWHM_Angular>DBL_EPSILON || (beam_Spot_Wing_Angular>DBL_EPSILON && beam_Spot_Wings_Intensity>DBL_EPSILON)))
//		{
//			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
//			{
//				double integral = f_SD(point_Index);
//				calculated_Values.S_Instrumental[point_Index] += integral*calculated_Values.R.front()/measurement.footprint_Factor_Vec[point_Index];
//			}
//		}
//		if(beam_Angular_Divergence_FWHM<=DBL_EPSILON && (beam_Spot_FWHM_Angular>DBL_EPSILON || (beam_Spot_Wing_Angular>DBL_EPSILON && beam_Spot_Wings_Intensity>DBL_EPSILON)))
//		{
//			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
//			{
//				double integral = f_S(point_Index, beam_Theta_0_Position);
//				calculated_Values.S_Instrumental[point_Index] += integral*calculated_Values.R.front()/measurement.footprint_Factor_Vec[point_Index];
//			}
//		}
//		if(beam_Angular_Divergence_FWHM>DBL_EPSILON && (beam_Spot_FWHM_Angular<=DBL_EPSILON && (beam_Spot_Wing_Angular<=DBL_EPSILON || beam_Spot_Wings_Intensity<=DBL_EPSILON)))
//		{
//			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
//			{
//				double integral = f_D(point_Index);
//				calculated_Values.S_Instrumental[point_Index] += integral*calculated_Values.R.front()/measurement.footprint_Factor_Vec[point_Index];
//			}
//		}
//		if(beam_Angular_Divergence_FWHM<=DBL_EPSILON && (beam_Spot_FWHM_Angular<=DBL_EPSILON && (beam_Spot_Wing_Angular<=DBL_EPSILON || beam_Spot_Wings_Intensity<=DBL_EPSILON)))
//		{
//			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
//			{
//				double integral = f(point_Index);
//				calculated_Values.S_Instrumental[point_Index] += integral*calculated_Values.R.front()/measurement.footprint_Factor_Vec[point_Index];
//			}
//		}
//	});
//}
//void Main_Calculation_Module::wrap_With_Specular_Vector(Calculated_Values& calculated_Values, const Data& measurement)
//{
//	const vector<double>& argument = measurement.beam_Theta_0_Angle_Vec;
//	const vector<double>& beam_Angular_FWHM = measurement.theta_0_Resolution_Vec;
//	double detector_FWHM = measurement.theta_Resolution_FWHM;
//	QString beam_Angular_Function = measurement.beam_Theta_0_Distribution.distribution_Function;
//	QString detector_Function = measurement.theta_Distribution;

//	double beam_Range_Limit = 3;
//	vector<double> beam_Norm_Factor(beam_Angular_FWHM.size(),1);
//	double(*beam_Distribution)(double, double);
//	if(beam_Angular_Function == distributions[Gate])	{beam_Distribution = Global_Variables::distribution_Gate;	  for(size_t i=0;i<beam_Angular_FWHM.size();i++){beam_Norm_Factor[i] = 1./beam_Angular_FWHM[i];					}	beam_Range_Limit = 0.5; } else
//	if(beam_Angular_Function == distributions[Gaussian]){beam_Distribution = Global_Variables::distribution_Gaussian; for(size_t i=0;i<beam_Angular_FWHM.size();i++){beam_Norm_Factor[i] = sqrt(log(16.)/M_PI)/beam_Angular_FWHM[i];}	beam_Range_Limit = 3;   } else return;

//	double detector_Range_Limit = 3;
//	double(*detector_Distribution)(double, double);
//	if(detector_Function == distributions[Gate])	{detector_Distribution = Global_Variables::distribution_Gate;	 detector_Range_Limit = 0.5; } else
//	if(detector_Function == distributions[Gaussian]){detector_Distribution = Global_Variables::distribution_Gaussian;detector_Range_Limit = 3;   } else return;

//	double log_2 = sqrt(log(2));
//	double sqrt_pi = sqrt(M_PI/log(2));

//	// for accounting spot
//	double beam_Spot_FWHM_Angular = qRadiansToDegrees(measurement.beam_Geometry.size/measurement.detector_1D.distance_To_Sample);
//	double beam_Spot_Smoothing = measurement.beam_Geometry.smoothing;
//	double spot_Range_Limit = 3;
//	auto f_Spot = [&](double x){return Global_Variables::beam_Profile(x, 1, beam_Spot_Smoothing);};
//	double spot_Norm_Factor = gauss_kronrod<double,31>::integrate(f_Spot, -spot_Range_Limit, spot_Range_Limit, 3, 1e-7);

//	/// detector only (no spot, no divergence)
//	auto f = [&](double point_Index)
//	{
//		double theta_0 = argument[point_Index];
//		double theta;
//		if(measurement.measurement_Type == measurement_Types[Rocking_Curve])	{
//			theta = 2*measurement.beam_Theta_0_Specular_Position - theta_0;
//		}
//		if(measurement.measurement_Type == measurement_Types[Offset_Scan])		{
//			theta = theta_0 + measurement.detector_Theta_Offset;
//		}

//		double integral = detector_Distribution(detector_FWHM, theta-theta_0);
//		calculated_Values.S_Instrumental[point_Index] += integral*calculated_Values.R[point_Index];
//	};
//	/// detector and angular divergence (no spot)
//	auto f_D = [&](double point_Index)
//	{
//		double theta_0 = argument[point_Index];
//		double theta;
//		if(measurement.measurement_Type == measurement_Types[Rocking_Curve])	{
//			theta = 2*measurement.beam_Theta_0_Specular_Position - theta_0;
//		}
//		if(measurement.measurement_Type == measurement_Types[Offset_Scan])		{
//			theta = theta_0 + measurement.detector_Theta_Offset;
//		}
//		double beam_Angular_FWHM_Val = beam_Angular_FWHM[point_Index];
//		double beam_Norm_Factor_Val = beam_Norm_Factor[point_Index];
//		double distance_t_t0 = theta - theta_0;
//		auto f_Divergence_and_Detector = [&](double theta_a)
//		{
//			return detector_Distribution(detector_FWHM, theta_a-theta) * beam_Distribution(beam_Angular_FWHM_Val, theta_a-theta_0)*beam_Norm_Factor_Val;
//		};

//		double left_Bound_a  = max(theta - detector_Range_Limit*detector_FWHM, theta_0 - beam_Range_Limit*beam_Angular_FWHM_Val);
//		double right_Bound_a = min(theta + detector_Range_Limit*detector_FWHM, theta_0 + beam_Range_Limit*beam_Angular_FWHM_Val);

//		if(left_Bound_a>=right_Bound_a) return;

//		double integral;
//		if(beam_Angular_Function == distributions[Gaussian] && detector_Function == distributions[Gaussian])
//		{
//			integral = beam_Norm_Factor_Val * detector_FWHM * beam_Angular_FWHM_Val * sqrt_pi * pow(2.,-1-4*distance_t_t0*distance_t_t0/(detector_FWHM*detector_FWHM + beam_Angular_FWHM_Val*beam_Angular_FWHM_Val)) / sqrt((detector_FWHM*detector_FWHM + beam_Angular_FWHM_Val*beam_Angular_FWHM_Val));
//		} else
//		if(beam_Angular_Function == distributions[Gaussian] && detector_Function == distributions[Gate])
//		{
//			integral = beam_Norm_Factor_Val * 0.25 * beam_Angular_FWHM_Val * sqrt_pi * (erf(log_2*(detector_FWHM-2*distance_t_t0)/beam_Angular_FWHM_Val)+erf(log_2*(detector_FWHM+2*distance_t_t0)/beam_Angular_FWHM_Val));
//		} else
//		if(beam_Angular_Function == distributions[Gate] && detector_Function == distributions[Gaussian])
//		{
//			integral = beam_Norm_Factor_Val * 0.25 * detector_FWHM * sqrt_pi * (erf(log_2*(beam_Angular_FWHM_Val-2*distance_t_t0)/detector_FWHM)+erf(log_2*(beam_Angular_FWHM_Val+2*distance_t_t0)/detector_FWHM));
//		} else
//		{
//			integral = gauss_kronrod<double,31>::integrate(f_Divergence_and_Detector, left_Bound_a, right_Bound_a, 3, 1e-7);
//		}
//		calculated_Values.S_Instrumental[point_Index] += integral*calculated_Values.R[point_Index];
//	};
//	/// detector and spot (no divergence)
//	auto f_S = [&](double point_Index)
//	{
//		double theta_0 = argument[point_Index];
//		double theta;
//		if(measurement.measurement_Type == measurement_Types[Rocking_Curve])	{
//			theta = 2*measurement.beam_Theta_0_Specular_Position - theta_0;
//		}
//		if(measurement.measurement_Type == measurement_Types[Offset_Scan])		{
//			theta = theta_0 + measurement.detector_Theta_Offset;
//		}
//		auto f_Spot_and_Detector = [&](double theta_i)
//		{
//			return detector_Distribution(detector_FWHM, theta_i-theta) * Global_Variables::beam_Profile(theta_i-theta_0, beam_Spot_FWHM_Angular, beam_Spot_Smoothing)/(beam_Spot_FWHM_Angular*spot_Norm_Factor);
//		};

//		double left_Bound_i  = max(theta - detector_Range_Limit*detector_FWHM, theta_0 - spot_Range_Limit*beam_Spot_FWHM_Angular);
//		double right_Bound_i = min(theta + detector_Range_Limit*detector_FWHM, theta_0 + spot_Range_Limit*beam_Spot_FWHM_Angular);

//		if(left_Bound_i>=right_Bound_i) return;

//		double integral = gauss_kronrod<double,31>::integrate(f_Spot_and_Detector, left_Bound_i, right_Bound_i, 3, 1e-7);
//		calculated_Values.S_Instrumental[point_Index] += integral*calculated_Values.R[point_Index];
//	};
//	/// detector and spot and divergence
//	auto f_SD = [&](double point_Index)
//	{
//		double theta_0 = argument[point_Index];
//		double theta;
//		if(measurement.measurement_Type == measurement_Types[Rocking_Curve])	{
//			theta = 2*measurement.beam_Theta_0_Specular_Position - theta_0;
//		}
//		if(measurement.measurement_Type == measurement_Types[Offset_Scan])		{
//			theta = theta_0 + measurement.detector_Theta_Offset;
//		}
//		double beam_Angular_FWHM_Val = beam_Angular_FWHM[point_Index];
//		double beam_Norm_Factor_Val = beam_Norm_Factor[point_Index];
//		auto f_Divergence_and_Spot_and_Detector = [&](double theta_i)
//		{
//			auto f_Divergence_and_Spot = [&](double theta_a)
//			{
//				return Global_Variables::beam_Profile(theta_a-theta_i, beam_Spot_FWHM_Angular, beam_Spot_Smoothing)/beam_Spot_FWHM_Angular * beam_Distribution(beam_Angular_FWHM_Val, theta_a-theta_0)*beam_Norm_Factor_Val;
//			};
//			double left_Bound_a  = max(-spot_Range_Limit*beam_Spot_FWHM_Angular + theta_i, -beam_Range_Limit*beam_Angular_FWHM_Val + theta_0);
//			double right_Bound_a = min( spot_Range_Limit*beam_Spot_FWHM_Angular + theta_i,  beam_Range_Limit*beam_Angular_FWHM_Val + theta_0);

//			if(left_Bound_a>=right_Bound_a) return 0.;

//			return detector_Distribution(detector_FWHM, theta_i-theta) * gauss_kronrod<double,31>::integrate(f_Divergence_and_Spot, left_Bound_a, right_Bound_a, 3, 1e-7);
//		};

//		double left_Bound_i  = theta - detector_Range_Limit*detector_FWHM;
//		double right_Bound_i = theta + detector_Range_Limit*detector_FWHM;

//		if(left_Bound_i>=right_Bound_i) return;

//		double integral = gauss_kronrod<double,31>::integrate(f_Divergence_and_Spot_and_Detector, left_Bound_i, right_Bound_i, 3, 1e-7);
//		calculated_Values.S_Instrumental[point_Index] += integral*calculated_Values.R[point_Index];
//	};

//	Global_Variables::parallel_For(argument.size(), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
//	{
//		Q_UNUSED(thread_Index)
//		for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
//		{
//			if(beam_Angular_FWHM[point_Index]>DBL_EPSILON && beam_Spot_FWHM_Angular>DBL_EPSILON)
//			{
//				f_SD(point_Index);
//			}
//			if(beam_Angular_FWHM[point_Index]<=DBL_EPSILON && beam_Spot_FWHM_Angular>DBL_EPSILON)
//			{
//				f_S(point_Index);
//			}
//			if(beam_Angular_FWHM[point_Index]>DBL_EPSILON && beam_Spot_FWHM_Angular<=DBL_EPSILON)
//			{
//				f_D(point_Index);
//			}
//			if(beam_Angular_FWHM[point_Index]<=DBL_EPSILON && beam_Spot_FWHM_Angular<=DBL_EPSILON)
//			{
//				f(point_Index);
//			}
//		}
//	});
//}

void Main_Calculation_Module::wrap_With_Specular_2D(Calculated_Values& calculated_Values, const Data& measurement)
{
	// detector
	double detector_FWHM_Theta = measurement.theta_Resolution_FWHM;
	double detector_Range_Limit_Theta = 2;
	double(*detector_Distribution_Theta)(double, double);
	if(measurement.theta_Distribution == distributions[Gate])		{detector_Distribution_Theta = Global_Variables::distribution_Gate;		detector_Range_Limit_Theta = 0.5;  } else
	if(measurement.theta_Distribution == distributions[Gaussian])	{detector_Distribution_Theta = Global_Variables::distribution_Gaussian;	detector_Range_Limit_Theta = 2;    } else return;

	double detector_FWHM_Phi = measurement.phi_Resolution_FWHM;
	double detector_Range_Limit_Phi = 2;
	double(*detector_Distribution_Phi)(double, double);
	if(measurement.phi_Distribution == distributions[Gate])		{detector_Distribution_Phi = Global_Variables::distribution_Gate;		detector_Range_Limit_Phi = 0.5;  } else
	if(measurement.phi_Distribution == distributions[Gaussian])	{detector_Distribution_Phi = Global_Variables::distribution_Gaussian;	detector_Range_Limit_Phi = 2;    } else return;

	// beam angular divergence
	double beam_Diverg_FWHM_Theta_0 = measurement.beam_Theta_0_Divergence_With_Curvature;//measurement.beam_Theta_0_Distribution.FWHM_distribution;
	double beam_Diverg_Range_Limit_Theta_0 = 3;
	double beam_Diverg_Norm_Factor_Theta_0 = 1;
	double(*beam_Distribution_Theta_0)(double, double);
	if(measurement.beam_Theta_0_Distribution.distribution_Function == distributions[Gate])		{beam_Distribution_Theta_0 = Global_Variables::distribution_Gate;	    beam_Diverg_Norm_Factor_Theta_0 = 1./beam_Diverg_FWHM_Theta_0;					beam_Diverg_Range_Limit_Theta_0 = 0.5;  } else
	if(measurement.beam_Theta_0_Distribution.distribution_Function == distributions[Gaussian])	{beam_Distribution_Theta_0 = Global_Variables::distribution_Gaussian;	beam_Diverg_Norm_Factor_Theta_0 = sqrt(log(16.)/M_PI)/beam_Diverg_FWHM_Theta_0;	beam_Diverg_Range_Limit_Theta_0 = 3;    } else return;

	double beam_Diverg_FWHM_Phi_0 = measurement.beam_Phi_0_Distribution.FWHM_distribution;
	double beam_Diverg_Range_Limit_Phi_0 = 3;
	double beam_Diverg_Norm_Factor_Phi_0 = 1;
	double(*beam_Distribution_Phi_0)(double, double);
	if(measurement.beam_Phi_0_Distribution.distribution_Function == distributions[Gate])	{beam_Distribution_Phi_0 = Global_Variables::distribution_Gate;		beam_Diverg_Norm_Factor_Phi_0 = 1./beam_Diverg_FWHM_Phi_0;					beam_Diverg_Range_Limit_Phi_0 = 0.5;  } else
	if(measurement.beam_Phi_0_Distribution.distribution_Function == distributions[Gaussian]){beam_Distribution_Phi_0 = Global_Variables::distribution_Gaussian; beam_Diverg_Norm_Factor_Phi_0 = sqrt(log(16.)/M_PI)/beam_Diverg_FWHM_Phi_0;	beam_Diverg_Range_Limit_Phi_0 = 3;    } else return;

	// beam spot
	double beam_Spot_FWHM_Theta_0 = qRadiansToDegrees(measurement.beam_Geometry.size                                     /measurement.detector_2D.distance_To_Sample);
	double max_Spot_Size_Theta_0  = qRadiansToDegrees(measurement.sample_Geometry.size*measurement.beam_Theta_0_Sin_Value/measurement.detector_2D.distance_To_Sample);
	double beam_Spot_Wing_FWHM_Theta_0 = qRadiansToDegrees(measurement.beam_Geometry.wings_Full_Width					 /measurement.detector_2D.distance_To_Sample);
	double beam_Spot_Wings_Theta_0_Intensity = measurement.beam_Geometry.wings_Intensity;
	double beam_Spot_Smoothing_Theta_0 = measurement.beam_Geometry.smoothing;
	double spot_Range_Main_Limit = 3;
	double spot_Range_Wing_Limit = 0.5;

	double arg_Limit_Theta_0 = max(10*beam_Spot_FWHM_Theta_0,10*beam_Spot_Wing_FWHM_Theta_0);
	double spot_Norm_Factor_Theta_0 = Global_Variables::beam_Profile_With_Wings_Integral( arg_Limit_Theta_0, beam_Spot_FWHM_Theta_0, beam_Spot_Smoothing_Theta_0, beam_Spot_Wing_FWHM_Theta_0, beam_Spot_Wings_Theta_0_Intensity)-
									  Global_Variables::beam_Profile_With_Wings_Integral(-arg_Limit_Theta_0, beam_Spot_FWHM_Theta_0, beam_Spot_Smoothing_Theta_0, beam_Spot_Wing_FWHM_Theta_0, beam_Spot_Wings_Theta_0_Intensity);
	double spot_Norm_Factor_Theta_0_Gate = Global_Variables::beam_Profile_With_Wings_Integral( arg_Limit_Theta_0, beam_Spot_FWHM_Theta_0, 0, beam_Spot_Wing_FWHM_Theta_0, beam_Spot_Wings_Theta_0_Intensity)-
										   Global_Variables::beam_Profile_With_Wings_Integral(-arg_Limit_Theta_0, beam_Spot_FWHM_Theta_0, 0, beam_Spot_Wing_FWHM_Theta_0, beam_Spot_Wings_Theta_0_Intensity);

	double beam_Spot_FWHM_Phi_0   = qRadiansToDegrees(measurement.beam_Geometry.lateral_Width/measurement.detector_2D.distance_To_Sample);
	double spot_Norm_Factor_Phi_0 = beam_Spot_FWHM_Phi_0;
//	double spot_Norm_Factor_Phi_0 = Global_Variables::beam_Profile_Integral( beam_Spot_FWHM_Phi_0/2, beam_Spot_FWHM_Phi_0, 0)-
//									Global_Variables::beam_Profile_Integral(-beam_Spot_FWHM_Phi_0/2, beam_Spot_FWHM_Phi_0, 0);


	// theta
	/// detector only (no spot, no divergence)
	auto f_Theta = [&](double theta_Index)
	{
		double theta_0 = measurement.beam_Theta_0_Angle_Value;
		double theta = measurement.detector_Theta_Angle_Vec[theta_Index];
		return detector_Distribution_Theta(detector_FWHM_Theta, theta_0-theta);
	};
	/// detector and angular divergence (no spot)
	auto f_Theta_D = [&](double theta_Index)
	{
		double theta_0 = measurement.beam_Theta_0_Angle_Value;
		double theta = measurement.detector_Theta_Angle_Vec[theta_Index];
		double distance_t_t0 = theta - theta_0;

		double left_Bound_a  = max(theta - detector_Range_Limit_Theta*detector_FWHM_Theta, theta_0 - beam_Diverg_Range_Limit_Theta_0*beam_Diverg_FWHM_Theta_0);
		double right_Bound_a = min(theta + detector_Range_Limit_Theta*detector_FWHM_Theta, theta_0 + beam_Diverg_Range_Limit_Theta_0*beam_Diverg_FWHM_Theta_0);

		if(left_Bound_a>=right_Bound_a) return 0.;

		// specific cases
		double integral_Theta;
		if(measurement.beam_Theta_0_Distribution.distribution_Function == distributions[Gaussian] && measurement.theta_Distribution == distributions[Gaussian])
		{
			integral_Theta = Global_Variables::gauss_Gauss_Integral(detector_FWHM_Theta, beam_Diverg_FWHM_Theta_0, distance_t_t0);
		} else
		if(measurement.beam_Theta_0_Distribution.distribution_Function == distributions[Gaussian] && measurement.theta_Distribution == distributions[Gate])
		{
			integral_Theta = Global_Variables::gate_Gauss_Integral(detector_FWHM_Theta, beam_Diverg_FWHM_Theta_0, distance_t_t0);
		} else
		if(measurement.beam_Theta_0_Distribution.distribution_Function == distributions[Gate] && measurement.theta_Distribution == distributions[Gaussian])
		{
			integral_Theta = Global_Variables::gate_Gauss_Integral(beam_Diverg_FWHM_Theta_0, detector_FWHM_Theta, distance_t_t0);
		} else
		{
			integral_Theta = Global_Variables::gate_Gate_Integral(detector_FWHM_Theta, beam_Diverg_FWHM_Theta_0, theta, theta_0);
		}
		return beam_Diverg_Norm_Factor_Theta_0*integral_Theta;
	};
	/// detector and spot (no divergence)
	auto f_Theta_S = [&](double theta_Index, double my_Theta_0)
	{
		double theta_0 = my_Theta_0;
		double theta = measurement.detector_Theta_Angle_Vec[theta_Index];
		double distance_t_t0 = theta - theta_0;

		double left_Bound_Wings = max(theta - detector_Range_Limit_Theta*detector_FWHM_Theta, theta_0 - spot_Range_Wing_Limit*beam_Spot_Wing_FWHM_Theta_0);
		double right_Bound_Wings= min(theta + detector_Range_Limit_Theta*detector_FWHM_Theta, theta_0 + spot_Range_Wing_Limit*beam_Spot_Wing_FWHM_Theta_0);

		double left_Bound_i		= max(theta - detector_Range_Limit_Theta*detector_FWHM_Theta, theta_0 - spot_Range_Main_Limit*beam_Spot_FWHM_Theta_0);
		double right_Bound_i	= min(theta + detector_Range_Limit_Theta*detector_FWHM_Theta, theta_0 + spot_Range_Main_Limit*beam_Spot_FWHM_Theta_0);

		// specific cases
		double integral_Theta = 0;

		// wings
		if(beam_Spot_Wing_FWHM_Theta_0 > DBL_EPSILON && beam_Spot_Wings_Theta_0_Intensity > DBL_EPSILON && right_Bound_Wings>left_Bound_Wings)
		{
			double wings_Width_Theta_0 = min(beam_Spot_Wing_FWHM_Theta_0, max_Spot_Size_Theta_0);

			if(measurement.theta_Distribution == distributions[Gate])
			{
				integral_Theta += beam_Spot_Wing_FWHM_Theta_0/spot_Norm_Factor_Theta_0*Global_Variables::gate_Gate_Integral(detector_FWHM_Theta, wings_Width_Theta_0, theta, theta_0);
			} else
			if(measurement.theta_Distribution == distributions[Gaussian])
			{
				integral_Theta += beam_Spot_Wing_FWHM_Theta_0/spot_Norm_Factor_Theta_0*Global_Variables::gate_Gauss_Integral(wings_Width_Theta_0, detector_FWHM_Theta, distance_t_t0);
			}
		}
		if(beam_Spot_Wing_FWHM_Theta_0 <= DBL_EPSILON || beam_Spot_Wings_Theta_0_Intensity <= DBL_EPSILON)
		{
			beam_Spot_Wings_Theta_0_Intensity = 0; // for using in beam weight
		}

		// main beam
		if(right_Bound_i>left_Bound_i)
		{
			if(beam_Spot_FWHM_Theta_0 > DBL_EPSILON)
			{
				if(measurement.theta_Distribution == distributions[Gate])
				{
					integral_Theta += (1.-beam_Spot_Wings_Theta_0_Intensity)/spot_Norm_Factor_Theta_0*(
							Global_Variables::beam_Profile_Integral_Bounded(distance_t_t0+detector_FWHM_Theta/2, beam_Spot_FWHM_Theta_0, beam_Spot_Smoothing_Theta_0, -max_Spot_Size_Theta_0/2, max_Spot_Size_Theta_0/2)
						   -Global_Variables::beam_Profile_Integral_Bounded(distance_t_t0-detector_FWHM_Theta/2, beam_Spot_FWHM_Theta_0, beam_Spot_Smoothing_Theta_0, -max_Spot_Size_Theta_0/2, max_Spot_Size_Theta_0/2)
								);
				} else
				if(measurement.theta_Distribution == distributions[Gaussian])
				{
//					double weight_Gate = 1-beam_Spot_Smoothing_Theta_0;
//					double weight_Gauss =  beam_Spot_Smoothing_Theta_0;

					/// use trimmed gate beam profile for Gauss detector sensitivity
					double beam_Width_Angular = min(beam_Spot_FWHM_Theta_0, max_Spot_Size_Theta_0);
					integral_Theta += /*weight_Gate **/(1.-beam_Spot_Wings_Theta_0_Intensity)/spot_Norm_Factor_Theta_0_Gate*Global_Variables::gate_Gauss_Integral (beam_Width_Angular,     detector_FWHM_Theta, distance_t_t0);
//					integral_Theta += /*weight_Gauss**/(1.-beam_Spot_Wings_Theta_0_Intensity)/spot_Norm_Factor_Theta_0*     Global_Variables::gauss_Gauss_Integral(beam_Spot_FWHM_Angular, detector_FWHM_Theta, distance_t_t0);
				}
			}
		}
		return integral_Theta;
	};
	/// detector and spot and divergence
	auto f_Theta_SD = [&](double theta_Index)
	{
		double theta_0 = measurement.beam_Theta_0_Angle_Value;
		auto f_Divergence_and_Spot_and_Detector_Theta = [&](double theta_0_i)
		{
			return beam_Diverg_Norm_Factor_Theta_0 * beam_Distribution_Theta_0(beam_Diverg_FWHM_Theta_0, theta_0_i-theta_0) * f_Theta_S(theta_Index, theta_0_i);
		};

		double left_Bound_a  = theta_0 - beam_Diverg_Range_Limit_Theta_0*beam_Diverg_FWHM_Theta_0;
		double right_Bound_a = theta_0 + beam_Diverg_Range_Limit_Theta_0*beam_Diverg_FWHM_Theta_0;

		if(left_Bound_a>=right_Bound_a) return 0.;

		double integral_Theta = 0;
		vector<double> interpoints(4+1);
		interpoints[0] = left_Bound_a;
		for(int i=1; i<interpoints.size(); i++)
		{
			interpoints[i] = left_Bound_a + i*(right_Bound_a-left_Bound_a)/(interpoints.size()-1);
			integral_Theta += gauss_kronrod<double,31>::integrate(f_Divergence_and_Spot_and_Detector_Theta, interpoints[i-1], interpoints[i], 3, 1e-7);
		}
		return integral_Theta;
	};


	// phi
	/// detector only (no spot, no divergence)
	auto f_Phi = [&](double phi_Index)
	{
		double phi = measurement.detector_Phi_Angle_Vec[phi_Index];
		return detector_Distribution_Phi(detector_FWHM_Phi, phi);
	};
	/// detector and angular divergence (no spot)
	auto f_Phi_D = [&](double phi_Index)
	{
		double phi_0 = 0;
		double phi = measurement.detector_Phi_Angle_Vec[phi_Index];
		double distance_p_p0 = phi - phi_0;

		double left_Bound_a  = max(phi - detector_Range_Limit_Phi*detector_FWHM_Phi, phi_0 - beam_Diverg_Range_Limit_Phi_0*beam_Diverg_FWHM_Phi_0);
		double right_Bound_a = min(phi + detector_Range_Limit_Phi*detector_FWHM_Phi, phi_0 + beam_Diverg_Range_Limit_Phi_0*beam_Diverg_FWHM_Phi_0);

		if(left_Bound_a>=right_Bound_a) return 0.;

		// specific cases
		double integral_Phi;
		if(measurement.beam_Phi_0_Distribution.distribution_Function == distributions[Gaussian] && measurement.phi_Distribution == distributions[Gaussian])
		{
			integral_Phi = Global_Variables::gauss_Gauss_Integral(detector_FWHM_Phi, beam_Diverg_FWHM_Phi_0, distance_p_p0);
		} else
		if(measurement.beam_Phi_0_Distribution.distribution_Function == distributions[Gaussian] && measurement.phi_Distribution == distributions[Gate])
		{
			integral_Phi = Global_Variables::gate_Gauss_Integral(detector_FWHM_Phi, beam_Diverg_FWHM_Phi_0, distance_p_p0);
		} else
		if(measurement.beam_Phi_0_Distribution.distribution_Function == distributions[Gate] && measurement.phi_Distribution == distributions[Gaussian])
		{
			integral_Phi = Global_Variables::gate_Gauss_Integral(beam_Diverg_FWHM_Phi_0, detector_FWHM_Phi, distance_p_p0);
		} else
		{
			integral_Phi = Global_Variables::gate_Gate_Integral(detector_FWHM_Phi, beam_Diverg_FWHM_Phi_0, phi, phi_0);
		}
		return beam_Diverg_Norm_Factor_Phi_0*integral_Phi;
	};
	/// detector and spot (no divergence)
	auto f_Phi_S = [&](double phi_Index, double my_Phi_0)
	{
		double phi_0 = my_Phi_0;
		double phi = measurement.detector_Phi_Angle_Vec[phi_Index];
		double distance_p_p0 = phi - phi_0;

		double  left_Bound_i = max(phi - detector_Range_Limit_Phi*detector_FWHM_Phi, phi_0 - spot_Range_Main_Limit*beam_Spot_FWHM_Phi_0);
		double right_Bound_i = min(phi + detector_Range_Limit_Phi*detector_FWHM_Phi, phi_0 + spot_Range_Main_Limit*beam_Spot_FWHM_Phi_0);

		// specific cases
		double integral_Phi = 0;
		if(right_Bound_i>left_Bound_i)
		{
			if(beam_Spot_FWHM_Phi_0 > DBL_EPSILON)
			{
				if(measurement.phi_Distribution == distributions[Gate])
				{
					integral_Phi += 1/spot_Norm_Factor_Phi_0*(
							Global_Variables::beam_Profile_Integral(distance_p_p0+detector_FWHM_Phi/2, beam_Spot_FWHM_Phi_0, 0)
						   -Global_Variables::beam_Profile_Integral(distance_p_p0-detector_FWHM_Phi/2, beam_Spot_FWHM_Phi_0, 0)
								);
				} else
				if(measurement.phi_Distribution == distributions[Gaussian])
				{
					integral_Phi += 1/spot_Norm_Factor_Phi_0*Global_Variables::gate_Gauss_Integral(beam_Spot_FWHM_Phi_0, detector_FWHM_Phi, distance_p_p0);
				}
			}
		}
		return integral_Phi;
	};
	/// detector and spot and divergence
	auto f_Phi_SD = [&](double phi_Index)
	{
		double phi_0 = 0;
		auto f_Divergence_and_Spot_and_Detector_Phi = [&](double phi_0_i)
		{
			return beam_Diverg_Norm_Factor_Phi_0 * beam_Distribution_Phi_0(beam_Diverg_FWHM_Phi_0, phi_0_i-phi_0) * f_Phi_S(phi_Index, phi_0_i);
		};

		double left_Bound_a  = phi_0 - beam_Diverg_Range_Limit_Phi_0*beam_Diverg_FWHM_Phi_0;
		double right_Bound_a = phi_0 + beam_Diverg_Range_Limit_Phi_0*beam_Diverg_FWHM_Phi_0;

		if(left_Bound_a>=right_Bound_a) return 0.;

		double integral_Phi = 0;
		vector<double> interpoints(4+1);
		interpoints[0] = left_Bound_a;
		for(int i=1; i<interpoints.size(); i++)
		{
			interpoints[i] = left_Bound_a + i*(right_Bound_a-left_Bound_a)/(interpoints.size()-1);
			integral_Phi += gauss_kronrod<double,31>::integrate(f_Divergence_and_Spot_and_Detector_Phi, interpoints[i-1], interpoints[i], 3, 1e-7);
		}
		return integral_Phi;
	};

	bool use_Beam_Spot = true;

	// parallelizing phi calculations
	vector<double> f_Phi_Vec(calculated_Values.GISAS_Instrumental.size(),-2020);
	Global_Variables::parallel_For(int(f_Phi_Vec.size()), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		if(use_Beam_Spot)
		{
			if(beam_Diverg_FWHM_Phi_0>DBL_EPSILON && beam_Spot_FWHM_Phi_0>DBL_EPSILON)	 {
				for(int phi_Index=n_Min; phi_Index<n_Max; ++phi_Index)
				{f_Phi_Vec[phi_Index] = f_Phi_SD(phi_Index);}
			}
			if(beam_Diverg_FWHM_Phi_0<=DBL_EPSILON && beam_Spot_FWHM_Phi_0>DBL_EPSILON) {
				for(int phi_Index=n_Min; phi_Index<n_Max; ++phi_Index)
				{f_Phi_Vec[phi_Index] = f_Phi_S(phi_Index,0);}
			}
		}
		if(beam_Diverg_FWHM_Phi_0>DBL_EPSILON && (!use_Beam_Spot || beam_Spot_FWHM_Phi_0<=DBL_EPSILON))	{
			for(int phi_Index=n_Min; phi_Index<n_Max; ++phi_Index)
			{f_Phi_Vec[phi_Index] = f_Phi_D(phi_Index);}
		}
		if(beam_Diverg_FWHM_Phi_0<=DBL_EPSILON && (!use_Beam_Spot || beam_Spot_FWHM_Phi_0<=DBL_EPSILON))	{
			for(int phi_Index=n_Min; phi_Index<n_Max; ++phi_Index)
			{f_Phi_Vec[phi_Index] = f_Phi(phi_Index);}
		}
	});

	Global_Variables::parallel_For(int(measurement.detector_Theta_Angle_Vec.size()), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		Q_UNUSED(thread_Index)

		for(int theta_Index=n_Min; theta_Index<n_Max; ++theta_Index)
		{
			double f_Theta_Val = -2020;
			if(use_Beam_Spot)
			{
				if(beam_Diverg_FWHM_Theta_0>DBL_EPSILON && (beam_Spot_FWHM_Theta_0>DBL_EPSILON || (beam_Spot_Wing_FWHM_Theta_0>DBL_EPSILON && beam_Spot_Wings_Theta_0_Intensity>DBL_EPSILON)))	  {
					f_Theta_Val = f_Theta_SD(theta_Index);
				}
				if(beam_Diverg_FWHM_Theta_0<=DBL_EPSILON && (beam_Spot_FWHM_Theta_0>DBL_EPSILON || (beam_Spot_Wing_FWHM_Theta_0>DBL_EPSILON && beam_Spot_Wings_Theta_0_Intensity>DBL_EPSILON)))  {
					f_Theta_Val = f_Theta_S(theta_Index,measurement.beam_Theta_0_Angle_Value);
				}
			}
			if(beam_Diverg_FWHM_Theta_0>DBL_EPSILON && (!use_Beam_Spot || (beam_Spot_FWHM_Theta_0<=DBL_EPSILON && (beam_Spot_Wing_FWHM_Theta_0<=DBL_EPSILON || beam_Spot_Wings_Theta_0_Intensity<=DBL_EPSILON))))  {
				f_Theta_Val = f_Theta_D(theta_Index);
			}
			if(beam_Diverg_FWHM_Theta_0<=DBL_EPSILON && (!use_Beam_Spot || (beam_Spot_FWHM_Theta_0<=DBL_EPSILON && (beam_Spot_Wing_FWHM_Theta_0<=DBL_EPSILON || beam_Spot_Wings_Theta_0_Intensity<=DBL_EPSILON))))  {
				f_Theta_Val = f_Theta(theta_Index);
			}
			f_Theta_Val = f_Theta_Val*calculated_Values.R.front();

			for(size_t phi_Index=0; phi_Index<calculated_Values.GISAS_Instrumental.size(); ++phi_Index)
			{
				calculated_Values.GISAS_Instrumental[phi_Index][theta_Index] += f_Theta_Val*f_Phi_Vec[phi_Index];
			}
		}
	});
}

void Main_Calculation_Module::wrap_2D_Curve(const Data& measurement,
											Calculated_Values& calculated_Values,
											const vector<double>& resolution_FWHM,
											QString distribution_Function,
											QString theta_Phi,
											bool theta_0_Beam_Profile)
{
	double range_Limit = 3;
	double(*distribution)(double, double);
	if(distribution_Function == distributions[Gate])	{distribution = Global_Variables::distribution_Gate;	range_Limit = 0.5; } else
	if(distribution_Function == distributions[Gaussian]){distribution = Global_Variables::distribution_Gaussian;range_Limit = 3;   } else// return;
	// TODO special case for test phi
	if(distribution_Function == "Short Lorentz")		{distribution = Global_Variables::distribution_Short_Lorentz; range_Limit = 25;  } else return;

	vector<vector<double>>& calculated = calculated_Values.GISAS_Map;
	vector<vector<double>>& wrapped = calculated_Values.GISAS_Instrumental;

	size_t num_Points = calculated.front().size();
	size_t phi_Points = calculated.size();

	// splining beam profile with wings for faster calculations
	gsl_spline* spline_Beam_Profile_With_Wings;
	gsl_interp_accel* acc_Beam_Profile_With_Wings;
	double max_Width;
	if(theta_0_Beam_Profile)
	{
		int profile_Points = 100;
		QVector<double> arg(profile_Points);
		QVector<double> val(profile_Points);

		max_Width = Global_Variables::beam_Profile_Bounds(measurement.beam_Geometry.size,measurement.beam_Geometry.size*measurement.beam_Geometry.smoothing);
		if(measurement.beam_Geometry.wings_Full_Width>DBL_EPSILON && measurement.beam_Geometry.wings_Intensity>DBL_EPSILON)	{
			max_Width = max(max_Width, measurement.beam_Geometry.wings_Full_Width/2);
		}
		max_Width = min(max_Width, measurement.sample_Geometry.size*measurement.beam_Theta_0_Sin_Value/2);
		max_Width = max(max_Width, DBL_EPSILON);
		double step = max_Width/(profile_Points-1);

		double x = 0;
		for(int i=0; i<profile_Points; i++)
		{
			arg[i] = x;
			val[i] = Global_Variables::beam_Profile_With_Wings(x, measurement.beam_Geometry.size, measurement.beam_Geometry.smoothing, measurement.beam_Geometry.wings_Full_Width, measurement.beam_Geometry.wings_Intensity);
			x+=step;
		}
		arg.append(max_Width+2*step);
		val.append(0);
		arg.append(MAX_DOUBLE);
		val.append(0);

		acc_Beam_Profile_With_Wings = gsl_interp_accel_alloc();
		spline_Beam_Profile_With_Wings = gsl_spline_alloc(gsl_interp_steffen, arg.size());
		gsl_spline_init(spline_Beam_Profile_With_Wings, arg.data(), val.data(), arg.size());
	}
	auto f_Beam_Profile_With_Wings = [&](double x_Distance)
	{
		return gsl_spline_eval(spline_Beam_Profile_With_Wings, x_Distance, acc_Beam_Profile_With_Wings);
	};


	auto f_Theta = [&](double theta_Index, double phi_Index)
	{
		wrapped[phi_Index][theta_Index] = calculated[phi_Index][theta_Index];
		double weight_Accumulator = 1;
		double weight = DBL_MIN;

		/*  positive theta direction  */
		double theta_Distance = 0;
		for(size_t t=theta_Index+1; (theta_Distance<range_Limit*resolution_FWHM[theta_Index] && t<num_Points); t++)
		{
			theta_Distance = abs(measurement.detector_Theta_Angle_Vec[t] - measurement.detector_Theta_Angle_Vec[theta_Index]);
			weight = distribution(resolution_FWHM[theta_Index], theta_Distance);

			wrapped[phi_Index][theta_Index] += weight*calculated[phi_Index][t];
			weight_Accumulator += weight;
		}
		/*  negative theta direction  */
		theta_Distance = 0;
		for(int t=theta_Index-1; (theta_Distance<range_Limit*resolution_FWHM[theta_Index] && t>=0); t--)
		{
			theta_Distance = abs(measurement.detector_Theta_Angle_Vec[t] - measurement.detector_Theta_Angle_Vec[theta_Index]);
			weight = distribution(resolution_FWHM[theta_Index], theta_Distance);

			wrapped[phi_Index][theta_Index] += weight*calculated[phi_Index][t];
			weight_Accumulator += weight;
		}
		wrapped[phi_Index][theta_Index] /= weight_Accumulator;
	};
	auto f_Theta_Beam_Spot_Profile = [&](double theta_Index, double phi_Index)
	{
		wrapped[phi_Index][theta_Index] = calculated[phi_Index][theta_Index];
		double weight_Accumulator = 1;
		double weight = DBL_MIN;

		/*  positive theta direction  */
		double theta_Distance = 0;
		double x_Distance = 0;
		for(size_t t=theta_Index+1; (x_Distance<max_Width  && t<num_Points); t++)
		{
			theta_Distance = abs(measurement.detector_Theta_Angle_Vec[t] - measurement.detector_Theta_Angle_Vec[theta_Index]);
			x_Distance = qDegreesToRadians(theta_Distance)*measurement.detector_2D.distance_To_Sample;
			weight = f_Beam_Profile_With_Wings(x_Distance);

			wrapped[phi_Index][theta_Index] += weight*calculated[phi_Index][t];
			weight_Accumulator += weight;
		}
		/*  negative theta direction  */
		theta_Distance = 0;
		x_Distance = 0;
		for(int t=theta_Index-1; (x_Distance<max_Width && t>=0); t--)
		{
			theta_Distance = abs(measurement.detector_Theta_Angle_Vec[t] - measurement.detector_Theta_Angle_Vec[theta_Index]);
			x_Distance = qDegreesToRadians(theta_Distance)*measurement.detector_2D.distance_To_Sample;
			weight = f_Beam_Profile_With_Wings(x_Distance);

			wrapped[phi_Index][theta_Index] += weight*calculated[phi_Index][t];
			weight_Accumulator += weight;
		}
		wrapped[phi_Index][theta_Index] /= weight_Accumulator;
	};

	auto f_Phi = [&](double theta_Index, double phi_Index)
	{
		wrapped[phi_Index][theta_Index] = calculated[phi_Index][theta_Index];
		double weight_Accumulator = 1;
		double weight = DBL_MIN;

		/*  positive phi direction  */
		double phi_Distance = 0;
		for(size_t p=phi_Index+1; (phi_Distance<range_Limit*resolution_FWHM[phi_Index] && p<phi_Points); p++)
		{
			phi_Distance = abs(measurement.detector_Phi_Angle_Vec[p] - measurement.detector_Phi_Angle_Vec[phi_Index]);
			weight = distribution(resolution_FWHM[phi_Index], phi_Distance);

			wrapped[phi_Index][theta_Index] += weight*calculated[p][theta_Index];
			weight_Accumulator += weight;
		}
		/*  negative phi direction  */
		phi_Distance = 0;
		for(int p=phi_Index-1; (phi_Distance<range_Limit*resolution_FWHM[phi_Index] && p>=0); p--)
		{
			phi_Distance = abs(measurement.detector_Phi_Angle_Vec[p] - measurement.detector_Phi_Angle_Vec[phi_Index]);
			weight = distribution(resolution_FWHM[phi_Index], phi_Distance);

			wrapped[phi_Index][theta_Index] += weight*calculated[p][theta_Index];
			weight_Accumulator += weight;
		}
		wrapped[phi_Index][theta_Index] /= weight_Accumulator;
	};

	// program
	Global_Variables::parallel_For(measurement.detector_Theta_Angle_Vec.size(), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		Q_UNUSED(thread_Index)
		if(theta_Phi == "theta")
		{
			if(theta_0_Beam_Profile)
			{
				for(int theta_Index=n_Min; theta_Index<n_Max; ++theta_Index)
				{
					for(size_t phi_Index=0; phi_Index<phi_Points; ++phi_Index)
					{
						f_Theta_Beam_Spot_Profile(theta_Index, phi_Index);
					}
				}
			} else
			{
				for(int theta_Index=n_Min; theta_Index<n_Max; ++theta_Index)
				{
					for(size_t phi_Index=0; phi_Index<phi_Points; ++phi_Index)
					{
						f_Theta(theta_Index, phi_Index);
					}
				}
			}
		} else
		{
			for(int theta_Index=n_Min; theta_Index<n_Max; ++theta_Index)
			{
				for(size_t phi_Index=0; phi_Index<phi_Points; ++phi_Index)
				{
					f_Phi(theta_Index, phi_Index);
				}
			}
		}
	});
	if(theta_0_Beam_Profile)
	{
		gsl_spline_free(spline_Beam_Profile_With_Wings);
		gsl_interp_accel_free(acc_Beam_Profile_With_Wings);
	}
}

template<class Type>
void Main_Calculation_Module::postprocessing(Data_Element<Type>& data_Element, Multilayer* multilayer, bool fit_Mode)
{
//	auto start = std::chrono::system_clock::now();

	const Data& measurement = data_Element.the_Class->measurement;
	Calculated_Values& calculated_Values = data_Element.the_Class->calculated_Values;

	// interpolation
	vector<double>* calculated_Curve = nullptr;
	vector<double>* working_Curve = nullptr;    
    if( data_Element.calc_Functions.check_Reflectance)
    {	calculated_Curve = &calculated_Values.R; calculated_Values.R_Instrumental = calculated_Values.R; working_Curve = &calculated_Values.R_Instrumental;}
    if( data_Element.calc_Functions.check_Transmittance)
    {	calculated_Curve = &calculated_Values.T; calculated_Values.T_Instrumental = calculated_Values.T; working_Curve = &calculated_Values.T_Instrumental;}
    if( data_Element.calc_Functions.check_Scattering)
	{	calculated_Curve = &calculated_Values.S; calculated_Values.S_Instrumental = calculated_Values.S; working_Curve = &calculated_Values.S_Instrumental;}

    // crutch
    if( measurement.measurement_Type != measurement_Types[GISAS_Map] ) {
        if(!calculated_Curve || !working_Curve)
            return;
        if(calculated_Curve->size() ==0 || working_Curve->size() == 0)
            return;
    }

	/// Specular_Scan
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// spectral distribution
			if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON && !measurement.spectral_Distribution.use_Sampling)	{
				wrap_Curve(measurement, measurement.beam_Theta_0_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_From_Spectral_Vec, working_Curve, measurement.spectral_Distribution.distribution_Function);
				*calculated_Curve = *working_Curve;
			}
			// theta_0 distribution
			if(measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON)		{
				wrap_Curve(measurement, measurement.beam_Theta_0_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_Vec, working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function);
			}
		}
		if( measurement.argument_Type  == argument_Types[Wavelength_Energy] )
		{
			// spectral distribution
			if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON)	{
				wrap_Curve(measurement, measurement.lambda_Vec, calculated_Curve, measurement.spectral_Resolution_Vec, working_Curve, measurement.spectral_Distribution.distribution_Function);
				*calculated_Curve = *working_Curve;
			}
			// theta_0 distribution
			if((measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON) && !measurement.beam_Theta_0_Distribution.use_Sampling)		{
				wrap_Curve(measurement, measurement.lambda_Vec, calculated_Curve, measurement.spectral_Resolution_From_Theta_0_Vec, working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function);
			}
		}
	}
	/// Rocking_Curve
	/// Offset_Scan
	if( measurement.measurement_Type == measurement_Types[Offset_Scan]   ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		if(multilayer->imperfections_Model.use_Roughness || multilayer->imperfections_Model.use_Particles)
		{
			/// first of all, remove singularities in specular direction
			// detector theta
			if(measurement.theta_Resolution_FWHM>DBL_EPSILON)		{
				for(size_t point_Index=0; point_Index<working_Curve->size(); ++point_Index)
				{
					(*working_Curve)[point_Index] *= measurement.detector_Factor_for_Intensity_Integration[point_Index];
	//				(*working_Curve)[point_Index] *= qDegreesToRadians(measurement.theta_Resolution_FWHM);
				}
				*calculated_Curve = *working_Curve;
			}

			// spectral distribution
			if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON && !measurement.spectral_Distribution.use_Sampling)	{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_Curve(measurement, measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_From_Spectral_Vec, working_Curve, measurement.spectral_Distribution.distribution_Function);
					*calculated_Curve = *working_Curve;
				}
			}
			// theta_0 distribution
			if((measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON) && !measurement.beam_Theta_0_Distribution.use_Sampling)		{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_Curve(measurement, measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_Vec_Rocking_Offset, working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function);
	//				wrap_Curve(measurement, measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_Vec,				   working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function);
					*calculated_Curve = *working_Curve;
				}
			}
			// no beam spot theta_0 for rocking and offset scans

			// detector theta
			if(measurement.theta_Resolution_FWHM>DBL_EPSILON)		{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_Curve(measurement, measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_Resolution_Vec, working_Curve, measurement.theta_Distribution, false, true);
				}
			}
		}
		// specular peak
		if(measurement.theta_Resolution_FWHM>DBL_EPSILON)		{
			if(data_Element.calc_Functions.add_Specular_Peak)
			{
				wrap_With_Specular(calculated_Values, measurement);
//				wrap_With_Specular_Vector(calculated_Values, measurement);
			}
		}
	}
	/// Detector_Scan
	if( measurement.measurement_Type == measurement_Types[Detector_Scan])
	{
		if(multilayer->imperfections_Model.use_Roughness || multilayer->imperfections_Model.use_Particles)
		{
			/// first of all, remove singularities in specular direction
			// detector theta
			if(measurement.theta_Resolution_FWHM>DBL_EPSILON)		{
				for(size_t point_Index=0; point_Index<working_Curve->size(); ++point_Index)
				{
					(*working_Curve)[point_Index] *= measurement.detector_Factor_for_Intensity_Integration[point_Index];
//					(*working_Curve)[point_Index] *= qDegreesToRadians(measurement.theta_Resolution_FWHM);
				}
				*calculated_Curve = *working_Curve;
			}
			// spectral distribution
			if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON && !measurement.spectral_Distribution.use_Sampling)	{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_Curve(measurement, measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_From_Spectral_Vec, working_Curve, measurement.spectral_Distribution.distribution_Function);
					*calculated_Curve = *working_Curve;
				}
			}
			// theta_0 distribution
			if((measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON) && !measurement.beam_Theta_0_Distribution.use_Sampling)	{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_Curve(measurement, measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_Vec, working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function);
					*calculated_Curve = *working_Curve;
				}
			}
			// beam spot theta_0
			if(measurement.beam_Geometry.size>DBL_EPSILON || (measurement.beam_Geometry.wings_Full_Width>DBL_EPSILON && measurement.beam_Geometry.wings_Intensity>DBL_EPSILON))		{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_Curve(measurement, measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_Vec, working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function, true);
					*calculated_Curve = *working_Curve;
				}
			}
			// detector theta
			if(measurement.theta_Resolution_FWHM>DBL_EPSILON)		{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_Curve(measurement, measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_Resolution_Vec, working_Curve, measurement.theta_Distribution, false, true);
				}
			}
		}
		// specular peak
		if(measurement.theta_Resolution_FWHM>DBL_EPSILON)		{
			if(data_Element.calc_Functions.add_Specular_Peak)
			{
				wrap_With_Specular(calculated_Values, measurement);
//				wrap_With_Specular_Single(calculated_Values, measurement);
			}
		}
	}
	/// GISAS_Map
	if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		bool recalculated = false;
		if(multilayer->imperfections_Model.use_Roughness || multilayer->imperfections_Model.use_Particles)
		{
			// spectral distribution
			if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON && !measurement.spectral_Distribution.use_Sampling)	{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_2D_Curve(measurement, calculated_Values, measurement.theta_0_Resolution_From_Spectral_Vec, measurement.spectral_Distribution.distribution_Function, "theta");
					calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
					recalculated = true;
				}
			}
			// theta_0 distribution
			if((measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON) && !measurement.beam_Theta_0_Distribution.use_Sampling)		{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_2D_Curve(measurement, calculated_Values, measurement.theta_0_Resolution_Vec, measurement.beam_Theta_0_Distribution.distribution_Function, "theta");
					calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
					recalculated = true;
				}
			}
			// beam spot theta_0
			if(measurement.beam_Geometry.size>DBL_EPSILON || (measurement.beam_Geometry.wings_Full_Width>DBL_EPSILON && measurement.beam_Geometry.wings_Intensity>DBL_EPSILON))		{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_2D_Curve(measurement, calculated_Values, measurement.theta_0_Resolution_Vec, measurement.beam_Theta_0_Distribution.distribution_Function, "theta", true);
					calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
					recalculated = true;
				}
			}
			// beam spot phi
			if(measurement.beam_Geometry.lateral_Width>DBL_EPSILON)	{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_2D_Curve(measurement, calculated_Values, measurement.phi_Beam_Spot_Size_Vec, distributions[Gate], "phi");
//					if(measurement.beam_Geometry.log_Profile_Plot) {
//						wrap_2D_Curve(measurement, calculated_Values, measurement.phi_Beam_Spot_Size_Vec, "Short Lorentz", "phi");
//					} else {
//						wrap_2D_Curve(measurement, calculated_Values, measurement.phi_Beam_Spot_Size_Vec, distributions[Gaussian], "phi");
//					}
					calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
					recalculated = true;
				}
			}
			// detector phi
			if(measurement.phi_Resolution_FWHM>DBL_EPSILON || measurement.beam_Phi_0_Distribution.FWHM_distribution>DBL_EPSILON)		{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_2D_Curve(measurement, calculated_Values, measurement.phi_Resolution_Vec, measurement.phi_Distribution, "phi");
					calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
					recalculated = true;
				}
			}

			// detector theta
			if(measurement.theta_Resolution_FWHM>DBL_EPSILON)	{
				if(data_Element.calc_Functions.instrumental_Smoothing)	{
					wrap_2D_Curve(measurement, calculated_Values, measurement.theta_Resolution_Vec, measurement.theta_Distribution, "theta");
				}

				// multiply on detector size to get real intensity
				if(measurement.phi_Resolution_FWHM>DBL_EPSILON)
				{
	//				Global_Variables::parallel_For(calculated_Values.GISAS_Map.front().size(), reflectivity_calc_threads, [&](int n_Min, int n_Max, int thread_Index)
	//				{
	//					for(int theta_Index=n_Min; theta_Index<n_Max; ++theta_Index)
						for(size_t theta_Index=0; theta_Index<calculated_Values.GISAS_Map.front().size(); ++theta_Index)
						{
//							double additional_Background = 1.5*double(calculated_Values.GISAS_Map.front().size()-theta_Index-1)/(calculated_Values.GISAS_Map.front().size()-1)*measurement.background;
							for(size_t phi_Index=0; phi_Index<calculated_Values.GISAS_Map.size(); ++phi_Index)
							{
								calculated_Values.GISAS_Instrumental[phi_Index][theta_Index] = calculated_Values.GISAS_Map[phi_Index][theta_Index]*
																									qDegreesToRadians(measurement.theta_Resolution_FWHM)*
																									qDegreesToRadians(measurement.phi_Resolution_FWHM)*
																									measurement.detector_Theta_Cos_Vec[theta_Index] +
																									measurement.background;// + additional_Background;
							}
						}
	//				});
				}
				recalculated = true;
			}
		} else
		// set to zero
		{
			calculated_Values.GISAS_Instrumental = calculated_Values.GISAS_Map;
		}
		// specular peak
		if(measurement.theta_Resolution_FWHM>DBL_EPSILON)		{
			if(data_Element.calc_Functions.add_Specular_Peak)
			{
//				auto start = std::chrono::system_clock::now();
				wrap_With_Specular_2D(calculated_Values, measurement);
//				auto end = std::chrono::system_clock::now();
//				auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//				qInfo() << "specular peak:    "<< elapsed.count()/1000000. << " seconds" << endl << endl;
			}
			recalculated = true;
		}
		if(!recalculated) calculated_Values.GISAS_Instrumental = calculated_Values.GISAS_Map;
	}

	/// FOOTPRINT
	// footprint for R
	if(	measurement.measurement_Type == measurement_Types[Specular_Scan] &&
	   (data_Element.calc_Functions.check_Reflectance ||
		data_Element.calc_Functions.check_Absorptance) )
	{
		for(size_t point_Index=0; point_Index<calculated_Values.R_Instrumental.size(); ++point_Index)
		{
			calculated_Values.R_Instrumental[point_Index] *= measurement.footprint_Factor_Vec[point_Index];
		}
	}
	// footprint for scattering. specular peak is preliminary divided by footprint
	if(	data_Element.calc_Functions.check_Scattering)
	{
		for(size_t point_Index=0; point_Index<calculated_Values.S_Instrumental.size(); ++point_Index)
		{
			calculated_Values.S_Instrumental[point_Index] *= measurement.footprint_Factor_Vec[point_Index];
		}
	}
	// footprint for GISAS is done in unwrapped_reflection

	/// BACKGROUND
	if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		// background for R
		if( data_Element.calc_Functions.check_Reflectance)	{
			for(size_t point_Index=0; point_Index<calculated_Values.R_Instrumental.size(); ++point_Index)	{
				calculated_Values.R_Instrumental[point_Index] += measurement.background;
			}
		}
		// background for T
		if( data_Element.calc_Functions.check_Transmittance)	{
			for(size_t point_Index=0; point_Index<calculated_Values.T_Instrumental.size(); ++point_Index)	{
				calculated_Values.T_Instrumental[point_Index] += measurement.background;
			}
		}
	} else
	{
		// background for scattering
		if( data_Element.calc_Functions.check_Scattering)	{
			for(size_t point_Index=0; point_Index<calculated_Values.S_Instrumental.size(); ++point_Index)	{
				calculated_Values.S_Instrumental[point_Index] += measurement.background;
			}
		}
		// background for GISAS is done in unwrapped_reflection
	}

	// mesh density
	if(data_Element.curve_Class == TARGET)
	{
		Target_Curve* target_Curve = qobject_cast<Target_Curve*>(data_Element.the_Class);
		if(target_Curve->curve.mesh_Density_Factor>1)
		{
			decrease_Mesh_Density(target_Curve, fit_Mode);
		}
	}
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//	qInfo() << "postprocessing:    "<< elapsed.count()/1000000. << " seconds" << endl << endl;
}
template void Main_Calculation_Module::postprocessing<Independent_Curve>(Data_Element<Independent_Curve>&, Multilayer*, bool);
template void Main_Calculation_Module::postprocessing<Target_Curve>     (Data_Element<Target_Curve>&, Multilayer*, bool);

void Main_Calculation_Module::fitting_and_Confidence()
{
	if(calc_Mode!=FITTING && calc_Mode!=CONFIDENCE)
	{
		QMessageBox::critical(nullptr, "Main_Calculation_Module::fitting", "wrong calc_Mode");
		return;
	}

	// prepare measurement vectors and fill calc_Trees
	preliminary_Calculation();

	// reload dependences
	if(!global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
	{
		global_Multilayer_Approach->table_Of_Structures = new Table_Of_Structures(true);
		global_Multilayer_Approach->table_Of_Structures->close();
	}

	save_Init_State_Trees();

	confidentials.clear_All();
	confidentials_Rejected_Thicknesses_and_Periods.clear_All();
	confidentials_Rejected_Periods.clear_All();

	fitables.clear_All();
	fit_Rejected_Sigmas.clear_All();
	fit_Rejected_Min_Max.clear_All();
	fit_Rejected_Thicknesses_and_Periods.clear_All();
	fit_Rejected_Periods.clear_All();

	// checking for lambda_Out_Of_Range
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
//		calculation_Trees[tab_Index]->fill_Calc_Trees();
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
			calculation_With_Sampling(calculation_Trees[tab_Index], target_Element);
			postprocessing(target_Element, multilayers[tab_Index], true);
			if(lambda_Out_Of_Range)	return;
		}
	}

	number_Of_Restricted_Regular_Components = 0;
	/// create calc tree, fitables and confidentials;
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		// prepare real_Calc_Tree (for all multilayers!)
//		calculation_Trees[tab_Index]->fill_Tree_From_Scratch(calculation_Trees[tab_Index]->real_Calc_Tree, calculation_Trees[tab_Index]->real_Struct_Tree, multilayers[tab_Index]);

		if( calculation_Trees[tab_Index]->target.size()>0 )
		{
			// find fitables over tree
			calc_Tree_Iteration(calculation_Trees[tab_Index]->real_Calc_Tree.begin(), tab_Index);
		}

		// additional fitables: target curve scale factors
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
			Parameter* parameter = &(target_Element.the_Class->curve.val_Factor);
			QString total_Name = "  " + Medium_BlackCircle_Sym + "  <" + multilayer_Tabs->tabText(tab_Index) + "> " + parameter->indicator.full_Name;

			if(parameter->fit.is_Fitable && !target_Element.the_Class->fit_Params.maximize_Integral)
			{
				// fixed
				fitables.struct_Names 		.push_back(multilayer_Tabs->tabText(parameter->indicator.tab_Index));
				fitables.param_Names		.push_back(parameter->indicator.full_Name);
				fitables.param_IDs			.push_back(parameter->indicator.id);

				// changeable
				fitables.param_Pointers		.push_back(parameter);
				fitables.values_Parametrized.push_back(parametrize(parameter->value, parameter->fit.min, parameter->fit.max)); // will be recalculated at solver initialization
				tree<Node>::iterator empty_Iterator;
				fitables.parent_Iterators	.push_back(empty_Iterator);					// used for period and gamma only, but should be filled for all for the length purpose!

				/// for rejection

				// min>=max
				if(parameter->fit.min>=parameter->fit.max)
				{
					fit_Rejected_Min_Max.param_Names.push_back(total_Name);
				}
			}
		}
	}

	/// fill pointers to slaves
	// starting from fitable top-masters
	for(Parameter* parameter : fitables.param_Pointers)	{
		slaves_Pointer_Iteration(parameter);
	}
	// starting from confidentials
	for(Parameter* parameter : confidentials.param_Pointers) {
		slaves_Pointer_Iteration(parameter);
	}

	// not used
	/// prepare expressions for slaves, starting from fitable top-masters
#ifdef EXPRTK
	expression_Vec.clear();
	argument_Vec.clear();
	slaves_Expression_Map.clear();
	for(Parameter* parameter : fitables.param_Pointers)
	{
		slaves_Expression_Iteration(parameter);
	}
#endif

	/// rejection
	if(reject()) return;

	/// --------------------------------------------------------------------------------------------------------------
	/// fitting from here
	/// --------------------------------------------------------------------------------------------------------------
	if( calc_Mode == FITTING )
	if( fitables.param_Pointers.size()>0 )
	{
		Fitting fitting_Instance(this);
		fitting_Instance.check_Maximization();
		bool is_Load_Init_State_Trees = false;
		bool go = fitting_Instance.fit();
		if(!go) return;
		print_Calculated_To_File();

		// replace the initial parameters
		QMessageBox::StandardButton reply = QMessageBox::question(nullptr,"Replace", "Fitting is done.\nDo you want to replace the parameters?",
																  QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
		if (reply == QMessageBox::Yes)
		{
			// for single fitting
			if(!global_Multilayer_Approach->fitting_Settings->randomized_Start)
			{
				renew_Item_Trees();
				add_Fit(after_Fitting_State);
			}

			// it also refreshs Independent tree copies
			global_Multilayer_Approach->refresh_All_Multilayers_View();
		} else
		{
			is_Load_Init_State_Trees = true;
		}

		// close item editors
		global_Multilayer_Approach->close_Item_Editors();

		// close aperiodic tables
		global_Multilayer_Approach->temporarily_Close_Regular_Aperiodics();

		// refresh table and aperiodics (anyway)
		if(global_Multilayer_Approach->runned_Tables_Of_Structures.contains(table_Of_Structures_Key))
		{
			// close table
			int active_Tab = global_Multilayer_Approach->table_Of_Structures->main_Tabs->currentIndex();
			global_Multilayer_Approach->table_Of_Structures->close();

			if(is_Load_Init_State_Trees) load_Init_State_Trees();

			// reopen table
			global_Multilayer_Approach->open_Table_Of_Structures();
			global_Multilayer_Approach->table_Of_Structures->main_Tabs->setCurrentIndex(active_Tab);

		} else
		{
			if(is_Load_Init_State_Trees) load_Init_State_Trees();
		}

		// reopen aperiodic tables
		global_Multilayer_Approach->reopen_Regular_Aperiodics();
	}
	/// --------------------------------------------------------------------------------------------------------------
	/// confidence intervals from here
	/// --------------------------------------------------------------------------------------------------------------
	if( calc_Mode == CONFIDENCE )
	if( confidentials.param_Pointers.size()>0 )
	{
		Fitables temp_Full_Fitables = fitables; // save full set, including (possibly) active confidentials
		for(uint confidence_Index=0; confidence_Index<confidentials.param_Pointers.size(); ++confidence_Index)
		{
			confidentials.param_Pointers[confidence_Index]->confidence.is_Active = true;
			fitables = temp_Full_Fitables;

			// get index of fitable with the same ID
			int corresponding_Index = fitables.param_IDs.indexOf(confidentials.param_IDs[confidence_Index]);

			// shortening fitables
			if(corresponding_Index>=0)
			{
				fitables.struct_Names.		 erase(fitables.struct_Names.begin() + corresponding_Index);
				fitables.param_Names.		 erase(fitables.param_Names.begin() + corresponding_Index);
				fitables.param_IDs.			 remove(corresponding_Index);

				fitables.param_Pointers.	 erase(fitables.param_Pointers.begin() + corresponding_Index);
				fitables.values_Parametrized.erase(fitables.values_Parametrized.begin() + corresponding_Index);
				fitables.parent_Iterators.	 erase(fitables.parent_Iterators.begin() + corresponding_Index);
			}

			// backup for restoring values
			vector<double> fitables_Pointers_Value_Backup(fitables.param_Pointers.size());
			for(size_t i=0; i<fitables.param_Pointers.size(); i++)			{
				fitables_Pointers_Value_Backup[i] = fitables.param_Pointers[i]->value;
			}
			vector<double> confidentials_Pointers_Value_Backup(confidentials.param_Pointers.size());
			for(size_t i=0; i<confidentials.param_Pointers.size(); i++)			{
				confidentials_Pointers_Value_Backup[i] = confidentials.param_Pointers[i]->value;
			}

			Fitting fitting_Instance(this);
			fitting_Instance.check_Maximization();
			bool go = fitting_Instance.confidence(fitables_Pointers_Value_Backup, confidentials_Pointers_Value_Backup, confidence_Index);
			confidentials.param_Pointers[confidence_Index]->confidence.is_Active = false;
			if(!go) return;
		}
	}
	/// --------------------------------------------------------------------------------------------------------------
}

void Main_Calculation_Module::save_Init_State_Trees()
{
	for(int tree_Index=0; tree_Index<calculation_Trees.size(); ++tree_Index)
	{
		copy_Real_Struct_Trees[tree_Index] = new QTreeWidget(this);
		Global_Variables::copy_Tree(calculation_Trees[tree_Index]->real_Struct_Tree, copy_Real_Struct_Trees[tree_Index]);
	}
}

void Main_Calculation_Module::load_Init_State_Trees()
{
	for(int tree_Index=0; tree_Index<calculation_Trees.size(); ++tree_Index)
	{
		Global_Variables::copy_Tree(copy_Real_Struct_Trees[tree_Index], calculation_Trees[tree_Index]->real_Struct_Tree);
		calculation_Trees[tree_Index]->real_Struct_Tree->expandAll();
	}
}

void Main_Calculation_Module::renew_Item_Trees()
{
	for(Calculation_Tree* calculation_Tree : calculation_Trees)
	{
		calculation_Tree->renew_Item_Tree_From_Calc_Tree(calculation_Tree->real_Calc_Tree.begin(),
														 calculation_Tree->real_Calc_Tree,
														 calculation_Tree->real_Struct_Tree->invisibleRootItem());
	}
}

bool Main_Calculation_Module::reject()
{
	/// fit reject

	// sigma value is close to zero
	if(fit_Rejected_Sigmas.param_IDs.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(fit_Rejected_Sigmas.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables", "Sigma start values are too small in:\n\n" +
													   rejected +
													   "\n\nChange them");
		return true;
	}

	// internal thicknesses and periods if overlying period or gamma are fitables
	if(fit_Rejected_Thicknesses_and_Periods.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(fit_Rejected_Thicknesses_and_Periods.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables", "Thicknesses and periods can't be fitted independently\nif overlying period or gamma are fitables:\n\n" +
													   rejected +
													   "\n\nRecheck fitables");
		return true;
	}

	// period value == 0
	if(fit_Rejected_Periods.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(fit_Rejected_Periods.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables", "Zero period values are not allowed for fitting:\n\n" +
													   rejected +
													   "\n\nIncrease them");
		return true;
	}

	// min>=max
	if(fit_Rejected_Min_Max.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(fit_Rejected_Min_Max.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables", "min>=max for the following fitables:\n\n" +
													   rejected +
													   "\n\nThey won't be fitted");
		return true;
	}

	/// confidence reject

	// fitiing or calculating interval for internal thicknesses and periods if overlying period or gamma are fitables or confidentials
	if(confidentials_Rejected_Thicknesses_and_Periods.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(confidentials_Rejected_Thicknesses_and_Periods.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad fitables/confidentials", "Fit or confidence interval calculation is not possible\nfor internal thicknesses and periods if overlying\nperiod or gamma are fitables of confidentials:\n\n" +
													   rejected +
													   "\n\nRecheck fitables/confidentials");
		return true;
	}

	// one of period values in interval == 0
	if(confidentials_Rejected_Periods.param_Names.size()>0)
	{
		QStringList rejected_List = QStringList::fromVector(QVector<QString>::fromStdVector(confidentials_Rejected_Periods.param_Names));
		QString rejected = rejected_List.join("\n");
		QMessageBox::information(nullptr, "Bad confidentials", "Zero period values are not allowed\nfor calculation confidence interval:\n\n" +
													   rejected +
													   "\n\nChange min/max");
		return true;
	}

	return false;
}

void Main_Calculation_Module::calc_Tree_Iteration(const tree<Node>::iterator& parent, int tab_Index, bool fitables_Period_Gamma, bool confidentials_Period_Gamma)
{
	Data& parent_Data = parent.node->data.struct_Data;

	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& struct_Data = child.node->data.struct_Data;

		// check if last layer
//		bool last_Layer = false;
//		tree<Node>::pre_order_iterator next_child = child;
//		next_child++;
//		Data& next_Struct_Data = next_child.node->data.struct_Data;
//		if(next_Struct_Data.item_Type == item_Type_Substrate)
//		{
//			last_Layer = true;
//		}
		find_Fittable_Confidence_Parameters(struct_Data, parent_Data, child, i, tab_Index, fitables_Period_Gamma, confidentials_Period_Gamma/*, last_Layer*/);

		// get number_Of_Restricted_Regular_Components to enlarge number of effective residual points
		if( struct_Data.item_Type == item_Type_Regular_Aperiodic )
		{
			for(int k=0; k<struct_Data.regular_Components.size(); k++)
			{
				if(struct_Data.regular_Components[k].use_Soft_Restrictions) { ++number_Of_Restricted_Regular_Components; }
			}
		}

		// check period and gamma
		if( struct_Data.item_Type == item_Type_Multilayer ||
			struct_Data.item_Type == item_Type_Regular_Aperiodic ||
			struct_Data.item_Type == item_Type_General_Aperiodic )
		{
			// check if period or gamma are fitables
			bool local_Fitables_Period_Gamma = fitables_Period_Gamma;
			if(struct_Data.period.fit.is_Fitable || (struct_Data.gamma.fit.is_Fitable && child.number_of_children() == 2))
			{
				local_Fitables_Period_Gamma = true;
			}

			// check if period or gamma are confidentials
			bool local_Confidentials_Period_Gamma = confidentials_Period_Gamma;
			if(struct_Data.period.confidence.calc_Conf_Interval || (struct_Data.gamma.confidence.calc_Conf_Interval && child.number_of_children() == 2))
			{
				local_Confidentials_Period_Gamma = true;
			}
			calc_Tree_Iteration(child, tab_Index, local_Fitables_Period_Gamma, local_Confidentials_Period_Gamma);
		}
	}
}

void Main_Calculation_Module::find_Fittable_Confidence_Parameters(Data& struct_Data, const Data& parent_Data, const tree<Node>::iterator& current, int child_Index, int tab_Index, bool fitables_Period_Gamma, bool confidentials_Period_Gamma/*, bool last_Layer*/)
{
	struct_Data.fill_Potentially_Fitable_Parameters_Vector(multilayers[tab_Index]->imperfections_Model/*, last_Layer*/);

	for(Parameter* parameter : struct_Data.potentially_Fitable_Parameters)
	{
		int index = Global_Variables::get_Index_For_Parameter_Name(struct_Data, *parameter);
		parameter->indicator.full_Name = Global_Variables::parameter_Name(struct_Data, parameter->indicator.whats_This, index);
		// for my_Sigmas
		if( parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma_Diffuse && !struct_Data.common_Sigma_Diffuse )
		{	parameter->fit.is_Fitable = struct_Data.sigma_Diffuse.fit.is_Fitable; }

		QString total_Name = "  " + Medium_BlackCircle_Sym + "  <" + multilayer_Tabs->tabText(parameter->indicator.tab_Index) + "> " + parameter->indicator.full_Name;

		// fitable and has no master
		if(parameter->fit.is_Fitable && !parameter->coupled.master.exist)
//		if(calc_Mode != CONFIDENCE || !parameter->confidence.calc_Conf_Interval) // only 1 confidential should be treated as non-fitable in CONFIDENCE mode at the same time
		{
			bool go = true;

			// for regular aperiodic: don't fit thickness/sigma of item-layer if not common thickness/sigma
			if(parent_Data.item_Type == item_Type_Regular_Aperiodic)
			{
				if(!parent_Data.regular_Components[child_Index].is_Common_Thickness && parameter->indicator.whats_This == whats_This_Thickness)		{ go = false; }
				if(!parent_Data.regular_Components[child_Index].is_Common_Sigma     && parameter->indicator.whats_This == whats_This_Sigma_Diffuse) { go = false; }
			}

			if(	parameter->indicator.whats_This == whats_This_Gamma && struct_Data.num_Repetition.value() != 2)	{go = false;}

			if(go)
			{
				// fixed
				fitables.struct_Names 		.push_back(multilayer_Tabs->tabText(parameter->indicator.tab_Index));
				fitables.param_Names		.push_back(parameter->indicator.full_Name);
				fitables.param_IDs			.push_back(parameter->indicator.id);

				// changeable
				fitables.param_Pointers		.push_back(parameter);
				fitables.values_Parametrized.push_back(parametrize(parameter->value, parameter->fit.min, parameter->fit.max)); // will be recalculated at solver initialization
				fitables.parent_Iterators	.push_back(current);					// used for period and gamma only, but should be filled for all for the length purpose!

//				qInfo() << parameter->indicator.full_Name << endl;
				/// for rejection

				// sigma value is close to zero. Roughness_Peak_Sigma is not checked
				Parameter sigma_Parameter;
				if(parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma_Diffuse) sigma_Parameter = struct_Data.sigma_Diffuse;
				if(parameter->indicator.whats_This == whats_This_Sigma_Diffuse)				  sigma_Parameter = *parameter;
				if(parameter->indicator.whats_This == whats_This_Sigma_Roughness)			  sigma_Parameter = *parameter;

				if( parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma_Diffuse ||
					parameter->indicator.whats_This == whats_This_Sigma_Diffuse ||
					parameter->indicator.whats_This == whats_This_Sigma_Roughness)
				if(sigma_Parameter.value < 0.1) {
					if(!fit_Rejected_Sigmas.param_IDs.contains(sigma_Parameter.indicator.id))
					{
						fit_Rejected_Sigmas.param_IDs.push_back(sigma_Parameter.indicator.id);
						fit_Rejected_Sigmas.param_Names.push_back("  " + Medium_BlackCircle_Sym + "  <" + multilayer_Tabs->tabText(sigma_Parameter.indicator.tab_Index) + "> " + sigma_Parameter.indicator.full_Name);
					}
				}

				// forbid fitting internal thicknesses and periods if overlying period or gamma are fitables
				if(fitables_Period_Gamma)
				if(parameter->indicator.whats_This == whats_This_Thickness || parameter->indicator.whats_This == whats_This_Period)
				{
					fit_Rejected_Thicknesses_and_Periods.param_Names.push_back(total_Name);
				}

				// forbid calculating interval for outer period or gamma if inner thickness or period are fitables
				if(confidentials_Period_Gamma)
				if(parameter->indicator.whats_This == whats_This_Thickness || parameter->indicator.whats_This == whats_This_Period)
				{ // qLineEdit.setFixedWidth(TABLE_FIX_WIDTH_LINE_EDIT)
					confidentials_Rejected_Thicknesses_and_Periods.param_Names.push_back(total_Name);
				}

				// period value == 0
				if(parameter->indicator.whats_This == whats_This_Period)
				if(abs(parameter->value) < DBL_MIN)
				{
					fit_Rejected_Periods.param_Names.push_back(total_Name);
				}

				// min>=max
				if(parameter->fit.min>=parameter->fit.max)
				{
					fit_Rejected_Min_Max.param_Names.push_back(total_Name);
				}
			}
		}

		// confidential
		if(calc_Mode == CONFIDENCE && parameter->confidence.calc_Conf_Interval)
		{
			// fixed
			confidentials.struct_Names	  .push_back(multilayer_Tabs->tabText(parameter->indicator.tab_Index));
			confidentials.param_Names	  .push_back(parameter->indicator.full_Name);
			confidentials.param_IDs		  .push_back(parameter->indicator.id);

			// changeable
			confidentials.param_Pointers  .push_back(parameter);
			confidentials.values		  .push_back(parameter->value);
			confidentials.parent_Iterators.push_back(current);		// used for period and gamma only, but should be filled for all for the length purpose!

			/// for rejection

			// forbid calculating interval for internal thicknesses and periods if overlying period or gamma are fitables or confidentials
			if(confidentials_Period_Gamma || fitables_Period_Gamma)
			if(parameter->indicator.whats_This == whats_This_Thickness || parameter->indicator.whats_This == whats_This_Period)
			{
				confidentials_Rejected_Thicknesses_and_Periods.param_Names.push_back(total_Name);
			}

			// one of period values in interval == 0
			if(parameter->indicator.whats_This == whats_This_Period)
			if(abs(parameter->confidence.min) < DBL_MIN || abs(parameter->confidence.max) < DBL_MIN)
			{
				confidentials_Rejected_Periods.param_Names.push_back(total_Name);
			}
		}
	}	
}

void Main_Calculation_Module::slaves_Pointer_Iteration(Parameter* master)
{
	master->coupled.slave_Pointers.clear(); // rewrite

	for(Parameter_Indicator& slave_Parameter_Indicator : master->coupled.slaves)
	{
		Parameter* slave = find_Slave_Pointer_by_Id(slave_Parameter_Indicator);
		// check
		if(slave == nullptr)
		{
			// TODO
//			QMessageBox::critical(nullptr, "Main_Calculation_Module::slaves_Vector_Iteration", slave_Parameter_Indicator.full_Name + "\n\nnot found");
//			exit(EXIT_FAILURE);

			// good way is refresh_Parameters_Connection_Over_Trees() in Multilayer_Approach. Not sure.
			master->coupled.slaves.removeAll(slave_Parameter_Indicator);
		} else
		{
			master->coupled.slave_Pointers.append(slave);
			slaves_Pointer_Iteration(slave);
		}
	}
}

void Main_Calculation_Module::slaves_Expression_Iteration(Parameter* master)
{
	if(master->coupled.slaves.size() != master->coupled.slave_Pointers.size())
	{
		QMessageBox::critical(nullptr, "Main_Calculation_Module::slaves_Expression_Iteration", "slaves.size() != slave_Pointers.size()");
		exit(EXIT_FAILURE);
	}
	for(int slave_Index=0; slave_Index<master->coupled.slaves.size(); ++slave_Index)
	{
#ifdef EXPRTK
		Parameter_Indicator& slave_Parameter_Indicator = master->coupled.slaves[slave_Index];
#endif
		Parameter* slave = master->coupled.slave_Pointers[slave_Index];

#ifdef EXPRTK
		exprtk::parser<double> parser;
		exprtk::symbol_table<double> symbol_table;

		expression_Vec.append(exprtk::expression<double>());

		argument_Vec.append(&master->value);
		slaves_Expression_Map.insert(slave->indicator.id, expression_Vec.size()-1);

		symbol_table.add_variable(expression_Master_Slave_Variable, *argument_Vec.last());
		symbol_table.add_constants();

		expression_Vec.last().register_symbol_table(symbol_table);
		parser.compile(slave_Parameter_Indicator.expression.toStdString(), expression_Vec.last());
#endif
		slaves_Expression_Iteration(slave);
	}
}

Parameter* Main_Calculation_Module::find_Slave_Pointer_by_Id(const Parameter_Indicator& slave_Parameter_Indicator)
{
	Parameter* pointer = nullptr;
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		find_Slave_Pointer_Calc_Tree_Iteration(calculation_Trees[tab_Index]->real_Calc_Tree.begin(), slave_Parameter_Indicator, pointer);
		if(pointer != nullptr)
		{
			return pointer;
		}
	}
	return pointer;
}

void Main_Calculation_Module::find_Slave_Pointer_Calc_Tree_Iteration(const tree<Node>::iterator& parent, const Parameter_Indicator& slave_Parameter_Indicator, Parameter*& pointer)
{
	// iterate over tree
	for(unsigned i=0; i<parent.number_of_children(); ++i)
	{
		tree<Node>::pre_order_iterator child = tree<Node>::child(parent,i);
		Data& struct_Data = child.node->data.struct_Data;

		if(struct_Data.id == slave_Parameter_Indicator.item_Id)
		{
			pointer = Global_Variables::get_Parameter_From_Struct_Item_by_Id(struct_Data, slave_Parameter_Indicator.id);
		}
		if(struct_Data.item_Type == item_Type_Regular_Aperiodic)
		{
			for(int k=0; k<struct_Data.regular_Components.size(); k++)
			{
				for(int n=0; n<struct_Data.num_Repetition.value(); n++)
				{
					Data& regular_Data = struct_Data.regular_Components[k].components[n];
					if(regular_Data.id == slave_Parameter_Indicator.item_Id)
					{
						pointer = Global_Variables::get_Parameter_From_Struct_Item_by_Id(regular_Data, slave_Parameter_Indicator.id);
					}
				}
			}
		}
		if( struct_Data.item_Type == item_Type_Multilayer ||
			struct_Data.item_Type == item_Type_General_Aperiodic ||
			struct_Data.item_Type == item_Type_Regular_Aperiodic ) // regular aperiodic here too
		{
			find_Slave_Pointer_Calc_Tree_Iteration(child, slave_Parameter_Indicator, pointer);
		}
	}
}

double Main_Calculation_Module::triangle_Wave(double x)
{
	return abs(x - 2*floor((x + 1)/2));
}

double Main_Calculation_Module::parametrize(double value, double min, double max)
{
	if(parametrization_Type == noparam)
	{
		return value;
	} else
	if(parametrization_Type == triangle)
	{
		return (value - min) / (max - min);
	} else
	{
		QMessageBox::warning(nullptr, "Main_Calculation_Module::parametrize", "Wrong parametrization_Type");
		return value;
	}
}

double Main_Calculation_Module::unparametrize(double parametrized_Shifted_Value, double min, double max)
{
	if(parametrization_Type == noparam)
	{
		return parametrized_Shifted_Value;
	} else
	if(parametrization_Type == triangle)
	{
		return triangle_Wave(parametrized_Shifted_Value) * (max - min) + min;
	} else
	{
		QMessageBox::warning(nullptr, "Main_Calculation_Module::unparametrize", "Wrong parametrization_Type");
		return parametrized_Shifted_Value;
	}
}

void Main_Calculation_Module::print_PSD_1D_To_File(Data_Element<Target_Curve>& data_Element, QString struct_Name, int index)
{
    // point as decimal separator
    QLocale tempLoc = QLocale::c();

    const Data& measurement = data_Element.the_Class->measurement;

	if(!print_1D_PSD_From_Scattering_On_Recalculation) return;
	if(	measurement.measurement_Type != measurement_Types[Detector_Scan] &&
		measurement.measurement_Type != measurement_Types[Rocking_Curve] &&
		measurement.measurement_Type != measurement_Types[Offset_Scan] ) return;

    QString first_Name = struct_Name + "_PSD_1D_" + data_Element.the_Class->index/*tempLoc.toString(index+1)*/ + "_" + data_Element.the_Class->name;

	QString specular_Substraction_Filename = "";
	QString specular_Substraction = "";
	if(data_Element.calc_Functions.add_Specular_Peak)
	{
		specular_Substraction_Filename = "_spec_substracted";
		specular_Substraction = ", specular peak substracted from measurement data before PSD calculation";
	}

    QString path = Global_Variables::working_Directory() + "/";

	int prec = 10;

	double angular_Coeff = angle_Coefficients_Map.value(data_Element.the_Class->angular_Units);
	double spectral_Coeff = wavelength_Coefficients_Map.value(data_Element.the_Class->spectral_Units);
	QString angular_Units_Name = angle_Units_Legend_Map.value(data_Element.the_Class->angular_Units);
	QString spectral_Units_Name = wavelength_Units_Legend_Map.value(data_Element.the_Class->spectral_Units);

	double arg_Coeff = spatial_Frequency_Coefficients_Map.value(spatial_frequency_units_export);
	double val_Coeff = PSD_1D_Value_Coefficients_Map.value(PSD_1D_value_units);
	QString arg_Units_Name = spatial_Frequency_Units_Legend_Map.value(spatial_frequency_units_export);
	QString val_Units_Name = PSD_1D_Value_Units_Legend_Map.value(PSD_1D_value_units);

	QString at_Fixed_Heading = Global_Variables::wavelength_Energy_Name(data_Element.the_Class->spectral_Units, true) + " = " +
                               tempLoc.toString(Global_Variables::wavelength_Energy(data_Element.the_Class->spectral_Units, measurement.lambda_Value)/spectral_Coeff, 'g', prec) + " " +
							   spectral_Units_Name;
    QString instrumental_Heading =  "spectral resolution (FWHM) = " + tempLoc.toString(measurement.spectral_Distribution.FWHM_distribution, 'g', prec) +
                                    "\n; beam divergence (FWHM) = " + tempLoc.toString(measurement.beam_Theta_0_Distribution.FWHM_distribution/angular_Coeff, 'g', prec) + " " + angular_Units_Name;
    QString geometry_Heading =  "beam width = " + tempLoc.toString(measurement.beam_Geometry.size, 'g', prec) + " mm" +
                                "\n; sample size = " + tempLoc.toString(measurement.sample_Geometry.size, 'g', prec) + " mm";

    QString detector_Width_Heading = "\n; detector angular width (FWHM) = " + tempLoc.toString(measurement.theta_Resolution_FWHM/angular_Coeff, 'g', prec) + " " + angular_Units_Name;

	if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
        at_Fixed_Heading += "\n; beam grazing angle = " + tempLoc.toString(measurement.beam_Theta_0_Angle_Value/angular_Coeff, 'g', prec) + " " + angular_Units_Name;
	}
	if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
        at_Fixed_Heading += "\n; specular grazing angle position = " + tempLoc.toString(measurement.beam_Theta_0_Specular_Position/angular_Coeff, 'g', prec) + " " + angular_Units_Name;
	}
	if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
        at_Fixed_Heading += "\n; detector offset from specular direction = " + tempLoc.toString(measurement.detector_Theta_Offset/angular_Coeff, 'g', prec) + " " + angular_Units_Name;
	}
	instrumental_Heading += detector_Width_Heading;
	QString argument_Heading = "Spatial frequency (" + arg_Units_Name + ")";
	QString value_Heading = "PSD (" + val_Units_Name + ")";

	vector<double> argument_Left  = data_Element.the_Class->calculated_Values.PSD_Left_Wing_Argument;
	vector<double> argument_Right = data_Element.the_Class->calculated_Values.PSD_Right_Wing_Argument;
	for(double& arg : argument_Left)	arg = arg/arg_Coeff;
	for(double& arg : argument_Right)	arg = arg/arg_Coeff;
	vector<double> value_Left  = data_Element.the_Class->calculated_Values.PSD_Left_Wing_Value;
	vector<double> value_Right = data_Element.the_Class->calculated_Values.PSD_Right_Wing_Value;
	for(double& val : value_Left)	val = val/val_Coeff;
	for(double& val : value_Right)	val = val/val_Coeff;

	// left wing
	if(argument_Left.size()>0)
	{
		QString name_Left = path + first_Name + "_left" + specular_Substraction_Filename+".txt";
		QFile file_Left(name_Left);
		file_Left.open(QIODevice::WriteOnly);
		QTextStream out_Left(&file_Left);
		out_Left.setFieldAlignment(QTextStream::AlignLeft);

		print_PSD_Data(out_Left,
					   argument_Left,
					   value_Left,
					   data_Element.the_Class->measurement.polarization,
					   "1D PSD for theta<theta_0 from "+data_Element.the_Class->measurement.measurement_Type+specular_Substraction,
					   argument_Heading,
					   value_Heading,
					   at_Fixed_Heading,
					   instrumental_Heading,
					   geometry_Heading);
		file_Left.close();
	}

	// right wing
	if(argument_Right.size()>0)
	{
		QString name_Right = path + first_Name + "_right" + specular_Substraction_Filename+".txt";
		QFile file_Right(name_Right);
		file_Right.open(QIODevice::WriteOnly);
		QTextStream out_Right(&file_Right);
		out_Right.setFieldAlignment(QTextStream::AlignLeft);

		print_PSD_Data(out_Right,
					   argument_Right,
					   value_Right,
					   data_Element.the_Class->measurement.polarization,
					   "1D PSD for theta>theta_0 from "+data_Element.the_Class->measurement.measurement_Type+specular_Substraction,
					   argument_Heading,
					   value_Heading,
					   at_Fixed_Heading,
					   instrumental_Heading,
					   geometry_Heading);
		file_Right.close();
	}
}

void Main_Calculation_Module::print_PSD_Data(QTextStream& out, vector<double>& arg, vector<double>& val, double incident_Polarization, QString data_Type, QString argument_Heading, QString value_Heading, QString at_Fixed_Heading, QString instrumental_Heading, QString geometry_Heading)
{
    // point as decimal separator
    QLocale tempLoc = QLocale::c();

	// headline
	int precision_Arg = 6;
	int precision_PSD = 6;

	int arg_Shift = 3;
	int width_Short= 23+precision_Arg;
	int width_Long = 11+precision_PSD;

	///------------------------------------------------------------------------
	/// headline
	{
		// top header
		{
            out << "; " << Global_Variables::date_Time() << endl;
			out << "; " << data_Type << endl << endl;
            out << "; polarization = " << tempLoc.toString(incident_Polarization,'f', 3) << endl;
			out << "; " << at_Fixed_Heading << endl << endl;
			out << "; " << instrumental_Heading << endl << endl;
			out << "; " << geometry_Heading << endl << endl;
		}

		// argument
		{
			out << qSetFieldWidth(arg_Shift-1) << ";";
			out << qSetFieldWidth(width_Short) << argument_Heading  << qSetFieldWidth(width_Long);
		}

		// psd
		{
			out << value_Heading << qSetFieldWidth(arg_Shift) << endl  << qSetFieldWidth(width_Short);
		}
	}
	///------------------------------------------------------------------------
	/// data
	{
		for(size_t i=0; i<arg.size(); ++i)
		{
			// argument
			{
                out << qSetFieldWidth(width_Short) << tempLoc.toString(arg[i],'e',precision_Arg)  << qSetFieldWidth(width_Long);
			}

			// psd
			{
                out << tempLoc.toString(val[i],'e',precision_PSD);
			}
			if(i!=arg.size()-1)	out << qSetFieldWidth(arg_Shift) << endl << qSetFieldWidth(width_Short);
		}
	}
}

void Main_Calculation_Module::print_Calculated_To_File()
{
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		int counter = 0;
		if( multilayers[tab_Index]->enable_Calc_Independent_Curves )
		{
			for(Data_Element<Independent_Curve>& independent : calculation_Trees[tab_Index]->independent)
			{
				print_Reflect_To_File(independent, multilayer_Tabs->tabText(tab_Index), counter++);
			}
		}
		counter = 0;
		if( multilayers[tab_Index]->enable_Calc_Target_Curves )
		{
			for(Data_Element<Target_Curve>& target : calculation_Trees[tab_Index]->target)
			{
				print_Reflect_To_File(target, multilayer_Tabs->tabText(tab_Index), counter);
				print_PSD_1D_To_File (target, multilayer_Tabs->tabText(tab_Index), counter);
				counter++;
			}
		}
	}
}

template <typename Type>
void Main_Calculation_Module::print_Reflect_To_File(Data_Element<Type>& data_Element, QString struct_Name, int index)
{
    // point as decimal separator
    QLocale tempLoc = QLocale::c();

	QString first_Name;
    if(data_Element.curve_Class == INDEPENDENT)	first_Name = struct_Name + "_independent_" + /*tempLoc.toString(index+1) + "_" +*/ data_Element.the_Class->name;
	if(data_Element.curve_Class == TARGET)
	{
		Target_Curve* target_Curve = qobject_cast<Target_Curve*>(data_Element.the_Class);
        first_Name = struct_Name + "_target_" + target_Curve->index/*tempLoc.toString(index+1)*/ + "_" + data_Element.the_Class->name;
	}

    QString path = Global_Variables::working_Directory() + "/";

	//-----------------------------------------------------------------------------------------------------
	// 1D
	//-----------------------------------------------------------------------------------------------------

	if(print_1D_Data_On_Recalculation)
	{
		int prec = 10;
		const Data& measurement = data_Element.the_Class->measurement;
		double angular_Coeff = angle_Coefficients_Map.value(data_Element.the_Class->angular_Units);
		double spectral_Coeff = wavelength_Coefficients_Map.value(data_Element.the_Class->spectral_Units);
		QString angular_Units_Name = angle_Units_Legend_Map.value(data_Element.the_Class->angular_Units);
		QString spectral_Units_Name = wavelength_Units_Legend_Map.value(data_Element.the_Class->spectral_Units);

		QString argument_Heading;
		QString at_Fixed_Heading = Global_Variables::wavelength_Energy_Name(data_Element.the_Class->spectral_Units, true) + " = " +
                                   tempLoc.toString(Global_Variables::wavelength_Energy(data_Element.the_Class->spectral_Units, measurement.lambda_Value)/spectral_Coeff, 'g', prec) + " " +
								   spectral_Units_Name;
        QString instrumental_Heading =  "spectral resolution (FWHM) = " + tempLoc.toString(measurement.spectral_Distribution.FWHM_distribution, 'g', prec) +
                                        "\n; beam divergence (FWHM) = " + tempLoc.toString(measurement.beam_Theta_0_Distribution.FWHM_distribution/angular_Coeff, 'g', prec) + " " + angular_Units_Name;
        QString geometry_Heading =  "beam width = " + tempLoc.toString(measurement.beam_Geometry.size, 'g', prec) + " mm" +
                                    "\n; sample size = " + tempLoc.toString(measurement.sample_Geometry.size, 'g', prec) + " mm";

        QString detector_Width_Heading = "\n; detector angular width (FWHM) = " + tempLoc.toString(measurement.theta_Resolution_FWHM/angular_Coeff, 'g', prec) + " " + angular_Units_Name;
		if(	data_Element.calc_Functions.check_Reflectance   ||
			data_Element.calc_Functions.check_Transmittance ||
			data_Element.calc_Functions.check_Absorptance   ||
			data_Element.calc_Functions.check_Scattering	)
		{
			vector<double> argument;
			if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Specular_Scan] )
			{
				if( data_Element.the_Class->measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
				{
					argument_Heading = "Grazing angle (" + angular_Units_Name + ")";
					argument = data_Element.the_Class->measurement.beam_Theta_0_Angle_Vec;
					for(double& arg : argument)	arg = arg/angular_Coeff;
				}
				if( data_Element.the_Class->measurement.argument_Type == argument_Types[Wavelength_Energy] )
				{
                    at_Fixed_Heading = "grazing angle = " + tempLoc.toString(measurement.beam_Theta_0_Angle_Value/angular_Coeff, 'g', prec) + " " + angular_Units_Name;

					argument_Heading = Global_Variables::wavelength_Energy_Name(data_Element.the_Class->spectral_Units) + " (" + spectral_Units_Name + ")";
					argument = data_Element.the_Class->measurement.lambda_Vec;
					for(double& arg : argument)	arg = Global_Variables::wavelength_Energy(data_Element.the_Class->spectral_Units,arg)/spectral_Coeff;
				}
			}
			if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Detector_Scan] )
			{
                at_Fixed_Heading += "\n; beam grazing angle = " + tempLoc.toString(measurement.beam_Theta_0_Angle_Value/angular_Coeff, 'g', prec) + " " + angular_Units_Name;
				instrumental_Heading += detector_Width_Heading;
				argument_Heading = "Detector angle (" + angular_Units_Name + ")";
				argument = data_Element.the_Class->measurement.detector_Theta_Angle_Vec;
				for(double& arg : argument)	arg = arg/angular_Coeff;
			}
			if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Rocking_Curve] )
			{
                at_Fixed_Heading += "\n; specular grazing angle position = " + tempLoc.toString(measurement.beam_Theta_0_Specular_Position/angular_Coeff, 'g', prec) + " " + angular_Units_Name;
				instrumental_Heading += detector_Width_Heading;
				argument_Heading = "Beam grazing angle (" + angular_Units_Name + ")";
				argument = data_Element.the_Class->measurement.beam_Theta_0_Angle_Vec;
				for(double& arg : argument)	arg = arg/angular_Coeff;
			}
			if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Offset_Scan] )
			{
                at_Fixed_Heading += "\n; detector offset from specular direction = " + tempLoc.toString(measurement.detector_Theta_Offset/angular_Coeff, 'g', prec) + " " + angular_Units_Name;
				instrumental_Heading += detector_Width_Heading;
				argument_Heading = "Beam grazing angle (" + angular_Units_Name + ")";
				argument = data_Element.the_Class->measurement.beam_Theta_0_Angle_Vec;
				for(double& arg : argument)	arg = arg/angular_Coeff;
			}

			QString name = path + first_Name + ".txt";
			QFile file(name);
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			out.setFieldAlignment(QTextStream::AlignLeft);

			print_Data(out,
					   argument,
					   data_Element.unwrapped_Reflection,
					   data_Element.calc_Functions,
					   data_Element.the_Class->measurement.polarization,
					   data_Element.the_Class->measurement.measurement_Type,
					   argument_Heading,
					   at_Fixed_Heading,
					   instrumental_Heading,
					   geometry_Heading);
			file.close();
		}
	}

	//-----------------------------------------------------------------------------------------------------
	// 2D
	//-----------------------------------------------------------------------------------------------------

	if(print_2D_Data_On_Recalculation)
	{
		// intensity
		if(data_Element.calc_Functions.check_Field)
		if(data_Element.the_Class->calculated_Values.field_Intensity.size()>0)
		{
			QString name = path + first_Name + "_intensity.txt";
			QFile file(name);
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			out.setFieldAlignment(QTextStream::AlignLeft);
			print_Matrix(intensity_Function, data_Element.calc_Functions, out, data_Element.the_Class->calculated_Values.field_Intensity, data_Element.the_Class->measurement, data_Element.the_Class->angular_Units, data_Element.the_Class->spectral_Units);
			file.close();
		}

		// absorption
		if(data_Element.calc_Functions.check_Joule)
		if(data_Element.the_Class->calculated_Values.absorption_Map.size()>0)
		{
			QString name = path + first_Name + "_absorption.txt";
			QFile file(name);
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			out.setFieldAlignment(QTextStream::AlignLeft);
			print_Matrix(joule_Function, data_Element.calc_Functions, out, data_Element.the_Class->calculated_Values.absorption_Map, data_Element.the_Class->measurement, data_Element.the_Class->angular_Units, data_Element.the_Class->spectral_Units);
			file.close();
		}

		// GISAS
		if(data_Element.calc_Functions.check_GISAS)
		if(data_Element.the_Class->calculated_Values.GISAS_Instrumental.size()>0)
		{
			QString name = path + first_Name + "_GISAS.txt";
			QFile file(name);
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			out.setFieldAlignment(QTextStream::AlignLeft);
			print_Matrix(gisas_Function, data_Element.calc_Functions, out, data_Element.the_Class->calculated_Values.GISAS_Instrumental, data_Element.the_Class->measurement, data_Element.the_Class->angular_Units, data_Element.the_Class->spectral_Units);
			file.close();
		}
	}
}
template void Main_Calculation_Module::print_Reflect_To_File<Independent_Curve>(Data_Element<Independent_Curve>&, QString, int);
template void Main_Calculation_Module::print_Reflect_To_File<Target_Curve>	   (Data_Element<Target_Curve>&,      QString, int);

void Main_Calculation_Module::print_Data(QTextStream &out,
										 vector<double> &arg,
										 Unwrapped_Reflection* unwrapped_Reflection,
										 Calc_Functions& calc_Functions,
										 // heading
										 double incident_Polarization,
										 QString measurement_Type,
										 QString argument_Heading,
										 QString at_Fixed_Heading,
										 QString instrumental_Heading,
										 QString geometry_Heading)
{
    // point as decimal separator
    QLocale tempLoc = QLocale::c();

	// headline

	QString R_mixed  = "R_mixed";
	QString R_s      = "R_s";
	QString R_p      = "R_p";
	QString Phi_R_s  = "Phase_R_s";
	QString Phi_R_p  = "Phase_R_p";

	QString T_mixed  = "T_mixed";
	QString T_s      = "T_s";
	QString T_p      = "T_p";
	QString Phi_T_s  = "Phase_T_s";
	QString Phi_T_p  = "Phase_T_p";

	QString A_mixed  = "A_mixed";
	QString A_s      = "A_s";
	QString A_p      = "A_p";

	QString S_mixed  = "S_mixed";
	QString S_s      = "S_s";
	QString S_p      = "S_p";

	int precision_Arg = 16;
	int precision_R_T_A_S = 6;
	int precision_Phi = 4;

	int arg_Shift = 3;
	int width_Short= 8+precision_Arg;
	int width_Long = 11+precision_R_T_A_S;

	///------------------------------------------------------------------------
	/// headline
	{
		// top header
		{
            out << "; " << Global_Variables::date_Time()  <<endl;
			out << "; " << measurement_Type << endl << endl;
            out << "; polarization = " << tempLoc.toString(incident_Polarization,'f', 3) << endl;
			out << "; " << at_Fixed_Heading << endl << endl;
			out << "; " << instrumental_Heading << endl << endl;
			out << "; " << geometry_Heading << endl << endl;
		}

		// argument
		{
			out << qSetFieldWidth(arg_Shift-1) << ";";
			out << qSetFieldWidth(width_Short) << argument_Heading  << qSetFieldWidth(width_Long);
		}

		// reflectance
		if(calc_Functions.check_Reflectance)
		{
																		out << R_mixed;
			if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << R_s;
			if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << R_p;
			if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << Phi_R_s;
			if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << Phi_R_p;
		}

		// transmittance
		if(calc_Functions.check_Transmittance)
		{
																		out << T_mixed;
			if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << T_s;
			if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << T_p;
			if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << Phi_T_s;
			if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << Phi_T_p;
		}

		// absorptance
		if(calc_Functions.check_Absorptance)
		{
																		out << A_mixed;
			if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << A_s;
			if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << A_p;
		}

		// scattering
		if(calc_Functions.check_Scattering)
		{																out << S_mixed;
			if(unwrapped_Reflection->multilayer->imperfections_Model.vertical_Correlation != partial_Correlation)
			{
				if((incident_Polarization + 1) > POLARIZATION_TOLERANCE) out << S_s;
				if((incident_Polarization - 1) <-POLARIZATION_TOLERANCE) out << S_p;
			}
		}
		out << qSetFieldWidth(arg_Shift) << endl  << qSetFieldWidth(width_Short);
	}
	///------------------------------------------------------------------------
	/// data
	{
		for(size_t i=0; i<arg.size(); ++i)
		{
			// argument
			{
                out << qSetFieldWidth(width_Short) << tempLoc.toString(arg[i],'f',precision_Arg)  << qSetFieldWidth(width_Long);
			}

			// reflectance
			if(calc_Functions.check_Reflectance) {
			if(unwrapped_Reflection->calculated_Values.R_Instrumental.size() == arg.size())
			{
                                                                            out << tempLoc.toString(unwrapped_Reflection->calculated_Values.R_Instrumental[i],'e',precision_R_T_A_S);
                if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.R_s           [i],'e',precision_R_T_A_S);
                if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.R_p           [i],'e',precision_R_T_A_S);
                if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.Phi_R_s       [i],'f',precision_Phi);
                if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.Phi_R_p       [i],'f',precision_Phi);
			}}

			// transmittance
			if(calc_Functions.check_Transmittance) {
			if(unwrapped_Reflection->calculated_Values.T_Instrumental.size() == arg.size())
			{
                                                                            out << tempLoc.toString(unwrapped_Reflection->calculated_Values.T_Instrumental[i],'e',precision_R_T_A_S);
                if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.T_s           [i],'e',precision_R_T_A_S);
                if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.T_p           [i],'e',precision_R_T_A_S);
                if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.Phi_T_s       [i],'f',precision_Phi);
                if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.Phi_T_p       [i],'f',precision_Phi);
			}}

			// absorptance
			if(calc_Functions.check_Absorptance)
			if(unwrapped_Reflection->calculated_Values.A.size() == arg.size())
			{
                                                                            out << tempLoc.toString(unwrapped_Reflection->calculated_Values.A			 [i],'e',precision_R_T_A_S);
                if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.A_s           [i],'e',precision_R_T_A_S);
                if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << tempLoc.toString(unwrapped_Reflection->calculated_Values.A_p           [i],'e',precision_R_T_A_S);
			}

			// scattering
			if(calc_Functions.check_Scattering) {
			if(unwrapped_Reflection->calculated_Values.S_Instrumental.size() == arg.size())
			{
                                                                             out << tempLoc.toString(unwrapped_Reflection->calculated_Values.S_Instrumental[i],'e',precision_R_T_A_S);
				if(unwrapped_Reflection->multilayer->imperfections_Model.vertical_Correlation != partial_Correlation)
				{
                    if((incident_Polarization + 1) > POLARIZATION_TOLERANCE) out << tempLoc.toString(unwrapped_Reflection->calculated_Values.S_s           [i],'e',precision_R_T_A_S);
                    if((incident_Polarization - 1) <-POLARIZATION_TOLERANCE) out << tempLoc.toString(unwrapped_Reflection->calculated_Values.S_p           [i],'e',precision_R_T_A_S);
				}
			}}

			if(i!=arg.size()-1)	out << qSetFieldWidth(arg_Shift) << endl << qSetFieldWidth(width_Short);
		}
	}
}

void Main_Calculation_Module::print_Matrix(QString function, const Calc_Functions& calc_Functions, QTextStream &out, const vector<vector<double>>& matrix, const Data& measurement, QString angular_Units, QString spectral_Units)
{
    // point as decimal separator
    QLocale tempLoc = QLocale::c();

	if(matrix.size()>0)
	{
        out << "; " << Global_Variables::date_Time() <<endl;

		if(function == intensity_Function)	out << "; " << "Field intensity distribution" << endl;
		if(function == joule_Function)		out << "; " << "Photoabsorption distribution" << endl;
		if(function == gisas_Function)		out << "; " << "GISAS data" << endl;
		out << "; " << "(" << matrix.front().size() << " x " << matrix.size() << ") points" << endl << endl;

		double angular_Coeff = angle_Coefficients_Map.value(angular_Units);
		double spectral_Coeff = wavelength_Coefficients_Map.value(spectral_Units);
		QString angular_Units_Name = angle_Units_Legend_Map.value(angular_Units);
		QString spectral_Units_Name = wavelength_Units_Legend_Map.value(spectral_Units);

		QString shift = "";
		for(int i=0; i<wavelength_Units_List.size(); ++i)
		{
			if(spectral_Units == wavelength_Units_List[i])
			{
				if( 0<=i && i<=nm)                           shift = "";
				if( eV<=i && i<wavelength_Units_List.size()) shift = "    ";
			}
		}

		int prec = 6;
        QString at_Fixed_Angle          = "beam grazing angle          :  "  + tempLoc.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff,'f', prec) + " " + angular_Units_Name;
		QString at_Fixed_Wavelength = Global_Variables::wavelength_Energy_Name(spectral_Units, true) + shift + "                  :  " +
                                      tempLoc.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.lambda_Value)/spectral_Coeff, 'f', prec) + " " +
									  spectral_Units_Name;
        QString spectral_Resolution     = "spectral resolution         :  " + tempLoc.toString(measurement.spectral_Distribution.FWHM_distribution, 'f', prec);
        QString beam_Theta_0_Divergence = "beam polar divergence       :  " + tempLoc.toString(measurement.beam_Theta_0_Distribution.FWHM_distribution/angular_Coeff, 'f', prec) + " " + angular_Units_Name;
        QString beam_Phi_0_Divergence   = "beam azimuthal divergence   :  " + tempLoc.toString(measurement.beam_Phi_0_Distribution.FWHM_distribution/angular_Coeff, 'f', prec) + " " + angular_Units_Name;
        QString beam_Geometry           = "beam width                  :  " + tempLoc.toString(measurement.beam_Geometry.size, 'f', prec) + " mm";
        QString sample_Geometry		    = "sample size                 :  " + tempLoc.toString(measurement.sample_Geometry.size, 'f', prec) + " mm";

		if(function == gisas_Function)
		{
            out << "; " << "polarization                :  " << tempLoc.toString(measurement.polarization,'f', prec) << endl;
			out << "; " << at_Fixed_Angle << endl;
			out << "; " << at_Fixed_Wavelength << endl;
			out << "; " << spectral_Resolution << endl;
			out << "; " << beam_Theta_0_Divergence << endl;
			out << "; " << beam_Phi_0_Divergence << endl;
			out << "; " << beam_Geometry << endl;
			out << "; " << sample_Geometry << endl;
			out << endl;

            out << "; " << "detector point size Theta   :  " << tempLoc.toString(measurement.theta_Resolution_FWHM/angular_Coeff,'f', prec) << " " << angular_Units_Name << endl;
            out << "; " << "detector point size Phi     :  " << tempLoc.toString(measurement.phi_Resolution_FWHM/angular_Coeff,'f', prec) << " " << angular_Units_Name << endl;
			out << endl;

            out << "; " << "detector Theta range   :   (" << tempLoc.toString(measurement.detector_Theta_Angle.independent.min/angular_Coeff,'f', prec)
                                            << " , " << tempLoc.toString(measurement.detector_Theta_Angle.independent.max/angular_Coeff,'f', prec) << ") " << angular_Units_Name << endl;
            out << "; " << "detector Phi range     :   (" << tempLoc.toString(measurement.detector_Phi_Angle.independent.min/angular_Coeff,'f', prec)
                                            << " , " << tempLoc.toString(measurement.detector_Phi_Angle.independent.max/angular_Coeff,'f', prec) << ") " << angular_Units_Name << endl;
		}
		if(function == intensity_Function || function == joule_Function)
		{
            out << "; " << "polarization       :  "  << tempLoc.toString(measurement.polarization,'f', prec) << endl;
            out << "; " << "depth              :  (" << tempLoc.toString(-calc_Functions.field_Ambient_Distance,'f', 2)
                                            << " , " << tempLoc.toString(-calc_Functions.field_Ambient_Distance + (matrix.front().size()-1)*calc_Functions.field_Step,'f', 2) << ") angstrom, step = "
                                            << tempLoc.toString(calc_Functions.field_Step,'f', 2) << " angstrom" << endl;

			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
			out << "; " << Global_Variables::wavelength_Energy_Name(spectral_Units, true) << shift << "         :  " <<
                   tempLoc.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.lambda_Value)/spectral_Coeff, 'f', prec) << " " <<
				   spectral_Units_Name << endl;
            out << "; " << "beam grazing angle :  (" << tempLoc.toString(measurement.beam_Theta_0_Angle.independent.min/angular_Coeff,'f', prec)
                           << " , " << tempLoc.toString(measurement.beam_Theta_0_Angle.independent.max/angular_Coeff,'f', prec) << ") " << angular_Units_Name << endl;
			}
			if(measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
			out << "; " << Global_Variables::wavelength_Energy_Name(spectral_Units, true) << shift << "         :  ("
                            << tempLoc.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.independent.min)/spectral_Coeff, 'f', prec) << " , "
                            << tempLoc.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.independent.max)/spectral_Coeff, 'f', prec) << ") " << spectral_Units_Name << endl;
            out << "; " << "beam grazing angle :  "  << tempLoc.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff,'f', prec) << " " << angular_Units_Name << endl;
			}
		}
		out << endl;

		int precision = 6;
		for(size_t i=0; i<matrix.size(); ++i)
		{
			for(size_t j=0; j<matrix.front().size(); ++j)
			{
                out << tempLoc.toString(matrix[i][j],'e',precision) << "\t";
			}
			out << endl;
		}
	}
}

void Main_Calculation_Module::add_Fit(QString name_Modificator, int run, QString par_Name, double val)
{
	// save new trees
	QVector<QTreeWidget*> fitted_Trees(multilayers.size());
	QVector<Imperfections_Model> fitted_Imperfections_Models(multilayers.size());
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		fitted_Trees[tab_Index] = calculation_Trees[tab_Index]->real_Struct_Tree;
		fitted_Imperfections_Models[tab_Index] = multilayers[tab_Index]->imperfections_Model;
	}
    global_Multilayer_Approach->add_Fitted_Structure(fitted_Trees, fitted_Imperfections_Models, name_Modificator, run, par_Name, val);
}
