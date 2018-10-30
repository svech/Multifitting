#ifndef APERIODIC_LOAD_SETUP_H
#define APERIODIC_LOAD_SETUP_H

#include "multilayer_approach/multilayer/structure_tree/structure_toolbar.h"

class Aperiodic_Load_Setup : public QDialog
{
	Q_OBJECT
public:
	explicit Aperiodic_Load_Setup(Aperiodic_Settings& aperiodic_Settings, Structure_Toolbar* structure_Toolbar, QWidget* parent = nullptr);

	Aperiodic_Settings& aperiodic_Settings;
	Structure_Toolbar* structure_Toolbar;
};

#endif // APERIODIC_LOAD_SETUP_H
