#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <string>

#include "ansi_codes.h"

// __PLATFORMIO_BUILD_DEBUG__ is set when build_type is set to debug in platformio.ini
#if defined(__PLATFORMIO_BUILD_DEBUG__) || defined(DBUG2)
#define DEBUG
#endif

#ifdef UNIT_TESTS
#undef DEBUG
#endif

#ifndef TEST_NATIVE
#include "../lib/hardware/fnUART.h"
#define Serial fnUartDebug
#endif

/*
  Debugging Macros
*/
#ifdef DEBUG
#ifdef TEST_NATIVE
    #define Debug_print(...) print( __VA_ARGS__ )
    #define Debug_printf(...) printf( __VA_ARGS__ )
    #define Debug_println(...) println( __VA_ARGS__ )
    #define Debug_printv(format, ...) {printf( ANSI_YELLOW "[%s:%u] %s(): " ANSI_GREEN_BOLD format ANSI_RESET "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);}
#else
    #define Debug_print(...) fnUartDebug.print( __VA_ARGS__ )
    #define Debug_printf(...) fnUartDebug.printf( __VA_ARGS__ )
    #define Debug_println(...) fnUartDebug.println( __VA_ARGS__ )
    #define Debug_printv(format, ...) {fnUartDebug.printf( ANSI_YELLOW "[%s:%u] %s(): " ANSI_GREEN_BOLD format ANSI_RESET "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);}
#endif
    #define HEAP_CHECK(x) Debug_printf("HEAP CHECK %s " x "\r\n", heap_caps_check_integrity_all(true) ? "PASSED":"FAILED")
#else
    #define Debug_print(...)
    #define Debug_printf(...)
    #define Debug_println(...)
    #define Debug_printv(format, ...)
    #define HEAP_CHECK(x)
#endif

#endif // _DEBUG_H_
