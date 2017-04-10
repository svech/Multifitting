#ifndef TABLE_OF_STRUCTURES_H
#define TABLE_OF_STRUCTURES_H

#include "global/settings.h"
#include "multilayer/structure_tree.h"

class Structure_Tree;

class Table_Of_Structures : public QWidget
{
	Q_OBJECT
	void closeEvent(QCloseEvent* event);
public:
	explicit Table_Of_Structures(QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures, QTabWidget* multilayer_Tabs, QWidget *parent = 0);

signals:

public slots:

public:
	void create_Main_Layout();
		void create_Table();
		void read_Trees();

	void add_Columns(int add_After);
	void span_Title_Rows();

	int basic_Row_Number = 0;
	int basic_Column_Number = 1;

	QTableWidget* main_Table;
	QList<Structure_Tree*> list_Of_Trees;
private:
	QList<int> rows_List_To_Span;
	QHBoxLayout* main_Layout;
	QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures;
	QTabWidget* multilayer_Tabs;

	Item_Type item_Type;

//	Measurement measurement;
	Ambient ambient;
	Layer layer;
	Stack_Content stack_Content;
	Substrate substrate;
};

#endif // TABLE_OF_STRUCTURES_H
