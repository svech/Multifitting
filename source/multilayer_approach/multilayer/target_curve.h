#ifndef TARGET_CURVE_H
#define TARGET_CURVE_H

#include <QtWidgets>
#include "global/settings.h"

class Target_Curve : public QWidget
{
	Q_OBJECT
public:
	explicit Target_Curve(QLabel* description_Label, QWidget *parent = 0);
	~Target_Curve();

	QLabel* description_Label;
	Curve curve;

public slots:
	void open_Window();

private:
	void import_Data();

private:
	int valid_Lines_Counter = 0;
	QStringList lines_List;

};

#endif // TARGET_CURVE_H
