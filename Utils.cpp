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

#include "Utils.h"


QString timestampString(double nSeconds, TimestampBase base)
{
	double n = nSeconds;
	QString sSuffix;

	switch (base)
	{
	case TimestampBase_Milli:
		n *= 1000;
		sSuffix = "ms";
		break;
	case TimestampBase_Seconds:
		sSuffix = "s";
		break;
	case TimestampBase_Minutes:
		n /= 60;
		sSuffix = "min";
		break;
	}

	QString s = QString("%1 %2").arg(n, 0, 'f', 1).arg(sSuffix);
	return s;
}

QString timestampString(double nSeconds, TimestampBase* pbase)
{
	TimestampBase base;
	if (nSeconds < 1)
		base = TimestampBase_Milli;
	else if (nSeconds < 60)
		base = TimestampBase_Seconds;
	else
		base = TimestampBase_Minutes;

	if (pbase != NULL)
		*pbase = base;

	return timestampString(nSeconds, base);
}

QPainterPath roundedRect(const QRect& rc, int nRadius)
{
	QPainterPath path;

	// Diameter
	qreal d = nRadius * 2 + 1;

	if (rc.width() < d || rc.height() < d)
		return path;
	
	qreal x0 = rc.left() + 0.5;
	qreal x3 = rc.right() - 1 + 0.5;
	qreal y0 = rc.top() + 0.5;
	qreal y3 = rc.bottom() - 1 + 0.5;

	QRectF rcTR(x3 - d, y0, d, d);
	QRectF rcBR(x3 - d, y3 - d, d, d);
	QRectF rcBL(x0, y3 - d, d, d);
	QRectF rcTL(x0, y0, d, d);

	path.moveTo(rcTR.topLeft());
	path.arcTo(rcTR, 90, -90);
	path.arcTo(rcBR, 0, -90);
	path.arcTo(rcBL, -90, -90);
	path.arcTo(rcTL, -180, -90);
	path.closeSubpath();

	return path;
}
