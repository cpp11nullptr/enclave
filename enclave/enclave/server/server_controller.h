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

#ifndef ENCLAVE_SERVER_CONTROLLER_H
#define ENCLAVE_SERVER_CONTROLLER_H

#include <QMetaObject>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslError>
#include <QThread>
#include <QAbstractEventDispatcher>

#include "../network/network_connection.h"

#include "server_worker.h"

namespace Enclave
{
	class ServerController
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(ServerController)

	public:
		ServerController();
		~ServerController() Q_DECL_OVERRIDE;

		QList<NetworkConnectionPointer> connections() const;

		void setEncryptionEnabled(const bool enabled);
		void setCaCertificates(const QList<QSslCertificate>& certificates);
		void setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate);
		void setIgnoredSslErrors(const QList<QSslError::SslError>& errors);

		void incomingConnection(qintptr socketDescriptor);

	private:
		QMap<ServerWorker*, NetworkConnectionPointer> mWorkersConnections;
		QScopedPointer<QThread> mThread;
		bool mEncryptionEnabled;
		QList<QSslCertificate> mCaCertificates;
		QSslKey mPrivateKey;
		QSslCertificate mCertificate;
		QList<QSslError::SslError> mIgnoredSslErrors;

		ServerWorker* createWorker() const;
		NetworkConnectionPointer createConnection(ServerWorker *worker) const;

	signals:
		void connectionEstablishedSignal(NetworkConnectionPointer connection);
		void connectionClosedSignal(NetworkConnectionPointer connection);

		void errorSignal(NetworkConnectionPointer connection, const QString& error, const bool ignored);

	private slots:
		void connectionEstablishedSlot();
		void connectionClosedSlot();

		void errorSlot(const QString& error, const bool ignored);

	};

	using ServerControllerPointer = QSharedPointer<ServerController>;
}

#endif // ENCLAVE_SERVER_CONTROLLER_H

