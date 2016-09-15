#include "multilayer.h"

Multilayer::Multilayer()
{
	QLabel *fileNameLabel = new QLabel(tr("File Name:"));
	QLineEdit *fileNameEdit = new QLineEdit("qq");

	QLabel *pathLabel = new QLabel(tr("Path:"));
	QLabel *pathValueLabel = new QLabel("qq");
	pathValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QLabel *sizeLabel = new QLabel(tr("Size:"));
	qlonglong size = 1024.00099/1024;
	QLabel *sizeValueLabel = new QLabel(tr("%1 K").arg(size));
	sizeValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QLabel *lastReadLabel = new QLabel(tr("Last Read:"));
	QLabel *lastReadValueLabel = new QLabel("yesterday");
	lastReadValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QLabel *lastModLabel = new QLabel(tr("Last Modified:"));
	QLabel *lastModValueLabel = new QLabel("today");
	lastModValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(fileNameLabel);
	mainLayout->addWidget(fileNameEdit);
//	mainLayout->addWidget(pathLabel);
//	mainLayout->addWidget(pathValueLabel);
//	mainLayout->addWidget(sizeLabel);
//	mainLayout->addWidget(sizeValueLabel);
//	mainLayout->addWidget(lastReadLabel);
//	mainLayout->addWidget(lastReadValueLabel);
//	mainLayout->addWidget(lastModLabel);
//	mainLayout->addWidget(lastModValueLabel);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}
