// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "launcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Launcher launcher;
//	launcher.show();

	return app.exec();
}
