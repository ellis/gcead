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

#include "Idac.h"


Idac::Idac(QObject* parent)
	: QObject(parent)
{
	m_state = IdacState_NotAvailable;
}

void Idac::setRanges(const QList<int>& ranges)
{
	m_anRanges = ranges;
}

void Idac::setLowpassStrings(const QStringList& strings)
{
	m_asLowpassStrings = strings;
}

void Idac::setHighpassStrings(const QStringList& strings)
{
	m_asHighpassStrings = strings;
}

/*
			QMessageBox::StandardButton btn = QMessageBox::question(
				NULL,
				tr("Retry?"),
				tr(IDS_BOOT_RETRY1).arg(sType),
				QMessageBox::Retry | QMessageBox::Abort);
			
			if (btn == QMessageBox::Abort)
				bAbort = true;

				
				
				QMessageBox::critical(
					NULL,
					tr("File Not Found"),
					tr("File not found: %0").arg(sFilename),
					QMessageBox::Ok,
					QMessageBox::Ok);
				
*/
