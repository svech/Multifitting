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

public:
	QString filename;	// should be empty
	QString filepath;	// should be empty

	void import_Data(QString filename);
	void create_Struct_Tree_Copy();
	void renew_Struct_Tree_Copy();

	QStringList lines_List;
	QString arg_Units;
	QString at_Fixed;
	QString arg_Type_For_Label;

	bool loaded_And_Ready = false;
};

#endif // TARGET_CURVE_H
