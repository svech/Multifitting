#include "independent_curve.h"

Independent_Curve::Independent_Curve(QWidget *parent) :
	QWidget(parent)
{
	{
		plot_Options.color=QColor(0, 0, 255);
		plot_Options.scatter_Shape = QCPScatterStyle::ssDisc;
		plot_Options.scatter_Size=0;
		plot_Options.thickness=2;
	}
	create_Main_Layout();
}

void Independent_Curve::create_Main_Layout()
{
	main_Layout = new QHBoxLayout(this);

	setup_Button = new QPushButton("Set up");
		setup_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	main_Layout->addWidget(setup_Button,0,Qt::AlignLeft);

	description_Label  = new QLabel("<no description>");
	main_Layout->addWidget(description_Label);
}

QString Independent_Curve::enlarge_Tab_Name()
{
	if(calc_Functions.check_Enabled)
	{
		QString suffix = "  ";
		if(calc_Functions.check_Reflectance)	suffix = suffix+symbol_R+",";
		if(calc_Functions.check_Transmittance)  suffix = suffix+symbol_T+",";
		if(calc_Functions.check_Absorptance)	suffix = suffix+symbol_A+",";
		if(calc_Functions.check_Field)			suffix = suffix+symbol_F+",";
		if(calc_Functions.check_Joule)			suffix = suffix+symbol_J+",";
		if(calc_Functions.check_User)			suffix = suffix+symbol_U+",";
		return suffix.remove(-1,1);
	} else
	{
		return "";
	}
}

void Independent_Curve::refresh_Description_Label()
{

}

