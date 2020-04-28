#include "independent_curve_editor.h"

Independent_Curve_Editor::Independent_Curve_Editor(Independent_Curve* independent_Curve, Multilayer* multilayer_Parent, QWidget *parent) :
	independent_Curve(independent_Curve),
	multilayer_Parent(multilayer_Parent),
	QWidget(parent)
{
	bool new_curve = false;
	if(independent_Curve->label_Text == "<no description>") new_curve = true;

	create_Main_Layout();
	setWindowTitle(independent_Curve->measurement.measurement_Type);
	setAttribute(Qt::WA_DeleteOnClose);

	if(new_curve)
	{
		/// now we have measurement type and consequently have a new curve. But initially fully unchecked
		if(global_Multilayer_Approach->runned_Calculation_Settings_Editor.contains(calc_Settings_Key))
		{
			int active_Tab_Optical_Graphs = global_Multilayer_Approach->runned_Optical_Graphs_1D.value(optical_Graphs_1D_Key)->main_Tabs->currentIndex();
			global_Multilayer_Approach->runned_Calculation_Settings_Editor.value(calc_Settings_Key)->close();
			global_Multilayer_Approach->open_Calculation_Settings();
			global_Multilayer_Approach->runned_Optical_Graphs_1D.value(optical_Graphs_1D_Key)->main_Tabs->setCurrentIndex(active_Tab_Optical_Graphs);
		}
	}
}

void Independent_Curve_Editor::closeEvent(QCloseEvent *event)
{
	multilayer_Parent->runned_Independent_Curve_Editors.remove(multilayer_Parent->runned_Independent_Curve_Editors.key(this));
	event->accept();
}

void Independent_Curve_Editor::create_Main_Layout()
{
	main_Layout = new QGridLayout(this);
		main_Layout->setSpacing(0);
		main_Layout->setContentsMargins(4,0,4,0);
		main_Layout->setSizeConstraint(QLayout::SetFixedSize);

	done_Button = new QPushButton("Done");
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,1,0,Qt::AlignCenter);
	connect(done_Button, &QPushButton::clicked, this, &Independent_Curve_Editor::close);

	create_Main_Part();

	Global_Variables::create_Shortcuts(this);
}


void Independent_Curve_Editor::create_Main_Part()
{
	if(	independent_Curve->measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		specular_Independent_Curve_Part = new Specular_Independent_Curve_Part(independent_Curve);
			main_Layout->addWidget(specular_Independent_Curve_Part,0,0);
	}
	if(	independent_Curve->measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
		detector_Independent_Curve_Part = new Detector_Independent_Curve_Part(independent_Curve);
			main_Layout->addWidget(detector_Independent_Curve_Part,0,0);
	}
	if(	independent_Curve->measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		rocking_Independent_Curve_Part = new Rocking_Independent_Curve_Part(independent_Curve);
			main_Layout->addWidget(rocking_Independent_Curve_Part,0,0);
	}
	if(	independent_Curve->measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		offset_Independent_Curve_Part = new Offset_Independent_Curve_Part(independent_Curve);
			main_Layout->addWidget(offset_Independent_Curve_Part,0,0);
	}
	if(	independent_Curve->measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		gisas_Independent_Curve_Part = new GISAS_Independent_Curve_Part(independent_Curve);
			main_Layout->addWidget(gisas_Independent_Curve_Part,0,0);
	}
}
