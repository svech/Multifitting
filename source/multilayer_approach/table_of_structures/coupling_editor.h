#ifndef COUPLING_EDITOR_H
#define COUPLING_EDITOR_H

#include "global/settings.h"
#include "my_table_widget.h"

class Coupling_Editor : public QDialog
{
	Q_OBJECT
public:
	explicit Coupling_Editor(Parameter parameter, QTabWidget* main_Tabs, QWidget *parent = 0);

	void closeEvent(QCloseEvent*);
	void set_Window_Geometry();
	void create_Main_Layout();

	void create_Master_Box();
	void create_Slave_Box();
		void set_Slave(int index_Pressed);
		void remove_Slave(int index_Pressed);
		void add_Slave(int index_Pressed);

	void enable_Getting_Parameter(QWidget* old, QWidget* now, QLabel* label, QLineEdit* line_Edit);
	void get_Parameter(QLabel* label);

	QTabWidget* main_Tabs;
	QVBoxLayout* main_Layout;

	QGroupBox* master_Group_Box;
		QLabel* master_Label;
		QLineEdit* master_Line_Edit;
		bool master_Connected = false;
		QPushButton* set_Master_Button;
		QPushButton* remove_Master_Button;

	QGroupBox* slave_Group_Box;
		QVBoxLayout* slave_Group_Box_Layout;
			QVector<QLabel*> slave_Label_Vec;
			QVector<QLineEdit*> slave_Line_Edit_Vec;
			QVector<bool> slave_Connected_Vec;

	QPushButton* done_Button;
};

#endif // COUPLING_EDITOR_H
