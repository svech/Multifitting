#include "layer_content.h"


Ambient::Ambient()
{
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Ambient_Values );
			material = default_Values.value( "ambient_default_material", 0 ).toString();
			density	 = default_Values.value( "ambient_default_density", 0 ).toDouble();
			composed = default_Values.value( "ambient_default_composed", 0 ).toBool();
		default_Values.endGroup();
	default_Values.endGroup();
}

Substrate::Substrate()
{	
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
	default_Values.beginGroup( Substrate_Values );
		material = default_Values.value( "substrate_default_material", 0 ).toString();
		density	 = default_Values.value( "substrate_default_density", 0 ).toDouble();
		composed = default_Values.value( "substrate_default_composed", 0 ).toBool();
		sigma    = default_Values.value( "substrate_default_sigma", 0 ).toDouble();
	default_Values.endGroup();
	default_Values.endGroup();

	interlayer_Composition.clear();
	interlayer_Composition.resize(transition_Layer_Functions.size());

	// erf interlayer
	interlayer_Composition[0].enabled=true;
	interlayer_Composition[0].interlayer=1;
	// other interlayers
	for(int i=1; i<interlayer_Composition.size(); ++i)
	{
		interlayer_Composition[i].enabled=false;
		interlayer_Composition[i].interlayer=1;
	}
}

Extreme_Layer::Extreme_Layer()
{
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Layer_Values );
			material = default_Values.value( "layer_default_material", 0 ).toString();
			density  = default_Values.value( "layer_default_density", 0 ).toDouble();
			composed = default_Values.value( "layer_default_composed", 0 ).toBool();
			sigma    = default_Values.value( "layer_default_sigma", 0 ).toDouble();
			thickness = default_Values.value( "layer_default_thickness", 0 ).toDouble();
		default_Values.endGroup();
	default_Values.endGroup();

	interlayer_Composition.clear();
	interlayer_Composition.resize(transition_Layer_Functions.size());

	// erf interlayer
	interlayer_Composition[0].enabled=true;
	interlayer_Composition[0].interlayer=1;
	// other interlayers
	for(int i=1; i<interlayer_Composition.size(); ++i)
	{
		interlayer_Composition[i].enabled=false;
		interlayer_Composition[i].interlayer=1;
	}
}

Layer::Layer()
{
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Layer_Values );
			material = default_Values.value( "layer_default_material", 0 ).toString();
			density  = default_Values.value( "layer_default_density", 0 ).toDouble();
			composed = default_Values.value( "layer_default_composed", 0 ).toBool();
			sigma    = default_Values.value( "layer_default_sigma", 0 ).toDouble();
			thickness = default_Values.value( "layer_default_thickness", 0 ).toDouble();
		default_Values.endGroup();
	default_Values.endGroup();

	interlayer_Composition.clear();
	interlayer_Composition.resize(transition_Layer_Functions.size());

	// erf interlayer
	interlayer_Composition[0].enabled=true;
	interlayer_Composition[0].interlayer=1;
	// other interlayers
	for(int i=1; i<interlayer_Composition.size(); ++i)
	{
		interlayer_Composition[i].enabled=false;
		interlayer_Composition[i].interlayer=1;
	}
}

Stack_Content::Stack_Content()
{
	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
	default_Values.beginGroup( Stack_Values );
		num_Repetition = default_Values.value( "stack_default_number_of_repetition", 0 ).toInt();
		period		   = default_Values.value( "stack_default_period", 0 ).toDouble();
		gamma		   = default_Values.value( "stack_default_gamma", 0 ).toDouble();
	default_Values.endGroup();
}
