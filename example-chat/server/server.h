#ifndef EXAMPLE_CHAT_SERVER_H
#define EXAMPLE_CHAT_SERVER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QScopedPointer>
#include <QFile>
#include <QSslKey>
#include <QSslCertificate>

#include "enclave/server/server.h"

#include "enclave/network/network_stream.h"
#include "enclave/network/network_connection.h"

#include "shared/command.h"
#include "shared/helper.h"

namespace Example
{
	using Enclave::NetworkStreamPointer;
	using Enclave::NetworkConnectionPointer;

	class ChatServer
		: public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(ChatServer)

	public:
		ChatServer();
		~ChatServer() Q_DECL_OVERRIDE;

	private:
		QScopedPointer<Enclave::Server> mServer;
		QList<QString> mUsersList;
		QMap<NetworkConnectionPointer, QString> mUsersMap;

		void userHelloCommand(NetworkConnectionPointer connection, NetworkStreamPointer stream);
		void userGoodByeCommand(NetworkConnectionPointer connection, NetworkStreamPointer stream);
		void chatMessageCommand(NetworkConnectionPointer connection, NetworkStreamPointer stream);

		void usersListChanged();
		void userDisconnected(NetworkConnectionPointer connection, const QString& nickname);

	private slots:
		void connectionClosedSlot(NetworkConnectionPointer connection);

		void dataReceivedSlot(NetworkConnectionPointer connection, NetworkStreamPointer stream);
		void errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored);

	};
}

#endif // EXAMPLE_CHAT_SERVER_H
