#include "distribution_editor.h"

Distribution_Editor::Distribution_Editor(Data& measurement, QString spectral_Units, QString angular_Units,
										 MyDoubleSpinBox* related_Spectral_SpinBox,
										 MyDoubleSpinBox* related_Theta_0_SpinBox,
										 MyDoubleSpinBox* related_Phi_0_SpinBox,
										 QWidget* parent) :
	measurement(measurement),
	spectral_Units(spectral_Units),
	angular_Units(angular_Units),
	related_Spectral_SpinBox(related_Spectral_SpinBox),
	related_Theta_0_SpinBox(related_Theta_0_SpinBox),
	related_Phi_0_SpinBox(related_Phi_0_SpinBox),
	QWidget(parent)
{
	setWindowTitle("Set up distribution");
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Window);
	create_Main_Layout();
}

void Distribution_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	hor_Layout = new QHBoxLayout;
	main_Layout->addLayout(hor_Layout);

	// ---------------------------------------------
	// wavelength
	// ---------------------------------------------
	QString name = "Spectral width, FWHM, ";
	if(	spectral_Units == wavelength_Units_List[angstrom] ||
		spectral_Units == wavelength_Units_List[nm]	   )
	{
		name += " " + Delta_Big_Sym + Lambda_Sym + "/" + Lambda_Sym ;
	} else
	{
		name += " " + Delta_Big_Sym + "E/E" ;
	}
	Distribution_Box* spectral_Distribution_Box = new Distribution_Box(measurement.spectral_Distribution, name, related_Spectral_SpinBox);
		hor_Layout->addWidget(spectral_Distribution_Box);

	// ---------------------------------------------
	// theta_0
	// ---------------------------------------------
	name = "Angular divergence, FWHM, " + Delta_Big_Sym + Theta_Sym + Zero_Subscript_Sym;
	Distribution_Box* theta_0_Distribution_Box = new Distribution_Box(measurement.beam_Theta_0_Distribution, name, related_Theta_0_SpinBox, angular_Units);
	hor_Layout->addWidget(theta_0_Distribution_Box);

	// ---------------------------------------------
	// phi_0
	// ---------------------------------------------
	if( measurement.measurement_Type == measurement_Types[GISAS] )
	{
		name = "Angular divergence, FWHM, " + Delta_Big_Sym + Phi_Sym + Zero_Subscript_Sym;
		Distribution_Box* phi_0_Distribution_Box = new Distribution_Box(measurement.beam_Phi_0_Distribution, name, related_Phi_0_SpinBox, angular_Units);
			hor_Layout->addWidget(phi_0_Distribution_Box);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	close_Button = new QPushButton("Close");
		close_Button->adjustSize();
		close_Button->setFixedSize(close_Button->size());
	main_Layout->addWidget(close_Button,0,Qt::AlignRight);

	connect(close_Button, &QPushButton::clicked, this, [=]{close();});
}
