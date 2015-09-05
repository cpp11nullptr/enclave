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

#include "server_controller.h"

namespace Enclave
{
	ServerController::ServerController()
		: mThread(new QThread()), mEncryptionEnabled(false)
	{
		mThread->start();
	}

	ServerController::~ServerController()
	{
		for (auto iter = mWorkersConnections.begin(); iter != mWorkersConnections.end(); ++iter)
		{
			ServerWorker *worker = iter.key();

			worker->deleteLater();
		}

		mWorkersConnections.clear();

		mThread->quit();
		mThread->wait();
	}

	QList<NetworkConnectionPointer> ServerController::connections() const
	{
		return mWorkersConnections.values();
	}

	void ServerController::setEncryptionEnabled(const bool enabled)
	{
		mEncryptionEnabled = enabled;
	}

	void ServerController::setCaCertificates(const QList<QSslCertificate>& certificates)
	{
		mCaCertificates = certificates;
	}

	void ServerController::setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate)
	{
		mPrivateKey = privateKey;
		mCertificate = certificate;
	}

	void ServerController::setIgnoredSslErrors(const QList<QSslError::SslError>& errors)
	{
		mIgnoredSslErrors = errors;
	}

	void ServerController::incomingConnection(qintptr socketDescriptor)
	{
		ServerWorker *worker = createWorker();

		mWorkersConnections[worker] = createConnection(worker);

		QMetaObject::invokeMethod(worker, "setEncryptionEnabledSlot", Qt::QueuedConnection,
			Q_ARG(bool, mEncryptionEnabled));

		QMetaObject::invokeMethod(worker, "setCaCertificatesSlot", Qt::QueuedConnection,
			Q_ARG(QList<QSslCertificate>, mCaCertificates));

		QMetaObject::invokeMethod(worker, "setEncryptionSlot", Qt::QueuedConnection,
			Q_ARG(QSslKey, mPrivateKey), Q_ARG(QSslCertificate, mCertificate));

		QMetaObject::invokeMethod(worker, "setIgnoredSslErrorsSlot", Qt::QueuedConnection,
			Q_ARG(QList<QSslError::SslError>, mIgnoredSslErrors));

		QMetaObject::invokeMethod(worker, "incomingConnectionSlot", Qt::QueuedConnection,
			Q_ARG(qintptr, socketDescriptor));
	}

	ServerWorker* ServerController::createWorker() const
	{
		ServerWorker *worker = new ServerWorker();

		worker->moveToThread(mThread.data());

		connect(worker, &ServerWorker::connectionEstablishedSignal, this, &ServerController::connectionEstablishedSlot);
		connect(worker, &ServerWorker::connectionClosedSignal, this, &ServerController::connectionClosedSlot);
		connect(worker, &ServerWorker::errorSignal, this, &ServerController::errorSlot);

		return worker;
	}

	NetworkConnectionPointer ServerController::createConnection(ServerWorker *worker) const
	{
		NetworkConnectionPointer connection = NetworkConnectionPointer(new NetworkConnection());

		connect(connection.data(), &NetworkConnection::sendDataSignal, worker, &ServerWorker::sendDataSlot);
		connect(connection.data(), &NetworkConnection::receiverReadySignal, worker, &ServerWorker::receiverReadySlot);
		connect(connection.data(), &NetworkConnection::closeConnectionSignal, worker, &ServerWorker::closeConnectionSlot);

		connect(worker, &ServerWorker::dataReceivedSignal, connection.data(), &NetworkConnection::dataReceivedSlot);

		return connection;
	}

	void ServerController::connectionEstablishedSlot()
	{
		ServerWorker *worker = qobject_cast<ServerWorker*>(sender());

		if (mWorkersConnections.contains(worker))
		{
			emit connectionEstablishedSignal(mWorkersConnections[worker]);
		}
	}

	void ServerController::connectionClosedSlot()
	{
		ServerWorker *worker = qobject_cast<ServerWorker*>(sender());

		if (mWorkersConnections.contains(worker))
		{
			NetworkConnectionPointer connection = mWorkersConnections[worker];

			mWorkersConnections.remove(worker);

			worker->deleteLater();

			emit connectionClosedSignal(connection);
		}
	}

	void ServerController::errorSlot(const QString& error, const bool ignored)
	{
		ServerWorker *worker = qobject_cast<ServerWorker*>(sender());

		if (mWorkersConnections.contains(worker))
		{
			emit errorSignal(mWorkersConnections[worker], error, ignored);
		}
	}
}
