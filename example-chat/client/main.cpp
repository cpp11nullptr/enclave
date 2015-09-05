#include <QApplication>

#include "window.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	Example::ChatWindow window;

	window.show();

	return app.exec();
}
