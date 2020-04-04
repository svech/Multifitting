#ifndef DISTRIBUTION_EDITOR_H
#define DISTRIBUTION_EDITOR_H

#include "distribution_box.h"

class Distribution_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Distribution_Editor(QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_One_Variable();
		void create_Plot();
		void create_Box();

	QVBoxLayout* main_Layout;
		QHBoxLayout* hor_Layout;

	QPushButton* close_Button;
};

#endif // DISTRIBUTION_EDITOR_H
