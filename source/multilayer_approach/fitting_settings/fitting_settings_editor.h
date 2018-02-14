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
	void create_Main_Params_Group_Box();
	void create_AdditionalParams_Group_Box();

	Multilayer_Approach* multilayer_Approach;
	Fitting_Settings* fitting_Settings;

	QVBoxLayout* main_Layout;
		QHBoxLayout* methods_Layout;
			QLabel* methods_Label;
			QComboBox* methods_Combo_Box;
			QVector<int> indices_Vec;

		QStackedWidget* pages_Stack;
			QWidget* page;
				QVBoxLayout* page_Layout;

				QGroupBox* fit_Params_Group_Box;
					QGridLayout* fit_Params_Group_Box_Layout;

				// for GSL TRS
					QLabel* max_Iter_Label;	   QLineEdit* max_Iter_Line_Edit;
					QLabel* x_Tolerance_Label; QLineEdit* x_Tolerance_Line_Edit;
					QLabel* g_Tolerance_Label; QLineEdit* g_Tolerance_Line_Edit;
					QLabel* f_Tolerance_Label; QLineEdit* f_Tolerance_Line_Edit;

				QGroupBox* additional_Params_Group_Box;
					QGridLayout* additional_Params_Group_Box_Layout;

				// for GSL TRS
					QLabel* scale_Label;		QComboBox* scale_Combo_Box;
					QLabel* solver_Label;		QComboBox* solver_Combo_Box;
					QLabel* fdtype_Label;		QComboBox* fdtype_Combo_Box;
					QLabel* factor_up_Label;	QLineEdit* factor_up_Line_Edit;
					QLabel* factor_down_Label;	QLineEdit* factor_down_Line_Edit;
					QLabel* avmax_Label;		QLineEdit* avmax_Line_Edit;
					QLabel* h_df_Label;			QLineEdit* h_df_Line_Edit;
					QLabel* h_fvv_Label;		QLineEdit* h_fvv_Line_Edit;

		QPushButton* done_Button;
};

#endif // FITTING_SETTINGS_EDITOR_H
