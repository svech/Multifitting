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
	interlayer_Composition.clear();

	Stoichiometry stoichiometry;

	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
	default_Values.beginGroup( Substrate_Values );
		material = default_Values.value( "substrate_default_material", 0 ).toString();
		density	 = default_Values.value( "substrate_default_density", 0 ).toDouble();
		composed = default_Values.value( "substrate_default_composed", 0 ).toBool();
		sigma    = default_Values.value( "substrate_default_sigma", 0 ).toDouble();
		stoichiometry.composition = default_Values.value( "substrate_default_interlayer_composition", 0 ).toDouble();
		stoichiometry.type		  = default_Values.value( "substrate_default_interlayer_type", 0 ).toString();
	default_Values.endGroup();
	default_Values.endGroup();

	interlayer_Composition.append(stoichiometry);
}

Extreme_Layer::Extreme_Layer()
{
	interlayer_Composition.clear();
	Stoichiometry stoichiometry;

	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Layer_Values );
			material = default_Values.value( "layer_default_material", 0 ).toString();
			density  = default_Values.value( "layer_default_density", 0 ).toDouble();
			composed = default_Values.value( "layer_default_composed", 0 ).toBool();
			sigma    = default_Values.value( "layer_default_sigma", 0 ).toDouble();
			stoichiometry.composition = default_Values.value( "layer_default_interlayer_composition", 0 ).toDouble();
			stoichiometry.type        = default_Values.value( "layer_default_interlayer_type", 0 ).toString();
			thickness = default_Values.value( "layer_default_thickness", 0 ).toDouble();
		default_Values.endGroup();
	default_Values.endGroup();

	interlayer_Composition.append(stoichiometry);
}

Layer::Layer()
{
	interlayer_Composition.clear();
	Stoichiometry stoichiometry;

	QSettings default_Values(Default_Values_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Layer_Values );
			material = default_Values.value( "layer_default_material", 0 ).toString();
			density  = default_Values.value( "layer_default_density", 0 ).toDouble();
			composed = default_Values.value( "layer_default_composed", 0 ).toBool();
			sigma    = default_Values.value( "layer_default_sigma", 0 ).toDouble();
			stoichiometry.composition = default_Values.value( "layer_default_interlayer_composition", 0 ).toDouble();
			stoichiometry.type        = default_Values.value( "layer_default_interlayer_type", 0 ).toString();
			thickness = default_Values.value( "layer_default_thickness", 0 ).toDouble();
		default_Values.endGroup();
	default_Values.endGroup();

	interlayer_Composition.append(stoichiometry);
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
