
/*!
 * Here we define the macros that print out debug messages when
 * MAGGIE_debug_LEVEL is > 0.
 *
 * These routines print fancier messages than the ones in C.
 * They can be included in C or C++ code.
 *
 * Here are the correspondencies with ROS logging system:
  maggieError( ... )      ROS_FATAL(__VA_ARGS__) + exit(-1)
  maggieWeakError( ... )  ROS_ERROR(__VA_ARGS__)
  maggiePrint(...)        ROS_WARN(__VA_ARGS__)
  maggieDebug1(...)       ROS_INFO(__VA_ARGS__)
  maggieDebug2(...)       ROS_INFO(__VA_ARGS__)
  maggieDdebug3(...)       ROS_DEBUG(__VA_ARGS__)
 *
 * For recommendation about how to use these levels, see
 * http://www.ros.org/wiki/Verbosity%20Levels
 *
 * \author Arnaud Ramey
 * \file debug.h
 */

#ifndef _MAGGIE_debug_H_
#define _MAGGIE_debug_H_

//////////////////////////////////////////////////////////////////////////////


#if defined(__cplusplus) && defined(AD_USE_ROS)

// use ROS logging mechanisms
#include <ros/ros.h>
#define maggiePrint(...)   ROS_WARN(__VA_ARGS__)
#define maggieDebug1(...)  ROS_INFO(__VA_ARGS__)
#define maggieDebug2(...)  ROS_INFO(__VA_ARGS__)
#define maggieDebug3(...)  ROS_DEBUG(__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#else // not AD_USE_ROS or not __cplusplus
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Std
#include <stdio.h> // for fprintf


// debug level
#define MAGGIE_debug_LEVEL 2
// 0 : does not print any maggieDebug*()
// 1 : prints only maggieDebug1(), but not maggieDebug2() or maggieDebug3()
// 2 : prints maggieDebug1() and maggieDebug2(), but not maggieDebug3()
// 3 : prints maggieDebug1(), maggieDebug2(), maggieDebug3()

//Colors
#define PRINTF_FORMAT_NO_ATTRIB 0
#define PRINTF_FORMAT_BOLD 1
#define PRINTF_FORMAT_UNDERLINE 4
#define PRINTF_FORMAT_BLACK 30
#define PRINTF_FORMAT_RED 31
#define PRINTF_FORMAT_GREEN 32
#define PRINTF_FORMAT_YELLOW 33
#define PRINTF_FORMAT_BLUE 34
#define PRINTF_FORMAT_MAGENTA 35
#define PRINTF_FORMAT_CYAN 36
#define PRINTF_FORMAT_WHITE 37

// comment for remove colors from the maggieDebug() functions
#define USE_PRINTF_COLORS

//
#define MARKER_PRINT  "@@@@"
#define MARKER_debug1 "@@@-"
#define MARKER_debug2 "@@--"
#define MARKER_debug3 "@---"

/*! Stringified __LINE__ */
#define MAGGIE_debug_STRINGIFY(x) #x
#define MAGGIE_debug_TO_STRING(x) MAGGIE_debug_STRINGIFY(x)

/*!
 * an extension of printf, with the call position
 */
#define maggiePrint( ... ) MAGGIE_debug(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_RED, MARKER_PRINT, __VA_ARGS__)

/*!
 *  Define debug() for each level of verbosity.
 *  Accepts message in the printf like fashion.
 */
#if MAGGIE_debug_LEVEL == 0
#define maggieDebug1(...)  {}
#define maggieDebug2(...)  {}
#define maggieDebug3(...)  {}

#elif MAGGIE_debug_LEVEL == 1
#define maggieDebug1(...)  MAGGIE_debug(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_GREEN, MARKER_debug1, __VA_ARGS__)
#define maggieDebug2(...)  {}
#define maggieDebug3(...)  {}

#elif MAGGIE_debug_LEVEL == 2
#define maggieDebug1(...)  MAGGIE_debug(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_GREEN, MARKER_debug1, __VA_ARGS__)
#define maggieDebug2(...)  MAGGIE_debug(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_BLUE,  MARKER_debug2,  __VA_ARGS__)
#define maggieDebug3(...)  {}

#elif MAGGIE_debug_LEVEL == 3
#define maggieDebug1(...)  MAGGIE_debug(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_GREEN, MARKER_debug1, __VA_ARGS__)
#define maggieDebug2(...)  MAGGIE_debug(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_BLUE,  MARKER_debug2, __VA_ARGS__)
#define maggieDebug3(...)  MAGGIE_debug(PRINTF_FORMAT_NO_ATTRIB, PRINTF_FORMAT_BLUE,  MARKER_debug3, __VA_ARGS__)
#endif


//////////////////////////////////////////////////////////////////////////////

/*!
 * The primitive macro for debugging.
 * Accepts message in the printf like fashion.
 */
#if defined(__cplusplus) //////////////////////////////////////////////////////

#include <string>
#define MAGGIE_FILE_POSITION \
    (  ((std::string) __FILE__).substr(1 + ((std::string) __FILE__).find_last_of('/')) \
    + ":" + MAGGIE_debug_TO_STRING(__LINE__) ) \


#ifdef __GNUC__
#define __MAGGIE_HERE__ ( std::string ("[") + MAGGIE_FILE_POSITION + std::string (" (") + std::string(__FUNCTION__) + std::string(")] \t") )
#else
#define __MAGGIE_HERE__ ( std::string ("[") + MAGGIE_FILE_POSITION + std::string ("] \t") )
#endif


#undef MAGGIE_debug
#ifdef USE_PRINTF_COLORS
#define MAGGIE_debug(printf_attr, printf_color, marker, ...) { \
    printf("%c[%d;%dm%s", 0x1B, printf_attr, printf_color, (std::string(marker) + __MAGGIE_HERE__).c_str()); \
    printf("%c[%dm", 0x1B, PRINTF_FORMAT_NO_ATTRIB); \
    printf(__VA_ARGS__); \
    printf("\n"); \
    }
#else  // USE_PRINTF_COLORS == false
#define MAGGIE_debug(printf_attr, printf_color, ...) { \
    printf("%s", (std::string(marker) + __MAGGIE_HERE__).c_str()); \
    printf(__VA_ARGS__); \
    printf("\n"); \
    }
#endif // USE_PRINTF_COLORS


#else // we compile C code ////////////////////////////////////////////////////

#ifdef USE_PRINTF_COLORS
#define MAGGIE_debug(printf_attr, printf_color, marker, ...) { \
    printf("%c[%d;%dm[" marker __FILE__ ":" MAGGIE_debug_TO_STRING(__LINE__) "] ", 0x1B, printf_attr, printf_color); \
    printf("%c[%dm", 0x1B, PRINTF_FORMAT_NO_ATTRIB); \
    printf(__VA_ARGS__); \
    printf("\n"); \
    }
#else  // USE_PRINTF_COLORS == false
#define MAGGIE_debug(printf_attr, printf_color, marker, ...) { \
    printf("[" marker __FILE__ ":" MAGGIE_debug_TO_STRING(__LINE__) "] " __VA_ARGS__); \
    printf("\n"); \
    }
#endif // USE_PRINTF_COLORS

#endif // defined(__cplusplus) ////////////////////////////////////////////////

#endif // not AD_USE_ROS or not __cplusplus

#endif /* _MAGGIE_debug_H_ */

