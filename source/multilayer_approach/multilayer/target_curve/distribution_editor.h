#ifndef DISTRIBUTION_EDITOR_H
#define DISTRIBUTION_EDITOR_H

#include "distribution_box.h"

class Distribution_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Distribution_Editor(Data& measurement, QString spectral_Units, QString angular_Units,
								 MyDoubleSpinBox* related_Spectral_SpinBox,
								 MyDoubleSpinBox* related_Theta_0_SpinBox,
								 MyDoubleSpinBox* related_Phi_0_SpinBox = nullptr, QWidget *parent = nullptr);

	void create_Main_Layout();

	Data& measurement;
	QString angular_Units;
	QString spectral_Units;

	MyDoubleSpinBox* related_Spectral_SpinBox;
	MyDoubleSpinBox* related_Theta_0_SpinBox;
	MyDoubleSpinBox* related_Phi_0_SpinBox;

	QVBoxLayout* main_Layout;
		QHBoxLayout* hor_Layout;

	QPushButton* close_Button;
};

#endif // DISTRIBUTION_EDITOR_H
