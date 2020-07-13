#include "main_calculation_module.h"

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

void Main_Calculation_Module::increase_Mesh_density(Data_Element<Target_Curve>& target)
{
	// PARAMETER
	Target_Curve* target_Curve = qobject_cast<Target_Curve*>(target.the_Class);
	if(target_Curve->curve.mesh_Density_Factor>1)
	{
		if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
		{
			vector<double>& angle = target_Curve->measurement.beam_Theta_0_Angle_Vec;
			vector<double> dense_Angle;
			dense_Angle.resize((angle.size()-1)*target_Curve->curve.mesh_Density_Factor+1);
			for(size_t i=0; i<angle.size()-1; i++)
			{
				double delta_Angle = (angle[i+1]-angle[i])/target_Curve->curve.mesh_Density_Factor;
				dense_Angle[i*target_Curve->curve.mesh_Density_Factor] = angle[i];
				for(int dense_Index=1; dense_Index<target_Curve->curve.mesh_Density_Factor; dense_Index++)
				{
					dense_Angle[i*target_Curve->curve.mesh_Density_Factor+dense_Index] = angle[i] + delta_Angle*(dense_Index+target_Curve->curve.mesh_Density_Shift);
				}
			}
			dense_Angle.back() = angle.back();
			target_Curve->measurement.beam_Theta_0_Angle_Vec = dense_Angle;
		}
		if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
		{
			vector<double>& lambda = target_Curve->measurement.lambda_Vec;
			vector<double> dense_Lambda;
			dense_Lambda.resize((lambda.size()-1)*target_Curve->curve.mesh_Density_Factor+1);
			for(size_t i=0; i<lambda.size()-1; i++)
			{
				double delta_Lambda = (lambda[i+1]-lambda[i])/target_Curve->curve.mesh_Density_Factor;
				dense_Lambda[i*target_Curve->curve.mesh_Density_Factor] = lambda[i];
				for(int dense_Index=1; dense_Index<target_Curve->curve.mesh_Density_Factor; dense_Index++)
				{
					dense_Lambda[i*target_Curve->curve.mesh_Density_Factor+dense_Index] = lambda[i] + delta_Lambda*(dense_Index+target_Curve->curve.mesh_Density_Shift);
				}
			}
			dense_Lambda.back() = lambda.back();
			target_Curve->measurement.lambda_Vec = dense_Lambda;
		}
	}
}

