#ifndef MULTILAYER_H
#define MULTILAYER_H

#include <QtWidgets>
#include "layer_content.h"
#include "global_definitions.h"

class Multilayer : public QWidget
{
	Q_OBJECT

public:
	Multilayer();

	QSettings gui_Settings;
	QSettings default_Values;

private:
	void create_Main_Layout();
		void create_Struct_Frame();
			void create_Struct_Tree();
			void create_Struct_Toolbar();
		void create_Variables_Frame();
			void create_Variables_Tabs();
				void create_Dependent_Variables_Tabs();
					void create_Specular_Functions();
					void create_Nonspecular_Functions();
				void create_Independent_Variables_List();
				void create_Independent_Variables_Toolbar();

				void create_Coupled_Parameters_List();
				void create_Coupled_Parameters_Toolbar();

				void create_Fitting_Parameters_List();
				void create_Fitting_Parameters_Toolbar();

				void create_Optimization_Parameters_List();
				void create_Optimization_Parameters_Toolbar();
		void create_Data_Frame();

private slots:
	void add_Layer		(bool);
	void add_Multilayer	(bool);
	void add_Substrate	(bool);
	void edit			(bool);
	void remove			(bool);
	void cut			(bool);
	void copy			(bool);
	void paste			(bool);
	void move_Up		(bool);
	void move_Down		(bool);
	void group			(bool);
	void ungroup		(bool);
	void thickness_Plot	(bool);
	void sigma_Plot		(bool);
	void destroy		(bool);

	void if_Selected();
	void add_Buffered_Layer(QTreeWidgetItem*new_Layer_Passed);
	void refresh_Toolbar();

private slots:
	void add_Measured_Data();
	void remove_Measured_Data();
	void add_Target_Profile();
	void remove_Target_Profile();

// state variable
private:
	int different_Layers_Counter=0;
	QTreeWidgetItem* buffered=NULL;

// useful definitions
private:
	int default_Column = 0;
	QString what_is_This_Ambient = "!ambient!";
	QString what_is_This_Substrate = "!substrate!";

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
					QTabWidget*	dependent_Variables_Tabs;
						QVBoxLayout* specular_Functions_Layout;
							QGroupBox* standard_Functions_Group_Box;
								QCheckBox* reflect_Functions;
								QCheckBox* transmit_Functions;
								QCheckBox* absorp_Functions;
							QGroupBox* field_Functions_Group_Box;
								QCheckBox* field_Intensity;
								QCheckBox* joule_Absorption;
							QGroupBox* user_Functions_Group_Box;
								QCheckBox* user_Supplied_Functions_Check;
								QLineEdit* user_Supplied_Functions;
						QVBoxLayout* nonspecular_Functions_Layout;
					QListWidget* independent_Variables_List;
					QToolBar* independent_Variables_Toolbar;

					QListWidget* coupled_Parameters_List;
					QToolBar* coupled_Parameters_Toolbar;

					QListWidget* fitting_Parameters_List;
					QToolBar* fitting_Parameters_Toolbar;

					QListWidget* optimization_Parameters_List;
					QToolBar* optimization_Parameters_Toolbar;

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
