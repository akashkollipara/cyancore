/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: compiler_macro.h
 * Description		: This file defines the compiler macro utilities
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once

#define _PRAGMA(x)		_Pragma (#x)
#define INFO(x)			_PRAGMA(message (#x))
#define WARN(x)			_PRAGMA(GCC warning #x)
#define ERROR(x)		_PRAGMA(GCC error #x)
#define TODO(x)			INFO(TODO: x)

#define _ATTRIBUTE(x)		__attribute__((x))
#define _WEAK			_ATTRIBUTE(weak)
#define _UNUSED			_ATTRIBUTE(unused)
#define _INLINE			_ATTRIBUTE(always_inline)
#define _NOINLINE		_ATTRIBUTE(noinline)
#define _ALIGN(x)		_ATTRIBUTE(aligned(x))
#define _SECTION(x)		_ATTRIBUTE(section(x))
#define _DEPRICATE		_ATTRIBUTE(depricated)
#define _ALIAS(x)		_ATTRIBUTE(alias(x))
#define _FALLTHROUGH		_ATTRIBUTE(fallthrough)
#define _NORETURN		_ATTRIBUTE(noreturn)
#define _NAKED			_ATTRIBUTE(naked)
#define EXPORT_C(x)		extern "C" x; x
#define EXPORT_C_PROTO(x)	extern "C" { x }
