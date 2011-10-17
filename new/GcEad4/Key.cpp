#include "Key.h"

#include "Check.h"


Key Key::empty = Key(QString(), QString(), QString(), QString());


Key Key::parse(const QString& sKey) {
	const int iColonLast = sKey.lastIndexOf(':');
	CHECK_ASSERT_RETVAL(iColonLast != 0, Key::empty);

	const QString sParent = (iColonLast > 0) ? sKey.left(iColonLast) : QString();
	const QString sSelf = sKey.mid(iColonLast + 1);
	const int iEqual = sKey.indexOf('=');
	CHECK_ASSERT_RETVAL(iEqual > 0, Key::empty);

	const QString sKind = sSelf.left(iEqual);
	const QString sName = sSelf.mid(iEqual + 1);
	CHECK_ASSERT_RETVAL(!sName.isEmpty(), Key::empty);

	Key key(sKey, sKind, sName, sParent);
	return key;
}

Key::Key(
	const QString& sKey,
	const QString& sKind,
	const QString& sName,
	const QString& sParent
)
	: kind(sKind), name(sName), sParent(sParent), m_sKey(sKey)
{
}
