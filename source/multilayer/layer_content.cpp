#include "layer_content.h"

Probe_Angle::Probe_Angle()
{
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Parameters_Init_Values );
		default_Values.beginGroup( Independent_Values );
			default_Values.beginGroup( Angle_Values );
				int default_num_points = default_Values.value( "default_num_points", -100 ).toInt();
				double default_min_angle = default_Values.value( "default_min_angle", -100.500 ).toDouble();
				double default_max_angle = default_Values.value( "default_max_angle", -100.500 ).toDouble();
				Angle_Type default_type = static_cast<Angle_Type>(default_Values.value( "default_type", 0 ).toInt());
				double default_angular_resolution = default_Values.value( "default_angular_resolution", 0 ).toDouble();
			default_Values.endGroup();
		default_Values.endGroup();
	default_Values.endGroup();

	angle.is_Independent = true;
	angle.num_Points = default_num_points;
	angle.min = default_min_angle;
	angle.max = default_max_angle;
	type = default_type;
	angular_Resolution = default_angular_resolution;
}

Radiation::Radiation()
{
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Parameters_Init_Values );
		default_Values.beginGroup( Independent_Values );
			default_Values.beginGroup( Radiation_Values );
				int default_num_points = default_Values.value( "default_num_points", -100 ).toInt();
				double default_min_angle = default_Values.value( "default_min_wavelength", -100.500 ).toDouble();
				double default_max_angle = default_Values.value( "default_max_wavelength", -100.500 ).toDouble();
				double default_spectral_resolution = default_Values.value( "default_spectral_resolution", -100.500 ).toDouble();
				double default_polarization = default_Values.value( "default_polarization", -100.500 ).toDouble();
				double default_polarization_sensitivity = default_Values.value( "default_polarization_sensitivity", -100.500 ).toDouble();
			default_Values.endGroup();
		default_Values.endGroup();
	default_Values.endGroup();

	radiation.is_Independent = true;
	radiation.num_Points = default_num_points;
	radiation.min = default_min_angle;
	radiation.max = default_max_angle;
	spectral_Resolution = default_spectral_resolution;
	polarization = default_polarization;
	polarization_Sensitivity = default_polarization_sensitivity;
}

Ambient::Ambient()
{
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Ambient_Values );
			material = default_Values.value( "ambient_default_material", 0 ).toString();
			density.value = default_Values.value( "ambient_default_density", 0 ).toDouble();
			composed_Material = default_Values.value( "ambient_default_composed", 0 ).toBool();
		default_Values.endGroup();
	default_Values.endGroup();
}

Substrate::Substrate()
{	
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
	default_Values.beginGroup( Substrate_Values );
		material = default_Values.value( "substrate_default_material", 0 ).toString();
		density.value	 = default_Values.value( "substrate_default_density", 0 ).toDouble();
		composed_Material = default_Values.value( "substrate_default_composed", 0 ).toBool();
		sigma.value    = default_Values.value( "substrate_default_sigma", 0 ).toDouble();
	default_Values.endGroup();
	default_Values.endGroup();

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
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Layer_Values );
			material = default_Values.value( "layer_default_material", 0 ).toString();
			density.value  = default_Values.value( "layer_default_density", 0 ).toDouble();
			composed_Material = default_Values.value( "layer_default_composed", 0 ).toBool();
			sigma.value    = default_Values.value( "layer_default_sigma", 0 ).toDouble();
			thickness.value = default_Values.value( "layer_default_thickness", 0 ).toDouble();
		default_Values.endGroup();
	default_Values.endGroup();

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
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Layer_Values );
			material = default_Values.value( "layer_default_material", 0 ).toString();
			density.value  = default_Values.value( "layer_default_density", 0 ).toDouble();
			composed_Material = default_Values.value( "layer_default_composed", 0 ).toBool();
			sigma.value    = default_Values.value( "layer_default_sigma", 0 ).toDouble();
			thickness.value = default_Values.value( "layer_default_thickness", 0 ).toDouble();
		default_Values.endGroup();
	default_Values.endGroup();

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
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
	default_Values.beginGroup( Stack_Values );
		num_Repetition.value = default_Values.value( "stack_default_number_of_repetition", 0 ).toInt();
		period.value   = default_Values.value( "stack_default_period", 0 ).toDouble();
		gamma.value	   = default_Values.value( "stack_default_gamma", 0 ).toDouble();
	default_Values.endGroup();
}

