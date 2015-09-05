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

#ifndef ENCLAVE_NETWORK_WORKER_H
#define ENCLAVE_NETWORK_WORKER_H

#include <QObject>
#include <QList>
#include <QQueue>
#include <QString>
#include <QByteArray>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QAbstractSocket>
#include <QSslKey>
#include <QSslSocket>
#include <QDataStream>
#include <QThread>

namespace Enclave
{
	class NetworkWorker
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(NetworkWorker)

	public:
		NetworkWorker();
		~NetworkWorker() Q_DECL_OVERRIDE;

	protected:
		QScopedPointer<QSslSocket> mSocket;

		void createSocket();

		bool isEncryptionEnabled() const;
		void prepareEncryption();

	private:
		quint32 mBytesExpected;
		QByteArray mBuffer;
		bool mReceiverReady;
		QQueue<QByteArray> mBufferQueue;
		bool mEncryptionEnabled;
		QList<QSslCertificate> mCaCertificates;
		QSslKey mPrivateKey;
		QSslCertificate mCertificate;
		QList<QSslError::SslError> mIgnoredSslErrors;

	signals:
		void connectionEstablishedSignal();
		void connectionClosedSignal();

		void dataReceivedSignal(const QByteArray& data);
		void errorSignal(const QString& error, const bool ignored);

	public slots:
		void sendDataSlot(const QByteArray& data);
		void closeConnectionSlot();

		void setEncryptionEnabledSlot(const bool enabled);
		void setCaCertificatesSlot(const QList<QSslCertificate>& certificates);
		void setEncryptionSlot(const QSslKey& privateKey, const QSslCertificate& certificate);
		void setIgnoredSslErrorsSlot(const QList<QSslError::SslError>& errors);

		void receiverReadySlot();

	private slots:
		void readyReadSlot();
		void errorSlot(QAbstractSocket::SocketError error);
		void sslErrorsSlot(QList<QSslError> errors);

	};

	using NetworkWorkerPointer = QSharedPointer<NetworkWorker>;
}

#endif // ENCLAVE_NETWORK_WORKER_H
