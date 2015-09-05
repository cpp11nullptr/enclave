#ifndef EXAMPLE_CHAT_SHARED_COMMAND_H
#define EXAMPLE_CHAT_SHARED_COMMAND_H

namespace Example
{
	enum class Command
	{
		UserHello,				// "Hello, server!"
		UserGoodBye,			// "Bye, bye, server!"

		ServerHello,			// "Hello, user!"

		UserConnected,			// user connected
		UserDisconnected,		// user disconnected
		UsersList,				// users list has changed
		ChatMessage				// chat message
	};
}

#endif // EXAMPLE_CHAT_SHARED_COMMAND_H

