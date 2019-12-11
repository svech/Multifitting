#ifndef PROFILE_PLOT_H
#define PROFILE_PLOT_H

#include "multilayer_approach/multilayer_approach.h"

class Profile_Plot : public QWidget
{
	Q_OBJECT
public:
	explicit Profile_Plot(Multilayer* multilayer, QWidget *parent = nullptr);

	void create_Main_Layout();
	void create_Plot_Frame_And_Scale();
	void plot_Data(bool fast = false);
	void calculate_Profile();
	void unwrap_Subtree(QVector<Data>& struct_Data_Vector, QTreeWidgetItem* item);
	double delta_Epsilon_Func(double z);

	Multilayer* multilayer;
	QVBoxLayout* main_Layout;
	QCustomPlot* custom_Plot;

	QVector<Data> struct_Data_Vector;
	QVector<double> boundary_Vector;
	QVector<double> thickness_Vector;
	QVector<double> sigma_Vector;
	QVector<QString> material_Vector;
	QVector<double> density_Vector;
	QVector<double> delta_Epsilon_Vector;
	QVector<double> beta_Epsilon_Vector;
	QVector<QString> different_Materials;

	QVector<double> arg;
	QVector<double> val;
};

#endif // PROFILE_PLOT_H
