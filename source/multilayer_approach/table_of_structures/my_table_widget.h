#ifndef MY_TABLE_WIDGET_H
#define MY_TABLE_WIDGET_H

#include "global/settings.h"
#include "coupling_editor.h"

class My_Table_Widget : public QTableWidget
{
	Q_OBJECT
public:
	My_Table_Widget(int rows, int columns,
					QMap<QLineEdit*,QTreeWidgetItem*>& line_Edits_Map,
					QMap<QWidget*,QTreeWidgetItem*>& coupled_Widgets_Map,
					QTabWidget* main_Tabs, QWidget *parent = Q_NULLPTR);

	QTabWidget* main_Tabs;
	QMap<QLineEdit*,QTreeWidgetItem*>& line_Edits_Map;
	QMap<QWidget*,QTreeWidgetItem*>& coupled_Widgets_Map;

	void contextMenuEvent(QContextMenuEvent *event);
	void open_Coupling_Editor(QWidget *coupling_Widget);

	QWidget* get_Cell_Widget();
};

#endif // MY_TABLE_WIDGET_H
