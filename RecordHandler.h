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

#ifndef __RECORDHANDLER_H
#define __RECORDHANDLER_H

#include <QVector>

#include <Idac/Idac.h>


class RecordHandler
{
public:
	RecordHandler(Idac* idac);

	const QVector<short>& digitalRaw() const { return m_anRaw[0]; }
	const QVector<short>& eadRaw() const { return m_anRaw[1]; }
	const QVector<short>& fidRaw() const { return m_anRaw[2]; }
	const QVector<double>& eadDisplay() const { return m_anDisplay[1]; }
	const QVector<double>& fidDisplay() const { return m_anDisplay[2]; }

	void updateRawToVoltageFactors();
	void calcRawToVoltageFactors(int iChan, int& nNum, int &nDen);
	bool check();
	bool convert();

private:
	Idac* m_idac;

	double m_anRawToVoltageFactors[3];
	bool m_bReportingError;
	QVector<short> m_anRaw[3];
	QVector<double> m_anDisplay[3];
};

#endif
