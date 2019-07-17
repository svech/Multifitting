#include "target_curve.h"
//#include "gsl/gsl_integration.h"

Target_Curve::Target_Curve(QLabel* description_Label, QTreeWidget* real_Struct_Tree, QWidget *parent) :
	description_Label(description_Label),
	real_Struct_Tree(real_Struct_Tree),
	measurement(item_Type_Measurement),
	QWidget(parent)
{
	curve.argument_Type = whats_This_Angle;		// angular curve
	curve.angle_Type = angle_Type_Grazing/*default_angle_type*/	;
	curve.angular_Units = angle_Units_List[degree]/*angle_units*/	;
	curve.spectral_Units = wavelength_Units_List[angstrom]/*wavelength_units*/;
	curve.value_Function = value_Function[Reflectance];
	curve.value_Mode = value_R_Mode[R];			// R

	curve.arg_Offset = 0; curve.arg_Factor = 1;
	curve.val_Offset = 0; curve.val_Factor = 1;

	{
		plot_Options_Calculated.color=QColor(0, 0, 255);
		plot_Options_Calculated.scatter_Shape = QCPScatterStyle::ssNone;
		plot_Options_Calculated.scatter_Size=5;
		plot_Options_Calculated.thickness=2;

		plot_Options_Calculated.color_Second=QColor(0, 255, 0);
		plot_Options_Calculated.scatter_Shape_Second = QCPScatterStyle::ssNone;
		plot_Options_Calculated.scatter_Size_Second=5;
		plot_Options_Calculated.thickness_Second=2;
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

		if( temp_Line[0]!=';' && temp_Line[0]!='#' && numbers.size()>0)
		try
		{
			bool ok_To_Double = false;

			/// argument

			if(numbers.size()<=number_Index) throw "arg | " + main_Exception_Text;
			double temp_Argument = QString(numbers[number_Index]).toDouble(&ok_To_Double);

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

			if(curve.value_Mode == value_R_Mode[R] || curve.value_Mode == value_T_Mode[T] )	// R , T
			{
				if(numbers.size()<=number_Index) throw "val_1 | " + main_Exception_Text;
				double temp_Val_1 = QString(numbers[number_Index]).toDouble(&ok_To_Double);
				++number_Index;

				Value val;
				val.val_1 = temp_Val_1;

				if(!ok_To_Double)
				{
					curve.argument.removeLast();
					goto skip_line_label;
				}
				curve.values.push_back(val);
			} else
			if(curve.value_Mode == value_R_Mode[R_Phi] )	// R+phi
			{
				if(numbers.size()<=number_Index) throw "val_1 | " + main_Exception_Text;
				double temp_Number_1 = QString(numbers[number_Index]).toDouble(&ok_To_Double);
				++number_Index;
				if(numbers.size()<=number_Index) throw "val_2 | " + main_Exception_Text;
				double temp_Number_2 = QString(numbers[number_Index]).toDouble(&ok_To_Double);
				++number_Index;

				Value val;
				val.val_1 = temp_Number_1;
				val.val_2 = temp_Number_2;

				if(!ok_To_Double)
				{
					curve.argument.removeLast();
					goto skip_line_label;
				}
				curve.values.push_back(val);
			} else
			{
				throw "wrong curve.value_Mode=" + curve.value_Mode + main_Exception_Text;
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

//struct Params
//{
//	double* argument_Vec;
//	double* value_Vec;
//	size_t size;
//	double energy_Value;
//};

//double beta_Func(double energy2, void* params)
//{
//	Params* iPB = reinterpret_cast<Params*>(params);

//	const gsl_interp_type* interp_type = gsl_interp_linear;
//	gsl_interp_accel* Acc = gsl_interp_accel_alloc();
//	gsl_spline* Spline = gsl_spline_alloc(interp_type, iPB->size);

//	gsl_spline_init(Spline, iPB->argument_Vec, iPB->value_Vec, iPB->size);

//	double energy = sqrt(energy2);
//	double interp_Value = gsl_spline_eval(Spline, energy, Acc);

//	gsl_spline_free(Spline);
//	gsl_interp_accel_free(Acc);

//	return interp_Value * 0.5;
//}

void Target_Curve::fill_Measurement_With_Data()
{
	if(loaded_And_Ready)
	{
		// check zero intensity
		if(abs(curve.beam_Intensity)<=DBL_EPSILON) curve.beam_Intensity = 1;

		// preliminary fill calculated data with 0
		calculated_Values.R.	resize(curve.argument.size());	calculated_Values.R.fill(0);
		calculated_Values.Phi_R.resize(curve.argument.size());	calculated_Values.Phi_R.fill(0);
		calculated_Values.T.	resize(curve.argument.size());	calculated_Values.T.fill(0);
		calculated_Values.A.	resize(curve.argument.size());	calculated_Values.A.fill(0);

		// argument shift
		// shift has the same units as data
		curve.shifted_Argument.resize(curve.argument.size());
		curve.shifted_Values.resize(curve.argument.size());

		double intensity_Factor = 1;
		if(curve.divide_On_Beam_Intensity) intensity_Factor = curve.beam_Intensity;

		for(int i=0; i<curve.argument.size(); ++i)
		{
			curve.shifted_Argument[i]     = curve.argument[i]                     *curve.arg_Factor+curve.arg_Offset;
			curve.shifted_Values[i].val_1 = curve.values[i].val_1/intensity_Factor*curve.val_Factor+curve.val_Offset;
			// shift only first
			curve.shifted_Values[i].val_2 = curve.values[i].val_2;//*curve.val_Factor+curve.val_Offset;
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
//			qInfo() << "min ="<<measurement.angle.first() << "; max" << measurement.angle.last();
		} else
		if(curve.argument_Type == whats_This_Wavelength)	// spectral
		{
			double coeff = wavelength_Coefficients_Map.value(curve.spectral_Units);
			measurement.lambda.resize(curve.shifted_Argument.size());
			for(int i=0; i<curve.shifted_Argument.size(); ++i)
			{
				measurement.lambda[i] = Global_Variables::wavelength_Energy(curve.spectral_Units,curve.shifted_Argument[i]*coeff);
			}
//			qInfo() << "min ="<<measurement.lambda.first() << "; max" << measurement.lambda.last();
		} else
		{
			QMessageBox::critical(nullptr, "Target_Curve::fill_Measurement_With_Data", "wrong curve.argument_Type="+curve.argument_Type);
			return;
		}
	}

//	// TEMPORARY (TODO)
//	{
//		// effect of beam size
//		double error;
//		int key = GSL_INTEG_GAUSS15;
//		const double epsabs = 1e-3;
//		const double epsrel = 1e-3;
//		size_t limit = 100;
//		gsl_integration_workspace* w = gsl_integration_workspace_alloc (limit);

//		double* val = new double[curve.argument.size()];
//		double* arg = new double[curve.argument.size()];
//		for(int i=0; i<curve.argument.size(); ++i)	{
//			arg[i] = curve.argument[i];
//			val[i] = curve.values[i].val_1;
//		}

//		QString name = "delta summary.txt";
//		QFile file(name);
//		if (file.open(QIODevice::WriteOnly))
//		{
//			QTextStream out(&file);
//			out.setFieldAlignment(QTextStream::AlignLeft);

//			for(double energy : curve.argument)
//			{
//				if( 10.0<=energy && energy<=251.501)
//				{
////					double energy = 181.50991673992075;//max(10.,curve.beam_Intensity);

//					Params params = { arg, val, curve.argument.size(), energy };
//					gsl_function F2 = { &beta_Func, &params };

//					double deltaEn_1=energy/100;
//					double deltaEn_2=energy/1000;
//					double lowLimit = 0.02;
//					double upLimit = 432000;

////					double re = 2.8179403267E-15;
////					double h = 4.135667662E-15;
////					double c = 299792458;
////					double q = 1.23479E29;
////					double lambda = (12398.41930092394328/energy)/1E10;
////					double Zx=(4.-pow(4./82.5,2.37));

//					// calculate
//					double output=-2019;
//					double integral = 0;

//					// division
//					double point_0 = lowLimit;
//					double point_1 = energy/100;
//					double point_2 = energy/10;
//					double point_3 = energy/5;
//					double point_4 = energy/2;
//					double point_5 = energy/1.5;
//					double point_6 = energy/1.1;
//					double point_7 = energy-deltaEn_1;
//					double point_8 = energy-deltaEn_2;
//					double point_9 = energy+deltaEn_2;
//					double point_10 = energy+deltaEn_1;
//					double point_11 = energy*1.1;
//					double point_12 = energy*1.5;
//					double point_13 = energy*2;
//					double point_14 = energy*2.5;
//					double point_15 = energy*3;
//					double point_16 = energy*4;
//					double point_17 = energy*6;
//					double point_18 = energy*10;
//					double point_19 = energy*20;
//					double point_20 = energy*40;
//					double point_21 = energy*100;
//					double point_22 = energy*200;
//					double point_23 = upLimit;

//					gsl_integration_qawc(&F2,pow(point_0,2), pow(point_1,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_1,2), pow(point_2,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_2,2), pow(point_3,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_3,2), pow(point_4,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_4,2), pow(point_5,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_5,2), pow(point_6,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_6,2), pow(point_7,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_7,2), pow(point_8,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_8,2), pow(point_9,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_9,2), pow(point_10,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_10,2), pow(point_11,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_11,2), pow(point_12,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_12,2), pow(point_13,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_13,2), pow(point_14,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_14,2), pow(point_15,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_15,2), pow(point_16,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_16,2), pow(point_17,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_17,2), pow(point_18,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_18,2), pow(point_19,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_19,2), pow(point_20,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_20,2), pow(point_21,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_20,2), pow(point_21,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_21,2), pow(point_22,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;
//					gsl_integration_qawc(&F2,pow(point_22,2), pow(point_23,2), pow(energy,2), epsabs, epsrel, limit, /*key,*/ w, &output, &error); integral-=output;

//					double delta = 2./M_PI * integral;


//					qInfo() << "energy =" << energy << "delta =" << delta << "beta =" << 2*beta_Func(energy*energy, &params);
//					out << energy << "\t\t" << delta << endl;
//				}
//			}

//			file.close();
//		}

//		delete val;
//		delete arg;
//		gsl_integration_workspace_free(w);
//	}
}

void Target_Curve::set_Text_To_Label()
{
	description_Label->setText(label_Text);
}

// serialization
QDataStream& operator <<( QDataStream& stream, const Target_Curve* target_Curve )
{
	return stream	<< target_Curve->curve << target_Curve->fit_Params << target_Curve->measurement << target_Curve->filename
					<< target_Curve->filepath << target_Curve->loaded_And_Ready	<< target_Curve->plot_Options_Experimental
					<< target_Curve->plot_Options_Calculated		// since 1.7.4
					<< target_Curve->calculated_Values

					<< target_Curve->lines_List << target_Curve->arg_Units << target_Curve->at_Fixed << target_Curve->arg_Type_For_Label << target_Curve->ang_Type_For_Label_At_Fixed << target_Curve->label_Text
	;
}
QDataStream& operator >>(QDataStream& stream,		 Target_Curve* target_Curve )
{
	stream	>> target_Curve->curve >> target_Curve->fit_Params >> target_Curve->measurement >> target_Curve->filename
			>> target_Curve->filepath >> target_Curve->loaded_And_Ready	>> target_Curve->plot_Options_Experimental;

	if(Global_Variables::check_Loaded_Version(1,7,4))
	{stream >> target_Curve->plot_Options_Calculated ; }	// since 1.7.4

	stream  >> target_Curve->calculated_Values
			>> target_Curve->lines_List >> target_Curve->arg_Units >> target_Curve->at_Fixed >> target_Curve->arg_Type_For_Label >> target_Curve->ang_Type_For_Label_At_Fixed >> target_Curve->label_Text;
	return stream;
}
