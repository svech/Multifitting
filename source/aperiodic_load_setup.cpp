#include "aperiodic_load_setup.h"

Aperiodic_Load_Setup::Aperiodic_Load_Setup(Aperiodic_Settings& aperiodic_Settings, Structure_Toolbar* structure_Toolbar, QWidget* parent) :
	aperiodic_Settings(aperiodic_Settings),
	structure_Toolbar(structure_Toolbar),
	QDialog(parent)
{
	setWindowTitle("Aperiodic settings");
//	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}
