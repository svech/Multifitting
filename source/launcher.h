#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QMainWindow>

class Launcher : public QMainWindow
{
	Q_OBJECT

public:
	Launcher(QWidget *parent = 0);
	~Launcher();
};

#endif // LAUNCHER_H
