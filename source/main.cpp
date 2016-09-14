#include "launcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	Launcher launcher;
	launcher.show();
	return app.exec();
}
