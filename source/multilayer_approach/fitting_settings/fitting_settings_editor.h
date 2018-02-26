#ifndef FITTING_SETTINGS_EDITOR_H
#define FITTING_SETTINGS_EDITOR_H

#include "multilayer_approach/multilayer_approach.h"
#include "fitting_settings.h"

class Fitting_Settings_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Fitting_Settings_Editor(QWidget* parent = nullptr);

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
	void set_Window_Geometry();
	void create_Menu();
	void create_Metods();
	void create_Pages();
	void create_GSL_Main_Params_Group_Box();
	void create_GSL_AdditionalParams_Group_Box();
	void create_SO_Main_Params_Group_Box();
	void create_SO_AdditionalParams_Group_Box();

	Fitting_Settings* fitting_Settings;

	QVBoxLayout* main_Layout;
		QHBoxLayout* methods_Layout;
			QLabel* methods_Label;
			QComboBox* methods_Combo_Box;
			QVector<int> indices_Vec;

		QStackedWidget* pages_Stack;

				/// GSL
				QWidget* GSL_Page;
					QVBoxLayout* GSL_Page_Layout;
						QGroupBox* GSL_Fit_Params_Group_Box;
							QGridLayout* GSL_Fit_Params_Group_Box_Layout;

				// for GSL TRS
					QCheckBox* GSL_randomized_Start_Check_Box;
					QLabel* GSL_num_Runs_Label;			QLineEdit* GSL_num_Runs_Line_Edit;
					QLabel* GSL_max_Iter_Label;			QLineEdit* GSL_max_Iter_Line_Edit;
					QLabel* GSL_common_Tolerance_Label;QLineEdit* GSL_common_Tolerance_Line_Edit;

				QGroupBox* GSL_Additional_Params_Group_Box;
					QGridLayout* GSL_Additional_Params_Group_Box_Layout;

				// for GSL TRS
					QLabel* GSL_x_Tolerance_Label;	QLineEdit* GSL_x_Tolerance_Line_Edit;
					QLabel* GSL_g_Tolerance_Label;	QLineEdit* GSL_g_Tolerance_Line_Edit;
					QLabel* GSL_f_Tolerance_Label;	QLineEdit* GSL_f_Tolerance_Line_Edit;

					QLabel* GSL_scale_Label;		QComboBox* GSL_scale_Combo_Box;
					QLabel* GSL_solver_Label;		QComboBox* GSL_solver_Combo_Box;
					QLabel* GSL_fdtype_Label;		QComboBox* GSL_fdtype_Combo_Box;
					QLabel* GSL_factor_up_Label;	QLineEdit* GSL_factor_up_Line_Edit;
					QLabel* GSL_factor_down_Label;	QLineEdit* GSL_factor_down_Line_Edit;
					QLabel* GSL_avmax_Label;		QLineEdit* GSL_avmax_Line_Edit;
					QLabel* GSL_h_df_Label;			QLineEdit* GSL_h_df_Line_Edit;
					QLabel* GSL_h_fvv_Label;		QLineEdit* GSL_h_fvv_Line_Edit;

				/// SwarmOps
				QWidget* SO_Page;
					QVBoxLayout* SO_Page_Layout;
						QGroupBox* SO_Fit_Params_Group_Box;
							QGridLayout* SO_Fit_Params_Group_Box_Layout;

				// for SwarmOps
					QCheckBox* SO_randomized_Start_Check_Box;
					QLabel* SO_num_Runs_Label;			QLineEdit* SO_num_Runs_Line_Edit;
					QLabel* SO_max_Evaluations_Label;	QLineEdit* SO_max_Evaluations_Line_Edit;
					QCheckBox* SO_max_Eval_Check_Box;	QLineEdit* SO_max_Eval_Factor_Line_Edit;

				QGroupBox* SO_Additional_Params_Group_Box;
					QGridLayout* SO_Additional_Params_Group_Box_Layout;

		QPushButton* done_Button;
};

#endif // FITTING_SETTINGS_EDITOR_H
