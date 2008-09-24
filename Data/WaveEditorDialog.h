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

#ifndef __WAVEEDITORDIALOG_H
#define __WAVEEDITORDIALOG_H

//#include <Widgets/GroupWidget.h>
#include <QDialog>

#include "WaveEditorWidget.h"


class ViewWaveInfo;


//class WaveEditorDialog : public GroupWidget
class WaveEditorDialog : public QDialog
{
public:
	WaveEditorDialog(ViewWaveInfo* vwi, WaveEditorFlags flags, QWidget* parent = NULL);

private:
	//WaveInfo* m_wave;
};

#endif
