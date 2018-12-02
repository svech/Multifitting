#ifndef LAYER_THICKNESS_TRANSFER_H
#define LAYER_THICKNESS_TRANSFER_H

#include "table_of_structures.h"
#include "standard/mydoublespinbox.h"

class Layer_Thickness_Transfer : public QDialog
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize() { QWidget::adjustSize(); }
public:
	explicit Layer_Thickness_Transfer(QWidget* item_Widget,
									  Table_Of_Structures* table_Of_Structures,
									  QWidget* parent = nullptr);

	void closeEvent(QCloseEvent*);
	void set_Window_Geometry();
	void create_Main_Layout();
	void create_Content_Box();
	void create_Layer_Lines();
	void spinBox_Lambda(MyDoubleSpinBox* current_Child_SpinBox, MyDoubleSpinBox* partner_SpinBox);
	void refresh(MyDoubleSpinBox* spinbox);
	void map_Line_Edit(id_Type id, MyDoubleSpinBox* spinbox);
	void reload();
	void lock_Unlock_Thickness_Transfer(QTreeWidgetItem* item);

	QWidget* item_Widget;
	QTreeWidgetItem* struct_Item;
	Table_Of_Structures* table_Of_Structures;
	Data struct_Data;
	double coeff;
	double step;
	QString units_Suffix;

	QMap<MyDoubleSpinBox*,id_Type> map_Of_Identicals;
	QMap<MyDoubleSpinBox*,MyDoubleSpinBox*> map_Of_Partners;
	QMap<MyDoubleSpinBox*,MyDoubleSpinBox*> map_Of_Line_Edits;
	QMap<MyDoubleSpinBox*,QTreeWidgetItem*> map_Of_Items;

	QVBoxLayout* main_Layout;
		QGroupBox* content_Group_Box;
			QVBoxLayout* content_Group_Box_Layout;
			MyDoubleSpinBox* period_SpinBox;
			MyDoubleSpinBox* step_SpinBox;
};

#endif // LAYER_THICKNESS_TRANSFER_H
