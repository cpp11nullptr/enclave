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

#include "network_stream.h"

namespace Enclave
{
	NetworkStream::NetworkStream(const QByteArray &buffer)
		: mType(Type::Input), mBuffer(buffer)
	{
		mStream.reset(new QDataStream(&mBuffer, QIODevice::ReadOnly));

		mStream->setVersion(QDataStream::Qt_5_0);

		(*mStream) >> mEventId;
		(*mStream) >> mCommand;
	}

	NetworkStream::~NetworkStream()
	{
		sendData();
	}

	NetworkStream::Type NetworkStream::type() const
	{
		return mType;
	}

	quint64 NetworkStream::eventId() const
	{
		return mEventId;
	}

	QByteArray NetworkStream::getRawData()
	{
		qint32 size = 0;
		(*mStream) >> size;

		char *data = new char[size];
		(*mStream).readRawData(data, size);

		QByteArray dataArray(data, size);

		delete[] data;

		return dataArray;
	}

	void NetworkStream::putRawData(const QByteArray& value)
	{
		(*mStream) << value.size();

		(*mStream).writeRawData(value.data(), value.size());
	}

	void NetworkStream::sendData()
	{
		if (mType != Type::Output || mDataSended)
		{
			return;
		}

		mStream->device()->seek(0);

		(*mStream) << (quint32)(mBuffer.size() - sizeof(quint32));

		emit sendDataSignal(mBuffer);

		mDataSended = true;
	}
}
