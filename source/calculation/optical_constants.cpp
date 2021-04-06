#include "optical_constants.h"

using namespace std;

Optical_Constants::Optical_Constants():
	num_Threads(optical_constants_read_threads),
	part_Material_Maps(num_Threads),
	part_Element_Maps(num_Threads)
{
	reload();
}

void Optical_Constants::reload()
{
	part_Material_Maps.clear();
	part_Material_Maps.resize(num_Threads);
	part_Element_Maps.clear();
	part_Element_Maps.resize(num_Threads);
	material_Map.clear();
	element_Map.clear();

	// read in background
	thread worker = thread(&Optical_Constants::read_All, this);
	worker.detach();
}

void Optical_Constants::read_All()
{
	auto start = std::chrono::system_clock::now();

	read_All_Materials();
	read_All_Elements();

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	qInfo() << "Read optical constants: "<< elapsed.count()/1000. << " seconds" << endl;

//	Material_Data temp_Material_Data = material_Map.value("Y_sae-lao.nk");
//	qInfo() << temp_Material_Data.substance << "\t" << temp_Material_Data.filename << endl;
//	for(int i=temp_Material_Data.material_Data.size()-20; i<temp_Material_Data.material_Data.size(); ++i)
////	for(int i=0; i<20; ++i)
//	{
//		qInfo() << Locale.toString(temp_Material_Data.material_Data[i].lambda,'f',9)
//				<< Locale.toString(temp_Material_Data.material_Data[i].re,'g',9)
//				<< Locale.toString(temp_Material_Data.material_Data[i].im,'g',9) << endl ;
//	}

//	Element_Data temp_Element_Data = element_Map.value("Ag.ff");
//	qInfo() << temp_Element_Data.element;
//	for(int i=temp_Element_Data.element_Data.size()-20; i<temp_Element_Data.element_Data.size(); ++i)
////	for(int i=0; i<20; ++i)
//	{
//		qInfo() << Locale.toString(temp_Element_Data.element_Data[i].lambda,'f',9)
//				<< Locale.toString(temp_Element_Data.element_Data[i].re,'g',9)
//				<< Locale.toString(temp_Element_Data.element_Data[i].im,'g',9) << endl ;
//	}
}

void Optical_Constants::read_nMin_nMax_Materials(int n_Min, int n_Max, int thread_Index)
{	
	for(int i=n_Min; i<n_Max; ++i)
	{
		Material_Data new_Material_Data;
		new_Material_Data.read_Material(nk_Files_List[i]);
		new_Material_Data.filename = nk_Files_List[i];
		part_Material_Maps[thread_Index].insert(nk_Files_List[i], new_Material_Data);
	}
}

void Optical_Constants::read_All_Materials()
{
	// TODO
	QDir nk_Dir(nk_Path);
	nk_Dir.setNameFilters(QStringList() << nk_Filter);
	nk_Files_List = nk_Dir.entryList(QDir::Files);
	nk_Files_List.append(QString(Vacuum)+nk_Ext);

	// ----------------------------------------------------------------------------------------------------------------------
	// parallelization
	vector<thread> workers(num_Threads);
	int delta_N = (nk_Files_List.size()) / num_Threads;	// number of points per thread

	for (int thread_Index = 0; thread_Index < num_Threads - 1; ++thread_Index)
	{
		int n_Min = thread_Index * delta_N;
		int n_Max = (thread_Index + 1) * delta_N;

		workers[thread_Index] = thread(&Optical_Constants::read_nMin_nMax_Materials, this, n_Min, n_Max, thread_Index);
	}

	int thread_Index = num_Threads - 1;
	int n_Min = thread_Index * delta_N;
	int n_Max = nk_Files_List.size();

	workers[thread_Index] = thread(&Optical_Constants::read_nMin_nMax_Materials, this, n_Min, n_Max, thread_Index);

	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		if (workers[thread_Index].joinable()) workers[thread_Index].join();	// присоединение потоков
	}
	// ----------------------------------------------------------------------------------------------------------------------

	// join data
	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		material_Map.unite(part_Material_Maps[thread_Index]);
	}
}

void Optical_Constants::read_nMin_nMax_Elements(int n_Min, int n_Max, int thread_Index)
{
	for(int i=n_Min; i<n_Max; ++i)
	{
		Element_Data new_Element_Data;
		new_Element_Data.read_Element(f1f2_Files_List[i]);
		QString filename = f1f2_Files_List[i];
		new_Element_Data.element = filename.split(".",QString::SkipEmptyParts)[0];
		part_Element_Maps[thread_Index].insert(f1f2_Files_List[i], new_Element_Data);
	}
}

