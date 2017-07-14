#ifndef COUPLING_EDITOR_H
#define COUPLING_EDITOR_H

#include "global/settings.h"

class Coupling_Editor : public QDialog
{
	Q_OBJECT
public:
	explicit Coupling_Editor(Parameter parameter, QWidget *parent = 0);

	void set_Window_Geometry();
	void create_Main_Layout();

	void create_Master_Box();
	void create_Slave_Box();
		void set_Slave(int index_Pressed);
		void remove_Slave(int index_Pressed);
		void add_Slave(int index_Pressed);

	int global_Counter = 0;
	QVBoxLayout* main_Layout;

	QGroupBox* master_Group_Box;
		QLabel* master_Label;
		QLineEdit* master_Line_Edit;
		QPushButton* set_Master_Button;
		QPushButton* remove_Master_Button;

	QGroupBox* slave_Group_Box;
		QVBoxLayout* slave_Group_Box_Layout;
			QVector<QLabel*> slave_Label_Vec;
			QVector<QLineEdit*> slave_Line_Edit_Vec;

	QPushButton* done_Button;
};

#endif // COUPLING_EDITOR_H
