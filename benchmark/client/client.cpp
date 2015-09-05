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

#include "client.h"

namespace Benchmark
{
	Client::Client()
		: mClient(new Enclave::Client()), mHost("127.0.0.1"), mPort(8888), mBenchPort(8889),
		  mCurrentBenchmark(BenchmarkNone)
	{
		connect(mClient.data(), &Enclave::Client::connectedSignal, this, &Client::connectedSlot);
		connect(mClient.data(), &Enclave::Client::dataReceivedSignal, this, &Client::dataReceivedSlot);
		connect(mClient.data(), &Enclave::Client::errorSignal, this, &Client::errorSlot);

		mClient->start(mHost, mPort);
	}

	Client::~Client()
	{
		mClient->stop();
	}

	void Client::terminate(const QString& error)
	{
		qCritical() << error;

		QCoreApplication::quit();
	}

	void Client::nextBenchmark()
	{
		mCurrentBenchmark = static_cast<BenchmarkType>(static_cast<int>(mCurrentBenchmark) + 1);

		if (mCurrentBenchmark == BenchmarkMax)
		{
			mCurrentBenchmark = BenchmarkNone;
		}
	}

	void Client::executeNextBenchmark()
	{
		nextBenchmark();

		if (mCurrentBenchmark == BenchmarkNone)
		{
			mClient->connection()->createStream(Command::ByeBye)->sendData();

			QCoreApplication::quit();

			return;
		}

		mBenchmarkTitle = tr("Benchmark #%1").arg(QString::number(mCurrentBenchmark));

		NetworkResponseCallback response = createBenchmarkResponseCallback();

		NetworkStreamPointer stream = mClient->connection()->createStream(Command::BenchmarkSelect, response);

		stream->put<qint32>(mCurrentBenchmark);

		Enclave::ServerThreadPolicy threadPolicy;
		int threadCount = 0;

		selectThreadPolicy(threadPolicy, threadCount);

		stream->put<qint32>(static_cast<qint32>(threadPolicy));
		stream->put<qint32>(threadCount);
	}

	void Client::selectThreadPolicy(Enclave::ServerThreadPolicy& threadPolicy, int& threadCount)
	{
		switch (mCurrentBenchmark)
		{
		case Benchmark0001:
		case Benchmark0006:
			threadPolicy = Enclave::ServerThreadPolicy::OnePerConnection;
			threadCount = -1;
			break;

		case Benchmark0002:
		case Benchmark0007:
			threadPolicy = Enclave::ServerThreadPolicy::FixedCount;
			threadCount = 1;
			break;

		case Benchmark0003:
		case Benchmark0008:
			threadPolicy = Enclave::ServerThreadPolicy::FixedCount;
			threadCount = 10;
			break;

		case Benchmark0004:
		case Benchmark0009:
			threadPolicy = Enclave::ServerThreadPolicy::FixedCount;
			threadCount = 100;
			break;

		case Benchmark0005:
		case Benchmark0010:
			threadPolicy = Enclave::ServerThreadPolicy::FixedCount;
			threadCount = 100;
			break;

		default:
			threadPolicy = Enclave::ServerThreadPolicy::FixedCount;
			threadCount = -1;
			break;
		}
	}

	NetworkResponseCallback Client::createConsBenchmarkResponseCallback()
	{
		mBenchmarkData = QByteArray(1024 * 1024 * 10, 0);
		mBenchmarkConnectionCount = 0;
		mBenchmarkConnectionLimit = 10;

		NetworkResponseCallback response = [this](NetworkStreamPointer stream)
		{
			if (stream->command<Command>() == Command::BenchmarkReady)
			{
				qDebug() << tr("%1 is ready").arg(mBenchmarkTitle);

				mBenchmarkStarted = QDateTime::currentMSecsSinceEpoch();

				Enclave::Client *client = new Enclave::Client();

				auto connectedSlot = createBenchClientConnectedSlot(client);
				auto disconnectedSlot = createBenchClientDisconnectedSlot(client, false);
				auto errorSlot = createBenchClientErrorSlot(client);

				connect(client, &Enclave::Client::connectedSignal, connectedSlot);
				connect(client, &Enclave::Client::disconnectedSignal, disconnectedSlot);
				connect(client, &Enclave::Client::errorSignal, errorSlot);

				client->start(mHost, mBenchPort);
			}
			else
			{
				terminate(tr("Benchmark %1 hasn't been ready").arg(mBenchmarkTitle));
			}
		};

		return response;
	}

