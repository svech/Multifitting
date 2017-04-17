// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "layer_data_types.h"

Measurement::Measurement() //-V730
{
	// angle
	probe_Angle.independent.is_Independent = true;
	probe_Angle.independent.min = default_min_angle;
	probe_Angle.independent.max = default_max_angle;
	probe_Angle.independent.num_Points = default_num_angular_points;
	probe_Angle.value = default_angle_value;

	angular_Resolution.value = default_angular_resolution;
	angular_Resolution.independent.min = angular_Resolution.value;
	angular_Resolution.independent.max = angular_Resolution.value;
	angular_Resolution.independent.num_Points = 1;
	angle_Type = default_angle_type;

	// wavelength
	wavelength.independent.is_Independent = true;
	wavelength.independent.min = default_min_wavelength;
	wavelength.independent.max = default_max_wavelength;
	wavelength.independent.num_Points = default_num_spectral_points;
	wavelength.value = default_wavelength_value;

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

void Measurement::calc_Independent_cos2_k()
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

Ambient::Ambient()
{
	material				= ambient_default_material;
	approved_Material		= material;
	absolute_Density.value	= ambient_default_absolute_density;
	relative_Density.value	= ambient_default_relative_density;
	permittivity.value		= ambient_default_permittivity;
	absorption.value		= ambient_default_absorption;
	composed_Material		= ambient_default_composed;
}

Substrate::Substrate()
{
	material				= substrate_default_material;
	approved_Material		= material;
	absolute_Density.value	= substrate_default_absolute_density;
	relative_Density.value	= substrate_default_relative_density;
	permittivity.value		= substrate_default_permittivity;
	absorption.value		= substrate_default_absorption;
	composed_Material		= substrate_default_composed;
	sigma.value				= substrate_default_sigma;

	interlayer_Composition.clear();
	interlayer_Composition.resize(int(transition_Layer_Functions_Size));

	// erf interlayer
	interlayer_Composition[0].enabled=true;
	interlayer_Composition[0].interlayer.value=1;
	interlayer_Composition[0].my_Sigma.value=0;
	// other interlayers
	for(int i=1; i<interlayer_Composition.size(); ++i)
	{
		interlayer_Composition[i].enabled=false;
		interlayer_Composition[i].interlayer.value=1;
		interlayer_Composition[i].my_Sigma.value=0;
	}
}

Extreme_Layer::Extreme_Layer() //-V730
{
	material				= layer_default_material;
	approved_Material		= material;
	absolute_Density.value	= layer_default_absolute_density;
	relative_Density.value	= layer_default_relative_density;
	permittivity.value		= layer_default_permittivity;
	absorption.value		= layer_default_absorption;
	composed_Material		= layer_default_composed;
	sigma.value				= layer_default_sigma;
	thickness.value			= layer_default_thickness;

	interlayer_Composition.clear();
	interlayer_Composition.resize(int(transition_Layer_Functions_Size));

	// erf interlayer
	interlayer_Composition[0].enabled=true;
	interlayer_Composition[0].interlayer.value=1;
	interlayer_Composition[0].my_Sigma.value=0;
	// other interlayers
	for(int i=1; i<interlayer_Composition.size(); ++i)
	{
		interlayer_Composition[i].enabled=false;
		interlayer_Composition[i].interlayer.value=1;
		interlayer_Composition[i].my_Sigma.value=0;
	}
}

Layer::Layer()
{
	material				= layer_default_material;
	approved_Material		= material;
	absolute_Density.value	= layer_default_absolute_density;
	relative_Density.value	= layer_default_relative_density;
	permittivity.value		= layer_default_permittivity;
	absorption.value		= layer_default_absorption;
	composed_Material		= layer_default_composed;
	sigma.value				= layer_default_sigma;
	thickness.value			= layer_default_thickness;

	interlayer_Composition.clear();
	interlayer_Composition.resize(int(transition_Layer_Functions_Size));

	// erf interlayer
	interlayer_Composition[0].enabled=true;
	interlayer_Composition[0].interlayer.value=1;
	interlayer_Composition[0].my_Sigma.value=0;
	// other interlayers
	for(int i=1; i<interlayer_Composition.size(); ++i)
	{
		interlayer_Composition[i].enabled=false;
		interlayer_Composition[i].interlayer.value=1;
		interlayer_Composition[i].my_Sigma.value=0;
	}

	// drift
	thickness_Drift.is_Drift_Line = false;
	thickness_Drift.is_Drift_Sine = false;
	thickness_Drift.is_Drift_Rand = false;

	thickness_Drift.drift_Line_Value.value = 0;
	thickness_Drift.drift_Rand_Rms.value = 0;
	thickness_Drift.drift_Sine_Amplitude.value = 0;
	thickness_Drift.drift_Sine_Frequency.value = 0.333;
	thickness_Drift.drift_Sine_Phase.value = 0;

	sigma_Drift = thickness_Drift;
}

Stack_Content::Stack_Content()
{
	num_Repetition.value= stack_default_number_of_repetition;
	period.value		= stack_default_period;
	gamma.value			= stack_default_gamma;
}

// serialization

/// measurement
QDataStream& operator <<( QDataStream& stream, const Measurement& measurement )
{
	return stream	<< measurement.probe_Angle << measurement.angular_Resolution << measurement.angle_Type
					<< measurement.wavelength << measurement.spectral_Resolution << measurement.polarization << measurement.polarization_Sensitivity;
}
QDataStream& operator >>( QDataStream& stream,		 Measurement& measurement )
{
	return stream	>> measurement.probe_Angle >> measurement.angular_Resolution >> measurement.angle_Type
					>> measurement.wavelength >> measurement.spectral_Resolution >> measurement.polarization >> measurement.polarization_Sensitivity;
}
/// ambient
QDataStream& operator <<( QDataStream& stream, const Ambient& ambient )
{
	return stream	<< ambient.composed_Material << ambient.material << ambient.absolute_Density << ambient.relative_Density << ambient.separate_Optical_Constants << ambient.permittivity << ambient.absorption	<< ambient.composition;
}
QDataStream& operator >>( QDataStream& stream,		 Ambient& ambient )
{
	return stream	>> ambient.composed_Material >> ambient.material >> ambient.absolute_Density >> ambient.relative_Density >> ambient.separate_Optical_Constants >> ambient.permittivity >> ambient.absorption	>> ambient.composition;
}
/// substrate
QDataStream& operator <<( QDataStream& stream, const Substrate& substrate )
{
	return stream	<< substrate.composed_Material << substrate.material << substrate.absolute_Density << substrate.relative_Density << substrate.separate_Optical_Constants << substrate.permittivity << substrate.absorption	<< substrate.composition
					<< substrate.use_PSD << substrate.sigma << substrate.interlayer_Composition;
}
QDataStream& operator >>( QDataStream& stream,		 Substrate& substrate )
{
	return stream	>> substrate.composed_Material >> substrate.material >> substrate.absolute_Density >> substrate.relative_Density >> substrate.separate_Optical_Constants >> substrate.permittivity >> substrate.absorption	>> substrate.composition
					>> substrate.use_PSD >> substrate.sigma >> substrate.interlayer_Composition;
}
/// extreme_Layer
QDataStream& operator <<( QDataStream& stream, const Extreme_Layer& extreme_Layer )
{
	return stream	<< extreme_Layer.composed_Material << extreme_Layer.material << extreme_Layer.absolute_Density << extreme_Layer.relative_Density << extreme_Layer.separate_Optical_Constants <<extreme_Layer.permittivity << extreme_Layer.absorption	<< extreme_Layer.composition
					<< extreme_Layer.use_PSD << extreme_Layer.sigma << extreme_Layer.interlayer_Composition
					<< extreme_Layer.layer_Index << extreme_Layer.thickness;
}
QDataStream& operator >>( QDataStream& stream,	     Extreme_Layer& extreme_Layer )
{
	return stream	>> extreme_Layer.composed_Material >> extreme_Layer.material >> extreme_Layer.absolute_Density >> extreme_Layer.relative_Density >> extreme_Layer.separate_Optical_Constants >> extreme_Layer.permittivity >> extreme_Layer.absorption	>> extreme_Layer.composition
					>> extreme_Layer.use_PSD >> extreme_Layer.sigma >> extreme_Layer.interlayer_Composition
					>> extreme_Layer.layer_Index >> extreme_Layer.thickness;
}
/// layer
QDataStream& operator <<( QDataStream& stream, const Layer& layer )
{
	return stream	<< layer.composed_Material << layer.material << layer.absolute_Density << layer.relative_Density << layer.separate_Optical_Constants << layer.permittivity << layer.absorption	<< layer.composition
					<< layer.use_PSD << layer.sigma << layer.interlayer_Composition
					<< layer.layer_Index << layer.thickness
					<< layer.thickness_Drift << layer.sigma_Drift
					<< layer.if_First << layer. First	<< layer. if_Last << layer. Last;
}
QDataStream& operator >>( QDataStream& stream,		 Layer& layer )
{
	return stream	>> layer.composed_Material >> layer.material >> layer.absolute_Density >> layer.relative_Density >> layer.separate_Optical_Constants >> layer.permittivity >> layer.absorption	>> layer.composition
					>> layer.use_PSD >> layer.sigma >> layer.interlayer_Composition
					>> layer.layer_Index >> layer.thickness
					>> layer.thickness_Drift >> layer.sigma_Drift
					>> layer.if_First >> layer. First	>> layer. if_Last >> layer. Last;
}
/// stack_Content
QDataStream& operator <<( QDataStream& stream, const Stack_Content& stack_Content )
{
	return stream	<<  stack_Content.num_Repetition << stack_Content.period << stack_Content.gamma;
}
QDataStream& operator >>( QDataStream& stream,		 Stack_Content& stack_Content )
{
	return stream	>>  stack_Content.num_Repetition >> stack_Content.period >> stack_Content.gamma;
}