void Main_Calculation_Module::decrease_Mesh_density(Data_Element<Target_Curve>& target, bool fit_Mode)
{
//	// PARAMETER

//	// decrease mesh density to default
//	Target_Curve* target_Curve = qobject_cast<Target_Curve*>(target.the_Class);
//	if(target_Curve->curve.mesh_Density_Factor>1)
//	{
//		if(!fit_Mode)
//		{
//			// decrease argument measurement
//			if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
//			{
//				vector<double>& angle = target_Curve->measurement.beam_Theta_0_Angle_Vec;
//				vector<double> sparse_Angle;
//				sparse_Angle.resize((angle.size()-1)/target_Curve->curve.mesh_Density_Factor+1);
//				for(int i=0; i<sparse_Angle.size(); i++)
//				{
//					sparse_Angle[i] = angle[i*target_Curve->curve.mesh_Density_Factor];
//				}
//				target_Curve->measurement.beam_Theta_0_Angle_Vec = sparse_Angle;
//			}
//			if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])
//			{
//				vector<double>& lambda = target_Curve->measurement.lambda_Vec;
//				vector<double> sparse_Lambda;
//				sparse_Lambda.resize((lambda.size()-1)/target_Curve->curve.mesh_Density_Factor+1);
//				for(int i=0; i<sparse_Lambda.size(); i++)
//				{
//					sparse_Lambda[i] = lambda[i*target_Curve->curve.mesh_Density_Factor];
//				}
//				target_Curve->measurement.lambda_Vec = sparse_Lambda;
//			}

//			// decrease value in calc_Functions
//			int size;
//			if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle])	{size = target_Curve->measurement.beam_Theta_0_Angle_Vec.size();}
//			if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy])	{size = target_Curve->measurement.lambda_Vec.size();}
//			vector<double> sparse_Val  (size);
//			vector<double> sparse_Val_s(size);
//			vector<double> sparse_Val_p(size);
//			vector<double> sparse_Phi_s(size);
//			vector<double> sparse_Phi_p(size);

//			/// R
//			// reflectance
//			if( target.unwrapped_Structure->calc_Functions.check_Reflectance ||
//				target.unwrapped_Structure->calc_Functions.check_Absorptance )
//			{
//				for(int i=0; i<size; i++)
//				{
//					sparse_Val  [i] = target.unwrapped_Reflection->calculated_Values.R_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Val_s[i] = target.unwrapped_Reflection->calculated_Values.R_s           [i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Val_p[i] = target.unwrapped_Reflection->calculated_Values.R_p           [i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Phi_s[i] = target.unwrapped_Reflection->calculated_Values.Phi_R_s       [i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Phi_p[i] = target.unwrapped_Reflection->calculated_Values.Phi_R_p       [i*target_Curve->curve.mesh_Density_Factor];
//				}
//				sparse_Val  .back() = target.unwrapped_Reflection->calculated_Values.R_Instrumental.back();
//				sparse_Val_s.back() = target.unwrapped_Reflection->calculated_Values.R_s           .back();
//				sparse_Val_p.back() = target.unwrapped_Reflection->calculated_Values.R_p           .back();
//				sparse_Phi_s.back() = target.unwrapped_Reflection->calculated_Values.Phi_R_s       .back();
//				sparse_Phi_p.back() = target.unwrapped_Reflection->calculated_Values.Phi_R_p       .back();

//				// copying
//				target.unwrapped_Reflection->calculated_Values.R				= sparse_Val;
//				target.unwrapped_Reflection->calculated_Values.R_Instrumental = sparse_Val;
//				target.unwrapped_Reflection->calculated_Values.R_s			= sparse_Val_s;
//				target.unwrapped_Reflection->calculated_Values.R_p			= sparse_Val_p;
//				target.unwrapped_Reflection->calculated_Values.Phi_R_s		= sparse_Phi_s;
//				target.unwrapped_Reflection->calculated_Values.Phi_R_p		= sparse_Phi_p;
//			}
//			/// T
//			// transmittance
//			if( target.unwrapped_Structure->calc_Functions.check_Transmittance ||
//				target.unwrapped_Structure->calc_Functions.check_Absorptance )
//			{
//				for(int i=0; i<size; i++)
//				{
//					sparse_Val  [i] = target.unwrapped_Reflection->calculated_Values.T_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Val_s[i] = target.unwrapped_Reflection->calculated_Values.T_s           [i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Val_p[i] = target.unwrapped_Reflection->calculated_Values.T_p           [i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Phi_s[i] = target.unwrapped_Reflection->calculated_Values.Phi_T_s       [i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Phi_p[i] = target.unwrapped_Reflection->calculated_Values.Phi_T_p       [i*target_Curve->curve.mesh_Density_Factor];
//				}
//				sparse_Val  .back() = target.unwrapped_Reflection->calculated_Values.T_Instrumental.back();
//				sparse_Val_s.back() = target.unwrapped_Reflection->calculated_Values.T_s           .back();
//				sparse_Val_p.back() = target.unwrapped_Reflection->calculated_Values.T_p           .back();
//				sparse_Phi_s.back() = target.unwrapped_Reflection->calculated_Values.Phi_T_s       .back();
//				sparse_Phi_p.back() = target.unwrapped_Reflection->calculated_Values.Phi_T_p       .back();

//				// copying
//				target.unwrapped_Reflection->calculated_Values.T			  = sparse_Val;
//				target.unwrapped_Reflection->calculated_Values.T_Instrumental = sparse_Val;
//				target.unwrapped_Reflection->calculated_Values.T_s			  = sparse_Val_s;
//				target.unwrapped_Reflection->calculated_Values.T_p			  = sparse_Val_p;
//				target.unwrapped_Reflection->calculated_Values.Phi_T_s		  = sparse_Phi_s;
//				target.unwrapped_Reflection->calculated_Values.Phi_T_p		  = sparse_Phi_p;
//			}
//			/// A
//			// absorptance
//			if( target.unwrapped_Structure->calc_Functions.check_Absorptance )
//			{
//				for(int i=0; i<size; i++)
//				{
//					sparse_Val  [i] = target.unwrapped_Reflection->calculated_Values.A	[i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Val_s[i] = target.unwrapped_Reflection->calculated_Values.A_s[i*target_Curve->curve.mesh_Density_Factor];
//					sparse_Val_p[i] = target.unwrapped_Reflection->calculated_Values.A_p[i*target_Curve->curve.mesh_Density_Factor];
//				}
//				sparse_Val  .back() = target.unwrapped_Reflection->calculated_Values.A  .back();
//				sparse_Val_s.back() = target.unwrapped_Reflection->calculated_Values.A_s.back();
//				sparse_Val_p.back() = target.unwrapped_Reflection->calculated_Values.A_p.back();

//				// copying
//				target.unwrapped_Reflection->calculated_Values.A   = sparse_Val;
//				target.unwrapped_Reflection->calculated_Values.A_s = sparse_Val_s;
//				target.unwrapped_Reflection->calculated_Values.A_p = sparse_Val_p;
//			}
//			// TODO
//			// scattering
//			// GISAS

//		} else
//		{
//			// if fitting
//			int size;
//			if(target_Curve->measurement.argument_Type == argument_Types[Beam_Grazing_Angle]){size = (target_Curve->measurement.beam_Theta_0_Angle_Vec.size()-1)/target_Curve->curve.mesh_Density_Factor+1;}
//			if(target_Curve->measurement.argument_Type == argument_Types[Wavelength_Energy]) {size = (target_Curve->measurement.lambda_Vec.            size()-1)/target_Curve->curve.mesh_Density_Factor+1;}
//			vector<double> sparse_Val  (size);

//			/// R
//			{
//				for(int i=0; i<size; i++)
//				{
//					sparse_Val  [i] = target.unwrapped_Reflection->calculated_Values.R_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
//				}
//				sparse_Val.back() = target.unwrapped_Reflection->calculated_Values.R_Instrumental.back();

//				// copying
//				target.unwrapped_Reflection->calculated_Values.R_Instrumental = sparse_Val;
//			}
//			/// T
//			{
//				for(int i=0; i<size; i++)
//				{
//					sparse_Val  [i] = target.unwrapped_Reflection->calculated_Values.T_Instrumental[i*target_Curve->curve.mesh_Density_Factor];
//				}
//				sparse_Val.back() = target.unwrapped_Reflection->calculated_Values.T_Instrumental.back();

//				// copying
//				target.unwrapped_Reflection->calculated_Values.T_Instrumental = sparse_Val;
//			}
//			// TODO
//			// scattering
//			// GISAS
//		}
//	}
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
			// TODO
//			increase_Mesh_density(target_Data_Element);
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
				calculation_With_Sampling_1_Case(calculation_Tree, data_Element, measurement.beam_Theta_0_Angle_Vec.size(), calculated_Curve, "spectral", R_and_T);
			}
			if( measurement.argument_Type  == argument_Types[Wavelength_Energy] )
			{
				// theta_0 distribution
				calculation_With_Sampling_1_Case(calculation_Tree, data_Element, measurement.lambda_Vec.size(), calculated_Curve, "angular", R_and_T);
			}
		}
		data_Element.calc_Functions.check_Absorptance = state_Absorptance;
		data_Element.calc_Functions.check_Field = state_Field;
		data_Element.calc_Functions.check_Joule = state_Joule;
	}
	/// Detector_Scan
	/// Rocking_Curve
	/// Offset_Scan
	if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan]   ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		//	if( data_Element.calc_Functions.check_Scattering)	calculated_Curve = &calculated_Values.S;

		//	if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		//	    measurement.measurement_Type == measurement_Types[GISAS_Map] )
		//	{
		//		if_Single_Beam_Value = true;
		//		if(spec_Scat_mode == SPECULAR_MODE) 	num_Points = 1;
		//		if(spec_Scat_mode == SCATTERED_MODE) 	num_Points = measurement.detector_Theta_Angle_Vec.size();
		//	}
		//	if( measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		//	    measurement.measurement_Type == measurement_Types[Offset_Scan] )
		//	{
		//		if(spec_Scat_mode == SPECULAR_MODE) 	num_Points = measurement.beam_Theta_0_Angle_Vec.size();
		//		if(spec_Scat_mode == SCATTERED_MODE) 	num_Points = measurement.beam_Theta_0_Angle_Vec.size();
		//	}

