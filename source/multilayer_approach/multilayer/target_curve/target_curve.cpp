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

	// argument shift & factor 1D only
	horizontal_Arg_Shift = 0;
	horizontal_Arg_Factor = 1;

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

	// type
	value_Type = no_Value_Type;
}

Fit_Params::Fit_Params()
{
	calculate = true;
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

	// units
	angular_Units = angle_Units_List[degree];
	spectral_Units = wavelength_Units_List[angstrom];
}

void Target_Curve::import_Data_From_File(QString bare_Filename)
{
	QFileInfo filename = bare_Filename;

	// reading whole file to lines_list
	QFile input_File(filename.absoluteFilePath());
	QString temp_Line = "not empty now";

	if(input_File.open(QIODevice::ReadOnly))
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
	if(	measurement.measurement_Type == measurement_Types[GISAS_Map] )
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
		{
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
						if(temp_Argument <= curve.argument.back()) goto skip_line_label; // read only monotonical arguments
					}
					if(curve.argument[curve.argument.size()-1]<curve.argument[curve.argument.size()-2]) // decreasing argument is allowed
					{
						if(temp_Argument >= curve.argument.back()) goto skip_line_label; // read only monotonical arguments
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
		} else
		{
			header.append(temp_Line);
		}
	}
}

void Target_Curve::parse_2D_Data()
{
	curve.value_2D.clear();
	for(int line_Index=0; line_Index<lines_List.size(); ++line_Index)
	{
		QString temp_Line = lines_List[line_Index];
		temp_Line = temp_Line.simplified();

		// check if not header line (starts from digit)
		bool is_Decimal = false;
		QString(temp_Line[0]).toInt(&is_Decimal);

		if(is_Decimal)
		{
			// split line into "numbers"
			QStringList potentional_Numbers = temp_Line.split(delimiters,QString::SkipEmptyParts);

			QVector<double> numbers_Row(potentional_Numbers.size());
			for(int num_Index=0; num_Index<potentional_Numbers.size(); num_Index++)
			{
				bool ok_To_Double = false;
				double temp_Value = QString(potentional_Numbers[num_Index]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas are considered as dots

				if(ok_To_Double)
				{
					numbers_Row[num_Index] = temp_Value;
				} else
				{
					numbers_Row[num_Index] = -2020;
					qInfo() << "Target_Curve::parse_2D_Data  :  bad number in row" << line_Index << "column" << num_Index << endl;
				}

			}
			curve.value_2D.append(numbers_Row);
		} else
		{
			header.append(temp_Line);
		}
	}

	// decision
	if(curve.value_2D.size()>3)
	{
		if(curve.value_2D.front().size()>3)
		{
			loaded_And_Ready = true;
		} else
		{
			loaded_And_Ready = false;
			qInfo() << "Target_Curve::parse_2D_Data  :  file should have at least 3 columns" << endl;
		}
	} else
	{
		loaded_And_Ready = false;
		qInfo() << "Target_Curve::parse_2D_Data  :  file should have at least 3 rows" << endl;
	}
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
	if(	measurement.measurement_Type == measurement_Types[GISAS_Map] )
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
		double delta = (curve.beam_Intensity_Final - curve.beam_Intensity_Initial)/max(int(curve.argument.size())-1,1);
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
	}
}

void Target_Curve::fill_Measurement_And_Curve_With_Shifted_2D_Data()
{
	if(loaded_And_Ready)
	{
		curve.value_2D_Shifted.resize(curve.value_2D.size());
		curve.value_2D_No_Scaling_And_Offset.resize(curve.value_2D.size());
		for(int i=0; i<curve.value_2D.size(); i++)
		{
			curve.value_2D_Shifted[i].resize(curve.value_2D[i].size());
			curve.value_2D_No_Scaling_And_Offset[i].resize(curve.value_2D[i].size());
		}

		double intensity_Factor = 1;
		if(curve.divide_On_Beam_Intensity) intensity_Factor = curve.beam_Intensity_Initial;
		for(int row=0; row<curve.value_2D.size(); row++)
		{
			for(int col=0; col<curve.value_2D.front().size(); col++)
			{
				curve.value_2D_Shifted			    [row][col] = curve.value_2D[row][col]/intensity_Factor * curve.val_Factor.value + curve.val_Shift;
				curve.value_2D_No_Scaling_And_Offset[row][col] = curve.value_2D[row][col]/intensity_Factor;
			}
		}
	}
}

