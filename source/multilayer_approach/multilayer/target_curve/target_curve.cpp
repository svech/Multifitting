#include "target_curve.h"

Curve::Curve()
{
	// mesh density : 1D only
	mesh_Density_Factor = 1;
	mesh_Density_Shift = 0.5;

	// subinterval
	use_Subinterval = false;
	outer_Area = false;
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
	beam_Time = 1;
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
	QObject(parent)
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
		QMessageBox::information(nullptr, "Target_Curve::import_Data", "Can't read file filename \"" + filename.fileName() + "\"");
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
	header.clear();

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
				QMessageBox::information(nullptr, "Target_Curve::import_Data", exception);
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
	std::cout << endl << "Loading 2D data. Wait..." << endl;

	curve.value_2D.clear();
	header.clear();

	QString data_Type = rectangular_Table_Data_Type; // by default

	/// check for data type by its first numeric line
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

			if(potentional_Numbers.size()==3)
			{
				bool ok_To_Int_0 = false;
				QString(potentional_Numbers[0]).toInt(&ok_To_Int_0);

				bool ok_To_Int_1 = false;
				QString(potentional_Numbers[1]).toInt(&ok_To_Int_1);

				if(ok_To_Int_0 && ok_To_Int_1) {data_Type = point_By_Point_Data_Type;}
			}
			break;
		}
	}

	/// reading data

	// for point-by-point table
	int last_Row = -2021;

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

			/// case of point-by-point table
			if(data_Type == point_By_Point_Data_Type)
			{
				bool ok_To_Int_0 = false;
				int row_Index = QString(potentional_Numbers[0]).replace(",", ".").toInt(&ok_To_Int_0);

				//bool ok_To_Int_1 = false;
				//int col_Index = QString(potentional_Numbers[1]).replace(",", ".").toInt(&ok_To_Int_1);

				bool ok_To_Double = false;
				double temp_Value = QString(potentional_Numbers[2]).replace(",", ".").toDouble(&ok_To_Double); // dots and commas are considered as dots

				// add new row
				if(row_Index > last_Row)
				{
					curve.value_2D.append(QVector<double>());
					last_Row = row_Index;
				}

				if(ok_To_Double)
				{
					curve.value_2D.last().append(temp_Value);
				} else
				{
					curve.value_2D.last().append(-2021);
					qInfo() << "Target_Curve::parse_2D_Data  :  bad number in row" << line_Index << endl;
				}


			} else
			/// case of rectangular table
			{
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
			}
		} else
		{
			header.append(temp_Line);
		}
	}

	// transpose
	if(data_Type == point_By_Point_Data_Type && curve.value_2D.size()>=3)
	{
		QVector<QVector<double>> value_2D_Transposed;
		value_2D_Transposed.resize(curve.value_2D.front().size());
		for(int i=0; i<value_2D_Transposed.size(); i++)
		{
			value_2D_Transposed[i].resize(curve.value_2D.size());
			for(int j=0; j<value_2D_Transposed[0].size(); j++)
			{
				value_2D_Transposed[i][j] = curve.value_2D[j][i];
			}
		}
		curve.value_2D = value_2D_Transposed;
	}

	std::cout << "Loaded" << endl << endl;

	// decision
	if(curve.value_2D.size()>=3)
	{
		if(curve.value_2D.front().size()>=3)
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
		int curve_Size = curve.argument.size();
		int binning_Factor = min(measurement.detector_1D.binning_Factor, curve_Size/2);
		if(measurement.detector_1D.use_Binning)
		{
			curve_Size = 1 + ((curve.argument.size() - 1) / measurement.detector_1D.binning_Factor);
		}


		/// resizing
		curve.shifted_Argument.resize(curve_Size);
		curve.shifted_Values.resize(curve_Size);
		curve.shifted_Values_No_Scaling_And_Offset.resize(curve_Size);

		/// intensity factor
		vector<double> intensity_Factor(curve_Size, 1);
		double delta = (curve.beam_Intensity_Final - curve.beam_Intensity_Initial)/max(curve_Size-1,1);
		if(curve.divide_On_Beam_Intensity)
		{
			for(int i=0; i<curve_Size; ++i)
			{
				intensity_Factor[i] = curve.beam_Time*(curve.beam_Intensity_Initial + i*delta);
			}
		}

		/// from curve to shifted curve
		if(measurement.detector_1D.use_Binning)
		{
			int last_Curve_Index = curve.argument.size()-1;
			for(int i=0; i<curve_Size; ++i)
			{
				curve.shifted_Argument[i] = 0;
				curve.shifted_Values  [i] = 0;
				curve.shifted_Values_No_Scaling_And_Offset[i] = 0;

				// points summation
				for(int k=0; k<measurement.detector_1D.binning_Factor; k++)
				{
					if(i < curve_Size-1)
					{
						curve.shifted_Argument[i] += curve.argument[i*binning_Factor + k];
						curve.shifted_Values  [i] += curve.values  [i*binning_Factor + k];
					} else
					// last point
					{
						curve.shifted_Argument[i] += curve.argument[last_Curve_Index - k];
						curve.shifted_Values  [i] += curve.values  [last_Curve_Index - k];
					}
				}
				curve.shifted_Argument[i] = curve.shifted_Argument[i]/binning_Factor * curve.horizontal_Arg_Factor + curve.horizontal_Arg_Shift;
				curve.shifted_Values_No_Scaling_And_Offset[i] = curve.shifted_Values[i]/binning_Factor/intensity_Factor[i];
				curve.shifted_Values  [i] = curve.shifted_Values  [i]/binning_Factor/intensity_Factor[i] * curve.val_Factor.value      + curve.val_Shift;
			}
		} else
		// no binning
		{
			for(int i=0; i<curve_Size; ++i)
			{
				curve.shifted_Argument[i] = curve.argument[i]                     * curve.horizontal_Arg_Factor + curve.horizontal_Arg_Shift;
				curve.shifted_Values  [i] = curve.values  [i]/intensity_Factor[i] * curve.val_Factor.value      + curve.val_Shift;
				curve.shifted_Values_No_Scaling_And_Offset[i] = curve.values[i]/intensity_Factor[i];
			}
		}
	}
}