//		// spectral distribution
//		if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON && !measurement.spectral_Distribution.use_Sampling)	{
//			wrap_Curve(measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_From_Spectral_Vec, working_Curve, measurement.spectral_Distribution.distribution_Function);
//			*calculated_Curve = *working_Curve;
//		}
//		// theta_0 distribution
//		if((measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON) && !measurement.beam_Theta_0_Distribution.use_Sampling)		{
//			wrap_Curve(measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_Vec, working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function);
//			*calculated_Curve = *working_Curve;
//		}
	}
//	/// GISAS_Map
//	if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
//	{
//		bool recalc = false;
//		// spectral distribution
//		if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON && !measurement.spectral_Distribution.use_Sampling)	{
//			wrap_2D_Curve(measurement, calculated_Values, measurement.theta_0_Resolution_From_Spectral_Vec, measurement.spectral_Distribution.distribution_Function, "theta");
//			calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
//			recalc = true;
//		}
//		// theta_0 distribution
//		if((measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON) && !measurement.beam_Theta_0_Distribution.use_Sampling)		{
//			wrap_2D_Curve(measurement, calculated_Values, measurement.theta_0_Resolution_Vec, measurement.beam_Theta_0_Distribution.distribution_Function, "theta");
//			calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
//			recalc = true;
//		}
//		if(!recalc) calculated_Values.GISAS_Instrumental = calculated_Values.GISAS_Map;
//	}
}
template void Main_Calculation_Module::calculation_With_Sampling<Independent_Curve>(Calculation_Tree*, Data_Element<Independent_Curve>&);
template void Main_Calculation_Module::calculation_With_Sampling<Target_Curve>	   (Calculation_Tree*, Data_Element<Target_Curve>&);


