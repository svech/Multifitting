#ifndef OPTICAL_CONSTANTS_H
#define OPTICAL_CONSTANTS_H

#include "global/settings.h"
#include <thread>
#include "gsl/gsl_interp.h"
#include "gsl/gsl_spline.h"

class Optical_Constants
{
public:
	Optical_Constants();

	QMap<QString, Material_Data> material_Map;
	QMap<QString, Element_Data>  element_Map;

	void reload();
	void interpolation_Epsilon(QVector<Point>& input_Values, QVector<double>& spectral_Points, QVector<complex<double>>& output_Values, QString material);
	void make_Epsilon_From_Factors(const QList<Stoichiometry>& composition, double density, QVector<double>& spectral_Points, QVector<complex<double>>& epsilon);
private:
	void read_All();
	void read_nMin_nMax_Materials(int n_Min, int n_Max, int thread_Index);
	void read_All_Materials();
	void read_nMin_nMax_Elements(int n_Min, int n_Max, int thread_Index);
	void read_All_Elements();

public:
	int num_Threads;

	QVector<QMap<QString, Material_Data>> part_Material_Maps;
	QVector<QMap<QString, Element_Data>>  part_Element_Maps;

	QStringList   nk_Files_List;
	QStringList f1f2_Files_List;
};

#endif // OPTICAL_CONSTANTS_H
