#include "launcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	Launcher launcher;
	launcher.show();
	return app.exec();
}

//#include <iostream>
//using namespace std;

//class Car
//{
//	public:
//	int speed;

//	int* pSpeed = &speed;
//};

//int main()
//{
//	Car c1, c2;
//	c1.speed = 1;       // direct access
//	cout << "c1 speed is " << c1.speed << endl;
//	cout << "c2 speed is " << c2.speed << endl;
//	*c1.pSpeed=123;
//	cout << "c1 speed is " << c1.speed << endl;
//	cout << "c2 speed is " << c2.speed << endl;
//	return 0;
//}
