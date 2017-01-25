#ifndef NODE_H
#define NODE_H

#include "global/layer_data_types.h"
#include "multilayer_approach/multilayer/independent_variables.h"
#include "tree.hh"

class Node
{
public:
	Node();
	Node(QTreeWidgetItem* item);

	int calculate_Intermediate_Points(tree<Node>::iterator active_Iter, QString active_Whats_This, QString& warning_Text);

	QString whats_This;
	QStringList whats_This_List;

	Measurement measurement;
	Ambient ambient;
	Layer layer;
	Stack_Content stack_Content;
	Substrate substrate;

	// for any type
	Ambient common_Data;
	complex<double> delta_Epsilon;
	complex<double> epsilon;
};

#endif // NODE_H