void Target_Curve::rotate_Data_From_Previous_State(QString left_Right)
{
	int new_Row_Count = curve.value_2D.front().size();
	int new_Col_Count = curve.value_2D.size();
	int new_Last_Row = new_Row_Count-1;
	int new_Last_Col = new_Col_Count-1;

	// resize
	QVector<QVector<double>>temp_Value_2D					   (new_Row_Count);
	vector<vector<double>>  temp_Value_2D_Shifted			   (new_Row_Count);
	vector<vector<double>>  temp_Value_2D_No_Scaling_And_Offset(new_Row_Count);
	for(int row=0; row<temp_Value_2D.size(); row++)
	{
		temp_Value_2D					   [row].resize(new_Col_Count);
		temp_Value_2D_Shifted			   [row].resize(new_Col_Count);
		temp_Value_2D_No_Scaling_And_Offset[row].resize(new_Col_Count);
	}

	// fill
	if(left_Right == right)
	{
		for(int row=0; row<new_Row_Count; row++)
		{
			for(int col=0; col<new_Col_Count; col++)
			{
				temp_Value_2D						[row][col]	= curve.value_2D					  [col][new_Last_Row-row];
				temp_Value_2D_Shifted				[row][col]	= curve.value_2D_Shifted			  [col][new_Last_Row-row];
				temp_Value_2D_No_Scaling_And_Offset	[row][col]	= curve.value_2D_No_Scaling_And_Offset[col][new_Last_Row-row];
			}
		}
	}
	if(left_Right == left)
	{
		for(int row=0; row<new_Row_Count; row++)
		{
			for(int col=0; col<new_Col_Count; col++)
			{
				temp_Value_2D						[row][col]	= curve.value_2D					  [new_Last_Col-col][row];
				temp_Value_2D_Shifted				[row][col]	= curve.value_2D_Shifted			  [new_Last_Col-col][row];
				temp_Value_2D_No_Scaling_And_Offset	[row][col]	= curve.value_2D_No_Scaling_And_Offset[new_Last_Col-col][row];
			}
		}
	}

	// change
	curve.value_2D = temp_Value_2D;
	curve.value_2D_Shifted = temp_Value_2D_Shifted;
	curve.value_2D_No_Scaling_And_Offset = temp_Value_2D_No_Scaling_And_Offset;
}

