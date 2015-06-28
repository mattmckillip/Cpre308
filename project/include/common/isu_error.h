/**
 * @addtogroup common
 * @{
 */
/**
 * @file      isu_error.h
 * @author    Jeramie Vens
 * @brief     Error handling and printing for the OS.
 */

#ifndef ISU_ERROR_H
#define ISU_ERROR_H

// include bash colors
#include "common/isu_color.h"

// to be added in the fugure
#define GET_ERRNO 0
#define SET_ERRNO(i)

/// @def PRINT_LEVEL
/// @brief The currently defined print level.
/// This is the currently defined print level.  Individula files
/// can redefine this to change the print level of that file.
#define PRINT_LEVEL 7

/// @def PRINT_EMERG
/// Prints out emergency messages, system is about to crash or is unstable
#define PRINT_EMERG 0
/// @def PRINT_ALERT
/// Prints out something bad happened and action must be taken immediately
#define PRINT_ALERT 1
/// @def PRINT_CRIT
/// Prints out a critical condition occurred like a serious hardware/software failure
#define PRINT_CRIT  2
/// @def PRINT_ERROR
/// Prints out an error condition, often used by drivers to indicate difficulties with the hardware
#define PRINT_ERROR 3
/// @def PRINT_WARN
/// Prints out a warning, meaning nothing serious by itself but might indicate problems
#define PRINT_WARN  4
/// @def PRINT_NOTICE
/// Prints out nothing serious, but notably nevertheless. Often used to report security events.
#define PRINT_NOTICE 5
/// @def PRINT_INFO
/// Prints out informational message e.g. startup information at driver initialization
#define PRINT_INFO   6
/// @def PRINT_DEBUG
/// Prints out debug messages
#define PRINT_DEBUG  7

/// @def PRINT_LEVEL_TO_COLOR
/// maps the print levels to colors in the print messages
#define PRINT_LEVEL_TO_COLOR(level) (\
	level == PRINT_EMERG ? FORE_BLACK BACK_RED : \
	level == PRINT_ALERT ? FORE_RED BACK_WHITE : \
	level == PRINT_CRIT ? FORE_RED BACK_CYAN : \
	level == PRINT_ERROR ? FORE_RED BACK_BLACK : \
	level == PRINT_WARN ?  FORE_YELLOW BACK_BLACK : \
	level == PRINT_NOTICE ? FORE_MAGENTA : \
	level == PRINT_INFO ? FORE_BLUE ATTR_BRIGHT : \
	level == PRINT_DEBUG ? FORE_CYAN : RESET \
	)

/// @def isu_print
/// Print a debug message to the screen at the given log level.  If PRINT_LEVEL
/// is set less than the given number then the message will not be printed.
#define isu_print(level, str, ...) \
	level <= PRINT_LEVEL ? \
		fprintf(stderr, "%s%s:%d: %s(): " str RESET "\n", \
			PRINT_LEVEL_TO_COLOR(level), \
			__FILE__, \
			__LINE__, \
			__func__, \
			##__VA_ARGS__) \
		: 0 


#endif

/// @}

