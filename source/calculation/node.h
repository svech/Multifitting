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

	int calculate_Intermediate_Points(tree<Node>::iterator this_Iter, tree<Node>::iterator active_Iter, QString active_Whats_This, QList<Node>& flat_List, QMap<int, tree<Node>::iterator>& flat_Tree_Map, QString& warning_Text);

	QString whats_This;
	QStringList whats_This_List;

	Measurement measurement;
	Ambient ambient;
	Layer layer;
	Stack_Content stack_Content;
	Substrate substrate;

	QVector<intermediate_Point> plot_Points;
};

#endif // NODE_H
