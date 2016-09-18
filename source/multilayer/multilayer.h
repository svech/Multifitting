#ifndef MULTILAYER_H
#define MULTILAYER_H

#include <QtWidgets>
#include "settings.h"

class Multilayer : public QWidget
{
	Q_OBJECT

public:
	Multilayer(Settings* settings);

	Settings* settings;

private:
	void create_Main_Layout();
		void create_Struct_Frame();
			void create_Struct_Tree();
			void create_Struct_Toolbar();
		void create_Variables_Frame();
			void create_Variable_Tabs();
		void create_Data_Frame();

private slots:
	void add_Measured_Data();
	void remove_Measured_Data();
	void add_Target_Profile();
	void remove_Target_Profile();

// ui objects to be placed in Multilayer tab
private:
	QVBoxLayout* main_Layout;
		QFrame* struct_Frame;
			QVBoxLayout* struct_Frame_Layout;
				QTreeWidget* struct_Tree;
				QToolBar* struct_Toolbar;
		QFrame* variables_Frame;
			QVBoxLayout* variables_Frame_Layout;
				QTabWidget*	variables_Tabs;
		QFrame* data_Frame;
			QVBoxLayout* data_Frame_Layout;
				QGroupBox* data_Measured_Data_Group_Box;
					QVBoxLayout* layout_Measured_Data_With_Frame_Vector;
						QVector<QFrame*> data_Measured_Data_Frame_Vector;
				QGroupBox* data_Target_Profile_Group_Box;
					QVBoxLayout* layout_Target_Profile_With_Frame_Vector;
						QVector<QFrame*> data_Target_Profile_Frame_Vector;
};

#endif // MULTILAYER_H
