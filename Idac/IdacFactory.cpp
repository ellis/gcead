/**
 * Copyright (C) 2009  Ellis Whitehead
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

#include "IdacFactory.h"

#include <QtDebug>
#include <QMetaType>
#include <QMutex>
#include <QPointer>

#include <Check.h>

#include "IdacControlThread.h"
#include "IdacDriverManager.h"
#include "IdacProxy.h"


static bool g_bExited;
static IdacProxy* g_proxy;
static QPointer<IdacDriverManager> g_manager;
static IdacControlThread* g_threadControl;


IdacProxy* IdacFactory::getProxy()
{
	CHECK_PRECOND_RETVAL(!g_bExited, NULL);

	if (!g_proxy)
	{
		g_proxy = new IdacProxy(getDriverManager(true));
	}
	return g_proxy;
}

IdacDriverManager* IdacFactory::getDriverManager(bool bSeparateThread)
{
	CHECK_PRECOND_RETVAL(!g_bExited, NULL);

	if (!g_manager)
	{
		if (!bSeparateThread)
		{
			g_manager = new IdacDriverManager();
		}
		else
		{
			g_threadControl = new IdacControlThread();
			g_threadControl->start();
			while (g_threadControl->manager() == NULL)
				g_threadControl->wait(100);
			g_manager = g_threadControl->manager();
		}
	}
	return g_manager;
}

void IdacFactory::exitIdacThreads()
{
	CHECK_PRECOND_RET(!g_bExited);

	if (g_threadControl != NULL) {
		g_threadControl->exit();
		g_threadControl->wait();
		delete g_threadControl;
		g_threadControl = NULL;
	}

	if (g_manager)
	{
		delete g_manager;
	}

	g_bExited = true;
}
