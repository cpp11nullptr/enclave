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

#include "network_worker.h"

namespace Enclave
{
	NetworkWorker::NetworkWorker()
		: mBytesExpected(0), mReceiverReady(false), mEncryptionEnabled(false)
	{
	}

	NetworkWorker::~NetworkWorker()
	{
	}

	void NetworkWorker::createSocket()
	{
		mSocket.reset(new QSslSocket());

		connect(mSocket.data(), &QSslSocket::disconnected, this, &NetworkWorker::connectionClosedSignal);
		connect(mSocket.data(), &QSslSocket::readyRead, this, &NetworkWorker::readyReadSlot);

		connect(mSocket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(errorSlot(QAbstractSocket::SocketError)));

		connect(mSocket.data(), SIGNAL(sslErrors(QList<QSslError>)),
			this, SLOT(sslErrorsSlot(QList<QSslError>)));
	}

	bool NetworkWorker::isEncryptionEnabled() const
	{
		return mEncryptionEnabled;
	}

	void NetworkWorker::prepareEncryption()
	{
		if (mSocket == nullptr || !mEncryptionEnabled)
		{
			return;
		}

		if (mCaCertificates.count() > 0)
		{
			mSocket->setCaCertificates(mCaCertificates);
		}

		if (!mPrivateKey.isNull())
		{
			mSocket->setPrivateKey(mPrivateKey);
		}

		if (!mCertificate.isNull())
		{
			mSocket->setLocalCertificate(mCertificate);
		}
	}

	void NetworkWorker::sendDataSlot(const QByteArray& data)
	{
		if (mSocket->state() != QAbstractSocket::ConnectedState)
		{
			emit errorSignal(tr("Socket error: Socket hasn't connected"), false);

			return;
		}

		mSocket->write(data);

		mSocket->flush();
	}

	void NetworkWorker::closeConnectionSlot()
	{
		mSocket->close();
	}

	void NetworkWorker::setEncryptionEnabledSlot(const bool enabled)
	{
		mEncryptionEnabled = enabled;
	}

	void NetworkWorker::setCaCertificatesSlot(const QList<QSslCertificate>& certificates)
	{
		mCaCertificates = certificates;
	}

	void NetworkWorker::setEncryptionSlot(const QSslKey& privateKey, const QSslCertificate& certificate)
	{
		mPrivateKey = privateKey;
		mCertificate = certificate;
	}

	void NetworkWorker::setIgnoredSslErrorsSlot(const QList<QSslError::SslError>& errors)
	{
		mIgnoredSslErrors = errors;
	}

	void NetworkWorker::receiverReadySlot()
	{
		mReceiverReady = true;

		while (!mBufferQueue.isEmpty())
		{
			emit dataReceivedSignal(mBufferQueue.dequeue());
		}
	}

	void NetworkWorker::readyReadSlot()
	{
		while (mSocket->bytesAvailable() > 0)
		{
			if (mBytesExpected == 0)
			{
				QByteArray buffer = mSocket->read(sizeof(quint32));

				QDataStream stream(&buffer, QIODevice::ReadOnly);
				stream.setVersion(QDataStream::Qt_5_0);

				stream >> mBytesExpected;
			}

			qint64 chunkSize = mSocket->bytesAvailable();
			chunkSize = qMin<qint64>(chunkSize, mBytesExpected - mBuffer.size());

			mBuffer.append(mSocket->read(chunkSize));

			if (mBytesExpected == (quint32)mBuffer.size())
			{
				if (mReceiverReady)
				{
					emit dataReceivedSignal(mBuffer);
				}
				else
				{
					mBufferQueue.enqueue(mBuffer);
				}

				mBytesExpected = 0;
				mBuffer.clear();
			}
		}
	}

	void NetworkWorker::errorSlot(QAbstractSocket::SocketError error)
	{
		if (error == QAbstractSocket::RemoteHostClosedError)
		{
			return;
		}

		emit errorSignal(tr("Socket error: %1").arg(static_cast<int>(error)), false);
	}

	void NetworkWorker::sslErrorsSlot(QList<QSslError> errors)
	{
		bool ignored = true;

		for (const QSslError& error : errors)
		{
			bool errorIgnored = mIgnoredSslErrors.contains(error.error());

			ignored = ignored && errorIgnored;

			emit errorSignal(tr("SSL error: %1").arg(error.errorString()), errorIgnored);
		}

		if (ignored)
		{
			mSocket->ignoreSslErrors(errors);
		}
	}
}
