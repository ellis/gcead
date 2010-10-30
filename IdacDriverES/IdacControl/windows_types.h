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

#ifndef __WINDOWS_TYPES_H
#define __WINDOWS_TYPES_H

// This is a substitute for the Windows typedefs for Linux

#include <qglobal.h>


#define DRIVER_EXPORT
#define FAR
#define WINAPI


typedef quint8 BYTE;
typedef qint16 SHORT;
typedef quint16 WORD;
typedef quint16 USHORT;
typedef qint32 LONG;
typedef quint32 DWORD;
typedef double DOUBLE;

typedef char* LPSTR;
typedef const char* LPCSTR;
typedef DWORD* LPDWORD;
typedef WORD* PWORD;
typedef LONG* LPLONG;

typedef int HANDLE;
typedef size_t SIZE;

#endif