	NetworkResponseCallback Client::createSimBenchmarkResponseCallback()
	{
		mBenchmarkData = QByteArray(1024 * 1024 * 10, 0);
		mBenchmarkConnectionCount = 0;
		mBenchmarkConnectionLimit = 10;

		NetworkResponseCallback response = [this](NetworkStreamPointer stream)
		{
			if (stream->command<Command>() == Command::BenchmarkReady)
			{
				qDebug() << tr("%1 is ready").arg(mBenchmarkTitle);

				mBenchmarkStarted = QDateTime::currentMSecsSinceEpoch();

				for (int i = 0; i < mBenchmarkConnectionLimit; ++i)
				{
					Enclave::Client *client = new Enclave::Client();

					auto connectedSlot = createBenchClientConnectedSlot(client);
					auto disconnectedSlot = createBenchClientDisconnectedSlot(client, true);
					auto errorSlot = createBenchClientErrorSlot(client);

					connect(client, &Enclave::Client::connectedSignal, connectedSlot);
					connect(client, &Enclave::Client::disconnectedSignal, disconnectedSlot);
					connect(client, &Enclave::Client::errorSignal, errorSlot);

					client->start(mHost, mBenchPort);
				}
			}
			else
			{
				terminate(tr("Benchmark %1 hasn't been ready").arg(mBenchmarkTitle));
			}
		};

		return response;
	}

	NetworkResponseCallback Client::createBenchmarkResponseCallback()
	{
		NetworkResponseCallback response;

		switch (mCurrentBenchmark)
		{
		case Benchmark0001:
		case Benchmark0002:
		case Benchmark0003:
		case Benchmark0004:
		case Benchmark0005:
			response = createConsBenchmarkResponseCallback();
			break;

		case Benchmark0006:
		case Benchmark0007:
		case Benchmark0008:
		case Benchmark0009:
		case Benchmark0010:
			response = createSimBenchmarkResponseCallback();
			break;

		default:
			break;
		}

		return response;
	}

	ConnectedSlot Client::createBenchClientConnectedSlot(Enclave::Client *client)
	{
		auto connectedSlot = [this, client](NetworkConnectionPointer connection)
		{
			Q_UNUSED(connection);

			auto someDataReceived = [this, client](NetworkStreamPointer stream)
			{
				Q_UNUSED(stream);

				client->stop();
			};

			NetworkStreamPointer dataStream = connection->createStream(Command::SomeData, someDataReceived);

			dataStream->putRawData(mBenchmarkData);
		};

		return connectedSlot;
	}

	DisconnectedSlot Client::createBenchClientDisconnectedSlot(Enclave::Client *client, const bool simultaneous)
	{
		DisconnectedSlot disconnectedSlot;

		if (simultaneous)
		{
			disconnectedSlot = [this, client](NetworkConnectionPointer connection)
			{
				Q_UNUSED(connection);

				if (++mBenchmarkConnectionCount >= mBenchmarkConnectionLimit)
				{
					qDebug() << tr("%1 done").arg(mBenchmarkTitle);

					qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - mBenchmarkStarted;

					NetworkStreamPointer outStream = mClient->connection()->createStream(Command::BenchmarkDone);

					outStream->put<qreal>(1.0 * elapsed / mBenchmarkConnectionLimit);

					outStream->sendData();

					client->deleteLater();

					executeNextBenchmark();
				}
			};
		}
		else
		{
			disconnectedSlot = [this, client](NetworkConnectionPointer connection)
			{
				Q_UNUSED(connection);

				if (++mBenchmarkConnectionCount >= mBenchmarkConnectionLimit)
				{
					qDebug() << tr("%1 done").arg(mBenchmarkTitle);

					qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - mBenchmarkStarted;

					NetworkStreamPointer outStream = mClient->connection()->createStream(Command::BenchmarkDone);

					outStream->put<qreal>(1.0 * elapsed / mBenchmarkConnectionLimit);

					outStream->sendData();

					client->deleteLater();

					executeNextBenchmark();
				}
				else
				{
					client->start(mHost, mBenchPort);
				}
			};
		}

		return disconnectedSlot;
	}

	ErrorSlot Client::createBenchClientErrorSlot(Enclave::Client *client)
	{
		auto clientErrorSlot = [this, client](NetworkConnectionPointer connection, const QString& error, bool ignored)
		{
			if (!ignored)
			{
				client->stop();

				delete client;

				mClient->connection()->createStream(Command::BenchmarkDone);
			}

			QString errorFull = tr("%1 error: %2").arg(mBenchmarkTitle, error);

			errorSlot(connection, errorFull, ignored);
		};

		return clientErrorSlot;
	}

	void Client::connectedSlot(NetworkConnectionPointer connection)
	{
		Q_UNUSED(connection);

		executeNextBenchmark();
	}

	void Client::dataReceivedSlot(NetworkConnectionPointer connection, NetworkStreamPointer stream)
	{
		Q_UNUSED(connection);

		switch (stream->command<Command>())
		{
		default:
			break;
		}
	}

	void Client::errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored)
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
