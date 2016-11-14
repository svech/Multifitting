#include "layer_data_types.h"

Measurement::Measurement()
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

Ambient::Ambient()
{
	material			= ambient_default_material;
	density.value		= ambient_default_density;
	permittivity.value	= ambient_default_permittivity;
	absorption.value	= ambient_default_absorption;
	composed_Material	= ambient_default_composed;
}

Substrate::Substrate()
{
	material		  = substrate_default_material;
	density.value	  = substrate_default_density;
	permittivity.value= substrate_default_permittivity;
	absorption.value  = substrate_default_absorption;
	composed_Material = substrate_default_composed;
	sigma.value		  = substrate_default_sigma;

	interlayer_Composition.clear();
	interlayer_Composition.resize(transition_Layer_Functions.size());

	// erf interlayer
	interlayer_Composition[0].enabled=true;
	interlayer_Composition[0].interlayer.value=1;
	// other interlayers
	for(int i=1; i<interlayer_Composition.size(); ++i)
	{
		interlayer_Composition[i].enabled=false;
		interlayer_Composition[i].interlayer.value=1;
	}
}

Extreme_Layer::Extreme_Layer()
{
	material			= layer_default_material;
	density.value		= layer_default_density;
	permittivity.value	= layer_default_permittivity;
	absorption.value	= layer_default_absorption;
	composed_Material	= layer_default_composed;
	sigma.value			= layer_default_sigma;
	thickness.value		= layer_default_thickness;

	interlayer_Composition.clear();
	interlayer_Composition.resize(transition_Layer_Functions.size());

	// erf interlayer
	interlayer_Composition[0].enabled=true;
	interlayer_Composition[0].interlayer.value=1;
	// other interlayers
	for(int i=1; i<interlayer_Composition.size(); ++i)
	{
		interlayer_Composition[i].enabled=false;
		interlayer_Composition[i].interlayer.value=1;
	}
}

Layer::Layer()
{
	material			= layer_default_material;
	density.value		= layer_default_density;
	permittivity.value	= layer_default_permittivity;
	absorption.value	= layer_default_absorption;
	composed_Material	= layer_default_composed;
	sigma.value			= layer_default_sigma;
	thickness.value		= layer_default_thickness;

	interlayer_Composition.clear();
	interlayer_Composition.resize(transition_Layer_Functions.size());

	// erf interlayer
	interlayer_Composition[0].enabled=true;
	interlayer_Composition[0].interlayer.value=1;
	// other interlayers
	for(int i=1; i<interlayer_Composition.size(); ++i)
	{
		interlayer_Composition[i].enabled=false;
		interlayer_Composition[i].interlayer.value=1;
	}
}

Stack_Content::Stack_Content()
{
	num_Repetition.value= stack_default_number_of_repetition;
	period.value		= stack_default_period;
	gamma.value			= stack_default_gamma;
}

// serialization

/// ambient
QDataStream& operator <<( QDataStream& stream, const Ambient& ambient )
{
	return stream	<< ambient.composed_Material << ambient.material << ambient.density << ambient.separate_Optical_Constants << ambient.permittivity << ambient.absorption	<< ambient.composition;
}
QDataStream& operator >>( QDataStream& stream,		 Ambient& ambient )
{
	return stream	>> ambient.composed_Material >> ambient.material >> ambient.density >> ambient.separate_Optical_Constants >> ambient.permittivity >> ambient.absorption	>> ambient.composition;
}
/// substrate
QDataStream& operator <<( QDataStream& stream, const Substrate& substrate )
{
	return stream	<< substrate.composed_Material << substrate.material << substrate.density << substrate.separate_Optical_Constants << substrate.permittivity << substrate.absorption	<< substrate.composition
					<< substrate.use_PSD << substrate.sigma << substrate.interlayer_Composition;
}
QDataStream& operator >>( QDataStream& stream,		 Substrate& substrate )
{
	return stream	>> substrate.composed_Material >> substrate.material >> substrate.density >> substrate.separate_Optical_Constants >> substrate.permittivity >> substrate.absorption	>> substrate.composition
					>> substrate.use_PSD >> substrate.sigma >> substrate.interlayer_Composition;
}
/// extreme_Layer
QDataStream& operator <<( QDataStream& stream, const Extreme_Layer& extreme_Layer )
{
	return stream	<< extreme_Layer.composed_Material << extreme_Layer.material << extreme_Layer.density << extreme_Layer.separate_Optical_Constants <<extreme_Layer.permittivity << extreme_Layer.absorption	<< extreme_Layer.composition
					<< extreme_Layer.use_PSD << extreme_Layer.sigma << extreme_Layer.interlayer_Composition
					<< extreme_Layer.layer_Index << extreme_Layer.thickness;
}
QDataStream& operator >>( QDataStream& stream,	     Extreme_Layer& extreme_Layer )
{
	return stream	>> extreme_Layer.composed_Material >> extreme_Layer.material >> extreme_Layer.density >> extreme_Layer.separate_Optical_Constants >> extreme_Layer.permittivity >> extreme_Layer.absorption	>> extreme_Layer.composition
					>> extreme_Layer.use_PSD >> extreme_Layer.sigma >> extreme_Layer.interlayer_Composition
					>> extreme_Layer.layer_Index >> extreme_Layer.thickness;
}
/// layer
QDataStream& operator <<( QDataStream& stream, const Layer& layer )
{
	return stream	<< layer.composed_Material << layer.material << layer.density << layer.separate_Optical_Constants << layer.permittivity << layer.absorption	<< layer.composition
					<< layer.use_PSD << layer.sigma << layer.interlayer_Composition
					<< layer.layer_Index << layer.thickness
					<< layer.drift_Model << layer.drift_Coefficients << layer.if_First << layer. First	<< layer. if_Last << layer. Last;
}
QDataStream& operator >>( QDataStream& stream,		 Layer& layer )
{
	return stream	>> layer.composed_Material >> layer.material >> layer.density >> layer.separate_Optical_Constants >> layer.permittivity >> layer.absorption	>> layer.composition
					>> layer.use_PSD >> layer.sigma >> layer.interlayer_Composition
					>> layer.layer_Index >> layer.thickness
					>> layer. drift_Model >> layer.drift_Coefficients >> layer.if_First >> layer. First	>> layer. if_Last >> layer. Last;
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