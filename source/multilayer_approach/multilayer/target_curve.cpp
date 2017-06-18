// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "target_curve.h"

Target_Curve::Target_Curve(QLabel* description_Label, QTreeWidget* real_Struct_Tree, QWidget *parent) :
	description_Label(description_Label),
	real_Struct_Tree(real_Struct_Tree),
	QWidget(parent)
{
	curve.argument_Type = whats_This_Wavelength;		// angular curve
	curve.angle_Type = Angle_Type::Incidence()/*default_angle_type*/;
	curve.angular_Units = angle_Units_List[2]/*angle_units*/;
	curve.spectral_Units = wavelength_Units_List[1]/*wavelength_units*/;
	curve.value_Function = value_Function[0];	// reflectivity
	curve.value_Mode = value_R_Mode[3];			// R

	curve.arg_Offset = -0.01; curve.arg_Factor = 0.52;
	curve.val_Offset = 0.2;   curve.val_Factor = 1.034;

	measurement.calc_Measured_cos2_k();
}

Target_Curve::~Target_Curve()
{
}

void Target_Curve::open_Window()
{
//	import_Data();
//	create_Measurement();
//	create_Struct_Tree_Copy();
//	description_Label->setText(curve.argument_Type + "; " + curve.value_Mode + "; " + QString::number(curve.argument.first()) + "-" + QString::number(curve.argument.last()) + arg_Units);
}

void Target_Curve::import_Data()
{
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
	curve.argument.clear();
	curve.values.clear();
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

			if(curve.value_Mode == value_R_Mode[0])	// R
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
	double lambda_Value = 1.54056;
	double angle_Value = 1;
	measurement.polarization.value = 0;

	// type of argument
	if(curve.argument_Type == whats_This_Angle)	// angular
	{
		arg_Units = Degree_Sym;
		measurement.angle = curve.argument;
		measurement.lambda_Value = lambda_Value;
	} else
	if(curve.argument_Type == whats_This_Wavelength)	// spectral
	{
		arg_Units = " "+Angstrom_Sym;
		measurement.lambda = curve.argument;
		measurement.angle_Value = angle_Value;
	}

	// type of value
	if(curve.value_Mode == value_R_Mode[0])	// R
	{
		qInfo() << "R";
	} else
	if(curve.value_Mode == value_R_Mode[1])	// R + Phi
	{
		qInfo() << "R+Phi";
	} else
	if(curve.value_Mode == value_R_Mode[2])	// r
	{
		qInfo() << "r";
	}
	if(curve.value_Mode == value_R_Mode[3])	// r + phi
	{
		qInfo() << "r+phi";
	}
}

void Target_Curve::create_Struct_Tree_Copy()
{
	struct_Tree_Copy = new QTreeWidget(this);
	renew_Struct_Tree_Copy();
}

void Target_Curve::renew_Struct_Tree_Copy()
{
	struct_Tree_Copy->clear();
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
