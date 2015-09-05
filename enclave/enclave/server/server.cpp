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

#include "server.h"

namespace Enclave
{
	Server::Server()
		: mThreadPolicy(ServerThreadPolicy::FixedCount), mThreadCount(-1),
		  mEncryptionEnabled(false), mControllerIndex(-1)
	{
		qRegisterMetaType<qintptr>("qintptr");
		qRegisterMetaType<QSslKey>("QSslKey");
		qRegisterMetaType<QSslCertificate>("QSslCertificate");
		qRegisterMetaType<QList<QSslCertificate>>("QList<QSslCertificate>");
		qRegisterMetaType<QList<QSslError::SslError>>("QList<QSslError::SslError>");
		qRegisterMetaType<NetworkConnectionPointer>("NetworkConnectionPointer");
	}

	Server::~Server()
	{
	}

	bool Server::setEncryptionEnabled(const bool enabled)
	{
		if (isListening())
		{
			return false;
		}

		mEncryptionEnabled = enabled;

		return true;
	}

	bool Server::setCaCertificates(const QList<QSslCertificate>& certificates)
	{
		if (isListening())
		{
			return false;
		}

		mCaCertificates = certificates;

		return true;
	}

	bool Server::setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate)
	{
		if (isListening())
		{
			return false;
		}

		mPrivateKey = privateKey;
		mCertificate = certificate;

		return true;
	}

	bool Server::setIgnoredSslErrors(const QList<QSslError::SslError>& errors)
	{
		if (isListening())
		{
			return false;
		}

		mIgnoredSslErrors = errors;

		return true;
	}

	bool Server::start(const QHostAddress& address, const quint16 port)
	{
		if (isListening())
		{
			return true;
		}

		initControllers();

		return listen(address, port);
	}

	void Server::stop()
	{
		if (!isListening())
		{
			return;
		}

		mControllers.clear();

		close();
	}

	bool Server::isStarted() const
	{
		return isListening();
	}

	QString Server::lastError() const
	{
		return errorString();
	}

	ServerThreadPolicy Server::threadPolicy() const
	{
		return mThreadPolicy;
	}

	void Server::setThreadPolicy(ServerThreadPolicy policy, int threadCount)
	{
		if (isListening())
		{
			return;
		}

		mThreadPolicy = policy;
		mThreadCount = threadCount;
	}

	QList<NetworkConnectionPointer> Server::connections() const
	{
		QList<NetworkConnectionPointer> connections;

		for (const ServerControllerPointer& controller : mControllers)
		{
			connections += controller->connections();
		}

		return connections;
	}

	void Server::incomingConnection(qintptr socketDescriptor)
	{
		ServerControllerPointer controller;

		if (mThreadPolicy == ServerThreadPolicy::FixedCount)
		{
			mControllerIndex = (mControllerIndex + 1) % mControllers.count();

			controller = mControllers.at(mControllerIndex);
		}
		else
		{
			controller = createController();

			mControllers.push_back(controller);
		}

		controller->incomingConnection(socketDescriptor);
	}

	void Server::initControllers()
	{
		if (mThreadPolicy != ServerThreadPolicy::FixedCount)
		{
			return;
		}

		int threadCount = mThreadCount;

		if (threadCount <= 0)
		{
			threadCount = QThread::idealThreadCount();
		}

		if (threadCount == -1)
		{
			threadCount = 2;
		}

		for (int i = 0; i < threadCount; ++i)
		{
			ServerControllerPointer controller = createController();

			mControllers.push_back(controller);
		}
	}

	ServerControllerPointer Server::createController()
	{
		ServerControllerPointer controller = ServerControllerPointer(new ServerController());

		connect(controller.data(), &ServerController::connectionEstablishedSignal, this, &Server::connectionEstablishedSlot);
		connect(controller.data(), &ServerController::connectionClosedSignal, this, &Server::connectionClosedSlot);
		connect(controller.data(), &ServerController::errorSignal, this, &Server::errorSlot);

		controller->setEncryptionEnabled(mEncryptionEnabled);
		controller->setCaCertificates(mCaCertificates);
		controller->setEncryption(mPrivateKey, mCertificate);
		controller->setIgnoredSslErrors(mIgnoredSslErrors);

		return controller;
	}

	void Server::connectionEstablishedSlot(NetworkConnectionPointer connection)
	{
		mConnectionsMap[connection.data()] = connection;

		connect(connection.data(), &NetworkConnection::dataReceivedSignal, this, &Server::dataReceivedSlot);

		emit connectionEstablishedSignal(connection);

		connection->receiverReady();
	}

	void Server::connectionClosedSlot(NetworkConnectionPointer connection)
	{
		emit connectionClosedSignal(connection);

		mConnectionsMap.remove(connection.data());

		if (mThreadPolicy == ServerThreadPolicy::OnePerConnection)
		{
			ServerController *senderController = qobject_cast<ServerController*>(sender());

			for (const ServerControllerPointer& controller : mControllers)
			{
				if (controller.data() == senderController)
				{
					mControllers.removeOne(controller);

					break;
				}
			}
		}
	}

	void Server::dataReceivedSlot(NetworkStreamPointer stream)
	{
		Q_UNUSED(stream);

		NetworkConnection *connection = qobject_cast<NetworkConnection*>(sender());

		if (mConnectionsMap.contains(connection))
		{
			emit dataReceivedSignal(mConnectionsMap[connection], stream);
		}
	}

	void Server::errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored)
	{
		emit errorSignal(connection, error, ignored);
	}
}
