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
		void horizontall_ScrollbarChanged(int value);
		void x_Axis_RangeChanged(const QCPRange& range);
	void create_Left_Side();
	void create_Plot_Frame_And_Scale();
	void plot_Data(bool recalculate_Profile = false);
	void calculate_Profile();
	void get_Delta_Epsilon(const Data& struct_Data, double& delta, double& beta);
	void get_Material(const Data& struct_Data);
	void get_Element_Map(const Data& struct_Data, QMap<QString,double>& element_Map);
	void unwrap_Subtree(QVector<Data>& struct_Data_Vector, QTreeWidgetItem* item, int num_Repetition, int period_Index);
	void get_Max_My_Sigma(QTreeWidgetItem* item, int periods_Factor = 1);
	complex<double> delta_Beta_Epsilon_Func(double z, QString given_Material_or_Element = "no material or element");
	void hide_Show_Other_Plots();

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
	double scrollbar_Factor = 200.0;

	int struct_Data_Counter = 1; // ambient is the first
	int struct_Data_Index = 0;
	double max_Sigma = 0.1;
	QVector<Data> struct_Data_Vector;
	QVector<double> boundary_Vector;
	vector<double> boundary_Vector_Std;
	QVector<double> thickness_Vector;
	 vector<double> discrete_Step_Vector;
	QVector<double> delta_Epsilon_Vector;
	QVector<double> beta_Epsilon_Vector;
	QVector<double> layer_Norm_Vector;
	QVector<QString> different_Materials;
	QVector<QString> different_Elements;
	QVector<QMap<QString,double>> element_Concentration_Map_Vector;

	/// data to plot
	// permittivity
	QVector<QCPGraphData> delta_To_Plot_Vector;
	QVector<QCPGraphData> beta_To_Plot_Vector;

	QVector<QCPGraphData> sharp_Delta_To_Plot_Vector;
	QVector<QCPGraphData> sharp_Beta_To_Plot_Vector;

	// materials
	QVector<QVector<QCPGraphData>> materials_To_Plot_Vector_Vector;
	QVector<QVector<QCPGraphData>> sharp_Materials_To_Plot_Vector_Vector;

	// elements
	QVector<QVector<QCPGraphData>> elements_To_Plot_Vector_Vector;
	QVector<QVector<QCPGraphData>> sharp_Elements_To_Plot_Vector_Vector;

	// general
	QVector<double> arg;
	QVector<double> val;
	QVector<double> val_Sharp;

	QVector<QVector<double>> val_Multiple;
	QVector<QVector<double>> val_Sharp_Multiple;

	QMap<QCPGraph*,QCPGraph*> map_Sharp_Smooth;
	bool visibility_State = true;
};

#endif // PROFILE_PLOT_H
