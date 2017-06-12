#include "target_curve_editor.h"

Target_Curve_Editor::Target_Curve_Editor(Target_Curve* target_Curve, QWidget *parent) : QWidget(parent)
{
	create_Main_Layout();

	setWindowTitle("Import Data");
	set_Window_Geometry();
	setAttribute(Qt::WA_DeleteOnClose);
}

void Target_Curve_Editor::closeEvent(QCloseEvent *event)
{
	event;

}

void Target_Curve_Editor::set_Window_Geometry()
{
	adjustSize();
	setFixedSize(size());
}

void Target_Curve_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	main_Layout->setContentsMargins(4,10,4,0);

//	create_Interface();
//		main_Layout->addWidget(line_Group_Box);
//		main_Layout->addWidget(sine_Group_Box);
//		main_Layout->addWidget(rand_Group_Box);

	QPushButton* done_Button = new QPushButton("Done", this);
		done_Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		done_Button->setFocus();
		done_Button->setDefault(true);
	main_Layout->addWidget(done_Button,0,Qt::AlignCenter);

	connect(done_Button, SIGNAL(clicked()), this, SLOT(close()));
}
