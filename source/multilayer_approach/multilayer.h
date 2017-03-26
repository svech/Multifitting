#ifndef MULTILAYER_H
#define MULTILAYER_H

#include "multilayer_approach.h"
#include "multilayer/structure_tree.h"
#include "multilayer/independent_variables.h"

class Structure_Tree;
class Multilayer_Approach;

class Multilayer : public QWidget
{
	Q_OBJECT
public:	
	explicit Multilayer(Multilayer_Approach* multilayer_Approach, QWidget *parent = 0);

signals:
	void refresh_All_Multilayers();

private:
	void create_Main_Layout();
		void create_Structure_Frame();
		void create_Variables_Frame();
			void create_Variables_Tabs();
				void create_Dependent_Variables_Tabs();
					void create_Specular_Functions();
					void create_Nonspecular_Functions();
				void create_Independent_Variables_Tabs();

				void create_Coupled_Parameters_List();
				void create_Coupled_Parameters_Toolbar();

				void create_Fitting_Parameters_List();
				void create_Fitting_Parameters_Toolbar();

				void create_Optimization_Parameters_List();
				void create_Optimization_Parameters_Toolbar();
		void create_Data_Frame();

public slots:
	void add_Independent_Variables_Tab();
	void change_Tab_Independent_Variables_Tab_Color(int index);
	void remove_Independent_Variables_Tab(int index);
	void rename_Independent_Variables_Tab(int tab_Index);

public slots:
	void refresh_Structure_And_Independent(QObject* my_Sender = NULL);

public slots:
//	void refresh_Text();
//	void refresh_State();

private slots:
	void add_Measured_Data();
	void remove_Measured_Data();
	void add_Target_Profile();
	void remove_Target_Profile();

public:
	Multilayer_Approach* multilayer_Approach;

	QVBoxLayout* main_Layout;
		QFrame* struct_Frame;
			QVBoxLayout* struct_Frame_Layout;
				Structure_Tree* structure_Tree;

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
					QTabWidget* independent_Variables_Plot_Tabs;
						QToolButton* independent_Variables_Corner_Button;

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
