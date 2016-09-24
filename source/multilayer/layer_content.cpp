#include "layer_content.h"


Ambient::Ambient()
{
	QSettings default_Values(Default_Value_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Ambient_Values );
			material = default_Values.value( "ambient_default_material", 0 ).toString();
			density	 = default_Values.value( "ambient_default_density", 0 ).toDouble();
		default_Values.endGroup();
	default_Values.endGroup();
}

Substrate::Substrate()
{

}

Extreme_Layer::Extreme_Layer()
{

}

Layer::Layer()
{
	QSettings default_Values(Default_Value_Path, QSettings::IniFormat);
	default_Values.beginGroup( Structure_Init_Values );
		default_Values.beginGroup( Layer_Values );
//			default_Values.value( "layer_default_material", 0 ).toString();
//			default_Values.value( "layer_default_density", 0 ).toDouble();
//			default_Values.value( "layer_default_sigma", 0 ).toDouble();
//			default_Values.value( "layer_default_interlayer_composition", 0 ).toDouble();
//			default_Values.value( "layer_default_interlayer_type", 0 ).toString();
//			default_Values.value( "layer_default_thickness", 0 ).toDouble();
		default_Values.endGroup();
	default_Values.endGroup();
}

Stack_Content::Stack_Content()
{

}
