#ifndef TARGET_CURVE_H
#define TARGET_CURVE_H

#include "global/settings.h"

class Target_Curve : public QWidget
{
	Q_OBJECT
public:
	explicit Target_Curve(QLabel* description_Label, QTreeWidget* real_Struct_Tree, QWidget *parent = 0);
	~Target_Curve();

	QLabel* description_Label;
	QTreeWidget* real_Struct_Tree;

	void import_Data(QString bare_Filename);
	void fill_Measurement_With_Data();
	void show_Description_Label();


	//------------------------------------------------------

	QString target_Data_Type = no_Data_Type;

	//------------------------------------------------------

	Curve curve;
	Fit_Params fit_Params;
	Data measurement;
	Calculated_Values calculated_Values;

	QString filename;	// should be empty
	QString filepath;	// should be empty

	bool loaded_And_Ready = false;
	Plot_Options plot_Options_Experimental;
	Plot_Options plot_Options_Calculated;

	QStringList lines_List;
	QString arg_Units;
	QString at_Fixed;
	QString ang_Type_For_Label_At_Fixed;
	QString arg_Type_For_Label;

	QString label_Text = "<no description>";
	QString index;

	Target_Curve& operator =(const Target_Curve& referent_Target_Curve);
};

QDataStream& operator <<( QDataStream& stream, const Target_Curve* target_Curve );
QDataStream& operator >>( QDataStream& stream,	     Target_Curve* target_Curve );

#endif // TARGET_CURVE_H
