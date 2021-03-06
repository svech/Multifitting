#ifndef GRADING_EDITOR_H
#define GRADING_EDITOR_H

#include "global/settings.h"

class Grading_Editor : public QDialog
{
    Q_OBJECT
public:
	explicit Grading_Editor(QTreeWidgetItem* item, QString drift_Name, QWidget* parent = 0);

signals:
	void grading_Edited();

public:
	void emit_Grading_Edited();
	void refresh_Data();

	void create_Main_Layout();
		void create_Interface();
            void read_Drift_From_Item();

	QTreeWidgetItem* item;
	int default_Min_Line_Size;
	QString drift_Name;
    Drift drift;
	Data layer;

    QVBoxLayout* main_Layout;
		QGroupBox* table_Group_Box;
//			QHBoxLayout* table_GB_Layout;
//				QCheckBox* show_Line_CheckBox;
//				QCheckBox* show_Sine_CheckBox;
//				QCheckBox* show_Rand_CheckBox;
		QGroupBox* line_Group_Box;
			QGridLayout* line_GB_Layout;
				QLabel* line_Value_Label; QLineEdit* line_Value_Line;
		QGroupBox* sine_Group_Box;
			QGridLayout* sine_GB_Layout;
				QLabel* sine_Amplitude_Label; QLineEdit* sine_Amplitude_Line;
				QLabel* sine_Frequency_Label; QLineEdit* sine_Frequency_Line;
				QLabel* sine_Phase_Label;	  QLineEdit* sine_Phase_Line;
		QGroupBox* rand_Group_Box;
			QGridLayout* rand_GB_Layout;
				QLabel* rand_Rms_Label; QLineEdit* rand_Rms_Line;

        QPushButton* done_Button;
};

#endif // GRADING_EDITOR_H