template<typename Type>
void Main_Calculation_Module::calculation_With_Sampling_1_Case(Calculation_Tree* calculation_Tree, Data_Element<Type>& data_Element, size_t num_Points, vector<double>* calculated_Curve, QString spectral_Angular, bool R_and_T)
{
	Calculated_Values& calculated_Values = data_Element.the_Class->calculated_Values;
	Data& measurement = data_Element.the_Class->measurement;

	double* param_Value;
	Distribution distribution;
	if(spectral_Angular == "spectral")
	{
		param_Value = &measurement.wavelength.value;
		distribution = measurement.spectral_Distribution;
	} else // angular
	{
		param_Value = &measurement.beam_Theta_0_Angle.value;
		distribution = measurement.beam_Theta_0_Distribution;
	}


	// anyway
	QVector<double> sampled_Position_Vec(1, *param_Value);
	QVector<double> sampled_Weight_Vec(1, 1.);

	// if sampled
	if( distribution.use_Sampling &&
		distribution.FWHM_distribution>DBL_EPSILON &&
		distribution.number_of_Samples>1)
	{
		sampled_Position_Vec.resize(distribution.number_of_Samples);
		sampled_Weight_Vec  .resize(distribution.number_of_Samples);
		Global_Variables::distribution_Sampling(distribution, sampled_Position_Vec, sampled_Weight_Vec);

		// go to absolute values and remove out of range points
		if(spectral_Angular == "spectral")
		{
			for(int sampling_Point = 0; sampling_Point<distribution.number_of_Samples; sampling_Point++)
			{
				sampled_Position_Vec[sampling_Point] = (*param_Value)*(1+sampled_Position_Vec[sampling_Point]);
				if(sampled_Position_Vec[sampling_Point] <= DBL_EPSILON)
				{
					sampled_Position_Vec[sampling_Point] = DBL_EPSILON;
					sampled_Weight_Vec  [sampling_Point] = 0;
				}
			}
		} else // angular
		{
			for(int sampling_Point = 0; sampling_Point<distribution.number_of_Samples; sampling_Point++)
			{
				sampled_Position_Vec[sampling_Point] = (*param_Value)+sampled_Position_Vec[sampling_Point];
				if(sampled_Position_Vec[sampling_Point] < DBL_EPSILON)
				{
					sampled_Position_Vec[sampling_Point] = DBL_EPSILON;
					sampled_Weight_Vec  [sampling_Point] = 0;
				}
				if(sampled_Position_Vec[sampling_Point] > 90)
				{
					sampled_Position_Vec[sampling_Point] = 180-sampled_Position_Vec[sampling_Point];
				}
				if(sampled_Position_Vec[sampling_Point] >= 180)
				{
					sampled_Position_Vec[sampling_Point] = DBL_EPSILON;
					sampled_Weight_Vec  [sampling_Point] = 0;
				}
			}
		}
	}

	// main part
	if(sampled_Position_Vec.size()>1)
	{
		// initialization of accumulator
		vector<double> temporary_State(num_Points);
		vector<double> temporary_State_T(num_Points);// special case


		double initial_Parameter = *param_Value;
		double weight = 0;
		for(int sampling_Point = 0; sampling_Point<sampled_Position_Vec.size(); sampling_Point++)
		{
			*param_Value = sampled_Position_Vec[sampling_Point];
			if(data_Element.curve_Class == INDEPENDENT)
			{
				Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(data_Element.the_Class);
				independent_Curve->calc_Independent_cos2_k();
			}
			if(data_Element.curve_Class == TARGET)
			{
				Target_Curve* target_Curve = qobject_cast<Target_Curve*>(data_Element.the_Class);
				target_Curve->calc_Measured_cos2_k();
			}
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

		*param_Value = initial_Parameter;
		if(data_Element.curve_Class == INDEPENDENT)
		{
			Independent_Curve* independent_Curve = qobject_cast<Independent_Curve*>(data_Element.the_Class);
			independent_Curve->calc_Independent_cos2_k();
		}
		if(data_Element.curve_Class == TARGET)
		{
			Target_Curve* target_Curve = qobject_cast<Target_Curve*>(data_Element.the_Class);
			target_Curve->calc_Measured_cos2_k();
		}
	} else
	{
		///---------------------------------------------------------------
		calculation_Tree->calculate_1_Curve(data_Element);
		///---------------------------------------------------------------
	}
}
template void Main_Calculation_Module::calculation_With_Sampling_1_Case<Independent_Curve>(Calculation_Tree*, Data_Element<Independent_Curve>&, size_t, vector<double>*, QString, bool);
template void Main_Calculation_Module::calculation_With_Sampling_1_Case<Target_Curve>	  (Calculation_Tree*, Data_Element<Target_Curve>&,      size_t, vector<double>*, QString, bool);

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

	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		for(Data_Element<Independent_Curve>& independent_Data_Element : calculation_Trees[tab_Index]->independent)
		{
			calculation_With_Sampling(calculation_Trees[tab_Index], independent_Data_Element);
			qInfo() << endl << endl;
			if(lambda_Out_Of_Range) return;
			postprocessing(independent_Data_Element);
		}
		for(Data_Element<Target_Curve>& target_Data_Element : calculation_Trees[tab_Index]->target)
		{
			calculation_With_Sampling(calculation_Trees[tab_Index], target_Data_Element);
//			decrease_Mesh_density(target_Data_Element);
			if(lambda_Out_Of_Range) return;
			postprocessing(target_Data_Element);
		}
	}
	if(print_And_Verbose)
	{
		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		qInfo() << "\nCalculation: "<< elapsed.count()/1000000. << " seconds" << endl;
	}

	// replot graphs
	Global_Variables::plot_All_Data_in_Graphs();
	Global_Variables::plot_All_Data_in_Profiles();

	if(print_And_Verbose)
	{
		print_Calculated_To_File();
	}
}