void Optical_Constants::read_All_Elements()
{
	// TODO
	QDir f1f2_Dir(f1f2_Path);
	f1f2_Dir.setNameFilters(QStringList() << f1f2_Filter);
	f1f2_Files_List = f1f2_Dir.entryList(QDir::Files);

	// ----------------------------------------------------------------------------------------------------------------------
	// parallelization
	vector<thread> workers(num_Threads);
	int delta_N = (f1f2_Files_List.size()) / num_Threads;	// number of points per thread

	for (int thread_Index = 0; thread_Index < num_Threads - 1; ++thread_Index)
	{
		int n_Min = thread_Index * delta_N;
		int n_Max = (thread_Index + 1) * delta_N;

		workers[thread_Index] = thread(&Optical_Constants::read_nMin_nMax_Elements, this, n_Min, n_Max, thread_Index);
	}

	int thread_Index = num_Threads - 1;
	int n_Min = thread_Index * delta_N;
	int n_Max = f1f2_Files_List.size();

	workers[thread_Index] = thread(&Optical_Constants::read_nMin_nMax_Elements, this, n_Min, n_Max, thread_Index);

	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		if (workers[thread_Index].joinable()) workers[thread_Index].join();	// присоединение потоков
	}
	// ----------------------------------------------------------------------------------------------------------------------

	// join data
	for (int thread_Index = 0; thread_Index < num_Threads; ++thread_Index)
	{
		element_Map.unite(part_Element_Maps[thread_Index]);
	}
}

void Optical_Constants::interpolation_Epsilon(QVector<Point>& input_Values, vector<double>& spectral_Points, vector<complex<double>>& output_Values, QString material)
{
	const gsl_interp_type *interp_type = gsl_interp_steffen;

	gsl_interp_accel* acc_Re = gsl_interp_accel_alloc();
	gsl_interp_accel* acc_Im = gsl_interp_accel_alloc();
	gsl_spline* spline_Re = gsl_spline_alloc(interp_type, input_Values.size());
	gsl_spline* spline_Im = gsl_spline_alloc(interp_type, input_Values.size()); //-V656

	QVector<double> lambda,re,im;
	for(int i=0; i<input_Values.size(); ++i)
	{
		lambda.push_back(input_Values[i].lambda);
		re.push_back(input_Values[i].re);
		im.push_back(input_Values[i].im);
	}

	// spline initialization
	gsl_spline_init(spline_Re, lambda.data(), re.data(), input_Values.size());
	gsl_spline_init(spline_Im, lambda.data(), im.data(), input_Values.size());

	output_Values.resize(spectral_Points.size());
	for(size_t l=0; l<spectral_Points.size(); ++l)
	{
		// range check
		if((spectral_Points[l] <= lambda.first()) ||
		   (spectral_Points[l] >= lambda.last()))
		{
			if(!lambda_Out_Of_Range)
			{
				QString warning_Text = "Wavelength is out of range for " + material + ".\nAcceptable range is " + Locale.toString(lambda.first())
																												+ " - " + Locale.toString(lambda.last())
																												+ " " + wavelength_Units_List[angstrom];
				QMessageBox::warning(nullptr, "Optical constants", warning_Text);
				lambda_Out_Of_Range = true;
			}
			if(spectral_Points[l] <= lambda.first()) output_Values[l] = complex<double>(gsl_spline_eval(spline_Re, lambda.first(), acc_Re), gsl_spline_eval(spline_Im, lambda.first(), acc_Im));
			if(spectral_Points[l] >= lambda.last() ) output_Values[l] = complex<double>(gsl_spline_eval(spline_Re, lambda.last(),  acc_Re), gsl_spline_eval(spline_Im, lambda.last(),  acc_Im));
		} else
		{
			output_Values[l] = complex<double>(gsl_spline_eval(spline_Re, spectral_Points[l], acc_Re), gsl_spline_eval(spline_Im, spectral_Points[l], acc_Im));
		}
	}

	gsl_spline_free(spline_Re);
	gsl_spline_free(spline_Im);
	gsl_interp_accel_free(acc_Re);
	gsl_interp_accel_free(acc_Im);
}

void Optical_Constants::make_Epsilon_From_Factors(const QList<Stoichiometry>& composition, double density, vector<double>& spectral_Points, vector<complex<double>>& epsilon)
{
	double denominator = 0;	// sum of stoich and masses

	// for each element
	for(int element_Index=0; element_Index<composition.size(); ++element_Index)
	{
		QString element = composition[element_Index].type;
		denominator += composition[element_Index].composition.value * sorted_Elements.value(element);
	}
	if(abs(denominator)<DBL_EPSILON) {denominator = DBL_EPSILON;}

	double compound_Concentration = Na * density / denominator;
	vector<double> element_Concentration(composition.size());
	vector<complex<double>> n (spectral_Points.size(), 1);	// index of refraction, initialized by 1
	vector<complex<double>> interpolated (spectral_Points.size(),-2016.0);

	// calculation of concentrations
	for(int element_Index=0; element_Index<composition.size(); ++element_Index)
	{
		QString element = composition[element_Index].type;
		Element_Data temp_Element_Data = element_Map.value(element + ff_Ext);

		element_Concentration[element_Index] = compound_Concentration * composition[element_Index].composition.value;
		interpolation_Epsilon(temp_Element_Data.element_Data, spectral_Points, interpolated, element);

		for(size_t point_Index=0; point_Index<spectral_Points.size(); ++point_Index)
		{
			n[point_Index] -= spectral_Points[point_Index] * spectral_Points[point_Index] * Q_q * element_Concentration[element_Index] * interpolated[point_Index];
		}
	}

	epsilon.resize(spectral_Points.size());
	for(size_t point_Index=0; point_Index<spectral_Points.size(); ++point_Index)
	{
		epsilon[point_Index] = conj(n[point_Index]*n[point_Index]);
	}
}
