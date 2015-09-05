#include "server.h"

namespace Benchmark
{
	Server::Server()
		: mServer(new Enclave::Server()), mHost(QHostAddress::Any), mPort(8888), mBenchPort(8889)
	{
		connect(mServer.data(), &Enclave::Server::dataReceivedSignal, this, &Server::dataReceivedSlot);
		connect(mServer.data(), &Enclave::Server::errorSignal, this, &Server::errorSlot);

		mServer->setThreadPolicy(Enclave::ServerThreadPolicy::FixedCount, 1);

		if (!mServer->start(mHost, mPort))
		{
			terminate(tr("Can't start control server: %1").arg(mServer->lastError()));
		}
	}

	Server::~Server()
	{
		mBenchServer->stop();
		mServer->stop();
	}

	void Server::terminate(const QString& error)
	{
		qCritical() << error;

		QCoreApplication::quit();
	}

	void Server::prepareBenchmark(NetworkStreamPointer stream)
	{
		QString benchmarkTitle = tr("Benchmark #%1").arg(QString::number(stream->get<qint32>()));

		qDebug() << tr("%1 selected").arg(benchmarkTitle);

		mBenchServer.reset(new Enclave::Server());

		Enclave::ServerThreadPolicy threadPolicy = static_cast<Enclave::ServerThreadPolicy>(stream->get<qint32>());
		int threadCount = stream->get<qint32>();

		mBenchServer->setThreadPolicy(threadPolicy, threadCount);

		auto dataReceived = [this, benchmarkTitle](NetworkConnectionPointer connection, NetworkStreamPointer stream)
		{
			Q_UNUSED(connection);

			Command command = stream->command<Command>();

			if (command == Command::SomeData)
			{
				mSomeData = stream->getRawData();

				stream->createResponse(Command::SomeDataReceived);
			}
			else
			{
				terminate(tr("Unknown \"%1\" command: %2").arg(benchmarkTitle, QString::number(static_cast<int>(command))));
			}
		};

		connect(mBenchServer.data(), &Enclave::Server::dataReceivedSignal, dataReceived);

		startBenchServer(benchmarkTitle, stream);
	}

	BenchServerErrorCallback Server::createBenchServerErrorCallback(const QString& benchmarkType)
	{
		auto callback = [this, benchmarkType](NetworkConnectionPointer connection, const QString& error, bool ignored)
		{
			QString errorFull = tr("Benchmark \"%1\" error: %2").arg(benchmarkType).arg(error);

			if (!ignored)
			{
				mBenchServer->stop();
			}

			errorSlot(connection, errorFull, ignored);
		};

		return callback;
	}

	void Server::startBenchServer(const QString& benchmarkTitle, NetworkStreamPointer stream)
	{		
		connect(mBenchServer.data(), &Enclave::Server::errorSignal, createBenchServerErrorCallback(benchmarkTitle));

		if (mBenchServer->start(mHost, mBenchPort))
		{
			stream->createResponse(Command::BenchmarkReady);
		}
		else
		{
			stream->createResponse(Command::BenchmarkNotReady);

			terminate(tr("Can't start bench server: %1").arg(mBenchServer->lastError()));
		}
	}

	void Server::dataReceivedSlot(NetworkConnectionPointer connection, NetworkStreamPointer stream)
	{
		Q_UNUSED(connection);

		switch (stream->command<Command>())
		{
		case Command::BenchmarkSelect:
			{
				prepareBenchmark(stream);

				break;
			}

		case Command::BenchmarkDone:
			{
				qDebug() << tr("Benchmark result: %1").arg(QString::number(stream->get<qreal>()));

				break;
			}

		case Command::ByeBye:
			{
				QCoreApplication::quit();

				break;
			}

		default:
			break;

		}
	}

	void Server::errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored)
	{
		Q_UNUSED(connection);

		if (ignored)
		{
			qWarning() << error;
		}
		else
		{
			terminate(error);
		}
	}
}