void Target_Curve::refresh_Description_Label()
{
	if(loaded_And_Ready)
	{
		QString lambda_Energy;
		if(	spectral_Units == wavelength_Units_List[angstrom] ||
			spectral_Units == wavelength_Units_List[nm]		  )
		{
			lambda_Energy = Lambda_Sym;
		} else
		{
			lambda_Energy = "E";
		}

		double spectral_Coeff = wavelength_Coefficients_Map.value(spectral_Units);
		double angular_Coeff = angle_Coefficients_Map.value(angular_Units);

		if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{		
				label_Text =  curve.value_Type + "; " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(curve.shifted_Argument.front(), thumbnail_double_format, 3) +
						"-" + Locale.toString(curve.shifted_Argument.back(), thumbnail_double_format, 3) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units,measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units;
			}
			if(measurement.argument_Type == argument_Types[Wavelength_Energy])
			{
				label_Text =  curve.value_Type + "; " + lambda_Energy + "=" +
							  Locale.toString(curve.shifted_Argument.front(), thumbnail_double_format, 3) +
						"-" + Locale.toString(curve.shifted_Argument.back(), thumbnail_double_format, 3) +
							  " " + spectral_Units + "; " + "at " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Detector_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Detector_Polar_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " + Theta_Sym + "=" +
							  Locale.toString(curve.shifted_Argument.front(), thumbnail_double_format, 3) +
						"-" + Locale.toString(curve.shifted_Argument.back(), thumbnail_double_format, 3) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + " and " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Rocking_Curve] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(curve.shifted_Argument.front(), thumbnail_double_format, 3) +
						"-" + Locale.toString(curve.shifted_Argument.back(), thumbnail_double_format, 3) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + ", specular=" +
							  Locale.toString(measurement.beam_Theta_0_Specular_Position/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
			if(measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " + Delta_Big_Sym + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(curve.shifted_Argument.front(), thumbnail_double_format, 3) +
						"-" + Locale.toString(curve.shifted_Argument.back(), thumbnail_double_format, 3) +
							  "" + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + ", specular=" +
							  Locale.toString(measurement.beam_Theta_0_Specular_Position/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}
		if(	measurement.measurement_Type == measurement_Types[Offset_Scan] )
		{
			if(measurement.argument_Type == argument_Types[Beam_Grazing_Angle])
			{
				label_Text =  measurement.measurement_Type + "; " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(curve.shifted_Argument.front(), thumbnail_double_format, 3) +
						"-" + Locale.toString(curve.shifted_Argument.back(), thumbnail_double_format, 3) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + ", offset =" +
							  Locale.toString(measurement.detector_Theta_Offset/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}
		if(	measurement.measurement_Type == measurement_Types[GISAS_Map] )
		{
			if(measurement.argument_Type == argument_Types[Detector_Theta_Phi_Angles])
			{
				label_Text =  measurement.measurement_Type + "; " + Theta_Sym + "=" +
							  Locale.toString(measurement.detector_Theta_Angle.independent.min/angular_Coeff) +
						"-" + Locale.toString(measurement.detector_Theta_Angle.independent.max/angular_Coeff) +
							  " " + angular_Units + "; " +			Phi_Sym + "=" +
							  Locale.toString(measurement.detector_Phi_Angle.independent.min/angular_Coeff) +
						"-" + Locale.toString(measurement.detector_Phi_Angle.independent.max/angular_Coeff) +
							  " " + angular_Units + "; " + "at " + lambda_Energy + "=" +
							  Locale.toString(Global_Variables::wavelength_Energy(spectral_Units, measurement.wavelength.value)/spectral_Coeff, thumbnail_double_format, thumbnail_wavelength_precision) +
							  " " + spectral_Units + " and " + Theta_Sym + Zero_Subscript_Sym + "=" +
							  Locale.toString(measurement.beam_Theta_0_Angle.value/angular_Coeff, thumbnail_double_format, thumbnail_angle_precision) +
							  " " + angular_Units;
			}
		}

		description_Label->setText(index + ": " + label_Text);
	} else
	{
		label_Text = "<no description>";
		description_Label->setText(label_Text);
	}
}

void Target_Curve::calc_Measured_cos2_k()
{
	double coeff_Spectral = wavelength_Coefficients_Map.value(spectral_Units);
	double coeff_Angular = angle_Coefficients_Map.value(angular_Units);

	if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// beam angle_cos_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(curve.shifted_Argument.size());
				measurement.beam_Theta_0_Cos_Vec.  resize(curve.shifted_Argument.size());
				measurement.beam_Theta_0_Cos2_Vec. resize(curve.shifted_Argument.size());

				for(int i=0; i<curve.shifted_Argument.size(); ++i)
				{
					measurement.beam_Theta_0_Angle_Vec[i] = curve.shifted_Argument[i]*coeff_Angular;
					measurement.beam_Theta_0_Cos_Vec [i] = cos(measurement.beam_Theta_0_Angle_Vec[i]*M_PI/180.);
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
				}
			}
			// k : single_value
			{
				measurement.lambda_Value = measurement.wavelength.value;
				measurement.k_Value = 2*M_PI/measurement.wavelength.value;
			}
		}
		if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
		{
			// beam angle_cos_cos2 : single value
			{
				measurement.beam_Theta_0_Angle_Value = measurement.beam_Theta_0_Angle.value;
				measurement.beam_Theta_0_Cos_Value = cos(measurement.beam_Theta_0_Angle_Value*M_PI/180.);
				measurement.beam_Theta_0_Cos2_Value = pow(measurement.beam_Theta_0_Cos_Value,2);
			}
			// k : vector
			{
				measurement.lambda_Vec.resize(curve.shifted_Argument.size());
				measurement.k_Vec.	   resize(curve.shifted_Argument.size());

				for(int i=0; i<curve.shifted_Argument.size(); ++i)
				{
					measurement.lambda_Vec[i] = Global_Variables::wavelength_Energy(spectral_Units, curve.shifted_Argument[i]*coeff_Spectral);
					measurement.k_Vec	  [i] = 2*M_PI/measurement.lambda_Vec[i];
				}
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Detector_Polar_Angle] )
		{
			// detector angle_cos : vector
			{
				measurement.detector_Theta_Angle_Vec.resize(curve.shifted_Argument.size());
				measurement.detector_Theta_Cos_Vec.  resize(curve.shifted_Argument.size());

				for(int i=0; i<curve.shifted_Argument.size(); ++i)
				{
					measurement.detector_Theta_Angle_Vec[i] = curve.shifted_Argument[i]*coeff_Angular;;
					measurement.detector_Theta_Cos_Vec[i] = cos(measurement.detector_Theta_Angle_Vec[i]*M_PI/180.);
				}
			}
			// beam angle_cos_cos2 : single value
			{
				measurement.beam_Theta_0_Angle_Value = measurement.beam_Theta_0_Angle.value;
				measurement.beam_Theta_0_Cos_Value = cos(measurement.beam_Theta_0_Angle_Value*M_PI/180.);
				measurement.beam_Theta_0_Cos2_Value = pow(measurement.beam_Theta_0_Cos_Value,2);
			}
			// k : single value
			{
				measurement.lambda_Value = measurement.wavelength.value;
				measurement.k_Value = 2*M_PI/measurement.wavelength.value;
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		/// different part
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// beam angle_cos_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(curve.shifted_Argument.size());
				measurement.beam_Theta_0_Cos_Vec.  resize(curve.shifted_Argument.size());
				measurement.beam_Theta_0_Cos2_Vec. resize(curve.shifted_Argument.size());

				for(int i=0; i<curve.shifted_Argument.size(); ++i)
				{
					measurement.beam_Theta_0_Angle_Vec[i] = curve.shifted_Argument[i]*coeff_Angular;
					measurement.beam_Theta_0_Cos_Vec [i] = cos(measurement.beam_Theta_0_Angle_Vec[i]*M_PI/180.);
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
				}
			}
		}
		if(measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle] )
		{
			// beam angle_cos_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(curve.shifted_Argument.size());
				measurement.beam_Theta_0_Cos_Vec.  resize(curve.shifted_Argument.size());
				measurement.beam_Theta_0_Cos2_Vec. resize(curve.shifted_Argument.size());

				for(int i=0; i<curve.shifted_Argument.size(); ++i)
				{
					measurement.beam_Theta_0_Angle_Vec[i] = curve.shifted_Argument[i]*coeff_Angular + measurement.beam_Theta_0_Specular_Position;
					measurement.beam_Theta_0_Cos_Vec [i] = cos(measurement.beam_Theta_0_Angle_Vec[i]*M_PI/180.);
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
				}
			}
		}
		/// common part
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] ||
			measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle] )
		{
			// detector angle_cos : vector, depends on beam
			{
				measurement.detector_Theta_Angle_Vec.resize(curve.shifted_Argument.size());
				measurement.detector_Theta_Cos_Vec.  resize(curve.shifted_Argument.size());

				for(int i=0; i<curve.shifted_Argument.size(); ++i)
				{
					double angle_Temp = 2*measurement.beam_Theta_0_Specular_Position - measurement.beam_Theta_0_Angle_Vec[i];
					measurement.detector_Theta_Angle_Vec[i] = angle_Temp;
					measurement.detector_Theta_Cos_Vec[i] = cos(angle_Temp*M_PI/180.);
				}
			}
			// k : single value
			{
				measurement.lambda_Value = measurement.wavelength.value;
				measurement.k_Value = 2*M_PI/measurement.wavelength.value;
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// beam angle_cos_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(curve.shifted_Argument.size());
				measurement.beam_Theta_0_Cos_Vec.  resize(curve.shifted_Argument.size());
				measurement.beam_Theta_0_Cos2_Vec. resize(curve.shifted_Argument.size());

				for(int i=0; i<curve.shifted_Argument.size(); ++i)
				{
					measurement.beam_Theta_0_Angle_Vec[i] = curve.shifted_Argument[i]*coeff_Angular;;
					measurement.beam_Theta_0_Cos_Vec [i] = cos(measurement.beam_Theta_0_Angle_Vec[i]*M_PI/180.);
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
				}
			}
			// detector angle_cos : vector, depends on beam
			{
				measurement.detector_Theta_Angle_Vec.resize(curve.shifted_Argument.size());
				measurement.detector_Theta_Cos_Vec.  resize(curve.shifted_Argument.size());

				for(int i=0; i<curve.shifted_Argument.size(); ++i)
				{
					measurement.detector_Theta_Angle_Vec[i] = measurement.beam_Theta_0_Angle_Vec[i] + measurement.detector_Theta_Offset;;
					measurement.detector_Theta_Cos_Vec[i] = cos(measurement.detector_Theta_Angle_Vec[i]*M_PI/180.);
				}
			}
			// k : single value
			{
				measurement.lambda_Value = measurement.wavelength.value;
				measurement.k_Value = 2*M_PI/measurement.wavelength.value;
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		if( measurement.argument_Type == argument_Types[Detector_Theta_Phi_Angles] )
		{
			// detector THETA angle_cos : vector
			{
				int num_Points_Theta = curve.value_2D_Shifted.front().size();
				measurement.detector_Theta_Angle_Vec.resize(num_Points_Theta);
				measurement.detector_Theta_Cos_Vec.  resize(num_Points_Theta);

				double angle_Step = (measurement.detector_Theta_Angle.independent.max - measurement.detector_Theta_Angle.independent.min) / (num_Points_Theta - 1);
				double angle_Temp =  measurement.detector_Theta_Angle.independent.min;
				for(int i=0; i<num_Points_Theta; ++i)
				{
					measurement.detector_Theta_Cos_Vec[i] = cos(angle_Temp*M_PI/180.);
					measurement.detector_Theta_Angle_Vec[i] = angle_Temp;
					angle_Temp += angle_Step;
				}
			}
			// detector PHI angle_cos_sin : vector
			{
				int num_Points_Phi = curve.value_2D_Shifted.size();
				measurement.detector_Phi_Angle_Vec.resize(num_Points_Phi);
				measurement.detector_Phi_Cos_Vec.  resize(num_Points_Phi);
				measurement.detector_Phi_Sin_Vec.  resize(num_Points_Phi);

				double angle_Step = (measurement.detector_Phi_Angle.independent.max - measurement.detector_Phi_Angle.independent.min) / (num_Points_Phi - 1);
				double angle_Temp =  measurement.detector_Phi_Angle.independent.min;
				for(int i=0; i<num_Points_Phi; ++i)
				{
					measurement.detector_Phi_Cos_Vec[i] = cos(angle_Temp*M_PI/180.);
					measurement.detector_Phi_Sin_Vec[i] = sin(angle_Temp*M_PI/180.);
					measurement.detector_Phi_Angle_Vec[i] = angle_Temp;
					angle_Temp += angle_Step;
				}
			}
			// beam angle_cos_cos2 : single value
			{
				measurement.beam_Theta_0_Angle_Value = measurement.beam_Theta_0_Angle.value;
				measurement.beam_Theta_0_Cos_Value  = cos(measurement.beam_Theta_0_Angle_Value*M_PI/180.);
				measurement.beam_Theta_0_Cos2_Value = pow(measurement.beam_Theta_0_Cos_Value,2);
			}
			// k : single value
			{
				measurement.lambda_Value = measurement.wavelength.value;
				measurement.k_Value = 2*M_PI/measurement.wavelength.value;
			}
		}
	}
}

