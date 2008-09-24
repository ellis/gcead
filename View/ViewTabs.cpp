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

#include "ViewTabs.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include <Check.h>

#include "Globals.h"
#include "MainScope.h"
#include "Utils.h"


ViewTabs::ViewTabs(MainScope* scope, QWidget* parent)
	: TabWidget(parent)
{
	m_scope = scope;

	addTab(tr("Averages"), Qt::white);
	addTab(tr("EADs"), Qt::white);
	addTab(tr("FIDs"), Qt::white);
	addTab(tr("All"), Qt::white);
	addTab(tr("Recording"), Qt::white);

	connect(this, SIGNAL(tabClicked(int)), this, SLOT(on_tabClicked(int)));
	connect(m_scope, SIGNAL(viewTypeChanged(EadView)), this, SLOT(on_scope_viewTypeChanged(EadView)));
}

void ViewTabs::on_tabClicked(int i)
{
	CHECK_PARAM_RET(i >= 0 && i < EadViewCount);

	m_scope->setViewType((EadView) i);
}

void ViewTabs::on_scope_viewTypeChanged(EadView viewType)
{
	setCurrentIndex((int) viewType);
}
