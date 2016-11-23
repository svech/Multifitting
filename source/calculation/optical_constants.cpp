#include "optical_constants.h"
#include "QtConcurrent/QtConcurrentRun"

Optical_Constants::Optical_Constants()
{
	{
		auto start = std::chrono::system_clock::now();

//		QFuture<void> f1 = QtConcurrent::run(this, &Optical_Constants::read_All_Materials);
		read_All_Materials();
		read_All_Elements();

		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		qInfo() << "Read all: "<< elapsed.count()/1000. << " seconds" << endl;
	}

//	Material_Data temp_Material_Data = material_Data_List[0];
//	for(int i=temp_Material_Data.material_Data.size()-20; i<temp_Material_Data.material_Data.size(); ++i)
//	for(int i=0; i<20; ++i)
//	{
//		qInfo() << QString::number(temp_Material_Data.material_Data[i].lambda,'f',9)
//				<< QString::number(temp_Material_Data.material_Data[i].n,'g',9)
//				<< QString::number(temp_Material_Data.material_Data[i].k,'g',9) ;
//	}

//	Element_Data temp_Element_Data = element_Data_List[0];
//	for(int i=temp_Element_Data.element_Data.size()-20; i<temp_Element_Data.element_Data.size(); ++i)
//	for(int i=0; i<20; ++i)
//	{
//		qInfo() << QString::number(temp_Element_Data.element_Data[i].lambda,'f',9)
//				<< QString::number(temp_Element_Data.element_Data[i].f1,'g',9)
//				<< QString::number(temp_Element_Data.element_Data[i].f2,'g',9) ;
//	}

}

void Optical_Constants::read_All_Materials()
{
	// TODO
	QDir nk_Dir(nk_Path);
	nk_Dir.setNameFilters(QStringList() << nk_Filter);
	nk_Files_List = nk_Dir.entryList(QDir::Files);

	for(int i=0; i<nk_Files_List.size(); ++i)
	{
		Material_Data new_Material_Data;
		new_Material_Data.read_Material(nk_Files_List[i]);
		new_Material_Data.filename = nk_Files_List[i];
		material_Data_List.append(new_Material_Data);
	}
}

void Optical_Constants::read_All_Elements()
{
	// TODO
	QDir f1f2_Dir(f1f2_Path);
	f1f2_Dir.setNameFilters(QStringList() << f1f2_Filter);
	f1f2_Files_List = f1f2_Dir.entryList(QDir::Files);

	for(int i=0; i<f1f2_Files_List.size(); ++i)
	{
		Element_Data new_Element_Data;
		new_Element_Data.read_Element(f1f2_Files_List[i]);
		element_Data_List.append(new_Element_Data);

		QString filename = f1f2_Files_List[i];
		new_Element_Data.element = filename.split(".",QString::SkipEmptyParts)[0];
	}
}