void Target_Curve::fill_Measurement_And_Curve_With_Shifted_2D_Data()
{
	if(loaded_And_Ready)
	{
		int row_Size = curve.value_2D.size();
		int col_Size = curve.value_2D.front().size();

		int last_Row_Index = curve.value_2D.size()-1;
		int last_Col_Index = curve.value_2D.front().size()-1;

		int row_Binning_Factor = min(measurement.detector_2D.phi_Binning_Factor,   row_Size/2);
		int col_Binning_Factor = min(measurement.detector_2D.theta_Binning_Factor, col_Size/2);

		if(measurement.detector_2D.use_Binning)
		{
			row_Size = 1 + ((curve.value_2D.size()         - 1) / row_Binning_Factor);
			col_Size = 1 + ((curve.value_2D.front().size() - 1) / col_Binning_Factor);
		}

		/// resizing
		curve.value_2D_Shifted.resize(row_Size);
		curve.value_2D_No_Scaling_And_Offset.resize(row_Size);
		for(int i=0; i<row_Size; i++)
		{
			curve.value_2D_Shifted[i].resize(col_Size);
			curve.value_2D_No_Scaling_And_Offset[i].resize(col_Size);
		}

		/// intensity factor
		double intensity_Factor = 1;
		if(curve.divide_On_Beam_Intensity) {intensity_Factor = curve.beam_Time*curve.beam_Intensity_Initial;}

		/// from curve to shifted curve
		if(measurement.detector_2D.use_Binning)
		{
			for(int row=0; row<row_Size; row++)
			{
				for(int col=0; col<col_Size; col++)
				{
					curve.value_2D_Shifted			    [row][col] = 0;
					curve.value_2D_No_Scaling_And_Offset[row][col] = 0;

					for(int r=0; r<row_Binning_Factor; r++)
					{
						for(int c=0; c<col_Binning_Factor; c++)
						{
							if(row < row_Size-1)
							{
								if(col < col_Size-1)
								{
									curve.value_2D_Shifted			    [row][col] += curve.value_2D[row*row_Binning_Factor+r][col*col_Binning_Factor+c];
								} else
								{
									curve.value_2D_Shifted			    [row][col] += curve.value_2D[row*row_Binning_Factor+r][last_Col_Index-c];
								}
							} else
							{
								if(col < col_Size-1)
								{
									curve.value_2D_Shifted			    [row][col] += curve.value_2D[last_Row_Index-r][col*col_Binning_Factor+c];
								} else
								{
									curve.value_2D_Shifted			    [row][col] += curve.value_2D[last_Row_Index-r][last_Col_Index-c];
								}
							}
						}
					}
					curve.value_2D_No_Scaling_And_Offset[row][col] = curve.value_2D_Shifted[row][col]/col_Binning_Factor/intensity_Factor;
					curve.value_2D_Shifted			    [row][col] = curve.value_2D_Shifted[row][col]/row_Binning_Factor/intensity_Factor * curve.val_Factor.value + curve.val_Shift;
				}
			}
		} else
		{
			for(int row=0; row<row_Size; row++)
			{
				for(int col=0; col<col_Size; col++)
				{
					curve.value_2D_No_Scaling_And_Offset[row][col] = curve.value_2D[row][col]/intensity_Factor;
					curve.value_2D_Shifted			    [row][col] = curve.value_2D[row][col]/intensity_Factor * curve.val_Factor.value + curve.val_Shift;
				}
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
	for(int row=0; row<temp_Value_2D.size(); row++)
	{
		temp_Value_2D					   [row].resize(new_Col_Count);
	}

	// fill
	if(left_Right == right)
	{
		for(int row=0; row<new_Row_Count; row++)
		{
			for(int col=0; col<new_Col_Count; col++)
			{
				temp_Value_2D						[row][col]	= curve.value_2D					  [col][new_Last_Row-row];
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
			}
		}
	}

	// change
	curve.value_2D = temp_Value_2D;
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

		if(name.size()>0)
		{	description_Label->setText(index + ": " + name + "; " + label_Text);}
		else
		{	description_Label->setText(index + ": " + label_Text);}
	} else
	{
		label_Text = "<no description>";
		description_Label->setText(label_Text);
	}
}

void Target_Curve::increase_Mesh_Density(vector<double>& dense_Vector)
{
	dense_Vector.resize((curve.shifted_Argument.size()-1)*curve.mesh_Density_Factor+1);
	for(size_t i=0; i<curve.shifted_Argument.size()-1; i++)
	{
		double delta = (curve.shifted_Argument[i+1]-curve.shifted_Argument[i])/curve.mesh_Density_Factor;
		dense_Vector[i*curve.mesh_Density_Factor] = curve.shifted_Argument[i];
		for(int dense_Index=1; dense_Index<curve.mesh_Density_Factor; dense_Index++)
		{
			dense_Vector[i*curve.mesh_Density_Factor+dense_Index] = curve.shifted_Argument[i] + delta*(dense_Index+curve.mesh_Density_Shift);
		}
	}
	dense_Vector.back() = curve.shifted_Argument.back();
}

void Target_Curve::calc_Measured_cos2_k(double angle_Shift, double lambda_Shift, bool decrease_Mesh)
{
	if(!loaded_And_Ready) return;

	double coeff_Spectral = wavelength_Coefficients_Map.value(spectral_Units);
	double coeff_Angular = angle_Coefficients_Map.value(angular_Units);

	// dense mesh
	vector<double> shifted_Dense_Argument = curve.shifted_Argument;
	if(curve.mesh_Density_Factor>1 && !decrease_Mesh) {increase_Mesh_Density(shifted_Dense_Argument);}

	if(	measurement.measurement_Type == measurement_Types[Specular_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// beam angle_cos_sin_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Cos_Vec.  resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Sin_Vec.  resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Cos2_Vec. resize(shifted_Dense_Argument.size());

				for(size_t i=0; i<shifted_Dense_Argument.size(); ++i)
				{
					measurement.beam_Theta_0_Angle_Vec[i] = shifted_Dense_Argument[i]*coeff_Angular + angle_Shift;
					measurement.beam_Theta_0_Cos_Vec [i] = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Vec[i])));
					measurement.beam_Theta_0_Sin_Vec [i] = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Vec[i])));
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
				}
			}
			// k : single_value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
		if( measurement.argument_Type == argument_Types[Wavelength_Energy] )
		{
			// beam angle_cos_sin_cos2 : single value
			{
				measurement.beam_Theta_0_Angle_Value = measurement.beam_Theta_0_Angle.value + angle_Shift;
				measurement.beam_Theta_0_Cos_Value = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Sin_Value = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Cos2_Value = pow(measurement.beam_Theta_0_Cos_Value,2);
			}
			// k : vector
			{
				measurement.lambda_Vec.resize(shifted_Dense_Argument.size());
				measurement.k_Vec.	   resize(shifted_Dense_Argument.size());

				for(size_t i=0; i<shifted_Dense_Argument.size(); ++i)
				{
					measurement.lambda_Vec[i] = max(Global_Variables::wavelength_Energy(spectral_Units, shifted_Dense_Argument[i]*coeff_Spectral) + lambda_Shift, DBL_EPSILON);
					measurement.k_Vec	  [i] = 2*M_PI/measurement.lambda_Vec[i];
				}
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[Detector_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Detector_Polar_Angle] )
		{
			// detector angle_cos_sin_cos2 : vector
			{
				measurement.detector_Theta_Angle_Vec.resize(shifted_Dense_Argument.size());
				measurement.detector_Theta_Cos_Vec.  resize(shifted_Dense_Argument.size());
				measurement.detector_Theta_Sin_Vec.  resize(shifted_Dense_Argument.size());
				measurement.detector_Theta_Cos2_Vec. resize(shifted_Dense_Argument.size());

				for(size_t i=0; i<shifted_Dense_Argument.size(); ++i)
				{
					measurement.detector_Theta_Angle_Vec[i] = shifted_Dense_Argument[i]*coeff_Angular;;
					measurement.detector_Theta_Cos_Vec [i] = cos(qDegreesToRadians(measurement.detector_Theta_Angle_Vec[i]));
					measurement.detector_Theta_Sin_Vec [i] = sin(qDegreesToRadians(measurement.detector_Theta_Angle_Vec[i]));
					measurement.detector_Theta_Cos2_Vec[i] = pow(measurement.detector_Theta_Cos_Vec[i],2);
				}
			}
			// beam angle_cos_sin_cos2 : single value
			{
				measurement.beam_Theta_0_Angle_Value = measurement.beam_Theta_0_Angle.value + angle_Shift;
				measurement.beam_Theta_0_Cos_Value = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Sin_Value = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Cos2_Value = pow(measurement.beam_Theta_0_Cos_Value,2);

				// for convenience of calculation
				measurement.beam_Theta_0_Cos_Vec.resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Sin_Vec.resize(shifted_Dense_Argument.size());
				for(size_t i=0; i<shifted_Dense_Argument.size(); ++i)
				{
					measurement.beam_Theta_0_Cos_Vec [i] = measurement.beam_Theta_0_Cos_Value;
					measurement.beam_Theta_0_Sin_Vec [i] = measurement.beam_Theta_0_Sin_Value;
				}
			}
			// k : single value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[Rocking_Curve] )
	{
		/// different part
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// beam angle_cos_sin_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Cos_Vec.  resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Sin_Vec.  resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Cos2_Vec. resize(shifted_Dense_Argument.size());

				for(size_t i=0; i<shifted_Dense_Argument.size(); ++i)
				{
					measurement.beam_Theta_0_Angle_Vec[i] = shifted_Dense_Argument[i]*coeff_Angular + angle_Shift;
					measurement.beam_Theta_0_Cos_Vec [i] = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Vec[i])));
					measurement.beam_Theta_0_Sin_Vec [i] = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Vec[i])));
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
				}
			}
		}
		if(measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle] )
		{
			// beam angle_cos_sin_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Cos_Vec.  resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Sin_Vec.  resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Cos2_Vec. resize(shifted_Dense_Argument.size());

				for(size_t i=0; i<shifted_Dense_Argument.size(); ++i)
				{
					measurement.beam_Theta_0_Angle_Vec[i] = shifted_Dense_Argument[i]*coeff_Angular + measurement.beam_Theta_0_Specular_Position + angle_Shift;
					measurement.beam_Theta_0_Cos_Vec [i] = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Vec[i])));
					measurement.beam_Theta_0_Sin_Vec [i] = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Vec[i])));
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
				}
			}
		}
		/// common part
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] ||
			measurement.argument_Type == argument_Types[Deviation_From_Specular_Angle] )
		{
			// detector angle_cos_sin_cos2 : vector, depends on beam
			{
				measurement.detector_Theta_Angle_Vec.resize(curve.shifted_Argument.size());
				measurement.detector_Theta_Cos_Vec.  resize(curve.shifted_Argument.size());
				measurement.detector_Theta_Sin_Vec.  resize(curve.shifted_Argument.size());
				measurement.detector_Theta_Cos2_Vec. resize(curve.shifted_Argument.size());

				for(size_t i=0; i<curve.shifted_Argument.size(); ++i)
				{
					double angle_Temp = 2*measurement.beam_Theta_0_Specular_Position - measurement.beam_Theta_0_Angle_Vec[i] + angle_Shift;
					measurement.detector_Theta_Angle_Vec[i] = angle_Temp;
					measurement.detector_Theta_Cos_Vec [i] = cos(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Sin_Vec [i] = sin(qDegreesToRadians(angle_Temp));
					measurement.detector_Theta_Cos2_Vec[i] = pow(measurement.detector_Theta_Cos_Vec[i],2);
				}
			}
			// k : single value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[Offset_Scan] )
	{
		if( measurement.argument_Type == argument_Types[Beam_Grazing_Angle] )
		{
			// beam angle_cos_sin_cos2 : vector
			{
				measurement.beam_Theta_0_Angle_Vec.resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Cos_Vec.  resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Sin_Vec.  resize(shifted_Dense_Argument.size());
				measurement.beam_Theta_0_Cos2_Vec. resize(shifted_Dense_Argument.size());

				for(size_t i=0; i<shifted_Dense_Argument.size(); ++i)
				{
					measurement.beam_Theta_0_Angle_Vec[i] = shifted_Dense_Argument[i]*coeff_Angular + angle_Shift;
					measurement.beam_Theta_0_Cos_Vec [i] = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Vec[i])));
					measurement.beam_Theta_0_Sin_Vec [i] = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Vec[i])));
					measurement.beam_Theta_0_Cos2_Vec[i] = pow(measurement.beam_Theta_0_Cos_Vec[i],2);
				}
			}
			// detector angle_cos_sin_cos2 : vector, depends on beam
			{
				measurement.detector_Theta_Angle_Vec.resize(shifted_Dense_Argument.size());
				measurement.detector_Theta_Cos_Vec.  resize(shifted_Dense_Argument.size());
				measurement.detector_Theta_Sin_Vec.  resize(shifted_Dense_Argument.size());
				measurement.detector_Theta_Cos2_Vec. resize(shifted_Dense_Argument.size());

				for(size_t i=0; i<shifted_Dense_Argument.size(); ++i)
				{
					measurement.detector_Theta_Angle_Vec[i] = measurement.beam_Theta_0_Angle_Vec[i] + measurement.detector_Theta_Offset - angle_Shift;
					measurement.detector_Theta_Cos_Vec [i] = cos(qDegreesToRadians(measurement.detector_Theta_Angle_Vec[i]));
					measurement.detector_Theta_Sin_Vec [i] = sin(qDegreesToRadians(measurement.detector_Theta_Angle_Vec[i]));
					measurement.detector_Theta_Cos2_Vec[i] = pow(measurement.detector_Theta_Cos_Vec[i],2);
				}
			}
			// k : single value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
	}
	if(	measurement.measurement_Type == measurement_Types[GISAS_Map] )
	{
		if( measurement.argument_Type == argument_Types[Detector_Theta_Phi_Angles] )
		{
			// detector THETA angle_cos_sin_cos2 : vector
			{
				int num_Points_Theta = curve.value_2D_Shifted.front().size();
				measurement.detector_Theta_Angle_Vec.resize(num_Points_Theta);
				measurement.detector_Theta_Cos_Vec.  resize(num_Points_Theta);
				measurement.detector_Theta_Sin_Vec.  resize(num_Points_Theta);
				measurement.detector_Theta_Cos2_Vec. resize(num_Points_Theta);

				double angle_Step = (measurement.detector_Theta_Angle.independent.max - measurement.detector_Theta_Angle.independent.min) / (num_Points_Theta - 1);
				double theta_Binning_Shift = 0;
				// no need?
//				if(measurement.detector_2D.use_Binning)	{theta_Binning_Shift = 0.5*(measurement.detector_2D.theta_Binning_Factor-1)*angle_Step;}
				double angle_Temp =  measurement.detector_Theta_Angle.independent.min;
				for(int i=0; i<num_Points_Theta; ++i)
				{
					measurement.detector_Theta_Angle_Vec[i] = angle_Temp + theta_Binning_Shift;
					measurement.detector_Theta_Cos_Vec [i] = cos(qDegreesToRadians(measurement.detector_Theta_Angle_Vec[i]));
					measurement.detector_Theta_Sin_Vec [i] = cos(qDegreesToRadians(measurement.detector_Theta_Angle_Vec[i]));
					measurement.detector_Theta_Cos2_Vec[i] = pow(measurement.detector_Theta_Cos_Vec [i],2);
					angle_Temp += angle_Step;
				}
			}
			// detector PHI angle_cos_sin : vector
			{
				size_t num_Points_Phi = curve.value_2D_Shifted.size();
				measurement.detector_Phi_Angle_Vec.resize(num_Points_Phi);
				measurement.detector_Phi_Cos_Vec.  resize(num_Points_Phi);
				measurement.detector_Phi_Sin_Vec.  resize(num_Points_Phi);

				double minimum = min(measurement.detector_Phi_Angle.independent.max,measurement.detector_Phi_Angle.independent.min);
				double maximum = max(measurement.detector_Phi_Angle.independent.max,measurement.detector_Phi_Angle.independent.min);

				double angle_Step = (maximum - minimum) / (num_Points_Phi - 1);
				double phi_Binning_Shift = 0;
				// no need? also problems with interpolation somewhere in calculation
//				if(measurement.detector_2D.use_Binning)	{phi_Binning_Shift = 0.5*(measurement.detector_2D.phi_Binning_Factor-1)*angle_Step;}
				double angle_Temp =  minimum;
				for(size_t i=0; i<num_Points_Phi; ++i)
				{
					measurement.detector_Phi_Angle_Vec[i] = angle_Temp + phi_Binning_Shift;
					measurement.detector_Phi_Cos_Vec[i] = cos(qDegreesToRadians(measurement.detector_Phi_Angle_Vec[i]));
					measurement.detector_Phi_Sin_Vec[i] = sin(qDegreesToRadians(measurement.detector_Phi_Angle_Vec[i]));
					angle_Temp += angle_Step;
				}

				// splitting PHI points on two parts for honest calculation and interpolation

				measurement.start_Phi_Index = 0;
				measurement.end_Phi_Number = num_Points_Phi;
				if( maximum * minimum < 0 && num_Points_Phi >=11 )
				{
					int index = std::lower_bound(measurement.detector_Phi_Angle_Vec.begin(), measurement.detector_Phi_Angle_Vec.end(), 0) - measurement.detector_Phi_Angle_Vec.begin() - 1;
					if(abs(maximum) >= abs(minimum))	measurement.start_Phi_Index = index;
					else								measurement.end_Phi_Number = index+2;
				}
			}
			// beam angle_cos_sin_cos2 : single value
			{
				measurement.beam_Theta_0_Angle_Value = measurement.beam_Theta_0_Angle.value + angle_Shift;
				measurement.beam_Theta_0_Cos_Value  = abs(cos(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Sin_Value  = abs(sin(qDegreesToRadians(measurement.beam_Theta_0_Angle_Value)));
				measurement.beam_Theta_0_Cos2_Value = pow(measurement.beam_Theta_0_Cos_Value,2);
			}
			// k : single value
			{
				measurement.lambda_Value = max(measurement.wavelength.value + lambda_Shift, DBL_EPSILON);
				measurement.k_Value = 2*M_PI/measurement.lambda_Value;
			}
		}
	}
}

void Target_Curve::calc_cos2_k(double angle_Shift, double lambda_Shift)
{
	calc_Measured_cos2_k(angle_Shift, lambda_Shift);
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
	calc_Functions = referent_Target_Curve.calc_Functions;
	plot_Options_Experimental = referent_Target_Curve.plot_Options_Experimental;
	plot_Options_Calculated = referent_Target_Curve.plot_Options_Calculated;

	lines_List = referent_Target_Curve.lines_List;
	header = referent_Target_Curve.header;

	label_Text = referent_Target_Curve.label_Text;
	index = referent_Target_Curve.index;
	name = referent_Target_Curve.name;	if(mark_As_Copy) name += " (Copy)";
	index_Name = referent_Target_Curve.index_Name;

	angular_Units = referent_Target_Curve.angular_Units;
	spectral_Units = referent_Target_Curve.spectral_Units;

	graph_2D_Positions = referent_Target_Curve.graph_2D_Positions;

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
					<< curve.use_Subinterval << curve.outer_Area << curve.subinterval_Left << curve.subinterval_Right << curve.subinterval_Top << curve.subinterval_Bottom
					<< curve.horizontal_Arg_Shift << curve.horizontal_Arg_Factor
					<< curve.val_Shift << curve.val_Factor
					<< curve.divide_On_Beam_Intensity << curve.beam_Intensity_Initial << curve.use_Final_Intensity << curve.beam_Intensity_Final << curve.beam_Time
					<< curve.value_Type << curve.argument << curve.values << curve.value_2D;
}
QDataStream& operator >>( QDataStream& stream,		 Curve& curve )
{
	if(Global_Variables::check_Loaded_Version(1,11,0))		// since 1.11.0
	{
		stream	>> curve.mesh_Density_Factor >> curve.mesh_Density_Shift
				>> curve.use_Subinterval >> curve.outer_Area >> curve.subinterval_Left >> curve.subinterval_Right >> curve.subinterval_Top >> curve.subinterval_Bottom
				>> curve.horizontal_Arg_Shift >> curve.horizontal_Arg_Factor
				>> curve.val_Shift >> curve.val_Factor
				>> curve.divide_On_Beam_Intensity >> curve.beam_Intensity_Initial >> curve.use_Final_Intensity >> curve.beam_Intensity_Final;

				if(Global_Variables::check_Loaded_Version(1,11,14))
				{ stream >> curve.beam_Time;}

		stream	>> curve.value_Type >> curve.argument >> curve.values >> curve.value_2D;
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
		for(size_t i=0; i<curve.shifted_Values.size(); i++)	{
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
			QString angle_Type;
			stream >> curve.argument_Type >> angle_Type >> curve.angular_Units >> curve.spectral_Units;
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
					<< target_Curve->calc_Functions
					<< target_Curve->plot_Options_Experimental
					<< target_Curve->plot_Options_Calculated
					<< target_Curve->header << target_Curve->label_Text << target_Curve->name
					<< target_Curve->angular_Units << target_Curve->spectral_Units << target_Curve->loaded_And_Ready
					<< target_Curve->graph_2D_Positions;
}
QDataStream& operator >>(QDataStream& stream,		 Target_Curve* target_Curve )
{
	if(Global_Variables::check_Loaded_Version(1,11,0))
	{
		stream	>> target_Curve->curve >> target_Curve->fit_Params >> target_Curve->measurement
				>> target_Curve->filename >> target_Curve->filepath
				>> target_Curve->calc_Functions
				>> target_Curve->plot_Options_Experimental
				>> target_Curve->plot_Options_Calculated
				>> target_Curve->header >> target_Curve->label_Text;

		if(Global_Variables::check_Loaded_Version(1,11,5))
		{stream	>> target_Curve->name;}

		stream	>> target_Curve->angular_Units >> target_Curve->spectral_Units >> target_Curve->loaded_And_Ready
				>> target_Curve->graph_2D_Positions;
	} else
	{
		stream	>> target_Curve->curve >> target_Curve->fit_Params >> target_Curve->measurement
				>> target_Curve->filename >> target_Curve->filepath;

		stream  >> target_Curve->loaded_And_Ready;

		stream	>> target_Curve->plot_Options_Experimental;

		if(Global_Variables::check_Loaded_Version(1,7,4))
		{stream >> target_Curve->plot_Options_Calculated;}

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

		// argument type and units from old files
		if(target_Curve->curve.argument_Type == "Wavelength") target_Curve->measurement.argument_Type = argument_Types[Wavelength_Energy];
		if(target_Curve->curve.argument_Type == "Angle")	  target_Curve->measurement.argument_Type = argument_Types[Beam_Grazing_Angle];
		if(angle_Units_List.	 contains(target_Curve->curve.angular_Units )) target_Curve->angular_Units  = target_Curve->curve.angular_Units;
		if(wavelength_Units_List.contains(target_Curve->curve.spectral_Units)) target_Curve->spectral_Units = target_Curve->curve.spectral_Units;
	}

	target_Curve->fill_Measurement_And_Curve_With_Shifted_Data();
	target_Curve->refresh_Description_Label();
	return stream;
}

