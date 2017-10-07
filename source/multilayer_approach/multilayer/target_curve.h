#ifndef TARGET_CURVE_H
#define TARGET_CURVE_H

#include "global/settings.h"

class Target_Curve : public QWidget
{
	Q_OBJECT
public:
	explicit Target_Curve(QLabel* description_Label, QTreeWidget* real_Struct_Tree, QWidget *parent = 0);

	QLabel* description_Label;
	QTreeWidget* real_Struct_Tree;
//	QTreeWidget* struct_Tree_Copy; // need for calculations

	void import_Data(QString filename);
	void fill_Measurement_With_Data();
//	void renew_Struct_Tree_Copy();
	void set_Text_To_Label();

	Curve curve;
	Data measurement;

	QString filename;	// should be empty
	QString filepath;	// should be empty

	bool loaded_And_Ready = false;

	QStringList lines_List;
	QString arg_Units;
	QString at_Fixed;
	QString arg_Type_For_Label;

	QString label_Text = "<no description>";
};

QDataStream& operator <<( QDataStream& stream, const Target_Curve* target_Curve );
QDataStream& operator >>( QDataStream& stream,	     Target_Curve* target_Curve );

#endif // TARGET_CURVE_H
