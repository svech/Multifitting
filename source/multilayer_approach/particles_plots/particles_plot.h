#ifndef PARTICLES_PLOT_H
#define PARTICLES_PLOT_H

#include "multilayer_approach/multilayer_approach.h"

class Particles_Plots_Window;

class Particles_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Particles_Plot(Multilayer* multilayer, Particles_Plots_Window* particles_Plots_Window, QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Left_Side();
	void create_Plot_Frame_And_Scale();
	void calculate_Profile();
	void calc_PSD_For_Interface(int interface_Index, QVector<double>& value_Vector, bool& use_Interface, QString& material, double& sigma_Eff, double nu_Min, double nu_Max);
	void plot_Data(bool recalculate_Profile = false);
	void lock_Interfaces();

	int num_Plot_Points;
	Multilayer* multilayer;
	QCustomPlot* custom_Plot;
	Particles_Plots_Window* particles_Plots_Window;
	QHBoxLayout* main_Layout;

	// left side
	QGroupBox* interface_GroupBox;
		QCheckBox* A_show_Layer_by_Number_CheckBox;
		QComboBox* A_layer_by_Number_ComboBox;
		QCheckBox* B_show_Layer_by_Number_CheckBox;
		QComboBox* B_layer_by_Number_ComboBox;

	QGroupBox* scale_GroupBox;
		MyDoubleSpinBox* orders_Spinbox;
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

	QGroupBox* units_GroupBox;
		QCheckBox* cursor_Cordinate_CheckBox;
		QComboBox* argument_ComboBox;
		QComboBox* value_ComboBox;


	int media_Counter = 2; // ambient is the first
	tree<Node> calc_Tree;
	vector<Node*> flat_Calc_Tree;	            // contains only Ambient, Layers, Substrate
	vector<Node*> short_Flat_Calc_Tree;	        // contains only Layers, Substrate, and no extreme layers
	vector<Node*> media_Node_Map_Vector;
	vector<Data>  media_Data_Map_Vector;		// without calculated epsilon, hi etc
	vector<int>   media_Period_Index_Map_Vector;

	QVector<double> boundary_Vector;
	QVector<double> thickness_Vector;

	// data to plot
	QVector<QCPGraphData> A_Layer_Plot_Vector;
	QVector<QCPGraphData> B_Layer_Plot_Vector;

	vector<double> arg;
	QVector<double> A_layer_Val;
	QVector<double> B_layer_Val;

	bool use_A_layer;
	bool use_B_layer;
};

#endif // PARTICLES_PLOT_H
