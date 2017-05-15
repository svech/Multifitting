#ifndef TARGET_CURVE_H
#define TARGET_CURVE_H

#include <QtWidgets>
#include "global/settings.h"

class Target_Curve : public QWidget
{
	Q_OBJECT
public:
	explicit Target_Curve(QLabel* description_Label, QTreeWidget* real_Struct_Tree, QWidget *parent = 0);
	~Target_Curve();

	QLabel* description_Label;
	QTreeWidget* real_Struct_Tree;
	QTreeWidget* struct_Tree_Copy;

	Curve curve;
	Measurement measurement;

public slots:
	void open_Window();

private:
	void import_Data();
	void create_Measurement();
	void create_Struct_Tree_Copy();

private:
	int valid_Lines_Counter = 0;
	QStringList lines_List;
	QString arg_Units;
};

#endif // TARGET_CURVE_H