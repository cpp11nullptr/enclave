#ifndef EXAMPLE_CHAT_WINDOW_H
#define EXAMPLE_CHAT_WINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QWidget>
#include <QStringList>
#include <QFile>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslError>

#include "enclave/client/client.h"

#include "enclave/network/network_stream.h"
#include "enclave/network/network_connection.h"

#include "shared/command.h"
#include "shared/helper.h"

namespace Example
{
	using Enclave::NetworkStreamPointer;
	using Enclave::NetworkConnectionPointer;

	class ChatWindow
		: public QMainWindow
	{
		Q_OBJECT
		Q_DISABLE_COPY(ChatWindow)

	public:
		ChatWindow();
		~ChatWindow() Q_DECL_OVERRIDE;

	private:
		QScopedPointer<Enclave::Client> mClient;
		bool mConnectionOk;

		QString mNickname;
		QListWidget *mUsers;
		QTextEdit *mChat;
		QLineEdit *mMessage;
		QPushButton *mSendMessage;

		void setupUI();

		void chatMessage(const QString& nickname, const QString& message);
		void infoMessage(const QString& message);
		void errorMessage(const QString& message);

	private slots:
		void connectedSlot(NetworkConnectionPointer connection);
		void disconnectedSlot(NetworkConnectionPointer connection);

		void dataReceivedSlot(NetworkConnectionPointer connection, NetworkStreamPointer stream);
		void errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored);

		void sendMessageSlot();

	};
}

#endif // EXAMPLE_CHAT_WINDOW_H

