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

#ifndef ENCLAVE_NETWORK_STREAM_H
#define ENCLAVE_NETWORK_STREAM_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QScopedPointer>
#include <QSharedPointer>

namespace Enclave
{
	class NetworkStream
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(NetworkStream)

	public:
		enum class Type
		{
			Input,
			Output
		};

		template<typename T>
		NetworkStream(quint64 eventId, const T& command);

		explicit NetworkStream(const QByteArray& buffer);
		~NetworkStream() Q_DECL_OVERRIDE;

		template<typename T>
		QSharedPointer<NetworkStream> createResponse(const T& command) const;

		Type type() const;
		quint64 eventId() const;

		template<typename T>
		T command() const;

		template<typename T>
		T get();

		template<typename T>
		void put(const T& value);

		template<typename T, typename... Args>
		void put(const T& value, Args... args);

		QByteArray getRawData();
		void putRawData(const QByteArray& value);

		void sendData();

	private:
		Type mType;
		quint64 mEventId;
		quint64 mCommand;
		bool mDataSended;
		QByteArray mBuffer;
		QScopedPointer<QDataStream> mStream;

	signals:
		void sendResponseDataSignal(const QByteArray& data);
		void sendDataSignal(const QByteArray& data);

	};

	template<typename T>
	NetworkStream::NetworkStream(quint64 eventId, const T& command)
		: mType(Type::Output), mEventId(eventId), mCommand(static_cast<quint64>(command)),
		  mDataSended(false)
	{
		mStream.reset(new QDataStream(&mBuffer, QIODevice::WriteOnly));
		mStream->setVersion(QDataStream::Qt_5_0);

		(*mStream) << (quint32)0;
		(*mStream) << mEventId;
		(*mStream) << mCommand;
	}

	template<typename T>
	QSharedPointer<NetworkStream> NetworkStream::createResponse(const T& command) const
	{
		QSharedPointer<NetworkStream> stream = QSharedPointer<NetworkStream>(new NetworkStream(mEventId, command));

		connect(stream.data(), &NetworkStream::sendDataSignal, this, &NetworkStream::sendResponseDataSignal);

		return stream;
	}

	template<typename T>
	T NetworkStream::command() const
	{
		return static_cast<T>(mCommand);
	}

	template<typename T>
	T NetworkStream::get()
	{
		T value;
		(*mStream) >> value;

		return value;
	}

	template<typename T>
	void NetworkStream::put(const T& value)
	{
		(*mStream) << value;
	}

	template<typename T, typename... Args>
	void NetworkStream::put(const T& value, Args... args)
	{
		(*mStream) << value;

		put(args...);
	}

	using NetworkStreamPointer = QSharedPointer<NetworkStream>;
}

Q_DECLARE_METATYPE(Enclave::NetworkStreamPointer)

#endif // ENCLAVE_NETWORK_STREAM_H
