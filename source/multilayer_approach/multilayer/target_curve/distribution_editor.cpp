#include "distribution_editor.h"

Distribution_Editor::Distribution_Editor(QWidget *parent) :
	QWidget(parent)
{
	setWindowTitle("Set up distribution");
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Window);
	create_Main_Layout();
}

void Distribution_Editor::create_Main_Layout()
{
	main_Layout = new QVBoxLayout(this);
	hor_Layout = new QHBoxLayout;
	main_Layout->addLayout(hor_Layout);

	create_One_Variable();
	create_One_Variable();
	create_One_Variable();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	close_Button = new QPushButton("Close");
		close_Button->adjustSize();
		close_Button->setFixedSize(close_Button->size());
	main_Layout->addWidget(close_Button,0,Qt::AlignRight);

	connect(close_Button, &QPushButton::clicked, this, [=]{close();});
}

void Distribution_Editor::create_One_Variable()
{
	QVBoxLayout* one_Variable_Layout = new QVBoxLayout;
	one_Variable_Layout->setSpacing(0);
	hor_Layout->addLayout(one_Variable_Layout);

	QCustomPlot* custom_Plot = new QCustomPlot;
	one_Variable_Layout->addWidget(custom_Plot,Qt::AlignBottom);

	Distribution_Box* distribution_Box = new Distribution_Box;
	one_Variable_Layout->addWidget(distribution_Box,0,Qt::AlignCenter);
}

void Distribution_Editor::create_Plot()
{




}

void Distribution_Editor::create_Box()
{


}
