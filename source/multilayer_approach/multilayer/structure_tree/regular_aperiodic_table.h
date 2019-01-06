#ifndef REGULAR_APERIODIC_TABLE_H
#define REGULAR_APERIODIC_TABLE_H

#include "global/settings.h"
#include "multilayer_approach/multilayer_approach.h"
#include "regular_aperiodic_table_widget.h"

class MyDoubleSpinBox;

class Regular_Aperiodic_Table : public QDialog
{
public:
	Regular_Aperiodic_Table(QTreeWidgetItem* item,
							QWidget* parent = Q_NULLPTR);

	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
		void create_Menu();
		void create_Table();
			void create_Simple_Label(int current_Row, int current_Column, QString text);

	void set_Window_Geometry();
	void write_Window_Geometry();

	QTreeWidgetItem* item;
	Data regular_Aperiodic_Data;
	Regular_Aperiodic_Table_Widget* regular_Table;

	QList<QLabel*> labels_List;
	QList<MyDoubleSpinBox*> spinboxes_List;
	QList<QCheckBox*> checkboxes_List;

	QVBoxLayout* main_Layout;
};

#endif // REGULAR_APERIODIC_TABLE_H
