#include "distribution_box.h"

Distribution_Box::Distribution_Box(QWidget *parent) : QWidget(parent)
{
	create_Main_Layout();
}

void Distribution_Box::create_Main_Layout()
{
	QVBoxLayout* main_Layout = new QVBoxLayout(this);
	groupbox = new QGroupBox("Distribution_Box");
	QGridLayout* groupbox_Layout = new QGridLayout(groupbox);
	main_Layout->addWidget(groupbox);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	resolution_Label = new QLabel("resolution_Label");
	groupbox_Layout->addWidget(resolution_Label,0,0,Qt::AlignLeft);

	resolution_SpinBox = new MyDoubleSpinBox(this,false);
		resolution_SpinBox->setAccelerated(true);
//		resolution_SpinBox->setRange(0, 1);
		resolution_SpinBox->setDecimals(6);
//		resolution_SpinBox->setValue(2020);
//		resolution_SpinBox->setSingleStep(0.333);
		resolution_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		resolution_SpinBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
	groupbox_Layout->addWidget(resolution_SpinBox,0,1,Qt::AlignLeft);
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	distribution_Label = new QLabel("Distribution");
	groupbox_Layout->addWidget(distribution_Label,1,0,Qt::AlignLeft);

	distribution_ComboBox = new QComboBox;
		distribution_ComboBox->addItems(distributions);
		distribution_ComboBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
	groupbox_Layout->addWidget(distribution_ComboBox,1,1,Qt::AlignLeft);
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	samples_Label = new QLabel("Number of samples");
	groupbox_Layout->addWidget(samples_Label,2,0,Qt::AlignLeft);

	sample_SpinBox = new QSpinBox;
		sample_SpinBox->setAccelerated(true);
		sample_SpinBox->setRange(1, MAX_INTEGER);
//		sample_SpinBox->setValue(2020);
		sample_SpinBox->setSingleStep(1);
		sample_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		sample_SpinBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
	groupbox_Layout->addWidget(sample_SpinBox,2,1,Qt::AlignLeft);
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	coverage_Label = new QLabel("Coverage");
	groupbox_Layout->addWidget(coverage_Label,3,0,Qt::AlignLeft);

	coverage_SpinBox = new MyDoubleSpinBox(this,false);
		coverage_SpinBox->setAccelerated(true);
		coverage_SpinBox->setRange(0, 5);
		coverage_SpinBox->setDecimals(3);
//		coverage_SpinBox->setValue(2020);
		coverage_SpinBox->setSingleStep(0.01);
		coverage_SpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
		coverage_SpinBox->setFixedWidth(DISTRIBUTION_BOX_FIELD_WIDTH);
	groupbox_Layout->addWidget(coverage_SpinBox,3,1,Qt::AlignLeft);
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	groupbox->adjustSize();
	groupbox->setFixedSize(groupbox->size());
}

