#include "optical_constants.h"
#include <thread>

using namespace std;

Optical_Constants::Optical_Constants():
	num_Threads(optical_Constants_Read_Threads),
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
	// TODO timers
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
//		qInfo() << QString::number(temp_Material_Data.material_Data[i].lambda,'f',9)
//				<< QString::number(temp_Material_Data.material_Data[i].n,'g',9)
//				<< QString::number(temp_Material_Data.material_Data[i].k,'g',9) ;
//	}

//	Element_Data temp_Element_Data = element_Map.value("Ag.ff");
//	qInfo() << temp_Element_Data.element;
//	for(int i=temp_Element_Data.element_Data.size()-20; i<temp_Element_Data.element_Data.size(); ++i)
////	for(int i=0; i<20; ++i)
//	{
//		qInfo() << QString::number(temp_Element_Data.element_Data[i].lambda,'f',9)
//				<< QString::number(temp_Element_Data.element_Data[i].f1,'g',9)
//				<< QString::number(temp_Element_Data.element_Data[i].f2,'g',9) ;
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
