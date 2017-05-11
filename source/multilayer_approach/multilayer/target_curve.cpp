#include "target_curve.h"

Target_Curve::Target_Curve(QLabel* description_Label, QWidget *parent) :
	description_Label(description_Label),
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
	description_Label->setText("imported");
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

			// common part
			{
				if(numbers.size()<=number_Index) throw "arg | " + main_Exception_Text;
				double temp_Number = QString(numbers[number_Index]).toDouble();
				curve.argument.push_back(temp_Number);
				++number_Index;
			}

			// type of argument
			if(curve.measurement_Type == measurement_Types[0])	// angular
			{
				if(curve.angle_Type == Angle_Type::Grazing())
				{

				}
				if(curve.angle_Type == Angle_Type::Incidence())
				{

				}
			} else
			if(curve.measurement_Type == measurement_Types[1])	// spectral
			{

			}

			/// value

			// type of value
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
				// common part
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

				if(curve.value_Type == value_Types[1])	// R + Phi
				{

				} else
				if(curve.value_Type == value_Types[2])	// r
				{

				}
			}
		}
		catch(QString exception )
		{
			qInfo() << exception;
		}
	}
}


