/**
 * Copyright (C) 2010  Ellis Whitehead
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

#include "TestBase.h"

#include <IdacFactory.h>

#include "RecordDialog.h"
#include "WaitForHardwareDialog.h"


bool TestUi::waitForHardware(IdacProxy* idac, bool /*bCloseOnAvailable*/)
{
	if (idac == NULL)
		return false;

	WaitForHardwareDialog dlg(idac, false);
	return dlg.exec();
}

bool TestUi::showRecordPreview(IdacProxy* idac)
{
	if (idac == NULL)
		return false;

	RecordDialog dlg(idac);
	bool ret = dlg.exec();
	qDebug() << "ret:" << ret;
	return ret;
}


TestBase::TestBase(int id, bool bIdac)
    : id(id)
{
	ui = new TestUi;
	IdacProxy* idac = NULL;
	if (bIdac)
		idac = IdacFactory::getProxy();
	scope = new MainScope(ui, idac);
	sz = scope->chart()->pixmap()->sizeForAvailableArea(QSize(300, 400), 10);
	iStep = 0;
}

TestBase::~TestBase()
{
	//delete ui; // ui is deleted by MainScope
	delete scope;
}

QString TestBase::getFilename(const QString& sLabel)
{
	iStep++;
    QString sFilename = QString("%0-%1-%2.png").arg(id).arg(iStep, 3, 10, QChar('0')).arg(sLabel);
	return sFilename;
}

QImage TestBase::getImage()
{
	const ChartPixmap* cp = scope->chart()->pixmap();
	scope->chart()->draw(sz);

	QRect rc = cp->borderRect();
	rc.adjust(-1, -1, 1, 1);
	QImage image = cp->pixmap().toImage().copy(rc);
	return image;
}

QString TestBase::snap(const QString& sLabel)
{
	QString sFilename = getFilename(sLabel);
	QImage image = getImage();
	image.save(sFilename);

	return sFilename;
}

QString TestBase::snapAndContrast(const QString& sLabel, const QString& sFilenameContrast)
{
	QString sFilename = getFilename(sLabel);
	QImage image = getImage();
	image.save(sFilename);

	compare(image, sFilename, sFilenameContrast, false);

	return sFilename;
}

bool TestBase::compare(const QImage& image, const QString& sFilename, const QString& sFilenameContrast, bool bExpectEqual)
{
	QImage orig(sFilenameContrast);
	bool bEqual = (image == orig);
	if (bEqual != bExpectEqual) {
		qDebug() << "Comparison to" << sFilenameContrast << "failed.  See" << sFilename;
		return false;
	}
	return true;
}

void TestBase::compare(const QString& sLabel, const QString& sFilenameContrast, bool bExpectEqual)
{
	QString sFilename = getFilename(sLabel);
	QImage image = getImage();

	if (!compare(image, sFilename, sFilenameContrast, bExpectEqual))
		image.save(sFilename);
}
