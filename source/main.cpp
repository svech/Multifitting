#include "launcher.h"
#include <QApplication>
///#include <fstream>

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	Q_UNUSED(context);

	if ((type != QtWarningMsg ||(!msg.startsWith("QWindowsWindow::setGeometry") &&
								 !msg.startsWith("QPainter::begin") &&
								 !msg.startsWith("QPainter::setRenderHint")
								 ))
			&&!msg.startsWith("void __cdecl"))
	{
		QByteArray localMsg = msg.toLocal8Bit();
		fprintf(stdout, localMsg.constData());
	}
}

int main(int argc, char *argv[])
{
	/** redirect cout to file
	// backup cout buffer and redirect to out.txt
	ofstream out("out.txt");

	auto* coutbuf = cout.rdbuf();
	cout.rdbuf(out.rdbuf());
	cout << "This will be redirected to file out.txt" << endl;

	// reset cout buffer
	cout.rdbuf(coutbuf);
	cout << "This will be printed on console" << endl;
	*/

	qInstallMessageHandler(myMessageOutput);
//	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // 1 with AA_EnableHighDpiScaling // 2 without AA_EnableHighDpiScaling
	QApplication app(argc, argv);
	Launcher launcher;
//	launcher.show();

	return app.exec();
}
