#include "helper.h"

namespace Example
{
	QSslKey Helper::loadPrivateKey(const QString& fileName)
	{
		QFile file(fileName);

		if (!file.open(QIODevice::ReadOnly))
		{
			qCritical() << QObject::tr("Can't load private key") << fileName;

			return QSslKey();
		}

		return QSslKey(&file, QSsl::Rsa);
	}

	QSslCertificate Helper::loadCertificate(const QString& fileName)
	{
		QFile file(fileName);

		if (!file.open(QIODevice::ReadOnly))
		{
			qCritical() << QObject::tr("Can't load certificate") << fileName;

			return QSslCertificate();
		}

		return QSslCertificate(&file);
	}
}
