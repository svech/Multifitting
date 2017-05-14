// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "target_curve.h"

Target_Curve::Target_Curve(QLabel* description_Label, QTreeWidget* real_Struct_Tree, QWidget *parent) :
	description_Label(description_Label),
	real_Struct_Tree(real_Struct_Tree),
	QWidget(parent)
{

	qInfo() << "Target_Curve created!";
}

Target_Curve::~Target_Curve()
{
	qInfo() << "Target_Curve deleted!";
}

void Target_Curve::open_Window()
{
	import_Data();
	create_Measurement();
	create_Struct_Tree_Copy();
	description_Label->setText(curve.measurement_Type + "; " + curve.value_Type + "; " + QString::number(curve.argument.first()) + "-" + QString::number(curve.argument.last()) + arg_Units);
}

void Target_Curve::import_Data()
{
	// TEMPORARY
	curve.measurement_Type = measurement_Types[0];
	curve.value_Type = value_Types[0];
	curve.angle_Type = Angle_Type::Grazing();

	QRegExp delims("\\ |\\,|\\:|\\t|\\;|\\{|\\}");

	/// reading data
	QString filename = "measured.txt";
	QFile input_File(filename);
	QString temp_Line = "not empty now";

	if (input_File.open(QIODevice::ReadOnly))
	{
		QTextStream input_Stream(&input_File);
		lines_List.clear();

		while ( !input_Stream.atEnd() )
		{
			temp_Line=input_Stream.readLine();
			lines_List.append(temp_Line);
		}
		input_File.close();
	} else {qInfo() << "Target_Curve::import_Data  :  Can't read file filename";}


	/// parsing data
	QString main_Exception_Text = "Target_Curve::import_Data  :  short data in " + filename;

	for(int line_Index=0; line_Index<lines_List.size(); ++line_Index)
	{
		temp_Line = lines_List[line_Index];
		QStringList numbers = temp_Line.split(delims,QString::SkipEmptyParts);
		int number_Index = 0;

		if( temp_Line[0]!=';' && temp_Line[0]!='#' && numbers.size()>0)
		try
		{
			/// argument

			if(numbers.size()<=number_Index) throw "arg | " + main_Exception_Text;
			double temp_Number = QString(numbers[number_Index]).toDouble();
			curve.argument.push_back(temp_Number);
			++number_Index;

			/// value

			if(curve.value_Type == value_Types[0])	// R
			{
				if(numbers.size()<=number_Index) throw "val_1 | " + main_Exception_Text;
				double temp_Number = QString(numbers[number_Index]).toDouble();
				++number_Index;

				Value val;
				val.val_1 = temp_Number;
				curve.values.push_back(val);
			} else
			{
				if(numbers.size()<=number_Index) throw "val_1 | " + main_Exception_Text;
				double temp_Number_1 = QString(numbers[number_Index]).toDouble();
				++number_Index;
				if(numbers.size()<=number_Index) throw "val_2 | " + main_Exception_Text;
				double temp_Number_2 = QString(numbers[number_Index]).toDouble();
				++number_Index;

				Value val;
				val.val_1 = temp_Number_1;
				val.val_2 = temp_Number_2;
				curve.values.push_back(val);
			}
		}
		catch(QString exception)
		{
			qInfo() << exception;
		}
	}
}

void Target_Curve::create_Measurement()
{
	// TODO reduce units to default
	measurement.angle_Type = curve.angle_Type;

	// TODO temporary
	double lambda_Value = 113;
	double angle_Value = 85;

	// type of argument
	if(curve.measurement_Type == measurement_Types[0])	// angular
	{
		arg_Units = Degree_Sym;
		measurement.angle = curve.argument;
		measurement.lambda_Value = lambda_Value;
	} else
	if(curve.measurement_Type == measurement_Types[1])	// spectral
	{
		arg_Units = " "+Angstrom_Sym;
		measurement.lambda = curve.argument;
		measurement.angle_Value = angle_Value;
	}

	// type of value
	if(curve.value_Type == value_Types[0])	// R
	{
		qInfo() << "R";
	} else
	if(curve.value_Type == value_Types[1])	// R + Phi
	{
		qInfo() << "R+Phi";
	} else
	if(curve.value_Type == value_Types[2])	// r
	{
		qInfo() << "r";
	}
}

void Target_Curve::create_Struct_Tree_Copy()
{
	struct_Tree_Copy = new QTreeWidget(this);
	struct_Tree_Copy->hide();					// can be used for degugging
	for(int i=0; i<real_Struct_Tree->topLevelItemCount(); i++)
	{
		struct_Tree_Copy->addTopLevelItem(real_Struct_Tree->topLevelItem(i)->clone());	// the data are also copied here (check?)
	}

	// add "measurement" item to the top
	QTreeWidgetItem* new_Measurement_Item = new QTreeWidgetItem;
	// set unique id to measurenent // NOT USED
	int id = rand()*RAND_SHIFT+rand();
	new_Measurement_Item->setStatusTip(DEFAULT_COLUMN, QString::number(id));

	QVariant var; var.setValue(measurement);
	new_Measurement_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
	new_Measurement_Item->setWhatsThis(DEFAULT_COLUMN, whats_This_Measurement);
	struct_Tree_Copy->insertTopLevelItem(0, new_Measurement_Item);
}
