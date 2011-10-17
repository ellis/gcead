#ifndef __KEY_H
#define __KEY_H

#include <QString>

class Key
{
public:
	static Key empty;
	static Key parse(const QString& sKey);

public:
	const QString kind;
	const QString name;
	const QString sParent;

public:
	/*
	Key(const QString& kind, const QString& name)
		: kind(kind), parent(), name(name)
		{}

	Key(const Key& parent, const QString& kind, const QString& name)
		: kind(kind), parent(parent), name(name)
		{}

	bool isValid() const;
	*/

private:
	/*Key()
		: kind(), parent(), name()
		{}*/
	Key(
		const QString& sKey,
		const QString& sKind,
		const QString& sName,
		const QString& sParent
	);

private:
	const QString m_sKey;
	//const QString m_sKind;
	//const QString m_sName;
	//const QString m_sParent;
};

#endif