void Main_Calculation_Module::wrap_Curve(const vector<double>& sparse_Argument,
										 const vector<double>* sparse_Input_Curve,
										 const vector<double>& resolution_FWHM,
										 vector<double>* output_Sparse_Curve,
										 QString distribution_Function)
{
	double range_Limit = 3;
	double(*distribution)(double, double);
	if(distribution_Function == distributions[Gate])	{distribution = Global_Variables::distribution_Gate;	range_Limit = 0.5; } else
	if(distribution_Function == distributions[Cosine])	{distribution = Global_Variables::distribution_Cosine;	range_Limit = 1;   } else
	if(distribution_Function == distributions[Gaussian]){distribution = Global_Variables::distribution_Gaussian;range_Limit = 2;   } else
	if(distribution_Function == distributions[Lorentz])	{distribution = Global_Variables::distribution_Lorentz;	range_Limit = MAX_DOUBLE;} else return;

	auto f = [&](double point_Index)
	{
		(*output_Sparse_Curve)[point_Index] = (*sparse_Input_Curve)[point_Index];
		double weight_Accumulator = 1;
		double weight = DBL_MIN;

		/*  positive direction  */
		double distance = 0;;
		for(size_t i=point_Index+1; (distance<range_Limit*resolution_FWHM[point_Index] && i<sparse_Argument.size()); i++)
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

	// program
	if(sparse_Argument.size()*resolution_FWHM[0]>50) // tunable
	{
		Global_Variables::parallel_For(sparse_Argument.size(), reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
		{
			Q_UNUSED(thread_Index)
			for(int point_Index=n_Min; point_Index<n_Max; ++point_Index)
			{
				f(point_Index);
			}
		});
	} else
	{
		for(size_t point_Index=0; point_Index<sparse_Argument.size(); ++point_Index)
		{
			f(point_Index);
		}
	}
}

void Main_Calculation_Module::wrap_2D_Curve(const Data& measurement, Calculated_Values& calculated_Values, const vector<double>& resolution_FWHM, QString distribution_Function, QString theta_Phi)
{
	double range_Limit = 3;
	double(*distribution)(double, double);
	if(distribution_Function == distributions[Gate])	{distribution = Global_Variables::distribution_Gate;	range_Limit = 0.5; } else
	if(distribution_Function == distributions[Cosine])  {distribution = Global_Variables::distribution_Cosine;  range_Limit = 1;   } else
	if(distribution_Function == distributions[Gaussian]){distribution = Global_Variables::distribution_Gaussian;range_Limit = 2;   } else
	if(distribution_Function == distributions[Lorentz]) {distribution = Global_Variables::distribution_Lorentz; range_Limit = MAX_DOUBLE;} else return;

	vector<vector<double>>& calculated = calculated_Values.GISAS_Map;
	vector<vector<double>>& wrapped = calculated_Values.GISAS_Instrumental;

	size_t num_Points = calculated.front().size();
	size_t phi_Points = calculated.size();

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
	Global_Variables::parallel_For(measurement.detector_Theta_Angle_Vec.size(), reflectivity_Calc_Threads, [&](int n_Min, int n_Max, int thread_Index)
	{
		Q_UNUSED(thread_Index)
		if(theta_Phi == "theta")
		{
			for(int theta_Index=n_Min; theta_Index<n_Max; ++theta_Index)
			{
				for(size_t phi_Index=0; phi_Index<phi_Points; ++phi_Index)
				{
					f_Theta(theta_Index, phi_Index);
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
}

template<class Type>
void Main_Calculation_Module::postprocessing(Data_Element<Type>& data_Element)
{
	auto start = std::chrono::system_clock::now();

	const Data& measurement = data_Element.the_Class->measurement;
	Calculated_Values& calculated_Values = data_Element.the_Class->calculated_Values;

	// interpolation
	vector<double>* calculated_Curve = nullptr;
	vector<double>* working_Curve = nullptr;
	if( data_Element.calc_Functions.check_Reflectance)
	{	calculated_Curve = &calculated_Values.R; calculated_Values.R_Instrumental = calculated_Values.R; working_Curve = &calculated_Values.R_Instrumental;}
	if( data_Element.calc_Functions.check_Transmittance)
	{	calculated_Curve = &calculated_Values.T; calculated_Values.R_Instrumental = calculated_Values.T; working_Curve = &calculated_Values.T_Instrumental;}
	if( data_Element.calc_Functions.check_Scattering)
	{	calculated_Curve = &calculated_Values.S; calculated_Values.R_Instrumental = calculated_Values.S; working_Curve = &calculated_Values.S_Instrumental;}


	/// Specular_Scan
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// spectral distribution
			if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON && !measurement.spectral_Distribution.use_Sampling)	{
				wrap_Curve(measurement.beam_Theta_0_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_From_Spectral_Vec, working_Curve, measurement.spectral_Distribution.distribution_Function);
				*calculated_Curve = *working_Curve;
			}
			// theta_0 distribution
			if(measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON)		{
				wrap_Curve(measurement.beam_Theta_0_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_Vec, working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function);
			}
		}
		if( measurement.argument_Type  == argument_Types[Wavelength_Energy] )
		{
			// spectral distribution
			if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON)	{
				wrap_Curve(measurement.lambda_Vec, calculated_Curve, measurement.spectral_Resolution_Vec, working_Curve, measurement.spectral_Distribution.distribution_Function);
				*calculated_Curve = *working_Curve;
			}
			// theta_0 distribution
			if((measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON) && !measurement.beam_Theta_0_Distribution.use_Sampling)		{
				wrap_Curve(measurement.lambda_Vec, calculated_Curve, measurement.spectral_Resolution_From_Theta_0_Vec, working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function);
			}
		}
	}
	/// Detector_Scan
	/// Rocking_Curve
	/// Offset_Scan
	if( measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan]   ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		// spectral distribution
		if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON && !measurement.spectral_Distribution.use_Sampling)	{
			wrap_Curve(measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_From_Spectral_Vec, working_Curve, measurement.spectral_Distribution.distribution_Function);
			*calculated_Curve = *working_Curve;
		}
		// theta_0 distribution
		if((measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON) && !measurement.beam_Theta_0_Distribution.use_Sampling)		{
			wrap_Curve(measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_0_Resolution_Vec, working_Curve, measurement.beam_Theta_0_Distribution.distribution_Function);
			*calculated_Curve = *working_Curve;
		}
		// detector
		if(measurement.theta_Resolution_FWHM>DBL_EPSILON)	{
			wrap_Curve(measurement.detector_Theta_Angle_Vec, calculated_Curve, measurement.theta_Resolution_Vec, working_Curve, measurement.theta_Distribution);
		}
	}
	/// GISAS_Map
	if( measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		bool recalc = false;
		// spectral distribution
		if(measurement.spectral_Distribution.FWHM_distribution>DBL_EPSILON && !measurement.spectral_Distribution.use_Sampling)	{
			wrap_2D_Curve(measurement, calculated_Values, measurement.theta_0_Resolution_From_Spectral_Vec, measurement.spectral_Distribution.distribution_Function, "theta");
			calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
			recalc = true;
		}
		// theta_0 distribution
		if((measurement.beam_Theta_0_Distribution.FWHM_distribution>DBL_EPSILON || abs(measurement.sample_Geometry.curvature)>DBL_EPSILON) && !measurement.beam_Theta_0_Distribution.use_Sampling)		{
			wrap_2D_Curve(measurement, calculated_Values, measurement.theta_0_Resolution_Vec, measurement.beam_Theta_0_Distribution.distribution_Function, "theta");
			calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
			recalc = true;
		}
		// detector phi
		if(measurement.phi_Resolution_FWHM>DBL_EPSILON || measurement.beam_Phi_0_Distribution.FWHM_distribution>DBL_EPSILON)		{
			wrap_2D_Curve(measurement, calculated_Values, measurement.phi_Resolution_Vec, measurement.phi_Distribution, "phi");
			calculated_Values.GISAS_Map = calculated_Values.GISAS_Instrumental;
			recalc = true;
		}
		// detector theta
		if(measurement.theta_Resolution_FWHM>DBL_EPSILON)	{
			wrap_2D_Curve(measurement, calculated_Values, measurement.theta_Resolution_Vec, measurement.theta_Distribution, "theta");
			recalc = true;
		}
		if(!recalc) calculated_Values.GISAS_Instrumental = calculated_Values.GISAS_Map;
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
	// footprint for scattering
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
		// footprint for R
		if( data_Element.calc_Functions.check_Reflectance)	{
			for(size_t point_Index=0; point_Index<calculated_Values.R_Instrumental.size(); ++point_Index)	{
				calculated_Values.R_Instrumental[point_Index] += measurement.background;
			}
		}
		// footprint for T
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

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	qInfo() << "postprocessing:    "<< elapsed.count()/1000000. << " seconds" << endl << endl;
}
template void Main_Calculation_Module::postprocessing<Independent_Curve>(Data_Element<Independent_Curve>&);
template void Main_Calculation_Module::postprocessing<Target_Curve>     (Data_Element<Target_Curve>&);

void Main_Calculation_Module::fitting_and_Confidence()
{
	if(calc_Mode!=FITTING && calc_Mode!=CONFIDENCE)
	{
		QMessageBox::critical(nullptr, "Main_Calculation_Module::fitting", "wrong calc_Mode");
		return;
	}

	// prepare cos2 and active_Parameter_Whats_This
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

	// cheking for lambda_Out_Of_Range
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		calculation_Trees[tab_Index]->fill_Calc_Trees();
		for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
		{
			// TODO scattered and specular cases
			calculation_Trees[tab_Index]->calculate_1_Kind(target_Element);			
			if(lambda_Out_Of_Range)
			{
				// TODO
//				decrease_Mesh_density(target_Element);
				return;
			} else
			{
				// TODO
//				decrease_Mesh_density(target_Element,true);
			}
		}
	}

	number_Of_Restricted_Regular_Components = 0;
	/// create calc tree, fitables and confidentials;
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		// prepare real_Calc_Tree  (for all multilayers!)
		calculation_Trees[tab_Index]->fill_Tree_From_Scratch(calculation_Trees[tab_Index]->real_Calc_Tree, calculation_Trees[tab_Index]->real_Struct_Tree, multilayers[tab_Index]);

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
		for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
		{
			for(Data_Element<Target_Curve>& target_Element : calculation_Trees[tab_Index]->target)
			{
				// TODO
//				decrease_Mesh_density(target_Element);
			}
		}
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
				add_Fit(fitted_State);
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

		find_Fittable_Confidence_Parameters(struct_Data, parent_Data, child, i, tab_Index, fitables_Period_Gamma, confidentials_Period_Gamma);

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

void Main_Calculation_Module::find_Fittable_Confidence_Parameters(Data& struct_Data, const Data& parent_Data, const tree<Node>::iterator& current, int child_Index, int tab_Index, bool fitables_Period_Gamma, bool confidentials_Period_Gamma)
{
	struct_Data.fill_Potentially_Fitable_Parameters_Vector(multilayers[tab_Index]->imperfections_Model);
	for(Parameter* parameter : struct_Data.potentially_Fitable_Parameters)
	{
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
				if(!parent_Data.regular_Components[child_Index].is_Common_Thickness && parameter->indicator.whats_This == whats_This_Thickness) 	{ go = false; }
				if(!parent_Data.regular_Components[child_Index].is_Common_Sigma     && parameter->indicator.whats_This == whats_This_Sigma_Diffuse) 	    { go = false; }
			}

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

				/// for rejection

				// sigma value is close to zero
				Parameter sigma_Parameter;
				if(parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma_Diffuse) sigma_Parameter = struct_Data.sigma_Diffuse;
				if(parameter->indicator.whats_This == whats_This_Sigma_Diffuse) sigma_Parameter = *parameter;

				if(parameter->indicator.whats_This == whats_This_Interlayer_My_Sigma_Diffuse || parameter->indicator.whats_This == whats_This_Sigma_Diffuse)
				if(sigma_Parameter.value < 0.1)
				if(!fit_Rejected_Sigmas.param_IDs.contains(sigma_Parameter.indicator.id))
				{
					fit_Rejected_Sigmas.param_IDs.push_back(sigma_Parameter.indicator.id);
					fit_Rejected_Sigmas.param_Names.push_back("  " + Medium_BlackCircle_Sym + "  <" + multilayer_Tabs->tabText(sigma_Parameter.indicator.tab_Index) + "> " + sigma_Parameter.indicator.full_Name);
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
				print_Reflect_To_File(target, multilayer_Tabs->tabText(tab_Index), counter++);
			}
		}
	}
}

