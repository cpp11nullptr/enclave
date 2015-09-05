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

namespace Enclave
{
	Client::Client()
		: mStarted(false)
	{
		qRegisterMetaType<QSslKey>("QSslKey");
		qRegisterMetaType<QSslCertificate>("QSslCertificate");
		qRegisterMetaType<QList<QSslCertificate>>("QList<QSslCertificate>");
		qRegisterMetaType<QList<QSslError::SslError>>("QList<QSslError::SslError>");
		qRegisterMetaType<NetworkConnectionPointer>("NetworkConnectionPointer");

		mController = ClientControllerPointer(new ClientController());

		connect(mController.data(), &ClientController::connectionEstablishedSignal, this, &Client::connectedSlot);
		connect(mController.data(), &ClientController::connectionClosedSignal, this, &Client::disconnectedSlot);
		connect(mController.data(), &ClientController::errorSignal, this, &Client::errorSlot);

		connect(mController->connection().data(), &NetworkConnection::dataReceivedSignal, this, &Client::dataReceivedSlot);
	}

	Client::~Client()
	{
	}

	bool Client::setEncryptionEnabled(const bool enabled)
	{
		if (mStarted)
		{
			return false;
		}

		mController->setEncryptionEnabled(enabled);

		return true;
	}

	bool Client::setCaCertificates(const QList<QSslCertificate>& certificates)
	{
		if (mStarted)
		{
			return false;
		}

		mController->setCaCertificates(certificates);

		return true;
	}

	bool Client::setEncryption(const QSslKey& privateKey, const QSslCertificate& certificate)
	{
		if (mStarted)
		{
			return false;
		}

		mController->setEncryption(privateKey, certificate);

		return true;
	}

	bool Client::setIgnoredSslErrors(const QList<QSslError::SslError>& errors)
	{
		if (mStarted)
		{
			return false;
		}

		mController->setIgnoredSslErrors(errors);

		return true;
	}

	NetworkConnectionPointer Client::connection() const
	{
		return mController->connection();
	}

	void Client::start(const QString& hostName, const quint16 port)
	{
		mController->setConnectionOptions(hostName, port);

		mController->connectToHost();
	}

	void Client::stop()
	{
		mController->disconnectFromHost();
	}

	bool Client::isStarted() const
	{
		return mStarted;
	}

	void Client::connectedSlot()
	{
		mStarted = true;

		emit connectedSignal(mController->connection());

		mController->connection()->receiverReady();
	}

	void Client::disconnectedSlot()
	{
		mStarted = false;

		emit disconnectedSignal(mController->connection());
	}

	void Client::dataReceivedSlot(NetworkStreamPointer stream)
	{
		emit dataReceivedSignal(mController->connection(), stream);
	}

	void Client::errorSlot(const QString& error, const bool ignored)
	{
		emit errorSignal(mController->connection(), error, ignored);
	}
}
