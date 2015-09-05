#include <QCoreApplication>

#include "server.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	Example::ChatServer server;

	Q_UNUSED(server);

	return app.exec();
}
