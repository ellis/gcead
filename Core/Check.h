/**
 * Copyright (C) 2008,2010  Ellis Whitehead
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

#ifndef __CHECK_H
#define __CHECK_H

class QString;

/// checkFailure() is called by the CHECK_* macros when a check fails
/// This function will need to be implemented somewhere in the application code
/// in order to handle a check failure sensibly.
extern void checkFailure(const char* sFile, int iLine, const char* s);

extern void checkLog(const char* sFile, int iLine, const QString& sType, const QString& sMessage);



// Only define checkFailure()
//#ifdef QT_NO_DEBUG
#if 0
	#define __CHECK_COND_RETVAL(x, ret) \
		{ if ((x) == false) { return (ret); } }
	#define __CHECK_COND_RET(x) \
		{ if ((x) == false) { return; } }
	#define __CHECK_COND_NORET(x) \
		{ }

	/// Check a precondition, and return the given value if the check fails
	#define CHECK_PRECOND_RETVAL(x, ret) __CHECK_COND_RETVAL(x, ret)

	/// Check a precondition, and return if the check fails
	#define CHECK_PRECOND_RET(x) __CHECK_COND_RET(x)

	/// Check a parameter for validity, and return the given value if the check fails
	#define CHECK_PARAM_RETVAL(x, ret) __CHECK_COND_RETVAL(x, ret)

	/// Check a parameter for validity, and return if the check fails
	#define CHECK_PARAM_RET(x) __CHECK_COND_RET(x)

	/// Check a condition for validity, and return the given value if the check fails
	#define CHECK_ASSERT_RETVAL(x, ret) __CHECK_COND_RETVAL(x, ret)

	/// Check a condition for validity, and return if the check fails
	#define CHECK_ASSERT_RET(x) __CHECK_COND_RET(x)

	/// Check a condition for validity, and just print debug info if the check fails
	#define CHECK_ASSERT_NORET(x) __CHECK_COND_NORET(x)

	#define CHECK_FAILURE_RETVAL(ret) \
		{ return (ret); }

	#define CHECK_FAILURE_RET() \
		{ return; }
#else
	#define LOG(nDebugLevel, s) checkLog(__FILE__, __LINE__, "LOG", s)

	#define __CHECK_COND_RETVAL(x, ret) \
		{ if ((x) == false) { checkFailure(__FILE__, __LINE__, #x); return ret; } }
	#define __CHECK_COND_RET(x) \
		{ if ((x) == false) { checkFailure(__FILE__, __LINE__, #x); return; } }
	#define __CHECK_COND_NORET(x) \
		{ if ((x) == false) { checkFailure(__FILE__, __LINE__, #x); } }

	/// Check a precondition, and return the given value if the check fails
	#define CHECK_PRECOND_RETVAL(x, ret) __CHECK_COND_RETVAL(x, ret)

	/// Check a precondition, and return if the check fails
	#define CHECK_PRECOND_RET(x) __CHECK_COND_RET(x)

	/// Check a precondition and continue without returning
	#define CHECK_PRECOND_NORET(x) __CHECK_COND_NORET(x)

	/// Check a parameter for validity, and return the given value if the check fails
	#define CHECK_PARAM_RETVAL(x, ret) __CHECK_COND_RETVAL(x, ret)

	/// Check a parameter for validity, and return if the check fails
	#define CHECK_PARAM_RET(x) __CHECK_COND_RET(x)

	/// Check a parameter for validity and continue without returning
	#define CHECK_PARAM_NORET(x) __CHECK_COND_NORET(x)

	/// Check a condition for validity, and return the given value if the check fails
	#define CHECK_ASSERT_RETVAL(x, ret) __CHECK_COND_RETVAL(x, ret)

	/// Check a condition for validity, and return if the check fails
	#define CHECK_ASSERT_RET(x) __CHECK_COND_RET(x)

	/// Check a condition for validity, and just print debug info if the check fails
	#define CHECK_ASSERT_NORET(x) __CHECK_COND_NORET(x)

	#define CHECK_FAILURE_RETVAL(ret) \
		{ checkFailure(__FILE__, __LINE__, ""); return (ret); }

	#define CHECK_FAILURE_RET() \
		{ checkFailure(__FILE__, __LINE__, ""); return; }
#endif


#define IS_UINT8(n) (n >= 0 && n <= 0xff)
#define IS_INT16(n) (n >= -32768 && n <= 0x7fff)

/*
#ifdef __cplusplus
inline uint8 C_UINT8(int16 n)   { CHECK_ASSERT_RETVAL(n <= 255 && n >= 0, 0); return (uint8) n; }
inline uint8 C_UINT8(uint16 n)  { CHECK_ASSERT_RETVAL(n <= 255, 0); return (uint8) n; }
//inline uint8 C_UINT8(int n)     { CHECK_ASSERT_RETVAL(n <= 255 && n >= 0, 0); return (uint8) n; }
inline int16 C_INT16(uint16 n)  { CHECK_ASSERT_RETVAL(n <= 0x7fff, 0); return (int16) n; }
inline int16 C_INT16(uint32 n)  { CHECK_ASSERT_RETVAL(n <= 0x7fff, 0); return (int16) n; }
inline int16 C_INT16(float32 n) { CHECK_ASSERT_RETVAL(n >= -32768 && n <= 0x7fff, 0); return (int16) n; }
inline uint16 C_UINT16(int32 n)  { CHECK_ASSERT_RETVAL(n <= 0xffff, 0); return (int16) n; }
inline uint16 C_UINT16(uint32 n)  { CHECK_ASSERT_RETVAL(n <= 0xffff, 0); return (uint16) n; }
inline uint16 C_UINT16(float32 n) { CHECK_ASSERT_RETVAL(n <= 0xffff, 0); return (uint16) n; }
#endif
*/

#endif
