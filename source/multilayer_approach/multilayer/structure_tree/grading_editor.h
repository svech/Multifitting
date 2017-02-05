#ifndef GRADING_EDITOR_H
#define GRADING_EDITOR_H

#include <QtWidgets>

class Grading_Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Grading_Editor(QTreeWidgetItem* item, QWidget* parent = 0);

signals:

public slots:
};

#endif // GRADING_EDITOR_H
