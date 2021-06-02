#ifndef ROUGHNESS_PLOT_H
#define ROUGHNESS_PLOT_H

#include "multilayer_approach/multilayer_approach.h"

class Roughness_Plots_Window;

class Roughness_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Roughness_Plot(Multilayer* multilayer, Roughness_Plots_Window* roughness_Plots_Window, QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Left_Side();
	void create_Plot_Frame_And_Scale();
	void plot_Data(bool recalculate_Profile = false);

	Multilayer* multilayer;
	QCustomPlot* custom_Plot;
	Roughness_Plots_Window* roughness_Plots_Window;
	QHBoxLayout* main_Layout;

	// left side
		QGroupBox* value_Type_GroupBox;
			QRadioButton* PSD_1D_RadioButton;
			QRadioButton* PSD_2D_RadioButton;

		QGroupBox* interface_GroupBox;
			QCheckBox* show_Top_Surface_CheckBox;
			QComboBox* show_Interface_by_Number_ComboBox;
			QCheckBox* show_Substrate_Surface_CheckBox;

		QGroupBox* scale_GroupBox;
			QCheckBox* cursor_Cordinate_CheckBox;
			QCheckBox* rescale_X_CheckBox;
			QLineEdit* min_X_LineEdit;
			QLineEdit* max_X_LineEdit;
			QLabel* x_Units_Label;
			QCheckBox* rescale_Y_CheckBox;

			QLabel* scale_Y_Label;
			QRadioButton* lin_Y_RadioButton;
			QRadioButton* log_Y_RadioButton;
			QLabel* scale_X_Label;
			QRadioButton* lin_X_RadioButton;
			QRadioButton* log_X_RadioButton;
};

#endif // ROUGHNESS_PLOT_H
