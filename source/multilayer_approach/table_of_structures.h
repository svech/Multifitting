#ifndef TABLE_OF_STRUCTURES_H
#define TABLE_OF_STRUCTURES_H

#include "global/settings.h"

class Table_Of_Structures : public QWidget
{
	Q_OBJECT
	void closeEvent(QCloseEvent* event);
public:
	explicit Table_Of_Structures(QList<Table_Of_Structures*>* table_Of_Structures_List, QWidget *parent = 0);

signals:

public slots:

public:
	void create_Main_Layout();

	QTableWidget* main_Table;
private:
	QHBoxLayout* main_Layout;
	QList<Table_Of_Structures*>* table_Of_Structures_List;
};

#endif // TABLE_OF_STRUCTURES_H
