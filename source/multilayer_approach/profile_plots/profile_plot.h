#ifndef PROFILE_PLOT_H
#define PROFILE_PLOT_H

#include "multilayer_approach/multilayer_approach.h"

class Profile_Plots_Window;

class Profile_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Profile_Plot(Multilayer* multilayer, Profile_Plots_Window* profile_Plots_Window, QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Left_Side();
	void create_Plot_Frame_And_Scale();
	void plot_Data(bool recalculate_Profile = false);
	void calculate_Profile();
	void unwrap_Subtree(QVector<Data>& struct_Data_Vector, QTreeWidgetItem* item, int num_Repetition, int period_Index);
	void get_Max_My_Sigma(QTreeWidgetItem* item);
	complex<double> delta_Beta_Epsilon_Func(double z);

	Multilayer* multilayer;
	Profile_Plots_Window* profile_Plots_Window;
	QHBoxLayout* main_Layout;

	// left side
		QGroupBox* value_Type_GroupBox;
			QRadioButton* permittivity_RadioButton;
			QLabel* at_Wavelength_Label;
			QLineEdit* at_Wavelength_LineEdit;
			QLabel* at_Wavelength_Unints_Label;
			QRadioButton* delta_RadioButton;
			QRadioButton* beta_RadioButton;
			QRadioButton* materials_RadioButton;
			QRadioButton* elements_RadioButton;

		QGroupBox* line_Type_GroupBox;
			QCheckBox* use_Roughness_CheckBox;
			QCheckBox* use_Diffusiness_CheckBox;
			QCheckBox* show_Sharp_CheckBox;
			QCheckBox* discretization_CheckBox;
			QCheckBox* cursor_Cordinate_CheckBox;

		QGroupBox* scale_GroupBox;
			QCheckBox* rescale_X_CheckBox;
			QCheckBox* rescale_Y_CheckBox;
			QLabel* scale_Y_Label;
			QRadioButton* lin_Y_RadioButton;
			QRadioButton* log_Y_RadioButton;


	QCustomPlot* custom_Plot;
	QScrollBar* horizontall_Scrollbar;

	double max_Sigma = 0.1;
	QVector<Data> struct_Data_Vector;
	QVector<double> boundary_Vector;
	QVector<double> thickness_Vector;
	QVector<QString> material_Vector;
	QVector<double> density_Vector;
	QVector<double> delta_Epsilon_Vector;
	QVector<double> beta_Epsilon_Vector;
	QVector<double> layer_Norm_Vector;
	QVector<QString> different_Materials;

	/// data to plot
	// permittivity
	QVector<QCPGraphData> delta_To_Plot_Vector;
	QVector<QCPGraphData> beta_To_Plot_Vector;

	// materials
	QVector<QVector<QCPGraphData>> materials_To_Plot_Vector_Vector;
	QVector<QString> material_Labels_Vector;

	// elements
	QVector<QVector<QCPGraphData>> elements_To_Plot_Vector_Vector;
	QVector<QString> elements_Labels_Vector;

	// general
	QVector<double> arg;
	QVector<double> val;
};

#endif // PROFILE_PLOT_H
