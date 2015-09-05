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

#ifndef ENCLAVE_SERVER_H
#define ENCLAVE_SERVER_H

#include <QList>
#include <QTcpServer>
#include <QSharedPointer>
#include <QSslKey>
#include <QSslCertificate>

#include "../network/network_connection.h"

#include "server_controller.h"

namespace Enclave
{
	enum class ServerThreadPolicy
	{
		OnePerConnection,
		FixedCount
	};

	class Server
		: public QTcpServer
	{
		Q_OBJECT
		Q_DISABLE_COPY(Server)

	public:
		Server();
		~Server() Q_DECL_OVERRIDE;

		bool setEncryptionEnabled(const bool enabled);
		bool setCaCertificates(const QList<QSslCertificate>& certificates);
		bool setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate);
		bool setIgnoredSslErrors(const QList<QSslError::SslError>& errors);

		bool start(const QHostAddress& address, const quint16 port);
		void stop();

		bool isStarted() const;
		QString lastError() const;

		ServerThreadPolicy threadPolicy() const;
		void setThreadPolicy(ServerThreadPolicy policy, int threadCount = -1);

		QList<NetworkConnectionPointer> connections() const;

	protected:
		void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

	private:
		ServerThreadPolicy mThreadPolicy;
		int mThreadCount;
		bool mEncryptionEnabled;
		QList<QSslCertificate> mCaCertificates;
		QSslKey mPrivateKey;
		QSslCertificate mCertificate;
		QList<QSslError::SslError> mIgnoredSslErrors;
		int mControllerIndex;
		QList<ServerControllerPointer> mControllers;
		QMap<NetworkConnection*, NetworkConnectionPointer> mConnectionsMap;

		void initControllers();
		ServerControllerPointer createController();

	signals:
		void connectionEstablishedSignal(NetworkConnectionPointer connection);
		void connectionClosedSignal(NetworkConnectionPointer connection);

		void dataReceivedSignal(NetworkConnectionPointer connection, NetworkStreamPointer stream);
		void errorSignal(NetworkConnectionPointer connection, const QString& error, const bool ignored);

	private slots:
		void connectionEstablishedSlot(NetworkConnectionPointer connection);
		void connectionClosedSlot(NetworkConnectionPointer connection);

		void dataReceivedSlot(NetworkStreamPointer stream);
		void errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored);

	};
}

#endif // ENCLAVE_SERVER_H

