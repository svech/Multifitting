#ifndef MULTILAYER_H
#define MULTILAYER_H

#include <QtWidgets>
#include "layer_content.h"
#include "item_editing.h"
#include "global_definitions.h"
#include "independent_variables.h"

class Multilayer : public QWidget
{
	Q_OBJECT
	Q_INVOKABLE void refreshOverStruct() { Multilayer::refresh_Over_Struct(); }

public:
	Multilayer();
	~Multilayer();

	QSettings gui_Settings;
	QSettings default_Values;

	QMap<QTreeWidgetItem*, Item_Editing*> runned_Editors;
	QList<Item_Editing*> list_Editors;

	// temporary
	//------------------------------
	void print_Hidden_Copy(int index)
	{
		QTreeWidgetItemIterator it(plottable_Struct_Tree_Vec[index]);
		qInfo() << "\n---------------------\nplot " << index << ":\n";
		while(*it)
		{
			QTreeWidgetItem* item = *it;
			qInfo() << item->whatsThis(default_Column);
			++it;
		}
	}
	//------------------------------
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
				void create_Independent_Variables_Tabs();


				void create_Coupled_Parameters_List();
				void create_Coupled_Parameters_Toolbar();

				void create_Fitting_Parameters_List();
				void create_Fitting_Parameters_Toolbar();

				void create_Optimization_Parameters_List();
				void create_Optimization_Parameters_Toolbar();
		void create_Data_Frame();

private slots:
	// independent variables
	void add_Independent_Variables_Tab();
	void change_Tab_Independent_Variables_Tab_Color(int index);
	void remove_Independent_Variables_Tab(int index);
	void rename_Independent_Variables_Tab(int tab_Index);
	void reset_Independent_Variables_Structure();

	// structure toolbar
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
	void if_DoubleClicked(QTreeWidgetItem *item, int column);
	void add_Buffered_Layer(QTreeWidgetItem* new_Layer_Passed);
	void refresh_Toolbar();
	void set_Structure_Item_Text(QTreeWidgetItem* item);

	void editor_Close();
	void editors_Refresh();

	void multiple_Refresh_Over_Struct();
	void refresh_Over_Struct();
		void iterate_Over_Struct(QTreeWidgetItem* item = NULL);
			void refresh_If_Layer(QTreeWidgetItem* this_Item);
		void iterate_Over_Multilayers(QTreeWidgetItem* item = NULL);
			void refresh_If_Multilayer(QTreeWidgetItem* this_Item);
				void find_First_Layer(QTreeWidgetItem* this_Item, int& first);
				void find_Last_Layer(QTreeWidgetItem* this_Item, int& last);
				void find_Period(QTreeWidgetItem* this_Item);

	// data management
	void add_Measured_Data();
	void remove_Measured_Data();
	void add_Target_Profile();
	void remove_Target_Profile();

	// state variable
private:
	int different_Layers_Counter=0;
	QTreeWidgetItem* buffered=NULL;
	bool independent_Tabs_Exist = false;

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
					QTabWidget* independent_Variables_Plot_Tabs;
						QToolButton* independent_Variables_Corner_Button;
						QVector<Independent_Variables*> independent_Widget_Vec;
						QVector<QTreeWidget*> plottable_Struct_Tree_Vec;

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
