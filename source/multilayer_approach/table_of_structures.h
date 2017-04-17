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
	void closed();
	void data_Edited();

public slots:
	void resize_Line_Edit(QString text, QLineEdit* line_Edit = NULL);
	void emit_Data_Edited();

	void refresh_Element(QString temp = "empty");
	void refresh_Stoich(QString temp = "empty");
	void refresh_Fit_Element(bool b = false);

	void refresh_Material(QString temp = "empty");

	void refresh_Parameter(QString temp = "empty");


	void cells_On_Off(bool b = false);

	void check_Material();
	void browse_Material(bool b = false);

public:
	void create_Main_Layout();
		void create_Table();
		void read_Trees();

	void add_Columns(int add_After);
		void create_Combo_Elements(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
		void create_Stoich(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString val_Type);
		void create_Stoich_Check_Box_Fit(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);

		void create_Label(int current_Row, int current_Column, QString text);
		void create_Line_Edit(int current_Row, int current_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This, QString val_Type);
		void create_Check_Box_Fit(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String, QString whats_This);

		void create_Material(int current_Row, int start_Column, QTreeWidgetItem* structure_Item, QString item_Type_String);
		void create_Browse_Button(int current_Row, int start_Column);

	QString material_From_Composition(QList<Stoichiometry>& composition);
	void span_Structure_Headers();
	void span_Structure_Items();
	void fit_Column(int start_Width, int current_Column);


	int basic_Row_Number = 0;
	int basic_Column_Number = 1;

	QTableWidget* main_Table;
	QList<Structure_Tree*> list_Of_Trees;
private:
	QList<int> rows_List_To_Span;
	QHBoxLayout* main_Layout;
	QMap<QString, Table_Of_Structures*>* runned_Tables_Of_Structures;
	QTabWidget* multilayer_Tabs;

	QString item_Type_String;

	QMap<QComboBox*,QTreeWidgetItem*> elements_Map;
	QMap<QLineEdit*,QTreeWidgetItem*> line_Edits_Map;
	QMap<QCheckBox*,QTreeWidgetItem*> check_Boxes_Map;
};

#endif // TABLE_OF_STRUCTURES_H
