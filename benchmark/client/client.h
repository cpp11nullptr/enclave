/*******************************************************************************

The MIT License (MIT)

Copyright (c) 2015 Ievgen Polyvanyi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*******************************************************************************/

#ifndef BENCHMARK_CLIENT_H
#define BENCHMARK_CLIENT_H

#include <QCoreApplication>

#include <functional>

#include "enclave/client/client.h"
#include "enclave/server/server.h"

#include "enclave/network/network_stream.h"
#include "enclave/network/network_connection.h"

#include "shared/command.h"

namespace Benchmark
{
	using Enclave::NetworkStreamPointer;
	using Enclave::NetworkConnectionPointer;
	using Enclave::NetworkResponseCallback;

	using ConnectedSlot = std::function<void(NetworkConnectionPointer)>;
	using DisconnectedSlot = std::function<void(NetworkConnectionPointer)>;
	using ErrorSlot = std::function<void(NetworkConnectionPointer connection, const QString& error, bool ignored)>;

	class Client
		: public QObject
	{
		Q_OBJECT

	public:
		Client();
		~Client() override;

	private:		
		QScopedPointer<Enclave::Client> mClient;
		QString mHost;
		int mPort;
		int mBenchPort;

		void terminate(const QString& error);

		QString mBenchmarkTitle;
		mutable QByteArray mBenchmarkData;
		mutable int mBenchmarkConnectionLimit;
		mutable int mBenchmarkConnectionCount;
		mutable qint64 mBenchmarkStarted;

		BenchmarkType mCurrentBenchmark;

		void nextBenchmark();
		void executeNextBenchmark();

		void selectThreadPolicy(Enclave::ServerThreadPolicy& threadPolicy, int& threadCount);

		NetworkResponseCallback createConsBenchmarkResponseCallback();
		NetworkResponseCallback createSimBenchmarkResponseCallback();

		NetworkResponseCallback createBenchmarkResponseCallback();

		ConnectedSlot createBenchClientConnectedSlot(Enclave::Client *client);
		DisconnectedSlot createBenchClientDisconnectedSlot(Enclave::Client *client, const bool simultaneous);
		ErrorSlot createBenchClientErrorSlot(Enclave::Client *client);

	private slots:
		void connectedSlot(NetworkConnectionPointer connection);

		void dataReceivedSlot(NetworkConnectionPointer connection, NetworkStreamPointer stream);
		void errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored);

	};
}

#endif // BENCHMARK_CLIENT_H

