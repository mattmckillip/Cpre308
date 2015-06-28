/**
 * @addtogroup common
 * @{
 */
/**
 * @file      isu_color.h
 * @author    Jeramie Vens
 * @brief     Bash Color Codes
 */

#ifndef ISU_COLOR_H
#define ISU_COLOR_H

/// @def RESET
/// Reset the terminal to default colors
#define RESET "\x1b[0m"

/// @def FORE_BLACK
/// Set the foreground color to black
#define FORE_BLACK		"\x1b[30m" 
/// @def FORE_RED
/// Set the foreground color to red
#define FORE_RED 		"\x1b[31m"
/// @def FORE_GREEN
/// Set the foreground color to green
#define FORE_GREEN 		"\x1b[32m"
/// @def FORE_YELLOW
/// Set the foreground color to yellow
#define FORE_YELLOW 	"\x1b[33m"
/// @def FORE_BLUE
/// Set the foreground color to blue
#define FORE_BLUE 		"\x1b[34m"
/// @def FORE_MAGENTA
/// Set the foreground color to magenta
#define FORE_MAGENTA	"\x1b[35m"
/// @def FORE_CYAN
/// Set the foreground color to CYAN
#define FORE_CYAN 		"\x1b[36m"
/// @def FORE_WHITE
/// Set the foreground color to white
#define FORE_WHITE		"\x1b[37m"

/// @def BACK_BLACK
/// Set the background color to black
#define BACK_BLACK		"\x1b[40m"
/// @def BACK_RED
/// Set the background color to red
#define BACK_RED		"\x1b[41m"
/// @def BACK_GREEN
/// Set the background color to green
#define BACK_GREEN		"\x1b[42m"
/// @def BACK_YELLOW
/// Set the background color to yellow
#define BACK_YELLOW		"\x1b[43m"
/// @def BACK_BLUE
/// Set the background color to blue
#define BACK_BLUE		"\x1b[44m"
/// @def BACK_MAGENTA
/// Set the background color to magenta
#define BACK_MAGENTA	"\x1b[45m"
/// @def BACK_CYAN
/// Set the background color to cyan
#define BACK_CYAN		"\x1b[46m"
/// @def BACK_WHITE
/// Set the background color to white
#define BACK_WHITE		"\x1b[47m"

/// @def ATTR_BRIGHT
/// Set the text to be brightly colored 
#define ATTR_BRIGHT		"\x1b[1m"
/// @def ATTR_DIM
/// Set the text to be dimmly colored
#define ATTR_DIM		"\x1b[2m"
/// @def ATTR_UNDERSCORE
/// Set the text to have an underscore
#define ATTR_UNDERSCORE	"\x1b[4m"
/// @def ATTR_REVERSE
/// Invert the foreground and background colors
#define ATTR_REVERSE	"\x1b[7m"
/// @def ATTR_HIDDEN
/// Set the text to not be visible
#define ATTR_HIDDEN		"\x1b[8m"

#endif

/// @}