Target_Curve& Target_Curve::operator =(const Target_Curve& referent_Target_Curve)
{
	description_Label->setText(referent_Target_Curve.description_Label->text());

	curve = referent_Target_Curve.curve;
	fit_Params = referent_Target_Curve.fit_Params;
	measurement = referent_Target_Curve.measurement;	measurement.reset_All_IDs();
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
					<< curve.horizontal_Arg_Shift << curve.horizontal_Arg_Factor
					<< curve.val_Shift << curve.val_Factor
					<< curve.divide_On_Beam_Intensity << curve.beam_Intensity_Initial << curve.use_Final_Intensity << curve.beam_Intensity_Final
					<< curve.value_Type << curve.argument << curve.values << curve.value_2D;
}
QDataStream& operator >>( QDataStream& stream,		 Curve& curve )
{
	if(Global_Variables::check_Loaded_Version(1,11,0))		// since 1.11.0
	{
		stream	>> curve.mesh_Density_Factor >> curve.mesh_Density_Shift
				>> curve.use_Subinterval >> curve.subinterval_Left >> curve.subinterval_Right >> curve.subinterval_Top >> curve.subinterval_Bottom
				>> curve.horizontal_Arg_Shift >> curve.horizontal_Arg_Factor
				>> curve.val_Shift >> curve.val_Factor
				>> curve.divide_On_Beam_Intensity >> curve.beam_Intensity_Initial >> curve.use_Final_Intensity >> curve.beam_Intensity_Final
				>> curve.value_Type >> curve.argument >> curve.values >> curve.value_2D;
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
		stream  >> curve.argument;

		// ----------------------------------------------------
		QVector<double> shifted_Argument;
		stream >> shifted_Argument;
		// ----------------------------------------------------

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
			QString angular_Units;
			QString spectral_Units;
			stream >> argument_Type >> angle_Type >> angular_Units >> spectral_Units;
		}
		stream >> curve.value_Type;
		{
			QString value_Mode;
			stream >> value_Mode;
		}
	}
	return stream;
}

