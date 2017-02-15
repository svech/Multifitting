#ifndef GRADING_EDITOR_H
#define GRADING_EDITOR_H

#include "global/settings.h"

class Grading_Editor : public QDialog
{
    Q_OBJECT
public:
    explicit Grading_Editor(QTreeWidgetItem* item, QString parameter_Name, QWidget* parent = 0);

signals:
    void refresh();
private slots:
    void emit_Refresh();

private:
    void create_Main_Layout();
        void create_Menu();
        void create_Interface();
            void read_Drift_From_Item();
    void set_Window_Geometry();

    QTreeWidgetItem* item;
    QString parameter_Name;

private:
    QVBoxLayout* main_Layout;
        QGroupBox* group_Box;
            QVBoxLayout* group_Box_Layout;
                QCheckBox* line_Check;
                    QLineEdit* line_Line;
                QCheckBox* sine_Check;
                    QLineEdit* sine_Amplitude_Line;
                    QLineEdit* sine_Frequency_Line;
                    QLineEdit* sine_Phase_Line;
                QCheckBox* rand_Check;
                    QLineEdit* rms_Line;

        QPushButton* done_Button;
};

#endif // GRADING_EDITOR_H
