// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "layer_data_types.h"

Data::Data(QString item_Type_Passed)
{
	id = Global_Definitions::generate_Id();

	if(item_Type_Passed != "empty")
	{
		if( (item_Type_Passed != item_Type_Measurement) &&
			(item_Type_Passed != item_Type_Ambient) &&
			(item_Type_Passed != item_Type_Layer) &&
			(item_Type_Passed != item_Type_Substrate) &&
			(item_Type_Passed != item_Type_Multilayer) &&
			(item_Type_Passed != item_Type_Aperiodic) )
		{
			qInfo() << "\n\nData::Data : wrong item_Type\n\n";
		} else
		{
			item_Type = item_Type_Passed;
			parent_Item_Type = NOPARENT;
		}
//		qInfo() << "Created:" << item_Type << id;
	} else
	{
//		qInfo() << "Data::Data : empty call" << id;
	}

	// Measurement
	{
		// angle
		{
			probe_Angle.value = default_angle_value;
			probe_Angle.independent.is_Independent = true;
			probe_Angle.independent.min = default_min_angle;
			probe_Angle.independent.max = default_max_angle;
			probe_Angle.independent.num_Points = default_num_angular_points;
			probe_Angle.indicator.whats_This = whats_This_Angle;
			probe_Angle.indicator.item_Id = id;

			angular_Resolution.value = default_angular_resolution;
			angular_Resolution.independent.min = angular_Resolution.value;
			angular_Resolution.independent.max = angular_Resolution.value;
			angular_Resolution.independent.num_Points = 1;
			angular_Resolution.indicator.whats_This = whats_This_Angular_Resolution;
			angular_Resolution.indicator.item_Id = id;
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
			wavelength.indicator.item_Id = id;

			spectral_Resolution.value = default_spectral_resolution;
			spectral_Resolution.independent.min = spectral_Resolution.value;
			spectral_Resolution.independent.max = spectral_Resolution.value;
			spectral_Resolution.independent.num_Points = 1;
			spectral_Resolution.indicator.whats_This = whats_This_Spectral_Resolution;
			spectral_Resolution.indicator.item_Id = id;

			polarization.value = default_polarization;
			polarization.independent.min = polarization.value;
			polarization.independent.max = polarization.value;
			polarization.independent.num_Points = 1;
			polarization.indicator.whats_This = whats_This_Polarization;
			polarization.indicator.item_Id = id;

			polarization_Sensitivity.value = default_polarization_sensitivity;
			polarization_Sensitivity.independent.min = polarization_Sensitivity.value;
			polarization_Sensitivity.independent.max = polarization_Sensitivity.value;
			polarization_Sensitivity.independent.num_Points = 1;
			polarization_Sensitivity.indicator.whats_This = whats_This_Polarization_Sensitivity;
			polarization_Sensitivity.indicator.item_Id = id;

			background.value = 0;
			background.independent.min = background.value;
			background.independent.max = background.value;
			background.independent.num_Points = 1;
			background.indicator.whats_This = whats_This_Background;
			background.indicator.item_Id = id;
		}
		// geometry
		{
			beam_Size.value = 0.055;
			beam_Size.independent.min = beam_Size.value;
			beam_Size.independent.max = beam_Size.value;
			beam_Size.independent.num_Points = 1;
			beam_Size.indicator.whats_This = whats_This_Beam_Size;
			beam_Size.indicator.item_Id = id;

			beam_Profile_Spreading.value = 2.0;
			beam_Profile_Spreading.independent.min = 1.5;
			beam_Profile_Spreading.independent.max = 3;
			beam_Profile_Spreading.independent.num_Points = 1;
			beam_Profile_Spreading.indicator.whats_This = whats_This_Beam_Profile_Spreading;
			beam_Profile_Spreading.indicator.item_Id = id;

			sample_Size.value = 25;
			sample_Size.independent.min = 20;
			sample_Size.independent.max = 30;
			sample_Size.independent.num_Points = 1;
			sample_Size.indicator.whats_This = whats_This_Sample_Size;
			sample_Size.indicator.item_Id = id;

			sample_Shift.value = 0;
			sample_Shift.independent.min = 0;
			sample_Shift.independent.max = 10;
			sample_Shift.independent.num_Points = 1;
			sample_Shift.indicator.whats_This = whats_This_Sample_Shift;
			sample_Shift.indicator.item_Id = id;
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
			absolute_Density.indicator.item_Id = id;

			absolute_Density.confidence.calc_Conf_Interval = false;
			absolute_Density.confidence.min = absolute_Density.fit.min;
			absolute_Density.confidence.max = absolute_Density.fit.max;
			absolute_Density.confidence.num_Points = default_num_confidence_points;
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
			relative_Density.indicator.item_Id = id;

			relative_Density.confidence.calc_Conf_Interval = false;
			relative_Density.confidence.min = relative_Density.fit.min;
			relative_Density.confidence.max = relative_Density.fit.max;
			relative_Density.confidence.num_Points = default_num_confidence_points;
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
			permittivity.indicator.item_Id = id;

			permittivity.confidence.calc_Conf_Interval = false;
			permittivity.confidence.min = permittivity.fit.min;
			permittivity.confidence.max = permittivity.fit.max;
			permittivity.confidence.num_Points = default_num_confidence_points;
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
			absorption.indicator.item_Id = id;

			absorption.confidence.calc_Conf_Interval = false;
			absorption.confidence.min = absorption.fit.min;
			absorption.confidence.max = absorption.fit.max;
			absorption.confidence.num_Points = default_num_confidence_points;
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
			sigma.fit.min = 1;//sigma_Dispersion_Min*sigma.value;
			sigma.fit.max = max(sigma_Dispersion_Max*sigma.value, 20.);
			sigma.indicator.whats_This = whats_This_Sigma;
			sigma.indicator.item_Id = id;

			sigma.confidence.calc_Conf_Interval = false;
			sigma.confidence.min = sigma.fit.min;
			sigma.confidence.max = sigma.fit.max;
			sigma.confidence.num_Points = default_num_confidence_points;
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
					interlayer_Composition[i].interlayer.indicator.item_Id = id;

					interlayer_Composition[i].interlayer.confidence.calc_Conf_Interval = false;
					interlayer_Composition[i].interlayer.confidence.min = interlayer_Composition[i].interlayer.fit.min;
					interlayer_Composition[i].interlayer.confidence.max = interlayer_Composition[i].interlayer.fit.max;
					interlayer_Composition[i].interlayer.confidence.num_Points = default_num_confidence_points;
				}
				{
					interlayer_Composition[i].my_Sigma.value=0;
					interlayer_Composition[i].my_Sigma.fit.is_Fitable = false;
					interlayer_Composition[i].my_Sigma.fit.min = sigma_Dispersion_Min*interlayer_Composition[i].my_Sigma.value;
					interlayer_Composition[i].my_Sigma.fit.max = sigma_Dispersion_Max*interlayer_Composition[i].my_Sigma.value;
					interlayer_Composition[i].my_Sigma.indicator.item_Id = id;

					interlayer_Composition[i].my_Sigma.confidence.calc_Conf_Interval = false;
					interlayer_Composition[i].my_Sigma.confidence.min = interlayer_Composition[i].my_Sigma.fit.min;
					interlayer_Composition[i].my_Sigma.confidence.max = interlayer_Composition[i].my_Sigma.fit.max;
					interlayer_Composition[i].my_Sigma.confidence.num_Points = default_num_confidence_points;
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
			thickness.fit.max = thickness.value*(2+dispersion);
			thickness.indicator.whats_This = whats_This_Thickness;
			thickness.indicator.item_Id = id;

			thickness.confidence.calc_Conf_Interval = false;
			thickness.confidence.min = thickness.fit.min;
			thickness.confidence.max = thickness.fit.max;
			thickness.confidence.num_Points = default_num_confidence_points;
		}
		// thickness drift
		{
			thickness_Drift.is_Drift_Line = false;
			thickness_Drift.is_Drift_Sine = false;
			thickness_Drift.is_Drift_Rand = false;

			thickness_Drift.show_Drift_Line = true;
			thickness_Drift.show_Drift_Rand = true;
			thickness_Drift.show_Drift_Sine = false;
			{
				thickness_Drift.drift_Line_Value.value = 0;
				thickness_Drift.drift_Line_Value.fit.min = -thickness_Drift.drift_Line_Value.value*2;
				thickness_Drift.drift_Line_Value.fit.max =  thickness_Drift.drift_Line_Value.value*2;
				thickness_Drift.drift_Line_Value.indicator.whats_This = whats_This_Thickness_Drift_Line_Value;
				thickness_Drift.drift_Line_Value.indicator.item_Id = id;

				thickness_Drift.drift_Line_Value.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Line_Value.confidence.min = thickness_Drift.drift_Line_Value.fit.min;
				thickness_Drift.drift_Line_Value.confidence.max = thickness_Drift.drift_Line_Value.fit.max;
				thickness_Drift.drift_Line_Value.confidence.num_Points = default_num_confidence_points;
			}
			{
				thickness_Drift.drift_Rand_Rms.value = 0;
				thickness_Drift.drift_Rand_Rms.fit.min = 0;
				thickness_Drift.drift_Rand_Rms.fit.max = thickness_Drift.drift_Rand_Rms.value*2;
				thickness_Drift.drift_Rand_Rms.indicator.whats_This = whats_This_Thickness_Drift_Rand_Rms;
				thickness_Drift.drift_Rand_Rms.indicator.item_Id = id;

				thickness_Drift.drift_Rand_Rms.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Rand_Rms.confidence.min = thickness_Drift.drift_Rand_Rms.fit.min;
				thickness_Drift.drift_Rand_Rms.confidence.max = thickness_Drift.drift_Rand_Rms.fit.max;
				thickness_Drift.drift_Rand_Rms.confidence.num_Points = default_num_confidence_points;
			}
			{
				thickness_Drift.drift_Sine_Amplitude.value = 0;
				thickness_Drift.drift_Sine_Amplitude.fit.min = 0;
				thickness_Drift.drift_Sine_Amplitude.fit.max = thickness_Drift.drift_Sine_Amplitude.value*2;
				thickness_Drift.drift_Sine_Amplitude.indicator.whats_This = whats_This_Thickness_Drift_Sine_Amplitude;
				thickness_Drift.drift_Sine_Amplitude.indicator.item_Id = id;

				thickness_Drift.drift_Sine_Amplitude.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Sine_Amplitude.confidence.min = thickness_Drift.drift_Sine_Amplitude.fit.min;
				thickness_Drift.drift_Sine_Amplitude.confidence.max = thickness_Drift.drift_Sine_Amplitude.fit.max;
				thickness_Drift.drift_Sine_Amplitude.confidence.num_Points = default_num_confidence_points;
			}
			{
				thickness_Drift.drift_Sine_Frequency.value = 0.333333333333333333333;
				thickness_Drift.drift_Sine_Frequency.fit.min = thickness_Drift.drift_Sine_Frequency.value*(1-dispersion);
				thickness_Drift.drift_Sine_Frequency.fit.max = thickness_Drift.drift_Sine_Frequency.value*(1+dispersion);
				thickness_Drift.drift_Sine_Frequency.indicator.whats_This = whats_This_Thickness_Drift_Sine_Frequency;
				thickness_Drift.drift_Sine_Frequency.indicator.item_Id = id;

				thickness_Drift.drift_Sine_Frequency.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Sine_Frequency.confidence.min = thickness_Drift.drift_Sine_Frequency.fit.min;
				thickness_Drift.drift_Sine_Frequency.confidence.max = thickness_Drift.drift_Sine_Frequency.fit.max;
				thickness_Drift.drift_Sine_Frequency.confidence.num_Points = default_num_confidence_points;
			}
			{
				thickness_Drift.drift_Sine_Phase.value = 0;
				thickness_Drift.drift_Sine_Phase.fit.min = 0;
				thickness_Drift.drift_Sine_Phase.fit.max = 1;
				thickness_Drift.drift_Sine_Phase.indicator.whats_This = whats_This_Thickness_Drift_Sine_Phase;
				thickness_Drift.drift_Sine_Phase.indicator.item_Id = id;

				thickness_Drift.drift_Sine_Phase.confidence.calc_Conf_Interval = false;
				thickness_Drift.drift_Sine_Phase.confidence.min = thickness_Drift.drift_Sine_Phase.fit.min;
				thickness_Drift.drift_Sine_Phase.confidence.max = thickness_Drift.drift_Sine_Phase.fit.max;
				thickness_Drift.drift_Sine_Phase.confidence.num_Points = default_num_confidence_points;
			}
		}
		// sigma drift
		{
			sigma_Drift.is_Drift_Line = false;
			sigma_Drift.is_Drift_Sine = false;
			sigma_Drift.is_Drift_Rand = false;

			sigma_Drift.show_Drift_Line = false;
			sigma_Drift.show_Drift_Rand = false;
			sigma_Drift.show_Drift_Sine = false;
			{
				sigma_Drift.drift_Line_Value.value = 0;
				sigma_Drift.drift_Line_Value.fit.min = -thickness_Drift.drift_Line_Value.value*2;
				sigma_Drift.drift_Line_Value.fit.max =  thickness_Drift.drift_Line_Value.value*2;
				sigma_Drift.drift_Line_Value.indicator.whats_This = whats_This_Sigma_Drift_Line_Value;
				sigma_Drift.drift_Line_Value.indicator.item_Id = id;

				sigma_Drift.drift_Line_Value.confidence.calc_Conf_Interval = false;
				sigma_Drift.drift_Line_Value.confidence.min = sigma_Drift.drift_Line_Value.fit.min;
				sigma_Drift.drift_Line_Value.confidence.max = sigma_Drift.drift_Line_Value.fit.max;
				sigma_Drift.drift_Line_Value.confidence.num_Points = default_num_confidence_points;
			}
			{
				sigma_Drift.drift_Rand_Rms.value = 0;
				sigma_Drift.drift_Rand_Rms.fit.min = 0;
				sigma_Drift.drift_Rand_Rms.fit.max = thickness_Drift.drift_Rand_Rms.value*2;
				sigma_Drift.drift_Rand_Rms.indicator.whats_This = whats_This_Sigma_Drift_Rand_Rms;
				sigma_Drift.drift_Rand_Rms.indicator.item_Id = id;

				sigma_Drift.drift_Rand_Rms.confidence.calc_Conf_Interval = false;
				sigma_Drift.drift_Rand_Rms.confidence.min = sigma_Drift.drift_Rand_Rms.fit.min;
				sigma_Drift.drift_Rand_Rms.confidence.max = sigma_Drift.drift_Rand_Rms.fit.max;
				sigma_Drift.drift_Rand_Rms.confidence.num_Points = default_num_confidence_points;
			}
			{
				sigma_Drift.drift_Sine_Amplitude.value = 0;
				sigma_Drift.drift_Sine_Amplitude.fit.min = 0;
				sigma_Drift.drift_Sine_Amplitude.fit.max = thickness_Drift.drift_Sine_Amplitude.value*2;
				sigma_Drift.drift_Sine_Amplitude.indicator.whats_This = whats_This_Sigma_Drift_Sine_Amplitude;
				sigma_Drift.drift_Sine_Amplitude.indicator.item_Id = id;

				sigma_Drift.drift_Sine_Amplitude.confidence.calc_Conf_Interval = false;
				sigma_Drift.drift_Sine_Amplitude.confidence.min = sigma_Drift.drift_Sine_Amplitude.fit.min;
				sigma_Drift.drift_Sine_Amplitude.confidence.max = sigma_Drift.drift_Sine_Amplitude.fit.max;
				sigma_Drift.drift_Sine_Amplitude.confidence.num_Points = default_num_confidence_points;
			}
			{
				sigma_Drift.drift_Sine_Frequency.value = 0.333333333333333333333;
				sigma_Drift.drift_Sine_Frequency.fit.min = thickness_Drift.drift_Sine_Frequency.value*(1-dispersion);
				sigma_Drift.drift_Sine_Frequency.fit.max = thickness_Drift.drift_Sine_Frequency.value*(1+dispersion);
				sigma_Drift.drift_Sine_Frequency.indicator.whats_This = whats_This_Sigma_Drift_Sine_Frequency;
				sigma_Drift.drift_Sine_Frequency.indicator.item_Id = id;

				sigma_Drift.drift_Sine_Frequency.confidence.calc_Conf_Interval = false;
				sigma_Drift.drift_Sine_Frequency.confidence.min = sigma_Drift.drift_Sine_Frequency.fit.min;
				sigma_Drift.drift_Sine_Frequency.confidence.max = sigma_Drift.drift_Sine_Frequency.fit.max;
				sigma_Drift.drift_Sine_Frequency.confidence.num_Points = default_num_confidence_points;
			}
			{
				sigma_Drift.drift_Sine_Phase.value = 0;
				sigma_Drift.drift_Sine_Phase.fit.min = 0;
				sigma_Drift.drift_Sine_Phase.fit.max = 1;
				sigma_Drift.drift_Sine_Phase.indicator.whats_This = whats_This_Sigma_Drift_Sine_Phase;
				sigma_Drift.drift_Sine_Phase.indicator.item_Id = id;

				sigma_Drift.drift_Sine_Phase.confidence.calc_Conf_Interval = false;
				sigma_Drift.drift_Sine_Phase.confidence.min = sigma_Drift.drift_Sine_Phase.fit.min;
				sigma_Drift.drift_Sine_Phase.confidence.max = sigma_Drift.drift_Sine_Phase.fit.max;
				sigma_Drift.drift_Sine_Phase.confidence.num_Points = default_num_confidence_points;
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
			period.indicator.item_Id = id;

			period.confidence.calc_Conf_Interval = false;
			period.confidence.min = period.fit.min;
			period.confidence.max = period.fit.max;
			period.confidence.num_Points = default_num_confidence_points;
		}
		// gamma
		{
			gamma.value	= stack_default_gamma;
			gamma.fit.is_Fitable = false;
			gamma.fit.min = 0;
			gamma.fit.max = 1;
			gamma.indicator.whats_This = whats_This_Gamma;
			gamma.indicator.item_Id = id;

			gamma.confidence.calc_Conf_Interval = false;
			gamma.confidence.min = gamma.fit.min;
			gamma.confidence.max = gamma.fit.max;
			gamma.confidence.num_Points = default_num_confidence_points;
		}
	}
}

void Data::reset_All_IDs()
{
	id = Global_Definitions::generate_Id();
	///---------------------------------------------
	// Measurement
	//---------------------------------------------
		// angle
		probe_Angle				.indicator.id = Global_Definitions::generate_Id(); probe_Angle				.indicator.item_Id = id;	probe_Angle				.coupled.clear_Coupled();
		angular_Resolution		.indicator.id = Global_Definitions::generate_Id(); angular_Resolution		.indicator.item_Id = id;	angular_Resolution		.coupled.clear_Coupled();

		// wavelength
		wavelength				.indicator.id = Global_Definitions::generate_Id(); wavelength				.indicator.item_Id = id;	wavelength				.coupled.clear_Coupled();
		spectral_Resolution		.indicator.id = Global_Definitions::generate_Id(); spectral_Resolution		.indicator.item_Id = id;	spectral_Resolution		.coupled.clear_Coupled();
		polarization			.indicator.id = Global_Definitions::generate_Id(); polarization				.indicator.item_Id = id;	polarization			.coupled.clear_Coupled();
		polarization_Sensitivity.indicator.id = Global_Definitions::generate_Id(); polarization_Sensitivity	.indicator.item_Id = id;	polarization_Sensitivity.coupled.clear_Coupled();
		background				.indicator.id = Global_Definitions::generate_Id(); background				.indicator.item_Id = id;	background				.coupled.clear_Coupled();

		// geometry
		beam_Size				.indicator.id = Global_Definitions::generate_Id(); beam_Size				.indicator.item_Id = id;	beam_Size				.coupled.clear_Coupled();
		beam_Profile_Spreading	.indicator.id = Global_Definitions::generate_Id(); beam_Profile_Spreading	.indicator.item_Id = id;	beam_Profile_Spreading	.coupled.clear_Coupled();
		sample_Size				.indicator.id = Global_Definitions::generate_Id(); sample_Size				.indicator.item_Id = id;	sample_Size				.coupled.clear_Coupled();
		sample_Shift			.indicator.id = Global_Definitions::generate_Id(); sample_Shift				.indicator.item_Id = id;	sample_Shift			.coupled.clear_Coupled();
	///---------------------------------------------
	///---------------------------------------------
	// Ambient, Layer, Substrate
	//---------------------------------------------
		relative_Density.indicator.id = Global_Definitions::generate_Id(); relative_Density	.indicator.item_Id = id;		relative_Density	.coupled.clear_Coupled();
		absolute_Density.indicator.id = Global_Definitions::generate_Id(); absolute_Density	.indicator.item_Id = id;		absolute_Density	.coupled.clear_Coupled();

		permittivity	.indicator.id = Global_Definitions::generate_Id(); permittivity		.indicator.item_Id = id;		permittivity		.coupled.clear_Coupled();
		absorption		.indicator.id = Global_Definitions::generate_Id(); absorption		.indicator.item_Id = id;		absorption			.coupled.clear_Coupled();

		// material composition
		for(auto& stoich : composition)
		{
			stoich.composition.indicator.id = Global_Definitions::generate_Id(); stoich.composition.indicator.item_Id = id;	stoich.composition	.coupled.clear_Coupled();
		}
	///---------------------------------------------
	///---------------------------------------------
	// Layer, Substrate
	//---------------------------------------------
		sigma.indicator.id = Global_Definitions::generate_Id(); sigma.indicator.item_Id = id;				sigma.coupled.clear_Coupled();
		for(Interlayer& inter : interlayer_Composition)
		{
			inter.interlayer.indicator.id = Global_Definitions::generate_Id(); inter.interlayer	.indicator.item_Id = id;	inter.interlayer.coupled.clear_Coupled();
			inter.my_Sigma	.indicator.id = Global_Definitions::generate_Id(); inter.my_Sigma	.indicator.item_Id = id;	inter.my_Sigma.	 coupled.clear_Coupled();
		}
	///---------------------------------------------
	///---------------------------------------------
	// Layer
	//---------------------------------------------
		thickness.indicator.id = Global_Definitions::generate_Id(); thickness.indicator.item_Id = id;		thickness.coupled.clear_Coupled();

		// thickness drift
		thickness_Drift.drift_Line_Value	.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Line_Value		.indicator.item_Id = id;	thickness_Drift.drift_Line_Value.		coupled.clear_Coupled();
		thickness_Drift.drift_Rand_Rms		.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Rand_Rms		.indicator.item_Id = id;	thickness_Drift.drift_Rand_Rms.			coupled.clear_Coupled();
		thickness_Drift.drift_Sine_Amplitude.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Sine_Amplitude	.indicator.item_Id = id;	thickness_Drift.drift_Sine_Amplitude.	coupled.clear_Coupled();
		thickness_Drift.drift_Sine_Frequency.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Sine_Frequency	.indicator.item_Id = id;	thickness_Drift.drift_Sine_Frequency.	coupled.clear_Coupled();
		thickness_Drift.drift_Sine_Phase	.indicator.id = Global_Definitions::generate_Id(); thickness_Drift.drift_Sine_Phase		.indicator.item_Id = id;	thickness_Drift.drift_Sine_Phase.		coupled.clear_Coupled();

		// sigma drift
		sigma_Drift.drift_Line_Value		.indicator.id = Global_Definitions::generate_Id(); sigma_Drift.drift_Line_Value		.indicator.item_Id = id;		sigma_Drift.drift_Line_Value	.coupled.clear_Coupled();
		sigma_Drift.drift_Rand_Rms			.indicator.id = Global_Definitions::generate_Id(); sigma_Drift.drift_Rand_Rms		.indicator.item_Id = id;		sigma_Drift.drift_Rand_Rms		.coupled.clear_Coupled();
		sigma_Drift.drift_Sine_Amplitude	.indicator.id = Global_Definitions::generate_Id(); sigma_Drift.drift_Sine_Amplitude	.indicator.item_Id = id;		sigma_Drift.drift_Sine_Amplitude.coupled.clear_Coupled();
		sigma_Drift.drift_Sine_Frequency	.indicator.id = Global_Definitions::generate_Id(); sigma_Drift.drift_Sine_Frequency	.indicator.item_Id = id;		sigma_Drift.drift_Sine_Frequency.coupled.clear_Coupled();
		sigma_Drift.drift_Sine_Phase		.indicator.id = Global_Definitions::generate_Id(); sigma_Drift.drift_Sine_Phase		.indicator.item_Id = id;		sigma_Drift.drift_Sine_Phase	.coupled.clear_Coupled();
	///---------------------------------------------
	///---------------------------------------------
	// Multilayer, Aperiodic
	//---------------------------------------------
		period.indicator.id = Global_Definitions::generate_Id(); period	.indicator.item_Id = id;	period.coupled.clear_Coupled();
		gamma .indicator.id = Global_Definitions::generate_Id(); gamma	.indicator.item_Id = id;	gamma. coupled.clear_Coupled();
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

void Data::fill_Potentially_Fitable_Parameters_Vector()
{
	potentially_Fitable_Parameters.clear();

	///---------------------------------------------
	// Measurement
	//---------------------------------------------
		// angle
		potentially_Fitable_Parameters.push_back(&probe_Angle);
		potentially_Fitable_Parameters.push_back(&angular_Resolution);

		// wavelength
		potentially_Fitable_Parameters.push_back(&wavelength);
		potentially_Fitable_Parameters.push_back(&spectral_Resolution);
		potentially_Fitable_Parameters.push_back(&polarization);
		potentially_Fitable_Parameters.push_back(&polarization_Sensitivity);
		potentially_Fitable_Parameters.push_back(&background);

		// geometry
		potentially_Fitable_Parameters.push_back(&beam_Size);
		potentially_Fitable_Parameters.push_back(&beam_Profile_Spreading);
		potentially_Fitable_Parameters.push_back(&sample_Size);
		potentially_Fitable_Parameters.push_back(&sample_Shift);
	///---------------------------------------------
	///---------------------------------------------
	// Ambient, Layer, Substrate
	//---------------------------------------------
		if(!composed_Material)
			potentially_Fitable_Parameters.push_back(&relative_Density);
		else
			potentially_Fitable_Parameters.push_back(&absolute_Density);

		// tabular material
//		all_Parameters.push_back(&permittivity);
//		all_Parameters.push_back(&absorption);

		// material composition
		if(composition.size()>1 && composed_Material)
		{
			for(Stoichiometry& stoichiometry : composition)
			{
				potentially_Fitable_Parameters.push_back(&stoichiometry.composition);
			}
		}
	///---------------------------------------------
	///---------------------------------------------
	// Layer, Substrate
	//---------------------------------------------
		int enabled_Counter=0;
		for(Interlayer& interlayer : interlayer_Composition)
		{
			if(interlayer.enabled)
			{
				++enabled_Counter;
				potentially_Fitable_Parameters.push_back(&interlayer.interlayer);

				if(!common_Sigma)
					potentially_Fitable_Parameters.push_back(&interlayer.my_Sigma);
			}
		}

		if(common_Sigma && enabled_Counter>0)
			potentially_Fitable_Parameters.push_back(&sigma);

	///---------------------------------------------
	///---------------------------------------------
	// Layer
	//---------------------------------------------
		potentially_Fitable_Parameters.push_back(&thickness);

		// thickness drift
		if(thickness_Drift.is_Drift_Line)
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Line_Value);

		if(thickness_Drift.is_Drift_Sine)
		{
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Sine_Amplitude);
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Sine_Frequency);
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Sine_Phase);
		}

		if(thickness_Drift.is_Drift_Rand)
			potentially_Fitable_Parameters.push_back(&thickness_Drift.drift_Rand_Rms);

		// sigma drift
		if(sigma_Drift.is_Drift_Line)
			potentially_Fitable_Parameters.push_back(&sigma_Drift.drift_Line_Value);

		if(sigma_Drift.is_Drift_Sine)
		{
			potentially_Fitable_Parameters.push_back(&sigma_Drift.drift_Sine_Amplitude);
			potentially_Fitable_Parameters.push_back(&sigma_Drift.drift_Sine_Frequency);
			potentially_Fitable_Parameters.push_back(&sigma_Drift.drift_Sine_Phase);
		}

		if(sigma_Drift.is_Drift_Rand)
			potentially_Fitable_Parameters.push_back(&sigma_Drift.drift_Rand_Rms);

	///---------------------------------------------
	///---------------------------------------------
	// Multilayer
	//---------------------------------------------
		potentially_Fitable_Parameters.push_back(&period);
		potentially_Fitable_Parameters.push_back(&gamma);
}

