// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "layer_data_types.h"

Data::Data(QString item_Type_Passed)
{
	id = Global_Definitions::random_Id();

	if(item_Type_Passed != "empty")
	{
		if( (item_Type_Passed != item_Type_Measurement) &&
			(item_Type_Passed != item_Type_Ambient) &&
			(item_Type_Passed != item_Type_Layer) &&
			(item_Type_Passed != item_Type_Substrate) &&
			(item_Type_Passed != item_Type_Multilayer) )
		{
			qInfo() << "\n\nData::Data : wrong item_Type\n\n";
		} else
		{
			item_Type = item_Type_Passed;
		}
		qInfo() << "Created:" << item_Type << id;
	} else
	{
		qInfo() << "Data::Data : empty call" << id;
	}

	// Measurement
	{
		// angle
		{
			probe_Angle.independent.is_Independent = true;
			probe_Angle.independent.min = default_min_angle;
			probe_Angle.independent.max = default_max_angle;
			probe_Angle.independent.num_Points = default_num_angular_points;
			probe_Angle.value = default_angle_value;
			probe_Angle.indicator.whats_This = whats_This_Angle;

			angle_Type = angle_Type_Grazing;
			angle_Value = 90;

			angular_Resolution.value = default_angular_resolution;
			angular_Resolution.independent.min = angular_Resolution.value;
			angular_Resolution.independent.max = angular_Resolution.value;
			angular_Resolution.independent.num_Points = 1;
			angle_Type = default_angle_type;
		}
		// wavelength
		{
			wavelength.independent.is_Independent = true;
			wavelength.independent.min = default_min_wavelength;
			wavelength.independent.max = default_max_wavelength;
			wavelength.independent.num_Points = default_num_spectral_points;
			wavelength.value = default_wavelength_value;
			wavelength.indicator.whats_This = whats_This_Wavelength;

			lambda_Value = 1.54056;

			spectral_Resolution.value = default_spectral_resolution;
			spectral_Resolution.independent.min = spectral_Resolution.value;
			spectral_Resolution.independent.max = spectral_Resolution.value;
			spectral_Resolution.independent.num_Points = 1;

			polarization.value = default_polarization;
			polarization.independent.min = polarization.value;
			polarization.independent.max = polarization.value;
			polarization.independent.num_Points = 1;

			polarization_Sensitivity.value = default_polarization_sensitivity;
			polarization_Sensitivity.independent.min = polarization_Sensitivity.value;
			polarization_Sensitivity.independent.max = polarization_Sensitivity.value;
			polarization_Sensitivity.independent.num_Points = 1;
		}
	}

	// Ambient, Layer, Substrate
	{
		// material
		{
			if(item_Type == item_Type_Ambient)		material = ambient_default_material;
			if(item_Type == item_Type_Layer)		material = layer_default_material;
			if(item_Type == item_Type_Substrate)	material = substrate_default_material;

			approved_Material = material;
		}
		// absolute density
		{
			if(item_Type == item_Type_Ambient)		absolute_Density.value =   ambient_default_absolute_density;
			if(item_Type == item_Type_Layer)		absolute_Density.value =     layer_default_absolute_density;
			if(item_Type == item_Type_Substrate)	absolute_Density.value = substrate_default_absolute_density;
			absolute_Density.fit.is_Fitable = false;
			absolute_Density.fit.min = absolute_Density.value*(1-dispersion);
			absolute_Density.fit.max = absolute_Density.value*(1+dispersion);
			absolute_Density.indicator.whats_This = whats_This_Absolute_Density;
		}
		// relative density
		{
			if(item_Type == item_Type_Ambient)		relative_Density.value =   ambient_default_relative_density;
			if(item_Type == item_Type_Layer)		relative_Density.value =     layer_default_relative_density;
			if(item_Type == item_Type_Substrate)	relative_Density.value = substrate_default_relative_density;
			relative_Density.fit.is_Fitable = false;
			relative_Density.fit.min = relative_Density.value*(1-dispersion);
			relative_Density.fit.max = relative_Density.value*(1+dispersion);
			relative_Density.indicator.whats_This = whats_This_Relative_Density;
		}
		// permittivity
		{
			if(item_Type == item_Type_Ambient)		permittivity.value =   ambient_default_permittivity;
			if(item_Type == item_Type_Layer)		permittivity.value =     layer_default_permittivity;
			if(item_Type == item_Type_Substrate)	permittivity.value = substrate_default_permittivity;
			permittivity.fit.is_Fitable = false;
			permittivity.fit.min = permittivity.value*(1-dispersion);
			permittivity.fit.max = permittivity.value*(1+dispersion);
			permittivity.indicator.whats_This = whats_This_Permittivity;
		}
		// absorption
		{
			if(item_Type == item_Type_Ambient)		absorption.value =   ambient_default_absorption;
			if(item_Type == item_Type_Layer)		absorption.value =     layer_default_absorption;
			if(item_Type == item_Type_Substrate)	absorption.value = substrate_default_absorption;
			absorption.fit.is_Fitable = false;
			absorption.fit.min = absorption.value*(1-dispersion);
			absorption.fit.max = absorption.value*(1+dispersion);
			absorption.indicator.whats_This = whats_This_Absorption;
		}
		// composed material
		{
			if(item_Type == item_Type_Ambient)		composed_Material =   ambient_default_composed;
			if(item_Type == item_Type_Layer)		composed_Material =     layer_default_composed;
			if(item_Type == item_Type_Substrate)	composed_Material = substrate_default_composed;
		}
	}

	// Layer, Substrate
	{
		// sigma
		{
			if(item_Type == item_Type_Layer)		sigma.value =     layer_default_sigma;
			if(item_Type == item_Type_Substrate)	sigma.value = substrate_default_sigma;
			sigma.fit.is_Fitable = false;
			sigma.fit.min = sigma_Dispersion_Min*sigma.value;
			sigma.fit.max = sigma_Dispersion_Max*sigma.value;
			sigma.indicator.whats_This = whats_This_Sigma;
		}
		// interlayer composition
		{
			interlayer_Composition.clear();
			interlayer_Composition.resize(transition_Layer_Functions_Size);
			for(int i=0; i<interlayer_Composition.size(); ++i)
			{
				if(i==0)	{interlayer_Composition[i].enabled=true; }
				else 		{interlayer_Composition[i].enabled=false;}
				{
					interlayer_Composition[i].interlayer.value=1;
					interlayer_Composition[i].interlayer.fit.is_Fitable = false;
					interlayer_Composition[i].interlayer.fit.min = 0;
					interlayer_Composition[i].interlayer.fit.max = 1;
				}
				{
					interlayer_Composition[i].my_Sigma.value=0;
					interlayer_Composition[i].my_Sigma.fit.is_Fitable = false;
					interlayer_Composition[i].my_Sigma.fit.min = sigma_Dispersion_Min*interlayer_Composition[i].my_Sigma.value;
					interlayer_Composition[i].my_Sigma.fit.max = sigma_Dispersion_Max*interlayer_Composition[i].my_Sigma.value;
				}
			}
		}
	}

	// Layer
	{
		// thickness
		{
			thickness.value	= layer_default_thickness;
			thickness.fit.is_Fitable = false;
			thickness.fit.min = thickness.value*(1-dispersion);
			thickness.fit.max = thickness.value*(1+dispersion);
			thickness.indicator.whats_This = whats_This_Thickness;
		}
		// thickness drift
		{
			thickness_Drift.is_Drift_Line = false;
			thickness_Drift.is_Drift_Sine = false;
			thickness_Drift.is_Drift_Rand = false;
			{
				thickness_Drift.drift_Line_Value.value = 0;
				thickness_Drift.drift_Line_Value.fit.min = -thickness_Drift.drift_Line_Value.value*2;
				thickness_Drift.drift_Line_Value.fit.max =  thickness_Drift.drift_Line_Value.value*2;
				thickness_Drift.drift_Line_Value.indicator.whats_This = whats_This_Thickness_Drift_Line_Value;
			}
			{
				thickness_Drift.drift_Rand_Rms.value = 0;
				thickness_Drift.drift_Rand_Rms.fit.min = 0;
				thickness_Drift.drift_Rand_Rms.fit.max = thickness_Drift.drift_Rand_Rms.value*2;
				thickness_Drift.drift_Rand_Rms.indicator.whats_This = whats_This_Thickness_Drift_Rand_Rms;
			}
			{
				thickness_Drift.drift_Sine_Amplitude.value = 0;
				thickness_Drift.drift_Sine_Amplitude.fit.min = 0;
				thickness_Drift.drift_Sine_Amplitude.fit.max = thickness_Drift.drift_Sine_Amplitude.value*2;
				thickness_Drift.drift_Sine_Amplitude.indicator.whats_This = whats_This_Thickness_Drift_Sine_Amplitude;
			}
			{
				thickness_Drift.drift_Sine_Frequency.value = 0.333333333333333333333;
				thickness_Drift.drift_Sine_Frequency.fit.min = thickness_Drift.drift_Sine_Frequency.value*(1-dispersion);
				thickness_Drift.drift_Sine_Frequency.fit.max = thickness_Drift.drift_Sine_Frequency.value*(1+dispersion);
				thickness_Drift.drift_Sine_Frequency.indicator.whats_This = whats_This_Thickness_Drift_Sine_Frequency;
			}
			{
				thickness_Drift.drift_Sine_Phase.value = 0;
				thickness_Drift.drift_Sine_Phase.fit.min = 0;
				thickness_Drift.drift_Sine_Phase.fit.max = 1;
				thickness_Drift.drift_Sine_Phase.indicator.whats_This = whats_This_Thickness_Drift_Sine_Phase;
			}
		}
		// sigma drift
		{
			sigma_Drift.is_Drift_Line = false;
			sigma_Drift.is_Drift_Sine = false;
			sigma_Drift.is_Drift_Rand = false;
			{
				sigma_Drift.drift_Line_Value.value = 0;
				sigma_Drift.drift_Line_Value.fit.min = -thickness_Drift.drift_Line_Value.value*2;
				sigma_Drift.drift_Line_Value.fit.max =  thickness_Drift.drift_Line_Value.value*2;
				sigma_Drift.drift_Line_Value.indicator.whats_This = whats_This_Sigma_Drift_Line_Value;
			}
			{
				sigma_Drift.drift_Rand_Rms.value = 0;
				sigma_Drift.drift_Rand_Rms.fit.min = 0;
				sigma_Drift.drift_Rand_Rms.fit.max = thickness_Drift.drift_Rand_Rms.value*2;
				sigma_Drift.drift_Rand_Rms.indicator.whats_This = whats_This_Sigma_Drift_Rand_Rms;
			}
			{
				sigma_Drift.drift_Sine_Amplitude.value = 0;
				sigma_Drift.drift_Sine_Amplitude.fit.min = 0;
				sigma_Drift.drift_Sine_Amplitude.fit.max = thickness_Drift.drift_Sine_Amplitude.value*2;
				sigma_Drift.drift_Sine_Amplitude.indicator.whats_This = whats_This_Sigma_Drift_Sine_Amplitude;
			}
			{
				sigma_Drift.drift_Sine_Frequency.value = 0.333333333333333333333;
				sigma_Drift.drift_Sine_Frequency.fit.min = thickness_Drift.drift_Sine_Frequency.value*(1-dispersion);
				sigma_Drift.drift_Sine_Frequency.fit.max = thickness_Drift.drift_Sine_Frequency.value*(1+dispersion);
				sigma_Drift.drift_Sine_Frequency.indicator.whats_This = whats_This_Sigma_Drift_Sine_Frequency;
			}
			{
				sigma_Drift.drift_Sine_Phase.value = 0;
				sigma_Drift.drift_Sine_Phase.fit.min = 0;
				sigma_Drift.drift_Sine_Phase.fit.max = 1;
				sigma_Drift.drift_Sine_Phase.indicator.whats_This = whats_This_Sigma_Drift_Sine_Phase;
			}
		}
	}

	// Multilayer
	{
		// num repetition
		{
			num_Repetition.value = stack_default_number_of_repetition;
		}
		// period
		{
			period.value = stack_default_period;
			period.fit.is_Fitable = false;
			period.fit.min = period.value*(1-dispersion);
			period.fit.max = period.value*(1+dispersion);
			period.indicator.whats_This = whats_This_Period;

		}
		// gamma
		{
			gamma.value	= stack_default_gamma;
			gamma.fit.is_Fitable = false;
			gamma.fit.min = 0;
			gamma.fit.max = 1;
			gamma.indicator.whats_This = whats_This_Gamma;
		}
	}
}

