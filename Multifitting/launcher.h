#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QWidget>

class Launcher : public QWidget
{
	Q_OBJECT

public:
	Launcher(QWidget *parent = 0);
	~Launcher();
};

#endif // LAUNCHER_H
