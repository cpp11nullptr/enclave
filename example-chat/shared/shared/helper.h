#ifndef EXAMPLE_CHAT_SHARED_HELPER_H
#define EXAMPLE_CHAT_SHARED_HELPER_H

#include <QSslKey>
#include <QSslCertificate>
#include <QFile>
#include <QDebug>

namespace Example
{
	class Helper
	{
	public:
		static QSslKey loadPrivateKey(const QString& fileName);
		static QSslCertificate loadCertificate(const QString& fileName);

	};
}

#endif // EXAMPLE_CHAT_SHARED_HELPER_H

