#include <QCoreApplication>

#include "server.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	Benchmark::Server server;

	Q_UNUSED(server);

	return app.exec();
}
