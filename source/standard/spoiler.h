#ifndef SPOILER_H
#define SPOILER_H

#include "global/layer_data_types.h"

class Spoiler : public QWidget
{
	Q_OBJECT
private:
//public:
	QGridLayout mainLayout;
	QToolButton toggleButton;
	QFrame headerLine;
	QParallelAnimationGroup toggleAnimation;
	QScrollArea contentArea;
	QWidget* parent;
	int animationDuration;
public:
	explicit Spoiler(const QString & title = "", const int animationDuration = 300, QWidget *parent = 0);
	void setContentLayout(QLayout & contentLayout);
};

#endif // SPOILER_H
