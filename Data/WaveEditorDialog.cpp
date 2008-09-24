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

#include "WaveEditorDialog.h"

#include <QGridLayout>
#include <QPushButton>

#include <Data/ViewInfo.h>
#include <Data/WaveInfo.h>
#include <Data/WaveEditorWidget.h>


WaveEditorDialog::WaveEditorDialog(ViewWaveInfo* vwi, WaveEditorFlags flags, QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(vwi->wave()->sName);

	int iRow;

	WaveEditorWidget* editor = new WaveEditorWidget();
	editor->setWave(vwi, flags);

	QPushButton* btn = new QPushButton(tr("Close"));
	btn->setDefault(true);
	connect(btn, SIGNAL(clicked()), this, SLOT(accept()));

	QGridLayout* grid = new QGridLayout();
	iRow = 0;
	grid->addWidget(editor, iRow++, 0, 1, 2);
	grid->addWidget(btn, iRow, 1);
	setLayout(grid);
}
