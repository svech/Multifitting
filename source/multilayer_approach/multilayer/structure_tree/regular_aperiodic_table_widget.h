#ifndef REGULAR_APERIODIC_TABLE_WIDGET_H
#define REGULAR_APERIODIC_TABLE_WIDGET_H

#include "global/global_definitions.h"

class Regular_Aperiodic_Table_Widget : public QTableWidget
{
public:
	Regular_Aperiodic_Table_Widget(int rows,
								   int columns,
								   QWidget* parent = Q_NULLPTR);
};

#endif // REGULAR_APERIODIC_TABLE_WIDGET_H
