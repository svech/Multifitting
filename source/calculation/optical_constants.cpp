#include "optical_constants.h"

Optical_Constants::Optical_Constants()
{
	read_All_Materials();
}

void Optical_Constants::read_All_Materials()
{
	// TODO
	QDir nk_Dir(nk_Path);
	nk_Files_List = nk_Dir.entryList(QDir::Files);

	for(int i=0; i<nk_Files_List.size(); ++i)
	{
		Material_Data new_Material_Data;
		new_Material_Data.read_Material(nk_Files_List[i]);

	}
}

void Optical_Constants::read_All_Elements()
{
	// TODO
}