// serialization

/// measurement
QDataStream& operator <<( QDataStream& stream, const Data& data )
{
	return stream				
				<< data.parent_Item_Type << data.item_Type << data.id
				<< data.item_Enabled		// since 1.7.7
			// Measurement
				<< data.probe_Angle << data.cos2 << data.angle << data.cos2_Value << data.angle_Value << data.angular_Resolution << data.angle_Type
				<< data.wavelength << data.k << data.lambda << data.k_Value << data.lambda_Value << data.spectral_Resolution << data.polarization << data.polarization_Sensitivity << data.background
				<< data.beam_Size << data.beam_Profile_Spreading << data.sample_Size  << data.sample_Shift
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
	stream		>> data.parent_Item_Type >> data.item_Type >> data.id;
	if(Global_Variables::check_Loaded_Version(1,7,7))
	{stream >> data.item_Enabled; }	// since 1.7.7

			// Measurement
	stream		>> data.probe_Angle >> data.cos2 >> data.angle >> data.cos2_Value >> data.angle_Value >> data.angular_Resolution >> data.angle_Type
				>> data.wavelength >> data.k >> data.lambda >> data.k_Value >> data.lambda_Value >> data.spectral_Resolution >> data.polarization >> data.polarization_Sensitivity >> data.background
				>> data.beam_Size >> data.beam_Profile_Spreading >> data.sample_Size  >> data.sample_Shift
			// Ambient, Layer, Substrate
				>> data.composed_Material >> data.material >> data.approved_Material >> data.absolute_Density >> data.relative_Density
				>> data.separate_Optical_Constants >> data.permittivity >> data.absorption >> data.composition
			// Layer, Substrate
				>> data.use_PSD >> data.common_Sigma >> data.sigma >> data.interlayer_Composition
			// Layer
				>> data.layer_Index >> data.has_Parent >> data.thickness >> data.thickness_Drift >> data.sigma_Drift
			// Multilayer
				>> data.first_Layer_Index >> data.last_Layer_Index >> data.num_Repetition >> data.period >> data.gamma;
	return stream;
}
