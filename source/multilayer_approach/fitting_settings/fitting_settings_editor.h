#ifndef FITTING_SETTINGS_EDITOR_H
#define FITTING_SETTINGS_EDITOR_H

#include "multilayer_approach/multilayer_approach.h"
#include "fitting_settings.h"

class Fitting_Settings_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Fitting_Settings_Editor(Multilayer_Approach* multilayer_Approach, QWidget* parent = nullptr);

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

	Multilayer_Approach* multilayer_Approach;
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
					QLabel* max_Iter_Label;	   QLineEdit* max_Iter_Line_Edit;
					QLabel* x_Tolerance_Label; QLineEdit* x_Tolerance_Line_Edit;
					QLabel* g_Tolerance_Label; QLineEdit* g_Tolerance_Line_Edit;
					QLabel* f_Tolerance_Label; QLineEdit* f_Tolerance_Line_Edit;

				QGroupBox* GSL_Additional_Params_Group_Box;
					QGridLayout* GSL_Additional_Params_Group_Box_Layout;

				// for GSL TRS
					QLabel* scale_Label;		QComboBox* scale_Combo_Box;
					QLabel* solver_Label;		QComboBox* solver_Combo_Box;
					QLabel* fdtype_Label;		QComboBox* fdtype_Combo_Box;
					QLabel* factor_up_Label;	QLineEdit* factor_up_Line_Edit;
					QLabel* factor_down_Label;	QLineEdit* factor_down_Line_Edit;
					QLabel* avmax_Label;		QLineEdit* avmax_Line_Edit;
					QLabel* h_df_Label;			QLineEdit* h_df_Line_Edit;
					QLabel* h_fvv_Label;		QLineEdit* h_fvv_Line_Edit;

				/// SwarmOps
				QWidget* SO_Page;
					QVBoxLayout* SO_Page_Layout;
						QGroupBox* SO_Fit_Params_Group_Box;
							QGridLayout* SO_Fit_Params_Group_Box_Layout;

				// for SwarmOps
					QLabel* num_Runs_Label;			QLineEdit* num_Runs_Line_Edit;
					QLabel* max_Evaluations_Label;	QLineEdit* max_Evaluations_Line_Edit;
					QLabel* max_Eval_Factor_Label;	QLineEdit* max_Eval_Factor_Line_Edit;
					QCheckBox* max_Eval_Check_Box;

				QGroupBox* SO_Additional_Params_Group_Box;
					QGridLayout* SO_Additional_Params_Group_Box_Layout;

		QPushButton* done_Button;
};

#endif // FITTING_SETTINGS_EDITOR_H
