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
private slots:
	void emit_Grading_Edited();
    void refresh_Data(QString q = "nothing");
    void refresh_Data_Bool(bool);

private:
    void create_Main_Layout();
        void create_Menu();
        void create_Interface();
            void read_Drift_From_Item();
    void set_Window_Geometry();

    QTreeWidgetItem* item;
	QString drift_Name;
	int default_Min_Line_Size;
    Drift drift;

private:
    QVBoxLayout* main_Layout;
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
