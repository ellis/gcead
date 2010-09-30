#ifndef FILTERINFO_H
#define FILTERINFO_H

#include <QObject>

#include "EadEnums.h"


class EadFile;


class FilterInfo : public QObject
{
    Q_OBJECT

	Q_PROPERTY(FilterType type READ type CONSTANT)
	Q_PROPERTY(QString name READ name CONSTANT)

public:
	FilterInfo(EadFile *file);

	EadFile* file() const { return m_file; }

	virtual FilterType type() const = 0;
	virtual QString name() const = 0;

signals:

public slots:

private:
	EadFile* const m_file;
};

class Filter1Info : public FilterInfo
{
	Q_OBJECT

	Q_PROPERTY(double width READ width WRITE setWidth NOTIFY widthChanged)

public:
	Filter1Info(EadFile* file);

// FilterInfo overrides
public:
	FilterType type() const;
	QString name() const;

public:
	double width() const { return m_nWidth; }
	void setWidth(double nWidth);

signals:
	void widthChanged();

private:
	double m_nWidth;
};

#endif // FILTERINFO_H