template <typename Type>
void Main_Calculation_Module::print_Reflect_To_File(Data_Element<Type>& data_Element, QString struct_Name, int index)
{
	QString first_Name;
	if(data_Element.curve_Class == INDEPENDENT)	first_Name = "calc_" + struct_Name + "_independent";
	if(data_Element.curve_Class == TARGET)		first_Name = "calc_" + struct_Name + "_target";

	QString path = "";
	if(use_working_directory) path = working_directory + "/";
	if(use_last_directory)	  path = last_directory + "/";

	//-----------------------------------------------------------------------------------------------------
	// 1D
	//-----------------------------------------------------------------------------------------------------

	if(print_1D_Data_On_Recalculation)
	{
		if(	data_Element.calc_Functions.check_Reflectance   ||
			data_Element.calc_Functions.check_Transmittance ||
			data_Element.calc_Functions.check_Absorptance   ||
			data_Element.calc_Functions.check_Scattering	)
		{
			double angular_Coeff = angle_Coefficients_Map.value(data_Element.the_Class->angular_Units);
			double spectral_Coeff = wavelength_Coefficients_Map.value(data_Element.the_Class->spectral_Units);

			vector<double> argument;
			if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Specular_Scan] )
			{
				if( data_Element.the_Class->measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
				{
					argument = data_Element.the_Class->measurement.beam_Theta_0_Angle_Vec;
					for(double arg : argument)	arg = arg/angular_Coeff;
				}
				if( data_Element.the_Class->measurement.argument_Type == argument_Types[Wavelength_Energy] )
				{
					argument = data_Element.the_Class->measurement.lambda_Vec;
					for(double arg : argument)	arg = Global_Variables::wavelength_Energy(data_Element.the_Class->spectral_Units,arg)/spectral_Coeff;
				}
			}
			if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Detector_Scan] )
			{
				argument = data_Element.the_Class->measurement.detector_Theta_Angle_Vec;
				for(double arg : argument)	arg = arg/angular_Coeff;
			}
			if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Rocking_Curve] )
			{
				argument = data_Element.the_Class->measurement.beam_Theta_0_Angle_Vec;
				for(double arg : argument)	arg = arg/angular_Coeff;
			}
			if(	data_Element.the_Class->measurement.measurement_Type == measurement_Types[Offset_Scan]   )
			{
				argument = data_Element.the_Class->measurement.beam_Theta_0_Angle_Vec;
				for(double arg : argument)	arg = arg/angular_Coeff;
			}

			QString name = path + first_Name+"_"+Locale.toString(index)+".txt";
			QFile file(name);
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			out.setFieldAlignment(QTextStream::AlignLeft);

			print_Data(out,
					   argument,
					   data_Element.unwrapped_Reflection,
					   data_Element.calc_Functions,
					   data_Element.the_Class->measurement.polarization
					   );
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
			QString name = path + first_Name+"_"+Locale.toString(index)+"_intensity.txt";
			QFile file(name);
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			out.setFieldAlignment(QTextStream::AlignLeft);
			print_Matrix(intensity_Function, data_Element.calc_Functions, out, data_Element.the_Class->calculated_Values.field_Intensity, data_Element.the_Class->measurement);
			file.close();
		}

		// absorption
		if(data_Element.calc_Functions.check_Joule)
		if(data_Element.the_Class->calculated_Values.absorption_Map.size()>0)
		{
			QString name = path + first_Name+"_"+Locale.toString(index)+"_absorption.txt";
			QFile file(name);
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			out.setFieldAlignment(QTextStream::AlignLeft);
			print_Matrix(joule_Function, data_Element.calc_Functions, out, data_Element.the_Class->calculated_Values.absorption_Map, data_Element.the_Class->measurement);
			file.close();
		}

		// GISAS
		if(data_Element.calc_Functions.check_GISAS)
		if(data_Element.the_Class->calculated_Values.GISAS_Instrumental.size()>0)
		{
			QString name = path + first_Name+"_"+Locale.toString(index)+"_GISAS.txt";
			QFile file(name);
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			out.setFieldAlignment(QTextStream::AlignLeft);
			print_Matrix(gisas_Function, data_Element.calc_Functions, out, data_Element.the_Class->calculated_Values.GISAS_Instrumental, data_Element.the_Class->measurement);
			file.close();
		}
	}
}
template void Main_Calculation_Module::print_Reflect_To_File<Independent_Curve>(Data_Element<Independent_Curve>&, QString, int);
template void Main_Calculation_Module::print_Reflect_To_File<Target_Curve>	   (Data_Element<Target_Curve>&,      QString, int);

