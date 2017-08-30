#ifndef COUPLING_EDITOR_H
#define COUPLING_EDITOR_H

#include "global/settings.h"
#include "my_table_widget.h"

class Coupling_Editor : public QDialog
{
	Q_OBJECT
public:
	explicit Coupling_Editor(QWidget* coupling_Widget, QTabWidget* main_Tabs, QWidget *parent = 0);

	void closeEvent(QCloseEvent*);
	void set_Window_Geometry();
	void create_Main_Layout();

	void create_Master_Box();
		void load_Master();
		void save_Master();
	void create_Slave_Box();
		void set_Slave(int index_Pressed);
		void remove_Slave(int index_Pressed);
		void add_Slave(int index_Pressed);
		void load_Slaves();
		void save_Slaves();

	void enable_Getting_Parameter(QWidget* old, QWidget* now, QLabel* label, QLineEdit* line_Edit);
	void get_Parameter(QLabel* label);

	QWidget* search_Widget_By_Id(int id);

	QWidget* coupling_Widget;
	Parameter coupling_Parameter;
	QTabWidget* main_Tabs;
	QVBoxLayout* main_Layout;

	QGroupBox* master_Group_Box;
		QWidget* master_Widget;
		QWidget* old_Master_Widget;
		QLabel* master_Label;
		QLineEdit* master_Line_Edit;

		QPushButton* set_Master_Button;
		QPushButton* remove_Master_Button;

	QGroupBox* slave_Group_Box;
		QVBoxLayout* slave_Group_Box_Layout;
			QVector<QWidget*> slave_Widget_Vec;
			QVector<QWidget*> old_Slave_Widget_Vec;
			QVector<QLabel*> slave_Label_Vec;
			QVector<QLineEdit*> slave_Line_Edit_Vec;

	QPushButton* done_Button;

	QString no_Master_Text = ".........<no master>.........";
	QString no_Slave_Text  = ".........<no slave>..........";
};

#endif // COUPLING_EDITOR_H
