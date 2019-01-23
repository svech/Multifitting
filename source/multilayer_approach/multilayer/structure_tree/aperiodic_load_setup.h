#ifndef APERIODIC_LOAD_SETUP_H
#define APERIODIC_LOAD_SETUP_H

#include "multilayer_approach/multilayer/structure_tree/structure_toolbar.h"

class Aperiodic_Load_Setup : public QDialog
{
	Q_OBJECT
public:
	explicit Aperiodic_Load_Setup(Aperiodic_Settings& aperiodic_Settings, QString item_Type, QWidget* parent = nullptr);

	void set_Window_Geometry();
	void create_Main_Layout();
		void create_GroupBox();

	Aperiodic_Settings& aperiodic_Settings;
	QString item_Type;

	QVBoxLayout* main_Layout;
		QGroupBox* main_GroupBox;
		QPushButton* load_Button;
		QPushButton* close_Button;
};

#endif // APERIODIC_LOAD_SETUP_H