Independent_Curve& Independent_Curve::operator =(const Independent_Curve& referent_Independent_Curve)
{
//	tab_Name = referent_Independent_Variables.tab_Name;

//	calculated_Values = referent_Independent_Variables.calculated_Values;

//	calc_Functions = referent_Independent_Variables.calc_Functions;
//	plot_Options = referent_Independent_Variables.plot_Options;

//	// I don't know how to copy all things. So I just don't copy everything.

//	// --------------------------------------------------------------------------
//	// make active/inactive corresponding independent variable (from measurement)
//	// --------------------------------------------------------------------------
//	/// probe angle
//	QVariant var;
//	int i=0;

//	// set active/inactive
//	Independent_Indicator probe_Ang_Ind          =                                independent_Variables_List->item(i)->data(Qt::UserRole).value<Independent_Indicator>();
//	Independent_Indicator referent_Probe_Ang_Ind = referent_Independent_Variables.independent_Variables_List->item(i)->data(Qt::UserRole).value<Independent_Indicator>();
//	probe_Ang_Ind.is_Active = referent_Probe_Ang_Ind.is_Active;
//	var.setValue(probe_Ang_Ind);
//	independent_Variables_List->item(i)->setData(Qt::UserRole, var);

//	// change text
//	QString current_Item_Text = independent_Variables_List->item(i)->text();
//	QStringList item_Text_List = current_Item_Text.split(active, QString::SkipEmptyParts);
//	independent_Variables_List->item(i)->setText(item_Text_List[0]);
//	if(probe_Ang_Ind.is_Active)
//	{
//		independent_Variables_List->item(i)->setText(independent_Variables_List->item(i)->text() + active);
//	}

//	/// wavelength
//	i=1;

//	// set active/inactive
//	Independent_Indicator probe_Wav_Ind          =                                independent_Variables_List->item(i)->data(Qt::UserRole).value<Independent_Indicator>();
//	Independent_Indicator referent_Probe_Wav_Ind = referent_Independent_Variables.independent_Variables_List->item(i)->data(Qt::UserRole).value<Independent_Indicator>();
//	probe_Wav_Ind.is_Active = referent_Probe_Wav_Ind.is_Active;
//	var.setValue(probe_Wav_Ind);
//	independent_Variables_List->item(i)->setData(Qt::UserRole, var);

//	// change text
//	current_Item_Text = independent_Variables_List->item(i)->text();
//	item_Text_List = current_Item_Text.split(active, QString::SkipEmptyParts);
//	independent_Variables_List->item(i)->setText(item_Text_List[0]);
//	if(probe_Wav_Ind.is_Active)
//	{
//		independent_Variables_List->item(i)->setText(independent_Variables_List->item(i)->text() + active);
//	}
//	argument_Type = referent_Independent_Variables.argument_Type;
//	// --------------------------------------------------------------------------

//	// PARAMETER

//	// value and number of points

//	measurement.measurement_Type				   = referent_Independent_Variables.measurement.measurement_Type;

//	/// wavelength
//	measurement.wavelength.value				   = referent_Independent_Variables.measurement.wavelength.value;
//	measurement.wavelength.independent.min		   = referent_Independent_Variables.measurement.wavelength.independent.min;
//	measurement.wavelength.independent.max		   = referent_Independent_Variables.measurement.wavelength.independent.max;
//	measurement.wavelength.independent.num_Points  = referent_Independent_Variables.measurement.wavelength.independent.num_Points;

//	measurement.k_Vec							   = referent_Independent_Variables.measurement.k_Vec;
//	measurement.lambda_Vec						   = referent_Independent_Variables.measurement.lambda_Vec;
//	measurement.k_Value							   = referent_Independent_Variables.measurement.k_Value;
//	measurement.lambda_Value					   = referent_Independent_Variables.measurement.lambda_Value;

//	/// theta angle
//	measurement.beam_Theta_0_Angle.value				    = referent_Independent_Variables.measurement.beam_Theta_0_Angle.value;
//	measurement.beam_Theta_0_Angle.independent.min		= referent_Independent_Variables.measurement.beam_Theta_0_Angle.independent.min;
//	measurement.beam_Theta_0_Angle.independent.max		= referent_Independent_Variables.measurement.beam_Theta_0_Angle.independent.max;
//	measurement.beam_Theta_0_Angle.independent.num_Points	= referent_Independent_Variables.measurement.beam_Theta_0_Angle.independent.num_Points;

//	measurement.beam_Theta_0_Cos2_Vec						= referent_Independent_Variables.measurement.beam_Theta_0_Cos2_Vec;
//	measurement.beam_Theta_0_Angle_Vec					= referent_Independent_Variables.measurement.beam_Theta_0_Angle_Vec;
//	measurement.beam_Theta_0_Cos2_Value					= referent_Independent_Variables.measurement.beam_Theta_0_Cos2_Value;
//	measurement.beam_Theta_0_Angle_Value					= referent_Independent_Variables.measurement.beam_Theta_0_Angle_Value;

//	/// alpha angle
//	measurement.detector_Theta_Angle.value				    = referent_Independent_Variables.measurement.detector_Theta_Angle.value;
//	measurement.detector_Theta_Angle.independent.min		= referent_Independent_Variables.measurement.detector_Theta_Angle.independent.min;
//	measurement.detector_Theta_Angle.independent.max		= referent_Independent_Variables.measurement.detector_Theta_Angle.independent.max;
//	measurement.detector_Theta_Angle.independent.num_Points	= referent_Independent_Variables.measurement.detector_Theta_Angle.independent.num_Points;

//	measurement.detector_Theta_Cos2_Vec						= referent_Independent_Variables.measurement.detector_Theta_Cos2_Vec;
//	measurement.detector_Theta_Angle_Vec					= referent_Independent_Variables.measurement.detector_Theta_Angle_Vec;
//	measurement.detector_Theta_Cos2_Value					= referent_Independent_Variables.measurement.detector_Theta_Cos2_Value;
//	measurement.detector_Theta_Angle_Value					= referent_Independent_Variables.measurement.detector_Theta_Angle_Value;

//	/// phi angle
//	measurement.detector_Phi_Angle.value				    = referent_Independent_Variables.measurement.detector_Phi_Angle.value;
//	measurement.detector_Phi_Angle.independent.min			= referent_Independent_Variables.measurement.detector_Phi_Angle.independent.min;
//	measurement.detector_Phi_Angle.independent.max			= referent_Independent_Variables.measurement.detector_Phi_Angle.independent.max;
//	measurement.detector_Phi_Angle.independent.num_Points	= referent_Independent_Variables.measurement.detector_Phi_Angle.independent.num_Points;

//	measurement.detector_Phi_Cos2_Vec						= referent_Independent_Variables.measurement.detector_Phi_Cos2_Vec;
//	measurement.detector_Phi_Angle_Vec						= referent_Independent_Variables.measurement.detector_Phi_Angle_Vec;
//	measurement.detector_Phi_Cos2_Value						= referent_Independent_Variables.measurement.detector_Phi_Cos2_Value;
//	measurement.detector_Phi_Angle_Value					= referent_Independent_Variables.measurement.detector_Phi_Angle_Value;

//	/// resolution
//	measurement.spectral_Distribution				= referent_Independent_Variables.measurement.spectral_Distribution;
//	measurement.beam_Theta_0_Distribution			= referent_Independent_Variables.measurement.beam_Theta_0_Distribution;
//	measurement.beam_Phi_0_Distribution				= referent_Independent_Variables.measurement.beam_Phi_0_Distribution;

//	/// detector
//	measurement.detector_1D		= referent_Independent_Variables.measurement.detector_1D;
//	measurement.detector_2D		= referent_Independent_Variables.measurement.detector_2D;

//	/// footprint
//	measurement.beam_Geometry	= referent_Independent_Variables.measurement.beam_Geometry;
//	measurement.sample_Geometry = referent_Independent_Variables.measurement.sample_Geometry;

//	/// other
//	measurement.polarization	= referent_Independent_Variables.measurement.polarization;
//	measurement.background		= referent_Independent_Variables.measurement.background;

//	// save
//	var.setValue(measurement);
//	measurement_Item->setData(DEFAULT_COLUMN,Qt::UserRole,var);

//	// refresh text
//	i=0;
//	Independent_Variables_Editor* editor = new Independent_Variables_Editor(measurement_Item, independent_Variables_List->item(i), independent_Variables_List, argument_Type);
//		editor->close();
//	i=1;
//	editor = new Independent_Variables_Editor(measurement_Item, independent_Variables_List->item(i), independent_Variables_List, argument_Type);
//		editor->close();

	return *this;
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Independent_Curve* independent_Curve )
{
	return stream;
//	<< independent_Variables->measurement << independent_Variables->calc_Functions
//					<< independent_Variables->calculated_Values
//					<< independent_Variables->tab_Name << independent_Variables->plot_Options
//					<< independent_Variables->argument_Type;		// since 1.7.5

}
QDataStream& operator >>(QDataStream& stream,		 Independent_Curve* independent_Curve )
{
//	stream	>> independent_Variables->measurement >> independent_Variables->calc_Functions;
//	stream >> independent_Variables->calculated_Values;
//	stream >> independent_Variables->tab_Name >> independent_Variables->plot_Options;

//	if(Global_Variables::check_Loaded_Version(1,7,5))
//	{ stream >> independent_Variables->argument_Type ; }	// since 1.7.5

	return stream;
}
