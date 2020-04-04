#ifndef DISTRIBUTION_BOX_H
#define DISTRIBUTION_BOX_H

#include "global/settings.h"
#include "standard/mydoublespinbox.h"
#include "distribution_editor.h"

class Distribution_Box : public QWidget
{
	Q_OBJECT
public:
	explicit Distribution_Box(QWidget *parent = nullptr);

	void create_Main_Layout();
	QGroupBox* groupbox;
	QLabel* resolution_Label;		MyDoubleSpinBox* resolution_SpinBox;
	QLabel* distribution_Label;		QComboBox* distribution_ComboBox;
	QLabel* samples_Label;			QSpinBox* sample_SpinBox;
	QLabel* coverage_Label;			MyDoubleSpinBox* coverage_SpinBox;

};

#endif // DISTRIBUTION_BOX_H
