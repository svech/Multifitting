#ifndef DISTRIBUTION_BOX_H
#define DISTRIBUTION_BOX_H

#include "global/settings.h"
#include "standard/mydoublespinbox.h"
#include "distribution_editor.h"

class Distribution_Box : public QWidget
{
	Q_OBJECT
public:
    explicit Distribution_Box(QString measurement_Type,
                              Distribution& distribution,
                              QString pre_Name,
                              QString symbolic_Name,
                              MyDoubleSpinBox* related_SpinBox,
                              QString angular_Units = "",
                              bool totally_Forbid_Sampling = false,
                              QString phi = "",
                              QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Plot();
	void create_Box();
	double set_From_Value(double real_input);
	double save_To_Value(double nominal_input);
	void replot();
	void connecting();

	Distribution& distribution;
	QString pre_Name;
	QString symbolic_Name;
	MyDoubleSpinBox* related_SpinBox;
	QString angular_Units;
	bool totally_Forbid_Sampling;
	QString measurement_Type;
	QString phi;

	QVBoxLayout* main_Layout;

	QCustomPlot* distribution_Plot;
	QCPBars* bars;

	QGroupBox* groupbox;
	QLabel* resolution_Label;		MyDoubleSpinBox* resolution_SpinBox;
	QLabel* distribution_Label;		QComboBox* distribution_ComboBox;
	QCheckBox* sampling_Checkbox;
	QLabel* samples_Label;			QSpinBox* sample_SpinBox;

};

#endif // DISTRIBUTION_BOX_H
