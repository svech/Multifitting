#ifndef MULTILAYER_H
#define MULTILAYER_H

#include "multilayer_approach.h"
#include "multilayer/structure_tree.h"
#include "multilayer/independent_variables.h"
#include "multilayer/target_curve.h"
#include "multilayer/target_curve/target_curve_editor.h"

class Structure_Tree;
class Multilayer_Approach;

class Multilayer : public QWidget
{
	Q_OBJECT
public:	
	explicit Multilayer(Multilayer_Approach* multilayer_Approach, QWidget *parent = 0);

signals:
	void refresh_All_Multilayers();

public:
	void create_Main_Layout();
		void create_Structure_Frame();
		void create_Variables_Frame();
			void create_Variables_Tabs();
				void create_Independent_Variables_Tabs();
		void create_Data_Frame();

	void add_Independent_Variables_Tab();
	void change_Tab_Independent_Variables_Tab_Color(int index);
	void remove_Independent_Variables_Tab(int index);
	void rename_Independent_Variables_Tab(int tab_Index);

	void refresh_Structure_And_Independent(QObject* my_Sender = NULL);
	void add_Target_Curve   (int index_Pressed);
	void remove_Target_Curve(int index_Pressed, bool clean = false);
	void open_Import_Window(Target_Curve* target_Curve);

	Multilayer_Approach* multilayer_Approach;

	QVBoxLayout* main_Layout;
		QFrame* struct_Frame;
			QVBoxLayout* struct_Frame_Layout;
				Structure_Tree* structure_Tree;

		QFrame* variables_Frame;
			QVBoxLayout* variables_Frame_Layout;
				QTabWidget*	variables_Tabs;
					QTabWidget* independent_Variables_Plot_Tabs;
						QToolButton* independent_Variables_Corner_Button;

					QPushButton* structure_Table_Button;

		QFrame* data_Frame;
			QVBoxLayout* data_Frame_Layout;
				QGroupBox* data_Target_Profile_Group_Box;
					QVBoxLayout* layout_Target_Profile_With_Frame_Vector;
						QVector<QFrame*> data_Target_Profile_Frame_Vector;
						QVector<Target_Curve*> target_Profiles_Vector;

	bool enable_Calc_Target_Curves = true;
	bool enable_Calc_Independent_Curves = true;
};

#endif // MULTILAYER_H
