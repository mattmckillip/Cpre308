/**
 * @addtogroup common Common
 * @brief     Common headers needed by the entire system
 * @{
 */
/**
 * @file      isu_types.h
 * @author    Jeramie Vens
 * @brief     Basic types used throughout the system
 */


#ifndef ISU_TYPES_H
#define ISU_TYPES_H

#include <stdint.h>

/// Error codes returned from various functions.  More can be added as needed
typedef enum ISU_ERROR_CODE_ENUM
{
	/// function returned successfully
	ISU_RET_SUCCESS = 0,
	/// there was a memory error causing the return
	ISU_RET_EMEM,
	/// there was an invalid parameter
	ISU_RET_EPARAM,
	/// the function was attempted to be used incorrectly
	ISU_RET_EUSAGE
}isu_return_t;

#endif