void Data::reset_All_IDs()
{
	id = Global_Definitions::random_Id();
	///---------------------------------------------
	// Measurement
	//---------------------------------------------
		// angle
		probe_Angle				.indicator.id = Global_Definitions::random_Id();
		angular_Resolution		.indicator.id = Global_Definitions::random_Id();

		// wavelength
		wavelength				.indicator.id = Global_Definitions::random_Id();
		spectral_Resolution		.indicator.id = Global_Definitions::random_Id();
		polarization			.indicator.id = Global_Definitions::random_Id();
		polarization_Sensitivity.indicator.id = Global_Definitions::random_Id();
	///---------------------------------------------
	///---------------------------------------------
	// Ambient, Layer, Substrate
	//---------------------------------------------
		relative_Density.indicator.id = Global_Definitions::random_Id();
		absolute_Density.indicator.id = Global_Definitions::random_Id();

		permittivity	.indicator.id = Global_Definitions::random_Id();
		absorption		.indicator.id = Global_Definitions::random_Id();

		// material composition
		for(auto& stoich : composition)
		{
			stoich.composition.indicator.id = Global_Definitions::random_Id();
		}
	///---------------------------------------------
	///---------------------------------------------
	// Layer, Substrate
	//---------------------------------------------
		sigma.indicator.id = Global_Definitions::random_Id();
		for(Interlayer& inter : interlayer_Composition)
		{
			inter.interlayer.indicator.id = Global_Definitions::random_Id();
			inter.my_Sigma	.indicator.id = Global_Definitions::random_Id();
		}
	///---------------------------------------------
	///---------------------------------------------
	// Layer
	//---------------------------------------------
		thickness.indicator.id = Global_Definitions::random_Id();

		// thickness drift
		thickness_Drift.drift_Line_Value	.indicator.id = Global_Definitions::random_Id();
		thickness_Drift.drift_Rand_Rms		.indicator.id = Global_Definitions::random_Id();
		thickness_Drift.drift_Sine_Amplitude.indicator.id = Global_Definitions::random_Id();
		thickness_Drift.drift_Sine_Frequency.indicator.id = Global_Definitions::random_Id();
		thickness_Drift.drift_Sine_Phase	.indicator.id = Global_Definitions::random_Id();

		// sigma drift
		sigma_Drift.drift_Line_Value		.indicator.id = Global_Definitions::random_Id();
		sigma_Drift.drift_Rand_Rms			.indicator.id = Global_Definitions::random_Id();
		sigma_Drift.drift_Sine_Amplitude	.indicator.id = Global_Definitions::random_Id();
		sigma_Drift.drift_Sine_Frequency	.indicator.id = Global_Definitions::random_Id();
		sigma_Drift.drift_Sine_Phase		.indicator.id = Global_Definitions::random_Id();
	///---------------------------------------------
	///---------------------------------------------
	// Multilayer
	//---------------------------------------------
		period.indicator.id = Global_Definitions::random_Id();
		gamma .indicator.id = Global_Definitions::random_Id();
}

