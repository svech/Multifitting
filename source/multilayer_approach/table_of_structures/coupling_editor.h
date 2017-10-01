#ifndef COUPLING_EDITOR_H
#define COUPLING_EDITOR_H

#include "global/settings.h"
#include "my_table_widget.h"
#include "multilayer_approach/table_of_structures.h"

class Coupling_Editor : public QDialog
{
	Q_OBJECT
public:
	explicit Coupling_Editor(QWidget* coupling_Widget,
							 QMap<QWidget*,QTreeWidgetItem*>& coupled_Widgets_Item,
							 QMap<int, QWidget*>& coupled_Widgets_Id,
							 QTabWidget* main_Tabs,
							 QWidget *parent = 0);

	void closeEvent(QCloseEvent*);
	void set_Window_Geometry();
	void create_Main_Layout();

	void create_Master_Box();
		void remove_Master();
		void load_Master();
		void save_External_Master();
	void create_Slave_Box();
		void remove_Slave(int index_Pressed);
		void add_Slave(int index_Pressed);
		void load_Slaves();
		void save_External_Slaves();
		void clear_Nonexisting_Slaves();
	void refresh_Reload_Coupled(QString refresh_Reload, Parameter& parameter, QWidget* widget);
	void check_Expression();

	void enable_Getting_Parameter(QWidget* old, QWidget* now, QLabel* label, QLineEdit* line_Edit);
	void get_Parameter(QLabel* label);

	QWidget* coupling_Widget;
	Parameter coupling_Parameter;
	QTabWidget* main_Tabs;
	QVBoxLayout* main_Layout;

	QGroupBox* master_Group_Box;
		QWidget* master_Widget = NULL;
		QWidget* old_Master_Widget = NULL;
		QLabel* master_Label;
		QLineEdit* master_Line_Edit;

		QPushButton* remove_Master_Button;

	QGroupBox* slave_Group_Box;
		QVBoxLayout* slave_Group_Box_Layout;
			QVector<QWidget*> slave_Widget_Vec;
			QVector<QLabel*> slave_Label_Vec;
			QVector<QLineEdit*> slave_Line_Edit_Vec;
			QVector<Parameter_Indicator> old_Slaves;

	QPushButton* done_Button;

	QString no_Master_Text = ".........<no master>.........";
	QString no_Slave_Text  = ".........<no slave>..........";

	QMap<QWidget*,QTreeWidgetItem*>& coupled_Widgets_Item;
	QMap<int, QWidget*>& coupled_Widgets_Id;
};

#endif // COUPLING_EDITOR_H