void Main_Calculation_Module::print_Data(QTextStream &out, vector<double> &arg, Unwrapped_Reflection* unwrapped_Reflection, Calc_Functions& calc_Functions, double incident_Polarization)
{
	// point as decimal separator
	Locale=QLocale::c();

	// headline
	QString argument = "argument";

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
		// argument
		{
			out << "; polarization = " << Locale.toString(incident_Polarization,'f', 3) << endl;
			out << qSetFieldWidth(arg_Shift-1) << ";";
			out << qSetFieldWidth(width_Short) << argument  << qSetFieldWidth(width_Long);
		}

		if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)
		if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)


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
																		out <<T_mixed;
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
				out << qSetFieldWidth(width_Short) << Locale.toString(arg[i],'f',precision_Arg)  << qSetFieldWidth(width_Long);
			}

			// reflectance
			if(calc_Functions.check_Reflectance)
			if(unwrapped_Reflection->calculated_Values.R_Instrumental.size() == arg.size())
			{
																			out << Locale.toString(unwrapped_Reflection->calculated_Values.R_Instrumental[i],'e',precision_R_T_A_S);
				if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.R_s           [i],'e',precision_R_T_A_S);
				if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.R_p           [i],'e',precision_R_T_A_S);
				if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.Phi_R_s       [i],'f',precision_Phi);
				if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.Phi_R_p       [i],'f',precision_Phi);
			}

			// transmittance
			if(calc_Functions.check_Transmittance)
			if(unwrapped_Reflection->calculated_Values.T_Instrumental.size() == arg.size())
			{
																			out << Locale.toString(unwrapped_Reflection->calculated_Values.T_Instrumental[i],'e',precision_R_T_A_S);
				if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.T_s           [i],'e',precision_R_T_A_S);
				if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.T_p           [i],'e',precision_R_T_A_S);
				if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.Phi_T_s       [i],'f',precision_Phi);
				if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.Phi_T_p       [i],'f',precision_Phi);
			}

			// absorptance
			if(calc_Functions.check_Absorptance)
			if(unwrapped_Reflection->calculated_Values.A.size() == arg.size())
			{
																			out << Locale.toString(unwrapped_Reflection->calculated_Values.A			 [i],'e',precision_R_T_A_S);
				if( (incident_Polarization + 1) > POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.A_s           [i],'e',precision_R_T_A_S);
				if( (incident_Polarization - 1) <-POLARIZATION_TOLERANCE)	out << Locale.toString(unwrapped_Reflection->calculated_Values.A_p           [i],'e',precision_R_T_A_S);
			}

			// scattering
			if(calc_Functions.check_Scattering)
			if(unwrapped_Reflection->calculated_Values.S_Instrumental.size() == arg.size())
			{
																			 out << Locale.toString(unwrapped_Reflection->calculated_Values.S_Instrumental[i],'e',precision_R_T_A_S);
				if(unwrapped_Reflection->multilayer->imperfections_Model.vertical_Correlation != partial_Correlation)
				{
					if((incident_Polarization + 1) > POLARIZATION_TOLERANCE) out << Locale.toString(unwrapped_Reflection->calculated_Values.S_s           [i],'e',precision_R_T_A_S);
					if((incident_Polarization - 1) <-POLARIZATION_TOLERANCE) out << Locale.toString(unwrapped_Reflection->calculated_Values.S_p           [i],'e',precision_R_T_A_S);
				}
			}

			if(i!=arg.size()-1)	out << qSetFieldWidth(arg_Shift) << endl << qSetFieldWidth(width_Short);
		}
	}

	// back to system locale
	Locale = QLocale::system();
}

