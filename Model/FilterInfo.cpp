#include "FilterInfo.h"

#include "EadFile.h"


FilterInfo::FilterInfo(EadFile* file)
	: QObject(file), m_file(file)
{
}

Filter1Info::Filter1Info(EadFile* file)
	: FilterInfo(file)
{
	m_nWidth = 0;
}

FilterType Filter1Info::type() const { return FilterType_1; }
QString Filter1Info::name() const { return "Smoothing"; }

void Filter1Info::setWidth(double nWidth)
{
	if (nWidth != m_nWidth) {
		m_nWidth = nWidth;
		emit widthChanged();
	}
}
