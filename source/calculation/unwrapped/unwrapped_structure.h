#ifndef UNWRAPPED_STRUCTURE_H
#define UNWRAPPED_STRUCTURE_H

#include "calculation/node.h"

class Unwrapped_Structure
{
public:
	Unwrapped_Structure();
	Unwrapped_Structure(int num_Media);

	int num_Threads;
	int num_Media;
	int num_Layers;
	int num_Boundaries;

	MyVector<complex<double>> epsilon;								//	[media]
	MyVector<double> epsilon_Norm;									//	[media]
	MyVector<double> epsilon_RE;									//	[media]
	MyVector<double> epsilon_IM;									//	[media]

	MyVector<double> sigma;											//	[boundary]
	MyVector<MyVector<Interlayer>> boundary_Interlayer_Composition;	//  [boundary][function]
	MyVector<double> thickness;										//	[layer]

	int fill_Epsilon_Max_Depth_2  (tree<Node>::iterator parent, int media_Index = 0);
	int fill_Sigma_Max_Depth_2    (tree<Node>::iterator parent, int boundary_Index = 0);
	int fill_Thickness_Max_Depth_2(tree<Node>::iterator parent, int layer_Index = 0);

	int fill_Epsilon  (tree<Node>::iterator parent, int media_Index = 0);
	int fill_Sigma    (tree<Node>::iterator parent, int boundary_Index = 0);
	int fill_Thickness(tree<Node>::iterator parent, int layer_Index = 0);
};

template <typename T>
void print_Vector	(QString name, MyVector<T>& vec, int transpose)	// output
{
	if(vec.size()==0) return;
	if(transpose==0)
	{
		cout<<name.toStdString()<<"[0.."<<vec.size()-1<<"] = ";
		for(auto i=0; i<vec.size(); ++i)
			cout<<vec[i]<<"\t";
	} else
	for(auto i=0; i<vec.size(); ++i)
	{
		cout<<name.toStdString()<<"["<<i<<"] = ";
		cout<<vec[i]<<endl;
	}
	cout<<endl;
}

#endif // UNWRAPPED_STRUCTURE_H
