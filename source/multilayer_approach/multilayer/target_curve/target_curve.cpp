#include "target_curve.h"

Curve::Curve()
{
	// mesh density : 1D only
	mesh_Density_Factor = 1;
	mesh_Density_Shift = 0.5;

	// subinterval
	use_Subinterval = false;
	subinterval_Left = 0;
	subinterval_Right = 1;
	// 2D only
	subinterval_Top = 1;
	subinterval_Bottom = -1;

	// argument shift & factor
	horizontal_Arg_Shift = 0;
	horizontal_Arg_Factor = 1;
	// 2D only
	vertical_Arg_Shift = 0;
	vertical_Arg_Factor = 1;

	// value shift & factor
	val_Shift = 0;
	val_Factor.value = 1;
	val_Factor.fit.min = 0.1;
	val_Factor.fit.max = 10;
	val_Factor.indicator.full_Name = "Target curve scale factor";

	// intensity
	divide_On_Beam_Intensity = true;
	beam_Intensity_Initial = 1;
	// 1D only
	use_Final_Intensity = false;
	beam_Intensity_Final = 1;

	// units
	angular_Units = angle_Units_List[degree];
	spectral_Units = wavelength_Units_List[angstrom];

	// type
	value_Type = no_Value_Type;
}

Fit_Params::Fit_Params()
{
	calc = true;
	fit = true;
	norm = true;
	adjust_Scale_Factor = false;
	maximize_Integral = false;
	weight = 1;
	weight_Sqrt = 1;
	fit_Function = "log("+QString(fit_Function_Variable)+"+1E-5)";// "log(R+1E-5); sin(phi/2)";
	power = 2;
	use_Chi2 = false;
}
void Fit_Params::create_Expressions_for_Residual()
{
   expression_Vec.clear();

#ifdef EXPRTK
   exprtk::parser<double> parser;
   exprtk::symbol_table<double> symbol_table;
   symbol_table.add_variable(fit_Function_Variable, expression_Argument);
   symbol_table.add_constants();
#endif

   QVector<QString> bare_String_Expressions = fit_Function.split(fit_Function_Separator, QString::SkipEmptyParts).toVector();
   for(int i=0; i<bare_String_Expressions.size(); ++i)
   {
	   if(!bare_String_Expressions[i].split(" ", QString::SkipEmptyParts).empty())
	   {
#ifdef EXPRTK
		   exprtk::expression<double> new_Expression;
#else
		   QString new_Expression;
#endif
		   expression_Vec.append(new_Expression);
#ifdef EXPRTK
		   expression_Vec[i].register_symbol_table(symbol_table);
		   parser.compile(bare_String_Expressions[i].toStdString(), expression_Vec[i]);
#endif
	   }
   }
}

Target_Curve::Target_Curve(QLabel* description_Label, QWidget *parent) :
	description_Label(description_Label),
	measurement(item_Type_Measurement),
	QWidget(parent)
{
	plot_Options_Calculated.color=QColor(0, 0, 255);
	plot_Options_Calculated.scatter_Shape = QCPScatterStyle::ssDisc;
	plot_Options_Calculated.scatter_Size=0;
	plot_Options_Calculated.thickness=2;
}

void Target_Curve::import_Data_From_File(QString bare_Filename)
{
	QFileInfo filename = bare_Filename;

	// reading whole file to lines_kist
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
}

void Target_Curve::parse_Data_From_List()
{
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] ||
		measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		parse_1D_Data();
	}
	if(	measurement.measurement_Type == measurement_Types[GISAS] )
	{
		parse_2D_Data();
	}
}

