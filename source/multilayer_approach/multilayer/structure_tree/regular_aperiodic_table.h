#ifndef REGULAR_APERIODIC_TABLE_H
#define REGULAR_APERIODIC_TABLE_H

#include "global/settings.h"
#include "multilayer_approach/multilayer_approach.h"
#include "regular_aperiodic_table_widget.h"

class MyDoubleSpinBox;

class Regular_Aperiodic_Table : public QDialog
{
	Q_OBJECT

public:
	Regular_Aperiodic_Table(QTreeWidgetItem* item,
							QWidget* parent = Q_NULLPTR);
signals:
	void regular_Aperiodic_Edited();

public:
	bool eventFilter(QObject *obj, QEvent *event);
	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
		void create_Menu();
		void create_Table();
			void create_Simple_Label (int current_Row, int current_Column, QString text);
			void create_Step_Spin_Box(int current_Row, int current_Column, QString whats_This);

	void refresh_Regular_Component(Data &current_Layer, int i);

	void set_Window_Geometry();
	void write_Window_Geometry();
	void emit_Regular_Aperiodic_Edited();

	QTreeWidgetItem* item;
	Data regular_Aperiodic_Data;
	Regular_Aperiodic_Table_Widget* regular_Table;

	QList<QLabel*> labels_List;
	QList<MyDoubleSpinBox*> spinboxes_List;
	QList<MyDoubleSpinBox*> thickness_Spinboxes_List;
	QList<MyDoubleSpinBox*> sigma_Spinboxes_List;
	QList<QCheckBox*> checkboxes_List;

	QVBoxLayout* main_Layout;
};

#endif // REGULAR_APERIODIC_TABLE_H
