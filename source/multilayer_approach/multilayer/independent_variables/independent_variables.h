#ifndef INDEPENDENT_VARIABLES_H
#define INDEPENDENT_VARIABLES_H

#include "multilayer_approach/multilayer/independent_variables/independent_variables_editor.h"
#include "variable_selection.h"

class Independent_Variables : public QWidget
{
	Q_OBJECT
public:
	explicit Independent_Variables(QTreeWidget* real_Struct_Tree, QWidget *parent = 0);
	~Independent_Variables();

signals:
	void refresh_Multilayer();

public:
	void create_Struct_Tree_Copy();
	void create_Main_Layout();
	void create_Independent_Variables_List();
	void create_Independent_Variables_Toolbar();

	void remove_Independent_Variable(QListWidgetItem* list_Item);

	void add_Independent_Variable();
		void cause_Editing_Independent_Variable();
		void edit_Independent_Variable(QListWidgetItem* list_Item);
		void remove_Current_Independent_Variable();
		void choose_Independent_Calc_Function();
	void refresh_Independent_State_All();

	void if_Selected();

	void reset_Independent_Variables_Structure();
	void clear_Structure_Copy();
	void clear_Unused_Independent_List();
	void refresh_Text();
	QString enlarge_Tab_Name();

	QString tab_Name;

	QTreeWidgetItem* measurement_Item;
	Data measurement;
	Calculated_Values calculated_Values;

	Calc_Functions calc_Functions;
	Plot_Options plot_Options;

	QMap<id_Type, QListWidgetItem*>* independent_Variables_List_Map;		// independent IDs and corresponding items in ListWidget
	QTreeWidget* struct_Tree_Copy;										// modified copy of real tree to be plotted
	QTreeWidget* real_Struct_Tree;										// real tree itself

	QString argument_Type;
	QListWidget* independent_Variables_List;
	QToolBar* independent_Variables_Toolbar;
};

QDataStream& operator <<( QDataStream& stream, const Independent_Variables* independent_Variables );
QDataStream& operator >>( QDataStream& stream,	     Independent_Variables* independent_Variables );

#endif // INDEPENDENT_VARIABLES_H
