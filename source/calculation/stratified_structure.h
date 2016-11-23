#ifndef STRATIFIED_STRUCTURE_H
#define STRATIFIED_STRUCTURE_H

#include "calculation_tree.h"

class Stratified_Structure
{
public:
	Stratified_Structure(Calculation_Tree* calculation_Tree);

private:
	void unwrap_Structure();


	vector<myMatrix> unwrapped_Matrix_Structure;
};

#endif // STRATIFIED_STRUCTURE_H
