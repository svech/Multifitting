// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "target_curve.h"

Target_Curve::Target_Curve(QLabel* description_Label, QTreeWidget* real_Struct_Tree, QWidget *parent) :
	description_Label(description_Label),
	real_Struct_Tree(real_Struct_Tree),
	data(item_Type_Measurement),
	QWidget(parent)
{
	curve.argument_Type = whats_This_Angle;		// angular curve
	curve.angle_Type = angle_Type_Grazing/*default_angle_type*/	;
	curve.angular_Units = angle_Units_List[0]/*angle_units*/	;
	curve.spectral_Units = wavelength_Units_List[0]/*wavelength_units*/;
	curve.value_Function = value_Function[Reflectance];
	curve.value_Mode = value_R_Mode[R];			// R

	curve.arg_Offset = -0.01; curve.arg_Factor = 0.52;
	curve.val_Offset = 0.2;   curve.val_Factor = 1.034;

	data.calc_Measured_cos2_k();
//	struct_Tree_Copy = new QTreeWidget(this);
}

Target_Curve::~Target_Curve()
{
}

void Target_Curve::import_Data(QString filename)
{
	QRegExp delims("\\ |\\,|\\:|\\t|\\;|\\{|\\}");

	/// reading data
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
	} else
	{
		loaded_And_Ready = false;
		QMessageBox::information(this, "Target_Curve::import_Data", "Can't read file filename \"" + input_File.fileName() + "\"");
	}


	/// parsing data
	curve.argument.clear();
	curve.values.clear();
	QString main_Exception_Text = "short data in \"" + input_File.fileName() + "\"";
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

			if(curve.value_Mode == value_R_Mode[R] || curve.value_Mode == value_T_Mode[T] || curve.value_Mode == value_A_Mode[A])	// R , T , A
			{
				if(numbers.size()<=number_Index) throw "val_1 | " + main_Exception_Text;
				double temp_Number = QString(numbers[number_Index]).toDouble();
				++number_Index;

				Value val;
				val.val_1 = temp_Number;
				curve.values.push_back(val);
			} else
			if(curve.value_Mode == value_R_Mode[R_Phi] || curve.value_Mode == value_R_Mode[r_Re_Im] || curve.value_Mode == value_R_Mode[r_Abs_Phi])	// R+phi ; r, Re+Im ; |r|+phi
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
			loaded_And_Ready = true;
		}
		catch(QString exception)
		{
			loaded_And_Ready = false;
			QMessageBox::information(this, "Target_Curve::import_Data", exception);
			break;
		}
	}
}

void Target_Curve::fill_Measurement_With_Data()
{
	if(curve.argument_Type == whats_This_Angle)			// angular
	{
		data.angle = curve.argument;
	} else
	if(curve.argument_Type == whats_This_Wavelength)	// spectral
	{
		data.lambda = curve.argument;
	}
}

//void Target_Curve::create_Struct_Tree_Copy()
//{
//	struct_Tree_Copy = new QTreeWidget(this);
//	renew_Struct_Tree_Copy();
//}

//void Target_Curve::renew_Struct_Tree_Copy()
//{
//	struct_Tree_Copy->clear();
//	struct_Tree_Copy->hide();					// can be used for debugging
//	for(int i=0; i<real_Struct_Tree->topLevelItemCount(); i++)
//	{
//		struct_Tree_Copy->addTopLevelItem(real_Struct_Tree->topLevelItem(i)->clone());	// the data are also copied here (check?)
//	}

//	// add "measurement" item to the top
//	QTreeWidgetItem* new_Measurement_Item = new QTreeWidgetItem;

//	// set unique id to measurenent // NOT USED
////	int id = rand()*RAND_SHIFT+rand();
////	new_Measurement_Item->setStatusTip(DEFAULT_COLUMN, QString::number(id));

//	QVariant var;
//	var.setValue(data);
//	new_Measurement_Item->setData(DEFAULT_COLUMN, Qt::UserRole, var);
////	new_Measurement_Item->setWhatsThis(DEFAULT_COLUMN, item_Type_Measurement);
//	struct_Tree_Copy->insertTopLevelItem(0, new_Measurement_Item);
//}

void Target_Curve::set_Text_To_Label()
{
	description_Label->setText(label_Text);
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Target_Curve* target_Curve )
{
	return stream	<< target_Curve->curve << target_Curve->data << target_Curve->filename << target_Curve->filepath << target_Curve->loaded_And_Ready
					<< target_Curve->lines_List << target_Curve->arg_Units << target_Curve->at_Fixed << target_Curve->arg_Type_For_Label << target_Curve->label_Text
	;
}
QDataStream& operator >>(QDataStream& stream,		 Target_Curve* target_Curve )
{
	return stream	>> target_Curve->curve >> target_Curve->data >> target_Curve->filename >> target_Curve->filepath >> target_Curve->loaded_And_Ready
					>> target_Curve->lines_List >> target_Curve->arg_Units >> target_Curve->at_Fixed >> target_Curve->arg_Type_For_Label >> target_Curve->label_Text
	;
}
