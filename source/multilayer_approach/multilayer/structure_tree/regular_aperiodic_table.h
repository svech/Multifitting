#ifndef REGULAR_APERIODIC_TABLE_H
#define REGULAR_APERIODIC_TABLE_H

#include "global/settings.h"
#include "multilayer_approach/multilayer_approach.h"
#include "regular_aperiodic_table_widget.h"

class MyDoubleSpinBox;

class Regular_Aperiodic_Table : public QDialog
{
	Q_OBJECT
	Q_INVOKABLE void adjustSize() {  }
public:
	Regular_Aperiodic_Table(QTreeWidgetItem* item,
							Multilayer* multilayer,
							QWidget* parent = Q_NULLPTR);
signals:
	void regular_Aperiodic_Edited();
	void closed();

public:
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	void closeEvent(QCloseEvent* event);
	void create_Main_Layout();
		void create_Menu();
		void create_Table();
			void create_Simple_Label (int current_Row, int current_Column, QString text, QLabel* label = nullptr);
			void create_Step_Spin_Box(int current_Row, int current_Column, QString whats_This);
			void colorize_Material();
			void colorize_Thickness_Fit(QCheckBox* thickness_Fit_CheckBox);

	void refresh_Regular_Component(Data &current_Layer, int i);
	void refresh_Thickness_Checkboxes(int i, int n);
	void if_Fit_All(int i);
	void reload_One_Widget(QWidget* widget_To_Reload);
	void reload_All_Widgets(QString identifier = "");
		void reload_Auxiliary_Labels();
		void reload_Material_Labels();
		void reload_Thicknesses();
		void reload_Sigmas();
		void reload_Densities();
		void reload_Steps();
		void reload_Checkboxes();

	void set_Window_Geometry();
	void write_Window_Geometry();
	void emit_Regular_Aperiodic_Edited();
	void save();

	bool modifier_Key_Still_Pressed = false;
	bool modifier_Key_Still_Pressed_Duplicate = false;

	QTreeWidgetItem* item;
	Multilayer* multilayer;
	Data regular_Aperiodic_Data;
	Regular_Aperiodic_Table_Widget* regular_Table;

	QList<QLabel*> auxiliary_Labels_List;
	QList<QLabel*> material_Labels_List;
	QList<MyDoubleSpinBox*> thickness_Spinboxes_List;
	QList<MyDoubleSpinBox*> sigma_Spinboxes_List;
	QList<MyDoubleSpinBox*> density_Spinboxes_List;
	QList<MyDoubleSpinBox*> step_Spinboxes_List;
	QList<QCheckBox*> thickness_Checkboxes_List;
	QMap<QCheckBox*,QWidget*> thickness_Back_Widgets_Map;

	QVBoxLayout* main_Layout;
};

#endif // REGULAR_APERIODIC_TABLE_H
