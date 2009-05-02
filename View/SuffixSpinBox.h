/**
 * Copyright (C) 2008  Ellis Whitehead
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SUFFIXSPINBOX_H
#define __SUFFIXSPINBOX_H

#include <QSpinBox>
#include <QStringList>


class SuffixSpinBox : public QSpinBox
{
public:
	SuffixSpinBox(QWidget* parent = NULL)
		: QSpinBox(parent)
	{
	}

	void setStringList(const QStringList list)
	{
		m_list = list;
	}

// Overrides for QSpinBox
protected:
	int valueFromText(const QString& s) const
	{
		int i = m_list.indexOf(s);
		return i;
	}

	QString textFromValue(int n) const
	{
		if (n >= 0 && n < m_list.count())
			return m_list[n];
		return QString();
	}

private:
	QStringList m_list;
};

#endif
