#ifndef CONFIDENCE_INTERVAL_EDITOR_H
#define CONFIDENCE_INTERVAL_EDITOR_H

#include "coupling_editor.h"

class Coupling_Editor;

class Confidence_Interval_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Confidence_Interval_Editor(Coupling_Editor* coupling_Editor, QWidget *parent = nullptr);

	void closeEvent(QCloseEvent*);
	void set_Window_Geometry();
	void create_Main_Layout();
		void create_Group_Box();

	Coupling_Editor* coupling_Editor;

	QString from_Text = "points, from";
	QString to_Text = "to";
	QString step_Text = "step size";
	QString end_Bracket_Text = "]";
	QString units = "";


	QVBoxLayout* main_Layout;
		QGroupBox* main_GroupBox;
			QVBoxLayout* groupBox_Layout;
				QLineEdit* num_Points;
				QLabel* from_Label;
				QLineEdit* min_Edit;
				QLabel* to_Label;
				QLineEdit* max_Edit;
				QLabel* units_Label;
				QLabel* step_Label;
				QLineEdit* step_Edit;
				QLabel* step_Units_Label;

	QPushButton* done_Button;
};

#endif // CONFIDENCE_INTERVAL_EDITOR_H
