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
	void calculate_Profile();
	void calc_PSD_For_Interface(int interface_Index, QVector<double>& value_Vector, bool& use_Interface, QString& material, double& sigma_Eff, double nu_Min, double nu_Max);
	void plot_Data(bool recalculate_Profile = false);
	void lock_Interfaces();

	int num_Plot_Points;
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
			QCheckBox* show_Interface_by_Number_CheckBox;
			QComboBox* interface_by_Number_ComboBox;
			QCheckBox* show_Substrate_Surface_CheckBox;

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
			QComboBox* PSD_1D_ComboBox;
			QComboBox* PSD_2D_ComboBox;


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
		QVector<QCPGraphData> top_Surface_Plot_Vector;
		QVector<QCPGraphData> current_Interface_Plot_Vector;
		QVector<QCPGraphData> substrate_Surface_Plot_Vector;

		vector<double> arg;
		QVector<double> top_Surface_Val;
		QVector<double> current_Interface_Val;
		QVector<double> substrate_Surface_Val;

		bool use_Top_Surface;
		bool use_Current_Interface;
		bool use_Substrate_Surface;
};

#endif // ROUGHNESS_PLOT_H
