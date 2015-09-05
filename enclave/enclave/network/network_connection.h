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

#ifndef ENCLAVE_NETWORK_CONNECTION_H
#define ENCLAVE_NETWORK_CONNECTION_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QByteArray>
#include <QSharedPointer>
#include <functional>

#include "network_stream.h"

namespace Enclave
{
	using NetworkResponseCallback = std::function<void(NetworkStreamPointer)>;

	class NetworkConnection
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(NetworkConnection)

	public:
		NetworkConnection();
		~NetworkConnection() Q_DECL_OVERRIDE;

		template<typename T>
		NetworkStreamPointer createStream(const T& command);

		template<typename T>
		NetworkStreamPointer createStream(const T& command, NetworkResponseCallback responseCallback);

		void receiverReady();
		void closeConnection();

	private:
		quint64 mEventIdCounter;
		QMap<quint64, NetworkResponseCallback> mResponseCallbacks;

	signals:
		void sendDataSignal(const QByteArray& data);
		void dataReceivedSignal(NetworkStreamPointer stream);

		void receiverReadySignal();
		void closeConnectionSignal();

	public slots:
		void dataReceivedSlot(const QByteArray& data);

	};

	template<typename T>
	NetworkStreamPointer NetworkConnection::createStream(const T& command)
	{
		NetworkStreamPointer stream = NetworkStreamPointer(new NetworkStream(mEventIdCounter++, command));

		connect(stream.data(), SIGNAL(sendDataSignal(QByteArray)),
			this, SIGNAL(sendDataSignal(QByteArray)));

		return stream;
	}

	template<typename T>
	NetworkStreamPointer NetworkConnection::createStream(const T& command, NetworkResponseCallback responseCallback)
	{
		NetworkStreamPointer stream = NetworkStreamPointer(new NetworkStream(mEventIdCounter++, command));

		connect(stream.data(), SIGNAL(sendDataSignal(QByteArray)),
			this, SIGNAL(sendDataSignal(QByteArray)));

		mResponseCallbacks[stream->eventId()] = responseCallback;

		return stream;
	}

	using NetworkConnectionPointer = QSharedPointer<NetworkConnection>;
}

Q_DECLARE_METATYPE(Enclave::NetworkConnectionPointer)

#endif // ENCLAVE_NETWORK_CONNECTION_H