void Data::calc_Measured_cos2_k()
{
	// cos2
	angle_Value = probe_Angle.value;
	cos2_Value = pow(cos(probe_Angle.value*M_PI/180.),2);

	cos2.resize(angle.size());
	for(int i=0; i<angle.size(); ++i)
	{
		cos2[i] = pow(cos(angle[i]*M_PI/180.),2);
	}

	// k
	lambda_Value = wavelength.value;
	k_Value = 2*M_PI/wavelength.value;

	k.resize(lambda.size());
	for(int i=0; i<lambda.size(); ++i)
	{
		k[i] = 2*M_PI/lambda[i];
	}
}

void Data::calc_Independent_cos2_k()
{
	// cos2
	angle_Value = probe_Angle.value;
	cos2_Value = pow(cos(probe_Angle.value*M_PI/180.),2);
	angle.resize(probe_Angle.independent.num_Points);
	cos2.resize(probe_Angle.independent.num_Points);

	if(probe_Angle.independent.num_Points>1)
	{
		double angle_Step = (probe_Angle.independent.max - probe_Angle.independent.min) / (probe_Angle.independent.num_Points - 1);
		double angle_Temp = probe_Angle.independent.min;
		for(int i=0; i<probe_Angle.independent.num_Points; ++i)
		{
			cos2[i] = pow(cos(angle_Temp*M_PI/180.),2);
			angle[i] = angle_Temp;
			angle_Temp += angle_Step;
		}
	} else
	{
		cos2[0] = cos2_Value;
		angle[0] = angle_Value;
	}

	// k
	lambda_Value = wavelength.value;
	k_Value = 2*M_PI/wavelength.value;
	lambda.resize(wavelength.independent.num_Points);
	k.resize(wavelength.independent.num_Points);

	if(wavelength.independent.num_Points>1)
	{
		double wave_Step = (wavelength.independent.max - wavelength.independent.min) / (wavelength.independent.num_Points - 1);
		double wave = wavelength.independent.min;
		for(int i=0; i<wavelength.independent.num_Points; ++i)
		{
			k[i] = 2*M_PI/wave;
			lambda[i] = wave;
			wave += wave_Step;
		}
	} else
	{
		lambda[0] = lambda_Value;
		k[0] = k_Value;
	}
}

