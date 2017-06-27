#ifndef TARGET_CURVE_EDITOR_H
#define TARGET_CURVE_EDITOR_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/target_curve.h"

class Target_Curve_Editor : public QWidget
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize()							{ QWidget::adjustSize(); }
public:
	explicit Target_Curve_Editor(Target_Curve* target_Curve, QWidget *parent = 0);

	Target_Curve* target_Curve;
private:
	void closeEvent(QCloseEvent *event);
	void create_Main_Layout();
		void create_Filepath_GroupBox();
		void create_Data_GroupBox();
	void set_Window_Geometry();


	void read_Data_File(QString filepath);
	void browse_Data_File();

	void fill_Arg_Units_ComboBox(QString arg_Type);
	void fill_Val_Modes_ComboBox(QString val_Mode);
	void change_Arg_Units_ComboBox(QString arg_Units);
	void change_At_Fixed_Units_ComboBox(QString fixed_Units);

	void resize_Line_Edit(QLineEdit* line_Edit = NULL);
	void resize_ComboBox(QLineEdit* line_Edit = NULL);

	void show_All();
		void show_Filepath();
		void show_Curve_Data();
		void show_Measurement_Data();
	void refresh_Data();
		void refresh_Filepath(QString filepath);

private:
	QVBoxLayout* main_Layout;
		QGroupBox* filepath_GroupBox;
			QComboBox* filepath_ComboBox;
			QPushButton* browse_Button;

		QGroupBox* data_GroupBox;
			QLabel* arg_Type_Label;
			QComboBox* arg_Type_ComboBox;
			QLabel* arg_Units_Label;
			QComboBox* arg_Units_ComboBox;
			QLabel* arg_Offset_Label;
			QDoubleSpinBox* arg_Offset_SpinBox;
			QLabel* arg_Factor_Label;
			QDoubleSpinBox* arg_Factor_SpinBox;

			QLabel* val_Function_Label;
			QComboBox* val_Function_ComboBox;
			QLabel* val_Mode_Label;
			QComboBox* val_Mode_ComboBox;
			QLabel* val_Offset_Label;
			QDoubleSpinBox* val_Offset_SpinBox;
			QLabel* val_Factor_Label;
			QDoubleSpinBox* val_Factor_SpinBox;

			QLabel* at_Fixed_Label;
			QLineEdit* at_Fixed_LineEdit;
			QComboBox* at_Fixed_Units_ComboBox;
			QLabel* polarization_Label;
			QLineEdit* polarization_LineEdit;
			QLabel* polarization_Sensitivity_Label;
			QLineEdit* polarization_Sensitivity_LineEdit;
			QLabel* spectral_Resolution_Label;
			QLineEdit* spectral_Resolution_LineEdit;
			QLabel* angular_Resolution_Label;
			QLineEdit* angular_Resolution_LineEdit;
			QLabel* angular_Units_Label;

		QLabel* data_Indicator_Label;
		QPushButton* close_Button;
};

#endif // TARGET_CURVE_EDITOR_H