void Target_Curve::parse_1D_Data()
{
	curve.argument.clear();
	curve.values.clear();

	for(int line_Index=0; line_Index<lines_List.size(); ++line_Index)
	{
		QString temp_Line = lines_List[line_Index];
		temp_Line = temp_Line.simplified();

		// check if not header line (starts from digit)
		bool is_Decimal = false;
		QString(temp_Line[0]).toInt(&is_Decimal);

		// split line into "numbers"
		QStringList potentional_Numbers = temp_Line.split(delimiters,QString::SkipEmptyParts);

		if(is_Decimal && potentional_Numbers.size()>=2)	// temp_Line[0]!=';' && temp_Line[0]!='#' && numbers.size()>0    less common
		try
		{
			bool ok_To_Double = false;

			double temp_Argument = QString(potentional_Numbers[0]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas are considered as dots
			double temp_Value    = QString(potentional_Numbers[1]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas are considered as dots

			if(!ok_To_Double) goto skip_line_label;

			// argument should be monotonic
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
			curve.values.push_back(temp_Value);

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
}

void Target_Curve::parse_2D_Data()
{
	// TODO
	qInfo() << "Target_Curve::parse_2D_Data() not implemented" << endl;
}

void Target_Curve::fill_Measurement_And_Curve_With_Shifted_Data()
{
	if( measurement.measurement_Type == measurement_Types[Specular_Scan] ||
		measurement.measurement_Type == measurement_Types[Detector_Scan] ||
		measurement.measurement_Type == measurement_Types[Rocking_Curve] ||
		measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		fill_Measurement_And_Curve_With_Shifted_1D_Data();
	}
	if(	measurement.measurement_Type == measurement_Types[GISAS] )
	{
		fill_Measurement_And_Curve_With_Shifted_2D_Data();
	}
}

void Target_Curve::fill_Measurement_And_Curve_With_Shifted_1D_Data()
{
	if(loaded_And_Ready)
	{
		// shifted data filling. shift has the same units as main data
		curve.shifted_Argument.resize(curve.argument.size());
		curve.shifted_Values.resize(curve.argument.size());
		curve.shifted_Values_No_Scaling_And_Offset.resize(curve.argument.size());

		vector<double> intensity_Factor(curve.argument.size(),1);
		double delta = (curve.beam_Intensity_Final - curve.beam_Intensity_Initial)/max(curve.argument.size()-1,1);
		if(curve.divide_On_Beam_Intensity)
		{
			for(int i=0; i<curve.argument.size(); ++i)
			{
				intensity_Factor[i] = curve.beam_Intensity_Initial + i*delta;
			}
		}

		for(int i=0; i<curve.argument.size(); ++i)
		{
			curve.shifted_Argument[i] = curve.argument[i]                     * curve.horizontal_Arg_Factor + curve.horizontal_Arg_Shift;
			curve.shifted_Values  [i] = curve.values  [i]/intensity_Factor[i] * curve.val_Factor.value      + curve.val_Shift;
			curve.shifted_Values_No_Scaling_And_Offset[i] = curve.values[i]/intensity_Factor[i];
		}

		// measurement filling
		if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
		{
			double coeff = angle_Coefficients_Map.value(curve.angular_Units);
			measurement.beam_Theta_0_Angle_Vec.resize(curve.shifted_Argument.size());
			for(int i=0; i<curve.shifted_Argument.size(); ++i)
			{
				measurement.beam_Theta_0_Angle_Vec[i] = curve.shifted_Argument[i]*coeff;
			}
		}
		if(measurement.argument_Type == argument_Types[Wavelength_Energy])
		{
			double coeff = wavelength_Coefficients_Map.value(curve.spectral_Units);
			measurement.lambda_Vec.resize(curve.shifted_Argument.size());
			for(int i=0; i<curve.shifted_Argument.size(); ++i)
			{
				measurement.lambda_Vec[i] = Global_Variables::wavelength_Energy(curve.spectral_Units, curve.shifted_Argument[i]*coeff);
			}
		}
	}
}

void Target_Curve::fill_Measurement_And_Curve_With_Shifted_2D_Data()
{
	// TODO
	qInfo() << "Target_Curve::fill_Measurement_And_Curve_With_Shifted_2D_Data()) not implemented" << endl;
}

void Target_Curve::refresh_Description_Label()
{
	if(loaded_And_Ready)
	{
		double spectral_Coeff = wavelength_Coefficients_Map.value(curve.spectral_Units);
		double angular_Coeff = angle_Coefficients_Map.value(curve.angular_Units);

		if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{		
				label_Text =  curve.value_Type + "; " +
							  Locale.toString(curve.shifted_Argument.first()) + "-" + Locale.toString(curve.shifted_Argument.last()) +
							  "" + curve.angular_Units + "; " + "at " +
							  Locale.toString(Global_Variables::wavelength_Energy(curve.spectral_Units,measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + curve.spectral_Units;
			}
			if(measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
				label_Text =  curve.value_Type + "; " +
							  Locale.toString(curve.shifted_Argument.first()) + "-" + Locale.toString(curve.shifted_Argument.last()) +
							  "" + curve.spectral_Units + "; " + "at " +
							  Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + curve.angular_Units;
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Detector_Scan] )
		{
	//		detector_Target_Curve_Part->refresh_Description_Label();
		}
		if(	measurement.measurement_Type == measurement_Types[Rocking_Curve] )
		{
	//		rocking_Target_Curve_Part->refresh_Description_Label();
		}
		if(	measurement.measurement_Type == measurement_Types[Offset_Scan] )
		{
	//		offset_Target_Curve_Part->refresh_Description_Label();
		}
		if(	measurement.measurement_Type == measurement_Types[GISAS] )
		{
	//		gisas_Target_Curve_Part->refresh_Description_Label();
		}

		description_Label->setText(index + ": " + label_Text);
	} else
	{
		label_Text = "<no description>";
		description_Label->setText(label_Text);
	}
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

	label_Text = referent_Target_Curve.label_Text;
	index = referent_Target_Curve.index;

	return *this;
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Value& value )
{
	return stream << value.val_1 << value.val_2;
}
QDataStream& operator >>( QDataStream& stream,		 Value& value )
{
	return stream >> value.val_1 >> value.val_2;
}

QDataStream& operator <<( QDataStream& stream, const Curve& curve )
{
	return stream	<< curve.mesh_Density_Factor << curve.mesh_Density_Shift
					<< curve.use_Subinterval << curve.subinterval_Left << curve.subinterval_Right << curve.subinterval_Top << curve.subinterval_Bottom
					<< curve.horizontal_Arg_Shift << curve.horizontal_Arg_Factor << curve.vertical_Arg_Shift << curve.vertical_Arg_Factor
					<< curve.val_Shift << curve.val_Factor
					<< curve.divide_On_Beam_Intensity << curve.beam_Intensity_Initial << curve.use_Final_Intensity << curve.beam_Intensity_Final
					<< curve.angular_Units << curve.spectral_Units << curve.value_Type;
}
QDataStream& operator >>( QDataStream& stream,		 Curve& curve )
{
	if(Global_Variables::check_Loaded_Version(1,11,0))		// since 1.11.0
	{
		stream	>> curve.mesh_Density_Factor >> curve.mesh_Density_Shift
				>> curve.use_Subinterval >> curve.subinterval_Left >> curve.subinterval_Right >> curve.subinterval_Top >> curve.subinterval_Bottom
				>> curve.horizontal_Arg_Shift >> curve.horizontal_Arg_Factor >> curve.vertical_Arg_Shift >> curve.vertical_Arg_Factor
				>> curve.val_Shift >> curve.val_Factor
				>> curve.divide_On_Beam_Intensity >> curve.beam_Intensity_Initial >> curve.use_Final_Intensity >> curve.beam_Intensity_Final
				>> curve.angular_Units >> curve.spectral_Units >> curve.value_Type;		
	} else // before 1.11.0
	{
		if(Global_Variables::check_Loaded_Version(1,10,1))		// since 1.10.1
		{
			stream >> curve.use_Subinterval;
			if(Global_Variables::check_Loaded_Version(1,10,2))		// since 1.10.2
			{
				stream >> curve.mesh_Density_Factor >> curve.mesh_Density_Shift;  // Parameter since 1.10.2
			}
			stream >> curve.subinterval_Left >> curve.subinterval_Right;
		}
		stream  >> curve.argument >> curve.shifted_Argument;

		// values are just double now
		{
			// read
			QVector<Value> values;
			QVector<Value> shifted_Values;
			stream >> values >> shifted_Values;
			// convert
			curve.values.resize(values.size());
			curve.shifted_Values.resize(values.size());
			for(int i=0; i<values.size(); i++)
			{
				curve.values[i] = values[i].val_1;
				curve.shifted_Values[i] = shifted_Values[i].val_1;
			}
		}

		stream >> curve.horizontal_Arg_Shift >> curve.horizontal_Arg_Factor;
		stream >> curve.val_Shift;

		if(Global_Variables::check_Loaded_Version(1,10,2))		// since 1.10.2
			 {stream >> curve.val_Factor;}
		else {stream >> curve.val_Factor.value;}

		// instead of loading shifted_Values_No_Scaling_And_Offset // since 1.10.2
		curve.shifted_Values_No_Scaling_And_Offset.resize(curve.shifted_Values.size());
		for(int i=0; i<curve.shifted_Values.size(); i++)	{
			curve.shifted_Values_No_Scaling_And_Offset[i] = (curve.shifted_Values[i] - curve.val_Shift)/curve.val_Factor.value; // since 1.10.2
		}

		if(Global_Variables::check_Loaded_Version(1,7,1))		// since 1.7.1
		{
			if(Global_Variables::check_Loaded_Version(1,9,3))
			{stream >> curve.beam_Intensity_Initial >> curve.beam_Intensity_Final; } 		// since 1.9.3
			else
			{
				stream >> curve.beam_Intensity_Initial;
				curve.beam_Intensity_Final = curve.beam_Intensity_Initial;
			}
			curve.use_Final_Intensity = true;
		}

		if(Global_Variables::check_Loaded_Version(1,8,1))
		{stream >> curve.divide_On_Beam_Intensity; }	// since 1.8.1

		{
			QString argument_Type;
			QString angle_Type;
			stream >> argument_Type >> angle_Type ;
		}
		stream >> curve.angular_Units >> curve.spectral_Units >> curve.value_Type;
		{
			QString value_Mode;
			stream >> value_Mode;
		}
	}
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Fit_Params& fit_Params )
{
	return stream << fit_Params.calc << fit_Params.fit << fit_Params.norm
				  << fit_Params.adjust_Scale_Factor // since 1.10.2
				  << fit_Params.maximize_Integral // since 1.9.3
				  << fit_Params.weight << fit_Params.fit_Function
				  << fit_Params.use_Chi2 // since 1.8.1
				  << fit_Params.power; // since 1.8.8
}
QDataStream& operator >>( QDataStream& stream,		 Fit_Params& fit_Params )
{
	stream >> fit_Params.calc >> fit_Params.fit >> fit_Params.norm;

	if(Global_Variables::check_Loaded_Version(1,10,2))
	{stream >> fit_Params.adjust_Scale_Factor;}

	if(Global_Variables::check_Loaded_Version(1,9,3))
	{stream >> fit_Params.maximize_Integral; }			// since 1.9.3

	stream >> fit_Params.weight >> fit_Params.fit_Function;

	if(Global_Variables::check_Loaded_Version(1,8,1))
	{stream >> fit_Params.use_Chi2; }		// since 1.8.1

	if(Global_Variables::check_Loaded_Version(1,8,8) && !Global_Variables::check_Loaded_Version(1,9,3))
	{
		int int_Power;
		stream >> int_Power;
		fit_Params.power = double(int_Power);	// since 1.8.8
	}

	if(Global_Variables::check_Loaded_Version(1,9,3))
	{stream >> fit_Params.power; }			// since 1.9.3

	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Target_Curve* target_Curve )
{
	return stream	<< target_Curve->curve << target_Curve->fit_Params << target_Curve->measurement << target_Curve->filename
					<< target_Curve->filepath << target_Curve->plot_Options_Experimental
					<< target_Curve->plot_Options_Calculated
					<< target_Curve->calculated_Values
					<< target_Curve->lines_List << target_Curve->label_Text;
}
QDataStream& operator >>(QDataStream& stream,		 Target_Curve* target_Curve )
{
	stream	>> target_Curve->curve >> target_Curve->fit_Params >> target_Curve->measurement
			>> target_Curve->filename >> target_Curve->filepath;
	if(!Global_Variables::check_Loaded_Version(1,11,0))
	{
		stream >> target_Curve->loaded_And_Ready;
	}
	stream	>> target_Curve->plot_Options_Experimental;

	if(Global_Variables::check_Loaded_Version(1,7,4))
	{stream >> target_Curve->plot_Options_Calculated ; }

	stream  >> target_Curve->calculated_Values
			>> target_Curve->lines_List;
	if(Global_Variables::check_Loaded_Version(1,11,0))
	{
		QString arg_Units;
		stream >> arg_Units;
		QString at_Fixed;
		stream >> at_Fixed;
		QString arg_Type_For_Label;
		stream >> arg_Type_For_Label;
		QString ang_Type_For_Label_At_Fixed;
		stream >> ang_Type_For_Label_At_Fixed;
	}
	stream >> target_Curve->label_Text;

	if(Global_Variables::check_Loaded_Version(1,11,0))
	{
		// loaded_And_Ready is calculated here, after main load
		target_Curve->parse_Data_From_List();
		target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
		target_Curve->refresh_Description_Label();
	}
	return stream;
}