// serialization

/// measurement
QDataStream& operator <<( QDataStream& stream, const Data& data )
{
	return stream
				<< data.item_Type << data.id
			// Measurement
				<< data.probe_Angle << data.cos2 << data.angle << data.cos2_Value << data.angle_Value << data.angular_Resolution << data.angle_Type
				<< data.wavelength << data.k << data.lambda << data.k_Value << data.lambda_Value << data.spectral_Resolution << data.polarization << data.polarization_Sensitivity
			// Ambient, Layer, Substrate
				<< data.composed_Material << data.material << data.approved_Material << data.absolute_Density << data.relative_Density
				<< data.separate_Optical_Constants << data.permittivity << data.absorption << data.composition
			// Layer, Substrate
				<< data.use_PSD << data.common_Sigma << data.sigma << data.interlayer_Composition
			// Layer
				<< data.layer_Index << data.has_Parent << data.thickness << data.thickness_Drift << data.sigma_Drift
			// Multilayer
				<< data.first_Layer_Index << data.last_Layer_Index << data.num_Repetition << data.period << data.gamma;
}
QDataStream& operator >>( QDataStream& stream,		 Data& data )
{
	return stream
				>> data.item_Type >> data.id
			// Measurement
				>> data.probe_Angle >> data.cos2 >> data.angle >> data.cos2_Value >> data.angle_Value >> data.angular_Resolution >> data.angle_Type
				>> data.wavelength >> data.k >> data.lambda >> data.k_Value >> data.lambda_Value >> data.spectral_Resolution >> data.polarization >> data.polarization_Sensitivity
			// Ambient, Layer, Substrate
				>> data.composed_Material >> data.material >> data.approved_Material >> data.absolute_Density >> data.relative_Density
				>> data.separate_Optical_Constants >> data.permittivity >> data.absorption >> data.composition
			// Layer, Substrate
				>> data.use_PSD >> data.common_Sigma >> data.sigma >> data.interlayer_Composition
			// Layer
				>> data.layer_Index >> data.has_Parent >> data.thickness >> data.thickness_Drift >> data.sigma_Drift
			// Multilayer
				>> data.first_Layer_Index >> data.last_Layer_Index >> data.num_Repetition >> data.period >> data.gamma;
}
