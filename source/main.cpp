#include "launcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	IF_GUI
	{
		Launcher launcher;
		launcher.show();
		return app.exec();
	}
	else
	{
		Launcher launcher;
		return 0;
	}
}
