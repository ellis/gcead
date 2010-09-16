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

#include "PanelTabs.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "Globals.h"
#include "MainScope.h"
#include "Utils.h"


PanelTabs::PanelTabs(MainScope* scope, QWidget* parent)
	: TabWidget(parent)
{
	m_scope = scope;

	addTab(tr("View"), QColor(0xe0, 0xe0, 0xff));
	addTab(tr("Markers"), QColor(0xe0, 0xe0, 0xff));
	addTab(tr("Publish"), QColor(0xe0, 0xe0, 0xff));
	//addTab(tr("Publish"), QColor(0xe0, 0xff, 0xe0));

	connect(this, SIGNAL(tabClicked(int)), this, SLOT(on_tabClicked(int)));
	connect(m_scope, SIGNAL(taskTypeChanged(EadTask)), this, SLOT(on_scope_taskTypeChanged(EadTask)));
}

void PanelTabs::on_tabClicked(int i)
{
	m_scope->setTaskType((EadTask) i);
}

void PanelTabs::on_scope_taskTypeChanged(EadTask taskType)
{
	setCurrentIndex((int) taskType);
}
