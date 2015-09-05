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

#ifndef ENCLAVE_CLIENT_H
#define ENCLAVE_CLIENT_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QSslError>

#include "client_controller.h"

#include "../network/network_connection.h"

namespace Enclave
{
	class Client
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(Client)

	public:
		Client();
		~Client() Q_DECL_OVERRIDE;

		bool setEncryptionEnabled(const bool enabled);
		bool setCaCertificates(const QList<QSslCertificate>& certificates);
		bool setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate);
		bool setIgnoredSslErrors(const QList<QSslError::SslError>& errors);

		NetworkConnectionPointer connection() const;

		void start(const QString& hostName, const quint16 port);
		void stop();

		bool isStarted() const;

	private:		
		bool mStarted;
		ClientControllerPointer mController;

	signals:
		void connectedSignal(NetworkConnectionPointer connection);
		void disconnectedSignal(NetworkConnectionPointer connection);

		void dataReceivedSignal(NetworkConnectionPointer connection, NetworkStreamPointer stream);
		void errorSignal(NetworkConnectionPointer connection, const QString& error, const bool ignored);

	private slots:
		void connectedSlot();
		void disconnectedSlot();

		void dataReceivedSlot(NetworkStreamPointer stream);
		void errorSlot(const QString& error, const bool ignored);

	};
}

#endif // ENCLAVE_CLIENT_H

