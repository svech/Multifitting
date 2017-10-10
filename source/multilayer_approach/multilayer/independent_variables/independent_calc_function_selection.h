#ifndef INDEPENDENT_CALC_FUNCTION_SELECTION_H
#define INDEPENDENT_CALC_FUNCTION_SELECTION_H

#include "multilayer_approach/multilayer.h"

class Independent_Calc_Function_Selection : public QDialog
{
	Q_OBJECT
public:
	explicit Independent_Calc_Function_Selection(Independent_Variables* independent_Variables, QWidget *parent = nullptr);

public:
	void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
	void set_Window_Geometry();

	void refresh_calc_Functions(bool);

	Independent_Variables* independent_Variables;

	QVBoxLayout* main_Layout;
		QGroupBox* standard_Functions_Group_Box;
			QCheckBox* reflect_Functions;
			QCheckBox* transmit_Functions;
			QCheckBox* absorp_Functions;
		QGroupBox* field_Functions_Group_Box;
			QCheckBox* field_Intensity;
			QCheckBox* joule_Absorption;
		QGroupBox* user_Functions_Group_Box;
			QCheckBox* user_Supplied_Functions_Check;
			QLineEdit* user_Supplied_Functions;

	QPushButton* done_Button;
};

#endif // INDEPENDENT_CALC_FUNCTION_SELECTION_H
