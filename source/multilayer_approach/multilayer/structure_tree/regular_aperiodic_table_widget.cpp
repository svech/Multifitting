#include "regular_aperiodic_table_widget.h"

Regular_Aperiodic_Table_Widget::Regular_Aperiodic_Table_Widget(int rows, int columns, QWidget *parent) :
	QTableWidget(parent)
{
//	verticalHeader()->setVisible(false);
	horizontalHeader()->setVisible(false);
	horizontalHeader()->setDisabled(true);
	verticalHeader()->setDisabled(true);
//	setSelectionMode(QAbstractItemView::NoSelection);
	setAttribute(Qt::WA_DeleteOnClose);

	for(int i=0; i<rows; ++i)		insertRow(i);
	for(int i=0; i<columns; ++i)	insertColumn(i);

	QAbstractItemModel *model1 = model();
	QStringList labels;
	for(int i=0; i<rowCount(); ++i)
	{
		QVariant data = model1->headerData(i, Qt::Vertical);
		if(i>=4)
			labels << QString("%1").arg(data.toInt() - 4);
		else {
			labels << "";
		}
	}
	setVerticalHeaderLabels(labels);
}
