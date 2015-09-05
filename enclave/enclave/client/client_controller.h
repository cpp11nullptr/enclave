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

#ifndef ENCLAVE_CLIENT_NETWORK_CONTROLLER_H
#define ENCLAVE_CLIENT_NETWORK_CONTROLLER_H

#include <QMetaObject>
#include <QString>
#include <QByteArray>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QSslCertificate>
#include <QSslError>
#include <QThread>

#include "../network/network_connection.h"

#include "client_worker.h"

namespace Enclave
{
	class ClientController
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(ClientController)

	public:
		ClientController();
		~ClientController() Q_DECL_OVERRIDE;

		NetworkConnectionPointer connection() const;

		void setEncryptionEnabled(const bool enabled);
		void setCaCertificates(const QList<QSslCertificate>& certificates);
		void setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate);
		void setIgnoredSslErrors(const QList<QSslError::SslError>& errors);

		void setConnectionOptions(const QString& hostName, const quint16 port);

		void connectToHost();
		void disconnectFromHost();

	private:
		ClientWorker *mWorker;
		NetworkConnectionPointer mConnection;
		QScopedPointer<QThread> mThread;

		ClientWorker* createWorker() const;
		NetworkConnectionPointer createConnection(ClientWorker *worker) const;

	signals:
		void connectionEstablishedSignal();
		void connectionClosedSignal();

		void errorSignal(const QString& error, const bool ignored);

	};

	using ClientControllerPointer = QSharedPointer<ClientController>;
}

#endif // ENCLAVE_CLIENT_NETWORK_CONTROLLER_H
