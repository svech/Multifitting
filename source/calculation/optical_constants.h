#ifndef OPTICAL_CONSTANTS_H
#define OPTICAL_CONSTANTS_H

#include "global/settings.h"

class Optical_Constants
{
public:
	Optical_Constants();

	QList<Material_Data> material_Data_List;
	QList<Element_Data>	  element_Data_List;

private:
	void read_All_Materials();
	void read_All_Elements();

private:
	QStringList   nk_Files_List;
	QStringList f1f2_Files_List;
};

#endif // OPTICAL_CONSTANTS_H