QDataStream& operator <<( QDataStream& stream, const Fit_Params& fit_Params )
{
	return stream << fit_Params.calculate << fit_Params.fit << fit_Params.norm
				  << fit_Params.adjust_Scale_Factor // since 1.10.2
				  << fit_Params.maximize_Integral // since 1.9.3
				  << fit_Params.weight << fit_Params.fit_Function
				  << fit_Params.use_Chi2 // since 1.8.1
				  << fit_Params.power; // since 1.8.8
}
QDataStream& operator >>( QDataStream& stream,		 Fit_Params& fit_Params )
{
	stream >> fit_Params.calculate >> fit_Params.fit >> fit_Params.norm;

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
	return stream	<< target_Curve->curve << target_Curve->fit_Params << target_Curve->measurement
					<< target_Curve->filename << target_Curve->filepath
					<< target_Curve->plot_Options_Experimental
					<< target_Curve->plot_Options_Calculated
					<< target_Curve->header << target_Curve->label_Text
					<< target_Curve->angular_Units << target_Curve->spectral_Units << target_Curve->loaded_And_Ready;
}
QDataStream& operator >>(QDataStream& stream,		 Target_Curve* target_Curve )
{
	if(Global_Variables::check_Loaded_Version(1,11,0))
	{
		stream	>> target_Curve->curve >> target_Curve->fit_Params >> target_Curve->measurement
				>> target_Curve->filename >> target_Curve->filepath
				>> target_Curve->plot_Options_Experimental
				>> target_Curve->plot_Options_Calculated
				>> target_Curve->header >> target_Curve->label_Text
				>> target_Curve->angular_Units >> target_Curve->spectral_Units >> target_Curve->loaded_And_Ready;

	} else
	{
		stream	>> target_Curve->curve >> target_Curve->fit_Params >> target_Curve->measurement
				>> target_Curve->filename >> target_Curve->filepath;

		stream >> target_Curve->loaded_And_Ready;

		stream	>> target_Curve->plot_Options_Experimental;

		if(Global_Variables::check_Loaded_Version(1,7,4))
		{stream >> target_Curve->plot_Options_Calculated ; }

		Old_Calculated_Values old_Calculated_Values;
		stream  >> old_Calculated_Values;

		stream	>> target_Curve->lines_List;

		// --------------------------------------------------------------------------
		QString arg_Units;					stream >> arg_Units;
		QString at_Fixed;					stream >> at_Fixed;
		QString arg_Type_For_Label;			stream >> arg_Type_For_Label;
		QString ang_Type_For_Label_At_Fixed;stream >> ang_Type_For_Label_At_Fixed;
		// --------------------------------------------------------------------------

		stream >> target_Curve->label_Text;
	}

	target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
	target_Curve->refresh_Description_Label();
	return stream;
}

