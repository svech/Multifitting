#include "launcher.h"
#include <QApplication>

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	Q_UNUSED(context);

	if (type != QtWarningMsg || !msg.startsWith("QWindowsWindow::setGeometry")) {
		QByteArray localMsg = msg.toLocal8Bit();
		fprintf(stdout, localMsg.constData());
	}
}

int main(int argc, char *argv[])
{
	qInstallMessageHandler(myMessageOutput);
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);
	Launcher launcher;
//	launcher.show();

	return app.exec();
}
