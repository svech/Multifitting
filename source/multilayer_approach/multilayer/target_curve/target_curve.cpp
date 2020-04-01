#include "target_curve.h"

Target_Curve::Target_Curve(QLabel* description_Label, QTreeWidget* real_Struct_Tree, QWidget *parent) :
	description_Label(description_Label),
	real_Struct_Tree(real_Struct_Tree),
	measurement(item_Type_Measurement),
	QWidget(parent)
{
	curve.argument_Type = whats_This_Angle;		// angular curve
	curve.angular_Units = angle_Units_List[degree]/*angle_units*/	;
	curve.spectral_Units = wavelength_Units_List[angstrom]/*wavelength_units*/;
	curve.value_Function = specular_Value_Function[Reflectance];

//	curve.arg_Offset = 0; curve.arg_Factor = 1;
//	curve.val_Offset = 0; curve.val_Factor = 1;

	{
		plot_Options_Calculated.color=QColor(0, 0, 255);
		plot_Options_Calculated.scatter_Shape = QCPScatterStyle::ssDisc;
		plot_Options_Calculated.scatter_Size=0;
		plot_Options_Calculated.thickness=2;
	}
}

Target_Curve::~Target_Curve()
{
}

void Target_Curve::import_Data(QString bare_Filename)
{
	QFileInfo filename = bare_Filename;

	/// reading data
	QFile input_File(filename.absoluteFilePath());
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
		QMessageBox::information(this, "Target_Curve::import_Data", "Can't read file filename \"" + filename.fileName() + "\"");
		return;
	}

	/// parsing data
	curve.argument.clear();
	curve.values.clear();
	QString main_Exception_Text = "short data in \"" + filename.fileName() + "\"";
	for(int line_Index=0; line_Index<lines_List.size(); ++line_Index)
	{
		temp_Line = lines_List[line_Index];
		QStringList numbers = temp_Line.split(delimiters,QString::SkipEmptyParts);
		int number_Index = 0;

		// check if header
		temp_Line = temp_Line.simplified();
		bool is_Decimal = false;
		QString(temp_Line[0]).toInt(&is_Decimal);

		if(is_Decimal && numbers.size()>0)	// temp_Line[0]!=';' && temp_Line[0]!='#' && numbers.size()>0    less common
		try
		{
			bool ok_To_Double = false;

			/// argument

			if(numbers.size()<=number_Index) throw "arg | " + main_Exception_Text;
//			double temp_Argument = QString(numbers[number_Index]).toDouble(&ok_To_Double);
			double temp_Argument = QString(numbers[number_Index]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas

			if(!ok_To_Double) goto skip_line_label;

			if(curve.argument.size()>=2)
			{
				if(curve.argument[curve.argument.size()-1]>curve.argument[curve.argument.size()-2]) // increasing argument is allowed
				{
					if(temp_Argument <= curve.argument.last()) goto skip_line_label; // read only monotonical arguments
				}
				if(curve.argument[curve.argument.size()-1]<curve.argument[curve.argument.size()-2]) // decreasing argument is allowed
				{
					if(temp_Argument >= curve.argument.last()) goto skip_line_label; // read only monotonical arguments
				}
			}
			curve.argument.push_back(temp_Argument);
			++number_Index;

			/// value

			if(curve.value_Function == specular_Value_Function[Reflectance] || curve.value_Function == specular_Value_Function[Transmittance] )	// R , T
			{
				if(numbers.size()<=number_Index) throw "val_1 | " + main_Exception_Text;
//				double temp_Val = QString(numbers[number_Index]).toDouble(&ok_To_Double);
				double temp_Val = QString(numbers[number_Index]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas
				++number_Index;

				if(!ok_To_Double)
				{
					curve.argument.removeLast();
					goto skip_line_label;
				}
				curve.values.push_back(temp_Val);
			} else
			{
				throw "wrong curve.value_Function=" + curve.value_Function + main_Exception_Text;
			}
			loaded_And_Ready = true;

			// this line may be skipped
			skip_line_label: ok_To_Double = false;
		}
		catch(QString& exception)
		{
			loaded_And_Ready = false;
			QMessageBox::information(this, "Target_Curve::import_Data", exception);
			return;
		}
	}
	if(	curve.argument.size()!=curve.values.size() )
	{
		QMessageBox::critical(nullptr, "Target_Curve::import_Data", "curve.argument.size()!=curve.values.size()");
		loaded_And_Ready = false;
		return;
	}
}

void Target_Curve::fill_Measurement_With_Data()
{
	if(loaded_And_Ready)
	{
		// check zero intensity
		if(abs(curve.beam_Intensity_Start)<=DBL_EPSILON) curve.beam_Intensity_Start = 1;
		if(abs(curve.beam_Intensity_Final)<=DBL_EPSILON) curve.beam_Intensity_Final = 1;

		// preliminary fill calculated data with 0
		calculated_Values.R.	resize(curve.argument.size());	calculated_Values.R.fill(0);
		calculated_Values.Phi_R.resize(curve.argument.size());	calculated_Values.Phi_R.fill(0);
		calculated_Values.T.	resize(curve.argument.size());	calculated_Values.T.fill(0);
		calculated_Values.A.	resize(curve.argument.size());	calculated_Values.A.fill(0);

		// argument shift
		// shift has the same units as data
		curve.shifted_Argument.resize(curve.argument.size());
		curve.shifted_Values.resize(curve.argument.size());
		curve.shifted_Values_No_Scaling_And_Offset.resize(curve.argument.size());

		vector<double> intensity_Factor(curve.argument.size(),1);
		double delta = (curve.beam_Intensity_Final - curve.beam_Intensity_Start)/max(curve.argument.size()-1,1);
		if(curve.divide_On_Beam_Intensity)
		for(int i=0; i<curve.argument.size(); ++i)
		{
			intensity_Factor[i] = curve.beam_Intensity_Start + i*delta;
		}

		for(int i=0; i<curve.argument.size(); ++i)
		{
			curve.shifted_Argument[i] = curve.argument[i]                   * curve.horizontal_Arg_Factor      +curve.horizontal_Arg_Offset;
			curve.shifted_Values  [i] = curve.values[i]/intensity_Factor[i] * curve.val_Factor.value+curve.val_Offset;
			curve.shifted_Values_No_Scaling_And_Offset[i] = curve.values[i]/intensity_Factor[i];
		}

		// measurement filling
		if(curve.argument_Type == whats_This_Angle)			// angular
		{
			double coeff = angle_Coefficients_Map.value(curve.angular_Units);
			measurement.angle.resize(curve.shifted_Argument.size());
			for(int i=0; i<curve.shifted_Argument.size(); ++i)
			{
				measurement.angle[i] = curve.shifted_Argument[i]*coeff;
			}
		} else
		if(curve.argument_Type == whats_This_Wavelength)	// spectral
		{
			double coeff = wavelength_Coefficients_Map.value(curve.spectral_Units);
			measurement.lambda.resize(curve.shifted_Argument.size());
			for(int i=0; i<curve.shifted_Argument.size(); ++i)
			{
				measurement.lambda[i] = Global_Variables::wavelength_Energy(curve.spectral_Units,curve.shifted_Argument[i]*coeff);
			}
		} else
		{
			QMessageBox::critical(nullptr, "Target_Curve::fill_Measurement_With_Data", "wrong curve.argument_Type="+curve.argument_Type);
			return;
		}
	}
}

void Target_Curve::show_Description_Label()
{
	if(loaded_And_Ready)
	{
		QString spacer;
		if(curve.argument_Type == whats_This_Angle)
		{
			arg_Type_For_Label = "Angular";

			arg_Units = curve.angular_Units;

			double coeff = wavelength_Coefficients_Map.value(curve.spectral_Units);
			at_Fixed = Locale.toString(Global_Variables::wavelength_Energy(curve.spectral_Units,measurement.wavelength.value)/coeff, thumbnail_double_format, thumbnail_wavelength_precision)+" "+curve.spectral_Units;
			spacer = "";
		}
		if(curve.argument_Type == whats_This_Wavelength)
		{
			arg_Type_For_Label = "Spectral";
			arg_Units = curve.spectral_Units;

			double coeff = angle_Coefficients_Map.value(curve.angular_Units);
			at_Fixed = Locale.toString(measurement.probe_Angle.value/coeff, thumbnail_double_format, thumbnail_angle_precision)+" "+curve.angular_Units;
			spacer = " ";
		}

		label_Text = index + ": " +
					arg_Type_For_Label + "; " +
					specular_Value_Function_Short[specular_Value_Function.indexOf(curve.value_Function)] + "; " +
					Locale.toString(curve.shifted_Argument.first()) + "-" + Locale.toString(curve.shifted_Argument.last()) + spacer + arg_Units + "; " +
					"at " + at_Fixed;
	} else
	{
		label_Text = "<no description>";
	}

	description_Label->setText(label_Text);
}

Target_Curve& Target_Curve::operator =(const Target_Curve& referent_Target_Curve)
{
	description_Label->setText(referent_Target_Curve.description_Label->text());

	curve = referent_Target_Curve.curve;
	fit_Params = referent_Target_Curve.fit_Params;
	measurement = referent_Target_Curve.measurement;
	calculated_Values = referent_Target_Curve.calculated_Values;

	filename = referent_Target_Curve.filename;	// should be empty
	filepath = referent_Target_Curve.filepath;	// should be empty

	loaded_And_Ready = referent_Target_Curve.loaded_And_Ready;
	plot_Options_Experimental = referent_Target_Curve.plot_Options_Experimental;
	plot_Options_Calculated = referent_Target_Curve.plot_Options_Calculated;

	lines_List = referent_Target_Curve.lines_List;
	arg_Units = referent_Target_Curve.arg_Units;
	at_Fixed = referent_Target_Curve.at_Fixed;
	arg_Type_For_Label = referent_Target_Curve.arg_Type_For_Label;

	label_Text = referent_Target_Curve.label_Text;
	index = referent_Target_Curve.index;

	return *this;
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Target_Curve* target_Curve )
{
	return stream	<< target_Curve->curve << target_Curve->fit_Params << target_Curve->measurement << target_Curve->filename
					<< target_Curve->filepath << target_Curve->loaded_And_Ready	<< target_Curve->plot_Options_Experimental
					<< target_Curve->plot_Options_Calculated		// since 1.7.4
					<< target_Curve->calculated_Values
					<< target_Curve->lines_List << target_Curve->arg_Units << target_Curve->at_Fixed << target_Curve->arg_Type_For_Label << target_Curve->label_Text;
}
QDataStream& operator >>(QDataStream& stream,		 Target_Curve* target_Curve )
{
	stream	>> target_Curve->curve >> target_Curve->fit_Params >> target_Curve->measurement >> target_Curve->filename
			>> target_Curve->filepath >> target_Curve->loaded_And_Ready	>> target_Curve->plot_Options_Experimental;

	if(Global_Variables::check_Loaded_Version(1,7,4))
	{stream >> target_Curve->plot_Options_Calculated ; }	// since 1.7.4

	stream  >> target_Curve->calculated_Values
			>> target_Curve->lines_List >> target_Curve->arg_Units >> target_Curve->at_Fixed >> target_Curve->arg_Type_For_Label ;
	if(Global_Variables::check_Loaded_Version(1,11,0)) // aren't used since 1.11.0
	{
		QString ang_Type_For_Label_At_Fixed;
		stream >> ang_Type_For_Label_At_Fixed;
	}
	stream >> target_Curve->label_Text;
	return stream;
}
