#include "window.h"

namespace Example
{
	ChatWindow::ChatWindow()
		: mClient(new Enclave::Client()), mConnectionOk(false)
	{
		mNickname = tr("User") + QString::number(QDateTime::currentMSecsSinceEpoch());

		setupUI();
		setWindowTitle(tr("Example::Chat"));

		QSslKey privateKey = Helper::loadPrivateKey("client.key");
		QSslCertificate certificate = Helper::loadCertificate("client.crt");

		QList<QSslError::SslError> ignoredSslErrors;

		ignoredSslErrors.push_back(QSslError::HostNameMismatch);
		ignoredSslErrors.push_back(QSslError::SelfSignedCertificateInChain);

		mClient->setEncryption(privateKey, certificate);
		mClient->setIgnoredSslErrors(ignoredSslErrors);
		mClient->setEncryptionEnabled(true);

		connect(mClient.data(), &Enclave::Client::connectedSignal, this, &ChatWindow::connectedSlot);
		connect(mClient.data(), &Enclave::Client::disconnectedSignal, this, &ChatWindow::disconnectedSlot);
		connect(mClient.data(), &Enclave::Client::dataReceivedSignal, this, &ChatWindow::dataReceivedSlot);
		connect(mClient.data(), &Enclave::Client::errorSignal, this, &ChatWindow::errorSlot);

		mClient->start("127.0.0.1", 7777);
	}

	ChatWindow::~ChatWindow()
	{
		NetworkStreamPointer stream = mClient->connection()->createStream(Command::UserGoodBye);

		stream->put(mNickname);
		stream->sendData();

		mClient->stop();
	}

	void ChatWindow::setupUI()
	{
		mUsers = new QListWidget();

		mChat = new QTextEdit();
		mChat->setReadOnly(true);

		mMessage = new QLineEdit();
		connect(mMessage, SIGNAL(returnPressed()), this, SLOT(sendMessageSlot()));

		mSendMessage = new QPushButton(tr("Send"));
		mSendMessage->setEnabled(false);
		connect(mSendMessage, SIGNAL(clicked()), this, SLOT(sendMessageSlot()));

		QHBoxLayout *messageLayout = new QHBoxLayout();
		messageLayout->addWidget(mMessage);
		messageLayout->addWidget(mSendMessage);

		QVBoxLayout *chatLayout = new QVBoxLayout();
		chatLayout->addWidget(mChat);
		chatLayout->addLayout(messageLayout);

		QWidget *chatWidget = new QWidget();
		chatWidget->setLayout(chatLayout);

		QSplitter *splitter = new QSplitter();
		splitter->addWidget(mUsers);
		splitter->addWidget(chatWidget);
		splitter->setStretchFactor(0, 1);
		splitter->setStretchFactor(1, 3);

		setCentralWidget(splitter);
	}

	void ChatWindow::chatMessage(const QString& nickname, const QString& message)
	{
		mChat->append("<i>" + nickname + ": </i>" + message);
	}

	void ChatWindow::infoMessage(const QString& message)
	{
		mChat->append("<font color=\"green\">[" + message + "]</font>");
	}

	void ChatWindow::errorMessage(const QString& message)
	{
		mChat->append("<font color=\"red\">[" + message + "]</font>");
	}

	void ChatWindow::connectedSlot(NetworkConnectionPointer connection)
	{
		NetworkStreamPointer stream = connection->createStream(Command::UserHello);

		stream->put(mNickname);
	}

	void ChatWindow::disconnectedSlot(NetworkConnectionPointer connection)
	{
		Q_UNUSED(connection);

		infoMessage(tr("Connection closed"));

		mConnectionOk = false;

		mUsers->clear();
		mSendMessage->setEnabled(false);
	}

	void ChatWindow::dataReceivedSlot(NetworkConnectionPointer connection, NetworkStreamPointer stream)
	{
		Q_UNUSED(connection);

		switch (stream->command<Command>())
		{
		case Command::ChatMessage:
			{
				QString nickname = stream->get<QString>();
				QString message = stream->get<QString>();
				chatMessage(nickname, message);

				break;
			}

		case Command::UsersList:
			{
				mUsers->clear();
				QStringList usersList = stream->get<QStringList>();
				mUsers->addItems(usersList);

				break;
			}

		case Command::UserConnected:
			{
				QString nickname = stream->get<QString>();
				infoMessage(tr("%1 join chat").arg(nickname));

				break;
			}

		case Command::UserDisconnected:
			{
				QString nickname = stream->get<QString>();
				infoMessage(tr("%1 left chat").arg(nickname));

				break;
			}

		case Command::ServerHello:
			{
				mSendMessage->setEnabled(true);
				mConnectionOk = true;

				break;
			}

		default:
			break;

		}
	}

	void ChatWindow::errorSlot(NetworkConnectionPointer connection, const QString& error, const bool ignored)
	{
		Q_UNUSED(connection);

		if (ignored)
		{
			infoMessage(error);
		}
		else
		{
			errorMessage(error);
		}
	}

	void ChatWindow::sendMessageSlot()
	{
		if (mMessage->text().isEmpty() || !mConnectionOk)
		{
			return;
		}

		NetworkStreamPointer stream = mClient->connection()->createStream(Command::ChatMessage);

		stream->put(mNickname);
		stream->put(mMessage->text());

		mMessage->clear();
	}
}
