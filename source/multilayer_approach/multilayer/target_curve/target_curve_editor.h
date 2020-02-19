#ifndef TARGET_CURVE_EDITOR_H
#define TARGET_CURVE_EDITOR_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/target_curve/target_curve.h"
#include "multilayer_approach/multilayer/multilayer.h"
#include "target_curve_plot.h"

class MyDoubleSpinBox;
class Target_Curve_Editor : public QDialog
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize()							{ QWidget::adjustSize(); }
public:
	explicit Target_Curve_Editor(Target_Curve* target_Curve, Multilayer* multilayer, QWidget *parent = 0);

	Target_Curve* target_Curve;
	Multilayer* multilayer_Parent;

	void closeEvent(QCloseEvent *event);
	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);
	void create_Main_Layout();
		void create_Plot();
		void create_Plot_Options_GroupBox();
		void create_Filepath_GroupBox();
		void create_Data_GroupBox();
		void reset_Subinterval();
		void create_Buttons();
	void set_Window_Geometry();


	void read_Data_File(QString filepath);
	void browse_Data_File();
	void export_Data_File();

	void fill_Arg_Units_ComboBox(QString arg_Type);
	void fill_Val_Modes_ComboBox(QString val_Mode);
	void change_Arg_Units_ComboBox(QString arg_Units);
	void change_At_Fixed_Units_ComboBox(QString fixed_Units);

	void resize_ComboBox(QLineEdit* line_Edit = nullptr);

	void show_All();
		void show_Filepath();
		void show_Curve_Data();
		void show_Measurement_Data();
			void show_Unit_Dependent_Data();
				void show_Angular_Resolution();

	void refresh_Filepath(QString filepath);
	void refresh_Argument_Type();
	void refresh_Value_Type();
	void refresh_Argument_Units();
	void refresh_Value_Mode();
	void refresh_At_Fixed_Value();
	void refresh_At_Fixed_Units();
	void refresh_Offsets();
	void refresh_Factors();
	void refresh_Beam_Intensity();
	void refresh_Polarization();
	void refresh_Resolution();
	void refresh_Measurement_Geometry();

	bool is_File_Exists = false;

	QVBoxLayout* main_Layout;
		Target_Curve_Plot* target_Curve_Plot;
		QWidget* bottom_Part_Widget;
		QVBoxLayout* bottom_Part_Layout;

		QGroupBox* plot_Options_GroupBox;

		QGroupBox* filepath_GroupBox;
			QComboBox* filepath_ComboBox;
			QPushButton* browse_Button;

		QGroupBox* data_GroupBox;		
			QCheckBox* main_Subinterval_Checkbox;
			MyDoubleSpinBox* from_Subinterval_SpinBox;
			QLabel* and_Subinterval_Label;
			MyDoubleSpinBox* to_Subinterval_SpinBox;

			QLabel* arg_Type_Label;
			QComboBox* arg_Type_ComboBox;
			QLabel* arg_Units_Label;
			QComboBox* arg_Units_ComboBox;
			QLabel* arg_Offset_Label;
			MyDoubleSpinBox* arg_Offset_SpinBox;
			QLabel* arg_Factor_Label;
			MyDoubleSpinBox* arg_Factor_SpinBox;

			QLabel* val_Function_Label;
			QComboBox* val_Function_ComboBox;
			QLabel* val_Mode_Label;
			QComboBox* val_Mode_ComboBox;
			QLabel* val_Offset_Label;
			MyDoubleSpinBox* val_Offset_SpinBox;
			QLabel* val_Factor_Label;
			MyDoubleSpinBox* val_Factor_SpinBox;
			QLabel* beam_Intensity_Start_Label;
			MyDoubleSpinBox* beam_Intensity_Start_SpinBox;
			QLabel* beam_Intensity_Final_Label;
			MyDoubleSpinBox* beam_Intensity_Final_SpinBox;
			QCheckBox* norm_On_Beam_Intensity;

			QLabel* at_Fixed_Label;
			QLineEdit* at_Fixed_LineEdit;
			QComboBox* at_Fixed_Units_ComboBox;
			QLabel* polarization_Label;
			QLineEdit* polarization_LineEdit;
			QLabel* spectral_Resolution_Label;
			MyDoubleSpinBox* spectral_Resolution_SpinBox;
//			QLabel* polarization_Sensitivity_Label;
//			QLineEdit* polarization_Sensitivity_LineEdit;
			QLabel* background_Label;
			QLineEdit* background_LineEdit;
			QLabel* angular_Resolution_Label;
			MyDoubleSpinBox* angular_Resolution_SpinBox;
			QLabel* angular_Units_Label;

			QLabel* beam_Size_Label;
			MyDoubleSpinBox* beam_Size_SpinBox;
			QLabel* beam_Size_Units_Label;
			QLabel* sample_Size_Label;
			MyDoubleSpinBox* sample_Size_SpinBox;
			QLabel* sample_Size_Units_Label;
			QLabel* beam_Profile_Spreading_Label;
			MyDoubleSpinBox* beam_Profile_Spreading_SpinBox;
			QLabel* sample_Shift_Label;
			MyDoubleSpinBox* sample_Shift_SpinBox;
			QLabel* sample_Shift_Units_Label;

		QLabel* data_Indicator_Label;
		QPushButton* close_Button;
		QPushButton* read_Data_Button;
		QPushButton* export_Data_Button;
};

#endif // TARGET_CURVE_EDITOR_H
