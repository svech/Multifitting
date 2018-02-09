#ifndef MY_TABLE_WIDGET_H
#define MY_TABLE_WIDGET_H

#include "global/settings.h"
#include "coupling_editor.h"

class My_Table_Widget : public QTableWidget
{
	Q_OBJECT
public:
	My_Table_Widget(int rows,
					int columns,
					QMap<QWidget*, QTreeWidgetItem*>& coupled_Back_Widget_and_Struct_Item,
					QMap<QWidget*, id_Type>&		  coupled_Back_Widget_and_Id,
					QTabWidget* main_Tabs,
					QWidget* parent = Q_NULLPTR);

	QTabWidget* main_Tabs;

	QMap<QWidget*, QTreeWidgetItem*>& coupled_Back_Widget_and_Struct_Item;
	QMap<QWidget*, id_Type>&		  coupled_Back_Widget_and_Id;

	void contextMenuEvent(QContextMenuEvent *event);
	void open_Coupling_Editor(QWidget *coupling_Widget);

	QWidget* get_Cell_Widget();
};

#endif // MY_TABLE_WIDGET_H
