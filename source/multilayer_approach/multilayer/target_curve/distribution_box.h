#ifndef DISTRIBUTION_BOX_H
#define DISTRIBUTION_BOX_H

#include "global/settings.h"
#include "standard/mydoublespinbox.h"
#include "distribution_editor.h"


class Distribution_Box : public QWidget
{
	Q_OBJECT
public:
	explicit Distribution_Box(Distribution& distribution, QString name, MyDoubleSpinBox* related_SpinBox, QString angular_Units = "", QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Plot();
	void create_Box();
	double set_From_Value(double real_input);
	double save_To_Value(double nominal_input);
	void replot();
	void connecting();

	Distribution& distribution;
	QString name;
	MyDoubleSpinBox* related_SpinBox;
	QString angular_Units;

	QVBoxLayout* main_Layout;

	QCustomPlot* distribution_Plot;
	QCPBars* bars;

	QGroupBox* groupbox;
	QLabel* resolution_Label;		MyDoubleSpinBox* resolution_SpinBox;
	QLabel* distribution_Label;		QComboBox* distribution_ComboBox;
	QLabel* samples_Label;			QSpinBox* sample_SpinBox;
	QLabel* coverage_Label;			MyDoubleSpinBox* coverage_SpinBox;

};

#endif // DISTRIBUTION_BOX_H
