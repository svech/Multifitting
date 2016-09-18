#include "launcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

//	app.setStyle(QStyleFactory::create("Fusion"));
//	app.setStyle(QStyleFactory::create("Windows"));
	app.setStyle(QStyleFactory::create("WindowsXP"));
//	app.setStyle(QStyleFactory::create("WindowsVista"));

	Launcher launcher;
	launcher.show();
	return app.exec();
}