void Main_Calculation_Module::print_Matrix(QString function, const Calc_Functions& calc_Functions, QTextStream &out, const vector<vector<double>>& matrix, const Data& measurement)
{
	// point as decimal separator
	Locale = QLocale::c();

	if(matrix.size()>0)
	{
		if(function == intensity_Function)	out << "Field intensity distribution" << endl;
		if(function == joule_Function)		out << "Photoabsorption distribution" << endl;
		if(function == gisas_Function)		out << "GISAS data" << endl;
		out << "(" << matrix.front().size() << " x " << matrix.size() << ") points" << endl;

		if(function == gisas_Function)
		{
			out << "Theta    :   (" << Locale.toString(measurement.detector_Theta_Angle.independent.min,'f', 6)
						   << " , " << Locale.toString(measurement.detector_Theta_Angle.independent.max,'f', 6) << ") degrees" << endl;
			out << "Phi      :   (" << Locale.toString(measurement.detector_Phi_Angle.independent.min,'f', 6)
						   << " , " << Locale.toString(measurement.detector_Phi_Angle.independent.max,'f', 6) << ") degrees" << endl;
			out << "Theta_0  :   "  << Locale.toString(measurement.beam_Theta_0_Angle.value,'f', 6) << " degrees" << endl;
			out << "Lambda   :   "  << Locale.toString(measurement.wavelength.value,'f', 6) << " angstrom" << endl;
		}
		if(function == intensity_Function || function == joule_Function)
		{
			out << "Depth    :   (" << Locale.toString(-calc_Functions.field_Ambient_Distance,'f', 2)
						   << " , " << Locale.toString(-calc_Functions.field_Ambient_Distance + (matrix.front().size()-1)*calc_Functions.field_Step,'f', 2) << ") angstrom, step = "
						   << Locale.toString(calc_Functions.field_Step,'f', 2) << " angstrom" << endl;

			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
			out << "Theta_0  :   (" << Locale.toString(measurement.beam_Theta_0_Angle.independent.min,'f', 6)
						   << " , " << Locale.toString(measurement.beam_Theta_0_Angle.independent.max,'f', 6) << ") degrees" << endl;
			out << "Lambda   :   "  << Locale.toString(measurement.wavelength.value,'f', 6) << " angstrom" << endl;
			}
			if(measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
			out << "Lambda   :   (" << Locale.toString(measurement.wavelength.independent.min,'f', 6)
						   << " , " << Locale.toString(measurement.wavelength.independent.max,'f', 6) << ") angstrom" << endl;
			out << "Theta_0  :   "  << Locale.toString(measurement.beam_Theta_0_Angle.value,'f', 6) << " degrees" << endl;
			}
		}
			out << "pol      :   " << Locale.toString(measurement.polarization,'f', 3) << endl;
		out << endl;

		int precision = 6;
		for(size_t j=0; j<matrix.front().size(); ++j)
		{
			for(size_t i=0; i<matrix.size(); ++i)
			{
				out << Locale.toString(matrix[i][j],'e',precision) << "\t";
			}
			out << endl;
		}
	}

	// back to system locale
	Locale = QLocale::system();
}

void Main_Calculation_Module::add_Fit(QString name_Modificator, int run)
{
	// save new trees
	QVector<QTreeWidget*> fitted_Trees(multilayers.size());
	for(int tab_Index=0; tab_Index<multilayers.size(); ++tab_Index)
	{
		fitted_Trees[tab_Index] = calculation_Trees[tab_Index]->real_Struct_Tree;
	}
	global_Multilayer_Approach->add_Fitted_Structure(fitted_Trees, name_Modificator, run);
}

