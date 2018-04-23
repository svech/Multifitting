// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QPropertyAnimation>
#include "spoiler.h"

Spoiler::Spoiler(const QString & title, const int animationDuration, QWidget *parent) :
	parent(parent),
	animationDuration(animationDuration)
{
	toggleButton.setStyleSheet("QToolButton { border: none; }");
	toggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	toggleButton.setArrowType(Qt::ArrowType::RightArrow);
	toggleButton.setText(title);
	toggleButton.setCheckable(true);
	toggleButton.setChecked(false);

	headerLine.setFrameShape(QFrame::HLine);
	headerLine.setFrameShadow(QFrame::Sunken);
	headerLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	contentArea.setStyleSheet("QScrollArea { border: none; }");//("QScrollArea { background-color: white; border: none; }");
	contentArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	// start out collapsed
	contentArea.setMaximumHeight(0);
	contentArea.setMinimumHeight(0);
	// let the entire widget grow and shrink with its content
	toggleAnimation.addAnimation(new QPropertyAnimation(this, "minimumHeight"));
	toggleAnimation.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
	toggleAnimation.addAnimation(new QPropertyAnimation(&contentArea, "maximumHeight"));
	// don't waste space
	mainLayout.setVerticalSpacing(0);
	mainLayout.setContentsMargins(0, 0, 0, 0);
	int row = 0;
	mainLayout.addWidget(&toggleButton, row, 0, 1, 1, Qt::AlignLeft);
	mainLayout.addWidget(&headerLine, row++, 2, 1, 1);
	mainLayout.addWidget(&contentArea,  row, 0, 1, 3);
	setLayout(&mainLayout);
	QObject::connect(&toggleButton, &QToolButton::clicked, [this](const bool checked)
	{
		toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
		toggleAnimation.setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
		toggleAnimation.start();
	});

	connect(&toggleAnimation, &QParallelAnimationGroup::finished, [=]
	{
		auto contentHeight = contentArea.layout()->sizeHint().height();
		if(toggleButton.isChecked())
		{
			parent->setFixedHeight(parent->height()+contentHeight);
		} else
		{
			parent->setFixedHeight(parent->height()-contentHeight);
		}
	});
}

void Spoiler::setContentLayout(QLayout & contentLayout)
{
	delete contentArea.layout();
	contentArea.setLayout(&contentLayout);
	const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
	auto contentHeight = contentLayout.sizeHint().height();
	for (int i = 0; i < toggleAnimation.animationCount() - 1; ++i)
	{
		QPropertyAnimation * spoilerAnimation = static_cast<QPropertyAnimation *>(toggleAnimation.animationAt(i));
		spoilerAnimation->setDuration(animationDuration);
		spoilerAnimation->setStartValue(collapsedHeight);
		spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
	}
	QPropertyAnimation * contentAnimation = static_cast<QPropertyAnimation *>(toggleAnimation.animationAt(toggleAnimation.animationCount() - 1));
	contentAnimation->setDuration(animationDuration);
	contentAnimation->setStartValue(0);
	contentAnimation->setEndValue(contentHeight);
}
