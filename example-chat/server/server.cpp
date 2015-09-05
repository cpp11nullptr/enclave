#include "server.h"

namespace Example
{
	ChatServer::ChatServer()
		: mServer(new Enclave::Server())
	{
		mServer->setThreadPolicy(Enclave::ServerThreadPolicy::FixedCount);

		QSslCertificate caCertificate = Helper::loadCertificate("ca.crt");

		QSslKey privateKey = Helper::loadPrivateKey("server.key");
		QSslCertificate certificate = Helper::loadCertificate("server.crt");

		mServer->setEncryptionEnabled(true);
		mServer->setCaCertificates(QList<QSslCertificate>() << caCertificate);
		mServer->setEncryption(privateKey, certificate);

		if (!mServer->start(QHostAddress::Any, 7777))
		{
			qCritical() << tr("Can't start server:") << mServer->lastError();

			return;
		}

		connect(mServer.data(), &Enclave::Server::connectionClosedSignal, this, &ChatServer::connectionClosedSlot);
		connect(mServer.data(), &Enclave::Server::dataReceivedSignal, this, &ChatServer::dataReceivedSlot);
		connect(mServer.data(), &Enclave::Server::errorSignal, this, &ChatServer::errorSlot);
	}

	ChatServer::~ChatServer()
	{
		mServer->stop();
	}

	void ChatServer::userHelloCommand(NetworkConnectionPointer connection, NetworkStreamPointer stream)
	{
		QString nickname = stream->get<QString>();

		mUsersList.append(nickname);
		mUsersMap[connection] = nickname;

		NetworkStreamPointer responseStream = connection->createStream(Command::ServerHello);
		responseStream->put(nickname);
		responseStream->sendData();

		for (NetworkConnectionPointer serverConnection : mServer->connections())
		{
			if (serverConnection != connection)
			{
				NetworkStreamPointer outStream = serverConnection->createStream(Command::UserConnected);

				outStream->put(nickname);
			}
		}

		usersListChanged();
	}

	void ChatServer::userGoodByeCommand(NetworkConnectionPointer connection, NetworkStreamPointer stream)
	{
		QString nickname = stream->get<QString>();

		userDisconnected(connection, nickname);
	}

	void ChatServer::chatMessageCommand(NetworkConnectionPointer connection, NetworkStreamPointer stream)
	{
		QString nickname = stream->get<QString>();
		QString message = stream->get<QString>();

		if (message.isEmpty())
		{
			return;
		}

		if (message.left(1) == "\\")
		{
			QString command = message.mid(1);

			if (command == "quit")
			{
				connection->closeConnection();
			}
		}
		else
		{
			for (auto iter = mUsersMap.begin(); iter != mUsersMap.end(); ++iter)
			{
				NetworkConnectionPointer outConnection = iter.key();

				NetworkStreamPointer outStream = outConnection->createStream(Command::ChatMessage);

				outStream->put(nickname, message);
			}
		}
	}

	void ChatServer::userDisconnected(NetworkConnectionPointer connection, const QString& nickname)
	{
		mUsersList.removeOne(nickname);

		for (NetworkConnectionPointer serverConnection : mServer->connections())
		{
			if (serverConnection != connection)
			{
				NetworkStreamPointer outStream = serverConnection->createStream(Command::UserDisconnected);

				outStream->put(nickname);
			}
		}

		usersListChanged();
	}

	void ChatServer::usersListChanged()
	{
		for (NetworkConnectionPointer serverConnection : mServer->connections())
		{
			NetworkStreamPointer outStream = serverConnection->createStream(Command::UsersList);

			outStream->put(mUsersList);
		}
	}

	void ChatServer::connectionClosedSlot(NetworkConnectionPointer connection)
	{
		if (mUsersMap.contains(connection))
		{
			const QString& nickname = mUsersMap[connection];

			if (mUsersList.contains(nickname))
			{
				userDisconnected(connection, nickname);
			}

			mUsersMap.remove(connection);
		}
	}

	void ChatServer::dataReceivedSlot(NetworkConnectionPointer connection, NetworkStreamPointer stream)
	{
		switch (stream->command<Command>())
		{
		case Command::UserHello:
			userHelloCommand(connection, stream);
			break;

		case Command::UserGoodBye:
			userGoodByeCommand(connection, stream);
			break;

		case Command::ChatMessage:
			chatMessageCommand(connection, stream);
			break;

		default:
			break;

		}
	}

	void ChatServer::errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored)
	{
		Q_UNUSED(connection);

		if (ignored)
		{
			qCritical() << error;
		}
		else
		{
			qWarning() << error;
		}
	}
}
