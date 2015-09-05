#ifndef BENCHMARK_SERVER_H
#define BENCHMARK_SERVER_H

#include <QCoreApplication>
#include <QSharedPointer>
#include <QList>
#include <QDateTime>
#include <functional>

#include "enclave/server/server.h"

#include "enclave/network/network_stream.h"
#include "enclave/network/network_connection.h"

#include "shared/command.h"

namespace Benchmark
{
	using Enclave::NetworkStreamPointer;
	using Enclave::NetworkConnectionPointer;
	using Enclave::NetworkResponseCallback;

	using BenchServerErrorCallback = std::function<void(NetworkConnectionPointer,const QString&,bool)>;

	class Server
		: public QObject
	{
		Q_OBJECT

	public:
		Server();
		~Server() override;

	private:
		QScopedPointer<Enclave::Server> mServer;
		QScopedPointer<Enclave::Server> mBenchServer;
		QHostAddress mHost;
		int mPort;
		int mBenchPort;

		void terminate(const QString& error);

		QByteArray mSomeData;

		void prepareBenchmark(NetworkStreamPointer stream);

		BenchServerErrorCallback createBenchServerErrorCallback(const QString& benchmarkType);

		void startBenchServer(const QString& benchmarkTitle, NetworkStreamPointer stream);

	private slots:
		void dataReceivedSlot(NetworkConnectionPointer connection, NetworkStreamPointer stream);
		void errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored);

	};
}

#endif // BENCHMARK_SERVER_H

