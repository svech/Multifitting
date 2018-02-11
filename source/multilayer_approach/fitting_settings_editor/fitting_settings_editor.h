#ifndef FITTING_SETTINGS_EDITOR_H
#define FITTING_SETTINGS_EDITOR_H

#include "multilayer_approach/multilayer_approach.h"

class Fitting_Settings_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Fitting_Settings_Editor(Multilayer_Approach* multilayer_Approach, QWidget *parent = nullptr);

signals:

public slots:
};

#endif // FITTING_SETTINGS_EDITOR_H
